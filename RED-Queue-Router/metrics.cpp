#include "metrics.h"

Metrics::Metrics(QObject* parent) : QObject(parent) {}

void Metrics::record(int portId, size_t queueLen, int dropped, int forwarded, SimTime timestamp) {
    _portMetrics[portId].queueLengths.push_back(queueLen);
    _portMetrics[portId].droppedPackets.push_back(dropped);
    _portMetrics[portId].forwardedPackets.push_back(forwarded);
    _portMetrics[portId].timestamps.push_back(timestamp);

    emit updated(portId, queueLen, dropped, forwarded, timestamp);
}

QVector<double> Metrics::getQueueLengths(int portId) const {
    auto it = _portMetrics.find(portId);
    if (it != _portMetrics.end()) {
        return QVector<double>(it->second.queueLengths.begin(), it->second.queueLengths.end());
    }
    return {};
}

QVector<int> Metrics::getDroppedPackets(int portId) const {
    auto it = _portMetrics.find(portId);
    if (it != _portMetrics.end()) {
        return QVector<int>(it->second.droppedPackets.begin(), it->second.droppedPackets.end());
    }
    return {};
}

QVector<int> Metrics::getForwardedPackets(int portId) const {
    auto it = _portMetrics.find(portId);
    if (it != _portMetrics.end()) {
        return QVector<int>(it->second.forwardedPackets.begin(), it->second.forwardedPackets.end());
    }
    return {};
}

QVector<double> Metrics::getTimestamps(int portId) const {
    auto it = _portMetrics.find(portId);
    if (it != _portMetrics.end()) {
        return QVector<double>(it->second.timestamps.begin(), it->second.timestamps.end());
    }
    return {};
}

Metrics* metrics = new Metrics();
