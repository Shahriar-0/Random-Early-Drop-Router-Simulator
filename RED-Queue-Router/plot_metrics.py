#!/usr/bin/env python3

import os
import sys
import csv
from collections import Counter

try:
    import matplotlib.pyplot as plt
except ModuleNotFoundError:
    print("matplotlib is not installed. Install it with:\n"
          "  python -m pip install matplotlib\n", file=sys.stderr)
    sys.exit(1)


def load_events(path):
    t, q_logged, d, f = [], [], [], []
    with open(path, newline="") as fh:
        r = csv.DictReader(fh)
        for row in r:
            try:
                t.append(float(row["t"]))
                q_logged.append(int(row["queue_len"]))
                d.append(int(row["drop"]))
                f.append(int(row["forward"]))
            except Exception:
                continue
    return t, q_logged, d, f


def cumulative(seq):
    out, s = [], 0
    for v in seq:
        s += v
        out.append(s)
    return out


def pred_red_p(q):
    if q <= 1:
        return 0.0
    if q >= 5:
        return 1.0
    return 0.3 * q - 0.4


def save_png(path):
    d = os.path.dirname(path)
    if d and not os.path.exists(d):
        os.makedirs(d, exist_ok=True)
    plt.tight_layout()
    plt.savefig(path, dpi=160)
    plt.close()


def stem_compat(x, y, **kwargs):
    ax = plt.gca()
    try:
        ax.stem(x, y, **kwargs)
    except TypeError:
        kwargs.pop("use_line_collection", None)
        ax.stem(x, y, **kwargs)


def main(logdir):
    events_csv = os.path.join(logdir, "events.csv")
    if not os.path.exists(events_csv):
        print(f"events.csv not found in: {logdir}", file=sys.stderr)
        sys.exit(2)

    t, q_logged, d, f = load_events(events_csv)
    if not t:
        print("No rows found in events.csv; nothing to plot.", file=sys.stderr)
        sys.exit(0)

    plt.figure()
    stem_compat(t, d)
    plt.title("Drops over time")
    plt.xlabel("time"); plt.ylabel("drop (0/1)")
    save_png(os.path.join(logdir, "plot_dropped_events.png"))

    plt.figure()
    stem_compat(t, f)
    plt.title("Forwarded over time")
    plt.xlabel("time"); plt.ylabel("forward (0/1)")
    save_png(os.path.join(logdir, "plot_forwarded_events.png"))

    plt.figure()
    plt.plot(t, cumulative(d))
    plt.title("Dropped packets")
    plt.xlabel("time"); plt.ylabel("dropped")
    save_png(os.path.join(logdir, "plot_dropped_cumulative.png"))

    plt.figure()
    plt.plot(t, cumulative(f))
    plt.title("Forwarded packets")
    plt.xlabel("time"); plt.ylabel("forwarded")
    save_png(os.path.join(logdir, "plot_forwarded_cumulative.png"))

    plt.figure()
    plt.plot(t, q_logged)
    plt.title("Queue length over time")
    plt.xlabel("time"); plt.ylabel("queue length (logged)")
    save_png(os.path.join(logdir, "plot_queue_length.png"))

    arrivals_q_pre = []
    arrivals_drop = []
    for qi, di, fi in zip(q_logged, d, f):
        if fi == 0:  # arrival row
            q_pre = qi if di == 1 else max(0, qi - 1)
            arrivals_q_pre.append(q_pre)
            arrivals_drop.append(di)

    drop_hist_pre = Counter(q for q, di in zip(arrivals_q_pre, arrivals_drop) if di)
    xs = sorted(drop_hist_pre.keys())
    ys = [drop_hist_pre[x] for x in xs]
    plt.figure()
    plt.bar(xs, ys)
    plt.title("Drops by queue length")
    plt.xlabel("queue length q_pre"); plt.ylabel("drop count")
    save_png(os.path.join(logdir, "plot_drops_by_queue_len_pre.png"))

    exposures = Counter(arrivals_q_pre)                               # arrivals at q_pre
    drop_counts = Counter(q for q, di in zip(arrivals_q_pre, arrivals_drop) if di)

    qs = sorted(k for k in exposures.keys() if 0 <= k <= 64)
    emp = [drop_counts.get(k, 0) / exposures[k] for k in qs]
    theory = [pred_red_p(k) for k in qs]

    # CSV for your report
    out_csv = os.path.join(logdir, "red_empirical_vs_theory.csv")
    with open(out_csv, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["queue_len_pre", "arrival_exposures", "arrival_drops", "empirical_p", "theoretical_p"])
        for k in qs:
            e = exposures[k]
            dc = drop_counts.get(k, 0)
            ep = dc / e if e else 0.0
            tp = pred_red_p(k)
            w.writerow([k, e, dc, f"{ep:.6f}", f"{tp:.6f}"])

    plt.figure()
    plt.plot(qs, emp, marker="o", label="empirical")
    plt.plot(qs, theory, marker="s", linestyle="--", label="theoretical")
    plt.title("RED drop probability: empirical vs theoretical")
    plt.xlabel("queue length q_pre"); plt.ylabel("P(drop | q_pre)")
    plt.legend()
    save_png(os.path.join(logdir, "plot_red_empirical_vs_theory.png"))

    print("Saved plots and CSVs to:", logdir)
    for name in [
        "plot_dropped_events.png",
        "plot_forwarded_events.png",
        "plot_dropped_cumulative.png",
        "plot_forwarded_cumulative.png",
        "plot_queue_length.png",
        "plot_drops_by_queue_len_pre.png",
        "plot_red_empirical_vs_theory.png",
        "red_empirical_vs_theory.csv",
    ]:
        print(" -", name)


if __name__ == "__main__":
    logdir = sys.argv[1] if len(sys.argv) > 1 else "logs"
    main(logdir)
