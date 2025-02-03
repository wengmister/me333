#include <xc.h>          // Load the proper header for the processor

#define MAXCYCLES   1000000   // Initial delay cycle count
#define DELTACYCLES 100000    // Amount to decrement the delay each time

void delay(int cycles);

int main(void) {
    TRISBCLR = 0x30;
    LATBbits.LATB4 = 0;
    LATBbits.LATB5 = 1;

    int cycles = MAXCYCLES; // Initialize cycles to MAXCYCLES

    while(1) {
        delay(cycles);              // Delay for 'cycles' iterations
        LATBINV = 0b110000;         // Toggle GREEN and YELLOW LEDs

        cycles -= DELTACYCLES;       // Decrease the delay
        if (cycles < 0) {           // If cycles is less than zero,
            cycles = MAXCYCLES;     // reset it to MAXCYCLES.
        }
    }
    
    return 0;
}

void delay(int cycles) {
    int j;
    for (j = 0; j < cycles; j++) {
        // The inner loop waits while the USER switch (PORTA RA4) is not pressed.
        // When RA4 is low (FALSE), the loop stalls here.
        while (!PORTAbits.RA4) {
            ;   // Do nothing.
        }
    }
}
