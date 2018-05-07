/*
 * 
 * Proyecto de Imagenología Médica
 * Tomógrafo computarizado por luz infraroja
 * 
 * #DontCrackUnderPressure
 * Octavio Rojas 
 * Abril 2018
 * 
 */

#include "derivative.h" 	/* Include peripheral declarations	*/
#include "adc.h"			/* ADC´s initialization				*/
#include "flags.h"			/* Flags used to run the program	*/
#include "instructions.h"	/* System's instructions			*/
#include "timers.h"			/* Timers' initialization			*/
#include "gpio.h"			/* GPIO configuration				*/
#include "uart.h"			/* UART initialization				*/
#include "constants.h"		/* All the constanst used			*/
#include "command.h"		/* Commands recognized by UART		*/
#include "buffer.h"			/* Buffer functions					*/
#include "lcd.h"			/* LCD functions					*/

////Buffers

bufferType Buffer_lcd = { 0, 0, BUFLEN, { } };

bufferType *lcd_bf;

bufferType Buffer_rx = { 0, 0, BUFLEN, { } };

bufferType *rx_bf;

bufferType Buffer_tx = { 0, 0, BUFLEN, { } };

bufferType *tx_bf;

bufferType Buffer_tx2 = { 0, 0, BUFLEN, { } };

bufferType *tx2_bf;




//Command

CommandString commandString = { CMDLEN, 0, 0, { } };

CommandString *commandString_p;

int main(void)
{
	
	lcd_bf = &Buffer_lcd;
	rx_bf = &Buffer_rx;

	tx_bf = &Buffer_tx;
	
	tx2_bf = &Buffer_tx2;
	
	commandString_p = &commandString;
	
	//---------------------------------------------------------------

	global_variables_init();

	global_modules_initializer();
	gpio_lcd_ports_init();
	
	lcd_initialize(lcd_bf);
	
	
	//light_on_off();

	while (1) 
	{
		
		
		//rx_status=1;
		if (rx_status == 1) 
		{
			
		rx_status = 0;

			if (commandString_p->is_full == 1) 
			{
				
			command_clear(commandString_p);
			
			} 
			else
			{
				
			cmd_code = command_compare_cmd(commandString_p);
			//cmd_code = 1;
			
				if (cmd_code != 0)
				{

				uart_send_done(tx_bf);

				}

				switch (cmd_code) 
				{

				case SYS_ON: //YA
					
					//Bandera para prender
					turn_sys_on_f = 1;
												
					system_on_off(tx_bf);
					
					break;

				case SYS_OFF: //YA

					//Bandera para apagar
					turn_sys_on_f = 0;
					
					system_on_off(tx_bf);
										
					break;

				case SYS_SET: //YA
					
					verify_system(tx_bf);
											
					break;

				case LIGHT_ON: //YA
					
					//Bandera que controla prender
					turn_light_on_f = 1;
					
					light_on_off(tx_bf);					
					
					break;

				case LIGHT_OFF: //YA

					//Bandera que controla apagar
					turn_light_on_f = 0;
					
					light_on_off(tx_bf);
					
					break;

				case SCAN_START: //YA
					
					send_Matlabisready(tx_bf);
					
					mili_arg = parse_scan_length(commandString_p); //Extrae la información de milímetros del string que recibe el UART
										
					degr_arg = parse_scan_angle(commandString_p);  //Extrae la información de grados del string que recibe el UART
					
					
					button_send_data_f=0;
					while(button_send_data_f==0)
					{
						button_send_data_f = (GPIOA_PDIR);
						button_send_data_f = (GPIOA_PDIR&(0x2000));
						button_send_data_f = (button_send_data_f>>13);
					}
					
																
						ADC_init();
																
						start_scan_f = 1;
						
						//Para pasarle los argumentos a esta función hay que parsear el string que reciba el UART 
						//a unsigned ints: milimeter y degrees.					
						
						start_scan(mili_arg, degr_arg, lcd_bf);
	
					
					
					
					break;
					
				case SCAN_STOP: //YA

					stop_scan(tx_bf);
					
					break;
									
				case RESTART: //YA

					restart_scan(tx_bf);
					
					break;				
					
				default:

					break;

				}

			}

			command_clear(commandString_p);

		}
		
	}
	return 0;	
}


//Esta es la única ISR que debe estar aquí por que se declaran los types que se ocupan en la ISR. Segun SO, no puedo pasar argumentos al handler
//porque están diseñados para que sean llamados por el hardware. No por el software.


void UART0_IRQHandler(void) 
{

	//WRITE

	if (((UART0_S1 & 0x80) >> 7) && (!buffer_isempty(tx_bf))) 
	{

		UART0_D = buffer_pop(tx_bf);

		if (buffer_isempty(tx_bf)) 
		{

			UART0_C2 &= ~(0x80);

		}

	}



	//READ

	if ((UART0_S1 & 0x20) >> 5 && !(buffer_isfull(rx_bf))) {

		uart_recive_value = UART0_D;

		buffer_push(tx_bf, uart_recive_value);

		if (uart_recive_value != CARR_RETURN) {

			command_add_item(commandString_p, uart_recive_value);

		} else {

			buffer_push(rx_bf, NEW_LINE);

			buffer_push(tx_bf, NEW_LINE);

			rx_status = 1;

		}

		UART0_C2 |= 0x80;	//Turn on TX interrupt

	}

}

void UART1_IRQHandler(void) 
{
	if (((UART1_S1 & 0x80) >> 7) && (!buffer_isempty(tx2_bf))) 
		{

			UART1_D = buffer_pop(tx2_bf);

			if (buffer_isempty(tx2_bf)) 
			{

				UART1_C2 &= ~(0x80);

			}

		}
}

void ADC0_IRQHandler(void)
{

	if ((ADC0_SC1A &(1<<7))==(1<<7))
	{
		unsigned int temp = (ADC0_RA); 
		
		//range: [0 255]
		

		
		
		
			
		if(timerStateReached==1)
		{
								
			//LPTMR0_CMR = 500; //.5seg
			
			ADC_reading = temp;
						
			buffer_push(tx_bf, ADC_reading);
			UART0_C2 |= 0x80;	//Turn on TX interrupt
			
			timerStateReached = 0;
				
			ADC_Interrup_event = 1;

		}

		ADC0_SC1A |=4;

	}

}

void LPTimer_IRQHandler() 
{

	LPTMR0_CSR |= (1 << 7);	//Clear timer compare flag

	timerStateReached = 1;

}


void TPM0_IRQHandler(void)
{

	TPM0_SC |= (1 << 7);

}

void FTM0_IRQHandler()
{

	TPM0_SC |= (1 << 7); 		//TOF clear interrupt flag

	TPM0_C2SC |= (1<<7);

	tmp_counter_50ms_tick(lcd_bf);

}
