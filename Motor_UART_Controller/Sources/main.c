#include "derivative.h" 
#include "global_variables.h"
#include "buffer.h"
#include "command.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"

//Low power Timer and TPM
void LPTM_init(void);
void timer0_init(void);

//Step motor
void toggle_signal(void);
void shift_step_motor(void);
void shift_step_motor_manual(signed int motor_angle);

//Buffer
bufferType Buffer_rx = { 0, 0, BUFLEN, { } };
bufferType *rx_bf;
bufferType Buffer_tx = { 0, 0, BUFLEN, { } };
bufferType *tx_bf;

//Command
CommandString commandString = { CMDLEN, 0, 0, { } };
CommandString *commandString_p;

void tmp_counter_10ms_tick(void);
void tmp_counter_1sec_tick(void);
void tmp_counter_5sec_tick(void);

unsigned long motor_sequence[8] = { 0x00000008, 0x0000000C, 0x00000004,
		0x00000006, 0x00000002, 0x00000003, 0x00000001, 0x00000009 };

//LCD display

void lcd_initialize(bufferType *bf);
void write_to_lcd(bufferType *bf);
void lcd_send_temperature(bufferType *bf);
void lcd_send_velocity(bufferType *bf);
void gpio_lcd_ports_init(void);
void lcd_10ms_check_buffer(void);



unsigned char DB7_4;

struct lcdState {
	unsigned char RS[2];
	unsigned int EN;
	unsigned int nextState[2];
};

typedef const struct lcdState LcdEncoding;


LcdEncoding lcdEncoding[5] = { { { 0, 1 }, 0, { SET_RS, IDLE } },		//IDLE
		{ { 0, 1 }, 0, { DATA_ENABLE_SET, DATA_ENABLE_SET } },	//SET_RS
		{ { 0, 1 }, 1, { ENABLE_CLEAR, ENABLE_CLEAR } },	//DATA_ENABLE_SET
		{ { 0, 1 }, 0, { DATA_CLEAR, DATA_CLEAR } },			//ENABLE_CLEAR 
		{ { 0, 1 }, 0, { IDLE, IDLE } },						//DATA_CLEAR
		};

bufferType Buffer_lcd = { 0, 0, BUFLEN, { } };
bufferType *lcd_bf;

void global_modules_initializer(void);
void global_modules_initializer(void) {
	RGB_init();
	Ports_init();
	LPTM_init();
	ADC_init();
	uart_init();
	timer0_init();
	Set_timer_signal_GPIO();
}

int main(void) {

	lcd_bf = &Buffer_lcd;
	rx_bf = &Buffer_rx;
	tx_bf = &Buffer_tx;
	commandString_p = &commandString;

	global_variables_initializer();
	global_modules_initializer();
	gpio_lcd_ports_init();
	lcd_initialize(lcd_bf);

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
					get_motor_vel_string(motor_vel_string, commandString_p);
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
	tmp_counter_10ms_tick();
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

void timer0_init(void) {
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
	//TPM0_C2V =0x0000C350;		//50,000 -> 50000 clock cycles of 1MHz -> 50ms
	TPM0_MOD = 0x00002710;
	NVIC_ICPR |= (1 << 17);
	NVIC_ISER |= (1 << 17);
	TPM0_SC |= (1 << 3);		//CMOD select clock mode mux
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
		GPIOB_PDOR = ((motor_sequence[motorSequenceIndex++]) & 0x0000000F);
		if (motorSequenceIndex >= 8)
			motorSequenceIndex = 0;
		if (current_angle > 96) {
			current_angle = 0;
		}
		current_angle++;
	} else {
		GPIOB_PDOR = ((motor_sequence[motorSequenceIndex--]) & 0x0000000F);
		if (motorSequenceIndex <= 0)
			motorSequenceIndex = 7;
		if (current_angle == 0 || current_angle > 96) {
					current_angle = 96;
				}
		current_angle--;
	}
}

void shift_step_motor_manual(signed int motor_angle) {
	if (current_angle == motor_angle)
		return;
	if (current_angle > motor_angle) {
		GPIOB_PDOR = ((motor_sequence[motorSequenceIndex--]) & 0x0000000F);
		if (motorSequenceIndex < 0)
			motorSequenceIndex = 7;
		if (current_angle == 0 || current_angle > 96) {
			current_angle = 96;
		}
		current_angle--;
	} else if (current_angle < motor_angle) {
		GPIOB_PDOR = ((motor_sequence[motorSequenceIndex++]) & 0x0000000F);
		if (motorSequenceIndex >= 8)
			motorSequenceIndex = 0;
		if (current_angle > 96) {
			current_angle = 0;
		}
		current_angle++;
	}
}

void tmp_counter_10ms_tick() {
	tmp_counter_10ms++;
	lcd_10ms_check_buffer();
	if (tmp_counter_10ms >= 100) {
		tmp_counter_10ms = 0;
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
		motor_free_running_flag = 0;
	} else {
		uart_send_temperature(tx_bf);
	}
	lcd_send_temperature(lcd_bf);
	lcd_send_velocity(lcd_bf);
}

void gpio_lcd_ports_init(void) {
	SIM_SCGC5 |= (1 << 9);
	SIM_SCGC5 |= (1 << 11);
	PORTA_PCR17 =(1<<8); 		//PTA17 GPIO = EN
	PORTA_PCR16 =(1<<8); 		//PTA16 GPIO = RS
	PORTC_PCR12 =(1<<8); 		//PTC12 GPIO = DB7
	PORTC_PCR13 =(1<<8); 		//PTC13 GPIO = DB6
	PORTC_PCR16 =(1<<8); 		//PTC16 GPIO  = DB5
	PORTC_PCR17 =(1<<8); 		//PTC17 GPIO  = DB4
	GPIOA_PDDR |= (1 << 17);		//Set GPIOA17 as output
	GPIOA_PDDR |= (1 << 16);		//Set GPIOA16 as output
	GPIOC_PDDR |= 0x00033000;	//Set GPIOC as output
}

