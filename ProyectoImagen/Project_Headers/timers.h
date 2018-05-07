/*
 * timers.h
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "derivative.h"
#include "constants.h"
#include "buffer.h"
#ifndef TIMERS_H_
#define TIMERS_H_

void LPTMR_init(void);
void TPM_init(void);
void tmp_counter_50ms_tick(bufferType *lcd_bf);
void tmp_counter_1sec_tick();
void tmp_counter_5sec_tick();

#endif /* TIMERS_H_ */
