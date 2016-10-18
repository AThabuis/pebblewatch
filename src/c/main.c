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
TextLayer *first_layer;
TextLayer *second_layer;
BitmapLayer *batman_layer;
static GBitmap* batman;



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
    //static uint32_t mag = 0;
    //static uint32_t mag1 = 0;
    //static uint32_t mag2 = 0;

    //mobile_mean_accel(data,num_samples,&mag);
    //mag += 1;
    //mag1 = mag % 60;
    //mag2 = mag / 60;
  
    //ancien code pour afficher juste les coordonnées au carré
    //Read samples 0's x, y and z values
    /* int32_t x = abs(data[0].x) * abs(data[0].x);
    int32_t y = abs(data[0].y) * abs(data[0].y);
    int32_t z = abs(data[0].z) * abs(data[0].z); */
  

    //tab of chars to print the results on the watch
    //static char results[60];
  
    //Print the results in the LOG
    //APP_LOG(APP_LOG_LEVEL_INFO, "Magnitude : \n%lu",mag);
    //APP_LOG(APP_LOG_LEVEL_INFO, "x2 : %lu, y2 : %lu,\nz2 : %lu\n, mag %lu",x, y, z, mag); //ancien code pour afficher juste les coordonnées
    
    //Print the results on the watch
    //snprintf(results, 60, "%lu:%lu",mag2,mag1);
    //snprintf(results, 60, "x2 : %lu, y2 : %lu,\nz2 : %lu\n, mag %lu",x, y, z, mag);  //ancien code pour afficher juste les coordonnées
    //text_layer_set_text(first_layer, results);
}

  static void select_click_handler_select(ClickRecognizerRef recognizer, void *context) {
    // A single click has just occured
    APP_LOG(APP_LOG_LEVEL_INFO, "Select");
    
    layer_set_hidden(text_layer_get_layer(background_layer), true);
    layer_set_hidden(text_layer_get_layer(first_layer), true);
    layer_set_hidden(text_layer_get_layer(second_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(batman_layer), false);
  }
  static void select_click_handler_up(ClickRecognizerRef recognizer, void *context) {
    // A single click has just occured
    APP_LOG(APP_LOG_LEVEL_INFO, "Up");
    
    text_layer_set_background_color(background_layer, GColorBlack);
    layer_set_hidden(text_layer_get_layer(background_layer), false);
    layer_set_hidden(text_layer_get_layer(first_layer), false);
    layer_set_hidden(text_layer_get_layer(second_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(batman_layer), true);

  }
  static void select_click_handler_down(ClickRecognizerRef recognizer, void *context) {
    // A single click has just occured
    APP_LOG(APP_LOG_LEVEL_INFO, "Down");
    
    text_layer_set_background_color(background_layer, GColorWhite);
    layer_set_hidden(text_layer_get_layer(background_layer), false);
    layer_set_hidden(text_layer_get_layer(first_layer), true);
    layer_set_hidden(text_layer_get_layer(second_layer), false);
    layer_set_hidden(bitmap_layer_get_layer(batman_layer), true);
  }

  static void click_config_provider(void *context) {
    // Subcribe to button click events here
    ButtonId id_select = BUTTON_ID_SELECT;  // The Select button
    ButtonId id_up   = BUTTON_ID_UP;  // The Up button
    ButtonId id_down = BUTTON_ID_DOWN;  // The Down button

    window_single_click_subscribe(id_select, select_click_handler_select);
    window_single_click_subscribe(id_up, select_click_handler_up);
    window_single_click_subscribe(id_down, select_click_handler_down);
  }

// Init function called when app is launched
static void init(void) {

  	// Create main Window element and assign to pointer
  	main_window = window_create();
    Layer *window_layer = window_get_root_layer(main_window);  
    GRect bounds = layer_get_bounds(window_layer);
  
    // Use this provider to add button click subscriptions
    window_set_click_config_provider(main_window, click_config_provider);


		// Create background Layer
		background_layer = text_layer_create(GRect( 0, 0, 144, 168));
		// Setup background layer color (black)
		text_layer_set_background_color(background_layer, GColorBlack);

  
  
		// Create text Layer
		//helloWorld_layer = text_layer_create(GRect( 20, 65, 100, 20));
    // Create the TextLayer with specific bounds
    first_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
    second_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
    batman_layer = bitmap_layer_create(
      GRect(5,5,48,48));
    bitmap_layer_set_compositing_mode(batman_layer, GCompOpSet);
    bitmap_layer_set_bitmap(batman_layer,batman);
  
		// Setup layer Information
		text_layer_set_background_color(first_layer, GColorClear);
		text_layer_set_text_color(first_layer, GColorWhite);	
		text_layer_set_font(first_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  	text_layer_set_text_alignment(first_layer, GTextAlignmentCenter);
  
  	// Setup layer Information
		text_layer_set_background_color(second_layer, GColorClear);
		text_layer_set_text_color(second_layer, GColorBlack);	
		text_layer_set_font(second_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  	text_layer_set_text_alignment(second_layer, GTextAlignmentCenter);
  
    bitmap_layer_set_background_color(batman_layer, GColorClear);

  	// Add layers as childs layers to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
	  layer_add_child(window_layer, text_layer_get_layer(first_layer));
    layer_add_child(window_layer, text_layer_get_layer(second_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(batman_layer));
  
    gbitmap_create_with_resource(RESOURCE_ID_BATMAN);
  
    char first[60] = "First";
    text_layer_set_text(first_layer, first);
    char second[60] = "Second";
    text_layer_set_text(second_layer, second);
    layer_set_hidden(text_layer_get_layer(second_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(batman_layer), true);

  
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
	  text_layer_destroy(first_layer);
    text_layer_destroy(second_layer);
    window_destroy(main_window);
}




int main(void) {
    init();
    app_event_loop();
    deinit();
}

