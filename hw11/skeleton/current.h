#ifndef CURRENT_H
#define CURRENT_H

typedef struct {
    float Kp;
    float Ki;
} CurrentGains;

void set_current_gains(float kp, float ki);
CurrentGains get_current_gains();

#endif // CURRENT_H