/*
 * command.c
 *
 *  Created on: Apr 8, 2018
 *      Author: Octavio Rojas
 */
#include "command.h"
#include "constants.h"


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



uint8_t command_compare_cmd(CommandString *commandString) 
{

	//SYS:ON & SYS:OFF & SYS:SET

	if (commandString->data[0] == 'S' && 
		commandString->data[1] == 'Y' &&
		commandString->data[2] == 'S' && 
		commandString->data[3] == ':' ) {

	if (commandString->data[4] == 'O' && 
		commandString->data[5] == 'N' && 
		commandString->data[6] == 0) {

		return SYS_ON;} //1
	else if(commandString->data[4] == 'O' &&
			commandString->data[5] == 'F' &&
			commandString->data[6] == 'F') {

		return SYS_OFF;} //2
	
	else if(commandString->data[4] == 'S' &&
			commandString->data[5] == 'E' &&
			commandString->data[6] == 'T') {

		return SYS_SET;} //3

	else return 0;

	}

	//LIGHT:ON & LIGHT:OFF

	else if(commandString->data[0] == 'L' && 
			commandString->data[1] == 'I' &&
			commandString->data[2] == 'G' && 
			commandString->data[3] == 'H' &&
			commandString->data[4] == 'T' && 
			commandString->data[5] == ':' &&
			commandString->data[6] == 'O' ) {

		if (commandString->data[7] == 'N' &&  
			commandString->data[8] == 0) {

		return LIGHT_ON;} //4
	
	else if(commandString->data[7] == 'F' &&
			commandString->data[8] == 'F') {

		return LIGHT_OFF;}//5
	
	else return 0;

	}


	//SCAN_START & SCAN_STOP & SCAN_PAUSE & SCAN_NUM
	
	else if(commandString->data[0] == 'S' && 
			commandString->data[1] == 'C' &&
			commandString->data[2] == 'A' && 
			commandString->data[3] == 'N' &&
			commandString->data[4] == ':') {

		if (commandString->data[5] == 'S' && 
			commandString->data[6] == 'T' &&
			commandString->data[7] == 'A' && 
			commandString->data[8] == 'R' &&
			commandString->data[9] == 'T') {

		return SCAN_START;} //6
	
	else if(commandString->data[5] == 'S' && 
			commandString->data[6] == 'T' &&
			commandString->data[7] == 'O' && 
			commandString->data[8] == 'P') {

		return SCAN_STOP;} //7
	
	else if(commandString->data[5] == 'P' && 
			commandString->data[6] == 'A' &&
			commandString->data[7] == 'U' && 
			commandString->data[8] == 'S' &&
			commandString->data[9] == 'E') {

		return SCAN_PAUSE;} //8
	
	else if(commandString->data[5] == 'N' && 
			commandString->data[6] == 'U' &&
			commandString->data[7] == 'M') {

		return SCAN_NUM;} //9
					 
	else return 0;

	}
	
	//RESTART
	
	else if(commandString->data[0] == 'R' &&
			commandString->data[1] == 'E' &&
			commandString->data[2] == 'S' &&
			commandString->data[3] == 'T' &&
			commandString->data[4] == 'A' && 
			commandString->data[5] == 'R' && 
			commandString->data[6] == 'T' &&
			commandString->data[7] == 0)

				return RESTART; //10
	
	//Y
		else if (commandString->data[0] == 'Y' && commandString->data[1] == 0)

				return Y; //11
	//N
		else if (commandString->data[0] == 'N' && commandString->data[1] == 0)

				return N; //12
	
		else if(commandString->data[0] == 'M' &&
				commandString->data[1] == 'A' &&
				commandString->data[2] == 'T' &&
				commandString->data[3] == 'L' &&
				commandString->data[4] == 'A' && 
				commandString->data[5] == 'B' && 
				commandString->data[6] == ':' &&
				commandString->data[1] == 'R' &&
				commandString->data[2] == 'E' &&
				commandString->data[3] == 'A' &&
				commandString->data[4] == 'D' && 
				commandString->data[5] == 'Y' && 
				commandString->data[7] == 0)

					return MATLAB; //

}



signed int parse_motor_angle(CommandString *commandString) {

	unsigned long val = (unsigned long) (100 * (commandString->data[7] - 0x30)

			+ 10 * (commandString->data[8] - 0x30)

			+ (commandString->data[9] - 0x30));

	if (0 <= val && val <= 124)

		return 0;

	else if (125 <= val && val <= 249)

		return 1;

	else if (250 <= val && val <= 374)

		return 2;

	else if (375 <= val && val <= 499)

		return 3;

	else if (500 <= val && val <= 624)

		return 4;

	else if (625 <= val && val <= 749)

		return 5;

	else if (750 <= val && val <= 874)

		return 6;

	else if (875 <= val && val <= 999)

		return 7;

	else

		return -1;

}



unsigned int parse_motor_velocity(CommandString *commandString) {

	signed long val = (signed long) (100 * (commandString->data[4] - 0x30)

			+ 10 * (commandString->data[5] - 0x30)

			+ (commandString->data[7] - 0x30));

	val = (-49) * val + 49451;

	return (unsigned int) val;

}



unsigned int parse_temperature_limit(CommandString *commandString) {

	unsigned int val = (signed long) (1000 * (commandString->data[10] - 0x30)

			+ 100 * (commandString->data[11] - 0x30)

			+ 10 * (commandString->data[12] - 0x30)

			+ (commandString->data[14] - 0x30));

	return val;

}

