/*
 * adc.c
 *
 *  Created on: Apr 22, 2018
 *      Author: Elias g
 */
#include "derivative.h" 
#include "adc.h" 

void ADC_init(void) {
	SIM_SCGC6 |= (1 << 27);
	SIM_SCGC5 |= (1 << 13);
	NVIC_ICPR |= (1 << 15);
	NVIC_ISER |= (1 << 15);
	//ADC0
	ADC0_CFG1 = 0x00000000;
	ADC0_CFG2 = (0 << 4);		//Seleccionar el canal A del ADC
	ADC0_SC1A =0b1000100;//Habilitar interrupciones del ADC y el canal AD4-> esta en el canal PTE21
}


