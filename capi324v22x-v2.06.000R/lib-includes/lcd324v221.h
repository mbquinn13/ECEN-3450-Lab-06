/*
 *  This source code is part of the CEENBoT-API.
 *
 *  Copyright (C) 2011-2017 - University of Nebraska Board of Regents.
 *  Written and Created by: Jose Santos <ceenbot.api@digital-brain.info>
 *
 *  The CEENBoT-API is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Auth: Jose Santos
// Desc: Header file for the LCD driver module for the 324v221 board.  It pro-
//       vides the functions/resources for interfacing with the onboard LCD
//       display.

#ifndef __LCD324V221_H__
#define __LCD324V221_H__

#define __LCD324MODULE__

#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "utils324v221.h"
#include "sys324v221.h"
#include "spi324v221.h"
#include "tiny324v221.h"
#include "spkr324v221.h"

// ============================= defines ==================================== //
#ifndef __NO_LCD_PRINTF

    #define LCD_printf  printf
    #define LCD_printf_RC( r, c, ... ) \
        do{ LCD_set_RC( r, c ); LCD_printf( __VA_ARGS__ ); }while(0)


    #define LCD_printf_PGM printf_P
    #define LCD_printf_RC_PGM( r, c, ... ) \
        do{ LCD_set_RC( r, c ); LCD_printf_PGM( __VA_ARGS__ ); }while(0)

#else

    #define LCD_printf( ... )
    #define LCD_printf_RC( ... )

#endif /* __NO_LCD_PRINTF */

// These macros are used for 'command' formation for configuring and
// controlling the LCD display.  All such commands are prefixed with
// 'LCDCMD'.
#define LCDCMD_COL_ADDR_H   0x10        /* High-nybble column LCD address.   */
#define LCDCMD_COL_ADDR_L   0x00        /* Low-nybble column LCD address.    */ 
#define LCDCMD_SET_PG_ADDR  0xB0        /* Command to set page address.      */
#define LCDCMD_DRAM_START_ADDR  0x40    /* Display RAM 'scan' start address. */

// LCD display 'metrics'.
#define LCD_PIX_WIDTH       128     /* LCD Pixel width.     */
#define LCD_PIX_HEIGHT      32      /* LCD Pixel height.    */
#define LCD_PAGE_PIXHEIGHT  8       /* Page width in pixels.*/

// Character/Line parameters.
#define LCD_nCOLS           21
#define LCD_nPAGES          4


// Desc: This macro issued to control the 'A0' line on the LCD (command/data).
//       CLEAR (or '0') for 'command' and SET (or '1') for 'data'.
#define LCD_A0( state )     SCBV( state, PB3, PORTB )

#define LCD_write_cmd( cmd )    LCD_write( cmd, LCD_CMD )
#define LCD_write_data( data )  LCD_write( data, LCD_DATA )

// Macros for extracting 'nibbles' (4-bits) from 'byte-wide data'.
#define LCD_LO_NIBBLE( byte )    ( 0x0F & ( byte ) )
#define LCD_HI_NIBBLE( byte )    ( 0x0F & (( byte ) >> 4 ) )    

// Macro for setting the last error.
// #define LCD_SET_LAST_ERROR( error )  ( LCD_lastError = ( error ) )

// *** Backlight ***
#define LCD_MIN_BACKLIGHT   0
#define LCD_MAX_BACKLIGHT   31
#define LCD_FLASH_BACKLIGHT 32

// *** Font Related ***
#define LCD_MAXPG_HCHARS    21  /* Maximum horizontal characters per page. */
#define LCD_MAXPG_VCHARS    1   /* Maximum vertical characters per page.   */
#define LCD_FONTPIXWIDTH    5   /* Font width in pixels.                   */
#define LCD_FONTPIXHEIGHT   7   /* Font height in pixels.                  */

