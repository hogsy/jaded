/*$T MENin.cpp GC!1.40 07/08/99 11:39:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Class that is the main menu of appli or editors. This is a frame with a title, and with
            2 panes.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "EDIbaseframe.h"
#include "MENframe.h"
#include "MENlist.h"
#include "MENmenu.h"
#include "MENinmenu.h"
#include "EDIaction.h"
#include "MENstrings.h"
#include "Res/Res.h"
#include "ENGine/Sources/ENGmsg.h"

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EMEN_cl_FrameIn, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_NCHITTEST()
    ON_WM_SIZE()
    ON_WM_NCPAINT()
    ON_WM_NCLBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_NCCALCSIZE()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EMEN_cl_FrameIn::EMEN_cl_FrameIn(EDI_cl_ActionList *_po_Actions, EDI_cl_BaseFrame *_po_Editor)
{
    mpo_Actions = _po_Actions;
    mpo_Editor = _po_Editor;
    mpo_ListUp = NULL;
    mpo_ListDown = NULL;
    mb_Locked = FALSE;
    mpo_VarsView = new EVAV_cl_View;
    mb_MyCanDD = FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EMEN_cl_FrameIn::~EMEN_cl_FrameIn(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION    pos;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    delete mpo_ListUp;
    delete mpo_ListDown;
    delete mpo_VarsView;

    /* Remove frame from left frame list */
    pos = mpo_Parent->mo_List.Find(this);
    if(pos)
        mpo_Parent->mo_List.RemoveAt(pos);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EMEN_cl_FrameIn::OnCreate(LPCREATESTRUCT lpcs)
{
    CFrameWnd::OnCreate(lpcs);

    /* Create static splitter */
    mo_Splitter.CreateStatic(this, 2, 1, WS_VISIBLE | WS_CHILD);

    /* Create variable view. */
    mpo_VarsView->mb_CanDragDrop = FALSE;
    mst_VarsViewStruct.po_ListItems = &mo_ListItems;
    mst_VarsViewStruct.psz_NameCol1 = EMEN_STR_Csz_Name;
    mst_VarsViewStruct.i_WidthCol1 = 150;
    mst_VarsViewStruct.psz_NameCol2 = EMEN_STR_Csz_Value;
    mpo_VarsView->MyCreate
        (
            &mo_Splitter,
            &mst_VarsViewStruct,
            mpo_Actions->mpo_Editor,
            mo_Splitter.IdFromRowCol(1, 0)
        );
    mpo_VarsView->ShowWindow(SW_HIDE);

    /* Create list control to display actions (up) */
    mpo_ListUp = new EMEN_cl_FrameList;
    mpo_ListUp->Create
        (
            WS_CHILD | LVS_OWNERDRAWFIXED | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | WS_VISIBLE | LVS_REPORT,
            CRect(0, 0, 0, 0),
            &mo_Splitter,
            mo_Splitter.IdFromRowCol(0, 0)
        );

    mpo_ListUp->SetBkColor(GetSysColor(COLOR_BTNFACE));
    mpo_ListUp->mpo_Actions = mpo_Actions;
    mpo_ListUp->mb_Up = TRUE;
    mpo_ListUp->mpo_Parent = this;

    /* Create list control to display actions (down) */
    mpo_ListDown = new EMEN_cl_FrameList;
    mpo_ListDown->Create
        (
            WS_CHILD | LVS_OWNERDRAWFIXED | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | WS_VISIBLE | LVS_REPORT,
            CRect(0, 0, 0, 0),
            &mo_Splitter,
            mo_Splitter.IdFromRowCol(1, 0)
        );

    mpo_ListDown->SetBkColor(GetSysColor(COLOR_BTNFACE));
    mpo_ListDown->mpo_Actions = mpo_Actions;
    mpo_ListDown->mb_Up = FALSE;
    mpo_ListDown->mpo_Parent = this;
    mpo_ListDown->SetParent(&mo_Splitter);

    /* Insert columns in list */
    mpo_ListUp->InsertColumn(0, "");
    mpo_ListDown->InsertColumn(0, "");

    /* Fill list with actions */
	if(mpo_Editor)
	{
		if(mpo_Editor->mst_BaseIni.YSplitMenu < 50) mpo_Editor->mst_BaseIni.YSplitMenu = 50;
		if(mpo_Editor->mst_BaseIni.YSplitMenu > 1000) mpo_Editor->mst_BaseIni.YSplitMenu = 1000;
		mo_Splitter.SetRowInfo(0, mpo_Editor->mst_BaseIni.YSplitMenu, 0);
	}
	else
	{
		mo_Splitter.SetRowInfo(0, 300, 0);
	}
    mo_Splitter.RecalcLayout();
    FillList();

	/* Selection par defaut */
	if(mpo_Editor)
	{
		EDI_cl_Action *po_Action;
		if(mpo_Editor->mst_BaseIni.ISplitMenu < 0 || mpo_Editor->mst_BaseIni.ISplitMenu > mpo_ListUp->GetItemCount())
			mpo_Editor->mst_BaseIni.ISplitMenu = 0;
		if(mpo_Editor->mst_BaseIni.ISplitMenu < mpo_ListUp->GetItemCount())
		{
			po_Action = (EDI_cl_Action *) mpo_ListUp->GetItemData(mpo_Editor->mst_BaseIni.ISplitMenu);
			if(po_Action && po_Action->mui_NumDown)  /* Validate only if they are action in categ */
			{
				mpo_ListUp->SetItemState(mpo_Editor->mst_BaseIni.ISplitMenu, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				FillList();
			}
		}
	}

    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::SaveIni(void)
{
	int a;
    if(mpo_Editor) mo_Splitter.GetRowInfo(0, mpo_Editor->mst_BaseIni.YSplitMenu, a);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnDestroy(void)
{
	SaveIni();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EMEN_cl_FrameIn::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LRESULT EMEN_cl_FrameIn::OnNcHitTest(CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetWindowRect(&o_Rect);
    o_Rect.bottom = o_Rect.top + SIZE_CAPTIONY;
    if(o_Rect.PtInRect(pt))
    {
        if(pt.x > o_Rect.right - 20) return 10000;   /* Lock button */
        return HTCAPTION;
    }

    return HTNOWHERE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    int     i_Top;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_Top = lpncsp->rgrc[0].top;
    CFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
    lpncsp->rgrc[0].top = i_Top + SIZE_CAPTIONY;
}

/*
 ===================================================================================================
    Aim:    To draw the frame caption.
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnNcPaint(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect       o_Rect, oRect1;
    CWindowDC   dc(this);
    CDC         *pDC;
    CString     mo_Title;
    CFont       *po_Font;
    CBitmap     o_Bmp, o_Bmp1;
    CDC         dcBitmap;
    UINT        ui_Res;
    COLORREF    xCol, xCol1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pDC = &dc;

    GetWindowRect(&o_Rect);
    o_Rect.OffsetRect(-o_Rect.left, -o_Rect.top);
    o_Rect.bottom = SIZE_CAPTIONY;

    /* Draw caption bar */
    xCol = xCol1 = GetSysColor(COLOR_ACTIVECAPTION);
    pDC->FillSolidRect(&o_Rect, GetSysColor(COLOR_ACTIVECAPTION));

    /* Lock/Unlock state */
    if(mb_Locked)
        ui_Res = BMP_LOCKED_MENU;
    else
        ui_Res = BMP_UNLOCKED_MENU;

    M_MF()->DrawTransparentBmp
        (
            ui_Res,
            pDC,
            o_Rect.right - 20,
            o_Rect.top,
            16,
            SIZE_CAPTIONY,
            16,
            SIZE_CAPTIONY,
            RGB(192, 192, 192)
        );

    /* Caption text */
    GetWindowText(mo_Title);
    pDC->SetBkMode(TRANSPARENT);
    po_Font = pDC->SelectObject(&M_MF()->mo_Fnt);

    oRect1 = o_Rect;
    oRect1.top += 1;
    pDC->SetTextAlign(TA_CENTER);
    pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
    pDC->ExtTextOut
        (
            oRect1.left + ((oRect1.right - oRect1.left) / 2),
            oRect1.top,
            ETO_CLIPPED,
            &oRect1,
            (char *) (LPCSTR) mo_Title,
            L_strlen((char *) (LPCSTR) mo_Title),
            0
        );
    oRect1 = o_Rect;
    oRect1.left -= 1;
    pDC->SetTextColor(0x00FFFFFF);
    pDC->ExtTextOut
        (
            oRect1.left + ((oRect1.right - oRect1.left) / 2),
            oRect1.top,
            ETO_CLIPPED,
            &oRect1,
            (char *) (LPCSTR) mo_Title,
            L_strlen((char *) (LPCSTR) mo_Title),
            0
        );

    xCol = M_MF()->u4_Interpol2PackedColor(0x00000000, xCol, 0.4f);
    xCol1 = M_MF()->u4_Interpol2PackedColor(0x00FFFFFF, xCol1, 0.5f);
    pDC->Draw3dRect(&o_Rect, xCol1, xCol);
    pDC->SelectObject(po_Font);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnSize(UINT, int cx, int cy)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LV_COLUMN   st_Column;
    CWnd        *po_Child;
    CRect       o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mpo_ListUp)
    {
        /* Item ID 5 is to retreive the list inside the frame */
        po_Child = GetDlgItem(5);
        if(po_Child)
        {
            po_Child->GetWindowRect(&o_Rect);
            ScreenToClient(&o_Rect);
            cy = o_Rect.top;
        }

        /* Move splitter */
        mo_Splitter.MoveWindow(-2, -2, cx + 4, cy + 2);
        mo_Splitter.RecalcLayout();

        /* Set columns (at least one column to display something) */
        GetParent()->GetClientRect(&o_Rect);
        st_Column.mask = LVCF_WIDTH;
        st_Column.cx = o_Rect.right - 1;
        mpo_ListUp->SetColumn(0, &st_Column);
        mpo_ListDown->SetColumn(0, &st_Column);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_Action *EMEN_cl_FrameIn::FillList(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos, ppos, pos1;
    EDI_cl_BaseFrame    *po_Editor;
    EDI_cl_ConfigList   *po_List;
    EDI_cl_Action       *po_Action, *po_Action1;
    EDI_cl_Action       *po_ActionSel, *po_LastAction;
    LV_ITEM             st_ListCtrlItem;
    int                 i, num, i_Sel;
    int                 i_FillDown;
    CString             o_String;
    CSize               o_Size;
    CDC                 *pDC;
    int                 i_Num, i_ValidNum, i_VarAct;
    BOOL                b_LastSep;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_ActionSel = NULL;

    /* Get current up list selected item */
    i_FillDown = 0;

	if (!mpo_ListUp) return NULL;
	if (!mpo_ListDown) return NULL;

	i_Sel = mpo_ListUp->GetNextItem(-1, LVNI_SELECTED);
	
	/* Delete content of lists */
	mpo_ListUp->DeleteAllItems();
	mpo_ListDown->DeleteAllItems();

    if(!mpo_Actions) return NULL;

    /* Get current selected action list */
    pos = mpo_Actions->mo_List.FindIndex(mpo_Actions->mi_CurrentList);
    if(!pos) return NULL;

    /* Delete all dynamics actions */
    po_List = mpo_Actions->mo_List.GetAt(pos);
    pos = po_List->mo_List.GetHeadPosition();
    while(pos)
    {
        ppos = pos;
        po_Action = po_List->mo_List.GetNext(pos);
        if(po_Action->mb_Dyn)
        {
            delete po_List->mo_List.GetAt(ppos);
            po_List->mo_List.RemoveAt(ppos);
        }
    }

    /*$2
     -----------------------------------------------------------------------------------------------
        Parse action list
     -----------------------------------------------------------------------------------------------
     */

    pos = mpo_Actions->mo_List.FindIndex(mpo_Actions->mi_CurrentList);
    po_List = mpo_Actions->mo_List.GetAt(pos);
    pos = po_List->mo_List.GetHeadPosition();
    i = num = 0;
    i_ValidNum = 0;
    pDC = GetDC();
    b_LastSep = FALSE;
    while(pos)
    {
        po_Action = po_List->mo_List.GetNext(pos);

        /* Name */
        st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM;
        st_ListCtrlItem.iItem = i;
        st_ListCtrlItem.iSubItem = 0;

        /* Action for an editor. */
		po_Action->mb_Disabled = FALSE;
        if(mpo_Actions->mpo_Editor)
        {
            /* Does the action validate ? */
            if(!mpo_Actions->mpo_Editor->b_OnActionValidate(po_Action->mul_Action)) 
			{
				o_String = po_Action->mo_DisplayName;
				po_Action->mb_Disabled = TRUE;
				po_Action->mui_State = -1;
			}
			else
			{
				/* Ask editor to construct string to display */
				mpo_Actions->mpo_Editor->OnActionUI
					(
						po_Action->mul_Action,
						po_Action->mo_DisplayName,
						o_String
					);

				/* Ask editor for action state */
	            po_Action->mui_State = mpo_Actions->mpo_Editor->ui_OnActionState(po_Action->mul_Action);
			}
            st_ListCtrlItem.pszText = (char *) (LPCSTR) o_String;
        }

        /* Global action (mainframe). */
        else
        {
            /* Does the action validate ? */
            if(!M_MF()->b_OnActionValidate(po_Action->mul_Action))
			{
				o_String = po_Action->mo_DisplayName;
				po_Action->mb_Disabled = TRUE;
				po_Action->mui_State = -1;
			}
			else
			{
				/* Ask main frame to construct string to display */
				M_MF()->OnActionUI(po_Action->mul_Action, po_Action->mo_DisplayName, o_String);

				/* Ask mainframe for action state */
				po_Action->mui_State = M_MF()->ui_OnActionState(po_Action->mul_Action);
			}

            st_ListCtrlItem.pszText = (char *) (LPCSTR) o_String;
        }

        /* Reached another category ? */
        if(((int) po_Action->mul_Action < 0) && i_FillDown) i_FillDown = 2;

        /* The text itself */
        st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
        st_ListCtrlItem.lParam = (ULONG) po_Action;
        if((int) po_Action->mul_Action < 0)
        {
            /* To count number of valid action in categ */
            po_LastAction = po_Action;
            po_LastAction->mui_NumDown = 0;

            if(num == i_Sel)
            {
                st_ListCtrlItem.mask |= LVIF_STATE;
                st_ListCtrlItem.state = LVIS_SELECTED;
                i_FillDown = 1;
                po_ActionSel = po_Action;
            }

            /* Request for varview or normal menu */
            po_Editor = mpo_Actions->mpo_Editor;
            if
            (
                ((po_Editor && po_Editor->i_IsItVarAction(po_Action->mul_Action, NULL))) ||
                (!po_Editor && M_MF()->i_IsItVarAction(po_Action->mul_Action, NULL))
            )
            {
                po_LastAction->mui_NumDown++;   /* Simulate an action count */
                if(num == i_Sel)
                {
                    if(po_Editor)
                        i_VarAct = po_Editor->i_IsItVarAction(po_Action->mul_Action, mpo_VarsView);
                    else
                        i_VarAct = M_MF()->i_IsItVarAction(po_Action->mul_Action, mpo_VarsView);
                    if(i_VarAct == 1)
                        mpo_VarsView->mpo_ListBox->SetItemList(&mo_ListItems);
                    mpo_VarsView->SetParent(&mo_Splitter);
                    mpo_VarsView->Invalidate();
                    mpo_VarsView->ShowWindow(SW_SHOW);
                    mpo_ListDown->ShowWindow(SW_HIDE);
                    mo_Splitter.RecalcLayout();
                }
            }
            else if(num == i_Sel)
            {
                mpo_VarsView->ShowWindow(SW_HIDE);
                mpo_ListDown->SetParent(&mo_Splitter);
                mpo_ListDown->ShowWindow(SW_SHOW);
                mo_Splitter.RecalcLayout();
            }

            num++;
            mpo_ListUp->InsertItem(&st_ListCtrlItem);

            /* Restore state (inhibit it) */
            st_ListCtrlItem.mask &= ~LVIF_STATE;
        }

        /* Count action */
        if((int) po_Action->mul_Action > 0)
            po_LastAction->mui_NumDown++;

        /* Action */
        if((i_FillDown == 1) && ((int) po_Action->mul_Action >= 0))
        {
            if((int) po_Action->mul_Action > 0) i_ValidNum++;
            if(i_ValidNum)
            {
                mpo_ListDown->InsertItem(&st_ListCtrlItem);

                if(po_Action->mul_Action == 0)
                    b_LastSep = TRUE;
                else
                    b_LastSep = FALSE;
            }
        }

        /* Count dynamic actions */
        pos1 = po_List->mo_List.Find(po_LastAction);
        if(mpo_Actions->mpo_Editor)
            i_Num = mpo_Actions->mpo_Editor->ui_ActionFillDynamic(po_List, pos1);
        else
            i_Num = M_MF()->ui_ActionFillDynamic(po_List, pos1);
        po_LastAction->mui_NumDown += i_Num;

        i++;
    }

    /*$2
     -----------------------------------------------------------------------------------------------
        Dynamic actions ?
     -----------------------------------------------------------------------------------------------
     */

    if(po_ActionSel)
    {
        pos = po_List->mo_List.Find(po_ActionSel);
        if(mpo_Actions->mpo_Editor)
            i_Num = mpo_Actions->mpo_Editor->ui_ActionFillDynamic(po_List, pos);
        else
            i_Num = M_MF()->ui_ActionFillDynamic(po_List, pos);

        if(i_Num)
        {
            i_ValidNum += i_Num;

            po_List->mo_List.GetNext(pos);
            i = mpo_ListDown->GetItemCount();
            while(i_Num)
            {
                po_Action = po_List->mo_List.GetNext(pos);

                st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM;
                st_ListCtrlItem.iItem = i++;
                st_ListCtrlItem.iSubItem = 0;
                st_ListCtrlItem.pszText = (char *) (LPCSTR) po_Action->mo_DisplayName;
                st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
                st_ListCtrlItem.lParam = (ULONG) po_Action;
                mpo_ListDown->InsertItem(&st_ListCtrlItem);
                if(po_Action->mul_Action == 0)
                    b_LastSep = TRUE;
                else
                    b_LastSep = FALSE;

                i_Num--;
            }
        }
    }

    /* Delete unused sep */
    for(i = 0; i < mpo_ListDown->GetItemCount(); i++)
    {
        po_Action = (EDI_cl_Action *) mpo_ListDown->GetItemData(i);
        if(po_Action->mul_Action == 0)
        {
            if(i < mpo_ListDown->GetItemCount() - 1)
            {
                po_Action1 = (EDI_cl_Action *) mpo_ListDown->GetItemData(i + 1);
                if(po_Action1->mul_Action == 0)
                {
                    mpo_ListDown->DeleteItem(i);
                    i--;
                    continue;
                }
            }
            else if(po_Action->mul_Action == 0)
                mpo_ListDown->DeleteItem(i);
        }
    }

    ReleaseDC(pDC);

    /* If there's no valid item in down list (for example only separators), delete all */
    if(!i_ValidNum)
        mpo_ListDown->DeleteAllItems();

    /* Invalidate now */
    mpo_ListUp->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
    mpo_ListDown->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
	return po_ActionSel;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Lock/Unlock menu */
    if(nHitTest == 10000)
    {
        mb_Locked = mb_Locked ? FALSE : TRUE;
        OnNcPaint();

        if(!mb_Locked) return;
    }

    /* Validate menu */
    mpo_Parent->NcClick(this);

    /* Begin drag & drop */
    EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
    EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
    EDI_gst_DragDrop.i_Param1 = (int) this;
    EDI_gst_DragDrop.i_Param2 = (int) mpo_Parent;

    ScreenToClient(&point);
    M_MF()->BeginDragDrop(point, this, mpo_Parent);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnRButtonDown(UINT nHitTest, CPoint point)
{
    M_MF()->CancelDragDrop();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnLButtonUp(UINT nHitTest, CPoint point)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_cl_BaseFrame    *po_Ed;
    BOOL                b_Can;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Can = M_MF()->b_MoveDragDrop(point);
    M_MF()->EndDragDrop(point);

    /* Is it a drag&drop in another editor (not in a menu) */
    if(b_Can && mb_MyCanDD)
    {
        po_Ed = EDI_gst_DragDrop.po_DestEditor;
        CreateTopMenu(po_Ed);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_FrameIn::OnMouseMove(UINT nHitTest, CPoint point)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_cl_BaseFrame    *po_Ed;
    BOOL                b_Can;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Can = M_MF()->b_MoveDragDrop(point);

    /*
     * Force drag&drop to be forbidden, cause the real process of end of drag&drop will be
     * special and will be treat in lbuttonup of that class. Determin the cursor shape.
     */
    mb_MyCanDD = FALSE;
    if(b_Can)
    {
        EDI_gst_DragDrop.b_CanDragDrop = FALSE;
        po_Ed = EDI_gst_DragDrop.po_DestEditor;
        if(po_Ed)
        {
            /* Move a menu to the same editor type (normal d&d) */
            if
            (
                (EDI_gst_DragDrop.po_SourceEditor->mi_PosInGArray == po_Ed->mi_PosInGArray) &&
                (EDI_gst_DragDrop.po_SourceEditor != po_Ed)
            )
            {
                ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
                mb_MyCanDD = TRUE;
            }

            /* To create a top menu in dest editor */
            else if((po_Ed->mpo_Actions == mpo_Actions) && (!po_Ed->mpo_Menu))
            {
                ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
                mb_MyCanDD = TRUE;
            }
            else
            {
                ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
                mb_MyCanDD = FALSE;
            }
        }
        else if((!M_MF()->mpo_Menu) && !mpo_Editor)
        {
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
            mb_MyCanDD = TRUE;
        }
        else
        {
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
            mb_MyCanDD = FALSE;
        }
    }
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Call to create the menu that will appeared at the top of the editor. This function will
            be called at init or when we d&d a valid menu in the editor.
 ===================================================================================================
 */
void EMEN_cl_FrameIn::CreateTopMenu(EDI_cl_BaseFrame *_po_Ed, BOOL _b_Refresh)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_Menu    *po_Menu;
    TBBUTTON        b;
    CRect           o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Menu = new EMEN_cl_Menu;
    po_Menu->mpo_MenuFrame = this;

    /*$S
     * Init actions and toolbar info (toolbar in top menu)
     */
    if(_po_Ed)
    {
        po_Menu->mpo_Actions = _po_Ed->mpo_Actions;

        /* -1 => No toolbar by default (if no resource is present) */
        po_Menu->mi_PosXInMenu = -1;
        if(_po_Ed->mst_Def.ui_ResToolBar)
        {
            po_Menu->mui_ResToolBar = _po_Ed->mst_Def.ui_ResToolBar;
            po_Menu->mi_PosXInMenu = _po_Ed->mst_BaseIni.i_TopInMenu;
			po_Menu->mb_LeftMode = _po_Ed->mst_BaseIni.mb_LeftMode;
        }
    }
    else
    {
        po_Menu->mpo_Actions = M_MF()->mpo_Actions;
        po_Menu->mui_ResToolBar = EDI_IDR_TOOLBAR;
        po_Menu->mi_PosXInMenu = M_MF()->mst_Ini.i_TopInMenu;
		po_Menu->mb_LeftMode = FALSE;
    }

    po_Menu->mpo_Editor = _po_Ed;

    /* For an editor, simply create a frame that will be docked (with hands...) at the top. */
    if(_po_Ed)
    {
        po_Menu->Create
            (
                NULL,
                NULL,
                WS_CLIPCHILDREN | WS_CHILD,
                CRect(0, 0, 100, 50),
                _po_Ed->GetParent()
            );
        po_Menu->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		po_Menu->Invalidate();
        _po_Ed->mpo_Menu = po_Menu;
    }

    /*
     * For main frame it's a bit complicated. We must create a dummy toolbar, and create the menu
     * as child of that toolbar
     */
    else
    {
        M_MF()->mpo_MenuBar = new CToolBar;
        M_MF()->mpo_MenuBar->Create(M_MF());
        M_MF()->mpo_MenuBar->ModifyStyle(0, CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM);

        /*
         * To be sure that the toolbar will be the first of the list of all controlbars already
         * present in mainframe. So that toolbar will always be shown on top of client area.
         */
        M_MF()->mpo_MenuBar->BringWindowToTop();

        /* Add a dummy button to be sure that toolbar will have a good height */
        b.iBitmap = 0;
        b.dwData = 0;
        b.iString = NULL;
        b.idCommand = 0;
        b.fsState = 0;
        b.fsStyle = TBSTYLE_BUTTON;
        M_MF()->mpo_MenuBar->GetToolBarCtrl().InsertButton(0, &b);
        M_MF()->mpo_MenuBar->SetHeight(26);

        /* Create the menu as child of the toolbar */
        po_Menu->Create
            (
                NULL,
                NULL,
                WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD,
                CRect(0, 0, 500, 60),
                M_MF()->mpo_MenuBar
            );
        po_Menu->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
        M_MF()->mpo_Menu = po_Menu;
    }

    if(_po_Ed)
        _po_Ed->mst_BaseIni.b_TopMenu = TRUE;
    else
        M_MF()->mst_Ini.b_TopMenu = TRUE;

    /* Force repaint frame, toolbar and menu */
    if(_b_Refresh)
    {
        if(_po_Ed)
        {
			M_MF()->LockDisplay(_po_Ed->mpo_MyView);
            _po_Ed->mpo_MyView->OnChangePane(NULL, NULL);
			ENG_gb_GlobalLock = TRUE;
			_po_Ed->mpo_MyView->GetWindowRect(&o_Rect);
			_po_Ed->mpo_MyView->GetParent()->ScreenToClient(&o_Rect);
			o_Rect.right--;
			_po_Ed->mpo_MyView->MoveWindow(&o_Rect, FALSE);
			o_Rect.right++;
			ENG_gb_GlobalLock = FALSE;
			_po_Ed->mpo_MyView->MoveWindow(&o_Rect);
			M_MF()->UnlockDisplay(_po_Ed->mpo_MyView);
        }
        else
        {
            M_MF()->mpo_Menu->Invalidate();
            M_MF()->GetClientRect(&o_Rect);
            M_MF()->OnSize(0, o_Rect.right, o_Rect.bottom);
            M_MF()->RecalcLayout();
        }
    }
}

/*
 ===================================================================================================
    Aim:    Called to destroy the top menu of the editor (or mainframe).
 ===================================================================================================
 */
void EMEN_cl_FrameIn::DestroyTopMenu(EDI_cl_BaseFrame *_po_Ed)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_po_Ed)
    {
        /* Save position of menu */
        if(_po_Ed->mpo_Menu)
        {
            if(_po_Ed->mpo_Menu->mpo_InMenu)
            {
                _po_Ed->mpo_Menu->mpo_InMenu->GetWindowRect(&o_Rect);
                _po_Ed->mpo_Menu->mpo_InMenu->ScreenToClient(&o_Rect);
                _po_Ed->mst_BaseIni.i_TopInMenu = o_Rect.right - o_Rect.left + 1;
            }

            /* Destroy menu */
			_po_Ed->mpo_Menu->ShowWindow(SW_HIDE);
            _po_Ed->mpo_Menu->DestroyWindow();
            _po_Ed->mpo_Menu = NULL;
        }

        _po_Ed->mst_BaseIni.b_TopMenu = FALSE;

        /* Force repaint editor */
		M_MF()->LockDisplay(_po_Ed->mpo_MyView);
        _po_Ed->mpo_MyView->OnChangePane(NULL, NULL);
		ENG_gb_GlobalLock = TRUE;
		_po_Ed->mpo_MyView->GetWindowRect(&o_Rect);
		_po_Ed->mpo_MyView->GetParent()->ScreenToClient(&o_Rect);
		o_Rect.right--;
		_po_Ed->mpo_MyView->MoveWindow(&o_Rect, FALSE);
		o_Rect.right++;
		ENG_gb_GlobalLock = FALSE;
		_po_Ed->mpo_MyView->MoveWindow(&o_Rect);
		M_MF()->UnlockDisplay(_po_Ed->mpo_MyView);
    }
    else
    {
        /* Save position of menu */
        if(M_MF()->mpo_Menu->mpo_InMenu)
        {
            M_MF()->mpo_Menu->mpo_InMenu->GetWindowRect(&o_Rect);
            M_MF()->mpo_Menu->mpo_InMenu->ScreenToClient(&o_Rect);
            M_MF()->mst_Ini.i_TopInMenu = o_Rect.right - o_Rect.left + 1;
        }

        /* Destroy menu */
        M_MF()->mpo_Menu->DestroyWindow();
        M_MF()->mpo_Menu = NULL;
        M_MF()->mst_Ini.b_TopMenu = FALSE;
        delete M_MF()->mpo_MenuBar;
        M_MF()->mpo_MenuBar = NULL;

        /* Force repaint mainframe */
        M_MF()->GetClientRect(&o_Rect);
        M_MF()->OnSize(0, o_Rect.right, o_Rect.bottom);
        M_MF()->RecalcLayout();
    }
}

#endif
