#include <main.h>
#include <delay.h>
#include <lcd20x4.h>
#include <ds1307.h>
#include <dht22.h>

void Bip(void);
void Bip_Alarm(void);
void DS1307_Write_Alarm1(void);
void DS1307_Write_Alarm2(void);
void DS1307_Read_Alarm(void);
void Set_Clock(void);
void Set_Alarm(void);
void KeyBoard(void);
void Hien_Thi_LCD(void);
void Set_Timer(void){
	TMOD &= 0xF0;
	TMOD |= 0x01;
	EA = 1;
	ET0 = 1;
	TF0 = 1;
}

void main(){
	Set_Timer();
	LCD20X4_Init();
	DS1307_Init();
	DS1307_SQW(CLOCK_1HZ);

	DS1307_Read_Alarm();
	
	while(1){
		if(Status_Mode_Time == 0 && read_time == 1){
			DS1307_Read_Time();
			read_time = 0;
		}
		if(read_temp == 1){
			ET0 = 0;
			if(DHT22_Read()){
				fTemp = DHT22_Temperature();
				fHumi = DHT22_Humidity();
				read_temp = 0;
				ET0 = 1;
				Error_DHT = 0;
			} else {
				Error_DHT = 1;
				read_temp = 0;
			}
	}

	KeyBoard();
	Hien_Thi_LCD();
	Bip_Alarm();

	}
}

void T0_ISR(void) interrupt 1{
	TR0 = 0;
	TH0 = 0x3C;
	TL0 = 0xB0;
	TR0 = 1;
	CountTimer++;
	if(CountTimer == 5) read_time = 1;
	if(CountTimer == 10){
		read_time = 1;
		Clock_1Hz = ~Clock_1Hz;
		Count2++;
		CountTimer = 0;
	}
	if(Count2 == 6){ 
		Count2 = 0;
		read_temp = 1;
	}
}

void Bip(void){
	unsigned char i;
	for(i = 0; i < 50; i++){
		Buzzer = 1;
		delay_us(25);
		Buzzer = 0;
		delay_us(25);
	}
}

void Bip_Alarm(void){
	static char Two_Bip1 = 0, Two_Bip2 = 0;
	if(Clock_1Hz) Two_Bip1 = Two_Bip2 = 0;
	
	if(Alarm1.On_Off == 1){
		if(Alarm1.Hour == Hour && Alarm1.Minute == Minute){ // Kiem tra gio de bat bao thuc
			if(Second < 5) Turn_On_Alarm1 = 1;
		} else {																		// Sau 1 phut thi tu tat bao thuc
			Turn_On_Alarm1 = 0;
		}
		if(Turn_On_Alarm1 == 1 && Two_Bip1 <= 2){
			if(Alarm1.Days.Day_2 == (Day == 2)) Bip();
			if(Alarm1.Days.Day_3 == (Day == 3)) Bip();
			if(Alarm1.Days.Day_4 == (Day == 4)) Bip();
			if(Alarm1.Days.Day_5 == (Day == 5)) Bip();
			if(Alarm1.Days.Day_6 == (Day == 6)) Bip();
			if(Alarm1.Days.Day_7 == (Day == 7)) Bip();
			if(Alarm1.Days.Day_8 == (Day == 1)) Bip();
			Two_Bip1++;
			delay_ms(50);
		}
	}
	
	if(Alarm2.On_Off == 1){
		if(Alarm2.Hour == Hour && Alarm2.Minute == Minute){
			if(Second < 5) Turn_On_Alarm2 = 1;
		} else {
				Turn_On_Alarm2 = 1;
		}
		if(Turn_On_Alarm2 == 1 && Two_Bip2 <= 2){
			if(Alarm2.Days.Day_2 == (Day == 2)) Bip();
			if(Alarm2.Days.Day_3 == (Day == 3)) Bip();
			if(Alarm2.Days.Day_4 == (Day == 4)) Bip();
			if(Alarm2.Days.Day_5 == (Day == 5)) Bip();
			if(Alarm2.Days.Day_6 == (Day == 6)) Bip();
			if(Alarm2.Days.Day_7 == (Day == 7)) Bip();
			if(Alarm2.Days.Day_8 == (Day == 1)) Bip();
			Two_Bip2++;
			delay_ms(50);
		}
	}
}


