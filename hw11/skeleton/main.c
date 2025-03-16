#include "nu32dip.h" // config bits, constants, funcs for startup and UART
#include "encoder.h"
#include "ina219.h"
#include "current.h"
#include "mode.h"
#include "position.h"
#include <stdio.h>

#define BUF_SIZE 200

void timer3_init()
{
    // Configure Timer3 for a 20kHz PWM frequency
    T3CONbits.TCKPS = 0;        // Prescaler = 1
    PR3 = NU32DIP_SYS_FREQ/20000 - 1;  // Set period for 20kHz
    TMR3 = 0;                   // Initial TMR3 count is 0
    // Turn on Timer3
    T3CONbits.ON = 1;
}

void timer4_init()
{
    // Configure Timer 4 for a 5kHz current ISR frequency
    T4CONbits.TCKPS = 2;        // Prescaler = 4
    PR4 = NU32DIP_SYS_FREQ/20000 - 1;  // Set period for 5kHz
    TMR4 = 0;                   // Initial TMR4 count is 0
    // Turn on Timer4
    IPC4bits.T4IP = 4;          // Interrupt priority 4
    IPC4bits.T4IS = 0;          // Subpriority 0
    IFS0bits.T4IF = 0;          // Clear interrupt flag
    IEC0bits.T4IE = 1;          // Enable interrupt
    T4CONbits.ON = 1;
}

void timer5_init()
{
    // Configure Timer 5 for a 200Hz position ISR frequency
    T5CONbits.TCKPS = 7;        // Prescaler = 256
    PR5 = NU32DIP_SYS_FREQ/256/200 - 1;  // Set period for 200Hz
    TMR5 = 0;                   // Initial TMR5 count is 0
    // Turn on Timer5
    IPC5bits.T5IP = 3;          // Interrupt priority 3
    IPC5bits.T5IS = 0;          // Subpriority 0
    IFS0bits.T5IF = 0;          // Clear interrupt flag
    IEC0bits.T5IE = 1;          // Enable interrupt
    T5CONbits.ON = 1;
}

void OC1_init()
{
    // Disable analog functionality on Port A and set RA0 as digital output
    ANSELA = 0;  
    TRISAbits.TRISA0 = 0;  

    // Map RA0 (RPA0) to OC1 for PWM output
    RPA0Rbits.RPA0R = 0b0101;  

    // Configure OC1 for PWM mode using Timer3
    OC1CONbits.OCTSEL = 1;      // Use Timer3 as the clock source for OC1
    OC1CONbits.OCM = 0b110;     // PWM mode without fault pin
    OC1R = 0;             // Initialize at 50% duty cycle
    OC1RS = 0;

    // Turn on OC1
    OC1CONbits.ON = 1;
}

int main()
{
    char buffer[BUF_SIZE];
    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    NU32DIP_GREEN = 1; // turn off the LEDs
    NU32DIP_YELLOW = 1;
    __builtin_disable_interrupts();
    UART2_Startup();
    INA219_Startup();

    // Configure RB10 as a digital output for direction control.
    TRISBbits.TRISB10 = 0;

    timer3_init();
    timer4_init();

    OC1_init();

    __builtin_enable_interrupts();

    NU32DIP_GREEN = 0; // turn on green LED

    // Main Loop
    while (1)
    {
        NU32DIP_ReadUART1(buffer, BUF_SIZE); // we expect the next character to be a menu command
        NU32DIP_YELLOW = 1;                  // clear the error LED
        switch (buffer[0])
        {
        case 'a': // read current sensor adc ##NOT IMPLEMENTED##
        {
            NU32DIP_WriteUART1("Current sensor ADC is not implemented due to hardware change.\n");
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
            NU32DIP_WriteUART1("Enter PWM (-100 to 100): \r\n");
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
                char msg[50];
                sprintf(msg, "Setting PWM to %d\r\n", pwm);
                NU32DIP_WriteUART1(msg);
                set_pwm(pwm);
                set_mode(PWM);
            }
            break;
        }
        case 'g': // set current gains
        {
            // NU32DIP_WriteUART1("Enter Kp: \r\n");
            char kp_buffer[50];
            float kp;

            NU32DIP_ReadUART1(kp_buffer, BUF_SIZE); 
            // get the kp value
            sscanf(kp_buffer, "%f", &kp);

            // NU32DIP_WriteUART1("Enter Ki: \r\n");
            char ki_buffer[50];
            float ki;

            NU32DIP_ReadUART1(ki_buffer, BUF_SIZE); 
            // get the ki value
            sscanf(ki_buffer, "%f", &ki);

            set_current_gains(kp, ki);
            // NU32DIP_WriteUART1("Current gains set\r\n");
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
        case 'i': // set position gains
        {
            // NU32DIP_WriteUART1("Enter Kp: \r\n");
            char kp_buffer[50];
            float kp;

            NU32DIP_ReadUART1(kp_buffer, BUF_SIZE); 
            // get the kp value
            sscanf(kp_buffer, "%f", &kp);

            // NU32DIP_WriteUART1("Enter Ki: \r\n");
            char ki_buffer[50];
            float ki;

            NU32DIP_ReadUART1(ki_buffer, BUF_SIZE); 
            // get the ki value
            sscanf(ki_buffer, "%f", &ki);

            // NU32DIP_WriteUART1("Enter Kd: \r\n");
            char kd_buffer[50];
            float kd;

            NU32DIP_ReadUART1(kd_buffer, BUF_SIZE); 
            // get the kd value
            sscanf(kd_buffer, "%f", &kd);

            set_position_gains(kp, ki, kd);
            // NU32DIP_WriteUART1("Position gains set\r\n");
            break;
        }
        case 'j':
        {
            PositionGains gains = get_position_gains();
            char m[50];
            sprintf(m, "Kp: %.2f, Ki: %.2f, Kd: %.2f \r\n", gains.Kp, gains.Ki, gains.Kd);
            NU32DIP_WriteUART1(m);
            break;
        }
        case 'k':
        {
            set_mode(ITEST);
            // NU32DIP_WriteUART1("Current controller in ITEST mode\r\n");
            break;
        }
        case 'p': // unpower the motor
        {
            set_mode(IDLE);
            NU32DIP_WriteUART1("Motor unpowered\r\n");
            break;
        }
        case 'q':
        {
            NU32DIP_WriteUART1("Exiting program\r\n");
            set_mode(IDLE);
            return 0;
            break;
        }
        case 'r': // get mode
        {
            char m[50];
            switch (get_mode())
            {
            case IDLE:
                sprintf(m, "Mode: IDLE\r\n");
                break;
            case PWM:
                sprintf(m, "Mode: PWM\r\n");
                break;
            case ITEST:
                sprintf(m, "Mode: ITEST\r\n");
                break;
            case HOLD:
                sprintf(m, "Mode: HOLD\r\n");
                break;
            case TRACK:
                sprintf(m, "Mode: TRACK\r\n");
                break;
            default:
                sprintf(m, "Mode: INVALID\r\n");
                break;
            }
            NU32DIP_WriteUART1(m);
            break;
        }
        default:
        {
            NU32DIP_YELLOW = 0; // turn on LED2 to indicate an error
            set_mode(IDLE);
            NU32DIP_WriteUART1("Invalid command\r\n");
            break;
        }
        }
    }
    return 0;
}