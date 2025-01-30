# Homework 3 ME333 - Winter 2025
Zhengyang Kris Weng Submission 01/27/2025

### 3. Describe the four functions that pin 12 of the PIC32MX170F256B can have. Is it 5 V tolerant?
    AN4:

        This indicates that the pin can function as an analog input for the analog-to-digital converter (ADC). Specifically, it is the fourth analog input channel (AN4).

    C1IN-:

        This refers to the negative input for the Comparator 1 module. Comparators are used to compare two analog voltages and provide a digital output based on which voltage is higher.

    CN6:

        This indicates that the pin can be used as an input for the Change Notification feature. Change Notification pins can detect changes in the logic level (high to low or low to high) and generate an interrupt, which is useful for monitoring external events.

    RB4:

        This is the default digital I/O function of the pin. It is part of Port B, and the pin can be configured as a general-purpose input or output (GPIO).

    Also this pin is not 5V tolerant. This pin operates on 3.3V logic level.

### 4. referring to the Data Sheet section on I/O Ports, what is the name of the SFR you have to modify if you want to change pins on PORTC from output to input?

**TRISC.**

    • A TRIS bit set = 1 configures the corresponding I/O port pin as an input.
    • A TRIS bit set = 0 configures the corresponding I/O port pin as an output.
    • A read from a TRIS register reads the last value written to the TRIS register.

From `PIC32MX Family Reference Manual` section 12.2.

### 5. The SFR CM1CON controls comparator behavior. Referring to the Memory Organization section of the Data Sheet, what is the reset value of CM1CON in hexadecimal?

**0x000000C3**

From `PIC32MX Family Reference Manual` section 23.1.

### 6. In one sentence each, without going into detail, explain the basic function of the following items shown in the PIC32 architecture block diagram Figure 2.2: SYSCLK, PBCLK, PORTA to PORTG (and indicate which of these can be used for analog input on  the PIC32MX170F256B ’s PIC32), Timer1 to Timer5, 10-bit ADC, PWM OC1-5, Data RAM, Program Flash Memory, and Prefetch Cache Module.

    SYSCLK: The system clock signal that drives the core and peripherals of the PIC32 microcontroller.

    PBCLK: The peripheral bus clock, derived from SYSCLK, used to clock peripherals at a configurable frequency.

    PORTA to PORTG: Digital I/O ports for interfacing with external devices.

    Timer1 to Timer5: Hardware timers used for counting, timing, and generating precise delays or events.

    10-bit ADC: A 10-bit analog-to-digital converter for converting analog signals to digital values.

    PWM OC1-5: Pulse-width modulation modules for generating variable-duty-cycle signals.

    Data RAM: Volatile memory used for temporary data storage during program execution.

    Program Flash Memory: Non-volatile memory for storing the program code.

    Prefetch Cache Module: Enhances performance by caching instructions from program memory for faster execution.

### 7. List the peripherals that are not clocked by PBCLK.

    USB Module: The USB peripheral typically uses a dedicated clock source, such as the USB PLL or an external crystal, to meet the precise timing requirements of the USB protocol.

    RTCC (Real-Time Clock and Calendar): The RTCC module uses a separate low-frequency oscillator for timekeeping.

    Watchdog Timer (WDT): The Watchdog Timer has its own dedicated low-frequency oscillator to ensure it operates independently of the main clock.

    Primary Oscillator (POSC) and Secondary Oscillator (SOSC): These are clock sources themselves and are not clocked by PBCLK.

    Internal Fast RC Oscillator (FRC): This is an independent clock source used for system startup or as a backup clock.

### 8. If the ADC is measuring values between 0 and 3.3 V, what is the largest voltage difference that it may not be able to detect? (It’s a 10-bit ADC.)

    Resolution = 3.3V / 2^10 = 3.3V / 1024 = 0.00322V

### 9. Refer to the Reference Manual chapter on the Prefetch Cache. What is the maximum size of a program loop, in bytes, that can be completely stored in the cache?

    Instruction Caching
        The 16-line cache supplies an instruction every clock, for loops up to 256 bytes long.

