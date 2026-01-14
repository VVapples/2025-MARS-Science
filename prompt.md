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
    - sending the data back to the remote control location

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
        - M-D2 : m2006 controlled by ___ ; Used to bring motor up and down
        - M-S1 : DC-Motor controlled by ___ ; A pump to controll liquids
        - M-S2 : DC-Motor controlled by ___ ; A pump to controll liquids
        - M-S3 : DC-Motor controlled by ___ ; A pump to controll liquids
        - M-S4 : DC-Motor controlled by ___ ; A pump to controll liquids
        - M-S5 : DC-Motor controlled by ___ ; A pump to controll liquids
        - M-S6 : DC-Motor controlled by ___ ; A pump to controll liquids
        - M-S7 : DC-Motor controlled by ___ ; A pump to controll liquids
        - M-S8 : SG-90HV controlled directly ; Motor to turn C-P
    - Sensors
        - S-1 : Compacitive Soil Moisture Sensor V1.0
        - S-2 : AE-BME280

### Components connected to Jetson nano
    - Cameras
        - C-P : paranoma camera
        - C-S1 : experiment camera; watches experiment in lit conditions
        - C-S2 : experiment camera; watches experiment in dark conditions

### Teensy Pin layout
    - S-1 : directly to teensy/analog/GPIO14
    - S-2 : directly to teensy/i2c0/0x76
