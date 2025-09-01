#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <queue>
#include <vector>
#include "event.h"
#include "metrics.h"

class Simulator : public QObject {
    Q_OBJECT
public:
    explicit Simulator(QObject* parent=nullptr);
    SimTime now() const noexcept { return _now; }
    void schedule(const Event& e);
    void run(SimTime until);

signals:
    void packetEvent(int nodeId, PacketPtr pkt, EventType type, SimTime t);

public slots:
    void record(int portId, int queueLen, int dropped, int forwarded, SimTime timestamp);

public:
    Metrics* metrics {nullptr};

private:
    SimTime _now {0.0};
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> _pq;
};

#endif // SIMULATOR_H
