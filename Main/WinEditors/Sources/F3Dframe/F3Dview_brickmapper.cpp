#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbrickmapper_dlg.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "F3Dframe/F3Dview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/OBJects/OBJinit.h"



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::BrickMapper(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_BrickMapper	*o_BrickMap = new EDIA_cl_BrickMapper(this);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_BrickMap->DoModeless();

	mpo_BrickMapper = o_BrickMap;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *F3D_cl_View::GetFirstSel()
{
	return SEL_pst_GetFirst(M_F3D_Sel, SEL_C_SIF_Object);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *F3D_cl_View::GetNextSel(SEL_tdst_SelectedItem *p_Sel)
{
	return SEL_pst_GetNext(M_F3D_Sel, p_Sel, SEL_C_SIF_All);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Replace(void *_p_Old, void *_p_New)
{
	SEL_ReplaceItemContent(M_F3D_Sel, _p_Old, _p_New);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_Prefab *F3D_cl_View::BrickLoad(BIG_INDEX h_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, l_NbObj, l_Active;
	static int				i_RefCount;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_Prefab			*P;
	ULONG					gul_PrefabKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Load prefab
	P = Prefab_pst_Load(h_File, NULL);
	if(!P || !P->l_NbRef) return NULL;

	l_NbObj = 0;
	l_Active = (P->l_Type == OBJPREFAB_C_AllRef) ? 1 : 0;
	for(i = 0; i < P->l_NbRef; i++) P->dst_Ref[i].c_IsActive = l_Active;

	switch(P->l_Type)
	{
	case OBJPREFAB_C_Cycle:
	case OBJPREFAB_C_Random:
	case OBJPREFAB_C_RandomUser:
		P->dst_Ref[P->c_CurrentRef].c_IsActive = 1;
		break;
	}

	/* get prefix key */
	gul_PrefabKey = BIG_ul_GetNewKey(BIG_C_InvalidIndex);

	/* Scan all group */
	for(i = 0; i < P->l_NbRef; i++)
	{
		if(!P->dst_Ref[i].c_IsActive) continue;

		if(P->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsGao)
		{
			// Load GAOs the prefab is composed of
			WOR_gpst_WorldToLoadIn = mst_WinHandles.pst_World;
			LOA_MakeFileRef(P->dst_Ref[i].ul_Key, (ULONG *) &pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
			LOA_Resolve();

            pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
			pst_GO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_HiddenByWowFilter; 
			pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
			pst_GO->ul_PrefabKey = BIG_FileKey(h_File);
			pst_GO->ul_PrefabObjKey = BIG_FileKey(P->dst_Ref[i].ul_Index);

			WOR_World_JustAfterLoadObject(WOR_gpst_WorldToLoadIn, pst_GO, FALSE, TRUE);
		}
	}

	return P;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::BrickUpdate(OBJ_tdst_GameObject *p_GO)
{
	OBJ_UpdateGaoGao(mst_WinHandles.pst_World, p_GO, p_GO->ul_PrefabObjKey, 0xfff41b7f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::BrickDetach(OBJ_tdst_GameObject *p_GO)
{
	WOR_World_DetachObject(mst_WinHandles.pst_World, p_GO);
	OBJ_GameObject_Remove(p_GO, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::BrickMapperClose()
{
	mpo_BrickMapper = NULL;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::BrickMapperSave(OBJ_tdst_GameObject *pst_GO)
{
	char				asz_Path[BIG_C_MaxLenPath];
	ULONG				ul_GOKey;

	ul_GOKey = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	if(ul_GOKey != BIG_C_InvalidKey)
	{
		ul_GOKey = BIG_ul_SearchKeyToFat(ul_GOKey);
		if(ul_GOKey == BIG_C_InvalidIndex)
		{
			ERR_X_Warning(0, "Game object is in world but file has been destroyed !!! Zapping...", pst_GO->sz_Name);
		}
		BIG_ComputeFullName(BIG_ParentFile(ul_GOKey), asz_Path);
	}

	//WOR_GetPath(mst_WinHandles.pst_World, asz_Path);
	OBJ_ul_GameObject_Save(mst_WinHandles.pst_World, pst_GO, asz_Path);

	//WOR_l_World_Save(mst_WinHandles.pst_World);
}


#endif