#include <pebble.h>
#ifndef FREQUENCY
#define FREQUENCY


// Function that computes the max peak
uint16_t max_peak(unsigned short* , int32_t* ,uint16_t );
  

// Function that computes the correct frequency after the FFT
int16_t freq_calculator(unsigned short* );

//Function that counts the number of steps based on the step_frequency
//Also takes care of the case when the frequency changes "in the middle of a step"
void step_callback();
void update_freq_step(uint16_t);
uint16_t get_n_steps();
void reset_n_steps();
void update_reset_called(short); 

#endif 