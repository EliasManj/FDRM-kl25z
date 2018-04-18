/*
 * command.h
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */

#include "derivative.h"
#include "global_variables.h"
#ifndef COMMAND_H_
#define COMMAND_H_

//Define Command struct
struct Command {
	volatile uint8_t size;
	volatile uint8_t n_items;
	volatile uint8_t is_full;
	volatile char data[CMDLEN];
};

typedef struct Command CommandString;

uint8_t string_compare(volatile char *array1, volatile char *array2);
void strcopy(volatile char *dest, volatile char *source);
void command_add_item(CommandString *commandString, char item);
void command_clear(CommandString *commandString);
uint8_t command_compare_cmd_ugly(CommandString *commandString);
signed int parse_motor_angle(CommandString *commandString);
unsigned int parse_motor_velocity(CommandString *commandString);
unsigned int parse_temperature_limit(CommandString *commandString);
void get_motor_vel_string(volatile char *dest, CommandString *commandString);

#endif /* COMMAND_H_ */
