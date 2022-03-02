/*!
 *  \file    ucglib_xmega_hal.c
 *  \author  Wim Dolman (<a href="mailto:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    30-09-2020
 *  \version 4.0
 *
 *  \brief   Xmega Hardware Abstraction Layer for ucglib from Oli Kraus
 *
 *  \details This Hardware Abstraction Layer is created confirm the
 *           <a href="https://github.com/olikraus/ucglib/wiki/hal">instructions</a> of Oli Kraus.
 *
 *           The function <code>ucg_connectXmega()</code> sets the connections between the
 *           Xmega and the display. The function <code>ucg_printXmegaConnection()</code> 
 *           prints the connections to the standard output.
 *
 *           This file contains two callback function. The function <code>ucg_com_xmega()</code>
 *           handles the communication with the Xmega using SPI or bit banging. Bit banging
 *           is more flexable, but 1.2 to 2 times slower.
 *
 *           Earlier versions of this HAL (version 2.0, 2.1 and 3.0) contain some extensions 
 *           for printing facilities and for using images. 
 *           These extensions are now placed in separated files (ucg_print.c and ucg_bmp.c) that 
 *           can be added to ucglib.
 *
 *           These HAL uses two static variables. It is not so complicated to add these to the
 *           ucg_t struct. In that case you can use multiple displays, but because of the
 *           overhead that solution will be about 20% slower.
 *
 */

#ifndef _UCGLIB_XMEGA_H
#define _UCGLIB_XMEGA_H

#ifndef __AVR_ATxmega256A3U__
#warning "The precompiled library is built for the ATxmega256A3U"
#endif

#ifndef _UCG_H
#error "You need to include ucg.h first"
#endif

#ifndef WITH_USER_PTR
#error "Ucglib_xmega uses the user pointer from ucg_t! The macro WITH_USER_PTR must be defined in ucg.h (at line 100) or defined global"
#endif

#define UCG_XMEGA_PIN_NULL  -1, NULL, 0     //!< Sentinel element for pin array

typedef enum pin_enum {
  UCG_XMEGA_PIN_SCK,        //!< Index for SCK pin display
  UCG_XMEGA_PIN_SDA,        //!< Index for SDA pin display
  UCG_XMEGA_PIN_SDI,        //!< Index for SDI pin display
  UCG_XMEGA_PIN_CS,         //!< Index for CS  pin display
  UCG_XMEGA_PIN_RST,        //!< Index for RST pin display
  UCG_XMEGA_PIN_CD,         //!< Index for CD  pin display
  UCG_XMEGA_PIN_BLK,        //!< Index for BLK pin display
} pin_enum_t;               //!< Typedef for pin connection index

//!< Struct pin connection
typedef struct pin_struct {
  pin_enum_t  index;        //!< Index connection
  PORT_t     *port;         //!< Port of pin connection
  uint8_t     bp;           //!< Position of pin connection
} pin_t;                    //!< Typedef for pin connection index

// connection
void    ucg_connectXmega(void *pInterface, pin_t *pArray, uint8_t blkDisabled);
void    ucg_printXmegaConnection(void);

// communication (callback functions)
int16_t ucg_commXmega(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data);
int16_t ucg_commXmegaSPI(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data);

#endif