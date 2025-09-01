#ifndef EVENT_H
#define EVENT_H

#include <memory>

using SimTime = double;

enum class EventType { ARRIVAL, DEPARTURE, TIMER };

struct Packet;
using PacketPtr = std::shared_ptr<Packet>;

struct Event {
    EventType type {};
    SimTime   time {0.0};
    int       nodeId {0};
    PacketPtr pkt {};

    bool operator>(const Event& other) const noexcept {
        return time > other.time;
    }
};

#endif // EVENT_H
