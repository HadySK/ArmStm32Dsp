#include <stdio.h>
#include "uart.h"

#define GPIOAEN  	 (1U<<0)
#define UART2EN      (1U<<17)
#define UART_CR1_TE  (1U<<3)
#define UART_CR1_RE  (1U<<2)
#define UART_CR1_UE  (1U<<13)
#define SR_TXE (1U<<7)
#define SR_RXNE (1U<<5)
/*
#define SYS_FREQ (16000000)
#define APB1_CLK SYS_FREQ
 */
//update frequency after changing PLL to 100MHz so APB1 is 100mhz/4 = 25MHz
//#define SYS_FREQ (100000000)
#define SYS_FREQ (160000000)
#define APB1_CLK (SYS_FREQ/4)

#define UART_BAUDRATE 115200
static void computeUartBd(uint32_t periphClk, uint32_t baudRate);

void uart2TxInit(){

	/*config uart gpio pin*/
	//enable clock access gpioa
	RCC->AHB1ENR |= GPIOAEN;
	//set pa2 mode to alternate fn mode
	GPIOA->MODER &= ~(1U<<4);
	GPIOA->MODER |= (1U<<5);
	//set pa2 to alternate function type to uart_tx(AF07) ARFL
	GPIOA->AFR[0]  |=  (0x7U <<8);
	//clear bit 11
	GPIOA->AFR[0]  &= ~(1U<<11);

	/*config uart gpio pin*/
	//enable clock access uart2
	RCC->APB1ENR |= UART2EN;
	//config baudrate
	computeUartBd(APB1_CLK, UART_BAUDRATE);
	//config transfer direction
	//two lines below could be combine into 1, since we clear the register.
	USART2->CR1 = UART_CR1_TE; // we are using equal and not or | to clear the register and just configure the TE bit

	//enable uart module
	USART2->CR1 |= UART_CR1_UE;

	//config for RX

	//set pa3 mode to alternate fn mode
	GPIOA->MODER &= ~(1U<<6);
	GPIOA->MODER |= (1U<<7);
	//set pa2 to alternate function type to uart_tx(AF07) ARFL
	GPIOA->AFR[0]  |=  (0x7U <<12);
	//clear bit 11
	GPIOA->AFR[0]  &= ~(1U<<15);
	USART2->CR1 |= UART_CR1_RE;
}


static void computeUartBd(uint32_t periphClk, uint32_t baudRate){

	uint16_t baudRateCnt = ((periphClk + (baudRate/2))/baudRate);

	USART2->BRR = baudRateCnt;

}

void uart2_write(int ch){
	/*check that transmit data register is empty*/
	while(!((USART2->SR) & SR_TXE)){}
	/*write to transmit data register*/
	USART2->DR = (ch & 0xFF);
}

//this function provided by st to route printf to a comm interface (eg uart)
int __io_putchar(int ch){
	uart2_write(ch);
	return ch;
}


void uart2_read(void){

	while(!((USART2->SR) & SR_RXNE)){
		// Poll the status register until RX data is ready
	}
	uint8_t readCh = USART2->DR;
	printf("Received character: %c\n\r", readCh);
}

