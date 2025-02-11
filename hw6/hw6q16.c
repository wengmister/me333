#include "nu32dip.h" // Constants and functions for startup and UART

#define DEBOUNCE_TIME 800000   // 20ms debounce delay 
#define LED_ON_TIME   10000000 // LED on time: 0.25 s

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void)
{
    // Disable INT0 interrupts so that bouncing doesn't retrigger the ISR.
    IEC0bits.INT0IE = 0;
    
    // Wait for a short period to allow the mechanical contacts to settle.
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < DEBOUNCE_TIME) { ; }
    
    // Check if the button is still pressed.
    if (PORTBbits.RB7 == 1) {
        // If the button has been released, it was likely a bounce.
        IFS0bits.INT0IF = 0;  // Clear the interrupt flag.
        IEC0bits.INT0IE = 1;  // Re-enable INT0 interrupts.
        return;
    }
    
    // If we get here, we have a valid button press.
    NU32DIP_GREEN = 0;  // Turn on LED (active low)
    NU32DIP_YELLOW = 0; // Turn on LED (active low)

    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < LED_ON_TIME) { ; } // Keep LEDs on for 0.25 s

    NU32DIP_GREEN = 1;  // Turn off LED
    NU32DIP_YELLOW = 1; // Turn off LED

    // Clear the INT0 interrupt flag and re-enable the interrupt.
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 1;
}

int main(void)
{
    NU32DIP_Startup();                // Cache on, minimum flash wait states, interrupts on, LED/button init, UART init
    __builtin_disable_interrupts();   // Disable interrupts during setup
    
    // Configure external interrupt INT0 for falling edge triggering.
    INTCONbits.INT0EP = 0;      // Falling edge trigger (button pressed)
    IPC0bits.INT0IP = 2;        // Set INT0 priority to 2
    IPC0bits.INT0IS = 1;        // Set INT0 subpriority to 1
    IFS0bits.INT0IF = 0;        // Clear the INT0 interrupt flag
    IEC0bits.INT0IE = 1;        // Enable INT0 interrupts
    
    __builtin_enable_interrupts(); // Enable interrupts

    // Connect RD7 (USER button) to INT0 (D0) on your board.
    while (1) {
        ; // Main loop does nothing – all action is in the ISR.
    }
    return 0;
}
