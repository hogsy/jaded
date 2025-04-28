/*$T TIMdefs.h GC!1.52 01/13/00 15:43:33 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __TIMDEF_H__
#define __TIMDEF_H__

#include "BASe/BASsys.h"
#include "BASe/BAStypes.h"
#include "LINks/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"

#ifdef PSX2_TARGET
/*
 *#####################################
 *##  SPECIFIC  PSX2_TARGET          ##
 *#####################################
 */

#include <eeregs.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern ULONG    TIM_gul_TicksCount;         
	
_inline_ ULONG TIM_ul_GetLowPartTimerInternalCounter(void)
{
	return (TIM_gul_TicksCount + (ULONG)*T1_COUNT);
}

float   ps2TIM_f_RealTimeClock(void);

#else

#ifdef _GAMECUBE

#include <dolphin/os.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *#####################################
 *##  SPECIFIC GAMECUBE         	 ##	
 *#####################################
 */
 
_inline_ ULONG TIM_ul_GetLowPartTimerInternalCounter(void)
{
	return (OSGetTick());
}
 
#else
#ifdef _XENON

//#ifdef __cplusplus
//extern "C"
//{
//#endif

typedef unsigned int u_int;

/*
 *#####################################
 *##  SPECIFIC XENON                 ##	
 *#####################################
 */

/*
 ===================================================================================================
    Aim:    To get pentium internal counter low part
 ===================================================================================================
 */
_inline_ ULONG _fastcall_ TIM_ul_GetLowPartTimerInternalCounter(void)
{    
	return GetTickCount();
}

/*
 ===================================================================================================
    Aim:    To get pentium internal counter full part
 ===================================================================================================
 */
_inline_ void _fastcall_ TIM_GetTimerInternalCounter(void *_p_Res)
{
    if (_p_Res)
        *(DWORD*)_p_Res = GetTickCount();
}

#else
/*
 *#####################################
 *##  NON SPECIFIC CONSOLE       	##	
 *#####################################
 */

#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#pragma once
/* To get pentium internal counter low part */
#pragma warning(disable : 4035)

#define RDTSC   __asm _emit 0x0F __asm _emit 0x31

/*
 ===================================================================================================
    Aim:    To get pentium internal counter low part
 ===================================================================================================
 */
ULONG _fastcall_ TIM_ul_GetLowPartTimerInternalCounter( void );

#pragma warning(default: 4035)

/*
 ===================================================================================================
    Aim:    To get pentium internal counter full part
 ===================================================================================================
 */
_inline_ void _fastcall_ TIM_GetTimerInternalCounter(void *_p_Res)
{
#if defined( _MSC_VER ) && defined( _WIN32 )
    __asm
    {
		push eax
		push ebx
		push edx
        RDTSC
        mov ebx, dword ptr _p_Res
        mov[ebx], eax
        mov[ebx + 4], edx
		pop edx
		pop ebx
		pop eax
    }
#elif defined( _WIN32 )
	if (_p_Res)
		*(DWORD*)_p_Res = GetTickCount();
#else
	if ( _p_Res == NULL )
		return;

	struct timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	*( DWORD * ) _p_Res = TIM_ul_GetLowPartTimerInternalCounter();
#endif
}
#endif //XENON
#endif
#endif
/*
 *#####################################
 *##  END OF SPECIFIC PART           ##
 *#####################################
 */




/*$4
 ***************************************************************************************************
    Global variables
 ***************************************************************************************************
 */
extern ULONG    TIM_gul_TicksPerSecond;
extern UCHAR    TIM_guc_SynchroFrequencyIndex;       /* Current index in the tables of possible
                                                     * frequencies */
extern float    TIM_gf_MainClock;
extern float    TIM_gf_dt;
extern float    TIM_gf_EngineLoopStartTime;
extern float    TIM_gf_EngineLoopEndTime;
extern int		TIM_gi_LastNumFrame;
extern int		TIM_gi_NumFrame;
extern int		TIM_gi_CountNumFrame;

#ifndef PSX2_TARGET
extern ULONG    TIM_gul_StartCounter[2];
extern float    TIM_gf_EditorTime;
extern ULONG    TIM_gul_SystemStartTime;
#endif

/*$4
 ***************************************************************************************************
    $4 functions
 ***************************************************************************************************
 */
extern void     TIM_Clock_Update(void);
extern void     TIM_Clock_Reset(void);
extern float    TIM_f_Clock_TrueRead(void);
extern ULONG    TIM_ul_QuickGetTicksPerSecond(void);

extern float    TIM_f_Counter_TrueRead(void);
extern ULONG    TIM_ul_PreciseGetTicksPerSecond(void);
extern void     TIM_UpdateCPUClockFrequency(void);

/*
 ===================================================================================================
    Aim:    Read the main clock (calculated at the beginning of the loop)
 ===================================================================================================
 */
_inline_ float TIM_f_Clock_Read(void)
{
    return TIM_gf_MainClock;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __TIMDEF_H__ */