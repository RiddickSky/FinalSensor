#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "MacroAndConst.h"
#include "DS1302DY.h"
#include "LCD1602YT.h"
#include "18B20.h"
#include "key44y.h"
#include "UartPrintf.h"

sbit LED = P3^3;
sbit BELL = P3^7;

char i=0; //ȫ�ְ���SubSw��־  �Ӳ˵�����9

char TemF;	// �¶ȵı�־λ
//char TimeString[21],TempString[9]; // �ֱ���ʱ����¶ȴ����ַ���   ���ַ���  �����ǵ���ֵת������  ��ChangAllValueToString()�����н��� 
//char KeyValMain = 0;  //main������4*4����ɨ��ֵ

//uint8 DS1302[7] ; //��DS1302�е�������ʱ�������������  ������DS1302Ch����Ľ���
unsigned char DS1302Ch[7] = {0};
unsigned char DS1302Limit[7] = {99,12,31,23,59,59,7};

uint8 volatile PageF = 0;
uint8 volatile SubSw = 0;

int8 TiAH = 8;	//����Сʱ
int8 TiAM = 1;	//���ӷ���
bit   TAOC = 1; //���ӿ��ر�־λ


uint8 TeAH = 30; //DS18B20���������¶ȵ���������
//uint8 TeAHFloat = 5;   // DS18B20���������¶ȵ�С������
bit TeAHF	= 1;   // ����λ

uint8 TeAL = 20;  //DS18B20���������¶ȵ���������
//uint8 TeALFloat = 4;
bit TeALF	= 0;

uint8 YearCh = 10;
uint8 MontCh = 9 ;

void LCD1602_Display_1302();
void LCD1602_Display_18B20();
void SystemIni( );
void DisplayTempTime( );
void KeyEqu2( ); // �жϰ����Ƿ�Ϊ2
void KeyEqu9( ); // ����9 Ϊ��ѡ��� ��ӦSubSW����
void DisplayTimeAlarm();
void DisplayTempAlarm();
void KeyEqu1_Plus( uint8 i);
void KeyEqu5_Sub(uint8 i);
void SetDs1302Display(uint8 i);
//void Bell(void);
void delay1ms();   // �����ʱ��������֪����Լ���,ֻ�������ڹ���Bell����΢��ʱһ�°���
uint8 KeyData_Plus( uint8 Data );   //��Ӧ�ı�����
uint8 KeyData_Sub ( uint8 Data ); //��Ӧ�ı�����
void TimeAlarmBell(); //���ӷ���������
void TempAlarmBellLed(); // �¶ȱ�������
void JudgeMonthDays(void); 	 // �����·ݵ��������
uint8 GetWeekFromDay(unsigned char year,unsigned char month,unsigned char day);//������һ�������ڼ�

/***************************************************************************************************/
void UartService(void)  interrupt 4
{
	 char RecieveBuff = 0;
	EA = 0;
	ES = 0;
   //************************����λ����������ʱҪ��16���Ʒ���,����׼ȷ***************************//
		if(1 == RI)
		{
			RecieveBuff = SBUF;				// ������յ�������
			RI = 0;						// ���ձ�־����
		}
		if(RecieveBuff == 0)
		{
			LED = 0;
			BELL = 1;
			delay1ms();
			delay1ms();
			delay1ms();
			Uart_Printf("%nGet %d%n",RecieveBuff);
		}
	else	if(RecieveBuff == 1)
		{
			LED = 0;  

			BELL = 0;
		
			delay1ms();
			Uart_Printf("%nGet %d%n",RecieveBuff);
		}
	else	if(RecieveBuff == 2)
		{
			LED  = 0;  

			BELL = 0;
			delay1ms();
			delay1ms();
			delay1ms();
			delay1ms();
			delay1ms();
			delay1ms();
			Uart_Printf("%nGet %d%n",RecieveBuff);
		}
//	else
	if(RecieveBuff == 'W')	  //�������ʱ��λ�����ַ�ģʽ(�ô��ڵ��������) ���ص�Ҳ��W  ע���շ������ַ�ģʽ(��Ҫѡ16����)
		{
			LED = 0;  

			BELL = 0;
			delay1ms();
			delay1ms();
			delay1ms();
				delay1ms();
			delay1ms();
			delay1ms();
				delay1ms();
			delay1ms();
			delay1ms();
				delay1ms();
			delay1ms();
			delay1ms();
			Uart_Printf("%nGet %c%n",RecieveBuff);	
		}
		BELL = 1;
/*
	if(state == 1)
		{
			BELL = 0;
			LED = 0;
			delay1ms();
		}
	if(state == 2)
	{
		BELL = 1;
		LED  = 1;
		delay1ms();
	}
*/	
	EA = 1;
	ES = 1;
	//TR1 = 1;

}


