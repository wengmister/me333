#include "position.h"


static float kpp = 0.0;
static float kip = 0.0;
static float kdp = 0.0;  // default values

void set_position_gains(float kp, float ki, float kd)
{
    kpp = kp;
    kip = ki;
    kdp = kd;
}

PositionGains get_position_gains()
{
    PositionGains gains;
    gains.Kp = kpp;
    gains.Ki = kip;
    gains.Kd = kdp;
    return gains;
}


// void __ISR(_TIMER_5_VECTOR, IPL3SOFT) PositionController(void)