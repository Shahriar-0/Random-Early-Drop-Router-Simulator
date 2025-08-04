#include "queue.h"
#include <cassert>

REDQueue::REDQueue(size_t cap, double wq, double minTh, double maxTh, double maxP)
    : _cap(cap), _wq(wq), _minTh(minTh), _maxTh(maxTh), _maxP(maxP) {}

size_t REDQueue::size() const { return _dq.size(); }

void REDQueue::enqueue(const PacketPtr& p) {
    assert(_dq.size() < _cap && "enqueue called when queue is full!");
    _dq.push_back(p);
}

bool REDQueue::offer(const PacketPtr& p) {
    _avg = (1 - _wq) * _avg + _wq * _dq.size();
    if (_avg < _minTh) {
        enqueue(p);
        _count = 0; // reset after below-threshold enqueue
        return true;
    }
    if (_avg >= _maxTh) {
        _count = 0;
        return false;
    }

    double Pd = (_avg - _minTh) / (_maxTh - _minTh) * _maxP;
    double Pa = Pd / (1.0 - _count * Pd);
    if (Pa > 1.0) Pa = 1.0; // just in case

    if (std::bernoulli_distribution(Pa)(_rng)) {
        _count = 0;
        return false;
    }
    enqueue(p);
    _count++;
    return true;
}

PacketPtr REDQueue::poll() {
    if (_dq.empty()) return {};
    auto p = _dq.front(); _dq.pop_front();
    return p;
}