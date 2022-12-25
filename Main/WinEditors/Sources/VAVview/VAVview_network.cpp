/*$T VAVview_network.cpp GC!1.41 09/09/99 11:51:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORvars.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Network_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul = *(ULONG *) _p_Value;
    if(!ul) return "None";
    ul = LOA_ul_SearchKeyWithAddress(ul);
	if((int) ul == -1) return "UNKNOWN REFERENCE";
    ul = BIG_ul_SearchKeyToFat(ul);
    return BIG_NameFile(ul);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Network_FillSelect
(
    CWnd                *_po_Wnd,
    EVAV_cl_ViewItem    *_po_Data,
    void                *_p_Value,
    BOOL                _b_Fill
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox       *po_Combo;
    WOR_tdst_World  *pst_World;
    BIG_KEY         ul_SelKey, ul_Key;
	BIG_INDEX		ul_Fat;
    int             iSel, iIndex;
    ULONG           i;
	char			asz_Sel[BIG_C_MaxLenName];
	TAB_tdst_PFelem	*pst_CurrentElemW, *pst_EndElemW;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Combo = (CComboBox *) _po_Wnd;

    /* Fill the combo box with the list of all networks */
    if(_b_Fill)
    {
        /* Search the current network */
        ul_SelKey = *(ULONG *) _p_Value;
        if(!ul_SelKey)
            ul_SelKey = BIG_C_InvalidIndex;
        else
            ul_SelKey = LOA_ul_SearchKeyWithAddress(ul_SelKey);

        po_Combo->ResetContent();

        /* None network */
        iIndex = po_Combo->InsertString(0, "None");
		L_strcpy(asz_Sel, "None");
        po_Combo->SetItemData(iIndex, BIG_C_InvalidIndex);
        iSel = 1;

        /* Scan all networks */
		pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
		pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
		for(; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
		{
			pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
			if(TAB_b_IsAHole(pst_World)) continue;
			if(pst_World->pst_AllNetworks)
			{
				for(i = 0; i < pst_World->pst_AllNetworks->ul_Num; i++)
				{
					ul_Key =
						LOA_ul_SearchKeyWithAddress((ULONG) pst_World->pst_AllNetworks->ppst_AllNetworks[i]);
					if(ul_Key == BIG_C_InvalidIndex) continue;
					ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
					if(ul_Fat == BIG_C_InvalidIndex) continue;
					iIndex = po_Combo->AddString(BIG_NameFile(ul_Fat));
					if(ul_Key == ul_SelKey) L_strcpy(asz_Sel, BIG_NameFile(ul_Fat));
					po_Combo->SetItemData
						(
							iIndex,
							(ULONG) pst_World->pst_AllNetworks->ppst_AllNetworks[i]
						);
				}
			}
		}

        /* Select current function */
        po_Combo->SetTopIndex(0);
        po_Combo->SetCurSel(po_Combo->FindStringExact(-1, asz_Sel));
        return;
    }

    /* Search the selected network, and change it */
    iSel = po_Combo->GetCurSel();
    if(iSel == -1) return;
    ul_SelKey = po_Combo->GetItemData(iSel);
    if(ul_SelKey == BIG_C_InvalidIndex)
        *(ULONG *) _p_Value = NULL;
    else
    {
        *(ULONG *) _p_Value = ul_SelKey;
    }
}

#endif /* ACTIVE_EDITORS */
