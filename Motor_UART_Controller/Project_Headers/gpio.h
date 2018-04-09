/*
 * gpio.h
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */
#include "derivative.h"
#include "global_variables.h"
#ifndef GPIO_H_
#define GPIO_H_

void RGB_init(void);
void shift_rgb_leds(void);
void Ports_init(void);
void Set_timer_signal_GPIO(void);

#endif /* GPIO_H_ */
