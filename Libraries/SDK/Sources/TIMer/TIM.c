/*$T TIM.c GC!1.67 01/03/00 12:33:19 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "TIMdefs.h"
#include "PROfiler/PRO.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "MATHs/MATH.h"

#if defined(PSX2_TARGET) && (__cplusplus)
extern "C" {
#endif

extern float TIM_gf_MainClockReal;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TIM_InitModule(void)
{
#ifndef PSX2_TARGET

#ifdef _GAMECUBE
/* 
 * ##########################################
 * ###         GAMECUBE            		  ###
 * ##########################################
 *
 */

#else

#if defined(_XBOX) || defined(_XENON)
	/* Quick Compute CPU ticks per second */
    TIM_gul_TicksPerSecond = TIM_ul_QuickGetTicksPerSecond();

	/* Reads the system clock at the start of the application */
	TIM_Clock_Reset();

    /* Initialises the main clock */
    TIM_gf_MainClock = Cf_Zero;

	/* Init the time of the engine loop start/end*/
	TIM_gf_EngineLoopEndTime = TIM_gf_EngineLoopStartTime = TIM_f_Clock_TrueRead();

#else // _XBOX

#ifdef ACTIVE_EDITORS
	PRO_InitModule();
#endif
	/* Quick Compute CPU ticks per second */
    TIM_gul_TicksPerSecond = TIM_ul_QuickGetTicksPerSecond();

	/* Reads the system clock at the start of the application */
	TIM_gul_SystemStartTime = L_timeGetTime();

	/* Initialises the main counter */
    TIM_GetTimerInternalCounter(&TIM_gul_StartCounter[0]);

    /* Initialises the main clock */
    TIM_gf_MainClock = TIM_gf_MainClockReal = Cf_Zero;

    /* Init the time spent in the editors */
    TIM_gf_EditorTime = Cf_Zero;

	/* Init the time of the engine loop start/end*/
	TIM_gf_EngineLoopStartTime=TIM_f_Clock_TrueRead();
	TIM_gf_EngineLoopEndTime=TIM_gf_EngineLoopStartTime;
	
#endif // _XBOX
#endif /* _GAMECUBE */
#else
/* 
 * ##########################################
 * ###             PSX2_TARGET            ###
 * ##########################################
 *
 */
	/* Quick Compute CPU ticks per second */
    TIM_gul_TicksPerSecond = TIM_ul_QuickGetTicksPerSecond();

	/* Initialises synchronisation frequency */
	TIM_guc_SynchroFrequencyIndex=2;
/*	TIM_gf_SynchroFrequencyTable[0]=50.0f;
	TIM_gf_SynchroFrequencyTable[1]=60.0f;
	TIM_gf_SynchroFrequencyTable[2]=75.0f;
	TIM_gf_SynchroFrequencyTable[3]=85.0f;
	TIM_gf_SynchroFrequencyTable[4]=100.0f;
	TIM_SetSynchroFrequencyValue(TIM_gf_SynchroFrequencyTable[TIM_guc_SynchroFrequencyIndex]);//*/

    /* Initialises the main clock */
    TIM_gf_MainClock = TIM_gf_MainClockReal = Cf_Zero;
	TIM_gul_TicksCount = 0;

	/* Init the time of the engine loop start/end*/
	TIM_gf_EngineLoopStartTime=TIM_f_Clock_TrueRead();
	TIM_gf_EngineLoopEndTime=TIM_gf_EngineLoopStartTime;
#endif
}

#if defined(PSX2_TARGET) && (__cplusplus)
}
#endif
