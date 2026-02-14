

//symbol to use cm4 math lib
#define ARM_MATH_CM4
//enable fpu for CMSIS
#define __FPU_PRESENT
#include <stdio.h>

#define STM32F446xx
#include "stm32f4xx.h"
#include "stm32f446xx.h"
#include "signals.h"
#include "uart.h"
#include "systick.h"
#include "arm_math.h"
#include "arm_sorting.h"
#include "miscfn.h"
#include "clkcfg.h"
#include "adc.h"
#include "filters.h"

extern float _5hz_signal[HZ_5_SIG_LEN];
extern float32_t inputSignal_f32_1kHz_15kHz[KHZ_15_SIG_LEN];
float g_insigSample;
extern float32_t  impulse_response[IMP_RSP_LEN];
float g_impRspSample;
float32_t  outputSigArr[HZ_10_100_500HZ_SIGLEN+ LPF_70HZ_IMP_RESP_LEN -1];

arm_rfft_fast_instance_f32 fftHandler;
#define FFTLENGTH (512)

extern float32_t IpSig_10hz_100hz_500hz[HZ_10_100_500HZ_SIGLEN];
extern float32_t Lpf_70Hz_impulseResponse[LPF_70HZ_IMP_RESP_LEN];
extern float32_t Hpf_400hz_impulseResponse[HPF_400HZ_IMP_RESP_LEN];
volatile float32_t  reX2[HZ_10_100_500HZ_SIGLEN/2]; //real part of DFT is half the length of i/p signal
volatile float32_t  imX2[HZ_10_100_500HZ_SIGLEN/2]; //imaginary part of DFT is half the length of i/p signal

volatile float32_t  outputArrDFTCMSIS2[HZ_10_100_500HZ_SIGLEN];

float32_t magnitude[HZ_10_100_500HZ_SIGLEN/2 + 1];

volatile float32_t  outputArrMvg[KHZ_15_SIG_LEN];
volatile uint32_t sensorValue;
volatile uint32_t filteredSensorVal;

extern float LP_1HZ_2HZ_IMPULSE_RESPONSE[IMP_RSP_LENGTH];
fir_filter lpfFir;

rx_dataType sensorDataBuffer[RXFIFO_SIZE];
uint8_t fifoFullFlag, processFlag;

#define INPUT_SIG_LEN RXFIFO_SIZE
uint32_t  outputSigArrFifo[INPUT_SIG_LEN+ IMP_RSP_LENGTH -1];
volatile float32_t  outputArrMvgFifo[INPUT_SIG_LEN];

static void clearDataBuffer();

