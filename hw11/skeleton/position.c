#include "position.h"


static float kpp = 7.5;
static float kip = 0.05;
static float kdp = 75.0;  // default values

static float position = 0.0;
static float pos_ref_current = 0.0;

#define MAX_TRAJECTORY_POINTS 1000  // Define a maximum size for the trajectory array
static float trajectory[MAX_TRAJECTORY_POINTS];
static float tracked_trajectory[MAX_TRAJECTORY_POINTS];
static int trajectory_length = 0;

void set_trajectory(float *traj, int length)
{
    if (length > MAX_TRAJECTORY_POINTS) {
        length = MAX_TRAJECTORY_POINTS;  // Ensure we do not exceed the array size
    }
    trajectory_length = length;
    for (int i = 0; i < length; i++) {
        trajectory[i] = traj[i];
    }
}

void sendTrackData(float *refArray, float *actualArray, int length)
{
    char buffer[MAX_TRAJECTORY_POINTS];
    // Send the number of data points
    sprintf(buffer, "%i\n", length);
    NU32DIP_WriteUART1(buffer);

    // Send the reference then the actual current data
    for (int i = 0; i < length; i++)
    {
        sprintf(buffer, "%f %f\n", refArray[i], actualArray[i]);
        NU32DIP_WriteUART1(buffer);
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
            static int traj_counter = 0;
            static int previous_error = 0;
            static int integral = 0;
            float actual_angle = get_current_angle();
            tracked_trajectory[traj_counter] = (float)actual_angle;
            // PID Controller
            int error = trajectory[traj_counter] - actual_angle;
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
            previous_error = error;

            traj_counter++;
            if (traj_counter >= MAX_TRAJECTORY_POINTS)
            {
                position = trajectory[traj_counter - 1];
                previous_error = 0;
                integral = 0;
                traj_counter = 0;
                sendTrackData(trajectory, tracked_trajectory, MAX_TRAJECTORY_POINTS);
                set_mode(HOLD);
            }
        }
        default:
            break;
    }
    // clear the ISR flag
    IFS0bits.T5IF = 0;
}