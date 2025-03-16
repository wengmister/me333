#include "current.h"
#include "mode.h"
#include "ina219.h"
#include <stdio.h>

static float Kp = 0.0;
static float Ki = 0.0;  // default values
static int pwm = 0;

void set_pwm(int p)
{
    pwm = p;
}

void set_current_gains(float kp, float ki)
{
    Kp = kp;
    Ki = ki;
}

CurrentGains get_current_gains()
{
    CurrentGains gains;
    gains.Kp = Kp;
    gains.Ki = Ki;
    return gains;
}

void sendITestDataToPython(float *refArray, float *actualArray, int length)
{
    char buffer[100];
    // Send the number of data points
    sprintf(buffer, "%d\n", length);
    NU32DIP_WriteUART1(buffer);

    // Send the reference then the actual current data
    for (int i = 0; i < length; i++)
    {
        sprintf(buffer, "%d %d\n", refArray[i], actualArray[i]);
        NU32DIP_WriteUART1(buffer);
    }
}

void __ISR(_TIMER_4_VECTOR, IPL4SOFT) CurrentController(void)
{
    static int counter = 0;
    static float reference_current = 200.0; // Initial reference current in mA
    static float actual_current_array[100]; // Array to store actual current values
    static float reference_current_array[100]; // Array to store reference current values

    Mode m = get_mode();
    switch (m)
    {
        case IDLE:
            OC1RS = 0;
            break;
        case PWM:
        {
            if (pwm < 0) {
                LATBbits.LATB10 = 1;  // Negative direction
            } else {
                LATBbits.LATB10 = 0;  // Positive (or zero) direction
            }

            int pwm_abs = abs(pwm);
            OC1RS = (unsigned int)((pwm_abs / 100.0) * PR3);
            break;
        }
        case ITEST:
        {
            // Toggle reference current every 25 ISR executions
            if (counter == 25 || counter == 50 || counter == 75) {
                reference_current = -reference_current;
            }

            // Read actual current from sensor (assuming a function read_current_mA() exists)
            float current = INA219_read_current();

            // Save reference and actual current data
            reference_current_array[counter] = reference_current;
            actual_current_array[counter] = current;

            // PI controller
            float error = reference_current - current;
            static float integral = 0.0;
            integral += error;
            float control_signal = Kp * error + Ki * integral;

            // Set PWM and direction based on control signal
            if (control_signal < 0) {
                LATBbits.LATB10 = 1;  // Negative direction
                control_signal = -control_signal;
            } else {
                LATBbits.LATB10 = 0;  // Positive direction
            }

            OC1RS = (unsigned int)((control_signal / 100.0) * PR3);

            counter++;          // Increment counter
            if (counter >= 100) // End ITEST mode after 100 samples (two full cycles)
            {
                set_mode(IDLE);
                sendITestDataToPython(reference_current_array, actual_current_array, 100);
                counter = 0;  // Reset counter
                integral = 0; // Reset integral
            }
            break;
        }
        case HOLD:
            // implement current controller
            break;
        case TRACK:
            // implement current controller
            break;
        default:
            break;
    }
    // Clear the interrupt flag
    IFS0bits.T4IF = 0;
}