#include <pebble.h>
#include "acceleration_process.h"
#include "step_frequency.h"
#include "fourier.h"
#include "variables.h"

TextLayer *pedometer_layer;

short mag_128[128] = {0}; 	  // Tableau des magnitudes 
short gbloc = 0;			      // point de départ du tableau
short fi[128];
short fr[128];



//Function called when "num_samples" accelerometer samples are ready
void accel_data_handler(AccelData *data, uint32_t num_samples)
{ 	
		//APP_LOG(APP_LOG_LEVEL_INFO, "Event data\n"); //ancien code pour afficher juste les coordonnées
	
		uint16_t j;
		gbloc = 0;
		static short dofft = 2;       // faire la fft tous les 75 datas
	
    for(j=0; j<num_samples; j++)  //we make a table of magnitude
    {
			
			unsigned int temp = (data[j].x)*(data[j].x)
                                   + (data[j].y)*(data[j].y) 
                                   + (data[j].z)*(data[j].z);
			// to avoid overflows, we just take the most significant bits
			if((temp>>6)>32767)
				temp = 32767; 
			
			else	temp = temp>>6; 
			
					mag_128[(j+gbloc)%128] = (short)(temp); 
					//APP_LOG(APP_LOG_LEVEL_INFO, "%u,",temp);
    }
		
		// Se déplace de 25 cases sans jamais dépasser 127
  	gbloc+=25;
		gbloc%=128; 
	
		int16_t i = 0; 
		// Appel FFT sur 128, toutes les 50 datas pour plus de précisions
		if(!dofft)
		{
			for(i=127;i>=0;i--)
			{
				fi[i]=0;
				fr[i]=mag_128[(i+gbloc)%128];
				//Y_freq[i]=0; 
			}
		
			fix_fft(Y_freq, fr, fi, M, 0);
		
			
			// Calcule de la bonne fréquence 
			int ff = freq_calculator(Y_freq);
      update_freq_step(ff); //update the step frequency
	
	    //tab of chars to print the results on the watch
	    static char results[60];
	  
	    //Print the results in the LOG
	    //APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%lu",mag);
	    //APP_LOG(APP_LOG_LEVEL_INFO, "right_freq = %d\n",ff); 
	    
	    //Print the results on the watch
	    //snprintf(results, 60, "Magnitude : \n%lu",mag);  
      /*uint16_t number_steps = 0;
      number_steps = get_n_steps();
	    snprintf(results, 60, "right_freq = %u,\n nb_step = %d\n",ff, number_steps);  //ancien code pour afficher juste les coordonnées
	    text_layer_set_text(pedometer_layer, results);*/
      
			dofft = 2; 
		}
		else dofft--; 
}



void init_accel(void)
{
    uint32_t num_samples = 25;
  
    //Allow accelerometer event
    accel_data_service_subscribe(num_samples, accel_data_handler);
    
    //Define accelerometer sampling rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
  
    //Timer
    app_timer_register(T_CALL_ST*100, step_callback, NULL);
  
		APP_LOG(APP_LOG_LEVEL_INFO, "Init accel finished\n");   
}

