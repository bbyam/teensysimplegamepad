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

   simple_gamepad_config.h
   This file should be modified to set the parameters of the gamepad.
   ======================================================================== */

#ifndef SIMPLE_GAMEPAD_DEF_H
#define SIMPLE_GAMEPAD_DEF_H

/*
    This file defines all the settings for the gamepad. To create a new gamepad,
    simply edit the settings in this file and recompile. The simple gamepad
    supports a single 2-axis directional pad with one or more buttons.

    These settings will create a gamepad exposing the following buttons to the
    OS, with the number of buttons specified by the settings. This ordering uses
    inputs along the long edges first, with the two on the short egde (D4, D5)
    and the interior button (E6) last.

        UP:     Port B0
        DOWN:   Port B1
        LEFT:   Port B2
        RIGHT:  Port B3

        BTN1:   Port B7
        BTN2:   Port D0
        BTN3:   Port D1
        BTN4:   Port D2
        BTN5:   Port D3
        BTN6:   Port C6
        BTN7:   Port C7
        BTN8:   Port D7
        BTN9:   Port B4
        BTN10:  Port B5
        BTN11:  Port B6
        BTN12:  Port F7
        BTN13:  Port F6
        BTN14:  Port F5
        BTN15:  Port F4
        BTN16:  Port F1
        BTN17:  Port F0
        BTN18:  Port D4
        BTN19:  Port D5
        BTN20:  Port E6

        All unsued buttons will be configured as outputs to save power (possible
        floating connections constantly changing state if left as inputs) and
        are available for other custom configuration/usage.

        Port D6, with the LED, is not used and will be configured as an output.
*/


/* ======================================================================== */
/* Modify these settings to configure the gamepad operation           */
/* ======================================================================== */

/* IMPORTANT: Read this before setting Manufacturer and Product name/ID!
   These settings specify the manufacturer and product names reported to the OS
   However, it is important to note that Windows will cache this information in
   the registry the first time the device is plugged in, and then use the cache
   from then on. The cache is based on the Product ID for the device. So if you
   attempt to change another value, like the product string, Windows will use
   cache instead and you will not see the product name updaed.
   
   Searching for the product name in the registry and deleting the entries may
   fix this, but if it doesn't, you may have to locate the cached INF files and
   remove them as well, or use a different Product ID (which could clash with
   other existing devices.

   Also note, if creating different gamepads to be used at the same time, they
   should have different Product IDs and names. The product ID 0x047A is the one
   used in the Teensy USB-to-Serial example code.
   */
#define STR_MANUFACTURER    L"Manufacturer"
#define STR_PRODUCT         L"Simple Gamepad"
#define PRODUCT_ID          0x047A

/* this sets the serial number string reported by the device */
#define STR_SERIAL_NUMBER   L"00001"

/* this setting determines the number of buttons defined and presented to the OS
   The minimum is 1. The maximum is 20. */
#define BUTTON_COUNT    8

/* enables the internal pull-up resitors on all inputs when defined. Connecting
   the pin to ground activates the button. If this is 0, then extenal pull-up
   resistors must be used */
#define USE_INTERNAL_PULL_UPS   1



#endif /* SIMPLE_GAMEPAD_DEF_H */



