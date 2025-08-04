#ifndef EVENT_H
#define EVENT_H

#include "packet.h"

enum class EventType {
    ARRIVAL,
    DEPARTURE,
    TIMER,
    RESUME,
};

struct Event {
    EventType type;
    SimTime time;
    int nodeId;
    PacketPtr pkt;

    bool operator>(const Event& other) const { return time > other.time; }
};

#endif