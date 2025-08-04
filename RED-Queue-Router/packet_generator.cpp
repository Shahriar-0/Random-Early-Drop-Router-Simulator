#include "packet_generator.h"

uint64_t PacketGenerator::_ctr = 0;

void PacketGenerator::start() { scheduleNext(0); }
void PacketGenerator::reset() { _backoff = false; }

PacketGenerator::PacketGenerator(int id, double rate, int dst, Simulator* sim)
    : QObject(sim), _id(id), _dst(dst), _rate(rate), _sim(sim), _dist(rate), _uniform(0, 1) {
    connect(_sim, &Simulator::packetEvent, this, &PacketGenerator::handleEvent);
    connect(sim, &Simulator::finished, this, &PacketGenerator::reset);
}

void  PacketGenerator::handleEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t) {
    if (type == EventType::TIMER && nodeId == _id && !_backoff) send(t); // only handles its own backoff notifications
}

void PacketGenerator::onCongestion(int genId) {
    if (genId != _id) return;
    _backoff = true;
    double backoffInterval = _uniform(_rng);
    _sim->schedule({EventType::TIMER, _sim->now() + backoffInterval, _id, nullptr}); // schedule resume
}

void PacketGenerator::send(SimTime now) {
    auto pkt = std::make_shared<Packet>(_ctr++, now, 1024, _id, _dst);
    _sim->schedule({EventType::ARRIVAL, now,  _dst, pkt});
    scheduleNext(now);
}

void PacketGenerator::scheduleNext(SimTime now) {
    double dt = _dist(_rng);
    _sim->schedule({EventType::TIMER, now+dt, _id, nullptr});
}