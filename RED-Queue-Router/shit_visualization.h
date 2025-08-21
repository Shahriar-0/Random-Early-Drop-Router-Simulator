#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QWidget>
#include <QtCharts>

#include "metrics.h"

class Visualization : public QWidget {
    Q_OBJECT

    QTabWidget* _tabs;
    QChartView* _timeSeriesView;
    QChartView* _dropFunctionView;
    Metrics* _metrics;
    double _minTh, _maxTh, _maxP;
    int _capacity;

public:
    Visualization(QWidget* parent = nullptr);
    void setMetrics(Metrics* metrics);
    void setREDParams(double minTh, double maxTh, double maxP, int capacity);
    void plotTimeSeries(int portId);
    void plotDropFunction();
};

#endif