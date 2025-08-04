#ifndef EVENT_H
#define EVENT_H

#include <memory>

#include "packet.h"

using PacketPtr = std::shared_ptr<Packet>;

enum class EventType { ARRIVAL,
                       DEPARTURE,
                       TIMER };

struct Event {
    EventType type;
    SimTime time;
    int nodeId;
    PacketPtr pkt;
};

#endif