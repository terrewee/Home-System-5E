#define  F_CPU   2000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "clock.h"
#include "ucglib/ucg.h"
#include "ucglib_xmega.h"
#include "serialF0/serialF0.h"
#include "nrf24/nrf24spiXM2.h"
#include "nrf24/nrf24L01.h"

// ----------------------------SET DISPLAY-------------------------------
pin_t connectArraySPI[] = {
	{ UCG_XMEGA_PIN_RST, &PORTD, PIN3_bp },   // D3 as reset
	{ UCG_XMEGA_PIN_CD,  &PORTD, PIN2_bp },   // D2 as CD
	{ UCG_XMEGA_PIN_NULL }
};

ucg_t ucg;

// ---------------------------COMMUNICATION--------------------------------

unsigned int8_t pipe[5] = {0x48, 0x76, 0x41, 0x30, 0x31};	// pipe address "HVA01"
unsigned int8_t packet[32];

// setup connection
void init_nrf(){
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
	
	// interrupt
	PORTF.INT0MASK |= PIN6_bm;
	PORTF.PIN6CTRL	= PORT_ISC_FALLING_gc;
	PORTF.INTCTRL	= (PORTF.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;
	
	// open pipes
	nrfOpen64WritingPipe(pipe);
	nrfOpenReadingPipe(0, pipe);
	nrfStartListening();
}

void send(unsigned int8_t* &message){
	nrfWrite(message, sizeof(unsigned int16_t));
}

// use led to show activity of communication
void init_pwm(){
	PORTC.OUTCLR = PIN0_bm;
	PORTC.DIRSET = PIN0_bm;
	
	TCC0.CTRLB	= TC0_CCAEN_bm | TC_WGMODE_SINGLESLOPE_gc;
	TCC0.CTRLA	= TC_CLKSEL_DIV1_gc;
	TCC0.PER	= 9999;
	TCC0.CCA	= 0;
}

// get package interrupt
ISR(PORTF_INT0_vect){
	unsigned int8_t tx_ds, max_rt, rx_dr;
	
	nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);
	
	if (rx_dr){
		nrfRead(packet,2);
		TCC0.CCABUFL	= packet[0];
		TCC0.CCABUFH	= packet[1];
	}
}

// ---------------------------inputs?--------------------------------

void set_adcch_input(ADC_CH_t *ch, uint8_t pos_pin_gc, uint8_t neg_pin_gc){
}

void init_adc(void){
}

// ------------GLOBALS-------------
// states, used in the state machine
enum states{off, standby, central, window, temprature};
int state = standby;
int devices = 0; // keeps track of how many devices are connected

// ---------------------------FUNCTIONS--------------------------------

// -----STATES-----

// this state is to let the device know what type he is
void standbyState(){
	// insert switching states by input here
}

// state for the central unit with display
void centralState(){
	while(true){
		// measure temp
		// read screen input
		// refresh screen
		// check in with the other devices if there are any updates
		// write changes to storage
	}
}

// state for the unit at the window
void windowState(){
	while(true){
		// measure outside
		// measure inside
		// if connected to the central unit: exchange data
		// if needed, open/close window
	}
}

// state for the heating unit
void tempratureState(){
	// measure inside
	// measure outside(?)
	// if connected to the central unit: exchange data
	// if needed turn on/off heater
	// if needed turn on/off ventilation
}

// -----ANALOG-----

// measure value
int measure(){
	int value;
	
	// update value here
	
	return value;
}

// enable/disable device
void power(){
	//set pins high or low to turn connected devices on/off
}


// ---------------------------MAIN--------------------------------
int main(void){
	//-----INIT-----
	init_stream(F_CPU);
	init_adc();
	init_clock();
	init_nrf();
	
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	
	sei();
	
	// connect ucg to xmega
	ucg_connectXmega(&SPID, connectArraySPI, 0);
	ucg_Init(&ucg, ucg_dev_st7735_18x128x160, ucg_ext_st7735_18, (int (*)(struct _ucg_t *, int,  unsigned int,  unsigned char *)) ucg_commXmegaSPI);
	
	//-----MAIN PROGRAM-----
	while(1){
		switch(state){
			case standby:		standbyState();		break;
			case central:		centralState();		break;
			case window:		windowState();		break;
			case temprature:	tempratureState();	break;
		}
	}
}
