/*
 * adc.c 
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 *      Functions that allow usage of ADC0
 */

#include "adc.h"
#include "global_variables.h"
	
void ADC_init(void)
{
	//Inicialización del ADC

	SIM_SCGC6 |= (1 << 27);

	NVIC_ICPR |= (1 << 15);

	NVIC_ISER |= (1 << 15);

	//ADC0, el channel se tiene que cambiar para leer de un pin o de otro. 
	// ADC0_SE15 C1 default ADCH=01111 SELECCIONADO
	// ADC0_SE11 C2 default ADCH=01011

	ADC0_CFG1 = 0x00000000;
	
	ADC0_CFG2 = (0 << 4);		//Seleccionar el canal A del ADC

	ADC0_SC1A = 0b1000000;		//Habilitar interrupciones del ADC
	
	ADC0_SC1A |= 0b01111;  
}

void scan_laser(void)
{
	//PORTE_PCR29 = (1<<8); PTE29 GPIO láser 1
	timerStateReached = 1;
	while (ADC_Interrup_event != 1)
	{}
	ADC_Interrup_event = 0;
	//Apager led láser PORTE_PCR29
}


