/*
 * Freedom.h
 *
 *  Created on: Mar 19, 2013
 *      Author: ehughes
 */

#ifndef FREEDOM_H_
#define FREEDOM_H_

#define RED_LED_LOC		(1<<18)
#define GREEN_LED_LOC	(1<<19)
#define BLUE_LED_LOC	(1<<1)

#define RED_LED_OFF 		GPIOB_PSOR = RED_LED_LOC
#define RED_LED_ON   		GPIOB_PCOR = RED_LED_LOC
#define RED_LED_TOGGLE  	GPIOB_PTOR = RED_LED_LOC


#define GREEN_LED_OFF 		GPIOB_PSOR = GREEN_LED_LOC
#define GREEN_LED_ON   		GPIOB_PCOR = GREEN_LED_LOC
#define GREEN_LED_TOGGLE   	GPIOB_PTOR = GREEN_LED_LOC

#define BLUE_LED_OFF		GPIOD_PSOR = BLUE_LED_LOC
#define BLUE_LED_ON  		GPIOD_PCOR = BLUE_LED_LOC
#define BLUE_LED_TOGGLE   	GPIOD_PTOR = BLUE_LED_LOC

void InitFreedom();


#endif /* FREEDOM_H_ */
