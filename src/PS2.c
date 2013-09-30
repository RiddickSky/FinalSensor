#include<reg52.h>
#include "MacroAndConst.h"
#include "PS2.h"

#define SHIFT 0x12

/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U                                                                        �U
�U                      Ӳ  ��  ��  ��  ��  ��                            �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/

sbit io_dat_ps2key = P3^5;                  //PS2���̵����ݶ˿�
sbit io_clk_ps2key = P3^4;                  //PS2���̵�ʱ�Ӷ˿�

/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U                                                                        �U
�U                      ��  ��  ת  ��  ��  ��                            �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/
//û�а���Shift��ʱ�ļ���ת��
static unsigned char code UnShifted[59][2] = {
									          0x1C, 'a',0x32, 'b',0x21, 'c',0x23, 'd',0x24, 'e',
                                              0x2B, 'f',0x34, 'g',0x33, 'h',0x43, 'i',0x3B, 'j',
                                              0x42, 'k',0x4B, 'l',0x3A, 'm',0x31, 'n',0x44, 'o',
                                              0x4D, 'p',0x15, 'q',0x2D, 'r',0x1B, 's',0x2C, 't',
                                              0x3C, 'u',0x2A, 'v',0x1D, 'w',0x22, 'x',0x35, 'y',
                                              0x1A, 'z',0x45, '0',0x16, '1',0x1E, '2',0x26, '3',
                                              0x25, '4',0x2E, '5',0x36, '6',0x3D, '7',0x3E, '8',
											  0x46, '9',0x0E, '`',0x4E, '-',0x55, '=',0x5D, '\\',
                                              0x29, ' ',0x54, '[',0x5B, ']',0x4C, ';',0x52, '\'',
                                              0x41, ',',0x49, '.',0x4A, '/',0x71, '.',0x70, '0',
                                              0x69, '1',0x72, '2',0x7A, '3',0x6B, '4',0x73, '5',
											  0x74, '6',0x6C, '7',0x75, '8',0x7D, '9',
											  };
