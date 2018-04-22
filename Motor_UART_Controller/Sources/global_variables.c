/*
 * global_variables.c
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */

#include "global_variables.h"

//Define global initializers
void global_variables_initializer(void) {
	LEDs[0] = 0;
	LEDs[1] = 1;
	LEDs[2] = 1;
	motor_vel_string[0] = '0';
	motor_vel_string[1] = '0';
	motor_vel_string[2] = '0';
	timerStateReached = 0;
	motorSequenceIndex = 0;
	motor_free_running_flag = 0;
	motor_dir_flag = 1;
	uart_recive_value = 0;
	motor_manual_angle_flag = 0;
	tmp_counter_10ms = 0;
	tmp_counter_1sec = 0;
	tmp_counter_5sec = 0;
	temperature = 0;
	temperature_limit = 9999;
	current_angle = 0;
	motor_vel = 0;
	//LCD
	current_lcd_state = IDLE;
	lcd_en = 0;
	lcd_rs = 0;
	lcd_db7 = 0;
	lcd_db6 = 0;
	lcd_db5 = 0;
	lcd_db4 = 0;
	buffer_almost = 0;
	lcd_rs_was_set = 0;
}
