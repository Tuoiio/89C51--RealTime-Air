#include <regx51.h>

/**********Khai bao chan ket noi************/
sbit SDA = P1^0;
sbit SCL = P1^1;

/********Khai bao chan giao tiep LCD************/
#define LCD_RS P0_0
#define LCD_RW P0_1
#define LCD_EN P0_2
#define LCD_D4 P0_4
#define LCD_D5 P0_5
#define LCD_D6 P0_6
#define LCD_D7 P0_7

/*******Khai bao giao tiep nut nhan************/
sbit Button_Mode = P2^0;
sbit Button_Ok   = P2^1;
sbit Button_Up   = P2^2;
sbit Button_Down = P2^3;
sbit Buzzer      = P2^4; 

typedef enum {false,true} bool;

sbit DQ = P1^2;

enum{
	READ_ROM = 0x33,
	MATCH_ROM = 0x55,
	SKIP_ROM = 0xCC,
	SEARCH_ROM = 0xF0,
	ALARM_SREACH = 0xEC,
	WRITE_SCRATCHPAD = 0x4E,
	READ_SCRATCHPAD = 0xBE,
	COPY_SCRATCHPAD = 0x48,
	CONVERT = 0x44,
	READ_POWER = 0xB4
};

enum{
	MODE_9BIT = 0x1F,
	MODE_10BIT = 0x3F,
	MODE_11BIT = 0x5F,
	MODE_12BIT = 0x7F
};

char *Days[] =  {"SUN","MON","TUE","WED","THU","FRI","SAT"};
 char Second, Minute, Hour, Date, Day, Month, Year, Mode;
unsigned char Status_Mode_Alarm = 0,Status_Mode_Time = 0,Time_Break_Up = 0, Time_Break_Down = 0,Time_Break_Mode = 0;
bit Old_Ok, Old_Mode,Old_Up,Old_Down, am_pm, Mode12_24, read_time, Choose_Alarm_Active,Choose_Alarm   , Set_Time, Set_ALM1, Set_ALM2, Clock_1Hz;
bit Prioritized, read_temp;
unsigned char a, b;
int iTemp;
float fTemp;

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

/*******************************************CHUONG TRINH GIAO TIEP I2C*******************************************/

/******************** delay cua I2C************************/
void delay_I2C(unsigned int time){
	while(time--);
}
/********************Bat dau gui tin hieu*****************/
void I2C_Start(void){
	SDA = 1;
	SCL = 1;
	SDA = 0;
	delay_I2C(2);
	SCL = 0;
}
/*******************Dung gui tin hieu********************/
void I2C_Stop(void){
	SDA = 0;
	SCL = 0;
	SCL = 1;
	delay_I2C(2);
	SDA = 1;
}

/*********************Nhan ACK tu slave******************/
bit I2C_Get_ACK(void){
	bit Result;
	SDA = 1;
	delay_I2C(2);
	SCL = 1;
	delay_I2C(2);
	Result = SDA;
	SCL = 0;
	return Result;
}

bit I2C_Write(unsigned char Data){
	unsigned char i;
	for(i = 0; i < 8; i++){
		SDA = (Data << i) & 0x80;
		SCL = 1;
		delay_I2C(2); 
		SCL = 0;
	}
	// ACK
	return(I2C_Get_ACK());
}


/********************Gui ACK tu Master******************/
void I2C_ACK(void){
	SDA = 0;
	delay_I2C(2);
	SCL = 1;
	delay_I2C(2);
	SCL = 0;
}
/*******************Gui NACK tu Master********************/
void I2C_NACK(void){
	SDA = 1;
	delay_I2C(2);
	SCL = 1;
	delay_I2C(2);
	SCL = 0;
}

/********************* Doc du lieu tu Slave*****************/
unsigned char I2C_Read(bit ACK){
	unsigned char i, Data = 0x00;
	for(i = 0; i < 8; i++){
		SDA = 1;
		delay_I2C(2);
		SCL = 1;
		delay_I2C(2);
		Data = (Data << 1) | SDA;
		SCL = 0;
	}
	// ACK
	if(ACK){ I2C_ACK();}
	else    {I2C_NACK();}
	return Data;
}

/*******************************************HET CHUONG TRINH GIAO TIEP I2C*******************************************/



/*******************************************CHUONG TRINH GIAO TIEP DS1307*******************************************/

