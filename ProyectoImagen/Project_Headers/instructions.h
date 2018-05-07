/*
 * instrucctions.h
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */

#include "derivative.h"
#include "buffer.h"
#include "command.h"
#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

void system_on_off(bufferType *tx_bf);
void light_on_off(bufferType *tx_bf);
void verify_system(bufferType *tx_bf);
void move_motor_CCW(unsigned int num_motor, unsigned char until_one);
void move_motor_CW(unsigned int num_motor, unsigned char until_one);
void traslational_scan(void);
void rotational_scan(void);
void move_step (unsigned short steps, unsigned short motor, unsigned int delay_val);
void start_scan(unsigned int milimeters, unsigned int degrees,  bufferType *tx_bf);
void stop_scan(bufferType *tx_bf);
void restart_scan(bufferType *tx_bf);
void pause_scan(void);
void set_num_slices(unsigned short number);
unsigned int parse_scan_angle(CommandString *commandString);
unsigned int parse_scan_length(CommandString *commandString);
void check_sensors(void);
void delay (unsigned long time_ms);
void time_delay_ms(unsigned int count_val);

#endif /* INSTRUCTIONS_H_ */
