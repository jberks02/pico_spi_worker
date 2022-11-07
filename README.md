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

## Connection Mapping

Numberings from here forward are in reference to physical pins not gpio numbers. 

1. Serial Data line for I2C - goes to PCA9685 IC. 
2. Serial Clock line for I2C - goes to PCA9685 IC.
3. GND
4. 1A of L239D IC(1)
5. 2A of L239D IC(1)
6. 3A of L239D IC(1)
7. 4A of L239D IC(1)
8. GND
9. 1A of L239D IC(2)
10. 2A of L239D IC(2)
11. 3A of L239D IC(2)
12. 4A of L239D IC(2)
13. GND
14. Stepper Motor(1) EndStop 1
15. Stepper Motor(1) EndStop 2
16. Stepper Motor(2) EndStop 1
17. Stepper Motor(2) EndStop 2
18. GND
19. This is connected to a generic output with uni directional current at 5V
20. This is connected to a generic output with uni directional current at 5V
21. SPI interface RX - MOSI
22. SPI interface CSn - Chip Select line
23. GND
24. SPI interface SCK - Serial Clock Line
25. SPI interface TX - MISO
26. Generic GPIO input
27. Generic GPIO input
28. GND
29. Generic GPIO input
30. RUN - not planned for use. 
31. Analogue to Digital input 0
32. Analogue to Digital input 1
33. GND
34. Analogue to Digital input 2
35. Analogue to Digital input 3
36. 3V out - rail connects to all logic power circuits found throughout architecture
37. 3v3 enable pin - not planned for use. 
38. GND
39. VSYS - power in
40. VBUS - usb power out
