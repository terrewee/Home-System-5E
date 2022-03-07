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

void set_adcch_input(ADC_CH_t *ch, uint8_t pos_pin_gc, uint8_t neg_pin_gc){
}

void init_adc(void){
}

// ------------GLOBALS-------------
// states, used in the state machine
enum states{off, standby, central, window, temprature};
int state = standby;
int devices = 0; // keeps track of how many devices are connected

//info for other devices
struct Device{
	char	name[20];
	int		ip[20];
	int		ID[1];
	int		type[1];
};


// ---------------------------FUNCTIONS--------------------------------

// -----COMMUNICATION-----
// will connect to another device through blue tooth
Device connect(char[20] name, int[20] ip){
	
	// insert connecting here
	// should get type from the other device as well
	
	if(/*connection is successful*/){
		devices++;	//increase devices 
		struct Device newDevice;
		newDevice.name = name;
		newDevice.ip = ip;
		newDevice.ID = devices;
		newDevice.type = /*recieved device type from connection*/;
		return newDevice;
	}
	//if it can't connect
	return NULL;
}

// for requesting commands to other devices
int sendCommand(Device device, int commandID){
	
	//insert sending data to other device and getting a response back here
	
	if(/*recieve positive response back*/){
		return 1;
	}
	else if(/*negative response back*/){
		return -1;
	}
	//if no response back
	return 0;
}

// for executing received commands NOTE: I don't know how blue tooth communication is handled, this is just an assumption
void executeCommand(){
	switch (){
		// insert command options
	}
	// need to send back if executed successfully
}

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
