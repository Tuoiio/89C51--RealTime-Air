#ifndef _MAIN_H_
#define _MAIN_H_

#include <regx51.h>

/*******Khai bao giao tiep nut nhan************/
sbit Button_Mode = P2^0;
sbit Button_Ok   =   P2^1;
sbit Button_Up   =   P2^2;
sbit Button_Down = P2^3;
sbit Buzzer      =      P2^4; 

typedef enum {false,true} bool;

char *Days[] =  {"SUN","MON","TUE","WED","THU","FRI","SAT"};

 char Second = 0, Minute = 0, Hour = 0, Date = 0, Day = 0, Month = 0, Year = 0, Mode = 0;

unsigned char Status_Mode_Alarm = 0, Status_Mode_Time = 0, Time_Break_Up = 0, Time_Break_Down = 0, Time_Break_Mode = 0;

bit Old_Ok, Old_Mode, Old_Up, Old_Down;

bit am_pm, Mode12_24, read_time, read_temp = 1;

bit Choose_Alarm_Active, Choose_Alarm_1_Or_2;

bit Set_Time, Set_ALM1, Set_ALM2, Clock_1Hz, Prioritized, Error_DHT;

bit Turn_On_Alarm1, Turn_On_Alarm2;

float fTemp, fHumi;

char customChar[] = {0x00, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04};

unsigned char CountTimer = 0, Count2 = 0;
struct Alarm{
	char Minute ;
	char Hour ;
	char Display ;
	char Status_Mode;
	bool On_Off;
	struct Alarm_Day{
		bool Day_2,Day_3,Day_4;
		bool Day_5,Day_6,Day_7,Day_8;
	}Days;
}Alarm1, Alarm2, *Alarm_Pointer;
#endif