### 10. Explain why the path between flash memory and the prefetch cache module is 128 bits wide instead of 32, 64, or 256 bits.

    A 128-bit wide path allows the microcontroller to fetch four 32-bit instructions (or data words) in a single memory access cycle. This increases the throughput of instruction fetching, reducing the number of memory accesses needed and improving overall execution speed.

### 11. Explain how a digital output could be configured to swing between 0 and 4 V, even though the PIC32 is powered by 3.3 V.

    An external voltage supply would be needed to configure a digital output on a PIC32 microcontroller to swing between 0 V and 4 V. While the PIC32 is powered by 3.3 V, one can use a level shifter circuit, such as a MOSFET-based shifter or a dedicated IC (e.g., TXB0108), converts the 0–3.3 V logic levels from the PIC32 to 0–4 V by using a 4 V supply for the high-side voltage. The level shifter ensures that the output signal matches the desired voltage range without exposing the PIC32 to voltages higher than 3.3 V, protecting the microcontroller while achieving the required output swing.

### 12. PIC32’s have increased their flash and RAM over the years. What is the maximum amount of flash memory a PIC32 can have before the current choice of base addresses in the physical memory map (for RAM, flash, peripherals, and boot flash) would have to be changed? What is the maximum amount of RAM? Give your answers in bytes in hexadecimal.

    Maximum Flash Memory: 0x200000 bytes (2 MB) before the memory map needs to be changed.

    Maximum RAM: 0x80000 bytes (512 KB) before the memory map needs to be changed.

### 13. Examine the Special Features section of the Data Sheet.
#### a. If you want your PBCLK frequency to be half the frequency of SYSCLK, which bits of which Device Configuration Register do you have to modify? What values do you give those bits?

    for REGISTER 27-2: DEVCFG1 bit 13-12:
    FPBDIV<1:0>: Peripheral Bus Clock Divisor Default Value bits
    11 = PBCLK is SYSCLK divided by 8
    10 = PBCLK is SYSCLK divided by 4
    01 = PBCLK is SYSCLK divided by 2
    00 = PBCLK is SYSCLK divided by 1

    to get PBCLK = SYSCLK / 2, we set FPBDIV = 0b01.

#### b. Which bit(s) of which SFR set the watchdog timer to be enabled? Which bit(s) set the postscale that determines the time interval during which the watchdog must be reset to prevent it from restarting the PIC32? What values would you give these bits to enable the watchdog and to set the time interval to be the maximum?

    bit 23 FWDTEN: Watchdog Timer Enable bit
        1 = Watchdog Timer is enabled and cannot be disabled by software
        0 = Watchdog Timer is not enabled; it can be enabled in software

    bit 20-16 WDTPS<4:0>: Watchdog Timer Postscale Select bits
        10100 = 1:1048576
        10011 = 1:524288
        10010 = 1:262144
        10001 = 1:131072
        10000 = 1:65536
        01111 = 1:32768
        01110 = 1:16384
        01101 = 1:8192
        01100 = 1:4096
        01011 = 1:2048
        01010 = 1:1024
        01001 = 1:512
        01000 = 1:256
        00111 = 1:128
        00110 = 1:64
        00101 = 1:32
        00100 = 1:16
        00011 = 1:8
        00010 = 1:4
        00001 = 1:2
        00000 = 1:1
        
    A WTDTPS<4:0> = 10100 would set the time interval to maximum.

