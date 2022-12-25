#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libpad.h>
#include "Gsp.h"

extern void Gsp_AE_ScreenSaver(u_int TimeTo0 , u_int Mode) ;

#define SS_Lenght 		128
#define SS_StackSize 	2048

static u_int SS_ThreadStack[SS_StackSize] __attribute__((aligned(16)));
static u_int SS_SemaphID;
static u_int SS_ThreadID;
static u_int SS_MainThreadID;
static volatile u_int SS_SCCountDown;

void GSP_SS_ExecuteSS()
{
//	if (sceGsSyncPath(1, 0)) return;
	if (gsulSyncroPath & (FLUSH_DMA2|FLUSH_DMA1|GIF_LOCKED)) return;
	if (GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs) return;
	Gsp_AE_ScreenSaver(SS_SCCountDown,1);
};


void GSP_ThreadEntry()
{
	static u_int Inc = 0;
	while (1)
	{
		WaitSema(SS_SemaphID);
		if (SS_SCCountDown) SS_SCCountDown --;
		GSP_SS_ExecuteSS();
		ResumeThread(SS_MainThreadID);		
	}
}



void GSP_SS_Init()
{
	struct SemaParam sema;
	struct ThreadParam param;
	
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
#ifdef GSP_PS2_BENCH
static u_int SS_Enable = 1;
#endif
extern u_int volatile VBlankOdd;
extern void	Gsp_ClearBorderExtend();
extern int BarrePos;
void GSP_InitSS()
{
	if (!GspGlobal_ACCESS(ulFade_Counter))
	{
		Gsp_AE_FADE_PUSH(1);
		Gsp_AE_FADE();
		Gsp_ClearBorderExtend();
		GspGlobal_ACCESS(ulFade_Alpha) = 1.0f;
		Gsp_SyncVB(0);
		Gsp_FlipFB();
		GSP_SS_ExecuteSS();
		Gsp_AE_FADE();
		GspGlobal_ACCESS(ulFade_Alpha) = 1.0f;
		Gsp_ClearBorderExtend();
		Gsp_SyncVB(0);
		Gsp_FlipFB();
		GSP_SS_ExecuteSS();
		GspGlobal_ACCESS(ulFade_Alpha) = 1.0f;
		BarrePos = 0;
	}
#ifdef GSP_PS2_BENCH
	if (!SS_Enable) return;
#endif	
	SS_SCCountDown = SS_Lenght;
	*GS_BGCOLOR = 0;
	Gsp_Flush(FLUSH_ALL);
}
void GSP_SS_StartScreenSaver()
{
	if (SS_SCCountDown) return;
	GSP_InitSS();
#ifndef _DEBUG0	
	GspGlobal_ACCESS(ScreenSaver) = GSP_SS_GO;
#endif
}
void GSP_SS_StopScreenSaver()
{
	SS_SCCountDown = 0;
	GspGlobal_ACCESS(ScreenSaver) = NULL;
}

#ifdef GSP_PS2_BENCH
void GSP_SS_TestScreenSaver()
{
	SS_Enable = 1;
	scePcStop();
	GSP_SS_StartScreenSaver();
	while (SS_SCCountDown)
	{
		Gsp_SyncVB(0);
	};
	GSP_SS_StopScreenSaver();
}
#endif


/* This folowing function is called from a interrup!! */
void GSP_SS_GO()
{
	if (SS_SCCountDown)
	{
		iSignalSema(SS_SemaphID);
		iSuspendThread(SS_MainThreadID);
	} else GSP_SS_StopScreenSaver();
}


