/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "AIundo.h"
#include "AIview.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
EAI_cl_UndoString::EAI_cl_UndoString
(
    EAI_cl_View *_po_Edit,
    LONG        _l_Sel,
    CString     &_o_Str,
    BOOL        _b_Locked
) :
    EDI_cl_UndoModif(_b_Locked)
{
    mpo_Edit = _po_Edit;
    ml_Sel = _l_Sel;
    mo_String = _o_Str;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EAI_cl_UndoString::b_Undo(void)
{
    mpo_Edit->HideSelection(TRUE, FALSE);
    mpo_Edit->SetSel(ml_Sel, ml_Sel + mo_String.GetLength());
    mpo_Edit->ReplaceSel("");
    mpo_Edit->HideSelection(FALSE, FALSE);
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EAI_cl_UndoString::b_Do(void)
{
    mpo_Edit->HideSelection(TRUE, FALSE);
    mpo_Edit->SetSel(ml_Sel, ml_Sel);
    mpo_Edit->ReplaceSel(mo_String);
    mpo_Edit->HideSelection(FALSE, FALSE);
    return TRUE;
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
EAI_cl_UndoDelete::EAI_cl_UndoDelete
(
    EAI_cl_View *_po_Edit,
    CHARRANGE   &cr,
    CString     &_o_Str,
    BOOL        _b_Locked
) :
    EDI_cl_UndoModif(_b_Locked)
{
    mpo_Edit = _po_Edit;
    m_cr.cpMin = cr.cpMin;
    m_cr.cpMax = cr.cpMax;
    mo_String = _o_Str;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EAI_cl_UndoDelete::b_Undo(void)
{
    mpo_Edit->HideSelection(TRUE, FALSE);
    mpo_Edit->SetSel(m_cr.cpMin, m_cr.cpMin);
    mpo_Edit->ReplaceSel(mo_String);
    mpo_Edit->HideSelection(FALSE, FALSE);
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EAI_cl_UndoDelete::b_Do(void)
{
    mpo_Edit->HideSelection(TRUE, FALSE);
    mpo_Edit->SetSel(m_cr);
    mpo_Edit->ReplaceSel("");
    mpo_Edit->HideSelection(FALSE, FALSE);
    return TRUE;
}

#endif /* ACTIVE_EDITORS */
