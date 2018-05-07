/*
 * global_variables.h
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "derivative.h"
#include "constants.h"
#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_


//Indicators

int cmd_code;

int rx_status;

char command[BUFLEN];

char uart_recive_value;

unsigned int rotation_sensor_1;
	
unsigned int rotation_sensor_2;
	
unsigned char timerStateReached;

unsigned int ADC_reading;

unsigned char adc_string[3];

unsigned char reading_string[4];

unsigned char ADC_Interrup_event;




unsigned int mili_arg;

unsigned int degr_arg;


//TMP Timer

unsigned int tmp_counter_50ms;

unsigned int tmp_counter_1sec;

unsigned int tmp_counter_5sec;



void global_variables_init(void);

void global_modules_initializer(void);


#endif /* GLOBAL_VARIABLES_H_ */