//����Shift��ʱ�ļ���ת��
/*static unsigned char code Shifted[59][2] =  {												
											  0x1C, 'A',0x32, 'B',0x21, 'C',0x23, 'D',0x24, 'E',
                                              0x2B, 'F',0x34, 'G',0x33, 'H',0x43, 'I',0x3B, 'J',
                                              0x42, 'K',0x4B, 'L',0x3A, 'M',0x31, 'N',0x44, 'O',
                                              0x4D, 'P',0x15, 'Q',0x2D, 'R',0x1B, 'S',0x2C, 'T',
                                              0x3C, 'U',0x2A, 'V',0x1D, 'W',0x22, 'X',0x35, 'Y',
                                              0x1A, 'Z',0x45, '0',0x16, '1',0x1E, '2',0x26, '3',
                                              0x25, '4',0x2E, '5',0x36, '6',0x3D, '7',0x3E, '8',
                                              0x46, '9',0x0E, '~',0x4E, '_',0x55, '+',0x5D, '|',
                                              0x29, ' ',0x54, '{',0x5B, '}',0x4C, ':',0x52, '"',
                                              0x41, '<',0x49, '>',0x4A, '?',0x71, '.',0x70, '0',
                                              0x69, '1',0x72, '2',0x7A, '3',0x6B, '4',0x73, '5',
											  0x74, '6',0x6C, '7',0x75, '8',0x7D, '9',
											  };
/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U                                                                        �U
�U                      ��  ��  ԭ  ��  ��  ��                            �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/
static uchar ReceiveByte(void);
static void SkipCode(void);
static uchar ReceiveDat(void);              //����һ��������
static uchar ChangeASCII(uchar dat);        //����ͨ��תΪ�����ASCII��
uchar ReceiveDatFromPS2KeyBoard(void);      //������Ӧ������ASCII��
/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U  �� �� ���� static uchar ReceiveByte(void)                             �U
�U  ����ģʽ�� �ڲ�                                                       �U
�U  ����˵���� ���ؼ��̴��͵�һ���ֽ�.                                    �U
�U  ���ú����� ��                                                         �U
�U  ȫ�ֱ����� ��                                                         �U
�U  �˿��ź�:  io_clk_ps2key��io_dat_ps2key                               �U
�U  ��    �룺 ��                                                         �U
�U  ��    �أ� 11λ�����е�8λ����.                                       �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-09                 �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-09                 �U
�U  E-mail  �� hanlinxp@163.com      QQ�� 33737559                        �U
�U  ��    ���� v1.0                                                       �U
�U                                                                        �U
�U                      Copyright@2006 - 2007                             �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/
static uchar ReceiveByte(void)
{
    uchar i,dat = 0;
    while(!io_clk_ps2key);          //��ʼλ0
    while(io_clk_ps2key);
    for(i=0;i<8;i++)                //����8λ����
    {
        dat >>= 1;
        if(io_dat_ps2key)
            dat |= 0x80;
        while(!io_clk_ps2key);
        while(io_clk_ps2key);
    }
                                    //�˴�ȡ����У��λ
    while(!io_clk_ps2key);
    while(io_clk_ps2key);
    while(!io_clk_ps2key);
    return dat;
}
/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U  �� �� ���� static void SkipCode(void)                                 �U
�U  ����ģʽ�� �ڲ�                                                       �U
�U  ����˵���� �������������.                                            �U
�U  ���ú����� ��                                                         �U
�U  ȫ�ֱ����� ��                                                         �U
�U  �˿��ź�:  io_clk_ps2key��io_dat_ps2key                               �U
�U  ��    �룺 ��                                                         �U
�U  ��    �أ� ��                                                         �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-09                 �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-09                 �U
�U  E-mail  �� hanlinxp@163.com      QQ�� 33737559                        �U
�U  ��    ���� v1.0                                                       �U
�U                                                                        �U
�U                      Copyright@2006 - 2007                             �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/
static void SkipCode(void)
{
    (void)ReceiveByte();
    (void)ReceiveByte();
}
/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U  �� �� ���� static uchar ReceiveDat(void)                              �U
�U  ����ģʽ�� �ڲ�                                                       �U
�U  ����˵���� ���ؼ��̰�����Ӧ��һ����ͨ��.                              �U
�U  ���ú����� ReceiveByte()��SkipCode()                                  �U
�U  ȫ�ֱ����� ��                                                         �U
�U  �˿��ź�:  ��                                                         �U
�U  ��    �룺 ��                                                         �U
�U  ��    �أ� ���̵Ľ�ͨ��.                                              �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-08                 �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-09                 �U
�U  E-mail  �� hanlinxp@163.com      QQ�� 33737559                        �U
�U  ��    ���� v1.1                                                       �U
�U                                                                        �U
�U                      Copyright@2006 - 2007                             �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/
static uchar ReceiveDat(void)
{
    uchar dat;
    if(io_clk_ps2key)                       //����޼�����,����0
        return 0;
    dat = ReceiveByte();
    SkipCode();
    return dat;                             //���ؽ�ͨ��
}
/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U  �� �� ���� static uchar ChangeASCII(uchar dat)                        �U
�U  ����ģʽ�� �ڲ�                                                       �U
�U  ����˵���� ����ͨ��תΪ����̰�����Ӧ��ASCII��.                       �U
�U  ���ú����� ��                                                         �U
�U  ȫ�ֱ����� ��                                                         �U
�U  �˿��ź�:  ��                                                         �U
�U  ��    �룺 ��                                                         �U
�U  ��    �أ� ���̰�����ASCII��.                                         �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-08                 �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-08                 �U
�U  E-mail  �� hanlinxp@163.com      QQ�� 33737559                        �U
�U  ��    ���� v1.0                                                       �U
�U                                                                        �U
�U                      Copyright@2006 - 2007                             �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/
static uchar ChangeASCII(uchar dat)
{
    uchar i;
    if(dat == 0)
        return 0;
    for(i=0;i<59;i++)
    {
        if(UnShifted[i][0] == dat)          //û��Shift��
            return UnShifted[i][1];
    }
    return 0;
}
/*
�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r
�U  �� �� ���� uchar ReceiveDatFromPS2KeyBoard(void)                      �U
�U  ����ģʽ�� ȫ��                                                       �U
�U  ����˵���� ���ؼ��̰�����ASCII��,û�а�����Ϊ0.                       �U
�U  ���ú����� ChangeASCII()��ReceiveDat()                                �U
�U  ȫ�ֱ����� ��                                                         �U
�U  �˿��ź�:  ��                                                         �U
�U  ��    �룺 ��                                                         �U
�U  ��    �أ� ���̰�����ASCII��.                                         �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-08                 �U
�U  �� �� �ߣ� ����                  ��    �ڣ�2007-02-08                 �U
�U  E-mail  �� hanlinxp@163.com      QQ�� 33737559                        �U
�U  ��    ���� v1.0                                                       �U
�U                                                                        �U
�U                      Copyright@2006 - 2007                             �U
�U                                                                        �U
�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s
*/
uchar ReceiveDatFromPS2KeyBoard(void)
{
    return ChangeASCII(ReceiveDat());
}