void main()
{
   SystemIni( );
 
  while(1)
  {
  	/**********************************
	while(0 == RI);
	if(1 == RI)
		{
			RI = 0;
			LED = 0;
		}
     *******************************/

	 Uart_Printf("Hello%n");
//	UART_TXDSTRING("Test the Uart is OK !! From Func", 1);
//	    printf("From Printf");


	   TempAlarmBellLed(); // �¶ȱ�������
//=============================---1---Time Temperature Display==========================================//	
  	    SubSw = 0;
	  while(PageF == 2)
	  {
		 TimeAlarmBell();//Time Alarm ����
		 TempAlarmBellLed();

		for( i = 0 ; i<8; i++)	// iΪSubSw9����ֵ
		{
			//Clear1602 	 // ����  ������ȥ���ӱ�־��û��ָ����λ�ö���ʾ

			TimeAlarmBell();
			TempAlarmBellLed(); // �¶ȱ�������
		
			if(1 == SubSw)		DS1302Ch[0] = ReadYs();
			if(2 == SubSw)		DS1302Ch[1] = ReadMn();
			if(3 == SubSw)		DS1302Ch[2] = ReadDay();
			if(4 == SubSw)		DS1302Ch[3] = ReadHr();
			if(5 == SubSw)		DS1302Ch[4] = ReadMin();
			if(6 == SubSw)		DS1302Ch[5] = ReadSec();
			if(7 == SubSw)		DS1302Ch[6] = ReadWe();
		 
			if(2 != PageF)	break;

			while(i == SubSw)
		    {

				if(SubSw == 0)	break; 

				SetDs1302Display(SubSw);

				if(0 == DS1302Ch[6])  //��ֹ���ڳ���0  ����Ϊunsigned char ��  ��SetDs1302Display()�����мӼ�ʱ�ܳ���0
					DS1302Ch[6] = 1;
				
				if(0 == DS1302Ch[1])  //��ֹ�·ݳ���0  �·�Ϊunsigned char ��  ��SetDs1302Display()�����мӼ�ʱ�ܳ���0
					DS1302Ch[1] = 1;


//******************��������α߽��ж��� �õ���DS1302Ch[]���� �������֮ǰ��Ҫ��ˢ��һ���������****************************//
//*****************���庯����SystemIni()ϵͳ��ʼ��������(	DS1302Ch[0] = ReadYs();...���)*********************************//
				JudgeMonthDays(); 
				if(DS1302Ch[i-1] > DS1302Limit[i-1])	// ��ֹ����ʱ��ʱ����  ���Сʱʱ ����24
				{
					DS1302Ch[i-1] = DS1302Limit[i-1]; 
				}
			
				if(0 == DS1302Ch[i-1] && KeyScanValue()==5)	 //
				{
					DS1302Ch[i-1] = DS1302Limit[i-1];
				}
				if(1 == DS1302Ch[1]&& KeyScanValue()==5)
					DS1302Ch[1] = 12;
						
				if(DS1302Limit[i-1] == DS1302Ch[i-1] && KeyScanValue()==1)
				{
					if( 1 != i) DS1302Ch[i-1] = 0;  // ���ڴ�1��ʼ ����0
					else 
						DS1302Ch[i-1] = 1;
				}

				if(1 == i)	
				{
					WriteYs (DS1302Ch[0]);  WriteWe( GetWeekFromDay(ReadYs(), ReadMn(), ReadDay()) );   //�Զ��������� 
				}
				if(2 == i)
				{
					WriteMn (DS1302Ch[1]); WriteWe( GetWeekFromDay(ReadYs(), ReadMn(), ReadDay()) );   //�Զ��������� 
				}
				if(3 == i)
				{
					WriteDay(DS1302Ch[2]);	WriteWe( GetWeekFromDay(ReadYs(), ReadMn(), ReadDay()) );   //�Զ��������� 
				}
				if(4 == i)
				{
					WriteHr (DS1302Ch[3]);WriteWe( GetWeekFromDay(ReadYs(), ReadMn(), ReadDay()) );   //�Զ��������� 
				}
				if(5 == i)
				{
					WriteMin(DS1302Ch[4]);WriteWe( GetWeekFromDay(ReadYs(), ReadMn(), ReadDay()) );   //�Զ��������� 
				}

				if(6 == i)
				{
					WriteSec(DS1302Ch[5]);WriteWe( GetWeekFromDay(ReadYs(), ReadMn(), ReadDay()) );   //�Զ��������� 
				}
				if(7 == i)
				{ 
				 WriteWe( GetWeekFromDay(ReadYs(), ReadMn(), ReadDay()) );   //�Զ���������  ע������������� 7==i �������²�����
				 															 //����ֻ�а�9��(ʹSubSw==7)ʹi==7,������while(i==SubSw)������
																			 //�����Ҫ��9�� �ù����������λ�� �Ż��Զ�����
				//	WriteWe (DS1302Ch[6]);
				}
		
				if(0 == SubSw) 			
				{
				  TimeAlarmBell();
				  TempAlarmBellLed(); // �¶ȱ�������
				  WriteC( NOFlashCur1602 , 1 );
			  
				  DisplayTempTime( );
				  KeyEqu9( );
				}

		/**************����������������ù����ʾ����***�����Ҳһ��****************/		  
				  delay1ms();
				  WriteC( NOFlashCur1602 , 1 );
		/***************************************************************************/
			    DisplayTempTime( );
			    KeyEqu9( );	
			    KeyEqu2( );
				if(2 != PageF)	break;	
			}

			while(0 == SubSw)
			{
				TimeAlarmBell();
	
				DisplayTempTime( );
			    KeyEqu9( );	
			    KeyEqu2( );
		 		if(2 != PageF)	break;
			}

		 }

			KeyEqu9( );
			if(PageF != 2)	break;
		}
//===============================---2--Time Alarm Set============================================// 
	  //LCDRst();
		Clear1602 	 // ����  ������ȥ���ӱ�־��û��ָ����λ�ö���ʾ
	  SubSw = 0;
	   while(PageF == 1)
	  {
		  TempAlarmBellLed(); // �¶ȱ�������
	  	while(0 == SubSw)
		{
			KeyEqu9( );
			KeyEqu2( );
	  		DisplayTimeAlarm();
			if(PageF != 1)	break;		
		}
	/************************************************************/
		while(1 == SubSw)
		{
			
			LcdSetXY(0,1);
			WriteC( FlashCur1602 , 1 );

		    if( KeyScanValue()==5||KeyScanValue()==1)
				TAOC = ~TAOC;
				/***********************************/
				delay1ms();
				WriteC( NOFlashCur1602 , 1 );
				
			 DisplayTimeAlarm();

			KeyEqu9( );
			KeyEqu2( );
			if(PageF != 1)	break;	
		}
		WriteC( NOFlashCur1602 , 1 );

		while(2 == SubSw)
		{
			LcdSetXY(4,1);
			WriteC( FlashCur1602 , 1 );
			
			TiAH = KeyData_Plus( TiAH );
 			TiAH = KeyData_Sub ( TiAH );
			if(TiAH > 23) 
				TiAH = 0;
			if(TiAH < 0)
				TiAH = 23;
		
			delay1ms();
			WriteC( NOFlashCur1602 , 1 );

		    DisplayTimeAlarm();

			KeyEqu9( );
			KeyEqu2( );
			if(PageF != 1)
			break;	
		}
		while(3 == SubSw)
		{
			LcdSetXY(7,1);
			WriteC( FlashCur1602 , 1 );
			
			TiAM = KeyData_Plus( TiAM );
 			TiAM = KeyData_Sub ( TiAM );
		  	 
			 if(TiAM > 59) 
				TiAM = 0;
			 if(TiAM < 0)
			    TiAM = 59;
		    delay1ms();
				WriteC( NOFlashCur1602 , 1 );

			DisplayTimeAlarm();

			KeyEqu9( );
			KeyEqu2( );
			if(PageF != 1)
			break;	
		}

		if(4 == SubSw)//���������������ǽ�������ﰴ������(2��,��Ӧ����ΪPageF)��Ч
			break;	
	//����������������KeyEqu2()������һ����  	
		//KeyEqu2( );
		KeyEqu9( );
	
		if(PageF != 1)
			break;
	  }

//===============================---3--Temperature Alarm Set============================================// 
	    LCDRst();
	    SubSw = 0;
		//TempAlarmBellLed(); // �¶ȱ�������
	  while(PageF == 0)
	  {
		TempAlarmBellLed(); // �¶ȱ�������
		while(0 == SubSw)
		{
		 	TempAlarmBellLed(); // �¶ȱ�������
			KeyEqu9( );		
	  		DisplayTempAlarm();	
			KeyEqu2( );
			if(PageF != 0)
				break;
			break;		
		}

	  while(1 == SubSw)
		{	
			TempAlarmBellLed(); // �¶ȱ�������		
			LcdSetXY(3,1);
			WriteC( FlashCur1602 , 1 );
			
		    if( KeyScanValue()==5||KeyScanValue()==1)
				TeALF = ~TeALF;
		  
		    delay1ms();
			WriteC( NOFlashCur1602 , 1 );
		
	  		DisplayTempAlarm();	

			KeyEqu9( );	
			
			KeyEqu2( );
			if(PageF != 0)
				break;
			
		}

			while(2 == SubSw)
		{	
			TempAlarmBellLed(); // �¶ȱ�������			
			LcdSetXY(5,1);
			WriteC( FlashCur1602 , 1 );
			
		    TeAL = KeyData_Plus( TeAL );
 			TeAL = KeyData_Sub ( TeAL );
		  
		    delay1ms();
			WriteC( NOFlashCur1602 , 1 );
		
	  		DisplayTempAlarm();	

			KeyEqu9( );	
			
			KeyEqu2( );
			if(PageF != 0)
				break;
			
		}
		 
			while(3 == SubSw)
		{
			TempAlarmBellLed(); // �¶ȱ�������
			LcdSetXY(11,1);
			WriteC( FlashCur1602 , 1 );
			
		    if( KeyScanValue()==5||KeyScanValue()==1)
				TeAHF = ~TeAHF;
		  
		    delay1ms();
			WriteC( NOFlashCur1602 , 1 );
		
	  		DisplayTempAlarm();	

			KeyEqu9( );	
			KeyEqu2( );
			if(PageF != 0)
				break;
			
		}

			while(4 == SubSw)
		{	
			TempAlarmBellLed(); // �¶ȱ�������			
			LcdSetXY(13,1);
			WriteC( FlashCur1602 , 1 );
			
		    TeAH = KeyData_Plus( TeAH );
 			TeAH = KeyData_Sub ( TeAH );
		  
		    delay1ms();
			WriteC( NOFlashCur1602 , 1 );
		
	  		DisplayTempAlarm();	

			KeyEqu9( );	
			
			KeyEqu2( );
			if(PageF != 0)
				break;
			
		}
		if(SubSw == 5)
			break;	
	//	KeyEqu2( );		
		if(PageF != 0)
			break;
	  }
	 
	
  }
}




