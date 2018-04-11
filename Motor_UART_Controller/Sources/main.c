#include "derivative.h" 
#include "global_variables.h"
#include "buffer.h"
#include "command.h"
#include "uart.h"
#include "gpio.h"

//Low power Timer and TPM
void LPTM_init(void);
void Timer_init(void);

//Step motor
void toggle_signal(void);
void shift_step_motor(void);
void shift_step_motor_manual(signed int motor_angle);

//ADC
void ADC_init(void);

//Buffer
bufferType Buffer_rx = { 0, 0, BUFLEN, { } };
bufferType *rx_bf;
bufferType Buffer_tx = { 0, 0, BUFLEN, { } };
bufferType *tx_bf;

//Command
CommandString commandString = { CMDLEN, 0, 0, { } };
CommandString *commandString_p;

void tmp_counter_50ms_tick(void);
void tmp_counter_1sec_tick(void);
void tmp_counter_5sec_tick(void);

unsigned long motor_sequence[8] = { 0x00000008, 0x0000000C, 0x00000004,
		0x00000006, 0x00000002, 0x00000003, 0x00000001, 0x00000009 };

void global_modules_initializer(void);
void global_modules_initializer(void) {
	RGB_init();
	Ports_init();
	LPTM_init();
	ADC_init();
	uart_init();
	Timer_init();
	Set_timer_signal_GPIO();
}

