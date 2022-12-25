/*$T ACTcompute.c GC! 1.081 11/13/02 17:43:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "BASe/BASarray.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIdebug.h"

#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
int ARAM_Anims_RAM = 0;
int ARAM_Anims_ARAM = 0;
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern EVE_tdst_ListTracks	*EVE_gpst_CurrentListTracks;
extern float				EVE_gf_ElapsedDT;

#ifndef _FINAL_
ULONG	ACT_RAMStoreKit[32] =
{
	0x330021b0,	0,	/* Bat */
	0x4902c4c1,	0,	/* Ann */
	0x4902cf70,	0,	/* Denham */
	0x3d00d46b,	0,	/* Hayes */
	0x72007354,	0,	/* Jack */
	0x3D00c9d7,	0,	/* JaF */
	0x4902CF41,	0,	/* Jimmy */
	0x72006B01,	0,	/* Marin */
	0x4902cf9f,	0,	/* Preston */
	0x26004e0d,	0,	/* Raptor */
	0x7200798c,	0,	/* Rex */
	0x72006CF5,	0,	/* KONG */
	0xFFFFFFFF,	0,
	0xFFFFFFFF,	0,
	0xFFFFFFFF,	0,
	0xFFFFFFFF,	0

};
#endif

#ifdef ANIMS_USE_ARAM

ACT_tdst_CacheNode	ACT_gast_Cache[ACT_MaxNumberOfActionKits * ACT_CacheLinesPerActionKit] = {0};
LONG				ACT_gl_FirstFreeIndex = 0;

void ACT_Cache_Update(ACT_st_ActionKit *_pst_ActionKit);

