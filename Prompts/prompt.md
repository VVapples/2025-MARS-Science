# Prompt

## Basic information

    This project is about a module on a rover.
    The objective is to recieve orders form a remote control location and conduct them accordingly.
    The tasks include:
    - Making a Paranoma image
    - collecting locational infromation via sensors
    - collecting dirt samples
    - conducting a chemical analysis of the dirt samples
    - capturing images of the analysis
    - sending the data to the remote control location

## System Structure

    This system will contain 3 main parts:
    - The Teensy
    - The Jetson nano
    - The data center (A computer)
    
### Tasks for each parts
    - The Teensy
        -> Controlling motors
        -> Collecting sensor data from sensors
        -> send collected data to teensy
        -> log each ang every action/data
        -> change modes from commands from the data center(Relayed to Teensy via Jetson nano)
    - The Jetson nano
        -> Recieve commands from data center
        -> Relay commands to teensy
        -> Recieve data from teensy
        -> Send data to data center
        -> Control Cameras
        -> Log each ang every action/data
    - The data center (A computer)
        -> Recieve data from jetson nano
        -> Log each ang every action/data
        -> Send commands to jetson nano
        -> GUI to control the rover/to send commands/show data

### connections
    Teensy - Jetson nano
        -> Serial Connection

    Jetson nano - Data Center
        -> internet connection via ros2 node network
    
### Componests connected to Teensy
    - Motors
        - M-D1 : BLDC-Motor controlled by BLD-510b ; Used as main motor for the drill
        - M-D2 : BLDC-Motor controlled by BLD-510b ; Used as main motor for the drill
        - M-D3 : m2006 controlled by CAN connection and C610 controller; Used to bring motor up and down
        - M-S1 : DC-Motor controlled by POLOLU-4035 motor controller; A pump to controll liquids
        - M-S2 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids
        - M-S3 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids
        - M-S4 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids
        - M-S5 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids
        - M-S6 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids
        - M-S7 : DC-Motor controlled by POLOLU-4035 motor controller ; A pump to controll liquids
        - M-S8 : SG-90HV controlled directly ; Motor to turn C-P
        - M-A1 : BLDC motor controlled by DRI0058 ; controlls X axis on the camera location
        - M-A2 : BLDC motor controlled by DRI0058 ; controlls Y axis on the camera location
    - Sensors
        - S-1  : Compacitive Soil Moisture Sensor V1.0
        - S-2  : AE-BME280
        - S-A1 : Ampage signal for M-1S from POLOLU-4035 / Voltage is devided by halve
        - S-A2 : Ampage signal for M-1S from POLOLU-4035 / Voltage is devided by halve
        - S-A3 : Ampage signal for M-1S from POLOLU-4035 / Voltage is devided by halve
        - S-A4 : Ampage signal for M-1S from POLOLU-4035 / Voltage is devided by halve
        - S-A5 : Ampage signal for M-1S from POLOLU-4035 / Voltage is devided by halve
        - S-A6 : Ampage signal for M-1S from POLOLU-4035 / Voltage is devided by halve
        - S-A7 : Ampage signal for M-1S from POLOLU-4035 / Voltage is devided by halve
        - S-B1 : Return data from M-D1 / set of Alarm and Rotation speed data
        - S-B1 : Return data from M-D2 / set of Alarm and Rotation speed data
    - LEDs
        - L-W  : White LED used to light bright experiment conditions
        - L-D  : UV-LED used to light specific expertiment conditions
    - Extentions
        - E-1  : AE-PCA9685 PWM extender @ 0x40/ For connections read pinlayout.md
        - E-2  : AE-PCA9685 PWM extender @ 0x41/ For connections read pinlayout.md
    - Others
        - CAN    : CAN connection module / WAVESHARE-3945

### Components connected to Jetson nano
    - Cameras
        - C-P  : paranoma camera
        - C-S1 : experiment camera; watches experiment in lit conditions / moved by M-A1 M-A2
        - C-S2 : experiment camera; watches experiment in dark conditions

