// This file includes random supporting functions like,
// functions to plot signals or perform some calculation

#include "miscfn.h"


extern float _5hz_signal[HZ_5_SIG_LEN];
extern float32_t inputSignal_f32_1kHz_15kHz[KHZ_15_SIG_LEN];
extern float g_insigSample;
extern float32_t  impulse_response[IMP_RSP_LEN];
extern float g_impRspSample;
extern float32_t  outputSigArr[KHZ_15_SIG_LEN+ IMP_RSP_LEN -1];
extern float32_t  outputSigRunSum[KHZ_15_SIG_LEN];
extern float32_t _640_points_ecg_[ECG_SIG_LEN];
extern arm_rfft_fast_instance_f32 fftHandler;
#define FFTLENGTH2 (512)

uint32_t timeBefore, timeAfter, timeTaken;
float32_t timeS, timeMs;
float32_t timeS_CMSIS, timeMs_CMSIS;

void plotInput(){

	for(int i = 0 ; i < KHZ_15_SIG_LEN ; i++){
		g_insigSample = inputSignal_f32_1kHz_15kHz[i];
		delayFn(9000);

	}

}
void serialPlotInput(){


	for(int i = 0 ; i < KHZ_15_SIG_LEN ; i++){
			printf("%f\r\n",inputSignal_f32_1kHz_15kHz[i]);
			delayFn(9000);
		}

}

void delayFn(int dly){
for (int i = 0; i < dly ; i++){}

}

void plotSignal(float32_t *inputSignal, uint32_t sigLength){
	for(int i = 0 ; i < sigLength ; i++){
			g_impRspSample = inputSignal[i];
			delayFn(9000);

		}
}

void serialPlotSignal(float32_t* inputSignal, uint32_t sigLength){

	for(int i = 0 ; i < sigLength ; i++){
			printf("%f\r\n",inputSignal[i]);
			delayFn(9000);
		}

}


void serialPlotRunSum(){


	   for( int k = 0; k < (KHZ_15_SIG_LEN); k++ )
	   {

		   printf("%f,",OFFSET1 + inputSignal_f32_1kHz_15kHz[k]);
		   printf("%f\n\r",outputSigArr[k]);
		   delayFn(9000);

	   }

}
void serialPlotAll(){
	uint32_t i,j,k;
	i = j = 0;

	   for( k = 0; k < (KHZ_15_SIG_LEN + IMP_RSP_LEN -1); k++ )
	   {
		   i++;
		   j++;
		   if( i == KHZ_15_SIG_LEN ) {

			   i = 0;
		   }
		   if( j == IMP_RSP_LEN ) {

			   j = 0;
			}

		   printf("%f,",OFFSET1 + inputSignal_f32_1kHz_15kHz[i]);
		   printf("%f,",OFFSET2 + impulse_response[j]);
		   printf("%f\n\r",outputSigArr[k]);
		   delayFn(9000);

	   }

}



void calcRunningSum (float32_t  *sigSrcArr,float32_t  *sigDestArr,
		 uint32_t   sigSrcLength){
	for (int i = 0 ; i <sigSrcLength; i++){
		if (i == 0){

		sigDestArr[i] =  sigSrcArr[i];
		}
		else {

		sigDestArr[i] = sigDestArr[i-1] + sigSrcArr[i];
		}
	}
}



void convSig(float32_t  *sigSrcArr,
			 float32_t  *sigDestArr,
			 float32_t  *impRespArr,
			 uint32_t   sigSrcLength,
			 uint32_t   impSrcLength){
	uint32_t i, j;
	uint32_t sigDestLength = sigSrcLength + impSrcLength -1;
	//clear o/p buffer
	for ( i = 0; i < sigDestLength; i++){
		sigDestArr[i] = 0;
	}
	//convolution
	for ( i = 0; i < sigSrcLength; i++){
		for ( j = 0; j < impSrcLength; j++){
					sigDestArr[i+j] += sigSrcArr[i] *impRespArr[j];
				}
		}
}

void calcSigDFT(float32_t  *sigSrcArr,
		 float32_t  *sigDestReArr,
		 float32_t  *sigDestImgArr,
		 uint32_t   sigLength){

	int i,k,j;

		/*Clear destination buffers*/
		for( j = 0; j < (sigLength/2); j++)
		{
			sigDestReArr[j] = 0;
			sigDestImgArr[j] = 0;
		}

		/*Compute DFT*/
		for( k = 0; k < (sigLength/2); k++ )
		{
		    for( i = 0; i < sigLength; i++)
		    {
		    	sigDestReArr[k] = sigDestReArr[k] +sigSrcArr[i]*cos(2*PI*k*i/sigLength);
		    	sigDestImgArr[k] = sigDestImgArr[k] +sigSrcArr[i]*sin(2*PI*k*i/sigLength);

		    }
		}


}