//char		*ACT_Cache_Memory[2*1024*1024];
//ULONG		ACT_ul_CachePtr = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ACT_i_Cache_GetGlobalIndex(EVE_tdst_ListTracks *_pst_TrackList)
{
	if(!_pst_TrackList) return -1;

	if(_pst_TrackList->pst_Cache)
	{
		return(_pst_TrackList->pst_Cache - ACT_gast_Cache);
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ACT_i_Cache_GetLocalIndex(ACT_st_ActionKit *_pst_ActionKit, EVE_tdst_ListTracks *_pst_TrackList)
{
	/*~~*/
	int i;
	/*~~*/

	if(!_pst_ActionKit || !_pst_TrackList) return -1;
	
	for(i = 0; i < _pst_ActionKit->w_NumCacheLines; i++)
	{
		if(ACT_gast_Cache[_pst_ActionKit->al_CacheIndexes[i]].pst_TrackList == _pst_TrackList) return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ARAM_DMA_ActionLoadCallback(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest		*p;
	EVE_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;

	pst_Track = (EVE_tdst_Track *) p->owner;
	pst_Track->w_Type |= EVE_C_Track_InRAMGC;

	fn_vARAMManagementAfterTransfer(p);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG ACT_FindFirstFreeIndex()
{
	int i;

	for(i = 0; i < ACT_MaxNumberOfActionKits * ACT_CacheLinesPerActionKit; i++)
	{
		if(!ACT_gast_Cache[i].pst_TrackList)
			return i;
	}

	OSReport("Global TrackList Cache FULL !!!");
	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ACT_i_Cache_LoadAnim(ACT_st_ActionKit *_pst_ActionKit, EVE_tdst_ListTracks *_pst_TrackList)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, index, indexL;
	EVE_tdst_Track		*pst_Track;
	EVE_tdst_ListTracks	*pst_TL;
	ACT_tdst_CacheNode	*pst_CacheLine;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_ActionKit || !_pst_TrackList) return -1;

	/* Check if the Anim is in the Global Array of Loaded-In-RAM TrackLists */
	index = ACT_i_Cache_GetGlobalIndex(_pst_TrackList);

	/* If (index != -1), the TrackList is in RAM */
	if(index != -1)
	{
		indexL = ACT_i_Cache_GetLocalIndex(_pst_ActionKit, _pst_TrackList);

		/*	TrackList is in RAM but it is not referenced by THIS ActionKit.
			That means that several ActionKits use this TrackList. Increase the CacheLine Share value */
		if(indexL == -1)
		{
			ACT_gast_Cache[index].l_Shares ++;
			
			indexL = _pst_ActionKit->w_NextIndex;
			if(_pst_ActionKit->al_CacheIndexes[indexL] == -1)
				_pst_ActionKit->al_CacheIndexes[indexL] = index;
			else
			{
				pst_CacheLine = &ACT_gast_Cache[_pst_ActionKit->al_CacheIndexes[indexL]];			
				
				if(pst_CacheLine->pst_TrackList)
				{
					if(pst_CacheLine->l_Shares == 1)
					{
						pst_TL = pst_CacheLine->pst_TrackList;
						for(i = 0; i < (int) pst_TL->uw_NumTracks; i++)
						{
							if(pst_TL->pst_AllTracks[i].w_Type & EVE_C_Track_InRAMGC)
							{
								MEM_Free(pst_TL->pst_AllTracks[i].pst_AllEvents);
								pst_TL->pst_AllTracks[i].pst_AllEvents = pst_CacheLine->ppst_ListEvents[i];
								pst_TL->pst_AllTracks[i].w_Type &= ~EVE_C_Track_InRAMGC;
							}
						}

						ARAM_Anims_RAM--;

						MEM_Free(pst_CacheLine->ppst_ListEvents);
						pst_TL->pst_Cache = NULL;
						pst_TL->ul_GC_Flags &= ~EVE_C_ListTracks_InRAMGC;
						
						pst_CacheLine->l_Shares = 1;		
						pst_CacheLine->l_Counter = 0;
						pst_CacheLine->l_Frames = 0;		
						pst_CacheLine->pst_TrackList = NULL;
					}
				}
					
				_pst_ActionKit->al_CacheIndexes[indexL] = index;		
			}			
		}
	}

	 /* If the Animation is not in the Pool, we add it in the first free spot or remove the oldest one. */
	if(index == -1)
	{
		index = _pst_ActionKit->w_NextIndex;

		if(_pst_ActionKit->al_CacheIndexes[index] == -1)
		{
			_pst_ActionKit->al_CacheIndexes[index] = ACT_FindFirstFreeIndex();
		}

		index = _pst_ActionKit->al_CacheIndexes[index];

		pst_CacheLine = &ACT_gast_Cache[index];

		if(pst_CacheLine->pst_TrackList)
		{
			/* If CacheLine is not a free one, we have to free the Anim that was there b4 (Oldest one for this ActionKit) */		
			if(pst_CacheLine->l_Shares == 1)
			{
				pst_TL = pst_CacheLine->pst_TrackList;
				for(i = 0; i < (int) pst_TL->uw_NumTracks; i++)
				{
					if(pst_TL->pst_AllTracks[i].w_Type & EVE_C_Track_InRAMGC)
					{
						MEM_Free(pst_TL->pst_AllTracks[i].pst_AllEvents);
						pst_TL->pst_AllTracks[i].pst_AllEvents = pst_CacheLine->ppst_ListEvents[i];
						pst_TL->pst_AllTracks[i].w_Type &= ~EVE_C_Track_InRAMGC;
					}
				}

				ARAM_Anims_RAM--;

				MEM_Free(pst_CacheLine->ppst_ListEvents);
				pst_TL->pst_Cache = NULL;
				pst_TL->ul_GC_Flags &= ~EVE_C_ListTracks_InRAMGC;
			}
			else
			{
				pst_CacheLine->l_Shares --;
				index = _pst_ActionKit->w_NextIndex;
				_pst_ActionKit->al_CacheIndexes[index] = ACT_FindFirstFreeIndex();
				index = _pst_ActionKit->al_CacheIndexes[index];
			
				pst_CacheLine = &ACT_gast_Cache[index];					
			}
		}
			
		pst_CacheLine->l_Shares = 1;				
		pst_CacheLine->l_Counter = 0;
		pst_CacheLine->l_Frames = 0;		
		pst_CacheLine->pst_TrackList = _pst_TrackList;
		_pst_TrackList->pst_Cache = pst_CacheLine;				
			
		/* Load all list events from Aram */
		pst_CacheLine->ppst_ListEvents = (EVE_tdst_Event **) MEM_p_Alloc(_pst_TrackList->uw_NumTracks * sizeof(EVE_tdst_Event *));		

		for(i = 0; i < (int) _pst_TrackList->uw_NumTracks; i++)
		{
			pst_Track = &_pst_TrackList->pst_AllTracks[i];

			if(pst_Track->w_Type & EVE_C_Track_UseARAM)
			{
				pst_CacheLine->ppst_ListEvents[i] = _pst_TrackList->pst_AllTracks[i].pst_AllEvents;
				_pst_TrackList->pst_AllTracks[i].pst_AllEvents = (EVE_tdst_Event *) MEM_p_AllocAlign(OSRoundUp32B(pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event)), 32);
					
				_pst_TrackList->pst_AllTracks[i].w_Type &= ~EVE_C_Track_InRAMGC;
				ARAM_GetFromARAM
				(
					(u32) _pst_TrackList->pst_AllTracks[i].pst_AllEvents,
					(u32) pst_CacheLine->ppst_ListEvents[i],
					OSRoundUp32B(pst_Track->uw_NumEvents * sizeof(EVE_tdst_Event)),
					(u32) & _pst_TrackList->pst_AllTracks[i],
					ARAM_DMA_ActionLoadCallback
				);
			}
		}

		ARAM_Anims_RAM++;

		/* For Now, horrible synchronous DMA transfer */
		for(i = 0; i < (int) _pst_TrackList->uw_NumTracks; i++)
		{
			if(_pst_TrackList->pst_AllTracks[i].w_Type & EVE_C_Track_UseARAM)
			{

				while(!(_pst_TrackList->pst_AllTracks[i].w_Type & EVE_C_Track_InRAMGC))
				{
				}
			}
		}

		_pst_TrackList->ul_GC_Flags |= EVE_C_ListTracks_InRAMGC;
	}
	else
    	ACT_gast_Cache[index].l_Counter = 0;

    ACT_Cache_Update(_pst_ActionKit);

	return index;
}

/*
 =======================================================================================================================
    Aim:    Update all Cache nodes internal counters and finds the oldest or first free node.
 =======================================================================================================================
 */
void ACT_Cache_Update(ACT_st_ActionKit *_pst_ActionKit)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		i;
	LONG	l_Oldest;
	LONG	l_OldestIndex;
	LONG	l_Cache;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!_pst_ActionKit) return;

	l_Oldest = -1;
	l_OldestIndex = -1;
	_pst_ActionKit->w_NextIndex = -1;
	for(i = 0; i < _pst_ActionKit->w_NumCacheLines; i++)
	{
		l_Cache = _pst_ActionKit->al_CacheIndexes[i];
		if(l_Cache != -1) 
			ACT_gast_Cache[l_Cache].l_Counter++;
		else
			_pst_ActionKit->w_NextIndex = i;

		if((_pst_ActionKit->w_NextIndex == -1) && (ACT_gast_Cache[l_Cache].l_Counter > l_Oldest))
		{
			l_Oldest = ACT_gast_Cache[l_Cache].l_Counter;
			l_OldestIndex  = i;
		}
	}
	
	if(_pst_ActionKit->w_NextIndex == -1)
	{
		if(l_OldestIndex == -1)
		{
#ifdef _DEBUG
			OSReport("[BUG 123]");
#endif
			l_OldestIndex = 0;
		}
		_pst_ActionKit->w_NextIndex = l_OldestIndex;
	}
	
}

#endif /* GAMECUBE */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_PartialAction_OnOff(OBJ_tdst_GameObject *_pst_GO, int _iPartialActionNumber, int _i_OnOff)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_tdst_Anim			*pst_PartialAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if
	(
		!_pst_GO
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->apst_PartialAnim[_iPartialActionNumber]
	)
		return;

	pst_PartialAnim = _pst_GO->pst_Base->pst_GameObjectAnim->apst_PartialAnim[_iPartialActionNumber];

	if(_i_OnOff)
	{
		if(pst_PartialAnim->uw_Flag & ANI_C_AnimFlag_DontPlayAnim)
			pst_PartialAnim->uw_Flag &= ~ANI_C_AnimFlag_DontPlayAnim;
	}
	else
	{
		if(!(pst_PartialAnim->uw_Flag & ANI_C_AnimFlag_DontPlayAnim))
		{
			pst_PartialAnim->uw_Flag |= ANI_C_AnimFlag_DontPlayAnim;
			pst_PartialAnim->uc_BlendTime = _pst_GO->pst_Base->pst_GameObjectAnim->uc_BlendLength_OUT;
			pst_PartialAnim->uw_BlendCurTime = 0;
		}
	}

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_SetPartialAction(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber, BOOL _b_Force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	ACT_st_ActionKit		*pst_ActionKit;
	ACT_st_Action			*pst_Action;
	ACT_st_ActionItem		*pst_ActionItem;
	EVE_tdst_ListTracks		*pst_TrackList;
	UCHAR					uc_Flag;
	ULONG					ul_DynaFlag;
	DYN_tdst_Dyna			*pst_Dyna;
	OBJ_tdst_GameObject		*pst_SaveGO;
	EVE_tdst_Data			*pst_SaveData;
	EVE_tdst_ListTracks		*pst_SaveListTracks;
	EVE_tdst_Track			*pst_SaveTrack;
	EVE_tdst_Event			*pst_SaveEvent;
	EVE_tdst_Params			*pst_SaveParam;
	MATH_tdst_Matrix		*pst_SaveRefMatrix;
	ANI_tdst_Anim			*pst_BlendAnim;
	int                     index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO || !_pst_GO->pst_Base || !_pst_GO->pst_Base->pst_GameObjectAnim) return;


    index = -1;
    pst_BlendAnim = NULL;
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_ActionKit = pst_GOAnim->pst_ActionKit;


	/* We ask again for a partial action currently in use. We reset the timer of the anim */
	if((_iActionNumber == pst_GOAnim->uw_CurrentPartialActionIndex) && pst_GOAnim->apst_PartialAnim[0])
	{
#ifdef ANIMS_USE_ARAM
		/* If data is not in Cache (and should be), Load it From ARAM */
		EVE_tdst_ListTracks		*pst_TrackList;
	
		pst_ActionKit = pst_GOAnim->pst_ActionKit;	
		pst_TrackList = pst_GOAnim->apst_PartialAnim[0]->pst_Data->pst_ListTracks;
		if((pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM))
		{		
			ACT_i_Cache_LoadAnim(pst_ActionKit, pst_TrackList);
		}
#endif
	
		EVE_SetTracksTime(pst_GOAnim->apst_PartialAnim[0]->pst_Data, 0.0f);
	}

	if((!pst_ActionKit) || (_iActionNumber < 0) || (_iActionNumber >= pst_ActionKit->uw_NumberOfAction)) return;

	if(_iActionNumber && !pst_ActionKit->apst_Action[_iActionNumber])
	{
#ifdef ACTIVE_EDITORS
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Msg[200];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Msg, "[ACT_PartialActionSet] %s has NO Action %i", _pst_GO->sz_Name, _iActionNumber);
			ERR_X_Warning(0, asz_Msg, NULL);
		}

