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
#define THRESHOLD 150 //10000*0.26.^2 Threshold de fréquence minimum // déterminé expérimentalement
#define M 7			// 128 = 2^7; 

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


// Fonction qui calcule la bonne fréquence après la FFT
// Prend en argument le tableau des modules de FFT
static int16_t freq_calculator(unsigned short* Y_freq)
{	
	int32_t ipeak[14]={0}; // tableau contenant les pics principaux
	uint16_t L_peak=0; 		// nombre de pics principaux
	uint16_t right_freq=0;		// fréquence finale déterminée part la FFT
	uint16_t imax_peak=0; 	// indice du pic maximum 

	// On ne parcoure que les fréquences intéressantes, de 0.5 à 3.5Hz
	for( uint16_t l=ID_05; l<ID_35; l++)
	{// If before and after are lower 
		//If local peak is high enough
  	if((Y_freq[l] < Y_freq[l+1] )&&( Y_freq[l+1] > Y_freq[l+2] )&& (Y_freq[l+1] > THRESHOLD))
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
			right_freq = df*100*ipeak[0];
		}
  	else if(L_peak >1) 
			{
     		// Recherche du pic maximum 
      	imax_peak = ipeak[0];
			
				int k=1;
      	for(k=1; k<L_peak;k++)
				{
					if(Y_freq[ipeak[k]] > Y_freq[imax_peak])
					{
        		imax_peak = ipeak[k];
					}
				}
      	// Initialisation de la bonne fréquence à celle maximum
      	right_freq = df*100*imax_peak;
			
				// Taille des pics accepté à 40%
				uint16_t Y_max = 0.4*Y_freq[imax_peak];
			
      	for(k =1;k<L_peak;k++)
				{
      		// Selectionne les pics jusqu'a 40% de la taille du maximum
        	if(Y_freq[ipeak[k]] > Y_max)
					{
        		// Recherche la première harmonique du pic max
          	// S'il y en a une, c'est elle la bonne fréquence
          	if(2*imax_peak-ipeak[k] < 1)
						{
          		right_freq = df*100*ipeak[k];
						}               
					}
				}
  			//else right_freq = df*imax_peak;
			}
	}
	else right_freq = 0;
	
	
	return right_freq; 
}


//Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
{ 
    //static int16_t mag = 0;  
    //mobile_mean_accel(data,num_samples,&mag);
  
  	/*
    //ancien code pour afficher juste les coordonnées au carré
    //Read samples 0's x, y and z values
    int32_t x = abs(data[0].x) * abs(data[0].x);
    int32_t y = abs(data[0].y) * abs(data[0].y);
    int32_t z = abs(data[0].z) * abs(data[0].z);
  	*/
		uint16_t i;
		gbloc = 0;
	
    for(i=0; i<num_samples; i++)//we make a tabble of magnitude
    {
			//unsigned int temp 
				mag_128[i+gbloc] = (data[i].x>>3)*(data[i].x>>3)
                                   + (data[i].y>>3)*(data[i].y>>3) 
                                   + (data[i].z>>3)*(data[i].z>>3);
					//mag_128[i+gbloc] = (short)(temp>>6); 
			APP_LOG(APP_LOG_LEVEL_INFO, "%u,",mag_128[i+gbloc]);
    }
		
		// Se déplace de 25 cases sans jamais dépasser 127
  	gbloc+=25;
		gbloc%=128; 
		
		// Appel FFT sur 128
	
    for(i=0;i<128;i++)
		{
				fi[i]=0;
				fr[i]=mag_128[(i+gbloc)%128];
				Y_freq[i]=0; 
		}
		
		fix_fft(Y_freq, fr, fi, M, 0);
		
		for(i=ID_05;i<ID_35;i++)
		{
			//APP_LOG(APP_LOG_LEVEL_INFO, "ID_0%d = %hu\n",i,Y_freq[i]);
		}
		
		// Calcule de la bonne fréquence 
		int ff = freq_calculator(Y_freq);
	
    //tab of chars to print the results on the watch
    static char results[60];
  
    //Print the results in the LOG
    //APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%lu",mag);
    //APP_LOG(APP_LOG_LEVEL_INFO, "right_freq = %d\n",ff); //ancien code pour afficher juste les coordonnées
    
    //Print the results on the watch
    //snprintf(results, 60, "Magnitude : \n%lu",mag);  
    snprintf(results, 60, "right_freq = %u\n",ff);  //ancien code pour afficher juste les coordonnées
    text_layer_set_text(pedometer_layer, results);
}


// ....Pas sûr que sa soit utile.... 
// Initialisation du buffer à 0	***************************************************
static void init_buf(void) 
{
		int i=0;
    for(i=0;i<128;i++)
		{
				mag_128[i]=0;
		}
}



// Init function called when app is launched
static void init(void) {

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
	  APP_LOG(APP_LOG_LEVEL_DEBUG, "Just write my first app!");
    
  
    //****************************************************************
    uint32_t num_samples = 25;
  
    //Allow accelerometer event
    accel_data_service_subscribe(num_samples, accel_data_handler);
    
    //Define accelerometer sampling rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
		
		// init_buffer
		//init_buf(); 
}






// deinit function called when the app is closed
static void deinit(void) {
    
    //Stop Accelerometer
    accel_data_service_unsubscribe();
    
    // Destroy layers and main window 
    text_layer_destroy(background_layer);
	  text_layer_destroy(pedometer_layer);
    window_destroy(main_window);
}





int main(void) {
    init();
    app_event_loop();
    deinit();
}



