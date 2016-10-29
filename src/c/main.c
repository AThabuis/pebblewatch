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
#include "variables.h"
#include "fourier.h"
#include "step_frequency.h"


// Declare the main window and two text layers
Window *main_window;
Window *text_window;
Window *user_height_window;

MenuLayer *main_menu_layer;
TextLayer *text_layer;
TextLayer *height_m;
TextLayer *height_cm;
TextLayer *background_layer;
TextLayer *pedometer_layer;

short mag_128[128] = {0};     // Tableau des magnitudes 
short gbloc = 0;            // point de départ du tableau
short fi[128];
short fr[128];

void open_main_window(void);
void close_main_window(void);
void open_text_window(char *text);
void close_text_window(void);
void open_user_height_window(void);
void close_user_height_window(void);

int user_height_m  = 1;
int user_height_cm = 70;
int user_min_height = 70;

enum Menu_title {START_COUNT, CHANGE_HEIGHT, RESET};
void update_user_height_display(void)
{
    if (user_height_cm < 10)
    {
      APP_LOG(APP_LOG_LEVEL_INFO, "Size:%dm0%d\n",user_height_m,user_height_cm);
    }
    else
    {
      APP_LOG(APP_LOG_LEVEL_INFO, "Size:%dm%d\n",user_height_m,user_height_cm);
    }
    static char m[5];
    static char cm[5];
  
    snprintf(m, 5, "%dm",user_height_m);
    snprintf(cm, 5, "%d",user_height_cm);
  
    text_layer_set_text(height_m, m);
    text_layer_set_text(height_cm, cm);
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
  Window *window = (Window *)context;
  
    if (user_height_m*100+user_height_cm <= user_min_height)
    {
        return;  
    }
    if(user_height_m >= 0 && user_height_cm-5 < 0)
    {
        if(user_height_m == 0)
        {
            user_height_cm = 0;
        }
        else
        {
          user_height_cm = 95;
          user_height_m--;
        }
    }
    else
    {
        user_height_cm -= 5;  
    }
    update_user_height_display();
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    window_stack_push(user_height_window, false);
    window_stack_push(main_window, true);
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
  Window *window = (Window *)context;
  
  if(user_height_m!= 3 && user_height_cm+5 >= 100)
    {
        if(user_height_m == 2)
        {
            user_height_cm = 95;
        }
        else
        {
          user_height_cm = 0;
          user_height_m++;
        }
    }
  else
  {
      user_height_cm += 5;
  }
    update_user_height_display();
}

void config_provider(Window *window) {
  // single click
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}

uint16_t get_num_rows_callback( MenuLayer *menu_layer, 
                                uint16_t section_index,
                                void *context)
{
  const uint16_t num_rows = 5;
  return num_rows;
}

void draw_row_callback( GContext *ctx, const Layer *cell_layer, 
                        MenuIndex *cell_index,
                        void *context)
{
  static char s_buff[16];
  
  switch (cell_index->row)
  {
      case START_COUNT:
          snprintf(s_buff, sizeof(s_buff), "Start");
          break;
      
      case CHANGE_HEIGHT:
          snprintf(s_buff, sizeof(s_buff), "Change height");
          break;
    
      case RESET:
          snprintf(s_buff, sizeof(s_buff), "Reset");
          break;
    
      default:
          snprintf(s_buff, sizeof(s_buff), "Row %d", (int)cell_index->row);
          break;
  }

  // Draw this row's index
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);
}

int16_t get_cell_height_callback( struct MenuLayer *menu_layer,
                                  MenuIndex *cell_index,
                                  void *context)
{
  const int16_t cell_height = 44;
  return cell_height;
}

void select_callback( struct MenuLayer *menu_layer,
                      MenuIndex *cell_index,
                      void *context)
{
  // Do something in response to the button press
  switch (cell_index->row)
  {
    case START_COUNT:
      APP_LOG(APP_LOG_LEVEL_INFO, "Index 0\n");
      open_text_window("Start counting");
      break;
    case CHANGE_HEIGHT:
      APP_LOG(APP_LOG_LEVEL_INFO, "Index 1\n");
      open_user_height_window(); 
      break;
    case RESET:
      APP_LOG(APP_LOG_LEVEL_INFO, "Index 2\n");
      open_text_window("Reset");
      break;
    case 3:
      APP_LOG(APP_LOG_LEVEL_INFO, "Index 3\n");
      open_text_window("Bon....");
      break;
    case 4:
      APP_LOG(APP_LOG_LEVEL_INFO, "Index 4\n");
      open_text_window("SUpEr");
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_INFO, "Default\n");
      break;
  }

}



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
    // Create background Layer
    //background_layer = text_layer_create(GRect( 0, 0, 144, 168));
    // Setup background layer color (black)
    //text_layer_set_background_color(background_layer, GColorClear);
  
    // Create text Layer
    //helloWorld_layer = text_layer_create(GRect( 20, 65, 100, 20));
    // Create the TextLayer with specific bounds
    //pedometer_layer = text_layer_create(
    //  GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
    
    // Setup layer Information
    //text_layer_set_background_color(pedometer_layer, GColorClear);
    //text_layer_set_text_color(pedometer_layer, GColorWhite);  
    //text_layer_set_font(pedometer_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    //text_layer_set_text_alignment(pedometer_layer, GTextAlignmentCenter);

    // Add layers as childs layers to the Window's root layer
    // layer_add_child(window_layer, text_layer_get_layer(background_layer));
    // layer_add_child(window_layer, text_layer_get_layer(pedometer_layer));
    
    // Add a logging meassage (for debug)
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Just write my first app!");
    
    //uint32_t num_samples = 25;
  
    //Allow accelerometer event
    //accel_data_service_subscribe(num_samples, accel_data_handler);
    
    //Define accelerometer sampling rate
    //accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
    open_main_window();
    APP_LOG(APP_LOG_LEVEL_INFO, "Init finished\n"); 
  
  //Timer
    //app_timer_register(T_CALL_ST*100, step_callback, NULL);
    //app_timer_register(1000, fft_callback, NULL);
  
    //layer_set_hidden(text_layer_get_layer(pedometer_layer), true);
}






