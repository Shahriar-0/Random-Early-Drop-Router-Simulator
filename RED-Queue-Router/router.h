#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <unordered_map>

#include "metrics.h"
#include "queue.h"
#include "simulator.h"

class Router : public QObject {
    Q_OBJECT
    int _id;
    Simulator* _sim;
    struct Port {
        int nxt;
        int transDly, propDly;
        REDQueue queue; // technically we only use this queue for PC3 but let's keep the structure
        int dropped = 0, forwarded = 0;

        Port(int nextHop, int transDly, int propDly, size_t cap, double wq, double minTh, double maxTh, double maxP)
            : nxt(nextHop), transDly(transDly), propDly(propDly), queue(cap, wq, minTh, maxTh, maxP), dropped(0), forwarded(0) {}
    };
    std::unordered_map<int, Port> _ports;

public:
    Router(int id, Simulator* sim);
    void addPort(int dest, int transDly, int propDly, size_t cap, double wq, double minTh, double maxTh, double maxP);

signals:
    void congestion(int generatorId);

private slots:
    void handleEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t);

private:
    void receivedDst(Port& p, PacketPtr pkt);
};

#endif