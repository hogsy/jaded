//_ **********************************************************
//_ 
//_ Name: EnTabCtrl.cpp 
//_ Purpose: 
//_ Created: 15 September 1998 
//_ Author: D.R.Godson
//_ Modified By: 
//_ 
//_ Copyright (c) 1998 Brilliant Digital Entertainment Inc. 
//_ 
//_ **********************************************************

// EnTabCtrl.cpp : implementation file
//

#include "Precomp.h"
#include "EnTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnTabCtrl

DWORD CEnTabCtrl::s_dwCustomLook = 1;

enum { PADDING = 3, EDGE = 20};

//////////////////////////////////////////////////////////////////////////////
// helpers
COLORREF Darker(COLORREF crBase, float fFactor)
{
	ASSERT (fFactor < 1.0f && fFactor > 0.0f);

	fFactor = min(fFactor, 1.0f);
	fFactor = max(fFactor, 0.0f);

	BYTE bRed, bBlue, bGreen;
	BYTE bRedShadow, bBlueShadow, bGreenShadow;

	bRed = GetRValue(crBase);
	bBlue = GetBValue(crBase);
	bGreen = GetGValue(crBase);

	bRedShadow = (BYTE)(bRed * fFactor);
	bBlueShadow = (BYTE)(bBlue * fFactor);
	bGreenShadow = (BYTE)(bGreen * fFactor);

	return RGB(bRedShadow, bGreenShadow, bBlueShadow);
}

COLORREF Lighter(COLORREF crBase, float fFactor) 
{
	ASSERT (fFactor > 1.0f);

	fFactor = max(fFactor, 1.0f);

	BYTE bRed, bBlue, bGreen;
	BYTE bRedHilite, bBlueHilite, bGreenHilite;

	bRed = GetRValue(crBase);
	bBlue = GetBValue(crBase);
	bGreen = GetGValue(crBase);

	bRedHilite = (BYTE)min((int)(bRed * fFactor), 255);
	bBlueHilite = (BYTE)min((int)(bBlue * fFactor), 255);
	bGreenHilite = (BYTE)min((int)(bGreen * fFactor), 255);

	return RGB(bRedHilite, bGreenHilite, bBlueHilite);
}

CSize FormatText(CString& sText, CDC* pDC, int nWidth)
{
	CRect rect(0, 0, nWidth, 20);
	UINT uFlags = DT_CALCRECT | DT_SINGLELINE | DT_MODIFYSTRING | DT_END_ELLIPSIS;

	::DrawText(pDC->GetSafeHdc(), sText.GetBuffer(sText.GetLength() + 4), -1, rect, uFlags);
	sText.ReleaseBuffer();

	return pDC->GetTextExtent(sText);
}

////////////////////////////////////////////////////////////////////////////////////////

CEnTabCtrl::CEnTabCtrl()
{
}

CEnTabCtrl::~CEnTabCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CEnTabCtrl message handlers

void CEnTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	TC_ITEM     tci;
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	HIMAGELIST hilTabs = (HIMAGELIST)TabCtrl_GetImageList(GetSafeHwnd());

	BOOL bSelected = (lpdis->itemID == (UINT)GetCurSel());
	BOOL bColor = (s_dwCustomLook & ETC_COLOR);

	CRect rItem(lpdis->rcItem);

	if(GetStyle() & TCS_BOTTOM)
	{
		if (bSelected)
			rItem.top -= 1;
		else
			rItem.top -= 2;
	}
	else
	{
		if (bSelected)
			rItem.bottom -= 1;
		else
			rItem.bottom += 2;
	}

	// tab
	// blend from back color to COLOR_3DFACE if 16 bit mode or better
	COLORREF crFrom = GetTabColor(bSelected);
	
	if (s_dwCustomLook & ETC_GRADIENT && pDC->GetDeviceCaps(BITSPIXEL) >= 16)
	{
		COLORREF crTo = bSelected ? ::GetSysColor(COLOR_3DFACE) : Darker(!bColor || m_crBack == -1 ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 0.7f);

		int nROrg = GetRValue(crFrom);
		int nGOrg = GetGValue(crFrom);
		int nBOrg = GetBValue(crFrom);
		int nRDiff = GetRValue(crTo) - nROrg;
		int nGDiff = GetGValue(crTo) - nGOrg;
		int nBDiff = GetBValue(crTo) - nBOrg;
		
		int nHeight = rItem.Height();

		if(GetStyle() & TCS_BOTTOM)
		{
			for (int nLine = nHeight - 1; nLine >= 0; nLine -= 2)
			{
				int nRed = nROrg + (nLine * nRDiff) / nHeight;
				int nGreen = nGOrg + (nLine * nGDiff) / nHeight;
				int nBlue = nBOrg + (nLine * nBDiff) / nHeight;		
				pDC->FillSolidRect(CRect(rItem.left, rItem.top + nLine, rItem.right, rItem.top + nLine + 2), RGB(nRed, nGreen, nBlue));
			}
		}
		else
		{
			for (int nLine = 0; nLine < nHeight; nLine += 2)
			{
				int nRed = nROrg + (nLine * nRDiff) / nHeight;
				int nGreen = nGOrg + (nLine * nGDiff) / nHeight;
				int nBlue = nBOrg + (nLine * nBDiff) / nHeight;		
				pDC->FillSolidRect(CRect(rItem.left, rItem.top + nLine, rItem.right, rItem.top + nLine + 2), RGB(nRed, nGreen, nBlue));
			}
		}
	}
	else // simple solid fill
		pDC->FillSolidRect(rItem, crFrom);

	// text & icon
	rItem.left += PADDING;
	if(GetStyle() & TCS_BOTTOM) 
		rItem.top += PADDING + (bSelected ? 1 : -1);
	else
		rItem.top += PADDING + (bSelected ? 1 : 0);

	pDC->SetBkMode(TRANSPARENT);

	CString sTemp;
	tci.mask        = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText     = sTemp.GetBuffer(100);
	tci.cchTextMax  = 99;
	GetItem(lpdis->itemID, &tci);
	sTemp.ReleaseBuffer();

	// icon
	if (hilTabs)
	{
		ImageList_Draw(hilTabs, tci.iImage, *pDC, rItem.left, rItem.top, ILD_TRANSPARENT);
		rItem.left += 16 + PADDING;
	}

	// text
	rItem.right -= PADDING;
	FormatText(sTemp, pDC, rItem.Width());

	pDC->SetTextColor(GetTabTextColor(bSelected));
	pDC->DrawText(sTemp, rItem, DT_NOPREFIX | DT_CENTER);
}

