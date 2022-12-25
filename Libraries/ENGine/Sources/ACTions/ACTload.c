/*$T ACTload.c GC! 1.081 04/07/03 15:33:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "ACTstruct.h"
#include "ANImation/ANIload.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "EDIpaths.h"


#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#endif
#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)	ReadLong((char *) _a)
#else
#define _ReadLong(_a)	*(ULONG *) _a
#endif
#if defined(PSX2_TARGET) && (__cplusplus)
extern "C"
{
#endif



/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ACT_ul_ActionCallback(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*pc_Buffer;
	ULONG				ul_Size, ul_Mem;
	ACT_st_Action		*pst_Action;
	int					i, j;
	ACT_st_ActionItem	*p;
	BIG_KEY				ul_Key;
	BAS_tdst_barray		*pst_Trans;
	UCHAR				uc_NumberOfActionItem;
#ifdef ACTIVE_EDITORS
	BIG_INDEX			ul_Fat;
	char				asz_Path[BIG_C_MaxLenPath];

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#endif

	uc_NumberOfActionItem = 0;
	/* Read the entire action into a temporary buffer */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	ul_Mem = 0;
	if(ul_Size)
	{
		uc_NumberOfActionItem = LOA_ReadUChar(&pc_Buffer);

		/*
		 * Compute the minimal size to allocate. Only the used number of action items will
		 * be allocated instead of allocating the full ACT_C_SizeOfAction.
		 */
		ul_Mem = uc_NumberOfActionItem * sizeof(ACT_st_ActionItem);
		ul_Mem += sizeof(ACT_st_Action);
		ul_Mem -= sizeof(ACT_st_ActionItem) * ACT_C_SizeOfAction;
	}

	/* Allocate the necessary memory to store the action */
#ifdef ACTIVE_EDITORS
	pst_Action = (ACT_st_Action *) MEM_p_Alloc(sizeof(ACT_st_Action));

	/* Initialize the action */
	L_memset(pst_Action, 0, sizeof(ACT_st_Action));
	p = pst_Action->ast_ActionItem;
	for(i = 0; i < ACT_C_SizeOfAction; i++, p++)
	{
		p->uc_Repetition = 1;
		p->uc_Frequency = ACT_C_DefaultAnimFrequency;
	}

#else
	if(!ul_Mem)
		pst_Action = (ACT_st_Action *) MEM_p_Alloc(sizeof(ACT_st_Action));
	else
		pst_Action = (ACT_st_Action *) MEM_p_Alloc(ul_Mem);
