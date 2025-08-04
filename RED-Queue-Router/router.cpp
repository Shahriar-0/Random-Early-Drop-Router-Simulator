#include "router.h"

#include <cassert>

Router::Router(int id, Simulator* sim) : QObject(sim), _id(id), _sim(sim) {
    connect(_sim, &Simulator::packetEvent, this, &Router::handleEvent);
}

void Router::addPort(int dest, double rate, double delay, size_t cap, double wq, double minTh, double maxTh, double maxP) {
    _ports.emplace(dest, Port(dest, rate, delay, cap, wq, minTh, maxTh, maxP));
}

void Router::handleEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t) {
    if (nodeId != _id || type != EventType::ARRIVAL || !pkt) return;

    auto it = _ports.find(pkt->dst());
    assert(it != _ports.end() && "Port not found"); // FIXME: if things got shitty just return idk man qt is shit
    Port& port = it->second;

    bool dropped = !port.queue.offer(pkt);
    if (dropped) {
        port.dropped++;
        emit congestion(pkt->src());
    }
    else {
        port.forwarded++;
        emit metrics->record(pkt->dst(), port.queue.size(), port.dropped, port.forwarded, t);
        nextHop(port, t);
    }
}

void Router::nextHop(Port& p, SimTime t) {
    // FIXME: not sure but i think we should handle sending port delay here as well
    // although that's a shitty design but since it's small i think it works
    auto pkt = p.queue.poll();
    double ser = pkt->size() * 8.0 / p.rate;
    _sim->schedule({EventType::ARRIVAL, t + ser + p.delay, p.nxt, pkt});
}
