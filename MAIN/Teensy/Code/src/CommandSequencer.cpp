#include "CommandSequencer.h"

#include "MotorDrivers.h"

CommandSequencer::CommandSequencer()
    : drill_(nullptr),
      pump_(nullptr),
      gimbal_(nullptr),
      vertical_(nullptr),
      head_(0),
      tail_(0),
      count_(0),
      running_(false),
      active_{CommandType::STOP, MotorId::DRILL, 0.0f, 0},
      commandStartMs_(0) {}

void CommandSequencer::attachMotors(DrillMotor *drill, PumpMotor *pump, GimbalMotor *gimbal, VerticalMotor *vertical) {
    drill_ = drill;
    pump_ = pump;
    gimbal_ = gimbal;
    vertical_ = vertical;
}

void CommandSequencer::begin() {
    if (drill_) {
        drill_->begin();
    }
    if (pump_) {
        pump_->begin();
    }
    if (gimbal_) {
        gimbal_->begin();
    }
    if (vertical_) {
        vertical_->begin();
    }
}

bool CommandSequencer::enqueue(const Command &cmd) {
    if (count_ >= kQueueSize) {
        return false;
    }
    queue_[tail_] = cmd;
    tail_ = (tail_ + 1) % kQueueSize;
    ++count_;
    return true;
}

void CommandSequencer::clear() {
    head_ = 0;
    tail_ = 0;
    count_ = 0;
    running_ = false;
}

void CommandSequencer::update() {
    if (!running_) {
        if (count_ == 0) {
            return;
        }
        active_ = queue_[head_];
        head_ = (head_ + 1) % kQueueSize;
        --count_;
        startCommand(active_);
        commandStartMs_ = millis();
        running_ = true;
        return;
    }

    if (active_.durationMs == 0) {
        running_ = false;
        return;
    }

    uint32_t elapsed = millis() - commandStartMs_;
    if (elapsed >= active_.durationMs) {
        if (active_.type == CommandType::SET_SPEED) {
            stopMotor(active_.motor);
        }
        running_ = false;
    }
}

bool CommandSequencer::isIdle() const {
    return !running_ && count_ == 0;
}

void CommandSequencer::startCommand(const Command &cmd) {
    switch (cmd.type) {
        case CommandType::SET_SPEED:
            setMotorSpeed(cmd.motor, cmd.speed);
            break;
        case CommandType::STOP:
        default:
            stopMotor(cmd.motor);
            break;
    }
}

void CommandSequencer::stopMotor(MotorId motor) {
    switch (motor) {
        case MotorId::DRILL:
            if (drill_) {
                drill_->stop();
            }
            break;
        case MotorId::PUMP:
            if (pump_) {
                pump_->stop();
            }
            break;
        case MotorId::GIMBAL:
            if (gimbal_) {
                gimbal_->stop();
            }
            break;
        case MotorId::VERTICAL:
            if (vertical_) {
                vertical_->stop();
            }
            break;
        default:
            break;
    }
}

void CommandSequencer::setMotorSpeed(MotorId motor, float speed) {
    switch (motor) {
        case MotorId::DRILL:
            if (drill_) {
                drill_->setSpeed(speed);
            }
            break;
        case MotorId::PUMP:
            if (pump_) {
                pump_->setSpeed(speed);
            }
            break;
        case MotorId::GIMBAL:
            if (gimbal_) {
                gimbal_->setSpeed(speed);
            }
            break;
        case MotorId::VERTICAL:
            if (vertical_) {
                vertical_->setSpeed(speed);
            }
            break;
        default:
            break;
    }
}
