/*
 * Systick.h
 *
 *  Created on: Jan 13, 2013
 *      Author: plexu_000
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#define SYSTICK_FREQUENCY	1000

void Delay_mS(unsigned int TicksIn_mS);
void InitSysTick();

extern volatile unsigned int Ticker[4];

#endif /* SYSTICK_H_ */
