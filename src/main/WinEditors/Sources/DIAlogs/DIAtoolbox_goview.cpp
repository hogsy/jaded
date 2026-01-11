/*$T SONview.cpp GC!1.71 02/28/00 10:48:37 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAtoolbox_goview.h"
#include "Res/Res.h"

IMPLEMENT_DYNCREATE(EDIA_cl_ToolBox_GOView, CFormView)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_GOView::EDIA_cl_ToolBox_GOView(EDIA_cl_ToolBoxDialog *_po_ToolBox) : CFormView(DIALOGS_IDD_TOOLBOX_GO)
{
    mpo_ToolBox= _po_ToolBox;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_GOView::EDIA_cl_ToolBox_GOView(void) : CFormView(DIALOGS_IDD_TOOLBOX_GO)
{
    EDIA_cl_ToolBox_GOView(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_GOView::~EDIA_cl_ToolBox_GOView(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ToolBox_GOView::Create
(
    LPCTSTR     lpszClassName,
    LPCTSTR     lpszWindowName,
    DWORD       dwStyle,
    const RECT  &rect,
    CWnd        *pParentWnd
)
{
    CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, 0, NULL);
    return TRUE;
}


/*$4
 ***********************************************************************************************************************
    Message map
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_ToolBox_GOView, CFormView)
END_MESSAGE_MAP()



#endif /* ACTIVE_EDITORS */
