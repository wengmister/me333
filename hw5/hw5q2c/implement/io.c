#include <stdio.h>
#include <string.h>
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
void sendOutput(double *arr, int years);

int getUserInput(Investment *invp)
{
    int valid;
    char inputString[100];
    char responseString[100];

    // Prompt user for input via UART
    NU32DIP_WriteUART1("Enter investment, growth rate, number of years (up to 100): ");
    
    // Read user input from UART
    NU32DIP_ReadUART1(inputString, sizeof(inputString) - 1);

    // Parse the input string using sscanf
    if (sscanf(inputString, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years)) != 3)
    {
        valid = 0;
    }
    else
    {
        valid = (invp->inv0 > 0) && (invp->growth > 0) && 
                (invp->years > 0) && (invp->years <= MAX_YEARS);
    }

    // Send validation result to UART
    sprintf(responseString, "Valid input? %d\n", valid);
    NU32DIP_WriteUART1(responseString);

    if (!valid)
    {
        NU32DIP_WriteUART1("Invalid input; exiting.\n");
    }

    return valid;
}

void sendOutput(double *arr, int yrs)
{
    int i;
    char outstring[100];

    // Send results via UART
    NU32DIP_WriteUART1("\nRESULTS:\n\n");
    for (i = 0; i <= yrs; i++)
    {
        sprintf(outstring, "Year %3d: %10.2f\n", i, arr[i]);
        NU32DIP_WriteUART1(outstring);
    }
    NU32DIP_WriteUART1("\n");
}
