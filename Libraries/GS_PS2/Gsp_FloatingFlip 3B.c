
#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libpad.h>
#include <devvif1.h>
#include <devvu1.h>
#include <devvu0.h>
#include <devgif.h>
#include "Gsp.h"
u32 volatile InterruptLocker = 0;

#ifdef GSP_USE_TRIPLE_BUFFERRING

extern void GSP_ArtisanalFlushAll();

u32 ps2_ScreenMode = 0;

static volatile int ulT0OneFrame = 0;
volatile u_int gul_RealTimeCounter = 0;
u_int volatile VBlankCounter = 0;
u_int volatile VBlankOdd = 0;

u_int volatile GSPDMASEMAPHORE = 0;
u_int volatile TRIPLE_BIGSWAP = 0;
u_int TRIPLE_BIGSWAP_SOFT = 0;

u_int RealFBP2 = 0;

extern u_int gsWorldIsLoading;
extern u_int ENG_gp_DoubleRendering;
extern u_int ENG_gp_DoubleRenderingLocker;
void Gsp_FloatingFlip_START_DO(u_int MainCall);
extern Gsp_FlipFB_ASYNC();	

#ifndef _FINAL_
u32 ALLCOLOR[1024];
s32 ALLCOLOR_NB = 0;

void PUSH_COLOR(u32 Value)
{
	
	u32 COLOR;
	COLOR = ((Value>>0) & 3)<<6 ;
	COLOR |= ((Value>>2) & 3)<<(6 + 8)  ;
	COLOR |= ((Value>>4) & 3)<<(6 + 16)  ;
	ALLCOLOR[ALLCOLOR_NB ++] = COLOR;
	*GS_BGCOLOR = COLOR;
}
void POP_COLOR()
{
	if (ALLCOLOR_NB > 0)
	{
		
		ALLCOLOR_NB --;
		*GS_BGCOLOR = ALLCOLOR[ALLCOLOR_NB];
	}
	else
	*GS_BGCOLOR = 0;
}
#endif

volatile u_int gul_LastRealTimeCounter = 0;
//static volatile u_int gul_Trace = 0;
//volatile u_int ulHiddenFloatingFlipFlag = 0;
//int gul_CurrentFrameRate = 0;

float TheSpecialRasterFuck = 0.0f;


void Gsp_Begin_WaitDL()
{
}

void Gsp_End_WaitDL()
{
}
/*
void Gsp_FlipFB_Local()
{
	GSP_SetVideoCRTC(GSP_IsFrameODD() ^ 1 , 0);
}
*/


void GSP_FF_Init()
{
	*T0_MODE = (3 << 0) | (1 << 7) | (1 << 9);
}
static u32 ret0000;

#pragma	nofpregforblkmv	on


int Gsp_vblankHandler(int)
{
	Interrupt_Entry();
	VBlankCounter++;
	ulT0OneFrame = *T0_COUNT;
	gul_RealTimeCounter += *T0_COUNT;
	*T0_COUNT = 0;

	if (GspGlobal_ACCESS(ModePEnable))
	{
		VBlankOdd ^= 1;
	} else
	{
		VBlankOdd = 1;
		if ((*GS_CSR) & (1L << 13L)) VBlankOdd = 0;
	}
	if (TRIPLE_BIGSWAP)
	{
		// I can Switch ;
		GspGlobal_ACCESS(LastVBlankCounter) = VBlankCounter;
		if (!(TRIPLE_BIGSWAP & 2))
		{
			DPUT_GS_DISPLAY2(*(u_long *)&GspGlobal_ACCESS(gs_DE1).display); //DISPLAY2
			DPUT_GS_DISPLAY1( *(u_long *)&GspGlobal_ACCESS(gs_DE1).display1); //DISPLAY1*/
			DPUT_GS_DISPFB2(*(u_long *)&GspGlobal_ACCESS(gs_DE1).dispfb);  //DISPFB2
			DPUT_GS_DISPFB1(*(u_long *)&GspGlobal_ACCESS(gs_DE1).dispfb1);  //DISPFB1
			DPUT_GS_PMODE(*(u_long *)&GspGlobal_ACCESS(gs_DE1).pmode);   //PMODE
			DPUT_GS_SMODE2(*(u_long *)&GspGlobal_ACCESS(gs_DE1).smode2);  //SMODE2*/
		}
		else
		{
			DPUT_GS_DISPLAY2(*(u_long *)&GspGlobal_ACCESS(gs_DE2).display); //DISPLAY2
			DPUT_GS_DISPLAY1( *(u_long *)&GspGlobal_ACCESS(gs_DE2).display1); //DISPLAY1*/
			DPUT_GS_DISPFB2(*(u_long *)&GspGlobal_ACCESS(gs_DE2).dispfb);  //DISPFB2
			DPUT_GS_DISPFB1(*(u_long *)&GspGlobal_ACCESS(gs_DE2).dispfb1);  //DISPFB1
			DPUT_GS_PMODE(*(u_long *)&GspGlobal_ACCESS(gs_DE2).pmode);   //PMODE
			DPUT_GS_SMODE2(*(u_long *)&GspGlobal_ACCESS(gs_DE2).smode2);  //SMODE2*/
		}
		TRIPLE_BIGSWAP = 0; 
   	}
	if (GspGlobal_ACCESS(VBlankHook))  
		GspGlobal_ACCESS(VBlankHook)();
	ExitHandler();
	Interrupt_Exit();	

   	return 0;
}
#pragma	nofpregforblkmv	reset

