/*
	GPU monitor
	Used for calculating the time spent in the XBOX GPU

	09/07/03	Tiziano Sardone, Ubi Studios Milan
*/
#include "Gx8GPUMon.h"
#include <d3d8.h>
#include "Sdk\Sources\TIMer\PROfiler\XBox\XbUtil.h"
#include <d3dx8.h>

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//#define	ENABLE_BACKBUFFER_SCALING	//Enable this top use the backbuffer scaling feature
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "Gx8\Gx8renderstate.h"
float	gGPUStartTime;
float	gGPUStopTime;
float	gFrameStartTime;
float	gFrameStopTime;
float	gGPUFinalTime;
float	gFrameFinalTime;

float GPUProcessTime(void)
{
	return gGPUStopTime -gGPUStartTime;
}


void Gx8_StartGPUMon(void)
{
	gGPUStartTime = XBUtil_Timer( TIMER_GETABSOLUTETIME );
}

/* Stop GPU timer and save the result in gGPUFinalTime.
	Actually uses as the wait for the swap of the backbuffer to the front
*/
void Gx8_StopGPUMon(void)
{
	gGPUStopTime = XBUtil_Timer( TIMER_GETABSOLUTETIME );
	gGPUFinalTime += gGPUStopTime - gGPUStartTime;

}

/* Start profiling the timer for the actual frame */
void Gx8_StartFrameMon(void)
{
	gFrameStartTime = XBUtil_Timer( TIMER_GETABSOLUTETIME );
}

/* Stop GPU timer and save the result in gGPUFinalTime */
void Gx8_StopFrameMon(void)
{
	gFrameStopTime = XBUtil_Timer( TIMER_GETABSOLUTETIME );
	gFrameFinalTime += gFrameStopTime -gFrameStartTime ;
}

/*Reset the Frame timer */
void Gx8_ResetFrameMon(void)
{
	gFrameFinalTime=0.0f;
}

/*Reset the GPU timer */
void Gx8_ResetGPUMon(void)
{
	gGPUFinalTime=0.0f;
}

/* get the last wait-time for the GPU */
float Gx8_GetGPUTiming(void)
{
	return gGPUFinalTime;
}

/* get the last computed frame time */
float Gx8_GetFrameTiming(void)
{
	return gFrameFinalTime;
}

/* Here ìs the the function that perform backbuffer screen adaptation based on framerate
	Function is very simple: it take the TrameTime (time spent rendering ONE whole frame
	of the game) and the SwapWaitTime (the time for swapping backbuffer to front).
	If the game run under 60fps and the wait time is more than 0.001f (in a normal 
	situation the swap time is very small) the backbuffer is scaled.
	This test is dove every GPUMON_SAMPLED_FRAMES and averaged. In a triple-buffer
	display 3 is enought.
	NOTE: scaling is done only in X direction.
*/
void Gx8_AdaptToFramerate(float OneTrameTime, float SwapWaitTime)
{
#if defined(ENABLE_BACKBUFFER_SCALING)
	static float ScaleX = 1.0f;
	static float ScaleY = 1.0f;
	static float DegradeImageLevel = 0.0f;
	static float TimersArrayCPU[GPUMON_SAMPLED_FRAMES];	//Array for OneTrameTime
	static float TimersArrayGPU[GPUMON_SAMPLED_FRAMES];	//Array for SwapWaitTime
	static int TimerArrayPos = 0;

	//Store the timings
	TimersArrayCPU[TimerArrayPos] = OneTrameTime;
	TimersArrayGPU[TimerArrayPos] = SwapWaitTime;

	TimerArrayPos++;
	if( TimerArrayPos == GPUMON_SAMPLED_FRAMES)
	{
		//average last GPUMON_SAMPLED_FRAMES timers
		float ResultFrameTime = 0;
		float ResultGPUTime = 0;
		int i;

		//Average frame timing
		for(i=0; i<GPUMON_SAMPLED_FRAMES; i++)
			ResultFrameTime += TimersArrayCPU[i];
		ResultFrameTime /= (float)GPUMON_SAMPLED_FRAMES;

		//average GPu wait due to SWAP
		for(i=0; i<GPUMON_SAMPLED_FRAMES; i++)
			ResultGPUTime += TimersArrayGPU[i];
		ResultGPUTime /= (float)GPUMON_SAMPLED_FRAMES;

/*		char str0[233];
		sprintf(str0,"FrameTime: %f, GPUTime: %f\n",ResultFrameTime,ResultGPUTime);
		OutputDebugString(str0);*/
		//Check if we're in the ranges...
		if( ResultFrameTime>=0.03f &&
			ResultGPUTime>=0.005f	)
		{
			//we're GPU bound... decrease backbuffer size
			DegradeImageLevel = GPUMON_DEGRADE_DECREMENT;
		}
		else
		{
			//we're no more GPU bound... increase to reach original dimension
			DegradeImageLevel = GPUMON_DEGRADE_INCREMENT;
		}

		TimerArrayPos=0;


	}
 	ScaleX += DegradeImageLevel;

	//Check if ScaleX don't exeed maximun values
	if(ScaleX>1.0f)
		ScaleX=1.0f;

	if(ScaleX<GPUMON_MIN_REFERENCE)
		ScaleX=GPUMON_MIN_REFERENCE;


	//call the Scaling of backbuffer
	Gx8_ScaleBackBuffer(ScaleX,ScaleY);
#else
	//Feature not requested... set it to normal!
	Gx8_ScaleBackBuffer(1.0f,1.0f);
#endif
}