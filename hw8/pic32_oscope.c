#include "NU32dip.h"

// ADC information
#define MAXPLOTPTS 1000
#define SAMPLE_TIME 6 // 24MHz*250ns
void ADC_Startup();
unsigned int adc_sample_convert(int);

int main(void)
{
  NU32DIP_Startup();
  ADC_Startup();

  char message[100];
  char command;
  int datapoints;
  int index;
  short data[MAXPLOTPTS];

  while (1)
  {
    // wait for oscope.py to send a command and number of data points to collect
    NU32DIP_ReadUART1(message, 100);
    sscanf(message, "%c %d", &command, &datapoints);
    if (datapoints > MAXPLOTPTS){
      datapoints = MAXPLOTPTS;
    }

    // get the data
    if (command == 'a'){
      // read at 100Hz
      for (index = 0; index < datapoints; index++){
        _CP0_SET_COUNT(0);
        data[index] = adc_sample_convert(1);
        while (_CP0_GET_COUNT() < 24000000 / 100){}
      }
    }
    else if (command == 'b'){
      // read at 200kHz
      for (index = 0; index < datapoints; index++){
        _CP0_SET_COUNT(0);
        data[index] = adc_sample_convert(1);
        while (_CP0_GET_COUNT() < 24000000 / 200000){}
      }
    }

    // print the data back
    for (index = 0; index < datapoints; index++)
    {
      sprintf(message, "%d %d\r\n", datapoints - index, data[index]);
      NU32DIP_WriteUART1(message);
    }
  }
  return 0;
}

void ADC_Startup(){
  ANSELAbits.ANSA1 = 1; // AN1 is an adc pin
  AD1CON3bits.ADCS = 1; // ADC clock period is Tad = 2*(ADCS+1)*Tpb =2*2*(1/48000000Hz) = 83ns > 75ns
  AD1CON1bits.ADON = 1;
}

unsigned int adc_sample_convert(int pin)
{
  unsigned int elapsed = 0, finish_time = 0;
  AD1CHSbits.CH0SA = pin;
  AD1CON1bits.SAMP = 1;
  elapsed = _CP0_GET_COUNT();
  finish_time = elapsed + SAMPLE_TIME;
  while (_CP0_GET_COUNT() < finish_time)
  {
    ;
  }
  AD1CON1bits.SAMP = 0;
  while (!AD1CON1bits.DONE)
  {
    ;
  }
  return ADC1BUF0;
}