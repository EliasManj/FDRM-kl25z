/*
 * global_variables.h
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */
#include "derivative.h" 

#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_

//LCD FSM
#define IDLE			0
#define SET_RS			1
#define DATA_ENABLE_SET	2
#define ENABLE_CLEAR	3
#define DATA_CLEAR		4

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
volatile unsigned char motor_vel_string[3];

//TMP Timer
unsigned int tmp_counter_10ms;
unsigned int tmp_counter_1sec;
unsigned int tmp_counter_5sec;

//LCD
unsigned int current_lcd_state;
unsigned char lcd_en;
unsigned char lcd_rs;
unsigned char lcd_db7;
unsigned char lcd_db6;
unsigned char lcd_db5;
unsigned char lcd_db4;
unsigned char buffer_almost;
char lcd_data;
unsigned char lcd_rs_was_set;

void global_variables_initializer(void);

#endif /* GLOBAL_VARIABLES_H_ */
