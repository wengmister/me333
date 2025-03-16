#include "current.h"
#include "mode.h"
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
            // implement current controller
            break;
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