// Special 'bitmap characters' in the Character Table.
#define BATT_0BAR		0x80    // battery 0%
#define BATT_1BAR		0x81    // battery 17%
#define BATT_2BAR		0x82    // battery 33%
#define BATT_3BAR		0x83    // battery 50%
#define BATT_4BAR		0x84    // battery 67%
#define BATT_5BAR		0x85    // battery 83%
#define BATT_6BAR		0x86    // battery 100%
#define BATT_CHG 		0x87    // battery charging (up arrow)
#define BATT_AC 		0x88    // AC powered (small AC)
#define BATT_NEEDCHG	0x89    // battery needs charging (double !!)

// ====================== global type declarations ========================== //
// Custom type used with 'LCD_write()' function to designate data category.
typedef enum LCD_DTYPE_TYPE  
{ 

    LCD_CMD = 0, 
    LCD_DATA 

}    LCD_DTYPE;

typedef enum LCD_SUBSYS_TYPE 
{ 

    LCD_CLOSED = 0, 
    LCD_OPEN 

} LCD_SUBSYS;

typedef struct LCD_COORD_TYPE {

    unsigned char col;    // Holds an LCD column address ( 0 - 127 ).
    unsigned char row;    // Holds an LCD page address ( 0=Bottom to 3=Top )

} LCD_COORD;

// Function prototype declaration for declaring an 'lcd change notify' 
// function that can then be registered via 'LCD_register_lcd_change_notify()'
// function.
typedef void ( *LCD_CHANGE_NOTIFY_FUNC_PTR )( void );

typedef struct LCD_PARAMS_TYPE {

    volatile unsigned char curr_col;    // Keeps track of the LCD's current
                                        // pixel column address ( 0 - 127 ).

    volatile unsigned char curr_page;   // Keeps track of the LCD's current
                                        // page. ( 3 = top, 0 = bottom ).

    BOOL lcd_change_notify;             // 'TRUE' if the user has registered
                                        // an 'lcd change notify' function.

    LCD_CHANGE_NOTIFY_FUNC_PTR p_change_notify_func;

} LCD_PARAMS;

// ============================ prototypes ================================== //
// Input Args: None.
// Returns: A value of type 'SUBSYS_STATUS', which will be one of the
//          following enumerated constants:
//
//              'SUBSYS_CLOSED' - Indicates the subsystm is closed.
//
//              'SUBSYS_OPEN'   - The subsystm is successfully open and can
//                                now be used.
//
//              'SUBSYS_ALREADY_OPEN' - Indicates the subsystem has already
//                                      been previously opened and initialized
//                                      by another client (subsystem).
//
//              'SUBSYS_IN_USE' - Indicates the subsystem [cannot be closed]
//                                because it is being used, or there is a
//                                dependency that needs to be met.
//
//              'SUBSYS_NOT_AVAILABLE' - Attempting to open a subsystem whose
//                                       resource is not available.
//
//              'SUBSYS_ERROR' - Generic unknown error.
//
//              'SUBSYS_INIT_FAILED' - It means things started out good, but
//                                     something happened along the way (i.e.,
//                                     hardware dependency not responding).
//
//              'SUBSYS_DEPENDENCY_ERROR' - Could not open because a dependency
//                                          (on another subsystem) could not be
//                                          met b/c it is not available or does
//                                          not exist.
//
//              'SUBSYS_DEPENDENCY_CONFLICT' - Could not open because an
//                                             UNSHARABLE resource needed by
//                                             the subsystem is already in use.
//
//          Generally speaking, users are normally interested (and should
//          just check) if 'SUBSYS_OPEN' or 'SUBSYS_ALREADY_OPEN' was returned
//          while all others should be treated as errors.
//
// Desc: Initializes the LCD and check that other resources (such as SPI) are
//       available for use.  The function ensures of acquiring other
//       dependencies or ensuring that these dependencies are avaible.
//       Specifically, it ensures the SPI subsystem is open as the LCD is
//       an SPI device and that the TINY subsystem (which is in control of
//       the LCD's backlight) is also available.  The user does NOT need to
//       independently call 'SPI_open()' nor 'ATTINY_open()'.
//
//       Users should at least check to see if the function returns 
//       'SUBSYS_OPEN' or 'SUBSYS_ALREADY_OPEN' and proceed accordingly.  The
//       rest of the return status values should be treated as errors and
//       are mainly of interest during debugging, or for debugging purposes.
//
//       The typical 'open' sequence might look like:
//
//          SUBSYS_STATUS opstat;
//
//          // Attempt to open the subsystem in question.
//          opstat = XXXXX_open();
//
//          // Proceed only IF successfully opened.
//          if( ( opstat == SUBSYS_OPEN ) || ( opstat == SUBSYS_ALREADY_OPEN ) )
//          {
//                  /* Your code here */
//          }
//          else
//          {
//                  /* Spit out an error message, or somethin' */
//          }
//
//       It is up to the user to determine if only 'SUBSYS_OPEN' is necessary
//       and whether or not a 'SUBSYS_ALREADY_OPEN' is permissible.  If the
//       user requires exclusive use of a subsystem module, then he/she must
//       want to be the 'first' to open the device.  At this point, then, 
//       the user will want to make sure only 'SUBYS_OPEN' is returned and
//       treat everything else (even 'SUBSYS_ALREADY_OPEN') as an error.
//
extern SUBSYS_STATUS LCD_open( void );

