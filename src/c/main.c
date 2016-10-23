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


// Declare the main window and two text layers
Window *main_window;
TextLayer *background_layer;
TextLayer *pedometer_layer;




//Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
{ 
    uint32_t mag = 0;  
    mobile_mean_accel(data,num_samples,&mag);
  
  
    //ancien code pour afficher juste les coordonnées au carré
    //Read samples 0's x, y and z values
    int32_t x = abs(data[0].x) * abs(data[0].x);
    int32_t y = abs(data[0].y) * abs(data[0].y);
    int32_t z = abs(data[0].z) * abs(data[0].z);
  

    //tab of chars to print the results on the watch
    static char results[60];
  
    //Print the results in the LOG
    //APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%lu",mag);
    APP_LOG(APP_LOG_LEVEL_INFO, "x2 : %lu, y2 : %lu,\nz2 : %lu\n, mag %lu",x, y, z, mag); //ancien code pour afficher juste les coordonnées
    
    //Print the results on the watch
    //snprintf(results, 60, "Magnitude : \n%lu",mag);  
    snprintf(results, 60, "x2 : %lu, y2 : %lu,\nz2 : %lu\n, mag %lu",x, y, z, mag);  //ancien code pour afficher juste les coordonnées
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





int main(void) {
    init();
    app_event_loop();
    deinit();
}

