/*$T EVEinit.c GC! 1.081 04/04/03 11:53:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "EVEstruct.h"
#include "EVEinit.h"
#include "EVEnt_interpolationkey.h"
#include "EVEconst.h"
#include "EVEplay.h"
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/AIengine.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "EDIpaths.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"

#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
BOOL		EVE_gb_NeedToRecomputeSND = FALSE;
BOOL		EVE_gb_EndNeedToRecomputeSND = FALSE;
#endif
extern int	ANI_gi_ImportMode;


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_FreeTrack(EVE_tdst_Track *_pst_Track)
{
	if(!_pst_Track->pst_AllEvents) return;

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~*/
		USHORT	i;
		/*~~~~~~*/

		/* Free all events specific data */
		for(i = 0; i < _pst_Track->uw_NumEvents; i++)
		{
			if
			(
				_pst_Track->pst_AllEvents[i].p_Data
			&&	!(EVE_w_Event_InterpolationKey_GetFlags(&_pst_Track->pst_AllEvents[i]) & EVE_C_EventFlag_Symetric)
			) MEM_Free(_pst_Track->pst_AllEvents[i].p_Data);
		}
	}

#else
#ifdef ANIMS_USE_ARAM
	if(_pst_Track && _pst_Track->pc_AllData && !(_pst_Track->w_Type & EVE_C_Track_UseARAM))
	{
		MEM_Free(_pst_Track->pc_AllData);
	}

#else
	if(_pst_Track->pc_AllData) MEM_Free(_pst_Track->pc_AllData);
#endif
#endif
#ifdef ANIMS_USE_ARAM
	if(_pst_Track->w_Type & EVE_C_Track_UseARAM)
	{
		if(_pst_Track->w_Type & EVE_C_Track_InRAMGC) MEM_Free(_pst_Track->pst_AllEvents);
	}
	else
		MEM_Free(_pst_Track->pst_AllEvents);
#else
	MEM_Free(_pst_Track->pst_AllEvents);
#endif
	_pst_Track->pst_AllEvents = NULL;
	_pst_Track->pc_AllData = NULL;
}

