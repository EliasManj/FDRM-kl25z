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
	timerStateReached = 0;
	motorSequenceIndex = 0;
	motor_free_running_flag = 0;
	motor_dir_flag = 1;
	uart_recive_value = 0;
	motor_manual_angle_flag = 0;
	tmp_counter_50ms = 0;
	tmp_counter_1sec = 0;
	tmp_counter_5sec = 0;
	temperature = 0;
	temperature_limit = 9999;
	current_angle = 0;
}
