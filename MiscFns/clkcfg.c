#include "clkcfg.h"

/*
 * NUCLEO-F446RE can go up to 180MHz , here we are going up to 100MHz
 * System clock source = HSE
 * SYSCLK = 100 MHz
 * HCLK = 100 MHz
 * AHB prescaler = 1
 * for F446RE APB1 prescaler should be 4 for 25mhz and APB2 should be 2 for 50 mhz
 * we should run at 180 to get 45mhz APB1 which is max frequency allowed
 * and get 90mhz for APB2
 * APB1 prescaler = 2
 * APB2 prescaler = 1
 * HSE = 8 MHz
 */
/*we write q m n values for PLL based on cubeMX to get 100mhz*/
/*
#define PLL_M  4
#define PLL_N  200
#define PLL_P  4
#define PLL_Q  9
*/
//trying 160 mhz values , APB1 40mhz with 4 prescaler, APB2 80mhz with 2 prescaler
#define PLL_M  4
#define PLL_N  160
#define PLL_P  2
#define PLL_Q  2
/*
 * PLLP specified in the datasheet, it's not direct number for division factor
 * each bit value correspond to a division factor for ex bits 010b is 6
 *  00: PLLP = 2
	01: PLLP = 4
	10: PLLP = 6
	11: PLLP = 8
 * */

static void tim2Callback();

void clk100MhzCfg(){
	/*enable HSE oscillator */
	RCC->CR |= RCC_CR_HSEON;
	/*Wait until HSE ready pin (HSERDY) goes low*/
	while((RCC->CR  & RCC_CR_HSERDY) == (RCC_CR_HSERDY)){}
	/*set HCLK prescaler*/
	//default is 0 , technically this is not needed
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	/*set APB1 prescaler*/
	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	/*set APB2 prescaler*/
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	/*Configure PLL*/
	/*RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (PLL_P << 16)
			| (RCC_PLLCFGR_PLLSRC_HSE)|  (PLL_Q << 24) ;*/
	RCC->PLLCFGR = PLL_M | (PLL_N << 6)| (((PLL_P >>1)-1) << 16)|
			(RCC_PLLCFGR_PLLSRC_HSE)|(PLL_Q <<24);
	/*turn on PLL*/
	RCC->CR |= RCC_CR_PLLON;
	/*wait for PLL to come on*/
	while((RCC->CR  & RCC_CR_PLLRDY) == (0)){}
	/*Set Flash Latency*/
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	//FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
	FLASH->ACR |= FLASH_ACR_LATENCY_5WS; //for 160mhz
	/*Select the main PLL as the clock source*/
	/*HSE 8mhz is the clock source for PLL, and PLL is what will generate 100 mhz*/
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	/*Wait for main PLL to be used as clk source*/
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL ){}

}

#define TIM2EN   (1u << 0)
#define CR1_CEN  (1u << 0)
#define DIER_UIE (1u << 0)

//APB1 periph clock is 40mhz (timer clock is 80mhz)
void tim2Interrupt1HzInit(){
	/*enable clock access to timer 2*/
	RCC->APB1ENR |= TIM2EN;
	/*Set prescale value */
	TIM2->PSC =   8000-1;     //at 80MHz 80 000 000 / 8000 = 10000
	/*set auto-reload value*/
	//TIM2->ARR =10000-1 ; /// 10000/10000 = 1 << so clock become 1 hz
	TIM2->ARR =10-1 ; /// 10000/10 = 1000 << so clock become 1 Khz
	/*clean counter*/
	TIM2->CNT = 0;
	/*Enable timer */
	TIM2->CR1 = CR1_CEN;
	/*Enable timer interrupt*/
	//DIER, dma and interrupt enable register
	TIM2->DIER |= DIER_UIE;
	/*Enable timer interrupt in NVIC*/
	NVIC_EnableIRQ(TIM2_IRQn);

}



static void tim2Callback(){
	printf("A second just passed !! \n\r");
	/*Check if FIFO not full*/
//	if(fifoFullFlag == 1){ //fifo not full
//		fifoFullFlag = rxFifoPut(adcRead());
//	}else{/*RX FIFO full*/
//		processFlag = 1;
//	}

}
void TIM2_IRQHandler(){
	/*clear update interrupt flag*/
	TIM2->SR &= ~TIM2SR_UIF;
	/*do something..*/
	tim2Callback();
}