void DS1307_Write_Alarm1(void){
	// Khi cai dat xong gia tri thoi gian thi ghi xuong ds1307 de phong luc mat dien
	DS1307_Write(0x08,Alarm1.Minute);
	DS1307_Write(0x09,Alarm1.Hour);
	DS1307_Write(0x0A,Alarm1.On_Off);
	DS1307_Write(0x0B,Alarm1.Days.Day_2);
	DS1307_Write(0x0C,Alarm1.Days.Day_3);
	DS1307_Write(0x0D,Alarm1.Days.Day_4);
	DS1307_Write(0x0E,Alarm1.Days.Day_5);
	DS1307_Write(0x0F,Alarm1.Days.Day_6);
	DS1307_Write(0x10,Alarm1.Days.Day_7);
	DS1307_Write(0x11,Alarm1.Days.Day_8);
}

void DS1307_Write_Alarm2(void){
	// Khi cai dat xong gia tri thoi gian thi ghi xuong ds1307 de phong luc mat dien
	DS1307_Write(0x12,Alarm2.Minute);
	DS1307_Write(0x13,Alarm2.Hour);
	DS1307_Write(0x14,Alarm2.On_Off);
	DS1307_Write(0x15,Alarm2.Days.Day_2);
	DS1307_Write(0x16,Alarm2.Days.Day_3);
	DS1307_Write(0x17,Alarm2.Days.Day_4);
	DS1307_Write(0x18,Alarm2.Days.Day_5);
	DS1307_Write(0x19,Alarm2.Days.Day_6);
	DS1307_Write(0x1A,Alarm2.Days.Day_7);
	DS1307_Write(0x1B,Alarm2.Days.Day_8);
}

void DS1307_Read_Alarm(){     
	Alarm1.Minute		 = DS1307_Read(0x08);    
	Alarm1.Hour    		 = DS1307_Read(0x09);
	Alarm1.On_Off  		 = DS1307_Read(0x0A);     
	Alarm1.Days.Day_2    = DS1307_Read(0x0B);
	Alarm1.Days.Day_3    = DS1307_Read(0x0C);
	Alarm1.Days.Day_4 	 = DS1307_Read(0x0D);
	Alarm1.Days.Day_5    = DS1307_Read(0x0E);
	Alarm1.Days.Day_6	 = DS1307_Read(0x0F);
	Alarm1.Days.Day_7	 = DS1307_Read(0x10);
	Alarm1.Days.Day_8	 =	DS1307_Read(0x11);
	
	Alarm2.Minute		 = DS1307_Read(0x12);    
	Alarm2.Hour    		 = DS1307_Read(0x13);
	Alarm2.On_Off  		 = DS1307_Read(0x14);     
	Alarm2.Days.Day_2    = DS1307_Read(0x15);
	Alarm2.Days.Day_3    = DS1307_Read(0x16);
	Alarm2.Days.Day_4 	 = DS1307_Read(0x17);
	Alarm2.Days.Day_5    = DS1307_Read(0x18);
	Alarm2.Days.Day_6	 = DS1307_Read(0x19);
	Alarm2.Days.Day_7	 = DS1307_Read(0x1A);
	Alarm2.Days.Day_8	 =	DS1307_Read(0x1B);
}

