#include "metrics.h"

Metrics::Metrics(QObject* parent) : QObject(parent) {}
Metrics* metrics = new Metrics;

void Metrics::record(size_t qlen, int dropped, int fwd, SimTime t) {
    _queueLens.push_back(qlen);
    _drops.push_back(dropped);
    _fwds.push_back(fwd);
    _times.push_back(t);
    emit updated(qlen, dropped, fwd, t);
}

QVector<size_t> Metrics::queueLengths() const {
    return QVector<size_t>(_queueLens.begin(), _queueLens.end());
}

QVector<int> Metrics::droppedPackets() const {
    return QVector<int>(_drops.begin(), _drops.end());
}

QVector<int> Metrics::forwardedPackets() const {
    return QVector<int>(_fwds.begin(), _fwds.end());
}

QVector<double> Metrics::timestamps() const {
    return QVector<double>(_times.begin(), _times.end());
}