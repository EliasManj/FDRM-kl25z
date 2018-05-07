/*
 * flags.c
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */

#include "flags.h"

////////////
// Variables bandera globales (el código funcio gracias a estas variables bandera)
////////////

unsigned char turn_sys_on_f 		= 0;
unsigned char system_on_f 			= 0;

unsigned char turn_light_on_f 		= 0;
unsigned char light_on_f			= 0;

unsigned char sys_ready_f 			= 0;

unsigned char start_scan_f			= 0;

unsigned char start_deg_pos_f 		= 0;
unsigned char end_deg_pos_f 		= 0;
unsigned char start_trasl_pos_f 	= 0;
unsigned char end_trasl_pos_f 		= 0;

unsigned char door_ajar_f 			= 0; 

unsigned char ended_traslation_f	= 0;
unsigned char ended_rotation_f		= 1;
unsigned char problem_f				= 0;

unsigned int selecc_motor 			= 0;
unsigned char scanning_f			= 0;

unsigned int returning_f 			= 0;

unsigned int button_send_data_f	= 0;

unsigned int hola = 0;
