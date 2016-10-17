/*---------------------------------------------------------------------------
Template for TP of the course "System Engineering" 2016, EPFL

Authors: Flavien Bardyn & Martin Savary
Version: 1.0
Date: 10.08.2016

Use this "HelloWorld" example as basis to code your own app, which should at least 
count steps precisely based on accelerometer data. 

- Add the accelerometer data acquisition
- Implement your own pedometer using these data
- (Add an estimation of the distance travelled)

- Make an effort on the design of the app, try to do something fun!
- Comment and indent your code properly!
- Try to use your imagination and not google (we already did it, and it's disappointing!)
  to offer us a smart and original solution of pedometer

Don't hesitate to ask us questions.
Good luck and have fun!
---------------------------------------------------------------------------*/

// Include Pebble library
#include <pebble.h>
#include <math.h>
#include <stdio.h>


// Declare the main window and two text layers
Window *main_window;
TextLayer *background_layer;
TextLayer *pedometer_layer;



static void mobile_mean_accel(AccelData *data, uint32_t num_samples, uint32_t *mag)
{
    static int32_t last_value1 = 0;  //second last value for the mobile mean for the last call
    static int32_t last_value2 = 0; //last value for the mobile mean for the last call 
    
    int32_t future_value1 = 0;
    int32_t future_value2 = 0; //pour ne pas écraser les données, dû au sens de rotation du buffer
  
    int32_t data_mag[num_samples];
    uint16_t i = 0;
  
    //use of abs() because oddly the multiplication keeps the sign of the variable here
    for(i=0; i<num_samples; i++)//we make a tabble of magnitude
    {
        /*data_mag[i] = (int32_t) sqrt((abs((data[i].x)*(data[i].x)) 
                                   + abs((data[i].y)*(data[i].y)) 
                                   + abs((data[i].z)*(data[i].z))) / 3);*/
        data_mag[i] = abs((data[i].x)*(data[i].x)) 
                                   + abs((data[i].y)*(data[i].y)) 
                                   + abs((data[i].z)*(data[i].z));
    }
  
    future_value1 = data_mag[num_samples - 2];//second-last data of the table
    future_value2 = data_mag[num_samples - 1];//last data of the table
  
    for(i=0; i<num_samples; i++)
    {
      /*La moyenne mobile part du premier élément du buffer actuel pour aller au dernier élément.
      La moyenne se faisant sur 3 éléments, lorsque l'on se trouve dans le haut (Premier élément puis le second 
      élément) on utilise les deux derniers éléments du précédent buffer (nommé last_value1/2) pour 
      effectuer la moyenne */
      if(i==0)//1er élément du buffer (moy avec les deux dernières valeurs de l'ancien buffer)
      {
         data_mag[0]=
            (last_value1 + last_value2 + data_mag[0]) / 3;
      }
      else if(i == 1)//2eme élément du buffer (moy avec la dernière valeur de l'ancien buffer et le premier élément du buffer actuel)
      {
          data_mag[1]=
            (last_value2  + data_mag[0] + data_mag[1]) / 3;
      }
      else//Pour le reste du buffer
      {
          data_mag[i]=
            (data_mag[i-2] + data_mag[i-1] + data_mag[i]) / 3;
      }
    }
    
    //On attribut les deux dernières valeurs de l'ancien buffer sauvegardées à last_value
    last_value1=future_value1;
    last_value2=future_value2;  
  
  
    //magnitude a pour valeur le dernier élément du tableau
    *mag = data_mag[num_samples-1];
}







//Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
{ 
    uint32_t mag = 0;  
    mobile_mean_accel(data,num_samples,&mag);
  
  
    //ancien code pour afficher juste les coordonnées au carré
    //Read samples 0's x, y and z values
    /* int32_t x = abs(data[0].x) * abs(data[0].x);
    int32_t y = abs(data[0].y) * abs(data[0].y);
    int32_t z = abs(data[0].z) * abs(data[0].z); */
  

    //tab of chars to print the results on the watch
    static char results[60];
  
    //Print the results in the LOG
    APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%lu",mag);
    //APP_LOG(APP_LOG_LEVEL_INFO, "x2 : %lu, y2 : %lu,\nz2 : %lu\n, mag %lu",x, y, z, mag); //ancien code pour afficher juste les coordonnées
    
    //Print the results on the watch
    snprintf(results, 60, "Magnitude : \n%lu",mag);
    //snprintf(results, 60, "x2 : %lu, y2 : %lu,\nz2 : %lu\n, mag %lu",x, y, z, mag);  //ancien code pour afficher juste les coordonnées
    text_layer_set_text(pedometer_layer, results);
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
    accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);
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