#endif
		return;
	}

	pst_Action = pst_GOAnim->pst_CurrentPartialAction = pst_ActionKit->apst_Action[_iActionNumber];

	pst_GOAnim->uw_CurrentPartialActionIndex = (USHORT)_iActionNumber;
	pst_GOAnim->uc_CurrentPartialActionItemIndex = 0;

	/* Set the Anim */
	if(pst_Action && (pst_Action->uc_NumberOfActionItem > 0))
	{
		pst_ActionItem = &pst_Action->ast_ActionItem[0];
		pst_TrackList = pst_ActionItem->pst_TrackList;

#ifdef ACTIVE_EDITORS
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[100];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "Action [%u]  with invalid animation", _iActionNumber);
			if(ENG_gb_EngineRunning) AI_Check(pst_TrackList, asz_Log);
			if(!pst_TrackList) return;
		}

#endif
		/* Alloc first Partial Anim structure if needed. */
		if((!pst_GOAnim->apst_PartialAnim[0]) || ((ULONG) pst_GOAnim->apst_PartialAnim[0] == (ULONG) - 1))
			pst_GOAnim->apst_PartialAnim[0] = ANI_AllocAnim();

		/* Set Track List */
		if(!pst_GOAnim->apst_PartialAnim[0]->pst_Data || (pst_GOAnim->apst_PartialAnim[0]->pst_Data->pst_ListTracks != pst_TrackList))
		{
			ANI_SetTrackList(pst_GOAnim->apst_PartialAnim[0], pst_TrackList);
		}

		/* Change Shape if ActionItem has one. */
		if(pst_ActionItem->pst_Shape)
			ANI_ChangeShape(_pst_GO, pst_ActionItem->pst_Shape);
		else
			ANI_ChangeShape(_pst_GO, NULL);

		uc_Flag = pst_ActionItem->uc_Flag;

		/* Set the Flags for the Dynamics */
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Dyna))
		{
			pst_Dyna = _pst_GO->pst_Base->pst_Dyna;

			ul_DynaFlag = DYN_ul_GetDynFlags(pst_Dyna);
			ul_DynaFlag &= ~(DYN_C_IgnoreGravity | DYN_C_IgnoreTraction | DYN_C_IgnoreStream);

			ul_DynaFlag |=
			(
				(uc_Flag & ACT_C_ActionItemFlag_IgnoreGravity ? DYN_C_IgnoreGravity : 0) |
					(uc_Flag & ACT_C_ActionItemFlag_IgnoreTraction ? DYN_C_IgnoreTraction : 0) |
					(uc_Flag & ACT_C_ActionItemFlag_IgnoreStream ? DYN_C_IgnoreStream : 0)
			);

			DYN_SetDynFlags(pst_Dyna, ul_DynaFlag);
		}

		pst_GOAnim->apst_PartialAnim[0]->uc_BlendFlags = 0;
		pst_GOAnim->apst_PartialAnim[0]->uc_BlendTime = pst_GOAnim->uc_BlendLength_IN;
		pst_GOAnim->apst_PartialAnim[0]->uw_BlendCurTime = 0;

		/* Set the Mode for the Anim */
		pst_GOAnim->apst_PartialAnim[0]->uw_Flag &= ~ANI_C_AnimFlag_PlayModeMask;
		pst_GOAnim->apst_PartialAnim[0]->uw_Flag &= ~ANI_C_AnimFlag_DontPlayAnim;
		pst_GOAnim->apst_PartialAnim[0]->uw_Flag |= (uc_Flag & ANI_C_AnimFlag_PlayModeMask);

