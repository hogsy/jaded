/*$T EVEsave.c GC! 1.081 10/06/00 14:40:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "EVEstruct.h"
#include "EVEconst.h"
#include "EVEinit.h"
#include "EVEsave.h"
#include "EVEnt_aifunc.h"
#include "EVEnt_interpolationkey.h"
#include "EVEnt_morphkey.h"
#include "EVEnt_timekey.h"
#include "EVEplay.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGspecial.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKstruct.h"
#include "EDIpaths.h"
#include "BASe/MEMory/MEM.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Duplicate(OBJ_tdst_GameObject *_pst_SrcGO, OBJ_tdst_GameObject *_pst_NewGO, BOOL _b_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Data;
	EVE_tdst_Data	*pst_Src;
#ifdef ACTIVE_EDITORS
	ULONG			ul_Index;
	char			az_Path[512];
	char			az_Name[512];
	int				i;
#endif
	extern BOOL OBJ_gb_DuplicateAI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_b_File)
	{
		pst_Src = _pst_SrcGO->pst_Extended->pst_Events;
		pst_Data = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
		pst_Data->pst_ListParam = NULL;
		pst_Data->pst_ListTracks = NULL;

		if(pst_Src->pst_ListParam)
		{
			pst_Data->pst_ListParam = (struct EVE_tdst_Params_ *) MEM_p_Alloc(pst_Src->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Params));
			L_memcpy
			(
				pst_Data->pst_ListParam,
				pst_Src->pst_ListParam,
				pst_Src->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Params)
			);
		}

		if(pst_Src->pst_ListTracks)
		{
			pst_Data->pst_ListTracks = pst_Src->pst_ListTracks;
			if(OBJ_gb_DuplicateAI) pst_Data->pst_ListTracks->ul_NbOfInstances++;
		}

		_pst_NewGO->pst_Extended->pst_Events = pst_Data;
		return;
	}

	pst_Data = EVE_pst_DuplicateData(_pst_SrcGO->pst_Extended->pst_Events);
	_pst_NewGO->pst_Extended->pst_Events = pst_Data;

#ifdef ACTIVE_EDITORS
	if(_b_File)
	{
		/* To retreive name of object */
		ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_NewGO->pst_World);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
		BIG_ComputeFullName(BIG_ParentFile(ul_Index), az_Path);
		L_strcat(az_Path, "/");
		L_strcat(az_Path, EDI_Csz_Path_ListTracks);
		LINK_RegisterDuplicate
		(
			_pst_SrcGO->pst_Extended->pst_Events->pst_ListTracks,
			_pst_NewGO->pst_Extended->pst_Events->pst_ListTracks,
			0
		);

		ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_SrcGO);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
		L_strcpy(az_Name, BIG_NameFile(ul_Index));
		*L_strrchr(az_Name, '.') = 0;
		L_strcat(az_Name, EDI_Csz_ExtEventAllsTracks);
		i = 0;
		while(BIG_ul_SearchFileExt(az_Path, az_Name) != BIG_C_InvalidIndex) az_Name[i++] = '_';

		ul_Index = BIG_ul_CreateFile(az_Path, az_Name);
		LOA_AddAddress(ul_Index, _pst_NewGO->pst_Extended->pst_Events->pst_ListTracks);
		EVE_SaveListTracks(_pst_NewGO, _pst_NewGO->pst_Extended->pst_Events->pst_ListTracks);
	}