#ifdef ANIMS_USE_ARAM
extern int	ARAM_Anims_RAM;
extern int	ARAM_Anims_ARAM;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_DeleteListTracks(struct EVE_tdst_ListTracks_ *_pst_List)
{
	/*~~~~~~*/
	ULONG	i;
	extern BOOL LOA_b_IsSpecialAddrToDestroy(ULONG ul_Addr);
	/*~~~~~~*/

	if(!_pst_List) return;

	if(
		(LOA_b_IsSpecialAddrToDestroy((ULONG)_pst_List) || ENG_gb_ExitApplication) && 
		(_pst_List->ul_NbOfInstances > 1000000)
	  )
	{
		_pst_List->ul_NbOfInstances -= 1000000;
	}

	_pst_List->ul_NbOfInstances--;

#ifdef ANIMS_USE_ARAM
	if((_pst_List->ul_NbOfInstances == 1000000) && (_pst_List->ul_GC_Flags & EVE_C_ListTracks_InRAMGC))
	{
		for(i = 0; i < _pst_List->uw_NumTracks; i++)
		{
			if(_pst_List->pst_AllTracks[i].w_Type & EVE_C_Track_InRAMGC)
			{
				MEM_Free(_pst_List->pst_AllTracks[i].pst_AllEvents);
				_pst_List->pst_AllTracks[i].pst_AllEvents = _pst_List->pst_Cache->ppst_ListEvents[i];
				_pst_List->pst_AllTracks[i].w_Type &= ~EVE_C_Track_InRAMGC;
			}
		}
		
		ARAM_Anims_RAM--;		

		MEM_Free(_pst_List->pst_Cache->ppst_ListEvents);
		_pst_List->pst_Cache->pst_TrackList = NULL;
		_pst_List->ul_GC_Flags &= ~EVE_C_ListTracks_InRAMGC;
		_pst_List->pst_Cache = NULL;
	}

#endif
#ifdef ACTIVE_EDITORS
	if(_pst_List->ul_NbOfInstances == -1) ERR_X_Warning(0, "List Track desallocation Warning", NULL);
#endif
	if(!_pst_List->ul_NbOfInstances || _pst_List->ul_NbOfInstances == 0xFFFFFFFF)
	{
		for(i = 0; i < _pst_List->uw_NumTracks; i++) EVE_FreeTrack(&_pst_List->pst_AllTracks[i]);

		if(_pst_List->pst_AllTracks) MEM_Free(_pst_List->pst_AllTracks);

		LOA_DeleteAddress(_pst_List);
		LINK_DelRegisterPointer(_pst_List);
		MEM_Free(_pst_List);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_InitData(struct EVE_tdst_Data_ *_pst_Data, EVE_tdst_ListTracks *_pst_TrackList)
{
	if(_pst_Data->pst_ListTracks && _pst_Data->pst_ListParam)
	{
		if(_pst_Data->pst_ListTracks->uw_NumTracks < _pst_TrackList->uw_NumTracks)
		{
			_pst_Data->pst_ListParam = (EVE_tdst_Params *) MEM_p_Realloc
				(
					_pst_Data->pst_ListParam,
					_pst_TrackList->uw_NumTracks * sizeof(EVE_tdst_Params)
				);
			L_memset(_pst_Data->pst_ListParam, 0, _pst_TrackList->uw_NumTracks * sizeof(EVE_tdst_Params));
		}
		else
		{
			L_memset(_pst_Data->pst_ListParam, 0, _pst_Data->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Params));
		}
	}
	else
	{
		if(_pst_Data->pst_ListParam)
		{
			MEM_Free(_pst_Data->pst_ListParam);
			_pst_Data->pst_ListParam = NULL;
		}

		_pst_Data->pst_ListParam = (EVE_tdst_Params *) MEM_p_Alloc(_pst_TrackList->uw_NumTracks * sizeof(EVE_tdst_Params));
		L_memset(_pst_Data->pst_ListParam, 0, _pst_TrackList->uw_NumTracks * sizeof(EVE_tdst_Params));
	}

	_pst_Data->pst_ListTracks = _pst_TrackList;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_AllocateMainStruct(OBJ_tdst_GameObject *_pst_GO)
{
	ERR_X_Assert(_pst_GO->pst_Extended);

	_pst_GO->pst_Extended->pst_Events = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
	L_memset(_pst_GO->pst_Extended->pst_Events, 0, sizeof(EVE_tdst_Data));

	_pst_GO->pst_Extended->pst_Events->pst_ListTracks = (EVE_tdst_ListTracks *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));
	L_memset(_pst_GO->pst_Extended->pst_Events->pst_ListTracks, 0, sizeof(EVE_tdst_ListTracks));
	_pst_GO->pst_Extended->pst_Events->pst_ListTracks->ul_NbOfInstances = 1;
	_pst_GO->pst_Extended->pst_Events->pst_ListParam = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_FreeMainStruct(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = _pst_GO->pst_Extended->pst_Events;
	if(!pst_Data) return;

	if(pst_Data->pst_ListParam) MEM_Free(pst_Data->pst_ListParam);
	if(pst_Data->pst_ListTracks) EVE_DeleteListTracks(pst_Data->pst_ListTracks);
	MEM_Free(pst_Data);

	_pst_GO->pst_Extended->pst_Events = NULL;
}

/*
 =======================================================================================================================
    Aim: For Object Duplication ...
 =======================================================================================================================
 */
EVE_tdst_Data *EVE_pst_DuplicateData(struct EVE_tdst_Data_ *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Ret;
	int				i, j;
	EVE_tdst_Track	*pst_Track;
	void			*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ret = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
	pst_Ret->pst_ListParam = NULL;
	pst_Ret->pst_ListTracks = NULL;

	/* List of params */
	if(_pst_Data && _pst_Data->pst_ListParam)
	{
		pst_Ret->pst_ListParam = (struct EVE_tdst_Params_ *) MEM_p_Alloc(_pst_Data->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Params));
		L_memcpy
		(
			pst_Ret->pst_ListParam,
			_pst_Data->pst_ListParam,
			_pst_Data->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Params)
		);
	}

	/* List of tracks */
	if(_pst_Data && _pst_Data->pst_ListTracks)
	{
		pst_Ret->pst_ListTracks = (struct EVE_tdst_ListTracks_ *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));
		L_memcpy(pst_Ret->pst_ListTracks, _pst_Data->pst_ListTracks, sizeof(EVE_tdst_ListTracks));

		if(_pst_Data->pst_ListTracks->pst_AllTracks)
		{
			pst_Ret->pst_ListTracks->pst_AllTracks = (EVE_tdst_Track *) MEM_p_Alloc(_pst_Data->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Track));
			L_memcpy
			(
				pst_Ret->pst_ListTracks->pst_AllTracks,
				_pst_Data->pst_ListTracks->pst_AllTracks,
				_pst_Data->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Track)
			);

			pst_Track = pst_Ret->pst_ListTracks->pst_AllTracks;
			for(i = 0; i < pst_Ret->pst_ListTracks->uw_NumTracks; i++)
			{
				if(pst_Track->pst_AllEvents)
				{
					pst_Track->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Alloc(pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event));
					L_memcpy
					(
						pst_Track->pst_AllEvents,
						_pst_Data->pst_ListTracks->pst_AllTracks[i].pst_AllEvents,
						pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event)
					);

					for(j = 0; j < pst_Track->uw_NumEvents; j++)
					{
						if(pst_Track->pst_AllEvents[j].p_Data)
						{
							p = MEM_p_Alloc(*(short *) pst_Track->pst_AllEvents[j].p_Data);
							L_memcpy
							(
								p,
								pst_Track->pst_AllEvents[j].p_Data,
								*(short *) pst_Track->pst_AllEvents[j].p_Data
							);
							pst_Track->pst_AllEvents[j].p_Data = p;
						}
					}
				}

#ifndef ACTIVE_EDITORS
				if(pst_Track->pc_AllData)
				{
					pst_Track->pc_AllData = (char *) MEM_p_Alloc(pst_Track->ul_DataLength);
					L_memcpy
					(
						pst_Track->pc_AllData,
						_pst_Data->pst_ListTracks->pst_AllTracks[i].pc_AllData,
						pst_Track->ul_DataLength
					);
				}

#endif
				pst_Track++;
			}
		}
	}

	return pst_Ret;
}

