# Teensy Control - G-Code Interface

This folder contains the command parser and sequencer that translate simple G-code-like lines sent over Serial into motor actions.

## How G-code is translated

Incoming lines are read from Serial and parsed in Thread_Comm. The supported commands are:

- HB: heartbeat; updates the safety watchdog and replies with OK.
- STOP: stops all motors; replies with OK.
- SENS: prints a sensor snapshot line (SOIL, TEMP, HUM, PRES, TS).
- G1: sets a motor speed with optional duration.

### G1 format

G1 M<motor_index> S<speed> [T<duration_ms>]

- M: motor index (1=DRILL, 2=PUMP, 3=GIMBAL, 4=VERTICAL).
- S: speed as a float in range -1.0 to 1.0. If you send -100..100, it will be auto-scaled to -1.0..1.0.
- T: optional duration in milliseconds. If omitted or 0, the command runs until another command changes it.

The parser converts a valid G1 into a CommandSequencer::Command and enqueues it. The sequencer executes commands in order, and if T is provided it stops the motor after the duration.

## Examples

Basic drill forward at 50% (normalized):

G1 M1 S0.5

Drill reverse at 25% for 3 seconds:

G1 M1 S-0.25 T3000

Pump forward at 75% for 1 second:

G1 M2 S0.75 T1000

Gimbal negative direction at 20% (servo pulse left):

G1 M3 S-0.2

Vertical motor up at full speed for 2 seconds:

G1 M4 S1.0 T2000

Stop all motors:

STOP

Print sensors:

SENS

Heartbeat ping:

HB

## Notes

- Speed values outside -1..1 are treated as percent and divided by 100 (for example, S50 becomes 0.5).
- Commands are queued (size 16). If the queue is full, the parser replies with ERR QUEUE_FULL.