#ifdef ACTIVE_EDITORS
		pst_GOAnim->apst_PartialAnim[0]->uc_Mode = 0;
		if(pst_GOAnim->apst_PartialAnim[0]->uw_Flag & ANI_C_AnimFlag_DontPlayAnim) pst_GOAnim->apst_PartialAnim[0]->uc_Mode |= 1;
		if(pst_GOAnim->apst_PartialAnim[0]->uw_Flag == ANI_C_AnimFlag_SpeedFromAnim) pst_GOAnim->apst_PartialAnim[0]->uc_Mode |= 2;
		if(pst_GOAnim->apst_PartialAnim[0]->uw_Flag == ANI_C_AnimFlag_UpdateFrequency) pst_GOAnim->apst_PartialAnim[0]->uc_Mode |= 4;
		if(pst_GOAnim->apst_PartialAnim[0]->uw_Flag == ANI_C_AnimFlag_SpeedFromDyna) pst_GOAnim->apst_PartialAnim[0]->uc_Mode |= 8;
#endif
		/* For the moment, Default Frequency is 60 Hz */
		pst_GOAnim->apst_PartialAnim[0]->uc_AnimFrequency = (((int) pst_ActionItem->uc_Frequency) * 60 / ACT_C_DefaultAnimFrequency);



		/* Save the Events Global vars. */
		pst_SaveGO = EVE_gpst_OwnerGAO;
		pst_SaveData = EVE_gpst_CurrentData;
		pst_SaveListTracks = EVE_gpst_CurrentListTracks;
		pst_SaveRefMatrix = EVE_gpst_CurrentRefMatrix;
		pst_SaveTrack = EVE_gpst_CurrentTrack;
		pst_SaveParam = EVE_gpst_CurrentParam;
		pst_SaveEvent = EVE_gpst_CurrentEvent;

		if(!_pst_GO->pst_Extended || !(_pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_OptimAnim))
		{
			ANI_PartialAnim_PlayAll(_pst_GO, 0.0f);
		}

		/* Restore the Events Global vars. */
		EVE_gpst_OwnerGAO = pst_SaveGO;
		EVE_gpst_CurrentData = pst_SaveData;
		EVE_gpst_CurrentListTracks = pst_SaveListTracks;
		EVE_gpst_CurrentRefMatrix = pst_SaveRefMatrix;
		EVE_gpst_CurrentTrack = pst_SaveTrack;
		EVE_gpst_CurrentParam = pst_SaveParam;
		EVE_gpst_CurrentEvent = pst_SaveEvent;
	}

#ifdef ACTIVE_EDITORS
	LINK_UpdatePointer(_pst_GO);
