#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <memory>
#include <string>

using SimTime = double;

class Packet {
    uint64_t _id;
    SimTime _creation_time;
    size_t _size; // ?: do we need fractional size or this would do?
    int _src, _dst;
    std::string _content;

public:
    Packet(uint64_t id, SimTime creation, size_t size, int src, int dst, std::string content = "")
        : _id(id), _creation_time(creation), _size(size), _src(src), _dst(dst), _content(content) {}

    uint64_t id() const { return _id; }
    SimTime creationTime() const { return _creation_time; }
    size_t size() const { return _size; }
    int src() const { return _src; }
    int dst() const { return _dst; }
    std::string content() const { return _content; }

    bool operator==(const Packet& other) const {
        return _id == other._id &&
               _creation_time == other._creation_time &&
               _size == other._size &&
               _src == other._src &&
               _dst == other._dst &&
               _content == other._content;
        // although id should be enough because of static in generators but just to be safe
    }
};

using PacketPtr = std::shared_ptr<Packet>;
#endif