#ifndef __WS2812_H
#define __WS2812_H

#include "at32f421_clock.h"

#define PIXEL_NUM 77

//硬件spi模拟ws2811时序（用spi的8位数据模拟ws281x的一位数据）
//要将系统时钟设置为56M，分频数设置为8，则SPI的通信频率为7M，传输一位数据的时间约为143纳秒（ns）
//3*143 = 429ns   5*143 = 715ns  符合WS281X芯片的通信时序。
//  _____   
// |     |___|   11111000  high level
//  ___         
// |   |_____|   11100000  low level

#define WS_HIGH 0XF8
#define WS_LOW  0XE0

#define WITHE_s 	ws281x_color(10,10,10)
#define RAD_s 		ws281x_color(30,0,0)
#define GREEN_s 	ws281x_color(0,20,0)
#define BLUE_s 		ws281x_color(0,0,30)
#define YELLOW_s 	ws281x_color(10,10,0)
#define ORANGE_s 	ws281x_color(16,8,0)

#define WITHE_ss 	ws281x_color(4*light_gain,4*light_gain,4*light_gain)
#define RAD_ss 		ws281x_color(12*light_gain,0*light_gain,0*light_gain)
#define GREEN_ss 	ws281x_color(0*light_gain,8*light_gain,0*light_gain)
#define BLUE_ss 	ws281x_color(0*light_gain,0*light_gain,10*light_gain)
#define YELLOW_ss ws281x_color(4*light_gain,4*light_gain,0*light_gain)
#define ORANGE_ss ws281x_color(6*light_gain,3*light_gain,0*light_gain)
#define PINK_ss 	ws281x_color(5*light_gain,3*light_gain,4*light_gain)

//uint8_t order_array[PIXEL_NUM];

void ws281x_init(void);
void ws281x_closeAll(void);//关闭并显示所有像素
uint32_t ws281x_color(uint8_t red, uint8_t green, uint8_t blue);//将RGB颜色转换成GRB颜色
void ws281x_setPixelColor(uint16_t n ,uint32_t GRBcolor);//以RGB设置单个像素
void ws281x_setPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue);//以RGB设置单个像素
void ws281x_closePixel(uint16_t n);//关闭指定像素
void ws281x_show(void);//开启DMA传输，显示像素
void ws281x_setNothing(void);
void ws281x_setColor(uint32_t GRBcolor);
void ws281x_setArrayColor(const uint8_t * array, uint32_t GRBcolor);
void ws281x_setMixArrayColor(const uint8_t * array1, uint32_t GRBcolor1,const uint8_t * array2, uint32_t GRBcolor2);
void ws281x_addArrayColor(const uint8_t * array, uint32_t GRBcolor);
void ws281x_setArrayPixelRGB(const uint8_t * red, const uint8_t * green, const uint8_t * blue);
uint32_t ws281x_wheel(uint8_t wheelPos);
void ws281x_rainbow(uint8_t wait);
#endif