/*
 =======================================================================================================================
    Aim: For Object Duplication ...
 =======================================================================================================================
 */
EVE_tdst_ListTracks *EVE_pst_DuplicateListTracks(struct EVE_tdst_Data_ *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks		*pst_Ret;
	int						i, j;
	EVE_tdst_Track			*pst_Track;
	void					*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* List of tracks */
	if(_pst_Data && _pst_Data->pst_ListTracks)
	{
		pst_Ret = (struct EVE_tdst_ListTracks_ *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));
		L_memcpy(pst_Ret, _pst_Data->pst_ListTracks, sizeof(EVE_tdst_ListTracks));

		if(_pst_Data->pst_ListTracks->pst_AllTracks)
		{
			pst_Ret->pst_AllTracks = (EVE_tdst_Track *) MEM_p_Alloc(_pst_Data->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Track));
			L_memcpy
			(
				pst_Ret->pst_AllTracks,
				_pst_Data->pst_ListTracks->pst_AllTracks,
				_pst_Data->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Track)
			);

			pst_Track = pst_Ret->pst_AllTracks;
			for(i = 0; i < pst_Ret->uw_NumTracks; i++)
			{
				if(pst_Track->pst_AllEvents)
				{
					pst_Track->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Alloc(pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event));
					L_memcpy
					(
						pst_Track->pst_AllEvents,
						_pst_Data->pst_ListTracks->pst_AllTracks[i].pst_AllEvents,
						pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event)
					);

					for(j = 0; j < pst_Track->uw_NumEvents; j++)
					{
						if(pst_Track->pst_AllEvents[j].p_Data)
						{
							p = MEM_p_Alloc(*(short *) pst_Track->pst_AllEvents[j].p_Data);
							L_memcpy
							(
								p,
								pst_Track->pst_AllEvents[j].p_Data,
								*(short *) pst_Track->pst_AllEvents[j].p_Data
							);
							pst_Track->pst_AllEvents[j].p_Data = p;
						}
					}
				}

#ifndef ACTIVE_EDITORS
				if(pst_Track->pc_AllData)
				{
					pst_Track->pc_AllData = (char *) MEM_p_Alloc(pst_Track->ul_DataLength);
					L_memcpy
					(
						pst_Track->pc_AllData,
						_pst_Data->pst_ListTracks->pst_AllTracks[i].pc_AllData,
						pst_Track->ul_DataLength
					);
				}

