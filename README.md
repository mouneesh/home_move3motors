# Stepper Motor Control with Homing Functionality

## Overview
This Arduino program controls three stepper motors with the AccelStepper library. Each motor is equipped with a hall sensor for homing purposes. The program reads commands from the serial input to rotate the motors either clockwise (CW) or counterclockwise (CCW) by a specified number of steps. It also handles the homing sequence to ensure that the motors start from a known position.

## Features
- Homing functionality using hall sensors for all three motors
- Control each motor's movement with serial commands (CW/CCW)
- Supports interrupt-driven hall sensor detection for precise homing
- Configurable steps per revolution for each motor

## Hardware Requirements
- Arduino Mega (or similar)
- Three stepper motors
- Three hall effect sensors
- Stepper motor drivers
- External power supply for the motors (if required)

## Pin Connections
- **Motor 1:**
  - Direction pin: `30`
  - Step pin: `32`
  - Enable pin: `28`
  - Hall sensor: `18`
- **Motor 2:**
  - Direction pin: `40`
  - Step pin: `36`
  - Enable pin: `38`
  - Hall sensor: `21`
- **Motor 3:**
  - Direction pin: `11`
  - Step pin: `12`
  - Enable pin: `10`
  - Hall sensor: `3`

## How to Use
1. Connect the motors and hall sensors to the specified pins.
2. Upload the code to the Arduino.
3. Open the Serial Monitor (baud rate `9600`).
4. Enter commands to control the motors in the following format:
For example:
- `1,CW,1000` will move Motor 1 clockwise by 1000 steps.
- `2,CCW,500` will move Motor 2 counterclockwise by 500 steps.

## Homing Process
Upon startup, each motor will move to its home position using the attached hall sensor. Once all motors reach their home positions, the program will print "Homing Completed" and the motors are ready for movement commands.

## Dependencies
- [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/): Library to control stepper motors

## Future Improvements
- Add error handling for invalid commands or motor numbers.
- Implement feedback for real-time motor position tracking.
