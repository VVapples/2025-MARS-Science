### connections
    Teensy - Jetson nano
        -> Serial Connection

    Jetson nano - Data Center
        -> internet connection via ros2 node network
    
### Componests connected to Teensy
    - Motors
        - M-D1 : BLDC-Motor controlled by BLD-510b ; Used as main motor for the drill ; use drivers own way, probably PWM
        - M-D2 : BLDC-Motor controlled by BLD-510b ; Used as main motor for the drill ; use drivers own way, probably PWM
        - M-D3 : m2006 controlled by CAN connection and C610 controller; Used to bring motor up and down ; use CAN
        - M-S1 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids; uses PH/EN mode ; use drivers own way, probably PWM
        - M-S2 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids; uses PH/EN mode ; use drivers own way, probably PWM
        - M-S3 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids; uses PH/EN mode ; use drivers own way, probably PWM
        - M-S4 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids; uses PH/EN mode ; use drivers own way, probably PWM
        - M-S5 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids; uses PH/EN mode ; use drivers own way, probably PWM
        - M-S6 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids; uses PH/EN mode ; use drivers own way, probably PWM
        - M-S7 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids; uses PH/EN mode ; use drivers own way, probably PWM
        - M-S8 : SG-90HV controlled directly ; Motor to turn C-P ; use PWM write to control
        - M-A1 : BLDC motor controlled by Simple BLDC driver ; controlls X axis on the camera location ; use SimpleFOC
        - M-A2 : BLDC motor controlled by Simple BLDC driver ; controlls Y axis on the camera location ; use SimpleFOC
    - Sensors
        - S-1  : Compacitive Soil Moisture Sensor V1.0
        - S-2  : AE-BME280
        - S-A1 : AS5600 encoder connected to M-S1 to see its rotation
        - S-A2 : AS5600 encoder connected to M-S2 to see its rotation
        - S-A3 : AS5600 encoder connected to M-S3 to see its rotation
        - S-A4 : AS5600 encoder connected to M-S4 to see its rotation
        - S-A5 : AS5600 encoder connected to M-S5 to see its rotation
        - S-A6 : AS5600 encoder connected to M-S6 to see its rotation
        - S-A7 : AS5600 encoder connected to M-S7 to see its rotation
        - S-B1 : Return data from M-D1 ; set of Alarm and Rotation speed data
        - S-B2 : Return data from M-D2 ; set of Alarm and Rotation speed data
    - LEDs
        - L-W  : White LED used to light bright experiment conditions
        - L-D  : UV-LED used to light specific expertiment conditions
    - Extentions
        - E-1  : PCA9685PW_118 PWM extender @ 0x40; For connections read pinlayout.md
        - E-2  : TCA9548APWR I2C encoder @ 0x77; For connections read pinlayout.md
    - Others
        - CAN    : CAN connection module ; uART-CAN
        - HEAT  :   Heater coil ; very simple, just add current to activate
### Components connected to Jetson nano
    - Cameras
        - C-P  : paranoma camera
        - C-S1 : experiment camera; watches experiment in lit conditions ; moved by M-A1 M-A2
        - C-S2 : experiment camera; watches experiment in dark conditions

