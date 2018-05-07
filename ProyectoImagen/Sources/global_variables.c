/*
 * global_varaibles.c
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "global_variables.h"
#include "gpio.h"
#include "adc.h"
#include "timers.h"
#include "uart.h"

void global_variables_init(void)
{

	rotation_sensor_1 		= 0;
	
	rotation_sensor_2 		= 0;
	
	timerStateReached 		= 0;

	uart_recive_value 		= 0;

	tmp_counter_50ms 		= 0;

	tmp_counter_1sec 		= 0;

	tmp_counter_5sec 		= 0;

	ADC_reading 			= 0;
	
	mili_arg 				= 0;

	degr_arg 				= 0;

	ADC_Interrup_event		= 0;
	
}
void global_modules_initializer(void) {

	Pin_init();

	LPTMR_init();
	
	TPM_init();

	//ADC_init();

	UART_init();
}
