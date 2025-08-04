#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <string>

using SimTime = double;

class Packet {
    uint64_t _id;
    SimTime _creation_time;
    size_t _size;
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
};

#endif