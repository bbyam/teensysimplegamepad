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

   simple_gamepad.c
   This defines the main entry point, initial configuration, and loop
   ======================================================================== */

#include <util/delay.h>
#include "simple_gamepad_defs.h"
#include "simple_gamepad_usb.h"


#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

// Number of 1 ms sleeps until state is automatically transmitted when there has been no change
#define NOCHANGE_TX_COUNT   33  // Approx 30 per second


int main(void)
{
    uint8_t noChangeCounter = 0;

    // set for 16 MHz clock
    CPU_PRESCALE(0);

    // good explenation of how AVR pins work:
    // http://www.pjrc.com/teensy/pins.html

    // configure Teensy inputs for buttons - this also configures LED pin for output
    simple_gamepad_configure();

    // Initialize the USB, and then wait for the host to set configuration.
    // If the Teensy is powered without a PC connected to the USB port,
    // this will wait forever.
    usb_init();
    while (!usb_configured()) /* wait */ ;

    // Wait an extra second for the PC's operating system to load drivers
    // and do whatever it does to actually be ready for input
    _delay_ms(1000);

    // Initialize and transmit initial state
    simple_gampad_read_buttons();
    usb_simple_gamepad_send();

    for (;;)
    {
        if (simple_gampad_read_buttons())
        {
            // Send if state changed or if the host requested descriptors
            // The time this send takes will work as simple debounce
            usb_simple_gamepad_send();
            noChangeCounter = 0;
        }
        else
        {
            // Brief sleep otherwise
            _delay_ms(1);

            // Continue to transmit state every so often when there's no change
            if (++noChangeCounter >= NOCHANGE_TX_COUNT)
            {
                usb_simple_gamepad_send();
                noChangeCounter = 0;
            }
        }
    }
}



