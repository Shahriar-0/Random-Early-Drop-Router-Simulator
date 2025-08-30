#include "packet_generator.h"

uint64_t PacketGenerator::_ctr = 0;
void PacketGenerator::start() { scheduleNext(0); }
void PacketGenerator::reset() { _backoff = false; }

PacketGenerator::PacketGenerator(int id, double rate, int dst, Simulator* sim, int transmissionDelay, int propagationDelay)
    : QObject(sim), _id(id), _dst(dst), _rate(rate), _sim(sim), _transmissionDelay(transmissionDelay), _propagationDelay(propagationDelay), _dist(rate), _uniform(0, 1) {
    connect(_sim, &Simulator::packetEvent, this, &PacketGenerator::handleEvent);
    connect(sim, &Simulator::finished, this, &PacketGenerator::reset);
}

void PacketGenerator::handleEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t) {
    if (nodeId == _id) {
        if (type == EventType::TIMER && !_backoff) send(t);
        if (type == EventType::RESUME) resumeGeneration();
    }
}

void PacketGenerator::onCongestion(int genId) {
    if (genId != _id) return;
    _backoff = true;
    double backoffInterval = _uniform(_rng) * 2.0 + 0.5;
    _sim->schedule({EventType::RESUME, _sim->now() + backoffInterval, _id, nullptr});
}

void PacketGenerator::send(SimTime now) {
    auto pkt = std::make_shared<Packet>(_ctr++, now, 1024, _id, _dst);
    _sim->schedule({EventType::ARRIVAL, now + _transmissionDelay + _propagationDelay, _dst, pkt});
    scheduleNext(now);
}

void PacketGenerator::scheduleNext(SimTime now) {
    double dt = _dist(_rng);
    _sim->schedule({EventType::TIMER, now + dt, _id, nullptr});
}

void PacketGenerator::resumeGeneration() {
    _backoff = false;
    scheduleNext(_sim->now());
}
