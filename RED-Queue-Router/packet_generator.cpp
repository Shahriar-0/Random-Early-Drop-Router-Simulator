#include "packet_generator.h"
#include "router.h"
#include "packet.h"

PacketGenerator::PacketGenerator(int id, int dst, double rate, double txRate, double propDelay, Simulator* sim, QObject* parent)
    : QObject(parent), _id(id), _dst(dst), _genRate(rate), _txRate(txRate), _propDelay(propDelay), _sim(sim),
      _rng(std::random_device{}()), _exp(rate) {
    QObject::connect(_sim, &Simulator::packetEvent, this, &PacketGenerator::onEvent, Qt::UniqueConnection);
}

void PacketGenerator::setSeed(uint32_t seed) {
    _rng.seed(seed);
    _exp = std::exponential_distribution<double>(_genRate);
}

void PacketGenerator::start(SimTime at) {
    scheduleNext(at);
}

void PacketGenerator::scheduleNext(SimTime now) {
    if (_backoff) return;
    double dt = _exp(_rng);
    _sim->schedule({EventType::TIMER, now + dt, _id, nullptr});
}

void PacketGenerator::send(SimTime now) {
    if (_backoff) return;
    auto pkt = std::make_shared<Packet>(_ctr++, now, 1024, _id, _dst);
    double txDelay = 1.0 / _txRate;
    _sim->schedule({EventType::ARRIVAL, now + txDelay + _propDelay, _dst, pkt});
    scheduleNext(now);
}

void PacketGenerator::onEvent(int nodeId, PacketPtr, EventType type, SimTime t) {
    if (type == EventType::TIMER && nodeId == _id) {
        if (_backoff) {
            if (t >= _resumeAt) {
                _backoff = false;
                _lastResumeScheduled = 0.0;
                send(t);
            } // else ignore early timer
        } else {
            send(t);
        }
    }
}

void PacketGenerator::onCongested() {
    double wait = _uni(_rng) / _genRate;       // ~0.25..0.75 for rate=2
    double deadline = _sim->now() + wait;
    if (!_backoff) {
        _backoff = true;
        _resumeAt = deadline;
        _lastResumeScheduled = deadline;
        _sim->schedule({EventType::TIMER, deadline, _id, nullptr});
    } else {
        if (deadline > _resumeAt) _resumeAt = deadline;
        if (deadline > _lastResumeScheduled) {
            _lastResumeScheduled = deadline;
            _sim->schedule({EventType::TIMER, deadline, _id, nullptr});
        }
    }
}

void PacketGenerator::resume() {
    _backoff = false;
    scheduleNext(_sim->now());
}
