#include "adc.h"

//adc简单延时函数
static void AdcDelay(unsigned int time)
{
	while(time--);
}

//adc初始化函数
void AdcInit(void)
{
	gpio_init_type gpio_initstructure;
	adc_base_config_type adc_base_struct;
	
  crm_periph_clock_enable(ADC_CRM, TRUE);
	crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
	crm_adc_clock_div_set(CRM_ADC_DIV_6);

  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = ADC_Pin;
	gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_init(ADC_GPIO, &gpio_initstructure);

  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 1;
  adc_base_config(ADC1, &adc_base_struct);
	
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_enable(ADC1, TRUE);
	
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1)==SET);
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1)==SET);
}

//获得ADC值
//ch:通道值 0~3
uint16_t GetAdc(adc_channel_select_type ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	adc_ordinary_channel_set(ADC1, ch, 1, ADC_SAMPLETIME_239_5);	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	adc_ordinary_software_trigger_enable(ADC1, TRUE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(adc_flag_get(ADC1, ADC_CCE_FLAG)==RESET);//等待转换结束

	return adc_ordinary_conversion_data_get(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//求取平均值
uint16_t GetAdcAverage(adc_channel_select_type ch,uint8_t times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=GetAdc(ch);
		AdcDelay(5);
	}
	return temp_val/times;
}