#endif

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_SetAction(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber, int _iActionItemNumber, BOOL _b_Force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	ACT_st_ActionKit		*pst_ActionKit;
	ACT_st_Action			*pst_Action;
	ACT_st_ActionItem		*pst_ActionItem;
	EVE_tdst_ListTracks		*pst_TrackList;
	UCHAR					uc_Flag;
	ULONG					ul_DynaFlag;
	DYN_tdst_Dyna			*pst_Dyna;
	ACT_tdst_Transition		*pst_Transition;
	ACT_tdst_Transition		*pst_LastTransition;
	OBJ_tdst_GameObject		*pst_SaveGO;
	EVE_tdst_Data			*pst_SaveData;
	EVE_tdst_ListTracks		*pst_SaveListTracks;
	EVE_tdst_Track			*pst_SaveTrack;
	EVE_tdst_Event			*pst_SaveEvent;
	EVE_tdst_Params			*pst_SaveParam;
	MATH_tdst_Matrix		*pst_SaveRefMatrix;
	float					f_Cur, f_Time, f_TotalOld, f_TotalNew;
	unsigned short			uw_CurrentEvent, uw_NumEvents;
	float					f_Delay;
	ANI_tdst_Anim			*pst_BlendAnim;
	char					c_Trans;
	int                     index;
	BOOL					b_ForceNoBlend;
	BOOL					b_ForceReinitData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    index = -1;
    pst_BlendAnim = NULL;
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	/*
	 * If the 0x80000000 is added to the Action Number, that means that we want to
	 * force it even if the current action is the same one.
	 */
	if(_iActionNumber & 0x80000000)
	{
		_b_Force = TRUE;
		_iActionNumber &= ~0x80000000;
	}

	if(_iActionNumber & 0x40000000)
	{
		b_ForceNoBlend = TRUE;
		_iActionNumber &= ~0x40000000;
	}
	else
		b_ForceNoBlend = FALSE;

	if(_iActionNumber & 0x20000000)
	{
		b_ForceReinitData = TRUE;
		_iActionNumber &= ~0x20000000;
	}
	else
		b_ForceReinitData = FALSE;



	/*
	 * If this is the same action AND if the current Action is not finished, we do
	 * nothing
	 */
	if
	(
		!_b_Force
	&&	(_iActionNumber == pst_GOAnim->uw_CurrentActionIndex)
	&&	pst_GOAnim->apst_Anim[0]
	&&	(ANI_b_PlayAnim(pst_GOAnim->apst_Anim[0]))
	)
	{
#ifdef ANIMS_USE_ARAM
		/*
		 * Resets the cache counter of current Anim because another SetAction has been
		 * called for it
		 */
		{
			pst_TrackList = pst_GOAnim->pst_ActionKit->apst_Action[pst_GOAnim->uw_CurrentActionIndex]->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].pst_TrackList;

			if(pst_TrackList && pst_TrackList->pst_Cache)
				pst_TrackList->pst_Cache->l_Counter = 0;
		}

#endif
		return;
	}

	/*$F
	 * ------------------------------------------------------------------------------- 
	 * If the ACT_SetAction is called from EVE_Player, we MUST NOT complete the
	 * ACT_SetAction right now. Indeed, this ACT_SetAction may occur a fraction of
	 * time after the beginning of the current Frame and, if it is the case, we have
	 * to play the previous Action during this fraction of time and ONLY THEN perform
	 * the ACT_SetAction. So, we only set Flags for the ANI_Player to give it the Info
	 * that a ACT_SetAction has been ordered.
	 * ------------------------------------------------------------------------------- 
	 */
	if(ENG_gb_EVERunning)
	{
		_pst_GO->pst_Base->pst_GameObjectAnim->uc_Frequency = 255;
		_pst_GO->pst_Base->pst_GameObjectAnim->f_NextActionDT = EVE_gf_ElapsedDT;
		_pst_GO->pst_Base->pst_GameObjectAnim->uw_NextActionIndex = _iActionNumber;
		return;
	}

	pst_ActionKit = pst_GOAnim->pst_ActionKit;
	if((!pst_ActionKit) || (_iActionNumber < 0) || (_iActionNumber >= pst_ActionKit->uw_NumberOfAction)) return;

	if(_iActionNumber && !pst_ActionKit->apst_Action[_iActionNumber])
	{
#ifdef ACTIVE_EDITORS
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Msg[200];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Msg, "[ACT_ActionSet] %s has NO Action %i", _pst_GO->sz_Name, _iActionNumber);
			ERR_X_Warning(0, asz_Msg, NULL);
		}

