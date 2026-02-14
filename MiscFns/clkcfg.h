#ifndef __CLKCFG_H
#define __CLKCFG_H

#ifndef STM32F446xx
#define STM32F446xx
#endif
#include "stm32f4xx.h"

#define TIM2SR_UIF (1u<<0)

void clk100MhzCfg();

void tim2Interrupt1HzInit();
void TIM2_IRQHandler();

#endif /*__CLKCFG_H*/
