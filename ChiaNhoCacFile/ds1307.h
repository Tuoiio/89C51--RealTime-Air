#ifndef _DS1307_H_
#define _DS1307_H_




enum{
	CLOCK_1HZ = 0x90,
	CLOCK_4096HZ = 0x91,
	CLOCK_8192HZ = 0x92,
	CLOCK_32768HZ = 0x93
};

unsigned char DS1307_Read(unsigned char address);
void DS1307_Read_Time(void);
void DS1307_Write(unsigned char Addrees, unsigned char Data);
void DS1307_Write_Time(void);
void DS1307_Init();
void DS1307_SQW(char mode);

#endif