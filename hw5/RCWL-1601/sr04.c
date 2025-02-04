#include "sr04.h"
#include "nu32dip.h"
#include <xc.h>

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

// System clock frequency (adjust to your actual clock frequency)

// Initialize the pins used by the SR04
void SR04_Startup() {
    ANSELA = 0; // Turn off the analog input functionality that overrides everything else
    ANSELB = 0;
    TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
    TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
    TRIG = 0; // Initialize TRIG to LOW
}

// Trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout) {
    unsigned int start_count = 0;
    unsigned int end_count = 0;

    // Turn on TRIG
    TRIG = 1;
    _CP0_SET_COUNT(0);

    NU32DIP_WriteUART1("TRIG ON\r\n");
    // Wait for 10 us
    while(_CP0_GET_COUNT() < 240) {
        ;  // Do nothing
    }
    // Turn off TRIG
    TRIG = 0;
    NU32DIP_WriteUART1("TRIG OFF\r\n");
    
    // Wait until ECHO is HIGH
    while (ECHO == 0) {
        ;  // Do nothing
    }
    NU32DIP_WriteUART1("ECHO HIGH\r\n");
    
    // Start counting when ECHO goes HIGH
    start_count = _CP0_GET_COUNT();  // Use Timer1 as the core timer

    // Wait until ECHO is LOW or timeout occurs
    while ((ECHO == 1) && (_CP0_GET_COUNT() < timeout)) {
        ;
    }

    NU32DIP_WriteUART1("ECHO LOW\r\n");
    
    // Capture the end time
    end_count = _CP0_GET_COUNT();
    
    // Return the difference in time
    return end_count - start_count;
}

// Read the raw value and convert it to meters
float SR04_read_meters() {
    // Read the raw SR04 value
    unsigned int raw_ticks = SR04_read_raw(24000000/4);  // Timeout after 1/4 seconds
    
    float time_in_seconds = (float)raw_ticks / (NU32DIP_SYS_FREQ / 2);  // Convert ticks to seconds
    float distance = (time_in_seconds * 346.0) / 2.0;  // Distance in meters
    
    return distance;
}