#endif
	/* Load the action from the temporary buffer */
	if(ul_Mem)
	{
		pst_Action->uc_NumberOfActionItem = uc_NumberOfActionItem;
		pst_Action->uc_ActionItemNumberForLoop = LOA_ReadUChar(&pc_Buffer);

		/*
		 * The Counter is always set back to zero when loading, use in the engine only for
		 * reference counting, no need to save it.
		 */
		LOA_ReadUShort_Ed(&pc_Buffer, &pst_Action->uw_Counter);

		for(i = 0, p = pst_Action->ast_ActionItem; i < uc_NumberOfActionItem; ++i, ++p)
		{
			p->pst_TrackList = (EVE_tdst_ListTracks *) LOA_ReadULong(&pc_Buffer);	/* key resolved later
																					 * pst_Transitions only used as a
																					 * flag (i.e. Null or ptr) to
																					 * specify if there are
																					 * transitions, actual array is
																					 * stored after the action */
			p->pst_Transitions = (BAS_tdst_barray *) LOA_ReadLong(&pc_Buffer);
			p->pst_Shape = (ANI_tdst_Shape *) LOA_ReadULong(&pc_Buffer);			/* key resolved later */
			p->uc_Repetition = LOA_ReadUChar(&pc_Buffer);
			p->uc_NumberOfFrameForBlend = LOA_ReadUChar(&pc_Buffer);
			p->uc_Frequency = LOA_ReadUChar(&pc_Buffer);
			p->uc_CustomBit = LOA_ReadUChar(&pc_Buffer);
			p->uw_DesignFlags = LOA_ReadUShort(&pc_Buffer);
			p->uc_Flag = LOA_ReadUChar(&pc_Buffer);

			/* Not used, no need to save it in binary version */
			LOA_ReadUChar_Ed(&pc_Buffer, &p->uc_Dummy);

			if(pst_Action->ast_ActionItem[i].uc_Repetition == 0) pst_Action->ast_ActionItem[i].uc_Repetition = 1;
		}
	}
	else
	{
		pst_Action->uc_NumberOfActionItem = 0;
		pst_Action->uc_ActionItemNumberForLoop = 0;
	}

	pst_Action->uw_Counter = 0;

	/* Ask to load all the referenced animations */
	for(i = 0, p = pst_Action->ast_ActionItem; i < (int) uc_NumberOfActionItem; i++, p++)
	{
		/* Animations */
		ul_Key = (BIG_KEY) p->pst_TrackList;
#ifdef ACTIVE_EDITORS
		ul_Key = ANI_DataToTrack(ul_Key);
#endif
		if(ul_Key != BIG_C_InvalidIndex)
		{
			LOA_MakeFileRef(ul_Key, (ULONG *) &p->pst_TrackList, EVE_ul_CallbackLoadListTracks, LOA_C_MustExists);			

		}

#ifdef ACTIVE_EDITORS
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			char	asz_Warning[100];
			/*~~~~~~~~~~~~~~~~~~~~~*/
			extern ULONG			LOA_gul_CurrentKey;
			sprintf(asz_Warning, "Invalid Anim Key File (can't find) : %x referenced by %x", (BIG_KEY) p->pst_TrackList,LOA_gul_CurrentKey);
			ERR_X_Warning(0, asz_Warning, NULL);
			p->pst_TrackList = NULL;
		}

#endif
		/* Shape */
		ul_Key = (BIG_KEY) p->pst_Shape;
		if(ul_Key != BIG_C_InvalidIndex)
			LOA_MakeFileRef(ul_Key, (ULONG *) &p->pst_Shape, ANI_ul_ShapeCallback, LOA_C_MustExists);
		else
			p->pst_Shape = NULL;
	}

	/* Transitions */
	for(i = 0; i < uc_NumberOfActionItem; i++)
	{
		if(pst_Action->ast_ActionItem[i].pst_Transitions)
		{
			pst_Trans = (BAS_tdst_barray *) MEM_p_Alloc(sizeof(BAS_tdst_barray));

			/* the base key does not need to be loaded, it will be allocated below. */
			LOA_ReadLong_Ed(&pc_Buffer, (LONG *) &pst_Trans->base);
			pst_Trans->num = LOA_ReadInt(&pc_Buffer);

			/*
			 * The size of the array will be initialized to the number of elements, no need to
			 * load it.
			 */
			LOA_ReadInt_Ed(&pc_Buffer, &pst_Trans->size);
			pst_Trans->size = pst_Trans->num;
			pst_Trans->gran = LOA_ReadInt(&pc_Buffer);

#ifdef ACTIVE_EDITORS
			pst_Trans->base = (BAS_tdst_Key *) L_malloc(pst_Trans->size * sizeof(BAS_tdst_Key));
#else
			pst_Trans->base = (BAS_tdst_Key *) MEM_p_Alloc(pst_Trans->size * sizeof(BAS_tdst_Key));
#endif
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~*/

				/* Load the keys */
				BAS_tdst_Key		*pKey;
				ACT_tdst_Transition *pTrans;
				/*~~~~~~~~~~~~~~~~~~~~~~~~*/

				for(j = 0, pKey = pst_Trans->base; j < pst_Trans->num; ++j, ++pKey)
				{
					pKey->ul_Key = LOA_ReadLong(&pc_Buffer);

					/* ul_val will be allocated below, no need to load it. */
					LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pKey->ul_Val);
				}

				for(j = 0, pKey = pst_Trans->base; j < pst_Trans->num; ++j, ++pKey)
				{
					pKey->ul_Val = (ULONG) MEM_p_Alloc(sizeof(ACT_tdst_Transition));
					pTrans = (ACT_tdst_Transition *) pKey->ul_Val;
					pTrans->uw_Action = LOA_ReadUShort(&pc_Buffer);
					pTrans->uc_Flag = LOA_ReadUChar(&pc_Buffer);
					pTrans->uc_Blend = LOA_ReadUChar(&pc_Buffer);
				}
			}

			pst_Action->ast_ActionItem[i].pst_Transitions = pst_Trans;
		}
	}

