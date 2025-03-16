#include "current.h"
#include <stdio.h>

static float Kp = 0.0;
static float Ki = 0.0;  // default values

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