/*$T PROdefs.h GC! 1.081 04/04/00 13:03:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __PROdefs_H__
#define __PROdefs_H__

#include "BASe/BAStypes.h"
#include "BASe/ERRors/ERRasser.h"
#include "TIMer/TIMdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#if defined(_DEBUG) || defined(RELEASE) || defined(ACTIVE_EDITORS)
#ifndef PSX2_TARGET
#ifndef _FINAL_
#ifndef MAX_PLUGIN
#define RASTERS_ON
#endif
#endif
#endif
#endif /* _DEBUG || RELEASE || ACTIVE_EDITORS */

extern float TIM_gf_SynchroFrequency;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef RASTERS_ON

/* Raster type. */
typedef enum	PRO_tde_Type_
{
	PRO_E_Time,		/* Prof is used to count time */
	PRO_E_Long,		/* Prof is used to display a LONG value */
	PRO_E_String,	/* Prof is used to display a string */
	PRO_E_Table,	/* Prof is used to display the content of an engine table */
	PRO_E_Float,    /* Prof is used to display a float */
	PRO_E_MsgGlob	/* Prof is used to display the content of a global list of messages */
} PRO_tde_Type;

/*
 -----------------------------------------------------------------------------------------------------------------------
    To define a trame raster.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	PRO_tdst_TrameRaster_
{
	PRO_tde_Type					e_Type;
	ULONG							ul_UserData;
	USHORT							uw_StartCount;
	char							*psz_Category;
	char							*psz_SubCategory;
	char							*psz_Name;
	BOOL							b_NeedDisplay;
	ULONG							ul_StartValue;
	ULONG							ul_CurrentValue;
	ULONG							ul_PrevValue;

    // Values over current time window
	ULONG							ul_MinValue;
	ULONG							ul_MaxValue;
    float                           f_AverageValue;
    ULONG                           ul_ValueNb;

    // Values for prev time window
    float                           f_PrevMin;
    float                           f_PrevMax;
    float                           f_PrevAverage;

	float							f_timetot;
	float							f_timetotframe;
	float							f_timeresframe;
	float							f_Max;
	int								c_timetot;
	int								lastcounter;
	struct PRO_tdst_TrameRaster_	*pst_NextRaster;
	struct PRO_tdst_TrameRaster_	*pst_PrevRaster;
} PRO_tdst_TrameRaster;

/* Global variable. First raster of the list */
extern PRO_tdst_TrameRaster *_PRO_gpst_FirstTrameRaster;

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void PRO_ZeroTrameRaster(PRO_tdst_TrameRaster *_pst_Raster)
{
	_pst_Raster->ul_CurrentValue = 0;
}

/*
 =======================================================================================================================
    Aim:    To start a chrono for a raster. Save in ul_StartValue the current chrono.
 =======================================================================================================================
 */
_inline_ void PRO_StartTrameRaster(PRO_tdst_TrameRaster *_pst_Raster)
{
	extern int PRO_TrameCounter;

	/* This is not a time raster... */
	ERR_X_Assert(_pst_Raster->e_Type == PRO_E_Time);

	if(_pst_Raster->uw_StartCount == 0) _pst_Raster->ul_StartValue = TIM_ul_GetLowPartTimerInternalCounter();
	_pst_Raster->uw_StartCount++;
	_pst_Raster->c_timetot++;

	if(_pst_Raster->lastcounter != PRO_TrameCounter)
	{
		_pst_Raster->lastcounter = PRO_TrameCounter;
		_pst_Raster->f_timeresframe = _pst_Raster->f_timetotframe;
		_pst_Raster->f_timetotframe = 0;
	}
}

/*
 =======================================================================================================================
    Aim:    To stop a chrono. ul_CurrentValue will be the difference between started chrono, more it's old value (so we
            can do more that one start/stop in each trame).
 =======================================================================================================================
 */
_inline_ void PRO_StopTrameRaster(PRO_tdst_TrameRaster *_pst_Raster)
{
	ULONG	res;
	float	fres;

	/* If a stop without start is encountered, we do nothing */
	if(_pst_Raster->uw_StartCount == 0) return;

	_pst_Raster->uw_StartCount--;
	if(_pst_Raster->uw_StartCount == 0)
	{
		res = TIM_ul_GetLowPartTimerInternalCounter() - _pst_Raster->ul_StartValue;
		_pst_Raster->ul_CurrentValue += res;
		fres = (float) res / (float) TIM_gul_TicksPerSecond;
		fres *= TIM_gf_SynchroFrequency;
		if(_pst_Raster->f_Max < fres) _pst_Raster->f_Max = fres;
		_pst_Raster->f_timetot += fres;
		_pst_Raster->f_timetotframe += fres;
	}
}

/*
 =======================================================================================================================
    Aim:    Force ul_UserData to the given float value
 =======================================================================================================================
 */
_inline_ void PRO_SetRasterFloat(PRO_tdst_TrameRaster *_pst_Raster, float _f_Value)
{
	/*~~~~~~~~*/
	float	*pf;
	/*~~~~~~~~*/

	pf = (float *) &_pst_Raster->ul_UserData;
	*pf = _f_Value;
}

/*
 =======================================================================================================================
    Aim:    Force ul_UserData to the given value
 =======================================================================================================================
 */
_inline_ void PRO_SetRasterLong(PRO_tdst_TrameRaster *_pst_Raster, LONG _l_Value)
{
	_pst_Raster->ul_UserData = _l_Value;
}

/*
 =======================================================================================================================
    Aim:    Increment ul_UserData with a value
 =======================================================================================================================
 */
_inline_ void PRO_IncRasterLong(PRO_tdst_TrameRaster *_pst_Raster, LONG _l_Inc)
{
	_pst_Raster->ul_UserData += _l_Inc;
}

/*$4
 ***********************************************************************************************************************
    Functions.
 ***********************************************************************************************************************
 */

extern void PRO_FirstInitTrameRaster(PRO_tdst_TrameRaster *, char *, char *, char *, PRO_tde_Type, ULONG);
extern void PRO_KillRaster(PRO_tdst_TrameRaster *);
extern void PRO_ForceDisplayRasters(void);
extern void PRO_ResetAllMaxValues(void);
#else /* !RASTERS_ON */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Rasters are not present
 -----------------------------------------------------------------------------------------------------------------------
 */

#define PRO_FirstInitTrameRaster(a, b, c, d, e, f)
#define PRO_ZeroTrameRaster(a)
#define PRO_StartTrameRaster(a)
#define PRO_StopTrameRaster(a)
#define PRO_SetRasterLong(a, b)
#define PRO_IncRasterLong(a, b)
#endif /* RASTERS_ON */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __PROdefs_H__ */
