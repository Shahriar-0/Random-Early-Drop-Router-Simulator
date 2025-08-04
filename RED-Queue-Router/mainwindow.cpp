#include "mainwindow.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    resize(1200, 800);
}

void MainWindow::setupUi() {
    QWidget* central = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;

    QGroupBox* paramGroup = new QGroupBox("RED Parameters");
    QGridLayout* paramLayout = new QGridLayout;

    paramLayout->addWidget(new QLabel("Min Threshold:"), 0, 0);
    _minThSpin = new QDoubleSpinBox;
    _minThSpin->setRange(0, 100);
    _minThSpin->setValue(2);
    paramLayout->addWidget(_minThSpin, 0, 1);

    paramLayout->addWidget(new QLabel("Max Threshold:"), 1, 0);
    _maxThSpin = new QDoubleSpinBox;
    _maxThSpin->setRange(0, 100);
    _maxThSpin->setValue(4);
    paramLayout->addWidget(_maxThSpin, 1, 1);

    paramLayout->addWidget(new QLabel("Max Probability:"), 2, 0);
    _maxPSpin = new QDoubleSpinBox;
    _maxPSpin->setRange(0, 1);
    _maxPSpin->setValue(0.2);
    paramLayout->addWidget(_maxPSpin, 2, 1);

    paramLayout->addWidget(new QLabel("Weight (wq):"), 3, 0);
    _wqSpin = new QDoubleSpinBox;
    _wqSpin->setRange(0.001, 1.0);
    _wqSpin->setValue(0.002);
    paramLayout->addWidget(_wqSpin, 3, 1);

    paramLayout->addWidget(new QLabel("Duration (s):"), 4, 0);
    _durationSpin = new QSpinBox;
    _durationSpin->setRange(1, 1000);
    _durationSpin->setValue(100);
    paramLayout->addWidget(_durationSpin, 4, 1);

    paramGroup->setLayout(paramLayout);
    mainLayout->addWidget(paramGroup);

    _visualization = new Visualization;
    mainLayout->addWidget(_visualization);

    _runButton = new QPushButton("Run Simulation");
    connect(_runButton, &QPushButton::clicked, this, &MainWindow::runSimulation);
    mainLayout->addWidget(_runButton);

    central->setLayout(mainLayout);
    setCentralWidget(central);
}

void MainWindow::runSimulation() {
    _runButton->setEnabled(false);

    _worker = new Worker(
        _minThSpin->value(),
        _maxThSpin->value(),
        _maxPSpin->value(),
        _wqSpin->value(),
        _durationSpin->value());

    _workerThread = new QThread;
    _worker->moveToThread(_workerThread);

    connect(_workerThread, &QThread::started, _worker, &Worker::run);
    connect(_worker, &Worker::finished, _workerThread, &QThread::quit);
    connect(_worker, &Worker::finished, this, &MainWindow::simulationFinished);
    connect(_worker, &Worker::progress, this, &MainWindow::updateProgress);

    _workerThread->start();
}

void MainWindow::simulationFinished() {
    // _visualization->setMetrics(metrics);
    _visualization->setREDParams(
        _minThSpin->value(),
        _maxThSpin->value(),
        _maxPSpin->value(),
        6 // Queue capacity
    );
    _visualization->plotTimeSeries(100); // Receiver port ID=100
    _visualization->plotDropFunction();

    _worker->deleteLater();
    _workerThread->deleteLater();
    _runButton->setEnabled(true);
}

void MainWindow::updateProgress(int percent) {
    statusBar()->showMessage(QString("Simulating: %1%").arg(percent));
}

MainWindow::~MainWindow() {
    if (_workerThread && _workerThread->isRunning()) {
        _workerThread->quit();
        _workerThread->wait();
    }
}