/************************************/
void delay_us(unsigned int t){
        while(t--);
}
void delay_ms(unsigned int t){
        unsigned int i,j;
        for(i=0;i<t;i++)
        for(j=0;j<125;j++);
}

void Bip(void){
	unsigned char i;
	for(i = 0; i < 100; i++){
		Buzzer = 1;
		delay_us(25);
		Buzzer = 0;
		delay_us(25);
		}
}

/**************Ctr giao tiep LCD 16x2 4bit**********************/
void LCD_Enable(void){
        LCD_EN =1;
        delay_us(3);
        LCD_EN=0;
        delay_us(50);
}
//Ham Gui 4 Bit Du Lieu Ra LCD
void LCD_Send4Bit(unsigned char Data){
        LCD_D4=Data & 0x01;
        LCD_D5=(Data>>1)&1;
        LCD_D6=(Data>>2)&1;
        LCD_D7=(Data>>3)&1;
}
// Ham Gui 1 Lenh Cho LCD
void LCD_SendCommand(unsigned char command){
        LCD_Send4Bit(command >>4);/* Gui 4 bit cao */
        LCD_Enable();
        LCD_Send4Bit(command); /* Gui 4 bit thap*/
        LCD_Enable();
}
void LCD_Clear(){// Ham Xoa Man Hinh LCD
        LCD_SendCommand(0x01);
        delay_us(10);
}
// Ham Khoi Tao LCD
void LCD_Init(){
        LCD_Send4Bit(0x00);
        delay_ms(20);
        LCD_RS=0;
        LCD_RW=0;
        LCD_Send4Bit(0x03);
        LCD_Enable();
        delay_ms(5);
        LCD_Enable();
        delay_us(100);
        LCD_Enable();
        LCD_Send4Bit(0x02);
        LCD_Enable();
        LCD_SendCommand( 0x28 ); // giao thuc 4 bit, hien thi 2 hang, ki tu 5x8
        LCD_SendCommand( 0x0c); // cho phep hien thi man hinh
        LCD_SendCommand( 0x06 ); // tang ID, khong dich khung hinh
        LCD_SendCommand(0x01); // xoa toan bo khung hinh
}
void LCD_Gotoxy(unsigned char x, unsigned char y){
        unsigned char address;
        if(!y)address=(0x80+x);
        else address=(0xc0+x);
        delay_us(1000);
        LCD_SendCommand(address);
        delay_us(50);
}
void LCD_PutChar(unsigned char Data){//Ham Gui 1 Ki Tu
        LCD_RS=1;
        LCD_SendCommand(Data);
        LCD_RS=0 ;
}
void LCD_Puts (char *s){//Ham gui 1 chuoi ky tu
        while (*s){
                LCD_PutChar(*s);
                s++;
        }
}
void LCD_SendInteger(int iNumber){
	char i, CacKyTu[10];
	if(iNumber != 0){
		if(iNumber < 0) LCD_PutChar(0x2D);  // dau tru
		for(i = 0; iNumber != 0; i++){
			if(iNumber > 0)
				CacKyTu[i] = iNumber  % 10;
			else
				CacKyTu[i] = (iNumber * -1) % 10;
			iNumber = iNumber / 10;
		}
		while(i > 0){
			LCD_PutChar(CacKyTu[i - 1] | 0x30);
			i--;
		}
	}
	else{
		LCD_PutChar(0 + 0x30);
	}
}

void LCD_SendFloat(float fNumber){
	int a, b;
	a = (int)fNumber;
	if (fNumber >= 0.0) b = (fNumber - a) * 10;
	else{
		b = (fNumber - a) * (-10);
		if(fNumber <= -0.1 && fNumber > -1.0)
			LCD_PutChar(0x2D);
	}
	LCD_SendInteger(a);
	if(b >= 0){
		LCD_PutChar('.');
		LCD_SendInteger(b);
	}
}

void DS18B20_Reset(void) reentrant{
	char Result = 1;
	while(Result){
		DQ = 0;
		delay_us(60);
		DQ = 1;
		delay_us(5);
		Result = DQ;
		delay_us(10);
	}
}

char DS18B20_Read(void){
	char i, Result = 0;
	for(i = 0; i < 8; i++){
		DQ = 0;
		DQ = 1;
		delay_us(3);
		if(DQ){
			Result = Result | (0x01 << i);
			delay_us(6);
		}
	}
	return Result;
}

