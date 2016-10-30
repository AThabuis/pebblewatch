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


// deinit function called when the app is closed
static void deinit(void) 
{
    APP_LOG(APP_LOG_LEVEL_INFO, "The END\n"); 
  
    //Stop Accelerometer
    accel_data_service_unsubscribe();

    // Destroy layers and main window
    close_user_height_window();
    close_step_display_window();
    close_text_window();
    close_main_window();
}

int main(void) 
{
    open_main_window();
    app_event_loop();
    deinit();
}




