#ifndef _UARTMODIFY1302_H_
#define _UARTMODIFY1302_H_

#define 	ERROR 0
#define		SUCC  1

//---------------------------------------------------
#define        END_NUM     4

extern unsigned char code END_CODE[4] ;	 
extern unsigned char  end_num ;
extern unsigned char  Data_num;		// �������ݸ�����¼
extern unsigned char  commd[30];			// �������ݻ���
extern unsigned char  D_add;				// ���ݵ�ַ
extern bit	 flag_deal ;		// �����־


extern void UartChangeDS1302(void);

#endif