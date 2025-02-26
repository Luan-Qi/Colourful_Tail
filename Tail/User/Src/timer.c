#include "timer.h"

uint32_t system_time = 0;

crm_clocks_freq_type crm_clocks_freq_struct = {0};

void clkout_config(void)
{
	/* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* enable tmr1 clock */
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);

  /* tmr1 configuration */
  /* time base configuration */
  /* systemclock/12000/5000 = 2hz */
  tmr_base_init(TMR1, 9, (crm_clocks_freq_struct.ahb_freq / 10000) - 1);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);

  /* tmr1 overflow interrupt nvic init */
  //nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR1_BRK_OVF_TRG_HALL_IRQn, 0, 0);

  /* enable tmr1 */
  tmr_counter_enable(TMR1, TRUE);
}

/**
  * @brief  this function handles timer1 overflow handler.
  * @param  none
  * @retval none
  */
void TMR1_BRK_OVF_TRG_HALL_IRQHandler(void)
{
  if(tmr_flag_get(TMR1, TMR_OVF_FLAG) != RESET)
  {
		system_time++;
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
}