void calcSigIDFT(float32_t  *idftOutArr,
		 float32_t  *sigDestReArr,
		 float32_t  *sigDestImgArr,
		 uint32_t   sigLength){
	int i,k;
	//normalize amplitude
	for ( k = 1; k < ((sigLength/2)-1); k++){
		sigDestReArr[k]  =  sigDestReArr[k]/(sigLength/2) ;
		sigDestImgArr[k] = -sigDestImgArr[k]/(sigLength/2) ;
	}
	//normalize amplitude for index 0 and index sigLength/2
	sigDestReArr[0]  =  sigDestReArr[0]/(sigLength) ;
	sigDestImgArr[0] = -sigDestImgArr[0]/(sigLength) ;
	sigDestReArr[(sigLength/2)-1]  =  sigDestReArr[(sigLength/2)-1]/(sigLength) ;
	sigDestImgArr[(sigLength/2)-1] = -sigDestImgArr[(sigLength/2)-1]/(sigLength) ;

	//computer IDFT
	//clear o/p buffer
	for ( i = 0; i < sigLength; i++){
		idftOutArr[i]  =  0 ;
	}
	//perform IDFT

	//outer loop loops over o/p signal and inner loop loops over i/p signal
	for ( k = 0; k < (sigLength/2); k++){
		for ( i = 0; i < sigLength; i++){
			idftOutArr[i] += sigDestReArr[k] * cos(2*PI*k*i/sigLength)
							+sigDestImgArr[k] * sin(2*PI*k*i/sigLength);

			}
	}

}
void getDftOpMagnitude(float32_t  *sigSrcArr,uint32_t   sigLength){
	  for( int k = 0; k < (sigLength/2); k++ )
		   {
		  	  sigSrcArr[k] = fabs(sigSrcArr[k]);
		   }

}

void serialPlotReX(float32_t  *sigSrcArr,uint32_t   sigLength){

	   for( int k = 0; k < (sigLength/2); k++ )
	   {
		   printf("%f\n\r",sigSrcArr[k]);
		   delayFn(9000);
	   }

}


void getDftOpMagnitudeCMSIS(float32_t  *sigSrcArr,uint32_t   sigLength){
	  for( int k = 1; k < (sigLength/2); k+=2 )
		   {
		  	  sigSrcArr[k] = fabs(sigSrcArr[k]);
		   }

}

void serialPlotReXCMSIS(float32_t  *sigSrcArr,uint32_t   sigLength){

	   for( int k = 1; k < (sigLength/2); k+=2 )
	   {
		   printf("%f\n\r",sigSrcArr[k]);
		   delayFn(9000);
	   }

}

void serialPlotDFTIDFT(float32_t  *sigDFTArr,uint32_t   sigDFTLength,
		float32_t  *sigIDFTArr,uint32_t   sigIDFTLength){
	uint32_t i;

	   for( i = 0; i < sigDFTLength; i++ )
	   {

		   printf("%f,",OFFSET1 + sigDFTArr[i]);
		   printf("%f\n\r", sigIDFTArr[i]);
		   delayFn(9000);

	   }

}

void movingAverage(float32_t  *sigSrcArr, float32_t  *sigOutArr,
		uint32_t   sigLength, uint32_t filterPts){
	int i,j;
	for(i= floor(filterPts/2); i<(sigLength - (filterPts/2))-1; i++){
		sigOutArr[i] = 0;
		for(j= -(floor(filterPts/2)); j < floor(filterPts/2); j++){
			sigOutArr[i] += sigSrcArr[i+j];
		}
		sigOutArr[i] = sigOutArr[i]/filterPts;
	}
}
//LED PA5

#define GIOAEN	(1U << 0) // to enable clock for GPIO A
#define PIN5	(1<<5)
#define LED_PIN	PIN5

void blinkLed(){
	/*Enable clock access to GPIOA*/
		RCC->AHB1ENR |= GIOAEN;
		/*Set PIN5 to output mode*/
		GPIOA->MODER |=  (1U << 10);
		GPIOA->MODER &= ~(1U << 11);
		//GPIOA->MODER &= 0xFFFFFBFF;
	    /* Loop forever */
		while(1){
			GPIOA->ODR ^= LED_PIN;
			for (int i = 0; i<100000; i++){}
		}
}

