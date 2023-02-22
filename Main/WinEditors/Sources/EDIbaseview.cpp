/*$T EDIbaseview.cpp GC!1.71 02/18/00 10:55:28 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "EDImainframe.h"
#include "EDImainframe_act.h"
#include "EDIbaseframe.h"
#include "EDIbaseview.h"
#include "EDIerrid.h"
#include "EDIstrings.h"
#include "Res/Res.h"
#include "EDIapp.h"
#include "EDImsg.h"
#include "EDIpaths.h"
#include "EDIeditors_infos.h"
#include "EDItors/Sources/MENu/MENframe.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDItors/Sources/RASters/RASframe.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGfat.h"
#include "AIinterp/Sources/AIdebug.h"
#include "ENGine/Sources/ENGmsg.h"

extern ERAS_cl_Frame *gpo_RasterEditor;
/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EDI_cl_BaseView, CTabCtrl)
BEGIN_MESSAGE_MAP(EDI_cl_BaseView, CTabCtrl)
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_PAINT()
    ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnChangePane)
    ON_COMMAND_RANGE(WM_USER + 1, WM_USER + 100, OnPopupMenu)
    ON_COMMAND_RANGE(WM_USER + 101, WM_USER + 200, OnLinkTo)
    ON_COMMAND_RANGE(WM_USER + 201, WM_USER + 300, OnInitFrom)
    ON_WM_NCHITTEST()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseView::EDI_cl_BaseView(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseView::EDI_cl_BaseView(int _i_Pane)
{
    mi_PopupMenu = 0;
    mb_Outside = FALSE;
    mi_NumPane = _i_Pane;
    mi_GameOn = 0;
    mb_GameKey = FALSE;
    mi_PaneDragDrop = 0;
    mb_DragDropCopy = FALSE;
    mb_TabFill = TRUE;
    mb_Floating = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseView::~EDI_cl_BaseView(void)
{
}

/*$4
 ***********************************************************************************************************************
    MESSAGES
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_BaseView::PreCreateWindow(CREATESTRUCT &cs)
{
	return CTabCtrl::PreCreateWindow(cs);
}

/*
 =======================================================================================================================
    Aim:    Resize the current validated editor
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnSize(UINT nType, int cx, int cy)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i, iTop;
    BOOL                b_Lock;
    EDI_cl_BaseFrame    *po_Editor;
    CRect               o_Rect, o_RectItem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Lock = FALSE;

    if(!::IsWindow(m_hWnd)) return;

    /* To avoid flashing */
    if
    (
        (M_CurSelScr(mi_NumPane) < M_CurNumEd(mi_NumPane)) &&
        (M_CurSelScr(mi_NumPane) != -1) &&
        (M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor) &&
        (M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor->IsWindowVisible())
    )
    {
        b_Lock = TRUE;
        M_MF()->LockDisplay(M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor);
    }

    /* Search the selected editor to resize it */
    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        po_Editor = M_CurEd(mi_NumPane, i).po_Editor;
        if(po_Editor && po_Editor->mi_NumPane == GetCurSel())
        {
            if(po_Editor->mpo_Menu)
            {
                if(!po_Editor->mpo_Menu->mb_LeftMode)
                {
					GetItemRect(0, o_RectItem);
					iTop = o_RectItem.bottom + 2;
                    po_Editor->mpo_Menu->MoveWindow(2, iTop, cx - 6, 20);
                    po_Editor->MoveWindow(2, iTop + 20, cx - 6, cy - iTop - 20 - 3);
                }
                else
                {
                    po_Editor->mpo_Menu->MoveWindow(2, 25, 20, cy - 28);
                    po_Editor->MoveWindow(22, 25, cx - 26, cy - 28);
                }
            }
            else
            {
                po_Editor->MoveWindow(2, 25, cx - 6, cy - 28);
            }

            /* No need to continue, we have found activated editor */
            break;
        }
    }

    /* Call parent function */
    CTabCtrl::OnSize(0, cx, cy);

    if(b_Lock) M_MF()->UnlockDisplay(M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp)
{
    /*~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~*/

    CTabCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
    if(bCalcValidRects)
    {
        if(mb_TabFill)
        {
            if(M_MF()->mpo_MaxView != this || !M_MF()->mst_Desktop.b_VeryMaximized)
            {
                GetItemRect(0, &o_Rect);
                lpncsp->rgrc[0].top -= o_Rect.bottom + 4;
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnNcPaint(void)
{
    CTabCtrl::OnNcPaint();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnPaint(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect, o_Rect1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

	CPaintDC dc(this); // device context for painting
	dc.SelectObject(GetFont());
	DRAWITEMSTRUCT dis;
	dis.CtlType = ODT_TAB;
	dis.CtlID = GetDlgCtrlID();
	dis.hwndItem = GetSafeHwnd();
	dis.hDC = dc.GetSafeHdc();
	dis.itemAction = ODA_DRAWENTIRE;

	// draw the rest of the border
	CRect rClient, rPage;
	GetClientRect(&dis.rcItem);
	rPage = dis.rcItem;
	AdjustRect(FALSE, rPage);
	dis.rcItem.top = rPage.top - 2;

	// paint the tabs first and then the borders
	int nTab = GetItemCount();
	int nSel = GetCurSel();

	if (!nTab) // no pages added
		return;

	while (nTab--)
	{
		dis.itemID = nTab;
		dis.itemState = 0;
		VERIFY(GetItemRect(nTab, &dis.rcItem));
		dis.rcItem.top -= 1;
		DrawItem(&dis);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_BaseView::OnEraseBkgnd(CDC *pDC)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect, o_Rect1, o_Rect2;
    CWnd    *po_Wnd;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(GetItemCount())
    {
		po_Wnd = M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor;
		if(po_Wnd && IsWindow(po_Wnd->m_hWnd))
        {
            po_Wnd->GetWindowRect(&o_Rect);
            ScreenToClient(&o_Rect);
            pDC->ExcludeClipRect(&o_Rect);
        }
    }

    if(GetItemCount()) CTabCtrl::OnEraseBkgnd(pDC);

    GetWindowRect(&o_Rect);
    ScreenToClient(&o_Rect);
    if(GetItemCount())
    {
        GetItemRect(GetItemCount() - 1, o_Rect1);
        o_Rect1.top = o_Rect.top;
        o_Rect1.left = o_Rect1.right;
        o_Rect1.right = o_Rect.right;
        pDC->FillSolidRect(o_Rect1, GetSysColor(COLOR_3DSHADOW));
    }
    else
    {
        pDC->FillSolidRect(o_Rect, GetSysColor(COLOR_3DSHADOW));
    }

    return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Draw the tab title. We want a small rect with a given color at the left of the name
            (the color depends of editor name), and we want to draw a focus rect if the tab or one
            of its childs as the input focus.
 =======================================================================================================================
 */
void EDI_cl_BaseView::DrawItem(DRAWITEMSTRUCT *lpDrawItemStruct)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CDC     dc, *pDC;
    TC_ITEM st_Item;
    char    asz_Name[100], *p;
    CRect   o_Rect, o_RectTmp;
    CWnd    *po_Wnd;
    ULONG   ul_Of;
	BOOL	focus;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mb_TabFill) return;

    pDC = dc.FromHandle(lpDrawItemStruct->hDC);
    pDC->SetBkMode(TRANSPARENT);

    po_Wnd = GetFocus();
    while((po_Wnd) && (po_Wnd != this)) po_Wnd = po_Wnd->GetParent();

    /* Get name */
    st_Item.mask = TVIF_TEXT;
    st_Item.pszText = asz_Name;
    st_Item.cchTextMax = 100;
    GetItem(lpDrawItemStruct->itemID, &st_Item);

    /* Compute a color depending on editor name */
    ul_Of = 0;
    p = asz_Name;
    while(*p == ' ') p++;
    while(*p && *p != ' ')
    {
        ul_Of = (ul_Of * 5) + L_toupper(*p) - (*p >> 2);
        p++;
    }

	pDC->SetBkMode(TRANSPARENT);
    if(!M_MF()->mst_Desktop.b_ShortMode)
    {
        if(po_Wnd && GetCurFocus() == (int) lpDrawItemStruct->itemID)
        {
            o_RectTmp = lpDrawItemStruct->rcItem;
            if(asz_Name[L_strlen(asz_Name) - 1] == ' ') o_RectTmp.right -= 16;
            pDC->FillSolidRect(o_RectTmp, GetSysColor(COLOR_ACTIVECAPTION));
            pDC->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
        }
        else
        {
            pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_BTNFACE));
            pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
        }
    }

    /* Draw title */
    o_Rect = lpDrawItemStruct->rcItem;
    if(!M_MF()->mst_Desktop.b_ShortMode)
    {
        if(!po_Wnd || GetCurFocus() != (int) lpDrawItemStruct->itemID)
		{
			focus = FALSE;
	        pDC->SetTextColor(0);
		}
		else
		{
			focus = TRUE;
	        pDC->SetTextColor(0x00FFFFFF);
		}
	    pDC->ExtTextOut(o_Rect.left + 5, o_Rect.top + 1, ETO_CLIPPED, &o_Rect, asz_Name, L_strlen(asz_Name), 0);
    }

    /* Draw a color rectangle */
    ul_Of &= 0x00FFFFFF;

    if(!M_MF()->mst_Desktop.b_ShortMode)
    {
        o_Rect.top = lpDrawItemStruct->rcItem.top + 3;
        o_Rect.bottom = o_Rect.top + 10;
        o_Rect.left = lpDrawItemStruct->rcItem.left + 4;
        o_Rect.right = o_Rect.left + 10;

        pDC->FillSolidRect(&o_Rect, (COLORREF) ul_Of);
        o_Rect.InflateRect(1, 1);
        pDC->Draw3dRect
            (
                o_Rect,
                M_MF()->u4_Interpol2PackedColor(ul_Of, 0x00FFFFFF, 0.5f),
                M_MF()->u4_Interpol2PackedColor(ul_Of, 0, 0.5f)
            );
        o_Rect.InflateRect(-1, -1);
        pDC->Draw3dRect
            (
                o_Rect,
                M_MF()->u4_Interpol2PackedColor(ul_Of, 0x00FFFFFF, 0.6f),
                M_MF()->u4_Interpol2PackedColor(ul_Of, 0, 0.3f)
            );
    }
    else
    {
        o_Rect = lpDrawItemStruct->rcItem;
        o_Rect.left -= 2;
        o_Rect.top -= 2;
        o_Rect.right += 2;
        o_Rect.bottom += 4;
        pDC->FillSolidRect(&o_Rect, (COLORREF) ul_Of);
    }



    
    o_Rect = lpDrawItemStruct->rcItem;
	o_Rect.top--;
	if(focus)
	{
		o_Rect.bottom++;
		pDC->Draw3dRect(o_Rect, 0x00FFFFFF, 0x00FFFFFF);
	}
	else if(GetCurSel() == lpDrawItemStruct->itemID)
	{
		pDC->Draw3dRect(o_Rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_BTNFACE));
		o_Rect = lpDrawItemStruct->rcItem;
		o_Rect.left = o_Rect.right - 1;
		pDC->FillSolidRect(o_Rect, GetSysColor(COLOR_3DSHADOW));
	}
	else
		pDC->Draw3dRect(o_Rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
        
    /* Draw close button */
    if(!M_MF()->mst_Desktop.b_ShortMode)
    {
        if(asz_Name[L_strlen(asz_Name) - 1] == ' ')
        {
            o_Rect = lpDrawItemStruct->rcItem;
            o_Rect.right -= 2;
            o_Rect.left = o_Rect.right - 12;
            o_Rect.bottom = o_Rect.top + 12;
            pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
            pDC->ExtTextOut(o_Rect.left + 3, o_Rect.top - 1, 0, o_Rect, "X", 1, NULL);
            pDC->Draw3dRect(o_Rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
        }
    }
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS:: EDITORS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    To create one new editor.

    Out:    The index of the editor in the array.
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_BaseView::po_AddOneEditor(int _i_Type, int _i_Cpt, int _i_Pos)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i_Num;
    int                 i_Pos;
    EDI_cl_BaseFrame    *po_Editor;
    EDI_cl_BaseFrame    *po_Original;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Search a free place in array */
    if(_i_Pos == -1)
    {
        for(i_Num = 0; i_Num < M_CurNumEd(mi_NumPane); i_Num++)
            if(M_CurEd(mi_NumPane, i_Num).po_Editor == NULL)
                break;
    }
    else
        i_Num = _i_Pos;

    ERR_X_Error(i_Num != EDI_C_MaxEditors, EDI_ERR_Csz_TooManyEditors, NULL);

    /* Search pos in registered editor list */
    for(i_Pos = 0; i_Pos < M_MF()->mi_TotalEditors; i_Pos++)
    {
        if(M_MF()->mast_ListOfEditors[i_Pos].i_Type == _i_Type)
            break;
    }

    /* Editor is not registered. Exit */
    if(i_Pos == M_MF()->mi_TotalEditors) 
	{
		M_CurEd(mi_NumPane, i_Num).po_Editor = NULL;
		return NULL;
	}

    /* Create class of the editor */
    M_CurEd(mi_NumPane, i_Num).po_Editor = (EDI_cl_BaseFrame *) (M_MF()->mast_ListOfEditors[i_Pos].po_Class->CreateObject());

    po_Editor = M_CurEd(mi_NumPane, i_Num).po_Editor;
    po_Editor->mi_PosInGArray = i_Pos;
    po_Original = M_MF()->mast_ListOfEditors[i_Pos].po_Instance;
    M_CurEd(mi_NumPane, i_Num).i_Type = _i_Type;
    M_CurEd(mi_NumPane, i_Num).i_Cpt = _i_Cpt;

    /* Init ID and def struct */
    L_memcpy(&po_Editor->mst_Def, &M_MF()->mast_ListOfEditors[i_Pos], sizeof(EDI_tdst_DefEdit));
    if(_i_Cpt)
    {
        sprintf(po_Editor->mst_Def.asz_Name, "%s (%d)", M_MF()->mast_ListOfEditors[i_Pos].asz_Name, _i_Cpt);
    }
    else
    {
        L_strcpy(po_Editor->mst_Def.asz_Name, M_MF()->mast_ListOfEditors[i_Pos].asz_Name);
    }

    /* Init some editor values */
    po_Editor->mb_IsPresent = FALSE;
    po_Editor->mi_NumPane = -1;
    po_Editor->mb_IsActivate = FALSE;
    po_Editor->mpo_MyView = this;
    po_Editor->mpo_Original = po_Original;
    po_Editor->mi_NumEdit = _i_Cpt;
    if(po_Original)
    {
        po_Editor->mst_BaseIni.i_NumCopy = po_Original->mst_BaseIni.i_NumCopy;
        po_Original->mab_PresentEdit[_i_Cpt] = TRUE;
    }
    else
    {
        po_Editor->mab_PresentEdit[0] = TRUE;
        po_Editor->mst_BaseIni.i_NumCopy = 0;
    }

    /* Create frame */
    po_Editor->Create(NULL, NULL, WS_CLIPCHILDREN | WS_CHILD, CRect(0, 0, 100, 100), this, NULL);

    /* One more registered editor */
    if(i_Num == M_CurNumEd(mi_NumPane)) M_CurNumEd(mi_NumPane)++;

    return po_Editor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::CreateMainEditors(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i, j, i_Type, i_Cpt;
    EDI_cl_BaseFrame    *po_Editor;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        i_Type = M_CurEd(mi_NumPane, i).i_Type;
        i_Cpt = M_CurEd(mi_NumPane, i).i_Cpt;
        if((i_Type != -1) && (i_Cpt == 0))
        {
            /* Add editor */
            po_Editor = po_AddOneEditor(i_Type, 0, i);

            /* Remember instance in global list of editors */
            if(po_Editor)
            {
                for(j = 0; j < M_MF()->mi_TotalEditors; j++)
                {
                    if(M_MF()->mast_ListOfEditors[j].i_Type == i_Type)
                        break;
                }

                M_MF()->mast_ListOfEditors[j].po_Instance = po_Editor;
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::CreateDuplicateEditors(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    int i, i_Type, i_Cpt;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        i_Type = M_CurEd(mi_NumPane, i).i_Type;
        i_Cpt = M_CurEd(mi_NumPane, i).i_Cpt;
        if((i_Type != -1) && (i_Cpt))
        {
            /* Add editor */
            po_AddOneEditor(i_Type, i_Cpt, i);
        }
    }
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS:: EDITORS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_BaseView::i_NumEditFromPane(int _i_Pane)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
		if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane == _i_Pane))
        {
            return i;
        }
    }

    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::DisactivateEditor(EDI_cl_BaseFrame *po_Editor)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_Frame   *po_Menu;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!po_Editor) return;

    po_Editor->ShowWindow(SW_HIDE);
    if(po_Editor->mpo_Menu) po_Editor->mpo_Menu->ShowWindow(SW_HIDE);
    po_Editor->OnDisactivate();

    /* Delete left menu */
    if(po_Editor->mpo_MenuFrame)
    {
        po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, po_Editor->mst_BaseIni.i_CountMenu);
        if(!po_Menu)
        {
            po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, 0);
        }

        po_Menu->RemoveMenu(po_Editor->mpo_MenuFrame);
    }
}

/*
 =======================================================================================================================
    Aim:    Call when the user change the current pane in the tabctrl of the view.
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnChangePane(NMHDR *pNotifyStruct, LRESULT *result)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i_Pane;
    EDI_cl_BaseFrame    *po_Editor;
    EDI_cl_BaseFrame    *po_OldEditor;
    EDI_cl_BaseFrame    *po_NewEditor;
    CRect               o_Rect;
    EMEN_cl_Frame       *po_Menu;
    int                 cx, cy;
	int					iTop;
	CRect				o_RectItem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get current selected pane */
    i_Pane = GetCurFocus();
    if(i_Pane == -1) return;
    ENG_gb_GlobalLock = TRUE;

    M_MF()->LockDisplay(this);
	M_MF()->SendMessageToEditors(EDI_MESSAGE_LOCKGMENU, 0, 0);

    /* Retreive the editor that correspond to the pane */
    i_Pane = i_NumEditFromPane(i_Pane);

	if((M_CurSelScr(mi_NumPane) != i_Pane) && (M_CurSelScr(mi_NumPane) != -1))
	{
		po_OldEditor = M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor;
		if(po_OldEditor) DisactivateEditor(po_OldEditor);
	}

    /* Activate new one */
    po_OldEditor = M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor;
    po_Editor = M_CurEd(mi_NumPane, i_Pane).po_Editor;
	po_NewEditor = NULL;
    if(po_Editor && po_Editor->b_CanActivate())
    {
        /* Set left menu */
        po_Editor->mpo_MenuFrame = NULL;
        if(po_Editor->b_HasMenu())
        {
            po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, po_Editor->mst_BaseIni.i_CountMenu);
            if(!po_Menu)
            {
                po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, 0);
            }

            if(po_Menu)
            {
                po_Editor->mpo_MenuFrame = po_Menu->po_AddMenu
                    (
                        po_Editor->mst_Def.asz_Name,
                        po_Editor->mpo_Actions,
                        po_Editor,
                        M_MF()->i_GetNumWithView(this) + 1,
                        TRUE
                    );

                /* Create top menu if b_TopMenu is TRUE */
                if(po_Editor->mst_BaseIni.b_TopMenu && !po_Editor->mpo_Menu)
                {
                    po_Editor->mpo_MenuFrame->CreateTopMenu(po_Editor, FALSE);
                    if(po_Editor->mst_BaseIni.mb_LeftMode) po_Editor->mpo_Menu->SetTop(FALSE);
                }
                else if(po_Editor->mpo_Menu)
                {
                    po_Editor->mpo_Menu->mpo_MenuFrame = po_Editor->mpo_MenuFrame;
                }
            }
        }

        /* Resize and display editor */
        GetWindowRect(o_Rect);

        cx = o_Rect.right - o_Rect.left + 1;
        cy = o_Rect.bottom - o_Rect.top + 1;

        if(po_Editor->mpo_Menu)
        {
            if(!po_Editor->mpo_Menu->mb_LeftMode)
            {
				GetItemRect(0, o_RectItem);
				iTop = o_RectItem.bottom + 4;
                po_Editor->mpo_Menu->MoveWindow(2, iTop, cx - 6, 20);
				po_Editor->MoveWindow(2, iTop + 20, cx - 6, cy - iTop - 20 - 3);
            }
            else
            {
                po_Editor->mpo_Menu->MoveWindow(2, 25, 20, cy - 28);
                po_Editor->MoveWindow(22, 25, cx - 26, cy - 28);
            }

            po_Editor->mpo_Menu->ModifyStyle(0, WS_VISIBLE);
            po_Editor->mpo_Menu->ShowWindow(SW_SHOW);
        }
        else
        {
            po_Editor->MoveWindow(2, 25, cx - 6, cy - 3);
        }

		po_NewEditor = po_Editor;
        po_Editor->ModifyStyle(0, WS_VISIBLE);
        po_Editor->ShowWindow(SW_SHOW);
        po_Editor->OnActivate();
    }

    /* Disactivate old editor */
