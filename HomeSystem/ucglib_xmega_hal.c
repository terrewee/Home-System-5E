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
 
#ifndef F_CPU
#define F_CPU 320000000UL                 // The default system clock is 32 MHz
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ucglib/ucg.h"
#include "ucglib_xmega.h"

//!< Struct for connection data
typedef struct ucg_xmega_comm_struct {
  void  (*pTransfer) (uint8_t);  //!< pointer to transfer function
  SPI_t  *pSPI;          //!< pointer to SPI port (NULL in case of bitbanging)
  PORT_t *pSCK;          //!< pointer to port of SCK connection 
  PORT_t *pSDI;          //!< pointer to port of SDI connection  (not used in case of bit banging)
  PORT_t *pSDA;          //!< pointer to port of SDA connection 
  PORT_t *pCS;           //!< pointer to port of CS  connection 
  PORT_t *pRST;          //!< pointer to port of RST (RESET) connection 
  PORT_t *pCD;           //!< pointer to port of CD  (A0) connection 
  PORT_t *pBLK;          //!< pointer to port of BLK (LED) connection 
  uint8_t bpSCK;         //!< pin position of SCK connection 
  uint8_t bpSDI;         //!< pin position of SDI connection  (not used in case of bit banging)
  uint8_t bpSDA;         //!< pin position of SDA connection 
  uint8_t bpCS;          //!< pin position of CS  connection 
  uint8_t bpRST;         //!< pin position of RST (RESET) connection 
  uint8_t bpCD;          //!< pin position of CD  (A0) connection 
  uint8_t bpBLK;         //!< pin position of BLK (LED) connection 
  uint8_t bmSCK;         //!< bit mask of SCK connection 
  uint8_t bmSDI;         //!< bit mask of SDI connection  (not used in case of bit banging)
  uint8_t bmSDA;         //!< bit mask of SDA connection 
  uint8_t bmCS;          //!< bit mask of CS  connection 
  uint8_t bmRST;         //!< bit mask of RST (RESET) connection 
  uint8_t bmCD;          //!< bit mask of CD  (A0) connection 
  uint8_t bmBLK;         //!< bit mask of BLK (LED) connection 
  uint8_t blkDisabled;   //!< if 1 the BLK (LED) connection) is disabled 
} ucg_xmega_comm_t;

static ucg_xmega_comm_t  _commInterface;  //!< local struct for connection with Xmega
static pin_t             _pinArray[7];    //!< local array with pinconnections

// local functions
static void  _xmega_init(void);
static void  _xmega_transfer_bb(uint8_t data);
static void  _xmega_transfer_spi(uint8_t data);
static void  _xmega_disable(void);
static char  _get_port(PORT_t *p);
static char  _get_spi(SPI_t *s);
static void  _print_port(char c);
static void  _print_bp(uint8_t bp);
static void  _print_bm(uint8_t bm);

#define _XMEGA_TRANSFER_SPI(x)\
  _commInterface.pSPI->DATA = (x);\
  while(!(_commInterface.pSPI->STATUS & (SPI_IF_bm))); //!< macro for fast communications

// connection display Xmega

/*! \brief  Defines the connections of the display with the Xmega
 *
 *  \param  pInterface   pointer to the SPI-interface
 *  \param  pArray       pointer to an array with pinconnections
 *  \param  blkDisabled  index to disable BLK-connection
 * 
 *          1. If pInterface is NULL bit banging is used.
 *
 *          2. The pointer pArray points to an array with pins.
 *          A pin consist of three elements: an index, a pointer
 *          to a port and a pinposition. 
 *          The last element is UCG_XMEGA_PIN_NULL.
 *          Example 1 using a SPI-interface:
 * \verbatim   pin_t connectArraySPI[] = {
 *                 { UCG_XMEGA_PIN_RST, &PORTD, PIN3_bp },   // RST
 *                 { UCG_XMEGA_PIN_CD,  &PORTD, PIN2_bp },   // CD
 *                 { UCG_XMEGA_PIN_BLK, &PORTD, PIN1_bp },   // BLK
 *                 { UCG_XMEGA_PIN_NULL }
 *             }; \endverbatim
 *
 *          Example 2 using bit banging:
 * \verbatim  pin_t connectArrayBBcompatibelSPID[] = {
 *               { UCG_XMEGA_PIN_SCK, &PORTD, PIN7_bp },   // SCK
 *               { UCG_XMEGA_PIN_SDA, &PORTD, PIN5_bp },   // SDA
 *               { UCG_XMEGA_PIN_CS,  &PORTD, PIN4_bp },   // CS
 *               { UCG_XMEGA_PIN_RST, &PORTD, PIN3_bp },   // RST
 *               { UCG_XMEGA_PIN_CD,  &PORTD, PIN2_bp },   // CD
 *               { UCG_XMEGA_PIN_BLK, &PORTD, PIN1_bp },   // BLK
 *               { UCG_XMEGA_PIN_NULL }    
 *             }; \endverbatim
 *
 *          3. If blkDisabled is 1 the pin of the XMega that is 
 *          connected to BLK will disabled.
 *          On a breadboard it is now possible to connect VCC to BLK.
 *          Be carefull with a powerline of 5V! 
 * 
 *  \return void
 */
