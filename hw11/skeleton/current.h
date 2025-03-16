#ifndef CURRENT_H
#define CURRENT_H

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "nu32dip.h"

typedef struct {
    float Kp;
    float Ki;
} CurrentGains;

void set_current_gains(float kp, float ki);
CurrentGains get_current_gains();

// void __ISR(_TIMER_4_VECTOR, IPL6SOFT) CurrentController(void);

#endif // CURRENT_H