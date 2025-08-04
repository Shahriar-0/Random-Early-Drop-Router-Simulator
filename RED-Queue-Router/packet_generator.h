#ifndef PACKETGENERATOR_H
#define PACKETGENERATOR_H

#include <QObject>
#include <random>

#include "packet.h"
#include "simulator.h"

class PacketGenerator : public QObject {
    Q_OBJECT
    int _id, _dst;
    int _bandwidth; // ?: in bps maybe? do we even need this?
    double _rate;
    Simulator* _sim;
    std::exponential_distribution<double> _dist;
    std::uniform_real_distribution<double> _uniform;
    std::mt19937 _rng{std::random_device{}()};
    static uint64_t _ctr;
    bool _backoff{false};

public:
    PacketGenerator(int id, double rate, int dst, Simulator* sim);
    void start();

private slots:
    void handleEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t);
    void onCongestion(int genId);
    void send(SimTime now);
    void scheduleNext(SimTime now);
    void reset();
    void resumeGeneration();
};

#endif
