#include "simulator.h"

Simulator::Simulator(QObject* parent) : QObject(parent) {}

void Simulator::schedule(const Event& ev) {
    _pq.push(ev);
}

void Simulator::run(SimTime until) {
    while (!_pq.empty()) {
        auto ev = _pq.top();
        if (ev.time > until) break;
        _pq.pop();
        _now = ev.time;
        emit packetEvent(ev.nodeId, ev.pkt, ev.type, _now);
    }
    _now = until;
}

void Simulator::record(int portId, int queueLen, int dropped, int forwarded, SimTime timestamp) {
    if (metrics) metrics->record(portId, queueLen, dropped, forwarded, timestamp);
}