void DS18B20_Write(char Data) reentrant{
	char i;
	for(i = 0; i < 8; i++){
		DQ = 0;
		Data = Data >> 1;
		DQ = CY;
		delay_us(3);
		DQ = 1;
		delay_us(6);
	}
}

void DS18B20_Init(char aHigh, char aLow, char Mode){
	DS18B20_Reset();
	DS18B20_Write(SKIP_ROM);
	DS18B20_Write(WRITE_SCRATCHPAD);
	DS18B20_Write(aHigh);
	DS18B20_Write(aLow);
	DS18B20_Write(Mode);
}




/*****************Ham doc gia tri cho IC DS1307*******************************/
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
	Mode12_24 = Hour & 0x40;

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
	y = (Year%4)*100;
	// Tang
	if(Minute > 0x3B) Minute = 0;
	if(Hour > 0x17)	Hour = 0;
	if(Day > 0x07)	Day = 1;
	if((Month == 0x02) && (Date > 0x1D) && (y == 0)){	Date = 1;}
	else if((Month == 0x02) && (Date > 0x1C) && (y != 0)){	Date = 1;}
	else if(((Month == 0x04) || (Month == 0x06) || (Month == 0x09) || (Month == 0x0B)) && (Date > 0x1E) && (y != 0)){	Date = 1;	}
	else if((Date > 0x1F)){	Date = 1;}
	if(Month > 0x0C)	Month = 1;
	
//	//Giam
	if(Minute < 0) Minute = 0x3B;
	if(Hour < 0) Hour = 0x17;
	if(Day < 1) Day = 0x07;
	if((Month == 0x02) && (Date < 0x01) && (y == 0)){	Date = 0x1D;}
	else if((Month == 0x02) && (Date < 0x01) && (y != 0)){	Date = 0x1C;}
	else if(((Month == 0x04) || (Month == 0x06) || (Month == 0x09) || (Month == 0x0B)) && (Date < 0x01) && (y != 0)){	Date = 0x1E;	}
	else if((Date < 1))	Date = 0x1F;
	if(Month < 1) Month = 0x0C;
}


void Set_Time_Alarm(void){
	// Neu bat bao thuc 1 thi cai dat bao thuc 1
	switch(Alarm_Pointer->Status_Mode){
		case 1: case 2: { Alarm_Pointer->Display = 0; break; }
		case 3: case 4: { Alarm_Pointer->Display = 1; break; }
		case 5: case 6: { Alarm_Pointer->Display = 2; break; }
		case 7: case 8: { Alarm_Pointer->Display = 3; break; }
		case 9: case 10:{ Alarm_Pointer->Display = 4; break; }
		case 11: { Alarm_Pointer->Status_Mode = 1; break;}
		default: { break; }
	}

	// Tang
	if(Alarm_Pointer->Minute > 59) Alarm_Pointer->Minute = 0;
	if(Alarm_Pointer->Hour > 23) Alarm_Pointer->Hour  = 0;
	// Giam
	if(Alarm_Pointer->Minute < 0) Alarm_Pointer->Minute = 59;
	if(Alarm_Pointer->Hour < 0) Alarm_Pointer->Hour = 23;
	
	if(Alarm1.Hour == Hour && Alarm1.Minute == Minute){	
		if(Alarm1.Days.Day_2 == (Day == 2)) Bip();
		if(Alarm1.Days.Day_3 == (Day == 3)) Bip();
		if(Alarm1.Days.Day_4 == (Day == 4)) Bip();
		if(Alarm1.Days.Day_5 == (Day == 5)) Bip();
		if(Alarm1.Days.Day_6 == (Day == 6)) Bip();
		if(Alarm1.Days.Day_7 == (Day == 7)) Bip();
		if(Alarm1.Days.Day_8 == (Day == 1)) Bip();
	}
	
	if(Alarm2.Hour == Hour && Alarm2.Minute == Minute){	
		if(Alarm2.Days.Day_2 == (Day == 2)) Bip();
		if(Alarm2.Days.Day_3 == (Day == 3)) Bip();
		if(Alarm2.Days.Day_4 == (Day == 4)) Bip();
		if(Alarm2.Days.Day_5 == (Day == 5)) Bip();
		if(Alarm2.Days.Day_6 == (Day == 6)) Bip();
		if(Alarm2.Days.Day_7 == (Day == 7)) Bip();
		if(Alarm2.Days.Day_8 == (Day == 1)) Bip();
	}
}
	

