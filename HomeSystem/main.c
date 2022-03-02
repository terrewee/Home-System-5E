#define  F_CPU   32000000UL

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

// ----------------------------SET DISPLAY-------------------------------
pin_t connectArraySPI[] = {
	{ UCG_XMEGA_PIN_RST, &PORTD, PIN3_bp },   // D3 as reset
	{ UCG_XMEGA_PIN_CD,  &PORTD, PIN2_bp },   // D2 as CD
	{ UCG_XMEGA_PIN_NULL }
};

ucg_t    ucg;

// ---------------------------inputs?--------------------------------

void set_adcch_input(ADC_CH_t *ch, uint8_t pos_pin_gc, uint8_t neg_pin_gc)
{
}

void init_adc(void)
{
}

// ------------GLOBALS-------------
// game states, used in the state machine
enum states{off, standby, central, window, temprature};
int state = standby;


// ---------------------------FUNCTIONS--------------------------------


// ---------------------------MAIN--------------------------------
int main(void){
	//-----INIT-----
	init_stream(F_CPU);
	init_adc();
	init_clock();
	srand(time(0));
	
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