void CEnTabCtrl::DrawItemBorder(LPDRAWITEMSTRUCT lpdis)
{
	ASSERT (s_dwCustomLook & ETC_FLAT);

	BOOL bSelected = (lpdis->itemID == (UINT)GetCurSel());
	BOOL bBackTabs = (s_dwCustomLook & ETC_BACKTABS);

	CRect rItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC); 

	COLORREF crTab = GetTabColor(bSelected);
	COLORREF crHighlight = Lighter(crTab, 1.5f);
	COLORREF crShadow = Darker(crTab, 0.75f);

	if (bSelected || bBackTabs)
	{
		rItem.bottom += bSelected ? -1 : 1;

		// edges
		pDC->FillSolidRect(CRect(rItem.left, rItem.top, rItem.left + 1, rItem.bottom), crHighlight);
		pDC->FillSolidRect(CRect(rItem.left, rItem.top, rItem.right, rItem.top + 1), crHighlight);
		pDC->FillSolidRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), crShadow);
	}
	else // draw simple dividers
	{
		pDC->FillSolidRect(CRect(rItem.left - 1, rItem.top, rItem.left, rItem.bottom), crShadow);
		pDC->FillSolidRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), crShadow);
	}
}

void CEnTabCtrl::DrawMainBorder(LPDRAWITEMSTRUCT lpdis)
{
	CRect rBorder(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC); 

	COLORREF crTab = GetTabColor();
	COLORREF crHighlight = Lighter(crTab, 1.5f);
	COLORREF crShadow = Darker(crTab, 0.75f);

	pDC->Draw3dRect(rBorder, crHighlight, crShadow);
}

COLORREF CEnTabCtrl::GetTabColor(BOOL bSelected)
{
	BOOL bColor = (s_dwCustomLook & ETC_COLOR);
	BOOL bHiliteSel = (s_dwCustomLook & ETC_SELECTION);
	BOOL bBackTabs = (s_dwCustomLook & ETC_BACKTABS);
	BOOL bFlat = (s_dwCustomLook & ETC_FLAT);

	if (bSelected && bHiliteSel)
	{
		if (bColor)
			return Lighter((m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 1.4f);
		else
			return Lighter(::GetSysColor(COLOR_3DFACE), 1.4f);
	}
	else if (!bSelected)
	{
		if (bBackTabs || !bFlat)
		{
			if (bColor)
				return Darker((m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 0.9f);
			else
				return Darker(::GetSysColor(COLOR_3DFACE), 0.9f);
		}
		else
			return (m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack;
	}

	// else
	return ::GetSysColor(COLOR_3DFACE);
}

COLORREF CEnTabCtrl::GetTabTextColor(BOOL bSelected)
{
	BOOL bColor = (s_dwCustomLook & ETC_COLOR);
	BOOL bFlat = (s_dwCustomLook & ETC_FLAT);

	if (1)//bSelected)
	{
		return ::GetSysColor(COLOR_WINDOWTEXT);
	}
	else
	{
		if (bColor || bFlat)
			return Darker((m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 0.5f);
		else
			return Darker(::GetSysColor(COLOR_3DFACE), 0.5f);
	}

	// else
	return Darker(::GetSysColor(COLOR_3DFACE), 0.5f);
}

void CEnTabCtrl::EnableCustomLook(BOOL bEnable, DWORD dwStyle)
{
	if (!bEnable)
		dwStyle = 0;

	s_dwCustomLook = dwStyle;
}

void CEnTabCtrl::PreSubclassWindow() 
{
	CBaseTabCtrl::PreSubclassWindow();

	if (s_dwCustomLook)
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
}
