/*$T EDIbaseframe.cpp GC!1.52 10/14/99 11:09:16 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDIaction.h"
#include "EDItors/Sources/MENu/MENframe.h"
#include "EDItors/Sources/MENu/MENin.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "LINKs/LINKmsg.h"
#include "Dialogs/BtnST.h"

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EDI_cl_BaseFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(EDI_cl_BaseFrame, CFrameWnd)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_BaseFrame::EDI_cl_BaseFrame(void)
{
    mpo_DialogBar = NULL;
    mpo_MenuFrame = NULL;
    mpo_Menu = NULL;
	mb_Persist = FALSE;

    mpo_Actions = new EDI_cl_ActionList;
    mpo_Actions->mpo_Editor = this;

    L_memset(mab_PresentEdit, FALSE, sizeof(mab_PresentEdit));

    mi_FullScreenResolution = -1;

    ReinitIni();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_BaseFrame::~EDI_cl_BaseFrame(void)
{
    if(mpo_DialogBar)
    {
        mpo_DialogBar->DestroyWindow();
        delete mpo_DialogBar;
    }

    /* Delete menu frame in left frame list */
    mpo_MenuFrame = NULL;

    /* Delete top menu if present */
    if(mpo_Menu) delete mpo_Menu;

    /* Delete action list */
    delete mpo_Actions;

    /* Be careful of pipe editor */
    if(M_MF()->mpo_PipeFrame == this)
        M_MF()->mpo_PipeFrame = NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_BaseFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    CFrameWnd::PreCreateWindow(cs);
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EDI_cl_BaseFrame::OnCreate(LPCREATESTRUCT lpcs)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Name[255];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(asz_Name, "Creating editor %s", mst_Def.asz_Name);
	if(EDI_gpo_EnterWnd)
		EDI_gpo_EnterWnd->DisplayMessage(asz_Name);
	else
	{
		LINK_gb_CanLog = FALSE;
		LINK_PrintStatusMsg(asz_Name);
		LINK_gb_CanLog = TRUE;
	}

    CFrameWnd::OnCreate(lpcs);

    /* Create dialog bar */
    if(mst_Def.ui_ResDialogBar)
    {
        mpo_DialogBar = new CDialogBar;
        if(mpo_DialogBar == NULL) return 1;

        mpo_DialogBar->Create
            (
                this,
                MAKEINTRESOURCE(mst_Def.ui_ResDialogBar),
                WS_VISIBLE | CBRS_TOP,
                mst_Def.ui_ResDialogBar
            );

        mpo_DialogBar->SetBarStyle(mpo_DialogBar->GetBarStyle() &~CBRS_BORDER_ANY);
    }

	EDIA_cl_BaseDialog::SetTheme(this);
    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::OnSize(UINT nType, int cx, int cy)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    CFrameWnd::OnSize(nType, cx, cy);

    /* Move dialog bar */
    if(mpo_DialogBar && mst_BaseIni.b_DialogBarVisible)
    {
        mpo_DialogBar->GetWindowRect(&o_Rect);
        mpo_DialogBar->MoveWindow(0, 0, cx, o_Rect.bottom - o_Rect.top);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_BaseFrame::OnEraseBkgnd(CDC *pDC)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DWORD   x_Col;
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    x_Col = GetSysColor(COLOR_BTNFACE);

    GetWindowRect(&o_Rect);
    ScreenToClient(&o_Rect);
    pDC->FillSolidRect(o_Rect, x_Col);

    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EDI_cl_BaseFrame::i_GetPosDup(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < EDI_C_MaxDuplicate; i++)
    {
        if(mab_PresentEdit[i] == FALSE)
        {
            mst_BaseIni.i_NumCopy++;
            return i;
        }
    }

    return -1;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::RefreshMenu(void)
{
    if(mpo_MenuFrame && IsWindow(mpo_MenuFrame->m_hWnd)&& mpo_MenuFrame->mpo_Parent )
        mpo_MenuFrame->FillList();
    if(mpo_Menu)
	{
		mpo_Menu->Invalidate();
		if(mpo_Menu->mpo_InMenu) mpo_Menu->Invalidate();
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::OpenProject(void)
{
    ReinitIni();
    LoadIni();

    /* Read default action list */
    mpo_Actions->DeleteAll();
    mpo_Actions->ParseBuffer(psz_OnActionGetBase());

    /* Read action list. Always read maineditor (and not copy) file */
    if(!(GetAsyncKeyState(VK_SHIFT) & 0x8000))
    {
        if(mpo_Original)
            mpo_Actions->ReadFile(mpo_Original->mst_Def.asz_Name);
        else
            mpo_Actions->ReadFile(mst_Def.asz_Name);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::OnActivate(void)
{
    mb_IsActivate = TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::OnDisactivate(void)
{
    mb_IsActivate = FALSE;

    /* If editor or one of its child has the focus, force loose it */
    if(EDI_go_TheApp.GetFocusedEditor() == this) M_MF()->SetFocus();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::GetClientInsideRect(CRect &_o_Rect)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect, o_Rect1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetClientRect(_o_Rect);
    if(mst_BaseIni.b_DialogBarVisible && mpo_DialogBar)
    {
        mpo_DialogBar->GetClientRect(&o_Rect1);
        _o_Rect.top += o_Rect1.bottom;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::InitPopupMenuAction(EMEN_cl_SubMenu *_po_Menu)
{
    M_MF()->InitPopupMenuAction(this, _po_Menu);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_BaseFrame::AddPopupMenuAction(EMEN_cl_SubMenu *_po_Menu, ULONG _ul_Action)
{
    return M_MF()->AddPopupMenuAction(this, _po_Menu, _ul_Action, FALSE, NULL, -1);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::TrackPopupMenuAction(CPoint pt, EMEN_cl_SubMenu *_po_Menu)
{
    M_MF()->TrackPopupMenuAction(this, pt, _po_Menu);
}

#endif /* ACTIVE_EDITORS */
