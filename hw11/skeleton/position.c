#include "position.h"


static float kpp = 6.0;
static float kip = 0.0;
static float kdp = 60.0;  // default values

static float position = 0.0;
static float pos_ref_current = 0.0;

void set_desired_angle(float angle)
{
    position = angle;
}

float get_current_angle()
{
    // encoder has 334 lines, so 334*4 = 1336 counts per revolution
    WriteUART2("a");
    while(!get_encoder_flag()){}
    set_encoder_flag(0);
    char m[50];
    int p = get_encoder_count();
    float angle = (float)p/1336*360;
    return angle;
}

void set_position_gains(float kp, float ki, float kd)
{
    kpp = kp;
    kip = ki;
    kdp = kd;
}

float get_pos_ref_current()
{
    return pos_ref_current;
}

PositionGains get_position_gains()
{
    PositionGains gains;
    gains.Kp = kpp;
    gains.Ki = kip;
    gains.Kd = kdp;
    return gains;
}


void __ISR(_TIMER_5_VECTOR, IPL5SOFT) PositionController(void)
{
    switch (get_mode())
    {
        case HOLD:
        {
            // get the current angle
            float current_angle = get_current_angle();
            // get the desired angle
            float desired_angle = position;
            // calculate the error
            float error = desired_angle - current_angle;
            // calculate the control effort
            float control_effort = kpp * error + kip * error + kdp * error;
            // set the control effort
            pos_ref_current = control_effort;
            break;
        }
        default:
            break;
    }
    // clear the ISR flag
    IFS0bits.T5IF = 0;
}