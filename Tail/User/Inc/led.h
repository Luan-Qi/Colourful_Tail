#ifndef __LED_H
#define __LED_H

#include "stdio.h"
#include "at32f421_clock.h"

#define POWER_CRM				CRM_GPIOA_PERIPH_CLOCK
#define POWER_GPIO				GPIOA
#define POWER_EN_PIN			GPIO_PINS_1

#define CHG_DET_CRM				CRM_GPIOB_PERIPH_CLOCK
#define CHG_DET_GPIO				GPIOB
#define CHG_DET_PIN			GPIO_PINS_2

void led_init(void);
void power_init(void);
void power_on(void);
void power_off(void);
void led_startup(void);

#endif