static uint8_t readFifo(rx_dataType * dataBuff);
void convTesting();
int main(void)
{
	/*Enable FPU*/
	SCB->CPACR |= ((3U << 20) | (3U << 22));
	/*Setclock tree*/
	clk100MhzCfg();
	/*init uart*/
	uart2TxInit();

	/*Initialize SysTick Counter*/
	systickCounterInit();
	//init adc
	pa1AdcInit();
	//enable timer2 interrupt
	tim2Interrupt1HzInit();
	//start conversion
	startAdcConversion();

	/*Init FIFO*/
	rxFifoInit();
	uint32_t rxData;

	/*Initialize FIR filter*/
	firFilterInit(&lpfFir, LP_1HZ_2HZ_IMPULSE_RESPONSE, IMP_RSP_LENGTH);
	//init fft handler
	arm_rfft_fast_init_f32(&fftHandler, FFTLENGTH);
	/*CMSIS DSP API*/

	arm_rfft_fast_f32	(	&fftHandler,
	(float32_t * )	IpSig_10hz_100hz_500hz,
	(float32_t * )	outputArrDFTCMSIS2,
	0
	);

	//getDftOpMagnitudeCMSIS(outputArrDFTCMSIS2, (HZ_10_100_500HZ_SIGLEN/2));
	arm_cmplx_mag_f32(outputArrDFTCMSIS2, magnitude, HZ_10_100_500HZ_SIGLEN/2);
	serialPlotReX((float32_t  *)magnitude,(uint32_t) (HZ_10_100_500HZ_SIGLEN/2));

	movingAverage( inputSignal_f32_1kHz_15kHz,  outputArrMvg,
			KHZ_15_SIG_LEN, MAFLTR_PTS);


 	while(1){
 		/*serialPlotDFTIDFT((float32_t  *)inputSignal_f32_1kHz_15kHz,(uint32_t)   KHZ_15_SIG_LEN,
 			(float32_t  *)outputArrMvg,(uint32_t)   KHZ_15_SIG_LEN);*/
 		//serialPlotReXCMSIS(outputArrDFTCMSIS2, (HZ_10_100_500HZ_SIGLEN/2));

 		/*sensorValue = adcRead();
 		filteredSensorVal = firFilterUpdate(&lpfFir, sensorValue);
 		printf("%d,", (int)sensorValue);
 		printf("%d\n\r", (int)filteredSensorVal);*/
// 		for (int i = 0; i< 50; i++){
// 				rxFifoPut(adcRead());
// 			}
// 		for (int i = 0; i< 50; i++){
// 			rxFifoGet(&rxData);
//
// 		}
//
// 		if(processFlag){
// 			clearDataBuffer();
// 			/*Read FIFO into data buffer*/
// 			for(int i = 0; i<RXFIFO_SIZE ; i++){
// 				/*wait until entire batch is collected from ADC
// 				 * entire batch is FIFO SIZE*/
// 				while(fifoFullFlag == 1){}
//
// 				/*Read data into data buff*/
// 				fifoFullFlag = readFifo(sensorDataBuffer +i);
// 			}
// 			/*perform digital signal processing*/
//
// 			 arm_conv_f32((float32_t  *)sensorDataBuffer,
// 									(uint32_t)   INPUT_SIG_LEN,
// 									(float32_t  *)LP_1HZ_2HZ_IMPULSE_RESPONSE,
// 									(uint32_t)  IMP_RSP_LENGTH,
// 									(float32_t  *)outputSigArrFifo);
//
// 			movingAverage( sensorDataBuffer,  outputArrMvgFifo,
// 					INPUT_SIG_LEN, MAFLTR_PTS);
//
// 			for(int i = 0; i<(INPUT_SIG_LEN+ IMP_RSP_LENGTH -1) ; i++){
// 				if(i<INPUT_SIG_LEN){
//
// 		 		printf("%d,", (int)(OFFSET2 +sensorDataBuffer[i]));
// 				}
// 		 		printf("%d\n\r", (int)outputSigArrFifo[i]);
// 			}
// 			/*
//
// 			for(int i = 0; i<(INPUT_SIG_LEN) ; i++){
//
// 		 		printf("%d,", (int)(OFFSET2 +sensorDataBuffer[i]));
//
// 		 		printf("%d\n\r", (int)outputArrMvgFifo[i]);
// 			}
//
// 			*/
//
// 			/*reset process flag to exit loop*/
// 		 	processFlag = 0;
// 		}
// 		delayFn(100000);

 	}

}
static uint8_t readFifo(rx_dataType * dataBuff){
	volatile uint8_t rdFlag;
	/*place FIFO data into data buffer*/
	rdFlag = rxFifoGet(dataBuff);
	/*if FIFO is empty then reset FIFO full flag*/
	if(rdFlag ==0){
		/*this will start the fifo put routine again to
		 * collect the next batch of samples*/
		fifoFullFlag = 1;
	}else{
		//keep fifo full flag at fifo Full (0)
		fifoFullFlag = 0;
	}
	return fifoFullFlag;
}

static void clearDataBuffer(){
	for(int i = 0; i < RXFIFO_SIZE; i++){
		sensorDataBuffer[i] = 0;
	}
}



