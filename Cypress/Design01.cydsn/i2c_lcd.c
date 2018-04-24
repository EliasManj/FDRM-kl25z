#include "i2c_lcd.h"

static unsigned int ohm;
static unsigned int pt;
unsigned char rti_flag = 0;
unsigned char wr_dir = 0x4E;
unsigned char state;
unsigned char bit4_mode = 0;
unsigned char dir;
unsigned char tmp_bf;

void i2c_send(unsigned char data) {
	I2CP_I2CMasterSendStart(0x27, 0x00);
	I2CP_I2CMasterWriteByte(data);
	rti_flag = 0;
}

void i2c_idle(void) {
	I2CP_I2CMasterSendStop();
}

void I2C_LCD_Init(unsigned char address) {
	dir = address << 1;
	i2c_send(0x08);
	state = 0x08;
	i2c_idle();
	CyDelay(20);
	lcd_write_uchar(0b00110000, 0);
	CyDelay(5);
	lcd_write_uchar(0b00110000, 0);
	CyDelay(1);
	lcd_write_uchar(0b00110000, 0);
	lcd_4_bit();
	lcd_cursor_inc();
	lcd_cursor_on();
	lcd_clear();
}

void lcd_clear(void) {
	lcd_write_uchar(0b00000001, 0);
}

void lcd_4_bit(void) {
	lcd_write_uchar(0b00101000, 0);
	bit4_mode = 1;
}

void lcd_cursor_inc(void) {
	lcd_write_uchar(0b00000110, 0);
}

void lcd_line0(unsigned char posicion) {
	lcd_write_uchar(0b10000000 + posicion, 0);
}

void lcd_line1_pos(unsigned char posicion) {
	lcd_write_uchar_pos(0b10000000 + posicion, 0);
}

void lcd_line1(unsigned char posicion) {
	lcd_write_uchar(0b11000000 + posicion, 0);
}

void lcd_line2_pos(unsigned char posicion) {
	lcd_write_uchar_pos(0b11000000 + posicion, 0);
}

void lcd_cursor_on(void) {
	lcd_write_uchar(0b00001110, 0);
}

void lcd_write_char(unsigned char character) {
	lcd_write_uchar(character, 1);
}

void lcd_write_int(unsigned char character) {
	lcd_write_uchar_pos(character, 1);
}


void delay_ms(unsigned int delay) {
	unsigned int contador;
	delay = (int) (delay * 4000);
	for (contador = 0; contador < delay; contador++)
		asm ("nop");
}

void delay_us(unsigned int delay) {
	unsigned int contador;
	delay = (int) (delay * 4);
	for (contador = 0; contador < delay; contador++)
		asm ("nop");
}

void delay_micro(unsigned int delay) {
	delay = (int) (delay / 4);
	if (pt < delay)
		pt++;
	else
		pt = 0;
}

void delay_mili(unsigned int delay) {
	delay = (int) (delay * 2); //250
	if (ohm < delay)
		ohm++;
	else
		ohm = 0;
}

void delay_op(void) {
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
}

void lcd_write_uchar_pos(unsigned char dato, unsigned char tipo) {
	state = 0x08;

	if (tipo == 0)
		i2c_send(state);
	else {
		state = 0b00000001 | state;
		i2c_send(state);
	}

	state = 0b00000100 | state;
	i2c_send(state);

	state = state | (dato & 0b11110000);
	i2c_send(state);

	state = 0b11111001 & state;
	i2c_send(state);

	i2c_idle();
	delay_op();

	if (tipo == 0)
		delay_mili(2);
	else
		delay_micro(50);

	if (bit4_mode == 1) {
		state = state & 0b00001111;
		state = 0b00000100 | (state & 0b00001001);
		i2c_send(state);
		state = state | (dato << 4);
		i2c_send(state);
		state = 0b11111001 & state;
		i2c_send(state);
		i2c_idle();
		delay_op();
		if (tipo == 0)
			delay_mili(2);
		else
			delay_micro(50);
	}

}

void lcd_write_uchar(unsigned char dato, unsigned char tipo) {
	state = 0x08;
	i2c_send(dir);
	if (tipo == 0) {
		i2c_send(state);
	} else {
		state = 0b00000001 | state;
		i2c_send(state);
	}
	state = 0b00000100 | state;
	state = state | (dato & 0b11110000);
	i2c_send(state);
	state = 0b11111001 & state;
	i2c_send(state);
	i2c_idle();
	if (tipo == 0)
		delay_ms(2);
	else
		delay_us(50);
	if (bit4_mode == 1) {
		state = state & 0b00001111;
		state = 0b00000100 | (state & 0b00001001);
		i2c_send(state);
		state = state | (dato << 4);
		i2c_send(state);
		state = 0b11111001 & state;
		i2c_send(state);
		i2c_idle();
		if (tipo == 0)
			delay_ms(2);
		else
			delay_us(50);
	}
}