_Try_
    if((M_CurSelScr(mi_NumPane) != i_Pane) && (M_CurSelScr(mi_NumPane) != -1))
    {
        po_Editor = M_CurEd(mi_NumPane, M_CurSelScr(mi_NumPane)).po_Editor;
        if(po_Editor) DisactivateEditor(po_Editor);
    }

_Catch_
_End_
    /* Save new pane number */
    M_CurSelScr(mi_NumPane) = i_NumEditFromPane(GetCurFocus());

    /* Call virtual method */
    ChangePane();

    /* Force resize frame */
    GetWindowRect(o_Rect);
    GetParent()->ScreenToClient(&o_Rect);
    ENG_gb_GlobalLock = FALSE;
	SendMessage(WM_SIZE, 0, o_Rect.Width() + (o_Rect.Height() << 16));

    M_MF()->UnlockDisplay(this);
	M_MF()->SendMessageToEditors(EDI_MESSAGE_UNLOCKGMENU, 0, 0);
}

/*
 =======================================================================================================================
    Aim:    Call by an editor when he want to be activate.
 =======================================================================================================================
 */
void EDI_cl_BaseView::IWantToBeActive(EDI_cl_BaseFrame *_po_Editor)
{
    if(_po_Editor->b_CanActivate() == FALSE)
        return;
    if(_po_Editor->IsWindowVisible()) return;
    SetCurSel(_po_Editor->mi_NumPane);
    OnChangePane(NULL, NULL);
}

