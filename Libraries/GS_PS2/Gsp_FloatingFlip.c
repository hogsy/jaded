#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libpad.h>
#include <devvif1.h>
#include <devvu1.h>
#include <devvu0.h>
#include <devgif.h>
#include "Gsp.h"

static u_int GSP_SignalCameraCutA;
extern void GSP_DestroyAllBackCaches(void);
extern void GEO_DoUncache(void);

void GSP_ArtisanalFlushAll();
#ifndef GSP_USE_TRIPLE_BUFFERRING


#define GSP_WAIT_VSYNC
#define SS_StackSize 	20480

u32 ps2_ScreenMode = 0;

static volatile int ulT0OneFrame = 0;
volatile u_int gul_RealTimeCounter = 0;
u_int volatile VBlankCounter = 0;
u_int volatile VBlankOdd = 0;
volatile u_int ulHiddenFloatingFlipFlag = 0;




static u_int SS_ThreadStack[SS_StackSize] __attribute__((aligned(16)));
static u_int SS_SemaphID;
static u_int SS_ThreadID;

u_int SS_MainThreadID;
extern u_int gsWorldIsLoading;
extern u_int ENG_gp_DoubleRendering;
extern u_int ENG_gp_DoubleRenderingLocker;
void Gsp_FloatingFlip_START_DO(u_int MainCall);
extern Gsp_FlipFB_ASYNC();	
extern u_int *RamZBuffer;


static volatile u_int gul_NextFrameNumber = 0;
volatile u_int gul_LastRealTimeCounter = 0;
volatile u_int gul_LostRealTimeCounter = 0;
//static volatile u_int gul_Trace = 0;
//volatile u_int ulHiddenFloatingFlipFlag = 0;
int gul_CurrentFrameRate = 0;
static int gul_CurrentFrameRateDisplay = 0;
static volatile int gl_CurrentFrameRateDelta = 0;
static u_int gul_LastVBLankCounter = 0;
static u_int gl_Locker = 0;
#define SWAP_DOUBLE_TIME();

static u_int LostBase;
static u_int TimHandle = 0;

void Gsp_Begin_WaitDL()
{
	if (!TimHandle)	
	{
		LostBase = gul_RealTimeCounter + *T0_COUNT;
	}
	TimHandle++;
}

void Gsp_End_WaitDL()
{
	TimHandle--;
	if (!TimHandle)
	{
		gul_LostRealTimeCounter += (gul_RealTimeCounter + *T0_COUNT) - LostBase;
	}
}

void GSP_IncrementFrameRate(int Value)
{
	gl_CurrentFrameRateDelta += Value;
}
void Gsp_FlipFB_Local()
{
	GSP_SetVideoCRTC(GSP_IsFrameODD() ^ 1 , 0);
	Gsp_DrawBuf_JustAfterFlip();
	if (GSP_SignalCameraCutA) GSP_SignalCameraCutA--;
}



void GSP_ThreadEntry()
{
	while (1)
	{
		WaitSema(SS_SemaphID);
		
		GSP_ArtisanalFlushAll();
	    	
		Gsp_FlipFB_Local();
		
		ResumeThread(SS_MainThreadID);		
	}
}