#endif
				pst_Track++;
			}
		}
	}

	return pst_Ret;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Event *EVE_pst_NewEvent(EVE_tdst_Track *_pst_Track, int _i_Pos, int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Event;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Track->uw_NumEvents++;
	if(_pst_Track->uw_NumEvents == 1)
	{
		_pst_Track->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Alloc(sizeof(EVE_tdst_Event));
	}
	else
	{
		_pst_Track->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Realloc
			(
				_pst_Track->pst_AllEvents,
				sizeof(EVE_tdst_Event) * _pst_Track->uw_NumEvents
			);
	}

	if(_i_Pos == -1)
	{
		pst_Event = &_pst_Track->pst_AllEvents[_pst_Track->uw_NumEvents - 1];
		L_memset(pst_Event, 0, sizeof(EVE_tdst_Event));
	}
	else
	{
		L_memmove
		(
			_pst_Track->pst_AllEvents + _i_Pos + 1,
			_pst_Track->pst_AllEvents + _i_Pos,
			(_pst_Track->uw_NumEvents - _i_Pos - 1) * sizeof(EVE_tdst_Event)
		);
		pst_Event = &_pst_Track->pst_AllEvents[_i_Pos];
		L_memset(pst_Event, 0, sizeof(EVE_tdst_Event));
	}

	/* Base init */
	pst_Event->w_Flags = _i_Type;
	pst_Event->uw_NumFrames = 60;
	return pst_Event;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_DeleteEvent(EVE_tdst_Track *_pst_Track, int _i_Pos)
{
	/* Free user data */
	if
	(
		_pst_Track->pst_AllEvents[_i_Pos].p_Data
	&&	!(_pst_Track->pst_AllEvents[_i_Pos].w_Flags & EVE_C_EventFlag_Symetric)
	)

	{
		MEM_Free(_pst_Track->pst_AllEvents[_i_Pos].p_Data);
	}

	/* Move if necessary */
	if(_i_Pos < _pst_Track->uw_NumEvents - 1)
	{
		L_memmove
		(
			_pst_Track->pst_AllEvents + _i_Pos,
			_pst_Track->pst_AllEvents + _i_Pos + 1,
			(_pst_Track->uw_NumEvents - _i_Pos - 1) * sizeof(EVE_tdst_Event)
		);
	}

	/* Reallocate list of events */
	_pst_Track->uw_NumEvents--;
	if(!_pst_Track->uw_NumEvents)
	{
		MEM_Free(_pst_Track->pst_AllEvents);
		_pst_Track->pst_AllEvents = NULL;
	}
	else
	{
		_pst_Track->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Realloc
			(
				_pst_Track->pst_AllEvents,
				sizeof(EVE_tdst_Event) * _pst_Track->uw_NumEvents
			);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Track *EVE_pst_NewTrack(EVE_tdst_Data *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Data->pst_ListTracks->uw_NumTracks++;
	if(_pst_Data->pst_ListTracks->uw_NumTracks == 1)
	{
		_pst_Data->pst_ListTracks->pst_AllTracks = (EVE_tdst_Track *) MEM_p_Alloc(sizeof(EVE_tdst_Track));
		_pst_Data->pst_ListParam = (EVE_tdst_Params *) MEM_p_Alloc(sizeof(EVE_tdst_Params));
	}
	else
	{
		_pst_Data->pst_ListTracks->pst_AllTracks = (EVE_tdst_Track *) MEM_p_Realloc
			(
				_pst_Data->pst_ListTracks->pst_AllTracks,
				sizeof(EVE_tdst_Track) * _pst_Data->pst_ListTracks->uw_NumTracks
			);
		_pst_Data->pst_ListParam = (EVE_tdst_Params *) MEM_p_Realloc
			(
				_pst_Data->pst_ListParam,
				sizeof(EVE_tdst_Params) * _pst_Data->pst_ListTracks->uw_NumTracks
			);
	}

	pst_Track = &_pst_Data->pst_ListTracks->pst_AllTracks[_pst_Data->pst_ListTracks->uw_NumTracks - 1];
	L_memset(pst_Track, 0, sizeof(EVE_tdst_Track));
	L_memset(&_pst_Data->pst_ListParam[_pst_Data->pst_ListTracks->uw_NumTracks - 1], 0, sizeof(EVE_tdst_Params));

	/* By default, a track is looping */
	pst_Track->uw_Flags |= EVE_C_Track_AutoLoop;
	pst_Track->uw_Gizmo = 0xFFFF;

	return pst_Track;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_DeleteTrack(EVE_tdst_Data *_pst_Data, int _i_Num)
{
	/* Delete all events */
	EVE_FreeTrack(&_pst_Data->pst_ListTracks->pst_AllTracks[_i_Num]);

	/* Move if necessary */
	if(_i_Num != _pst_Data->pst_ListTracks->uw_NumTracks - 1)
	{
		L_memmove
		(
			_pst_Data->pst_ListTracks->pst_AllTracks + _i_Num,
			_pst_Data->pst_ListTracks->pst_AllTracks + _i_Num + 1,
			(_pst_Data->pst_ListTracks->uw_NumTracks - _i_Num - 1) * sizeof(EVE_tdst_Track)
		);
		L_memmove
		(
			_pst_Data->pst_ListParam + _i_Num,
			_pst_Data->pst_ListParam + _i_Num + 1,
			(_pst_Data->pst_ListTracks->uw_NumTracks - _i_Num - 1) * sizeof(EVE_tdst_Params)
		);
	}

	/* Reallocate list of events */
	_pst_Data->pst_ListTracks->uw_NumTracks--;
	if(!_pst_Data->pst_ListTracks->uw_NumTracks)
	{
		MEM_Free(_pst_Data->pst_ListTracks->pst_AllTracks);
		MEM_Free(_pst_Data->pst_ListParam);
		_pst_Data->pst_ListTracks->pst_AllTracks = NULL;
		_pst_Data->pst_ListParam = NULL;
	}
	else
	{
		_pst_Data->pst_ListTracks->pst_AllTracks = (EVE_tdst_Track *) MEM_p_Realloc
			(
				_pst_Data->pst_ListTracks->pst_AllTracks,
				sizeof(EVE_tdst_Track) * _pst_Data->pst_ListTracks->uw_NumTracks
			);
		_pst_Data->pst_ListParam = (EVE_tdst_Params *) MEM_p_Realloc
			(
				_pst_Data->pst_ListParam,
				sizeof(EVE_tdst_Params) * _pst_Data->pst_ListTracks->uw_NumTracks
			);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EVE_RotationTrackOptimization
(
	EVE_tdst_Track	*_pst_Track,
	USHORT			_uw_StartEvent,
	USHORT			_uw_EndEvent,
	float			_f_Epsilon
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT				i, uw_EndEvent;
	MATH_tdst_Matrix	*pst_PreviousM, *pst_CurrentM, *pst_NextM;
	MATH_tdst_Matrix	st_InterpoledM;
	EVE_tdst_Event		*pst_PreviousEvent, *pst_CurrentEvent, *pst_NextEvent;
	float				t;
	ULONG				ul_AnnihilatedKeys;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = _uw_StartEvent + 1;
	uw_EndEvent = _uw_EndEvent;
	ul_AnnihilatedKeys = 0;
	while(i < uw_EndEvent - 1)
	{
		pst_PreviousEvent = _pst_Track->pst_AllEvents + i - 1;
		pst_CurrentEvent = _pst_Track->pst_AllEvents + i;
		pst_NextEvent = _pst_Track->pst_AllEvents + i + 1;

		pst_PreviousM = EVE_pst_Event_InterpolationKey_GetRotation(pst_PreviousEvent);
		pst_CurrentM = EVE_pst_Event_InterpolationKey_GetRotation(pst_CurrentEvent);
		pst_NextM = EVE_pst_Event_InterpolationKey_GetRotation(pst_NextEvent);

		if(!pst_PreviousM || !pst_CurrentM || !pst_NextM)
		{
			i ++;
			continue;
		}

		MATH_SetCorrectType(pst_CurrentM);
		t = (float) (EVE_NumFrames(pst_PreviousEvent) / (EVE_NumFrames(pst_PreviousEvent) + EVE_NumFrames(pst_CurrentEvent)));
		MATH_MatrixBlend(&st_InterpoledM, pst_PreviousM, pst_NextM, t, 1);
		if(MATH_b_EqMatrixWithEpsilon(&st_InterpoledM, pst_CurrentM, _f_Epsilon))
		{
			pst_PreviousEvent->uw_NumFrames += EVE_NumFrames(pst_CurrentEvent);
			EVE_DeleteEvent(_pst_Track, i);
			ul_AnnihilatedKeys++;
			uw_EndEvent--;
		}
		else
			i++;
	}

	return ul_AnnihilatedKeys;
}

int	ul_AnnihilatedKeys = 0;
int	totalkeyafteroptim = 0;
int Test2000 = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EVE_QuaternionTrackOptimization2
(
	EVE_tdst_Track	*_pst_Track,
	float			_f_Epsilon
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT					i;
	MATH_tdst_Quaternion	st_PreviousQ, st_CurrentQ, st_NextQ, st_InterpoledQ;
	EVE_tdst_Event			*pst_PreviousEvent, *pst_CurrentEvent, *pst_NextEvent;
	float					t, fPrecision;
	MATH_tdst_Quaternion	st_ConjQ, st_TempQ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = 1;
	while(i < _pst_Track->uw_NumEvents - 1)
	{
		pst_PreviousEvent = _pst_Track->pst_AllEvents + i - 1;
		pst_CurrentEvent = _pst_Track->pst_AllEvents + i;
		pst_NextEvent = _pst_Track->pst_AllEvents + i + 1;

		EVE_Event_InterpolationKey_GetQuaternion(pst_PreviousEvent, &st_PreviousQ);
		EVE_Event_InterpolationKey_GetQuaternion(pst_CurrentEvent, &st_CurrentQ);
		EVE_Event_InterpolationKey_GetQuaternion(pst_NextEvent, &st_NextQ);

		t = ((float) EVE_NumFrames(pst_PreviousEvent) / (EVE_NumFrames(pst_PreviousEvent) + EVE_NumFrames(pst_CurrentEvent)));

		/* Quaternion Blend */
		{
			/*~~~~~~~~~~~~~~~~~*/
			float	fTheta;
			float	fInvSinTheta;
			float	fCosTheta;
			/*~~~~~~~~~~~~~~~~~*/

			fTheta = 1.57f;
			fInvSinTheta = 1.0f;
			fCosTheta = MATH_f_QuaternionDotProduct(&st_PreviousQ, &st_NextQ);
			if(fCosTheta < 0.0f)
			{
				MATH_NegQuaternion(&st_NextQ);
				fCosTheta = -fCosTheta;
			}

			if(fCosTheta > 1.0f)  fCosTheta = 1.0f;

			if(fAbs(fCosTheta - 1.0f) > 1E-7f)
			{
				fTheta = fAcos(fCosTheta);
				fInvSinTheta = fInv(fSin(fTheta));
			}
			else
			{
				fTheta = 0.0f;
				fInvSinTheta = 0.0f;
			}

			MATH_Slerp(&st_InterpoledQ, t, &st_PreviousQ, &st_NextQ, fTheta, fInvSinTheta);
		}

		/* conjugué */
		st_ConjQ.x = -st_CurrentQ.x;
		st_ConjQ.y = -st_CurrentQ.y;
		st_ConjQ.z= -st_CurrentQ.z;
		st_ConjQ.w = st_CurrentQ.w;

		MATH_MulQuaternion(&st_TempQ, &st_InterpoledQ, &st_ConjQ);

		if(st_TempQ.w < 0.0f)
			MATH_NegQuaternion(&st_TempQ);


		/*
		if((_pst_Track->uw_Gizmo == 0xFFFF) || (_pst_Track->uw_Gizmo == 0))
			fPrecision = 0.000001f;
		else
			fPrecision = 0.0001f;

		if((_pst_Track->uw_Gizmo == 32))
			fPrecision = 0.0001f;
		else
		*/
			fPrecision = 0.000001f;

		if(st_TempQ.w >= 1.0f - fPrecision)
		{
			pst_PreviousEvent->uw_NumFrames += EVE_NumFrames(pst_CurrentEvent);
			EVE_DeleteEvent(_pst_Track, i);
			ul_AnnihilatedKeys++;
		}
		else
		{
			i++;
		}
	}

	/* compute track num of frames after optim */
	i = 0;
	while(i < _pst_Track->uw_NumEvents)
	{

		totalkeyafteroptim ++ ;
		i ++;
	}

	return ul_AnnihilatedKeys;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EVE_QuaternionTrackOptimization
(
	EVE_tdst_Track	*_pst_Track,
	float			_f_Epsilon
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT					i, j, uw_Frame;
	MATH_tdst_Quaternion	st_PreviousQ, st_CurrentQ, st_NextQ, st_InterpoledQ;
	MATH_tdst_Matrix		st_CurrentM;
	EVE_tdst_Event			*pst_PreviousEvent, *pst_CurrentEvent, *pst_NextEvent;
	float					t, f_MaxError, f_XError, f_YError, f_ZError, f_Error;
	BOOL					b_30HzOptim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = 1;
	while(i < _pst_Track->uw_NumEvents - 1)
	{
		uw_Frame = 0;
		for(j = 0; j < i; j++)
		{
			uw_Frame += EVE_NumFrames(_pst_Track->pst_AllEvents + j);
		}

		pst_PreviousEvent = _pst_Track->pst_AllEvents + i - 1;
		pst_CurrentEvent = _pst_Track->pst_AllEvents + i;
		pst_NextEvent = _pst_Track->pst_AllEvents + i + 1;

		EVE_Event_InterpolationKey_GetQuaternion(pst_PreviousEvent, &st_PreviousQ);
		EVE_Event_InterpolationKey_GetQuaternion(pst_CurrentEvent, &st_CurrentQ);
		EVE_Event_InterpolationKey_GetQuaternion(pst_NextEvent, &st_NextQ);

		/*$off*/
		f_XError = (float) fabs(st_PreviousQ.x - st_CurrentQ.x) > (float) fabs(st_NextQ.x - st_CurrentQ.x) ? (float) fabs(st_PreviousQ.x - st_CurrentQ.x) : (float) fabs(st_NextQ.x - st_CurrentQ.x);
		f_YError = (float) fabs(st_PreviousQ.y - st_CurrentQ.y) > (float) fabs(st_NextQ.y - st_CurrentQ.y) ? (float) fabs(st_PreviousQ.y - st_CurrentQ.y) : (float) fabs(st_NextQ.y - st_CurrentQ.y);
		f_ZError = (float) fabs(st_PreviousQ.z - st_CurrentQ.z) > (float) fabs(st_NextQ.z - st_CurrentQ.z) ? (float) fabs(st_PreviousQ.z - st_CurrentQ.z) : (float) fabs(st_NextQ.z - st_CurrentQ.z);
		f_MaxError = (f_XError > f_YError) ? (f_XError > f_ZError) ? f_XError : f_ZError : (f_YError > f_ZError) ? f_YError : f_ZError;

		MATH_ConvertQuaternionToMatrix(&st_CurrentM, &st_CurrentQ);
		MATH_InitVector(&st_CurrentM.T, 0.0f, 0.0f, 0.0f);

		t = ((float) EVE_NumFrames(pst_PreviousEvent) / (EVE_NumFrames(pst_PreviousEvent) + EVE_NumFrames(pst_CurrentEvent)));
		/*$on*/
		/* Quaternion Blend */
		{
			/*~~~~~~~~~~~~~~~~~*/
			float	fTheta;
			float	fInvSinTheta;
			float	fCosTheta;
			/*~~~~~~~~~~~~~~~~~*/

			fTheta = 1.57f;
			fInvSinTheta = 1.0f;
			fCosTheta = MATH_f_QuaternionDotProduct(&st_PreviousQ, &st_NextQ);
			if(fCosTheta < 0.0f)
			{
				MATH_NegQuaternion(&st_NextQ);
				fCosTheta = -fCosTheta;
			}

			if(fCosTheta > 1.0f)  fCosTheta = 1.0f;

			if(fAbs(fCosTheta - 1.0f) > 1E-7f)
			{
				fTheta = fAcos(fCosTheta);
				fInvSinTheta = fInv(fSin(fTheta));
			}
			else
			{
				fTheta = 0.0f;
				fInvSinTheta = 0.0f;
			}

			MATH_Slerp(&st_InterpoledQ, t, &st_PreviousQ, &st_NextQ, fTheta, fInvSinTheta);
		}

		if(_f_Epsilon == 0.0f)	/* Default value for floating epsilon system */
		{
			f_Error = 1.0f;
			f_MaxError /= 10.0f;
			if(f_MaxError)
			{
				while(f_MaxError < 1.0f)
				{
					f_MaxError *= 10.0f;
					f_Error /= 10.0f;
				}
			}

			f_Error = (float) (f_Error > (float) 1E-3) ? (float) 1E-3 : (f_Error < (float) 1E-7) ? (float) 1E-7 : f_Error;
		}
		else
			f_Error = _f_Epsilon;

		b_30HzOptim = FALSE;
		/*
		b_30HzOptim = 
						(
							(
								(EVE_NumFrames(pst_PreviousEvent)== 1)
							&&	(EVE_NumFrames(pst_CurrentEvent)== 1)
							&&	(uw_Frame % 2)
							)
						||
							(
								(EVE_NumFrames(pst_NextEvent) == 1)
							&&	(EVE_NumFrames(pst_CurrentEvent) == 1)
							&&	(uw_Frame % 2)
							)

						);
		*/


		if
		(
			(
				fEqWithEpsilon(st_InterpoledQ.x, st_CurrentQ.x, f_Error)
			&&	fEqWithEpsilon(st_InterpoledQ.y, st_CurrentQ.y, f_Error)
			&&	fEqWithEpsilon(st_InterpoledQ.z, st_CurrentQ.z, f_Error)
			&&	fEqWithEpsilon(st_InterpoledQ.w, st_CurrentQ.w, f_Error)
			)
		||
			(b_30HzOptim)
		)
		{

			/* conjugué */

			if(b_30HzOptim)
				Test2000 ++;

			pst_PreviousEvent->uw_NumFrames += EVE_NumFrames(pst_CurrentEvent);
			EVE_DeleteEvent(_pst_Track, i);
			ul_AnnihilatedKeys++;
		}
		else
		{
			i++;
		}
	}

	/* compute track num of frames after optim */
	i = 0;
	while(i < _pst_Track->uw_NumEvents)
	{

		totalkeyafteroptim ++ ;
		i ++;
	}

	return ul_AnnihilatedKeys;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EVE_TranslationTrackOptimization
(
	EVE_tdst_Track	*_pst_Track,
	float			_f_Epsilon
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT				i, j, uw_EndEvent, uw_Frame;
	MATH_tdst_Vector	*pst_PreviousV, *pst_CurrentV, *pst_NextV;
	MATH_tdst_Vector	st_InterpoledV;
	EVE_tdst_Event		*pst_PreviousEvent, *pst_CurrentEvent, *pst_NextEvent;
	float				t, f_MaxError, f_XError, f_YError, f_ZError, f_Error;
	ULONG				ul_AnnihilatedKeys;
	BOOL				b_30HzOptim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = 1;
	uw_EndEvent = _pst_Track->uw_NumEvents;
	ul_AnnihilatedKeys = 0;
	while(i < uw_EndEvent - 1)
	{
		uw_Frame = 0;
		for(j = 0; j < i; j++)
		{
			uw_Frame += EVE_NumFrames(_pst_Track->pst_AllEvents + j);
		}

		pst_PreviousEvent = _pst_Track->pst_AllEvents + i - 1;
		pst_CurrentEvent = _pst_Track->pst_AllEvents + i;
		pst_NextEvent = _pst_Track->pst_AllEvents + i + 1;

		pst_PreviousV = EVE_pst_Event_InterpolationKey_GetPos(pst_PreviousEvent);
		pst_CurrentV = EVE_pst_Event_InterpolationKey_GetPos(pst_CurrentEvent);
		pst_NextV = EVE_pst_Event_InterpolationKey_GetPos(pst_NextEvent);


		if(!pst_PreviousV || !pst_CurrentV || !pst_NextV) 
		{
			i ++;
			continue;
		}

		/*$off*/
		f_XError = (float) fabs(pst_PreviousV->x - pst_CurrentV->x) > (float) fabs(pst_NextV->x - pst_CurrentV->x) ? (float) fabs(pst_PreviousV->x - pst_CurrentV->x) : (float) fabs(pst_NextV->x - pst_CurrentV->x);
		f_YError = (float) fabs(pst_PreviousV->y - pst_CurrentV->y) > (float) fabs(pst_NextV->y - pst_CurrentV->y) ? (float) fabs(pst_PreviousV->y - pst_CurrentV->y) : (float) fabs(pst_NextV->y - pst_CurrentV->y);
		f_ZError = (float) fabs(pst_PreviousV->z - pst_CurrentV->z) > (float) fabs(pst_NextV->z - pst_CurrentV->z) ? (float) fabs(pst_PreviousV->z - pst_CurrentV->z) : (float) fabs(pst_NextV->z - pst_CurrentV->z);
		f_MaxError = (f_XError > f_YError) ? (f_XError > f_ZError) ? f_XError : f_ZError : (f_YError > f_ZError) ? f_YError : f_ZError;

		t = ((float) EVE_NumFrames(pst_PreviousEvent) / (EVE_NumFrames(pst_PreviousEvent) + EVE_NumFrames(pst_CurrentEvent)) );

		f_Error = 1.0f;
		if(f_MaxError)
		{
			f_MaxError /= 10.0f;
			while(f_MaxError < 1.0f)
			{
				f_MaxError *= 10.0f;
				f_Error /= 10.0f;
			}
		}

		if(_f_Epsilon == 0.0f) {
			// Default value for floating epsilon
			f_Error = (float) (f_Error > (float) 1E-2) ? (float) 1E-2 : (f_Error < (float) 1E-7) ? (float) 1E-7 : f_Error;
		}
		else
			f_Error = _f_Epsilon;


		b_30HzOptim = 
						(
							(
								(EVE_NumFrames(pst_PreviousEvent)== 1)
							&&	(EVE_NumFrames(pst_CurrentEvent)== 1)
							&&	(uw_Frame % 2)
							)
						||
							(
								(EVE_NumFrames(pst_NextEvent) == 1)
							&&	(EVE_NumFrames(pst_CurrentEvent) == 1)
							&&	(uw_Frame % 2)
							)

						);


		/*$on*/
		MATH_AddVector(&st_InterpoledV, pst_PreviousV, pst_NextV);
		MATH_ScaleEqualVector(&st_InterpoledV, t);
		if(MATH_b_EqVectorWithEpsilon(&st_InterpoledV, pst_CurrentV, f_Error)) // || b_30HzOptim)
		{
			pst_PreviousEvent->uw_NumFrames += EVE_NumFrames(pst_CurrentEvent);
			EVE_DeleteEvent(_pst_Track, i);
			ul_AnnihilatedKeys++;
			uw_EndEvent--;
		}
		else
			i++;
	}

	return ul_AnnihilatedKeys;
}

BOOL			ANI_b_ForceOptimization = FALSE;
static float	gf_SpecificErrors[100];

/*
 =======================================================================================================================
	Aim: We want to set a specific Optimzation threhold for a Track.


	0.0f	- Default Optim Value
	1.0f	- No Optim
	 X		- Error of specific Track
 =======================================================================================================================
 */
extern int ANI_gi_ImportMode;

BOOL EVE_b_FillErrorArray(OBJ_tdst_GameObject *_pst_GO)
{
	/* Reset the Array */
	L_memset(gf_SpecificErrors, 0, sizeof(float) * 100);

	if((_pst_GO && ((!L_stricmp(_pst_GO->sz_Name, "KIT_Sally.gao")) || (!L_stricmp(_pst_GO->sz_Name, "_PJ_Sally.gao")))) || (ANI_gi_ImportMode == 2))
	{
		gf_SpecificErrors[1] = 1E-3f;
		gf_SpecificErrors[2] = 1E-3f;
		gf_SpecificErrors[5] = 1E-3f;
		gf_SpecificErrors[6] = 1E-3f;

		return TRUE;
	}

	return FALSE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVE_MysticalOptimization
(
	OBJ_tdst_GameObject	*_pst_GO,
	EVE_tdst_ListTracks *_pst_ListTracks,
	float				_f_DefaultErrorR,
	float				_f_DefaultErrorT
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	BOOL			b_Optim, b_SpecificError;
	short			w_Type;
	USHORT			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Optim = FALSE;
	b_SpecificError = FALSE;

	if(_pst_ListTracks->uw_NumTracks > 100)
		ERR_X_Assert(0);

	b_SpecificError = EVE_b_FillErrorArray(_pst_GO);

	for(i = 0; i < _pst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = &_pst_ListTracks->pst_AllTracks[i];

		/* The Track may have already been optimized. We optimize tracks only once */
		if(!ANI_b_ForceOptimization && (pst_Track->uw_Flags & EVE_C_Track_Optimized)) continue;
		pst_Track->uw_Flags |= EVE_C_Track_Optimized;

		b_Optim = TRUE;

		if(b_SpecificError && (gf_SpecificErrors[pst_Track->uw_Gizmo] == 1.0f))
			continue;

		if(pst_Track->pst_AllEvents)
			w_Type = EVE_w_Event_InterpolationKey_GetFlags(pst_Track->pst_AllEvents) & EVE_C_EventFlag_Type;
		else
			w_Type = -1;
		if(w_Type == EVE_C_EventFlag_InterpolationKey)
		{
			w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Track->pst_AllEvents);

			if(w_Type & 4)		/* Rotations with Matrix */
				EVE_RotationTrackOptimization(pst_Track, 0, pst_Track->uw_NumEvents, (b_SpecificError) ? ((gf_SpecificErrors[pst_Track->uw_Gizmo] != 0.0f) ? (gf_SpecificErrors[pst_Track->uw_Gizmo]) : (_f_DefaultErrorR)): (_f_DefaultErrorR));
			else
			{
				if(w_Type & 16) /* Rotations with Quaternions */
					EVE_QuaternionTrackOptimization2(pst_Track, (b_SpecificError) ? ((gf_SpecificErrors[pst_Track->uw_Gizmo] != 0.0f) ? (gf_SpecificErrors[pst_Track->uw_Gizmo]) : (_f_DefaultErrorR)): (_f_DefaultErrorR));
				else			/* Rotations with Translations */
					EVE_TranslationTrackOptimization(pst_Track, (b_SpecificError) ? ((gf_SpecificErrors[pst_Track->uw_Gizmo] != 0.0f) ? (gf_SpecificErrors[pst_Track->uw_Gizmo]) : (_f_DefaultErrorT)): (_f_DefaultErrorT));
			}
		}
		else
			continue;
	}

	return b_Optim;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_ScaleTranslation
(
	EVE_tdst_Track	*_pst_Track,
	USHORT			_uw_StartEvent,
	USHORT			_uw_EndEvent,
	float			_f_Scale
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_CurrentV;
	EVE_tdst_Event		*pst_CurrentEvent;
	USHORT				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = _uw_StartEvent; i < _uw_EndEvent; i++)
	{
		pst_CurrentEvent = _pst_Track->pst_AllEvents + i;
		pst_CurrentV = EVE_pst_Event_InterpolationKey_GetPos(pst_CurrentEvent);

		MATH_ScaleEqualVector(pst_CurrentV, _f_Scale);
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_ScaleAllTranslations
(
	EVE_tdst_ListTracks *_pst_ListTracks,
	float				_f_Scale
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	short			w_Type;
	USHORT			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_ListTracks->uw_NumTracks > 100)
		ERR_X_Assert(0);

	for(i = 0; i < _pst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = &_pst_ListTracks->pst_AllTracks[i];

		if(pst_Track->pst_AllEvents)
			w_Type = EVE_w_Event_InterpolationKey_GetFlags(pst_Track->pst_AllEvents) & EVE_C_EventFlag_Type;
		else
			w_Type = -1;
		if(w_Type == EVE_C_EventFlag_InterpolationKey)
		{
			w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Track->pst_AllEvents);

			if(!(w_Type & 4) && !(w_Type & 16))
			{
				EVE_ScaleTranslation(pst_Track, 0, pst_Track->uw_NumEvents, _f_Scale);
			}
		}
		else
			continue;
	}
}


#endif // ACTIVE_EDITORS
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