/****************************************************************************
* rfft(float X[],int N)                                                     *
*     A real-valued, in-place, split-radix FFT program                      *
*     Decimation-in-time, cos/sin in second loop                            *
*     Input: float X[1]...X[N] (NB Fortran style: 1st pt X[1] not X[0]!)    *
*     Length is N=2**M (i.e. N must be power of 2--no error checking)       *
*     Output in X[1]...X[N], in order:                                      *
*           [Re(0), Re(1),..., Re(N/2), Im(N/2-1),..., Im(1)]               *
*                                                                           *
* Original Fortran code by Sorensen; published in H.V. Sorensen, D.L. Jones,*
* M.T. Heideman, C.S. Burrus (1987) Real-valued fast fourier transform      *
* algorithms.  IEEE Trans on Acoustics, Speech, & Signal Processing, 35,    *
* 849-863.  Adapted to C by Bill Simpson, 1995  wsimpson@uwinnipeg.ca       *
****************************************************************************/
  
  
void rfft(float X[],int N){
    int32_t I,I0,I1,I2,I3,I4,I5,I6,I7,I8, IS,ID;
    int32_t J,K,M,N2,N4,N8;
    float A,A3,CC1,SS1,CC3,SS3,E,R1,XT;
    float T1,T2,T3,T4,T5,T6;  
    
    M=(int)(log(N)/log(2.0));               /* N=2^M */
    
    /* ----Digit reverse counter--------------------------------------------- */
    J = 1;
    for(I=1;I<N;I++)
    {
        if (I<J)
        {
          XT    = X[J];
          X[J]  = X[I];
          X[I]  = XT;
        }
        K = N/2;
        while(K<J)
        {
          J -= K;
          K /= 2;
        }
        J += K;
    }
    
    /* ----Length two butterflies--------------------------------------------- */
    IS = 1;
    ID = 4;
    do
            {
            for(I0 = IS;I0<=N;I0+=ID)
                    {
                    I1    = I0 + 1;
                    R1    = X[I0];
                    X[I0] = R1 + X[I1];
                    X[I1] = R1 - X[I1];
                    }
            IS = 2 * ID - 1;
            ID = 4 * ID;
            }while(IS<N);
    /* ----L shaped butterflies----------------------------------------------- */
    N2 = 2;
    for(K=2;K<=M;K++)
            {
            N2    = N2 * 2;
            N4    = N2/4;
            N8    = N2/8;
            E     = (float) 6.2831853071719586f/N2;
            IS    = 0;
            ID    = N2 * 2;
            do
                    {
                    for(I=IS;I<N;I+=ID)
                            {
                            I1 = I + 1;
                            I2 = I1 + N4;
                            I3 = I2 + N4;
                            I4 = I3 + N4;
                            T1 = X[I4] +X[I3];
                            X[I4] = X[I4] - X[I3];
                            X[I3] = X[I1] - T1;
                            X[I1] = X[I1] + T1;
                            if(N4!=1)
                                    {
                                    I1 += N8;
                                    I2 += N8;
                                    I3 += N8;
                                    I4 += N8;
                                    T1 = (X[I3] + X[I4])*.7071067811865475244f;
                                    T2 = (X[I3] - X[I4])*.7071067811865475244f;
                                    X[I4] = X[I2] - T1;
                                    X[I3] = -X[I2] - T1;
                                    X[I2] = X[I1] - T2;
                                    X[I1] = X[I1] + T2;
                                    }
                            }
                            IS = 2 * ID - N2;
                            ID = 4 * ID;
                    }while(IS<N);
            A = E;
            for(J= 2;J<=N8;J++)
                    {
                    A3 = 3.0 * A;
                    CC1   = cos(A);
                    SS1   = sin(A);  /*typo A3--really A?*/
                    CC3   = cos(A3); /*typo 3--really A3?*/
                    SS3   = sin(A3);
                    A = (float)J * E;
                    IS = 0;
                    ID = 2 * N2;
                    do        
                            {
                            for(I=IS;I<N;I+=ID)
                                    {
                                    I1 = I + J;
                                    I2 = I1 + N4;
                                    I3 = I2 + N4;
                                    I4 = I3 + N4;
                                    I5 = I + N4 - J + 2;
                                    I6 = I5 + N4;
                                    I7 = I6 + N4;
                                    I8 = I7 + N4;
                                    T1 = X[I3] * CC1 + X[I7] * SS1;
                                    T2 = X[I7] * CC1 - X[I3] * SS1;
                                    T3 = X[I4] * CC3 + X[I8] * SS3;
                                    T4 = X[I8] * CC3 - X[I4] * SS3;
                                    T5 = T1 + T3;
                                    T6 = T2 + T4;
                                    T3 = T1 - T3;
                                    T4 = T2 - T4;
                                    T2 = X[I6] + T6;
                                    X[I3] = T6 - X[I6];
                                    X[I8] = T2;
                                    T2    = X[I2] - T3;
                                    X[I7] = -X[I2] - T3;
                                    X[I4] = T2;
                                    T1    = X[I1] + T5;
                                    X[I6] = X[I1] - T5;
                                    X[I1] = T1;
                                    T1    = X[I5] + T4;
                                    X[I5] = X[I5] - T4;
                                    X[I2] = T1;
                                    }
                            IS = 2 * ID - N2;
                            ID = 4 * ID;
                            }while(IS<N);
                    }
            }
    return;
}



int main(void) {
    init();
    app_event_loop();
    deinit();
}

