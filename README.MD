# Bodenseehaie - Microcontroller Software
Main software for our soccer robots (open league) for RoboCup-Junior. 
With this code we won the RoboCup 2016. 

The code is developed as an Atmel-Studio Project (but should compile with GCC as well) and is intended to be run on an Atmel/Microchip-AtXMega64A1.

## Fuses
In Atmel-Studio: Tools->Device Programming

**FF;00;FF;FF;F2**

## LEDs
LED 1: Watchdog
	
LED 2: Brownout
	
LED 1 and 2: Software Reset
	
LED 3: On Line or out of bounds
	
LED 4: Captured Ball
	
LED 5: -
	
LED 6: Voltage warning
	
LED 7: IMU is starting
	
LED 8: IMU fully calibrated

## Switch
Switch 1: Motors

Switch 2 und 4: Spielprogramm:

| Switch 2 | Switch 4  | Mode |
|:----------:|:-----------:|---------------------------------------------|
| 0          | 0           | Normal mode | 
| 1          | 0           | Mode with special tricks |
| 0          | 1           | Goalie |
| 1          | 1           | PID and line test |

Switch 3: Display output enable (if motors are off USB is enabled as well)

Switch 5: Activ/Passiv

Switch 6: Line calibration
