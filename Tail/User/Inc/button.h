#ifndef __BUTTON_H
#define __BUTTON_H

#include "stdio.h"
#include "at32f421_clock.h"

#define BUTTON_GPIO_CRM	CRM_GPIOA_PERIPH_CLOCK
#define BUTTON_GPIO			GPIOA
#define BUTTON_PIN			GPIO_PINS_2

void button_init(void);
uint8_t button_scan(void);
void button_keep_startup(void);
uint8_t button_hander(void);
void button_delay_ms(uint32_t nms);

#endif

