/*!
 *  \file    nrf24_test_send.c
 *  \author  Wim Dolman (<a href="mailto:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    30-06-2016
 *  \version 1.0
 *
 *  \brief   Test program for sedning data with a Nordic NRF24L01p and a Xmega
 *
 *  \details The hardware configuration consists of a sender and a receiver.
 *           This file contains the program for the sender.
 *           The receiver is in file nrf24_test_receiver.c.
 *           The sender is a HvA-Xmegaboard with a external potmeter.
 *  \image   html  xmega3u_nrf_ptx_prx.png
 *  \image   latex xmega3u_nrf_ptx_prx.eps          
 *           The sender measures the analog value on input PA0. 
 *           The measured value, which is in the range from 0 to 2047, is send
 *           with the nrf24l01+. 
 *           The least significant byte of the 16-bits value is send first.
 *
 *           This test program is based on example in paragraph E.1 from 
 *           <a href="http://www.dolman-wim.nl/xmega/index.php">'De taal C en de Xmega'</a>
 *
 */
#define  F_CPU   2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24/nrf24spiXM2.h"
#include "nrf24/nrf24L01.h"

uint8_t  pipe[5] = {0x48, 0x76, 0x41, 0x30, 0x31};       //!< pipe address "HvA01"

void     init_nrf(void);
void     init_adc(void);
int16_t  read_adc(void);

/*! \brief   main routine for sender  
 *
 *           It initializes the nrf24L01+ and the ADC.
 *           Every 20 ms the program measures the input value and send the value
 *           with the nrf24L01+.
 *           The least significant byte is send first.
 *
 *           This routine is code E.2 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *
 *  \return  int
 */
int main(void)
{
  int16_t result;

  init_adc();
  init_nrf();

  while (1) {
    result = read_adc();
    nrfWrite( (uint8_t *) &result, sizeof(uint16_t) );  // little endian: low byte is sent first
    _delay_ms(20);
  }
}

/*! \brief   Initializes nrf24L01+ 
 *
 *           This function is almost the same as the init_nrf() of the receiver.
 *           There are two pipes. One for sending the value and one for the
 *           acknowledge
 *
 *           This routine is code E.1 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *
 *  \return void
 */
void init_nrf(void)
{
  nrfspiInit();                                        // Initialize SPI
  nrfBegin();                                          // Initialize radio module

  nrfSetRetries(NRF_SETUP_ARD_1000US_gc,               // Auto Retransmission Delay: 1000 us
                NRF_SETUP_ARC_8RETRANSMIT_gc);         // Auto Retransmission Count: 8 retries
  nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);             // Power Control: -6 dBm
  nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);          // Data Rate: 250 Kbps
  nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);               // CRC Check
  nrfSetChannel(54);                                   // Channel: 54
  nrfSetAutoAck(1);                                    // Auto Acknowledge on
  nrfEnableDynamicPayloads();                          // Enable Dynamic Payloads

  nrfClearInterruptBits();                             // Clear interrupt bits
  nrfFlushRx();                                        // Flush fifo's
  nrfFlushTx();

  nrfOpenWritingPipe(pipe);                            // Pipe for sending
  nrfOpenReadingPipe(0, pipe);                         // Necessary for acknowledge
}

/*! \brief   Initializes ADC
 *
 *           It initializes PA0 as channel 0 of ADCA in a signed single ended coversion.
 *
 *           This routine is almost (it uses PA0 in stead of PA2) equal to init_adc() of 
 *           code 20.3 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *
 *  \return void
 */
void init_adc(void)
{
  PORTA.DIRCLR     = PIN0_bm;                              // 
  ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc |               // PA0 to + channel 0
                     ADC_CH_MUXNEG_GND_MODE3_gc;           // GND to - channel 0
  ADCA.CH0.CTRL    = ADC_CH_INPUTMODE_DIFF_gc;             // channel 0 differential
  ADCA.REFCTRL     = ADC_REFSEL_INTVCC_gc;
  ADCA.CTRLB       = ADC_RESOLUTION_12BIT_gc |
                     ADC_CONMODE_bm;                       // signed conversion
  ADCA.PRESCALER   = ADC_PRESCALER_DIV16_gc;
  ADCA.CTRLA       = ADC_ENABLE_bm;
}

/*! \brief   read ADC
 *
 *           This function reads a value from channel 0 of ADCA.
 *
 *           This routine is equal to read_adc() of code 20.3 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 *
 *  \return void
 */
int16_t read_adc(void)                                     // return a signed
{
  int16_t res;                                             // is also signed

  ADCA.CH0.CTRL |= ADC_CH_START_bm;
  while ( !(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm) ) ;
  res = ADCA.CH0.RES;
  ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm;

  return res;
}