void Set_Clock(void){
	unsigned char y;
	y = (Year % 4)* 100;
	// Tang
	if(Minute > 59) Minute = 0;
	if(Hour > 23) 	  Hour = 0;
	if(Day > 7) 	  Day = 1;
	if((Month == 2) && (Date > 29) && (y == 0)) Date = 1;
	else if((Month == 2) && (Date > 29) && (y != 0)) Date = 1;
	else if(((Month == 4) || (Month == 6) || (Month == 9) || (Month == 11)) && Date > 30 && (y != 0))	Date = 1;
	else if(Date > 31) Date = 1;
	if(Month > 12) Month = 1;
	
	// Giam
	if(Minute < 0) Minute = 59;
	if(Hour < 0) 	  Hour = 23;
	if(Day < 1) 	  Day = 7;
	if((Month == 2) && (Date < 1) && (y == 0)) Date = 29;
	else if((Month == 2) && (Date > 29) && (y != 0)) Date = 1;
	else if(((Month == 4) || (Month == 6) || (Month == 9) || (Month == 11)) && (Date < 1) && (y != 0))	Date = 30;
	else if(Date < 1) Date = 31;
	if(Month < 1) Month = 12;
}

void Set_Alarm(void){
	switch(Alarm_Pointer->Status_Mode){
		case 1: case 2: { Alarm_Pointer->Display = 0; break; }
		case 3: case 4: { Alarm_Pointer->Display = 1; break; }
		case 5: case 6: { Alarm_Pointer->Display = 2; break; }
		case 7: case 8: { Alarm_Pointer->Display = 3; break; }
		case 9: case 10:{ Alarm_Pointer->Display = 4; break; }
		case 11: { Alarm_Pointer->Status_Mode = 1;  break; }
		default: { 												  break; }
	}

	// Tang
	if(Alarm_Pointer->Minute > 59) Alarm_Pointer->Minute = 0;
	if(Alarm_Pointer->Hour > 23) Alarm_Pointer->Hour  = 0;
	// Giam
	if(Alarm_Pointer->Minute < 0) Alarm_Pointer->Minute = 59;
	if(Alarm_Pointer->Hour < 0) Alarm_Pointer->Hour = 23;
	
}

