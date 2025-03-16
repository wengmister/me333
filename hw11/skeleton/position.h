#ifndef POSITION_H
#define POSITION_H

#include <xc.h>
#include "mode.h"

typedef struct {
    float Kp;
    float Ki;
    float Kd;
} PositionGains;


void set_position_gains(float kp, float ki, float kd);
PositionGains get_position_gains();


#endif  // POSITION_H