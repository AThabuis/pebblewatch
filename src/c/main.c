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


// Declare the main window and two text layers
Window *main_window;
TextLayer *background_layer;
TextLayer *helloWorld_layer;



/*AJOUT start-----------------------------------------------------------------*/
//Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
{ 
    static int16_t last_value1 = 0;  //second last value for the mobile mean for the last call
    static int16_t last_value2 = 0; //last value for the mobile mean for the last call 
    
  
    int16_t future_value1 = 0;
    int16_t future_value2 = 0; //pour ne pas écraser les données, dû au sens de rotation du buffer
  
    int mag = 0;
    int16_t data_mag[num_samples];
    uint16_t i = 0;
  
    
    for(i=0; i<num_samples; i++)
    {
        data_mag[i] = (data[i].x)*(data[i].x) + (data[i].y)*(data[i].y) + (data[i].z)*(data[i].z);
    }
  
    future_value1 = data_mag[num_samples - 2];//second-last data of the table
    future_value2 = data_mag[num_samples - 1];//last data of the table
  
    for(i=0; i<=num_samples; i++)
    {
      /*La moyenne mobile part du dernier élément du buffer actuel pour aller à son premier élément.
      La moyenne se faisant sur 3 éléments, lorsque l'on se trouve dans le haut (2ème puis premier 
      élément) on utilise les deux derniers éléments du précédent buffer (nommé last_value1/2) pour 
      effectuer la moyenne */
      if(i<(num_samples - 3))//dans le "bas" du buffer
      {
         data_mag[num_samples-1 -i]=
            (data_mag[num_samples-1 -i] + data_mag[num_samples-1 -(i+1)] + data_mag[num_samples-1 -(i+2)]) / 3;
      }
      else if(i == (num_samples - 2))//Deuxième élément du tableau
      {
          data_mag[num_samples-1 -i]=
            (data_mag[num_samples-1 -i] + data_mag[num_samples-1 -(i+1)] + last_value1) / 3;
      }
      else//Au niveau du premier élément
      {
          data_mag[num_samples-1 -i]=
            (data_mag[num_samples-1 -i] + last_value1 + last_value2) / 3;
      }
    }
    
    //On attribut les deux dernières valeurs de l'ancien buffer sauvegardées à last_value
    last_value1=future_value1;
    last_value2=future_value2;
    
  //magnitude a pour valeur le dernier élément du tableau
    mag = data_mag[num_samples-1];
    
 /*   //ancien code pour afficher juste les coordonnées
  //Read samples 0's x, y and z values
    int16_t x = data[0].x;
    int16_t y = data[0].y;
    int16_t z = data[0].z;
    int16_t mag = x*x +y*y+ z*z;*/
  
    //tab of chars to print the results on the watch
    static char results[60];
  
    //Print the results in the LOG
    APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%d",mag);
  //APP_LOG(APP_LOG_LEVEL_INFO, "x : %d, \ny : %d,\n z : %d, mag : %d",x, y, z, mag); //ancien code pour afficher juste les coordonnées
    
    //Print the results on the watch
    snprintf(results, 60, "Magnitude : \n%d",mag);
   // snprintf(results, 60, "x : %d, \ny : %d,\n z : %d, mag : %d",x, y, z, mag);  //ancien code pour afficher juste les coordonnées
  text_layer_set_text(helloWorld_layer, results);
}
/*AJOUT end---------------------------------------------------------------------------*/



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
    helloWorld_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
		// Setup layer Information
		text_layer_set_background_color(helloWorld_layer, GColorClear);
		text_layer_set_text_color(helloWorld_layer, GColorWhite);	
		text_layer_set_font(helloWorld_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));//change 18 --> 14
  	text_layer_set_text_alignment(helloWorld_layer, GTextAlignmentCenter);

  	// Add layers as childs layers to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
	  layer_add_child(window_layer, text_layer_get_layer(helloWorld_layer));
  
  	// Show the window on the watch, with animated = true
  	window_stack_push(main_window, true);
    
    // Add a logging meassage (for debug)
	  APP_LOG(APP_LOG_LEVEL_DEBUG, "Just write my first app!");
    
  /*AJOUT start-----------------------------------------------------------------------*/
    
    uint32_t num_samples = 25;
  
    //Allow accelerometer event
    accel_data_service_subscribe(num_samples, accel_data_handler);
    
    //Define accelerometer sampling rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);
  /*AJOUT end----------------------------------------------------------------------------*/
}






// deinit function called when the app is closed
static void deinit(void) {
    
    //Stop Accelerometer
    accel_data_service_unsubscribe();
    
    // Destroy layers and main window 
    text_layer_destroy(background_layer);
	  text_layer_destroy(helloWorld_layer);
    window_destroy(main_window);
}




int main(void) {
    init();
    app_event_loop();
    deinit();
}

