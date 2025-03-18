#ifndef CURRENT_H
#define CURRENT_H

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "mode.h"
#include "position.h"
#include "ina219.h"
#include "nu32dip.h"

typedef struct {
    float Kp;
    float Ki;
} CurrentGains;

void set_pwm(int p);
void set_current_gains(float kp, float ki);
CurrentGains get_current_gains();
void sendITestDataToPython(float *refArray, float *actualArray, int length);

#endif // CURRENT_H