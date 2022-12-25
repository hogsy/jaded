#pragma once

#if defined( __cplusplus )
extern "C"
{
#endif

#include "xtl.h"

#define	GPUMON_SAMPLED_FRAMES	3				//Max numebr of samples
#define	GPUMON_MIN_REFERENCE	0.6f			//Reference value. The min scale value
#define	GPUMON_DEGRADE_DECREMENT	-0.025f		//Decrement factor
#define	GPUMON_DEGRADE_INCREMENT	0.025f		//Increment factor

void Gx8_StartGPUMon(void);		//Start the timer
void Gx8_StopGPUMon(void);		//Stop and calc the wait on swap
float Gx8_GetGPUTiming(void);	//Get the wait on swap
void Gx8_StartFrameMon(void);	//Start timing for frame
void Gx8_StopFrameMon(void);	//Calc frame timing
void Gx8_ResetFrameMon(void);	//reset the timer
void Gx8_ResetGPUMon(void);		//reset the timer
float Gx8_GetFrameTiming(void);	//Get frame timing
void Gx8_AdaptToFramerate(float OneTrameTime, float SwapWait);	//Calculate and scale the backbuffer


#if defined( __cplusplus )
}
#endif
