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
void calculateGrowth(Investment *invp);

void calculateGrowth(Investment *invp)
{
    int i;
    for (i = 1; i <= invp->years; i = i + 1)
    {
        invp->invarray[i] = invp->growth * invp->invarray[i - 1];
    }
}
