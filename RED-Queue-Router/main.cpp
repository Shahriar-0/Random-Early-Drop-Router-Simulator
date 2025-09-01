#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QDir>
#include <QDebug>

#include "simulator.h"
#include "router.h"
#include "packet_generator.h"
#include "metrics.h"

static void writeRedTable(const QString& path) {
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f);
        out << "queue_len,p_drop\n";
        for (int q = 0; q <= 6; ++q) {
            double p;
            if (q == 0 || q == 1) p = 0.0;
            else if (q >= 5)       p = 1.0;
            else                   p = 0.3 * q - 0.4; // 2->0.2, 3->0.5, 4->0.8
            out << q << "," << p << "\n";
        }
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("RED-Queue-Router");
    QCoreApplication::setApplicationVersion("1.2");

    QCommandLineParser parser;
    parser.setApplicationDescription("RED Router Simulator");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption seedOpt(QStringList() << "s" << "seed", "Random seed (uint).", "seed", "0");
    QCommandLineOption untilOpt(QStringList() << "t" << "until", "Simulation time horizon.", "until", "50.0");
    parser.addOption(seedOpt);
    parser.addOption(untilOpt);
    parser.process(app);

    uint32_t seed = parser.value(seedOpt).toUInt();
    double until = parser.value(untilOpt).toDouble();
    qInfo() << "ARGS:" << "until=" << until << "seed=" << seed << "cwd=" << QDir::currentPath();

    Simulator sim;
    Metrics m;
    sim.metrics = &m;

    Router router(100, &sim);
    router.setBufferCap(6);
    router.setOutput(200, 1.0, 0.0);

    PacketGenerator genA(1, 100, 2.0, 2.0, 0.0, &sim);
    PacketGenerator genB(2, 100, 2.0, 2.0, 0.0, &sim);
    // PacketGenerator genC(1, 100, 2.0, 2.0, 0.0, &sim);


    if (seed != 0) {
        genA.setSeed(seed + 1);
        genB.setSeed(seed + 2);
        // genC.setSeed(seed + 3);
    }

    QObject::connect(&router, &Router::congested, &genA, &PacketGenerator::onCongested);
    QObject::connect(&router, &Router::congested, &genB, &PacketGenerator::onCongested);
    // QObject::connect(&router, &Router::congested, &genC, &PacketGenerator::onCongested);


    genA.start(0.0);
    genB.start(0.0);
    // genC.start(0.0);

    sim.run(until);

    QDir().mkpath("logs");
    writeRedTable("logs/red_probability_table.csv");

    {
        QFile fe("logs/events.csv");
        if (fe.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&fe);
            out << "t,queue_len,drop,forward\n";
            auto ts = m.getTimes(0);
            auto ql = m.getQueueLens(0);
            auto dr = m.getDropped(0);
            auto fw = m.getForwarded(0);
            int n = std::min({ts.size(), ql.size(), dr.size(), fw.size()});
            for (int i=0; i<n; ++i) {
                out << ts[i] << "," << ql[i] << "," << dr[i] << "," << fw[i] << "\n";
            }
        }
    }

    int totalDrops = 0, totalFwds = 0, samples = 0;
    QVector<int> dropByQ(7, 0);      // q=0..6
    QVector<int> fwdByQ(7, 0);
    {
        auto ts = m.getTimes(0);
        auto ql = m.getQueueLens(0);
        auto dr = m.getDropped(0);
        auto fw = m.getForwarded(0);
        int n = std::min({ts.size(), ql.size(), dr.size(), fw.size()});
        samples = n;
        for (int i=0; i<n; ++i) {
            totalDrops += dr[i];
            totalFwds  += fw[i];
            int q = (ql[i] >= 0 && ql[i] <= 6) ? ql[i] : (ql[i] < 0 ? 0 : 6);
            if (dr[i]) dropByQ[q]++;
            if (fw[i]) fwdByQ[q]++;
        }
    }

    {
        QFile fd("logs/drops_by_queue_len.csv");
        if (fd.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&fd);
            out << "queue_len,drop_count\n";
            for (int q=0; q<=6; ++q) out << q << "," << dropByQ[q] << "\n";
        }
        QFile ff("logs/forward_by_queue_len.csv");
        if (ff.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&ff);
            out << "queue_len,forward_count\n";
            for (int q=0; q<=6; ++q) out << q << "," << fwdByQ[q] << "\n";
        }
    }

    {
        QFile flog("logs/sim_summary.txt");
        if (flog.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&flog);
            out << "samples=" << samples << "\n";
            out << "total_forwarded=" << totalFwds << "\n";
            out << "total_dropped=" << totalDrops << "\n";
        }
    }

    qInfo() << "DONE:" << "samples=" << samples << "forwarded=" << totalFwds << "dropped=" << totalDrops
            << "logs=" << (QDir::currentPath() + "/logs");

    return 0;
}
