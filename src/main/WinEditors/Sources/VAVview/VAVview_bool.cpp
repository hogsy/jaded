/*$T VAVview_bool.cpp GC!1.20 03/31/99 16:21:03 */

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
CString EVAV_Bool_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    if(*((BOOL *) _p_Value))
        return "true";
    else
        return "false";
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Bool_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CButton		*po_Button;
    int         i_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Button = (CButton *) _po_Wnd;
    if(_b_Fill)
    {
        if(*((BOOL *) _p_Value))
			po_Button->SetCheck(1);
		else
			po_Button->SetCheck(0);

        return;
    }

    /* Want to retreive the result of a selection */
    i_Res = po_Button->GetCheck();
    *((BOOL *) _p_Value) = i_Res ? TRUE : FALSE;
}

#endif /* ACTIVE_EDITORS */