void ucg_connectXmega(void *pInterface, pin_t *pArray, uint8_t blkDisabled)
{
  PORT_t *p = NULL;

  // init _pinArray
  for(int i=0; i<7; i++) {
    _pinArray[i].index = 12;
    _pinArray[i].port  = NULL;
    _pinArray[i].bp    = 8;
  }

  // sort pinArray
  while ( pArray->index != 0xFF ) {
    if ( (pArray->index < UCG_XMEGA_PIN_SCK) || (pArray->index > UCG_XMEGA_PIN_BLK) ) continue;
    _pinArray[pArray->index].port = pArray->port;
    _pinArray[pArray->index].bp   = pArray->bp;
    pArray++;
  }

  // init _commInterface
  if (pInterface == NULL) {
    _commInterface.pTransfer = _xmega_transfer_bb;
    _commInterface.pSPI  = NULL;
    _commInterface.pSCK  = _pinArray[UCG_XMEGA_PIN_SCK].port;
    _commInterface.pSDI  = NULL;
    _commInterface.pSDA  = _pinArray[UCG_XMEGA_PIN_SDA].port;
    _commInterface.pCS   = _pinArray[UCG_XMEGA_PIN_CS].port;
    _commInterface.pRST  = _pinArray[UCG_XMEGA_PIN_RST].port;
    _commInterface.pCD   = _pinArray[UCG_XMEGA_PIN_CD].port;
    _commInterface.pBLK  = _pinArray[UCG_XMEGA_PIN_BLK].port;
    _commInterface.bpSCK = _pinArray[UCG_XMEGA_PIN_SCK].bp;
    _commInterface.bpSDI = 8; // no SDI
    _commInterface.bmSDI = 0; // no SDI
    _commInterface.bpSDA = _pinArray[UCG_XMEGA_PIN_SDA].bp;
    _commInterface.bpCS  = _pinArray[UCG_XMEGA_PIN_CS].bp;
    _commInterface.bpRST = _pinArray[UCG_XMEGA_PIN_RST].bp;
    _commInterface.bpCD  = _pinArray[UCG_XMEGA_PIN_CD].bp;
    _commInterface.bpBLK = _pinArray[UCG_XMEGA_PIN_BLK].bp;
  } else {
    _commInterface.pTransfer = _xmega_transfer_spi;
    _commInterface.pSPI  = (SPI_t *) pInterface;
    switch ( (uint16_t) pInterface ) {
      #ifdef SPIC 
        case (uint16_t)&SPIC: p = &PORTC; break;
      #endif
      #ifdef SPID 
        case (uint16_t)&SPID: p = &PORTD; break;
      #endif
      #ifdef SPIE 
        case (uint16_t)&SPIE: p = &PORTE; break;
      #endif
      #ifdef SPIF 
        case (uint16_t)&SPIF: p = &PORTF; break;
      #endif
      #ifdef SPIG 
        case (uint16_t)&SPIG: p = &PORTG; break;
      #endif
    }
    _commInterface.pSCK  = p;
    _commInterface.pSDI  = p;
    _commInterface.pSDA  = p;
    _commInterface.pCS   = p;
    _commInterface.pRST  = _pinArray[UCG_XMEGA_PIN_RST].port;
    _commInterface.pCD   = _pinArray[UCG_XMEGA_PIN_CD].port;
    _commInterface.pBLK  = _pinArray[UCG_XMEGA_PIN_BLK].port;
    _commInterface.bpSCK = PIN7_bp;
    _commInterface.bpSDI = PIN6_bp;
    _commInterface.bmSDI = PIN6_bm;
    _commInterface.bpSDA = PIN5_bp;
    _commInterface.bpCS  = PIN4_bp;
    _commInterface.bpRST = _pinArray[UCG_XMEGA_PIN_RST].bp;
    _commInterface.bpCD  = _pinArray[UCG_XMEGA_PIN_CD].bp;
    _commInterface.bpBLK = _pinArray[UCG_XMEGA_PIN_BLK].bp;
  }
  
  _commInterface.bmSCK = ( 1 << _commInterface.bpSCK );
  _commInterface.bmSDA = ( 1 << _commInterface.bpSDA );
  _commInterface.bmCS  = ( 1 << _commInterface.bpCS  );
  _commInterface.bmRST = ( 1 << _commInterface.bpRST );
  _commInterface.bmCD  = ( 1 << _commInterface.bpCD  );
  _commInterface.bmBLK = ( 1 << _commInterface.bpBLK );

  _commInterface.blkDisabled = blkDisabled;
}


