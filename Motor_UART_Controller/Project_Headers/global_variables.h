/*
 * global_variables.h
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */
#include "derivative.h" 

#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_

//ASCII codes
#define BUFLEN 		50
#define CMDLEN		18
#define NEW_LINE 	0x0A
#define CARR_RETURN 0x0D
#define BACKSPACE 	0x08
#define SPACE 	0x32

//Commands encoding
#define DIR_CW 		1
#define DIR_CCW		2
#define MOTOR_ON 	3
#define MOTOR_OFF 	4
#define STEP_CW		5
#define TEMPLIMIT 	6
#define RPS			7

//Define variables
int rx_status;
unsigned int motor_vel;
char command[BUFLEN];
char uart_recive_value;
int cmd_code;
unsigned long LEDs[3];
unsigned long led_temp;
signed int motor_angle;

//Indicators
unsigned int motor_free_running_flag;
unsigned int motor_dir_flag;
unsigned int motor_manual_angle_flag;
unsigned char timerStateReached;
signed char motorSequenceIndex;
unsigned int temperature;
unsigned int temperature_limit;
unsigned char temperature_string[4];
unsigned int current_angle;

//TMP Timer
unsigned int tmp_counter_10ms;
unsigned int tmp_counter_1sec;
unsigned int tmp_counter_5sec;

void global_variables_initializer(void);

#endif /* GLOBAL_VARIABLES_H_ */
