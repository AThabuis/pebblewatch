/*---------------------------------------------------------------------------

Authors: Adrien Thabuis & Antoine Laurens & J.F. B.
Version: 1.0
Date: 10.2016

user_interface.h

---------------------------------------------------------------------------*/

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

void open_main_window(void);
void close_main_window(void);
void open_reset_window(void);
void close_reset_window(void);
void open_user_height_window(void);
void close_user_height_window(void);
void open_step_display_window(void);
void close_step_display_window(void);

void update_number_steps_display(uint16_t);

enum User_height {H_M, H_CM};

#endif