/*
 * command.c
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */

#include "command.h"

uint8_t string_compare(volatile char *array1, volatile char *array2) {
	int i;
	int response = 0;
	i = 0;
	while (array1[i] == array2[i] && response == 0) {
		if (array1[i] == '\0' || array2[i] == '\0') {
			response = 1;
		}
		i++;
	}
	return response;
}

void strcopy(volatile char *dest, volatile char *source) {
	char data;
	do {
		data = *dest++ = *source++;
	} while (data);
}

void command_add_item(CommandString *commandString, char item) {
	if (commandString->is_full == 0) {
		commandString->data[commandString->n_items++] = item;
	}
	if (commandString->n_items >= (commandString->size - 1)) {
		commandString->is_full = 1;
	}
}

void command_clear(CommandString *commandString) {
	uint8_t i = 0;
	while (i < commandString->size) {
		commandString->data[i++] = 0;
	}
	commandString->n_items = 0;
	commandString->is_full = 0;
}

uint8_t command_compare_cmd_ugly(CommandString *commandString) {
	//DIR:
	if (commandString->data[0] == 'D' && commandString->data[1] == 'I'
			&& commandString->data[2] == 'R' && commandString->data[3] == ':'
			&& commandString->data[4] == 'C') {
		if (commandString->data[5] == 'W' && commandString->data[6] == 0) {
			return DIR_CW;
		} else if (commandString->data[5] == 'C'
				&& commandString->data[6] == 'W') {
			return DIR_CCW;
		} else
			return 0;
	}
	//MOTOR
	else if (commandString->data[0] == 'M' && commandString->data[1] == 'O'
			&& commandString->data[2] == 'T' && commandString->data[3] == 'O'
			&& commandString->data[4] == 'R' && commandString->data[5] == ':'
			&& commandString->data[6] == 'O') {
		if (commandString->data[7] == 'N' && commandString->data[8] == 0) {
			return MOTOR_ON;
		} else if (commandString->data[7] == 'F'
				&& commandString->data[8] == 'F'
				&& commandString->data[9] == 0) {
			return MOTOR_OFF;
		} else
			return 0;
	}
	//STEPCW
	else if (commandString->data[0] == 'S' && commandString->data[1] == 'T'
			&& commandString->data[2] == 'E' && commandString->data[3] == 'P'
			&& commandString->data[4] == 'C' && commandString->data[5] == 'W'
			&& commandString->data[6] == ':' && commandString->data[10] == 0
			&& commandString->n_items < 11) {
		return STEP_CW;
	}
	//TEMPLIMIT
	else if (commandString->data[0] == 'T' && commandString->data[1] == 'E'
			&& commandString->data[2] == 'M' && commandString->data[3] == 'P'
			&& commandString->data[4] == 'L' && commandString->data[5] == 'I'
			&& commandString->data[6] == 'M' && commandString->data[7] == 'I'
			&& commandString->data[8] == 'T' && commandString->data[9] == ':'
			&& commandString->data[15] == 0) {
		return TEMPLIMIT;
	}
	//RPS
	else if (commandString->data[0] == 'R' && commandString->data[1] == 'P'
			&& commandString->data[2] == 'S' && commandString->data[3] == ':'
			&& commandString->data[6] == '.' && commandString->data[8] == 0
			&& commandString->n_items < 9)
		return RPS;
	return 0;
}

signed int parse_motor_angle(CommandString *commandString) {
	unsigned long val = (unsigned long) (100 * (commandString->data[7] - 0x30)
			+ 10 * (commandString->data[8] - 0x30)
			+ (commandString->data[9] - 0x30));
	val = (val*96)/999;
	if(val==0)return 0;
	return val;
}

unsigned int parse_motor_velocity(CommandString *commandString) {
	unsigned long val = (signed long) (100 * (commandString->data[4] - 0x30)
			+ 10 * (commandString->data[5] - 0x30)
			+ (commandString->data[7] - 0x30));
	val = (625*10)/val;
	return (unsigned int) val;
}

unsigned int parse_temperature_limit(CommandString *commandString) {
	unsigned int val = (signed long) (1000 * (commandString->data[10] - 0x30)
			+ 100 * (commandString->data[11] - 0x30)
			+ 10 * (commandString->data[12] - 0x30)
			+ (commandString->data[14] - 0x30));
	return val;
}

void get_motor_vel_string(volatile unsigned char *dest, CommandString *commandString){
	dest[2] = commandString->data[4];
	dest[1] = commandString->data[5];
	dest[0] = commandString->data[7];
}



