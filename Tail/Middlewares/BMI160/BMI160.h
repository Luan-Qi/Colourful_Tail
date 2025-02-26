#ifndef __BMI160_H
#define __BMI160_H

#include "at32f421_clock.h"

#define MPU_DEVICE_ID_REG		0X00	//����ID�Ĵ���

//���AD0��(9��)�ӵ�,IIC��ַΪ0X68(���������λ).
//�����V3.3,��IIC��ַΪ0X69(���������λ).
#define MPU_ADDR				0X69

uint8_t MPU_Get_Gyroscope(short *gyr_x,short *gyr_y,short *gyr_z);
uint8_t MPU_Get_Accelerometer(short *acc_x,short *acc_y,short *acc_z);

uint8_t BMI160_init(void);
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data);
uint8_t MPU_Read_Byte(uint8_t reg);


#endif


