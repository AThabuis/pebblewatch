#include <pebble.h>
#include "user_interface.h"
#include "step_frequency.h"
#include "acceleration_process.h"

// Declare the main window and two text layers
Window *main_window;
Window *text_window;
Window *step_display_window;
Window *user_height_window;

MenuLayer *main_menu_layer;
TextLayer *text_layer;
TextLayer *height_m_layer;
TextLayer *height_cm_layer;
TextLayer *steps_layer;
TextLayer *background_layer;

int user_height[2] = {1,70};

enum Menu_title {START_COUNT, CHANGE_HEIGHT, RESET};

void update_user_height_display(void);

short first_call = 1;


const int* get_user_height(void)
{
    return user_height;
}



void update_user_height_display(void)
{
    static char m[5];
    static char cm[5];
  
    snprintf(m, 5, "%dm",user_height[H_M]);
    
    if (user_height[H_CM] < 10)
    {
      snprintf(cm, 5, "0%d",user_height[H_CM]);
    }
    else
    {
      snprintf(cm, 5, "%d",user_height[H_CM]);
    }
  
    text_layer_set_text(height_m_layer, m);
    text_layer_set_text(height_cm_layer, cm);
}



void update_number_steps_display(const uint16_t steps_number)
{
    static char nb_steps_display[10];
    
    snprintf(nb_steps_display, 10, "Steps: %u\n", steps_number);
    text_layer_set_text(steps_layer, nb_steps_display);
}


void down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{  
    int user_min_height = 70;
  
    if (user_height[H_M]*100+user_height[H_CM] <= user_min_height)
    {
        return;  
    }
    if(user_height[H_M] >= 0 && user_height[H_CM]-5 < 0)
    {
        if(user_height[H_M] == 0)
        {
            user_height[H_CM] = 0;
        }
        else
        {
          user_height[H_CM] = 95;
          user_height[H_M]--;
        }
    }
    else
    {
        user_height[H_CM] -= 5;  
    }
    update_user_height_display();
}



void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    //window_stack_push(user_height_window, false);
    //window_stack_push(main_window, true);
    //close_user_height_window();
}



void up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if(user_height[H_M]!= 3 && user_height[H_CM]+5 >= 100)
    {
        if(user_height[H_M] == 2)
        {
            user_height[H_CM] = 95;
        }
        else
        {
          user_height[H_CM] = 0;
          user_height[H_M]++;
        }
    }
    else
    {
        user_height[H_CM] += 5;
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
  const uint16_t num_rows = 3;
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
      open_step_display_window();
      if(first_call == 1)
      {
        init_accel();
        first_call = 0;
      }
      break;
    case CHANGE_HEIGHT:
      APP_LOG(APP_LOG_LEVEL_INFO, "Index 1\n");
      open_user_height_window(); 
      break;
    case RESET:
      APP_LOG(APP_LOG_LEVEL_INFO, "Index 2\n");
      first_call = 1 ;//We can call again for the "first" time the step counter, accelerometer process functions etc ...
      reset_n_steps();
      accel_data_service_unsubscribe();//Stop Accelerometer
      open_text_window("Reset Done");
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_INFO, "Default\n");
      break;
  }

}



void open_text_window(char *text)
{
  
    // Create main Window element and assign to pointer
  	text_window = window_create();
    Layer *window_layer = window_get_root_layer(text_window);  
    GRect bounds = layer_get_bounds(window_layer);
  
    // Create background Layer
		background_layer = text_layer_create(GRect( 0, 0, bounds.size.w, bounds.size.h));
		// Setup background layer color (black)
		text_layer_set_background_color(background_layer, GColorBlack);
  
    // Create the TextLayer with specific bounds
    text_layer = text_layer_create(GRect( 0, 0, bounds.size.w, bounds.size.h));
  
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
    window_stack_push(text_window, false);
  
    window_destroy(text_window);
    return;
}

void open_step_display_window(void)
{
  
    // Create main Window element and assign to pointer
  	step_display_window = window_create();
    Layer *window_layer = window_get_root_layer(step_display_window);  
    GRect bounds = layer_get_bounds(window_layer);
  
    // Create background Layer
		background_layer = text_layer_create(GRect( 0, 0, bounds.size.w, bounds.size.h));
		// Setup background layer color (black)
		text_layer_set_background_color(background_layer, GColorBlack);
  
    // Create the TextLayer with specific bounds
    steps_layer = text_layer_create(GRect( 0, 0, bounds.size.w, bounds.size.h));
  
    // Setup layer Information
		text_layer_set_background_color(steps_layer, GColorClear);
		text_layer_set_text_color(steps_layer, GColorWhite);	
		text_layer_set_font(steps_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  	text_layer_set_text_alignment(steps_layer, GTextAlignmentCenter);
  
    // Add to the Window
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
    layer_add_child(window_layer, text_layer_get_layer(steps_layer));
  
    // Update number of step display
    uint16_t n_step = get_n_steps();
    update_number_steps_display(n_step);
  
    // Show the window on the watch, with animated = true
  	window_stack_push(step_display_window, true);
    return;
}


void close_step_display_window(void)
{
    // Destroy the MenuLayer
    text_layer_destroy(steps_layer);
    text_layer_destroy(background_layer);
    window_stack_push(step_display_window, false);
  
    window_destroy(step_display_window);
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
  
    int padding = 5; // Layer padding
    int sqr_w = (bounds.size.w-2*padding); // layer width
    int sqr_h = bounds.size.h/3; // layer height
    
    // Create the TextLayer with specific bounds
    height_m_layer  = text_layer_create(GRect(padding,padding,sqr_w,sqr_h));
    height_cm_layer = text_layer_create(GRect(padding,2*padding+sqr_h,sqr_w,sqr_h));
  
    // Setup layer Information
		text_layer_set_background_color(height_m_layer, GColorBlack);
		text_layer_set_text_color(height_m_layer, GColorWhite);	
		text_layer_set_font(height_m_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  	text_layer_set_text_alignment(height_m_layer, GTextAlignmentCenter);
  
    text_layer_set_background_color(height_cm_layer, GColorBlack);
		text_layer_set_text_color(height_cm_layer, GColorWhite);	
		text_layer_set_font(height_cm_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  	text_layer_set_text_alignment(height_cm_layer, GTextAlignmentCenter);
  
    // Add to the Window
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
    layer_add_child(window_layer, text_layer_get_layer(height_m_layer));
    layer_add_child(window_layer, text_layer_get_layer(height_cm_layer));
    
    // Update height display
    update_user_height_display();
  
    // Configure click callbacks
    window_set_click_config_provider(user_height_window, (ClickConfigProvider) config_provider);
  
    // Show the window on the watch, with animated = true
  	window_stack_push(user_height_window, true);
                        
    return;
}



void close_user_height_window()
{
    // Destroy the MenuLayer
    text_layer_destroy(height_m_layer);
    text_layer_destroy(height_cm_layer);
    text_layer_destroy(background_layer);
  
    window_stack_push(user_height_window, false);
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