## Single Examples

motorS1 forward at 50% (normalized):

G1 MS1 S0.5

motorD1 reverse at 25% for 3 seconds:

G1 MD1 S-0.25 T3000

motorA2 forward at 75% for 1 second:

G1 MA2 S0.75 T1000

motorS3 negative direction at 20% (servo pulse left):

G1 MS3 S-0.2

motorS4 motor up at full speed for 2 seconds:

G1 MS4 S1.0 T2000

motorA2 no movement (stop) for 6 seconds:

G1 MA2 S0 T6000

Stop all sequence (especially when theres multiple sequences going on at once and something happens, this just stops all and clears the queue):

STOP

Print sensors:

SENS

Heartbeat ping:

HB

Pause for 1 second:

P1000

## Multi Examples (Each line shell be considerd as a separate sequence)
motor A1 reverse for 1s then pause for 1s then reverse for 1s
motor A2 forward for 1.5s then stop for 0.5 then reverse for 1s
while sending Sensor readings every second
(in parallel)

ex1:
G1 MA1 S-1.0 T1000 P1000 G1 MA1 S-1.0 T1000
G2 MA2 S1 T1500 G2 MA2 S0 T500 G1 MA2 S-1 T1000
SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000

ex2:
G1 MA1 S-1.0 T1000 P1000 G1 MA1 S-1.0 T1000
G2 MA2 S1 T1500 P500 G1 MA2 S-1 T1000
SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000 SENS P1000



motor A1 reverse for 1s then motor A2 forward for 1s
motor S1 40% forward for 0.5s then motor S2 60% forward for 0.6s then motor S3 60% forward for 0.7s
wait 1s then motor S4 at full forward (no time) then wait 0.5s and then motor S5 at full reverse
(yes, all in parallel)

G1 MA1 S-1.0 T1000 G1 MA2 S1.0 T1000
G1 MS1 S0.4 T500 G1 MS2 S0.6 T600 G1 MS3 S0.6 T700 
P1000 G1 MS4 S1 P500 G1 G1 MS5 S-1
