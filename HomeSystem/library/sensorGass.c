#include "sensorGass.h"
#include "ucglib/ucg.h"

#define offset 0;

// set up pins for the sensor
void init_adc_sensor_gass(void){
	PORTA.DIRCLR     = PIN0_bm;    
	
	ADCA.CH0->MUXCTRL = ADC_CH_MUXPOS_PIN0_gc | ADC_CH_MUXNEG_GND_MODE3_gc;
	ADCA.CH0->CTRL    = ADC_CH_INPUTMODE_DIFF_gc;
	
	ADCA.CTRLB       =	ADC_RESOLUTION_12BIT_gc |
	ADC_CONMODE_bm |
	ADC_FREERUN_bm; // free running mode

	ADCA.REFCTRL     =  ADC_REFSEL_INTVCC_gc;
	ADCA.PRESCALER   =  ADC_PRESCALER_DIV16_gc;
	ADCA.CTRLA       =  ADC_ENABLE_bm;
	ADCA.EVCTRL      =	ADC_SWEEP_0123_gc| // sweep ch. 0,1,2,3
						ADC_EVSEL_0123_gc| // default, no trigger only sweep
						ADC_EVACT_NONE_gc; // no internal or external trigger
}

// reads the sensor
int readSensor(){
	return ADCA.CH0.RES+offset;
}