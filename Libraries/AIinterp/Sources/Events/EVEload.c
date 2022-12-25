/*$T EVEload.c GC 1.138 06/27/05 16:32:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "EVEstruct.h"
#include "EVEconst.h"
#include "EVEinit.h"
#include "EVEnt_aifunc.h"
#include "EVEnt_interpolationkey.h"
#include "EVEnt_morphkey.h"
#include "EVEnt_timekey.h"
#include "EVEplay.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGspecial.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "BIGfiles/LOAding/LOAread.h"


EVE_tdst_ListTracks *EVE_gpst_CurrentReolve;
extern ULONG		LOA_ul_FileTypeSize[40];

extern ULONG		Event_SizeOf_Ram;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    /*
 =======================================================================================================================
 */
void EVE_ResolveTrackRef(EVE_tdst_ListTracks *_pst_List, EVE_tdst_Track *_pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT			i;
	EVE_tdst_Event	*pst_Event;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Track->pst_AllEvents) return;
	if(!(_pst_Track->uw_Flags & EVE_C_Track_MustResolve)) return;
	EVE_gpst_CurrentReolve = _pst_List;

	/* Resolve all events refs */
	for(i = 0; i < _pst_Track->uw_NumEvents; i++)
	{
		pst_Event = _pst_Track->pst_AllEvents + i;
		if(pst_Event->w_Flags & EVE_C_EventFlag_Symetric)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			short	w_NumTrack, w_NumEvent;
			ULONG	ul_FakeData;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			ul_FakeData = (ULONG) pst_Event->p_Data;
			w_NumTrack = (short) ul_FakeData;
			ul_FakeData = ul_FakeData >> 16;
			w_NumEvent = (short) ul_FakeData;

			pst_Event->p_Data = _pst_List->pst_AllTracks[w_NumTrack].pst_AllEvents[w_NumEvent].p_Data;
		}
		else if((EVE_w_Event_InterpolationKey_GetFlags(pst_Event) & EVE_C_EventFlag_Type) == EVE_C_EventFlag_AIFunction)
			EVE_Event_AIFunction_Resolve(pst_Event);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_ResolveListTrackRef(EVE_tdst_ListTracks *_pst_List)
{
	/*~~*/
	int i;
	/*~~*/

	if(!_pst_List) return;
	if(_pst_List->uw_Flags & EVE_C_Tracks_Resolved) return;
	PROPS2_StartRaster(&PROPS2_gst_EVE_ResolveListTrackRef);
	_pst_List->uw_Flags |= EVE_C_Tracks_Resolved;
	for(i = 0; i < _pst_List->uw_NumTracks; i++) EVE_ResolveTrackRef(_pst_List, &_pst_List->pst_AllTracks[i]);
	PROPS2_StopRaster(&PROPS2_gst_EVE_ResolveListTrackRef);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_ResolveRefs(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Use)
{
	if(!_pst_GO->pst_Extended->pst_Events)
	{
		ERR_X_Warning(0, "Identity (Events) without a corresponding struct", _pst_GO->sz_Name);
		return;
	}

	if(!_pst_GO->pst_Extended->pst_Events->pst_ListTracks) return;

	/* Allocate instanciated struct */
	if(_pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks)
	{
		_pst_GO->pst_Extended->pst_Events->pst_ListParam = (EVE_tdst_Params *) MEM_p_Alloc(_pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Params));
		L_memset
		(
			_pst_GO->pst_Extended->pst_Events->pst_ListParam,
			0,
			_pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks * sizeof(EVE_tdst_Params)
		);
	}

	/* Resolve all tracks */
	EVE_ResolveListTrackRef(_pst_GO->pst_Extended->pst_Events->pst_ListTracks);

	/* Init instanciated datas */
	EVE_ReinitData(_pst_GO->pst_Extended->pst_Events);

	/* One more instance */
	if(_b_Use) _pst_GO->pst_Extended->pst_Events->pst_ListTracks->ul_NbOfInstances++;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_LoadListEvents
(
	EVE_tdst_Track	*_pst_Track,
	char			*pc_Buf,
	ULONG			*_pul_Add,
	USHORT			_uw_OptimFlags,
	ULONG			_ul_BitField
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT			i;
	char			*pc_Mem;
	BOOL			b_Symetric, b_Data;
	ULONG			ul_TotalOffset, ul_Offset;
	USHORT			optim;
	BOOL			b_SameAllTrack;
	BOOL			b_UglyOptim;
	USHORT			w_Size, w_Type;
#ifndef ACTIVE_EDITORS
	USHORT			l_Size;
#endif
	char			*pc1;
	BOOL			b_Translation;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_SameAllTrack = (_uw_OptimFlags & EVE_C_Track_SameFlags) && (_uw_OptimFlags & EVE_C_Track_SameType) &&	(_uw_OptimFlags & EVE_C_Track_SameSize);

	b_UglyOptim = FALSE;
	ul_Offset = 0;
	ul_TotalOffset = 0;

	pc_Mem = pc_Buf;

	_pst_Track->uw_Flags &= ~EVE_C_Track_MustResolve;
	_pst_Track->uw_NumEvents = (USHORT) LOA_ReadLong(&pc_Buf);
	_pst_Track->pst_AllEvents = NULL;

	if(_pst_Track->uw_NumEvents)
	{
		MEMpro_StartMemRaster();

		/* In engine, a big buffer contains all specific datas */
		if(_pst_Track->ul_DataLength)
		{
			/* Lookahead in the buffer to get optimization settings to minimize allocation */
			pc1 = pc_Buf;

			if(!(_uw_OptimFlags & EVE_C_Track_Under256))
				_LOA_ReadShort(&pc1, NULL, LOA_eBinLookAheadData);
			else
				_LOA_ReadChar(&pc1, NULL, LOA_eBinLookAheadData);

			/* Skip the flags (16 bits), first event always has flags */
			_LOA_ReadShort(&pc1, NULL, LOA_eBinLookAheadData);

			_LOA_ReadUShort(&pc1, &w_Size, LOA_eBinLookAheadData);
			_LOA_ReadUShort(&pc1, &w_Type, LOA_eBinLookAheadData);

			b_Translation = (w_Type & EVE_InterKeyType_Translation_Mask);

#ifndef ACTIVE_EDITORS
			if(_uw_OptimFlags & EVE_C_Track_SameSize)
			{
				b_UglyOptim = b_SameAllTrack && (w_Type & EVE_InterKey_CompressedQuaternion);

				/* With ugly optim, w_Size and l_Size are not used. */
				if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
				{
					if(w_Type & EVE_InterKey_CompressedQuaternionMask)
					{
						if(_uw_OptimFlags & EVE_C_Track_SameType)
							w_Size = sizeof(MATH_tdst_CompressedQuaternion);
						else
							w_Size = sizeof(MATH_tdst_Quaternion);	/* Mixed compressed and uncompressed -> we use only
																	 * uncompressed. */
					}
					else
						w_Size = w_Size;
				}

				if(w_Type & EVE_InterKeyType_Translation_Mask)
				{
					w_Size = sizeof(MATH_tdst_Vector) * (w_Type & EVE_InterKeyType_Translation_Mask);
				}

				l_Size = _pst_Track->uw_NumEvents * w_Size;

				if(!b_UglyOptim)
				{
					_pst_Track->pc_AllData = (char *) MEM_p_VMAlloc(l_Size);
				}
				else
				{
					_pst_Track->pc_AllData = NULL;
				}

				LOA_ul_FileTypeSize[31] += (b_UglyOptim ? 0 : (l_Size + 8));
			}
			else
			{
				_pst_Track->pc_AllData = (char *) MEM_p_Alloc(_pst_Track->ul_DataLength);
				LOA_ul_FileTypeSize[31] += _pst_Track->ul_DataLength + 8;
			}
#endif /* ACTIVE_EDITORS */
			b_Data = TRUE;
		}
		else
		{
			b_Data = FALSE;
		}

		_pst_Track->pst_AllEvents = (EVE_tdst_Event *) MEM_p_VMAlloc(_pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event));

		LOA_ul_FileTypeSize[30] += _pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event) + 8;

		for(i = 0; i < _pst_Track->uw_NumEvents; i++)
		{
			b_Symetric = FALSE;
			if(_uw_OptimFlags & EVE_C_Track_Under256)
			{
				_pst_Track->pst_AllEvents[i].uw_NumFrames = (USHORT) LOA_ReadUChar(&pc_Buf);
			}
			else
			{
				_pst_Track->pst_AllEvents[i].uw_NumFrames = LOA_ReadUShort(&pc_Buf);
			}

			if(_uw_OptimFlags & EVE_C_Track_SameFlags)
			{
				if(i == 0)
				{
					_pst_Track->pst_AllEvents[i].w_Flags = LOA_ReadUShort(&pc_Buf);
				}
				else
				{
					_pst_Track->pst_AllEvents[i].w_Flags = b_UglyOptim ? EVE_C_EventFlag_InterpolationKey : _pst_Track->pst_AllEvents[0].w_Flags;
					_pst_Track->pst_AllEvents[i].w_Flags &= ~EVE_C_EventFlag_Flash;
				}
			}
			else
			{
				_pst_Track->pst_AllEvents[i].w_Flags = LOA_ReadUShort(&pc_Buf);
			}

			_pst_Track->pst_AllEvents[i].p_Data = NULL;
			_pst_Track->pst_AllEvents[i].w_Flags &= ~EVE_C_EventFlag_Selected;
			if(_pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Symetric)
			{
				_pst_Track->uw_Flags |= EVE_C_Track_MustResolve;
				b_Symetric = TRUE;
			}

			switch(_pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Type)
			{

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_AIFunction:
#ifdef ACTIVE_EDITORS
				if(!b_Data) LINK_PrintStatusMsg("Specific Data Length Problem.");
#endif
				ul_Offset = EVE_Event_AIFunction_Load
					(
						&_pst_Track->pst_AllEvents[i],
						pc_Buf,
						_pst_Track->pc_AllData + ul_TotalOffset
					);
				pc_Buf += ul_Offset;

				/* NULL Data AI Event case */
				if(ul_Offset != 4) ul_TotalOffset += ul_Offset;
				_pst_Track->uw_Flags |= EVE_C_Track_MustResolve;
				break;

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_InterpolationKey:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					EVE_tdst_Event	*pst_PreviousEvent;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_PreviousEvent = (i != 0) ? &_pst_Track->pst_AllEvents[i - 1] : NULL;

#ifdef ACTIVE_EDITORS
					if(!b_Data) LINK_PrintStatusMsg("Specific Data Length Problem.");
#endif
					if(b_Symetric)
					{
						_pst_Track->pst_AllEvents[i].p_Data = (void *) LOA_ReadLong(&pc_Buf);
					}
					else
					{
						optim = _uw_OptimFlags;
						if(!i) optim |= EVE_C_Track_FirstEvent;

						ul_Offset = EVE_Event_InterpolationKey_Load
							(
								&_pst_Track->pst_AllEvents[i],
								pst_PreviousEvent,
								pc_Buf,
								_pst_Track->pc_AllData + ul_TotalOffset,
								_pst_Track,
								optim
							);

						pc_Buf += ul_Offset;

						if(!b_UglyOptim)
						{
#ifndef ACTIVE_EDITORS
							if(_uw_OptimFlags & EVE_C_Track_SameSize)
							{
								ul_TotalOffset += (ULONG) (_pst_Track->pst_AllEvents[i].t.size >> 1) - 4;
							}
							else
#endif
								ul_TotalOffset += (ULONG) (*(short *) _pst_Track->pst_AllEvents[i].p_Data);
						}
					}
				}
				break;

			/*$2------------------------------------------------------------------------------------------------------*/

			case EVE_C_EventFlag_MorphKey:
				{
					ul_Offset = EVE_Event_MorphKey_Load
						(
							&_pst_Track->pst_AllEvents[i],
							pc_Buf
						);

					pc_Buf += ul_Offset;
					ul_TotalOffset += (ULONG) (*(short *) _pst_Track->pst_AllEvents[i].p_Data);
				}
				break;

			default:
				i *= 1;
			}

#ifdef ACTIVE_EDITORS0
			if(ul_TotalOffset > _pst_Track->ul_DataLength)
			{
				/*~~~~~~~~~~~~~~~~~*/
				BIG_KEY ul_File;
				char	asz_Log[200];
				/*~~~~~~~~~~~~~~~~~*/

				ul_File = LOA_ul_GetCurrentKey();
				sprintf
				(
					asz_Log,
					"(%x)  Size of Specific data overpass allocated buffer [Release problem ONLY]",
					(ULONG) ul_File
				);
				ERR_X_Warning(0, asz_Log, NULL);
			}
#endif
		}

		MEMpro_StopMemRaster(MEMpro_Id_EVE_ListEvents);
	}

	*_pul_Add = pc_Buf - pc_Mem;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EVE_ul_CallbackLoadListTracks(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks *pst_All;
	ULONG				ul_Size, ul_Num;
	char				*pc_Buf;
	ULONG				i, ul_BitField;
	BOOL				b_AnimTrack;
	USHORT				uw_OptimFlags;
	BOOL				b_Change;
	extern int			BIG_gi_SpecialRead;
	BOOL				b_Spec;
	ULONG				LOA_30_OLD, LOA_31_OLD;
#ifdef ACTIVE_EDITORS
	extern int			EDI_gi_GenSpe;
	BIG_KEY				ul_File;
	char				asz_Path[BIG_C_MaxLenPath];
#endif
	extern int			WOR_gi_IsLoadingFix ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_All = (EVE_tdst_ListTracks *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));

