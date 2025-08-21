#include <QApplication>

// #include "mainwindow.h"

// TODO: use these four in initialization of router and packet generator
// for packet generator obviously should use the first two
// for router, for port 1 and 2 use generator delay and for port 3 use receiver delay
constexpr int TIME_SCALE = 0.5; // preferably change this instead of distributions
constexpr int GENERATOR_TRANSMISSION_DELAY = 2 * TIME_SCALE;
constexpr int GENERATOR_PROPAGATION_DELAY = 2 * TIME_SCALE;
constexpr int RECEIVER_TRANSMISSION_DELAY = TIME_SCALE;
constexpr int RECEIVER_PROPAGATION_DELAY = TIME_SCALE;

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    // TODO:
    // MainWindow w;
    // w.show();
    return a.exec();
}