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
    -
