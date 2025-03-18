#ifndef MODE_H
#define MODE_H

typedef enum {
    IDLE,
    PWM,
    ITEST,
    HOLD,
    TRACK
} Mode;

Mode get_mode();
void set_mode(Mode m);

#endif // MODE_H
