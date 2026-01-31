/*$T VAVview_aifunc.cpp GC 1.139 03/10/04 15:40:32 */


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
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "BAse/CLIbrary/CLImem.h"
#include "AIinterp/Sources/AIengine.h"
#include "DIAlogs/DIAlist2_dlg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString GaoName(OBJ_tdst_GameObject *pst_GO)
{
	CString	str;
	char	az[512];

	if((int) pst_GO == -1) return "";
	if(!pst_GO) return "";
#	if !defined( NDEBUG )
	if(!OBJ_IsValidGAO(pst_GO)) return "";
#	endif
	L_strcpy(az, pst_GO->sz_Name);
	if(L_strrchr(az, '.')) *L_strrchr(az, '.') = 0;
	str = az + CString("  -  ");
	return str;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_Trigger_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
	/*~~~~~~~~~~~~~~~~*/
	SCR_tt_Trigger	*pt;
	char			*pz;
	CString			str;
	CString			str0;
	CString			str1;
	CString			str2;
	CString			str3;
	/*~~~~~~~~~~~~~~~~*/

	pt = (SCR_tt_Trigger *) _p_Value;
	if(!pt->az_Name[0]) return "<EMPTY>";
	if(!pt->pt_Proc) return "<INVALID>";
	pz = pt->az_Name;
	pz++; /* @ */
	while(L_isdigit(*pz)) pz++;

	str = pz;
	str0.Format("%d %d  ", pt->t_Msg.i_Int1, pt->t_Msg.i_Int2);
	str1 = GaoName(pt->t_Msg.pst_GAO1);
	str2 = GaoName(pt->t_Msg.pst_GAO2);
	str3 = GaoName(pt->t_Msg.pst_GAO3);
	if(!str1.IsEmpty() || !str2.IsEmpty() || !str3.IsEmpty()) 
	{
		str += "    ( ";
		str += str0;
		str += str1;
		str += str2;
		str += str3;
		str += ")";
	}

	return (char *) (LPCSTR) str;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_Trigger_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_List2Dialog o_Dlg(LIST2_ModeTrigger);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Dlg.pt_Trigger = (SCR_tt_Trigger *) _p_Value;
	o_Dlg.DoModal();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_AIFunc_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
	/*~~~~~~~*/
	ULONG	ul;
	/*~~~~~~~*/

	ul = *(ULONG *) _p_Value;
	if(!ul) return "None";
	ul = LOA_ul_SearchKeyWithAddress(ul);
	ul = BIG_ul_SearchKeyToFat(ul);
	return BIG_NameFile(ul);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_AIFunc_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox			*po_Combo;
	BIG_INDEX			ul_Model, ul_File;
	BIG_KEY				ul_SelKey;
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_SizeModel, i;
	char				*psz_Temp;
	char				asz_Name[BIG_C_MaxLenName];
	char				asz_Sel[BIG_C_MaxLenName];
	int					iSel, iIndex;
	AI_tdst_Instance	*pst_Instance;
	AI_tdst_Function	*pst_Function;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Combo = (CComboBox *) _po_Wnd;
	pst_Instance = (AI_tdst_Instance *) _po_Data->mi_Param1;

	/* Fill the combo box with the list of all functions */
	if(_b_Fill)
	{
		/* Search the current function file */
		ul_SelKey = *(ULONG *) _p_Value;
		if(!ul_SelKey)
			ul_SelKey = BIG_C_InvalidIndex;
		else
			ul_SelKey = LOA_ul_SearchKeyWithAddress(ul_SelKey);

		/* Search the model file */
		po_Combo->ResetContent();
		ul_Model = (ULONG) pst_Instance->pst_Model;
		ul_Model = LOA_ul_SearchKeyWithAddress(ul_Model);
		ERR_X_Assert(ul_Model != BIG_C_InvalidIndex);
		ul_Model = BIG_ul_SearchKeyToFat(ul_Model);
		ERR_X_Assert(ul_Model != BIG_C_InvalidIndex);

		/* None function */
		iIndex = po_Combo->InsertString(0, "None");
		L_strcpy(asz_Sel, "None");
		po_Combo->SetItemData(iIndex, BIG_C_InvalidIndex);

		/* Add all functions of model */
		pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_Model), &ul_SizeModel);
		for(i = 0; i < (ul_SizeModel / sizeof(BIG_tdst_GroupElem)); i++)
		{
			if(!pst_Buf[i].ul_Key) continue;
			ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
			if(ul_File == BIG_C_InvalidIndex) continue;
			L_strcpy(asz_Name, BIG_NameFile(ul_File));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(!psz_Temp) continue;

			/* Is it a function file ? */
			if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineFct))
			{
				iIndex = po_Combo->AddString(asz_Name);
				if(pst_Buf[i].ul_Key == ul_SelKey) L_strcpy(asz_Sel, asz_Name);
				po_Combo->SetItemData(iIndex, pst_Buf[i].ul_Key);
			}
		}

		/* Select current function */
		po_Combo->SetTopIndex(0);
		po_Combo->SetCurSel(po_Combo->FindStringExact(-1, asz_Sel));
		L_free(pst_Buf);
		return;
	}

	/* Search the selected function, and change it */
	iSel = po_Combo->GetCurSel();
	if(iSel == -1) return;
	ul_SelKey = po_Combo->GetItemData(iSel);
	if(ul_SelKey == BIG_C_InvalidIndex)
		* (ULONG *) _p_Value = NULL;
	else
	{
		ul_SelKey = BIG_ul_SearchKeyToFat(ul_SelKey);
		ul_SelKey = LOA_ul_SearchAddress(BIG_PosFile(ul_SelKey));
		*(ULONG *) _p_Value = ul_SelKey;

		/* Reset the corresponding jump node */
		if(_po_Data->mi_Param2 != -1)
		{
			pst_Function = (AI_tdst_Function *) ul_SelKey;
			pst_Instance->apst_JumpNode[_po_Data->mi_Param2] = pst_Function->pst_RootNode;
		}
	}
}
#endif /* ACTIVE_EDITORS */