#ifndef _FINAL_
	pst_All->ul_SizeOf_Ram = 0;
	pst_All->ul_SizeOf_Ram -= LOA_ul_FileTypeSize[30] + LOA_ul_FileTypeSize[31] + LOA_ul_FileTypeSize[32] + LOA_ul_FileTypeSize[33];
#endif
	LOA_ul_FileTypeSize[32] += sizeof(EVE_tdst_ListTracks) + 8;


	/*
	-----------------------------------------------------------------------------------------------------------------------
	TREAT FIX WORLD - BEGIN
	-----------------------------------------------------------------------------------------------------------------------
	*/

	//
	// if generating the fix world => register the key
	//
#ifdef ACTIVE_EDITORS
	if(EDI_gi_GenSpe) LOA_AddKeyToSpecialArray(LOA_ul_GetCurrentKey());
#endif


	//
	// if the key is into a fix world, set the instance nb to 1 000 000 (to forbit the unload action)
	//
	b_Spec = LOA_IsSpecial(LOA_ul_GetCurrentKey());
	if(b_Spec) 
	{
#ifdef JADEFUSION
		extern void LOA_AddAddressToSpecialArray(ULONG _h_Key, ULONG _ul_Adr);//POPOWARNING
#else
		extern LOA_AddAddressToSpecialArray(ULONG _h_Key, ULONG _ul_Adr);//POPOWARNING
#endif		
		LOA_AddAddressToSpecialArray(LOA_ul_GetCurrentKey(), (ULONG)pst_All);
		pst_All->ul_NbOfInstances = 1000000; /* This value (1000000) is USED for Fix desallocation in EVEinit.c. */
	}
	else
		pst_All->ul_NbOfInstances = 0;


	//
	// if binarizing, but not the fix then discard the data from bin process
	//
	b_Change = FALSE;
