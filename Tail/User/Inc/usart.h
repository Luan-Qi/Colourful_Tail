#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include <stdint.h>

void uart_print_init(uint32_t baudrate);
void uart_wifi_init(void);

#endif

