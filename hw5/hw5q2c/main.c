#include <stdio.h>
#include "nu32dip.h"

// Define constants and structures
#define MAX_YEARS 100

typedef struct
{
    double inv0;
    double growth;
    int years;
    double invarray[MAX_YEARS + 1];
} Investment;

// Function prototypes
int getUserInput(Investment *invp);
void calculateGrowth(Investment *invp);
void sendOutput(double *arr, int years);

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
