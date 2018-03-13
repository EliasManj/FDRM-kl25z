/*
 * Freedom.c
 *
 *  Created on: Mar 19, 2013
 *      Author: ehughes
 */
#include "Derivative.h"
#include "Freedom.h"
#include "derivative.h"

void InitFreedom()
{

	//First order of business is to enable the Clocks to the ports!
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	
	//Now,  setup the port mux for GPIO! See Page 163 and 183 of KL25 Sub-Family Reference Manual, Rev. 3, September 2012 
	PORTB_PCR18 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;
	PORTB_PCR19 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;
	PORTD_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;
	
	
	//Set the GPIO to outputs in the data direction register
	//See Page 778 of KL25 Sub-Family Reference Manual, Rev. 3, September 2012 
	GPIOB_PDDR |=  RED_LED_LOC | GREEN_LED_LOC;
	GPIOD_PDDR |=  BLUE_LED_LOC;
	
	BLUE_LED_OFF;
	GREEN_LED_OFF;
	RED_LED_OFF;
	
}
