#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H

#include <QObject>
#include <random>
#include "event.h"
#include "simulator.h"

class PacketGenerator : public QObject {
    Q_OBJECT
public:
    PacketGenerator(int id, int dst, double rate, double txRate, double propDelay, Simulator* sim, QObject* parent=nullptr);

public slots:
    void onEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t);
    void onCongested();
    void resume();

public:
    void start(SimTime at);
    void setSeed(uint32_t seed);

private:
    void scheduleNext(SimTime now);
    void send(SimTime now);

private:
    int _id;
    int _dst;
    double _genRate; // pkts per unit time
    double _txRate;
    double _propDelay;
    Simulator* _sim;

    bool   _backoff {false};
    double _resumeAt {0.0};
    double _lastResumeScheduled {0.0};
    int    _ctr {0};

    std::mt19937 _rng;
    std::exponential_distribution<double> _exp;
    std::uniform_real_distribution<double> _uni{0.5, 1.5};
};

#endif // PACKET_GENERATOR_H
