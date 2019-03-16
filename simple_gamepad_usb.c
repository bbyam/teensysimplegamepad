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

   simple_gamepad_usb.c
   This file contains all the generic USB code for all gamepads.
   ======================================================================== */



#include "simple_gamepad_usb.h"
#include "simple_gamepad_defs.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


// Mac OS-X and Linux automatically load the correct drivers.  On
// Windows, even though the driver is supplied by Microsoft, an
// INF file is needed to load the driver.  These numbers need to
// match the INF file.
#define VENDOR_ID   0x16C0


// USB devices are supposed to implment a halt feature, which is
// rarely (if ever) used.  If you comment this line out, the halt
// code will be removed, saving 102 bytes of space (gcc 4.3.0).
// This is not strictly USB compliant, but works with all major
// operating systems.
#define SUPPORT_ENDPOINT_HALT



#define EP_TYPE_CONTROL         0x00
#define EP_TYPE_BULK_IN         0x81
#define EP_TYPE_BULK_OUT        0x80
#define EP_TYPE_INTERRUPT_IN        0xC1
#define EP_TYPE_INTERRUPT_OUT       0xC0
#define EP_TYPE_ISOCHRONOUS_IN      0x41
#define EP_TYPE_ISOCHRONOUS_OUT     0x40

#define EP_SINGLE_BUFFER    0x02
#define EP_DOUBLE_BUFFER    0x06

#define EP_SIZE(s)  ((s) == 64 ? 0x30 : \
                    ((s) == 32 ? 0x20 : \
                    ((s) == 16 ? 0x10 : \
                        0x00)))

#define MAX_ENDPOINT    4

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

#if defined(__AVR_AT90USB162__)
#define HW_CONFIG()
#define PLL_CONFIG() (PLLCSR = ((1<<PLLE)|(1<<PLLP0)))
#define USB_CONFIG() (USBCON = (1<<USBE))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_ATmega32U4__)
#define HW_CONFIG() (UHWCON = 0x01)
#define PLL_CONFIG() (PLLCSR = 0x12)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_AT90USB646__)
#define HW_CONFIG() (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x1A)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_AT90USB1286__)
#define HW_CONFIG() (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x16)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#endif

// standard control endpoint request types
#define GET_STATUS              0
#define CLEAR_FEATURE           1
#define SET_FEATURE             3
#define SET_ADDRESS             5
#define GET_DESCRIPTOR          6
#define GET_CONFIGURATION       8
#define SET_CONFIGURATION       9
#define GET_INTERFACE           10
#define SET_INTERFACE           11
// HID (human interface device)
#define HID_GET_REPORT          1
#define HID_GET_IDLE            2
#define HID_GET_PROTOCOL        3
#define HID_SET_REPORT          9
#define HID_SET_IDLE            10
#define HID_SET_PROTOCOL        11
// CDC (communication class device)
#define CDC_SET_LINE_CODING         0x20
#define CDC_GET_LINE_CODING         0x21
#define CDC_SET_CONTROL_LINE_STATE  0x22


/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define ENDPOINT0_SIZE  32

#define GAMEPAD_INTERFACE   0
#define GAMEPAD_SIZE        8
#define GAMEPAD_BUFFER      EP_DOUBLE_BUFFER

static const uint8_t PROGMEM endpoint_config_table[] =
{
    1, EP_TYPE_INTERRUPT_IN,  EP_SIZE(GAMEPAD_SIZE) | GAMEPAD_BUFFER,
    0,
    0,
    0
};


/**************************************************************************
 *
 *  Descriptor Data
 *
 **************************************************************************/

// Descriptors are the data that your computer reads when it auto-detects
// this USB device (called "enumeration" in USB lingo).  The most commonly
// changed items are editable at the top of this file.  Changing things
// in here should only be done by those who've read chapter 9 of the USB
// spec and relevant portions of any USB class specifications!


