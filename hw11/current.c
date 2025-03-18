#include "current.h"
#include <stdio.h>

static float Kp = 0.1;
static float Ki = 0.04;  // default values
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

void sendITestData(float *refArray, float *actualArray, int length)
{
    char buffer[100];
    // Send the number of data points
    sprintf(buffer, "%d\n", length);
    NU32DIP_WriteUART1(buffer);

    // Send the reference then the actual current data
    for (int i = 0; i < length; i++)
    {
        sprintf(buffer, "%f %f\n", refArray[i], actualArray[i]);
        NU32DIP_WriteUART1(buffer);
    }
}

void __ISR(_TIMER_4_VECTOR, IPL4SOFT) CurrentController(void)
{
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
            static int counter = 0;
            static float reference_current = 200.0; // Initial reference current in mA
            static float actual_current_array[100]; // Array to store actual current values
            static float reference_current_array[100]; // Array to store reference current values
            // Toggle reference current every 25 ISR executions
            if (counter == 25 || counter == 50 || counter == 75) {
                reference_current = -reference_current;
            }

            // Read actual current from sensor
            float current = INA219_read_current();

            // Save reference and actual current data
            reference_current_array[counter] = reference_current;
            actual_current_array[counter] = current;

            // PI controller
            float error = reference_current - current;
            static float integral = 0.0;
            integral += error;
            // Add integral windup protection
            if (integral > 1000) integral = 1000;
            if (integral < -1000) integral = -1000;
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
                sendITestData(reference_current_array, actual_current_array, 100);
                counter = 0;  // Reset counter
                integral = 0; // Reset integral
            }
            break;
        }
        case HOLD:
        {
            int current = INA219_read_current(); // read current in mA
            // get position reference current
            int reference_current = get_pos_ref_current();

            // PI controller
            float error = reference_current - current;
            static float integral = 0.0;
            integral += error;
            // Add integral windup protection
            if (integral > 1000) integral = 1000;
            if (integral < -1000) integral = -1000;
            float control_signal = Kp * error + Ki * integral;
            if (control_signal > 100)
            {
                control_signal = 100; // upper bound
            }
            else if (control_signal < -100)
            {
                control_signal = -100; // lower bound
            }

            // Corrected direction control logic
            if (control_signal < 0) {
                LATBbits.LATB10 = 1;  // Negative direction
                control_signal = -control_signal;
            } else {
                LATBbits.LATB10 = 0;  // Positive direction
            }

            OC1RS = (unsigned int)((control_signal / 100.0) * PR3);
            break;
        }
        case TRACK:
        {
            int current = INA219_read_current(); // read current in mA
            // get position reference current
            int reference_current = get_pos_ref_current();

            // PI controller
            float error = reference_current - current;
            static float integral = 0.0;
            integral += error;
            // Add integral windup protection
            if (integral > 1000) integral = 1000;
            if (integral < -1000) integral = -1000;
            float control_signal = Kp * error + Ki * integral;
            if (control_signal > 100)
            {
                control_signal = 100; // upper bound
            }
            else if (control_signal < -100)
            {
                control_signal = -100; // lower bound
            }

            // Corrected direction control logic
            if (control_signal < 0) {
                LATBbits.LATB10 = 1;  // Negative direction
                control_signal = -control_signal;
            } else {
                LATBbits.LATB10 = 0;  // Positive direction
            }

            OC1RS = (unsigned int)((control_signal / 100.0) * PR3);
            break;
        }
        default:
            break;
    }
    // Clear the interrupt flag
    IFS0bits.T4IF = 0;
}