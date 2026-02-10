#ifndef ROVER_CONFIG_H
#define ROVER_CONFIG_H

// Global configuration for pins, IDs, mock settings, and logging.

// ----------------------------
// Operation modes (mock system)
// ----------------------------
#define MODE_REAL 0
#define MODE_FULL_MOCK 1
#define MODE_PARTIAL_MOCK 2

// Select active mode here.
#define MOCK_MODE MODE_PARTIAL_MOCK

// Subsystem mock flags (only used in MODE_PARTIAL_MOCK)
static const bool MOCK_DRILL = true;     // M-D1, M-D2
static const bool MOCK_PUMP = true;      // M-S1..M-S7
static const bool MOCK_SERVO = true;     // M-S8
static const bool MOCK_GIMBAL = true;    // M-A1, M-A2
static const bool MOCK_VERTICAL = true;  // M-D3 (CAN)
static const bool MOCK_SENSORS = true;   // All sensors
static const bool MOCK_CAN = true;       // CAN bus (M-D3)
static const bool MOCK_LED = true;       // LEDs

// ----------------------------
// Teensy GPIO pin mapping
// ----------------------------
static const int PIN_MD1_PG = 2;
static const int PIN_MD2_PG = 3;
static const int PIN_MD2_ALM = 4;
static const int PIN_MA1_FAULT = 5;
static const int PIN_MA2_FAULT = 6;
static const int PIN_MD1_ALM = 7;

static const int PIN_MA1_IN1 = 8;
static const int PIN_MA1_IN2 = 9;
static const int PIN_MA1_IN3 = 10;
static const int PIN_MA2_IN1 = 11;
static const int PIN_MA2_IN2 = 12;
static const int PIN_MA2_IN3 = 13;

static const int PIN_S1_SOIL = 14;
static const int PIN_SA1 = 15;

static const int PIN_E2_SCL = 16;
static const int PIN_E2_SDA = 17;
static const int PIN_E1_SDA = 18;
static const int PIN_E1_SCL = 19;

static const int PIN_SA2 = 20;
static const int PIN_SA3 = 21;

static const int PIN_CAN_CRX = 22;
static const int PIN_CAN_CTX = 23;

static const int PIN_SA4 = 24;
static const int PIN_SA5 = 25;
static const int PIN_SA6 = 26;
static const int PIN_SA7 = 27;

// ----------------------------
// PCA9685 channel mapping
// Channel = Label_Number - 3
// ----------------------------
static const int E1_R3_CH = 0;  // M-D1 SV
static const int E1_R4_CH = 1;  // M-D1 BK
static const int E1_R5_CH = 2;  // M-D1 EN
static const int E1_R6_CH = 3;  // M-D1 F/R
static const int E1_R7_CH = 4;  // M-S8 SIG
static const int E1_R8_CH = 5;  // L-W
static const int E1_R9_CH = 6;  // L-D
static const int E1_R10_CH = 7; // M-A1 EN
static const int E1_R11_CH = 8; // M-D2 F/R
static const int E1_R12_CH = 9; // M-D2 EN
static const int E1_R13_CH = 10; // M-D2 BK
static const int E1_R14_CH = 11; // M-D2 SV
static const int E1_R18_CH = 15; // M-A2 EN

static const int E2_R3_CH = 0;  // M-S1 EN
static const int E2_R4_CH = 1;  // M-S1 PH
static const int E2_R5_CH = 2;  // M-S2 EN
static const int E2_R6_CH = 3;  // M-S2 PH
static const int E2_R7_CH = 4;  // M-S3 EN
static const int E2_R8_CH = 5;  // M-S3 PH
static const int E2_R9_CH = 6;  // M-S4 EN
static const int E2_R10_CH = 7; // M-S4 PH
static const int E2_R13_CH = 10; // M-S7 PH
static const int E2_R14_CH = 11; // M-S7 EN
static const int E2_R15_CH = 12; // M-S6 PH
static const int E2_R16_CH = 13; // M-S6 EN
static const int E2_R17_CH = 14; // M-S5 PH
static const int E2_R18_CH = 15; // M-S5 EN

// PCA9685 I2C addresses
static const int E1_PCA9685_ADDR = 0x40;
static const int E2_PCA9685_ADDR = 0x41;

// ----------------------------
// G-code ID mapping
// ----------------------------
enum class GcodeMotorId : int {
  M1 = 1,
  M2 = 2,
  M3 = 3,
  M4 = 4,
  M5 = 5,
  M6 = 6,
  M7 = 7,
  M8 = 8,
  M9 = 9,
  M10 = 10,
  M11 = 11,
  M12 = 12,
  M13 = 13
};

enum class ComponentMotorId : int {
  MD1 = 1,
  MD2 = 2,
  MD3 = 3,
  MS1 = 4,
  MS2 = 5,
  MS3 = 6,
  MS4 = 7,
  MS5 = 8,
  MS6 = 9,
  MS7 = 10,
  MS8 = 11,
  MA1 = 12,
  MA2 = 13
};

struct MotorIdMapEntry {
  GcodeMotorId gcodeId;
  ComponentMotorId componentId;
};

static const MotorIdMapEntry MOTOR_ID_MAP[] = {
  {GcodeMotorId::M1, ComponentMotorId::MD1},
  {GcodeMotorId::M2, ComponentMotorId::MD2},
  {GcodeMotorId::M3, ComponentMotorId::MD3},
  {GcodeMotorId::M4, ComponentMotorId::MS1},
  {GcodeMotorId::M5, ComponentMotorId::MS2},
  {GcodeMotorId::M6, ComponentMotorId::MS3},
  {GcodeMotorId::M7, ComponentMotorId::MS4},
  {GcodeMotorId::M8, ComponentMotorId::MS5},
  {GcodeMotorId::M9, ComponentMotorId::MS6},
  {GcodeMotorId::M10, ComponentMotorId::MS7},
  {GcodeMotorId::M11, ComponentMotorId::MS8},
  {GcodeMotorId::M12, ComponentMotorId::MA1},
  {GcodeMotorId::M13, ComponentMotorId::MA2}
};

// ----------------------------
// Logging constants
// ----------------------------
enum class LogLevel : int {
  INFO = 0,
  DATA = 1,
  WARN = 2,
  ERROR = 3
};

enum class LogTag : int {
  SYS = 0,
  MOTOR = 1,
  SENSOR = 2,
  CAN = 3
};

#endif  // ROVER_CONFIG_H