static const uint8_t PROGMEM device_descriptor[] =
{
    18,                 // bLength
    1,                  // bDescriptorType
    0x00, 0x02,         // bcdUSB
    0,                  // bDeviceClass
    0,                  // bDeviceSubClass
    0,                  // bDeviceProtocol
    ENDPOINT0_SIZE,     // bMaxPacketSize0
    LSB(VENDOR_ID),
    MSB(VENDOR_ID),     // idVendor
    LSB(PRODUCT_ID),
    MSB(PRODUCT_ID),    // idProduct
    0x00, 0x01,         // bcdDevice
    1,                  // iManufacturer
    2,                  // iProduct
    3,                  // iSerialNumber
    1                   // bNumConfigurations
};


#define CONFIG1_DESC_SIZE       (9+9+9+7)
#define GAMEPAD_HID_DESC_OFFSET (9+9)
static const uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] =
{
    // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
    9,                  // bLength;
    2,                  // bDescriptorType;
    LSB(CONFIG1_DESC_SIZE), // wTotalLength
    MSB(CONFIG1_DESC_SIZE),
    1,                  // bNumInterfaces
    1,                  // bConfigurationValue
    0,                  // iConfiguration
    0x80,               // bmAttributes
    50,                 // bMaxPower
    // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
    9,                  // bLength
    4,                  // bDescriptorType
    GAMEPAD_INTERFACE,  // bInterfaceNumber
    0,                  // bAlternateSetting
    1,                  // bNumEndpoints
    0x03,               // bInterfaceClass (0x03 = HID)
    0x00,               // bInterfaceSubClass (0x00 = No Boot)
    0x00,               // bInterfaceProtocol (0x00 = No Protocol)
    0,                  // iInterface
    // HID interface descriptor, HID 1.11 spec, section 6.2.1
    9,                  // bLength
    0x21,               // bDescriptorType
    0x11, 0x01,         // bcdHID
    0,                  // bCountryCode
    1,                  // bNumDescriptors
    0x22,               // bDescriptorType
    sizeof(gamepad_hid_report_desc), // wDescriptorLength
    0,
    // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
    7,                  // bLength
    5,                  // bDescriptorType
    GAMEPAD_ENDPOINT | 0x80, // bEndpointAddress
    0x03,               // bmAttributes (0x03=intr)
    GAMEPAD_SIZE, 0,    // wMaxPacketSize
    10                  // bInterval
};

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
struct usb_string_descriptor_struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    int16_t wString[];
};
static const struct usb_string_descriptor_struct PROGMEM string0 =
{
    4,
    3,
    {0x0409}
};
static const struct usb_string_descriptor_struct PROGMEM string1 =
{
    sizeof(STR_MANUFACTURER),
    3,
    STR_MANUFACTURER
};
static const struct usb_string_descriptor_struct PROGMEM string2 =
{
    sizeof(STR_PRODUCT),
    3,
    STR_PRODUCT
};
static const struct usb_string_descriptor_struct PROGMEM string3 =
{
    sizeof(STR_SERIAL_NUMBER),
    3,
    STR_SERIAL_NUMBER
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const struct descriptor_list_struct
{
    uint16_t wValue;
    uint16_t wIndex;
    const uint8_t *addr;
    uint8_t length;
} PROGMEM descriptor_list[] =
{
    {0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
    {0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
    {0x2100, GAMEPAD_INTERFACE, config1_descriptor+GAMEPAD_HID_DESC_OFFSET, 9},
    {0x2200, GAMEPAD_INTERFACE, gamepad_hid_report_desc, sizeof(gamepad_hid_report_desc)},
    {0x0300, 0x0000, (const uint8_t *)&string0, 4},
    {0x0301, 0x0409, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
    {0x0302, 0x0409, (const uint8_t *)&string2, sizeof(STR_PRODUCT)},
    {0x0303, 0x0409, (const uint8_t *)&string3, sizeof(STR_SERIAL_NUMBER)}
};
#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))


/**************************************************************************
 *
 *  Variables - these are the only non-stack RAM usage
 *
 **************************************************************************/

// zero when we are not configured, non-zero when enumerated
volatile uint8_t usb_configuration = 0;

// protocol setting from the host.  We use exactly the same report
// either way, so this variable only stores the setting since we
// are required to be able to report which setting is in use.
static uint8_t gamepad_protocol = 1;

static uint8_t gamepad_idle_config = 0;


/**************************************************************************
 *
 *  Public Functions - these are the API intended for the user
 *
 **************************************************************************/

// initialize USB
void usb_init(void)
{
    HW_CONFIG();
    USB_FREEZE();               // enable USB
    PLL_CONFIG();               // config PLL
    while (!(PLLCSR & (1<<PLOCK)));     // wait for PLL lock
    USB_CONFIG();               // start USB clock
    UDCON = 0;              // enable attach resistor
    usb_configuration = 0;
    UDIEN = (1<<EORSTE)|(1<<SOFE);
    sei();
}

// return 0 if the USB is not configured, or the configuration
// number selected by the HOST
uint8_t usb_configured(void)
{
    return usb_configuration;
}

/**************************************************************************
 *
 *  Private Functions - not intended for general user consumption....
 *
 **************************************************************************/

ISR(USB_GEN_vect)
{
    uint8_t intbits;

    intbits = UDINT;
    UDINT = 0;
    if (intbits & (1<<EORSTI))
    {
        UENUM = 0;
        UECONX = 1;
        UECFG0X = EP_TYPE_CONTROL;
        UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
        UEIENX = (1<<RXSTPE);
        usb_configuration = 0;
    }
}

// Misc functions to wait for ready and send/receive packets
static inline void usb_wait_in_ready(void)
{
    while (!(UEINTX & (1<<TXINI))) ;
}
static inline void usb_send_in(void)
{
    UEINTX = ~(1<<TXINI);
}
static inline void usb_wait_receive_out(void)
{
    while (!(UEINTX & (1<<RXOUTI))) ;
}
static inline void usb_ack_out(void)
{
    UEINTX = ~(1<<RXOUTI);
}

// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR(USB_COM_vect)
{
    uint8_t intbits;
    const uint8_t *list;
    const uint8_t *cfg;
    uint8_t i, n, len, en;
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint16_t desc_val;
    const uint8_t *desc_addr;
    uint8_t desc_length;

    UENUM = 0;
    intbits = UEINTX;
    if (intbits & (1<<RXSTPI))
    {
        bmRequestType = UEDATX;
        bRequest = UEDATX;
        wValue = UEDATX;
        wValue |= (UEDATX << 8);
        wIndex = UEDATX;
        wIndex |= (UEDATX << 8);
        wLength = UEDATX;
        wLength |= (UEDATX << 8);
        UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
        if (bRequest == GET_DESCRIPTOR)
        {
            list = (const uint8_t *)descriptor_list;
            for (i=0; ; i++)
            {
                if (i >= NUM_DESC_LIST)
                {
                    UECONX = (1<<STALLRQ)|(1<<EPEN);  //stall
                    return;
                }
                desc_val = pgm_read_word(list);
                if (desc_val != wValue)
                {
                    list += sizeof(struct descriptor_list_struct);
                    continue;
                }
                list += 2;
                desc_val = pgm_read_word(list);
                if (desc_val != wIndex)
                {
                    list += sizeof(struct descriptor_list_struct)-2;
                    continue;
                }
                list += 2;
                desc_addr = (const uint8_t *)pgm_read_word(list);
                list += 2;
                desc_length = pgm_read_byte(list);
                break;
            }
            len = (wLength < 256) ? wLength : 255;
            if (len > desc_length) len = desc_length;
            do
            {
                // wait for host ready for IN packet
                do
                {
                    i = UEINTX;
                } while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
                if (i & (1<<RXOUTI))
                    return; // abort
                // send IN packet
                n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
                for (i = n; i; i--)
                {
                    UEDATX = pgm_read_byte(desc_addr++);
                }
                len -= n;
                usb_send_in();
            } while (len || n == ENDPOINT0_SIZE);
            return;
        }
        if (bRequest == SET_ADDRESS)
        {
            usb_send_in();
            usb_wait_in_ready();
            UDADDR = wValue | (1<<ADDEN);
            return;
        }
        if (bRequest == SET_CONFIGURATION && bmRequestType == 0)
        {
            usb_configuration = wValue;
            usb_send_in();
            cfg = endpoint_config_table;
            for (i=1; i<5; i++)
            {
                UENUM = i;
                en = pgm_read_byte(cfg++);
                UECONX = en;
                if (en)
                {
                    UECFG0X = pgm_read_byte(cfg++);
                    UECFG1X = pgm_read_byte(cfg++);
                }
            }
            UERST = 0x1E;
            UERST = 0;
            return;
        }
        if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80)
        {
            usb_wait_in_ready();
            UEDATX = usb_configuration;
            usb_send_in();
            return;
        }

        if (bRequest == GET_STATUS)
        {
            usb_wait_in_ready();
            i = 0;
            #ifdef SUPPORT_ENDPOINT_HALT
            if (bmRequestType == 0x82)
            {
                UENUM = wIndex;
                if (UECONX & (1<<STALLRQ)) i = 1;
                UENUM = 0;
            }
            #endif
            UEDATX = i;
            UEDATX = 0;
            usb_send_in();
            return;
        }
        #ifdef SUPPORT_ENDPOINT_HALT
        if ((bRequest == CLEAR_FEATURE || bRequest == SET_FEATURE)
          && bmRequestType == 0x02 && wValue == 0)
        {
            i = wIndex & 0x7F;
            if (i >= 1 && i <= MAX_ENDPOINT)
            {
                usb_send_in();
                UENUM = i;
                if (bRequest == SET_FEATURE)
                {
                    UECONX = (1<<STALLRQ)|(1<<EPEN);
                }
                else
                {
                    UECONX = (1<<STALLRQC)|(1<<RSTDT)|(1<<EPEN);
                    UERST = (1 << i);
                    UERST = 0;
                }
                return;
            }
        }
        #endif
        if (wIndex == GAMEPAD_INTERFACE)
        {
            if (bmRequestType == 0xA1)
            {
                if (bRequest == HID_GET_REPORT)
                {
                    usb_wait_in_ready();
                    UEDATX = g_gamepadState.x_axis;
                    UEDATX = g_gamepadState.y_axis;
                    for (i = 0; i < BUTTON_ARRAY_SIZE; i++)
                        UEDATX = g_gamepadState.buttons[i];
                    usb_send_in();
                    return;
                }
                if (bRequest == HID_GET_IDLE)
                {
                    usb_wait_in_ready();
                    UEDATX = gamepad_idle_config;
                    usb_send_in();
                    return;
                }
                if (bRequest == HID_GET_PROTOCOL)
                {
                    usb_wait_in_ready();
                    UEDATX = gamepad_protocol;
                    usb_send_in();
                    return;
                }
            }
            if (bmRequestType == 0x21)
            {
                if (bRequest == HID_SET_REPORT)
                {
                    usb_wait_receive_out();
                    usb_ack_out();
                    usb_send_in();
                    return;
                }
                if (bRequest == HID_SET_IDLE)
                {
                    gamepad_idle_config = (wValue >> 8);
                    usb_send_in();
                    return;
                }
                if (bRequest == HID_SET_PROTOCOL)
                {
                    gamepad_protocol = wValue;
                    usb_send_in();
                    return;
                }
            }
        }
    }
    UECONX = (1<<STALLRQ) | (1<<EPEN);  // stall
}




