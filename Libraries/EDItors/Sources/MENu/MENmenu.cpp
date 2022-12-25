/*$T MENmenu.cpp GC 1.134 04/23/04 14:00:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Aim: The top menu that can be docked in main frame or in each editor. This menu
 * is the reflect of the action menu in MEN editor.
 */
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "EDIerrid.h"
#include "EDIaction.h"
#include "EDIapp.h"
#include "MENmenu.h"
#include "EDImainframe.h"
#include "MENin.h"
#include "MENlist.h"
#include "MENsubmenu.h"
#include "MENinmenu.h"
#include "ENGine/Sources/ENGmsg.h"
#include "Res/Res.h"
#include "DIAlogs/DIAchecklist_dlg.h"
#include "BIGfiles/BIGfat.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define POSBUT	14
extern BOOL EDI_gb_HasClosed;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EMEN_cl_Menu, CFrameWnd)
BEGIN_MESSAGE_MAP(EMEN_cl_Menu, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EMEN_cl_Menu::EMEN_cl_Menu(void)
{
	mi_Sel = -1;
	mi_LastSel = -1;
	mb_SelOK = FALSE;
	mpo_InMenu = NULL;
	mi_PosXInMenu = 50;
	mi_MinPosInMenu = 0;
	mui_ResToolBar = -1;
	mb_LeftMode = FALSE;
	mb_DragMode = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EMEN_cl_Menu::~EMEN_cl_Menu(void)
{
	mpo_MenuFrame = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EMEN_cl_Menu::OnCreate(LPCREATESTRUCT lpcs)
{
	CFrameWnd::OnCreate(lpcs);

	/* Create inside menu */
	if(mi_PosXInMenu != -1)
	{
		mpo_InMenu = new EMEN_cl_InMenu(mui_ResToolBar);
		mpo_InMenu->mpo_Parent = this;
		mpo_InMenu->Create
			(
				NULL,
				NULL,
				WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE | WS_THICKFRAME,
				CRect(0, 0, 100, 100),
				this
			);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_Menu::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	CSize	oSize;
	int		x, cxm;
	/*~~~~~~~~~~~*/

	CFrameWnd::OnSize(nType, cx, cy);

	/* Move inmenu frame */
	if(mpo_InMenu)
	{
		if(!mb_LeftMode)
		{
			oSize.cy = GetSystemMetrics(SM_CYSIZEFRAME) + 1;
			GetClientRect(&o_Rect);

			/* Born X */
			if(o_Rect.right > o_Rect.left)
			{
				x = o_Rect.right - mi_PosXInMenu - POSBUT - 14;
				cxm = mi_PosXInMenu;
				if(x < mi_MinPosInMenu)
				{
					cxm -= (mi_MinPosInMenu - x);
					x = mi_MinPosInMenu;
				}

				if(cxm <= 0)
					mpo_InMenu->ShowWindow(SW_HIDE);
				else if(!mpo_InMenu->IsWindowVisible())
					mpo_InMenu->ShowWindow(SW_SHOW);

				mpo_InMenu->mo_Toolbar.SetBarStyle(CBRS_ALIGN_TOP);

				mpo_InMenu->MoveWindow(x, -oSize.cy - 1, cxm, o_Rect.bottom + (2 * oSize.cy) + 5);
			}
		}
		else
		{
			oSize.cx = GetSystemMetrics(SM_CXSIZEFRAME);
			GetClientRect(&o_Rect);

			/* Born Y */
			if(o_Rect.bottom > o_Rect.top)
			{
				x = o_Rect.bottom - mi_PosXInMenu - POSBUT - 14;
				cxm = mi_PosXInMenu;
				if(x < mi_MinPosInMenu)
				{
					cxm -= (mi_MinPosInMenu - x);
					x = mi_MinPosInMenu;
				}

				if(cxm <= 0)
					mpo_InMenu->ShowWindow(SW_HIDE);
				else if(!mpo_InMenu->IsWindowVisible())
					mpo_InMenu->ShowWindow(SW_SHOW);

				mpo_InMenu->mo_Toolbar.SetBarStyle(CBRS_ALIGN_LEFT);
				mpo_InMenu->MoveWindow(-oSize.cx - 1, x, o_Rect.right + (2 * oSize.cx) + 1, cxm);
			}
		}
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim: Will erase the menu background, but will display menu categories too...
 =======================================================================================================================
 */
int EMEN_cl_Menu::i_GetDrawSel(CDC *pDC, BOOL _b_Update, CRect &o_SelRect)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos, pos1;
	EDI_cl_ConfigList	*po_List;
	EDI_cl_Action		*po_Action;
	EDI_cl_Action		*po_Action1;
	int					x, i;
	CSize				o_Size;
	CRect				o_Clip, o_Rect, o_NewRect, o_TmpRect, o_Rect4, o_NewRect1;
	CPoint				pt;
	BOOL				b_First;
	BOOL				b_CanDraw;
	int					i_NewSel;
	CFont				*po_OldFnt;
	int					cxm;
	BOOL				b_InSel;
	COLORREF			colmen;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Erase background */
	GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	if(_b_Update)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		TRIVERTEX		vert[2];
		GRADIENT_RECT	mesh;
		COLORREF		col1, col2;
		CWnd			*po_Wnd;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!mpo_Editor)
		{
			pDC->FillSolidRect(o_Rect, GetSysColor(COLOR_BTNFACE));
		}
		else
		{
			po_Wnd = GetFocus();
			while((po_Wnd) && (po_Wnd != GetParent())) po_Wnd = po_Wnd->GetParent();

			if(po_Wnd)
			{
				col1 = GetSysColor(COLOR_ACTIVECAPTION);
				col2 = GetSysColor(COLOR_ACTIVECAPTION);
			}
			else
			{
				col1 = GetSysColor(COLOR_3DSHADOW);
				col2 = GetSysColor(COLOR_BTNFACE);
			}

			vert[0].x = o_Rect.left;
			vert[0].y = o_Rect.top;
			vert[0].Alpha = 0;
			vert[0].Red = (COLOR16) ((col1 & 0xFF) >> 0) << 8;
			vert[0].Green = (COLOR16) ((col1 & 0xFF00) >> 8) << 8;
			vert[0].Blue = (COLOR16) ((col1 & 0xFF0000) >> 16) << 8;
			vert[1].x = o_Rect.right;
			vert[1].y = o_Rect.bottom;
			vert[1].Alpha = 0;
			vert[1].Red = (COLOR16) ((col2 & 0xFF) >> 0) << 8;
			vert[1].Green = (COLOR16) ((col2 & 0xFF00) >> 8) << 8;
			vert[1].Blue = (COLOR16) ((col2 & 0xFF0000) >> 16) << 8;
			mesh.UpperLeft = 0;
			mesh.LowerRight = 1;
			pDC->GradientFill(vert, 2, &mesh, 1, mb_LeftMode ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
		}
	}

	pos = mpo_Actions->mo_List.FindIndex(mpo_Actions->mi_CurrentList);
	if(!pos) return TRUE;
	po_List = mpo_Actions->mo_List.GetAt(pos);
	pos = po_List->mo_List.GetHeadPosition();

	if(!mb_LeftMode)
		po_OldFnt = pDC->SelectObject(&M_MF()->mo_Fnt);
	else
	{
		po_OldFnt = pDC->SelectObject(&M_MF()->mo_Fnt1);
		pDC->SetTextAlign(TA_LEFT);
	}

	/* No current selection */
	i_NewSel = -1;
	i = 0;

	/* Draw menu */
	pDC->SetBkMode(TRANSPARENT);
	GetClientRect(&o_Rect);

	o_Clip = o_Rect;
	if(!mb_LeftMode)
		o_Clip.right -= (POSBUT + 4);
	else
		o_Clip.bottom -= (POSBUT + 4);

	if(!mb_LeftMode)
	{
		x = o_Rect.left + 14;
		o_Rect.top += 2;
	}
	else
	{
		x = o_Rect.top + 14;
		o_Rect.left += 2;
	}

	b_First = TRUE;
	while(pos)
	{
		po_Action = po_List->mo_List.GetNext(pos);

		if((int) po_Action->mul_Action < 0)
		{
			/* Is it a var menu ? If yes, don't display it */
			b_CanDraw = TRUE;

			/* Get text width, and display it */
			if(b_CanDraw)
			{
				o_Size = pDC->GetTextExtent(po_Action->mo_DisplayName);

				/* Categ can be selected */
				if(po_Action->mui_NumDown)
				{
					if(_b_Update)
					{
						pDC->SetTextColor(mpo_Editor ? 0x00FFFFFF : 0);
						if(!mb_LeftMode)
						{
							pDC->ExtTextOut(x, o_Rect.top, ETO_CLIPPED, o_Clip, po_Action->mo_DisplayName, NULL);
						}
						else
						{
							pDC->ExtTextOut
								(
									o_Rect.left,
									x + o_Size.cx,
									ETO_CLIPPED,
									o_Clip,
									po_Action->mo_DisplayName,
									NULL
								);
						}
					}
				}

				/* Categ can't be selected */
				else if(_b_Update)
				{
					pDC->SetTextColor(GetSysColor(COLOR_3DSHADOW));
					if(!mb_LeftMode)
					{
						pDC->ExtTextOut(x + 1, o_Rect.top, ETO_CLIPPED, o_Clip, po_Action->mo_DisplayName, NULL);
					}
					else
					{
						pDC->ExtTextOut
							(
								o_Rect.left,
								x + o_Size.cx,
								ETO_CLIPPED,
								o_Clip,
								po_Action->mo_DisplayName,
								NULL
							);
					}

					pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				}

				if(!mb_LeftMode)
					o_NewRect = CRect(x, o_Rect.top, x + o_Size.cx, o_Rect.bottom);
				else
					o_NewRect = CRect(o_Rect.left, x, o_Rect.right, x + o_Size.cx);
			}

			/* Is there another category after that one ? */
			pos1 = pos;
			while(pos1)
			{
				po_Action1 = po_List->mo_List.GetAt(pos1);
				if((int) po_Action1->mul_Action < 0) break;
				po_List->mo_List.GetNext(pos1);
			}

			if(b_CanDraw)
			{
				b_First = FALSE;
				x += o_Size.cx + 12;

				/* Is it a selected menu ? */
				GetCursorPos(&pt);
				b_InSel = FALSE;
				if(WindowFromPoint(pt) == this) b_InSel = TRUE;
				ScreenToClient(&pt);
				o_NewRect1 = o_NewRect;
				if(mb_LeftMode)
				{
					o_NewRect1.left -= 5;
					o_NewRect1.right += 10;
					o_NewRect1.top -= 4;
					o_NewRect1.bottom += 4;
				}
				else
				{
					o_NewRect1.top -= 5;
					o_NewRect1.bottom += 10;
					o_NewRect1.left -= 4;
					o_NewRect1.right += 4;
				}

				if(po_Action->mui_NumDown && o_NewRect1.PtInRect(pt) && b_InSel)
				{
					colmen = mpo_Editor ? GetSysColor(COLOR_3DHILIGHT) : 0;
					o_TmpRect = o_NewRect;
					if(!mb_LeftMode)
					{
						o_TmpRect.left -= 3;
						o_TmpRect.right += 3;
						o_TmpRect.bottom = o_TmpRect.top + 16;
						if(o_TmpRect.right > o_Clip.right) o_TmpRect.right = o_Clip.right;
						if(o_TmpRect.left < o_Clip.right)
						{
							if(_b_Update)
							{
								if(mb_SelOK)
									pDC->Draw3dRect(o_TmpRect, colmen, colmen);
								else
									pDC->Draw3dRect(o_TmpRect, colmen, colmen);
							}

							i_NewSel = i;
							o_SelRect = o_TmpRect;
						}
					}
					else
					{
						o_TmpRect.top -= 3;
						o_TmpRect.bottom += 3;
						o_TmpRect.right = o_TmpRect.left + 16;
						if(o_TmpRect.bottom > o_Clip.bottom) o_TmpRect.bottom = o_Clip.bottom;
						if(o_TmpRect.top < o_Clip.bottom)
						{
							if(_b_Update)
							{
								if(mb_SelOK)
									pDC->Draw3dRect(o_TmpRect, colmen, colmen);
								else
									pDC->Draw3dRect(o_TmpRect, colmen, colmen);
							}

							i_NewSel = i;
							o_SelRect = o_TmpRect;
						}
					}
				}

				i++;
			}
		}
	}

	/* Born toolbar pos */
	GetClientRect(&o_Rect);
	if(!mb_LeftMode)
	{
		if(mpo_InMenu && o_Rect.right > o_Rect.left)
		{
			x = o_Rect.right - mi_PosXInMenu - POSBUT - 14;
			cxm = mi_PosXInMenu;
			if(x < mi_MinPosInMenu)
			{
				cxm -= (mi_MinPosInMenu - x);
			}

			if(cxm <= 0)
				mpo_InMenu->ShowWindow(SW_HIDE);
			else if(!mpo_InMenu->IsWindowVisible())
				mpo_InMenu->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if(mpo_InMenu && o_Rect.bottom > o_Rect.top)
		{
			x = o_Rect.bottom - mi_PosXInMenu - POSBUT - 14;
			cxm = mi_PosXInMenu;
			if(x < mi_MinPosInMenu)
			{
				cxm -= (mi_MinPosInMenu - x);
			}

			if(cxm <= 0)
				mpo_InMenu->ShowWindow(SW_HIDE);
			else if(!mpo_InMenu->IsWindowVisible())
				mpo_InMenu->ShowWindow(SW_SHOW);
		}
	}

	pDC->SelectObject(po_OldFnt);
	if(i_NewSel == -1) mi_LastSel = -1;
	return i_NewSel;
}

/*
 =======================================================================================================================
    Aim: Will erase the menu background, but will display menu categories too...
 =======================================================================================================================
 */
BOOL EMEN_cl_Menu::OnEraseBkgnd(CDC *pDC)
{
	mi_Sel = i_GetDrawSel(pDC, TRUE, mo_SelRect);
	return TRUE;
}

/*
 =======================================================================================================================
    Aim: Set capture to the top menu, cause if mouse goes outside, we want the "selected" box to be hide (and without
    capture, we don't receive the message if mouse is outside client are).
 =======================================================================================================================
 */
void EMEN_cl_Menu::OnMouseMove(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect, o_SelRect;
	CPoint	pt1;
	int		iNewSel;
	CDC		*pDC;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_DragMode)
	{
		if(!mo_DragRect.PtInRect(pt))
		{
			mo_DragRect.left = mo_DragRect.top = mo_DragRect.right = mo_DragRect.bottom = 0;
			ClientToScreen(&pt);
			mpo_Editor->ScreenToClient(&pt);
			mpo_Editor->GetClientRect(&o_Rect);
			if(mb_LeftMode && (pt.y < o_Rect.top + 20) && (pt.y > o_Rect.top))
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
			else if(!mb_LeftMode && (pt.x < o_Rect.left + 20) && (pt.x > o_Rect.left))
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
			else
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
		}

		return;
	}

	/* Invalidate menu to force drawing selected box under mouse */
	pDC = GetWindowDC();
	iNewSel = i_GetDrawSel(pDC, FALSE, o_SelRect);
	if(iNewSel != mi_Sel)
	{
		InvalidateRect(mo_SelRect);
		InvalidateRect(o_SelRect);
	}

	ReleaseDC(pDC);

	/*
	 * Capture mouse, except if mouse is outside (but with the invalidate above, this
	 * rect will be hidden
	 */
	GetClientRect(&o_Rect);
	SetCapture();
	if(!o_Rect.PtInRect(pt))
	{
		mi_LastSel = -1;
		ReleaseCapture();
	}

	/*
	 * Release if cursor is in list rect, but on a different window (floating toolbar
	 * for example)
	 */
	pt1 = pt;
	ClientToScreen(&pt1);
	if(WindowFromPoint(pt1) != this) ReleaseCapture();
}

/*
 =======================================================================================================================
    Aim: Left button down on a menu to display a popup one, or on close button to destory top menu.
 =======================================================================================================================
 */
void EMEN_cl_Menu::OnLButtonDblClk(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Top fill view
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mpo_Editor)
	{
		mpo_Editor->mpo_MyView->mb_TabFill = mpo_Editor->mpo_MyView->mb_TabFill ? FALSE : TRUE;
		mpo_Editor->mpo_MyView->GetWindowRect(&o_Rect);
		mpo_Editor->mpo_MyView->GetParent()->ScreenToClient(&o_Rect);
		ENG_gb_GlobalLock = TRUE;
		o_Rect.right--;
		mpo_Editor->mpo_MyView->MoveWindow(&o_Rect, FALSE);
		o_Rect.right++;
		ENG_gb_GlobalLock = FALSE;
		mpo_Editor->mpo_MyView->MoveWindow(&o_Rect);
	}
}

/*
 =======================================================================================================================
    Aim: Left button down on a menu to display a popup one, or on close button to destory top menu.
 =======================================================================================================================
 */
void EMEN_cl_Menu::OnLButtonDown(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect			o_Rect;
	EMEN_cl_SubMenu o_Popup(TRUE);
	int				i_Item, i_Sel, i_LastSel;
	EDI_cl_Action	*po_Action;
	ULONG			ul_Action;
	BOOL			b_LastIsSep, b_FirstSep;
	CDC				*pDC;
	CWnd			*po_Wnd;
	EDI_cl_Action	*pSel;
	MSG				msg;
	static int		i_JustAfter = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Cursor on a menu ? We first select the corresponding category in left menu, so that we get the valid actions. The
	    we user mpo_ListDown to retreive all actions and display them in a popup menu.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	po_Wnd = GetFocus();
	if((mi_Sel != -1) && (mpo_MenuFrame) && (mi_Sel != mi_LastSel))
	{
		/* Remember last selection */
		mi_LastSel = mi_Sel;

		/* Select the corresponding item, and fill the list */
		M_MF()->LockDisplay(mpo_MenuFrame);
		i_LastSel = mpo_MenuFrame->mpo_ListUp->GetNextItem(-1, LVIS_SELECTED);
		mpo_MenuFrame->mpo_ListUp->SetItemState(mi_Sel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		pSel = mpo_MenuFrame->FillList();

		/* Is it a var view ? */
		if(pSel)
		{
			if
			(
				(
					(
						mpo_MenuFrame->mpo_Actions->mpo_Editor
					&&	(mpo_Actions->mpo_Editor->i_IsItVarAction(pSel->mul_Action, NULL))
					)
				)
			||	(!mpo_MenuFrame->mpo_Actions->mpo_Editor && (M_MF()->i_IsItVarAction(pSel->mul_Action, NULL)))
			)
			{
				if(GetMessageExtraInfo() == 1)
				{
					mi_Sel = mi_LastSel = -1;
					Invalidate();
					if(mpo_MenuFrame) M_MF()->UnlockDisplay(mpo_MenuFrame);
					return;
				}
				else
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					EDIA_cl_CheckListDialog *po_Dialog;
					int						i_VarAct;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					po_Dialog = new EDIA_cl_CheckListDialog
						(
							"Options",
							CPoint(0, 0),
							mpo_MenuFrame->mpo_Actions->mpo_Editor
						);
					if(mpo_MenuFrame->mpo_Actions->mpo_Editor)
					{
						i_VarAct = mpo_MenuFrame->mpo_Actions->mpo_Editor->i_IsItVarAction
							(
								pSel->mul_Action,
								po_Dialog->mpo_VarsView
							);
					}
					else
						i_VarAct = M_MF()->i_IsItVarAction(pSel->mul_Action, po_Dialog->mpo_VarsView);
					Invalidate();
					po_Dialog->DoModeless();
					if(mpo_MenuFrame) M_MF()->UnlockDisplay(mpo_MenuFrame);
					return;
				}
			}
		}

		/* Create popup menu */
		EDI_go_TheApp.SetFocusPaneColor();
		o_Popup.CreatePopupMenu();
		o_Popup.mpo_MenuFrame = mpo_MenuFrame;

		/* Construct menu */
		b_LastIsSep = FALSE;
		b_FirstSep = TRUE;
		for(i_Item = 0; i_Item < mpo_MenuFrame->mpo_ListDown->GetItemCount(); i_Item++)
		{
			UINT flags;
			po_Action = (EDI_cl_Action *) mpo_MenuFrame->mpo_ListDown->GetItemData(i_Item);
			if((po_Action->mul_Action <= 0) && (b_FirstSep)) continue;
			b_FirstSep = FALSE;
			if(po_Action->mb_Disabled)
				flags = MF_OWNERDRAW | MF_BYPOSITION | MF_DISABLED;
			else
				flags = MF_OWNERDRAW | MF_BYPOSITION;
			o_Popup.InsertMenu(i_Item + 1, flags, i_Item + 1, (char *) po_Action);
			if(po_Action->mul_Action <= 0)
				b_LastIsSep = TRUE;
			else
				b_LastIsSep = FALSE;
		}

		/* Exit if no action associated with the selected category */
		i_Sel = -1;
		if(i_Item)
		{
			/* Delete a dummy end separator */
			if(b_LastIsSep) o_Popup.DeleteMenu(o_Popup.GetMenuItemCount() - 1, MF_BYPOSITION);

			/* Display menu */
			if(!mb_LeftMode)
			{
				pt.x = mo_SelRect.left;
				pt.y = mo_SelRect.bottom;
			}
			else
			{
				pt.x = mo_SelRect.right;
				pt.y = mo_SelRect.top;
			}

			ClientToScreen(&pt);

			EDI_gb_LockKeyboard = TRUE;
			mb_SelOK = TRUE;
			Invalidate();

			EDI_gb_TopMenuMode = TRUE;
			EDI_gh_TopMenuMode = m_hWnd;
			i_Sel = o_Popup.TrackPopupMenu
				(
					TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
					pt.x,
					pt.y,
					this
				);

			while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				switch(msg.message)
				{
				case WM_LBUTTONDOWN:
					goto zap;
				default:
					PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					break;
				}
			}

zap:
			i_JustAfter = 1;
			EDI_gb_TopMenuMode = FALSE;

			mb_SelOK = FALSE;
			EDI_gb_LockKeyboard = FALSE;
		}

		if((short) i_Sel > 0)
		{
			po_Action = ((EDI_cl_Action *) mpo_MenuFrame->mpo_ListDown->GetItemData(i_Sel - 1));
			ul_Action = po_Action->mul_Action;
		}

		/* Restore initial selected category */
		if(mpo_MenuFrame)
		{
			mpo_MenuFrame->mpo_ListUp->SetItemState(mi_Sel, 0, LVIS_FOCUSED | LVIS_SELECTED);
			mpo_MenuFrame->mpo_ListUp->SetItemState
				(
					i_LastSel,
					LVIS_FOCUSED | LVIS_SELECTED,
					LVIS_FOCUSED | LVIS_SELECTED
				);
			mpo_MenuFrame->FillList();
			M_MF()->UnlockDisplay(mpo_MenuFrame);
		}

		if(!i_Item) return;

		/* Send action to mainframe or associated editor */
#ifdef JADEFUSION
		if((short) i_Sel > 0 && po_Action->mb_Disabled != 1)
#else
		if((short) i_Sel > 0 && !po_Action->mb_Disabled)
#endif
		{
			_Try_ 
			if(po_Wnd) po_Wnd->SetFocus();
			if(mpo_Editor)
			{
				mpo_Editor->OnAction(ul_Action);
			}
			else
			{
				M_MF()->OnAction(ul_Action);
			}
			_Catch_ 
			_End_
		}

		/* If an action has deleted that object, return */
		if(BIG_Handle() == NULL) return;
		if(!IsWindow(m_hWnd)) return;
		if(EDI_gb_HasClosed)
		{
			EDI_gb_HasClosed = FALSE;
			return;
		}

		/* Refresh menu after action */
		if(mpo_MenuFrame && IsWindow(mpo_MenuFrame->m_hWnd)) mpo_MenuFrame->FillList();

		/* Destroy menu */
		o_Popup.DestroyMenu();

		/*
		 * Invalidate now. This will cause mi_Sel to be update, so if we exit popupmenu
		 * with left click, and if the left click is on a valid category, we will display
		 * a new menu.
		 */
		pDC = GetWindowDC();
		mi_Sel = i_GetDrawSel(pDC, FALSE, mo_SelRect);
		if(mi_Sel == -1) Invalidate();
		ReleaseDC(pDC);
	}
	else if((mi_Sel == -1) && (mi_LastSel == -1))
	{
		if(i_JustAfter)
		{
			i_JustAfter = 0;
			return;
		}

		/* When lbuttondown is simulate by application */
		if(GetMessageExtraInfo() == 1) return;

		mi_Sel = mi_LastSel = -1;
		Invalidate();
		EDI_gb_TopMenuMode = FALSE;
		mb_SelOK = FALSE;
		EDI_gb_LockKeyboard = FALSE;

		/* For drag & drop */
		if(mpo_Editor)
		{
			SetCapture();
			mb_DragMode = TRUE;
			mo_DragRect.left = pt.x - 5;
			mo_DragRect.top = pt.y - 5;
			mo_DragRect.right = pt.x + 5;
			mo_DragRect.bottom = pt.y + 5;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_Menu::SetTop(BOOL _b_Top)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	mpo_Editor->GetClientRect(&o_Rect);
	if(_b_Top)
	{
		mb_LeftMode = FALSE;
		mpo_Editor->mst_BaseIni.mb_LeftMode = FALSE;

		ENG_gb_GlobalLock = TRUE;
		mpo_Editor->mpo_MyView->GetWindowRect(&o_Rect);
		mpo_Editor->mpo_MyView->GetParent()->ScreenToClient(&o_Rect);
		o_Rect.right--;
		mpo_Editor->mpo_MyView->MoveWindow(o_Rect, FALSE);
		o_Rect.right++;
		ENG_gb_GlobalLock = FALSE;
		mpo_Editor->mpo_MyView->MoveWindow(o_Rect);
	}
	else
	{
		mb_LeftMode = TRUE;
		mpo_Editor->mst_BaseIni.mb_LeftMode = TRUE;
		ENG_gb_GlobalLock = TRUE;
		mpo_Editor->mpo_MyView->GetWindowRect(&o_Rect);
		mpo_Editor->mpo_MyView->GetParent()->ScreenToClient(&o_Rect);
		o_Rect.right--;
		mpo_Editor->mpo_MyView->MoveWindow(o_Rect, FALSE);
		o_Rect.right++;
		ENG_gb_GlobalLock = FALSE;
		mpo_Editor->mpo_MyView->MoveWindow(o_Rect);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_Menu::OnLButtonUp(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mb_DragMode)
	{
		mb_DragMode = FALSE;
		ReleaseCapture();
		if(mo_DragRect.PtInRect(pt)) return;
		ClientToScreen(&pt);
		mpo_Editor->ScreenToClient(&pt);
		mpo_Editor->GetClientRect(&o_Rect);
		if(mb_LeftMode)
		{
			if((pt.y < o_Rect.top + 20) && (pt.y > o_Rect.top))
			{
				SetTop(TRUE);
			}
		}
		else
		{
			if((pt.x < o_Rect.left + 20) && (pt.x > o_Rect.left))
			{
				SetTop(FALSE);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_Menu::OnRButtonDown(UINT, CPoint)
{
	if(mb_DragMode)
	{
		mb_DragMode = FALSE;
		ReleaseCapture();
		return;
	}

	if(mi_Sel == -1) M_MF()->OnHistory();
}
#endif /* ACTIVE_EDITORS */