#endif
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Search for a transition
	 -------------------------------------------------------------------------------------------------------------------
	 */

	c_Trans = 0;
	pst_LastTransition = NULL;
	if(pst_GOAnim->pst_Transition)
	{
		/* Change to the same action */
		pst_LastTransition = pst_GOAnim->pst_Transition;
		c_Trans = 1;
		if(pst_GOAnim->uw_NextActionIndex == _iActionNumber) return;
		pst_GOAnim->pst_Transition = NULL;
	}

	/* Resets possible blend */
	pst_GOAnim->uc_AnimUsed &= ~2;

	if(pst_GOAnim->uw_CurrentActionIndex != 0xFFFF)
	{
		pst_Transition = ACT_pst_FindTransition
			(
				pst_ActionKit,
				pst_GOAnim->uw_CurrentActionIndex,
				pst_GOAnim->uc_CurrentActionItemIndex,
				(USHORT) _iActionNumber
			);

		/* No default transition between current action and itself */
		if
		(
			pst_Transition
		&& !_b_Force
		&&	pst_GOAnim->uw_CurrentActionIndex == (USHORT) _iActionNumber
		&&	pst_Transition->uc_Flag & ACT_C_TF_DefaultTransition
		)
		{
			pst_Transition = NULL;
		}

		/* Default transition */
		if
		(
			!pst_Transition
		&&	(
				!c_Trans
			||	(pst_LastTransition && pst_LastTransition->uc_Flag & ACT_C_TF_NextDefaultTransition)
			||	(
					pst_LastTransition && _iActionNumber != pst_GOAnim->uw_NextActionIndex && pst_GOAnim->
						uw_NextActionIndex != 0xFFFF
				)
			)
		)
		{
			pst_Transition = ACT_pst_FindTransition(pst_ActionKit, (USHORT) _iActionNumber, 0, (USHORT) _iActionNumber);
			if(pst_Transition && (!(pst_Transition->uc_Flag & ACT_C_TF_DefaultTransition))) pst_Transition = NULL;
			if
			(
				pst_Transition
			&&	pst_GOAnim->uw_CurrentActionIndex == (USHORT) _iActionNumber
			&&	!(pst_Transition->uc_Flag & ACT_C_TF_NextDefaultTransition)
			) pst_Transition = NULL;
		}

		if(!pst_Transition)
		{
			if(pst_ActionKit->DefaultTrans.uc_Blend)
			{
				pst_Transition = &pst_ActionKit->DefaultTrans;
				pst_Transition->uw_Action = _iActionNumber;
			}
		}

		if(b_ForceNoBlend)
			pst_Transition = NULL;

		if(pst_Transition)
		{
			/* Get previous anim progression if Progressive or Inverse progressive transition */
			if(pst_Transition->uc_Flag & (ACT_C_TF_BlendProgressive | ACT_C_TF_BlendProgressiveInv))
				EVE_GetTracksTime(pst_GOAnim->apst_Anim[0]->pst_Data, 0, &f_Cur, &f_TotalOld);

			/* Compute and store parameters for blend */
			if(pst_Transition->uc_Blend && pst_GOAnim->apst_Anim[0])
			{
				/* Alloc second anim if not already allocated */
				if(!(pst_GOAnim->uc_AnimUsed & 2) && !pst_GOAnim->apst_Anim[1])
					pst_GOAnim->apst_Anim[1] = ANI_AllocAnim();
				pst_BlendAnim = pst_GOAnim->apst_Anim[1];
				
				/* There's a blend so anim 2 have to be used */
				pst_GOAnim->uc_AnimUsed |= 2;

				/* Get all information about old animation and store it into second anim */
				pst_Dyna = (DYN_tdst_Dyna *) pst_BlendAnim->pst_Data;
				L_memcpy(pst_BlendAnim, pst_GOAnim->apst_Anim[0], sizeof(ANI_tdst_Anim));
				pst_GOAnim->apst_Anim[0]->pst_Data = (EVE_tdst_Data *) pst_Dyna;
				pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks = NULL;

				/* Copy flash matrix into second Anim Reference Matrix */
				MATH_CopyMatrix(&pst_BlendAnim->st_Ref, OBJ_pst_GetFlashMatrix(_pst_GO));

				/* Store other parameters : blend time, blended action and action item, blend flags */
				pst_BlendAnim->uc_BlendTime = pst_Transition->uc_Blend;
				pst_BlendAnim->uw_BlendedAction = pst_GOAnim->uw_CurrentActionIndex;
				pst_BlendAnim->uc_BlendedActionItem = pst_GOAnim->uc_CurrentActionItemIndex;
				pst_BlendAnim->uc_BlendFlags = pst_Transition->uc_Flag;


				/* Temp STOCK */
				
				pst_BlendAnim->uc_BlendFlags |= ACT_C_TF_BlendStock;
				pst_BlendAnim->uc_BlendFlags &= ~ACT_C_TF_BlendFreezeBones;
				
				/* End Temp STOCK */


				/* Init some other data : current blend time and current flags */
				pst_BlendAnim->uw_BlendCurTime = 256;
				pst_BlendAnim->uc_MiscFlags = 0;

				/* Freeze blended animation if anim have not to be played anymore */
				if(!ANI_b_PlayAnim(pst_BlendAnim))
				{
					pst_BlendAnim->uc_BlendFlags |= ACT_C_TF_BlendFreezeBones;
					pst_BlendAnim->uc_MiscFlags |= ANI_C_MiscFlag_MagicBox_AfterEnd;
				}
				else
				{
					/* Look if Blend Anim is at its very end. */
					uw_CurrentEvent = pst_BlendAnim->pst_Data->pst_ListParam->uw_CurrentEvent;
					uw_NumEvents = pst_BlendAnim->pst_Data->pst_ListTracks->pst_AllTracks->uw_NumEvents - 1;

					if(uw_CurrentEvent == uw_NumEvents)
					{
						f_Time = pst_BlendAnim->pst_Data->pst_ListParam->f_Time;
						f_Delay = EVE_FrameToTime(pst_BlendAnim->pst_Data->pst_ListTracks->pst_AllTracks->pst_AllEvents[uw_CurrentEvent].uw_NumFrames & 0x7FFF);

						if(f_Time == f_Delay)
						{
							pst_BlendAnim->uc_BlendFlags |= ACT_C_TF_BlendFreezeBones;
							pst_BlendAnim->uc_MiscFlags |= ANI_C_MiscFlag_MagicBox_AfterEnd;
						}
					}
				}
			}

			pst_GOAnim->uw_NextActionIndex = (unsigned short) _iActionNumber;
			pst_GOAnim->pst_Transition = pst_Transition;
			_iActionNumber = pst_Transition->uw_Action;
		}
	}
	else
		pst_Transition = NULL;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Set action
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_Action = pst_GOAnim->pst_CurrentAction = pst_ActionKit->apst_Action[_iActionNumber];
	pst_GOAnim->uw_CurrentActionIndex = (unsigned short)_iActionNumber;

	/* Set the Anim */
	if(pst_Action && (pst_Action->uc_NumberOfActionItem > _iActionItemNumber))
	{
		pst_ActionItem = &pst_Action->ast_ActionItem[_iActionItemNumber];
		pst_TrackList = pst_ActionItem->pst_TrackList;

#ifdef ANIMS_USE_ARAM
		/* If TrackList of ActionItem is not in Cache (and should be), Load it From ARAM */
		if((pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM))
		{		
			ACT_i_Cache_LoadAnim(pst_ActionKit, pst_TrackList);
		}
#endif
#ifdef ACTIVE_EDITORS
		/* Check the pst_TrackList pointer. */
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[100];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "Action [%u]  with invalid animation", _iActionNumber);
			if(ENG_gb_EngineRunning) AI_Check(pst_TrackList, asz_Log);
			if(!pst_TrackList) return;
		}

