#ifndef QUEUE_H
#define QUEUE_H

#include <deque>
#include <memory>
#include <random>

#include "packet.h"

class REDQueue {
    size_t _cap;
    double _wq, _minTh, _maxTh, _maxP;
    double _avg{0};
    uint64_t _count{0};
    std::deque<PacketPtr> _dq;
    std::mt19937 _rng{std::random_device{}()};

public:
    REDQueue(size_t cap, double wq, double minTh, double maxTh, double maxP);
    bool offer(const PacketPtr& p);
    PacketPtr poll();
    size_t size() const;

private:
    void enqueue(const PacketPtr& p);
};

#endif