 /*$T ACTsave.c GC! 1.081 05/03/00 17:36:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/ACtions/ACTstruct.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/LOading/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BASe/MEMory/MEM.h"
#include "ACTinit.h"
#include "ACTsave.h"
#include "ACTload.h"

BOOL	ACT_gb_ForceARAM = FALSE;
BOOL	ACT_gb_ForceSaveListTracks = FALSE;


void EVE_SaveListTracks(OBJ_tdst_GameObject *, EVE_tdst_ListTracks *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_LoadSetAction(ACT_st_ActionKit *_pst_Kit, BIG_INDEX _ul_Index, int _i_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ACT_st_Action	*pst_Action;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Action = (ACT_st_Action *) LOA_ul_SearchAddress(BIG_PosFile(_ul_Index));
	if(pst_Action == (ACT_st_Action *) BIG_C_InvalidIndex)
	{
		LOA_MakeFileRef(BIG_FileKey(_ul_Index), (ULONG *) &pst_Action, ACT_ul_ActionCallback, LOA_C_MustExists);
		LOA_Resolve();
	}
	else
	{
		ACT_UseAction(pst_Action);
	}
	if(!pst_Action) return;
	if((int) pst_Action == -1) return;

	/* Insert action in action kit */
	if(_i_Pos == -1)
	{
		_pst_Kit->apst_Action[_pst_Kit->uw_NumberOfAction] = pst_Action;
		_pst_Kit->uw_NumberOfAction++;
	}
	else
	{
		ERR_X_Assert(_i_Pos < _pst_Kit->uw_NumberOfAction);
		if(_pst_Kit->apst_Action[_i_Pos]) ACT_FreeAction(&_pst_Kit->apst_Action[_i_Pos]);
		_pst_Kit->apst_Action[_i_Pos] = pst_Action;
	}

	/* Action kit */
	ACT_ul_SaveActionKit(_pst_Kit);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACT_NewAction(ACT_st_ActionKit *_pst_Kit, char *_psz_Name, int _i_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenPath];
	BIG_INDEX		ul_Index;
	ACT_st_Action	*pst_Action;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ACT_ActionRestoreInit(_pst_Kit);
	ERR_X_Error(_pst_Kit->uw_NumberOfAction != ACT_C_SizeOfActionKit, "Too many actions in action kit", NULL);

	if(BIG_b_CheckName(_psz_Name) == FALSE) return;
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Kit);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

	L_strcpy(asz_Name, _psz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtAction);
	ERR_X_Error(BIG_ul_SearchFileExt(asz_Path, asz_Name) == BIG_C_InvalidIndex, "Action File Already Exists !", NULL);

	/* Create file for action */
	ul_Index = BIG_ul_CreateFile(asz_Path, asz_Name);

	/* Allocate and associate file */
	pst_Action = (ACT_st_Action*)MEM_p_Alloc(sizeof(ACT_st_Action));
	L_memset(pst_Action, 0, sizeof(ACT_st_Action));
	ACT_UseAction(pst_Action);
	LOA_AddAddress(ul_Index, pst_Action);

	ACT_ul_SaveAction(pst_Action);

	/* Insert action in action kit */
	if(_i_Pos == -1)
	{
		_pst_Kit->apst_Action[_pst_Kit->uw_NumberOfAction] = pst_Action;
		_pst_Kit->uw_NumberOfAction++;
	}
	else
	{
		ERR_X_Assert(_i_Pos < _pst_Kit->uw_NumberOfAction);
		if(_pst_Kit->apst_Action[_i_Pos]) ACT_FreeAction(&_pst_Kit->apst_Action[_i_Pos]);
		_pst_Kit->apst_Action[_i_Pos] = pst_Action;
	}

	/* Action kit */
	ACT_ul_SaveActionKit(_pst_Kit);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ACT_ul_SaveAction(ACT_st_Action *_pst_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Index;
	int					i, j, iNumberOfActionItem;
	ACT_st_ActionItem	*pst_ActionItem;
	ACT_st_ActionItem	st_ActionItem;
	char				az_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search the index of the file into the FAT */
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Action);
	if(ul_Index == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;

	/* Compute the file name and the path name and open the file */
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), az_Path);
	SAV_Begin(az_Path, BIG_NameFile(ul_Index));

	/* Save the content of the file */
	iNumberOfActionItem = _pst_Action->uc_NumberOfActionItem;
	if(iNumberOfActionItem)
	{
		SAV_Buffer(_pst_Action, 4);

		/* Animations */
		pst_ActionItem = _pst_Action->ast_ActionItem;
		for(i = 0; i < iNumberOfActionItem; i++, pst_ActionItem++)
		{
			st_ActionItem = *pst_ActionItem;
			if(st_ActionItem.pst_TrackList)
				st_ActionItem.pst_TrackList = (EVE_tdst_ListTracks *) LOA_ul_SearchKeyWithAddress((ULONG) st_ActionItem.pst_TrackList);
			if(st_ActionItem.pst_Shape)
				st_ActionItem.pst_Shape = (ANI_tdst_Shape *) LOA_ul_SearchKeyWithAddress((ULONG) st_ActionItem.pst_Shape);
			else
				st_ActionItem.pst_Shape = (ANI_tdst_Shape *) BIG_C_InvalidKey;
//			st_ActionItem.pst_Transitions = NULL;
			SAV_Buffer(&st_ActionItem, sizeof(st_ActionItem));
		}

		/* Transitions */
		for(i = 0; i < iNumberOfActionItem; i++)
		{
			if(_pst_Action->ast_ActionItem[i].pst_Transitions)
			{
				SAV_Buffer(_pst_Action->ast_ActionItem[i].pst_Transitions, sizeof(BAS_tdst_barray));
				SAV_Buffer
				(
					_pst_Action->ast_ActionItem[i].pst_Transitions->base,
					_pst_Action->ast_ActionItem[i].pst_Transitions->num * sizeof(BAS_tdst_Key)
				);
				for(j = 0; j < _pst_Action->ast_ActionItem[i].pst_Transitions->num; j++)
				{
					SAV_Buffer
					(
						(void *) _pst_Action->ast_ActionItem[i].pst_Transitions->base[j].ul_Val,
						sizeof(ACT_tdst_Transition)
					);
				}
			}
		}
	}

	return SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ACT_ul_SaveActionKit(ACT_st_ActionKit *_pst_ActionKit)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j, iNumberOfAction;
	ACT_st_Action		*pst_Action;
	ACT_st_ActionItem	*pst_ActionItem;
	BIG_tdst_GroupElem	st_Elem;
	BIG_INDEX			ul_Index;
	char				az_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iNumberOfAction = _pst_ActionKit->uw_NumberOfAction;

	/* Search the index of the file into the FAT */
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_ActionKit);
	if(ul_Index == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;

	/* Compute the file name and the path name and open the file */
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), az_Path);
	SAV_Begin(az_Path, BIG_NameFile(ul_Index));


	/* Save the content of the file. The structure is the structure of a group file */
	for(i = 0; i < iNumberOfAction; i++)
	{
		pst_Action = _pst_ActionKit->apst_Action[i];

		if(pst_Action)
		{
			st_Elem.ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Action);
			L_memcpy(&st_Elem.ul_Type, EDI_Csz_ExtAction, sizeof(st_Elem.ul_Type));
		}
		else
		{
			st_Elem.ul_Key = st_Elem.ul_Type = 0;
		}

		SAV_Buffer(&st_Elem, sizeof(st_Elem));
	}

	if(_pst_ActionKit->ul_Flag & 1) /*  Force Kit in ARAM Tag */
	{
		st_Elem.ul_Key = 1;
		st_Elem.ul_Type = 0;

		SAV_Buffer(&st_Elem, sizeof(st_Elem));
	}


	/* Close the file */
	ul_Index = SAV_ul_End();

	/* Save all the referenced actions */
	ACT_gb_ForceARAM = (_pst_ActionKit->ul_Flag & 1); /*  Force Kit in ARAM Tag */
	for(i = 0; i < iNumberOfAction; i++)
	{
		pst_Action = _pst_ActionKit->apst_Action[i];
		if(pst_Action) ACT_ul_SaveAction(pst_Action);
	}

	if(ACT_gb_ForceSaveListTracks)
	{
		for(i = 0; i < iNumberOfAction; i++)
		{
			pst_Action = _pst_ActionKit->apst_Action[i];
			if(pst_Action)
			{
				for(j = 0; j < pst_Action->uc_NumberOfActionItem; j++)
				{
					pst_ActionItem = &pst_Action->ast_ActionItem[j];
					if(pst_ActionItem->pst_TrackList)
					{
						EVE_SaveListTracks(NULL, pst_ActionItem->pst_TrackList);
					}
				}
			}
		}

		ACT_gb_ForceSaveListTracks = FALSE;
	}

	ACT_gb_ForceARAM = FALSE;

	return /* Ul_Index */ 0;
}

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
