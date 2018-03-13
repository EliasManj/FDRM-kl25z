/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "derivative.h" /* include peripheral declarations */
#include "mcg.h"
#include "Systick.h"
#include "Freedom.h"
#include "TPM1.h"


int main(void)
{
	
	pll_init(8000000, LOW_POWER, CRYSTAL,4,24,MCGOUT);
	//Core Clock is now at 48MHz using the 8MHZ Crystal

	InitFreedom(); //This initializes the PORT I/O for the LED, etc on the Freedom board.
	
	InitSysTick(); // Systick is now interrupting once a mS!
	
	InitTPM1();
	
	for(;;)
	{	   
		//TPM1 Ticker fires every 20mS.  50 Ticks means One seconds
		if(TPM1_Ticker >= 50)
		{
			TPM1_Ticker =  0; //time to do something!  Reset the timing variable....
			RED_LED_TOGGLE;
		}
	}
	
	return 0;
}
