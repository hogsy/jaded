/*$T CListCtrlEx.cpp GC! 1.081 04/01/04 21:59:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "stdafx.h"
#include "CListCtrlEx.h"

#include "SoundBase.h"
#include "CInitFile.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(CListCtrlEx, CListCtrl)
BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    CListCtrlEx construction/destruction
 =======================================================================================================================
 */
CListCtrlEx::CListCtrlEx(void)
{
	m_bFullRowSel = FALSE;

	m_cxClient = 0;
	m_cxStateImageOffset = 0;

	m_clrBkgnd = m_clrTextBk = APP_M_Get()->mpo_IniFile->m_BackGroundColor;
	m_clrText = APP_M_Get()->mpo_IniFile->m_TextColor;

    m_SelclrText = APP_M_Get()->mpo_IniFile->m_SelTextColor;
	m_SelclrBkgnd = APP_M_Get()->mpo_IniFile->m_SelBackGroundColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CListCtrlEx::~CListCtrlEx(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CListCtrlEx::PreCreateWindow(CREATESTRUCT &cs)
{
	/* default is report view and full row selection */
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED;
	m_bFullRowSel = TRUE;

	return(CListCtrl::PreCreateWindow(cs));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CListCtrlEx::SetFullRowSel(BOOL bFullRowSel)
{
	/*~~~~~~~~~*/
	BOOL	bRet;
	/*~~~~~~~~~*/

	/* no painting during change */
	LockWindowUpdate();

	m_bFullRowSel = bFullRowSel;

	if(m_bFullRowSel)
		bRet = ModifyStyle(0L, LVS_OWNERDRAWFIXED);
	else
		bRet = ModifyStyle(LVS_OWNERDRAWFIXED, 0L);

	/* repaint window if we are not changing view type */
	if(bRet && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT) Invalidate();

	/* repaint changes */
	UnlockWindowUpdate();

	return(bRet);
}

/* CListCtrlEx drawing */

/* offsets for first and other columns */
#define OFFSET_FIRST	2
#define OFFSET_OTHER	6

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC				*pDC;
	CRect			rcItem(lpDrawItemStruct->rcItem);
	UINT			uiFlags;
	CImageList		*pImageList;
	int				nItem;
	BOOL			bFocus;
	COLORREF		clrTextSave, clrBkSave;
	COLORREF		clrImage;
	static _TCHAR	szBuff[MAX_PATH];
	LPCTSTR			pszText;

	/* get item data */
	LV_ITEM			lvi;
	BOOL			bSelected;
	CRect			rcAllLabels;
	CRect			rcLabel;

	CRect			rcIcon;
	LV_COLUMN		lvc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	uiFlags = ILD_TRANSPARENT;
	nItem = lpDrawItemStruct->itemID;
	bFocus = (GetFocus() == this);
	clrImage = m_clrBkgnd;

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF; /* get all state flags */
	GetItem(&lvi);

	bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;

	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

	/* set colors if item is selected */
	GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;

	if(bSelected)
	{
		clrTextSave = pDC->SetTextColor(m_SelclrText);
		clrBkSave = pDC->SetBkColor(m_SelclrBkgnd );
		pDC->FillRect(rcAllLabels, &CBrush(m_SelclrBkgnd));
	}
	else
	{
		pDC->SetTextColor(m_clrText);
		pDC->SetBkColor(m_clrBkgnd);
		pDC->FillRect(rcAllLabels, &CBrush(m_clrTextBk));
	}

	/* set color and mask for the icon */
	if(lvi.state & LVIS_CUT)
	{
		clrImage = m_clrBkgnd;
		uiFlags |= ILD_BLEND50;
	}
	else if(bSelected)
	{
		clrImage = m_SelclrBkgnd;
		uiFlags |= ILD_BLEND50;
	}

	/* draw normal and overlay icon */
	GetItemRect(nItem, rcIcon, LVIR_ICON);

	pImageList = GetImageList(LVSIL_SMALL);
	if(pImageList)
	{
		/*~~~~~~~~~~~~~~~~~~*/
		UINT	nOvlImageMask;
		/*~~~~~~~~~~~~~~~~~~*/

		nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;

		if(rcItem.left < rcItem.right - 1)
		{
			ImageList_DrawEx
			(
				pImageList->m_hImageList,
				lvi.iImage,
				pDC->m_hDC,
				rcIcon.left,
				rcIcon.top,
				16,
				16,
				m_clrBkgnd,
				clrImage,
				uiFlags | nOvlImageMask
			);
		}
	}

	/* draw item label */
	GetItemRect(nItem, rcItem, LVIR_LABEL);
	rcItem.right -= m_cxStateImageOffset;

	pszText = MakeShortString(pDC, szBuff, rcItem.right - rcItem.left, 2 * OFFSET_FIRST);

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	pDC->DrawText(pszText, -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

	/* draw labels for extra columns */
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		/*~~~~~~~~~~~~~*/
		UINT	nJustify;

		int		nRetLen;
		/*~~~~~~~~~~~~~*/

		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		nRetLen = GetItemText(nItem, nColumn, szBuff, sizeof(szBuff));

		if(nRetLen == 0) continue;

		pszText = MakeShortString(pDC, szBuff, rcItem.right - rcItem.left, 2 * OFFSET_OTHER);

		nJustify = DT_LEFT;

		if(pszText == szBuff)
		{
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default: break;
			}
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

	/* draw focus rectangle if item has focus */
	if(lvi.state & LVIS_FOCUSED && bFocus) pDC->DrawFocusRect(rcAllLabels);

	/* set original colors if item was selected */
	if(bSelected)
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LPCTSTR CListCtrlEx::MakeShortString(CDC *pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static const _TCHAR szThreeDots[] = _T("...");

	int					nStringLen;

	static _TCHAR		szShort[MAX_PATH];

	int					nAddLen;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	nStringLen = lstrlen(lpszLong);

	if(nStringLen == 0 || (pDC->GetTextExtent(lpszLong, nStringLen).cx + nOffset) <= nColumnLen)
	{
		return(lpszLong);
	}

	lstrcpy(szShort, lpszLong);
	nAddLen = pDC->GetTextExtent(szThreeDots, sizeof(szThreeDots)).cx;

	for(int i = nStringLen - 1; i > 0; i--)
	{
		szShort[i] = 0;
		if((pDC->GetTextExtent(szShort, i).cx + nOffset + nAddLen) <= nColumnLen)
		{
			break;
		}
	}

	lstrcat(szShort, szThreeDots);
	return(szShort);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CListCtrlEx::RepaintSelectedItems(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CRect	rcItem, rcLabel;
	int		nItem;
	/*~~~~~~~~~~~~~~~~~~~~*/

	nItem = GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		GetItemRect(nItem, rcItem, LVIR_BOUNDS);
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;

		InvalidateRect(rcItem, FALSE);
	}

	/* if selected items should not be preserved, invalidate them */
	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = GetNextItem(-1, LVNI_SELECTED); nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		{
			GetItemRect(nItem, rcItem, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcItem.left = rcLabel.left;

			InvalidateRect(rcItem, FALSE);
		}
	}

	/* update changes */
	UpdateWindow();
}

/*
 =======================================================================================================================
    CListCtrlEx message handlers
 =======================================================================================================================
 */
LRESULT CListCtrlEx::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~*/
	OSVERSIONINFO	info;
	/*~~~~~~~~~~~~~~~~~*/

	info.dwOSVersionInfoSize = sizeof(info);
	VERIFY(::GetVersionEx(&info));

	if((int) wParam == LVSIL_STATE && info.dwMajorVersion < 4)
	{
		/*~~~~~~~*/
		int cx, cy;
		/*~~~~~~~*/

		if(::ImageList_GetIconSize((HIMAGELIST) lParam, &cx, &cy))
			m_cxStateImageOffset = cx;
		else
			m_cxStateImageOffset = 0;
	}

	return(Default());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT CListCtrlEx::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	m_clrText = (COLORREF) lParam;
	return(Default());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT CListCtrlEx::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrTextBk = (COLORREF) lParam;
	return(Default());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT CListCtrlEx::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrBkgnd = (COLORREF) lParam;
	return(Default());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CListCtrlEx::OnSize(UINT nType, int cx, int cy)
{
	m_cxClient = cx;
	CListCtrl::OnSize(nType, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CListCtrlEx::OnPaint(void)
{
	/*
	 * in full row select mode, we need to extend the clipping region so we can paint
	 * a selection all the way to the right
	 */
	CListCtrl::OnPaint();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CListCtrlEx::OnSetFocus(CWnd *pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);

	/* repaint items that should change appearance */
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT) RepaintSelectedItems();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CListCtrlEx::OnKillFocus(CWnd *pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);

	/* check if we are losing focus to label edit box */
	if(pNewWnd != NULL && pNewWnd->GetParent() == this) return;

	/* repaint items that should change appearance */
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT) RepaintSelectedItems();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

