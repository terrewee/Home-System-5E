#define F_CPU 32000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

#include "nrf24spiXM2.h"
#include "nrf24L01.h"
#include "clock.h"
#include "mq135.h"

#define LED_ON_TIME 2000				//time the lights need to stay on after last movement
#define CYCLE_DELAY 200					//time before next cycle starts

uint8_t Elec[4] = {0,0,0,0};			//array for data from "Electronic" node             [Temprature, CO2, Humidity, Sensor weight]
uint8_t Window[5] = {0,0,0,0,0};		//array for data from "Window" node                 [Temprature, CO2, Humidity, Sensor weight, Window open]
uint16_t Movement = 0;					//16 bits interger for time since last movement     
uint8_t Smoke = 0;						//8 bits interger for smoke particals in ppm
uint8_t Interface[5] = {0,0,0,0,0};		//array for data from "interface" node              [Temprature, Temprature setting, CO2, Humidity, Sensor weight]

int number_Received_Elec = 0;        	//number of times something has been received form "Electronic" node  
int number_Received_Window = 0;        	//number of times something has been received form "Window" node              
int number_Received_Light = 0;      	//number of times something has been received form "Light" node
int number_Received_Interface = 0;      //number of times something has been received form "Interface" node   

int time_out = 0;							//cycles that there is no movement

uint32_t received_packet[32];			//create a place to store received data

uint8_t pipes[4][5] =
{
	{0x30, 0x47, 0x72, 0x70, 0x45}, 	//pipe address "0GrpE"
	{0x31, 0x47, 0x72, 0x70, 0x45},		//pipe address "1GrpE"
	{0x32, 0x47, 0x72, 0x70, 0x45},		//pipe address "2GrpE"
	{0x33, 0x47, 0x72, 0x70, 0x45},		//pipe address "3GrpE"
};

//define functions
void init_Nrf(void);

//interrupt from NFR IC
ISR(PORTF_INT0_vect)
{
	uint8_t tx_ds, max_rt, rx_dr;
	uint8_t packet_length;
	nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);									//check what happend from the NRF
	if ( rx_dr ) 
	{																			//check if the action is data received
		packet_length = nrfGetDynamicPayloadSize();								//get the size of the received data	
		nrfRead(received_packet, packet_length );								//store the received data
		
		uint32_t data = received_packet[0];										//create a single 32 bits interger from the received data 
		
		if((data >> 30) == 0)													//check if address is 0
		{																		//store the data in the correct place using bit shifting 
			Elec[0] = ((data << 2) >> 24);
			Elec[1] = ((data << 10) >> 25);
			Elec[2] = ((data << 17) >> 25);
			Elec[3] = ((data << 24) >> 24);
            number_Received_Elec++;
		}
		else if ((data >> 30) == 1)												//check if address is 1
		{																		//store the data in the correct place using bit shifting 
			Window[0] = ((data << 2) >> 24);
			Window[1] = ((data << 10) >> 25);
			Window[2] = ((data << 17) >> 25);
			Window[3] = ((data << 24) >> 24);
			Window[4] = ((data << 31) >> 31);
            number_Received_Window++;
		}
		else if ((data >> 30) == 2)												//check if address is 2
		{																		//store the data in the correct place using bit shifting 
			Movement = ((data << 8) >> 16);
			Smoke = ((data << 24) >> 24);
            number_Received_Light++;
		}
		else if ((data >> 30) == 3)												//check if address is 3
		{																		//store the data in the correct place using bit shifting 
			Interface[0] = ((data << 2) >> 24);
			Interface[1] = ((data << 10) >> 26);
			Interface[2] = ((data << 16) >> 25);
			Interface[3] = ((data << 23) >> 25);
			Interface[4] = ((data << 30) >> 30);
            number_Received_Interface++;
		}
	}
}

//interrupt the program when movement is detected to turn on the light en set time since last movment on 0
ISR(PORTE_INT1_vect)
{
	time_out = 0;
	PORTD.OUTSET = PIN0_bm;
}

int main(void)
{
	init_clock();
	init_Nrf();
	init_MQ135(PORTA, PIN0_bm);

	PORTE.INT1MASK = PIN6_bm;				//interrupt on pin E6
	PORTE.PIN6CTRL = PORT_ISC_RISING_gc;	//interrupt on rising edge
	PORTE.INTCTRL = PORT_INT1LVL_LO_gc ; 	//interrupts On

	PMIC.CTRL |= PMIC_LOLVLEN_bm;			//turn on low level interrupts	

	sei();
	
	PORTD.DIRSET |= PIN0_bm;				//set pin D0 as output
	
	while(1)
	{
		//if there is no more movement add one to the movement counter
		if(bit_is_clear(PORTE.IN,PIN6_bp))
		{
			 time_out++;
		}
		//if the movement counter exceeds the lights on time turn the lights off
		if((CYCLE_DELAY * time_out) >= LED_ON_TIME)
		{
			PORTD.OUTCLR = PIN0_bm;
		}
		
		//save data from this node
		Smoke = getPPM() / 10;
		Movement = (CYCLE_DELAY / (float)1000)*time_out;

		//send out messurements from this node
		uint32_t testgetal = ((int32_t)2 << 30) + ((int32_t)Movement << 8) + ((int32_t)Smoke);
		nrfStopListening();
		cli();
		nrfWrite( (uint8_t *) &testgetal, sizeof(uint32_t) );
		sei();
		nrfStartListening();

		_delay_ms(CYCLE_DELAY);
	}
}

//setup for NRF communication
void init_Nrf(void)
{
	nrfspiInit();                                                               //initialize SPI
	nrfBegin();                                                                 //initialize NRF module
	nrfSetRetries(NRF_SETUP_ARD_1000US_gc, NRF_SETUP_ARC_8RETRANSMIT_gc);		//if failed retry 8 times with a delay of 1000 us
	nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);									//power mode -6dB
	nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);									//data rate: 250kbps				
	nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);                                      //CRC check
	nrfSetChannel(24);															//set channel to 24
	nrfSetAutoAck(1);															//set 
	nrfEnableDynamicPayloads();													//enable the dynamic payload size
	
	nrfClearInterruptBits();													//clear interrupt bits
	nrfFlushRx();                                                               //Flush fifo's
	nrfFlushTx();

	PORTF.INT0MASK |= PIN6_bm;													//interrupt pin F0
	PORTF.PIN6CTRL = PORT_ISC_FALLING_gc;										//interrupts at falling edge
	PORTF.INTCTRL |= (PORTF.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc ; 	//interrupts On
	
	// Opening pipes
	nrfOpenWritingPipe((uint8_t *) pipes[2]);									//set 2GrpE as writing address
	nrfOpenReadingPipe(0, (uint8_t *) pipes[0]);								//set 0GrpE as reading address
	nrfOpenReadingPipe(1, (uint8_t *) pipes[1]);								//set 1GrpE as reading address
	nrfOpenReadingPipe(2, (uint8_t *) pipes[3]);								//set 3GrpE as reading address
	nrfStartListening();
	nrfPowerUp();
}
