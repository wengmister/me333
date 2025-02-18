#include "nu32dip.h"

#define NUMSAMPS 1000 // number of points in waveform
static volatile int Waveform[NUMSAMPS]; // waveform

void makeWaveform() {
    int i = 0, center = (2399 + 1) / 2, A = (2399 + 1) / 4; // center and amplitude
    for (i = 0; i < NUMSAMPS; ++i) {
        if (i < NUMSAMPS/2) {
            Waveform[i] = center + A; // High part of the square wave
        } else {
            Waveform[i] = center - A; // Low part of the square wave
        }
    }
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {
    static int counter = 0;    // Static counter for ISR entries

    OC1RS = Waveform[counter]; // Set PWM duty cycle from waveform

    counter++;                 // Increment counter
    if (counter >= NUMSAMPS) { // Reset counter after 1000 samples
        counter = 0;
    }

    IFS0bits.T2IF = 0;         // Clear Timer2 interrupt flag
}

int main(void)
{
    NU32DIP_Startup();
    makeWaveform();            // Create the square waveform

    ANSELA = 0;                // Turn off analog inputs
    RPA0Rbits.RPA0R = 0b0101;  // Map OC1 to pin RPA0 

    // Configure Timer3 for PWM at 20kHz
    T3CONbits.TCKPS = 0;       // Prescaler = 1
    PR3 = NU32DIP_SYS_FREQ/20000 - 1;                // 20kHz period
    TMR3 = 0;
    OC1CONbits.OCTSEL = 1;     // Timer3 as the clock source for OC1
    OC1CONbits.OCM = 0b110;    // PWM mode without fault
    OC1R = 1200;                // Initial duty cycle
    OC1RS = 1200;               
    T3CONbits.ON = 1;          // Turn on Timer3
    OC1CONbits.ON = 1;         // Turn on OC1

    // Configure Timer2 for 1 kHz ISR
    T2CONbits.TCKPS = 0b111;   // Prescaler = 256
    PR2 = NU32DIP_SYS_FREQ/(256 * 1000) - 1;                  // 1kHz at PBCLK=48MHz (48,000,000 / 256 / 1000 - 1)
    TMR2 = 0;
    IPC2bits.T2IP = 5;         // Interrupt priority 5
    IPC2bits.T2IS = 0;         // Subpriority 0
    IFS0bits.T2IF = 0;         // Clear interrupt flag
    IEC0bits.T2IE = 1;         // Enable interrupt
    T2CONbits.ON = 1;          // Turn on Timer2

    __builtin_enable_interrupts(); // Enable global interrupts

    while (1) {
        // Main loop is empty; PWM handled by ISR
    }

    return 0;
}
