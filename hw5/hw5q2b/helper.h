#ifndef HELPER_H
#define HELPER_H

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

#endif /* HELPER_H */
