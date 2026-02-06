#pragma once

#include <Arduino.h>

class DrillMotor;
class PumpMotor;
class GimbalMotor;
class VerticalMotor;

class CommandSequencer {
public:
    enum class MotorId : uint8_t {
        DRILL = 0,
        PUMP = 1,
        GIMBAL = 2,
        VERTICAL = 3
    };

    enum class CommandType : uint8_t {
        SET_SPEED = 0,
        STOP = 1
    };

    struct Command {
        CommandType type;
        MotorId motor;
        float speed;
        uint32_t durationMs;
    };

    CommandSequencer();

    void attachMotors(DrillMotor *drill, PumpMotor *pump, GimbalMotor *gimbal, VerticalMotor *vertical);
    void begin();
    bool enqueue(const Command &cmd);
    void clear();
    void update();
    bool isIdle() const;

private:
    static constexpr size_t kQueueSize = 16;

    void startCommand(const Command &cmd);
    void stopMotor(MotorId motor);
    void setMotorSpeed(MotorId motor, float speed);

    DrillMotor *drill_;
    PumpMotor *pump_;
    GimbalMotor *gimbal_;
    VerticalMotor *vertical_;

    Command queue_[kQueueSize];
    size_t head_;
    size_t tail_;
    size_t count_;

    bool running_;
    Command active_;
    uint32_t commandStartMs_;
};
