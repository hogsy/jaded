/*$T VAVview_flags.cpp GC!1.34 05/11/99 12:43:48 */

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
static BOOL sfnb_IsTrue(void *_p_Value, int i1, int i2, BOOL set = FALSE, BOOL val = TRUE)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UINT    ui_Res;
    UINT    ui_Flag;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ui_Flag = 1;

    switch(i2)
    {
    case 1:
        ui_Res = *((char *) _p_Value);
        break;

    case 2:
        ui_Res = *((short *) _p_Value);
        break;

    case 4:
        ui_Res = *((long *) _p_Value);
        break;
    }

    ui_Flag <<= i1;

    /* Set flag */
    if(set)
    {
        if(val)
            ui_Res |= ui_Flag;
        else
            ui_Res &= ~ui_Flag;

        switch(i2)
        {
        case 1:
            *((char *) _p_Value) = (char) ui_Res;
            break;

        case 2:
            *((short *) _p_Value) = (short) ui_Res;
            break;

        case 4:
            *((long *) _p_Value) = (long) ui_Res;
            break;
        }

        return val;
    }

    /* Get flag */
    else
    {
        if(ui_Res & ui_Flag)
        {
            return TRUE;
        }

        return FALSE;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Flags_DrawItem(EVAV_cl_ViewItem *_po_Item, void *_p_Value)
{
    if(sfnb_IsTrue(_p_Value, _po_Item->mi_Param1, _po_Item->mi_Param2))
        return "true";
    else
        return "false";
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Flags_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Item, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CButton *po_Button;
    BOOL    b_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Want to fill the combo box ? */
    po_Button = (CButton *) _po_Wnd;
    if(_b_Fill)
    {
        if(sfnb_IsTrue(_p_Value, _po_Item->mi_Param1, _po_Item->mi_Param2))
            po_Button->SetCheck(1);
        else
            po_Button->SetCheck(0);

        return;
    }

    /* Want to retreive the result of a selection */
    b_Res = (BOOL) po_Button->GetCheck();
    sfnb_IsTrue(_p_Value, _po_Item->mi_Param1, _po_Item->mi_Param2, TRUE, b_Res);
}

#endif /* ACTIVE_EDITORS */
