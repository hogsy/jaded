/*$T MENlist.cpp GC 1.134 04/22/04 10:19:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: The list to display actions in a menu frame. */
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "EDIaction.h"
#include "EDImainframe.h"
#include "MENlist.h"
#include "MENin.h"
#include "EDIerrid.h"
#include "DIAlogs/DIAkeyboard_dlg.h"
#include "Res/Res.h"

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

BEGIN_MESSAGE_MAP(EMEN_cl_FrameList, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EMEN_cl_FrameList::EMEN_cl_FrameList(void)
{
	mb_BeginDragDrop = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EMEN_cl_FrameList::~EMEN_cl_FrameList(void)
{
	mpo_Parent = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMEN_cl_FrameList::OnEraseBkgnd(CDC *pDC)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mpo_Parent) mpo_Parent->SaveIni();
	GetClientRect(&o_Rect);
	pDC->FillSolidRect(o_Rect, GetSysColor(COLOR_3DFACE));
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_FrameList::OnMouseMove(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Index, i_NewIndex;
	CRect			o_Rect;
	CPoint			pt1;
	EDI_cl_Action	*po_Action;
	CSize			o_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Drag & drop operation */
	i_Index = GetNextItem(-1, LVIS_SELECTED);
	if(i_Index != -1)
	{
		if(M_MF()->b_MoveDragDrop(pt))
		{
			po_Action = (EDI_cl_Action *) GetItemData(i_Index);
			if(po_Action->mo_Key.IsEmpty())
			{
				EDI_gst_DragDrop.b_CanDragDrop = FALSE;
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
			}
			else
			{
				EDI_gst_DragDrop.b_CanDragDrop = FALSE;
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
			}
		}
	}

	/* No special process for category list */
	if(mb_Up) return;
	if(EDI_gst_DragDrop.b_BeginDragDrop) return;

	/* To be sure that menu outline will disapeared if mouse goes outside list control. */
	SetCapture();
	GetClientRect(&o_Rect);
	if(!o_Rect.PtInRect(pt)) ReleaseCapture();

	/*
	 * Release if cursor is in list rect, but on a different window (floating toolbar
	 * for example)
	 */
	pt1 = pt;
	ClientToScreen(&pt1);
	if(WindowFromPoint(pt1) != this)
	{
		ReleaseCapture();
		i_Index = GetNextItem(-1, LVNI_SELECTED);
		if(i_Index != -1)
		{
			SetItemState(i_Index, 0, LVIS_FOCUSED | LVIS_SELECTED);
			GetItemRect(i_Index, &o_Rect, LVIR_BOUNDS);
			InvalidateRect(&o_Rect);
		}

		return;
	}

	/* Reset selected status for current selected item */
	if(i_Index != -1) SetItemState(i_Index, 0, LVIS_FOCUSED | LVIS_SELECTED);

	/* Select new item under mouse */
	i_NewIndex = HitTest(pt);
	if(i_NewIndex != -1) SetItemState(i_NewIndex, LVIS_SELECTED, LVIS_SELECTED);

	if(i_Index != i_NewIndex)
	{
		if(i_Index != -1)
		{
			GetItemRect(i_Index, &o_Rect, LVIR_BOUNDS);
			InvalidateRect(&o_Rect);
		}

		if(i_NewIndex != -1)
		{
			GetItemRect(i_NewIndex, &o_Rect, LVIR_BOUNDS);
			InvalidateRect(&o_Rect);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_FrameList::OnLButtonUp(UINT n, CPoint pt)
{
	if(EDI_gst_DragDrop.b_BeginDragDrop)
	{
		/* End of drag & drop */
		M_MF()->EndDragDrop(pt);
	}

	CListCtrl::OnLButtonUp(n, pt);
}

/*
 =======================================================================================================================
    Aim: We can drag & dropped a menu in an editor of the same type (to move the menu), or toan editor or appli (to add
    a top menu).
 =======================================================================================================================
 */
void EMEN_cl_FrameList::OnBeginDrag(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW *pNotifyStruct;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pNotifyStruct = (NM_LISTVIEW *) pNotifyStruct2;

	/* Can't drag & drop a category */
	if(mb_Up) return;

	EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
	EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
	EDI_gst_DragDrop.i_Param1 = 0;
	M_MF()->BeginDragDrop(pNotifyStruct->ptAction, this, NULL, EDI_DD_User);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_FrameList::OnRButtonDown(UINT n, CPoint pt)
{
	M_MF()->CancelDragDrop();
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_FrameList::OnLButtonDown(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_Index;
	EDI_cl_ConfigList		*po_Config;
	EDI_cl_Action			*po_Action, *po_NewAct;
	CRect					o_Rect;
	EDIA_cl_KeyboardDialog	o_Dialog;
	CWnd					*po_Wnd;
	CSize					o_Size;
	CDC						*pDC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Invalidate();
	ReleaseCapture();
	i_Index = HitTest(pt);
	if(i_Index == -1) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Special for up list
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mb_Up)
	{
		po_Action = (EDI_cl_Action *) GetItemData(i_Index);
		if(po_Action->mui_NumDown)	/* Validate only if they are action in categ */
		{
			if(mpo_Parent && mpo_Parent->mpo_Editor) mpo_Parent->mpo_Editor->mst_BaseIni.ISplitMenu = i_Index;
			po_Wnd = GetFocus();
			CListCtrl::OnLButtonDown(n, pt);
			if(po_Wnd && po_Wnd->IsWindowVisible()) po_Wnd->SetFocus();

			mpo_Parent->FillList();
		}

		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We are in the key column ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	po_Wnd = GetFocus();
	CListCtrl::OnLButtonDown(n, pt);
	if(po_Wnd) po_Wnd->SetFocus();

	/* No process if a drag & drop operation is running */
	if(EDI_gst_DragDrop.b_BeginDragDrop) return;

	if(i_Index != -1)
	{
		/* Test if we have clicked in the key area (to change the key) */
		po_Action = (EDI_cl_Action *) GetItemData(i_Index);
		GetItemRect(i_Index, &o_Rect, LVIR_BOUNDS);

		/* Get size of text. Force to be a constant if no associated key */
		pDC = GetDC();
		if(!po_Action->mo_Key.IsEmpty())
			o_Size = pDC->GetTextExtent(po_Action->mo_Key);
		else
			o_Size.cx = 0;
		ReleaseDC(pDC);

		o_Rect.left = o_Rect.right - (o_Size.cx + 5);
		if(o_Rect.PtInRect(pt))
		{
			if(o_Dialog.DoModal() == IDOK)
			{
				/*
				 * Inform owner about change. We want to get the muw_Key field of not correct (for
				 * a dynamic action)
				 */
				if(mpo_Actions->mpo_Editor)
					po_NewAct = mpo_Actions->mpo_Editor->po_ActionGetSetDynKey(mpo_Actions, po_Action);
				else
					po_NewAct = M_MF()->po_ActionGetSetDynKey(mpo_Actions, po_Action);

				/*
				 * If current action is associated with another one (po_NewAct), the save action
				 * will be new action instead of original one
				 */
				if(!po_NewAct) po_NewAct = po_Action;

				/* Save the new key */
				mpo_Actions->SaveUser(po_NewAct->mul_Action, o_Dialog.muw_Key, o_Dialog.mo_Key);
				mpo_Parent->FillList();

				/* Inform owner */
				if(mpo_Actions->mpo_Editor)
					mpo_Actions->mpo_Editor->OneActionKeyHasChanged();
				else
					M_MF()->OneActionKeyHasChanged();
			}

			SetItemState(i_Index, 0, LVIS_SELECTED);
			Invalidate();
			return;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Call the corresponding action procedure
	 -------------------------------------------------------------------------------------------------------------------
	 */

	po_Config = mpo_Actions->mo_List.GetAt(mpo_Actions->mo_List.FindIndex(mpo_Actions->mi_CurrentList));
	po_Action = (EDI_cl_Action *) GetItemData(i_Index);

	_Try_ 
	if(mpo_Actions == M_MF()->mpo_Actions) 
	{
		M_MF()->OnAction(po_Action->mul_Action);
	}
	else
	{
		mpo_Actions->mpo_Editor->OnAction(po_Action->mul_Action);
	}

	_Catch_ 
	_End_

	/* Refresh list */
	if(IsWindow(m_hWnd) && mpo_Parent)	/* Cause of action to close app */
		mpo_Parent->FillList();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_FrameList::DrawItem(DRAWITEMSTRUCT *pdraw)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	GetItemRect(pdraw->itemID, &o_Rect, LVIR_BOUNDS);
	DrawAction(pdraw, o_Rect);
}

/*
 =======================================================================================================================
    Aim: Call to draw an item action. In: pdraw DRAWITEMSTRUCT to get infos about item. _o_Rect The coords of the item.
    _b_TextFromList The text of the action is retreived in left list, or in action. Note: This function is called to
    draw an item for top menu and frame menu !!
 =======================================================================================================================
 */
void EMEN_cl_FrameList::DrawAction(DRAWITEMSTRUCT *pdraw, CRect _o_Rect, BOOL _b_TextFromList, BOOL _b_Sub)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC				*pDC, dc, bmpdc, newdc;
	CRect			o_Rect1, o_Rect2, o_Rect3;
	char			asz_Text[255];
	CBrush			o_Brush, *po_OldB;
	EDI_cl_Action	*po_Action, *po_New;
	UINT			ui_Align;
	CPoint			pt, size;
	CBitmap			bmp;
	CSize			o_Size;
	UINT			ui_TA;
	CString			o_Str;
	int				i_OffsetYTxt;
	CString			o_Display;
	COLORREF		colbk;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_Sub)
		colbk = GetSysColor(COLOR_WINDOW);
	else
		colbk = GetSysColor(COLOR_BTNFACE);

	pDC = dc.FromHandle(pdraw->hDC);
	pDC->SetBkMode(TRANSPARENT);

	/* Get associated action */
	po_Action = (EDI_cl_Action *) pdraw->itemData;
	o_Display = po_Action->mo_DisplayName;
	if(!po_Action->mo_RealDisplayName.IsEmpty()) o_Display = po_Action->mo_RealDisplayName;
	i_OffsetYTxt = 0;
	if(_b_Sub)
	{
		size = pDC->GetTextExtent(o_Display);
		if(M_MF()->mst_Ini.ui_HeightMenu < (UINT) size.y)
			i_OffsetYTxt = 0;
		else
			i_OffsetYTxt = (M_MF()->mst_Ini.ui_HeightMenu - size.y) / 2;
	}

	/* Retreive text */
	*asz_Text = 0;
	if(_b_TextFromList)
		GetItemText(pdraw->itemID, 0, asz_Text, 255);
	else
		L_strcpy(asz_Text, o_Display);

	o_Rect2 = _o_Rect;
	o_Rect2.left = 0;
	o_Rect2.right = o_Rect2.left + 22;
	pDC->FillSolidRect(&o_Rect2, GetSysColor(COLOR_BTNFACE));
	o_Rect2 = _o_Rect;
	o_Rect2.bottom++;
	o_Rect2.left += 18;
	o_Rect2.right += 20;
	pDC->FillSolidRect(&o_Rect2, colbk);

	/* Draw a separator */
	if(po_Action->mul_Action == 0)
	{
		o_Rect1 = _o_Rect;
		_o_Rect.top = _o_Rect.top + ((_o_Rect.bottom - _o_Rect.top) / 2);
		_o_Rect.top -= 2;
		_o_Rect.bottom = _o_Rect.top + 1;
		_o_Rect.left += 24;
		_o_Rect.right -= 4;
		pDC->Draw3dRect(&_o_Rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
		_o_Rect.left -= 24;

		/* Display a separator title */
		if(*asz_Text && L_strcmpi(asz_Text, "Sep"))
		{
			o_Str = " ";
			o_Str += asz_Text;
			o_Str += " ";
			L_strcpy(asz_Text, (char *) (LPCSTR) o_Str);
			pDC->SetBkMode(OPAQUE);
			pDC->SetBkColor(colbk);
			ui_TA = pDC->GetTextAlign();
			pDC->SetTextAlign(TA_CENTER);
			o_Size = pDC->GetTextExtent(o_Str);
			o_Rect2 = o_Rect1;
			o_Rect2.left = _o_Rect.Width() / 2 - o_Size.cx / 2;
			o_Rect2.right = _o_Rect.Width() / 2 + o_Size.cx / 2;
			o_Rect2.left -= 8;
			o_Rect2.right += 8;

			pDC->SetBkColor(colbk);
			pDC->SetTextColor(GetSysColor(COLOR_ACTIVECAPTION));
			pDC->ExtTextOut
				(
					o_Rect1.left + (o_Rect1.right - o_Rect1.left) / 2,
					o_Rect1.top + i_OffsetYTxt,
					ETO_CLIPPED,
					&o_Rect2,
					asz_Text,
					L_strlen(asz_Text),
					0
				);
			pDC->SetTextAlign(ui_TA);
		}

		return;
	}

	/* Create brush to draw selected item */
	o_Brush.CreateSolidBrush(colbk);
	po_OldB = pDC->SelectObject(&o_Brush);

	/* Force a dummy rect to eventually erase last selected box */
	if(!(pdraw->itemState & ODS_SELECTED))
	{
		o_Rect2 = _o_Rect;
		o_Rect2.left += 21;
		o_Rect2.bottom++;
		pDC->Draw3dRect(&o_Rect2, colbk, colbk);
	}

	/* If a valid action does not have key, request for an eventual dynamic fill */
	if(((int) po_Action->mul_Action >= 0) && ((int) po_Action->muw_Key == 0))
	{
		if(!mpo_Actions->mpo_Editor)
			po_New = M_MF()->po_ActionGetSetDynKey(mpo_Actions, po_Action);
		else
			po_New = mpo_Actions->mpo_Editor->po_ActionGetSetDynKey(mpo_Actions, po_Action);
		if(po_New)
		{
			po_Action->mo_Key = po_New->mo_Key;
			po_Action->mui_Resource = po_New->mui_Resource;
		}
	}

	/* The rect for key */
	if(!mb_Up && !po_Action->mo_Key.IsEmpty())
	{
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		o_Rect2 = _o_Rect;
		o_Size = pDC->GetTextExtent(po_Action->mo_Key);
		o_Rect2.left = o_Rect2.right - (o_Size.cx + 10);

		o_Rect3 = o_Rect2;
		o_Rect3.right = o_Rect3.left + 2;
		pDC->Draw3dRect(&o_Rect3, colbk, colbk);
	}

	/*
	 * Draw a small bitmap. We take the bitmap if for the toolbar button associated
	 * with the action, and we strech it. Don't display the bitmap if there's no key. £
	 * £
	 * Drawing is a bit complicated cause we want the back color of the bitmap (192,
	 * 192, 192) to be transparent.
	 */
	o_Rect1 = _o_Rect;
	if(!mb_Up && po_Action->mui_Resource && !po_Action->mo_Key.IsEmpty())
	{
		M_MF()->DrawTransparentBmp
			(
				po_Action->mui_Resource,
				pDC,
				_o_Rect.left + 4,
				_o_Rect.top,
				14,
				14,
				32,
				32,
				RGB(192, 192, 192)
			);
		o_Rect1.left += 16;
	}

	/* Draw selected box */
	if(pdraw->itemState & ODS_SELECTED && !po_Action->mb_Disabled)
	{
		o_Rect2 = _o_Rect;
		o_Rect2.bottom++;
		o_Rect2.left += 21;
		pDC->FillSolidRect(&o_Rect2, GetSysColor(COLOR_ACTIVECAPTION));
	}
	else
	{
		o_Rect2 = _o_Rect;
		o_Rect2.bottom++;
		o_Rect2.left += 21;
		pDC->FillSolidRect(&_o_Rect, colbk);
	}

	/* State of action (checkbox, radio...) */
	if(!mb_Up)
	{
		o_Rect2 = _o_Rect;
		o_Rect2.left = 0;
		o_Rect2.right = o_Rect2.left + 22;
		pDC->FillSolidRect(&o_Rect2, GetSysColor(COLOR_BTNFACE));

		if(((int) po_Action->mui_State != -1) && ((po_Action->mui_State & 0x00FFFFFF) || (!po_Action->mui_State)))
		{
			o_Rect2 = o_Rect1;
			o_Rect2.left -= 1;
			o_Rect2.right = o_Rect2.left + 16;
			pDC->DrawFrameControl(o_Rect2, DFC_BUTTON, po_Action->mui_State);
		}

		o_Rect1.left += 18;
	}

	/* Display action text */
	if(po_Action->muw_Key && po_Action->mul_Action)
	{
		o_Size = pDC->GetTextExtent(po_Action->mo_Key);
		o_Rect1.right -= (o_Size.cx + 10);
	}

	if(!mb_Up || po_Action->mui_NumDown)
	{
		if(po_Action->mb_Disabled)
			pDC->SetTextColor(GetSysColor(COLOR_3DSHADOW));
		else if(pdraw->itemState & ODS_SELECTED)
			pDC->SetTextColor(0x00FFFFFF);
		else if((po_Action->mui_State != -1) && (po_Action->mui_State & 0x80000000))
			pDC->SetTextColor(0x000000BF);
		else if((po_Action->mui_State != -1) && (po_Action->mui_State & 0x40000000))
			pDC->SetTextColor(0x00BF0000);
		else
			pDC->SetTextColor(0);
		pDC->ExtTextOut
			(
				o_Rect1.left + 5,
				o_Rect1.top + 1 + i_OffsetYTxt,
				ETO_CLIPPED,
				&o_Rect1,
				asz_Text,
				L_strlen(asz_Text),
				0
			);
	}
	else
	{
		pDC->SetTextColor(GetSysColor(COLOR_3DSHADOW));
		pDC->ExtTextOut
			(
				o_Rect1.left + 4,
				o_Rect1.top + i_OffsetYTxt,
				ETO_CLIPPED,
				&o_Rect1,
				asz_Text,
				L_strlen(asz_Text),
				0
			);

		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	}

	/* Display key (only for a valid action and for a valid key) */
	if(((int) po_Action->mul_Action >= 0) && !po_Action->mo_Key.IsEmpty())
	{
		o_Rect1 = _o_Rect;

		o_Size = pDC->GetTextExtent(po_Action->mo_Key);
		o_Rect1.left = o_Rect1.right - (o_Size.cx + 5);

		ui_Align = pDC->SetTextAlign(TA_RIGHT);

		if(po_Action->mb_Disabled)
			pDC->SetTextColor(GetSysColor(COLOR_3DSHADOW));
		else if(pdraw->itemState & ODS_SELECTED)
			pDC->SetTextColor(0x00FFFFFF);
		else
			pDC->SetTextColor(GetSysColor(COLOR_ACTIVECAPTION));
		pDC->ExtTextOut
			(
				o_Rect1.right - 5,
				o_Rect1.top + i_OffsetYTxt,
				ETO_CLIPPED,
				&o_Rect1,
				(char *) (LPCSTR) po_Action->mo_Key,
				L_strlen((char *) (LPCSTR) po_Action->mo_Key),
				0
			);
		pDC->SetTextAlign(ui_Align);
	}

	pDC->SelectObject(po_OldB);
	pDC->DeleteTempMap();

	DeleteObject(&o_Brush);
}
#endif
