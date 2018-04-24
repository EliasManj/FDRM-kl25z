#ifndef I2C_LCD_H_
#define I2C_LCD_H_

typedef struct {
	unsigned char baudrate;
	unsigned char I2C_ON_MST_ON;
	unsigned char MSTON;
	unsigned char MSTOFF;
	unsigned char flagOFF;
} I2CLCD;

void I2C_LCD_Init(unsigned char address);
void lcd_clear(void);
void lcd_4_bit(void);
void lcd_cursor_inc(void);
void lcd_line0(unsigned char posicion);
void lcd_line1(unsigned char posicion);
void lcd_cursor_on(void);
void lcd_write_char(unsigned char character);
void lcd_write_int(unsigned char character);
void lcd_write_uchar_pos(unsigned char dato, unsigned char tipo);
void lcd_write_uchar(unsigned char, unsigned char);
void delay_ms(unsigned int delay);
void delay_us(unsigned int delay);
void lcd_line1_pos(unsigned char posicion);
void delay_micro(unsigned int delay);
void delay_mili(unsigned int delay);
void delay_op(void);
void lcd_line2_pos(unsigned char posicion);
void i2c_send(unsigned char data);
void i2c_idle(void);

#endif
