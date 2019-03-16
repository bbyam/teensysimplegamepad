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

   simple_gamepad_usb.h
   This file has definitions for all generic USB usage for all gamepads.
   ======================================================================== */

#ifndef SIMPLE_GAMEPAD_USB_H
#define SIMPLE_GAMEPAD_USB_H

#include <stdint.h>

#define GAMEPAD_ENDPOINT    1

void usb_init(void);            // initialize everything
uint8_t usb_configured(void);   // is the USB port configured

extern volatile uint8_t usb_configuration;

#endif /* SIMPLE_GAMEPAD_USB_H */



