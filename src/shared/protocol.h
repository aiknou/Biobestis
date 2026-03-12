#pragma once

// Shared data structures and constants for transmitter <-> receiver communication.
// Both devices include this header so the packet format stays in sync.

// WiFi credentials (used by both sides)
#define WIFI_SSID     "aalto open"
#define WIFI_PASSWORD ""

// UDP port the receiver listens on
#define UDP_PORT 4210

// Packet sent from transmitter to receiver
struct SensorPacket {
    uint8_t  magic[2];   // 0xCA, 0xFE — sanity check
    int8_t   fullness;   // 0–100 %
    int8_t   humidity;   // 0–100 %
};

inline bool packetValid(const SensorPacket& p) {
    return p.magic[0] == 0xCA && p.magic[1] == 0xFE;
}
