/*$T MENinmenu.cpp GC! 1.081 05/30/00 19:02:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "EDImainframe.h"
#include "EDIbaseframe.h"
#include "EDIerrid.h"
#include "MENmenu.h"
#include "MENinmenu.h"
#include "RES/res.h"

#ifdef JADEFUSION
extern ULONG COLOR_ul_Blend(ULONG C1, ULONG C2, float f);
#else
extern "C" ULONG COLOR_ul_Blend(ULONG C1, ULONG C2, float f);
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

struct CToolBarData
{
	WORD	wVersion;
	WORD	wWidth;
	WORD	wHeight;
	WORD	wItemCount;

	/*
	 ===================================================================================================================
	 ===================================================================================================================
	 */
	WORD *items(void)
	{
		return (WORD *) (this + 1);
	}
};

/*
 =======================================================================================================================
    Byte order dependent (from BarTool.cpp
 =======================================================================================================================
 */
#define RGB_TO_RGBQUAD(r, g, b) (RGB(b, g, r))
#define CLR_TO_RGBQUAD(clr)		(RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))
#define _countof(array)			(sizeof(array) / sizeof(array[0]))

struct AFX_COLORMAP
{
	/* use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily */
	DWORD	rgbqFrom;
	int		iSysColorTo;
};

AFX_STATIC_DATA const AFX_COLORMAP	_afxSysColorMap[] =
{
	/* mapping from color in DIB to system color */
	{ RGB_TO_RGBQUAD(0x00, 0x00, 0x00), COLOR_BTNTEXT },

	/* black */
	{ RGB_TO_RGBQUAD(0x80, 0x80, 0x80), COLOR_BTNSHADOW },

	/* dark gray */
	{ RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0), COLOR_BTNFACE },

	/* bright gray */
	{ RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF), COLOR_BTNHIGHLIGHT }	/* white */
};

