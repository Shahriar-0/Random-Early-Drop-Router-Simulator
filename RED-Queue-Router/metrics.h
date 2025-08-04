#ifndef METRICS_H
#define METRICS_H

#include <QObject>
#include <unordered_map>
#include <vector>
#include <QVector>
#include "packet.h"

class Metrics : public QObject {
    Q_OBJECT

    struct PortMetrics {
        std::vector<size_t> queueLengths;
        std::vector<int> droppedPackets;
        std::vector<int> forwardedPackets;
        std::vector<SimTime> timestamps;
    };

    std::unordered_map<int, PortMetrics> _portMetrics;

public:
    explicit Metrics(QObject* parent);
    QVector<double> getQueueLengths(int portId) const;
    QVector<int> getDroppedPackets(int portId) const;
    QVector<int> getForwardedPackets(int portId) const;
    QVector<double> getTimestamps(int portId) const;

public slots:
    void record(int portId, size_t queueLen, int dropped, int forwarded, SimTime timestamp);

signals:
    void updated(int portId, size_t queueLen, int dropped, int forwarded, SimTime timestamp);
};

extern Metrics* metrics;

#endif