#ifdef ACTIVE_EDITORS
	if(BIG_gi_ReadMode == 1 && !BIG_gi_SpecialRead && b_Spec)
	{
		b_Change = TRUE;
		BIG_gi_ReadMode = 0;
	}
#endif
	/*
	-----------------------------------------------------------------------------------------------------------------------
	TREAT FIX WORLD - END
	-----------------------------------------------------------------------------------------------------------------------
	*/


	pc_Buf = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	/* Number of tracks */
	pst_All->uw_NumTracks = LOA_ReadUShort(&pc_Buf);
	if(!pst_All->uw_NumTracks) pst_All->uw_NumTracks = 0;
	pst_All->uw_Flags = LOA_ReadUShort(&pc_Buf);

#ifdef FORCE_ARAM
	pst_All->uw_Flags |= EVE_C_Tracks_ForceARAM;
#endif /* FORCE_ARAM */

	pst_All->uw_Flags &= ~EVE_C_Tracks_Resolved;
	pst_All->uw_Flags &= ~EVE_C_Tracks_Anims;

	/* Allocate all tracks */
	pst_All->pst_AllTracks = NULL;
	if(pst_All->uw_NumTracks)
	{
		pst_All->pst_AllTracks = (EVE_tdst_Track *) MEM_p_VMAlloc(pst_All->uw_NumTracks * sizeof(EVE_tdst_Track));
		LOA_ul_FileTypeSize[33] += pst_All->uw_NumTracks * sizeof(EVE_tdst_Track) + 8;
		L_memset(pst_All->pst_AllTracks, 0, pst_All->uw_NumTracks * sizeof(EVE_tdst_Track));
	}

	/* Parse all tracks */
	for(i = 0; i < pst_All->uw_NumTracks; i++)
	{
		/* Flags */
		pst_All->pst_AllTracks[i].uw_Flags = LOA_ReadUShort(&pc_Buf);

		uw_OptimFlags = pst_All->pst_AllTracks[i].uw_Flags & 0xFF00;

		uw_OptimFlags &= ~EVE_C_Track_Optimized;
		uw_OptimFlags &= ~EVE_C_Track_Anims;

		b_AnimTrack = pst_All->pst_AllTracks[i].uw_Flags & EVE_C_Track_Anims;

		/* We let the "Track already optimized" and "Anim" bits if set. */
		pst_All->pst_AllTracks[i].uw_Flags &= ~0x3F00;

		/* Gizmo */
		pst_All->pst_AllTracks[i].uw_Gizmo = LOA_ReadUShort(&pc_Buf);

		/* Data Length */
		pst_All->pst_AllTracks[i].ul_DataLength = LOA_ReadULong(&pc_Buf);

		if(!b_AnimTrack)
		{
			/* GAO */
			pst_All->pst_AllTracks[i].pst_GO = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
			if(pst_All->pst_AllTracks[i].pst_GO)
			{
#ifdef ACTIVE_EDITORS
				if((BIG_KEY) pst_All->pst_AllTracks[i].pst_GO == BIG_C_InvalidKey)
				{
					/*~~~~~~~~~~~~*/
					ULONG	l;
					char	az[512];
					/*~~~~~~~~~~~~*/

					pst_All->pst_AllTracks[i].pst_GO = NULL;
					l = BIG_ul_SearchKeyToFat(LOA_ul_GetCurrentKey());
					sprintf(az, "Tracklist %s, track number %d, invalid gao reference", BIG_NameFile(l), i);
					ERR_X_Warning(0, az, NULL);
				}
				else
#endif
				{
					LOA_MakeFileRef
					(
						(BIG_KEY) pst_All->pst_AllTracks[i].pst_GO,
						(ULONG *) &pst_All->pst_AllTracks[i].pst_GO,
						OBJ_ul_GameObjectCallback,
						LOA_C_MustExists
					);
				}
			}

			/* Dummy for track */
			LOA_ReadString_Ed(&pc_Buf, (CHAR *) &pst_All->pst_AllTracks[i].asz_Name, EVE_C_Tracks_LenName);
			LOA_ReadChar_Ed(&pc_Buf, &pst_All->pst_AllTracks[i].c_Color);
		}
		else
		{
			if(!uw_OptimFlags)
			{
				pst_All->pst_AllTracks[i].w_Type = (SHORT) LOA_ReadLong(&pc_Buf);
			}
		}

		ul_BitField = 0;
		if(b_AnimTrack)
			ul_BitField |= 1;
		else
			ul_BitField = 0;

		LOA_30_OLD = LOA_ul_FileTypeSize[30];
		LOA_31_OLD = LOA_ul_FileTypeSize[31];


		/* Load list events */
		EVE_LoadListEvents(&pst_All->pst_AllTracks[i], pc_Buf, &ul_Num, uw_OptimFlags, ul_BitField);

		pc_Buf += ul_Num;
	}

#ifdef ACTIVE_EDITORS
	ul_File = LOA_ul_GetCurrentKey();
	ul_File = BIG_ul_SearchKeyToFat(ul_File);
	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
	LINK_RegisterPointer(pst_All, LINK_C_ENG_TrackList, BIG_NameFile(ul_File), asz_Path);
	if(b_Change) BIG_gi_ReadMode = 1;
#endif


#ifndef _FINAL_
	pst_All->ul_SizeOf_Ram += LOA_ul_FileTypeSize[30] + LOA_ul_FileTypeSize[31] + LOA_ul_FileTypeSize[32] + LOA_ul_FileTypeSize[33];
#endif

	if(WOR_gi_IsLoadingFix > 1)
	{
		EVE_DeleteListTracks(pst_All);
		pst_All = NULL;
	}

	return(ULONG) pst_All;
}

