#include "sr04.h"
#include <xc.h>

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

// System clock frequency (adjust to your actual clock frequency)
#define SYS_CLOCK_FREQ 16000000 // 16 MHz

// Initialize the pins used by the SR04
void SR04_Startup() {
    ANSELA = 0; // Turn off the analog input functionality that overrides everything else
    ANSELB = 0;
    TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
    TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
    TRIG = 0; // Initialize TRIG to LOW

    // Initialize Timer1 for core timer purposes (example configuration)
    T1CONbits.TCKPS = 0b011; // Prescaler 1:8
    T1CONbits.TON = 1; // Turn on Timer1
}

// Implement a microsecond delay using Timer1
void delay_us(unsigned int delay) {
    // Clear the Timer1 register
    TMR1 = 0;
    // Set the timer period for the given delay in microseconds
    PR1 = (SYS_CLOCK_FREQ / 8 / 1000000) * delay; // (Prescaler * 1/Clock Frequency) * microseconds

    // Wait until the timer has counted up to the desired value
    while (IFS0bits.T1IF == 0); // Wait for Timer1 overflow flag
    IFS0bits.T1IF = 0; // Clear the interrupt flag
}

// Trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout) {
    unsigned int start_time = 0;
    unsigned int end_time = 0;

    // Turn on TRIG
    TRIG = 1;
    // Wait for at least 10us
    delay_us(10);
    // Turn off TRIG
    TRIG = 0;
    
    // Wait until ECHO is HIGH
    while (ECHO == 0 && timeout > 0) {
        timeout--;
    }
    
    // Start counting when ECHO goes HIGH
    start_time = TMR1;  // Use Timer1 as the core timer

    // Wait until ECHO is LOW or timeout occurs
    while (ECHO == 1 && timeout > 0) {
        timeout--;
    }
    
    // Capture the end time
    end_time = TMR1;
    
    // Return the difference in time
    return end_time - start_time;
}

// Read the raw value and convert it to meters
float SR04_read_meters() {
    // Read the raw SR04 value
    unsigned int raw_ticks = SR04_read_raw(10000);  // Timeout after 10000 core ticks (adjust if needed)
    
    // Convert the raw time to distance in meters (Sound velocity is 346 meters per second)
    // Time = (Distance / SoundSpeed) * 2, so Distance = (Time * SoundSpeed) / 2
    // Time is in core ticks, so we need to convert it to seconds by dividing by the clock rate (e.g., 16 MHz)
    
    float time_in_seconds = (float)raw_ticks / SYS_CLOCK_FREQ;  // Convert ticks to seconds
    float distance = (time_in_seconds * 346.0) / 2.0;  // Distance in meters
    
    return distance;
}
