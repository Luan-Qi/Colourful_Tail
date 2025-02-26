#include "button.h"
#include "delay.h"
#include "timer.h"

/*按键引脚初始化*/
void button_init(void)
{
	gpio_init_type gpio_init_struct;

	crm_periph_clock_enable(BUTTON_GPIO_CRM, TRUE);

	gpio_default_para_init(&gpio_init_struct);

	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = BUTTON_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init(BUTTON_GPIO, &gpio_init_struct);
}

uint8_t button_scan(void)
{
	if(gpio_input_data_bit_read(BUTTON_GPIO,BUTTON_PIN)==RESET)
	{
		delay_ms(1);
		if(gpio_input_data_bit_read(BUTTON_GPIO,BUTTON_PIN)==RESET) 
		{
			return 1;
		}
	}
	return 0;
}

void button_keep_startup(void)
{
	delay_ms(1000);
}

uint32_t current_time = 0;

uint8_t family_index = 0;
uint8_t type_index = 0;

uint8_t shut_down_flag = 0;

uint32_t press_down_time = 0;
uint32_t press_up_time = 0;
uint8_t butten_flag = 0;

uint8_t button_hander(void)
{
	if(butten_flag==0)
	{
		if(button_scan()==1)
		{
			press_down_time = system_time;
			butten_flag = 1;
		}
	}
	else
	{
		if(button_scan()==0)
		{
			press_up_time = system_time;
			butten_flag = 0;
			if((press_up_time-press_down_time)>1000) {shut_down_flag++;return 2;}
			else if((press_up_time-press_down_time)>500) {family_index++;type_index = 0;return 2;}
			else return 1;
		}
	}
	return 0;
}

void button_delay_ms(uint32_t nms)
{
	current_time = system_time;
	while((system_time-current_time)<nms)
	{
		switch(button_hander())
		{
			case 1:type_index++;break;
			case 2:type_index = 5;break;
		}
	}
}