void GSP_FF_Init()
{
	struct SemaParam sema;
	struct ThreadParam param;

	*T0_MODE = (3 << 0) | (1 << 7) | (1 << 9);

	SS_MainThreadID = GetThreadId();
	sema.initCount 		= 0;
	sema.maxCount 		= 1;
	sema.numWaitThreads = 1;
	SS_SemaphID 		= CreateSema(&sema);
	
	param.entry 		= GSP_ThreadEntry;
	param.stack 		= SS_ThreadStack;
	param.stackSize 	= SS_StackSize * 4;
	param.gpReg 		= &_gp;
	param.initPriority 	= 10;
	SS_ThreadID 		= CreateThread(&param);
	StartThread(SS_ThreadID, NULL);
}
int Gsp_vblankHandler(int)
{
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
	
	if (GspGlobal_ACCESS(Status) & GSP_Status_SVS)
	{
		if (GSP_SignalCameraCutA) *GS_BGCOLOR = 0xffffff;
		else
		{
			if ((gul_CurrentFrameRate >> 8) > 1)
				*GS_BGCOLOR = 0xFF << ((gul_CurrentFrameRate >> 8) << 3);
			else
				*GS_BGCOLOR = 0x30 << ((gul_CurrentFrameRate >> 8) << 3);
		} 
	} else *GS_BGCOLOR = 0;
	
   	if (GspGlobal_ACCESS(VBlankHook)) 
   		GspGlobal_ACCESS(VBlankHook)();
   	
   	ExitHandler();
   	return 0;
}


#ifdef GSP_PS2_BENCH
extern u_int bTakeSnapshot;
extern u_int bForce60;
#endif
float TheSpecialRasterFuck = 0.0f;
#define GSP_FF_LocalDT_History_Power_Of_2 4
void Gsp_FF_EndWorldLoad()
{
	gul_CurrentFrameRateDisplay = gul_CurrentFrameRate = 0x180;
	gul_NextFrameNumber = VBlankCounter + 3;
	gul_LostRealTimeCounter = 0;
	gl_Locker = 1 << GSP_FF_LocalDT_History_Power_Of_2;
	gul_LastRealTimeCounter = gul_RealTimeCounter - *T0_COUNT;
}

