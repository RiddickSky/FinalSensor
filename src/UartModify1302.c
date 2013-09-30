#include <reg52.h>
#include "MacroAndConst.h"
#include "uart.h"
#include "DS1302DY.h"
#include "UartModify1302.h"

unsigned char code END_CODE[4] = "#end";
unsigned char  end_num = 0;
unsigned char  Data_num = 0;		// �������ݸ�����¼
unsigned char  commd[30];			// �������ݻ���
unsigned char  D_add;				// ���ݵ�ַ
bit		       flag_deal = 0;		// �����־
//---------------------------------------------------

//��������Uart�յ��ĸ���ֵ  ��7��   �� �� �� ʱ �� �� ���� Ҳ���Ǵ���ÿ��state��  ע����ΪҪ������λ����Ҫ��uint16 ������8
uint8 Uart_1302_Temp_Val[7];  

//**************************����ת��*************************************
unsigned char DEC_conver(void)
{
	unsigned char state,i;
	state = 0;  	//�����ַ����м����
	i = 2; 			// ��commd[2] ����������ʼ  ��Ϊ��һ����W����R �ڶ����ǿո� ���Դӵ�������ʼ
//-----------------------�����ַ-------------------------------
	while(1)
	{
		if( (commd[i]>47)&&(commd[i]<58) ) 			// �ǡ�0��-��9���� ��48��57��10�� �ֱ���'0'��'9'
			state = (state*10)+(commd[i]-48);		// �����Ӧ��ASCII��ֵ	 + state*10 ָ����

		else if( commd[i]==' ' )
			break;			// �ǿո�,����	 ʲôҲ������ ��������

		else 													// ��'0'-'9'��' '
		{
			UART_TXDSTRING("add��ʽ��������������",1);
			return ERROR;									// ���ش���
		}

		if( i>4 )	// i��ʾAddress��λ�� ����������Ƶ�ַ��λ�������Լ4����ô��ַ����3λ��,�ⳬ����AT24C02������ַ
		{
			UART_TXDSTRING("Address���ܳ�����λ������������",1);
			return ERROR;
		}

		i++;
	}
 Uart_1302_Temp_Val[0] = state;
//	commd[0] = state;										// ��ַ�����commd[0]��
//-------------------------�������ݸ���--------------------------------
	i++;
	state = 0;
	while(1)
	{
		if( (commd[i]>47)&&(commd[i]<58) )
			state = (state*10)+(commd[i]-48);
		else if( commd[i]==' ' )	break;
		else 
		{
			UART_TXDSTRING("number��ʽ��������������",1);
			return ERROR;
		}
		if( i>8 )
		{
			UART_TXDSTRING("number̫������������",1);
			return ERROR;
		}
		i++;
	}
//	commd[1] = state;					// �������������commd[1]��
 Uart_1302_Temp_Val[1] = state;

 //###############����������#####################//
		i++;
	state = 0;
	while(1)
	{
		if( (commd[i]>47)&&(commd[i]<58) )
			state = (state*10)+(commd[i]-48);
		else if( commd[i]==' ' )	break;
		else 
		{
			UART_TXDSTRING("number��ʽ��������������",1);
			return ERROR;
		}
		if( i>8 )
		{
			UART_TXDSTRING("number̫������������",1);
			return ERROR;
		}
		i++;
	}
//	commd[2] = state;					// �������������commd[1]��
 Uart_1302_Temp_Val[2] = state;

//---------------------------------------------------------------------
	
	
 //###############���ĸ�����#####################//
		i++;
	state = 0;
	while(1)
	{
		if( (commd[i]>47)&&(commd[i]<58) )
			state = (state*10)+(commd[i]-48);
		else if( commd[i]==' ' )	break;
		else 
		{
			UART_TXDSTRING("number��ʽ��������������",1);
			return ERROR;
		}
		if( i>8 )
		{
			UART_TXDSTRING("number̫������������",1);
			return ERROR;
		}
		i++;
	}
	commd[3] = state;					// �������������commd[1]��
 Uart_1302_Temp_Val[3] = state;
//---------------------------------------------------------------------	


 //###############���������#####################//
		i++;
	state = 0;
	while(1)
	{
		if( (commd[i]>47)&&(commd[i]<58) )
			state = (state*10)+(commd[i]-48);
		else if( commd[i]==' ' )	break;
		else 
		{
			UART_TXDSTRING("number��ʽ��������������",1);
			return ERROR;
		}
		if( i>8 )
		{
			UART_TXDSTRING("number̫������������",1);
			return ERROR;
		}
		i++;
	}
//	commd[4] = state;					// �������������commd[1]��
 Uart_1302_Temp_Val[4] = state;
//---------------------------------------------------------------------



 //###############����������#####################//
		i++;
	state = 0;
	while(1)
	{
		if( (commd[i]>47)&&(commd[i]<58) )
			state = (state*10)+(commd[i]-48);
		else if( commd[i]==' ' )	break;
		else 
		{
			UART_TXDSTRING("number��ʽ��������������",1);
			return ERROR;
		}
		if( i>8 )
		{
			UART_TXDSTRING("number̫������������",1);
			return ERROR;
		}
		i++;
	}
	//commd[5] = state;					// �������������commd[1]��
	 Uart_1302_Temp_Val[5] = state;
//---------------------------------------------------------------------


 //###############���߸�����#####################//
		i++;
	state = 0;
	while(1)
	{
		if( (commd[i]>47)&&(commd[i]<58) )
			state = (state*10)+(commd[i]-48);  
			// ��һ�ν����state=0 �õ�����ʮλ����ֵ  (�������λ��,eg:12)Ȼ��ڶ�������ʱstate=1 �����͵õ�����������λ��
		else if( commd[i]==' ' )	break;	 // ֻ�������������while  
		else 
		{
			UART_TXDSTRING("number��ʽ��������������",1);
			return ERROR;
		}
		if( i>8 )
		{
			UART_TXDSTRING("number̫������������",1);
			return ERROR;
		}
		i++;
	}
//	commd[6] = state;					// �������������commd[1]��
 Uart_1302_Temp_Val[6] = state;
//---------------------------------------------------------------------

	
	D_add = ++i;					 	// ��¼���ݵ�ַ
	return SUCC;						// ��ȷ	
}


void UartChangeDS1302()
{
	
	if( DEC_conver()==ERROR )	return;			// ���󣬷���

	else
	{	
		ys  = Uart_1302_Temp_Val[0]	;
		mn  = Uart_1302_Temp_Val[1]	;
		day = Uart_1302_Temp_Val[2]	;
		hr  = Uart_1302_Temp_Val[3]	;
		min = Uart_1302_Temp_Val[4]	;
		sec = Uart_1302_Temp_Val[5]	;
		we  = Uart_1302_Temp_Val[6]	;
    }
    DS1302_Write_Ini_Data(ys,mn,day,hr,min,sec,we);
}