/*---------------------------------------------------------------------------

Authors:  Adrien Thabuis
          Antoine Laurens
          Jean-François Burnier
          Hugo Dupont
          Hugo Viard

Version: 1.0
Date: 11.2016

user_interface.h

---------------------------------------------------------------------------*/

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H


/*
 * \brief     Open the main window
 * \details   Call this function to open the main window displaying
 *            a menu (giving the ability to the user to browse between
 *            the different functionalities of the apps)
 */
void open_main_window(void);

/*
 * \brief     Close the main window
 * \details   Call this function to close the main window
 *            and all its layers         
 */
void close_main_window(void);

/*
 * \brief     Open the reset window
 * \details   Call this function to open a new window displaying
 *            a message saying that the app has been reset
 */
void open_reset_window(void);

/*
 * \brief     Close the reset window
 * \details   Call this function to close the reset window
 *            and all its layers and give the focus to the 
 *            main window
 */
void close_reset_window(void);

/*
 * \brief     Open the user height window
 * \details   Call this function to open a new window displaying
 *            the height of the user (the user may use this window
 *            to modify the height)
 */
void open_user_height_window(void);

/*
 * \brief     Close the user height window
 * \details   Call this function to close the height window
 *            and all its layers and give the focus to the 
 *            main window          
 */
void close_user_height_window(void);

/*
 * \brief     Open the step display window
 * \details   Call this function to open a new window displaying
 *            a steps and the distance traveled
 */
void open_step_display_window(void);

/*
 * \brief     Close the step display window
 * \details   Call this function to close the step display window
 *            and all its layers and give the focus to the 
 *            main window
 */
void close_step_display_window(void);

/*
 * \brief  Update number of step display
 */
void update_number_steps_display(uint16_t);

/*
 * \brief	Enum used to access the user height array
 * 
 *	H_M     To access the height in meters
 *  H_CM  	To access the height in centimeters
 */
enum User_height {H_M, H_CM};

#endif