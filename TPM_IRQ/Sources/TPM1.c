#include "TPM1.h"
#include "stdint.h"
#include "derivative.h"
#include "arm_cm0.h"

#define FTM1_CLOCK                                                                  (48000000)
#define FTM1_CLK_PRESCALE                                  						     6// Prescale Selector value - see comments in Status Control (SC) section for more details
#define FTM1_OVERFLOW_FREQUENCY 50  											  	// Desired Frequency of PWM Signal - Here 50Hz => 20ms period
																				    // use these to dial in servo steering to your particular servo

volatile uint32_t TPM1_Ticker = 0;


void FTM1_IRQHandler()
{
        //Clear the overflow mask if set.   According to the reference manual, we clear by writing a logic one!
		if(TPM1_SC & TPM_SC_TOF_MASK)
		{
			TPM1_SC |= TPM_SC_TOF_MASK;
		}
		
		if(TPM1_Ticker<0xFFFFFFFF)
			TPM1_Ticker++;
}

void InitTPM1()
{
               
               //Clock Setup for the TPM requires a couple steps.
               
   			
               //1st,  set the clock mux
               //See Page 124 of f the KL25 Sub-Family Reference Manual, Rev. 3, September 2012
               SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;// We Want MCGPLLCLK/2 (See Page 196 of the KL25 Sub-Family Reference Manual, Rev. 3, September 2012)
               SIM_SOPT2 &= ~(SIM_SOPT2_TPMSRC_MASK);
               SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); //We want the MCGPLLCLK/2 (See Page 196 of the KL25 Sub-Family Reference Manual, Rev. 3, September 2012)
  

				//Enable the Clock to the FTM1 Module
				//See Page 207 of f the KL25 Sub-Family Reference Manual, Rev. 3, September 2012
				SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK; 
               
               //The TPM Module has Clock.  Now set up the peripheral
               
               //Blow away the control registers to ensure that the counter is not running
               TPM1_SC = 0;
               TPM1_CONF = 0;
               
               //While the counter is disabled we can setup the prescaler
               
               TPM1_SC = TPM_SC_PS(FTM1_CLK_PRESCALE);
               TPM1_SC |= TPM_SC_TOIE_MASK; //Enable Interrupts for the Timer Overflow
               
               //Setup the mod register to get the correct  Period
               
               TPM1_MOD = FTM1_CLOCK/(1<<(FTM1_CLK_PRESCALE+1))/FTM1_OVERFLOW_FREQUENCY;
               
               //Enable the TPM COunter
               TPM1_SC |= TPM_SC_CMOD(1);
               
               //Enable TPM1 IRQ on the NVIC
               enable_irq (INT_TPM1-16);
              
                              
}

