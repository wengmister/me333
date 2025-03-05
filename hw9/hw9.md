# Homework 9 ME333 - Winter 2025
Zhengyang Kris Weng Submission 03/05/2025

## Chapter 28
### 28.4.1.1 The ~~NU32~~pico2 communicates with the encoder counter by an SPI channel. Which SPI channel will you use? Which ~~NU32~~ pico 2 pins does it use?
I'm using GP14 and GP15 on pico 2 to read the phases from the encoder. PIC then talks to the Pico 2 through UART 2.

### 28.4.1.2 The NU32 reads the MAX9918 current sensor using an ADC input. Which ADC input will you use? Which NU32 pin is it?
