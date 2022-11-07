# Pico Robotics Board firmware leveraging SPI bridge communication and Distributed Computing Model

## Problem Statement
In robotics there is a need to calculate motor movements, angles, and speeds and then use physical timing to keep things in sync. These timings with any size of computer eats up valuable compute time that would be better spent on higher level functioning. This board solves this problem by using an SPI interface to transmit a custom set of commands similar to gcode.

The full duplex nature of the SPI interface further allows for effecient use of compute on the main or "master" machine. This fire/receive and forget aspect means that the main computer can immediately move on with assurity that the board will handle it from that point forward. 

## Reasoning, Solution and Theory of Operation

The pico and it's RP2040 chip were chosen because of the existing C/C++ sdk offered for the chip. The sdk allows us to have the speed offered by C++ bare metal binary while keeping many layers of abstraction between implimentation and the chip layer. The Raspberry Pi foundation also offers superb support for their hardware, sdk, and their own OS which is planned to be run on the actual controlling module.

In solution to the problem statement, the firmware receives new commands from the control module, while also transmitting important information regarding its current state. Then the pico manages the necessary peripherals in order to ensure that the transmitted commands are executed properly. 

The theory of Operation is as follows: 
1. Pico is powered on and establishes the necessary structs and classes to maintain the correct state within the system. All pins are initialized at this time. 
2. Communication is initialized from the master device through the spi interface.
3. Need to communicate is acknowledged by the Pico and a new script of commands is created to transmit the state of the pico to the command module. 
4. Bytes are exchanged in full duplex mode between each system. 
5. Pico moves altered servos and motors to their correct locations.

## Project structure

This project is structured into two main folders, structs and modules, along with a main.cpp file that initializes each class described in the module files. "structs" is for classes that act more like json objects storing new data and moving it around the application. currently there are only two of these but as methods change and get updated I'm sure more will be added. 

The "main" file houses the initial startup for each module and also initializes the second core running the read/write layer. Once a new command set is read in the class maintaining the state of device is able to destribute that data to all the appropriate motors and devices. 