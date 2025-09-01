#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

#include <deque>
#include "event.h"

class BoundedQueue {
    std::deque<PacketPtr> _dq;
    size_t _cap;
public:
    explicit BoundedQueue(size_t cap=6) : _cap(cap) {}
    size_t size()   const noexcept { return _dq.size(); }
    size_t capacity() const noexcept { return _cap; }
    bool empty()    const noexcept { return _dq.empty(); }

    bool push(PacketPtr p) {
        if (_dq.size() >= _cap) return false;
        _dq.push_back(std::move(p));
        return true;
    }

    PacketPtr front() const { return _dq.empty() ? nullptr : _dq.front(); }

    PacketPtr pop() {
        if (_dq.empty()) return nullptr;
        auto p = _dq.front();
        _dq.pop_front();
        return p;
    }
};

#endif // BOUNDED_QUEUE_H
