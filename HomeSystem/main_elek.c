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
#include "bme280.h"

#define CYCLE_DELAY 500			        //time before next cycle starts
#define SENSOR_WEIGHT 1.5               //the weight of the sensor in calculations, can have 1 decimal number [value between 0 and 25.5]  
#define VALUE_RESET 10                  //amount of cycle it takes to reset received values to 0 if no new values are received;

#define DEFAULT_TEMPRATURE 20.0         //default temprature setting if no new one is received from "interface" node
#define DEFAULT_HUMIDITY 50             //default humidity setting
#define DEFAULT_MAX_CO2  1000           //the maximium amount of CO2 before action is taken

#define MAX_TEMPRATURE_OFFSET 2.0       //maximum allowed temprature differnce before action is taken
#define MAX_HUMIDITY_OFFSET 10          //maximum allowed humidity differnce before action is taken
#define MAX_CO2_OFFSET 400              //maximum CO2 in ppm differnce before action is taken

uint8_t Elec[4] = {0,0,0,0};			//array for data from "Electronic" node             [Temprature, CO2, Humidity, Sensor weight]
uint8_t Window[5] = {0,0,0,0,0};		//array for data from "Window" node                 [Temprature, CO2, Humidity, Sensor weight, Window open]
uint16_t Movement = 0;					//16 bits interger for time since last movement     
uint8_t Smoke = 0;						//8 bits interger for smoke particals in ppm
uint8_t Interface[5] = {0,0,0,0,0};		//array for data from "Interface" node              [Temprature, Temprature setting, CO2, Humidity, Sensor weight]

int number_Received_Elec = 0;           //number of times something has been received form "Electronic" node  
int number_Received_Window = 0;         //number of times something has been received form "Window" node              
int number_Received_Light = 0;          //number of times something has been received form "Light" node
int number_Received_Interface = 0;      //number of times something has been received form "Interface" node 

int number_Received_Window_old = 0;     //number of recored times something has been received form "Window" node in previous cycle
int number_Received_Interface_old = 0;  //number of recored times something has been received form "Interface" node in previous cycle

int number_Window_old = 0;              //number of cycles the received values have been the same
int number_Interface_old = 0;           //number of cycles the received values have been the same

float average_temprature = 0;           //stored avarage calculated temprature
float average_humidity = 0;             //stored avarage calculated humidity
float average_CO2 = 0;                  //stored avarage calculated CO2 value

int heating = 0;                        //stored heater state
int ventilation = 0;                    //stored ventilation state

uint32_t received_packet[32];			//create a place to store received data

uint8_t pipes[4][5] =
{
	{0x30, 0x47, 0x72, 0x70, 0x45},     //pipe address "0GrpE"
	{0x31, 0x47, 0x72, 0x70, 0x45},	    //pipe address "1GrpE"
	{0x32, 0x47, 0x72, 0x70, 0x45},	    //pipe address "2GrpE"
	{0x33, 0x47, 0x72, 0x70, 0x45},	    //pipe address "3GrpE"
};

//define functions
void init_Nrf(void);
void getNewData(void);
void checkReceived(void);

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

