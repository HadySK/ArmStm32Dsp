#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#define STM32F446xx
#include "stm32f4xx.h"


void uart2TxInit();
void uart2_write(int ch);
void uart2_read();









#endif /* __UART_H__*/
