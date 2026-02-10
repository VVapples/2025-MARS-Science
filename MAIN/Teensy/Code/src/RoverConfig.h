#pragma once

#include <stdint.h>

// -------------------------------
// Operation Modes and Mock Flags
// -------------------------------
enum RoverMode : uint8_t {
    MODE_REAL = 0,
    MODE_FULL_MOCK = 1,
    MODE_PARTIAL_MOCK = 2
};

constexpr RoverMode ROVER_MODE = MODE_REAL;

constexpr bool MOCK_DRILL = false;
constexpr bool MOCK_PUMP = false;
constexpr bool MOCK_GIMBAL = false;
constexpr bool MOCK_VERTICAL = false;
constexpr bool MOCK_SENSORS = false;

// -------------------------------
// Teensy Pins (GPIO numbers)
// -------------------------------
constexpr uint8_t PIN_M_D1_PG = 2;
constexpr uint8_t PIN_M_D2_PG = 3;
constexpr uint8_t PIN_M_D2_ALM = 4;
constexpr uint8_t PIN_M_A1_FAULT = 5;
constexpr uint8_t PIN_M_A2_FAULT = 6;
constexpr uint8_t PIN_M_D1_ALM = 7;
constexpr uint8_t PIN_M_A1_IN1 = 8;
constexpr uint8_t PIN_M_A1_IN2 = 9;
constexpr uint8_t PIN_M_A1_IN3 = 10;
constexpr uint8_t PIN_M_A2_IN1 = 11;
constexpr uint8_t PIN_M_A2_IN2 = 12;
constexpr uint8_t PIN_M_A2_IN3 = 13;
constexpr uint8_t PIN_S_1_SIGNAL = 14;
constexpr uint8_t PIN_S_A1_OUTPUT = 15;
constexpr uint8_t PIN_E2_SCL = 16;  // Wire1
constexpr uint8_t PIN_E2_SDA = 17;  // Wire1
constexpr uint8_t PIN_E1_SDA = 18;  // Wire0
constexpr uint8_t PIN_E1_SCL = 19;  // Wire0
constexpr uint8_t PIN_S_A2_OUTPUT = 20;
constexpr uint8_t PIN_S_A3_OUTPUT = 21;
constexpr uint8_t PIN_CAN_CRX = 22;
constexpr uint8_t PIN_CAN_CTX = 23;
constexpr uint8_t PIN_S_A4_OUTPUT = 24;
constexpr uint8_t PIN_S_A5_OUTPUT = 25;
constexpr uint8_t PIN_S_A6_OUTPUT = 26;
constexpr uint8_t PIN_S_A7_OUTPUT = 27;

// -------------------------------
// PCA9685 I2C Addresses
// -------------------------------
constexpr uint8_t PCA9685_E1_ADDR = 0x40;
constexpr uint8_t PCA9685_E2_ADDR = 0x41;

// -------------------------------
// PCA9685 Channel Mapping
// Rule: Channel = Label_Number - 3
// -------------------------------
// E-1 (PCA9685 @ 0x40 via Wire0)
constexpr uint8_t CH_E1_M_D1_SV = 0;   // R3
constexpr uint8_t CH_E1_M_D1_BK = 1;   // R4
constexpr uint8_t CH_E1_M_D1_EN = 2;   // R5
constexpr uint8_t CH_E1_M_D1_FR = 3;   // R6
constexpr uint8_t CH_E1_M_S8_SIG = 4;  // R7
constexpr uint8_t CH_E1_L_W = 5;       // R8
constexpr uint8_t CH_E1_L_D = 6;       // R9
constexpr uint8_t CH_E1_M_A1_EN = 7;   // R10
constexpr uint8_t CH_E1_M_D2_FR = 8;   // R11
constexpr uint8_t CH_E1_M_D2_EN = 9;   // R12
constexpr uint8_t CH_E1_M_D2_BK = 10;  // R13
constexpr uint8_t CH_E1_M_D2_SV = 11;  // R14
constexpr uint8_t CH_E1_M_A2_EN = 15;  // R18