/*Testing convolution and FFT using CMSIS function and hand-coded function and comparing the execution speed*/
void convTesting(){

	float32_t  reX[ECG_SIG_LEN/2];
	float32_t  imX[ECG_SIG_LEN/2];

	float32_t  outputSigArrECG[ECG_SIG_LEN];
	float32_t  outputSigArrDFT[ECG_SIG_LEN];
	/*Manual API*/
	timeBefore = SysTick->VAL;
	convSig((float32_t  *)inputSignal_f32_1kHz_15kHz,
			(float32_t  *)outputSigArr,
			(float32_t  *)impulse_response,
			(uint32_t)   KHZ_15_SIG_LEN,
			(uint32_t)  IMP_RSP_LEN);

	timeAfter = SysTick->VAL;
		if (timeAfter >= timeBefore) {
				timeTaken =  timeAfter - timeBefore;
			} else {
				timeTaken = timeBefore - timeAfter;
			}
	timeS =  timeTaken / CLOCKSPEED_HZ;
	timeMs= timeS * 1000.0f;
	 /*CMSIS DSP API*/
	timeBefore = SysTick->VAL;
	 arm_conv_f32((float32_t  *)inputSignal_f32_1kHz_15kHz,
							(uint32_t)   KHZ_15_SIG_LEN,
							(float32_t  *)impulse_response,
							(uint32_t)  IMP_RSP_LEN,
							(float32_t  *)outputSigArr);
	timeAfter = SysTick->VAL;
	if (timeAfter >= timeBefore) {
			timeTaken =  timeAfter - timeBefore;
		} else {
			timeTaken = timeBefore - timeAfter;
		}
	timeS_CMSIS =  timeTaken / CLOCKSPEED_HZ;
	timeMs_CMSIS= timeS_CMSIS * 1000.0f;


	uint32_t outputSigLength = KHZ_15_SIG_LEN+ IMP_RSP_LEN -1;

	timeBefore = SysTick->VAL;
	//compute signal DFT
	calcSigDFT((float32_t  *)_640_points_ecg_,
			 (float32_t  *)reX,
			 (float32_t  *)imX,
			 (uint32_t)   ECG_SIG_LEN);


	//computer signal IDFT
	 calcSigIDFT((float32_t  *)outputSigArrECG,
			 (float32_t  *)reX,
			 (float32_t  *)imX,
			 (uint32_t)   ECG_SIG_LEN);


	 timeAfter = SysTick->VAL;
	 		if (timeAfter >= timeBefore) {
	 				timeTaken =  timeAfter - timeBefore;
	 			} else {
	 				timeTaken = timeBefore - timeAfter;
	 			}
	 	timeS =  timeTaken / CLOCKSPEED_HZ;
	 	timeMs= timeS * 1000.0f;
	 	serialPlotDFTIDFT((float32_t  *)_640_points_ecg_,(uint32_t)   ECG_SIG_LEN,
	 	 				(float32_t  *)outputSigArrECG,(uint32_t)   ECG_SIG_LEN);

	 	//init fft handler
	 	arm_rfft_fast_init_f32(&fftHandler, FFTLENGTH2);
	 	 /*CMSIS DSP API*/
	 	timeBefore = SysTick->VAL;

	 	arm_rfft_fast_f32	(	&fftHandler,
	 	(float32_t * )	_640_points_ecg_,
	 	(float32_t * )	outputSigArrDFT,
	 	0
	 	);

	 	arm_rfft_fast_f32	(	&fftHandler,
	 		 	(float32_t * )	outputSigArrDFT,
	 		 	(float32_t * )	outputSigArrECG,
	 	1
	 	);
	 timeAfter = SysTick->VAL;
	 	if (timeAfter >= timeBefore) {
	 			timeTaken =  timeAfter - timeBefore;
	 		} else {
	 			timeTaken = timeBefore - timeAfter;
	 		}
	 	timeS_CMSIS =  timeTaken / CLOCKSPEED_HZ;
	 	timeMs_CMSIS= timeS_CMSIS * 1000.0f;
	//getDftOpMagnitude((float32_t  *)reX,(uint32_t)   ECG_SIG_LEN);
	//serialPlotReX((float32_t  *)reX,(uint32_t)   ECG_SIG_LEN);

	 	/* arm_conv_f32((float32_t  *)IpSig_10hz_100hz_500hz,
	 								(uint32_t)   HZ_10_100_500HZ_SIGLEN,
	 								(float32_t  *)Hpf_400hz_impulseResponse,
	 								(uint32_t)  HPF_400HZ_IMP_RESP_LEN,
	 								(float32_t  *)outputSigArr);
	 		//compute signal DFT
	 		calcSigDFT((float32_t  *)IpSig_10hz_100hz_500hz,
	 				  (float32_t  *)reX2,
	 				  (float32_t  *)imX2,
	 				  (uint32_t)   HZ_10_100_500HZ_SIGLEN);

	 		getDftOpMagnitude((float32_t  *)reX2,(uint32_t) (HZ_10_100_500HZ_SIGLEN/2));
	 		//arm_cmplx_mag_f32(reX2, imX2, HZ_10_100_500HZ_SIGLEN/2);
	 		serialPlotReX((float32_t  *)reX2,(uint32_t) (HZ_10_100_500HZ_SIGLEN/2));*/
}

