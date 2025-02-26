#include "led.h"
#include "delay.h"

void power_init(void)
{
	gpio_init_type gpio_init_struct;
	/* enable the led clock */
	crm_periph_clock_enable(POWER_CRM, TRUE);
	crm_periph_clock_enable(CHG_DET_CRM, TRUE);
	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure the led gpio */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_pins =  POWER_EN_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(POWER_GPIO, &gpio_init_struct);
	
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins =  CHG_DET_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init(CHG_DET_GPIO, &gpio_init_struct);
	
	gpio_bits_write(POWER_GPIO, POWER_EN_PIN, FALSE);
}


void power_on(void)
{
	gpio_bits_write(POWER_GPIO, POWER_EN_PIN, TRUE);
}

void power_off(void)
{
	gpio_bits_write(POWER_GPIO, POWER_EN_PIN, FALSE);
}