// -------------------------------------------------------------------------- //
// TODO
// Desc: Stop the LCD and release resources.
extern void LCD_close( void );
// -------------------------------------------------------------------------- //
// Input  Args: 'data' - Data to send to LCD.
//              'dtype' - Must be one of the following enumerated constants:
//                          'LCD_CMD', 'LCD_DATA'.
// Output Args: None.
// Globals Write: None.
// Globals  Read: None.
// Returns: Nothing.
//
// Desc: TODO
extern void LCD_write( unsigned char data, LCD_DTYPE dtype );

// -------------------------------------------------------------------------- //
// Input  Args: None.
// Output Args: None.
// Globals Write: None.
// Globals  Read: None.
// Returns: Nothing.
//
// Desc: TODO
extern void LCD_clear( void );

// -------------------------------------------------------------------------- //
// Input Args:
// Output Args:
// Globals Write: 'LCD_params'.
// Globals  Read: 'LCD_params'.
// Returns: 0 if output was successful, non-zero otherwise.
//
// Desc:  This function is a special 'handler function' internally called
//        by the standard output stream used in redirection of the output
//        stream.  It is eventually called indirectly when 'printf()' is
//        used.  It shold NOT be called by the user.  Users should use
//        'LCD_putchar' (no preceeding underscores) to achieve the same
//        task.
// static int __LCD_putchar( char c, FILE *stream );

// -------------------------------------------------------------------------- //
// Input  Args: 'c' - The ASCII character to be written.
// Output Args: None.
// Globals  Read: None.
// Globals Write: None.
// Returns: Nothing.
// Desc: Function writes a character 'c' on the LCD at the next 'location'
//       waiting to be written.  This could be where the current marker location
//       left off.  A user can override the 'next location' to be written by
//       invoking 'LCD_set_next_PGC()' to set the next Page and Column 
//       addresses.
extern void LCD_putchar( char c );

// -------------------------------------------------------------------------- //
// Desc: TODO
// static void LCD_copy_font_bitmap( char c );
// -------------------------------------------------------------------------- //
// Desc: Allows you to display a character 'c' at the designated x-position and
//       page-position.
extern 
    void LCD_putchar_XPG( unsigned char xpix_pos, unsigned char page, char c );
