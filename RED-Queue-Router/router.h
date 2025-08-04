#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <unordered_map>

#include "queue.h"
#include "simulator.h"
#include "metrics.h"

class Router : public QObject {
    Q_OBJECT
    int _id;
    Simulator* _sim;
    struct Port {
        int nxt;
        double rate, delay; // FIXME: maybe one of them is enough idk
        REDQueue queue;
        int dropped = 0, forwarded = 0;

        Port(int nextHop, double rate, double delay, size_t cap, double wq, double minTh, double maxTh, double maxP)
            : nxt(nextHop), rate(rate), delay(delay), queue(cap, wq, minTh, maxTh, maxP), dropped(0), forwarded(0) {}
    };
    std::unordered_map<int, Port> _ports;

public:
    Router(int id, Simulator* sim);
    void addPort(int dest, double rate, double delay, size_t cap, double wq, double minTh, double maxTh, double maxP);

signals:
    void congestion(int generatorId);

private slots:
    void handleEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t);

private:
    void nextHop(Port& p, SimTime t);
};

#endif