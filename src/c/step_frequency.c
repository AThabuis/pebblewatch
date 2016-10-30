#include <pebble.h>
#include "step_frequency.h"
#include "variables.h"
#include "user_interface.h"


// Variables globales 
const float df = 0.1953125; // interval entre 2 fréquences après FFT sur 128 pts. 
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



// Fonction qui calcule le pic maximum
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



// Fonction qui calcule la bonne fréquence après la FFT
// Prend en argument le tableau des modules de FFT
int16_t freq_calculator(unsigned short* Y_freq)
{	
	int32_t ipeak[14]={0};       // tableau contenant les pics principaux
	uint16_t L_peak=0; 		       // nombre de pics principaux
	uint16_t right_freq=0;		   // fréquence finale déterminée part la FFT 
	uint16_t imax_peak=0; 	     // indice du pic maximum
	//short k=0;
	
	// On ne parcoure que les fréquences intéressantes, de 0.5 à 3.5Hz
	for(uint16_t l=ID_05; l<ID_35; l++)
	{// If before and after are lower 
		//If local peak is high enough
  	if((Y_freq[l] < Y_freq[l+1] )&&( Y_freq[l+1] >= Y_freq[l+2] ) && (Y_freq[l+1] > THRESHOLD))
		{
      // store it
      ipeak[L_peak] = (l+1); 
			L_peak++;
		}
	}
	
	// S'il n'y a pas de pic au-dessus du premier Threshold, il ne marche pas. 
	if(L_peak > 0)
	{
  	//S'il n'a qu'un seul pic, c'est celui-ci le bon
  	if(L_peak == 1)
		{	
			if(Y_freq[ipeak[0]] > THRESHOLD)
				right_freq = df*100*ipeak[0];
			else right_freq =0;
		}
  	else if(L_peak >1) 
			{
     		// Recherche du pic maximum 
				// max_peak(Y_freq, ipeak, L_peak);
				imax_peak = max_peak(Y_freq,ipeak, L_peak);
				
				if(Y_freq[imax_peak] > THRESHOLD)
				{
					// Initialisation de la bonne fréquence à celle maximum
      		right_freq = df*100*imax_peak;
				}
			}
  		else right_freq = 0;
		}
		else right_freq = 0;

	return right_freq; 
}


//Function that count the number of steps in comparaison with the step_frequency
//Also take care of the case when the frequency change "in the middle of a step"
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


