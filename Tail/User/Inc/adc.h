#include "at32f421_clock.h"

#ifndef adc_h
#define adc_h

#define ADC_CRM				CRM_GPIOA_PERIPH_CLOCK	//定义OLED端口时钟总线		
#define ADC_GPIO			GPIOA
#define ADC_Pin			GPIO_PINS_0
#define ADC_Channel	ADC_CHANNEL_0

void AdcInit(void);	//adc初始化函数
uint16_t GetAdc(adc_channel_select_type ch);	//获得ADC值
uint16_t GetAdcAverage(adc_channel_select_type ch,uint8_t times);	//求取平均值
void GetAdcUse(void);//获取要使用的AD值

#endif

