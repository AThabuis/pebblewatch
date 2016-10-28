#include <pebble.h>
#ifndef VARIABLES
#define VARIABLES

#define SIZE_BUFFER 128 
#define SAMPLE_RATE 25	
#define ID_05 4            //indice du tableau de module de fréquence qui correspond à la premiepre fréquence au dessus de 0.5Hz (0.7812 Hz)
#define ID_35 18	
#define THRESHOLD 20       //10000*0.26.^2 Threshold de fréquence minimum // déterminé expérimentalement
#define M 7			           // 128 = 2^7; 
#define T_CALL_ST 1        //[ds] period we call the count step callback


// Variables globales 
unsigned short Y_freq[128];

#endif