BEGIN_MESSAGE_MAP(EMEN_cl_ToolBar, CToolBar)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMEN_cl_ToolBar::LoadFullColorToolBar(int nResID)
{
	return LoadFullColorToolBar(MAKEINTRESOURCE(nResID));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HBITMAP EMEN_cl_ToolBar::LoadMappedFullColorBitmap(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HGLOBAL				hglb;
	LPBITMAPINFOHEADER	lpBitmap;
	int					nColorTableSize;
	UINT				nSize;
	LPBITMAPINFOHEADER	lpBitmapInfo;
	DWORD				*pColorTable;

	int					nWidth;
	int					nHeight;
	HDC					hDCScreen;
	HBITMAP				hbm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((hglb = LoadResource(hInst, hRsrc)) == NULL) return NULL;

	lpBitmap = (LPBITMAPINFOHEADER) LockResource(hglb);

	if(lpBitmap == NULL) return NULL;

	/* make copy of BITMAPINFOHEADER so we can modify the color table */
	if(lpBitmap->biBitCount <= 8)
		nColorTableSize = (1 << lpBitmap->biBitCount);
	else
		nColorTableSize = 0;	/* No palette needed for 24 BPP DIB */
	nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);

	if(lpBitmapInfo == NULL) return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	/* color table is in RGBQUAD DIB format */
	pColorTable = (DWORD *) (((LPBYTE) lpBitmapInfo) + (UINT) lpBitmapInfo->biSize);

	for(int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		/* look for matching RGBQUAD color in original */
		for(int i = 0; i < _countof(_afxSysColorMap); i++)
		{
			if(pColorTable[iColor] == _afxSysColorMap[i].rgbqFrom)
			{
				if(bMono)
				{
					/* all colors except text become white */
					if(_afxSysColorMap[i].iSysColorTo != COLOR_BTNTEXT)
						pColorTable[iColor] = RGB_TO_RGBQUAD(255, 255, 255);
				}
				else
					pColorTable[iColor] = CLR_TO_RGBQUAD(::GetSysColor(_afxSysColorMap[i].iSysColorTo));
				break;
			}
		}
	}

	nWidth = (int) lpBitmapInfo->biWidth;
	nHeight = (int) lpBitmapInfo->biHeight;
	hDCScreen = ::GetDC(NULL);
	hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if(hbm != NULL)
	{
		/*~~~~~~~~~~~~~~*/
		HDC		hDCGlyphs;
		HBITMAP hbmOld;

		LPBYTE	lpBits;
		/*~~~~~~~~~~~~~~*/

		hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		lpBits = (LPBYTE) (lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits
		(
			hDCGlyphs,
			0,
			0,
			nWidth,
			nHeight,
			0,
			0,
			nWidth,
			nHeight,
			lpBits,
			(LPBITMAPINFO) lpBitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}

	::ReleaseDC(NULL, hDCScreen);

	/* free copy of bitmap info struct and resource itself */
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HBITMAP EMEN_cl_ToolBar::LoadMappedFullColorBitmap1(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HGLOBAL				hglb;
	LPBITMAPINFOHEADER	lpBitmap;
	int					nColorTableSize;
	UINT				nSize;
	LPBITMAPINFOHEADER	lpBitmapInfo;
	DWORD				*pColorTable;

	int					nWidth;
	int					nHeight;
	HDC					hDCScreen;
	HBITMAP				hbm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((hglb = LoadResource(hInst, hRsrc)) == NULL) return NULL;

	lpBitmap = (LPBITMAPINFOHEADER) LockResource(hglb);

	if(lpBitmap == NULL) return NULL;

	/* make copy of BITMAPINFOHEADER so we can modify the color table */
	if(lpBitmap->biBitCount <= 8)
		nColorTableSize = (1 << lpBitmap->biBitCount);
	else
		nColorTableSize = 0;	/* No palette needed for 24 BPP DIB */
	nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);

	if(lpBitmapInfo == NULL) return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	/* color table is in RGBQUAD DIB format */
	pColorTable = (DWORD *) (((LPBYTE) lpBitmapInfo) + (UINT) lpBitmapInfo->biSize);

	for(int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		if(pColorTable[iColor] == RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0))
		{
//			pColorTable[iColor] = CLR_TO_RGBQUAD(::GetSysColor(COLOR_BTNFACE));
		}
		else
		{
			int R = pColorTable[iColor] & 0xFF;
			int G = (pColorTable[iColor] >> 8) & 0xFF;
			int B = (pColorTable[iColor] >> 16) & 0xFF;
			float I = (0.596f * R) + (-0.275f * G) + (-0.321f * B);
//			pColorTable[iColor] = CLR_TO_RGBQUAD(COLOR_ul_Blend(pColorTable[iColor], 0x00FFFFFF, 0.5f));

			if(I)
				R = (int) (0xBB * (40 / I));
			else
				R = 0;
			pColorTable[iColor] = (R << 16) + (R << 8) + R;
		}
	}

	nWidth = (int) lpBitmapInfo->biWidth;
	nHeight = (int) lpBitmapInfo->biHeight;
	hDCScreen = ::GetDC(NULL);
	hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if(hbm != NULL)
	{
		/*~~~~~~~~~~~~~~*/
		HDC		hDCGlyphs;
		HBITMAP hbmOld;

		LPBYTE	lpBits;
		/*~~~~~~~~~~~~~~*/

		hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		lpBits = (LPBYTE) (lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits
		(
			hDCGlyphs,
			0,
			0,
			nWidth,
			nHeight,
			0,
			0,
			nWidth,
			nHeight,
			lpBits,
			(LPBITMAPINFO) lpBitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}

	::ReleaseDC(NULL, hDCScreen);

	/* free copy of bitmap info struct and resource itself */
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}

void EMEN_cl_ToolBar::ComputeDisabledBar(void)
{
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMEN_cl_ToolBar::LoadFullColorToolBar(LPCTSTR lpszResourceName)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	BOOL			bResult;
	UINT			*pItems;
	CToolBarData	*pData;
	HGLOBAL			hGlobal;
	HINSTANCE		hInst;
	HRSRC			hRsrc;
	/*~~~~~~~~~~~~~~~~~~~~*/

	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

	/* determine location of the bitmap in resource fork */
	hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);

	if(hRsrc == NULL) return FALSE;

	hGlobal = LoadResource(hInst, hRsrc);

	if(hGlobal == NULL) return FALSE;

	pData = (CToolBarData *) LockResource(hGlobal);

	if(pData == NULL) return FALSE;
	ASSERT(pData->wVersion == 1);

	pItems = new UINT[pData->wItemCount];

	for(int i = 0; i < pData->wItemCount; i++) pItems[i] = pData->items()[i];
	bResult = SetButtons(pItems, pData->wItemCount);

	delete[]pItems;

	if(bResult)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* set new sizes of the buttons */
		CSize	sizeImage(pData->wWidth, pData->wHeight);
		CSize	sizeButton(pData->wWidth + 7, pData->wHeight + 7);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		SetSizes(sizeButton, sizeImage);

		/* load bitmap now that sizes are known by the toolbar control */
		bResult = LoadFullColorBitmap(lpszResourceName);
	}

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	ComputeDisabledBar();

	return bResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMEN_cl_ToolBar::LoadFullColorBitmap(LPCTSTR lpszResourceName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	HBITMAP		hbmImageWell;
	HINSTANCE	hInstImageWell;
	HRSRC		hRsrcImageWell;
#if 0
	CBitmap	*p;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

	/* determine location of the bitmap in resource fork */
	hInstImageWell = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	hRsrcImageWell = ::FindResource(hInstImageWell, lpszResourceName, RT_BITMAP);

	if(hRsrcImageWell == NULL) return FALSE;

	/* load the bitmap */
	hbmImageWell = LoadMappedFullColorBitmap(hInstImageWell, hRsrcImageWell, FALSE);

	/* tell common control toolbar about the new bitmap */
	if(!AddReplaceBitmap(hbmImageWell)) return FALSE;

#if 0
	mo_DisabledIL.Create(16, 16, ILC_COLOR8 | ILC_MASK, 200, 0);
	hbmImageWell = LoadMappedFullColorBitmap1(hInstImageWell, hRsrcImageWell, FALSE);
	p = CBitmap::FromHandle(hbmImageWell);
	mo_DisabledIL.Add(p, RGB(0xc0,0xc0,0xc0));
	GetToolBarCtrl().SetDisabledImageList(&mo_DisabledIL);
#endif

	/* remember the resource handles so the bitmap can be recolored if necessary */
	m_hInstImageWell = hInstImageWell;
	m_hRsrcImageWell = hRsrcImageWell;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_ToolBar::OnSysColorChange(void)
{
	/* re-color bitmap for toolbar */
	if(m_hInstImageWell != NULL && m_hbmImageWell != NULL)
	{
		/*~~~~~~~~~~~*/
		HBITMAP hbmNew;
#if 0
		CBitmap *p;
#endif
		/*~~~~~~~~~~~*/

		hbmNew = LoadMappedFullColorBitmap(m_hInstImageWell, m_hRsrcImageWell, FALSE);
		if(hbmNew != NULL) AddReplaceBitmap(hbmNew);

#if 0
		mo_DisabledIL.DeleteImageList();
		mo_DisabledIL.Create(16, 16, ILC_COLOR8, 200, 0);
		hbmNew = LoadMappedFullColorBitmap1(m_hInstImageWell, m_hRsrcImageWell, FALSE);
		p = CBitmap::FromHandle(hbmNew);
		mo_DisabledIL.Add(p, RGB(0, 0, 0));
		GetToolBarCtrl().SetDisabledImageList(&mo_DisabledIL);
#endif
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_ToolBar::OnSize(UINT c, int a, int b)
{
	/*~~~~~~~~~~~~~*/

	/* get parent window (there should be one) */
	CWnd	*pParent;
	/*~~~~~~~~~~~~~*/

	pParent = GetParent();

	if(pParent)
	{
		/*~~~~~~~*/

		/* get rect for this toolbar */
		CRect	rw;
		CRect	rc;
		/*~~~~~~~*/

		GetWindowRect(&rw);

		/* convert rect to parent coords */
		rc = rw;

		pParent->ScreenToClient(&rc);

		/* invalidate this part of parent */
		pParent->InvalidateRect(&rc);

		/* now do all the other toolbars (etc) that belong to the parent */
		for(CWnd * pSibling = pParent->GetWindow(GW_CHILD); pSibling; pSibling = pSibling->GetNextWindow(GW_HWNDNEXT))
		{
			/*~~~~~~~*/
			CRect	rc; /* but do not draw ourselves */
			/*~~~~~~~*/

			if(pSibling == this) continue;

			/* convert rect to siblings coords */
			rc = rw;

			pSibling->ScreenToClient(&rc);

			/* invalidate this part of sibling */
			pSibling->InvalidateRect(&rc);
		}
	}

	CToolBar::OnSize(c, a, b);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_ToolBar::OnPaint(void)
{
	/* standard tolbar */
	CToolBar::OnPaint();

	/* erase the background */
	EraseNonClient();
}

/*
 =======================================================================================================================
    Erase the non-client area (borders) - copied from MFC implementation
 =======================================================================================================================
 */
void EMEN_cl_ToolBar::EraseNonClient(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/

	/* get window DC that is clipped to the non-client area */
	CWindowDC	dc(this);
	CRect		rectClient;
	CRect		rectWindow;
	/*~~~~~~~~~~~~~~~~~~~*/

	GetClientRect(rectClient);
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);

	/* draw borders in non-client area */
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);

	/* erase parts not drawn */
	dc.IntersectClipRect(rectWindow);
	SendMessage(WM_ERASEBKGND, (WPARAM) dc.m_hDC);

	DrawGripper(&dc, rectWindow);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EMEN_cl_InMenu, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_COMMAND_RANGE(0, 0xFFFF, OnCommand)
	ON_UPDATE_COMMAND_UI_RANGE(0, 0xFFFF, OnCommandUI)
END_MESSAGE_MAP()



/*
 =======================================================================================================================
 =======================================================================================================================
 */
EMEN_cl_InMenu::~EMEN_cl_InMenu(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EMEN_cl_InMenu::OnCreate(LPCREATESTRUCT lpcs)
{
	CFrameWnd::OnCreate(lpcs);

	if(!mpo_Parent->mb_LeftMode)
		mo_Toolbar.Create(this,  CBRS_TOOLTIPS | WS_CHILD | CBRS_TOP | WS_VISIBLE);
	else
		mo_Toolbar.Create(this,  CBRS_TOOLTIPS | WS_CHILD | CBRS_LEFT | WS_VISIBLE);
	mo_Toolbar.ModifyStyle(0, TBSTYLE_TRANSPARENT | TBSTYLE_FLAT);
	mo_Toolbar.LoadFullColorToolBar(mui_ResTB);
	mo_Toolbar.MoveWindow(0, -6, 100, 100);
    mo_Toolbar.EnableTrackingToolTips(TRUE);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMEN_cl_InMenu::OnEraseBkgnd(CDC *pDC)
{
	/*~~~~~~~~~~~~~~~*/
	CRect		o_Rect;
	COLORREF	xCol;
	/*~~~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);
	xCol = GetSysColor(COLOR_BTNFACE);
	o_Rect.right += 20;
	o_Rect.bottom += 20;
	pDC->FillSolidRect(o_Rect, xCol);
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    To avoid toolbar to be pos above menu (born X position).
 =======================================================================================================================
 */
void EMEN_cl_InMenu::OnSizing(UINT uiID, LPRECT prect)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	CFrameWnd::OnSizing(uiID, prect);

	o_Rect = prect;
	GetParent()->ScreenToClient(&o_Rect);

	if(!mpo_Parent->mb_LeftMode)
	{
		if(o_Rect.left < mpo_Parent->mi_MinPosInMenu)
		{
			o_Rect.left = mpo_Parent->mi_MinPosInMenu;
			GetParent()->ClientToScreen(&o_Rect);
			prect->left = o_Rect.left;
			if(prect->left >= prect->right)
				ShowWindow(SW_HIDE);
			else if(!IsWindowVisible()) ShowWindow(SW_SHOW);
		}
		else if(!IsWindowVisible()) ShowWindow(SW_SHOW);

		mpo_Parent->mi_PosXInMenu = (o_Rect.right - o_Rect.left) + 1;
	}
	else
	{
		if(o_Rect.top < mpo_Parent->mi_MinPosInMenu)
		{
			o_Rect.top = mpo_Parent->mi_MinPosInMenu;
			GetParent()->ClientToScreen(&o_Rect);
			prect->top = o_Rect.top;
			if(prect->top >= prect->bottom)
				ShowWindow(SW_HIDE);
			else if(!IsWindowVisible()) ShowWindow(SW_SHOW);
		}
		else if(!IsWindowVisible()) ShowWindow(SW_SHOW);

		mpo_Parent->mi_PosXInMenu = (o_Rect.bottom - o_Rect.top) + 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_InMenu::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	CFrameWnd::OnSize(nType, cx, cy);

	GetWindowRect(&o_Rect);
	if(o_Rect.Width() < 25)
		mo_Toolbar.ShowWindow(SW_HIDE);
	else
		mo_Toolbar.ShowWindow(SW_SHOW);

	if(!mpo_Parent || !mpo_Parent->mb_LeftMode)
		mo_Toolbar.MoveWindow(0, -3, cx, 100);
	else
		mo_Toolbar.MoveWindow(-3, 0, 100, cy);

	GetParent()->ScreenToClient(&o_Rect);
	o_Rect.top -= 50;
	o_Rect.left -= 50;
	GetParent()->InvalidateRect(&o_Rect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT EMEN_cl_InMenu::OnNcHitTest(CPoint pt)
{
	/*~~~~~~~~~~~*/
	UINT	ui_Hit;
	/*~~~~~~~~~~~*/

	ui_Hit = CFrameWnd::OnNcHitTest(pt);
	switch(ui_Hit)
	{
	case HTTOPRIGHT:
	case HTBOTTOMRIGHT:
	case HTRIGHT:
	case HTTOP:
	case HTTOPLEFT:
	case HTBOTTOMLEFT:
	case HTBOTTOM:
		if(!mpo_Parent->mb_LeftMode)
			return HTLEFT;
		else
			return HTTOP;
	}

	return ui_Hit;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_InMenu::OnCommand(UINT uiID)
{
	if(mpo_Parent->mpo_Editor)
		mpo_Parent->mpo_Editor->OnToolBarCommand(uiID);
	else
		M_MF()->OnToolBarCommand(uiID);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMEN_cl_InMenu::OnCommandUI(CCmdUI *pui)
{
	if(mpo_Parent->mpo_Editor)
		mpo_Parent->mpo_Editor->OnToolBarCommandUI(pui->m_nID, pui);
	else
		M_MF()->OnToolBarCommandUI(pui->m_nID, pui);
}

#endif /* ACTIVE_EDITORS */
