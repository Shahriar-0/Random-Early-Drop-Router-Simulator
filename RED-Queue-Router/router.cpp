#include "router.h"
#include <cassert>

Router::Router(int id, Simulator* sim, QObject* parent)
    : QObject(parent), _id(id), _sim(sim), _rng(std::random_device{}()) {
    QObject::connect(_sim, &Simulator::packetEvent, this, &Router::onEvent, Qt::UniqueConnection);
}

void Router::addInput(int srcId, double txRate, double propDelay, size_t cap) {
    _inputs.emplace(srcId, Port(srcId, txRate, propDelay, cap));
}

void Router::setOutput(int destId, double txRate, double propDelay) {
    _output = new Port(destId, txRate, propDelay, _bufferCap);
}

void Router::setBufferCap(size_t cap) {
    _bufferCap = cap;
    if (_output) _output->q = BoundedQueue(cap);
}

double Router::dropProb(size_t qlen) const {
    if (qlen == 0 || qlen == 1) return 0.0;
    if (qlen >= 5) return 1.0;
    return 0.3 * static_cast<double>(qlen) - 0.4; // 2->0.2, 3->0.5, 4->0.8
}

void Router::onEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t) {
    if (nodeId != _id) return; // ignore events for others
    if (!pkt && type != EventType::DEPARTURE) return;

    if (type == EventType::ARRIVAL) {
        auto qlen = _output ? _output->q.size() : 0;
        double p = dropProb(qlen);
        bool dropped = false;
        if (_ud(_rng) < p) {
            dropped = true;
        } else {
            if (!_output->q.push(pkt)) {
                dropped = true;
            }
        }
        if (dropped) {
            if (_output) _output->dropped++;
            emit congested();
            _sim->record(0, (int)(_output ? _output->q.size() : 0), 1, 0, t);
        } else {
            _sim->record(0, (int)(_output ? _output->q.size() : 0), 0, 0, t);
            if (_output && _output->q.size() == 1) {
                scheduleDequeue(t);
            }
        }
    } else if (type == EventType::DEPARTURE) {
        tryDequeue(t);
    }
}

void Router::scheduleDequeue(SimTime now) {
    if (!_output) return;
    if (_output->q.empty()) return;
    double service = 1.0 / _output->txRate;
    _sim->schedule({EventType::DEPARTURE, now + service + _output->propDelay, _id, _output->q.front()});
}

void Router::tryDequeue(SimTime now) {
    if (!_output) return;
    auto p = _output->q.pop();
    if (!p) return;
    _output->forwarded++;
    _sim->record(0, (int)_output->q.size(), 0, 1, now);
    if (!_output->q.empty()) {
        scheduleDequeue(now);
    }
}
