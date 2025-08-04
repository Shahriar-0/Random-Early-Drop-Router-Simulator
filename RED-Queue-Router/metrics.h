#ifndef METRICS_H
#define METRICS_H

#include <QObject>
#include <QVector>
#include <vector>

#include "packet.h"

class Metrics : public QObject {
    Q_OBJECT
    std::vector<double> _queueLens;
    std::vector<int> _drops, _fwds;
    std::vector<SimTime> _times;

public:
    explicit Metrics(QObject* parent = nullptr);

public slots:
    void record(size_t qlen, int dropped, int fwd, SimTime t);

signals:
    void updated(size_t queueLen, int dropped, int forwarded, SimTime timestamp);

    QVector<size_t> queueLengths() const;
    QVector<int> droppedPackets() const;
    QVector<int> forwardedPackets() const;
    QVector<double> timestamps() const;
};

extern Metrics* metrics;

#endif // METRICS_H
