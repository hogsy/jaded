/*$T BROframe_extras.cpp GC 1.138 06/18/03 10:35:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROgrpctrl.h"
#include "EDIpaths.h"
#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/ENGinit.h"
#include "GraphicDK/Sources/GEOmetric/GEOload.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLload.h"
#include "ENGine/Sources/COLlision/COLsave.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"
#ifdef JADEFUSION
#include "DIAlogs/DIAcheckworld_dlg.h"
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EBRO_cl_Frame::b_GetOBJ(OBJ_tdst_GameObject **_pp_Ret, char *_psz_Ext, BIG_INDEX *_pul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Index;
	LINK_tdst_Pointer	*p;
	int					i_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
	if(LIST_ACTIVATED())
	{
		if(mpo_ListCtrl->GetSelectedCount() > 1) return FALSE;
		i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
		if(i_Item == -1) return FALSE;
		ul_Index = mpo_ListCtrl->GetItemData(i_Item);
		if(EBRO_M_EngineMode())
		{
			p = LINK_p_SearchPointer((void *) ul_Index);
			if(!p) return FALSE;
			if(!L_strnicmp(_psz_Ext, EDI_Csz_ExtGameObject, L_strlen(_psz_Ext)))
			{
				if(p->i_Type != LINK_C_ENG_GameObjectOriented) return FALSE;
			}
		}
		else
		{
			if(!BIG_b_IsFileExtension(ul_Index, _psz_Ext)) return FALSE;
			if(_pul_Index) *_pul_Index = ul_Index;
			ul_Index = LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
			if(ul_Index == -1) ul_Index = 0;
		}
	}
	else if(GRP_ACTIVATED())
	{
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_GrpCtrl->GetSelectedCount() > 1) return FALSE;
		i_Item = mpo_GrpCtrl->GetNextItem(-1, LVNI_SELECTED);
		if(i_Item == -1) return FALSE;
		ul_Index = mpo_GrpCtrl->GetItemData(i_Item);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
		if(!BIG_b_IsFileExtension(ul_Index, _psz_Ext)) return FALSE;
		if(_pul_Index) *_pul_Index = ul_Index;
		ul_Index = LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
		if(ul_Index == -1) ul_Index = 0;
	}
	else
		return FALSE;

	if(_pp_Ret) *_pp_Ret = (OBJ_tdst_GameObject *) ul_Index;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::SelectGroup(int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX		ul_Index;
	EOUT_cl_Frame	*po_Out;
	char			asz_Name[BIG_C_MaxLenName];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get group */
	if(!b_GetOBJ(NULL, EDI_Csz_ExtObjGolGroups, &ul_Index))
	{
		if(!b_GetOBJ(NULL, EDI_Csz_ExtObjGroups, &ul_Index)) return;
		L_strcpy(asz_Name, BIG_NameFile(ul_Index));
		*L_strrchr(asz_Name, '.') = 0;
		L_strcat(asz_Name, EDI_Csz_ExtObjGolGroups);
		ul_Index = BIG_ul_SearchFile(BIG_ParentFile(ul_Index), asz_Name);
		if(ul_Index == BIG_C_InvalidIndex) return;
	}

	/* Select all objects */
	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, _i_Num);
	po_Out->mpo_EngineFrame->mpo_DisplayView->SelectGroup(ul_Index);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::Zoom3DView(int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EOUT_cl_Frame		*po_Out;
	OBJ_tdst_GameObject *p_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Zoom on object after computing bounding volume */
	if(!b_GetOBJ(&p_Obj, EDI_Csz_ExtGameObject)) return;
	if(!p_Obj) return;
	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, _i_Num);
	po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject(p_Obj);
	po_Out->mpo_EngineFrame->mpo_DisplayView->ZoomExtendSelected
		(
			&po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->st_Camera,
			po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World
		);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::ForceLoadObject(int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EOUT_cl_Frame		*po_Out;
	OBJ_tdst_GameObject *p_Obj;
	BIG_INDEX			ul_Index;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!b_GetOBJ(&p_Obj, EDI_Csz_ExtGameObject, &ul_Index)) return;
	if(p_Obj) return;

	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, _i_Num);
	pst_World = (WOR_tdst_World *) LOA_ul_SearchAddress(BIG_PosFile(po_Out->mul_CurrentWorld));
	ERR_X_Assert((int) pst_World != -1);

	LOA_MakeFileRef(BIG_FileKey(ul_Index), (ULONG *) &p_Obj, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	LOA_Resolve();

	WOR_World_JustAfterLoadObject(pst_World, p_Obj, TRUE, TRUE);

	if(WOR_gb_HasAddedMaterial)
	{
		GDI_l_AttachWorld(po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData, pst_World);
		WOR_gb_HasAddedMaterial = FALSE;
	}
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void EBRO_cl_Frame::FindCheckWorld(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EOUT_cl_Frame	*po_Out;
	int				i_Item;
	BIG_KEY			ul_Index;
	CString			strName;
	/*~~~~~~~~~~~~~~~~~~~~~*/
	
	if(mpo_TreeCtrl->GetSelectedItem() == NULL) return;
	if(LIST_ACTIVATED())
	{
		i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
		if(i_Item == -1) return;
		ul_Index = mpo_ListCtrl->GetItemData(i_Item);
		ul_Index = LOA_ul_SearchAddress(BIG_PosFile(ul_Index));

		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		if(!po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_CheckWorldDialog)
		{
			if(ul_Index == BIG_C_InvalidIndex)
				return;

			strName = BIG_NameFile(LOA_ul_SearchIndexWithAddress(ul_Index));
			if(strName.Find(EDI_Csz_ExtGraphicObject) != -1 || strName.Find(EDI_Csz_ExtGameObject) != -1)
			{
				po_Out->OnAction(EOUT_ACTION_WORLD_CHECK);
				po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_CheckWorldDialog->FillTree_Graphics();	
			}
			else if(strName.Find(EDI_Csz_ExtGraphicMaterial) != -1)
			{
				po_Out->OnAction(EOUT_ACTION_WORLD_CHECK);
				po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_CheckWorldDialog->FillTree_Materiaux();
			}
		}
		po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_CheckWorldDialog->b_Change_Selection(ul_Index);
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::CreateCob(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob		st_Cob;
	OBJ_tdst_GameObject *pst_GO;
	GRO_tdst_Struct		*pst_GroStruct;
	BIG_INDEX			ul_GroFile, ul_CobFile;
	char				asz_PathToCob[BIG_C_MaxLenPath];
	char				asz_CobName[BIG_C_MaxLenPath];
	char				*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!b_GetOBJ(&pst_GO, EDI_Csz_ExtGraphicObject, &ul_GroFile)) return;

	/* Load the Gro. */
	LOA_MakeFileRef(BIG_FileKey(ul_GroFile), (ULONG *) &pst_GroStruct, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
	LOA_Resolve();

	COL_ColMap_AddGeometric(&st_Cob, (GEO_tdst_Object *) pst_GroStruct, (MATH_tdst_Vector *) pst_GO);

	BIG_ComputeFullName(BIG_ParentDir(BIG_ParentFile(ul_GroFile)), asz_PathToCob);
	L_strcat(asz_PathToCob, "/");
	L_strcat(asz_PathToCob, EDI_Csz_Path_COLObjects);
	L_strcpy(asz_CobName, BIG_NameFile(ul_GroFile));
	psz_Temp = L_strrchr(asz_CobName, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_CobName, EDI_Csz_ExtCOLObject);

	/* Create dir if it doesnt exist */
	BIG_ul_CreateDir(asz_PathToCob);
	ul_CobFile = BIG_ul_SearchFileExt(asz_PathToCob, asz_CobName);

	/* If the file already exists,overwrite it. */
	if(ul_CobFile == BIG_C_InvalidIndex) ul_CobFile = BIG_ul_CreateFile(asz_PathToCob, asz_CobName);
	COL_SaveCob(&st_Cob, BIG_FileKey(ul_CobFile));

	M_MF()->FatHasChanged();
}

#ifdef JADEFUSION
extern void COL_SortGameMaterial(COL_tdst_GameMatList *);
extern void COL_SaveGameMaterial(COL_tdst_GameMatList *, BIG_KEY);
#else
extern "C" void COL_SortGameMaterial(COL_tdst_GameMatList *);
extern "C" void COL_SaveGameMaterial(COL_tdst_GameMatList *, BIG_KEY);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::GMAT_DisplayInfo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_GameMatList	*pst_GMatList;
	BIG_INDEX				ul_GMatListIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!b_GetOBJ(NULL, EDI_Csz_ExtCOLGMAT, &ul_GMatListIndex)) return;

	/* Load the Gro. */
	LOA_MakeFileRef
	(
		BIG_FileKey(ul_GMatListIndex),
		(ULONG *) &pst_GMatList,
		COL_ul_CallBackLoadGameMaterial,
		LOA_C_MustExists
	);
	LOA_Resolve();

	COL_SortGameMaterial(pst_GMatList);
	COL_GMat_DisplayInfo(pst_GMatList);
	COL_SaveGameMaterial(pst_GMatList, BIG_C_InvalidIndex);

	if(pst_GMatList && (!pst_GMatList->ul_NbOfInstances))
	{
		MEM_Free(pst_GMatList->pal_Id);
		MEM_Free(pst_GMatList->pst_GMat);
		LOA_DeleteAddress(pst_GMatList);
	}

	M_MF()->FatHasChanged();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::UnloadedReset(BIG_INDEX _ul_Root)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BIG_ComputeFullName(_ul_Root, asz_Path);
	ul_Index = BIG_FirstFile(_ul_Root);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_FileChanged(ul_Index) &= ~EDI_FHC_Loaded;
		ul_Index = BIG_NextFile(ul_Index);
	}

	ul_Index = BIG_SubDir(_ul_Root);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		UnloadedReset(ul_Index);
		ul_Index = BIG_NextDir(ul_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::ClearUnloaded(void)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	/*~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	UnloadedReset(mpo_TreeCtrl->GetItemData(h_Item));
	RefreshAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EBRO_cl_Frame::ul_CreateAssociatedSMD(void)
{
	/*~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	/*~~~~~~~~~~~~~~~~*/

	ul_File = BIG_C_InvalidIndex;
	if(b_GetOBJ(NULL, EDI_Csz_ExtSoundMusic, &ul_File))
		M_MF()->SendMessageToEditors(ESON_MESSAGE_CREATEASSSMD, ul_File, 0);
	else if(b_GetOBJ(NULL, EDI_Csz_ExtSoundAmbience, &ul_File))
		M_MF()->SendMessageToEditors(ESON_MESSAGE_CREATEASSSMD, ul_File, 0);
	else if(b_GetOBJ(NULL, EDI_Csz_ExtSoundDialog, &ul_File))
		M_MF()->SendMessageToEditors(ESON_MESSAGE_CREATEASSSMD, ul_File, 0);
	else if(b_GetOBJ(NULL, EDI_Csz_ExtSoundFile, &ul_File))
		M_MF()->SendMessageToEditors(ESON_MESSAGE_CREATEASSSMD, ul_File, 0);
	else if(b_GetOBJ(NULL, EDI_Csz_ExtLoadingSound, &ul_File))
		M_MF()->SendMessageToEditors(ESON_MESSAGE_CREATEASSSMD, ul_File, 0);
	return ul_File;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EBRO_cl_Frame::ul_CreateAssociatedMTX(void)
{
	/*~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	/*~~~~~~~~~~~~~~~~*/

	ul_File = BIG_C_InvalidIndex;
	if(b_GetOBJ(NULL, EDI_Csz_ExtVideo1, &ul_File))
		M_MF()->SendMessageToEditors(ESON_MESSAGE_GENERATE_MTX, ul_File, 0);
	return ul_File;
}


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
