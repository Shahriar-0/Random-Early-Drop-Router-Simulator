#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include "metrics.h"
#include "packet_generator.h"
#include "router.h"
#include "simulator.h"

class Worker : public QObject {
    Q_OBJECT

    double _minTh, _maxTh, _maxP, _wq;
    int _duration;

public:
    Worker(double minTh, double maxTh, double maxP, double wq, int duration);

public slots:
    void run();

signals:
    void finished();
    void progress(int percent);
};

#endif