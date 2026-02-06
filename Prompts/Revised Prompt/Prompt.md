# Role
You are a Senior Embedded Systems Architect and Robotics Engineer.
Your task is to design and implement the firmware for a "Rover Science Module" using a Teensy 4.1 microcontroller.

# Project Overview
The system is part of a planetary rover. The Teensy 4.1 acts as the real-time controller, communicating with a Jetson Nano (Host) via USB Serial to control multiple motors and sensors.

# Key Requirement: Advanced Mock System
**You must implement a "Hybrid Mocking System" to allow testing without full hardware.**
Define the mocking configuration in `src/RoverConfig.h` following these rules:
1.  **Operation Modes:** Define constants for `MODE_REAL`, `MODE_FULL_MOCK`, and `MODE_PARTIAL_MOCK`.
2.  **Subsystem Flags:** Define boolean flags for each module (e.g., `MOCK_DRILL`, `MOCK_PUMP`, `MOCK_SENSORS`).
3.  **Behavior Logic:**
    * **Full Mock:** All drivers bypass hardware calls and log actions to Serial. Sensors return synthetic data (e.g., sine wave).
    * **Partial Mock:** Drivers check their specific flag. If `true`, run in Mock mode; if `false`, run on Real Hardware.
    * **Real Mode:** All Mock flags are ignored.

# File Structure & Directory Organization (Strict: PlatformIO Standard)
**Constraint:** Separate "Drivers" (Library) from "Application Logic" (Source).

## 1. Global Configuration (in `src/`)
* **`src/RoverConfig.h`**:
    * **Settings:** Pin definitions, PID gains, Safety limits, Timeouts.
    * **Mock Config:** The Mode selector and Subsystem Mock flags described above.
    * **No Magic Numbers:** All logic files must reference this file.

## 2. Libraries (in `lib/`)
*Implementation of "Basic Hardware Operations". decoupled from system logic.*
* **`lib/MotorDrivers/`**
    * `MotorDrivers.h` / `.cpp`: Define a base class `MotorBase`. Implement:
        * `DrillMotor`, `PumpMotor`, `GimbalMotor` (SimpleFOC), `VerticalMotor` (FlexCAN).
        * **Requirement:** Each class must accept a `bool isMock` parameter in its constructor (or check the global config). If `isMock` is true, methods like `setSpeed()` must print to Serial instead of writing to registers.
* **`lib/SensorDrivers/`**
    * `SensorDrivers.h` / `.cpp`: Implement classes for `SoilSensor`, `BME280_Wrapper`, etc.
    * **Requirement:** If `isMock` is true, `read()` methods must return dummy values (random or patterned) for testing.

## 3. Application Logic (in `src/`)
* **`src/CommandSequencer.h` / `.cpp`**:
    * The "Brain". Manages a command queue and Macros.
    * Breaks down commands into `MotorDriver` calls.
* **`src/SensorRelay.h` / `.cpp`**:
    * Aggregates data from `SensorDrivers`, performs averaging/formatting.

## 4. Threads & Main (in `src/`)
* **`src/Thread_Control.cpp`**: High-priority (1kHz) loop for PID & Sequencer.
* **`src/Thread_Safety.cpp`**: Monitors Heartbeat, Over-current, and Alarms. Triggers **Emergency Stop**.
* **`src/Thread_Comm.cpp`**: Handles Serial parsing (G-Code style) and data reporting.
* **`src/main.cpp`**: Initializes objects based on `RoverConfig.h` Mock settings.

---

# Hardware & Implementation Rules

## 1. Pin Mapping Logic for PCA9685
* **Rule:** In `RoverConfig.h`, map pins using: `Channel = Label_Number - 3`.

## 2. Required Libraries
* **M-D3 (Vertical):** `FlexCAN_T4`
* **M-A1/M-A2 (Gimbal):** `SimpleFOC`
* **E-1/E-2 (Expanders):** `Adafruit_PWMServoDriver`

## 3. Communication Protocol
* **Format:** Text-based G-code style (e.g., `G1 M1 P100 S50`, `M3 P1`, `HB`).

## Reference Data
*(Refer to the attached `pinlayout.md` for the exact Pin and Board Label mapping. Use the information provided in the previous prompt regarding Hardware Map.)*