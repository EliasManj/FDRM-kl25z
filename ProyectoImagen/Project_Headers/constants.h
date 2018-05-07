/*
 * constants.h
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "derivative.h"
#ifndef CONSTANTS_H_
#define CONSTANTS_H_

////////////
//Constantes
////////////

//Códigos de motor

#define DEGREE_MOTOR	1

#define TRASL_MOTOR		2

//Delays constantes
#define SUPERSMALL		1

#define SMALL			50 //0b110010 

#define MEDIUM			200

#define BIG				500

//Milìmetro

#define ONE_MM			143 //143 pasos son 1 mm usando el sin fín actual

#define	ONE_STEP		1				

//Códigos ASCII

#define BUFLEN 			100

#define CMDLEN			30

#define NEW_LINE 		0x0A

#define CARR_RETURN 	0x0D

#define BACKSPACE 		0x08

#define SPACE 			0x32

//Códigos de comandos

#define SYS_ON 			1

#define SYS_OFF 		2
	
#define SYS_SET 		3

#define LIGHT_ON 		4

#define LIGHT_OFF 		5

#define SCAN_START 		6

#define SCAN_STOP 		7

#define SCAN_PAUSE 		8

#define SCAN_NUM 		9

#define RESTART			10

#define Y				11

#define N				12

#define MATLAB			13

//LCD display, estados de la FSM

#define IDLE			0

#define SET_RS			1

#define DATA_ENABLE_SET	2

#define ENABLE_CLEAR	3

#define DATA_CLEAR		4



void Constants_init(void);

#endif /* CONSTANTS_H_ */
