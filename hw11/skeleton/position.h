#ifndef POSITION_H
#define POSITION_H

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "encoder.h"
#include "mode.h"
#include "current.h"

#include "nu32dip.h"

typedef struct {
    float Kp;
    float Ki;
    float Kd;
} PositionGains;

void set_trajectory(float *traj, int length)
float get_trajectory_point(int index)

void set_position_gains(float kp, float ki, float kd);
PositionGains get_position_gains();

void set_desired_angle(float angle);
float get_current_angle(); 
float get_pos_ref_current();


#endif  // POSITION_H