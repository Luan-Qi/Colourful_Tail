#include "BMI160.h"
#include "myiic.h"


uint8_t BMI160_init(void)
{
	uint8_t res;
	IIC_Init();
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	printf("res=%#x\r\n",res);
	if(res!=0xD1)//����ID��ȷ
	{
		while(1){}
 	}
	MPU_Write_Byte(0x7e,0x11);//������Ĵ���д��0x11,ʹ���ٶȴ�����������ģʽ
	MPU_Write_Byte(0x7e,0x15);//������Ĵ���д��0x15,ʹ�����Ǵ�����������ģʽ 
	return 1;
}

//�õ�������ֵ(ԭʼֵ+ת��ֵ)
//gyr_x,gyr_y,gyr_z:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//       ����,�������
uint8_t MPU_Get_Gyroscope(short *gyr_x,short *gyr_y,short *gyr_z)
{
	unsigned short x,y,z;
	
	x =(MPU_Read_Byte(0x0c)&0xff);//���ٶ� X��
	x = x|((MPU_Read_Byte(0x0d)&0xff)<<8);
	
	y =( MPU_Read_Byte(0x0e)&0xff);//���ٶ� Y��
	y = y|((MPU_Read_Byte(0x0f)&0xff)<<8);
	
	z =( MPU_Read_Byte(0x10)&0xff);//���ٶ� Z��
	z = z|((MPU_Read_Byte(0x11)&0xff)<<8);
	
	*gyr_x = (signed short)x;
	*gyr_y = (signed short)y;
	*gyr_z = (signed short)z;
	
	// rangeΪ��2000��/sʱ��ת��Ϊ���ٶȡ�/s�Ĺ�ʽ
	*gyr_x = (*gyr_x*2000)/0x8000;
	// rangeΪ��2000��/sʱ��ת��Ϊ���ٶȡ�/s�Ĺ�ʽ
	*gyr_y = (*gyr_y*2000)/0x8000;
	// rangeΪ��2000��/sʱ��ת��Ϊ���ٶȡ�/s�Ĺ�ʽ  
	*gyr_z = (*gyr_z*2000)/0x8000;
	
	return 0;
}

//�õ����ٶ�ֵ(ԭʼֵ+ת��ֵ)
//acc_x,acc_y,acc_z:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//       ����,�������
uint8_t MPU_Get_Accelerometer(short *acc_x,short *acc_y,short *acc_z)
{
	unsigned short x,y,z;
	
	x =(MPU_Read_Byte(0x12)&0xff);//���ٶ� X�� 
	x = x|((MPU_Read_Byte(0x13)&0xff)<<8);
	
	y =(MPU_Read_Byte(0x14)&0xff);//���ٶ� Y��
	y = y|((MPU_Read_Byte(0x15)&0xff)<<8);
	
	z =(MPU_Read_Byte(0x16)&0xff);//���ٶ� Z��
	z = z|((MPU_Read_Byte(0x17)&0xff)<<8);
	
	*acc_x = (signed short)x;
	*acc_y = (signed short)y;
	*acc_z = (signed short)z;
	
	//������Ϊ��2gʱ��ת��Ϊg/s�ļ��ٶȻ��㹫ʽ
	*acc_x = (signed short)(((double)*acc_x*1000)/(0x8000/2));
	//������Ϊ��2gʱ��ת��Ϊg/s�ļ��ٶȻ��㹫ʽ
	*acc_y = (signed short)(((double)*acc_y*1000)/(0x8000/2));
	//������Ϊ��2gʱ��ת��Ϊg/s�ļ��ٶȻ��㹫ʽ  
	*acc_z = (signed short)(((double)*acc_z*1000)/(0x8000/2));
	
	return 0;
}

//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//       ����,�������
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
  IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//����������ַ+д����	
	if(IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		IIC_Stop();		 
		return 1;		
	}
  IIC_Send_Byte(reg);	//д�Ĵ�����ַ
  IIC_Wait_Ack();		//�ȴ�Ӧ��
	for(i=0;i<len;i++)
	{
		IIC_Send_Byte(buf[i]);	//��������
		if(IIC_Wait_Ack())		//�ȴ�ACK
		{
			IIC_Stop();	 
			return 1;		 
		}		
	}    
  IIC_Stop();	 
	return 0;	
} 
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//       ����,�������
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//����������ַ+д����	
	if(IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		IIC_Stop();		 
		return 1;		
	}
  IIC_Send_Byte(reg);	//д�Ĵ�����ַ
  IIC_Wait_Ack();		//�ȴ�Ӧ��
  IIC_Start();
	IIC_Send_Byte((addr<<1)|1);//����������ַ+������	
  IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	while(len)
	{
		if(len==1)*buf=IIC_Read_Byte(0);//������,����nACK 
		else *buf=IIC_Read_Byte(1);		//������,����ACK  
		len--;
		buf++; 
	}    
  IIC_Stop();	//����һ��ֹͣ���� 
	return 0;	
}
//IICдһ���ֽ� 
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//       ����,�������
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) 				 
{ 
  IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//����������ַ+д����	
	if(IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		IIC_Stop();		 
		return 1;		
	}
  IIC_Send_Byte(reg);	//д�Ĵ�����ַ
  IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	IIC_Send_Byte(data);//��������
	if(IIC_Wait_Ack())	//�ȴ�ACK
	{
		IIC_Stop();	 
		return 1;		 
	}		 
    IIC_Stop();	 
	return 0;
}
//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
  IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//����������ַ+д����	
	IIC_Wait_Ack();		//�ȴ�Ӧ�� 
  IIC_Send_Byte(reg);	//д�Ĵ�����ַ
  IIC_Wait_Ack();		//�ȴ�Ӧ��
  IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|1);//����������ַ+������	
  IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	res=IIC_Read_Byte(0);//��ȡ����,����nACK 
  IIC_Stop();			//����һ��ֹͣ���� 
	return res;		
}

