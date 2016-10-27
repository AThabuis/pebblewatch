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

// Declare the main window and two text layers
Window *main_window;
TextLayer *background_layer;
TextLayer *pedometer_layer;

// Variables globales 
short mag_128[128]; 	  // Tableau des magnitudes 
short gbloc;			      // point de départ du tableau
const float df = 0.1953125; // interval entre 2 fréquences après FFT sur 128 pts. 
unsigned short Y_freq[128];
short fi[128];
short fr[128];

uint16_t n_steps;      //number of steps
uint16_t f_st;         //steps frequency


#endif