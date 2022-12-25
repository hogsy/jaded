/*$T LOAdefs.h GC! 1.097 04/26/01 17:23:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __LOADEFS_H__
#define __LOADEFS_H__

#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"
#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Callback for loading. */
typedef ULONG (*LOA_tdpfnul_CallBack) (ULONG _ul_PosFile);

#define LOA_C_MustExists	 0x01
#define LOA_C_NotSavePtr	 0x04
#define LOA_C_TmpAlloc		 0x08
#define LOA_C_HasUserCounter 0x10
#define LOA_C_OnlyOneRef	 0x20

/*
 -----------------------------------------------------------------------------------------------------------------------
    Structure to define a reference.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	LOA_tdst_Ref_
{
	BIG_KEY					ul_Key;			/* Key of the reference to load */
	ULONG					*pul_Res;		/* To store the result of the loading */
	LOA_tdpfnul_CallBack	pfnul_CallBack; /* Callback to call */
	char					c_Type;			/* Type for reference */
#ifdef _DEBUG
    char					*s_CallbackName;
	char					*psz_File;
	int						i_NumLine;
#endif
} LOA_tdst_Ref;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constants
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
extern BAS_tdst_barray	LOA_gst_ArrayAdr;
#endif
#ifdef _DEBUG
#define LOA_MakeFileRef(a, b, c, d) _LOA_MakeFileRef(a, b, c, d, #c, __FILE__, __LINE__)
extern void				_LOA_MakeFileRef(BIG_KEY, ULONG *, LOA_tdpfnul_CallBack, char, char *, char *, int);
#else
#define LOA_MakeFileRef(a, b, c, d) _LOA_MakeFileRef(a, b, c, d)
extern void				_LOA_MakeFileRef(BIG_KEY, ULONG *, LOA_tdpfnul_CallBack, char);
#endif
extern void				LOA_Resolve(void);

extern ULONG			LOA_ul_SearchAddress(ULONG);
extern void				LOA_DeleteAddress(void *);

extern void				LOA_BeforeLoading(void);
extern void				LOA_AfterLoading(void);
extern ULONG			*LOA_pul_GetCurrentAddress(void);
extern BIG_KEY			LOA_ul_GetCurrentKey(void);
extern void				LOA_SetCurrentAddress(ULONG *);
extern void				LOA_AddPosAddress(int, void *);

extern void				LOA_EndSpeedMode(void);
extern void				LOA_BeginSpeedMode(BIG_KEY _ul_Key);
extern int				LOA_BreakSpeedMode(void);
extern void				LOA_ResumeSpeedMode(void);

extern BOOL				LOA_IsSpecial(ULONG);
extern void				LOA_LoadSpecialArray(void);
extern void				LOA_SaveSpecialArray(void);
extern void				LOA_AddKeyToSpecialArray(ULONG);
extern void				LOA_KillSpecialArray(void);

#ifdef ACTIVE_EDITORS
extern void				LOA_AddAddress(BIG_INDEX, void *);
BIG_KEY					LOA_ul_SearchKeyWithAddress(ULONG);
BIG_KEY					LOA_ul_SearchIndexWithAddress(ULONG);
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Vars
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
extern BAS_tdst_barray	LOA_gst_RefArray;
extern int				LOA_gi_InitRef;
#endif
extern LOA_tdst_Ref		*LOA_gpst_LastRef;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    loading duration rasters
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef PSX2_TARGET
#ifndef _FINAL_
#define LOA_DEFINE_LOAD_RASTER
#endif
#endif

#ifdef _FINAL_
#ifdef LOA_DEFINE_LOAD_RASTER
#undef LOA_DEFINE_LOAD_RASTER
#endif
#endif // _FINAL_

#ifdef LOA_DEFINE_LOAD_RASTER
#define LOA_Cte_LDI_SpeedMode_Engine	0
#define LOA_Cte_LDI_SpeedMode_Display	1
#define LOA_Cte_LDI_SpeedMode_Sound		2
#define LOA_Cte_LDI_AddWorldList		3
#define LOA_Cte_LDI_AttachWorld			4
#define LOA_Cte_LDI_BinRead				5
#define LOA_Cte_LDI_BinSeek				6
#define LOA_Cte_LDI_fRead				7
#define LOA_Cte_LDI_res1				8
#define LOA_Cte_LDI_res2				9
#define LOA_Cte_LDI_res3				10
#define LOA_Cte_LDI_res4				11
#define LOA_Cte_LDI_SpeedMode_Sound2	12
#define LOA_Cte_LDI_SpeedMode_Text      13
#define LOA_Cte_LDI_Number				14

extern float	TIM_gf_MainClock;
extern float	fOoGlobalAcc;
extern float	LOA_gaf_StartTime[LOA_Cte_LDI_Number];
extern float	LOA_gaf_Duration[LOA_Cte_LDI_Number];
extern float	TIM_f_Clock_TrueRead(void);

#ifdef PSX2_TARGET
extern float ps2TIM_f_RealTimeClock(void);
#define RealTime()  (ps2TIM_f_RealTimeClock())
#else
#define RealTime()  (TIM_f_Clock_TrueRead() * fOoGlobalAcc)
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void LOA_StartLoadRaster(int _id)
{
	LOA_gaf_StartTime[_id] = RealTime();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void LOA_StopLoadRaster(int _id)
{
	LOA_gaf_Duration[_id] = RealTime() - LOA_gaf_StartTime[_id];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void LOA_StartLoadRaster_r(int _id)
{
	LOA_gaf_StartTime[_id] = RealTime();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void LOA_StopLoadRaster_r(int _id)
{
	LOA_gaf_Duration[_id] += RealTime() - LOA_gaf_StartTime[_id];
}

void	LOA_InitLoadRaster(void);
void	LOA_DisplayDuration(void);

#else
#define LOA_StartLoadRaster(_id)
#define LOA_StopLoadRaster(_id)
#define LOA_StartLoadRaster_r(_id)
#define LOA_StopLoadRaster_r(_id)
#define LOA_InitLoadRaster()
#define LOA_DisplayDuration()
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __LOADEFS_H__ */