int main(void)
{
	init_clock();
	init_Nrf();
	init_MQ135(PORTA, PIN0_bm);
    init_BME280(F_CPU);
	sei();                                      
	
    Interface[1] = DEFAULT_TEMPRATURE * 10;     //set default temprature

    PORTD.DIRSET |= PIN0_bm | PIN1_bm;          //set pin D1 & D2 as outputs

	while(1)
	{
		getNewData();

        //turn on heater if the average temprature exceeds the offset and the heater is off
        if(average_temprature < (DEFAULT_TEMPRATURE - MAX_TEMPRATURE_OFFSET) && !heating)
        {
            heating = 1;
            PORTD.OUTSET = PIN0_bm;
        }
        //turn off heater if the average temprature exceeds the set temprature and the heater is on
        if(average_temprature > (DEFAULT_TEMPRATURE) && heating)
        {
            heating = 0;
            PORTD.OUTCLR = PIN0_bm;
        }
        
        //turn on ventilation if the average humidity exceeds the positive offset and the ventilation is off
        if(average_humidity > (DEFAULT_HUMIDITY + MAX_HUMIDITY_OFFSET) && !ventilation)
        {
            ventilation = 1;
            PORTD.OUTSET = PIN1_bm;
        }
        //turn off ventilation if the average humidity exceeds the negative offset and the ventilation is on
        if(average_humidity < (DEFAULT_HUMIDITY - MAX_HUMIDITY_OFFSET) && ventilation)
        {
            ventilation = 0;
            PORTD.OUTCLR = PIN1_bm;
        }

        //turn on ventilation if the average CO2 exceeds the offset and the ventilation is off and the humidity is within range
        if(average_CO2 > (DEFAULT_MAX_CO2 - MAX_CO2_OFFSET) && !ventilation && average_humidity > (DEFAULT_HUMIDITY - MAX_HUMIDITY_OFFSET))
        {
            ventilation = 1;
            PORTD.OUTSET = PIN1_bm;
        }

		//send out messurements from this node
		uint32_t testgetal = ((int32_t)0 << 30) + ((int32_t)Elec[0]<< 22) + ((int32_t)((int8_t)Elec[1] << 1) << 14) + ((int32_t)((int8_t)Elec[2] << 1) << 7) + (int32_t)Elec[3];
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
	nrfOpenWritingPipe((uint8_t *) pipes[0]);									//set 0GrpE as writing address
	nrfOpenReadingPipe(0, (uint8_t *) pipes[1]);								//set 1GrpE as reading address
	nrfOpenReadingPipe(1, (uint8_t *) pipes[2]);								//set 2GrpE as reading address
	nrfOpenReadingPipe(2, (uint8_t *) pipes[3]);								//set 3GrpE as reading address
	nrfStartListening();
	nrfPowerUp();
}

//store new measurement data that will be used for the calculations
void getNewData(void)
{
    //store new data for this node
    read_BME280(BME280_ADDRESS_1);
    Elec[0] = (getTemperature_C() * 10)/2;
    Elec[1] = getCorrectedPPM(getTemperature_C(),getHumidity()) / 10.0;
    Elec[2] = getHumidity();
    Elec[3] = SENSOR_WEIGHT*10;

    checkReceived();

    //calculate average temprature form all nodes
    average_temprature = ((((Elec[0] * 2) / 10.0) * (Elec[3] / 10.0)) + (((Window[0] * 2) / 10.0) * (Window[3] / 10.0)) + (((Interface[0] * 2) / 10.0) * Interface[4])) / ((Elec[3] / 10.0) + (Window[3] / 10.0) + Interface[4]);
    //calculate average humidity form all nodes
    average_humidity = ((Elec[2] * (Elec[3] / 10.0)) + (Window[2] * (Window[3] / 10.0)) + (Interface[3] * Interface[4])) / ((Elec[3] / 10.0) + (Window[3] / 10.0) + Interface[4]);
    //calculate average CO2 value form all nodes
    average_CO2 = (((Elec[1] * 10) * (Elec[3] / 10.0)) + ((Window[1] * 10) * (Window[3] / 10.0)) + ((Interface[2] * 10) * Interface[4])) / ((Elec[3] / 10.0) + (Window[3] / 10.0) + Interface[4]);
}

//check if received data from nodes is still "new"
void checkReceived(void)
{
    //check if data from "window" node is "new" if not reset it to default values
    if(number_Received_Window != number_Received_Window_old)
    {
       number_Received_Window = number_Received_Window_old;
       number_Window_old = 0; 
    } 
    else 
    {
        number_Window_old++;
        if(number_Window_old == VALUE_RESET)
        {
            Window[0] = 0;
            Window[1] = 0;
            Window[2] = 0;
            Window[3] = 0;
            Window[4] = 0;
        }
    }


    //check if data from "interface" node is "new" if not reset it to default values
    if(number_Received_Interface != number_Received_Interface_old)
    {
       number_Received_Interface = number_Received_Interface_old;
       number_Interface_old = 0; 
    } 
    else 
    {
        number_Interface_old++;
        if(number_Interface_old == VALUE_RESET)
        {
            Interface[0] = 0;
            Interface[1] = DEFAULT_TEMPRATURE * 10;
            Interface[2] = 0;
            Interface[3] = 0;
            Interface[4] = 0;
        }
    }
}
