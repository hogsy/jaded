/*$T VAVview_vector.cpp GC!1.34 05/11/99 14:52:09 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAvector_dlg.h"
#include "VAVview/VAVview.h"
#include "MATHs/MATH.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Vector_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Temp[256];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sprintf
    (
        asz_Temp,
        "(x=%.6f, y=%.6f, z=%.6f)",
        ((MATH_tdst_Vector *) _p_Value)->x,
        ((MATH_tdst_Vector *) _p_Value)->y,
        ((MATH_tdst_Vector *) _p_Value)->z
    );
    return asz_Temp;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Vector_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CEdit                   *po_Edit;
    CString                 o_String;
    EDIA_cl_VectorDialog    o_Dlg;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Click on button ? */
    po_Edit = (CEdit *) _po_Wnd;
    MATH_CopyVector(&o_Dlg.mst_Vector, (MATH_tdst_Vector *) _p_Value);
    o_Dlg.DoModal();
    MATH_CopyVector((MATH_tdst_Vector *) _p_Value, &o_Dlg.mst_Vector);
}

#endif /* ACTIVE_EDITORS */
