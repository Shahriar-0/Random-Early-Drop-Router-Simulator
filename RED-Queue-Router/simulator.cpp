#include "simulator.h"

void Simulator::schedule(const Event& ev) { _pq.push(ev); }
SimTime Simulator::now() const { return _currentTime; }

void Simulator::run(SimTime until) {
    while (!_pq.empty()) {
        Event ev = _pq.top(); _pq.pop();
        if (ev.time > until) break;
        _currentTime = ev.time;
        emit packetEvent(ev.nodeId, ev.pkt, ev.type, _currentTime);
    }
    emit finished();
}