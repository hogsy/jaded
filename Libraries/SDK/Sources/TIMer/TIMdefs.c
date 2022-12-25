/*$T TIMdefs.c GC! 1.081 09/26/02 10:13:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

/* This file contains all the function about timers. */
#ifdef PSX2_TARGET
#include <sys/types.h>
#include <eeregs.h>
#include "MATHfloat.h"
#include "PROdefs.h"
#include "ENGvars.h"
#include "PSX2struct.h"
#else /* ! PSX2_TARGET */
#include "TIMdefs.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/BAStypes.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "BASe/ERRors/ERRasser.h"
#include "MATHs/MATH.h"
#include "GDInterface/GDInterface.h"
#include "GRObject/GROrender.h"
#include "ENGine/Sources/ENGvars.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#endif /* ACTIVE_EDITOR */
#endif /* ! PSX2_TARGET */

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

extern ULONG INO_n_FrameCounter;

#ifdef PSX2_TARGET

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    PS2 only
 -----------------------------------------------------------------------------------------------------------------------
 */

#include <eekernel.h>

#define TIM_Cf_Timer0Freq	585937.5f				/* 150 MHz / 256 */
#define TIM_Cf_MaxDt		0.2666666666f			/* 1/3.75 second = 16 frames */
#define TIM_Cf_MinDt	    0.0166666666f		/* 1/60 second = 1 frame */

ULONG			TIM_gul_TicksCount;					/* ticks number since the beginning */
float			TIM_f_Timer1Frequency = 15625.0f;	/* timer 1 frequency, calculated at the beginning */
float			TIM_f_OoTimer1Frequency;			/* timer 1 frequency, calculated at the beginning */
volatile int	T1_Overflow = 0;
float			fGlobalAcc = 1.0f;
float			fOoGlobalAcc = 1.0f;
#else

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    win32 only
 -----------------------------------------------------------------------------------------------------------------------
 */

#define TIM_Cf_MaxDt	0.2666666666f		/* 1/3.75 second = 16 frames */
#define TIM_Cf_MinDt	0.0166666666f		/* 1/60 second = 1 frame */

char			TIM_gb_SynchroAdjust = 0;	/* Set to 1 to be in adjust mode */
ULONG			TIM_gul_StartCounter[2];	/* Counter at the start of the engine */
ULONG			TIM_gul_SystemStartTime;	/* System clock at the start of the application */

/*
 * Time spent in the editors (time when the engine is stopped, and time when
 * something is done in the editors)
 */
#if !defined(_XBOX) && !defined(_XENON)
float			TIM_gf_EditorTime;
#endif /* _XBOX */
#endif /* !PSX2_TARGET */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    all targets
 -----------------------------------------------------------------------------------------------------------------------
 */

ULONG	TIM_gul_TicksPerSecond = 1;				/* Number of CPU ticks in one second */
float	TIM_gf_SynchroFrequencyTable[5];	/* Table of 4 possible frequencies (50,60,75,85 Hz) */
UCHAR	TIM_guc_SynchroFrequencyIndex;		/* Current index in the tables of possible frequencies */
float	TIM_gf_SynchroFrequency = 60;		/* Current frequency used for synchronisation */
float	TIM_gf_SpeedFactor = 1.0f;

/*
 * Main Clock of the TIM module: it represents the time in seconds since the clock
 * has been initialized or reseted
 */
float	TIM_gf_MainClock = 0.0f;
float	TIM_gf_MainClockReal = 0.0f;

/*
 * Time between two calls of the function TIM_Clock_Update. If function
 * TIM_Clock_Update called each engine loop, it represents the time between two
 * engine frames
 */
float	TIM_gf_dt;
float	TIM_gf_realdt;
float	TIM_gf_EngineLoopStartTime;
float	TIM_gf_EngineLoopEndTime;
int		TIM_gi_LastNumFrame = 1;
int		TIM_gi_NumFrame = 1;
int		TIM_gi_CountNumFrame = 0;

/*$4
 ***********************************************************************************************************************
    GLOBAL FUNCTIONS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    PSX2
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef PSX2_TARGET
int			TIM_TIM1_Handler(int ca);
void		TIM_UpdateTimerFrequency(void);
extern BOOL ps2MAI_gb_VideoModeNTSC;
#define TIM_Cte_PAL_Freq	15625.0f
#define TIM_Cte_NTSC_Freq	15734.0f

/*$4
 ***********************************************************************************************************************
    ALARM for PS2 = real time clock
 ***********************************************************************************************************************
 */

