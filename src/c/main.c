/*---------------------------------------------------------------------------
Template for TP of the course "System Engineering" 2016, EPFL

Authors: Adrien Thabuis & Antoine Laurens
Version: 1.0
Date: 09.2016



- Add the accelerometer data acquisition
- Implement your own pedometer using these data
- (Add an estimation of the distance travelled)

- Make an effort on the design of the app, try to do something fun!
- Comment and indent your code properly!
- Try to use your imagination and not google (we already did it, and it's disappointing!)
  to offer us a smart and original solution of pedometer

---------------------------------------------------------------------------*/

// Include Pebble library
#include <pebble.h>
#include "mobile_mean_accel.h"
#include "fourier.h"

#define SIZE_BUFFER 128; 
#define SAMPLE_RATE 25;	
#define ID_05 4 //indice du tableau de module de fréquence qui correspond à la premiepre fréquence au dessus de 0.5Hz (0.7812 Hz)
#define ID_35 18	
#define THRESHOLD 20 //10000*0.26.^2 Threshold de fréquence minimum // déterminé expérimentalement
#define M 7			// 128 = 2^7; 
#define T_CALL_ST 1 //[ds] period we call the count step callback

// Declare the main window and two text layers
Window *main_window;
TextLayer *background_layer;
TextLayer *pedometer_layer;

// Variables globales 
short mag_128[128]={0}; 	// Tableau des magnitudes 
short gbloc = 0;			// point de départ du tableau
const float df = 0.1953125; // interval entre 2 fréquences après FFT sur 128 pts. 
unsigned short Y_freq[128];
short fi[128];
short fr[128];

uint16_t n_steps = 0;//number of steps
uint16_t f_st = 0; //steps frequency


// Fonction qui calcule le pic maximum
static uint16_t max_peak(unsigned short* Y_freq, int32_t* ipeak,uint16_t L_peak)
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
static int16_t freq_calculator(unsigned short* Y_freq)
{	
	int32_t ipeak[14]={0}; // tableau contenant les pics principaux
	uint16_t L_peak=0; 		// nombre de pics principaux
	uint16_t right_freq=0;		// fréquence finale déterminée part la FFT 
	uint16_t imax_peak=0; 	// indice du pic maximum
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
      	/*imax_peak = ipeak[0];
				
  			for(k=L_peak; k>0;k--)
				{
					if(Y_freq[ipeak[k]] > Y_freq[imax_peak])
					{
    				imax_peak = ipeak[k];
					}
				}*/
				
				if(Y_freq[imax_peak] > THRESHOLD)
				{
					// Initialisation de la bonne fréquence à celle maximum
      		right_freq = df*100*imax_peak;
					
					/*
					// Taille des pics accepté à 40%
					uint16_t Y_max = 0.4*Y_freq[imax_peak];
					short twofreq = 0; 
					short halffreq = 0; 
      		for(k =L_peak/2;k>0;k--)
					{
      			// Selectionne les pics jusqu'a 40% de la taille du maximum
        		if(Y_freq[ipeak[k]] > Y_max)
						{
        			// Recherche la première harmonique du pic max
          		// S'il y en a une, c'est elle la bonne fréquence
          		if(2*imax_peak-ipeak[k] < 2)
							{
          			//twofreq = 1; 
								right_freq = df*100*ipeak[k];
							}
							
							if(imax_peak/2-ipeak[k] < 1)
							{
								halffreq =1;
								if(twofreq)
									right_freq = df*100*ipeak[k];
								else if(halffreq) 
									right_freq = df*100*ipeak[k];
								else right_freq = 0; 
							}
						}
					}*/
				}
			}
  		else right_freq = 0;
		}
		else right_freq = 0;

	return right_freq; 
}


//Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
{ 	
		//APP_LOG(APP_LOG_LEVEL_INFO, "Event data\n"); //ancien code pour afficher juste les coordonnées
	
		uint16_t j;
		gbloc = 0;
		static short dofft = 2; // faire la fft tous les 75 datas
	
    for(j=0; j<num_samples; j++)//we make a table of magnitude
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
		
			/*for(i=ID_05;i<ID_35;i++)
			{
				APP_LOG(APP_LOG_LEVEL_INFO, "ID_0%d = %hu\n",i,Y_freq[i]);
			}*/
			
			// Calcule de la bonne fréquence 
			int ff = freq_calculator(Y_freq);
      f_st = ff; //update the step frequency
	
	    //tab of chars to print the results on the watch
	    static char results[60];
	  
	    //Print the results in the LOG
	    //APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%lu",mag);
	    //APP_LOG(APP_LOG_LEVEL_INFO, "right_freq = %d\n",ff); 
	    
	    //Print the results on the watch
	    //snprintf(results, 60, "Magnitude : \n%lu",mag);  
	    snprintf(results, 60, "right_freq = %u,\n nb_step = %d\n",ff, n_steps);  //ancien code pour afficher juste les coordonnées
	    text_layer_set_text(pedometer_layer, results);
			dofft = 2; 
		}
		else dofft--; 
		/*
		int Size_left = (int)heap_bytes_free(); 
		int Size_used = (int)heap_bytes_used();
		APP_LOG(APP_LOG_LEVEL_INFO, "Memory used: %d\n",Size_used); //ancien code pour afficher juste les coordonnées
		APP_LOG(APP_LOG_LEVEL_INFO, "Memory left: %d\n",Size_left); //ancien code pour afficher juste les coordonnées
		*/
}


//Function that count the number of steps in comparaison with the step_frequency
//Also take care of the case when the frequency change "in the middle of a step"
static void step_callback()
{  
    static int frac_step = 0;
    if(f_st != 0)
    {
        frac_step += f_st*T_CALL_ST;
    
        if(frac_step >= (1000))
        {
            frac_step -= 1000;
            n_steps++;
        }
    }   
  
    app_timer_register(T_CALL_ST*100, step_callback, NULL);
    return;
}


// Init function called when app is launched
static void init(void) 
{
  	// Create main Window element and assign to pointer
  	main_window = window_create();
    Layer *window_layer = window_get_root_layer(main_window);  
    GRect bounds = layer_get_bounds(window_layer);

		// Create background Layer
		background_layer = text_layer_create(GRect( 0, 0, 144, 168));
		// Setup background layer color (black)
		text_layer_set_background_color(background_layer, GColorBlack);
	
		// Create text Layer
		//helloWorld_layer = text_layer_create(GRect( 20, 65, 100, 20));
    // Create the TextLayer with specific bounds
    pedometer_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
		// Setup layer Information
		text_layer_set_background_color(pedometer_layer, GColorClear);
		text_layer_set_text_color(pedometer_layer, GColorWhite);	
		text_layer_set_font(pedometer_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  	text_layer_set_text_alignment(pedometer_layer, GTextAlignmentCenter);

  	// Add layers as childs layers to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
	  layer_add_child(window_layer, text_layer_get_layer(pedometer_layer));
  
  	// Show the window on the watch, with animated = true
  	window_stack_push(main_window, true);
    
    // Add a logging meassage (for debug)
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Just write my first app!");
    
    uint32_t num_samples = 25;
  
    //Allow accelerometer event
    accel_data_service_subscribe(num_samples, accel_data_handler);
    
    //Define accelerometer sampling rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
		
		// init_buffer
		//init_buf(); 
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "Init finished\n"); 
		/*
		int Size_left = (int)heap_bytes_free(); 
		int Size_used = (int)heap_bytes_used();
		APP_LOG(APP_LOG_LEVEL_INFO, "Memory used: %d\n",Size_used); //ancien code pour afficher juste les coordonnées
		APP_LOG(APP_LOG_LEVEL_INFO, "Memory left: %d\n",Size_left); //ancien code pour afficher juste les coordonnées
		*/

	//Timer
    app_timer_register(T_CALL_ST*100, step_callback, NULL);
    //app_timer_register(1000, fft_callback, NULL);
}






// deinit function called when the app is closed
static void deinit(void) 
{
    //APP_LOG(APP_LOG_LEVEL_INFO, "The END\n"); 
    //Stop Accelerometer
    accel_data_service_unsubscribe();
    
    // Destroy layers and main window 
    text_layer_destroy(background_layer);
	  text_layer_destroy(pedometer_layer);
    window_destroy(main_window);
}





int main(void) 
{
    init();
    app_event_loop();
    deinit();
}




