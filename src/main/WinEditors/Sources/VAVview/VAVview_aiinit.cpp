/*$T VAVview_aiinit.cpp GC!1.71 03/01/00 16:57:12 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"
#include "BAse/CLIbrary/CLImem.h"
#include "BAse/CLIbrary/CLIstr.h"
#include "AIinterp/Sources/AIstruct.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_AIInit_DrawItem(EVAV_cl_ViewItem *_po_Item, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul;
    char    asz_Name[200];
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    ul = *(USHORT *) _p_Value;
	ul &= AI_CVF_InitType;
	ul >>= 8;
	sprintf(asz_Name, "Init Level %d", ul);
	return asz_Name;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_AIInit_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox   *po_Combo;
    int         iSel, iAct;
	int			i;
    char    asz_Name[200];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Combo = (CComboBox *) _po_Wnd;
    iAct = ((*(USHORT *) _p_Value) & AI_CVF_InitType) >> 8;

    /* Fill the combo box with the list of all constants */
    if(_b_Fill)
    {
        po_Combo->ResetContent();
		for(i = 0; i <= 15; i++)
		{
			sprintf(asz_Name, "Init Level %d", i);
			iSel = po_Combo->AddString(asz_Name);
			po_Combo->SetItemData(iSel, i);
			if(iAct == iSel) po_Combo->SetCurSel(iSel);
		}
    }

    /* Search the selected network, and change it */
    iSel = po_Combo->GetCurSel();
    if(iSel == -1) return;
    iSel = po_Combo->GetItemData(iSel);
    *(USHORT *) _p_Value &= ~AI_CVF_InitType;
    iSel <<= 8;
    *(USHORT *) _p_Value |= iSel;
}

#endif /* ACTIVE_EDITORS */
