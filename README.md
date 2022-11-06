# Pico Robotics Board firmware leveraging SPI bridge communication and Distributed Computing Model

## Problem Statement
In robotics there is a need to calculate motor movements, angles, and speeds and then use physical timing to keep things in sync. These timings with any size of computer eats up valuable compute time that would be better spent on higher level functioning. This board solves this problem by using an SPI interface to transmit a custom set of commands similar to gcode.

The full duplex nature of the SPI interface further allows for effecient use of compute on the main or "master" machine. This fire/receive and forget aspect means that the main computer can immediately move on with assurity that the board will handle it from that point forward. 

## Reasoning, Solution and Theory of Operation

The pico and it's RP2040 chip were chosen because of the existing C/C++ sdk offered for the chip. The sdk allows us to have the speed offered by C++ bare metal binary while keeping many layers of abstraction. 