#endif
		/* Alloc first Anim structure if needed. */
		if((!pst_GOAnim->apst_Anim[0]) || ((ULONG) pst_GOAnim->apst_Anim[0] == (ULONG) - 1))
			pst_GOAnim->apst_Anim[0] = ANI_AllocAnim();

		/* Set Track List */
		if(!pst_GOAnim->apst_Anim[0]->pst_Data || b_ForceReinitData || (pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks != pst_TrackList))
		{
			ANI_SetTrackList(pst_GOAnim->apst_Anim[0], pst_TrackList);
		}

		/* Change Shape if ActionItem has one. */
		if(pst_ActionItem->pst_Shape)
			ANI_ChangeShape(_pst_GO, pst_ActionItem->pst_Shape);
		else
			ANI_ChangeShape(_pst_GO, NULL);

		pst_GOAnim->uc_AnimUsed |= 1;	/* Use the first anim */
		pst_GOAnim->uc_CurrentActionItemIndex = _iActionItemNumber;
		pst_GOAnim->uc_CounterForRepetition = 0;

		if(pst_GOAnim->uc_ForceMode)
			uc_Flag = pst_GOAnim->uc_ForceMode - 1;
		else
			uc_Flag = pst_ActionItem->uc_Flag;

		/* Set the Flags for the Dynamics */
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Dyna))
		{
			pst_Dyna = _pst_GO->pst_Base->pst_Dyna;

			ul_DynaFlag = DYN_ul_GetDynFlags(pst_Dyna);
			ul_DynaFlag &= ~(DYN_C_IgnoreGravity | DYN_C_IgnoreTraction | DYN_C_IgnoreStream);

			ul_DynaFlag |=
			(
				(uc_Flag & ACT_C_ActionItemFlag_IgnoreGravity ? DYN_C_IgnoreGravity : 0) |
					(uc_Flag & ACT_C_ActionItemFlag_IgnoreTraction ? DYN_C_IgnoreTraction : 0) |
						(uc_Flag & ACT_C_ActionItemFlag_IgnoreStream ? DYN_C_IgnoreStream : 0)
			);

			DYN_SetDynFlags(pst_Dyna, ul_DynaFlag);
		}

		/* Set the Mode for the Anim */
		pst_GOAnim->apst_Anim[0]->uw_Flag &= ~ANI_C_AnimFlag_PlayModeMask;
		pst_GOAnim->apst_Anim[0]->uw_Flag &= ~ANI_C_AnimFlag_DontPlayAnim;
		pst_GOAnim->apst_Anim[0]->uw_Flag |= (uc_Flag & ANI_C_AnimFlag_PlayModeMask);

		if(uc_Flag & ACT_C_ActionItemFlag_Developped)
			pst_GOAnim->apst_Anim[0]->uw_Flag |= ANI_C_AnimFlag_Developped;
		else
			pst_GOAnim->apst_Anim[0]->uw_Flag  &= ~ANI_C_AnimFlag_Developped;

#ifdef ACTIVE_EDITORS
		pst_GOAnim->apst_Anim[0]->uc_Mode = 0;
		if(pst_GOAnim->apst_Anim[0]->uw_Flag & ANI_C_AnimFlag_DontPlayAnim) pst_GOAnim->apst_Anim[0]->uc_Mode |= 1;
		if(pst_GOAnim->apst_Anim[0]->uw_Flag == ANI_C_AnimFlag_SpeedFromAnim) pst_GOAnim->apst_Anim[0]->uc_Mode |= 2;
		if(pst_GOAnim->apst_Anim[0]->uw_Flag == ANI_C_AnimFlag_UpdateFrequency)
			pst_GOAnim->apst_Anim[0]->uc_Mode |= 4;
		if(pst_GOAnim->apst_Anim[0]->uw_Flag == ANI_C_AnimFlag_SpeedFromDyna) pst_GOAnim->apst_Anim[0]->uc_Mode |= 8;
#endif
		/* For the moment, Default Frequency is 60 Hz */
		pst_GOAnim->apst_Anim[0]->uc_AnimFrequency = (((int) pst_ActionItem->uc_Frequency) * 60 / ACT_C_DefaultAnimFrequency);

		if(pst_Transition)
		{
			if(pst_Transition->uc_Flag & (ACT_C_TF_BlendProgressive | ACT_C_TF_BlendProgressiveInv))
			{
				EVE_GetTracksTime(pst_GOAnim->apst_Anim[0]->pst_Data, 0, NULL, &f_TotalNew);
				if(pst_Transition->uc_Flag & ACT_C_TF_BlendProgressive)
					f_Time = f_Cur / f_TotalOld * f_TotalNew;
				else
					f_Time = (1.0f - f_Cur / f_TotalOld) * f_TotalNew;
				EVE_SetTracksTime(pst_GOAnim->apst_Anim[0]->pst_Data, f_Time);

				/* Have to set good Magic Box position */
				ANI_UpdateFlash(_pst_GO, ANI_C_UpdateLeadAnim);
			}

			if(pst_GOAnim->uw_NextActionIndex == pst_GOAnim->uw_CurrentActionIndex)
			{
				pst_GOAnim->pst_Transition = NULL;
				pst_GOAnim->uw_NextActionIndex = (USHORT) - 1;
			}
		}

		/* ACT_ActionFrameSet Case ... */
		if(ENG_gb_AIRunning && pst_GOAnim->uw_Frame)
		{
			/*~~~~~~~~~~~~~~~~~~~~*/
			EVE_tdst_Data	*p_Data;
			/*~~~~~~~~~~~~~~~~~~~~*/

			p_Data = pst_GOAnim->apst_Anim[0]->pst_Data;
			EVE_SetTracksTime(p_Data, pst_GOAnim->uw_Frame * (1.0f / 60.0f));
		}

		/*
		 * Called ANI_Player now to be sure animation will be "visible" in the current
		 * frame
		 */
		f_Time = TIM_gf_dt;
		TIM_gf_dt = 0.0f;

		/* Save the Events Global vars. */
		pst_SaveGO = EVE_gpst_OwnerGAO;
		pst_SaveData = EVE_gpst_CurrentData;
		pst_SaveListTracks = EVE_gpst_CurrentListTracks;
		pst_SaveRefMatrix = EVE_gpst_CurrentRefMatrix;
		pst_SaveTrack = EVE_gpst_CurrentTrack;
		pst_SaveParam = EVE_gpst_CurrentParam;
		pst_SaveEvent = EVE_gpst_CurrentEvent;

		if(!_pst_GO->pst_Extended || !(_pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_OptimAnim))
		{
			ANI_OneCall(_pst_GO);
		}

		/* Restore the Events Global vars. */
		EVE_gpst_OwnerGAO = pst_SaveGO;
		EVE_gpst_CurrentData = pst_SaveData;
		EVE_gpst_CurrentListTracks = pst_SaveListTracks;
		EVE_gpst_CurrentRefMatrix = pst_SaveRefMatrix;
		EVE_gpst_CurrentTrack = pst_SaveTrack;
		EVE_gpst_CurrentParam = pst_SaveParam;
		EVE_gpst_CurrentEvent = pst_SaveEvent;

		TIM_gf_dt = f_Time;
	}
	else
	{
		pst_GOAnim->uc_AnimUsed = 0;	/* No anim */
	}

