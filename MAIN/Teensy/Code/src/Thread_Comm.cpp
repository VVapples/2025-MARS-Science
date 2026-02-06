#include <Arduino.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "CommandSequencer.h"
#include "SensorRelay.h"

void Safety_NotifyHeartbeat();

namespace {
constexpr size_t kLineBufferSize = 96;
char g_lineBuffer[kLineBufferSize];
size_t g_lineLen = 0;

bool startsWith(const char *line, const char *prefix) {
    return strncmp(line, prefix, strlen(prefix)) == 0;
}

bool parseUIntParam(const char *line, char key, uint32_t &value) {
    const char *pos = strchr(line, key);
    if (!pos) {
        return false;
    }
    char *end = nullptr;
    unsigned long parsed = strtoul(pos + 1, &end, 10);
    if (end == pos + 1) {
        return false;
    }
    value = static_cast<uint32_t>(parsed);
    return true;
}

bool parseFloatParam(const char *line, char key, float &value) {
    const char *pos = strchr(line, key);
    if (!pos) {
        return false;
    }
    char *end = nullptr;
    float parsed = strtof(pos + 1, &end);
    if (end == pos + 1) {
        return false;
    }
    value = parsed;
    return true;
}

bool motorFromIndex(uint32_t index, CommandSequencer::MotorId &motor) {
    switch (index) {
        case 1:
            motor = CommandSequencer::MotorId::DRILL;
            return true;
        case 2:
            motor = CommandSequencer::MotorId::PUMP;
            return true;
        case 3:
            motor = CommandSequencer::MotorId::GIMBAL;
            return true;
        case 4:
            motor = CommandSequencer::MotorId::VERTICAL;
            return true;
        default:
            return false;
    }
}

void handleStopAll(CommandSequencer *sequencer) {
    if (!sequencer) {
        return;
    }
    CommandSequencer::Command cmd;
    cmd.type = CommandSequencer::CommandType::STOP;
    cmd.speed = 0.0f;
    cmd.durationMs = 0;

    cmd.motor = CommandSequencer::MotorId::DRILL;
    sequencer->enqueue(cmd);
    cmd.motor = CommandSequencer::MotorId::PUMP;
    sequencer->enqueue(cmd);
    cmd.motor = CommandSequencer::MotorId::GIMBAL;
    sequencer->enqueue(cmd);
    cmd.motor = CommandSequencer::MotorId::VERTICAL;
    sequencer->enqueue(cmd);
}

void printSensors(SensorRelay *relay) {
    if (!relay) {
        return;
    }
    SensorRelay::Snapshot snapshot = relay->latest();
    Serial.print("SOIL=");
    Serial.print(snapshot.soilMoisture, 4);
    Serial.print(" TEMP=");
    Serial.print(snapshot.temperatureC, 2);
    Serial.print(" HUM=");
    Serial.print(snapshot.humidityPct, 2);
    Serial.print(" PRES=");
    Serial.print(snapshot.pressurePa, 1);
    Serial.print(" TS=");
    Serial.println(snapshot.timestampMs);
}

void processLine(char *line, CommandSequencer *sequencer, SensorRelay *relay) {
    while (isspace(*line)) {
        ++line;
    }
    if (*line == '\0') {
        return;
    }

    if (startsWith(line, "HB")) {
        Safety_NotifyHeartbeat();
        Serial.println("OK");
        return;
    }

    if (startsWith(line, "STOP")) {
        handleStopAll(sequencer);
        Serial.println("OK");
        return;
    }

    if (startsWith(line, "SENS")) {
        printSensors(relay);
        return;
    }

    if (startsWith(line, "G1")) {
        uint32_t motorIndex = 0;
        float speed = 0.0f;
        uint32_t duration = 0;

        if (!parseUIntParam(line, 'M', motorIndex)) {
            Serial.println("ERR MISSING_M");
            return;
        }
        if (!parseFloatParam(line, 'S', speed)) {
            Serial.println("ERR MISSING_S");
            return;
        }
        parseUIntParam(line, 'T', duration);

        CommandSequencer::MotorId motor;
        if (!motorFromIndex(motorIndex, motor)) {
            Serial.println("ERR BAD_M");
            return;
        }

        if (speed > 1.0f || speed < -1.0f) {
            speed /= 100.0f;
        }

        CommandSequencer::Command cmd;
        cmd.type = CommandSequencer::CommandType::SET_SPEED;
        cmd.motor = motor;
        cmd.speed = speed;
        cmd.durationMs = duration;

        if (!sequencer || !sequencer->enqueue(cmd)) {
            Serial.println("ERR QUEUE_FULL");
            return;
        }

        Serial.println("OK");
        return;
    }

    Serial.println("ERR UNKNOWN");
}
}

void ThreadComm_Init() {
    g_lineLen = 0;
}

void ThreadComm_Update(CommandSequencer *sequencer, SensorRelay *relay) {
    while (Serial.available() > 0) {
        int ch = Serial.read();
        if (ch == '\n' || ch == '\r') {
            if (g_lineLen > 0) {
                g_lineBuffer[g_lineLen] = '\0';
                processLine(g_lineBuffer, sequencer, relay);
                g_lineLen = 0;
            }
            continue;
        }
        if (g_lineLen + 1 < kLineBufferSize) {
            g_lineBuffer[g_lineLen++] = static_cast<char>(ch);
        } else {
            g_lineLen = 0;
        }
    }
}
