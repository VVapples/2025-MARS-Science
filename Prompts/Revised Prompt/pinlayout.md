This is the pin layout reference.
For Teensy, All pins are written in GPIO numbers.
For Extention modules (E-1, E-2), pins are written in Labels on the board (R3-R18).

# PINS TEENSY
2   M-D1 PG pin
3   M-D2 PG pin
4   M-D2 ALM pin
5   M-A1 fault pin
6   M-A2 fault pin
7   M-D1 ALM pin
8   M-A1 IN1 pin (PWM)
9   M-A1 IN2 pin (PWM)
10  M-A1 IN3 pin (PWM)
11  M-A2 IN1 pin (PWM)
12  M-A2 IN2 pin (PWM)
13  M-A2 IN3 pin (PWM)
14  S-1 Signal pin
15  S-A1 output signal
16  E-2 SCL pin (Wire1)
17  E-2 SDA pin (Wire1)
18  E-1 SDA pin (Wire0)
19  E-1 SCL pin (Wire0)
20  S-A2 output signal
21  S-A3 output signal
22  CAN CRX (FlexCAN)
23  CAN CTX (FlexCAN)
24  S-A4 output signal
25  S-A5 output signal
26  S-A6 output signal
27  S-A7 output signal

# PINS E-1 (PCA9685 @ 0x40 via Wire0)
R3  M-D1 SV pin
R4  M-D1 BK pin
R5  M-D1 EN pin
R6  M-D1 F/R pin
R7  M-S8 SIG pin
R8  L-W
R9  L-D
R10 M-A1 EN pin
R11 M-D2 F/R pin
R12 M-D2 EN pin
R13 M-D2 BK pin
R14 M-D2 SV pin
R18 M-A2 EN pin

# PINS E-2 (PCA9685 @ 0x41 via Wire1)
R3  M-S1 EN pin
R4  M-S1 PH pin
R5  M-S2 EN pin
R6  M-S2 PH pin
R7  M-S3 EN pin
R8  M-S3 PH pin
R9  M-S4 EN pin
R10 M-S4 PH pin
R13 M-S7 PH pin
R14 M-S7 EN pin
R15 M-S6 PH pin
R16 M-S6 EN pin
R17 M-S5 PH pin
R18 M-S5 EN pin