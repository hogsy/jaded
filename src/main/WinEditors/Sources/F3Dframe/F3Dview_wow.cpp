/*$T F3Dview_wow.cpp GC! 1.100 03/30/06 11:34:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbrickmapper_dlg.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "F3Dframe/F3Dview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "F3Dframe/F3Dstrings.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "LINKs/LINKtoed.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "EDItors/Sources/OUTput/OUTframe.h"

#include "DIAlogs/DIAchecklist_dlg.h"



EDIA_cl_SelectionDialog *selDialog;


/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void sWowDisplayRefresh(void *, void *_p_Sel, void *_p_Data, long)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem		*p_Sel;
	BIG_tdst_GroupElem		*pst_Grp;
	BOOL					*p_Data;
	BIG_INDEX				wowIndex, h_File;
	OBJ_tdst_GameObject		*pst_Gao;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Sel = (EVAV_cl_ViewItem *) _p_Sel;
	p_Data = (BOOL *) _p_Data;
	pst_Grp = (BIG_tdst_GroupElem *) p_Sel->mi_Param1;

	wowIndex = BIG_ul_SearchKeyToFat(pst_Grp[p_Sel->mi_Param2].ul_Key);
	h_File = BIG_FirstFile(BIG_ul_SearchDirInDir(BIG_ParentFile(wowIndex), "Game Objects"));

	while (h_File != BIG_C_InvalidIndex)
	{

		int addr = LOA_ul_SearchAddress(BIG_PosFile(h_File));
		if (addr != BIG_C_InvalidIndex)
		{
			pst_Gao = (OBJ_tdst_GameObject *) addr;
			if(*p_Data)
			{
				pst_Gao->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
				pst_Gao->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_HiddenByWowFilter;
			}
			else
			{
				pst_Gao->ul_EditorFlags |= OBJ_C_EditFlags_Hidden;
				pst_Gao->ul_AdditionalFlags |= OBJ_C_EdAddFlags_HiddenByWowFilter;
			}
		}

		h_File = BIG_NextFile(h_File);
	}

	selDialog->UpdateList();

	LINK_Refresh();

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DisplayWow(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_CheckListDialog *po_Dialog;
	BOOL					*pb_Displ;
	ULONG					ul_Pos, ul_Size;
	BIG_INDEX				ul_WorldIndex, h_File;
	char					*pc_Buf;
	BIG_tdst_GroupElem		*pst_Grp;
	char					wowName[100];
	OBJ_tdst_GameObject		*pst_Gao;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_CheckListDialog(F3D_STR_Csz_WowDisplayTitle, CPoint(0, 0));
	ul_WorldIndex = BIG_ul_SearchKeyToFat(mst_WinHandles.pst_World->h_WorldKey);
	
	// Reading .WOL file
	ul_Pos = BIG_ul_SearchKeyToPos(mst_WinHandles.pst_World->h_WorldKey);
	pc_Buf = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);
	pst_Grp = new BIG_tdst_GroupElem[ul_Size >> 3];
	pb_Displ = new BOOL[ul_Size >> 3];

	for(int i = 0 ; i < ul_Size >> 3 ; ++i)
	{
		pst_Grp[i].ul_Key = LOA_ReadULong(&pc_Buf);
		pst_Grp[i].ul_Type = LOA_ReadULong(&pc_Buf);

		/* get the first loaded GAO of WOW to get its visibility flag */
		h_File = BIG_FirstFile(BIG_ul_SearchDirInDir(BIG_ParentFile(BIG_ul_SearchKeyToFat(pst_Grp[i].ul_Key)), "Game Objects"));
		int addr = LOA_ul_SearchAddress(BIG_PosFile(h_File));
		while (addr == BIG_C_InvalidIndex && h_File != BIG_C_InvalidIndex)
		{
			h_File = BIG_NextFile(h_File);
			addr = LOA_ul_SearchAddress(BIG_PosFile(h_File));
		}
		if (addr == BIG_C_InvalidIndex)
			pb_Displ[i] = TRUE;
		else
		{
			pst_Gao = (OBJ_tdst_GameObject *) addr;
			pb_Displ[i] = pst_Gao->ul_EditorFlags & OBJ_C_EditFlags_Hidden ? FALSE : TRUE;
		}

		/* Add one bool item */
		sprintf(wowName, "%s", BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Grp[i].ul_Key)));

		selDialog = this->mpo_SelectionDialog;
		selDialog->mb_ShowHidden = FALSE;
		po_Dialog->AddItem(wowName, EVAV_EVVIT_Bool, &pb_Displ[i], EVAV_None, (int) pst_Grp, (int) i);
	}

	po_Dialog->mpo_VarsView->SetChangeCallback(sWowDisplayRefresh);
	po_Dialog->DoModal();

	delete pb_Displ;
	delete pst_Grp;
	LINK_Refresh();

}

#endif