/*$4
 ***********************************************************************************************************************
    INTERFACE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::ActivateDisactivateEditors(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i, j;
    TC_ITEM             st_Item;
    BOOL                b_NeedToChoose;
    BOOL                b_HasChoose;
    int                 i_NumPane;
    char                asz_Name[100];
    CWnd                *po_Wnd;
    EDI_cl_BaseFrame    *po_Editor;
    CRect               o_Rect;
    BOOL                b_Change;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    M_MF()->LockDisplay(this);

    b_NeedToChoose = FALSE;
    b_HasChoose = TRUE;
    b_Change = FALSE;

    /* Determins if current editor can now been disactivated */
    po_Wnd = GetFocus();

    i_NumPane = 0;
    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor == NULL)
            continue;

        /* Determin if editor must be deleted */
        if
        (
            (M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent) &&
            (
                (M_CurEd(mi_NumPane, i).po_Editor->mst_BaseIni.b_IsVisible == FALSE) ||
                (M_CurEd(mi_NumPane, i).po_Editor->b_CanActivate() == FALSE)
            )
        )
        {
            if(M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane == GetCurFocus())
            {
                b_NeedToChoose = TRUE;
                M_CurEd(mi_NumPane, i).po_Editor->ShowWindow(SW_HIDE);
                if(M_CurEd(mi_NumPane, i).po_Editor->mpo_Menu)
                    M_CurEd(mi_NumPane, i).po_Editor->mpo_Menu->ShowWindow(SW_HIDE);
                M_CurEd(mi_NumPane, i).po_Editor->OnDisactivate();
            }

            for(j = i + 1; j < M_CurNumEd(mi_NumPane); j++)
            {
                if((M_CurEd(mi_NumPane, j).po_Editor) && (M_CurEd(mi_NumPane, j).po_Editor->mi_NumPane != -1))
                {
                    M_CurEd(mi_NumPane, j).po_Editor->mi_NumPane--;
                }
            }

            M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent = FALSE;
            DeleteItem(M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane);
            M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane = -1;
            b_Change = TRUE;
        }

        if(M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent)
        {
            M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane = i_NumPane++;
            b_HasChoose = FALSE;
        }

        /* Determins if current editor can now been activated */
        if
        (
            (M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent == FALSE) &&
            (M_CurEd(mi_NumPane, i).po_Editor->b_CanActivate()) &&
            (M_CurEd(mi_NumPane, i).po_Editor->mst_BaseIni.b_IsVisible)
        )
        {
            for(j = i + 1; j < M_CurNumEd(mi_NumPane); j++)
            {
                if((M_CurEd(mi_NumPane, j).po_Editor) && (M_CurEd(mi_NumPane, j).po_Editor->mi_NumPane != -1))
                {
                    M_CurEd(mi_NumPane, j).po_Editor->mi_NumPane++;
                }
            }

            st_Item.mask = TCIF_TEXT;

            /* For icon on left of name */
            L_strcpy(asz_Name, "    ");
            L_strcat(asz_Name, M_CurEd(mi_NumPane, i).po_Editor->mst_Def.asz_Name);

            /* Add spaces at the end of the name to draw the close button */
            if(M_CurEd(mi_NumPane, i).po_Editor->mi_NumEdit)
                L_strcat(asz_Name, "     ");

            st_Item.pszText = asz_Name;
            M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent = TRUE;
            M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane = i_NumPane++;
            InsertItem(M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane, &st_Item);
            b_Change = TRUE;
        }
    }

    /* Choose a new pane to activate */
    if(b_NeedToChoose || b_HasChoose)
    {
        for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
        {
            if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent))
            {
                SetCurSel(M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane);
                M_CurEd(mi_NumPane, i).po_Editor->ShowWindow(SW_SHOW);
                if(M_CurEd(mi_NumPane, i).po_Editor->mpo_Menu)
                    M_CurEd(mi_NumPane, i).po_Editor->mpo_Menu->ShowWindow(SW_SHOW);
                M_CurEd(mi_NumPane, i).po_Editor->OnActivate();
                OnChangePane(NULL, NULL);
                break;
            }
        }
    }

    /*
     * Decrease number of editors if there are some empty spaces at the end of the
     * array.
     */
    for(i = M_CurNumEd(mi_NumPane) - 1; i >= 0; i--)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor == NULL)
            M_CurNumEd(mi_NumPane)--;
        else
            break;
    }

    /* Be sure that editor with focus is activated */
    if(po_Wnd && IsWindow(po_Wnd->m_hWnd)) po_Wnd->SetFocus();
    po_Editor = EDI_go_TheApp.GetFocusedEditor();
    if(po_Editor && !po_Editor->mb_IsActivate)
        M_MF()->SetFocus();

    /* Force resize */
    if(b_Change && GetParent())
    {
        ENG_gb_GlobalLock = TRUE;
        GetWindowRect(&o_Rect);
        GetParent()->ScreenToClient(&o_Rect);
		SendMessage(WM_SIZE, 0, o_Rect.Width() + (o_Rect.Height() << 16));
        ENG_gb_GlobalLock = FALSE;
    }

    M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::BeforeEngine(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor) M_CurEd(mi_NumPane, i).po_Editor->BeforeEngine();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::AfterEngine(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor) M_CurEd(mi_NumPane, i).po_Editor->AfterEngine();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OneTrameEnding(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i;
    EDI_cl_BaseFrame    *po_Editor;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        po_Editor = M_CurEd(mi_NumPane, i).po_Editor;

        if
        (
            (po_Editor) &&
            (po_Editor->mst_BaseIni.b_IsVisible) &&
            (po_Editor->mb_IsPresent) &&
            (po_Editor->b_CanBeLinkedToEngine()) &&
            (
                ((po_Editor->mst_BaseIni.b_EngineRefresh) && (EDI_go_TheApp.mb_RunEngine)) ||
                ((po_Editor->mst_BaseIni.b_EditorRefresh) && (!EDI_go_TheApp.mb_RunEngine))
            )
        )
        {
			if(po_Editor->mst_Def.i_Type == EDI_IDEDIT_RASTERS)
				gpo_RasterEditor = (ERAS_cl_Frame *) po_Editor;
			else
				po_Editor->OneTrameEnding();
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OpenProject(void)
{
    /*~~~~~~~~~~~~~~~~~*/
    int     i, i_Mem;
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~*/

    /* Call OpenProject for each one */
    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor) M_CurEd(mi_NumPane, i).po_Editor->OpenProject();
    }

    /* Display all editors */
    i_Mem = M_CurSelScr(mi_NumPane);
    ActivateDisactivateEditors();

    /* Select first pane */
    if((i_Mem != -1) && (M_CurEd(mi_NumPane, i_Mem).po_Editor))
    {
        SetCurSel(M_CurEd(mi_NumPane, i_Mem).po_Editor->mi_NumPane);
        OnChangePane(NULL, NULL);
    }

    /* Force resize view, to force resize editors */
    GetWindowRect(&o_Rect);
    GetParent()->ScreenToClient(&o_Rect);
	SendMessage(WM_SIZE, 0, o_Rect.Width() + (o_Rect.Height() << 16));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_BaseView::b_AcceptToCloseProject(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->b_AcceptToCloseProject() == FALSE))
            return FALSE;
    }

    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::CloseProject(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor)
        {
            M_CurEd(mi_NumPane, i).po_Editor->CloseProject();
            M_CurEd(mi_NumPane, i).po_Editor->DestroyWindow();
            M_CurEd(mi_NumPane, i).i_Type = -1;
            M_CurEd(mi_NumPane, i).i_Cpt = 0;
            M_CurEd(mi_NumPane, i).po_Editor = NULL;
            DeleteItem(0);
        }
    }

    M_CurNumEd(mi_NumPane) = 0;
    M_CurSelScr(mi_NumPane) = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::WhenAppClose(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor) M_CurEd(mi_NumPane, i).po_Editor->WhenAppClose();
    }

    /* Close the editors. */
    for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
    {
        if(M_CurEd(mi_NumPane, i).po_Editor) M_CurEd(mi_NumPane, i).po_Editor->DestroyWindow();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_BaseView::po_GetActivatedEditor(void)
{
    /*~~~~~~~~~~~~~~~~~*/
    int i_Sel, i_Num;
    /*~~~~~~~~~~~~~~~~~*/

    i_Sel = GetCurSel();
    if(i_Sel != -1)
    {
        i_Num = i_NumEditFromPane(i_Sel);
        return M_CurEd(mi_NumPane, i_Num).po_Editor;
    }

    return NULL;
}

/*$4
 ***********************************************************************************************************************
    Messages
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT EDI_cl_BaseView::OnNcHitTest(CPoint)
{
    return HTCLIENT;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void EDI_cl_BaseView::OnRButtonDown(UINT nID, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CMenu               o_Menu;
    CMenu               o_Menu1, o_Menu2, o_Menu3, o_Menu4;
    int                 i, j, k, i_Num, i_Pos;
    TC_HITTESTINFO      st_Hit;
    EDI_cl_BaseFrame    *po_Editor;
    EDI_cl_BaseView     *po_View;
    BOOL                b_Forced;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Drag&Drop to duplicate an editor */
    b_Forced = FALSE;

    if(EDI_gst_DragDrop.b_BeginDragDrop)
    {
        ReleaseCapture();
        EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
        return;
    }

    o_Menu.CreatePopupMenu();

    /* Get editor */
    i_Num = i_NumEditFromPane(GetCurSel());

    /* Force it if nID is -1 (external request). */
    st_Hit.pt = pt;
    i_Pos = 0;
    if(((int) nID == -1) || ((HitTest(&st_Hit) == GetCurSel()) && (HitTest(&st_Hit) != -1)))
    {
        po_Editor = M_CurEd(mi_NumPane, i_Num).po_Editor;

        /* Help */
        o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION, WM_USER + 40, "Help");
        o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCSTR) NULL);

        /* Display/Hide editor */
        o_Menu.InsertMenu(i_Pos, MF_BYPOSITION, WM_USER + 10, EDI_STR_Csz_PopupShow);
        if(po_Editor->mst_BaseIni.b_IsVisible) o_Menu.CheckMenuItem(i_Pos, MF_CHECKED | MF_BYPOSITION);
        i_Pos++;

        if(this != M_MF()->mpo_MenuView)
        {
            /* Maximised */
            o_Menu.InsertMenu(i_Pos, MF_BYPOSITION, WM_USER + 20, EDI_STR_Csz_Maximized);
            if(M_MF()->mpo_MaxView == this) o_Menu.CheckMenuItem(i_Pos, MF_CHECKED | MF_BYPOSITION);
            i_Pos++;

            /* Full maximised */
            o_Menu.InsertMenu(i_Pos, MF_BYPOSITION, WM_USER + 21, EDI_STR_Csz_FullMaximized);
            i_Pos++;
        }

        /* Floating */
        o_Menu.InsertMenu(i_Pos, MF_BYPOSITION, WM_USER + 23, EDI_STR_Csz_Floating);
        if(mb_Floating) o_Menu.CheckMenuItem(i_Pos, MF_CHECKED | MF_BYPOSITION);
        i_Pos++;

        if(po_Editor->b_CanBeLinkedToEngine())
        {
            /* Insert a separator */
            if(i_Pos) o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCSTR) NULL);

            /* Refresh by engine */
            o_Menu.InsertMenu(i_Pos, MF_BYPOSITION, WM_USER + 11, EDI_STR_Csz_PopupEngineRefresh);
            if(po_Editor->mst_BaseIni.b_EngineRefresh)
                o_Menu.CheckMenuItem(i_Pos, MF_CHECKED | MF_BYPOSITION);

            i_Pos++;

            /* Refresh by editor */
            o_Menu.InsertMenu(i_Pos, MF_BYPOSITION, WM_USER + 16, EDI_STR_Csz_PopupEditorRefresh);
            if(po_Editor->mst_BaseIni.b_EditorRefresh)
            {
                o_Menu.CheckMenuItem(i_Pos, MF_CHECKED | MF_BYPOSITION);
            }

            i_Pos++;
        }

        /* Links */
        if(po_Editor->b_CanBeLinked())
        {
            o_Menu1.CreatePopupMenu();
            M_MF()->EditorMenu(&o_Menu1, po_Editor, 1, 101);
            if(o_Menu.GetMenuItemCount())
            {
                o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCSTR) NULL);
                o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION | MF_POPUP, (UINT) o_Menu1.m_hMenu, EDI_STR_Csz_PopupLink);
                o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION, WM_USER + 17, EDI_STR_Csz_PopupPipe);
            }
        }

        /* Insert a separator */
        if(i_Pos) o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCSTR) NULL);

        /*$S
         * Ini
         */
        o_Menu2.CreatePopupMenu();
        o_Menu.InsertMenu(i_Pos++, MF_BYPOSITION | MF_POPUP, (UINT) o_Menu2.m_hMenu, EDI_STR_Csz_PopupIni);

        /* Save Current */
        o_Menu2.InsertMenu(i_Pos++, MF_BYPOSITION, WM_USER + 14, EDI_STR_Csz_PopupSaveIni);

        /* Save as default */
        o_Menu2.InsertMenu(i_Pos++, MF_BYPOSITION, WM_USER + 15, EDI_STR_Csz_PopupSaveIniDef);
        o_Menu2.InsertMenu(i_Pos++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCSTR) NULL);

        /* Hard reset */
        o_Menu2.InsertMenu(i_Pos++, MF_BYPOSITION, WM_USER + 12, EDI_STR_Csz_PopupReinit);

        /* Reload default */
        o_Menu2.InsertMenu(i_Pos++, MF_BYPOSITION, WM_USER + 13, EDI_STR_Csz_PopupReloadDef);

        /* Init from */
        o_Menu3.CreatePopupMenu();
        M_MF()->EditorMenu(&o_Menu3, po_Editor, 2, 201, po_Editor->mst_Def.i_Type);
        if(o_Menu3.GetMenuItemCount())
        {
            o_Menu2.InsertMenu(i_Pos++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCSTR) NULL);
            o_Menu2.InsertMenu(i_Pos++, MF_BYPOSITION | MF_POPUP, (UINT) o_Menu3.m_hMenu, EDI_STR_Csz_PopupInitFrom);
        }

        /* Show menu */
        ClientToScreen(&pt);
        mi_PopupMenu = 1;
        o_Menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
        return;
    }

    /* Menu to activate an editor. */
    i = i_Num = 0;
    while(i < M_CurNumEd(mi_NumPane))
    {
        po_Editor = M_CurEd(mi_NumPane, i).po_Editor;
        if((po_Editor) && (po_Editor->b_CanActivate()))
        {
            if(po_Editor->mst_BaseIni.b_IsVisible == FALSE)
                b_Forced = TRUE;

            o_Menu.InsertMenu(i_Num, MF_BYPOSITION, WM_USER + i + 1, po_Editor->mst_Def.asz_Name);
            if((po_Editor->mi_NumPane != -1) && (po_Editor->mi_NumPane == GetCurFocus()))
            {
                o_Menu.CheckMenuItem(i_Num, MF_CHECKED | MF_BYPOSITION);
            }

            i_Num++;
        }

        i++;
    }

    if(i_Num > 0)
    {
        o_Menu.InsertMenu(i_Num, MF_SEPARATOR | MF_BYPOSITION, 0, po_Editor->mst_Def.asz_Name);
        i++;
        i_Num++;
    }

    /* All the rest */
    i = 0;
    for(j = 0; j < EDI_C_MaxViews; j++)
    {
        po_View = M_MF()->po_GetViewWithNum(j);
        if(po_View != this)
        {
            k = 0;
            while(k < M_CurNumEd(po_View->mi_NumPane))
            {
                po_Editor = M_CurEd(po_View->mi_NumPane, k).po_Editor;
                if((po_Editor) && (po_Editor->b_CanActivate()))
                {
                    o_Menu.InsertMenu(i_Num, MF_BYPOSITION, WM_USER + 50 + i + 1, po_Editor->mst_Def.asz_Name);
                    i_Num++;
                }

                k++;
                i++;
            }
        }
    }

    if((i_Num > 1) || b_Forced)
    {
        ClientToScreen(&pt);
        mi_PopupMenu = 2;
        o_Menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnPopupMenu(UINT _i_ID)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i_Num;
    CRect               o_Rect;
    EDI_cl_BaseFrame    *po_Editor;
    char                asz_MemoName[BIG_C_MaxLenName];
    char                asz_MemoPath[BIG_C_MaxLenPath];
    char                asz_Path[BIG_C_MaxLenPath];
    char                *psz_Temp;
    MSG                 msg;
    int                 i, j, k, b;
    EDI_cl_BaseView     *po_View;
    CPoint              pt;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _i_ID -= WM_USER;
    switch(mi_PopupMenu)
    {
    /* Common config for all editors */
    case 1:
        i_Num = i_NumEditFromPane(GetCurSel());
        po_Editor = M_CurEd(mi_NumPane, i_Num).po_Editor;
        switch(_i_ID)
        {
        case 40:    /* Help */
            if(po_Editor->mpo_Original) po_Editor = po_Editor->mpo_Original;
            L_strcpy(asz_Path, po_Editor->mst_Def.asz_Name);
            L_strcat(asz_Path, "/");
            L_strcat(asz_Path, po_Editor->mst_Def.asz_Name);
            EDI_go_TheApp.SearchHelp(asz_Path);
            break;

        case 10:    /* Display/Hide editor */
            if(po_Editor->mst_BaseIni.b_IsVisible)
                po_Editor->mst_BaseIni.b_IsVisible = FALSE;
            else
                po_Editor->mst_BaseIni.b_IsVisible = TRUE;
            ActivateDisactivateEditors();

            /* Refresh left list of main frame */
            M_MF()->mpo_MenuFrame->FillList();
            return;

        case 20:    /* Maximized */
            SetFocus();
            M_MF()->OnAction(EDI_ACTION_DESKMAX);
            return;
        case 21:    /* Full Maximized */
            SetFocus();
            M_MF()->OnAction(EDI_ACTION_DESKFULLMAX);
            return;
        case 23:    /* Floating */
            if(mb_Floating)
                SetFloating(FALSE);
            else
                SetFloating(TRUE);
            return;

        case 22:
            SetFocus();
            M_MF()->OnAction(EDI_ACTION_DESKFULLMAXRES);
            return;

        case 11:    /* Refresh by engine */
            if(po_Editor->mst_BaseIni.b_EngineRefresh)
                po_Editor->mst_BaseIni.b_EngineRefresh = FALSE;
            else
                po_Editor->mst_BaseIni.b_EngineRefresh = TRUE;
            return;

        case 16:    /* Refresh by editor */
            if(po_Editor->mst_BaseIni.b_EditorRefresh)
                po_Editor->mst_BaseIni.b_EditorRefresh = FALSE;
            else
                po_Editor->mst_BaseIni.b_EditorRefresh = TRUE;
            return;

        case 17:    /* Pick a Data */
            if(M_MF()->mpo_PipeFrame == po_Editor)
            {
                M_MF()->mpo_PipeFrame = NULL;
            }
            else
            {
                M_MF()->mpo_PipeFrame = po_Editor;
                while(M_MF()->mpo_PipeFrame && GetMessage(&msg, NULL, NULL, NULL))
                {
                    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PIPE));
                    if(msg.message == WM_SETCURSOR) continue;
                    if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
                        break;
                    if(msg.message == WM_RBUTTONDOWN) break;
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                SendMessage(WM_SETCURSOR);
                M_MF()->mpo_PipeFrame = NULL;
            }

            return;

        case 12:    /* Hard reset */
            M_MF()->LockDisplay(po_Editor);
            po_Editor->ReinitIni();
            po_Editor->TreatIni();
            po_Editor->ForceRefresh();
            po_Editor->SaveIni();
            M_MF()->UnlockDisplay(po_Editor);
            return;

        case 13:    /* Reload default */
            M_MF()->LockDisplay(po_Editor);

            L_strcpy(asz_MemoName, po_Editor->mst_Def.asz_Name);
            L_strcat(asz_MemoName, EDI_Csz_ExtIni);

            if(BIG_ul_SearchFileExt(EDI_Csz_Ini, asz_MemoName) != BIG_C_InvalidIndex)
                BIG_DelFile(EDI_Csz_Ini, asz_MemoName);

            L_strcpy(asz_MemoPath, EDI_Csz_Ini_Desktop);
            L_strcat(asz_MemoPath, "/");
            L_strcat(asz_MemoPath, M_MF()->mst_Ini.asz_CurrentDeskName);
            if(BIG_ul_SearchFileExt(asz_MemoPath, asz_MemoName) != BIG_C_InvalidIndex)
                BIG_DelFile(asz_MemoPath, asz_MemoName);

            po_Editor->LoadIni();
            po_Editor->ForceRefresh();
            M_MF()->UnlockDisplay(po_Editor);
            return;

        case 14:    /* Save ini */
            po_Editor->SaveIni();
            return;

        case 15:    /* Save ini default */

            /* Change editor name (temporary) to compute default ini file name */
            L_strcpy(asz_MemoName, po_Editor->mst_Def.asz_Name);
            psz_Temp = po_Editor->mst_Def.asz_Name;
            while(!L_isspace(*psz_Temp) && *psz_Temp)
                psz_Temp++;
            *psz_Temp = 0;
            L_strcat(po_Editor->mst_Def.asz_Name, EDI_Csz_ExtIniDef);
            po_Editor->SaveIni();
            L_strcpy(po_Editor->mst_Def.asz_Name, asz_MemoName);
            return;
        }

        po_Editor->SetRedraw(FALSE);
        po_Editor->GetWindowRect(&o_Rect);
        po_Editor->SendMessage(WM_SIZE, 0, (o_Rect.right - o_Rect.left + 1) + ((o_Rect.bottom - o_Rect.top + 1) << 16));

        po_Editor->SetRedraw(TRUE);
        po_Editor->RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
        break;

    /* Change pane */
    case 2:
        if(_i_ID - 1 > (UINT)M_CurNumEd(mi_NumPane))
        {
            /* All the rest */
            i = _i_ID - 51;
            po_Editor = NULL;
            for(j = 0; j < EDI_C_MaxViews; j++)
            {
                po_View = M_MF()->po_GetViewWithNum(j);
                if(po_View != this)
                {
                    k = 0;
                    b = 0;
                    while(k < M_CurNumEd(po_View->mi_NumPane))
                    {
                        po_Editor = M_CurEd(po_View->mi_NumPane, k).po_Editor;
                        if(!i) goto l_Found;

                        if((po_Editor) && (po_Editor->b_CanActivate()) && (po_Editor->mst_BaseIni.b_IsVisible))
                            b++;

                        k++;
                        i--;
                    }
                }
            }

            if(!po_Editor) return;
l_Found:
            po_Editor->mpo_MyView->SetCapture();
            po_Editor->mpo_MyView->mpo_DragView = this;
            po_Editor->mpo_MyView->mb_Outside = FALSE;
            EDI_gst_DragDrop.b_BeginDragDrop = TRUE;
            EDI_gst_DragDrop.b_CanDragDrop = TRUE;
            if(!po_Editor->mst_BaseIni.b_IsVisible)
            {
                po_Editor->mst_BaseIni.b_IsVisible = TRUE;
                po_Editor->mpo_MyView->ActivateDisactivateEditors();
            }

            po_Editor->mpo_MyView->mi_PaneDragDrop = po_Editor->mpo_MyView->i_NumEditFromPane(b);
            po_Editor->mpo_MyView->mb_DragDropCopy = FALSE;
            GetCursorPos(&pt);
            po_Editor->mpo_MyView->ScreenToClient(&pt);
            po_Editor->mpo_MyView->OnLButtonUp(0, pt);
        }
        else
        {
            po_Editor = M_CurEd(mi_NumPane, _i_ID - 1).po_Editor;
            if(po_Editor)
            {
                if(po_Editor->mst_BaseIni.b_IsVisible == FALSE)
                {
                    po_Editor->mst_BaseIni.b_IsVisible = TRUE;
                    ActivateDisactivateEditors();
                }
            }

            SetCurSel(po_Editor->mi_NumPane);
            OnChangePane(NULL, NULL);
        }

        /* Refresh left list of main frame */
        M_MF()->mpo_MenuFrame->FillList();
        break;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnLinkTo(UINT _i_ID)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_cl_BaseFrame    *po_Editor, *po_Current;
    int                 i_Num;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _i_ID -= WM_USER;
    i_Num = i_NumEditFromPane(GetCurSel());
    po_Current = M_CurEd(mi_NumPane, i_Num).po_Editor;

    /* Retreive selected editor in menu */
    po_Editor = M_MF()->po_GetEditorByMenu(_i_ID, 101);

    /* Link/Unlink */
    if(po_Current->mst_BaseIni.ab_LinkTo[po_Editor->mi_PosInGArray][po_Editor->mi_NumEdit])
    {
        po_Current->mst_BaseIni.ab_LinkTo[po_Editor->mi_PosInGArray][po_Editor->mi_NumEdit] = FALSE;
        po_Editor->mst_BaseIni.ab_LinkTo[po_Current->mi_PosInGArray][po_Current->mi_NumEdit] = FALSE;

        /* Send message to inform */
        po_Current->i_OnMessage(EDI_MESSAGE_HASBEENUNLINKED, (ULONG) po_Current, (ULONG) po_Editor);
        po_Editor->i_OnMessage(EDI_MESSAGE_HASBEENUNLINKED, (ULONG) po_Current, (ULONG) po_Editor);
    }
    else
    {
        po_Current->mst_BaseIni.ab_LinkTo[po_Editor->mi_PosInGArray][po_Editor->mi_NumEdit] = TRUE;
        po_Editor->mst_BaseIni.ab_LinkTo[po_Current->mi_PosInGArray][po_Current->mi_NumEdit] = TRUE;

        /* Send message to inform */
        po_Current->i_OnMessage(EDI_MESSAGE_HASBEENLINKED, (ULONG) po_Current, (ULONG) po_Editor);
        po_Editor->i_OnMessage(EDI_MESSAGE_HASBEENLINKED, (ULONG) po_Current, (ULONG) po_Editor);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnInitFrom(UINT _i_ID)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_cl_BaseFrame    *po_Editor, *po_Current;
    int                 i_Num;
    char                asz_Memo[BIG_C_MaxLenPath];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _i_ID -= WM_USER;
    i_Num = i_NumEditFromPane(GetCurSel());
    po_Current = M_CurEd(mi_NumPane, i_Num).po_Editor;

    /* Retreive selected editor in menu */
    po_Editor = M_MF()->po_GetEditorByMenu(_i_ID, 201);

    /* Simulate loading with the dest editor ini file name */
    L_strcpy(asz_Memo, po_Current->mst_Def.asz_Name);
    L_strcpy(po_Current->mst_Def.asz_Name, po_Editor->mst_Def.asz_Name);

    M_MF()->LockDisplay(po_Current);

_Try_
    po_Editor->SaveIni();
    po_Current->LoadIni();
_Catch_
_End_
    /* Force refresh current editor */
    po_Current->ForceRefresh();
    M_MF()->UnlockDisplay(po_Current);

    L_strcpy(po_Current->mst_Def.asz_Name, asz_Memo);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnLButtonDown(UINT nFlags, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 mi_Res;
    TC_HITTESTINFO      st_Hit;
    EDI_cl_BaseFrame    *po_Editor;
    EMEN_cl_Frame       *po_Menu;
    CRect               o_Rect;
    char                asz_Name[100];
    TC_ITEM             st_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* No process if mouse is not on a tab */
    st_Hit.pt = pt;
    mi_Res = HitTest(&st_Hit);
    if(mi_Res == -1)
    {
        CTabCtrl::OnLButtonDown(nFlags, pt);
        return;
    }

    /* Force focus on tab */
    SetFocus();

    /* Prepare for drag & drop */
    EDI_gst_DragDrop.b_BeginDragDrop = TRUE;
    EDI_gst_DragDrop.b_CanDragDrop = FALSE;
    EDI_gst_DragDrop.po_CaptureWnd = this;

    SetCapture();
    CTabCtrl::OnLButtonDown(nFlags, pt);

    mi_PaneDragDrop = i_NumEditFromPane(GetCurSel());
    mb_Outside = FALSE;

    /* Activate the left menu */
    po_Editor = M_CurEd(mi_NumPane, mi_PaneDragDrop).po_Editor;
    po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, po_Editor->mst_BaseIni.i_CountMenu);
    if(!po_Menu)
    {
        po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, 0);
    }

    if(po_Menu && po_Menu->mpo_CurrentExpand != po_Editor->mpo_MenuFrame)
        po_Menu->NcClick(po_Editor->mpo_MenuFrame);

    /* Get name */
    st_Item.mask = TVIF_TEXT;
    st_Item.pszText = asz_Name;
    st_Item.cchTextMax = 100;
    GetItem(GetCurSel(), &st_Item);

    /* Mouse on close button */
    if(asz_Name[L_strlen(asz_Name) - 1] == ' ')
    {
        GetItemRect(GetCurSel(), o_Rect);
        o_Rect.left = o_Rect.right - 14;
        if(o_Rect.PtInRect(pt))
        {
            BOOL b_NeedActivate;
            b_NeedActivate = DestroyEditor(M_CurEd(mi_NumPane, mi_PaneDragDrop).po_Editor);
            if(b_NeedActivate) ActivateNewPane();
            EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
            return;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_BaseView::DestroyEditor(EDI_cl_BaseFrame *po_DstEditor)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_Frame   *po_Menu;
    int             i;
    BOOL            b_Activate;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Activate = FALSE;
    if(po_DstEditor->mpo_Original)
    {
        if(po_DstEditor->b_AcceptToCloseProject())
        {
            M_MF()->LockDisplay(&M_MF()->mo_BigSplitter);

            po_DstEditor->CloseProject();
            for(i = mi_PaneDragDrop + 1; i < M_CurNumEd(mi_NumPane); i++)
            {
                if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane != -1))
                {
                    M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane--;
                }
            }

            /* Delete associated menu */
            if(po_DstEditor->mpo_MenuFrame)
            {
                po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType
                    (
                        EDI_IDEDIT_MENU,
                        po_DstEditor->mst_BaseIni.i_CountMenu
                    );
                if(!po_Menu)
                {
                    po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, 0);
                }

                if(po_DstEditor->mpo_Menu) po_DstEditor->mpo_MenuFrame->DestroyTopMenu(po_DstEditor);

                po_Menu->RemoveMenu(po_DstEditor->mpo_MenuFrame);
            }

            /* Delete editor in original array */
            po_DstEditor->mpo_Original->mab_PresentEdit[po_DstEditor->mi_NumEdit] = FALSE;

            /* Decrease the number of copies of the original editor */
            po_DstEditor->mpo_Original->mst_BaseIni.i_NumCopy--;

            /* Delete editor of that view */
            DeleteItem(po_DstEditor->mi_NumPane);
            po_DstEditor->DestroyWindow();
            M_CurEd(mi_NumPane, mi_PaneDragDrop).i_Type = -1;
            M_CurEd(mi_NumPane, mi_PaneDragDrop).i_Cpt = 0;
            M_CurEd(mi_NumPane, mi_PaneDragDrop).po_Editor = NULL;
            b_Activate = TRUE;

            M_MF()->UnlockDisplay(&M_MF()->mo_BigSplitter);
        }
    }

    return b_Activate;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::ActivateNewPane(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    for(i = mi_PaneDragDrop + 1; i < M_CurNumEd(mi_NumPane); i++)
    {
        if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent))
            break;
    }

    if(i == M_CurNumEd(mi_NumPane))
    {
        for(i = mi_PaneDragDrop - 1; i >= 0; i--)
        {
            if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mb_IsPresent))
                break;
        }
    }

    M_CurSelScr(mi_NumPane) = i;
    if(i > -1)
    {
        SetCurSel(M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane);
        OnChangePane(NULL, NULL);
    }
}