/*! \brief  Print (debugging) the connections with the Xmega
 *
 *  \return void
 */
void ucg_printXmegaConnection(void)
{
  printf("\nConnections Overview\n");
  if (_commInterface.pSPI == NULL) {
    printf("interface     : bit banging\n");
  } else {
    printf("interface     : SPI%c\n", _get_spi(_commInterface.pSPI));
  }
  printf("              : CLK    SDI    SDA    CS     RST    CD     BLK\n");
  printf("port          : ");
  _print_port(_get_port(_commInterface.pSCK));
  _print_port(_get_port(_commInterface.pSDI));
  _print_port(_get_port(_commInterface.pSDA));
  _print_port(_get_port(_commInterface.pCS));
  _print_port(_get_port(_commInterface.pRST));
  _print_port(_get_port(_commInterface.pCD));
  _print_port(_get_port(_commInterface.pBLK));
  printf("\n");

  printf("bit position  : ");
  _print_bp(_commInterface.bpSCK);
  _print_bp(_commInterface.bpSDI);
  _print_bp(_commInterface.bpSDA);
  _print_bp(_commInterface.bpCS);
  _print_bp(_commInterface.bpRST);
  _print_bp(_commInterface.bpCD);
  _print_bp(_commInterface.bpBLK);
  printf("\n");
  printf("bit mask      : ");
  _print_bm(_commInterface.bmSCK);
  _print_bm(_commInterface.bmSDI);
  _print_bm(_commInterface.bmSDA);
  _print_bm(_commInterface.bmCS);
  _print_bm(_commInterface.bmRST);
  _print_bm(_commInterface.bmCD);
  _print_bm(_commInterface.bmBLK);
  printf("\n");
  if ( _commInterface.bpBLK > 7 ) {
    printf("blk           : not used\n");
  } else {
    if ( _commInterface.blkDisabled ) {
      printf("blk           : defined but disabled\n");
    } else {
      printf("blk           : active\n");
    }
  } 
}


// communication

/*! \brief  The callback function for communication between the Xmega and the display.
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  msg      number of the message (action to be done) 
 *  \param  arg      depends on msg: number of arguments, number of microseconds, ...
 *  \param  data     pointer to 8-bit data-array with bytes that needs to be send
 *
 *  \return 16-bit value, always 1
 */
int16_t ucg_commXmega(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data)
{
  switch(msg)
  {
    case UCG_COM_MSG_POWER_UP:
      _xmega_init();
      ucg_PrintInit(ucg);
      break;
    case UCG_COM_MSG_POWER_DOWN:
      _xmega_disable();
      break;
    case UCG_COM_MSG_DELAY:
      for(uint16_t i=0; i<arg; i++) {
        _delay_us(1);
      }
      break;
    case UCG_COM_MSG_CHANGE_RESET_LINE:
      if (arg) {
        _commInterface.pRST->OUTSET = _commInterface.bmRST;
      } else {
        _commInterface.pRST->OUTCLR = _commInterface.bmRST;
      }
      break;
    case UCG_COM_MSG_CHANGE_CS_LINE:
      if (arg) {
        _commInterface.pCS->OUTSET = _commInterface.bmCS;
      } else {
        _commInterface.pCS->OUTCLR = _commInterface.bmCS;
      }
      break;
    case UCG_COM_MSG_CHANGE_CD_LINE:
      if (arg) {
        _commInterface.pCD->OUTSET = _commInterface.bmCD;
      } else {
        _commInterface.pCD->OUTCLR = _commInterface.bmCD;
      }
      break;
    case UCG_COM_MSG_SEND_BYTE:
      _commInterface.pTransfer(arg);
      break;
    case UCG_COM_MSG_REPEAT_1_BYTE:
      while( arg > 0 ) {
        _commInterface.pTransfer(data[0]);
        arg--;
      }
      break;
    case UCG_COM_MSG_REPEAT_2_BYTES:
      while( arg > 0 ) {
        _commInterface.pTransfer(data[0]);
        _commInterface.pTransfer(data[1]);
        arg--;
      }
      break;
    case UCG_COM_MSG_REPEAT_3_BYTES:
      while( arg > 0 ) {
        _commInterface.pTransfer(data[0]);
        _commInterface.pTransfer(data[1]);
        _commInterface.pTransfer(data[2]);
        arg--;
      }
      break;
    case UCG_COM_MSG_SEND_STR:
      while( arg > 0 ) {
        _commInterface.pTransfer(*data++);
        arg--;
        }
      break;
  }  
  return 1;
}