void lcd_initialize(bufferType *bf) {
	buffer_push(bf, 0x03);
	buffer_push(bf, 0x03);
	buffer_push(bf, 0x03);
	buffer_push(bf, 0x02);
	buffer_push(bf, 0x02);
	buffer_push(bf, 0x08);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x0E);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x06);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x01);
	//Characteres
	buffer_push(bf, 0x14);
	buffer_push(bf, 0x18);

	buffer_push(bf, 0x14);
	buffer_push(bf, 0x1F);

	buffer_push(bf, 0x14);
	buffer_push(bf, 0x1C);

	buffer_push(bf, 0x14);
	buffer_push(bf, 0x11);

	buffer_push(bf, 0x12);
	buffer_push(bf, 0x10);

	//buffer_push(bf, 0x12);
}

void write_to_lcd(bufferType *bf) {
	lcd_db7 = (lcd_data & 0x08) >> 3;
	lcd_db6 = (lcd_data & 0x04) >> 2;
	lcd_db5 = (lcd_data & 0x02) >> 1;
	lcd_db4 = (lcd_data & 0x01) >> 0;
	GPIOC_PDOR ^= ((-(lcd_db7)) ^ GPIOC_PDOR ) & (1UL << 12); //PTC12 GPIO = DB7
	GPIOC_PDOR ^= ((-(lcd_db6)) ^ GPIOC_PDOR ) & (1UL << 13); //PTC13 GPIO = DB6
	GPIOC_PDOR ^= ((-(lcd_db5)) ^ GPIOC_PDOR ) & (1UL << 16); //PTC16 GPIO  = DB5
	GPIOC_PDOR ^= ((-(lcd_db4)) ^ GPIOC_PDOR ) & (1UL << 17); //PTC17 GPIO  = DB4
}

void lcd_10ms_check_buffer(void) {
	lcd_en = lcdEncoding[current_lcd_state].EN;
	GPIOA_PDOR ^= (-(lcd_en) ^ GPIOA_PDOR ) & (1UL << 17); //SET EN
	if ((!buffer_isempty(lcd_bf)) && current_lcd_state != 0) {
		if (current_lcd_state == SET_RS) {
			lcd_data = buffer_pop(lcd_bf);
			lcd_rs_was_set = 1;
			lcd_rs = lcdEncoding[current_lcd_state].RS[(lcd_data & 0x10) >> 4];
			GPIOA_PDOR ^= (-(lcd_rs) ^ GPIOA_PDOR ) & (1UL << 16); //SET RS
		}

	}
	if (current_lcd_state == DATA_ENABLE_SET) {
		write_to_lcd(lcd_bf);
	}
	current_lcd_state = lcdEncoding[current_lcd_state].nextState[buffer_isempty(
			lcd_bf)];
}

void lcd_send_temperature(bufferType *bf) {
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x01);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x02);
	if (motor_dir_flag) {
		//DIR_CW
		buffer_push(bf, 0x14);
		buffer_push(bf, 0x13);
	} else {
		buffer_push(bf, 0x14);
		buffer_push(bf, 0x13);
		buffer_push(bf, 0x14);
		buffer_push(bf, 0x13);
	}
	buffer_push(bf, 0x15);
	buffer_push(bf, 0x17);
	buffer_push(bf, 0x12);
	buffer_push(bf, 0x10);
	buffer_push(bf, 0x12);
	buffer_push(bf, 0x1D);
	buffer_push(bf, 0x12);
	buffer_push(bf, 0x10);
	dec2str4(temperature, temperature_string);
	buffer_push(bf, 0x10 | ((temperature_string[3] & 0xF0) >> 4));
	buffer_push(bf, 0x10 | ((temperature_string[3] & 0x0F)));
	buffer_push(bf, 0x10 | ((temperature_string[2] & 0xF0) >> 4));
	buffer_push(bf, 0x10 | ((temperature_string[2] & 0x0F)));
	buffer_push(bf, 0x10 | ((temperature_string[1] & 0xF0) >> 4));
	buffer_push(bf, 0x10 | ((temperature_string[1] & 0x0F)));
	buffer_push(bf, 0x12);
	buffer_push(bf, 0x1E);
	buffer_push(bf, 0x10 | ((temperature_string[0] & 0xF0) >> 4));
	buffer_push(bf, 0x10 | ((temperature_string[0] & 0x0F)));
}

void lcd_send_velocity(bufferType *bf) {
	buffer_push(bf, 0x0C);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x10 | ((motor_vel_string[2] & 0xF0) >> 4));
	buffer_push(bf, 0x10 | ((motor_vel_string[2] & 0x0F)));
	buffer_push(bf, 0x10 | ((motor_vel_string[1] & 0xF0) >> 4));
	buffer_push(bf, 0x10 | ((motor_vel_string[1] & 0x0F)));
	buffer_push(bf, 0x12);
	buffer_push(bf, 0x1E);
	buffer_push(bf, 0x10 | ((motor_vel_string[0] & 0xF0) >> 4));
	buffer_push(bf, 0x10 | ((motor_vel_string[0] & 0x0F)));
	buffer_push(bf, 0x12);
	buffer_push(bf, 0x10);
	buffer_push(bf, 0x15);
	buffer_push(bf, 0x12);
	buffer_push(bf, 0x15);
	buffer_push(bf, 0x10);
	buffer_push(bf, 0x15);
	buffer_push(bf, 0x13);
}