// E-2 (PCA9685 @ 0x41 via Wire1)
constexpr uint8_t CH_E2_M_S1_EN = 0;   // R3
constexpr uint8_t CH_E2_M_S1_PH = 1;   // R4
constexpr uint8_t CH_E2_M_S2_EN = 2;   // R5
constexpr uint8_t CH_E2_M_S2_PH = 3;   // R6
constexpr uint8_t CH_E2_M_S3_EN = 4;   // R7
constexpr uint8_t CH_E2_M_S3_PH = 5;   // R8
constexpr uint8_t CH_E2_M_S4_EN = 6;   // R9
constexpr uint8_t CH_E2_M_S4_PH = 7;   // R10
constexpr uint8_t CH_E2_M_S7_PH = 10;  // R13
constexpr uint8_t CH_E2_M_S7_EN = 11;  // R14
constexpr uint8_t CH_E2_M_S6_PH = 12;  // R15
constexpr uint8_t CH_E2_M_S6_EN = 13;  // R16
constexpr uint8_t CH_E2_M_S5_PH = 14;  // R17
constexpr uint8_t CH_E2_M_S5_EN = 15;  // R18

// -------------------------------
// PID Constants
// -------------------------------
struct PIDGains {
    float kp;
    float ki;
    float kd;
};

constexpr PIDGains PID_DRILL = {0.8f, 0.0f, 0.02f};
constexpr PIDGains PID_PUMP = {0.6f, 0.0f, 0.01f};
constexpr PIDGains PID_GIMBAL = {1.2f, 0.02f, 0.05f};
constexpr PIDGains PID_VERTICAL = {1.0f, 0.01f, 0.03f};

// -------------------------------
// Safety Limits and Timeouts
// -------------------------------
constexpr uint32_t HEARTBEAT_TIMEOUT_MS = 1000;
constexpr uint32_t COMMAND_TIMEOUT_MS = 500;
constexpr float OVERCURRENT_LIMIT_A = 5.0f;
constexpr float OVERTEMP_LIMIT_C = 75.0f;

// -------------------------------
// Driver Defaults
// -------------------------------
constexpr uint16_t PCA9685_PWM_MAX = 4095;
constexpr uint16_t PCA9685_PWM_MIN = 0;
constexpr uint32_t MOCK_WAVE_PERIOD_MS = 2000;
constexpr uint16_t ADC_MAX = 4095;
constexpr float MOCK_SENSOR_MIN = 0.0f;
constexpr float MOCK_SENSOR_MAX = 1.0f;
constexpr float MOCK_SOIL_MIN = 0.1f;
constexpr float MOCK_SOIL_MAX = 0.9f;
constexpr float MOCK_TEMP_MIN_C = 18.0f;
constexpr float MOCK_TEMP_MAX_C = 32.0f;
constexpr float MOCK_HUMIDITY_MIN_PCT = 30.0f;
constexpr float MOCK_HUMIDITY_MAX_PCT = 70.0f;
constexpr float MOCK_PRESSURE_MIN_PA = 98000.0f;
constexpr float MOCK_PRESSURE_MAX_PA = 103000.0f;
constexpr float GIMBAL_SUPPLY_V = 12.0f;
constexpr float VERTICAL_SUPPLY_V = 12.0f;
constexpr float VERTICAL_SPEED_SCALE = 32767.0f;
constexpr uint16_t PCA9685_PWM_FREQ_HZ = 1000;
constexpr uint8_t BME280_I2C_ADDR = 0x76;
constexpr uint8_t GIMBAL_POLE_PAIRS = 7;
constexpr uint8_t VERTICAL_POLE_PAIRS = 7;
constexpr uint32_t VERTICAL_CAN_ID = 0x100;
constexpr uint16_t SERVO_PULSE_MIN_US = 1000;
constexpr uint16_t SERVO_PULSE_NEUTRAL_US = 1500;
constexpr uint16_t SERVO_PULSE_MAX_US = 2000;
