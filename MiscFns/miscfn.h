#ifndef __MISCFN_H
#define __MISCFN_H

#include <stdint.h>
#include <stdio.h>

#include "signals.h"
#include "arm_math.h"
#include <stdint.h>

#ifndef STM32F446xx
#define STM32F446xx
#endif
#include "stm32f4xx.h"
#include "systick.h"

#define OFFSET1 (5u)
#define OFFSET2 (10u)
#define MAFLTR_PTS (11u)

void plotInput();
void serialPlotInput();
void delayFn(int dly);
void serialPlotSignal(float32_t* inputSignal, uint32_t sigLength);
void plotSignal(float32_t *inputSignal, uint32_t sigLength);
void serialPlotAll();
void serialPlotRunSum();
void calcRunningSum (float32_t  *sigSrcArr,float32_t  *sigDestArr,
		 uint32_t   sigSrcLength);

void convSig(float32_t  *sigSrcArr,
			 float32_t  *sigDestArr,
			 float32_t  *impRespArr,
			 uint32_t   sigSrcLength,
			 uint32_t   impSrcLength);
void calcSigDFT(float32_t  *sigSrcArr,
		 float32_t  *sigDestReArr,
		 float32_t  *sigDestImgArr,
		 uint32_t   sigLength);
void calcSigIDFT(float32_t  *idftOutArr,
		 float32_t  *sigDestReArr,
		 float32_t  *sigDestImgArr,
		 uint32_t   sigLength);
void serialPlotReX(float32_t  *sigSrcArr, uint32_t   sigLength);
void getDftOpMagnitude(float32_t  *sigSrcArr,uint32_t   sigLength);
void serialPlotReXCMSIS(float32_t  *sigSrcArr, uint32_t   sigLength);
void getDftOpMagnitudeCMSIS(float32_t  *sigSrcArr,uint32_t   sigLength);

void serialPlotDFTIDFT(float32_t  *sigDFTArr,uint32_t   sigDFTLength,
		float32_t  *sigIDFTArr,uint32_t   sigIDFTLength);
void movingAverage(float32_t  *sigSrcArr, float32_t  *sigOutArr,
		uint32_t   sigLength, uint32_t filterPts);
void blinkLed();
#endif /*__MISCFN_H*/
