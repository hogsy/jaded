/*$T BIGgroup.c GC 1.138 03/16/04 10:21:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BIGkey.h"
#include "BIGgroup.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "BIGmdfy_file.h"
#include "BIGmerge.h"
#include "BIGfat.h"
#include "BIGread.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int BIG_i_IsRefInGroup(BIG_INDEX _ul_Group, BIG_KEY _ul_Ref)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_Size, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), &ul_Size);

	/* Test if key is already here */
	for(i = 0; i < (ul_Size / sizeof(BIG_tdst_GroupElem)); i++)
		if(pst_Buf[i].ul_Key == _ul_Ref) return i;

	return -1;
}

#ifdef ACTIVE_EDITORS

ULONG BIG_ul_GetRefInGroupAtRank(BIG_INDEX _ul_Group, int _i_Rank)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	int					i_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), (ULONG*)&i_Size);
	i_Size /= sizeof(BIG_tdst_GroupElem);


	if(_i_Rank >= i_Size) return -1;
	if(_i_Rank < 0) return -1;

	return pst_Buf[_i_Rank].ul_Key;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_INDEX BIG_ul_GetFatGrpRef(BIG_INDEX _ul_Group, int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_Size, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), &ul_Size);

	/* Test if key is already here */
	for(i = 0; i < ul_Size / sizeof(BIG_tdst_GroupElem); i++)
	{
		if((int) i == _i_Num)
		{
			if(pst_Buf[i].ul_Key == 0) break;
			if(pst_Buf[i].ul_Key == BIG_C_InvalidKey) break;
			return BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		}
	}

	return BIG_C_InvalidIndex;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int BIG_i_GetNextGrpType(BIG_tdst_GroupElem *_pst_Group, int _i_Size, int _i_Beg, char *_psz_Type)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = _i_Beg + 1; i < _i_Size; i++)
	{
		if(_pst_Group[i].ul_Type == *((ULONG *) _psz_Type)) return i;
	}

	return -1;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_AddRefInGroup(BIG_INDEX _ul_Group, char *_psz_Name, BIG_KEY _ul_Ref)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf, *pst_Hole;
	ULONG				ul_Size, i;
	char				asz_Path[BIG_C_MaxLenPath];
	char				*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(BIG_b_IsGrpFile(_ul_Group));

    // check consistency
    if(BIG_b_IsFileExtension(_ul_Group,EDI_Csz_ExtTextLang))
    {
        if(!BIG_b_IsFileExtension(_ul_Ref,EDI_Csz_ExtTextFile)) return;
    }

	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), &ul_Size);

	/* Test if key is already here */
	pst_Hole = NULL;
	for(i = 0; i < (ul_Size / sizeof(BIG_tdst_GroupElem)); i++)
	{
		/* if(pst_Buf[i].ul_Key == _ul_Ref) return; */
		if(pst_Buf[i].ul_Key == 0) pst_Hole = &pst_Buf[i];
	}

	/* Else add the key at the end of the file */
	if(!pst_Hole)//POPOWARNING
	{
		BIG_gul_Length = ul_Size + sizeof(BIG_tdst_GroupElem);
#ifdef JADEFUSION
		BIG_gp_Buffer = BIG_p_RequestBuffer(BIG_gul_Length);
		pst_Buf = (BIG_tdst_GroupElem*)BIG_gp_Buffer;
#else
		pst_Buf = BIG_gp_Buffer = BIG_p_RequestBuffer(BIG_gul_Length);
#endif
		pst_Hole = &pst_Buf[i];
	}
	else
	{
		BIG_gul_Length = ul_Size;
		BIG_gp_Buffer = pst_Buf;
	}

	/* Add the ref (first LONG) */
	pst_Hole->ul_Key = _ul_Ref;

	/* Add the type (second LONG */
	psz_Temp = L_strrchr(_psz_Name, '.');
	if(psz_Temp)
		pst_Hole->ul_Type = *((LONG *) psz_Temp);
	else
		pst_Hole->ul_Type = 0;

	L_time(&BIG_gx_GlobalTime);
	BIG_ComputeFullName(BIG_ParentFile(_ul_Group), asz_Path);
	BIG_ul_UpdateCreateFile(asz_Path, BIG_NameFile(_ul_Group), NULL, TRUE);

	BIG_FileChanged(_ul_Group) = (BIG_FileChanged(_ul_Group) & EDI_FHC_Loaded) | EDI_FHC_AddUpdate;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_AddRefInGroupAtRank(BIG_INDEX _ul_Group, char *_psz_Name, BIG_KEY _ul_Ref, int _i_Rank)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf, *pst_Hole;
	ULONG				ul_Size, i;
	char				asz_Path[BIG_C_MaxLenPath];
	char				*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(BIG_b_IsGrpFile(_ul_Group));
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), &ul_Size);

	if(_i_Rank < (int)(ul_Size / sizeof(BIG_tdst_GroupElem)))
	{
		pst_Hole = &pst_Buf[_i_Rank];
		BIG_gul_Length = ul_Size;
		BIG_gp_Buffer = pst_Buf;
	}
	else
	{
		BIG_gul_Length = (_i_Rank+1) * sizeof(BIG_tdst_GroupElem);
#ifdef JADEFUSION
		BIG_gp_Buffer = BIG_p_RequestBuffer(BIG_gul_Length);
		pst_Buf = (BIG_tdst_GroupElem*)BIG_gp_Buffer;
#else
		pst_Buf = BIG_gp_Buffer = BIG_p_RequestBuffer(BIG_gul_Length);
#endif
		for(i = (ul_Size / sizeof(BIG_tdst_GroupElem)); (int)i < _i_Rank; i++)
		{
			pst_Buf[i].ul_Key = 0;
			pst_Buf[i].ul_Type = 0;
		}

		pst_Hole = &pst_Buf[i];
	}

	/* add key */
	pst_Hole->ul_Key = _ul_Ref;

	/* add file type */
	psz_Temp = L_strrchr(_psz_Name, '.');
	if(psz_Temp)
		pst_Hole->ul_Type = *((LONG *) psz_Temp);
	else
		pst_Hole->ul_Type = 0;

    /* save file */
	L_time(&BIG_gx_GlobalTime);
	BIG_ComputeFullName(BIG_ParentFile(_ul_Group), asz_Path);
	BIG_ul_UpdateCreateFile(asz_Path, BIG_NameFile(_ul_Group), NULL, TRUE);

	BIG_FileChanged(_ul_Group) = (BIG_FileChanged(_ul_Group) & EDI_FHC_Loaded) | EDI_FHC_AddUpdate;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_DelRefFromGroup(BIG_INDEX _ul_Group, BIG_KEY _ul_Ref)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf, *pst_Hole;
	ULONG				ul_Size, i;
	char				asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(BIG_b_IsGrpFile(_ul_Group));
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), &ul_Size);

	/* Test if key is already here */
	pst_Hole = NULL;
	for(i = 0; i < (ul_Size / sizeof(BIG_tdst_GroupElem)); i++)
	{
		if(pst_Buf[i].ul_Key == _ul_Ref)
		{
			pst_Buf[i].ul_Key = 0;	/* Key */
			pst_Buf[i].ul_Type = 0; /* Type (extension) */
			BIG_gul_Length = ul_Size;
			BIG_gp_Buffer = (void *) pst_Buf;
			L_time(&BIG_gx_GlobalTime);
			BIG_ComputeFullName(BIG_ParentFile(_ul_Group), asz_Path);
			BIG_ul_UpdateCreateFile(asz_Path, BIG_NameFile(_ul_Group), NULL, TRUE);
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_b_IsGrpFile(BIG_INDEX _ul_Group)
{
	/*~~~~~~~~~~~~~~*/
	char	*psz_Temp;
	/*~~~~~~~~~~~~~~*/

	psz_Temp = L_strrchr(BIG_NameFile(_ul_Group), '.');
	if(!psz_Temp) return FALSE;

	/* All group extensions... */
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtAIEditorModel, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtAIEngineModel, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtAIEditorDepend, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtGameObjects, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtAnimTbl, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtActionKit, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtObjModels, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtObjGolGroups, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtGrpWorld, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtWorldList, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtTextLang, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSoundBank, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSoundMetaBank, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtWorldText, 4)) return TRUE;
	return FALSE;
}
#endif /* ACTIVE_EDITORS */
