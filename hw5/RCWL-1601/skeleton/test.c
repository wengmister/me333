#include "sr04.h"
#include <xc.h>
#include <stdio.h>
#include "nu32dip.h"

void main() {
    // Startup the SR04 sensor
    SR04_Startup();
    
    // Create a buffer to hold the formatted string
    char buffer[50];  // Adjust size as necessary to hold the formatted string
    
    // Continuously read the distance and print it every 250ms
    while(1) {
        float distance = SR04_read_meters();  // Get distance in meters
        
        // Format the string
        snprintf(buffer, sizeof(buffer), "Distance: %.2f meters\r\n", distance);
        
        // Send the formatted string to UART
        NU32DIP_WriteUART1(buffer);
        
        // Delay for 250ms
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 5000000) {
            ;  // Do nothing
        }
    }
}