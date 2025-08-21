#include "router.h"

#include <cassert>

Router::Router(int id, Simulator* sim) : QObject(sim), _id(id), _sim(sim) {
    connect(_sim, &Simulator::packetEvent, this, &Router::handleEvent);
}

void Router::addPort(int dest, int transDly, int propDly, size_t cap, double wq, double minTh, double maxTh, double maxP) {
    _ports.emplace(dest, Port(dest, transDly, propDly, cap, wq, minTh, maxTh, maxP));
}

void Router::handleEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t) {
    if (!pkt) return;

    auto it = _ports.find(pkt->dst());
    assert(it != _ports.end() && "port not found"); // FIXME: if things got shitty just return idk man qt is shit
    Port& port = it->second;

    if (type == EventType::RECEIVED)
        receivedDst(port, pkt);
    else if (nodeId == _id || type == EventType::ARRIVAL) {
        bool dropped = !port.queue.offer(pkt);
        if (dropped) {
            port.dropped++;
            emit congestion(pkt->src());
            //? maybe record something as well? these fuckers didn't specify what to show
        }
        else {
            port.forwarded++;
            emit metrics->record(pkt->dst(), port.queue.size(), port.dropped, port.forwarded, t);
            _sim->schedule({EventType::RECEIVED, t + port.transDly + port.propDly, _id, pkt});
        }
    }
}

void Router::receivedDst(Port& p, PacketPtr pkt) {
    auto polled = p.queue.poll();
    assert(*polled == *pkt && "another packet is polled? wtf bro");
    //? maybe record something as well? these fuckers didn't specify what to show
}
