#include <pebble.h>
#include "acceleration_process.h"
#include "step_frequency.h"
#include "fourier.h"
#include "variables.h"

TextLayer *pedometer_layer;

short mag_128[128] = {0}; 	  // Magnitude's table
short gbloc = 0;			      // Starting point of the table
short fi[128];
short fr[128];



//Function called when "num_samples" accelerometer samples are ready
void accel_data_handler(AccelData *data, uint32_t num_samples)
{ 	
	uint16_t j;
	gbloc = 0;
		static short dofft = 2;       // do the fft every 50 sample data
		
    for(j=0; j<num_samples; j++)  //creation of the table of magnitude
    {
    	unsigned int temp = (data[j].x)*(data[j].x)
    	+ (data[j].y)*(data[j].y) 
    	+ (data[j].z)*(data[j].z);
		// to avoid overflows, we just take the most significant bits
    	if((temp>>6)>32767)
    		temp = 32767; 
    	
    	else	
    		temp = temp>>6; 
    	
    	mag_128[(j+gbloc)%128] = (short)(temp); 
    }
    
		// shifts by 25 without ever going over 127
    gbloc+=25;
    gbloc%=128; 
    
    int16_t i = 0; 
		// Calls the fft on 128 points every time 50 extra data points are sampled
    if(!dofft)
    {
    	for(i=127;i>=0;i--)
    	{
    		fi[i]=0;
    		fr[i]=mag_128[(i+gbloc)%128];
    	}
    	
    	fix_fft(Y_freq, fr, fi, M, 0);
    	
    	
			// Computation of the correct wlaking frequency 
    	int ff = freq_calculator(Y_freq);
      		update_freq_step(ff); //update the step frequency
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
    update_reset_called(0);//We allow again the interruptions
    
    APP_LOG(APP_LOG_LEVEL_INFO, "Init accel finished\n");   
}

