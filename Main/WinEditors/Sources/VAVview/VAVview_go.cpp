/*$T VAVview_go.cpp GC! 1.081 04/28/00 15:55:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/AIengine.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "EDImsg.h"
#include "EDIeditors_infos.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"

char	GO_asz_Msg[512];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_GO_DrawItem(EVAV_cl_ViewItem *_po_Item, void *_p_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = *(OBJ_tdst_GameObject **) _p_Value;
	if((int) pst_GO == -1) return "INVALID REFERENCE";
	if(!pst_GO) return "None";
	if(!OBJ_IsValidGAO(pst_GO)) return "INVALID GAO";
	return pst_GO->sz_Name;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_GO_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox			*po_Combo;
	WOR_tdst_World		*pst_World;
	BIG_KEY				ul_SelKey;
	int					iSel, iIndex;
	OBJ_tdst_GameObject *pst_GO;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	TAB_tdst_PFelem		*pst_CurrentElemW, *pst_EndElemW;
	char				asz_Sel[BIG_C_MaxLenName];
	char				az[128], az1[128], *pz;
	ULONG				key;
#ifdef JADEFUSION
	CList<CString, CString>	mo_LstPrefab_Str;
	CList<BIG_KEY, BIG_KEY>	mo_LstPrefab_Key;
#else
	CList<BIG_KEY, BIG_KEY>	mo_LstPrefab;
#endif
	EOUT_cl_Frame		*po_Out;
	BOOL				b_Filter=FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Combo = (CComboBox *) _po_Wnd;
	if(GetAsyncKeyState(VK_SHIFT) < 0) b_Filter=TRUE;
	

	/* Fill the combo box with the list of all objects of world */
	if(_b_Fill)
	{
		/* Search the current object */
		ul_SelKey = *(ULONG *) _p_Value;
		if(!ul_SelKey) ul_SelKey = BIG_C_InvalidIndex;

		po_Combo->ResetContent();

		/* None objects */
		iIndex = po_Combo->InsertString(0, "None");
		L_strcpy(asz_Sel, "None");
		po_Combo->SetItemData(iIndex, BIG_C_InvalidIndex);
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		if(b_Filter  && !po_Out) b_Filter = FALSE;
		if(b_Filter && !po_Out->mpo_EngineFrame) b_Filter = FALSE;
		if(b_Filter && !po_Out->mpo_EngineFrame->mpo_DisplayView) b_Filter = FALSE;
		if(b_Filter && !po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog) b_Filter = FALSE;

		/* Scan all objects */
		pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
		pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
		for(; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
		{
			pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
			if(TAB_b_IsAHole(pst_World)) continue;

			pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
			pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
			while(pst_PFElem <= pst_PFLastElem)
			{
				pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
				pst_PFElem++;
				if(TAB_b_IsAHole(pst_GO)) continue;
				if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) continue;

				if(po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog)
				{
					/* Prefab */
					if(!po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->mb_ShowPrefabSon && pst_GO->ul_PrefabKey && pst_GO->ul_PrefabKey != BIG_C_InvalidKey)
					{
#ifdef JADEFUSION
						CString strPFB(pst_GO->sz_Name);
						int index = strPFB.Find('@');
						if(index)
						{
							if(mo_LstPrefab_Str.Find(strPFB.Left(index)))
								continue;
							mo_LstPrefab_Str.AddTail(strPFB.Left(index));
						}
						else
#endif
						{
						sscanf(pst_GO->sz_Name, "[%x]", &key);
						if(key)
						{
#ifdef JADEFUSION
							if(mo_LstPrefab_Key.Find(key)) continue;
							mo_LstPrefab_Key.AddTail(key);
#else
							if(mo_LstPrefab.Find(key)) continue;
							mo_LstPrefab.AddTail(key);
#endif
						}
						}
					}
				}
				L_strcpy(az, pst_GO->sz_Name);
				if(po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog)
				{
					if(pst_GO->ul_PrefabKey && pst_GO->ul_PrefabKey != BIG_C_InvalidKey && !po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->mb_ShowPrefabSon )
					{
						pz = L_strrchr(az, ']');
						if(pz)
						{
							L_strcpy(az1, "PFB_");
							L_strcat(az1, pz + 2);
							L_strcpy(az, az1);
						}
#ifdef JADEFUSION
						else	//add test for new renaming of prefab
						{
							pz = L_strrchr(az, '@');
							if(pz)
							{
								*pz = 0;
								L_strcpy(az1, "PFB_");
								L_strcat(az1, az);
								L_strcpy(az, az1);
							}
						}
#endif
					}
				}
				
				if(b_Filter && po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->GaoNameIsSkipped(CString(az))) continue;

				iIndex = po_Combo->AddString(az);
				if((ULONG) pst_GO == ul_SelKey) L_strcpy(asz_Sel, pst_GO->sz_Name);
				po_Combo->SetItemData(iIndex, (ULONG) pst_GO);
			}
		}

		/* Select current function */
		po_Combo->SetTopIndex(0);
		po_Combo->SetCurSel(po_Combo->FindStringExact(-1, asz_Sel));
		return;
	}

	/* Search the selected object, and change it */
	iSel = po_Combo->GetCurSel();
	if(iSel == -1) return;
	ul_SelKey = po_Combo->GetItemData(iSel);
	if(ul_SelKey == BIG_C_InvalidIndex)
	{
		*(ULONG *) _p_Value = NULL;
	}
	else
	{
		*(ULONG *) _p_Value = ul_SelKey;
	}
}

#endif /* ACTIVE_EDITORS */