void SystemIni(void)
{
	LCDRst();
	DS1308_init();   // Ӳ������ϴ���ʧ��:  û�м���������  �����������д����ô�����85���ֵ
	DS1302Ch[0] = ReadYs ();
	DS1302Ch[1] = ReadMn ();
	DS1302Ch[2] = ReadDay();
	DS1302Ch[3] = ReadHr ();
	DS1302Ch[4] = ReadMin();
	DS1302Ch[5] = ReadSec();
	DS1302Ch[6] = ReadWe ();

	LCDRst();

//	DS1302_Write_Ini_Data(10,12,31,23,59,55,2); //��Ϊ�Ѿ���д��Ͷ�����ֵ��ת����BCD��  ����ֱ�Ӹ�10���Ƶ����Ϳ�����

 	// ������18B20ת��2�� ��һ��get_temperature�Ƕ���Ĭ�ϵ�+85 
	// �ڶ�����DS18B20��ɵ�һ��ת��,�����¶�ֵ,
	// �����¶ȱ����ĵ�һ���жϴ���(��TempAlarmBellLed������һ���ж�)
	get_temperature();
	get_temperature();
	
	Uart_Init(4800);
}

void LCD1602_Display_1302()
{
 
	DisC(0,0,ys/10+'0');
	DisC(1,0,ys%10+'0');
	
	DisC(3,0,mn/10+'0');
	DisC(4,0,mn%10+'0');
	
	DisC(6,0,day/10+'0');  
	DisC(7,0,day%10+'0');
	
	DisC(0,1,hr/10+'0');
	DisC(1,1,hr%10+'0');

	   	DisC(2,1,':');

	DisC(3,1,min/10+'0');
	DisC(4,1,min%10+'0');
		
		DisC(5,1,':');
	
	
	DisC(6,1,sec/10+'0');
	DisC(7,1,sec%10+'0');

	DisS(10,1,"Week:");		
	DisC(15,1,we+'0');
	
}



