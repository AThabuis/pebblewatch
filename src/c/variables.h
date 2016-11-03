#include <pebble.h>
#ifndef VARIABLES
#define VARIABLES

#define SIZE_BUFFER 128 
#define SAMPLE_RATE 25	
#define ID_05 4              
#define ID_35 18			 //index of the table that corresponds to the first frquency above 0.5Hz (it is 0.7812HZ)
#define THRESHOLD 20         //10000*0.26.^2 Threshold of the magnitude of the frequency// détermined through exprience
#define M 7			         // 128 = 2^7; power of 2 corresponding to the number of elements used for the FFT
#define T_CALL_ST 1          //[ds] period between the calls of the step callback



// Variables globales 
unsigned short Y_freq[128];

#endif