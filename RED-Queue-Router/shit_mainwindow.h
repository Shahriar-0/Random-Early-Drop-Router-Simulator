#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "visualization.h"
#include "worker.h"

QT_BEGIN_NAMESPACE
class QPushButton;
class QDoubleSpinBox;
class QSpinBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    Visualization* _visualization;
    QPushButton* _runButton;
    QDoubleSpinBox *_minThSpin, *_maxThSpin, *_maxPSpin, *_wqSpin;
    QSpinBox* _durationSpin;
    Worker* _worker;
    QThread* _workerThread;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void runSimulation();
    void simulationFinished();
    void updateProgress(int percent);

private:
    void setupUi();
};

#endif