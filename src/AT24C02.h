#include "IIc.h"

#define AT24C02_DeviceAdd_W 0xa0	//������ַ(д)
#define AT24C02_DeviceAdd_R 0xa1	//������ַ(��)

#define I2C_R_BIT_SUCCESS     0x01
#define I2C_R_BIT_ERR         0x02
#define I2C_W_BIT_SUCCESS     0x04
#define I2C_W_BIT_ERR         0x08


unsigned char AT24C02_flag;

//**************************************************************************
// �������ƣ�AT24C02_WriteByte()
// ��    �ܣ���AT24C02ָ����ַ��д��ָ�����ֽ�����
// ��ڲ�����add   AT24C02�ڵĵ�ַ
//           Num   д������ݸ�����С��16��
//           Byte  ��Ҫд��AT24C02�Ľ����ݵ�ַ
// ���ڲ�������
// ע    �⣺add��Num��������: add>=16*n��(add+Num)<16*(n+1))
//           ��д������ݶ����뱣֤��ͬһҳ��(һҳ16�ֽ�)��������
//				 û�з�ҳ���ܡ�	
//**************************************************************************
void AT24C02_WriteByte( unsigned char add,
			 					unsigned char Num,
                        unsigned char *Byte)
{
	unsigned char i;

	I2C_Start();										// ����I2C����
	I2C_Write8Bit(AT24C02_DeviceAdd_W);			// ����������ַ(д)
	AT24C02_flag = I2C_W_BIT_SUCCESS;			// ��־λ
	if( I2C_Check_Ack() == 0 )						// ���ӻ�Ӧ��
		AT24C02_flag = I2C_W_BIT_ERR;				// ��Ӧ��д����
	I2C_Write8Bit(add);   							// ���ͽ�Ҫ��������ݵĵ�ַ
	if( I2C_Check_Ack() == 0 )                // ���ӻ�Ӧ��
		AT24C02_flag = I2C_W_BIT_ERR;				// ��Ӧ��д����
	
	for( i=0; i<Num; i++ )
	{
		I2C_Write8Bit(Byte[i]);                // д������ 
		if( I2C_Check_Ack() == 0 )             // ���ӻ�Ӧ��
			AT24C02_flag = I2C_W_BIT_ERR;	
	}
	
	I2C_Stop();				        					// ֹͣI2C����
}

//**************************************************************************
// �������ƣ�AT24C02_ReadnByte()
// ��    �ܣ���AT24C02ָ����ַ�϶���ָ���ֽ�����
// ��ڲ�����add   AT24C02�ڵĵ�ַ
//           Num   ��Ҫ��ȡ�����ݸ���
//           *stor ��ȡ���ݺ�Ĵ洢��ַ
// ���ڲ�������
//**************************************************************************
/*void AT24C02_ReadnByte(unsigned int add,
                       unsigned int Num,
                       unsigned char *stor)
{    
	unsigned int  i;
	
	I2C_Start();										// ����I2C����
	
	I2C_Write8Bit(AT24C02_DeviceAdd_W);			// ����������ַ(д)
	AT24C02_flag = I2C_R_BIT_SUCCESS;
	if( I2C_Check_Ack() == 0 )                // ���ӻ�Ӧ��
		AT24C02_flag = I2C_R_BIT_ERR;	
	I2C_Write8Bit(add);   							// ���ͽ�Ҫ��������ݵĵ�ַ(MSB)
	if( I2C_Check_Ack() == 0 )                // ���ӻ�Ӧ��
		AT24C02_flag = I2C_R_BIT_ERR;	
	
	I2C_Start();				         			// �ٴ�����IIC����
	I2C_Write8Bit(AT24C02_DeviceAdd_R);	   	// ����������ַ(��)
	if( I2C_Check_Ack() == 0 )                // ���ӻ�Ӧ��
		AT24C02_flag = I2C_R_BIT_ERR;	  
	stor[0] = I2C_Read8Bit();		        		// ��ȡ��һ������  
	for( i=1; i<Num; i++ )
	{
		I2C_Send_Bit_0();                      // ����Ӧ��(ACK)��������������
		stor[i] = I2C_Read8Bit();		         // ��ȡ����
	}
	I2C_Send_Bit_1();				 					// ����NO ACK�źŽ�����ȡ����			
	I2C_Stop();				        					// ֹͣI2C����
}		*/