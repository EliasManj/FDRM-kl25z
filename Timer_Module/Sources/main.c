/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

void timerInit(void);

int main(void) {
	int counter = 0;

	for (;;) {
		counter++;
	}

	return 0;
}

void timerInit(void){
	SIM_SCGC6 |= (1<<24);		//CLK TMP0
	SIM_SCGC5 |= (1<<13);		//PTE29?
	PORTE_PCR29 = (3<<8);		//TMP0_CH2
	MCG_C1 |= (1<<1);			//IRCLKEN
	MCG_C2 |= 1;				//IRCS = 1 (Fast clock) 4MHz
	MCG_SC = (2<<1);			//FCRDIV = 4, divide by 4 = 1MHz
}
