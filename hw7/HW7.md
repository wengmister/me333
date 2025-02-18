# Homework 7 ME333 - Winter 2025
Zhengyang Kris Weng Submission 02/16/2025

## Chapter 24.
### 24.1.2 Choose R. Wire the circuit as shown in Figure 24.2, except for the connection from the LED to OC1. The LED and phototransistor should be pointing toward each other, with approximately one inch separation, as shown in Figure 24.4.

I'm using a 10k ohm resistor.

### 24.2.1 PWM calculation. You will use Timer3 as the timer base for OC1. You want a 20 kHz PWM on OC1. Timer3 takes the PBCLK as input and uses a prescaler of 1. What should PR3 be?

period = 1s/20khz = 50 us

since we have 48MHz clock,

50 * (1/48MHz) = 2400 ticks

PR3 = 2400 - 1 = 2399

### 24.2.2 PWM program. Write a program that uses your previous result to create a 20 kHz PWM output on OC1 (with no fault pin) using Timer3. Set the duty cycle to 75%. 

See `hw7q2.c` for more details. Also, demo picture `q2multimeter.jpg` and `q2oscilloscope.jpg`.

### 24.3.1 & 2:

See `hw7q3.c` for more details. Also, demo video `q3demo.mov`.

