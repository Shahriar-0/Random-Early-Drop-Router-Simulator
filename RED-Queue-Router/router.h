#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <unordered_map>
#include <random>
#include "queue.h"
#include "simulator.h"

class Router : public QObject {
    Q_OBJECT
public:
    explicit Router(int id, Simulator* sim, QObject* parent=nullptr);

    struct Port {
        int dest;
        double txRate;
        double propDelay;
        BoundedQueue q;
        int dropped{0};
        int forwarded{0};
        Port(int d, double tx, double prop, size_t cap)
            : dest(d), txRate(tx), propDelay(prop), q(cap) {}
    };

    void addInput(int srcId, double txRate, double propDelay, size_t cap);
    void setOutput(int destId, double txRate, double propDelay);
    void setBufferCap(size_t cap);
    double dropProb(size_t qlen) const;

signals:
    void congested();

public slots:
    void onEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t);

private:
    void scheduleDequeue(SimTime now);
    void tryDequeue(SimTime now);

private:
    int _id;
    Simulator* _sim;
    std::unordered_map<int, Port> _inputs;
    Port* _output{nullptr};
    size_t _bufferCap{6};
    std::mt19937 _rng;
    std::uniform_real_distribution<double> _ud{0.0, 1.0};
};

#endif // ROUTER_H
