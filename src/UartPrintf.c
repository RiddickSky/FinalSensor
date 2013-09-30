#include <reg52.h>
#include "UartPrintf.h"

/****************************************************************
��ʼ�����ڣ�11.0592MHz����ʱ��֧�ֲ����ʣ�1200��2400,4800 9600
****************************************************************/
void Uart_Init(unsigned int Rate)
{
	SCON= 0x40;                //���ڷ�ʽ1
	REN=1;                     //�������
	TMOD= 0x20;                //��ʱ��1��ʱ��ʽ2
	TR1 = 1;
	switch(Rate)
	{
		case 1200:
		PCON &=0x7f;                     //SMOD=0	
		TH1= 0xe8;                //11.0592MHz 1200������
		TL1= 0xe8; 
		break;
	
		case 2400:
		PCON |= 0x80;                     //SMOD=1
		TH1= 0xe8;                //11.0592MHz 2400������
		TL1= 0xe8;
		break;

		case 4800:
		PCON |= 0x80;                     //SMOD=1
		TH1= 0xf4;                //11.0592MHz 4800������
		TL1= 0xf4;
		break;
		
		case 9600:
		PCON |= 0x80;                     //SMOD=1
		TH1= 0xfa;                //11.0592MHz 4800������
		TL1= 0xfa;
		break;

		default:
		PCON |= 0x80;             
		TH1= 0xf4;                //Ĭ��4800������
		TL1= 0xf4;
		break;
	}
	EA = 1;
//	ES = 1;
	ET1= 0;	
	
}


/********************************************************************************
 �����������������ǰҪ�ȵ���Uart_Init(unsigned int Rate)���÷�������printfһ����
 �������"I Love You"��Uart_Printf("I Love You");
 
 %c  ����ַ�
 %d  ���ʮ����
 %b  ���8λ������
 %n  ����(����\n)
********************************************************************************/ 
void Uart_Printf(unsigned char *s,unsigned char n,...)
{
	unsigned char *ps=s;
	unsigned char *pn=&n;
	unsigned char t3,t2,t1;

//	TR1= 1;  // ��Ϊ��TR1  ��Ϊ�����ʷ�����
	TR1 = 1;
	EA = 0;
	ES = 0;//	 
	while(*ps!=0) 	
	{
		if(*ps=='%' && *(ps+1)=='c')  //����ַ�
		{
			SBUF=*pn;
			pn++;
			ps+=2;
			while(TI==0);
			TI=0;
		}

		else if(*ps=='%' && *(ps+1)=='d') //���ʮ����
		{
			t3=*pn/100;
			t2=(*pn%100)/10;
			t1=(*pn%100)%10;
		
			if(t3>0)
			{
				SBUF=t3+48;
				while(TI==0);
				TI=0;
			}
			if(t3>0 || t2>0)
			{
			   	SBUF=t2+48;
				while(TI==0);
				TI=0;
			} 	
	
			SBUF=t1+48;
			pn++;
			ps+=2;
			while(TI==0);
			TI=0;
			  
		}

		else if(*ps=='%' && *(ps+1)=='b')  //���������
		{
			for(t1=7;t1 >= 0;t1--)
			{
				SBUF=((*pn >> t1) & 0x01)+48; 
				while(TI==0);
				TI=0;
			}
			pn++;
			ps+=2;
		}

		else if(*ps=='%' && *(ps+1)=='n')   //������з�
		{
		 	SBUF=0x0d;
			while(TI==0);
			TI=0;

			SBUF=0x0a;
			while(TI==0);
			TI=0;

			ps+=2;
		}

	  	else
	  	{
			SBUF=*ps;
			ps++;
			while(TI==0);
			TI=0;
		}
		
	}
		 EA = 1;
		ES = 1;
//	TR1= 0;

}