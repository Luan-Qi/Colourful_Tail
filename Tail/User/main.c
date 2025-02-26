#include "at32f421_clock.h"
#include <stdio.h>
#include "delay.h"
#include "timer.h"
#include "usart.h"
#include "led.h"
#include "adc.h"
#include "button.h"
#include "ws2812.h"
#include "BMI160.h"
#include "pattern.h"

uint8_t reflash_flag = 1;

extern uint32_t current_time;

extern uint8_t shut_down_flag;

extern uint8_t family_index;
extern uint8_t type_index;
uint8_t fps_index;

uint16_t battery_volt = 0;
uint16_t battery_volt_max = 0;
uint16_t battery_volt_min = 4200;

uint8_t light_gain = 1;
uint8_t family_index_old = 0;

short gyrox,gyroy,gyroz;
short accex,accey,accez;

int main(void)
{
  system_clock_config();
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  uart_print_init(115200);
	clkout_config();
	delay_init();
	AdcInit();
	power_init();
	power_on();
	button_init();
	ws281x_init();
	BMI160_init();
	//电量显示
	ws281x_setArrayColor(pattern_battery,YELLOW_ss);delay_ms(10);
	if(gpio_input_data_bit_read(CHG_DET_GPIO,CHG_DET_PIN)==1)
	{
		power_on();
		battery_volt = GetAdcAverage(ADC_Channel,5)*3330/2048;
		printf("battery_volt: %d mV\r\n", battery_volt);
		if(battery_volt>4000)
		{
			ws281x_addArrayColor(pattern_battery_Full,GREEN_ss);
		}
		else if(battery_volt>3700)
		{
			ws281x_addArrayColor(pattern_battery_Medium,RAD_ss);
		}
		else if(battery_volt>3400)
		{
			ws281x_addArrayColor(pattern_battery_Low,RAD_ss);
		}
		else
		{
			ws281x_addArrayColor(pattern_battery_Warn,RAD_ss);
			delay_ms(1500);power_on();
		}
	}
	else//充电指示，仅反应充电
	{
		power_off();
		while(1)
		{
			if(battery_volt_max>4140)//充满显示并死循环
			{
				ws281x_setMixArrayColor(pattern_battery,ORANGE_ss,pattern_battery_Full,GREEN_ss);
				while(1){}
			}
			else if(battery_volt_max>3900)
			{
				ws281x_setMixArrayColor(pattern_battery,ORANGE_ss,pattern_battery_Medium,RAD_ss);delay_ms(500);
				ws281x_setMixArrayColor(pattern_battery,ORANGE_ss,pattern_battery_Full,RAD_ss);delay_ms(500);
			}
			else if(battery_volt_max>3600)
			{
				ws281x_setMixArrayColor(pattern_battery,ORANGE_ss,pattern_battery_Low,RAD_ss);delay_ms(500);
				ws281x_setMixArrayColor(pattern_battery,ORANGE_ss,pattern_battery_Medium,RAD_ss);delay_ms(500);
			}
			else
			{
				ws281x_setMixArrayColor(pattern_battery,ORANGE_ss,pattern_battery_empty,RAD_ss);delay_ms(500);
				ws281x_setMixArrayColor(pattern_battery,ORANGE_ss,pattern_battery_Low,RAD_ss);delay_ms(500);
			}
			battery_volt = GetAdcAverage(ADC_Channel,10)*3330/2048;
			if(battery_volt>battery_volt_max) battery_volt_max = battery_volt;
			if(battery_volt_max>battery_volt+50) battery_volt_max = battery_volt;
			printf("battery_volt: %d mV\r\n", battery_volt_max);
		}
	}
	//长按开机
	button_keep_startup();
	
//	while(button_scan()<3)
//	{
//		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);
//		MPU_Get_Accelerometer(&accex,&accey,&accez);
//		//printf("gyrox=%d,gyroy=%d,gyroz=%d,accex=%d,accey=%d,accez=%d\r\n",gyrox,gyroy,gyroz,accex,accey,accez);
//		//printf("accex=%d,accey=%d,accez=%d\r\n",accex,accey,accez);
//		if(accez<-200) printf("%d\r\n",accez);
//	}
	
	if(button_scan()==1) power_on();
	else power_off();
	//长按设置亮度，最大亮度5
	ws281x_closeAll();
	current_time = system_time;
	while(button_scan()==1)
	{
		if((system_time-current_time)>750)
		{
			ws281x_setArrayColor(pattern_number[light_gain],WITHE_ss);
			current_time = system_time;
			light_gain++;
			if(light_gain>5) light_gain = 5;
		}
	}
	
	delay_ms(100);
	
  while(1)
  {
		//静态图片
		switch(family_index)
		{
			case 0:
			{
				if(button_hander()==1){type_index++;}
				else{if(reflash_flag!=1) break;}
				if(type_index>7) type_index = 0;
				switch(type_index)
				{
					case 0:ws281x_setArrayColor(pattern_love,RAD_ss);break;
					case 1:ws281x_setArrayColor(pattern_fan[0],BLUE_ss);break;
					case 2:ws281x_setMixArrayColor(pattern_happy_A,ORANGE_ss,pattern_happy_B,RAD_ss);break;
					case 3:ws281x_setMixArrayColor(pattern_happy_A,ORANGE_ss,pattern_happy_C,RAD_ss);break;
					case 4:ws281x_setMixArrayColor(pattern_skeleton_A,GREEN_ss,pattern_skeleton_B,RAD_ss);break;
					case 5:ws281x_setMixArrayColor(pattern_warn_A[0],YELLOW_ss,pattern_warn_B,RAD_ss);break;
					case 6:ws281x_setMixArrayColor(pattern_battery,YELLOW_ss,pattern_battery_Full,RAD_ss);break;
					case 7:ws281x_setColor(PINK_ss);break;
				}
				break;
			}
			//循环动画
			case 1:
			{
				if(type_index>9) break;
				if(type_index==9) type_index = 0;
				switch(type_index)
				{
					case 0:
					{
						if(fps_index>2) fps_index = 0; else fps_index++;
						ws281x_setMixArrayColor(pattern_warn_A[fps_index],ORANGE_ss,pattern_warn_B,RAD_ss);
						button_delay_ms(pattern_warn_fps_delay[fps_index]);
						break;
					}
					case 1:
					{
						if(fps_index>0) fps_index = 0; else fps_index++;
						ws281x_setArrayColor(pattern_fan[fps_index],BLUE_ss);
						button_delay_ms(200);
						break;
					}
					case 2:
					{
						if(fps_index>22) fps_index = 0; else fps_index++;
						fps_index<12 ? ws281x_setArrayColor(pattern_love,ws281x_color(fps_index*light_gain,0,0)) : ws281x_setArrayColor(pattern_love,ws281x_color((23-fps_index)*light_gain,0,0));
						button_delay_ms(50);
						break;
					}
					case 3:
					{
						if(fps_index>6) fps_index = 0; else fps_index++;
						ws281x_setArrayColor(pattern_zoom[fps_index],RAD_ss);
						button_delay_ms(80);
						break;
					}
					case 4:
					{
						if(fps_index>8) fps_index = 0; else fps_index++;
						ws281x_setArrayColor(pattern_number[fps_index],WITHE_ss);
						button_delay_ms(800);
						break;
					}
					case 5:
					{
						if(fps_index>0) fps_index = 0; else fps_index++;
						fps_index ? ws281x_setMixArrayColor(pattern_happy_A,ORANGE_ss,pattern_happy_B,RAD_ss) : ws281x_setMixArrayColor(pattern_happy_A,ORANGE_ss,pattern_happy_C,RAD_ss);
						button_delay_ms(400);
						break;
					}
					case 6:
					{
						if(fps_index>0) fps_index = 0; else fps_index++;
						fps_index ? ws281x_setArrayColor(pattern_skeleton_A,GREEN_ss) : ws281x_setMixArrayColor(pattern_skeleton_A,GREEN_ss,pattern_skeleton_B,RAD_ss);
						button_delay_ms(400);
						break;
					}
					case 7:
					{
						if(fps_index>254) fps_index = 0; else fps_index++;
						ws281x_rainbow(fps_index);
						button_delay_ms(20);
						break;
					}
					case 8:
					{
						if(fps_index>0) fps_index = 0; else fps_index++;
						fps_index ? ws281x_setColor(ws281x_color(16*light_gain,16*light_gain,16*light_gain)) : ws281x_closeAll();
						button_delay_ms(60);
						break;
					}
				}
				break;
			}
			//数字显示
			case 2:
			{
				if(button_hander()==1){type_index++;}
				else{if(reflash_flag!=1) break;}
				if(type_index>9) type_index = 0;
				ws281x_setArrayColor(pattern_number[type_index],GREEN_ss);
				break;
			}
			case 3:
			{
				ws281x_setColor(RAD_s);
				while(accez<-200)
				{
					MPU_Get_Accelerometer(&accex,&accey,&accez);printf("%d\r\n",accez);
				}
				reflash_flag = 1;
				family_index = family_index_old;
				break;
			}
		}
		
		MPU_Get_Accelerometer(&accex,&accey,&accez);printf("%d\r\n",accez);
		if(accez<-200){family_index_old = family_index;family_index = 3;continue;}
		
		battery_volt = GetAdcAverage(ADC_Channel,3)*3330/2048;
		if(battery_volt<battery_volt_min) battery_volt_min = battery_volt;
		if(battery_volt_min+50>battery_volt) battery_volt_min = battery_volt;
		
		if(battery_volt_min<3400)//低电量关机，60秒后自动关机
		{
			ws281x_setMixArrayColor(pattern_battery,YELLOW_ss,pattern_battery_Warn,RAD_ss);
			current_time = system_time;
			while(button_scan()==0)
			{
				if((system_time-current_time)>60000)
				{
					break;
				}
			}
			shut_down_flag = 1;
		}
		if(family_index>2) family_index = 0;
		if(shut_down_flag>0) power_off();
  }
}

