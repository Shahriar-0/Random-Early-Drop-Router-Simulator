#include "visualization.h"

Visualization::Visualization(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    _tabs = new QTabWidget(this);

    _timeSeriesView = new QChartView();
    _timeSeriesView->setRenderHint(QPainter::Antialiasing);

    _dropFunctionView = new QChartView();
    _dropFunctionView->setRenderHint(QPainter::Antialiasing);

    _tabs->addTab(_timeSeriesView, "Time Series");
    _tabs->addTab(_dropFunctionView, "Drop Function");

    layout->addWidget(_tabs);
    setLayout(layout);
}

void Visualization::setMetrics(Metrics* metrics) {
    _metrics = metrics;
}

void Visualization::setREDParams(double minTh, double maxTh, double maxP, int capacity) {
    _minTh = minTh;
    _maxTh = maxTh;
    _maxP = maxP;
    _capacity = capacity;
}

void Visualization::plotTimeSeries(int portId) {
    if (!_metrics) return;

    QLineSeries* queueSeries = new QLineSeries();
    QScatterSeries* dropSeries = new QScatterSeries();
    dropSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    dropSeries->setColor(Qt::red);
    dropSeries->setMarkerSize(8.0);

    QVector<double> timestamps = _metrics->getTimestamps(portId);
    QVector<double> queueLengths = _metrics->getQueueLengths(portId);
    QVector<int> drops = _metrics->getDroppedPackets(portId);

    for (int i = 0; i < timestamps.size(); ++i) {
        queueSeries->append(timestamps[i], queueLengths[i]);
        if (drops[i] > 0) {
            dropSeries->append(timestamps[i], queueLengths[i]);
        }
    }

    QChart* chart = new QChart();
    chart->addSeries(queueSeries);
    chart->addSeries(dropSeries);
    chart->createDefaultAxes();
    chart->setTitle("Queue Length and Packet Drops");
    chart->axisX()->setTitleText("Time (s)");
    chart->axisY()->setTitleText("Queue Length");

    _timeSeriesView->setChart(chart);
}

void Visualization::plotDropFunction() {
    QLineSeries* dropSeries = new QLineSeries();

    for (int qlen = 0; qlen <= _capacity; ++qlen) {
        double dropProb = 0.0;
        if (qlen > _maxTh) {
            dropProb = 1.0;
        }
        else if (qlen > _minTh) {
            dropProb = _maxP * (qlen - _minTh) / (_maxTh - _minTh);
        }
        dropSeries->append(qlen, dropProb);
    }

    QChart* chart = new QChart();
    chart->addSeries(dropSeries);
    chart->createDefaultAxes();
    chart->setTitle("RED Drop Probability Function");
    chart->axisX()->setTitleText("Queue Length");
    chart->axisY()->setTitleText("Drop Probability");
    chart->axisY()->setRange(0, 1.0);

    _dropFunctionView->setChart(chart);
}