int main(void) {

	rx_bf = &Buffer_rx;
	tx_bf = &Buffer_tx;
	commandString_p = &commandString;

	global_variables_initializer();
	global_modules_initializer();

	while (1) {
		if (rx_status == 1) {
			rx_status = 0;
			if (commandString_p->is_full == 1) {
				command_clear(commandString_p);
			} else {
				cmd_code = command_compare_cmd_ugly(commandString_p);
				if (cmd_code != 0) {
					uart_send_done(tx_bf);
				}
				switch (cmd_code) {
				case DIR_CW:
					motor_dir_flag = 1;
					break;
				case DIR_CCW:
					motor_dir_flag = 0;
					break;
				case MOTOR_ON:
					motor_free_running_flag = 1;
					motor_manual_angle_flag = 0;
					break;
				case MOTOR_OFF:
					motor_free_running_flag = 0;
					motor_manual_angle_flag = 1;
					motor_angle = current_angle;
					break;
				case STEP_CW:
					motor_angle = parse_motor_angle(commandString_p);
					break;
				case TEMPLIMIT:
					temperature_limit = parse_temperature_limit(
							commandString_p);
					break;
				case RPS:
					motor_vel = parse_motor_velocity(commandString_p);
					LPTMR0_CSR = 0b01000000; //Activate the timer and enable interrupts	
					LPTMR0_CSR |= (1 << 7);		//Clear timer compare flag
					LPTMR0_CMR = motor_vel;
					LPTMR0_CSR = 0b01000001;//Activate the timer and enable interrupts	
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

void UART0_IRQHandler(void) {
	//WRITE
	if (((UART0_S1 & 0x80) >> 7) && (!buffer_isempty(tx_bf))) {
		UART0_D = buffer_pop(tx_bf);
		if (buffer_isempty(tx_bf)) {
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

void FTM0_IRQHandler() {
	TPM0_SC |= (1 << 7); 		//TOF clear interrupt flag
	TPM0_C2SC |= (1<<7);
	tmp_counter_50ms_tick();
}

void TPM0_IRQHandler(void) {
	TPM0_SC |= (1 << 7);
}

void ADC0_IRQHandler() {
	if ((ADC0_SC1A &(1<<7))==(1<<7)) {
		//unsigned int temp = ((ADC0_RA*1000)/225)+225;
		unsigned int temp = ((ADC0_RA*9999)/255);
		if(timerStateReached==1) {
			//LPTMR0_CMR = temp;
			temperature = temp;
			timerStateReached=0;
		}
		ADC0_SC1A |=4;
	}
}

void LPTimer_IRQHandler() {
	LPTMR0_CSR |= (1 << 7);	//Clear timer compare flag
	timerStateReached = 1;
	if (motor_free_running_flag == 1)
		shift_step_motor();
	else if (motor_manual_angle_flag == 1)
		shift_step_motor_manual(motor_angle);
	shift_rgb_leds();
}

void Timer_init(void) {
	//Setiing MCGIRCLK
	SIM_SCGC6 |= (1 << 24);		//CLK TMP0
	SIM_SCGC5 |= (1 << 13);		//PTE29
	PORTE_PCR29 |= (3<<8);		//TMP0_CH2 for PTE29
	MCG_C1 |= (1 << 1);			//IRCLKEN
	MCG_C2 |= 1;				//IRCS = 1 (Fast clock) 4MHz
	MCG_SC = (2 << 1);			//FCRDIV = 4, divide by 4 = 1MHz
	SIM_SOPT2 |= (3 << 24);		//TPM clock source select TMPSRC
	TPM0_SC |= (1 << 6);		//TOIE enable interrupt
	TPM0_SC |= (1 << 8);		//DMA enable overflow
	TPM0_C2SC = (5<<2);			//Output compare -> toggle mode FOR TMP0 CH2
	TPM0_C2V =0x0000C350;		//50,000 -> 50000 clock cycles of 1MHz -> 50ms
	NVIC_ICPR |= (1 << 17);
	NVIC_ISER |= (1 << 17);
	TPM0_SC |= (1 << 3);		//CMOD select clock mode mux
}

void ADC_init(void) {
	SIM_SCGC6 |= (1 << 27);
	SIM_SCGC5 |= (1 << 13);
	NVIC_ICPR |= (1 << 15);
	NVIC_ISER |= (1 << 15);
	//ADC0
	ADC0_CFG1 = 0x00000000;
	ADC0_CFG2 = (0 << 4);		//Seleccionar el canal A del ADC
	ADC0_SC1A =0b1000100;//Habilitar interrupciones del ADC y el canal AD4-> esta en el canal PTE21
}

void LPTM_init(void) {
	SIM_SCGC5 |= (1 << 0); //Activate the LPTMR in the system control gating register
	LPTMR0_PSR = 0b00000101; //Bypass the preescaler and select the LPO(low power oscilator of 1Khz as the source of the timer)
	LPTMR0_CMR = 500;			//compare of 500 clock cycles = .5 secs
	NVIC_ICPR |= (1 << 28);		//Clean flag of LPTM in the interrupt vector
	NVIC_ISER |= (1 << 28);		//Activate the LPTM interrupt
	LPTMR0_CSR = 0b01000001;	//Activate the timer and enable interrupts	
}

void shift_step_motor(void) {
	if (motor_dir_flag == 1) {
		current_angle++;
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex++]) & 0x0000000F);
		if (motorSequenceIndex >= 8)
			motorSequenceIndex = 0;
		if (current_angle > 0) {
			current_angle = 96;
		}
	} else {
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex--]) & 0x0000000F);
		if (motorSequenceIndex <= 0)
			motorSequenceIndex = 7;
		current_angle--;
		if (current_angle < 0) {
			current_angle = 96;
		}
	}
}

void shift_step_motor_manual(signed int motor_angle) {
	if (current_angle == motor_angle)
		return;
	if (current_angle > motor_angle) {
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex--]) & 0x0000000F);
		if (motorSequenceIndex < 0)
			motorSequenceIndex = 7;
		current_angle--;
		if (current_angle < 0) {
			current_angle = 96;
		}
	} else if (current_angle < motor_angle) {
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex++]) & 0x0000000F);
		if (motorSequenceIndex >= 8)
			motorSequenceIndex = 0;
		current_angle++;
		if (current_angle > 96) {
			current_angle = 0;
		}
	}
}

void tmp_counter_50ms_tick() {
	tmp_counter_50ms++;
	if (tmp_counter_50ms >= 20) {
		tmp_counter_50ms = 0;
		tmp_counter_1sec_tick();
	}
}

void tmp_counter_1sec_tick() {
	tmp_counter_1sec++;
	toggle_signal();
	if (tmp_counter_1sec >= 5) {
		tmp_counter_1sec = 0;
		tmp_counter_5sec_tick();
	}
}

void tmp_counter_5sec_tick() {
	if (temperature >= temperature_limit) {
		uart_send_overtemperature_detected(tx_bf);
	} else {
		uart_send_temperature(tx_bf);
	}
}
