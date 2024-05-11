#ifndef _I2C_H_
#define _I2C_H_



// Ham tao tre cua i2c
void Delay_I2C(unsigned int time);

// ham bat dau cua i2c
void I2C_Start(void);

// ham dung lai cua i2c
void I2C_Stop(void);

// ham nhan ack 
bit I2C_Get_ACK(void);

// ham gui du lieu tu master di slave
bit I2C_Write(unsigned char Data);

// ham gui ack tu master den slave (ACK tiep tuc doc, slave tiep tuc gui du lieu den master)
void I2C_ACK(void);

// ham gui nack tu master den slave (nack dung doc, salve se dung gui du lieu
void I2C_NACK(void);

// ham doc du lieu tu slave 
unsigned char I2C_Read(bit ACK);

#endif