/*! \brief  The callback function for communication with SPI between the Xmega and the display.
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  msg      number of the message (action to be done) 
 *  \param  arg      depends on msg: number of arguments, number of microseconds, ...
 *  \param  data     pointer to 8-bit data-array with bytes that needs to be send
 *
 *  This specific function can only be used with SPI and is about 25% faster than 
 *  the general ucg_commXmega
 *  
 *  \return 16-bit value, always 1
 */

int16_t ucg_commXmegaSPI(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data)
{
  switch(msg)
  {
    case UCG_COM_MSG_POWER_UP:
      _xmega_init();
      ucg_PrintInit(ucg);
      break;
    case UCG_COM_MSG_POWER_DOWN:
      _xmega_disable();
      break;
    case UCG_COM_MSG_DELAY:
      for(uint16_t i=0; i<arg; i++) {
        _delay_us(1);
      }
      break;
    case UCG_COM_MSG_CHANGE_RESET_LINE:
      if (arg) {
        _commInterface.pRST->OUTSET = _commInterface.bmRST;
      } else {
        _commInterface.pRST->OUTCLR = _commInterface.bmRST;
      }
      break;
    case UCG_COM_MSG_CHANGE_CS_LINE:
      if (arg) {
        _commInterface.pCS->OUTSET = _commInterface.bmCS;
      } else {
        _commInterface.pCS->OUTCLR = _commInterface.bmCS;
      }
      break;
    case UCG_COM_MSG_CHANGE_CD_LINE:
      if (arg) {
        _commInterface.pCD->OUTSET = _commInterface.bmCD;
      } else {
        _commInterface.pCD->OUTCLR = _commInterface.bmCD;
      }
      break;
    case UCG_COM_MSG_SEND_BYTE:
      _XMEGA_TRANSFER_SPI(arg);
      break;
    case UCG_COM_MSG_REPEAT_1_BYTE:
      while( arg > 0 ) {
        _XMEGA_TRANSFER_SPI(data[0]);
        arg--;
      }
      break;
    case UCG_COM_MSG_REPEAT_2_BYTES:
      while( arg > 0 ) {
        _XMEGA_TRANSFER_SPI(data[0]);
        _XMEGA_TRANSFER_SPI(data[1]);
        arg--;
      }
      break;
    case UCG_COM_MSG_REPEAT_3_BYTES:
      while( arg > 0 ) {
        _XMEGA_TRANSFER_SPI(data[0]);
        _XMEGA_TRANSFER_SPI(data[1]);
        _XMEGA_TRANSFER_SPI(data[2]);
        arg--;
      }
      break;
    case UCG_COM_MSG_SEND_STR:
      while( arg > 0 ) {
        _XMEGA_TRANSFER_SPI(*data++);
        arg--;
      }
      break;
  }  
  return 1;
}


// local functions communication 

/*  brief  Initialization of the communication 
 *
 *  return void
 */
static void _xmega_init(void)
{
  _commInterface.pRST->DIRSET = _commInterface.bmRST;
  _commInterface.pCD->DIRSET  = _commInterface.bmCD;
  if ( _commInterface.pBLK != NULL ) {     // BLK connected
    if ( _commInterface.blkDisabled ) {    // not used 
      *( (register8_t *) ( &(_commInterface.pBLK->PIN0CTRL) + _commInterface.bpBLK) ) = PORT_ISC_INPUT_DISABLE_gc;
    } else {                               // used
      _commInterface.pBLK->DIRSET = _commInterface.bmBLK;
      _commInterface.pBLK->OUTSET = _commInterface.bmBLK;
    }
  }      
  _commInterface.pSCK->DIRSET   = _commInterface.bmSCK;
  _commInterface.pSDA->DIRSET   = _commInterface.bmSDA;
  _commInterface.pCS->DIRSET    = _commInterface.bmCS;
  _commInterface.pCS->OUTSET    = _commInterface.bmCS;

  if ( _commInterface.pSPI != NULL ) {
    _commInterface.pSDI->DIRCLR  = _commInterface.bmSDI;
    _commInterface.pSPI->CTRL    = SPI_ENABLE_bm |  // enable SPI
                                   SPI_MASTER_bm |  // master mode
                                   SPI_CLK2X_bm  |  // double clock speed
                                // SPI_DORD_bm   |  // MSB first
                                   SPI_MODE_0_gc |  // SPI mode 0
                                   SPI_PRESCALER_DIV4_gc;  // prescaling 4
  }
}

