
#ifndef I2C_H_
#define I2C_H_

/*
typedef struct {
	unsigned char baudrate;
	unsigned char I2C_ON_MST_ON;
	unsigned char MSTON;
	unsigned char MSTOFF;
	unsigned char flagOFF;
}I2C;
*/

extern unsigned char flagRTII2C;
void I2C_Init(void);
void I2C_Write(unsigned char);
void I2C_Stop(void);
void I2C_ClearFlag(void);
void I2C_WriteFor(unsigned char data);
void nopDelay(void);






#endif /* I2C_H_ */