#define ps2TIM_Cte_AlarmPeriod	10
volatile unsigned int	TIM_ui_AlarmTickCount;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2TIM_AlarmCallback(int id, unsigned short time, void *arg)
{
	TIM_ui_AlarmTickCount++;
	iSetAlarm(ps2TIM_Cte_AlarmPeriod, ps2TIM_AlarmCallback, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2TIM_InitAlarm(void)
{
	TIM_ui_AlarmTickCount = 0;
	SetAlarm(ps2TIM_Cte_AlarmPeriod, ps2TIM_AlarmCallback, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float ps2TIM_f_RealTimeClock(void)
{
	/*~~~~~~~~*/
	float	fff;
	/*~~~~~~~~*/

	fff = fLongToFloat(TIM_ui_AlarmTickCount);
	fff = fff * TIM_f_OoTimer1Frequency * (float) ps2TIM_Cte_AlarmPeriod;

	return fff;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TIM_TIM1_Handler(int ca)
{
	*T1_MODE |= (1 << 11);
	T1_Overflow++;
	ExitHandler();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TIM_ul_QuickGetTicksPerSecond(void)
{
	/* start timer1 */
	AddIntcHandler(INTC_TIM1, TIM_TIM1_Handler, 0);
	EnableIntc(INTC_TIM1);
	*T1_MODE = (3 << 0) | (1 << 7) | (1 << 9);

	TIM_UpdateTimerFrequency();
	ps2TIM_InitAlarm();

	/* processes the timer 1 frequency */
	return(lFloatToLong(TIM_f_Timer1Frequency));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TIM_UpdateTimerFrequency(void)
{
	if(ps2MAI_gb_VideoModeNTSC)
	{
		TIM_f_Timer1Frequency = TIM_Cte_NTSC_Freq;
	}
	else
	{
		TIM_f_Timer1Frequency = TIM_Cte_PAL_Freq;
	}

	TIM_f_OoTimer1Frequency = 1.0f / TIM_f_Timer1Frequency;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TIM_Clock_Update(void)
{
	/*~~~~~~~~~~~~~*/
	UINT	TmpTimer;
	/*~~~~~~~~~~~~~*/

	/* stores the timer 1 count */
	TmpTimer = *T1_COUNT;

	/* Updates the main clock (time in seconds since the start of the application) */
	TIM_gf_MainClockReal = TIM_f_Clock_TrueRead();
	T1_Overflow = 0;

	/* Updates the ticks count */
	TIM_gul_TicksCount += (ULONG) TmpTimer;

	/* resets the timer 1 due to main clock update */
	*T1_COUNT = 0;

	/* calculates dt */
	if(!TmpTimer) TmpTimer=250;
	TIM_gf_dt = fDiv(fLongToFloat(TmpTimer), TIM_f_Timer1Frequency * fGlobalAcc);

	/* Check that the dt is positive */
	ERR_X_Assert(TIM_gf_dt > 0.0f);

	/* If (TIM_gf_dt<TIM_Cf_MinDt) TIM_gf_dt=TIM_Cf_MinDt; */
	TIM_gf_dt = fSup(TIM_gf_dt, TIM_Cf_MaxDt) ? TIM_Cf_MaxDt : TIM_gf_dt;


	if(fGlobalAcc > 2000.0f) TIM_gf_dt = 1.0f / ((fGlobalAcc - 2000.0f) * 60.0f);

	TIM_gf_dt *= TIM_gf_SpeedFactor;
	TIM_gf_MainClock += TIM_gf_dt;

    if(MAI_gst_MainHandles.pst_DisplayData && (MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames)) 
        TIM_gf_dt = MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames * TIM_Cf_MinDt;

    if (INO_b_RecordInput || INO_b_PlayInput)    
        TIM_gf_MainClock = INO_n_FrameCounter * 0.0333333f;  
}

/*
 =======================================================================================================================
    Aim:    Reads the internal counter and returns the time in seconds since the start of the application
 =======================================================================================================================
 */
float TIM_f_Clock_TrueRead(void)
{
	/*~~~~~~~~~~~~~*/
	float	f_Result;
	/*~~~~~~~~~~~~~*/

	if(fGlobalAcc > 2000.0f)
		f_Result = TIM_gf_MainClockReal + 1.0f / ((fGlobalAcc - 2000.0f) * 60.0f);
	else
	{
		f_Result = TIM_gf_MainClockReal + fDiv(fLongToFloat(*T1_COUNT), TIM_f_Timer1Frequency * fGlobalAcc);
		f_Result += fLongToFloat(T1_Overflow) * fDiv(65534.0f, TIM_f_Timer1Frequency * fGlobalAcc);
	}

    if (INO_b_RecordInput || INO_b_PlayInput)    
        f_Result = INO_n_FrameCounter * 0.0333333f;

	return(f_Result);
}

/*
 =======================================================================================================================
    Aim:    Makes a "reset" of the main clock

    Note:   The number of seconds since the start (TIM_gf_MainClock) is set to zero, and the initial counter is set to
            zero too
 =======================================================================================================================
 */
void TIM_Clock_Reset(void)
{
	/*
	 * BECAREFULL, the timer1 is not running !!! £
	 * == === £
	 * it will start after the TIM_ul_QuickGetTicksPerSecond call £
	 * e.g. it will start after TIM_InitModule call £
	 * configures timer 1 with H-BLNK frequency
	 */
	*T1_COUNT = 0;

	/* Initialises the main clock */
	TIM_gf_MainClockReal = TIM_gf_MainClock = Cf_Zero;
	TIM_gul_TicksCount = 0;
}

/*$4
 ***********************************************************************************************************************
    GAME CUBE
 ***********************************************************************************************************************
 */

#elif defined(_GAMECUBE)
static u64	g_StartTime;
static u64	g_LastTime;

float		fGlobalAcc = 1.0f;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TIM_ul_QuickGetTicksPerSecond(void)
{
	return OS_TIMER_CLOCK;	/* 485000000.0f; */
}

/*
 =======================================================================================================================
    Aim:    Calculates the time in seconds since the start of the application. Calculates the time since the last call
            of this function £

    Note:   Stores the number of seconds since start in the global variable TIM_gf_MainClock £
            Stores the time since last call of this function in TIM_gf_dt
 =======================================================================================================================
 */
u64 gcTIM_gu64_currentTime=0; 
void TIM_Clock_Update(void)
{
	/*~~~~~~~~~~~~*/
	u64 currentTime;
	/*~~~~~~~~~~~~*/

	currentTime = gcTIM_gu64_currentTime ? gcTIM_gu64_currentTime :  OSTicksToMicroseconds(OSGetTime());

	/* Updates the main clock (time in seconds since the start of the application) */
	TIM_gf_MainClockReal = (currentTime - g_StartTime) / 1000000.0f;

	TIM_gf_dt = fGlobalAcc * (currentTime - g_LastTime) / 1000000.0f;

	TIM_gf_dt = fSup(TIM_gf_dt, TIM_Cf_MaxDt) ? TIM_Cf_MaxDt : TIM_gf_dt;
	
	if(gcTIM_gu64_currentTime)
	{
		TIM_gf_dt = (TIM_gf_dt < 0.01f) ? 0.01f : TIM_gf_dt;
	}

	TIM_gf_dt *= TIM_gf_SpeedFactor;
	TIM_gf_MainClock += TIM_gf_dt;

	g_LastTime = currentTime;
	gcTIM_gu64_currentTime = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float TIM_f_Clock_TrueRead(void)
{
	/*~~~~~~~~~~~~*/
	u64 currentTime;
	/*~~~~~~~~~~~~*/

	currentTime = OSTicksToMicroseconds(OSGetTime()) - g_StartTime;
	return (currentTime / 1000000.0f) *fGlobalAcc;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TIM_Clock_Reset(void)
{
	g_LastTime = g_StartTime = OSTicksToMicroseconds(OSGetTime());
	TIM_gf_MainClockReal = TIM_gf_MainClock = Cf_Zero;
}

/*$4
 ***********************************************************************************************************************
    XBOX / XENON
 ***********************************************************************************************************************
 */

#elif defined(_XBOX) || defined(_XENON)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TIM_ul_QuickGetTicksPerSecond(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	LARGE_INTEGER	lFrequency;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(QueryPerformanceFrequency(&lFrequency))
		return((ULONG) lFrequency.QuadPart);
	else
		return 10000000;
}

/*
 =======================================================================================================================
    Aim:    Calculates the time in seconds since the start of the application. Calculates the time since the last call
            of this function

    Note:   Stores the number of seconds since start in the global variable TIM_gf_MainClock £
            Stores the time since last call of this function in TIM_gf_dt
 =======================================================================================================================
 */
void TIM_Clock_Update(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	DWORD	dwCurrentTime;
	float	f_OldMainClock;
	/*~~~~~~~~~~~~~~~~~~~*/

	f_OldMainClock = TIM_gf_MainClock;

	/* Calculate the time in seconds since the start of the application */
	dwCurrentTime = GetTickCount(); /* returns the number of milliseconds that have elapsed since the system was started */
	TIM_gf_MainClock = (dwCurrentTime - TIM_gul_SystemStartTime) * 0.001f;

	/* Calculates the dt (time since last call) */
	TIM_gf_dt = TIM_gf_MainClock - f_OldMainClock;

#ifdef JADEFUSION
	if(TIM_gf_dt <= 0.001f) TIM_gf_dt = 0.001f; // [ADAVID] Clamp to 1/1000th of a second
    if(TIM_gf_dt >= 0.063f) 
        TIM_gf_dt = 0.063f; // [JSS] Clamp with FPS lower than 15
#else
	if(TIM_gf_dt <= 0.01f) TIM_gf_dt = 0.01f;  /* 1/100 of frame */
#endif
	/* Check that the dt is positive */
	ERR_X_Assert(TIM_gf_dt > 0.0f);
	TIM_gf_dt = fSup(TIM_gf_dt, TIM_Cf_MaxDt) ? TIM_Cf_MaxDt : TIM_gf_dt;
	TIM_gf_realdt = TIM_gf_dt;
	TIM_gf_dt *= TIM_gf_SpeedFactor;

#ifdef JADEFUSION
    if(MAI_gst_MainHandles.pst_DisplayData && (MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames)) 
        TIM_gf_dt = MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames * TIM_Cf_MinDt;   

#ifdef JOYRECORDER
    if (INO_b_RecordInput || INO_b_PlayInput)    
        TIM_gf_MainClock = INO_n_FrameCounter * 0.0333333f;   
#endif
#endif

#ifdef RASTERS_ON
	PRO_SetRasterFloat(&ENG_gpst_RasterEng_EngineDt, TIM_gf_dt);
	PRO_SetRasterFloat(&ENG_gpst_RasterEng_EngineRealDt, TIM_gf_realdt);
#endif

#ifdef ACTIVE_EDITORS
	if
	(
		MAI_gst_MainHandles.pst_DisplayData
	&&	(MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames)
	) TIM_gf_dt = MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames * TIM_Cf_MinDt;
#endif
}

/*
 =======================================================================================================================
    Aim:    returns the time in seconds since the start of the application
 =======================================================================================================================
 */
float TIM_f_Clock_TrueRead(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	DWORD	dwCurrentTime;
	float	f_Result;
	/*~~~~~~~~~~~~~~~~~~*/

	dwCurrentTime = GetTickCount(); /* returns the number of milliseconds that have elapsed since the system was started */
	f_Result = (dwCurrentTime - TIM_gul_SystemStartTime) * 0.001f;

#ifdef JOYRECORDER
    if (INO_b_RecordInput || INO_b_PlayInput)    
        f_Result = INO_n_FrameCounter * 0.0333333f;
#endif

	return(f_Result);
}

/*
 =======================================================================================================================
    Aim:    Makes a "reset" of the main clock

    Note:   The number of seconds since the start (TIM_gf_MainClock) is set to zero, and the initial counter is set to
            zero too
 =======================================================================================================================
 */
void TIM_Clock_Reset(void)
{
	/* Initialises the main counter & start time */
	TIM_gul_StartCounter[0] = TIM_gul_SystemStartTime = GetTickCount(); /* in milliseconds */

	/* Initialises the main clock */
	TIM_gf_MainClock = Cf_Zero;
}

/*
 =======================================================================================================================
    Aim:    Adjusts the calculated frequency of the CPU clock.

    Out:    Processor clock frequency (in hertz)
 =======================================================================================================================
 */
ULONG TIM_ul_PreciseGetTicksPerSecond(void)
{
	return TIM_ul_QuickGetTicksPerSecond();
}

/*
 =======================================================================================================================
    Aim:    Update if necessary the value of the CPU clock
 =======================================================================================================================
 */
void TIM_UpdateCPUClockFrequency(void)
{
}

/*
 =======================================================================================================================
    Aim:    Reads the internal counter and returns the number of clocks since the start of application (in a float)
 =======================================================================================================================
 */
float TIM_f_Counter_TrueRead(void)
{
	/*~~~~~~~~~~~~~*/
	float	f_Result;
	/*~~~~~~~~~~~~~*/

	f_Result = TIM_f_Clock_TrueRead();
	f_Result *= TIM_ul_QuickGetTicksPerSecond();
	return(f_Result);
}

/*$4
 ***********************************************************************************************************************
    WIN32
 ***********************************************************************************************************************
 */

#else

/*
 =======================================================================================================================
    Aim:    Determins the frequency of the current CPU clock.

    Note:   That function has 2 modes:: - PC mode waits for 1/4 second before returning,

    Out:    Processor clock frequency (per second)
 =======================================================================================================================
 */
ULONG TIM_ul_QuickGetTicksPerSecond(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Count1, ul_Count2;
	ULONG	ul_Time;
	ULONG	ul_Return;
#ifdef ACTIVE_EDITORS
	char	asz_Tmp[255];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* ACTIVE_EDITORS */
	timeBeginPeriod(1);
	ul_Time = L_timeGetTime();
	ul_Count1 = TIM_ul_GetLowPartTimerInternalCounter();

	/* Wait for 1/4 second */
	while(L_timeGetTime() - ul_Time < 250);

	ul_Count2 = TIM_ul_GetLowPartTimerInternalCounter();
	timeEndPeriod(1);

	if(ul_Count2 > ul_Count1)
		ul_Return = (ul_Count2 - ul_Count1) * 4;
	else
		ul_Return = ((ul_Count2 + 0xFFFFFFFF - ul_Count1 + 1) * 4);

#ifdef ACTIVE_EDITORS
	sprintf(asz_Tmp, "Aproximate processor frequency calculated: %.2f MHz", fLongToFloat(ul_Return) / 1000000.0f);
	LINK_PrintStatusMsg(asz_Tmp);
#endif /* ACTIVE_EDITORS */

	return ul_Return;
}

/*
 =======================================================================================================================
    Aim:    Calculates the time in seconds since the start of the application. Calculates the time since the last call
            of this function £

    Note:   Stores the number of seconds since start in the global variable TIM_gf_MainClock £
            Stores the time since last call of this function in TIM_gf_dt
 =======================================================================================================================
 */
void TIM_Clock_Update(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float			f_OldMainClock;
#ifdef ACTIVE_EDITORS
	static float	sf_OldEditorTime = 0.0f;
#ifdef RASTERS_ON
	float			f_Tmp;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#endif
	f_OldMainClock = TIM_gf_MainClockReal;

	/* Calculate the time in seconds since the start of the application */
	TIM_gf_MainClockReal = TIM_f_Clock_TrueRead();

	/* Substract the total time spent in the editors */
	TIM_gf_MainClockReal -= TIM_gf_EditorTime;

	/* Calculates the dt (time since last call) */
	TIM_gf_dt = TIM_gf_MainClockReal - f_OldMainClock;

#if defined(ACTIVE_EDITORS) && defined(RASTERS_ON)
	f_Tmp = TIM_gf_EditorTime - sf_OldEditorTime;
	if(f_Tmp < 0.0f) f_Tmp = 1.0f / 100.0f;
	PRO_SetRasterFloat(&ENG_gpst_RasterEng_EditorsDt, f_Tmp);
#endif

#if !defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)
    if(TIM_gf_dt <= 0.001f) TIM_gf_dt = 0.001f;  /* 1/1000 of second */
#else
	if(TIM_gf_dt <= 1.0f / 100.0f) TIM_gf_dt = 1.0f / 100.0f;  /* 1/100 of frame */
#endif

	/* Check that the dt is positive */
	ERR_X_Assert(TIM_gf_dt > 0.0f);
	TIM_gf_dt = fSup(TIM_gf_dt, TIM_Cf_MaxDt) ? TIM_Cf_MaxDt : TIM_gf_dt;
	TIM_gf_realdt = TIM_gf_dt;
	TIM_gf_dt *= TIM_gf_SpeedFactor;

#ifdef RASTERS_ON
	PRO_SetRasterFloat(&ENG_gpst_RasterEng_EngineDt, TIM_gf_dt);
	PRO_SetRasterFloat(&ENG_gpst_RasterEng_EngineRealDt, TIM_gf_realdt);
#endif

#ifdef JADEFUSION
#if !defined(MAX_PLUGIN)
	if(MAI_gst_MainHandles.pst_DisplayData && (MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames)) 
        TIM_gf_dt = MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames * TIM_Cf_MinDt;
#endif
#else //JADEFUSION
#ifdef ACTIVE_EDITORS
	if
	(
		MAI_gst_MainHandles.pst_DisplayData
	&&	(MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames)
	) TIM_gf_dt = MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames * TIM_Cf_MinDt;
#endif //ACTIVE_EDITORS
#endif //JADEFUSION

#ifdef ACTIVE_EDITORS
	sf_OldEditorTime = TIM_gf_EditorTime;
#endif
	TIM_gf_MainClock += TIM_gf_dt;

    if (INO_b_RecordInput || INO_b_PlayInput)    
        TIM_gf_MainClock = INO_n_FrameCounter * 0.0333333f;   

}

/*
 =======================================================================================================================
    Aim:    Reads the internal counter and returns the time in seconds since the start of the application
 =======================================================================================================================
 */
float TIM_f_Clock_TrueRead(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_CurrentCounter[2];
	float	f_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Read the full ticks counter */
	TIM_GetTimerInternalCounter(&ul_CurrentCounter[0]);

	/* First calculate using hi part of the counter */
	f_Result = fLongToFloat(ul_CurrentCounter[1]) - fLongToFloat(TIM_gul_StartCounter[1]);
	f_Result *= Cf_2exp32;

	/* Then precise using lo part */
	f_Result += fLongToFloat(ul_CurrentCounter[0]) - fLongToFloat(TIM_gul_StartCounter[0]);

	/* From clocks to seconds... */
	f_Result = fDiv(f_Result, TIM_gul_TicksPerSecond);

    if (INO_b_RecordInput || INO_b_PlayInput)    
        f_Result = INO_n_FrameCounter * 0.0333333f;

	return(f_Result);
}

/*
 =======================================================================================================================
    Aim:    Makes a "reset" of the main clock

    Note:   The number of seconds since the start (TIM_gf_MainClock) is set to zero, and the initial counter is set to
            zero too
 =======================================================================================================================
 */
void TIM_Clock_Reset(void)
{
	/* Initialises the main counter */
	TIM_GetTimerInternalCounter(&TIM_gul_StartCounter[0]);

	/* Initialises the main clock */
	TIM_gf_MainClock = TIM_gf_MainClockReal = Cf_Zero;
}

/*
 =======================================================================================================================
    Aim:    Adjusts the calculated frequency of the CPU clock.

    Note:   The later after the beginning of the application this function is called, the more precise it is !

    Out:    Processor clock frequency (in hertz)
 =======================================================================================================================
 */
ULONG TIM_ul_PreciseGetTicksPerSecond(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	float	f_DeltaCycles;
	ULONG	ul_DeltaMs;
	/*~~~~~~~~~~~~~~~~~~*/

	/* Time (in milliseconds) since the begining */
	ul_DeltaMs = L_timeGetTime() - TIM_gul_SystemStartTime;

	/* Cycles since the begining */
	f_DeltaCycles = TIM_f_Counter_TrueRead();

	return(lFloatToLong((f_DeltaCycles / fLongToFloat(ul_DeltaMs)) * 1000.0f));
}

/*
 =======================================================================================================================
    Aim:    Update if necessary the value of the CPU clock
 =======================================================================================================================
 */
void TIM_UpdateCPUClockFrequency(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static char		c_update = 1;
	static float	f_LastUpdateTime;
	static LONG		l_OldTicksPerSecond = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(c_update)
	{
		/* Update every 5 second */
		if((TIM_f_Clock_TrueRead() - f_LastUpdateTime) > 5.0f)
		{
			f_LastUpdateTime = TIM_f_Clock_TrueRead();
			l_OldTicksPerSecond = (LONG) TIM_gul_TicksPerSecond;
			TIM_gul_TicksPerSecond = TIM_ul_PreciseGetTicksPerSecond();
			if((lAbs((LONG) TIM_gul_TicksPerSecond - l_OldTicksPerSecond)) < 40000)
			{
				c_update = 0;
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Reads the internal counter and returns the number of clocks since the start of application (in a float)
 =======================================================================================================================
 */
float TIM_f_Counter_TrueRead(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_CurrentCounter[2];
	float	f_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Read the full clocks counter */
	TIM_GetTimerInternalCounter(&ul_CurrentCounter[0]);

	/* First calculate using hi part of the counter */
	f_Result = fLongToFloat(ul_CurrentCounter[1]) - fLongToFloat(TIM_gul_StartCounter[1]);
	f_Result *= Cf_2exp32;

	/* Then precise using lo part */
	f_Result += fLongToFloat(ul_CurrentCounter[0]) - fLongToFloat(TIM_gul_StartCounter[0]);

	return(f_Result);
}

#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
