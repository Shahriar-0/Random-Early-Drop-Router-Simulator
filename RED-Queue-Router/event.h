#ifndef EVENT_H
#define EVENT_H

#include "packet.h"

enum class EventType {
    ARRIVAL,
    DEPARTURE, // ended up not using this but still keeping it just in case
    TIMER,
    RESUME,
    RECEIVED,
};

struct Event {
    EventType type;
    SimTime time;
    int nodeId;
    PacketPtr pkt;

    bool operator>(const Event& other) const { return time > other.time; }
};

#endif