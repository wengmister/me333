#include <stdio.h>
#include "nu32dip.h"
#include "helper.h"

int main(void)
{
    // Initialize UART for communication
    NU32DIP_Startup();

    Investment inv;
    while (getUserInput(&inv))
    {
        inv.invarray[0] = inv.inv0;
        calculateGrowth(&inv);
        sendOutput(inv.invarray, inv.years);
    }
    return 0;
}