// deinit function called when the app is closed
static void deinit(void) 
{
    APP_LOG(APP_LOG_LEVEL_INFO, "The END\n"); 
  
    //Stop Accelerometer
    //accel_data_service_unsubscribe();

    // Destroy layers and main window
    //text_layer_destroy(background_layer);
    //text_layer_destroy(pedometer_layer);
  
    close_user_height_window();
    close_text_window();
    close_main_window();
}

void open_text_window(char *text)
{
  
    // Create background Layer
    background_layer = text_layer_create(GRect( 0, 0, 144, 168));
    // Setup background layer color (black)
    text_layer_set_background_color(background_layer, GColorBlack);
  
    // Create main Window element and assign to pointer
    text_window = window_create();
    Layer *window_layer = window_get_root_layer(text_window);  
    GRect bounds = layer_get_bounds(window_layer);
  
    // Create the TextLayer with specific bounds
    text_layer = text_layer_create(GRect(0,
                                         PBL_IF_ROUND_ELSE(58, 52),
                                         bounds.size.w,
                                         50)
                                  );
  
    // Setup layer Information
    text_layer_set_background_color(text_layer, GColorClear);
    text_layer_set_text_color(text_layer, GColorWhite); 
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
    // Add to the Window
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
    layer_add_child(window_layer, text_layer_get_layer(text_layer));
    
    // Add text
    text_layer_set_text(text_layer, text);
  
    // Show the window on the watch, with animated = true
    window_stack_push(text_window, true);
    return;
}

void close_text_window(void)
{
    // Destroy the MenuLayer
    text_layer_destroy(text_layer);
    text_layer_destroy(background_layer);
  
    window_destroy(text_window);
  
    return;
}

void open_user_height_window(void)
{
    
    // Create main Window element and assign to pointer
    user_height_window = window_create();
    Layer *window_layer = window_get_root_layer(user_height_window);  
    GRect bounds = layer_get_bounds(window_layer);
  
    // Create background Layer
    background_layer = text_layer_create(GRect( 0, 0, bounds.size.w, bounds.size.h));
    // Setup background layer color (black)
    text_layer_set_background_color(background_layer, GColorWhite);
  
    // Create the TextLayer with specific bounds
    int padding = 5;
    int sqr_w = (bounds.size.w-2*padding);
    int sqr_h = bounds.size.h/3;
    height_m  = text_layer_create(GRect(padding,padding,sqr_w,sqr_h));
    height_cm = text_layer_create(GRect(padding,2*padding+sqr_h,sqr_w,sqr_h));
  
    // Setup layer Information
    text_layer_set_background_color(height_m, GColorBlack);
    text_layer_set_text_color(height_m, GColorWhite); 
    text_layer_set_font(height_m, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(height_m, GTextAlignmentCenter);
  
    text_layer_set_background_color(height_cm, GColorBlack);
    text_layer_set_text_color(height_cm, GColorWhite);  
    text_layer_set_font(height_cm, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(height_cm, GTextAlignmentCenter);
  
    // Add to the Window
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
    layer_add_child(window_layer, text_layer_get_layer(height_m));
    layer_add_child(window_layer, text_layer_get_layer(height_cm));
    
    // Add text
    update_user_height_display();
  
    window_set_click_config_provider(user_height_window, (ClickConfigProvider) config_provider);
  
    // Show the window on the watch, with animated = true
    window_stack_push(user_height_window, true);
                        
    return;
}
                        
void close_user_height_window()
{
    // Destroy the MenuLayer
    text_layer_destroy(height_m);
    text_layer_destroy(height_cm);
    text_layer_destroy(background_layer);
  
    window_destroy(user_height_window);
  
    return;
}

void open_main_window(void)
{
    // Create main Window element and assign to pointer
    main_window = window_create();
    Layer *window_layer = window_get_root_layer(main_window);  
    GRect bounds = layer_get_bounds(window_layer);
  
    // Create the MenuLayer
    main_menu_layer = menu_layer_create(bounds);

    // Let it receive click events
    menu_layer_set_click_config_onto_window(main_menu_layer, main_window);

    // Set the callbacks for behavior and rendering
    menu_layer_set_callbacks(main_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = get_num_rows_callback,
    .draw_row = draw_row_callback,
    .get_cell_height = get_cell_height_callback,
    .select_click = select_callback,
    });

    // Add to the Window
    layer_add_child(window_layer, menu_layer_get_layer(main_menu_layer));
  
    // Show the window on the watch, with animated = true
    window_stack_push(main_window, true);
    return;
}

void close_main_window(void)
{
    // Destroy the MenuLayer
    menu_layer_destroy(main_menu_layer);
    window_destroy(main_window);
  
    return;
}

int main(void) 
{
    init();
    app_event_loop();
    deinit();
}

