/*$T VAVlist_anim.cpp GC! 1.081 10/11/00 15:09:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"

/*
 =======================================================================================================================
    Aim:    Macro to add a new item in the list
 =======================================================================================================================
 */
#define A(a, b, c, d, e, f, g, h, i, p4, p5, p6, p7, fil, help) \
	{ \
		if(c) \
		{ \
			pos = mpo_ListItems->InsertAfter \
				( \
					pos, \
					po_NewItem = new EVAV_cl_ViewItem(a, b, c, d, e, f, g, h, i, p4, p5, p6, p7, fil) \
				); \
			i_NumFields++; \
			po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data; \
			po_NewItem->psz_Help = help; \
		} \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::ANIAddGameObjectAnim(POSITION pos, void *_p_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem		*po_Item, *po_NewItem;
	EVAV_tdpfnv_Change		pfn_CB;
	int						i_NumFields;
	ANI_st_GameObjectAnim	*pst_GameObjectAnim;
	int						iAnimUsed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	po_Item = pos ? mpo_ListItems->GetAt(pos) : mpo_ListItems->GetTail();

	/* Init all variables */
	pfn_CB = LINK_gast_StructTypes[LINK_C_COL_Instance].pfn_CB;
	i_NumFields = 0;

	pst_GameObjectAnim = (ANI_st_GameObjectAnim *) _p_Instance;
	iAnimUsed = pst_GameObjectAnim->uc_AnimUsed;

/*$off*/
	A("Blend",				EVAV_EVVIT_Flags, &pst_GameObjectAnim->uc_AnimUsed, EVAV_ReadOnly, 1, 1, 0, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Current action",		EVAV_EVVIT_Int, &pst_GameObjectAnim->uw_CurrentActionIndex, EVAV_ReadOnly, 0, 0xFFFF, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Current action item",EVAV_EVVIT_Int, &pst_GameObjectAnim->uc_CurrentActionItemIndex, EVAV_ReadOnly, 0, 0xFF, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Next action",		EVAV_EVVIT_Int, &pst_GameObjectAnim->uw_NextActionIndex, EVAV_ReadOnly, 0, 0xFFFF, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Current Repetition", EVAV_EVVIT_Int, &pst_GameObjectAnim->uc_CounterForRepetition, EVAV_ReadOnly, 0, 0xFF, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Tracks Synchro",		EVAV_EVVIT_Flags, &pst_GameObjectAnim->uc_Synchro, EVAV_ReadOnly, 0, 1, 0, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("LOD IA",				EVAV_EVVIT_Int, &pst_GameObjectAnim->c_LOD_IA, 0, 0, 0xFF, -1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("LOD BitField",		EVAV_EVVIT_Int, &pst_GameObjectAnim->c_LOD_Bitfield, 0, 0, 0xFF, -1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("LOD Dynamic",		EVAV_EVVIT_SubStruct, &pst_GameObjectAnim->c_LOD_Dynamic, 0, LINK_C_Animation_LOD, 0xFF, -1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);

	if(iAnimUsed == 0)
	{	
		A("No Anim", EVAV_EVVIT_String, "", EVAV_ReadOnly, 0, 0, 0, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	}
	else
	{
			if(pst_GameObjectAnim->uc_AnimUsed != 1)
			A("Blend Anim", EVAV_EVVIT_Pointer, pst_GameObjectAnim->apst_Anim[1], 0, LINK_C_ENG_BlendAnim, 0, 0, pfn_CB, 0x008EB48B, 0, 0, 0, 0, 0, 0);
			A("Lead Anim", EVAV_EVVIT_Pointer, pst_GameObjectAnim->apst_Anim[0], 0, LINK_C_ENG_Anim, 0, 0, pfn_CB, 0x008EB48B, 0, 0, 0, 0, 0, 0);
	}

/*$on*/

	/* Set number of fields of the initial pointer */
	po_Item->mi_NumFields = i_NumFields;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::ANIAddAnim(POSITION pos, void *_p_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	EVAV_tdpfnv_Change	pfn_CB;
	int					i_NumFields;
	ANI_tdst_Anim		*pst_Anim;
	ULONG				ul_Index;
	char				*pz_AnimName;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	po_Item = pos ? mpo_ListItems->GetAt(pos) : mpo_ListItems->GetTail();

	/* Init all variables */
	pfn_CB = LINK_gast_StructTypes[LINK_C_COL_Instance].pfn_CB;
	i_NumFields = 0;

	pst_Anim = (ANI_tdst_Anim *) _p_Instance;

	/* Search the name of the associated TrackList */
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_Data->pst_ListTracks);
	pz_AnimName = ul_Index != BIG_C_InvalidIndex ? BIG_NameFile(ul_Index) : "Unknown";

/*$off*/
	A("Name",					EVAV_EVVIT_String,	pz_AnimName, EVAV_ReadOnly, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Frequency",				EVAV_EVVIT_Int,		&pst_Anim->uc_AnimFrequency, 0, 0, 255, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Current Frame",			EVAV_EVVIT_Int,		&pst_Anim->uw_CurrentFrame, EVAV_ReadOnly, 0, 0xFF, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Total Frames",			EVAV_EVVIT_Int,		&pst_Anim->uw_TotalFrame, EVAV_ReadOnly, 0, 0xFF, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Mode",					EVAV_EVVIT_SubStruct, &pst_Anim->uc_Mode, 0, LINK_C_AnimFlags, 0, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);

/*$on*/

	/* Set number of fields of the initial pointer */
	po_Item->mi_NumFields = i_NumFields;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::ANIAddBlendAnim(POSITION pos, void *_p_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	EVAV_tdpfnv_Change	pfn_CB;
	int					i_NumFields;
	ANI_tdst_Anim		*pst_Anim;
	ULONG				ul_Index;
	char				*pz_AnimName;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	po_Item = pos ? mpo_ListItems->GetAt(pos) : mpo_ListItems->GetTail();

	/* Init all variables */
	pfn_CB = LINK_gast_StructTypes[LINK_C_COL_Instance].pfn_CB;
	i_NumFields = 0;

	pst_Anim = (ANI_tdst_Anim *) _p_Instance;

	/* Search the name of the associated TrackList */
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_Data->pst_ListTracks);
	pz_AnimName = ul_Index != BIG_C_InvalidIndex ? BIG_NameFile(ul_Index) : "Unknown";

/*$off*/
	A("Name",					EVAV_EVVIT_String,	pz_AnimName, EVAV_ReadOnly, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Frequency",				EVAV_EVVIT_Int,		&pst_Anim->uc_AnimFrequency, 0, 0, 255, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Current Frame",			EVAV_EVVIT_Int,		&pst_Anim->uw_CurrentFrame, EVAV_ReadOnly, 0, 0xFF, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Total Frames",			EVAV_EVVIT_Int,		&pst_Anim->uw_TotalFrame, EVAV_ReadOnly, 0, 0xFF, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Blend Action",			EVAV_EVVIT_Int,		&pst_Anim->uw_BlendedAction, EVAV_ReadOnly, 0, 0, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Blend Action Item",		EVAV_EVVIT_Int,		&pst_Anim->uc_BlendedActionItem, EVAV_ReadOnly, 0, 0, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Blend Frame",			EVAV_EVVIT_Int,		&pst_Anim->uw_CurrentBlendFrame, EVAV_ReadOnly, 0, 0, 2, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Blend Length",			EVAV_EVVIT_Int,		&pst_Anim->uc_BlendTime, EVAV_ReadOnly, 0, 0, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Mode",					EVAV_EVVIT_SubStruct, &pst_Anim->uc_Mode, 0, LINK_C_AnimFlags, 0, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);
	A("Blend Flags",			EVAV_EVVIT_SubStruct, &pst_Anim->uc_BlendFlags, 0, LINK_C_BlendFlags, 0, 1, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, 0, 0);

/*$on*/

	/* Set number of fields of the initial pointer */
	po_Item->mi_NumFields = i_NumFields;
}

