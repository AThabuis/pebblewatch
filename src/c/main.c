/*---------------------------------------------------------------------------

Template for TP of the course "System Engineering" 2016, EPFL

Authors: Adrien Thabuis & Antoine Laurens & J.F. B.
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
#include "user_interface.h"
#include "variables.h"
#include "fourier.h"
#include "step_frequency.h"

TextLayer *pedometer_layer;

short mag_128[128] = {0};     // Tableau des magnitudes 
short gbloc = 0;            // point de départ du tableau
short fi[128];
short fr[128];

//Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
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
      
      else  temp = temp>>6; 
      
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
      update_freq_step(ff); //update the step frequency
  
      //tab of chars to print the results on the watch
      static char results[60];
    
      //Print the results in the LOG
      //APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%lu",mag);
      //APP_LOG(APP_LOG_LEVEL_INFO, "right_freq = %d\n",ff); 
      
      //Print the results on the watch
      //snprintf(results, 60, "Magnitude : \n%lu",mag);  
      uint16_t number_steps = 0;
      number_steps = get_n_steps();
      snprintf(results, 60, "right_freq = %u,\n nb_step = %d\n",ff, number_steps);  //ancien code pour afficher juste les coordonnées
      text_layer_set_text(pedometer_layer, results);
      dofft = 2; 
    }
    else dofft--; 
}



// Init function called when app is launched
static void init(void) 
{  
    open_main_window();
  
    uint32_t num_samples = 25;
  
    //Allow accelerometer event
    //accel_data_service_subscribe(num_samples, accel_data_handler);
    
    //Define accelerometer sampling rate
    //accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
  
    APP_LOG(APP_LOG_LEVEL_INFO, "Init finished\n"); 
  
    //Timer
    //app_timer_register(T_CALL_ST*100, step_callback, NULL);
    //app_timer_register(1000, fft_callback, NULL);
}

// deinit function called when the app is closed
static void deinit(void) 
{
    APP_LOG(APP_LOG_LEVEL_INFO, "The END\n"); 
  
    //Stop Accelerometer
    //accel_data_service_unsubscribe();

    // Destroy layers and main window
    close_user_height_window();
    close_text_window();
    close_main_window();
}

int main(void) 
{
    init();
    app_event_loop();
    deinit();
}