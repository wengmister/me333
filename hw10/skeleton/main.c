#include "nu32dip.h" // config bits, constants, funcs for startup and UART
#include "encoder.h"
#include "ina219.h"
#include "current.h"
#include <stdio.h>

#define BUF_SIZE 200

int main()
{
    char buffer[BUF_SIZE];
    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    NU32DIP_GREEN = 1; // turn off the LEDs
    NU32DIP_YELLOW = 1;
    __builtin_disable_interrupts();
    UART2_Startup();
    INA219_Startup();

    // Disable analog functionality on Port A and set RA0 as digital output
    ANSELA = 0;  
    TRISAbits.TRISA0 = 0;  

    // Configure RB10 as a digital output for direction control.
    TRISBbits.TRISB10 = 0;

    // Map RA0 (RPA0) to OC1 for PWM output
    RPA0Rbits.RPA0R = 0b0101;  

    // Configure Timer3 for a 20kHz PWM frequency
    T3CONbits.TCKPS = 0;        // Prescaler = 1
    PR3 = NU32DIP_SYS_FREQ/20000 - 1;  // Set period for 20kHz (assuming NU32DIP_SYS_FREQ is defined)
    TMR3 = 0;

    // Configure OC1 for PWM mode using Timer3
    OC1CONbits.OCTSEL = 1;      // Use Timer3 as the clock source for OC1
    OC1CONbits.OCM = 0b110;     // PWM mode without fault pin
    OC1R = PR3/2;             // Initialize at 50% duty cycle
    OC1RS = PR3/2;
    
    // Turn on Timer3 and OC1
    T3CONbits.ON = 1;
    OC1CONbits.ON = 1;

    __builtin_enable_interrupts();


    // Main Loop
    while (1)
    {
        NU32DIP_ReadUART1(buffer, BUF_SIZE); // we expect the next character to be a menu command
        NU32DIP_YELLOW = 1;                  // clear the error LED
        switch (buffer[0])
        {
        case 'a': // read current sensor adc ##NOT IMPLEMENTED##
        {
            NU32DIP_WriteUART1("Current sensor ADC is not implemented due to hardware change.");
            break;
        }
        case 'b': // read current sensor mA;
        {
            float ma = INA219_read_current();
            char m[50];
            sprintf(m, "Current: %.2f mA\r\n", ma);
            NU32DIP_WriteUART1(m);
            break;
        }
        case 'c': // read encoder count
        {
            WriteUART2("a");
            while(!get_encoder_flag()){}
            set_encoder_flag(0);
            char m[50];
            int p = get_encoder_count();
            sprintf(m, "%d\r\n", p);
            NU32DIP_WriteUART1(m);
            break;
        }
        case 'e': // reset encoder count
        {
            WriteUART2("b");
            NU32DIP_WriteUART1("Encoder count reset\r\n");
            break;
        }
        case 'f': // set PWM (-100 to 100)
        {
            NU32DIP_WriteUART1("Enter PWM (-100 to 100): ");
            char pwm_buffer[50];
            int pwm;

            NU32DIP_ReadUART1(pwm_buffer, BUF_SIZE); 
            // get the pwm value
            sscanf(pwm_buffer, "%d", &pwm);
            if (pwm < -100 || pwm > 100)
            {
                NU32DIP_WriteUART1("Invalid PWM value\r\n");
            }
            else
            {
                char m[50];
                sprintf(m, "Setting PWM to %d\r\n", pwm);
                NU32DIP_WriteUART1(m);

                if (pwm < 0) {
                    LATBbits.LATB10 = 1;  // Negative direction
                } else {
                    LATBbits.LATB10 = 0;  // Positive (or zero) direction
                }

                int pwm_abs = abs(pwm);
                OC1RS = (unsigned int)((pwm_abs / 100.0) * PR3);
            }
            break;
        }
        case 'g': // set current gains
        {
            NU32DIP_WriteUART1("Enter Kp: ");
            char kp_buffer[50];
            float kp;

            NU32DIP_ReadUART1(kp_buffer, BUF_SIZE); 
            // get the kp value
            sscanf(kp_buffer, "%f", &kp);

            NU32DIP_WriteUART1("Enter Ki: ");
            char ki_buffer[50];
            float ki;

            NU32DIP_ReadUART1(ki_buffer, BUF_SIZE); 
            // get the ki value
            sscanf(ki_buffer, "%f", &ki);

            set_current_gains(kp, ki);
            NU32DIP_WriteUART1("Current gains set\r\n");
            break;
        }
        case 'h': // get current gains
        {
            CurrentGains gains = get_current_gains();
            char m[50];
            sprintf(m, "Kp: %.2f, Ki: %.2f\r\n", gains.Kp, gains.Ki);
            NU32DIP_WriteUART1(m);
            break;
        }
        case 'q':
        {
            // handle q for quit. Later you may want to return to IDLE mode here.
            break;
        }
        default:
        {
            NU32DIP_YELLOW = 0; // turn on LED2 to indicate an error
            break;
        }
        }
    }
    return 0;
}