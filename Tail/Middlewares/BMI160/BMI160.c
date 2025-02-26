#include "BMI160.h"
#include "myiic.h"


uint8_t BMI160_init(void)
{
	uint8_t res;
	IIC_Init();
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	printf("res=%#x\r\n",res);
	if(res!=0xD1)//器件ID正确
	{
		while(1){}
 	}
	MPU_Write_Byte(0x7e,0x11);//向命令寄存器写入0x11,使加速度处于正常工作模式
	MPU_Write_Byte(0x7e,0x15);//向命令寄存器写入0x15,使陀螺仪处于正常工作模式 
	return 1;
}

//得到陀螺仪值(原始值+转换值)
//gyr_x,gyr_y,gyr_z:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//       其他,错误代码
uint8_t MPU_Get_Gyroscope(short *gyr_x,short *gyr_y,short *gyr_z)
{
	unsigned short x,y,z;
	
	x =(MPU_Read_Byte(0x0c)&0xff);//角速度 X轴
	x = x|((MPU_Read_Byte(0x0d)&0xff)<<8);
	
	y =( MPU_Read_Byte(0x0e)&0xff);//角速度 Y轴
	y = y|((MPU_Read_Byte(0x0f)&0xff)<<8);
	
	z =( MPU_Read_Byte(0x10)&0xff);//角速度 Z轴
	z = z|((MPU_Read_Byte(0x11)&0xff)<<8);
	
	*gyr_x = (signed short)x;
	*gyr_y = (signed short)y;
	*gyr_z = (signed short)z;
	
	// range为±2000°/s时，转换为角速度°/s的公式
	*gyr_x = (*gyr_x*2000)/0x8000;
	// range为±2000°/s时，转换为角速度°/s的公式
	*gyr_y = (*gyr_y*2000)/0x8000;
	// range为±2000°/s时，转换为角速度°/s的公式  
	*gyr_z = (*gyr_z*2000)/0x8000;
	
	return 0;
}

//得到加速度值(原始值+转换值)
//acc_x,acc_y,acc_z:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//       其他,错误代码
uint8_t MPU_Get_Accelerometer(short *acc_x,short *acc_y,short *acc_z)
{
	unsigned short x,y,z;
	
	x =(MPU_Read_Byte(0x12)&0xff);//加速度 X轴 
	x = x|((MPU_Read_Byte(0x13)&0xff)<<8);
	
	y =(MPU_Read_Byte(0x14)&0xff);//加速度 Y轴
	y = y|((MPU_Read_Byte(0x15)&0xff)<<8);
	
	z =(MPU_Read_Byte(0x16)&0xff);//加速度 Z轴
	z = z|((MPU_Read_Byte(0x17)&0xff)<<8);
	
	*acc_x = (signed short)x;
	*acc_y = (signed short)y;
	*acc_z = (signed short)z;
	
	//当量程为±2g时，转换为g/s的加速度换算公式
	*acc_x = (signed short)(((double)*acc_x*1000)/(0x8000/2));
	//当量程为±2g时，转换为g/s的加速度换算公式
	*acc_y = (signed short)(((double)*acc_y*1000)/(0x8000/2));
	//当量程为±2g时，转换为g/s的加速度换算公式  
	*acc_z = (signed short)(((double)*acc_z*1000)/(0x8000/2));
	
	return 0;
}

//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//       其他,错误代码
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
  IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
  IIC_Send_Byte(reg);	//写寄存器地址
  IIC_Wait_Ack();		//等待应答
	for(i=0;i<len;i++)
	{
		IIC_Send_Byte(buf[i]);	//发送数据
		if(IIC_Wait_Ack())		//等待ACK
		{
			IIC_Stop();	 
			return 1;		 
		}		
	}    
  IIC_Stop();	 
	return 0;	
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//       其他,错误代码
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
  IIC_Send_Byte(reg);	//写寄存器地址
  IIC_Wait_Ack();		//等待应答
  IIC_Start();
	IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令	
  IIC_Wait_Ack();		//等待应答 
	while(len)
	{
		if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
  IIC_Stop();	//产生一个停止条件 
	return 0;	
}
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//       其他,错误代码
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) 				 
{ 
  IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
  IIC_Send_Byte(reg);	//写寄存器地址
  IIC_Wait_Ack();		//等待应答 
	IIC_Send_Byte(data);//发送数据
	if(IIC_Wait_Ack())	//等待ACK
	{
		IIC_Stop();	 
		return 1;		 
	}		 
    IIC_Stop();	 
	return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
  IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	IIC_Wait_Ack();		//等待应答 
  IIC_Send_Byte(reg);	//写寄存器地址
  IIC_Wait_Ack();		//等待应答
  IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|1);//发送器件地址+读命令	
  IIC_Wait_Ack();		//等待应答 
	res=IIC_Read_Byte(0);//读取数据,发送nACK 
  IIC_Stop();			//产生一个停止条件 
	return res;		
}

