/*!
 *  \file    nrf24_test_receive.c
 *  \author  Wim Dolman (<a href="mailto:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    30-06-2016
 *  \version 1.0
 *
 *  \brief   Test program for receiving data with a Nordic NRF24L01p and a Xmega
 *
 *  \details The hardware configuration consists of a sender and a receiver.
 *           This file contains the program for the receiver.
 *           The sender is in file nrf24_test_send.c.
 *           The receiver is just a HvA-Xmegaboard with nothing connected.
 *           The led is the blue led.
 *  \image   html  xmega3u_nrf_ptx_prx.png
 *  \image   latex xmega3u_nrf_ptx_prx.eps          
 *           The receiver gets two bytes from the sender. 
 *           The first byte received is the least significant byte and the second byte 
 *           is the most significant byte. 
 *           This 16-bits value is used to change the duty-cycle of the PWM-signaal 
 *
 *           This test program is based on example in paragraph E.1 from 
 *           <a href="http://www.dolman-wim.nl/xmega/index.php">'De taal C en de Xmega'</a>
 *
 */
#define  F_CPU  2000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "nrf24/nrf24spiXM2.h"
#include "nrf24/nrf24L01.h"

uint8_t  pipe[5] = {0x48, 0x76, 0x41, 0x30, 0x31};    //!<  pipe address is "HvA01"      
uint8_t  packet[32];                                  //!<  buffer for storing received data

void init_pwm(void);
void init_nrf(void);

/*! \brief   main routine for receiver  
 *
 *           It initializes the nrf24L01+ and the pwm output and enables the interrupt mechanism. 
 *           It does nothing. The program just waits for an interrupt of the nrf24L01+.
 *
 *           This routine is code E.4 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *
 *  \return  int
 */
int main(void)
{
  init_pwm();
  init_nrf();

  PMIC.CTRL |= PMIC_LOLVLEN_bm;
  sei();

  while (1) {}   // do nothing
}

/*! \brief   Initializes nrf24L01+ 
 *
 *           This function is almost the same as the init_nrf() of the sender.
 *           The interrupt is enabled. The interrupt pin is pin 6 of port F and
 *           responses to a falling edge.
 *           
 *           Only a pipe for reading opened and the radiomodule is set in the
 *           receive mode.
 *
 *           This routine is code E.3 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *
 *  \return void
 */
void init_nrf(void)
{
  nrfspiInit();
  nrfBegin();

  nrfSetRetries(NRF_SETUP_ARD_1000US_gc, NRF_SETUP_ARC_8RETRANSMIT_gc);
  nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);
  nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);
  nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);
  nrfSetChannel(54);
  nrfSetAutoAck(1);
  nrfEnableDynamicPayloads();

  nrfClearInterruptBits();
  nrfFlushRx();
  nrfFlushTx();

  // Interrupt
  PORTF.INT0MASK |= PIN6_bm;
  PORTF.PIN6CTRL  = PORT_ISC_FALLING_gc;
  PORTF.INTCTRL   = (PORTF.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;

  nrfOpenReadingPipe(0, pipe);
  nrfStartListening();
}

/*! \brief   Initializes pin 0 of port C as PWM ouput
 *
 *           With F_CPU is 2 MHz the frequency will be 200 Hz
 *
 *           This routine is code E.5 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *  \return void
 */
void init_pwm(void)
{
  PORTC.OUTCLR = PIN0_bm;
  PORTC.DIRSET = PIN0_bm;

  TCC0.CTRLB   = TC0_CCAEN_bm | TC_WGMODE_SINGLESLOPE_gc;
  TCC0.CTRLA   = TC_CLKSEL_DIV1_gc;
  TCC0.PER     = 9999;
  TCC0.CCA     = 0;
}

/*! \brief   Interrupt function for receiving data
 *
 *           If the interrupt is caused by the received data a data packet
 *           is read and assigned to CCABUF of timer/counter0 of port C.
 *
 *           This routine is code E.6 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *  \return void
 */
ISR(PORTF_INT0_vect)
{
  uint8_t  tx_ds, max_rt, rx_dr;

  nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);

  if ( rx_dr ) {
    nrfRead(packet, 2);
    TCC0.CCABUFL  =  packet[0];       // low byte was sent first
    TCC0.CCABUFH  =  packet[1];
  }
}