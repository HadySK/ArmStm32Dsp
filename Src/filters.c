#include "filters.h"
#include <stdlib.h>

void firFilterInit(fir_filter * fir, float32_t *fltrKernel, uint32_t kernelLen){

	/*Reset buffer idx*/
	fir->buffIdx = 0;
	/*clear output*/
	fir->out = 0.00f;
	/*Set filter kernel*/
	fir->kernel = fltrKernel;
	/*set filter kernel length*/
	fir->kernelLen = kernelLen;

	/*Dynamically allocate buffer and initialize to zero*/
	/*returns void pointer to the allocated memory area so we type cast it to the buffer type(float32_t)*/
	/*malloc allocates a single block of uninitialized memory,
	 * returning a pointer with garbage values,
	 * while calloc allocates multiple blocks for an array
	 * and initializes all bytes to zero,
	 * returning a zeroed pointer.*/
	fir->buff= (float32_t*)calloc(fir->kernelLen, sizeof(float32_t));

}

float32_t firFilterUpdate(fir_filter * fir, float32_t currSample){
	uint32_t sumIdx =0u;
	/*store latest sample in the buffer*/
	fir->buff[fir->buffIdx] = currSample;
	/*increment buffer index, wrap around at the end*/
	fir->buffIdx++;
	if(fir->buffIdx >= (fir->kernelLen)){
		fir->buffIdx = 0;
	}

	/*perform Convolution */
	fir->out = 0.00f;

	sumIdx = fir->buffIdx;

	for(int i = 0 ; i < (fir->kernelLen); i++){
		if (sumIdx > 0 ){
			sumIdx--;
		}else{
			sumIdx= (fir->kernelLen)-1;
		}
		fir->out += fir->kernel[i] * fir->buff[sumIdx];
	}

	return fir->out;
}