void KeyBoard(void){

	/*******************************************Chon che do ************************************/
		if(Button_Mode == 0){
			delay_ms(10);
			Time_Break_Mode++;
			if(Time_Break_Mode > 30 && Time_Break_Mode < 32){
				Bip();
				LCD_Clear();
				Choose_Alarm_Active = 1;
				Set_Time =  Status_Mode_Time = 0;
			}
			if(Time_Break_Mode > 58 && Time_Break_Mode < 60){
				Bip();
				LCD_Clear();
				while(!Button_Mode){
					LCD_Gotoxy(0,0);
					LCD_Puts("Setting Clock");
					Second = DS1307_Read(0x00); // tranh truong hop bi mat thoi gian vi giu phim lau
				}
				LCD_Clear();
				Choose_Alarm_Active = Choose_Alarm = 0;
				Set_Time = Status_Mode_Time = 1;
			}
			if(!Button_Mode && Old_Mode ){
				Bip();
				if(Set_Time){
					Status_Mode_Time++;
					if(Status_Mode_Time > 6) Status_Mode_Time = 1;
				}
				// Chi chon bat hoac tat bao thuc 1, 2. Chon xong khi bam ok (Set_Alarm) se ve muc 0
				if( Choose_Alarm_Active )	Choose_Alarm = ~Choose_Alarm;
				
				// Chon che do cho bao thuc 1 ,2
				if(Set_ALM1 && !Prioritized)	Alarm1.Status_Mode++;
				if(Set_ALM2 && Prioritized)	Alarm2.Status_Mode++;
			}
		}else{
			Time_Break_Mode = 0;
		}
	Old_Mode = Button_Mode;

		
	/*******************************************Nut bam tang gia tri ************************************/
	if(Button_Up == 0){
		delay_ms(10);
		// Nhan giu 1 khoang thoi gian thi gia tri lien tuc thi tang lien tuc
		Time_Break_Up++;
		if(Time_Break_Up >12){
			Time_Break_Up = 13;			// Tranh tran gia tri 
			Old_Up = 1;
		}
		// Nhan nha luon thi cong 1 gia tri
		if(!Button_Up && Old_Up){
			if(Time_Break_Up < 13) Bip();
			if(Set_Time){								// Cai dat thoi gian
				switch(Status_Mode_Time){
					case 1: { Minute++; break;}
					case 2: { Hour++;    break;}
					case 3: { Day++;     break;}
					case 4: { Date++;    break;}
					case 5: { Month++; break;}
					case 6: { Year++;    break;}
					default:{               break;}
				}
			}
			
			// Chi chon bat hoac tat bao thuc 1, 2. Chon xong khi bam ok (Set_Alarm) se ve muc 0
			if(Choose_Alarm_Active){
				if(Choose_Alarm == 0) Alarm1.On_Off = 1;
				if(Choose_Alarm == 1) Alarm2.On_Off = 1;
			}
			
			// Cai dat thoi gian bao thuc ngay bao thuc
			switch(Alarm_Pointer->Status_Mode){
				case 1: { Alarm_Pointer->Minute++; 		   break;}
				case 2: { Alarm_Pointer->Hour++;    		   break;}
				case 3: { Alarm_Pointer->Days.Day_2 = 1;   break;}
				case 4: { Alarm_Pointer->Days.Day_3 = 1;   break;}
				case 5: { Alarm_Pointer->Days.Day_4 = 1;   break;}
				case 6: { Alarm_Pointer->Days.Day_5 = 1;   break;}
				case 7: { Alarm_Pointer->Days.Day_6 = 1;   break;}
				case 8: { Alarm_Pointer->Days.Day_7 = 1;   break;}
				case 9: { Alarm_Pointer->Days.Day_8 = 1;   break;}
				default: { 						  break;}
			}
			

		}
	}
	else{
		Time_Break_Up = 0;
	}
	Old_Up = Button_Up;
	
	
	/*******************************************Nut bam giam gia tri ************************************/
	if(Button_Down == 0){
		delay_ms(10);
		// Nhan giu 1 khoang thoi gian thi gia tri giam lien tuc 
		Time_Break_Down++;
		if(Time_Break_Down > 12){
			Time_Break_Down = 13;
			Old_Down = 1;
		}
		// Nhan nha luon thi gia tri giam 1 don vi
		if(!Button_Down && Old_Down){
			if(Time_Break_Down < 13) Bip();
			if(Set_Time){
				switch(Status_Mode_Time){
					case 1: { Minute--; break;}
					case 2: { Hour--;    break;}
					case 3: { Day--;      break;}
					case 4: { Date--;     break;}
					case 5: { Month--;  break;}
					case 6: { Year--;     break;}
					default:{               break;}
				}
			}
			// Chi chon bat hoac tat bao thuc 1, 2. Chon xong khi bam ok (Set_Alarm) se ve muc 0
			if(Choose_Alarm_Active){
				if(Choose_Alarm == 0) Alarm1.On_Off = 0;
				if(Choose_Alarm == 1) Alarm2.On_Off = 0;
			}
			switch(Alarm_Pointer->Status_Mode){
				case 1: { Alarm_Pointer->Minute--; 		   break;}
				case 2: { Alarm_Pointer->Hour--;    		   break;}
				case 3: { Alarm_Pointer->Days.Day_2 = 0;   break;}
				case 4: { Alarm_Pointer->Days.Day_3 = 0;   break;}
				case 5: { Alarm_Pointer->Days.Day_4 = 0;   break;}
				case 6: { Alarm_Pointer->Days.Day_5 = 0;   break;}
				case 7: { Alarm_Pointer->Days.Day_6 = 0;   break;}
				case 8: { Alarm_Pointer->Days.Day_7 = 0;   break;}
				case 9: { Alarm_Pointer->Days.Day_8 = 0;   break;}
				default: { 						  break;}
			}
		}
	}
	else{
		Time_Break_Down = 0;
	}
	Old_Down = Button_Down;
			
	
	/*******************************************Nut bam hoan thanh ************************************/
	if(Button_Ok == 0){
		delay_ms(10);
		if((!Button_Ok) &&  Old_Ok){
			Bip();
			if(Set_Time){
				LCD_Clear();
				Status_Mode_Time  = Set_Time = 0;
				DS1307_Write_Time();
			}
			if(Choose_Alarm_Active){
				if(Alarm1.On_Off){ 
					Set_ALM1 = Alarm1.Status_Mode = 1;
					Choose_Alarm_Active = 0;
				}
				if(Alarm2.On_Off){
					Set_ALM2 = Alarm2.Status_Mode = 1;
					Choose_Alarm_Active = 0;
				}
				if( !Alarm1.On_Off && Alarm2.On_Off) Prioritized = 1; // trg hop chi bat bao thuc 2 thi Prioritized = 1 luon
				
				if(Alarm1.On_Off && Alarm2.On_Off) Prioritized = 0; // show_in_turn = 0 hien thi bao thuc 1 sau khi cai dat xong bao thuc 1 thi Prioritized = 1;
				
				// Neu khong muon cai dat bao thuc thi de ca 2 bao thuc o muc 0 khi bam ok se thoat ra man hinh chinh
				if( !Alarm1.On_Off && !Alarm2.On_Off ){
					DS1307_Write(0x0A,0x00);           // neu tat ca 2 bao thuc thi se ghi xuong ds1307 la trang thai off
					DS1307_Write(0x14, 0x00);
					Choose_Alarm_Active = 0;
				}
				// Neu bat ca 2 bao thuc thi se hien thi cai dat bao thuc 1 trc xong den bao thuc 2
			}
			// Khi cai dat xong thoi gian bao thuc se bam thoat ra man hinh chinh
			if(Alarm1.Status_Mode == 10){
				if(Set_ALM2) Prioritized = 1;
				DS1307_Write_Alarm1();
				Set_ALM1 = Alarm1.Status_Mode = 0;
			}
			
			if(Alarm2.Status_Mode == 10){
				DS1307_Write_Alarm2();
				Set_ALM2 = Alarm2.Status_Mode = 0;
			}
		}
	}
	Old_Ok = Button_Ok;
	
		if(Set_ALM1 && !Prioritized) Alarm_Pointer = &Alarm1;
		if(Set_ALM2 && Prioritized) Alarm_Pointer = &Alarm2;
	
		Set_Clock();
		Set_Time_Alarm();
}
			
		
void HienThiLCD(void){
	if(Set_Time && Clock_1Hz) goto nhapNhay;
	if( !Choose_Alarm_Active  && !Set_ALM1 && !Set_ALM2 && !Set_Time){
		nhapNhay:
		LCD_Gotoxy(0,0);
		LCD_PutChar(Hour / 10 | 0x30);
		LCD_PutChar(Hour % 10 | 0x30);
		LCD_PutChar(':');
		LCD_PutChar(Minute / 10 | 0x30);
		LCD_PutChar(Minute % 10 | 0x30);
		LCD_PutChar(':');
		LCD_PutChar(Second / 10 | 0x30);
		LCD_PutChar(Second % 10 | 0x30);
		
//		if(!Set_Time){
//			LCD_PutChar(' ');
//			LCD_SendFloat(fTemp);
//			if(fTemp > 0.0 && fTemp < 10.0) LCD_Gotoxy(12,0);
//			LCD_PutChar(0xDF);
//			LCD_Puts("C   ");
//		}
		
	
		LCD_Gotoxy(2,1);
		LCD_Puts(Days[Day-1]);
		LCD_PutChar(' ');
		LCD_PutChar(Date / 10 | 0x30);
		LCD_PutChar(Date % 10 | 0x30);
		LCD_PutChar('/');
		LCD_PutChar(Month / 10 | 0x30);
		LCD_PutChar(Month % 10 | 0x30);
		LCD_Puts("/20");
		LCD_PutChar(Year / 10 | 0x30);
		LCD_PutChar(Year % 10 | 0x30);
	}
	if(!Clock_1Hz){
		switch(Status_Mode_Time){
		case 1: {LCD_Gotoxy(3,0);  LCD_Puts("  ") ; break;}
		case 2: {LCD_Gotoxy(0,0);  LCD_Puts("  ");  break;}
		case 3: {LCD_Gotoxy(2,1);  LCD_Puts("   "); break;}
		case 4: {LCD_Gotoxy(6,1);  LCD_Puts("  ");  break;}
		case 5: {LCD_Gotoxy(9,1);  LCD_Puts("  ");  break;}
		case 6: {LCD_Gotoxy(14,1); LCD_Puts("  "); break;}
		default:{break;}
		}
	}

	/******************************************************* Hien thi bat tat bao thuc **********************************************************/
	if(Choose_Alarm_Active){
		LCD_Gotoxy(0,0);
		LCD_Puts(" Setting  Alarm ");
		// Hien thi bao thuc 1 che do on off va nhay nhay "on/off"
		LCD_Gotoxy(0,1);
		LCD_Puts("ALM1:");
		LCD_Gotoxy(5,1);
		if(Clock_1Hz){
			// Hien thi bao thuc 1 che do on/off
			if(Alarm1.On_Off == 1){
				LCD_Puts("ON ");
			}else{
				LCD_Puts("OF ");
			}
			// Hien thi bao thuc 2 che do on/off
			LCD_Gotoxy(9,1);
			LCD_Puts("ALM2:");
			LCD_Gotoxy(14,1);
			if(Alarm2.On_Off == 1){
				LCD_Puts("ON");
			}else{
				LCD_Puts("OF");
			}
		}
		else{
			// Hien thi nhap nhay the hien dang chon bao thuc 1 hoac 2
			if(Choose_Alarm == 0){
				LCD_Gotoxy(5,1);
				LCD_Puts("  ");
			}
			if(Choose_Alarm == 1){
			LCD_Gotoxy(14,1);
			LCD_Puts("  ");
			}
		}
		
		// Hien thi bao thuc 2 che do on off va nhay nhay "on/off"

	}
	/******************************************************* Het hien thi chon bat/tat bao thuc  **********************************************************/

		
	/********************************************************Hien thi cai dat bao thuc 1 ******************************************************************/
	if( Set_ALM1  || Set_ALM2){
		
		LCD_Gotoxy(0,0);
		if(Alarm_Pointer->Display != 0)	LCD_PutChar(0x7F);    // <-
		else{ LCD_PutChar(' ');}
		
		if(Alarm1.On_Off && !Prioritized)LCD_Puts(" Setting ALM1 ");
		if(Alarm2.On_Off && Prioritized) LCD_Puts(" Setting ALM2 ");
		
		if(Alarm_Pointer->Display != 4)	LCD_PutChar(0x7E);	  // ->
		else { LCD_PutChar(' ');}
		LCD_Gotoxy(0,1);
		
		if(Clock_1Hz){
			if(Alarm_Pointer->Display == 0){
				LCD_Puts("   ");
				LCD_PutChar(Alarm_Pointer->Hour / 10 | 0x30);
				LCD_PutChar(Alarm_Pointer->Hour % 10 | 0x30);
				LCD_PutChar(':');
				LCD_PutChar(Alarm_Pointer->Minute / 10 | 0x30);
				LCD_PutChar(Alarm_Pointer->Minute % 10 | 0x30);
				LCD_Puts("         ");
			}
			if(Alarm_Pointer->Display == 1){
				LCD_Puts("MON: ");
				if(Alarm_Pointer->Days.Day_2) LCD_Puts("v   ");
				else{ LCD_Puts("x   ");}
				
				LCD_Puts("TUE: ");
				if(Alarm_Pointer->Days.Day_3)  LCD_Puts("v   ");
				else{ LCD_Puts("x   ");}
			}
			if(Alarm_Pointer->Display == 2){
				LCD_Puts("WED: ");
				if(Alarm_Pointer->Days.Day_4)  LCD_Puts("v   ");
				else{ LCD_Puts("x   ");}
				
				LCD_Puts("THU: ");
				if(Alarm_Pointer->Days.Day_5)  LCD_Puts("v   ");
				else{ LCD_Puts("x   ");}
			}
			if(Alarm_Pointer->Display == 3){
				LCD_Puts("FRI: ");
				if(Alarm_Pointer->Days.Day_6)  LCD_Puts("v   ");
				else{ LCD_Puts("x   ");}
				
				LCD_Puts("SAT: ");
				if(Alarm_Pointer->Days.Day_7)  LCD_Puts("v   ");
				else{ LCD_Puts("x   ");}
			}
			if(Alarm_Pointer->Display == 4){
				LCD_Puts("SUN: ");
				if(Alarm_Pointer->Days.Day_8)  LCD_Puts("v   ");
				else{ LCD_Puts("x   ");}
				LCD_Puts("EXIT     ");
			}
		}
	
		else{
			switch(Alarm_Pointer->Status_Mode){
				case 1: { LCD_Gotoxy(6,1);   LCD_Puts("  ");	    	break;}
				case 2: { LCD_Gotoxy(3,1);   LCD_Puts("  ");	    	break;}
				case 3: { LCD_Gotoxy(5,1);   LCD_PutChar(' ');     	break;}
				case 4: { LCD_Gotoxy(14,1); LCD_PutChar(' ');		break;}
				case 5: { LCD_Gotoxy(5,1);   LCD_PutChar(' ');     	break;}
				case 6: { LCD_Gotoxy(14,1); LCD_PutChar(' ');		break;}
				case 7: { LCD_Gotoxy(5,1);   LCD_PutChar(' ');     	break;}
				case 8: { LCD_Gotoxy(14,1); LCD_PutChar(' ');		break;}
				case 9: { LCD_Gotoxy(5,1);   LCD_PutChar(' ');     	break;}
				case 10: { LCD_Gotoxy(9,1);   LCD_Puts("         ");  break;}
				default: { 						  		   						break;}
			}
		}
	}	
}

