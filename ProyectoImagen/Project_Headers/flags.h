/*
 * flags.h
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "derivative.h"
#ifndef FLAGS_H_
#define FLAGS_H_

////////////
// Variables bandera globales (el código funcio gracias a estas variables bandera)
////////////

unsigned char turn_sys_on_f;
unsigned char system_on_f;

unsigned char turn_light_on_f;
unsigned char light_on_f;

unsigned char sys_ready_f;

unsigned char start_scan_f;

unsigned char start_deg_pos_f;
unsigned char end_deg_pos_f;
unsigned char start_trasl_pos_f;
unsigned char end_trasl_pos_f;
unsigned char door_ajar_f;

unsigned char ended_traslation_f;
unsigned char ended_rotation_f;
unsigned char problem_f;	

unsigned int button_send_data_f;

unsigned int returning_f;

unsigned int hola;


unsigned int selecc_motor;


#endif /* FLAGS_H_ */
