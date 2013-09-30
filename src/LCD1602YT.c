#include<reg52.h>
#include<intrins.h>
#include"MacroAndConst.h"

#define DB P1

sbit LCDRS = P2^0;
sbit LCDRW = P2^1;
sbit LCDE = P2^2;
sbit DB7 = DB^7;

uint8 code SelfFont[ 40 ] =			  // ��ൽ40
{	
	0x08, 0x0f, 0x12, 0x0f, 0x0a, 0x1f, 0x02, 0x00,			//��ģ"��"	0	   
	0x0f, 0x11, 0x1f, 0x11, 0x1f, 0x11, 0x13, 0x00,			//��ģ"��"	1
	0x1f, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x1f, 0x00,			//��ģ"��"	2
	0x05, 0x0e, 0x14, 0x17, 0x14, 0x0e, 0x05, 0x00,			//���ӱ�־	3
   // 0x10, 0x18, 0x1C, 0x1E, 0x1C, 0x18, 0x10, 0x00,		//���ӱ�־ռ����(��������)	4 ����ǲ��ŵļ�ͷ����

	0x19, 0x1b, 0x04, 0x08, 0x08, 0x04, 0x03, 0x01			//����ַ�	   
};

//==================================================================//
//							��ʱ����								//
//==================================================================//
//�ղ���
void lnop()
{
	;
}

// ����ʱ5MS
void Delay5(void)
{
	word i=5552;
	while(i--);
}

//����ʱ400MS
void Delay400(void)
{
	byte i=5;
	word j;
	while(i--)
	{
		j=7269;
		while(j--);
	};
}

/***************************************
���æ�źŷ���:
***************************************/
//æ�źŵȴ�
void Busy()
{
 do
 {
	DB = 0xff;
	LCDRS = 0;
	LCDRW = 1;
	LCDE = 1;
	lnop();
 }while( DB7 );
}

/******************************************************
				��д����
[ע:	RS	�Ĵ���ѡ��	1-���ݼĴ���	0-ָ��Ĵ���
		RW	��/дѡ��	1-��			0-д
		E	ʹ���ź�	1-��ȡ			�½���-д��
*******************************************************/
//д��ָ��	��RS=0	RW=0	E�½��أ�
void WriteC( byte dd , byte jc )
{
	if(jc)	Busy();
	LCDRS = 0;
	LCDRW = 0;
	DB = dd;
	LCDE = 1;
	lnop();
	LCDE = 0;
}

//д������	��RS=1	RW=0	E�½��أ�
void WriteD( byte dd)
{
	Busy();
	LCDRS = 1;
	LCDRW = 0;
	DB = dd;
	LCDE = 1;
	lnop();
	LCDE = 0;
}

void LcdSetXY( uint8 X, uint8 Y )		 //������ʾ����λ��
{
	uint8 Address ;
	if( Y == 0 )
	{
		Address = 0x80 + X ;                //��һ��
	}
	else
	{
		Address = 0xc0 + X ;                //�ڶ���
	}
	WriteC( Address,0 ) ;
}

//ָ��λ����ʾһ���ַ�
void DisC(byte x,byte y,byte dd)
{
	y &= 0x1;
	x &= 0xF; //����X���ܴ���15��Y���ܴ���1
	if (y)
	{
		x |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
	}
	x |= 0x80; // ���ָ����	=> 80H + ��ʵ��ַ��
	WriteC(x, 0); //���ﲻ���æ�źţ����͵�ַ��
	WriteD(dd);
}


//ָ��λ����ʾһ���ַ�

void DisS(byte x,byte y,byte code *dd)
{
	byte i,l=0,j;
	while (dd[l] >31){l++;};	//�ж��ַ�������
	for (i=0;i<l;i++) 
	{
		DisC(x++,y,dd[i]);
		if (x==16)
		{
			x=0;	//����ʼ
		 	y ^= 1;	//���л�
			if(y == 0)	//����ʾ��һ����ͣ4S������ʾ
			{
				for(j=0; j<10; j++)
					Delay400();
				WriteC( 0x01,1 );
			}
		}
	}
}


void v_LcdWriteSelfFont_f( void )          //���Զ����ַ�д��CGRAM(���8��)
{
	uint8 i ;
	for( i = 0 ; i < 40 ;i++ )
	{
		WriteC( 0x40 + i,0 ) ;   //CGRAM��ʼ��ַ��0x40��ʼ
		WriteD( SelfFont[ i ] ) ;
	}
}

void v_LcdDisplaySelfFont_f( uint8 Address )
{
	 	WriteD( Address ) ;         //Addressȡֵ��Χ0~7,�ֱ��Ӧ8���Զ����ַ�
}


/*****************************************************************
                   ��λ����
*****************************************************************/
void LCDRst()
{
	Delay5();	//��ʱ15ms
	Delay5();
	Delay5();
	WriteC( 0x38,0 );	//����ʾģʽ
	Delay5();			//��������
	WriteC( 0x38,0 );	//�����æ�ź�
	Delay5();
	WriteC( 0x38,0 );
//----------------------//�Ժ�ÿ�ζ�Ҫ���æ�ź�
	WriteC( 0x38,1 );	//дָ�ָ��ţ�1-���æ�ź�	0-����⣩
	WriteC( 0x08,1 );	//����ʾ
	WriteC( 0x01,1 );	//����ʾ��
	WriteC( 0x06,1 );	//��/дһ�ַ��󣬵�ַ/������һ
	WriteC( 0x0c,1 );	//����ʾ
}