#endif
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EVE_ComputeDataLength(EVE_tdst_Track *_pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~*/
	USHORT	i;
	ULONG	ul_DataLength;
	/*~~~~~~~~~~~~~~~~~~*/

	ul_DataLength = 0;

	if(_pst_Track->uw_NumEvents)
	{
		for(i = 0; i < _pst_Track->uw_NumEvents; i++)
		{
			if(_pst_Track->pst_AllEvents[i].p_Data)
				ul_DataLength += (ULONG) * (short *) _pst_Track->pst_AllEvents[i].p_Data;
		}
	}

	return ul_DataLength;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
USHORT EVE_CreateOptimFlags(EVE_tdst_Track *_pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	USHORT			uw_OptimFlag;
	USHORT			uw_Size0, uw_Flags0, uw_Type0;
	USHORT			uw_Size, uw_Flags, uw_Type;
	BOOL			b_Flash, b_SameSize, b_Under256, b_SameFlags, b_SameType;
	EVE_tdst_Event	*pst_Event, *pst_PreviousEvent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Deal only with InterpolationKeys Tracks. */
	if((!_pst_Track->pst_AllEvents) ||((_pst_Track->pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey)) return 0;

	uw_OptimFlag = 0;
	b_Flash = FALSE;
	b_SameSize = TRUE;
	b_SameFlags = TRUE;
	b_SameType = TRUE;

	pst_Event = &_pst_Track->pst_AllEvents[0];
	pst_PreviousEvent = NULL;

	b_Under256 = (pst_Event->uw_NumFrames < 256);

	uw_Flags0 = pst_Event->w_Flags;

	if(uw_Flags0 & EVE_C_EventFlag_Flash)
	{
		b_Flash = TRUE;
		uw_Flags0 &= ~EVE_C_EventFlag_Flash;
	}

	uw_Type0 = EVE_w_Event_InterpolationKey_GetType(pst_Event);
	uw_Size0 = *((short *) pst_Event->p_Data);
	uw_Type0 += EVE_w_Event_ComputeCompressType(pst_Event, pst_PreviousEvent);

	for(i = 1; i < _pst_Track->uw_NumEvents; i++)
	{
		pst_Event = &_pst_Track->pst_AllEvents[i];
		pst_PreviousEvent = &_pst_Track->pst_AllEvents[i - 1];

		if(!pst_Event->p_Data) 
		{
			b_Flash = FALSE;
			b_SameSize = FALSE;
			b_SameFlags = FALSE;
			b_SameType = FALSE;
			b_Under256 = FALSE;
			continue;
		};
		if(!pst_PreviousEvent->p_Data) 
		{
			b_Flash = FALSE;
			b_SameSize = FALSE;
			b_SameFlags = FALSE;
			b_SameType = FALSE;
			b_Under256 = FALSE;
			continue;
		}


		uw_Flags = pst_Event->w_Flags;
		uw_Type = EVE_w_Event_InterpolationKey_GetType(pst_Event);
		uw_Size = *((short *) pst_Event->p_Data);
		uw_Type += EVE_w_Event_ComputeCompressType(pst_Event, pst_PreviousEvent);

		if(uw_Flags != uw_Flags0) b_SameFlags = FALSE;
		if(uw_Type != uw_Type0) b_SameType = FALSE;
		if(uw_Size != uw_Size0) b_SameSize = FALSE;
		if(pst_Event->uw_NumFrames >= 256) b_Under256 = FALSE;
	}

	if(b_SameFlags) uw_OptimFlag |= EVE_C_Track_SameFlags;
	if(b_SameType) uw_OptimFlag |= EVE_C_Track_SameType;
	if(b_SameSize) uw_OptimFlag |= EVE_C_Track_SameSize;
	if(b_Under256) uw_OptimFlag |= EVE_C_Track_Under256;
	if(b_Flash) uw_OptimFlag |= EVE_C_Track_Flash;

	return uw_OptimFlag;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SaveListEvents(OBJ_tdst_GameObject *_pst_GO, EVE_tdst_Track *_pst_Track, USHORT _uw_OptimFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Num, i;
	short			w_NumTrack, w_NumEvent;
	BOOL			b_Symetric;
	EVE_tdst_Event	*pst_Event, *pst_PreviousEvent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Number of events */
	ul_Num = _pst_Track->uw_NumEvents;
	SAV_Buffer(&ul_Num, 4);

	for(i = 0; i < ul_Num; i++)
	{
		b_Symetric = FALSE;

		pst_Event = &_pst_Track->pst_AllEvents[i];
		pst_PreviousEvent = (i != 0) ? &_pst_Track->pst_AllEvents[i - 1] : NULL;

		if(pst_Event->w_Flags & EVE_C_EventFlag_Symetric)
		{
			b_Symetric = TRUE;
			if(!EVE_b_GetSymetricEventInfo(_pst_GO, pst_Event, &w_NumTrack, &w_NumEvent))
			{
				b_Symetric = FALSE;
				LINK_PrintStatusMsg("List Events saving error. Cannot find the Reference of a Symetric event.");
				pst_Event->w_Flags &= ~EVE_C_EventFlag_Symetric;
			}
		}

		if(_uw_OptimFlags & EVE_C_Track_Under256)
			SAV_Buffer(&pst_Event->uw_NumFrames, 1);
		else
			SAV_Buffer(&pst_Event->uw_NumFrames, 2);

		if(_uw_OptimFlags & EVE_C_Track_SameFlags)
		{
			if(i == 0) SAV_Buffer(&pst_Event->w_Flags, 2);
		}
		else
			SAV_Buffer(&pst_Event->w_Flags, 2);

		switch(pst_Event->w_Flags & EVE_C_EventFlag_Type)
		{
		case EVE_C_EventFlag_AIFunction:
			EVE_Event_AIFunction_Save(pst_Event);
			break;
		case EVE_C_EventFlag_InterpolationKey:
			{
				if(b_Symetric)
				{
					SAV_Buffer(&w_NumTrack, sizeof(short));
					SAV_Buffer(&w_NumEvent, sizeof(short));
				}
				else
					EVE_Event_InterpolationKey_Save(pst_Event, pst_PreviousEvent, _uw_OptimFlags);
			}
			break;
		case EVE_C_EventFlag_MorphKey:
			EVE_Event_MorphKey_Save(pst_Event);
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVE_b_Animation(EVE_tdst_ListTracks *_pst_ListTracks)
{
	/*~~~~~~*/
	USHORT	i;
	/*~~~~~~*/

	if(!_pst_ListTracks) return FALSE;

	for(i = 0; i < _pst_ListTracks->uw_NumTracks; i++)
	{
		if
		(
			&_pst_ListTracks->pst_AllTracks[i]
		&&	(_pst_ListTracks->pst_AllTracks[i].pst_AllEvents)
		&&	((_pst_ListTracks->pst_AllTracks[i].pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
		&&	!(_pst_ListTracks->pst_AllTracks[i].pst_AllEvents[0].w_Flags & EVE_C_EventFlag_NoInterpolation)
		&&	(EVE_w_Event_InterpolationKey_GetType(&_pst_ListTracks->pst_AllTracks[i].pst_AllEvents[0]) & (EVE_InterKeyType_Rotation_Quaternion | EVE_InterKeyType_Translation_0))
		&&	(_pst_ListTracks->pst_AllTracks[i].pst_GO == NULL)
		) return TRUE;
	}

	return FALSE;
}

extern ACT_gb_ForceARAM;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_SaveListTracks(OBJ_tdst_GameObject *_pst_GO, EVE_tdst_ListTracks *_pst_List)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	ULONG		ul_DataLength;
	ULONG		ul_Tmp;
	ULONG		i, ul_Num;
	BOOL		b_Anim;
	USHORT		uw_Flags, uw_OptimFlags;
	char		asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_List) return;
	ul_File = LOA_ul_SearchKeyWithAddress((ULONG) _pst_List);
	if(ul_File == BIG_C_InvalidIndex) return;
	ul_File = BIG_ul_SearchKeyToFat(ul_File);
	if(ul_File == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(ul_File != BIG_C_InvalidIndex, "Cannot find ListTrack file", NULL);
		return;
	}

	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
	SAV_Begin(asz_Path, BIG_NameFile(ul_File));

	/* Number of tracks */
	SAV_Buffer(&_pst_List->uw_NumTracks, 2);

	if(ACT_gb_ForceARAM)
	{
		_pst_List->uw_Flags |= EVE_C_Tracks_ForceARAM;	/* Force ListTracks to be in ARAM */
	}
	else
	{
		_pst_List->uw_Flags &= ~EVE_C_Tracks_ForceARAM;	/* Force ListTracks to be in ARAM */
	}

	SAV_Buffer(&_pst_List->uw_Flags, 2);

	/* All tracks */
	ul_Num = (ULONG) _pst_List->uw_NumTracks;

	b_Anim = EVE_b_Animation(_pst_List);

	for(i = 0; i < ul_Num; i++)
	{

		if
		(
			b_Anim
		&&	(_pst_List->pst_AllTracks[i].pst_AllEvents)
		&&	(_pst_List->pst_AllTracks[i].pst_GO == NULL)
		&&	((_pst_List->pst_AllTracks[i].pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Type) & EVE_C_EventFlag_InterpolationKey)
		&&	!(_pst_List->pst_AllTracks[i].pst_AllEvents[0].w_Flags & EVE_C_EventFlag_NoInterpolation)
		&&  (EVE_w_Event_InterpolationKey_GetType(&_pst_List->pst_AllTracks[i].pst_AllEvents[0]) & (EVE_InterKeyType_Rotation_Quaternion | EVE_InterKeyType_Translation_0))
		) 
		{
			_pst_List->pst_AllTracks[i].uw_Flags |= EVE_C_Track_Anims;
		}
		else
		{
			_pst_List->pst_AllTracks[i].uw_Flags &= ~EVE_C_Track_Anims;
		}

		uw_Flags = _pst_List->pst_AllTracks[i].uw_Flags;

		if(_pst_List->pst_AllTracks[i].uw_Flags & EVE_C_Track_Anims)
			uw_OptimFlags = EVE_CreateOptimFlags(&_pst_List->pst_AllTracks[i]);
		else
			uw_OptimFlags = 0;

		uw_Flags += uw_OptimFlags;

		/* Flags */
		SAV_Buffer(&uw_Flags, 2);

		/* Gizmo */
		SAV_Buffer(&_pst_List->pst_AllTracks[i].uw_Gizmo, 2);

		/* Compute Data Length; */
		ul_DataLength = EVE_ComputeDataLength(&_pst_List->pst_AllTracks[i]);

		/* Data Length */
		SAV_Buffer(&ul_DataLength, 4);

		if
		(
			!b_Anim
		||	!(_pst_List->pst_AllTracks[i].pst_AllEvents)
		||	!(_pst_List->pst_AllTracks[i].pst_GO == NULL)
		||	!((_pst_List->pst_AllTracks[i].pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Type) & EVE_C_EventFlag_InterpolationKey)
		||	(_pst_List->pst_AllTracks[i].pst_AllEvents[0].w_Flags & EVE_C_EventFlag_NoInterpolation)
		||	!(EVE_w_Event_InterpolationKey_GetType(&_pst_List->pst_AllTracks[i].pst_AllEvents[0]) & (EVE_InterKeyType_Rotation_Quaternion | EVE_InterKeyType_Translation_0))
		)
		{
			ul_Tmp = (ULONG) _pst_List->pst_AllTracks[i].pst_GO;
			if(ul_Tmp) ul_Tmp = LOA_ul_SearchKeyWithAddress(ul_Tmp);
			SAV_Buffer(&ul_Tmp, 4);

			/* Dummy */
			_pst_List->pst_AllTracks[i].asz_Name[14] = 0;
			SAV_Buffer(_pst_List->pst_AllTracks[i].asz_Name, 15);
			SAV_Buffer(&_pst_List->pst_AllTracks[i].c_Color, 1);
		}
		else
		{
			if(!uw_OptimFlags)
			{
				ul_Tmp = (ULONG) (_pst_List->pst_AllTracks[i].pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Type);
				SAV_Buffer(&ul_Tmp, 4);
			}
		}

		/* List of events */
		if(_pst_List->pst_AllTracks[i].pst_AllEvents)
			EVE_SaveListEvents(_pst_GO, &_pst_List->pst_AllTracks[i], uw_OptimFlags);
		else
		{
			ul_Tmp = 0;
			SAV_Buffer(&ul_Tmp, 4);
		}
	}

//	BIG_specialmode = BIG_C_SpecialModeHUF;
	SAV_ul_End();
}
/*
 =======================================================================================================================
	Aim :	Check basic requirements on tracks. Display warnings if problems.
	
	Basic checks :
		- If there are morph keys, the associated gao must have a morph modifier and its dummy channels must
		  have been created.
		- If there is a morph key in a track, all morph keys of this track must have the same Param 0.
 =======================================================================================================================
 */
void EVE_CheckListTracks(OBJ_tdst_GameObject *_pst_GO, EVE_tdst_ListTracks *_pst_List)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_NumTracks, i_NumEvents, i_Temp;
	EVE_tdst_Event	*pst_Events;
	BOOL			b_UglyOptim;
	BOOL			*ab_TrackChecked, *ab_EventChecked = NULL;
	char			*sz_FileName;
	ULONG			ul_Index;
	int				i_TrIdx, i_EveIdx, i_TrackChIdx;
	char			sz_Msg[256], sz_Buff[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// No checking on animations yet (maybe later)
	if (EVE_b_Animation(_pst_List)) return;
	if (!_pst_List->pst_AllTracks) return;

	// For log messages...
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_List);
	if(ul_Index == BIG_C_InvalidIndex) return;
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	if(ul_Index == BIG_C_InvalidIndex)
		sz_FileName = "";
	else
		sz_FileName = BIG_NameFile(ul_Index);

	// Check all tracks
	i_NumTracks = (int) _pst_List->uw_NumTracks;
	ab_TrackChecked = (BOOL *) MEM_p_Alloc(i_NumTracks * sizeof(BOOL));
	L_memset(ab_TrackChecked, 0, i_NumTracks * sizeof(BOOL));
	for(i_TrIdx = 0; i_TrIdx < i_NumTracks; i_TrIdx++)
	{
		if (ab_TrackChecked[i_TrIdx]) continue;
		
		// Init check datas of the track
		i_TrackChIdx = 0;

		pst_Events = _pst_List->pst_AllTracks[i_TrIdx].pst_AllEvents;
		if (pst_Events)
		{
			// Check all events of the track
			i_NumEvents = (int) _pst_List->pst_AllTracks[i_TrIdx].uw_NumEvents;
			if (ab_EventChecked) MEM_Free(ab_EventChecked);
			ab_EventChecked = (BOOL *) MEM_p_Alloc(i_NumEvents * sizeof(BOOL));
			L_memset(ab_EventChecked, 0, i_NumEvents * sizeof(BOOL));
			for (i_EveIdx = 0; i_EveIdx < i_NumEvents; i_EveIdx++)
			{
				if (ab_EventChecked[i_EveIdx]) continue;
				// Treat different key types
				b_UglyOptim = pst_Events[i_EveIdx].uw_NumFrames & 0x8000;
				switch (b_UglyOptim ? EVE_C_EventFlag_InterpolationKey : (pst_Events[i_EveIdx].w_Flags & EVE_C_EventFlag_Type))
				{
					case EVE_C_EventFlag_MorphKey:
						// Check gao
						if (!_pst_List->pst_AllTracks[i_TrIdx].pst_GO)
						{
							// Warning
							sprintf( sz_Msg, "[%08X] Event %d : gao is NULL. There should be a gao.", ul_Index, i_EveIdx );
							ERR_X_Warning(0, sz_Msg, sz_FileName);
						}
						// Basic check
						if (!EVE_Event_MorphKey_Check(
								pst_Events +i_EveIdx,
								_pst_List->pst_AllTracks[i_TrIdx].pst_GO,
								sz_Buff,
								&i_Temp))
						{
							// Warning

							// TODO
							sprintf( sz_Msg, "[%08X] Event %d : ", ul_Index, i_EveIdx );
							sprintf( sz_Msg, "%s.", sz_Buff );
							ERR_X_Warning(0, sz_Msg, sz_FileName);
						}
						else
						{
							// Check that other morph keys of the track have same param 0;
							if (i_TrackChIdx == -1) i_TrackChIdx = i_Temp;
							else
							{
								if (i_TrackChIdx != i_Temp)
								{
									// Warning
									sprintf( sz_Msg, "[%08X] Track %d : All morph key don't have the same Morph Bone Index parameter ", ul_Index, i_TrIdx );
									ERR_X_Warning(0, sz_Msg, sz_FileName);
								}
							}
						}
						break;
				}
				ab_EventChecked[i_EveIdx] = TRUE;
			}
		}
		ab_TrackChecked[i_TrIdx] = TRUE;
	}
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
