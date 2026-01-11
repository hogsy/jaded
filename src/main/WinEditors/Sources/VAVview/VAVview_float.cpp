/*$T VAVview_float.cpp GC!1.39 07/05/99 14:53:41 */

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
CString EVAV_Float_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Temp[100];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sprintf(asz_Temp, "%.6f", *((float *) _p_Value));
    return asz_Temp;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Float_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CEdit   *po_Edit;
    CString o_String;
    float   f_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Want to fill ? */
    po_Edit = (CEdit *) _po_Wnd;
    if(_b_Fill)
    {
        po_Edit->SetWindowText(EVAV_Float_DrawItem(_po_Data, _p_Value));
        return;
    }

    po_Edit->GetWindowText(o_String);
    f_Res = (float) L_atof((char *) (LPCSTR) o_String);

    /* Born value */
    if((*(float *) &_po_Data->mi_Param1 != 0.0f) || (*(float *) &_po_Data->mi_Param2 != 0.0f))
    {
        if(f_Res < *(float *) &_po_Data->mi_Param1)
            f_Res = *(float *) &_po_Data->mi_Param1;
        if(f_Res > *(float *) &_po_Data->mi_Param2)
            f_Res = *(float *) &_po_Data->mi_Param2;
    }

    /* Save it */
    *((float *) _p_Value) = f_Res;
}

#endif /* ACTIVE_EDITORS */
