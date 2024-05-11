#include <ds1307.h>
#include <regx51.h>
#include <i2c.h>

extern char  Second, Minute, Hour, Day, Date, Month, Year;


//*****************Ham doc gia tri cho IC DS1307*******************************//
unsigned char DS1307_Read(unsigned char address){
	unsigned char  Temp, Result;
	I2C_Start();
	I2C_Write(0xD0);          // ghi dia chi cua chip DS1307
	I2C_Write(address);      //  dia chi bat dau cua thanh ghi trong DS1307
	I2C_Start();
	I2C_Write(0xD1);
	Result = I2C_Read(0);
	I2C_Stop();
							/* BCD to Hex */
	Temp = Result;
	Result = (((Result / 16)*10) + (Temp & 0x0F)); 
	
	return Result;
}

void DS1307_Read_Time(){     
	Second = DS1307_Read(0x00);    
	Minute = DS1307_Read(0x01);
	Hour   =  DS1307_Read(0x02);     
	Day    =   DS1307_Read(0x03);
	Date   =   DS1307_Read(0x04);
	Month  = DS1307_Read(0x05);
	Year   =   DS1307_Read(0x06);
	
}

/*****************Ham ghi gia tri cho IC DS1307*******************************/
void DS1307_Write(unsigned char address, unsigned char Data){
	unsigned char temp;
	// HEX to BCD //
	temp = Data;
	Data = (((Data/10)*16)| (temp % 10));
	I2C_Start();
	I2C_Write(0xD0);          // ghi dia chi cua chip DS1307
	I2C_Write(address);      //  dia chi bat dau cua thanh ghi trong DS1307
	I2C_Write(Data);         //  ghi gia tri vao dia chi cua thanh ghi trong DS1307
	I2C_Stop();
}

void DS1307_Write_Time(){

	DS1307_Write(0x00,Second);
	DS1307_Write(0x01,Minute);
	DS1307_Write(0x02,Hour);
	DS1307_Write(0x03,Day);
	DS1307_Write(0x04,Date);
	DS1307_Write(0x05,Month);
	DS1307_Write(0x06,Year);
}
void DS1307_Init(){
	unsigned char temp;
	temp = DS1307_Read(0x00);
	temp &= 0x7F;
	DS1307_Write(0x00,temp);
}

void DS1307_SQW(char mode){
	DS1307_Write(0x07,mode);
}

	