#ifdef ACTIVE_EDITORS
	LINK_UpdatePointer(_pst_GO);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
struct ACT_tdst_Transition_ *ACT_pst_FindTransition
(
	ACT_st_ActionKit	*_pst_ActionKit,
	USHORT				_uw_CurrentAction,
	UCHAR				_uc_CurrentItem,
	USHORT				_uw_DestAction
)
{
	/*~~~~~*/
	int iRes;
	/*~~~~~*/

	/* No current action */
	if(_uw_CurrentAction == 0xFFFF) return NULL;
	ERR_X_Assert(_pst_ActionKit);
	if(_pst_ActionKit->apst_Action[_uw_CurrentAction] == NULL) return NULL;
	if(!_pst_ActionKit->apst_Action[_uw_CurrentAction]->ast_ActionItem[_uc_CurrentItem].pst_Transitions) return NULL;
	iRes = BAS_bsearch
		(
			_uw_DestAction,
			_pst_ActionKit->apst_Action[_uw_CurrentAction]->ast_ActionItem[_uc_CurrentItem].pst_Transitions
		);
	if(iRes == -1) return NULL;
	return (ACT_tdst_Transition *) iRes;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_SetTransition
(
	ACT_st_ActionKit	*_pst_ActionKit,
	USHORT				_uw_CurrentAction,
	UCHAR				_uc_CurrentItem,
	USHORT				_uw_DestAction,
	ACT_tdst_Transition *_pst_Transition
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_barray		*pst_Array;
	ACT_tdst_Transition *pst_Trans;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Allocate transition array ? */
	ERR_X_Assert(_pst_ActionKit);
	ERR_X_Assert(_pst_ActionKit->apst_Action[_uw_CurrentAction]);
	pst_Array = _pst_ActionKit->apst_Action[_uw_CurrentAction]->ast_ActionItem[_uc_CurrentItem].pst_Transitions;
	if(!pst_Array)
	{
		pst_Array = (BAS_tdst_barray *) MEM_p_Alloc(sizeof(BAS_tdst_barray));
		_pst_ActionKit->apst_Action[_uw_CurrentAction]->ast_ActionItem[_uc_CurrentItem].pst_Transitions = pst_Array;
		BAS_binit(pst_Array, 5);
	}

	pst_Trans = (ACT_tdst_Transition *) MEM_p_Alloc(sizeof(ACT_tdst_Transition));
	L_memmove(pst_Trans, _pst_Transition, sizeof(ACT_tdst_Transition));
	BAS_binsert(_uw_DestAction, (ULONG) pst_Trans, pst_Array);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_RemoveTransition
(
	ACT_st_ActionKit	*_pst_ActionKit,
	USHORT				_uw_CurrentAction,
	UCHAR				_uc_CurrentItem,
	USHORT				_uw_DestAction
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_barray		*pst_Array;
	ACT_tdst_Transition *pst_Transition;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_ActionKit);
	ERR_X_Assert(_pst_ActionKit->apst_Action[_uw_CurrentAction]);
	pst_Array = _pst_ActionKit->apst_Action[_uw_CurrentAction]->ast_ActionItem[_uc_CurrentItem].pst_Transitions;
	if(!pst_Array) return;

	/* Search transition */
	pst_Transition = (ACT_tdst_Transition *) BAS_bsearch
		(
			_uw_DestAction,
			_pst_ActionKit->apst_Action[_uw_CurrentAction]->ast_ActionItem[_uc_CurrentItem].pst_Transitions
		);
	if((int) pst_Transition == -1) return;

	/* Free it */
	MEM_Free(pst_Transition);

	/* Delete of table */
	BAS_bdelete(_uw_DestAction, pst_Array);

	/* Delete table if it was the last transition */
	if(pst_Array->num == 0)
	{
		BAS_bfree(pst_Array);
		MEM_Free(pst_Array);
		_pst_ActionKit->apst_Action[_uw_CurrentAction]->ast_ActionItem[_uc_CurrentItem].pst_Transitions = NULL;
	}
}
#endif

#ifndef _FINAL_

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ACT_ul_ComputeOneACKSize(ACT_st_ActionKit *_pst_ActionKit)
{
	int		i, j;
	int		SizeOf;

	if(!_pst_ActionKit) return 0;

	SizeOf = 0;
	for(i = 0; i < _pst_ActionKit->uw_NumberOfAction; i++)
	{
		if(!_pst_ActionKit->apst_Action[i])
			continue;

		for(j = 0; j < _pst_ActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
		{
			if(_pst_ActionKit->apst_Action[i]->ast_ActionItem[j].pst_TrackList)
				SizeOf += _pst_ActionKit->apst_Action[i]->ast_ActionItem[j].pst_TrackList->ul_SizeOf_Ram;
		}
	}

	return SizeOf;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void	ACT_ul_ComputeWanted()
{
	ACT_st_ActionKit	*pst_ActionKit;
	int					i, pos;

	for(i = 0; i < 32; i+= 2)
	{
		if(ACT_RAMStoreKit[i] == 0xFFFFFFFF)
			continue;

		pos = BIG_ul_SearchKeyToPos(ACT_RAMStoreKit[i]);
		if(pos == 0xFFFFFFFF) continue;

		pst_ActionKit = (ACT_st_ActionKit *) LOA_ul_SearchAddress(pos);

		if(!pst_ActionKit || ((ULONG) pst_ActionKit == 0xFFFFFFFF))
			continue;

		ACT_RAMStoreKit[i + 1] = ACT_ul_ComputeOneACKSize(pst_ActionKit);
	}

}
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
