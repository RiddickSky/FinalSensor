#include<reg52.h>
#include "MacroAndConst.h"
#include "18B20.h"

sbit TMDAT=P3^6;    //DS1820�¶Ƚӿ�
bit f;             //���¶ȱ�־
bit flag;          //DS1820��ʼ���ɹ���־
unsigned char tempint,tempdf;	          //�¶��������ֺ�С������

void set_ds1820()              //��ʼ��DS1820
{
	while(1)
	{
		uchar delay;
		flag=0;
		TMDAT=1;                  //���߸ߵ�ƽ
		delay=1;
		while(--delay);           //��΢��ʱ
		TMDAT=0;                  //��������
		delay=250;
		while(--delay);           //��ʱ500us
		TMDAT=1;                  //��������
		delay=30;
		while(--delay);           //��ʱ60us
		while(!TMDAT)             //��DS1820��������
		{
			delay=120;  
			while(--delay);        //��ʱ240us
			if(TMDAT)
			{
				flag=1;             //DS1820��ʼ���ɹ���־
				break;
			}
		}
		if(flag)                  //��ʼ���ɹ�������ʱ480us,ʱ��Ҫ��
		{
			delay=240;
			while(--delay);
			break;
		}
	}
}

void write_ds1820(uchar command)  //д8λ
{
	uchar delay,i;
	for(i=8;i>0;i--)               //ѭ��8��д��
	{
		TMDAT=0;                    //��������
		delay=6;
		while(--delay);             //��ʱ12us
		TMDAT=command&0x01;         //�����ݷ��������ϣ����в���
		delay=25;
		while(--delay);             //��ʱ50us���������
		command=command>>1;         //��������һλ��׼���´�����
		TMDAT=1;                    //�ͷ�����
	}
}

void read_ds1820()                //��
{
	uchar delay,i,j,k,temp,temph,templ;
	j=2;                           //��2λ�ֽ�����
	do
	{
		for(i=8;i>0;i--)            //һ���ֽڷ�8λ��ȡ
		{
			temp>>=1;                //��ȡ1λ����1λ
			TMDAT=0;                 //�������õ͵�ƽ
			delay=1;
			while(--delay);          //��ʱ2us
			TMDAT=1;                 //��������
			delay=4;
			while(--delay);          //��ʱ8us
			if(TMDAT)temp|=0x80;     //��ȡ1λ����
			delay=25;
			while(--delay);          //��ȡ1λ���ݺ���ʱ50us
		}
		if(j==2)templ=temp;         //��ȡ�ĵ�һ�ֽڴ�templ
		else temph=temp;            //��ȡ�ĵڶ��ֽڴ�temph
	}while(--j);
	f=0;                           //��ʼ�¶ȱ�־Ϊ��
	if((temph&0xf8)!=0x00)         //���¶�Ϊ���Ĵ����Զ����Ʋ���Ĵ���
	{
		f=1;                        //Ϊ���¶�f��1
		temph=~temph;
		templ=~templ;
		k=templ+1;
		templ=k;
		if(k>255)
		{
			temph++;
		}
	}
	tempdf=templ&0x0f;            //����ȡ������ת�����¶�ֵ���������ִ�tempint,С�����ִ�tempdf
	templ>>=4;
	temph<<=4;
	tempint=temph|templ;
}

void get_temperature()           //�¶�ת��������¶��ӳ���
{
	set_ds1820();                 //��ʼ��DS18B20
	write_ds1820(0xcc);           //������ROMƥ������
	write_ds1820(0x44);           //���¶�ת������
	set_ds1820();                 //��ʼ��DS18B20
	write_ds1820(0xcc);           //������ROMƥ������
	write_ds1820(0xbe);           //�������¶�����
	read_ds1820();                //���������¶����ݱ��浽tempint��tempdf��
}