# RED Queue Router

We implemented a small, self-contained discrete-event simulator of a RED-enabled router fed by two Poisson traffic sources.
---


## Design Overview

We modeled three things:

1. a **clock** that orders future actions;
2. router and sources that wake up when the clock says it’s their turn;
3. a **log** of what actually happened.

---

## Core Data Structures

### `Event`

Events live in a min-heap ordered by `time`. Three kinds were enough:

* `TIMER` — wake a generator (Poisson send or backoff resume)
* `ARRIVAL` — packet reaches the router
* `DEPARTURE` — the router finishes serving the head-of-line

### `Packet`

A tiny POD created by the sources and carried to the router: `id`, `createdAt`, `sizeBytes`, `src`, `dst`.

### `BoundedQueue`

A simple FIFO with a fixed capacity (our router buffer). API: `push`, `pop`, `front`, `size`, `empty`, `capacity`.

---

## Components and Their Roles

### Simulator

**Role:** clock + scheduler + dispatcher.

* Holds a **min-heap** of `Event`s.
* `schedule(Event)` pushes a future event.
* `run(until)`:

  1. pop the earliest event `e` while `e.time ≤ until`,
  2. advance `_now = e.time`,
  3. **emit** `packetEvent(e.nodeId, e.pkt, e.type, e.time)`.
* `record(...)` forwards samples to `Metrics`.

Why we did it this way: one place owns time and ordering; actors react to a single signal and filter by `nodeId`.

---

### Router (RED + single server)

**Role:** the bottleneck link with RED on its buffer.

* **State:** one output `Port` (with `txRate`, `propDelay`, a `BoundedQueue`, and simple counters), RNG for RED.
* **Connects:** to `Simulator::packetEvent` in its constructor using `Qt::UniqueConnection`.
* **Guards:** `if (nodeId != _id) return;` inside `onEvent`.

**Arrival handling**

1. Read current `qlen`, compute `p = dropProb(qlen)`.
2. Drop with prob `p` (or if the buffer is full):

   * increment drop counters, `emit congested()`, `record(queue_len, drop=1, forward=0, t)`.
3. Otherwise push into the queue and `record(..., drop=0, forward=0, t)`.
   If we just went **0 → 1**, schedule one **DEPARTURE** at `now + 1/txRate + propDelay`.

**Departure handling**

* Pop one, count it as forwarded, `record(..., drop=0, forward=1, t)`.
* If the queue is still non-empty, immediately schedule the next **DEPARTURE**.
* The “0→1 rule” ensures we never stack multiple departures for the same head-of-line packet.

**Congestion signal**

* On every drop, `congested()` is emitted; sources subscribe to this to back off briefly.

---

### Packet Generators (Poisson + coalesced backoff)

**Role:** two identical `PacketGenerator`s produce a Poisson stream and ease off when the router drops.

**State**

* `_genRate` (pkts/unit), `_txRate` (serialization), `_propDelay`
* RNGs: exponential for Poisson, uniform for backoff jitter
* Backoff bookkeeping: `_backoff`, `_resumeAt`, `_lastResumeScheduled`
* `_ctr` for packet IDs

**Connections**

* Listen to `Simulator::packetEvent` (we only act on our own `TIMER`s).
* Connected to `Router::congested` → `onCongested()`.

**Key functions**

* `start(at)`: schedule first **TIMER** at `at + Exp(_genRate)`.
* `onEvent(..., TIMER, t)`:

  * if not backing off → `send(t)`;
  * if backing off and `t ≥ _resumeAt` → clear and `send(t)`;
  * else ignore (early).
* `send(now)`:

  * build `Packet`, schedule **ARRIVAL** to the router at `now + 1/_txRate + _propDelay`,
  * schedule next **TIMER** at `now + Exp(_genRate)`.
* `onCongested()`:

  * compute `deadline = now + U(0.5,1.5)/_genRate`,
  * if not backing off: set `_backoff=true; _resumeAt=deadline;` and schedule **one** resume TIMER,
  * if already backing off: extend `_resumeAt = max(_, deadline)` and, only if the extension passed the last scheduled, schedule **one** later resume TIMER.

Why we did it this way: coalesced backoff is robust , and it visibly reduces offered load when the router is loaded.

---

### Metrics

**Role:** a tiny time-series sink.

* Stores parallel `QVector`s for `times`, `queueLens`, `drops` (0/1), `forwards` (0/1) per port.
* The router calls `sim.record(...)` on every arrival and departure.
* `main.cpp` writes CSVs from these arrays after the run.

Note: `queue_len` in `events.csv` is the **post-event** size. The plotting script converts arrivals to **pre-enqueue** queue length for the empirical RED curve (if dropped, `q_pre = q_logged`; if accepted, `q_pre = q_logged − 1`).

---

## Packet/Data Flow

1. **Startup** (`main.cpp`)
   We create `Simulator`, `Metrics`, `Router` (cap=6, `txRate=1`), and two `PacketGenerator`s (rate=2 each). We connect `router.congested → gen.onCongested` for both sources. We call `gen.start(0.0)` and then `sim.run(until)`.

2. **TIMER → send**
   The simulator pops a generator’s TIMER. The generator wakes, constructs a `Packet`, schedules its **ARRIVAL** to the router at `now + 1/_txRate + _propDelay`, and schedules its next TIMER with an exponential draw.

3. **ARRIVAL → RED/Enqueue**
   The router reads `qlen`, applies RED. On drop it emits `congested()`; on accept it enqueues and, if the queue was empty, schedules the first **DEPARTURE**.

4. **DEPARTURE → forward**
   The router pops one, counts it as forwarded, and if the queue isn’t empty schedules the next **DEPARTURE** immediately.

5. **Backoff in action**
   During hot periods, drops extend `_resumeAt`; each extension schedules at most **one** later resume TIMER. When `_resumeAt` arrives, the source resumes sending.

6. **Recording**
   Every arrival/departure calls `record(...)`. After `run()`, we dump CSVs, and the Python helper produces figures that match the configured RED curve.

---
