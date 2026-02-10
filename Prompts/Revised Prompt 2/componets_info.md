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
        - M-A1 : BLDC motor controlled by DRI0058 ; controlls X axis on the camera location ; use SimpleFOC
        - M-A2 : BLDC motor controlled by DRI0058 ; controlls Y axis on the camera location ; use SimpleFOC
    - Sensors
        - S-1  : Compacitive Soil Moisture Sensor V1.0
        - S-2  : AE-BME280
        - S-A1 : Ampage signal for M-1S from POLOLU-4035 ; Voltage is devided by halve
        - S-A2 : Ampage signal for M-1S from POLOLU-4035 ; Voltage is devided by halve 
        - S-A3 : Ampage signal for M-1S from POLOLU-4035 ; Voltage is devided by halve 
        - S-A4 : Ampage signal for M-1S from POLOLU-4035 ; Voltage is devided by halve 
        - S-A5 : Ampage signal for M-1S from POLOLU-4035 ; Voltage is devided by halve 
        - S-A6 : Ampage signal for M-1S from POLOLU-4035 ; Voltage is devided by halve
        - S-A7 : Ampage signal for M-1S from POLOLU-4035 ; Voltage is devided by halve
        - S-B1 : Return data from M-D1 ; set of Alarm and Rotation speed data
        - S-B2 : Return data from M-D2 ; set of Alarm and Rotation speed data
    - LEDs
        - L-W  : White LED used to light bright experiment conditions
        - L-D  : UV-LED used to light specific expertiment conditions
    - Extentions
        - E-1  : AE-PCA9685 PWM extender @ 0x40; For connections read pinlayout.md
        - E-2  : AE-PCA9685 PWM extender @ 0x41; For connections read pinlayout.md
    - Others
        - CAN    : CAN connection module ; WAVESHARE-3945

### Components connected to Jetson nano
    - Cameras
        - C-P  : paranoma camera
        - C-S1 : experiment camera; watches experiment in lit conditions ; moved by M-A1 M-A2
        - C-S2 : experiment camera; watches experiment in dark conditions