/*
 =======================================================================================================================
    Aim:    Very important function that process the copy/move/deletion of panes (so of editors).
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnLButtonUp(UINT nFlags, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect               o_Rect;
    int                 i, i_Res, i_Num;
    TC_HITTESTINFO      st_Hit;
    char                *mpsz_Name;
    BOOL                b_NeedActivate;
    EDI_cl_BaseFrame    *po_Editor, *po_DstEditor;
    BOOL                b_CanSize;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_NeedActivate = FALSE;
    b_CanSize = FALSE;

    if(GetCapture() == NULL)
    {
        EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
        CTabCtrl::OnLButtonUp(nFlags, pt);
        return;
    }

    ReleaseCapture();
    if(M_CurEd(mi_NumPane, mi_PaneDragDrop).po_Editor == NULL)
    {
        EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
        CTabCtrl::OnLButtonUp(nFlags, pt);
        return;
    }

    if(EDI_gst_DragDrop.b_BeginDragDrop)
    {
        po_DstEditor = M_CurEd(mi_NumPane, mi_PaneDragDrop).po_Editor;

        EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
        if(EDI_gst_DragDrop.b_CanDragDrop)
        {
            M_MF()->LockDisplay(&M_MF()->mo_BigSplitter);
            b_CanSize = TRUE;

            /*$2
             -----------------------------------------------------------------------------------------------------------
                Insert a new place in destination.
             -----------------------------------------------------------------------------------------------------------
             */

            /* Pt must be in dragview coordinates */
            ClientToScreen(&pt);
            mpo_DragView->ScreenToClient(&pt);

            /* Search the place to insert the duplicated editor */
            st_Hit.pt = pt;
            i_Res = mpo_DragView->HitTest(&st_Hit);

            /* Shift dest editors to insert new one */
            if(i_Res != -1)
            {
                i_Res = mpo_DragView->i_NumEditFromPane(i_Res);

                if(i_Res && (M_CurEd(mpo_DragView->mi_NumPane, i_Res - 1).po_Editor == NULL))
                {
                    i_Res--;
                }
                else
                {
                    ERR_X_Error
                    (
                        M_CurNumEd(mpo_DragView->mi_NumPane) + 1 != EDI_C_MaxEditors,
                        EDI_ERR_Csz_TooManyEditors,
                        NULL
                    );

                    for(i = M_CurNumEd(mpo_DragView->mi_NumPane) - 1; i >= i_Res; i--)
                    {
                        L_memcpy
                        (
                            &M_CurEd(mpo_DragView->mi_NumPane, i + 1),
                            &M_CurEd(mpo_DragView->mi_NumPane, i),
                            sizeof(M_CurEd(mpo_DragView->mi_NumPane, i))
                        );
                    }

                    M_CurEd(mpo_DragView->mi_NumPane, i_Res).po_Editor = NULL;
                    M_CurNumEd(mpo_DragView->mi_NumPane)++;

                    M_CurSelScr(mpo_DragView->mi_NumPane)++;
                    if((mpo_DragView == this) && (i_Res <= mi_PaneDragDrop))
                        mi_PaneDragDrop++;
                }
            }
            else
            {
				// Modif on dec.9.2004, Yan Charbonneau
				// corrected a drag and drop problem when having to many drags, i_Res index was going out of scope.
				// replaced  
				// i_Res = M_CurNumEd(mpo_DragView->mi_NumPane);		
				// by 

				// Trying to find a spot for the editor
				int i = 0 ; 
				// if we do not find a spot for the editor, adding it at the end of the array
				// that's what [ <= ] is for
				for(i = 0; i <= M_CurNumEd(mpo_DragView->mi_NumPane); i++)
				{
					// Trying to search for released editor first 
					if	(	(M_CurEd(mpo_DragView->mi_NumPane, i).i_Type == -1 )  || 
							(M_CurEd(mpo_DragView->mi_NumPane, i).po_Editor == NULL)
						)
					{
						break;
					}
				}
				i_Res = i ;
            }

            /*$2
             -----------------------------------------------------------------------------------------------------------
                Duplicate editor.
             -----------------------------------------------------------------------------------------------------------
             */

            if(mb_DragDropCopy)
            {
                /* Compute name */
                if(po_DstEditor->mpo_Original)
                    mpsz_Name = po_DstEditor->mpo_Original->mst_Def.asz_Name;
                else
                    mpsz_Name = po_DstEditor->mst_Def.asz_Name;

                /* Add editor */
                if(po_DstEditor->mpo_Original)
                    i_Num = po_DstEditor->mpo_Original->i_GetPosDup();
                else
                    i_Num = po_DstEditor->i_GetPosDup();
                po_Editor = mpo_DragView->po_AddOneEditor(po_DstEditor->mst_Def.i_Type, i_Num, i_Res);
                po_Editor->SetFocus();
            }

            /*$2
             -----------------------------------------------------------------------------------------------------------
                Move editor.
             -----------------------------------------------------------------------------------------------------------
             */
            else
            {
                /* Decrease pane in source view */
                if(mpo_DragView != this)
                {
                    for(i = mi_PaneDragDrop + 1; i < M_CurNumEd(mi_NumPane); i++)
                    {
                        if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane != -1))
                        {
                            M_CurEd(mi_NumPane, i).po_Editor->mi_NumPane--;
                        }
                    }

                    po_DstEditor->SetParent(mpo_DragView);
                    if(po_DstEditor->mpo_Menu) po_DstEditor->mpo_Menu->SetParent(mpo_DragView);
                    if(GetCurSel() == po_DstEditor->mi_NumPane)
                        b_NeedActivate = TRUE;
                }
                else
                {
                    M_CurSelScr(mpo_DragView->mi_NumPane) = i_Res;
                }

                DeleteItem(po_DstEditor->mi_NumPane);

                /* Move editor at its new place */
                L_memcpy
                (
                    &M_CurEd(mpo_DragView->mi_NumPane, i_Res),
                    &M_CurEd(mi_NumPane, mi_PaneDragDrop),
                    sizeof(M_CurEd(mi_NumPane, mi_PaneDragDrop))
                ); 

                /* Reinit move */
                po_DstEditor->mb_IsPresent = FALSE;
                M_CurEd(mpo_DragView->mi_NumPane, i_Res).po_Editor->mpo_MyView = mpo_DragView;

                /* Init position of previous view */
				M_CurEd(mi_NumPane, mi_PaneDragDrop).i_Type = -1;
				M_CurEd(mi_NumPane, mi_PaneDragDrop).i_Cpt = 0;
				M_CurEd(mi_NumPane, mi_PaneDragDrop).po_Editor = NULL;

                if( M_CurNumEd(mpo_DragView->mi_NumPane) == i_Res )
				{
					M_CurNumEd(mpo_DragView->mi_NumPane)++;
				}

                mpo_DragView->SetFocus();
            }

            /*
             * Common to move and copy.£
             */

            /* Add new editor in a pane, if it should be activated */
            mpo_DragView->ActivateDisactivateEditors();

            /* This is like opening the project */
            if(mb_DragDropCopy) po_Editor->OpenProject();

            /* Select copy or moved editor */
            mpo_DragView->SetCurSel(M_CurEd(mpo_DragView->mi_NumPane, i_Res).po_Editor->mi_NumPane);
            mpo_DragView->OnChangePane(NULL, NULL);

            /* To force sizing new editor */
            mpo_DragView->GetWindowRect(o_Rect);
            mpo_DragView->GetParent()->ScreenToClient(&o_Rect);
			mpo_DragView->SendMessage(WM_SIZE, 0, o_Rect.Width() + (o_Rect.Height() << 16));

            M_MF()->UnlockDisplay(&M_MF()->mo_BigSplitter);
        }

        /*$2
         ---------------------------------------------------------------------------------------------------------------
            Destroy the copy by moving it on a wrong place.
         ---------------------------------------------------------------------------------------------------------------
         */
        else
        {
            if(mb_Outside)
            {
                b_NeedActivate = DestroyEditor(po_DstEditor);
                b_CanSize = TRUE;
            }
        }

        /*$2
         ---------------------------------------------------------------------------------------------------------------
            Search another pane to activate in current view.
         ---------------------------------------------------------------------------------------------------------------
         */

        if(b_NeedActivate) ActivateNewPane();

        /* Force resize view */
        if(b_CanSize)
        {
            ENG_gb_GlobalLock = TRUE;
            GetWindowRect(o_Rect);
            GetParent()->ScreenToClient(&o_Rect);
			SendMessage(WM_SIZE, 0, o_Rect.Width() + (o_Rect.Height() << 16));
            ENG_gb_GlobalLock = FALSE;
        }

        EDI_gst_DragDrop.b_BeginDragDrop = FALSE;

        /* Refresh left list of main frame */
        if(M_MF()->mpo_MenuFrame) M_MF()->mpo_MenuFrame->FillList();

        return;
    }

    CTabCtrl::OnLButtonUp(nFlags, pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnMouseMove(UINT nFlags, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i_Res;
    CRect               o_Rect;
    TC_HITTESTINFO      st_Hit;
    EDI_cl_BaseView     *po_View;
    EDI_cl_BaseFrame    *po_Editor;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_View = this;

    if(EDI_gst_DragDrop.b_BeginDragDrop)
    {
        /* We can't duplicate, cause we can't drag&drop */
        EDI_gst_DragDrop.b_CanDragDrop = FALSE;

        /* Special test if mouse is on the first selected pane */
        mb_Outside = FALSE;
        st_Hit.pt = pt;
        i_Res = HitTest(&st_Hit);
        if(i_Res == GetCurSel()) return;

        /* Only if we can duplicate the editor, and if it's not already a copy */
        ClientToScreen(&pt);
        mpo_DragView = M_MF()->po_ViewUnderPoint(pt);
        po_Editor = M_CurEd(mi_NumPane, mi_PaneDragDrop).po_Editor;
		if(!po_Editor) return;

        if(mpo_DragView)
        {
            EDI_gst_DragDrop.b_CanDragDrop = TRUE;

            /*
             * Determin if we can duplicate the editor : - Not a already copy - Editor enables
             * the copy. - Not too much copy.
             */
            if
            (
                (
                    ((po_Editor->mpo_Original == NULL) && (po_Editor->mst_BaseIni.i_NumCopy < EDI_C_MaxDuplicate - 1)) ||
                        (
                            (po_Editor->mpo_Original != NULL) &&
                            (po_Editor->mpo_Original->mst_BaseIni.i_NumCopy < EDI_C_MaxDuplicate - 1)
                        )
                ) &&
                (po_Editor->b_CanDuplicate())
            )
            {
                if(GetAsyncKeyState(VK_CONTROL) < 0)    /* NFlags & MK_CONTROL) */
                {
                    mb_DragDropCopy = TRUE;
                    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGCOPY));
                    return;
                }
            }

            /* Can we move the editor to another place ? */
            if((mpo_DragView != this) || (mpo_DragView->GetItemCount() > 1))
            {
                mb_DragDropCopy = FALSE;
                if(mpo_DragView != this)
                {
                    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
                    return;
                }

                if(i_Res == -1) i_Res = GetItemCount();
                i_Res -= mpo_DragView->GetCurSel();
                if((i_Res > 1) || (i_Res < 0))
                {
                    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
                    return;
                }
            }

            /* Here we can't duplicate, but we are inside pane area */
            EDI_gst_DragDrop.b_CanDragDrop = FALSE;
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
        }

        /* We are oustide pane area */
        else
        {
            if(po_Editor->mpo_Original != NULL)
            {
                mb_Outside = TRUE;
                ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGDEL));
            }
            else
            {
                ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
            }
        }

        return;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnLButtonDblClk(UINT, CPoint pt)
{
    /* (Re)set to floating */
    if(GetAsyncKeyState(VK_SHIFT) < 0)
    {
        SetFloating(mb_Floating ? FALSE : TRUE);
        return;
    }

    /* Swap maximised editor */
    if(this != M_MF()->mpo_MenuView && GetItemCount())
        M_MF()->SwapMaximised(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnKeyDown(UINT, UINT, UINT)
{
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(CFloating, CWnd)
BEGIN_MESSAGE_MAP(CFloating, CWnd)
    ON_WM_SIZE()
    ON_WM_NCLBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CFloating::CFloating(void)
{
    mpo_View = NULL;
    mb_Minimize = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFloating::OnSize(UINT n, int cx, int cy)
{
    mb_Minimize = FALSE;
    M_MF()->LockDisplay(this);
    CWnd::OnSize(n, cx, cy);
    if(mpo_View) mpo_View->MoveWindow(0, 0, cx, cy);
    M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFloating::OnNcLButtonDblClk(UINT, CPoint)
{
    /*~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    int     cy;
    /*~~~~~~~~~~~~~~~*/

    if(mb_Minimize)
    {
        mb_Minimize = FALSE;
        GetWindowRect(&o_Rect);
        o_Rect.right = o_Rect.left + mo_BeforeMin.Width();
        o_Rect.bottom = o_Rect.top + mo_BeforeMin.Height();
        MoveWindow(&o_Rect);
        return;
    }

    if(M_MF()->mpo_MaxView)
    {
        if(!mb_Minimize)
        {
            GetWindowRect(&mo_BeforeMin);
            GetWindowRect(&o_Rect);
            cy = GetSystemMetrics(SM_CYCAPTION);
            o_Rect.right = o_Rect.left + 64;
            o_Rect.bottom = o_Rect.top + cy + 4;
            MoveWindow(o_Rect);
            mb_Minimize = TRUE;
        }

        return;
    }

    if(mpo_View) mpo_View->SetFloating(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CFloating::OnEraseBkgnd(CDC *)
{
    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFloating::OnClose(void)
{
    if(mpo_View->mb_Floating)
        mpo_View->SetFloating(FALSE);
    else
        CWnd::OnClose();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::SetFloating(BOOL _b_Float)
{
    /*~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~*/

    if(!_b_Float && M_MF()->mpo_MaxView) return;

    if(_b_Float)
    {
        /* Create popup frame */
        p1 = new CFloating;
        p1->mpo_View = this;

        /* Create replacing frame */
        if(this == M_MF()->mpo_MenuView)
        {
            po_Parent = &M_MF()->mo_BigSplitter;
            uiID = M_MF()->mo_BigSplitter.IdFromRowCol(0, 0);
        }
        else if(this == M_MF()->mpo_LeftView)
        {
            po_Parent = &M_MF()->mo_Splitter1;
            uiID = M_MF()->mo_Splitter1.IdFromRowCol(0, 0);
        }
        else if(this == M_MF()->mpo_RightView)
        {
            po_Parent = &M_MF()->mo_Splitter2;
            uiID = M_MF()->mo_Splitter2.IdFromRowCol(0, 0);
        }
        else if(this == M_MF()->mpo_CornerView)
        {
            po_Parent = &M_MF()->mo_Splitter1;
            if(M_MF()->mo_Splitter1.GetRowCount() == 2)
                uiID = M_MF()->mo_Splitter1.IdFromRowCol(1, 0);
            else
                uiID = M_MF()->mo_Splitter1.IdFromRowCol(0, 1);
        }
        else if(this == M_MF()->mpo_BottomView)
        {
            po_Parent = &M_MF()->mo_Splitter2;
            if(M_MF()->mo_Splitter2.GetRowCount() == 2)
                uiID = M_MF()->mo_Splitter2.IdFromRowCol(1, 0);
            else
                uiID = M_MF()->mo_Splitter2.IdFromRowCol(0, 1);
        }

        p = new CEdit;
        GetWindowRect(&o_Rect);
        GetParent()->ScreenToClient(&o_Rect);
        p->Create(WS_CHILD, o_Rect, po_Parent, uiID);
        p->EnableWindow(FALSE);
        p->ShowWindow(SW_SHOW);

        ShowWindow(SW_HIDE);
        GetWindowRect(&o_Rect);
        M_MF()->ScreenToClient(&o_Rect);
        o_Rect.top += GetSystemMetrics(SM_CYCAPTION);
        o_Rect.bottom += GetSystemMetrics(SM_CYCAPTION);
        p1->CreateEx
            (
                WS_EX_TOOLWINDOW,
                NULL,
                "Floating Editors",
                WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD | WS_CAPTION | WS_THICKFRAME | WS_POPUP,
                o_Rect,
                M_MF(),
                NULL,
                NULL
            );
        SetParent(p1);
        p1->GetWindowRect(&o_Rect);
        o_Rect.top--;
        p1->MoveWindow(&o_Rect);
        p1->ShowWindow(SW_SHOW);
        ShowWindow(SW_SHOW);

        M_MF()->mo_BigSplitter.RecalcLayout();

        mb_Floating = TRUE;
        APP_go_Modeless.AddTail(m_hWnd);
        APP_go_ModelessNoRef.AddTail(m_hWnd);
    }
    else
    {
        mb_Floating = FALSE;
        p->DestroyWindow();
        SetParent(po_Parent);
        p1->DestroyWindow();
        delete p;
        delete p1;
        ((CSplitterWnd *) po_Parent)->RecalcLayout();

        APP_go_Modeless.RemoveAt(APP_go_Modeless.Find(m_hWnd));
        APP_go_ModelessNoRef.RemoveAt(APP_go_ModelessNoRef.Find(m_hWnd));
    }
}

#endif /* ACTIVE_EDITORS */
