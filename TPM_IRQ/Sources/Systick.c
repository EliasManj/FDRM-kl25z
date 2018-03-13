#include "Derivative.h"
#include "Systick.h"
#include "mcg.h"

static volatile unsigned int DelayTimerTick = 0;

volatile unsigned int Ticker[4];

//Since this SysTick is part of the Cortex M0 Core,   you need to look in the 
//Cortex M0 Generic Users Guide (ARM DUI 0497A)

//See Section 4.4
void InitSysTick()
{
	
	
	SYST_RVR = CORE_CLOCK/SYSTICK_FREQUENCY; // Set the Reload to the interrupt at our desired frequency
	SYST_CVR = 0; // Reset the Current Value
	SYST_CSR = SysTick_CSR_ENABLE_MASK | SysTick_CSR_TICKINT_MASK | SysTick_CSR_CLKSOURCE_MASK;

	//Important!  Since the Systick is part of the Cortex core and NOT a kinetis peripheral
	// its Interrupt line is not passed through NVIC.   You need to make sure that
	//the SysTickIRQ function is populated in the vector table.  See the kinetis_sysinit.c file
}

void SysTick_Handler()
{
	unsigned int i;
	
	if(DelayTimerTick<0xFFFFFFFF)
	{
		DelayTimerTick++;
	}
	
	for(i=0;i<4;i++)
	{
		if(Ticker[i]<0xFFFFFFFF)
			{
				Ticker[i]++;
			}
	}
}

void Delay_mS(unsigned int TicksIn_mS)
{
	DelayTimerTick = 0;

	while(DelayTimerTick<TicksIn_mS)
	{
	}
}
