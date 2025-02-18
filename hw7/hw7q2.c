#include "nu32dip.h"

int main(void)
{
    NU32DIP_Startup();
    // 0. Configure OC1
    ANSELA = 0; // Turn off analog inputs
    RPA0Rbits.RPA0R = 0b0101; // Map OC1 to pin RPA0 

    // 1. Configure Timer3
    T3CONbits.TCKPS = 0;    // Prescaler = 1
    PR3   = NU32DIP_SYS_FREQ / 20000 - 1;           // For a 20 kHz PWM at PBCLK=48 MHz, prescaler=1 // 2399;
    TMR3  = 0;              // Reset Timer3 to 0            

    // 2. Configure OC1 in PWM mode
    OC1CONbits.OCM = 0b110;  // PWM mode without fault
    OC1R   = 1800;            // Initial duty cycle (50% of 2399)
    OC1RS  = 1800;            // Secondary duty cycle register

    // 3. Start Timer3 and OC1
    T3CONbits.ON    = 1;  // Turn on Timer3
    OC1CONbits.ON = 1;    // Turn on OC1
    OC1CONbits.OCTSEL = 1;  // Select Timer3 as the clock source for OC1


    // 4. Main loop
    while (1) 
    {
        // Adjust OC1RS here for different duty cycles if needed
    }
    
    return 0;
}