void DS1307_Init(){
	// ham nay co chuc nang reset bit ch = 0 o dia chi 0x7F de bat bo giao dong tinh the thach anh
	unsigned char temp;
	temp = DS1307_Read(0x00);
	temp &= 0x7F;
	DS1307_Write(0x00,temp);
}

void Set_Timer(void){
	TMOD &= 0xF0;
	TMOD |= 0x01;
	EA = 1;
	ET0 = 1;
	TF0 = 1;
	IT1 = 1;
}

void main(){
	Set_Timer();
	DS1307_Init();
	//DS18B20_Init(0,0,MODE_12BIT);
	LCD_Init();
	DS1307_Read_Alarm();
	while(1){
		if(Status_Mode_Time == 0 && read_time == 1){
			read_time = 0;
				DS1307_Read_Time();
		}
		/*if(read_temp){
			read_temp = 0;
			DS18B20_Reset();
			DS18B20_Write(SKIP_ROM);
			DS18B20_Write(READ_SCRATCHPAD);
			a = DS18B20_Read();
			b = DS18B20_Read();
			iTemp = (b << 8) | a;
			fTemp = (float)(iTemp)/16.0;
		}
			*/
		//KeyBoard();
		HienThiLCD();
	}
}

unsigned char CountTimer = 0, Count2 = 0;
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
	if(Count2 == 3){ 
		Count2 = 0;
		DS18B20_Reset();
		DS18B20_Write(SKIP_ROM);
		DS18B20_Write(CONVERT);
		read_temp = 1;
	}
}












		
		
		
		
	