void KeyBoard(void){
	/*******************************************Chon che do ************************************/
	if(Button_Mode == 0){
		delay_ms(10);
		Time_Break_Mode++;
		if(Time_Break_Mode > 40 && Time_Break_Mode < 42){
			Bip();
			LCD20X4_Clear();
			Choose_Alarm_Active = 1;
			Set_Time = Status_Mode_Time = 0;
		}
		if(Time_Break_Mode > 60 && Time_Break_Mode < 62){
			Bip();
			LCD20X4_Clear();
			while(!Button_Mode){
				LCD20X4_Gotoxy(4,0);
				LCD20X4_PutString("Setting Clock");
				//Second = DS1307_Read(0x00);
			}
			LCD20X4_Clear();
			Choose_Alarm_Active = Choose_Alarm_1_Or_2 = 0;
			Set_Time = Status_Mode_Time = 1;
		}
		if(!Button_Mode && Old_Mode){
			delay_ms(20);
			Bip();
			// Cai dat thoi gian
			if(Set_Time){
				Status_Mode_Time++;
				if(Status_Mode_Time > 6) Status_Mode_Time = 1;
			}
			//Chon 1 trong 2 bao thuc de cai dat bao thuc bat hoac tat
			if(Choose_Alarm_Active)	Choose_Alarm_1_Or_2 =  ~Choose_Alarm_1_Or_2;
			
			// Chon gia tri de cai dat bao thuc 1, 2
			if(Set_ALM1 && !Prioritized)	Alarm1.Status_Mode++;
			if(Set_ALM2 &&  Prioritized) Alarm2.Status_Mode++;
		}
	} else {
		Time_Break_Mode = 0;
	}
	Old_Mode = Button_Mode;
	
	/*******************************************Nut bam tang gia tri ************************************/
	if(Button_Up == 0){
		delay_ms(10);
		// Nhan giu lien tuc trong 1 thoi gian thi gia tri tang lien tuc
		Time_Break_Up++;
		if(Time_Break_Up > 20){
			Time_Break_Up = 21;
			Old_Up = 1;
		}
		// Nhan nha luon thi gia tri tang 1 don vi
		if(!Button_Up && Old_Up){
			delay_ms(20);
			Clock_1Hz = 1;
			if(Time_Break_Up < 20) Bip();
			if(Set_Time){
				switch(Status_Mode_Time){
					case 1: { Minute++;	break; }
					case 2: { Hour++; 	break; }
					case 3: { Day++; 		break; }
					case 4: { Date++;		break; }
					case 5: { Month++;	break; }
					case 6: { Year++; 	break; }
					default:{					break; }
				}
			}
			// Nhan nut Up thi bao thuc bat che do "ON", sau khi an Ok thi(Choose_Alarm_Active) = 0
			if(Choose_Alarm_Active){
				if(Choose_Alarm_1_Or_2 == 0) Alarm1.On_Off = 1;
				if(Choose_Alarm_1_Or_2 == 1) Alarm2.On_Off = 1;
			}
			// Cai dat thoi gian bao thuc
			switch(Alarm_Pointer->Status_Mode){
				case 1: { Alarm_Pointer->Minute++; 		   break; }
				case 2: { Alarm_Pointer->Hour++;    		   break; }
				case 3: { Alarm_Pointer->Days.Day_2 = 1;   break; }
				case 4: { Alarm_Pointer->Days.Day_3 = 1;   break; }
				case 5: { Alarm_Pointer->Days.Day_4 = 1;   break; }
				case 6: { Alarm_Pointer->Days.Day_5 = 1;   break; }
				case 7: { Alarm_Pointer->Days.Day_6 = 1;   break; }
				case 8: { Alarm_Pointer->Days.Day_7 = 1;   break; }
				case 9: { Alarm_Pointer->Days.Day_8 = 1;   break; }
				default: { 											   break; }
			}
		}
	} else {
		Time_Break_Up = 0;
	}
	Old_Up = Button_Up;
	
	
	/*******************************************Nut bam tang gia tri ************************************/
	if(Button_Down == 0){
		delay_ms(10);
		// Nhan giu lien tuc trong 1 thoi gian thi gia tri tang lien tuc
		Time_Break_Down++;
		if(Time_Break_Down > 20){
			Time_Break_Down = 21;
			Old_Down = 1;
		}
		// Nhan nha luon thi gia tri giam 1 don vi
		if(!Button_Down && Old_Down){
			delay_ms(20);
			Clock_1Hz = 1;
			if(Time_Break_Down < 20) Bip();
			if(Set_Time){
				switch(Status_Mode_Time){
					case 1: { Minute--;	break; }
					case 2: { Hour--;	 	break; }
					case 3: { Day--; 		break; }
					case 4: { Date--;		break; }
					case 5: { Month--;	break; }
					case 6: { Year--;	 	break; }
					default:{					break; }
				}
			}
			// Nhan nut Down thi bao thuc bat che do "OFF", sau khi an Ok thi(Choose_Alarm_Active) = 0
			if(Choose_Alarm_Active){
				if(Choose_Alarm_1_Or_2 == 0) Alarm1.On_Off = 0;
				if(Choose_Alarm_1_Or_2 == 1) Alarm2.On_Off = 0;
			}
			// Cai dat thoi gian bao thuc
			switch(Alarm_Pointer->Status_Mode){
				case 1: { Alarm_Pointer->Minute--; 		   break; }
				case 2: { Alarm_Pointer->Hour--;    		   break; }
				case 3: { Alarm_Pointer->Days.Day_2 = 0;   break; }
				case 4: { Alarm_Pointer->Days.Day_3 = 0;   break; }
				case 5: { Alarm_Pointer->Days.Day_4 = 0;   break; }
				case 6: { Alarm_Pointer->Days.Day_5 = 0;   break; }
				case 7: { Alarm_Pointer->Days.Day_6 = 0;   break; }
				case 8: { Alarm_Pointer->Days.Day_7 = 0;   break; }
				case 9: { Alarm_Pointer->Days.Day_8 = 0;   break; }
				default: { 											   break; }
			}
		}
	} else {
		Time_Break_Down = 0;
	}
	Old_Down = Button_Down;
	
	
	/*******************************************Nut bam hoan thanh ************************************/
	if(Button_Ok == 0){
		delay_ms(10);
		if(!Button_Ok && Old_Ok){
			Bip();
			// Cai dat xong thoi gian
			if(Set_Time){
				Status_Mode_Time = Set_Time = 0;
				DS1307_Write_Time();
			}
			// Tat bao thuc
			if(Turn_On_Alarm1 || Turn_On_Alarm2) {
				Turn_On_Alarm1 = 0;
				Turn_On_Alarm2 = 0;
			}
			// Cai dat 
			if(Choose_Alarm_Active){
				LCD20X4_Clear();
				if(Alarm1.On_Off){
					Set_ALM1 = Alarm1.Status_Mode = 1;
					Choose_Alarm_Active = 0;
				}
				if(Alarm2.On_Off){
					Set_ALM2 = Alarm2.Status_Mode = 1;
					Choose_Alarm_Active = 0;
				}
				if(!Alarm1.On_Off  && Alarm2.On_Off) Prioritized = 1;	// truong hop chi bat bao thuc 2 thi LCD se hien thi cai dat bao thuc 2
				if(Alarm1.On_Off   && Alarm2.On_Off) Prioritized = 0;	// truong hop bat ca 2 thi LCD se hien thi cai dat bao thuc 1 truoc
				if(!Alarm1.On_Off && !Alarm2.On_Off){						// Neu ca 2 bao thuc o trang thai "OFF" thi bam OK se thoat ra man hinh chinh
					DS1307_Write(0x0A, 0x00);
					DS1307_Write(0x14, 0x00);
					Choose_Alarm_Active = 0;
				}
			}
			// Khi cai dat xong thoi gian bao thuc se bam thoat ra man hinh chinh
			if(Alarm1.Status_Mode == 10){
				if(Set_ALM2) Prioritized = 1;
				LCD20X4_Clear();
				DS1307_Write_Alarm1();
				Set_ALM1 = Alarm1.Status_Mode = 0;
			}
			
			if(Alarm2.Status_Mode == 10){
				LCD20X4_Clear();
				DS1307_Write_Alarm2();
				Set_ALM2 = Alarm2.Status_Mode = 0;
			}
		}
	}
	Old_Ok = Button_Ok;
		
		if(Set_ALM1 && !Prioritized) Alarm_Pointer = &Alarm1;
		if(Set_ALM2 && Prioritized) Alarm_Pointer = &Alarm2;
	Set_Clock();
	Set_Alarm();
}



