/*---------------------------------------------------------------------------

Authors: Adrien Thabuis & Antoine Laurens & J.F. B.
Version: 1.0
Date: 10.2016

user_interface.h

---------------------------------------------------------------------------*/

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

const int* get_user_height(void);
void open_main_window(void);
void close_main_window(void);
void open_text_window(char *text);
void close_text_window(void);
void open_user_height_window(void);
void close_user_height_window(void);

enum User_height {M, CM};

#endif