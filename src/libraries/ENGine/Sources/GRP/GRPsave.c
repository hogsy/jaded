/*$T GRPsave.c GC 1.139 04/07/04 13:04:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Save world */
#include "BASe/BAStypes.h"
#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGfat.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "BASe/MEMory/MEM.h"

/*
 =======================================================================================================================
    Aim: Save a group
 =======================================================================================================================
 */
void GRP_SaveGroup(OBJ_tdst_Group *_pst_Group)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	char		asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OBJ_OrderGroupByHie(_pst_Group);

	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Group);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

	SAV_Begin(asz_Path, BIG_NameFile(ul_Index));

	/* Key to pf table */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Group->pst_AllObjects);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	SAV_Buffer(&ul_Index, sizeof(BIG_INDEX));

	/* The group flags */
	SAV_Buffer(&(_pst_Group->ul_Flags), sizeof(LONG));

	/* The rest... */
	ul_Index = 0;
	SAV_Buffer(&ul_Index, sizeof(LONG));
	SAV_Buffer(&ul_Index, sizeof(LONG));

	SAV_ul_End();

	/* Save gol file */
	OBJ_SaveGolGroup(_pst_Group);
}

/*
 =======================================================================================================================
    Aim: Duplicates a loaded group into a new (already created) group Note: Duplicates all objects into the destination
    world and add the new group to the destination world
 =======================================================================================================================
 */
void GRP_DuplicateGroup(WOR_tdst_World *_pst_World, OBJ_tdst_Group *_pst_NewGroup, OBJ_tdst_Group *_pst_Group)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_NewGO;
	BIG_INDEX			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_NewGroup->ul_Flags = _pst_Group->ul_Flags;

	/* Duplicate each object of the group */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Group->pst_AllObjects);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_GO))
		{
			ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
			ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
			ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
			ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

			/* Duplicates the object into the new world */
			pst_NewGO = OBJ_pst_DuplicateGameObjectFile(_pst_World, ul_Index, 0, NULL, NULL);
			OBJ_AddInGroup(_pst_NewGroup, pst_NewGO);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_AfterLoaded(WOR_tdst_World *_pst_World, OBJ_tdst_Group *_pst_Group)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Duplicate each object of the group */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Group->pst_AllObjects);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_GO)) WOR_World_JustAfterLoadObject(_pst_World, pst_GO, TRUE, TRUE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_DetachAndDestroyGAOs(WOR_tdst_World *_pst_World, OBJ_tdst_Group *pst_Group, BOOL _b_Destroy )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Destroy all gameobjets not registered in world */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		WOR_World_DetachObject(_pst_World, pst_GO);
		if (_b_Destroy) 
			OBJ_GameObject_Remove(pst_GO, 1);
	}
}

/*
 =======================================================================================================================
    Aim: Duplicates a group into world _pst_World Note: If not already loaded, the group is loaded
 =======================================================================================================================
 */
OBJ_tdst_Group *GRP_pst_DuplicateGroupFile(WOR_tdst_World *_pst_World, BIG_INDEX _ul_FatFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group		*pst_Group, *pst_NewGroup;
	BOOL				b_MustDel;
	BIG_KEY				ul_Key;
	char				az_Path[BIG_C_MaxLenPath];
	BOOL				del;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load the group if it is not present in memory */
	b_MustDel = FALSE;
	pst_Group = (OBJ_tdst_Group *) LOA_ul_SearchAddress(BIG_PosFile(_ul_FatFile));

	if((BIG_INDEX) pst_Group == BIG_C_InvalidIndex)
	{
		/* Load the group */
		WOR_gpst_WorldToLoadIn = _pst_World;
		ul_Key = BIG_FileKey(_ul_FatFile);
		LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Group, GRP_ul_LoadGrpCallback, LOA_C_MustExists);
		LOA_Resolve();
		b_MustDel = TRUE;
		GRP_AfterLoaded(_pst_World, pst_Group);
	}

	/* Creates the New group and add it to the groups of the world */
	WOR_GetSubPath(_pst_World, EDI_Csz_Path_Groups, az_Path);
	pst_NewGroup = GRP_pst_CreateAndSaveNewGroup(az_Path, pst_Group->sz_Name, TRUE, FALSE, &del);

	/* Duplicates the loaded group into the new group */
	GRP_DuplicateGroup(_pst_World, pst_NewGroup, pst_Group);
	GRP_RepercuteHierarchy(pst_Group, pst_NewGroup);
	GRP_RepercuteAIRefs(pst_Group, pst_NewGroup);
	GRP_RepercuteEventsRefs(pst_Group, pst_NewGroup);
	GRP_ReinitAllGO(pst_NewGroup);

	if(b_MustDel)
	{
		/* Destroy all gameobjets not registered in world */
		GRP_DetachAndDestroyGAOs(_pst_World, pst_Group, TRUE);

		/* Free the old group from memory if loaded by this function */
		OBJ_FreeGroup(pst_Group);
	}

	GRP_SaveGroup(pst_NewGroup);

	return pst_NewGroup;
}
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