void LCD1602_Display_18B20()
{
	if(1 == f)  TemF = '-';
	if(0 == f)  TemF = '+';	

	DisC(10,0,TemF);
	DisC(11,0,tempint/10+'0');
	DisC(12,0,tempint%10+'0');
	DisC(13,0,'.');

	tempdf = (tempdf * 5 / 8); // ���¶ȵ�С�����ֵ�0.00~0.15ת����0.0~0.9  
	DisC(14,0,tempdf+'0');
}

void DisplayTempTime(void)	// PageF == 0  ʱ��ʾʱ����¶�
{

//	Clear1602 
//	delay1ms();
  /************************�������Զ����ַ� ��������**************************************/
    v_LcdWriteSelfFont_f( );

		LcdSetXY( 2,0 );  // �Զ����ַ�Ҫ��1602��ʾ������������ǰ��
	v_LcdDisplaySelfFont_f( 0 );  	   //��
		LcdSetXY( 5,0 );
	v_LcdDisplaySelfFont_f( 1 );	  // ��
		LcdSetXY( 8,0 );
	v_LcdDisplaySelfFont_f( 2 );	  // ��
	
	if(TAOC)
		LcdSetXY( 8,1 );
	v_LcdDisplaySelfFont_f( 3 );	  //
		LcdSetXY( 15,0 );
	v_LcdDisplaySelfFont_f( 4 );	  //�¶ȱ�־��	

	 	read_DS1308();
	LCD1602_Display_1302();

	get_temperature();
	LCD1602_Display_18B20();

}


 void KeyEqu2( void )
 {
 	 if( KeyScanValue()==2)
  {
	  PageF++;	  
  }
  	 if(PageF == 3)
	 {
	 	PageF = 0;
	 }
 }


 void KeyEqu9( void )
 {
 	 if( KeyScanValue()==9)
  {
  	 SubSw++;
  }
  if(SubSw == 8)
	 {
	 	SubSw = 0;
	 }
	
 }

