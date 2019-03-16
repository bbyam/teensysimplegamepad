# Teensy 2.0 Simple Gamepad

The Teensy 2.0 Simple Gamepad implements a simple USB gamepad with up/down/left/right and a configurable number of buttons (between 1 and 20). It is designed to run on the Teensy 2.0 board.

This code was designed to provide a working gamepad with customizable USB Manufacturer Name, Product Name, and Serial Number that requires minimal changes to the code. It is designed so that even someone with extremely minimal C coding experience can use the code to create a USB gamepad using the Teensy 2.0.

## Usage and Notes

To use the source code, you must first have a working Teensy 2.0 C development environment. This can be setup by following the [instructions on the Teensy website](http://www.pjrc.com/teensy/gcc.html), then you simply run `make` in the source directory after you configure your gamepad code.

To configure the gamepad code, all you need to do is edit the settings found in `simple_gamepad_config.h`. Follow the specific instructions in there. You will need to set the USB Manufacturer name, Product name, Product ID, and Serial Number. The Manufacturer ID is defaulted to the ID used in all the Teensy examples. Then, you simply decide the number of buttons your gamepad will have, and also there's an option if you are using your own external pull-up resistors. Any Teensy pins not used as buttons will be automatically configured as outputs, but you can also use them for other purporses if you want to modify the code. Then build it, and you will have a USB gamepad with up/down/left/right, plus the number of buttons you specified. The details about which pins map to which buttons are laid out in `simple_gamepad_config.h`.

This gamepad code was created when I could not find other solutions to create multiple gamepads with specific Manufacturer and Product names, as well as requiring unique serial numbers for each gamepad, for an HTPC inside an original NES case project I was working on.

## Credits

The following sources were used as reference/inspiration:
 
Teensy examples: http://www.pjrc.com/teensy/usb_serial.html  
SNES USB Gamepad: http://www.slashdev.ca/2011/10/20/snes-usb-gamepad/  
USB Reference: http://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/  
USB Reference: http://www.beyondlogic.org/usbnutshell/usb5.shtml

## Links

This code is open source, can be used for any purpose, and requires no attribution to me other than in the MIT license. However, I would love to hear how you have used my code in your project! Also, if you would like me to link to a project of yours derived from this one, just let me know.

2 Gamepads in one Teensy device for a Neo Geo Adapter:  
https://github.com/sylverb/NeoGeoUSBAdapter
