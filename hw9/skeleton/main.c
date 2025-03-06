#include "nu32dip.h" // config bits, constants, funcs for startup and UART
#include "encoder.h"
#include "ina219.h"

#define BUF_SIZE 200

int main()
{
    char buffer[BUF_SIZE];
    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    NU32DIP_GREEN = 1; // turn off the LEDs
    NU32DIP_YELLOW = 1;
    __builtin_disable_interrupts();
    UART2_Startup();
    INA219_Startup();
    __builtin_enable_interrupts();
    while (1)
    {
        NU32DIP_ReadUART1(buffer, BUF_SIZE); // we expect the next character to be a menu command
        NU32DIP_YELLOW = 1;                  // clear the error LED
        switch (buffer[0])
        {
        case 'a': // read current sensor adc ##NOT IMPLEMENTED##
        {
            NU32DIP_WriteUART1("Current sensor ADC is not implemented due to hardware change.");
            break;
        }
        case 'b': // read current sensor mA;
        {
            float ma = INA219_read_current();
            char m[50];
            sprintf(m, "Current: %.2f mA\r\n", ma);
            NU32DIP_WriteUART1(m);
            break;
        }
        case 'c': // read encoder count
        {
            WriteUART2("a");
            while(!get_encoder_flag()){}
            set_encoder_flag(0);
            char m[50];
            int p = get_encoder_count();
            sprintf(m, "%d\r\n", p);
            NU32DIP_WriteUART1(m);
            break;
        }
        case 'e': // reset encoder count
        {
            WriteUART2("b");
            NU32DIP_WriteUART1("Encoder count reset\r\n");
            break;
        }
        case 'q':
        {
            // handle q for quit. Later you may want to return to IDLE mode here.
            break;
        }
        default:
        {
            NU32DIP_YELLOW = 0; // turn on LED2 to indicate an error
            break;
        }
        }
    }
    return 0;
}