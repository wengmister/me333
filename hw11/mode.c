#include "mode.h"

static Mode mode = IDLE;

Mode get_mode()
{
    return mode;
}

void set_mode(Mode m)
{
    mode = m;
}