#include "nu32dip.h"
#include <stdio.h>

// Define a debounce delay for the CP0 timer (20 ms)
#define DEBOUNCE_TIME 800000   // (20 ms at 40 MHz core clock)
// Define a debounce delay loop constant for the software delay (adjust as needed)
#define DEBOUNCE_LOOP 50000

// Define states for the stopwatch.
#define WAITING 0
#define TIMING  1

// Global state variable: 0 = waiting to start; 1 = timing.
volatile int stopwatchState = WAITING;

// A simple software delay for debouncing that does not affect CP0 timer.
void delay_loop(void) {
    volatile int i;
    for(i = 0; i < DEBOUNCE_LOOP; i++) {
        ; // Do nothing: simple delay loop.
    }
}

//
// INT0 ISR: Triggered by the USER button (wired to INT0).
// Using IPL6AUTO so the compiler chooses a valid context-saving mode.
//
void __ISR(_EXTERNAL_0_VECTOR, IPL6AUTO) INT0ISR(void) {
    // Disable further INT0 interrupts while debouncing.
    IEC0bits.INT0IE = 0;
    
    if (stopwatchState == WAITING) {
        // --- Starting the stopwatch ---
        
        // Use the CP0 timer for debounce delay.
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < DEBOUNCE_TIME) { ; }
        
        // Check that the button is still pressed.
        if (PORTBbits.RB7 != 0) {
            IFS0bits.INT0IF = 0;
            IEC0bits.INT0IE = 1;
            return;
        }
        
        // Reset the CP0 timer to start the timing.
        _CP0_SET_COUNT(0);
        stopwatchState = TIMING;
        NU32DIP_WriteUART1("Press the USER button again to stop the timer.\r\n");
    } 
    else {  
        // --- Stopping the stopwatch ---
        // Use a software delay loop for debouncing, so we don't reset CP0.
        delay_loop();
        
        // Check that the button is still pressed.
        if (PORTBbits.RB7 != 0) {
            IFS0bits.INT0IF = 0;
            IEC0bits.INT0IE = 1;
            return;
        }
        
        // Read the CP0 timer which has been counting since the timer started.
        unsigned int t = _CP0_GET_COUNT();
        stopwatchState = WAITING;
        
        // Compute elapsed time in seconds. (40e6 core ticks per second)
        double seconds = t / 24000000.0;
        char msg[100];
        sprintf(msg, "%.3f seconds elapsed\r\n", seconds);
        NU32DIP_WriteUART1(msg);
        NU32DIP_WriteUART1("Press the USER button to start the timer.\r\n");
    }
    
    // Clear the INT0 interrupt flag and re-enable INT0 interrupts.
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 1;
}

//
// main: Initializes the board, configures INT0, and prints the initial message.
//
int main(void) {
    NU32DIP_Startup();  // Initialize cache, flash wait states, interrupts, LED/button, and UART
    
    // Configure INT0: falling-edge trigger, priority 6.
    __builtin_disable_interrupts();
    INTCONbits.INT0EP = 0;     // Falling edge trigger.
    IPC0bits.INT0IP = 6;       // Priority level 6.
    IFS0bits.INT0IF = 0;       // Clear the INT0 flag.
    IEC0bits.INT0IE = 1;       // Enable INT0.
    __builtin_enable_interrupts();
    
    // Print the initial message.
    NU32DIP_WriteUART1("Press the USER button to start the timer.\r\n");
    
    // Loop forever. All work is done in the ISR.
    while (1) {
        ; // Do nothing.
    }
    
    return 0;
}
