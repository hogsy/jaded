/*$T VAVview_string.cpp GC!1.20 03/31/99 16:22:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_String_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    return (char *) _p_Value;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_String_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CEdit   *po_Edit;
    CString o_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Want to fill ? */
    po_Edit = (CEdit *) _po_Wnd;
    if(_b_Fill)
    {
		po_Edit->SetWindowText(EVAV_String_DrawItem(_po_Data, _p_Value));
        return;
    }

    po_Edit->GetWindowText(o_String);
    L_strcpy((char *) _p_Value, (char *) (LPCSTR) o_String);
}

#endif /* ACTIVE_EDITORS */