void Gsp_FloatingFlip_START_DO(u_int MainCall)
{
	static u_int LocalDT;
	static u_int LocalFrameCounter = 0;
	u_int Render_I;
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
	GspGlobal_ACCESS(VBlankHook) = NULL;
	if (GspGlobal_ACCESS(ulFloatingFlipFlag)) Gsp_SyncVB(0);
	
//	GSP_ArtisanalFlushAll();
/*	
	GSP_ArtisanalFlushAll();
	GSP_RestoreCorrectPath(0);
//*/		
	if (MainCall)
	{
		LocalFrameCounter++;
		if ((LocalFrameCounter & ((1 << GSP_FF_LocalDT_History_Power_Of_2) - 1)) == 0)
		{
			LocalDT = (gul_RealTimeCounter + *T0_COUNT) - gul_LastRealTimeCounter;
			gul_LastRealTimeCounter = gul_RealTimeCounter + *T0_COUNT;
			LocalDT -= gul_LostRealTimeCounter;
			LocalDT >>= GSP_FF_LocalDT_History_Power_Of_2;
			gul_LostRealTimeCounter = 0;
			
			TheSpecialRasterFuck = 0.25f * (float)LocalDT / (float)ulT0OneFrame;
			if (TheSpecialRasterFuck > 4.0f) TheSpecialRasterFuck = 4.0f;
		}
	}
	Gsp_Begin_WaitDL();

	while (ulHiddenFloatingFlipFlag ) {}; // two frame of advance maximum 

	Gsp_End_WaitDL();
	
	
	if (GspGlobal_ACCESS(ulFloatingFlipFlag)) 
	{
		ulHiddenFloatingFlipFlag++;
		if (!(GSP_IsFrameODD()))
		{ // Select buffer 2 
			GspGlobal_ACCESS(CUR_REG_FRAME) = /*FBP*/((unsigned long)GspGlobal_ACCESS(FBP2) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/(0L << 32L);
		} else 
		{ /* Select Buffer 1 */
			GspGlobal_ACCESS(CUR_REG_FRAME) = /*FBP*/((unsigned long)GspGlobal_ACCESS(FBP1) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/(0L << 32L);
		}
		GspGlobal_ACCESS(FrameCounter)++;
		return;
	}


	GspGlobal_ACCESS(ulFade_Video_Enable) = GspGlobal_ACCESS(ulFade_Counter);
	Render_I = 0;
	if (GDI_gpst_CurDD)
		Render_I = GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I;
	/* previous frame DT (F-1) */
	if (!(gl_Locker || Render_I))
	{
		if ((gl_CurrentFrameRateDelta > 0) || (LocalDT > (ulT0OneFrame  * ((gul_CurrentFrameRate >> 8) + 1))))
		{
			gul_CurrentFrameRate += 0x100;
			gul_CurrentFrameRate |= 0x0ff;
			gl_Locker = 1 << GSP_FF_LocalDT_History_Power_Of_2;
		} else
		if ((gl_CurrentFrameRateDelta < 0) && (LocalDT < (ulT0OneFrame  * ((gul_CurrentFrameRate >> 8) + 0))))
		{
			gul_CurrentFrameRate -= ((gul_CurrentFrameRate >> 8) + 1) << GSP_FF_LocalDT_History_Power_Of_2;
		} else
		{
			gul_CurrentFrameRate |= 0x0ff;
		}
	} else
	if (gl_Locker)
			gl_Locker --;
	gl_CurrentFrameRateDelta = 0;
	
	SWAP_DOUBLE_TIME();
	
	gul_CurrentFrameRate = lMax(gul_CurrentFrameRate , 0x000);
	gul_CurrentFrameRate = lMin(gul_CurrentFrameRate , 0x3FF);
#ifdef GSP_PS2_BENCH
	if (bTakeSnapshot | bForce60) gul_CurrentFrameRate = 0;
#endif
	

	GspGlobal_ACCESS(FrameCounter) ++;
	
	
	
	/* Use this result for next engine loop (F+1)*/	
	*T1_MODE &= ~(1<<7);
	if (ENG_gp_DoubleRenderingLocker)
		*T1_COUNT = ulT0OneFrame * ((gul_CurrentFrameRate >> 8) + 1);
	else
		*T1_COUNT = (ENG_gp_DoubleRendering + 1) * ulT0OneFrame * ((gul_CurrentFrameRate >> 8) + 1);
	
	gul_NextFrameNumber += (gul_CurrentFrameRateDisplay >> 8) + 1;
	gul_NextFrameNumber = max(gul_NextFrameNumber , VBlankCounter + 0);
	gul_NextFrameNumber = min(gul_NextFrameNumber , VBlankCounter + 5);
	
	gul_CurrentFrameRateDisplay = gul_CurrentFrameRate;
	
	GspGlobal_ACCESS(fCurrentFrquency) = (gul_CurrentFrameRateDisplay >> 8) + 1;
	if (gsWorldIsLoading)
	{
		GspGlobal_ACCESS(ulFloatingFlipFlag) |= 1; /* Is activated */
		Gsp_FlipFB_ASYNC();
		Gsp_SyncVB(0); // Engine is late
	} else
	{
		GspGlobal_ACCESS(ulFloatingFlipFlag) |= 1; /* Is activated */
		Gsp_FlipFB_ASYNC();	
#ifdef GSP_WAIT_VSYNC

		if (gul_NextFrameNumber <= VBlankCounter ) 
		{
			Gsp_SyncVB(0); // Engine is late
		}
		else
			GspGlobal_ACCESS(VBlankHook) = GSP_FF_GO;//*/
#else		
		GspGlobal_ACCESS(ulFloatingFlipFlag) |= 1; /* Is activated */
		GspGlobal_ACCESS(VBlankHook) = GSP_FF_GO;
#endif
	}
}
void Gsp_FloatingFlip_START()
{
	Gsp_FloatingFlip_START_DO(1);
}
/* This folowing function is called from an interrup (VBLANK) !! */
/* VBlankCounter HAS BEEN INCREMENTED */
void GSP_FF_GO()
{
	/* Does engine ready ? */
	if (gul_NextFrameNumber > VBlankCounter )
	{
		if (!(gsulSyncroPath & (FLUSH_DMA1)))
		{
			// I must decrement the frame rate 
			// All is already finished
			GSP_IncrementFrameRate(-1);
			// I'm in advance and i'm ready => decrement
			// This is BUGGED if engine have >= 1 frames of advance => Corrected with local DT (see FloatingFlipStart)
		}
		// I'm in advance => return
		return;
	}
	
	if (gul_NextFrameNumber < VBlankCounter)
	{
		// I'm late 
		// I must Increment the frame rate 
		// Case cpu IS OUT
		GSP_IncrementFrameRate(1);
	} else
	if (gul_NextFrameNumber == VBlankCounter)
	{
		if (gsulSyncroPath & (FLUSH_DMA1))
		{
			// I'm late 
			// I must Increment the frame rate 
			// CASE FIFO is Out
			GSP_IncrementFrameRate(1);
		}
		// I Flip !!!
	}

			
	GspGlobal_ACCESS(ulFloatingFlipFlag) = 0; /* Is Desactivated */
	GspGlobal_ACCESS(VBlankHook) = NULL;
	iSuspendThread(SS_MainThreadID);
	iSignalSema(SS_SemaphID);//*/
}

/* 
	CPU Cannot advance anymore.
*/
int Gsp_SyncVB(u_int VBNumber)
{
	u_int ulMustFlip , Kx ;
	int LocalDelta;
	ulMustFlip = GspGlobal_ACCESS(ulFloatingFlipFlag);
	GspGlobal_ACCESS(VBlankHook) = NULL;
	GspGlobal_ACCESS(ulFloatingFlipFlag) = 0;
	
	Gsp_Begin_WaitDL();
	GSP_ArtisanalFlushAll();
	if (ulMustFlip)
	{
		gul_NextFrameNumber = max(gul_NextFrameNumber , VBlankCounter - 1);
		gul_NextFrameNumber = min(gul_NextFrameNumber , VBlankCounter + 5);
		if (gul_NextFrameNumber <= VBlankCounter)
		{
			// I'm late
			// I must Increment the frame rate 
			// Case cpu IS OUT
			GSP_IncrementFrameRate(1);
			
			// Flip without wait VSync!!
		} 
		else
		{
#ifdef GSP_WAIT_VSYNC
			Kx = VBlankCounter;
			while (Kx == VBlankCounter) {};
#endif		
			/* Does engine ready ? */
			if (gul_NextFrameNumber > VBlankCounter )
			{
				GSP_IncrementFrameRate(-1);
				// I'm in advance and i'm ready 
				//	=> decrement frame rate
				// 	=> wait for the good frame
#ifdef GSP_WAIT_VSYNC
				while (gul_NextFrameNumber > VBlankCounter) {};
#endif			
			} 
		}
	}
	Gsp_End_WaitDL();
	
	if (ulMustFlip)
	{
		Gsp_FlipFB_Local();
	}
	return VBlankOdd;
}



void GSP_FlushAllFloatingFlip()
{
	Gsp_Begin_WaitDL();
	GspGlobal_ACCESS(VBlankHook) = NULL;
	if (GspGlobal_ACCESS(ulFloatingFlipFlag)) Gsp_SyncVB(0);
	Gsp_End_WaitDL();
}
#endif


void GSP_SignalCameraCut()
{
	extern void GSP_FlushAll();
	if (!GSP_SignalCameraCutA) GSP_SignalCameraCutA = 4;
	GSP_FlushAll();
}

u_int GSP_Flush_VU1(register u_int Infinite)
{
	asm _volatile_ 
	{
		.set noreorder
vu_run:	bc2f vu_Idle
		addi Infinite,Infinite,-1
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		bne Infinite,$0,vu_run
		nop
vu_Idle:		
		.set reorder
	}
	return Infinite;
}
void GSP_RestoreCorrectPath(u_int Mode)
{
	u_int Local;
	u_int PALNTSC;

	/* ***************************************************************************************
	
		This is an antibug 
			 	Somtimes, (after playing average 3-4 hour), artisanal flush doesn't finish 
			and fail into this function (mode 0) or from GSP_drawbuf.c -> RenderBuffer (mode 1).
			
				Role of this is to COMPLETLY reset the drawing path... 
			
	*************************************************************************************** */
	/* 0-> Reset all the cache */
	
#ifndef _FINAL_
	extern void GSP_CheckBuffer(GSP_DMA_Source_Chain_TAG *);
	GSP_CheckBuffer((GSP_DMA_Source_Chain_TAG *)GspGlobal_ACCESS(GSP_BX_VAR).pCurrentDmaBuffer->p_BufferBase);
#endif	
	
	
	FlushCache(0);

	
	/* 1->Stop the 1 & 2 DMA's */

	Local = *D_ENABLER;
//	*D_ENABLEW = Local | 0x00010000;
	
	*D1_CHCR = (1/*From memory */ | (1<<2)/*chain Mode*/ | /*Start */ (0<<8));
	*D2_CHCR = (1/*From memory */ | (1<<2)/*chain Mode*/ | /*Start */ (0<<8));
//	*D_ENABLEW = Local;

	/* reset all possible things */
	
	sceDmaReset(0);
	sceDevVif1Reset();
	sceDevVu1Reset();
	sceDevGifReset();
	sceGsResetPath();
	
	if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
		PALNTSC = SCE_GS_PAL;
	else
		PALNTSC = SCE_GS_NTSC;
	
	switch (GspGlobal_ACCESS(VideoMode))
	{
		case GSP_VRES_xAUTO:
		case GSP_VRES_x1:
			sceGsResetGraph(0, SCE_GS_INTERLACE, PALNTSC, SCE_GS_FRAME);
			break;
		case GSP_VRES_x2:
			{
				if (GspGlobal_ACCESS(ModePEnable))
					sceGsResetGraph(0, SCE_GS_NOINTERLACE, PALNTSC, SCE_GS_FRAME);
				else
					sceGsResetGraph(0, SCE_GS_INTERLACE, PALNTSC, SCE_GS_FIELD);
			}
			break;
		case GSP_VRES_x2_TripleAA:
			sceGsResetGraph(0, SCE_GS_INTERLACE, PALNTSC, SCE_GS_FRAME);
			break;
	}


	sceDmaReset(1);
   	EnableDmac(DMAC_VIF1);
   	EnableDmac(DMAC_GIF);//*/
   	EnableDmac(DMAC_TO_SPR);//*/
   	EnableDmac(DMAC_FROM_SPR);//*/
	gsulSyncroPath = 0;
	
	if (GspGlobal_ACCESS(p_CurrentRegWriter) == &GspGlobal_ACCESS(stMRW1)) 
		GspGlobal_ACCESS(p_CurrentRegWriter) = &GspGlobal_ACCESS(stMRW2);
	else
		GspGlobal_ACCESS(p_CurrentRegWriter) = &GspGlobal_ACCESS(stMRW1);
	GSP_UnCachePtr(GspGlobal_ACCESS(p_CurrentRegWriter));
	GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs = 0;
		
	Gsp_LoadVU1Code();
	Gsp_Flush(FLUSH_ALL);
	
	/* reset rotative buffers */
	{
		register volatile GSP_BX	*p_Buffer;
		p_Buffer = GspGlobal_ACCESS(GSP_BX_VAR).pCurrentDmaBuffer; 
		while (p_Buffer->BX_State >= BX_USED_BY_DMA)
		{
			p_Buffer->BX_State = BX_READY_FOR_CPU;
			p_Buffer->BX_BackSize = 0;
			p_Buffer = p_Buffer->p_NextBuffer;
		}
		GspGlobal_ACCESS(GSP_BX_VAR).pCurrentDmaBuffer = p_Buffer;
	}
	

	FlushCache(0);
	

	
#ifdef GSP_USE_TRIPLE_BUFFERRING
	GSP_SetVideoCRTC(GSP_IsFrameODD() ^ 1 , 0);
#endif

	
	/* reset cached geometry */
/*	GSP_DestroyAllBackCaches();
	GSP_InitGeoCache(65536);
	GSP_InvalidateGeoCache(65536);
	GEO_DoUncache();*/

	gsulSyncroPath = 0;

}

void GSP_ArtisanalFlushAll()
{
	register u_int Infinite;
	extern void Gsp_ImpulseBX(Gsp_BigStruct	*p_BIG);
	extern void Gsp_ImpulseBX_ALL(Gsp_BigStruct	*p_BIG);
	
#ifdef GSP_USE_TRIPLE_BUFFERRING
	extern u_int volatile GSPDMASEMAPHORE;
	extern u_int volatile TRIPLE_BIGSWAP_SOFT;
	extern u_int volatile TRIPLE_BIGSWAP;
	Infinite = 1000000;
	while ((gs_st_Globals.GSP_BX_VAR.pCurrentDmaBuffer->BX_State == BX_READY_FOR_DMA)  && Infinite)
	{
		Infinite--;
		Gsp_ImpulseBX(&gs_st_Globals);
	}
	if (!Infinite) 
	{
		Infinite = 18;
		goto BAD_END;
	}
	Infinite = 100000000;
	while (GSPDMASEMAPHORE && Infinite) 
	{
		Infinite--;
		Gsp_ImpulseBX(&gs_st_Globals);
	}; // Wait the finish signal
	if (!Infinite) 
	{
		Infinite = 16;
		goto BAD_END;
	}
	Infinite = 100000000;
	while (TRIPLE_BIGSWAP && Infinite)
	{
		Gsp_ImpulseBX(&gs_st_Globals);
		Infinite--;
	}
	if (!Infinite) 
	{
		Infinite = 18;
		goto BAD_END;
	}
	
#endif
	/* flush DMA1 & DMA2 */
	Infinite = 100000000;
	while ((gsulSyncroPath & (FLUSH_DMA1|FLUSH_DMA2)) && Infinite)	
	{
		Infinite--;
	}; 
	if (!Infinite) 
	{
		Infinite = 0;
		goto BAD_END;
	}

	/* flush VIF1 */
	Infinite = 10000000;
	while (Infinite && (*VIF1_STAT & 3)) 
	{
		Infinite--;
	};
	if (!Infinite) 
	{
		Infinite = 1;
		goto BAD_END;
	}

	/* flush VU1  */
	Infinite = 10000000;
	Infinite = GSP_Flush_VU1(Infinite);
	if (!Infinite) 
	{
		Infinite = 2;
		goto BAD_END;
	}
	
	/* flush GIF */
	Infinite = 10000000;
	while (Infinite && (*GIF_STAT & (7 << 9))) 
	{
		Infinite--;
	};
	if (!Infinite) 
	{
		Infinite = 3;
		goto BAD_END;
	}

	/* flush GS  */
	Infinite = 10000000;
	DPUT_GS_CSR( GS_CSR_FINISH_M ); // Set to 1 for next used
	while (Infinite && ((*GS_CSR & GS_CSR_FLUSH_M) != 0)) 
	{
		Infinite--;
	};
	if (!Infinite) 
	{
		Infinite = 4;
		goto BAD_END;
	}

	gsulSyncroPath &= ~GIF_LOCKED;
	
	return;
	
BAD_END:

	GSP_RestoreCorrectPath(0);
	gsulSyncroPath &= ~GIF_LOCKED;
#ifdef GSP_USE_TRIPLE_BUFFERRING
	GSPDMASEMAPHORE = 0 ;
	TRIPLE_BIGSWAP = 0;
	TRIPLE_BIGSWAP_SOFT = 0;
#endif		
}


