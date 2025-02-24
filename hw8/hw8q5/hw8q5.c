#include "nu32dip.h"

#define NUMSAMPS 1000 // number of points in waveform
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point
static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData = 0; // if this flag = 1, currently storing

#define MAXPLOTPTS 1000
#define SAMPLE_TIME 6 // 24MHz*250ns
void ADC_Startup();
unsigned int adc_sample_convert(int);
static volatile int eint = 0;
// plot data
static volatile float Kp = 0, Ki = 0, eintmax = 1000; // control gains

void makeWaveform() {
    int i = 0, center = 500, A = 300; // center and amplitude
    for (i = 0; i < NUMSAMPS; ++i) {
        if (i < NUMSAMPS/2) {
            Waveform[i] = center + A; // High part of the square wave
        } else {
            Waveform[i] = center - A; // Low part of the square wave
        }
    }
}

void ADC_Startup(){
  ANSELAbits.ANSA1 = 1; // AN1 is an adc pin
  AD1CON3bits.ADCS = 1; // ADC clock period is Tad = 2*(ADCS+1)*Tpb =2*2*(1/48000000Hz) = 83ns > 75ns
  AD1CON1bits.ADON = 1;
}

unsigned int adc_sample_convert(int pin)
{
  unsigned int elapsed = 0, finish_time = 0;
  AD1CHSbits.CH0SA = pin;
  AD1CON1bits.SAMP = 1;
  elapsed = _CP0_GET_COUNT();
  finish_time = elapsed + SAMPLE_TIME;
  while (_CP0_GET_COUNT() < finish_time)
  {
    ;
  }
  AD1CON1bits.SAMP = 0;
  while (!AD1CON1bits.DONE)
  {
    ;
  }
  return ADC1BUF0;
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {
    static int counter = 0; // initialize counter once
    static int plotind = 0; // index for data arrays; counts up to PLOTPTS
    static int decctr = 0; // counts to store data one every DECIMATION
    static int adcval = 0; //
    
    adcval = adc_sample_convert(1); // sample the ADC

    float e = Waveform[counter] - adcval; // calculate the error
    
    eint = eint + e; // update the error integral
    if (eint > eintmax) {
        eint = eintmax;
    } else if (eint < -eintmax) {
        eint = -eintmax;
    }
    float u = Kp * e + Ki * eint; // calculate the control effort

    float unew = u + 50.0;
    if (unew > 100.0) {
       unew = 100.0;
    } else if (unew < 0.0) {
       unew = 0.0;
    }

    OC1RS = (unsigned int) ((unew/100.0) * PR3);


    if (StoringData) {
        decctr++;
        if (decctr == DECIMATION) { // after DECIMATION control loops,
            decctr = 0; // reset decimation counter
            ADCarray[plotind] = adcval; // store data in global arrays
            REFarray[plotind] = Waveform[counter];
            plotind++; // increment plot data index
        }
        if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
            plotind = 0; // reset the plot index
            StoringData = 0; // tell main data is ready to be sent to MATLAB
        }
    }
    counter++; // add one to counter every time ISR is entered
    if (counter == NUMSAMPS) {
        counter = 0; // rollover counter over when end of waveform reached
    }

    IFS0bits.T2IF = 0;         // Clear Timer2 interrupt flag
}

int main(void)
{
    NU32DIP_Startup();
    makeWaveform();            // Create the square waveform

    char message[100]; // message to and from python
    float kptemp = 0, kitemp = 0, eintmaxtemp = 0; // temporary local gains
    int i = 0; // plot data loop counter

    ANSELA = 0;                // Turn off analog inputs
    RPA0Rbits.RPA0R = 0b0101;  // Map OC1 to pin RPA0 

    ADC_Startup();             // Initialize the ADC

    // Configure Timer3 for PWM at 20kHz
    T3CONbits.TCKPS = 0;       // Prescaler = 1
    PR3 = NU32DIP_SYS_FREQ/20000 - 1;                // 20kHz period
    TMR3 = 0;
    OC1CONbits.OCTSEL = 1;     // Timer3 as the clock source for OC1
    OC1CONbits.OCM = 0b110;    // PWM mode without fault
    OC1R = 500;                // Initial duty cycle
    OC1RS = 500;               
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
        NU32DIP_ReadUART1(message, sizeof(message)); // wait for a message from MATLAB
        sscanf(message, "%f %f %f" , &kptemp, &kitemp, &eintmaxtemp);
        __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
        Kp = kptemp; // copy local variables to globals used by ISR
        Ki = kitemp;
        eintmax = eintmaxtemp;
        eint = 0; // reset the error integral
        __builtin_enable_interrupts(); // only 2 simple C commands while ISRs disabled
        StoringData = 1; // message to ISR to start storing data
        while (StoringData) { // wait until ISR says data storing is done
        ; // do nothing
        }
        for (i=0; i<PLOTPTS; i++) { // send plot data to MATLAB
        // when first number sent = 1, MATLAB knows we’re done
        sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
        NU32DIP_WriteUART1(message);
        }
    }

    return 0;
}
