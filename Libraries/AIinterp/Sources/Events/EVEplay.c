/*$T EVEplay.c GC! 1.081 10/14/02 10:08:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "EVEstruct.h"
#include "EVEconst.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "EVEplay.h"
#include "EVEnt_aifunc.h"
#include "EVEnt_interpolationkey.h"
#include "EVEnt_morphkey.h"
#include "EVEnt_timekey.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIdebug.h"
#include "AIinterp/Sources/AIstack.h"

#include "ENGine/Sources/ANImation/ANIplay.h"

#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern int					AI_EvalFunc_OBJ_GizmoSet_C(OBJ_tdst_GameObject *, int);
extern void					AI_EvalFunc_OBJ_GizmoRestore_C(OBJ_tdst_GameObject *, int);
OBJ_tdst_GameObject			*EVE_gpst_OwnerGAO = NULL;
OBJ_tdst_GameObject			*EVE_gpst_CurrentGAO = NULL;
EVE_tdst_Data				*EVE_gpst_CurrentData = NULL;
EVE_tdst_ListTracks			*EVE_gpst_CurrentListTracks = NULL;
EVE_tdst_Track				*EVE_gpst_CurrentTrack = NULL;
EVE_tdst_Params				*EVE_gpst_CurrentParam = NULL;
EVE_tdst_Event				*EVE_gpst_CurrentEvent = NULL;
#ifdef JADEFUSION
EVE_tdst_Event				*EVE_GetEventPointer(EVE_tdst_Track *_pst_Track, EVE_tdst_Params *_pst_Param, INT _i_Pos);
#endif
struct MATH_tdst_Matrix_	*EVE_gpst_CurrentRefMatrix = NULL;
struct MATH_tdst_Matrix_	*EVE_gpst_SavedMatrix = NULL;
float						EVE_gf_DT = 0.0f;
float						EVE_gf_ElapsedDT = 0.0f;
BOOL						EVE_gb_NeedToReparse = FALSE;
BOOL						EVE_gb_CanFlash = TRUE;
BOOL						EVE_gb_UseRunningPauseFlag = TRUE;
BOOL						EVE_gb_AddTime = FALSE;
BOOL						EVE_gb_PlayOnlyAnimTracks = FALSE;
BOOL						EVE_gb_ForcePlayerOnAllTracks = FALSE;
ULONG						*EVE_gpul_PartialMask;

// Gilles, 22/02/2006.
// Use to call EVE_FlashMatrix() only once by EVE_b_RunAllTracks(...).
// Corrected bug : with a translation track and a rotation track whose 1st events have flash matrix,
//                 EVE_FlashMatrix() is called twice. 1st time is ok. But during the 2nd call, the value
// of the  Gao's matrix used to update the flash matrix is wrong because the Gao has moved during the
// play of the first track.
BOOL						EVE_gb_FlashMatrixDone = FALSE;

#ifdef ACTIVE_EDITORS
BOOL						EVE_gb_ForceNotDone = FALSE;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *EVE_p_SymetrizeData(void *_p_Data)
{
	/*~~~~~~~~~~~~~~~~*/
	void	*p_SymData;
	short	w_InterType;
	/*~~~~~~~~~~~~~~~~*/

	if(!_p_Data) return NULL;

	p_SymData = MEM_p_Alloc(*(short *) _p_Data);
	L_memcpy(p_SymData, _p_Data, *(short *) _p_Data);

	w_InterType = *((short *) _p_Data + 1);

	if(w_InterType & EVE_InterKeyType_Translation_Mask)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	*pst_P;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_P = ((MATH_tdst_Vector *) ((char *) p_SymData + 4));
		pst_P->x = -pst_P->x;
	}

	if(w_InterType & EVE_InterKeyType_Rotation_Matrix)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix		*pst_M;
		MATH_tdst_Quaternion	st_Q;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_M = ((MATH_tdst_Matrix *) ((char *) p_SymData + 4));
		MATH_ConvertMatrixToQuaternion(&st_Q, pst_M);
		st_Q.y = -st_Q.y;
		st_Q.z = -st_Q.z;
		MATH_ConvertQuaternionToMatrix(pst_M, &st_Q);
	}

	if(w_InterType & EVE_InterKeyType_Rotation_Quaternion)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Quaternion	*pst_Q;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Q = ((MATH_tdst_Quaternion *) ((char *) p_SymData + 4));
		pst_Q->y = -pst_Q->y;
		pst_Q->z = -pst_Q->z;
	}

	return p_SymData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *EVE_pst_GetBoneFromGizmoIndex(short _w_Gizmo)
{
	if
	(
		!EVE_gpst_OwnerGAO
	||	!EVE_gpst_OwnerGAO->pst_Base
	||	!EVE_gpst_OwnerGAO->pst_Base->pst_GameObjectAnim
	||	!EVE_gpst_OwnerGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	) return NULL;

	if(_w_Gizmo != -1)
	{
		return (OBJ_tdst_GameObject *)
			(EVE_gpst_OwnerGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->p_Table +
				_w_Gizmo)->p_Pointer;
	}
	else
		return EVE_gpst_OwnerGAO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVE_b_GetSymetricEventInfo
(
	OBJ_tdst_GameObject *_pst_GO,
	EVE_tdst_Event		*_pst_Event,
	short				*_pw_NumTrack,
	short				*_pw_NumEvent
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks *pst_ListTracks;
	EVE_tdst_Track		*pst_Track;
	short				i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pst_GO
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks
	) return FALSE;

	pst_ListTracks = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks;

	for(i = 0; i < pst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = &pst_ListTracks->pst_AllTracks[i];

		for(j = 0; j < pst_Track->uw_NumEvents; j++)
		{
			if
			(
				(&pst_Track->pst_AllEvents[j] != _pst_Event)
			&&	!(pst_Track->pst_AllEvents[j].w_Flags & EVE_C_EventFlag_Symetric)
			&&	(pst_Track->pst_AllEvents[j].p_Data == _pst_Event->p_Data)
			)
			{
				*_pw_NumTrack = i;
				*_pw_NumEvent = j;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_ReinitTrack(EVE_tdst_Data *_pst_Data, ULONG ul_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	EVE_tdst_Params *pst_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Data) return;
	if(!_pst_Data->pst_ListTracks) return;
	if(!_pst_Data->pst_ListParam) return;

	pst_Track = &_pst_Data->pst_ListTracks->pst_AllTracks[ul_Track];
	pst_Params = &_pst_Data->pst_ListParam[ul_Track];
	if(!pst_Params) return;

	pst_Params->f_Time = 0.0f;
	pst_Params->uw_CurrentEvent = 0;
	pst_Params->uw_Flags &= ~EVE_C_Track_Running;
	if(pst_Track->uw_Flags & EVE_C_Track_RunningInit) pst_Params->uw_Flags |= EVE_C_Track_Running;
	pst_Params->uw_Flags &= ~EVE_C_Track_Pause;
	pst_Params->uw_Flags &= ~EVE_C_Track_EventDone;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_ReinitData(EVE_tdst_Data *_pst_Data)
{
	/*~~~~~~*/
	USHORT	i;
	/*~~~~~~*/

	if(!_pst_Data || !_pst_Data->pst_ListTracks) return;
	for(i = 0; i < _pst_Data->pst_ListTracks->uw_NumTracks; i++) EVE_ReinitTrack(_pst_Data, i);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SetRunningTrack(EVE_tdst_Params *_pst_Param, BOOL _b_Run)
{
	if(_b_Run)
	{
		if(!(_pst_Param->uw_Flags & EVE_C_Track_Running))
		{
			_pst_Param->f_Time = 0.0f;
			_pst_Param->uw_Flags |= EVE_C_Track_Running;
			_pst_Param->uw_Flags &= ~EVE_C_Track_Pause;
			_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
			_pst_Param->uw_CurrentEvent = 0;
		}
	}
	else
	{
		_pst_Param->uw_Flags &= ~EVE_C_Track_Running;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SetRunningTracks(EVE_tdst_Data *_pst_Data, BOOL _b_Run)
{
	/*~~~~~~*/
	USHORT	i;
	/*~~~~~~*/

	if(!_pst_Data) return;

	for(i = 0; i < _pst_Data->pst_ListTracks->uw_NumTracks; i++)
		EVE_SetRunningTrack(&_pst_Data->pst_ListParam[i], _b_Run);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SetPauseTrack(EVE_tdst_Params *_pst_Param, BOOL _b_Pause)
{
	if(_b_Pause)
	{
		if(!(_pst_Param->uw_Flags & EVE_C_Track_Pause)) _pst_Param->uw_Flags |= EVE_C_Track_Pause;
	}
	else
	{
		_pst_Param->uw_Flags &= ~EVE_C_Track_Pause;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SetPauseTracks(EVE_tdst_Data *_pst_Data, BOOL _b_Pause)
{
	/*~~~~~~*/
	USHORT	i;
	/*~~~~~~*/

	for(i = 0; i < _pst_Data->pst_ListTracks->uw_NumTracks; i++)
		EVE_SetPauseTrack(&_pst_Data->pst_ListParam[i], _b_Pause);
}

/*
 =======================================================================================================================
    a déplacer dans EVE
 =======================================================================================================================
 */
void EVE_GetTracksTime(EVE_tdst_Data *_pst_Data, int _i_Track, float *_f_Cur, float *_f_Total)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Event, *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f_Cur) *_f_Cur = 0.0f;
	if(_f_Total) *_f_Total = 0.0f;

	if
	(
		!_pst_Data
	||	!_pst_Data->pst_ListTracks
	||	!_pst_Data->pst_ListTracks->pst_AllTracks
	||	!_pst_Data->pst_ListTracks->pst_AllTracks[_i_Track].pst_AllEvents
	) return;

	if(!_f_Cur)
	{
		if(!_f_Total) return;
		pst_Event = _pst_Data->pst_ListTracks->pst_AllTracks[_i_Track].pst_AllEvents;
		pst_Last = pst_Event + _pst_Data->pst_ListTracks->pst_AllTracks[_i_Track].uw_NumEvents;
		for(; pst_Event < pst_Last; pst_Event++) *_f_Total += EVE_FrameToTime(EVE_NumFrames(pst_Event));
		return;
	}

	pst_Event = _pst_Data->pst_ListTracks->pst_AllTracks[_i_Track].pst_AllEvents;
	pst_Last = pst_Event + _pst_Data->pst_ListParam[_i_Track].uw_CurrentEvent;
	for(; pst_Event < pst_Last; pst_Event++) *_f_Cur += EVE_FrameToTime(EVE_NumFrames(pst_Event));

	if(_f_Total)
	{
		*_f_Total = *_f_Cur;
		pst_Last = _pst_Data->pst_ListTracks->pst_AllTracks[_i_Track].pst_AllEvents + _pst_Data->pst_ListTracks->pst_AllTracks[_i_Track].uw_NumEvents;
		for(; pst_Event < pst_Last; pst_Event++) *_f_Total += EVE_FrameToTime(EVE_NumFrames(pst_Event));
	}

	*_f_Cur += _pst_Data->pst_ListParam[_i_Track].f_Time;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SetTracksTime(EVE_tdst_Data *_pst_Data, float _f_Time)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float			f_AddDelay;
	USHORT			uw_NumFrames, uw_TotalFrames;
	EVE_tdst_Event	*pst_Event, *pst_LastEvent;
	EVE_tdst_Track	*pst_Track, *pst_LastTrack;
	EVE_tdst_Params *pst_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_TotalFrames = (USHORT) (_f_Time * 60.0f);
	f_AddDelay = _f_Time - (((float) uw_TotalFrames) / 60.0f);

	pst_Track = _pst_Data->pst_ListTracks->pst_AllTracks;
	pst_LastTrack = pst_Track + _pst_Data->pst_ListTracks->uw_NumTracks;
	pst_Params = _pst_Data->pst_ListParam;
	for(; pst_Track < pst_LastTrack; pst_Track++, pst_Params++)
	{
		uw_NumFrames = uw_TotalFrames;

		pst_Event = pst_Track->pst_AllEvents;
		pst_LastEvent = pst_Track->pst_AllEvents + pst_Track->uw_NumEvents;

		pst_Params->uw_CurrentEvent = 0;

		for(; pst_Event < pst_LastEvent; pst_Event++, pst_Params->uw_CurrentEvent++)
		{
			if(uw_NumFrames < EVE_NumFrames(pst_Event))
			{
				pst_Params->f_Time = (((float) uw_NumFrames) / 60.0f) + f_AddDelay;
				pst_Params->uw_Flags |= EVE_C_Track_EventDone;
				goto settrackstime_nextloop;
			}

			uw_NumFrames -= EVE_NumFrames(pst_Event);

			pst_Params->f_Time = 0.0f;
			pst_Params->uw_Flags |= EVE_C_Track_EventDone;
		}

settrackstime_nextloop: ;

		/* We cannot set an Event after the last one. */
		if(pst_Params->uw_CurrentEvent == pst_Track->uw_NumEvents)
			pst_Params->uw_CurrentEvent = pst_Track->uw_NumEvents - 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SetTracksRatio(EVE_tdst_Data *_pst_Data, float _f_Ratio)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float			f_AddDelay, f_Total, f_Time;
	USHORT			uw_NumFrames, uw_SaveNumFrames;
	EVE_tdst_Event	*pst_Event, *pst_LastEvent;
	EVE_tdst_Track	*pst_Track, *pst_LastTrack;
	EVE_tdst_Params *pst_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the Anim's length */
	EVE_GetTracksTime(_pst_Data, 0, NULL, &f_Total);

	f_Time = _f_Ratio * f_Total;

	uw_NumFrames = (USHORT) (f_Time * 60.0f);
	f_AddDelay = f_Time - (((float) uw_NumFrames) / 60.0f);

	uw_SaveNumFrames = uw_NumFrames;

	pst_Track = _pst_Data->pst_ListTracks->pst_AllTracks;
	pst_LastTrack = pst_Track + _pst_Data->pst_ListTracks->uw_NumTracks;
	pst_Params = _pst_Data->pst_ListParam;
	for(; pst_Track < pst_LastTrack; pst_Track++, pst_Params++)
	{
		uw_NumFrames = uw_SaveNumFrames;

		pst_Event = pst_Track->pst_AllEvents;
		pst_LastEvent = pst_Track->pst_AllEvents + pst_Track->uw_NumEvents;

		pst_Params->uw_CurrentEvent = 0;

		for(; pst_Event < pst_LastEvent; pst_Event++, pst_Params->uw_CurrentEvent++)
		{
			if(uw_NumFrames < EVE_NumFrames(pst_Event))
			{
				pst_Params->f_Time = (((float) uw_NumFrames) / 60.0f) + f_AddDelay;
				pst_Params->uw_Flags |= EVE_C_Track_EventDone;
				goto settrackstime_nextloop;
			}

			uw_NumFrames -= EVE_NumFrames(pst_Event);

			pst_Params->f_Time = 0.0f;
			pst_Params->uw_Flags |= EVE_C_Track_EventDone;
		}

settrackstime_nextloop: ;

		/* We cannot set an Event after the last one. */
		if(pst_Params->uw_CurrentEvent == pst_Track->uw_NumEvents)
			pst_Params->uw_CurrentEvent = pst_Track->uw_NumEvents - 1;
	}
}

/*
 =======================================================================================================================
    copy current game object matrix into list of tracks matrix £
    allocate eventually the matrix £
    use directly constants (EVE_gpst_CurrentListTracks and EVE_gpst_CurrentGAO
 =======================================================================================================================
 */
void EVE_FlashMatrix(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*pst_Matrix2Flash;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!EVE_gb_CanFlash) return;
	if(!EVE_gpst_CurrentListTracks) return;
	if(!EVE_gpst_CurrentGAO) return;

	pst_Matrix2Flash = (OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Hierarchy)) ? OBJ_pst_GetLocalMatrix(EVE_gpst_CurrentGAO) : OBJ_pst_GetAbsoluteMatrix(EVE_gpst_CurrentGAO);

	if(!EVE_gpst_CurrentRefMatrix)
		OBJ_SetFlashMatrix(EVE_gpst_CurrentGAO, pst_Matrix2Flash);
	else
		MATH_CopyMatrix(EVE_gpst_CurrentRefMatrix, pst_Matrix2Flash);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVE_i_SearchLabel(EVE_tdst_Track *_pst_Track, int _i_Label, float *_pf_TotalTime)
{
	/*~~~~~~~~~~~~*/
	int		i;
	float	f_Total;
	/*~~~~~~~~~~~~*/

	f_Total = 0.0f;
	for(i = 0; i < _pst_Track->uw_NumEvents; i++)
	{
		if
		(
			((_pst_Track->pst_AllEvents[i].w_Flags & 0xF000) == _i_Label)
		&&	((_pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_GotoLabel)
		&&	((_pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_SetTimeToLabel)
		)
		{
			*_pf_TotalTime = f_Total;
			return i;
		}

		f_Total += EVE_FrameToTime(EVE_NumFrames(&_pst_Track->pst_AllEvents[i]));
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVE_i_SearchTime(EVE_tdst_Track *_pst_Track, float *_pf_TotalTime)
{
	/*~~~~~~~~~~~~*/
	int		i;
	float	f_Total;
	float	f_cur;
	/*~~~~~~~~~~~~*/

	f_Total = *_pf_TotalTime;
	for(i = 0; i < _pst_Track->uw_NumEvents; i++)
	{
		f_cur = EVE_FrameToTime(EVE_NumFrames(&_pst_Track->pst_AllEvents[i]));
		if(f_cur >= f_Total)
		{
			if(f_cur - f_Total < (EVE_FrameToTime(1) - 1E-5f))
			{
				*_pf_TotalTime = 0.0f;
				i++;
			}
			else
			{
				*_pf_TotalTime = f_Total;
			}

			return i;
		}

		f_Total -= EVE_FrameToTime(EVE_NumFrames(&_pst_Track->pst_AllEvents[i]));
	}

	if(_pst_Track->pst_AllEvents)
	{
		*_pf_TotalTime = EVE_FrameToTime(EVE_NumFrames(&_pst_Track->pst_AllEvents[_pst_Track->uw_NumEvents - 1]));
		return _pst_Track->uw_NumEvents - 1;
	}
	else
	{
		*_pf_TotalTime = 0.0f;
		return 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SetAllTimesTo(float _f_Time)
{
	/*~~~~~~~~~~*/
	int		i;
	int		i_Num;
	float	f_Mem;
	/*~~~~~~~~~~*/

	for(i = 0; i < EVE_gpst_CurrentListTracks->uw_NumTracks; i++)
	{
		f_Mem = _f_Time;
		i_Num = EVE_i_SearchTime(&EVE_gpst_CurrentListTracks->pst_AllTracks[i], &f_Mem);
		if(i_Num != -1)
		{
			EVE_gpst_OwnerGAO->pst_Extended->pst_Events->pst_ListParam[i].uw_Flags &= ~EVE_C_Track_EventDone;
			EVE_gpst_OwnerGAO->pst_Extended->pst_Events->pst_ListParam[i].uw_CurrentEvent = i_Num;
			EVE_gpst_OwnerGAO->pst_Extended->pst_Events->pst_ListParam[i].f_Time = f_Mem;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_RunningTracksAtStart(EVE_tdst_Data *_pst_Data)
{
	/*~~*/
	int i;
	/*~~*/

	if(!_pst_Data) return;

	for(i = 0; i < _pst_Data->pst_ListTracks->uw_NumTracks; i++)
	{
		if(_pst_Data->pst_ListParam[i].uw_Flags & EVE_C_Track_RunningInit)
		{
			_pst_Data->pst_ListParam[i].uw_Flags &= ~EVE_C_Track_EventDone;
			_pst_Data->pst_ListParam[i].uw_CurrentEvent = 0;
			_pst_Data->pst_ListParam[i].f_Time = 0.0f;
		}
	}
}

void		EVE_Anim_TranslationKey_Play(EVE_tdst_Event *);
void		EVE_Anim_QuaternionKey_Play(EVE_tdst_Event *);
extern void ARAM_pst_Cache_PreLoad(EVE_tdst_Track *, ULONG);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVE_RunAnimTrack
(
	EVE_tdst_Track	*_pst_Track,
	EVE_tdst_Params *_pst_Param,
	ULONG			_ul_Track,
	char			_c_LOD,
	float			*_pf_RestTime
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL			b_UglyOptim;
	EVE_tdst_Event	*pst_Current;
	float			f_RestDT, f_EventLength;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* We loop back ONLY if we start playing at the Last Event */
	if(_pst_Param->uw_CurrentEvent == (_pst_Track->uw_NumEvents - 1))
	{
		if(_pst_Track->uw_Gizmo != 0xFF)
		{
			_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
			_pst_Param->uw_CurrentEvent = 0;
			_pst_Param->f_Time = 0.0f;
		}
		else
			EVE_RunningTracksAtStart(EVE_gpst_CurrentData);
	}

	f_RestDT = TIM_gf_dt;
	while(1)
	{
		/* Set current event */
		EVE_gpst_CurrentEvent = pst_Current = _pst_Track->pst_AllEvents + _pst_Param->uw_CurrentEvent;

		b_UglyOptim = pst_Current->uw_NumFrames & 0x8000;

		f_EventLength = EVE_FrameToTime(EVE_NumFrames(pst_Current));

		/* Flash matrix */
		if(!EVE_gpul_PartialMask && (b_UglyOptim ? ((pst_Current == _pst_Track->pst_AllEvents) && (EVE_gpst_CurrentListTracks->pst_AllTracks == _pst_Track)): (pst_Current->w_Flags & EVE_C_EventFlag_Flash)) && !(_pst_Param->uw_Flags & EVE_C_Track_EventDone))
		{
			EVE_FlashMatrix();
		}

		if(_pst_Param->f_Time + f_RestDT > f_EventLength - 1E-5f)
		{
			f_RestDT -= f_EventLength - _pst_Param->f_Time;
			f_RestDT = (f_RestDT > 0.0f) ? f_RestDT : 0.0f;

			if(_pst_Param->uw_CurrentEvent != (_pst_Track->uw_NumEvents - 1))
			{
				_pst_Param->f_Time = 0.0f;
				_pst_Param->uw_CurrentEvent++;
				_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
				continue;
			}
			else
				goto LastEventCase;
		}
		else
		{
			_pst_Param->f_Time += f_RestDT;
			f_RestDT = 0.0f;
		}

		/* One Frame out of 2 LOD (Always play MagicBox tracks) */
		{
			if((_ul_Track > 1) && (!f_RestDT) && (_c_LOD & EVE_LOD_OneFrameOutOfTwo))
			{
				if(_c_LOD & EVE_LOD_FrameParity)
				{
					_pst_Param->uw_Flags |= EVE_C_Track_EventDone;

					return FALSE;
				}
			}
		}

#ifdef ANIMS_USE_ARAM
		if((pst_Current->t.size & 0x0001) && (EVE_gpst_CurrentListTracks->ul_NbOfInstances > 1000000))
			ARAM_pst_Cache_PreLoad(_pst_Track, _pst_Param->uw_CurrentEvent);
#endif
		/* Play */
		if(EVE_w_Event_InterpolationKey_GetType(pst_Current) & EVE_InterKeyType_Rotation_Mask)
		{
			EVE_Anim_QuaternionKey_Play(pst_Current);
		}
		else
		{
			EVE_Anim_TranslationKey_Play(pst_Current);
		}

		/* Mark Event as already done (for Flash) */
		_pst_Param->uw_Flags |= EVE_C_Track_EventDone;

		/* No more time to be played ... we return */
		if(!f_RestDT) return FALSE;

LastEventCase:
#ifdef ANIMS_USE_ARAM
		if((pst_Current->t.size & 0x0001) && (EVE_gpst_CurrentListTracks->ul_NbOfInstances > 1000000))
			ARAM_pst_Cache_PreLoad(_pst_Track, _pst_Param->uw_CurrentEvent);
#endif
		/*$F
			We reach the Last Event of the Anim Track. We dont loop back right now because certain
			cases of blend uses the last frame of the Anim. We would lose this info if we looped back.
		*/

		/*$F We force the Play on the Last Key (Nonetheless, it would never be played with its zero-length) */
		if(EVE_w_Event_InterpolationKey_GetType(pst_Current) & EVE_InterKeyType_Rotation_Mask)
			EVE_Anim_QuaternionKey_Play(pst_Current);
		else
			EVE_Anim_TranslationKey_Play(pst_Current);

		_pst_Param->uw_CurrentEvent = _pst_Track->uw_NumEvents - 1;
		_pst_Param->f_Time = 0.0f;
		_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;

		*_pf_RestTime = f_RestDT;
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVE_RunOneTrack(EVE_tdst_Track *_pst_Track, EVE_tdst_Params *_pst_Param, BOOL _b_Engine, float *_pf_RestTime)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Current;
	EVE_tdst_Event	*pst_Ret;
	int				i_Jump;
	float			f_TotalTime, f_RestDT;
	BOOL			b_UglyOptim;
#ifdef ACTIVE_EDITORS
	int				i_Current;
	BOOL			b_NonZeroEvent;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Track->pst_AllEvents) return FALSE;

#ifdef ACTIVE_EDITORS
	b_NonZeroEvent = FALSE;
	i_Current = _pst_Param->uw_CurrentEvent;
#endif
	EVE_gpst_SavedMatrix = EVE_gpst_OwnerGAO->pst_GlobalMatrix;

	/* ??? */
	if(_pst_Track->uw_Flags & EVE_C_Track_Time) _pst_Param->i_Param1 = -1;

#ifdef ACTIVE_EDITORS

	/* If we are AFTER the last Event, we loop back. */
	if(_pst_Param->uw_CurrentEvent >= _pst_Track->uw_NumEvents)
	{
		LINK_PrintStatusMsg("After Last Event Bug");
		_pst_Param->uw_CurrentEvent = 0;
		_pst_Param->f_Time = 0;
		_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
	}

	if(EVE_gb_ForceNotDone)
	{
		_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
	}

#endif
	/* If we are at the end of the last Event and if the Track is AUTO-LOOP, we loop. */
	if
	(
		(_b_Engine)
	&&	(TIM_gf_dt)
	&&	(_pst_Param->uw_CurrentEvent == _pst_Track->uw_NumEvents - 1)
	&&	(_pst_Param->f_Time == EVE_FrameToTime(EVE_NumFrames(&_pst_Track->pst_AllEvents[_pst_Param->uw_CurrentEvent])))
	&&	(_pst_Track->uw_Flags & EVE_C_Track_AutoLoop)
	)
	{
		if(_pst_Track->uw_Gizmo != 0xFF)
		{
			_pst_Param->uw_CurrentEvent = 0;
			_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
		}
		else
		{
			EVE_RunningTracksAtStart(EVE_gpst_CurrentData);
		}
	}

	/* Rest DT to be played. It can be negative when BackPlaying. */
	f_RestDT = TIM_gf_dt;

	while(1)
	{
recom:

		/* Set current event */
		EVE_gpst_CurrentEvent = pst_Current = _pst_Track->pst_AllEvents + _pst_Param->uw_CurrentEvent;

		b_UglyOptim = pst_Current->uw_NumFrames & 0x8000;

#ifdef ACTIVE_EDITORS
		if(EVE_NumFrames(EVE_gpst_CurrentEvent) != 0) b_NonZeroEvent = TRUE;
#endif
		EVE_gf_ElapsedDT = TIM_gf_dt - f_RestDT;

#ifdef ACTIVE_EDITORS
		if((f_RestDT > 0.0f) && (TIM_gf_dt < 0.0f)) break;
#endif
		if(_pst_Param->f_Time + f_RestDT > EVE_FrameToTime(EVE_NumFrames(pst_Current)))
		{
			EVE_gf_DT = EVE_FrameToTime(EVE_NumFrames(pst_Current)) - _pst_Param->f_Time;
			f_RestDT -= EVE_gf_DT;
			_pst_Param->f_Time = EVE_FrameToTime(EVE_NumFrames(pst_Current));
		}

#ifdef ACTIVE_EDITORS
		else if(_pst_Param->f_Time + f_RestDT < 0.0f)
		{
			/* Back Playing. */
			EVE_gf_DT = -_pst_Param->f_Time;
			_pst_Param->f_Time = 0.0f;
			f_RestDT -= EVE_gf_DT;
		}

#endif
		else
		{
			EVE_gf_DT = f_RestDT;
			_pst_Param->f_Time += f_RestDT;
			f_RestDT = 0.0f;
		}

		/* Flash matrix */
		if
		(
			!EVE_gb_FlashMatrixDone
		&&	!EVE_gpul_PartialMask
		&&	(b_UglyOptim ? ((pst_Current == _pst_Track->pst_AllEvents) && (_pst_Track == EVE_gpst_CurrentListTracks->pst_AllTracks)): (pst_Current->w_Flags & EVE_C_EventFlag_Flash))
		&&	!(_pst_Param->uw_Flags & EVE_C_Track_EventDone)
		&&	!(EVE_gpst_CurrentListTracks->uw_Flags & EVE_C_Tracks_NoFlash)
		&&	(TIM_gf_dt >= 0.0f)
		)
		{
			EVE_gb_FlashMatrixDone = TRUE;
			EVE_FlashMatrix();
		}

		/* Play */
		if(!((b_UglyOptim) ? 0 : pst_Current->w_Flags & EVE_C_EventFlag_DoOnce) || !(_pst_Param->uw_Flags & EVE_C_Track_EventDone))
		{
			switch(b_UglyOptim ? EVE_C_EventFlag_InterpolationKey : (pst_Current->w_Flags & EVE_C_EventFlag_Type))
			{

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_InterpolationKey:
				if((f_RestDT == 0) || (_pst_Param->uw_CurrentEvent == _pst_Track->uw_NumEvents - 1))
					pst_Ret = EVE_Event_InterpolationKey_Play(pst_Current);
				break;

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_MorphKey:
				if((f_RestDT == 0) || (_pst_Param->uw_CurrentEvent == _pst_Track->uw_NumEvents - 1))
					pst_Ret = EVE_Event_MorphKey_Play(pst_Current);
				break;

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_AIFunction:
				if(!EVE_gb_PlayOnlyAnimTracks)
				{
					pst_Ret = EVE_Event_AIFunction_Play(pst_Current);

					/* Wait until condition ? */
					if(pst_Current->w_Flags & EVE_C_EventFlag_WaitFalse)
					{
						if(AI_PopInt())
						{
							_pst_Param->f_Time = 0.0f;
							return FALSE;
						}
					}
					else if(pst_Current->w_Flags & EVE_C_EventFlag_WaitTrue)
					{
						if(!(AI_PopInt()))
						{
							_pst_Param->f_Time = 0.0f;
							return FALSE;
						}
					}
				}
				break;

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_GotoLabel:
				if(!EVE_gb_PlayOnlyAnimTracks)
				{
					i_Jump = EVE_i_SearchLabel(_pst_Track, pst_Current->w_Flags & 0xF000, &f_TotalTime);
					if(i_Jump != -1)
					{
						_pst_Param->uw_CurrentEvent = i_Jump;
						_pst_Param->f_Time = 0.0f;
						goto recom;
					}
				}
				break;

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_SetTimeToLabel:
				if(!EVE_gb_PlayOnlyAnimTracks)
				{
					i_Jump = EVE_i_SearchLabel(_pst_Track, pst_Current->w_Flags & 0xF000, &f_TotalTime);
					if(i_Jump != -1)
					{
						EVE_SetAllTimesTo(f_TotalTime);
						EVE_gb_NeedToReparse = TRUE;
						return FALSE;
					}
				}
				break;
			}
		}

		/* Event is done */
		_pst_Param->uw_Flags |= EVE_C_Track_EventDone;

		/* Stay in the same event */
		if((f_RestDT == 0.0f) /* || (TIM_gf_dt == 0.0f) */ )
		{
#ifdef ACTIVE_EDITORS
			if(TIM_gf_dt && fabs(_pst_Param->f_Time - EVE_FrameToTime(EVE_NumFrames(pst_Current))) < 1e-5)
			{
				if(_pst_Param->uw_CurrentEvent != _pst_Track->uw_NumEvents - 1)
				{
					_pst_Param->f_Time = 0.0f;
					_pst_Param->uw_CurrentEvent++;
					_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
				}

				/* End of magic box track -> change anim action */
				if
				(
					_b_Engine
				&&	(TIM_gf_dt != 0.0f)
				&&	(_pst_Param->uw_CurrentEvent == _pst_Track->uw_NumEvents - 1)
				&&	(EVE_gpst_CurrentGAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
				&&	(EVE_gpst_CurrentGAO == EVE_gpst_OwnerGAO)
				&&	((pst_Current->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
				)
				{
					if(_pf_RestTime) *_pf_RestTime = f_RestDT;
					_pst_Param->f_Time = 0.0f;
					_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
					return TRUE;
				}
			}

#endif
			break;
		}

		/* Player back */
#ifdef ACTIVE_EDITORS
		if(TIM_gf_dt < 0.0f)
		{
			if(_pst_Param->uw_CurrentEvent == 0)
			{
				_pst_Param->f_Time = 0.0f;
				break;
			}
			else
			{
				_pst_Param->uw_CurrentEvent--;
				_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
				pst_Current = _pst_Track->pst_AllEvents + _pst_Param->uw_CurrentEvent;
				_pst_Param->f_Time = EVE_FrameToTime(EVE_NumFrames(pst_Current));
				continue;
			}
		}

#endif
		/* Pass to next event */
		_pst_Param->f_Time = 0.0f;

		/* This was the last event ? */
		if(_pst_Param->uw_CurrentEvent == _pst_Track->uw_NumEvents - 1)
		{
			if(_pst_Track->uw_Flags & EVE_C_Track_AutoLoop)
			{
				_pst_Param->uw_CurrentEvent = 0;
				_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
			}
			else if(_pst_Track->uw_Flags & EVE_C_Track_AutoStop)
			{
				_pst_Param->f_Time = EVE_FrameToTime(EVE_NumFrames(pst_Current));
				_pst_Param->uw_Flags &= ~EVE_C_Track_Running;
				break;
			}

			/* End of magic box track -> change anim action */
			if
			(
				(EVE_gpst_CurrentGAO == EVE_gpst_OwnerGAO)
			&&	(EVE_gpst_CurrentGAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
			)
			{
				if(_b_Engine && (b_UglyOptim ? 1 : ((pst_Current->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)) )
				{
					if(_pf_RestTime) *_pf_RestTime = f_RestDT;
					_pst_Param->f_Time = EVE_FrameToTime(EVE_NumFrames(pst_Current));
					_pst_Param->uw_CurrentEvent = _pst_Track->uw_NumEvents - 1;
					return TRUE;
				}
			}
		}

		/* Else pass to the next event */
		else
		{
			_pst_Param->uw_CurrentEvent++;
			_pst_Param->uw_Flags &= ~EVE_C_Track_EventDone;
		}

#ifdef ACTIVE_EDITORS
		ERR_X_Warning
		(
			(
				b_NonZeroEvent
			||	(
					!b_NonZeroEvent
				&&	(
						(i_Current != _pst_Param->uw_CurrentEvent)
					||	(EVE_NumFrames(&_pst_Track->pst_AllEvents[(_pst_Param->uw_CurrentEvent)]) >= 1)
					)
				)
			),
			"Event track seems to loop",
			NULL
		);
#endif
	}

	/* Rest time */
	if(_pf_RestTime) *_pf_RestTime = f_RestDT;

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SynchronizeTracks(ANI_st_GameObjectAnim *_pst_GOAnim)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	float			f_Time;
	EVE_tdst_Data	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = _pst_GOAnim->apst_Anim[0]->pst_Data;
	EVE_GetTracksTime(pst_Data, 0, &f_Time, NULL);
	EVE_SetTracksTime(pst_Data, f_Time);

	/* If we are in a Blend. We stop it. */
	if(_pst_GOAnim->uc_AnimUsed != 1) _pst_GOAnim->uc_AnimUsed = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char EVE_c_UpdateLOD(OBJ_tdst_GameObject *_pst_GO, BOOL _b_UpdateParity)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					c_Old_LOD;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	c_Old_LOD = pst_GOAnim->c_LOD_Dynamic & ~EVE_LOD_FrameParity;

	if(_b_UpdateParity)
		pst_GOAnim->c_LOD_Dynamic = (pst_GOAnim->c_LOD_Dynamic & EVE_LOD_FrameParity) ? 0 : EVE_LOD_FrameParity;

	if(pst_GOAnim->c_LOD_IA)
	{
		pst_GOAnim->c_LOD_Dynamic |= (pst_GOAnim->c_LOD_IA | EVE_LOD_OnlyMagicBoxWhenCulled);

		if((UCHAR) c_Old_LOD > (UCHAR) (pst_GOAnim->c_LOD_Dynamic & ~EVE_LOD_FrameParity))
			EVE_SynchronizeTracks(pst_GOAnim);

		return pst_GOAnim->c_LOD_Dynamic;
	}


	pst_GOAnim->c_LOD_Dynamic |= (EVE_LOD_OnlyMagicBoxWhenCulled) & pst_GOAnim->c_LOD_Bitfield;

	if(_pst_GO->uc_LOD_Vis < 10) pst_GOAnim->c_LOD_Dynamic |= (EVE_LOD_FreezeSkeleton) & pst_GOAnim->c_LOD_Bitfield;

	if(_pst_GO->uc_LOD_Vis < 25) pst_GOAnim->c_LOD_Dynamic |= (EVE_LOD_OneFrameOutOfTwo) & pst_GOAnim->c_LOD_Bitfield;

	if(_pst_GO->uc_LOD_Vis < 100)
	{
		pst_GOAnim->c_LOD_Dynamic |= (EVE_LOD_SkipTranslations + EVE_LOD_SkipExtremityBones) & pst_GOAnim->c_LOD_Bitfield;
	}

	if(_pst_GO->pst_Extended && (_pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipAnimT))
		pst_GOAnim->c_LOD_Dynamic |= EVE_LOD_SkipTranslations;


	if((UCHAR) c_Old_LOD > (UCHAR) (pst_GOAnim->c_LOD_Dynamic &~EVE_LOD_FrameParity))
		EVE_SynchronizeTracks(pst_GOAnim);

	return pst_GOAnim->c_LOD_Dynamic;
}

extern BOOL ENG_gb_ActiveSectorization;
extern int OBJ_ModifierType_C(OBJ_tdst_GameObject *, int);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVE_b_RunAnimation(EVE_tdst_Data *_pst_Data, float *_pf_RestTime)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks *pst_ListTracks;
	EVE_tdst_Params		*pst_Params;
	OBJ_tdst_GameObject *pst_BoneGO;
	OBJ_tdst_Group		*pst_Skeleton;
	MATH_tdst_Matrix	*pst_SaveRefMatrix;
	float				f_MagicBoxRestTimeToPlay;
	USHORT				uw_Gizmo;
	char				c_LOD;
	ULONG				i;
	BOOL				b_CulledOrInvisible;
	BOOL				b_ChangeAction;
	BOOL				b_AnimTrack;
	BOOL				b_MagicBoxReachedLastFrame;
	BOOL				b_MagicBox;
	BOOL				b_MainBone;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Data) return FALSE;

	pst_ListTracks = _pst_Data->pst_ListTracks;
	pst_Params = _pst_Data->pst_ListParam;
	if(!pst_ListTracks || !pst_Params) return FALSE;

	EVE_gpst_CurrentListTracks = pst_ListTracks;

	pst_SaveRefMatrix = EVE_gpst_CurrentRefMatrix;
	EVE_gpst_CurrentGAO = EVE_gpst_OwnerGAO;
	pst_Skeleton = EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton;


	/* Reinit de la position des os */
	if(pst_ListTracks->uw_NumTracks > 2)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
		OBJ_tdst_GameObject		*pst_BoneGO;
//		MATH_tdst_Vector		st_Temp;
		int						i;
//		float					Norm;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);	

		pst_CurrentBone ++;
		for(i = 1; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

			if(TAB_b_IsAHole(pst_BoneGO))
				continue;

			if
			(
				(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy))
			&& 	(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_HasInitialPos))
			&&	pst_BoneGO->pst_Base
			&&	pst_BoneGO->pst_Base->pst_Hierarchy
			&&	pst_BoneGO->pst_Base->pst_Hierarchy->pst_Father
			&& !(OBJ_ModifierType_C(pst_BoneGO, MDF_C_Modifier_Half_Angle))
			&& (!pst_BoneGO->pst_Extended || !(pst_BoneGO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_DontReinitPos))

			)
			{
				/*
				Norm = MATH_f_NormVector(&(pst_BoneGO->pst_GlobalMatrix + 1)->T);
				if(Norm != 0.0f)
				{
					MATH_SetNormVector(&st_Temp, &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, Norm);
					MATH_CopyVector(&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_Temp);
				}
				*/
				MATH_CopyVector(&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &(pst_BoneGO->pst_GlobalMatrix + 1)->T);
			}
		}
	}
	


	b_CulledOrInvisible = !(EVE_gb_ForcePlayerOnAllTracks) && ((EVE_gpst_CurrentGAO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) || !(EVE_gpst_CurrentGAO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible));

	if(b_CulledOrInvisible && EVE_gpst_CurrentGAO->pst_Extended && (EVE_gpst_CurrentGAO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AlwaysPlay))
		b_CulledOrInvisible = FALSE;

	if(EVE_gpst_CurrentGAO->uc_Secto) b_CulledOrInvisible = TRUE;

	c_LOD = EVE_c_UpdateLOD(EVE_gpst_OwnerGAO, TRUE);


	f_MagicBoxRestTimeToPlay = 0.0f;
	b_MagicBoxReachedLastFrame = FALSE;

	for(i = 0; i < pst_ListTracks->uw_NumTracks; i ++)
	{
		EVE_gpst_CurrentRefMatrix = pst_SaveRefMatrix;
		EVE_gpst_CurrentTrack = &pst_ListTracks->pst_AllTracks[i];
		EVE_gpst_CurrentParam = &pst_Params[i];

		if(EVE_gpst_CurrentTrack->pst_GO)
		{
			EVE_gpst_CurrentGAO = EVE_gpst_CurrentTrack->pst_GO;
			EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(EVE_gpst_CurrentGAO);
		}
		else
			EVE_gpst_CurrentGAO = EVE_gpst_OwnerGAO;


		b_MagicBox = (EVE_gpst_CurrentTrack->uw_Gizmo == 0xFFFF);
		b_MainBone = (EVE_gpst_CurrentTrack->uw_Gizmo == 0);

		/* Blend dont play Magic box tracks */
		if(!(EVE_gpst_CurrentParam->uw_Flags & EVE_C_Track_Running) || (EVE_gpst_CurrentParam->uw_Flags & EVE_C_Track_Pause)) 
			continue;

		b_AnimTrack = (EVE_gpst_CurrentTrack->uw_Flags & EVE_C_Track_Anims);

		/* Skip Translations LOD (Always keep MagicBox and MainBone tracks) */
		if
		(
			b_AnimTrack
		&&	(c_LOD & EVE_LOD_SkipTranslations)
		&&	!b_MagicBox
		&&	!b_MainBone
		&&	(EVE_w_Event_InterpolationKey_GetType(&EVE_gpst_CurrentTrack->pst_AllEvents[0]) & EVE_InterKeyType_Translation_Mask)
		)
		{
			continue;
		}

		/* Culled/Invisible */
		if(b_CulledOrInvisible && (c_LOD & EVE_LOD_OnlyMagicBoxWhenCulled) && !b_MagicBox)
		{
#ifdef _XENON_RENDER
            // Force anim playing because shadows need to be updated properly even
            // if subject is outside camera view
            if(!(EVE_gpst_OwnerGAO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_EmitShadowBuffer))
#endif
            {
				EVE_gpst_OwnerGAO->pst_Base->pst_GameObjectAnim->uc_Synchro = 0;
				goto CulledOrInvisibleOrFreeze;
			}
		}

		/* Freeze Skeleton LOD */
		if((c_LOD & EVE_LOD_FreezeSkeleton) && !b_MagicBox)
		{
			goto CulledOrInvisibleOrFreeze;
		}

		/* Extremity Bones LOD */
		if(b_AnimTrack && (c_LOD & EVE_LOD_SkipExtremityBones))
		{
			uw_Gizmo = EVE_gpst_CurrentTrack->uw_Gizmo;
			if(!b_MagicBox && (pst_Skeleton->pst_AllObjects->ul_NbElems > uw_Gizmo))
			{
				pst_BoneGO = (OBJ_tdst_GameObject *) (pst_Skeleton->pst_AllObjects->p_Table + uw_Gizmo)->p_Pointer;
				if
				(
					pst_BoneGO
				&&	pst_BoneGO->pst_Extended
				&&	(pst_BoneGO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_EndBone)
				) continue;
			}
		}

		/* We are maybe in a Partial Anim Play. Check if Gizmo is in the mask b4 playing it */
		if(EVE_gpul_PartialMask && b_AnimTrack && !ANI_b_GizmoInMask(EVE_gpul_PartialMask, EVE_gpst_CurrentTrack->uw_Gizmo))
			continue;

		if(b_AnimTrack && !(EVE_gpst_CurrentTrack->pst_GO))
		{
			b_ChangeAction = EVE_RunAnimTrack(EVE_gpst_CurrentTrack, EVE_gpst_CurrentParam, i, c_LOD, _pf_RestTime);
		}
		else
		{
			b_ChangeAction = EVE_RunOneTrack(&pst_ListTracks->pst_AllTracks[i], &pst_Params[i], TRUE, _pf_RestTime);
		}


		if((i == 0) || (EVE_gpul_PartialMask && !b_MagicBoxReachedLastFrame)) b_MagicBoxReachedLastFrame = b_ChangeAction;

		if(((i == 0)  || (EVE_gpul_PartialMask && !f_MagicBoxRestTimeToPlay)) && (b_ChangeAction))
		{
			f_MagicBoxRestTimeToPlay = *_pf_RestTime;
		}
	}

	*_pf_RestTime = f_MagicBoxRestTimeToPlay;
	return(b_MagicBoxReachedLastFrame);

	/*
	 * We loop back from the last track and play the AI tracks if any. ASAS we find
	 * an InterpolationKey track, we return
	 */
CulledOrInvisibleOrFreeze:
	for(i = pst_ListTracks->uw_NumTracks - 1; i > 0; i--)
	{
		if(pst_ListTracks->pst_AllTracks[i].uw_Flags & EVE_C_Track_Anims) break;

		EVE_gpst_CurrentTrack = &pst_ListTracks->pst_AllTracks[i];
		EVE_gpst_CurrentParam = &pst_Params[i];

		EVE_gpst_CurrentGAO = (EVE_gpst_CurrentTrack->pst_GO) ? EVE_gpst_CurrentTrack->pst_GO : EVE_gpst_OwnerGAO;
		EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(EVE_gpst_CurrentGAO);

		EVE_RunOneTrack(&pst_ListTracks->pst_AllTracks[i], &pst_Params[i], TRUE, _pf_RestTime);
	}

	*_pf_RestTime = f_MagicBoxRestTimeToPlay;
	return(b_MagicBoxReachedLastFrame);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVE_b_RunAllTracks(EVE_tdst_Data *_pst_Data, float *_pf_RestTime)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				i;
	EVE_tdst_ListTracks *pst_ListTracks;
	EVE_tdst_Params		*pst_Params;
	BOOL				b_AniTrack, b_NeedToChange;
	float				f_MagicRestTime;
	MATH_tdst_Matrix	*pst_SaveRefMatrix;
	BOOL				b_AnimCase, b_MagicBoxLoop;
	BOOL				b_MagicBox, b_MainBone;
	BOOL				b_AnimTrack;
	char				c_LOD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Data) return FALSE;
	pst_ListTracks = _pst_Data->pst_ListTracks;
	if(!pst_ListTracks) return FALSE;
	pst_Params = _pst_Data->pst_ListParam;
	if(!pst_Params) return FALSE;
	b_NeedToChange = FALSE;
	EVE_gb_FlashMatrixDone = FALSE;

	f_MagicRestTime = 0.0f;

	EVE_gpst_CurrentGAO = EVE_gpst_OwnerGAO;

	b_AnimCase =
		(
			!(ENG_gb_EVERunning) && (EVE_gpst_CurrentGAO->pst_Base)
		&&	(EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim) 
		&&	(EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
		);


	/* Reinit de la position des os (sauf en cas de recompute flash) */
//	if((pst_ListTracks->uw_NumTracks > 2) && b_AnimCase)
	if(0)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
		OBJ_tdst_Group			*pst_Skeleton;
		OBJ_tdst_GameObject		*pst_BoneGO;
		MATH_tdst_Vector		st_Temp;
		int						i;
		float					Norm;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Skeleton = EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);	

		pst_CurrentBone ++;
		for(i = 1; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

			if(TAB_b_IsAHole(pst_BoneGO))
				continue;

			if
			(
				(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy))
			&& 	(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_HasInitialPos))
			)
			{
				Norm = MATH_f_NormVector(&(pst_BoneGO->pst_GlobalMatrix + 1)->T);
				if(Norm != 0.0f)
				{
					MATH_SetNormVector(&st_Temp, &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, Norm);
					MATH_CopyVector(&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_Temp);
				}

			}
		}
	}


	if(b_AnimCase)
		c_LOD = EVE_c_UpdateLOD(EVE_gpst_CurrentGAO, FALSE);

	for(i = 0; i < pst_ListTracks->uw_NumTracks; i++)
	{
		/* If the GO is culled, we do not play non-magic Box Tracks (except AI tracks) */
		if
		(
			!EVE_gpst_CurrentGAO->pst_Extended
		||	!(EVE_gpst_CurrentGAO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AlwaysPlay)
		)
		{
			if
			(
				b_AnimCase
			&&	(i > 1)
			&&	!(EVE_gb_ForcePlayerOnAllTracks)
			&&	((EVE_gpst_CurrentGAO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) || !(EVE_gpst_CurrentGAO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible))
			&&	(pst_ListTracks->pst_AllTracks[i].pst_AllEvents)
			&&	((EVE_w_Event_InterpolationKey_GetFlags(pst_ListTracks->pst_AllTracks[i].pst_AllEvents) & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
			)
			{
				if((EVE_gpst_CurrentGAO->pst_Base) && (EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim))
					EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->uc_Synchro = 0;
				continue;
			}
		}


		b_MagicBox = (pst_ListTracks->pst_AllTracks[i].uw_Gizmo == 0xFFFF);
		b_MainBone = (pst_ListTracks->pst_AllTracks[i].uw_Gizmo == 0);

		b_AnimTrack = (pst_ListTracks->pst_AllTracks[i].uw_Flags & EVE_C_Track_Anims);

		/* Skip Translations LOD (Always keep MagicBox and MainBone tracks) */
		if
		(
			b_AnimTrack
		&&	(c_LOD & EVE_LOD_SkipTranslations)
		&&	!b_MagicBox
		&&	!b_MainBone
		&&	(EVE_w_Event_InterpolationKey_GetType(&pst_ListTracks->pst_AllTracks[i].pst_AllEvents[0]) & EVE_InterKeyType_Translation_Mask)
		)
		{
			continue;
		}

		/* We are maybe in a Partial Anim Play. Check if Gizmo is in the mask b4 playing it */
		if(EVE_gpul_PartialMask && b_AnimTrack && !ANI_b_GizmoInMask(EVE_gpul_PartialMask, pst_ListTracks->pst_AllTracks[i].uw_Gizmo))
			continue;

		pst_SaveRefMatrix = EVE_gpst_CurrentRefMatrix;

		if((pst_Params[i].uw_Flags & EVE_C_Track_Running) && (!(pst_Params[i].uw_Flags & EVE_C_Track_Pause)))
		{
			EVE_gpst_CurrentTrack = &pst_ListTracks->pst_AllTracks[i];
			EVE_gpst_CurrentParam = &pst_Params[i];
			if(EVE_gpst_CurrentTrack->pst_GO)
			{
				EVE_gpst_CurrentGAO = EVE_gpst_CurrentTrack->pst_GO;
				EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(EVE_gpst_CurrentGAO);
			}
			else
				EVE_gpst_CurrentGAO = EVE_gpst_OwnerGAO;

			/* Scan track */
			EVE_gb_NeedToReparse = FALSE;

			b_AniTrack = EVE_RunOneTrack(&pst_ListTracks->pst_AllTracks[i], &pst_Params[i], TRUE, _pf_RestTime);

			if(i == 0) b_MagicBoxLoop = b_AniTrack;

#if 0
			/* A Track has looped but NOT the Magic Box !!! ---> Bug */
			if
			(
				(i > 0)
			&&	(EVE_gpst_CurrentData->pst_ListParam[0].uw_Flags & EVE_C_Track_Running)
			&&	!b_MagicBoxLoop
			&&	(pst_ListTracks->pst_AllTracks[i].uw_Flags & EVE_C_Track_Anims)
			&&	b_AniTrack
			)
			{
				LINK_PrintStatusMsg("Track desynchronization error");
			}

#endif
			if(b_AniTrack)
			{
				b_NeedToChange = TRUE;

				/*
				 * There is no reason to take the Magic Box first track remaining time. The only
				 * reason is that it is the first one.
				 */
				if((i == 0) && _pf_RestTime) f_MagicRestTime = *_pf_RestTime;
			}

			/* Need to reparse all tracks cause of set time ? */
			if(EVE_gb_NeedToReparse)
			{
				i = (ULONG) - 1;
			}

			/* If the Event Player moves an GO with Anim, we have to update its FlashMatrix; */
			if(ENG_gb_EVERunning)
			{
				if(EVE_gpst_CurrentGAO->ul_IdentityFlags & OBJ_C_IdentityFlag_FlashMatrix)
				{
					ENG_gb_EVERunning = FALSE;
					ANI_UpdateFlash(EVE_gpst_CurrentGAO, ANI_C_UpdateAllAnims);
					ENG_gb_EVERunning = TRUE;
				}

				if(OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Dyna))
					EVE_gpst_CurrentGAO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColDisable;
			}
		}

		EVE_gpst_CurrentRefMatrix = pst_SaveRefMatrix;
	}

	if(_pf_RestTime) *_pf_RestTime = f_MagicRestTime;
	return b_NeedToChange;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_MainCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_EOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(L_setjmp(AI_gst_ContextCheck))
	{
		ERR_X_ForceError("This is an event error...", NULL);
		L_longjmp(AI_gst_ContextOut, 1);
		return;
	}

	ERR_gpst_ContextGAO = NULL;
	ERR_gpsz_ContextString = "Events";
#endif
	pst_EOT = &(_pst_World->st_EOT.st_Events);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_EOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_EOT);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;

#ifdef ACTIVE_EDITORS
		ERR_gpst_ContextGAO = pst_GAO;
#endif
		if(pst_GAO->pst_Extended && pst_GAO->pst_Extended->pst_Events)
		{
			/* Optim culled ? */
			if(pst_GAO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_OptimCulled && !pst_GAO->uc_LOD_Vis) continue;

			/* Check that the object has not been desactivated y a previous call of this loop */
			if(!OBJ_b_TestControlFlag(pst_GAO, OBJ_C_ControlFlag_ForceInactive))
			{
				EVE_gpst_OwnerGAO = pst_GAO;
				EVE_gpst_CurrentData = pst_GAO->pst_Extended->pst_Events;
				EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
				EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(pst_GAO);
				EVE_b_RunAllTracks(EVE_gpst_CurrentData, NULL);
			}
		}
	}

	/* Now event player is finished */
	EVE_gpst_CurrentTrack = NULL;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_AddTime
(
	OBJ_tdst_GameObject *_pst_GO,
	EVE_tdst_ListTracks *_pst_List,
	EVE_tdst_Params		*_pst_Param,
	EVE_tdst_Track		*_pst_Track,
	float				_f_Time
)
{
	/*~~~~~~~~~~~~~~~*/
	float	f_DT;
	BOOL	b_AnimCase;
	/*~~~~~~~~~~~~~~~*/

	/* if(!_f_Time) return; */
	if(!_pst_GO) return;

	b_AnimCase = FALSE;

	EVE_gpst_OwnerGAO = _pst_GO;
	EVE_gpst_CurrentListTracks = _pst_List;
	EVE_gpst_CurrentTrack = _pst_Track;
	EVE_gpst_CurrentParam = _pst_Param;
	if(_pst_Track->pst_GO)
		EVE_gpst_CurrentGAO = _pst_Track->pst_GO;
	else
		EVE_gpst_CurrentGAO = EVE_gpst_OwnerGAO;

	f_DT = TIM_gf_dt;
	TIM_gf_dt = _f_Time;
	EVE_gb_NeedToReparse = FALSE;

	/* Reference matrix */
	EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(EVE_gpst_CurrentGAO);

	if
	(
		!(_pst_GO->pst_Extended
		&&	_pst_GO->pst_Extended->pst_Events
		&&	(_pst_GO->pst_Extended->pst_Events->pst_ListTracks == _pst_List)
		)
	) b_AnimCase = TRUE;

#ifdef ACTIVE_EDITORS
	if(L_setjmp(AI_gst_ContextCheck))
	{
		ERR_X_ForceError("This is an event error...", NULL);
		EVE_gb_AddTime = FALSE;
		EVE_gpst_CurrentTrack = NULL;
		TIM_gf_dt = f_DT;
		return;
	}

	ERR_gpst_ContextGAO = _pst_GO;
	ERR_gpsz_ContextString = "Events";
#endif
	EVE_gb_AddTime = TRUE;
	EVE_RunOneTrack(_pst_Track, _pst_Param, FALSE, NULL);
	EVE_gb_AddTime = FALSE;
	EVE_gpst_CurrentTrack = NULL;
	TIM_gf_dt = f_DT;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Track *EVE_pst_GizmoToTrack(EVE_tdst_ListTracks *_pst_List, OBJ_tdst_GameObject *_pst_Gizmo)
{
	/*~~~~~~~~~*/
	int		inum;
	char	*pz;
	int		i;
	/*~~~~~~~~~*/

	if(_pst_Gizmo->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
	{
		pz = _pst_Gizmo->sz_Name + L_strlen(_pst_Gizmo->sz_Name) - 1;
		if(*pz == ']')
		{
			while(*pz != '[') pz--;
			pz--;
			while(L_isspace(*pz)) pz--;
		}

		while(L_isdigit(*pz)) pz--;
		pz++;
		inum = L_atoi(pz);
		for(i = 0; i < _pst_List->uw_NumTracks; i++)
		{
			if(_pst_List->pst_AllTracks[i].uw_Gizmo == inum) return &_pst_List->pst_AllTracks[i];
		}
	}

	return _pst_List->pst_AllTracks;
}

/*
 =======================================================================================================================
    return time from start of a track to start of an event. return a negative value if given event isn't found in given
    track
 =======================================================================================================================
 */
float EVE_f_GetTimeFromStartToEvent(EVE_tdst_Track *_pst_Track, struct EVE_tdst_Event_ *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Cur, *pst_Last;
	float			f_Time;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cur = _pst_Track->pst_AllEvents;
	pst_Last = pst_Cur + _pst_Track->uw_NumEvents;

	for(f_Time = 0; pst_Cur < pst_Last; pst_Cur++)
	{
		if(pst_Cur == _pst_Event) return f_Time;
		f_Time += EVE_FrameToTime(EVE_NumFrames(pst_Cur));
	}

	return -1;
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
