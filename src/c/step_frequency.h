#include <pebble.h>
#ifndef FREQUENCY
#define FREQUENCY


// Fonction qui calcule le pic maximum
uint16_t max_peak(unsigned short* , int32_t* ,uint16_t );
  

// Fonction qui calcule la bonne fréquence après la FFT
// Prend en argument le tableau des modules de FFT
int16_t freq_calculator(unsigned short* );
  

//Function that count the number of steps in comparaison with the step_frequency
//Also take care of the case when the frequency change "in the middle of a step"
void step_callback();


void update_freq_step(uint16_t);

uint16_t get_n_steps();
void reset_n_steps();
void update_reset_called(short); 

#endif 