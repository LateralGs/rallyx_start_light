#RallyCross Start Light

This repository holds the documentation for a starting light circuit developed for use in Pacific Northwest SCCA RallyX events.  The purpose of this project is to provide a way to safely signal competitors that they may commence their run when at the starting line.

Benefits over other methods:
- Usable durring the day or for night racing
- Consistant signaling to all competitors
- Ability to remotely control or override
- Operator can stand away from the car to avoid hazards
- Because it looks cool

This repository focuses on the electronics and firmware used to control the start lights.  Below are some recomendations for light solutions that can be used with this device.

## Functional overview

The device controls as set of red, yellow, and green lights.  When first powered up by connecting power the device cycles through all the light colors to allow the user to verify they all work.  The default state when the input control is not active is to display a solid red light.  When the input control is connected to ground the device begins the start sequence.  The start sequence consists of blinking the yellow light three times at one second intervals before going to a solid green.  The device will keep the light green for 30 seconds before returning to a solid red.  If at any time prior to the sequence completing the operator my deactivate the control input causing the red light to flash repeatedly for a few seconds before returning to a solid red.  The operator must deactivate the light prior to being able to activate it for a subsequent start sequence.

## Specifications

- Power consumption: < 30mA (circuit only)
- Input voltage range: 5v - 12v
- Max light current: 500mA (per channel)
- Input control: switch to ground
- Microcontroller: Arduino Pro Mini (either 3v or 5v)
- Aproximate Size: 65mm x 45mm x 20mm
- Adjustable PWM output

## Light Recomendations

The device will need to be able to control a red, yellow, and green light.  It is recomended to use LED bulbs whenever possible to reduce power consumption.  One solution is to use an industrial signal light tower designed for 12v.

## Interfacing and Control



## Circuit

## Firmware

