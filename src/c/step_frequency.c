#include <pebble.h>
#include "step_frequency.h"
#include "variables.h"
#include "user_interface.h"


// Global variables 
const float df = 0.1953125; // interval between 2 frequencies in an fft over 128 points
uint16_t f_st = 0; //steps frequency
uint16_t n_steps = 0;//number of steps
short reset_called = 0;



//update the step's frequency in the module 
void update_freq_step(uint16_t freq)
{
  f_st = freq;
}



//return the number of steps done
uint16_t get_n_steps()
{
  return n_steps;
}



void reset_n_steps()
{
  n_steps = 0;
  update_reset_called(1);//to close all the interruptions who're still being called
}

void update_reset_called(short reset_c)
{
  reset_called = reset_c;
}



// Function that compute the maximum peak
uint16_t max_peak(unsigned short* Y_freq, int32_t* ipeak,uint16_t L_peak)
{	
	uint16_t max_peaki = ipeak[0];
			
	int k=1;
  for(k=1; k<L_peak;k++)
	{
		if(Y_freq[ipeak[k]] > Y_freq[max_peaki])
		{
    	max_peaki = ipeak[k];
		}
	}
	return max_peaki;
}



// Function that computes the right frequency after the FFT
// param[in] Y_freq the table of the modulus of the fft
int16_t freq_calculator(unsigned short* Y_freq)
{	
	int32_t ipeak[14]={0};       // table containing the main peaks
	uint16_t L_peak=0; 		     // number of main peaks
	uint16_t right_freq=0;		 // final frequency determined thanks to FFT
	uint16_t imax_peak=0; 	     // index of the max peak
	
	// We only look at the interesting frequecies, from 0.5 to 3.5Hz
	for(uint16_t l=ID_05; l<ID_35; l++)
	{// If the values before and after are lower than the one we are looking at 
		//and if local peak is high enough
  	if((Y_freq[l] < Y_freq[l+1] )&&( Y_freq[l+1] >= Y_freq[l+2] ) && (Y_freq[l+1] > THRESHOLD))
		{
      // store it
      ipeak[L_peak] = (l+1); 
			L_peak++;
		}
	}
	
	// If there is no peak of the first threshold he is not walking
	if(L_peak > 0)
	{
  	//If there is a single peak then it is the good one
  	if(L_peak == 1)
		{	
			if(Y_freq[ipeak[0]] > THRESHOLD)
				right_freq = df*100*ipeak[0];
			else right_freq =0;
		}
  	else if(L_peak >1) 
			{
     		// Search for the max peak in case of multiple peaks
				imax_peak = max_peak(Y_freq,ipeak, L_peak);
				
				if(Y_freq[imax_peak] > THRESHOLD)
				{
					// Computation of the frequency corresponding to the peak
      		right_freq = df*100*imax_peak;
				}
			}
  		else right_freq = 0;
		}
		else right_freq = 0;

	return right_freq; 
}


//Function that counts the number of steps based on the step_frequency
//Also takes care of the case when the frequency changes "in the middle of a step"
void step_callback()
{  
  if(reset_called == 0)
  {
    static int frac_step = 0;
    if(f_st != 0)
    {
        frac_step += f_st*T_CALL_ST;
        if(frac_step >= (1000))
        {
            frac_step -= 1000;
            n_steps++;
            update_number_steps_display(n_steps);
        }
    }
    else 
    {
      frac_step = 0;
    }
    
    app_timer_register(T_CALL_ST*100, step_callback, NULL);
  }
  return;
}


