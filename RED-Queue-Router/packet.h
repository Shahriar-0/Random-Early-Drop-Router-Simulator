#ifndef PACKET_H
#define PACKET_H

#include <cstddef>

struct Packet {
    int      id;
    double   createdAt;
    size_t   sizeBytes;
    int      src;
    int      dst;
    Packet(int id_, double t, size_t sz, int s, int d)
        : id(id_), createdAt(t), sizeBytes(sz), src(s), dst(d) {}
};

#endif // PACKET_H