#ifdef ACTIVE_EDITORS
	ul_Fat = BIG_ul_SearchKeyToFat(LOA_ul_GetCurrentKey());
	BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
	LINK_RegisterPointer(pst_Action, LINK_C_ENG_Action, BIG_NameFile(ul_Fat), asz_Path);
#endif
	/* Return the loaded action */
	return (ULONG) pst_Action;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ACT_ul_ActionKitCallback(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*pc_Buffer;
	ULONG				ul_Size, ul_ActionKitSize;
	ACT_st_ActionKit	*pst_ActionKit;
	int					i, iNumberOfAction;
	ACT_st_Action		**ppst_Action;
	BIG_KEY				ul_Key;
	ULONG				ul_Type;
#ifdef ACTIVE_EDITORS
	BIG_INDEX			ul_Fat;
	char				asz_Path[BIG_C_MaxLenPath];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Read the entire action kit into a temporary buffer */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	/* Test if this is an empty action kit */
	if(ul_Size == 0)
	{
		/* Allocate the necessary memory to store the action kit */
		pst_ActionKit = (ACT_st_ActionKit *) MEM_p_Alloc(sizeof(ACT_st_ActionKit));
		L_memset(pst_ActionKit, 0, sizeof(ACT_st_ActionKit));
	}
	else
	{
		if(LOA_IsBinaryData()) iNumberOfAction = ul_Size / sizeof(BIG_KEY);	   /* When loading binary data, the unused part of the group
															 * element is not there */
		else
			iNumberOfAction = ul_Size / sizeof(BIG_tdst_GroupElem);

#ifdef ACTIVE_EDITORS
		ul_ActionKitSize = sizeof(ACT_st_ActionKit);
#else
		ul_ActionKitSize = sizeof(ACT_st_ActionKit) - ((ACT_C_SizeOfActionKit - iNumberOfAction) * sizeof(ACT_st_Action *));
#endif
		/* Allocate the necessary memory to store the action kit */
		pst_ActionKit = (ACT_st_ActionKit *) MEM_p_Alloc(ul_ActionKitSize);
		L_memset(pst_ActionKit, 0, ul_ActionKitSize);
		pst_ActionKit->uw_NumberOfAction = iNumberOfAction;
		pst_ActionKit->uw_Counter = 0;
		pst_ActionKit->ul_Flag = 0;
		pst_ActionKit->ppst_ActionInits = NULL;

		/* Ask to load all the referenced actions */
		ppst_Action = &pst_ActionKit->apst_Action[0];
		for(i = 0; i < iNumberOfAction; i++, ppst_Action++)
		{
			ul_Key = LOA_ReadLong(&pc_Buffer);

			/* Tag to indicate that this Kit must be inserted into ARAM */
			if(ul_Key == 1)
			{
				pst_ActionKit->uw_NumberOfAction--;
				iNumberOfAction--;
				pst_ActionKit->ul_Flag |= 1;
				LOA_ReadULong_Ed(&pc_Buffer, &ul_Type);
				continue;
			}

			/*
			 * the type of the element is used only for validation when loading from binary
			 * data, we can assume it is valid, no need to save it.
			 */
			LOA_ReadULong_Ed(&pc_Buffer, &ul_Type);
#ifdef ENABLE_ERR_MSG
			ul_Type = *(ULONG *) EDI_Csz_ExtAction;
#endif /* #ifdef ENABLE_ERR_MSG */
			if(ul_Key != BIG_C_InvalidIndex && ul_Key != 0)
			{
				ERR_X_Assert(L_strncmp((char *) &ul_Type, EDI_Csz_ExtAction, 4) == 0);
				LOA_MakeFileRef(ul_Key, (ULONG *) ppst_Action, ACT_ul_ActionCallback, LOA_C_MustExists);
			}
			else
			{
				*ppst_Action = NULL;
			}
		}
	}

#ifdef _GAMECUBE
	pst_ActionKit->w_NumCacheLines = 10;
	pst_ActionKit->w_NextIndex = 0;
	L_memset(pst_ActionKit->al_CacheIndexes, -1, ACT_CacheLinesPerActionKit * sizeof(LONG));
#endif
#ifdef ACTIVE_EDITORS
	ul_Fat = BIG_ul_SearchKeyToFat(LOA_ul_GetCurrentKey());
	BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
	LINK_RegisterPointer(pst_ActionKit, LINK_C_ENG_ActionKit, BIG_NameFile(ul_Fat), asz_Path);
#endif
	/* Return the loaded action kit */
	return (ULONG) pst_ActionKit;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_ActionSaveInit(ACT_st_ActionKit *_pst_ActionKit)
{
	/*~~*/
	int i;
	/*~~*/

	if(_pst_ActionKit->ppst_ActionInits) return;
	_pst_ActionKit->ppst_ActionInits = (ACT_st_Action **) MEM_p_Alloc(_pst_ActionKit->uw_NumberOfAction * sizeof(void *));
	L_memcpy
	(
		_pst_ActionKit->ppst_ActionInits,
		_pst_ActionKit->apst_Action,
		_pst_ActionKit->uw_NumberOfAction * sizeof(void *)
	);
	for(i = 0; i < _pst_ActionKit->uw_NumberOfAction; i++)
	{
		if(_pst_ActionKit->ppst_ActionInits[i]) ACT_UseAction(_pst_ActionKit->ppst_ActionInits[i]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_ActionRestoreInit(ACT_st_ActionKit *_pst_ActionKit)
{
	/*~~*/
	int i;
	/*~~*/

	if(!_pst_ActionKit->ppst_ActionInits) return;
	for(i = 0; i < _pst_ActionKit->uw_NumberOfAction; i++)
	{
		if(_pst_ActionKit->apst_Action[i] == _pst_ActionKit->ppst_ActionInits[i])
		{
			if(_pst_ActionKit->apst_Action[i]) ACT_FreeAction(&_pst_ActionKit->apst_Action[i]);
			continue;
		}

		if(_pst_ActionKit->apst_Action[i]) ACT_FreeAction(&_pst_ActionKit->apst_Action[i]);
		_pst_ActionKit->apst_Action[i] = _pst_ActionKit->ppst_ActionInits[i];
	}

	MEM_Free(_pst_ActionKit->ppst_ActionInits);
	_pst_ActionKit->ppst_ActionInits = NULL;
}

#ifdef ACTIVE_EDITORS
extern BOOL ANI_b_ForceOptimization;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_DegradeActionKit(OBJ_tdst_GameObject *_pst_GO, ACT_st_ActionKit *_pst_ActionKit)
{
	/*~~~~~*/
	int i, j;
	ULONG ul_Fat;
	/*~~~~~*/


	/* Action Kits that we want to degrade */
	ul_Fat = BIG_ul_SearchFileExt("ROOT/EngineDatas/04 Technical Bank/Anim Optim", "Anim Optim.gol");
	if(ul_Fat ==-1) return;
	if(BIG_i_IsRefInGroup(ul_Fat, LOA_ul_SearchKeyWithAddress((ULONG)_pst_GO)) == -1) return;
 
	ANI_b_ForceOptimization = TRUE;

	for(i = 0; i < _pst_ActionKit->uw_NumberOfAction; i++)
	{
		if(_pst_ActionKit->apst_Action[i])
		{
			for(j = 0; j < _pst_ActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
			{
				EVE_MysticalOptimization
				(
					_pst_GO,
					_pst_ActionKit->apst_Action[i]->ast_ActionItem[j].pst_TrackList,
					1E-2f,
					1E-3f
				);
				EVE_SaveListTracks(_pst_GO, _pst_ActionKit->apst_Action[i]->ast_ActionItem[j].pst_TrackList);
			}
		}
	}

	ANI_b_ForceOptimization = FALSE;
}

#endif
#if defined(PSX2_TARGET) && (__cplusplus)
}
#endif