// -------------------------------------------------------------------------- //
// Desc: This function allows you to display the character 'c' at the designated
//       rolw and column position.
extern void LCD_putchar_RC( unsigned char row, unsigned char col, char c );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function allows you to set the next 'Row/Column' character address
//       to write to.
extern void LCD_set_RC( unsigned char row, unsigned char col );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function returns the current 'row/col' address that will be written to
//       on the next 'write' by any of the character printing functions.  This
//       function is provided to allow the 'saving' of this information when
//       you need temporarily write to some other location on the LCD but you
//       don't wish to disturb the current cursor position.  For example:
//
//          LCD_COORD current_coords;
//
//          // Save it current coordinates.
//          current_coords = LCD_get_RC();
//
//          // ... do whatever you need to do ...
//
//          // Restore original coordinates.
//          LCD_set_RC( current_coords.row, current_coords.col );
//
//      That's it.
//
extern LCD_COORD LCD_get_RC( void );
// -------------------------------------------------------------------------- //
// Input  Args: TODO
// Output Args:
// Globals  Read:
// Globals Write:
// Returns:
//
// Desc: This function is an 'accessor' function for manipulating the
//       internal LCD global parameters.  It sets the 'next location' for
//       any subsequent writes to the LCD.  Note that this function is the 
//       'end-user' version of 'LCD_set_PGC_addr()'.  That function is an 
//       internal function which modifies the LCD parameters directly.  THIS 
//       function, in turn, only modifies the variables that simply hold the 
//       current status (i.e., the LCD itself is not modified).
extern 
  void LCD_set_next_PGC( unsigned char nextPageNum, unsigned char nextColAddr );
// -------------------------------------------------------------------------- //
// Input  Args: 'BL_level' - The backlight level.  Must be restricted to values
//                           in the range of 0-32, with the following meanings:
//                              0  = Backlight OFF.
//                              31 = Backlight FULL ON.
//                              32 = Backlight FLASH FOR CHARGE.
//
//                           Values BETWEEN 0 and 32 are provide intermediate
//                           intensity levels.
// Output Args: None.
// Globals  Read: None.
// Globals Write: None.
// Returns: Nothing.
//
// Desc: This function allows the backlight intensity level to be controlled.
//       It does this by telling the ATtiny (which is in control of the LCD's
//       backlight levels) to modulate the PWM accordingly.
extern void LCD_set_backlight( unsigned char BL_level );
// -------------------------------------------------------------------------- //
// Input Args: 'p_change_notify_func' - The address of a function of type
//                                      'LCD_CHANGE_NOTIFY_FUNC_PTR' must be 
//                                      provided to this argument.  It has the
//                                      form of:
//
//                                          void (*notify)( void )
// Output Args: None.
// Globals Read:  None.
// Globals Write: None.
// Returns: Nothing.
//
// Desc: This function allows you to register a callback function that gets
//       notified whenever the contents of the LCD display are modified.  This
//       is useful, for example, for drawing icons at fixed locations on the LCD
//       which must be redrawn whenever the contents are modified, thus allowing
//       the user the opportunity to re-draw any fixed-content graphics.
extern void LCD_register_lcd_change_notify( 
    LCD_CHANGE_NOTIFY_FUNC_PTR p_change_notify_func );
// -------------------------------------------------------------------------- //
// Input Args: 'p_xbm_data' - Pointer to linear array containing XBM data.
//
// Returns: Nothing.
//
// Desc: Function displays the image represented by the xbm formatted data in
//       in the linear array and displays it on the LCD.  This is sort of an
//       experimental feature for the time being.  Note that the row/col loc-
//       cations will be reset to their origins after completion of this 
//       function (equivalent to an 'LCD_clear()' effect).
//
//       When creating the XBM image, it _must_ be formatted in the following
//       manner before saving:
//
//          * Image size _must_ be 128x32 pixels.
//          * Rotate the finished image 90-degrees clockwise.
//          * Apply a HORIZONTAL flip.
//          * Save.
//          * Copy and paste the XBM data into your source code and make sure
//            to modify the array declaration to consist of the following
//            form:
//
//              const char <array_name> [] PROGMEM = { ...data... };
//
//            In particular, note the 'PROGMEM' declaration, which results in
//            the xbm data being stored in Program Memory instead of SRAM.  This
//            is a _mandatory_ step.
//
extern void LCD_draw_xbm( const char *p_xbm_data );

// ==================== external global declarations ======================== //
extern LCD_PARAMS LCD_params;
extern FILE       LCD_stdout;
extern const char char_bitmap[] PROGMEM;
extern const char CEENBoT_Inc_logo_bitmap[] PROGMEM;


#endif /* __LCD324V221_H__ */