#### C. The SYSCLK for a PIC32 can be generated several ways, as discussed in the Oscillator chapter in the Reference Manual and the Oscillator Configuration section in the Data Sheet. The PIC32 on the PIC32MX170F256B  uses the (external) primary oscillator in HS mode with the phase-locked loop (PLL) module. Which bits of which device configuration register enable the primary oscillator and turn on the PLL module?

    Again, in DEVCFG1, 
    
    bit 2-0 FNOSC<2:0>: Oscillator Selection bits we have:
    111 = Fast RC Oscillator with divide-by-N (FRCDIV)
    110 = FRCDIV16 Fast RC Oscillator with fixed divide-by-16 postscaler
    101 = Low-Power RC Oscillator (LPRC)
    100 = Secondary Oscillator (SOSC)
    011 = Primary Oscillator (P OSC ) with PLL module (XT+PLL, HS+PLL, EC+PLL)
    010 = Primary Oscillator (XT, HS, EC)(1)
    001 = Fast RC Oscillator with divide-by-N with PLL module (FRCDIV+PLL)
    000 = Fast RC Oscillator (FRC)

    bit 9-8 POSCMOD<1:0>: Primary Oscillator Configuration bits
    11 = Primary Oscillator is disabled
    10 = HS Oscillator mode is selected
    01 = XT Oscillator mode is selected
    00 = External Clock mode is selected

    To select primary oscillator with PLL module, in register DEVCFG1, bit FNOSC = 0b011, and POSCMOD = 0b10.

### 14. Your PIC32MX170F256B board provides four power rails: GND, regulated 3.3 V, regulated 5 V, and the unregulated input voltage (e.g., 6 V). You plan to put a load from the 5 V output to ground. If the load is modeled as a resistor, what is the smallest resistance that would be safe? An approximate answer is fine. In a sentence, explain how you arrived at the answer.

    For the PIC32MX170F256B board, the 5 V regulator is typically rated for a maximum current of 500 mA (0.5 A). For a 5V regulated output, and a maximum current of 0.5A, the smallest safe resistance can be calculated using ohm's law: 5V/0.5A = **10 ohm**.

### 15. The PIC32MX170F256B  could be powered by different voltages. Give a reasonable range of voltages that could be used, minimum to maximum, and explain the reason for the limits.

    Reasonable Voltage Range:
    Minimum Voltage: 3.0 V - the PIC32MX170F256B is designed to operate with a core voltage (VDD) of 3.0 V to 3.6 V.
    Maximum Voltage: 3.6 V (for direct VDD supply) or up to 5.5 V (if using an onboard regulator). If using an onboard voltage regulator (e.g., a 3.3 V regulator), the input voltage to the regulator can be higher, typically up to 5.5 V. The regulator steps this down to the required 3.3 V for the microcontroller.

### 16. Two buttons and two LEDs are interfaced to the PIC32 on the PIC32MX170F256B. Which pins are they connected to? Give the actual pin numbers, 1-64, as well as the name of the pin function as it is used on the PIC32MX170F256B . For example, pin 37 on the PIC32MX170F256B could have the function D+ (USB data line) or RG2 (Port G digital input/output), but only one of these functions could be active at a given time.

    Green LED is interfaced to pin 11:
    pin 11 has SOSCI (Secondary Oscillator Input.), RPB4 (Remappable Peripheral Input B4.), OR RB4 (General Purpose I/O (GPIO) Port B, Bit 4.)
    Since it's wired to a LED, it's only using the GPIO functionality.

    Yello LED is interfaced to pin 14:
    pin14 has PGED3 (Programming Data (Debugging/Programming Pin)), RPB5 (Remappable Peripheral Input B5), PMD7 (Parallel Master Data 7), and RB5 (General Purpose I/O (GPIO) Port B, Bit 5)
    This is also using the GPIO functionality

    User button is wired to pin 12:
    pin 12 has SOSCO (Secondary Oscillator Output), RPA4 (Remappable Peripheral Input A4), T1CK (Timer1 External Clock Input), CTED9 (Comparator Timer External Detect 9), PMA1 (Parallel Master Address 1), and RA4 (General Purpose I/O (GPIO) Port A, Bit 4):
    Since it's used as a button to trigger bootloader mode, this should use the GPIO functionality.

    Reset button is wired to pin 1, this is has the sole function of MCLR, master clear reset.
    When the MCLR pin is pulled low (to ground), it forces the microcontroller to reset, restarting program execution from the beginning.