#ifdef GSP_PS2_BENCH
extern u_int bTakeSnapshot;
extern u_int bForce60;
#endif
#define GSP_FF_LocalDT_History_Power_Of_2 4
void Gsp_FF_EndWorldLoad()
{
	GSP_ArtisanalFlushAll();
	gul_LastRealTimeCounter = gul_RealTimeCounter - *T0_COUNT;
}
extern void Gsp_ImpulseBX(Gsp_BigStruct	*p_BIG);
void Gsp_FloatingFlip_START_DO(u_int MainCall)
{
	static u_int LocalDT;
	u_int Infinite;
	float YOverX;
	switch (ps2_ScreenMode)
	{
	case 0:// 4/3 bb
		GspGlobal_ACCESS(YCorrectionCoef) = 4.0f/3.0f;
		break;
	case 1: // 4/3 
		GspGlobal_ACCESS(YCorrectionCoef) = 4.0f/3.0f;
		break;
	case 2: // 16/9
		GspGlobal_ACCESS(YCorrectionCoef) = 16.0f/9.0f;
		break;
	}
	GspGlobal_ACCESS(Rendering2D) = 0;

	
	LocalDT = (gul_RealTimeCounter + *T0_COUNT) - gul_LastRealTimeCounter;
	gul_LastRealTimeCounter += LocalDT;
	
	/* Use this result for next engine loop (F+1)*/	
	*T1_MODE &= ~(1<<7);
	if (ENG_gp_DoubleRenderingLocker)
		*T1_COUNT = ulT0OneFrame * LocalDT;
	else
		*T1_COUNT = (ENG_gp_DoubleRendering + 1) * LocalDT;
	
	TheSpecialRasterFuck = 0.25f * (float)LocalDT / (float)ulT0OneFrame;
	if (TheSpecialRasterFuck > 4.0f) TheSpecialRasterFuck = 4.0f;
	
	GspGlobal_ACCESS(fCurrentFrquency) = 2;
	
	GspGlobal_ACCESS(ulFade_Video_Enable) = GspGlobal_ACCESS(ulFade_Counter);
	GspGlobal_ACCESS(FrameCounter) ++;
	Infinite = 10000000;
	while (GSPDMASEMAPHORE && Infinite) 
	{
		Gsp_ImpulseBX(&gs_st_Globals);
		Infinite--;
	}; // Wait the finish signal
	while (TRIPLE_BIGSWAP && Infinite) 
	{
		Gsp_ImpulseBX(&gs_st_Globals);
		Infinite--;
	}; // Wait the finish signal
	if (!Infinite) 
		GSP_ArtisanalFlushAll();
	GSP_ManageGeoCache();
	Gsp_FlipFB_ASYNC();
	Gsp_FIFO_STOP();
	if (gsWorldIsLoading)
	{
		GSP_ArtisanalFlushAll();
	};
	//GSP_ArtisanalFlushAll();
}
void Gsp_FloatingFlip_START()
{
	Gsp_FloatingFlip_START_DO(1);
}
/* This folowing function is called from an interrup (VBLANK) !! */
/* VBlankCounter HAS BEEN INCREMENTED */
/* 
	Nothing to do in 3B !! .
*/
int Gsp_SyncVB(u_int VBNumber)
{
	return VBlankOdd;
}



void GSP_FlushAllFloatingFlip()
{
	u_int Infinite;
	Infinite = 10000000;
	while (GSPDMASEMAPHORE && Infinite) 
	{
		Gsp_ImpulseBX(&gs_st_Globals);
		Infinite--;
	}; // Wait the finish signal
	while (TRIPLE_BIGSWAP && Infinite) 
	{
		Gsp_ImpulseBX(&gs_st_Globals);
		Infinite--;
	}; // Wait the finish signal
	if (!Infinite) 
		GSP_ArtisanalFlushAll();
}
#endif //GSP_USE_TRIPLE_BUFFERRING