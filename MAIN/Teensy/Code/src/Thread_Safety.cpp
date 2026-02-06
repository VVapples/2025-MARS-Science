#include <Arduino.h>

#include "RoverConfig.h"

namespace {
bool g_estopLatched = false;
uint32_t g_lastHeartbeatMs = 0;
}

void Safety_Init() {
    g_estopLatched = false;
    g_lastHeartbeatMs = millis();
}

void Safety_NotifyHeartbeat() {
    g_lastHeartbeatMs = millis();
}

void Safety_Update() {
    if (g_estopLatched) {
        return;
    }

    uint32_t now = millis();
    if (now - g_lastHeartbeatMs > HEARTBEAT_TIMEOUT_MS) {
        g_estopLatched = true;
    }
}

bool Safety_EStopActive() {
    return g_estopLatched;
}
