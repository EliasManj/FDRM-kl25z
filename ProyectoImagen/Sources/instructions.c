/*
 * instrucctions.c
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "derivative.h"
#include "instructions.h"
#include "command.h"
#include "flags.h"
#include "buffer.h"
#include "adc.h"

	void system_on_off(bufferType *tx_bf)	//YA
	//activa un transistor de potencia que a su vez encience el sistema mecánico
	//La lògica ahorita es postiva 1==prendido
	//Probablemente para la implementación en físico se requiera de lógica inversa. 
	{
		if(turn_sys_on_f==1)
		{
			//Prender
			if(system_on_f==1)
			{
				//Mandar por UART system already on;
				uart_send_sysalreadyon(tx_bf);
				
			}
			else 
			{
				if(system_on_f==0)
				{
				//PIN_IO = 1; PTD07 GPIO Transistor de encendido y apagado
				GPIOD_PDOR|=(1<<7);
				system_on_f = 1;	
				}
			}
		}
		else
		{	
			//Apagar
			if(system_on_f==0)
			{
				//Mandar por UART system already off;
				uart_send_sysalreadyoff(tx_bf);
				
			}
			else 
			{
				if(system_on_f==1)
				{
				//PIN_IO=0; PTD07 GPIO Transistor de encendido y apagado
				GPIOD_PDOR|=(0<<7);
				system_on_f = 0;
				sys_ready_f = 0;
				
				}
			}
		}
		
		turn_sys_on_f = 0;
	}

	void light_on_off(bufferType *tx_bf)	//YA
	{
		//activa un transistor que a su vez enciende la luz
			{
				if(turn_light_on_f==1)
				{
					//Prender
					if(light_on_f==1)
					{
						//Mandar por UART light already on;
						uart_send_lightalreadyon(tx_bf);
					}
					else 
					{
						if(light_on_f==0)
						{
						//PIN_IO = 1; PTD06 GPIO Luces
						GPIOD_PDOR|=(1<<6);
						light_on_f = 1;	
						sys_ready_f = 0;
						
						}
					}
				}
				else
				{	
					//Apagar
					if(light_on_f==0)
					{
						//Mandar por UART light already off;
						uart_send_lightalreadyoff(tx_bf);
					}
					else 
					{
						if(light_on_f==1)
						{
						//PIN_IO=0; PTD06 GPIO Luces
						GPIOD_PDOR|=(0<<6);
						light_on_f = 0;
						
						}
					}
				}
			}
			
			turn_light_on_f = 0;
			
	}

	void verify_system(bufferType *tx_bf) 	//YA sólo falta verificar las direcciones de los motores
	{
		/* sys_ready_f;
		* start_deg_pos_f;
		* end_deg_pos_f;
		* start_trasl_pos_f;
		* end_trals_pos_f;
		* door_ajar_f;
		* system_on_f
		* light_on_f
		* */
		sys_ready_f = 0;
		system_on_f = 1;
		door_ajar_f = 0;
		light_on_f = 0;
		if( (system_on_f) & (!door_ajar_f) & (!light_on_f) )
		{
			check_sensors();
			
			start_deg_pos_f = 1;
			end_deg_pos_f = 0;
			if((start_deg_pos_f) & (!end_deg_pos_f))
			{
				start_trasl_pos_f = 1;
				end_trasl_pos_f = 0;
				if((start_trasl_pos_f) & (!end_trasl_pos_f))
				{
					//Mandar por UART "READY"
					uart_send_ready(tx_bf);
					sys_ready_f=1;
					//Preguntar Start?
					uart_send_startq(tx_bf);
					//Este es el caso ideal en el que to-do está en la posición correcta
					//y el sistema manda el mensaje de READY
					check_sensors();
					
					//
					system_on_f = 1;
					door_ajar_f = 0;
					light_on_f = 0;
					/*start_deg_pos_f = 1;
					end_deg_pos_f = 0;
					start_trasl_pos_f = 1;
					end_trasl_pos_f = 0;*/
					
				}
				else if((start_trasl_pos_f) && (!end_trasl_pos_f)==0)
				{
					//Mandar por UART "NOT READY"
					uart_send_notready(tx_bf);
					sys_ready_f = 0;
					//Mover_el_motor_CW(2,start_trasl_pos_f);
					move_motor_CW(TRASL_MOTOR, start_trasl_pos_f);
					sys_ready_f = 1;
					check_sensors();
					if (sys_ready_f)
					{
						//Mandar por UART READY
						uart_send_ready(tx_bf);
						//Preguntar Start?
						uart_send_startq(tx_bf);
						//Este es el caso en el que la posición de traslación estaba incorrecta
						//el sistema lo corrige y luego manda READY
					}
				}
				
			}
			else if((start_deg_pos_f) && (!end_deg_pos_f)==0) 
			{
				//Mandar por UART "NOT READY"
				uart_send_notready(tx_bf);
				sys_ready_f = 0;
				//Mover_el_motor_CW(1,start_deg_pos_f);
				move_motor_CW(DEGREE_MOTOR, start_deg_pos_f);
				
				check_sensors();
				if((start_trasl_pos_f)&&(!end_trasl_pos_f)==1)
				{
					//Mandar por UART "READY"
					uart_send_ready(tx_bf);
					sys_ready_f = 1;
					//Preguntar Start?
					uart_send_startq(tx_bf);
				}
				else if((start_trasl_pos_f)&&(!end_trasl_pos_f)==0)
				{
					//Mandar por UART "NOT READY"
					uart_send_notready(tx_bf);
					sys_ready_f = 0;
					//Mover_el_motor_CW(2,start_trasl_pos_f);
					move_motor_CW(TRASL_MOTOR, start_trasl_pos_f);
					sys_ready_f = 1;
					check_sensors();
				}
					else if (sys_ready_f) 
					{
						//Mandar por UART "READY"
						uart_send_ready(tx_bf);
						//Preguntar Start?
						uart_send_startq(tx_bf);
					}
			}
			else if(sys_ready_f)
			{
				sys_ready_f = 1;
				//Mandar por UART "READY"
				uart_send_ready(tx_bf);
				//Preguntar Start?
				uart_send_startq(tx_bf);
			}
		}
		else if(system_on_f)
		{
			//Mandar por UART "SYS IS OFF"
			uart_send_sysisoff(tx_bf);
			sys_ready_f = 0;
		}
		else if(!door_ajar_f)
		{
			//Mandar por UART "DOOR AJAR"
			uart_send_doorajar(tx_bf);
			sys_ready_f = 0;
		}
		else if(!light_on_f)
		{
			//Mandar por UART "LIGHT IS ON"
			uart_send_lightison(tx_bf);
			sys_ready_f = 0;
		}
	}
	
	void move_motor_CCW(unsigned int num_motor, unsigned char until_one) //YA
	{
		/*
		  	PORTB_PCR0 =(1<<8); 	//PTB0 GPIO CW+

			PORTB_PCR1 =(1<<8); 	//PTB1 GPIO CLK+
			
			#define DEGREE_MOTOR	1
			
			PORTB_PCR8 =(1<<8);		//PTB0 GPIO CW+
	
			PORTB_PCR9 =(1<<8);		//PTB1 GPIO CLK+
			
			#define TRASL_MOTOR		2
			
		 */
		unsigned int num_of_steps = 0;
		
		check_sensors();
		
		if(num_motor == DEGREE_MOTOR)
		{		
			if (until_one==0)
			{
				//PinDIR = 0/1; depende
				
				GPIOB_PDOR |= (1<<0); //Selecciona la dirección 1
				
				if(until_one==0)
				{
				check_sensors();
				//PinIO=1;
				GPIOB_PDOR|=(1<<1);
				//small_delay(); //queremos que el setting sea rápido
				delay(SMALL);
				//PinIO=0;
				GPIOB_PDOR|=(0<<1);
				num_of_steps++;
				}
			}
		}
		else if(num_motor == TRASL_MOTOR)
		{
			if (until_one==0)
			{
				//PinDIR = 0/1; depende
				
				GPIOB_PDOR |= (1<<8); //Selecciona la dirección 1
				
				if(until_one==0)
				{
				check_sensors();
				//PinIO=1;
				GPIOB_PDOR|=(1<<9);
				//small_delay(); //queremos que el setting sea rápido
				delay(SMALL);
				//PinIO=0;
				GPIOB_PDOR|=(0<<9);
				num_of_steps++;		
				}
			}
		}
			
		
	}
	
	void move_motor_CW(unsigned int num_motor, unsigned char until_one)	 //YA

	{
		/*
			PORTB_PCR0 =(1<<8); 	//PTB0 GPIO CW+

			PORTB_PCR1 =(1<<8); 	//PTB1 GPIO CLK+
			
			#define DEGREE_MOTOR	1
			
			PORTB_PCR8 =(1<<8);		//PTB0 GPIO CW+
	
			PORTB_PCR9 =(1<<8);		//PTB1 GPIO CLK+
			
			#define TRASL_MOTOR		2
					
		*/
		unsigned int num_of_steps = 0;
		
		check_sensors();
		
		if(num_motor == DEGREE_MOTOR)
		{		
			if (until_one==0)
			{
				//PinDIR = 0/1; depende
				
				GPIOB_PDOR |= (0<<0); //Selecciona la dirección 0
				
				if(until_one==0)
				{
				check_sensors();	
				//PinIO=1;
				GPIOB_PDOR|=(1<<1);
				//queremos que el setting sea rápido
				delay(SMALL);
				//PinIO=0;
				GPIOB_PDOR|=(0<<1);
				num_of_steps++;
				}
			}
		}
		else if(num_motor == TRASL_MOTOR)
		{
			if (until_one==0)
			{
				//PinDIR = 0/1; depende
				
				GPIOB_PDOR |= (0<<8); //Selecciona la dirección 0
				
				if(until_one==0)
				{
				check_sensors();
				//PinIO=0;
				GPIOB_PDOR|=(1<<9);
				//queremos que el setting sea rápido
				delay(SMALL);
				//PinIO=1;
				GPIOB_PDOR|=(0<<9);
				num_of_steps++;		
				}
			}
		}
	}
	
	void traslational_scan(void)// finalmente tal vez no la use. 
	{
		/*Variables involucradas:
		 * motor2_on
		 * sys_ready_f
		 * start_trasl_pos_f
		 * end_trals_pos_f
		 * num_steps_trasl
		 * ended_traslation
		 * */
		int num_steps_traslation = 0;
		if((start_trasl_pos_f &! end_trasl_pos_f))
		{
			if(num_steps_traslation <= ONE_MM) //1mm =143 steps
			{
				//Mandar por UART("Scanning #/60")
				//move_step(1,1); //Checar que sí sea el valor del motor
				num_steps_traslation++;
				ended_traslation_f = 0;
			}
			
		}
		else if(num_steps_traslation > ONE_MM)
		{
			num_steps_traslation = 0;
			
			ended_traslation_f = 1;
			
			//Mandar por UART("milímetro ## terminado")
			
		}
	}

	void rotational_scan(void)// finalmente tal vez no la use.
	{
		/*Variables involucradas:
		 * motor1_on
		 * sys_ready_f
		 * start_deg_pos_f
		 * end_deg_pos_f
		 */
		int num_steps_rotation = 0;
		
			if((start_deg_pos_f &! end_deg_pos_f))
			{
				if(num_steps_rotation <= 180) //1mm
				{
					//Mandar por UART("Scanning #/60")
					//move_step(1,2);
					num_steps_rotation++;
					ended_rotation_f = 0;
				}
				
			}
			else if(num_steps_rotation > 143)
			{
				num_steps_rotation = 0;
				
				ended_rotation_f = 1;
				
				//Mandar por UART("Rotación ## terminado");
				
			}
	}

	void move_step (unsigned short steps, unsigned short motor, unsigned int delay_val) //YA
	{
		unsigned int step_counter;
		step_counter =0;
		unsigned char j = 0;
		if(motor == DEGREE_MOTOR) //motor de angulo
		{
			for (j=0; j<= (steps-1); j++)
			{
				//PIN IO =1;
				GPIOB_PDOR|=(1<<1); //PTB1
				//delay
				//delay(delay_val);
				time_delay_ms(delay_val);
				//PIN IO =0;
				//PTB1
				GPIOB_PDOR ^= ((-(0)) ^ GPIOB_PDOR ) & (1UL << 1); //PTB1
				//delay
				//delay(delay_val);
				time_delay_ms(delay_val);
				step_counter++;
			}
		}
		else if( motor == TRASL_MOTOR) //Motor de traslación
		{
			for (j=0; j<= (steps-1); j++)
			{
				//PIN IO =1;
				GPIOB_PDOR|=(1<<9); //PTB9
				//delay;
				//delay(delay_val);
				time_delay_ms(delay_val);
				//PIN IO =1;
			 
				
				GPIOB_PDOR ^= ((-(0)) ^ GPIOB_PDOR ) & (1UL << 9); ////PTB9
				//delay;
				//delay(delay_val);
				time_delay_ms(delay_val);
				step_counter++;
			}
		}		
	}

	/*
	 * Ya puedo según... prender el sistema, puedo prender y apagar el foquito, puedo verificar el sistema
	 * puedo hacer que los motores se muevan en sentido horario o en sentido antihorario
	 * Puedo iniciar dos tipos de escaneo, rotacional y traslacional. 
	 * Puedo hacer que se mueva el motor 1 paso. 
	 * Hace falta consolidar la interacción de todas las funciones y crear una función única que sirva para escanear.
	 * Falta función para PARAR ESCANEO
	 * Falta función para REINICIAR ESCANEO
	 * Falta función para PAUSAR ESCANEO
	 * Falta función para SELECCIONAR NÚMERO DE CORTES (1 ó 2)
	 * Falta función para mandar por UART
	 */
	
	void start_scan(unsigned int milimeters, unsigned int degrees, bufferType *tx_mat_bf)
	{
		unsigned int rotations_counter = 1;
		unsigned int j = 0;
		unsigned int k = 0;
		unsigned int milimeters_counter = 0;
		problem_f = 0;
		
		if(start_scan_f ==1)
		{
		
		if((sys_ready_f==1)&&(ended_rotation_f)&&(rotations_counter))
		{
			
			for (k=0; k<= (degrees-1); k++) //hace esto degrees veces
			{
				GPIOB_PDOR ^= ((-(0)) ^ GPIOB_PDOR ) & (1UL << 8); //PTB8
				
				ended_rotation_f = 0;
										
				move_step(ONE_STEP,DEGREE_MOTOR,SUPERSMALL); //CAMBIAR A 1
				
				rotations_counter++;
				
				ended_rotation_f = 1;
				
				time_delay_ms(SMALL);
							
				
				if(ended_rotation_f == 1)
				{
					ended_traslation_f = 0;
					
					for (j=0; j<= (milimeters-1); j++) //hace esto milimetros veces
						{
																			
							move_step(ONE_MM,TRASL_MOTOR,SUPERSMALL);
							
							scan_laser();
							
							milimeters_counter++;
							
							time_delay_ms(SMALL);
							
						}
					ended_traslation_f = 1;
				
					if (milimeters_counter >= milimeters)
					{
						returning_f = 1;	
						lcd_send_returning(tx_mat_bf);
						
						milimeters_counter = 0;
						
						for (j = 0; j<= (milimeters-1); j++) {
								//cambiar pin de dirección de TRASL_MOTOR
								//PTB8 GPIO CW+ = 0
							
								GPIOB_PDOR ^= ((-(1)) ^ GPIOB_PDOR ) & (1UL << 8); //PTB8
							
								move_step(ONE_MM,TRASL_MOTOR,SUPERSMALL);
								
								milimeters_counter++;
								
								time_delay_ms(SMALL);
							}
						returning_f = 0;
					}
					else 
					{
						problem_f = 1;
					}
				}
				else 
				{
					problem_f = 2;
				}
				
			}
		}
		else 
		{
			problem_f = 3;
		}
		
		}
		start_scan_f = 0;
	}
	
	void stop_scan(bufferType *tx_bf)
	{
		sys_ready_f = 0;
		//Mandar por UART (Escaneo detenido)
		uart_send_stopscan(tx_bf);
	}
	
	void restart_scan(bufferType *tx_bf)
	{
		sys_ready_f = 0;
		verify_system (tx_bf);
	}
	

	/*
	 * Según esto ya están completas las funciones. 
	 * Al menos hay algo en todas ellas. Hay que modificar las de move_motor para agregar un
	 * parámetro de la velocidad del paso.
	 * 
	 * Obviamente va a haber algunos errores en el uso de las banderas
	 * porque no he analizado la interrelación de todas. 
	 * Para analizar eso creo que voy a imprimir to,do el código o al menos
	 * las funciones de intrucción y voy a ejecutar el código en mi cabeza una vez
	 * para ver cómo se comportan las variables. 
	 * COSAS POR TERMINAR: 
	 * 
	 * Tengo que definir el estado inicial de todas las banderas y variables globales.
	 * 		Imprimir el código de instrucciones y flags. 
	 * 		Ejecutar las funciones una vez para ver como deben de funcionar correctamente.
	 * 		Agregar o eliminar variables según se necesite.
	 * Tengo que eliminar las variables globales del código reutilizado que ya no sirven. 
	 * 		Cubierto en el punto anterior.
	 * Tengo que escribir el código para leer el ADC y las rutinas de interrupción tanto del
	 * LPTM0 como del ADC0. 
	 *		Leer el código del ADC0 de Elías y entender cómo funciona.
	 *		Extrapolar el funcionamiento a la aplicación que quiero.
	 * Tengo que escribir to,do el código que falta para que el UART reconozca los comandos.
	 * 		Describir en cada case, lo que debe de hacer el código.
	 * 		Agregar al command_compare todos los comandos que falten.
	 * 		Agregar a la función de send_uart to,do lo que quiera que escriba.
	 * Tengo que modificar el main para incluir todas las funciones que tengan que ejecutarse
	 * en el loop infinito. 
	 * 		Cuando esté haciendo el punto primero de esta lista se podrá hacer a la par este
	 * 		
	 * 
	 * 
	 */

	
	/*
	///El formato que debe recibir el UART para que funcionen correctamente las siguientes dos funciones debe ser:
	///SCAN:START:20,180
	///1234:67890:12,456
	 * 
	 * */
	
	unsigned int parse_scan_length(CommandString *commandString) {

		signed long val = (signed long) (10 * (commandString->data[11] - 0x30)

				+ (commandString->data[12] - 0x30));

		return (unsigned int) val;

	}
	

	unsigned int parse_scan_angle(CommandString *commandString) {

			signed long val = (signed long) (100 * (commandString->data[14] - 0x30)

					+ 10 * (commandString->data[15] - 0x30)

					+ (commandString->data[16] - 0x30));

			return (unsigned int) val;

		}
		
	void check_sensors(void)
	{
		/* 
		  	PORTE_PCR20 =(1<<8);		//PTE20 GPIO posición de inicio rotación
		
			PORTE_PCR21 =(1<<8);		//PTB21 GPIO posición de fin rotación

			PORTE_PCR22 =(1<<8); 		//PTB22 GPIO posición de inicio traslación

			PORTE_PCR23 =(1<<8); 		//PTB23 GPIO posición de fin traslación
		*/
		if ((GPIOE_PDIR&&(1<<20))==1)
		{
			
			start_deg_pos_f 	= 1;
		
		}
		
		else 
		{
			
			start_deg_pos_f 	= 0;
		
		}
			
		if ((GPIOE_PDIR&&(1<<21))==1)
		{
			
			end_deg_pos_f 		= 1;
		
		}
		
		else 
		{
			
			end_deg_pos_f 		= 0;
			
		}
			
		if ((GPIOE_PDIR&&(1<<22))==1)
		{
		
			start_trasl_pos_f 	= 1;
		
		}
		
		else 
		{
		
			start_trasl_pos_f 	= 0;
			
		}
			
		if ((GPIOE_PDIR&&(1<<23))==1)		
		{
			
			end_trasl_pos_f 	= 1;
		
		}
		
		else 
		{
			
			end_trasl_pos_f 	= 0;
			
		}
	}
	
	void delay (unsigned long time_ms)
	{
	    LPTMR0_PSR|=0b00000101;
	    LPTMR0_CMR=time_ms; //hasta 65,000 (65 segundos)
	    LPTMR0_CSR|=0b00000001;  
	    while (!(LPTMR0_CSR&0b10000000)){
	    	hola = 1;
	    }; 
	    LPTMR0_CSR=0;
	}
	
	void time_delay_ms(unsigned int count_val)
	{
	  /* Make sure the clock to the LPTMR is enabled */
	  SIM_SCGC5|=SIM_SCGC5_LPTMR_MASK;

	  /* Reset LPTMR settings */
	  LPTMR0_CSR=0;

	  /* Set the compare value to the number of ms to delay */
	  LPTMR0_CMR = count_val;

	  /* Set up LPTMR to use 1kHz LPO with no prescaler as its clock source */
	  LPTMR0_PSR = LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK;

	  /* Start the timer */
	  LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;

	  /* Wait for counter to reach compare value */
	  while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK));

	  /* Disable counter and Clear Timer Compare Flag */
	  LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;

	  return;
	}
	
	