/*  brief  Disable communication
*
*   return void
*/
static void _xmega_disable(void)
{
  if ( _commInterface.pSPI != NULL ) {
    _commInterface.pSPI->CTRL = _commInterface.pSPI->CTRL & ~SPI_ENABLE_bm;
  }
  if ( (_commInterface.pBLK != NULL) && (! _commInterface.blkDisabled) ) {   // connected and used
    _commInterface.pBLK->OUTCLR = _commInterface.bmBLK;
  }
}


/*  brief  Transfer a byte with bit banging
 *
 *  return void
 */
static inline void _xmega_transfer_bb(uint8_t data)
{
  for (uint8_t i=0; i<8; i++) {
    if (data & 0x80) {
      _commInterface.pSDA->OUTSET = _commInterface.bmSDA; 
    } else {
      _commInterface.pSDA->OUTCLR = _commInterface.bmSDA;
    }
    _commInterface.pSDA->OUTSET = _commInterface.bmSCK;
    _commInterface.pSDA->OUTCLR = _commInterface.bmSCK;
    data <<= 1;
  }
    
  _commInterface.pSDA->OUTSET = _commInterface.bmSDA;
}


/*  brief  Transfer a byte with SPI
 *
 *  return void
 */
static inline void _xmega_transfer_spi(uint8_t data)
{
  _commInterface.pSPI->DATA = data;
  while(!(_commInterface.pSPI->STATUS & (SPI_IF_bm)));
}

// local functions connection display Xmega

/*   brief  Gets (debugging) the last character of the PORT name
 *
 *   param  p   pointer to the PORT
 *
 *   return last character of the PORT name
 */
static char _get_port(PORT_t *p)
{
  switch ( (uint16_t) p ) {
    #ifdef PORTA
    case (uint16_t)&PORTA:  return 'A';
    #endif
    #ifdef PORTB
    case (uint16_t)&PORTB:  return 'B';
    #endif
    #ifdef PORTC
    case (uint16_t)&PORTC:  return 'C';
    #endif
    #ifdef PORTD
    case (uint16_t)&PORTD:  return 'D';
    #endif
    #ifdef PORTE
    case (uint16_t)&PORTE:  return 'E';
    #endif
    #ifdef PORTF
    case (uint16_t)&PORTF:  return 'F';
    #endif
    #ifdef PORTG
    case (uint16_t)&PORTG:  return 'G';
    #endif
  }

  return 'x';
}

/*  brief  Print (debugging) the SPI
 *
 *  param  s   pointer to the SPI
 *
 *  return void
 */
static char _get_spi(SPI_t *s)
{
  switch ( (uint16_t) s ) {
    #ifdef SPIC
    case (uint16_t)&SPIC:  return 'C';
    #endif
    #ifdef SPID
    case (uint16_t)&SPID:  return 'D';
    #endif
    #ifdef SPIE
    case (uint16_t)&SPIE:  return 'E';
    #endif
    #ifdef SPIF
    case (uint16_t)&SPIF:  return 'F';
    #endif
    #ifdef SPIG
    case (uint16_t)&SPIG:  return 'G';
    #endif
  }

  return 'x';
}

/*  brief  Print (debugging) the port 
 *
 *  param  c   last character of the PORT name
 *
 *  return void
 */
static void _print_port(char c)
{
  if ( c == 'x' ) {
    printf("%7s", "");
  } else {
    printf("PORT%c%2s", c, "");
  }
}

/*  brief  Print (debugging) the bitposition 
 *
 *  param  bp   bitposition
 *
 *  return void
 */
static void _print_bp(uint8_t bp)
{
  if ( bp > 7 ) {
    printf("%7s", "");
  } else {
    printf("%-6d%1s", bp, "");
  }
}

/*  brief  Print (debugging) the bitmask
 *
 *  param  bm   bitmask
 *
 *  return void
 */
static void _print_bm(uint8_t bm)
{
  if ( bm == 0 ) {
    printf("%7s", "");
  } else {
    printf("0x%02x%3s", bm, "");
  }
}


