#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <memory>
#include <queue>
#include <vector>

#include "event.h"

class Simulator : public QObject {
    Q_OBJECT
    SimTime _currentTime{0.0};
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> _pq;

public:
    Simulator(QObject* parent = nullptr) : QObject(parent) {}
    void schedule(const Event& ev);
    SimTime now() const;
    void run(SimTime until);
signals:
    void packetEvent(int nodeId, PacketPtr pkt, EventType type, SimTime time);
    void finished();
};

#endif