void Hien_Thi_LCD(void){
	if(Set_Time && Clock_1Hz) goto nhapNhay;
	if( !Choose_Alarm_Active  && !Set_ALM1 && !Set_ALM2 && !Set_Time){
		nhapNhay:
		LCD20X4_Gotoxy(6,2);
		LCD20X4_PutChar(Hour / 10 | 0x30);
		LCD20X4_PutChar(Hour % 10 | 0x30);
		LCD20X4_PutChar(':');
		LCD20X4_PutChar(Minute / 10 | 0x30);
		LCD20X4_PutChar(Minute % 10 | 0x30);
		LCD20X4_PutChar(':');
		LCD20X4_PutChar(Second / 10 | 0x30);
		LCD20X4_PutChar(Second % 10 | 0x30);
		LCD20X4_PutChar(CountTimer);
	
		LCD20X4_Gotoxy(2,3);
		LCD20X4_PutString(Days[Day-1]);
		LCD20X4_PutString("  ");
		LCD20X4_PutChar(Date / 10 | 0x30);
		LCD20X4_PutChar(Date % 10 | 0x30);
		LCD20X4_PutChar('/');
		LCD20X4_PutChar(Month / 10 | 0x30);
		LCD20X4_PutChar(Month % 10 | 0x30);
		LCD20X4_PutString("/20");
		LCD20X4_PutChar(Year / 10 | 0x30);
		LCD20X4_PutChar(Year % 10 | 0x30);
	}
	if(!Clock_1Hz){
		switch(Status_Mode_Time){
		case 1: {LCD20X4_Gotoxy(9,2);  LCD20X4_PutString("  ") ; break;}
		case 2: {LCD20X4_Gotoxy(6,2);  LCD20X4_PutString("  ");  break;}
		case 3: {LCD20X4_Gotoxy(2,3);  LCD20X4_PutString("   "); break;}
		case 4: {LCD20X4_Gotoxy(7,3);  LCD20X4_PutString("  ");  break;}
		case 5: {LCD20X4_Gotoxy(10,3);  LCD20X4_PutString("  ");  break;}
		case 6: {LCD20X4_Gotoxy(15,3);LCD20X4_PutString("  "); break;}
		default:{break;}
		}
	}
		if( !Choose_Alarm_Active  && !Set_ALM1 && !Set_ALM2 && !Set_Time){
			if(!Error_DHT){
				if(Turn_On_Alarm1 || Turn_On_Alarm2){
					LCD20X4_Gotoxy(3,0);
					LCD20X4_PutString("Bao Thuc ");
				} else {
					LCD20X4_Gotoxy(3,0);
					LCD20X4_PutString("He Thong Nhung");
				}
				LCD20X4_Gotoxy(0,1);
				LCD20X4_PutString("T:");
				LCD20X4_SendFloat(fTemp);
				if(fTemp >= 0.0 && fTemp <= 10.0)	LCD20X4_Gotoxy(5,1);
				LCD20X4_PutChar(0xDF);
				LCD20X4_PutString("C ");
				
				LCD20X4_Gotoxy(13,1);
				LCD20X4_PutString("H:");
				LCD20X4_SendFloat(fHumi);
				if(fHumi >= 0.0 && fHumi <= 10.0)	LCD20X4_Gotoxy(15,1);
				LCD20X4_PutString("% ");
			} else {
				LCD20X4_Gotoxy(0,1);
				LCD20X4_PutString("Not Connect DHT22");
			}	
	}

	/******************************************************* Hien thi bat tat bao thuc **********************************************************/
	if(Choose_Alarm_Active){
		LCD20X4_Gotoxy(3,0);
		LCD20X4_PutString("Setting  Alarm ");
		// Hien thi bao thuc 1 che do on off va nhay nhay "on/off"
		LCD20X4_Gotoxy(1,1);
		LCD20X4_PutString("ALM1:");
		LCD20X4_Gotoxy(6,1);
		if(Clock_1Hz){
			// Hien thi bao thuc 1 che do on/off
			if(Alarm1.On_Off == 1){
				LCD20X4_PutString("ON ");
			}else{
				LCD20X4_PutString("OF ");
			}
			// Hien thi bao thuc 2 che do on/off
			LCD20X4_Gotoxy(12,1);
			LCD20X4_PutString("ALM2:");
			LCD20X4_Gotoxy(17,1);
			if(Alarm2.On_Off == 1){
				LCD20X4_PutString("ON");
			}else{
				LCD20X4_PutString("OF");
			}
		}
		else{
			// Hien thi nhap nhay the hien dang chon bao thuc 1 hoac 2
			if(Choose_Alarm_1_Or_2 == 0){
				LCD20X4_Gotoxy(6,1);
				LCD20X4_PutString("  ");
			}
			if(Choose_Alarm_1_Or_2 == 1){
			LCD20X4_Gotoxy(17,1);
			LCD20X4_PutString("  ");
			}
		}
		
		// Hien thi bao thuc 2 che do on off va nhay nhay "on/off"

	}
	/******************************************************* Het hien thi chon bat/tat bao thuc  **********************************************************/

		
	/********************************************************Hien thi cai dat bao thuc 1 ******************************************************************/
	if( Set_ALM1  || Set_ALM2){
		
		LCD20X4_Gotoxy(0,0);
		if(Alarm_Pointer->Display != 0)	LCD20X4_PutChar(0x7F);    // <-
		else{ LCD20X4_PutChar(' ');}
		
		if(Alarm1.On_Off && !Prioritized)LCD20X4_PutString(" Setting ALM1 ");
		if(Alarm2.On_Off && Prioritized) LCD20X4_PutString(" Setting ALM2 ");
		
		if(Alarm_Pointer->Display != 4)	LCD20X4_PutChar(0x7E);	  // ->
		else { LCD20X4_PutChar(' ');}
		LCD20X4_Gotoxy(0,1);
		
		if(Clock_1Hz){
			if(Alarm_Pointer->Display == 0){
				LCD20X4_PutString("   ");
				LCD20X4_PutChar(Alarm_Pointer->Hour / 10 | 0x30);
				LCD20X4_PutChar(Alarm_Pointer->Hour % 10 | 0x30);
				LCD20X4_PutChar(':');
				LCD20X4_PutChar(Alarm_Pointer->Minute / 10 | 0x30);
				LCD20X4_PutChar(Alarm_Pointer->Minute % 10 | 0x30);
				LCD20X4_PutString("         ");
			}
			if(Alarm_Pointer->Display == 1){
				LCD20X4_PutString("MON: ");
				if(Alarm_Pointer->Days.Day_2 == 1) LCD20X4_PutString("v   ");
				else{ LCD20X4_PutString("x   ");}
				
				LCD20X4_PutString("TUE: ");
				if(Alarm_Pointer->Days.Day_3 == 1) LCD20X4_PutString("v   ");
				else{ LCD20X4_PutString("x   ");}
			}
			if(Alarm_Pointer->Display == 2){
				LCD20X4_PutString("WED: ");
				if(Alarm_Pointer->Days.Day_4 == 1)  LCD20X4_PutString("v   ");
				else{ LCD20X4_PutString("x   ");}
				
				LCD20X4_PutString("THU: ");
				if(Alarm_Pointer->Days.Day_5 == 1)  LCD20X4_PutString("v   ");
				else{ LCD20X4_PutString("x   ");}
			}
			if(Alarm_Pointer->Display == 3){
				LCD20X4_PutString("FRI: ");
				if(Alarm_Pointer->Days.Day_6 == 1)  LCD20X4_PutString("v   ");
				else{ LCD20X4_PutString("x   ");}
				
				LCD20X4_PutString("SAT: ");
				if(Alarm_Pointer->Days.Day_7 == 1)  LCD20X4_PutString("v   ");
				else{ LCD20X4_PutString("x   ");}
			}
			if(Alarm_Pointer->Display == 4){
				LCD20X4_PutString("SUN: ");
				if(Alarm_Pointer->Days.Day_8 == 1)  LCD20X4_PutString("v   ");
				else{ LCD20X4_PutString("x   ");}
				LCD20X4_PutString("EXIT     ");
			}
		}
	
		else{
			switch(Alarm_Pointer->Status_Mode){
				case 1: { LCD20X4_Gotoxy(6,1);   LCD20X4_PutString("  ");	    	break;}
				case 2: { LCD20X4_Gotoxy(3,1);   LCD20X4_PutString("  ");	    	break;}
				case 3: { LCD20X4_Gotoxy(5,1);   LCD20X4_PutChar(' ');   		  	break;}
				case 4: { LCD20X4_Gotoxy(14,1); LCD20X4_PutChar(' ');				break;}
				case 5: { LCD20X4_Gotoxy(5,1);   LCD20X4_PutChar(' ');    		 	break;}
				case 6: { LCD20X4_Gotoxy(14,1); LCD20X4_PutChar(' ');				break;}
				case 7: { LCD20X4_Gotoxy(5,1);   LCD20X4_PutChar(' ');    		 	break;}
				case 8: { LCD20X4_Gotoxy(14,1); LCD20X4_PutChar(' ');				break;}
				case 9: { LCD20X4_Gotoxy(5,1);   LCD20X4_PutChar(' ');    		 	break;}
				case 10: { LCD20X4_Gotoxy(9,1);   LCD20X4_PutString("         ");   break;}
				default: { 						  		   											break;}
			}
		}
	}	
}