void KeyEqu1_Plus( uint8 SubSw )   //��Ӧ��ʱ���
 {
 	 if( KeyScanValue()==1)
  {
  	 (DS1302Ch[SubSw-1])++;
  }
//  if( DS1302Ch[i] == DS1302Limit[i])
//	 {
//	 	DS1302Ch[i] = 0;
//	 }
	
 }

 
void KeyEqu5_Sub( uint8 SubSw )	 //��Ӧ��ʱ���
 {
 	 if( KeyScanValue()==5)
  {
  	 (DS1302Ch[SubSw-1])--;
  }
 // if(YearCh == 0)
//	 {
//	 	YearCh = 0x20;
//	 }
	
 }

uint8 KeyData_Plus( uint8 Data )   //��Ӧ�ı�����
 {
 	 if( KeyScanValue()==1)
  {
  	 ++Data;
  }

	 return Data;
 }

 
uint8 KeyData_Sub( uint8 Data )	 //��Ӧ�ı�����
 {
 	 if( KeyScanValue()==5)
  {
  	 --Data;
  }
 	 return Data;
 }

 void DisplayTimeAlarm()
 {
 	
	  DisS(0,0,"1--TimeAlarm:");

//	  DisC(0,1,(char)(TAOC) +'0');
	  
	  if(TAOC)
	  {
	  	LcdSetXY(0,1);
	  	v_LcdDisplaySelfFont_f( 3 );
	  }
	  else
	  {
	   	//LcdSetXY(0,1);
		DisC(0,1,'C');
	  }

	  DisC(3,1,TiAH/10+'0');
	  DisC(4,1,TiAH%10+'0');

	  DisC(5,1,':');

	  DisC(6,1,TiAM/10+'0');
	  DisC(7,1,TiAM%10+'0');

 }

 
 void DisplayTempAlarm()
 {
 	
	  DisS(0,0,"2--TempLimit:");

	  DisC(0,1,'L');
	  DisC(1,1,':');
	  DisC(3,1, ((char)(TeALF)==0)?'-':'+');

	  DisC(4,1,TeAL/10+'0');
	  DisC(5,1,TeAL%10+'0');
//	  DisC(4,1,'.');
//	  DisC(5,1,TeALFloat);
//	  DisC(6,1,'C');
	  	LcdSetXY( 6,1 );
	v_LcdDisplaySelfFont_f( 4 );	  //�¶ȱ�־��

	  

	  DisC(8,1,'H');
	  DisC(9,1,':');
	  DisC(11,1, ((char)(TeAHF)==0)?'-':'+');

	  DisC(12,1,TeAH/10+'0');
	  DisC(13,1,TeAH%10+'0');
//	  DisC(12,1,'.');
//	  DisC(13,1,TeAHFloat);
//	  DisC(14,1,'C');
	  	LcdSetXY( 14,1 );
	v_LcdDisplaySelfFont_f( 4 );	  //�¶ȱ�־��

 }


