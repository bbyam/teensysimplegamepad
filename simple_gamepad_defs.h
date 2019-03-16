/* Teensy Simple Gamepad
 * A Gamepad device with one 2-axis D-pad and 1 or more buttons
 * Copyright (C) 2013 Robert Byam, robertbyam.com
 *
 * Derived from keyboard and serial examples for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * http://www.pjrc.com/teensy/usb_serial.html
 * Copyright (c) 2008 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* ===========================================================================
   This software implements a generic gamepad with one 2-axis (four direction)
   D-Pad and 1 or more button.  The intended usage is that all of these files
   remain constant, and only the file simple_gamepad_config.h is modified to
   specify the configurable parameters for the gamepad

   simple_gamepad_defs.h
   This file builds the common structures from the configuration specified in
   simple_gamepad_config.h. This file contains all the definitions and
   functions that rely on the settings found in simple_gamepad_config.h
   ======================================================================== */

#ifndef SIMPLE_GAMEPAD_DEF_INTERNAL_H
#define SIMPLE_GAMEPAD_DEF_INTERNAL_H

/* get the gamepad configuration parameters */
#include "simple_gamepad_config.h"

#include <stdint.h>
#include <avr/pgmspace.h>

/* these funtions are used by the main program to perform the basic operations */

/* this function configures the hardware for the desired usage */
void simple_gamepad_configure(void);
/* this function reads the gamepad state from the hardware */
void simple_gampad_read_buttons(void);
/* this function transmits the state report */
int8_t usb_simple_gamepad_send(void);


/* button array, 1 bit for each button */
#define BUTTON_ARRAY_SIZE ((BUTTON_COUNT + 7) / 8)

typedef struct
{
    /* x and y axis */
    uint8_t x_axis;
    uint8_t y_axis;

    /* the buttons - one bit for each */
    uint8_t buttons[BUTTON_ARRAY_SIZE];

} gamepad_state;

extern gamepad_state g_gamepadState;

// these are used to set the axis values
#define AXIS_CENTER     ((uint8_t)0x80)
#define X_AXIS_LEFT     ((uint8_t)0x00)
#define X_AXIS_RIGHT    ((uint8_t)0xFF)
#define Y_AXIS_UP       ((uint8_t)0x00)
#define Y_AXIS_DOWN     ((uint8_t)0xFF)


#if (BUTTON_COUNT % 8) == 0
#define PADDING_BITS 0
#else
#define PADDING_BITS (8 - (BUTTON_COUNT % 8))
#endif

/* define the HID report */
static const uint8_t PROGMEM gamepad_hid_report_desc[] = {
    0x05, 0x01,         // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,         // USAGE (Game Pad)
    0xa1, 0x01,         // COLLECTION (Application)
    0xa1, 0x00,         //   COLLECTION (Physical)
    0x09, 0x30,         //     USAGE (X)
    0x09, 0x31,         //     USAGE (Y)
    0x15, 0x00,         //     LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,   //     LOGICAL_MAXIMUM (255)
    0x95, 0x02,         //     REPORT_COUNT (2)
    0x75, 0x08,         //     REPORT_SIZE (8)
    0x81, 0x02,         //     INPUT (Data,Var,Abs)
    0x05, 0x09,         //     USAGE_PAGE (Button)
    0x19, 0x01,         //     USAGE_MINIMUM (Button 1)
    0x29, BUTTON_COUNT, //     USAGE_MAXIMUM (Button N)
    0x15, 0x00,         //     LOGICAL_MINIMUM (0)
    0x25, 0x01,         //     LOGICAL_MAXIMUM (1)
    0x95, BUTTON_COUNT, //     REPORT_COUNT (Number of Buttons)
    0x75, 0x01,         //     REPORT_SIZE (1)
    0x81, 0x02,         //     INPUT (Data,Var,Abs)
#if PADDING_BITS != 0
    0x95, PADDING_BITS, //     REPORT_COUNT (Padding bits to fit to uint8_t)
    0x75, 0x01,         //     REPORT_SIZE (1)
    0x81, 0x03,         //     INPUT (Cnst,Var,Abs)
#endif
    0xc0,               //   END_COLLECTION
    0xc0                // END_COLLECTION
};



#endif /* SIMPLE_GAMEPAD_DEF_INTERNAL_H */




