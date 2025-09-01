#ifndef METRICS_H
#define METRICS_H

#include <QObject>
#include <QVector>

class Metrics : public QObject {
    Q_OBJECT
public:
    explicit Metrics(QObject* parent=nullptr) : QObject(parent) {}

    void record(int portId, int queued, int dropped, int forwarded, double t) {
        if (portId >= _queueLens.size()) {
            int newSize = portId + 1;
            _queueLens.resize(newSize);
            _drops.resize(newSize);
            _fwds.resize(newSize);
            _times.resize(newSize);
        }
        _queueLens[portId].push_back(queued);
        _drops[portId].push_back(dropped);
        _fwds[portId].push_back(forwarded);
        _times[portId].push_back(t);
    }

    QVector<int> getQueueLens(int portId) const { return _queueLens.value(portId); }
    QVector<int> getDropped(int portId)   const { return _drops.value(portId); }
    QVector<int> getForwarded(int portId) const { return _fwds.value(portId); }
    QVector<double> getTimes(int portId)  const { return _times.value(portId); }

private:
    QVector<QVector<int>> _queueLens;
    QVector<QVector<int>> _drops;
    QVector<QVector<int>> _fwds;
    QVector<QVector<double>> _times;
};

#endif // METRICS_H