void SetDs1302Display(uint8 i)
{


	KeyEqu1_Plus( i );
	KeyEqu5_Sub(  i );
	

	if(1 == SubSw)
		LcdSetXY(1, 0);
	if( SubSw==3 || SubSw==2 )
	{
		LcdSetXY(SubSw+SubSw+SubSw-2,0);  // 3*i-2
	}

	if(4 == SubSw)
		LcdSetXY(1, 1);
	if( SubSw==5 || SubSw==6 )
	{
		LcdSetXY( SubSw+SubSw+SubSw-11,1);  // 3*i-2
	}
	if(7==SubSw)
	{
		LcdSetXY(15,1);
	}
	 WriteC( FlashCur1602 , 1 );
}

void delay1ms()
{
	unsigned char i,j;
	for(i=0; i<100; i++)
		for(j=0; j<100; j++);
}

/*
void Bell(void)	 
{
	BELL = 0;
	delay1ms();
	BELL = 1;
}
*/


void TimeAlarmBell()
{
 	while( TAOC )
	 {
	 	if(min == TiAM &&  hr== TiAH)	 // min,hr
		{
//			BELL = 0;
			break;
		}
		else 
		{
			BELL  = 1;
			break;
		}
	 }
}



void TempAlarmBellLed() // �¶ȱ�������
{
 
	  //������ĸ��ж� Ϊ���¶Գ����� ��һ���������¶��ж�(��־λ==1)  �ڶ����Ǹ��¶ȵ��ж�(��־λ==0)//  
	 	if(      (( (~f == TeAHF) && (TeAHF == 1) ) && (tempint >= TeAH) ) 
		    || (( ( ~f == TeALF) && (TeALF == 1) ) && (tempint <= TeAL) ) 
	    	||	(( ( ~f ==TeAHF ) && (TeAHF == 0) ) && (tempint <= TeAH) )  
	    	|| (  ( ~f == TeALF && ( TeALF == 0) ) && (tempint >= TeAL) ) )
		{
		//	BELL = 0;
			LED  = 0;
	
		}
		else 
		{
		//	BELL  = 1;
			LED   = 1;
		}
	 
}

void JudgeMonthDays()  // �����·ݵ��������
{
	if(3 == i)   //�����Day  ��ô��������30 ����31 ����Ƕ�����ô��Ӧ���ж���ƽ��(28 days)��������(29 days)
							     //if((year%4==0&&year%100!=0)||(year%400==0)) {��ô������}	  
					{
						if(2 == ReadMn()) // ����Ƕ���
					 	{
							if( !(ReadYs()%4) )	  //��ȡ��� ���������  ����û�ж�%100!=0 ������20XX����������͹���
								DS1302Limit[2] = 29;  
							else 
							   	DS1302Limit[2] = 28;  
						}
						else // ������Ƕ���
							{
								if(4 == ReadMn() || 6 == ReadMn() || 9 == ReadMn() || 11 == ReadMn())  // ����·��� 4 6 9 11 ��ô��30days
									DS1302Limit[2] = 30;
								else 
									DS1302Limit[2] = 31;	//ʣ�µ��·���31days
							}
					 }
}

// ����2000��2099����һ�����ڼ�
// year    : 00-99
// month: 01-12
// day     : 01-31
uint8 GetWeekFromDay(unsigned char year,unsigned char month,unsigned char day)
{
    if( month == 1 || month == 2 )  
    {
        month += 12;
        if( year> 0 )
            year--;
        else
            year = 4;
    }

    // �������ڼ�(����һ��1��ʾ������������7��ʾ)
    return 1+(( day + 2*month + 3*(month+1)/5 + year + year/4 ) %7);
}  