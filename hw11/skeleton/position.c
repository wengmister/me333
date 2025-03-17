#include "position.h"


static float kpp = 7.5;
static float kip = 0.05;
static float kdp = 75.0;  // default values

static float position = 0.0;
static float pos_ref_current = 0.0;

static float trajectory[1000]; // Adjust size as needed
static int trajectory_length = 0;

void set_trajectory(float *traj, int length)
{
    trajectory_length = length;
    for (int i = 0; i < length; i++) {
        trajectory[i] = traj[i];
    }
}

float get_trajectory_point(int index)
{
    if (index < trajectory_length) {
        return trajectory[index];
    } else {
        return 0.0; // Return 0 if index is out of bounds
    }
}

void set_desired_angle(float angle)
{
    position = angle;
    // char m[50];
    // sprintf(m, "Desired angle: %.2f \r\n", position);
    // NU32DIP_WriteUART1(m);
}

float get_current_angle()
{
    // encoder has 334 lines, so 334*4 = 1336 counts per revolution
    WriteUART2("a");
    while(!get_encoder_flag()){}
    set_encoder_flag(0);
    int p = get_encoder_count();
    float angle = (float)p/1336*360;
    // char m[50];
    // sprintf(m, "Current angle: %.2f \r\n", angle);
    // NU32DIP_WriteUART1(m);
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
    Mode m = get_mode();
    switch (m)
    {
        case HOLD:
        {
            // get the current angle
            float current_angle = get_current_angle();
            // get the desired angle
            float desired_angle = position;
            // calculate the error
            float error = desired_angle - current_angle;

            // PID control variables
            static float integral = 0.0;
            static float previous_error = 0.0;

            // calculate the integral term
            integral += error;
            // Add integral windup protection
            if (integral > 1000) integral = 1000;
            if (integral < -1000) integral = -1000;

            // calculate the derivative term
            float derivative = error - previous_error;

            // calculate the control effort with PID
            float control_effort = kpp * error + kip * integral + kdp * derivative;

            // update the previous error
            previous_error = error;

            // set the control effort
            pos_ref_current = control_effort;
            break;
        }
        case TRACK:
        {
            if (trajectory_index < trajectory_length) {
                position = get_trajectory_point(trajectory_index);
                trajectory_index++;
            } else {
                set_mode(IDLE); // End of trajectory
                trajectory_index = 0;
            }
            break;
        }
        default:
            break;
    }
    // clear the ISR flag
    IFS0bits.T5IF = 0;
}