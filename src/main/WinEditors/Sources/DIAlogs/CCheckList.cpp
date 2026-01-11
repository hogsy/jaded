/*$T CCheckList.cpp GC 1.139 01/18/03 22:44:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "CCheckList.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(CCheckList, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CCheckList::CCheckList(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	NONCLIENTMETRICS	nm;
	/*~~~~~~~~~~~~~~~~~~~*/

	/* Font système de base */
	nm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0);
	mo_Font.CreateFontIndirect(&nm.lfMenuFont);
	mi_PosY = 0;
	mpo_ImageList = NULL;
	mi_Selected = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CCheckList::i_GetHeight(void)
{
	return 16;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCheckList::AddString(CString s, int i, BOOL b)
{
	mo_ListItems.AddTail(s);
	mo_ListIcons.AddTail(i);
	mo_ListEnabled.AddTail(b);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CCheckList::Do(CRect *rect, int width, CWnd *parent, int max, char *_pz_Caption)
{
	/*~~~~~~~~~~~~~~~*/
	CRect		o_Rect;
	int			height;
	MSG			m;
	int			cx;
	CPoint		pt1;
	POSITION	pos;
	CString		o_Str;
	CPoint		pt;
	short		zDelta;
    BOOL        bValid = TRUE;
	/*~~~~~~~~~~~~~~~*/

	ReleaseCapture();
	mi_MaxLines = max;
	mb_Save = 0;
	if(_pz_Caption)
	{
		mb_Caption = TRUE;
		Create(NULL, NULL, WS_CAPTION | WS_POPUP | WS_VSCROLL, CRect(0, 0, 0, 0), parent, NULL, WS_EX_TOOLWINDOW);
		SetWindowText(_pz_Caption);
	}
	else
	{
		mb_Caption = FALSE;
		Create(NULL, NULL, WS_POPUP | WS_VSCROLL, CRect(0, 0, 0, 0), parent);
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	}

	/* Hauteur */
	if(mi_MaxLines > mo_ListItems.GetCount())
		height = (i_GetHeight() * mo_ListItems.GetCount()) + 40;
	else
		height = (i_GetHeight() * mi_MaxLines) + 40;
	height += 2;	/* Bords de la boite */
	height += 6;	/* Bords dessiné autour du contenu */

	if(mb_Caption) height += GetSystemMetrics(SM_CYCAPTION);

	/* Les séparateurs prennent moins de place */
	pos = mo_ListItems.GetHeadPosition();
	while(pos)
	{
		o_Str = mo_ListItems.GetNext(pos);
		if(o_Str.IsEmpty()) height -= 8;
	}

	/* Scrollbar ? */
	width += GetSystemMetrics(SM_CXVSCROLL);
	if(mi_MaxLines < mo_ListItems.GetCount())
	{
		EnableScrollBar(SB_VERT);
		ShowScrollBar(SB_VERT);
		SetScrollRange(SB_VERT, 0, mo_ListItems.GetCount() - mi_MaxLines);
	}
	else
	{
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	}

	if(_pz_Caption)
	{
		pt.x = rect->left;
		pt.y = rect->top;
	}
	else
	{
		/* Ca dépasse ? */
		pt.x = rect->left;
		pt.y = rect->bottom;
		cx = GetSystemMetrics(SM_CXMAXIMIZED) - 5;
		if(pt.x + width > cx) pt.x = rect->right - width;
		cx = GetSystemMetrics(SM_CYMAXIMIZED) - 5;
		if(pt.y + height > cx) pt.y = rect->top - height;
	}

	o_Rect.left = pt.x;
	o_Rect.top = pt.y;
	o_Rect.right = o_Rect.left + width;
	o_Rect.bottom = o_Rect.top + height;

	/* Position */
	::SetWindowPos
	(
		m_hWnd,
		HWND_TOPMOST,
		o_Rect.left,
		o_Rect.top,
		o_Rect.Width(),
		o_Rect.Height(),
		SWP_NOACTIVATE | SWP_SHOWWINDOW
	);

	ShowWindow(SW_SHOW);

	if(mb_Caption) SetFocus();

	/* Modal */
	while(::GetMessage(&m, NULL, NULL, NULL))
	{
		/* Sortie si on appuie sur OK */
		if (mb_Save == 1)
		{
			bValid = TRUE;
			return bValid;
		}

		if (mb_Save == 2)
		{
			bValid = FALSE;
			return bValid;
		}

		TranslateMessage(&m);

		/* Sortie */
		if(m.message == WM_KEYDOWN && m.wParam == VK_ESCAPE)
        {
            bValid = FALSE;
            break;
        }
		if(m.message == WM_KEYDOWN && m.wParam == VK_RETURN) break;

		/* Sortie si click hors de la fenêtre */
		if(m.message == WM_MOUSEWHEEL)
		{
			zDelta = m.wParam >> 16;
			OnVScroll(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
			continue;
		}

		/* Sortie si click hors de la fenêtre */
		if(m.message == WM_LBUTTONDOWN)
		{
			GetCursorPos(&pt);
			GetWindowRect(&o_Rect);
			if(!o_Rect.PtInRect(pt))
			{
				if(mb_Caption) continue;
				break;
			}
		}

		/* Sortie si click bouton droit */
		if(m.message == WM_RBUTTONDOWN) break;

		/* Mouse move ? */
		if(m.message == WM_MOUSEMOVE)
		{
			if(mi_Selected != -1)
			{
				GetCursorPos(&pt1);
				ScreenToClient(&pt1);
				switch(mi_Selected)
				{
				case -2:	o_Rect = mo_RectAll; break;
				case -3:	o_Rect = mo_RectNone; break;
				case -4:	o_Rect = mo_RectInvert; break;
				case -5:	o_Rect = mo_RectOK; break;
				case -6:	o_Rect = mo_RectCancel; break;
				default:	o_Rect = mo_ListRects.GetAt(mo_ListRects.FindIndex(mi_Selected)); break;
				}

				if(!o_Rect.PtInRect(pt1)) Invalidate();
			}
			else
			{
				GetClientRect(&o_Rect);
				GetCursorPos(&pt1);
				ScreenToClient(&pt1);
				if(o_Rect.PtInRect(pt1)) Invalidate();
			}

			continue;
		}

		DispatchMessage(&m);
	}

	GetWindowRect(rect);
	ShowWindow(SW_HIDE);
    return bValid;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT CCheckList::DefWindowProc(UINT msg, WPARAM w, LPARAM l)
{
	if(msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
	return CFrameWnd::DefWindowProc(msg, w, l);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CCheckList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CFrameWnd::OnCreate(lpCreateStruct);
	return 0;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CCheckList::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCheckList::OnDestroy(void)
{
	mo_Font.DeleteObject();
	mo_ListItems.RemoveAll();
	mo_ListIcons.RemoveAll();
	mo_ListRects.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCheckList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *)
{
	/*~~~~*/
	int num;
	int i;
	/*~~~~*/

	num = 1;
	switch(nSBCode)
	{
	case SB_PAGEDOWN:
		num = mi_MaxLines / 2;

	case SB_LINEDOWN:
		for(i = 0; i < num; i++)
		{
			if(mi_MaxLines + (mi_PosY / i_GetHeight()) < mo_ListItems.GetCount())
			{
				mi_PosY += i_GetHeight();
				SetScrollPos(SB_VERT, mi_PosY / i_GetHeight());
				Invalidate();
			}
		}
		break;

	case SB_PAGEUP:
		num = mi_MaxLines / 2;

	case SB_LINEUP:
		for(i = 0; i < num; i++)
		{
			if(mi_PosY)
			{
				mi_PosY -= i_GetHeight();
				SetScrollPos(SB_VERT, mi_PosY / i_GetHeight());
				Invalidate();
			}
		}
		break;

	case SB_THUMBTRACK:
		mi_PosY = nPos * i_GetHeight();
		SetScrollPos(SB_VERT, mi_PosY / i_GetHeight());
		Invalidate();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static ULONG ABGR_u32_Interpol2Colors(ULONG P1, ULONG P2, float ZClipLocalCoef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ZClipLocalCoef >= 0.98f) return P2;
	if(ZClipLocalCoef <= 0.02f) return P1;
	*((float *) &Interpoler) = ZClipLocalCoef + 32768.0f + 16384.0f;
	RetValue = (Interpoler & 128) ? (P2 & 0xfefefefe) >> 1 : (P1 & 0xfefefefe) >> 1;
	RetValue += (Interpoler & 64) ? (P2 & 0xfcfcfcfc) >> 2 : (P1 & 0xfcfcfcfc) >> 2;
	RetValue += (Interpoler & 32) ? (P2 & 0xf8f8f8f8) >> 3 : (P1 & 0xf8f8f8f8) >> 3;
	RetValue += (Interpoler & 16) ? (P2 & 0xf0f0f0f0) >> 4 : (P1 & 0xf0f0f0f0) >> 4;
	RetValue += (Interpoler & 8) ? (P2 & 0xe0e0e0e0) >> 5 : (P1 & 0xe0e0e0e0) >> 5;
	RetValue += (Interpoler & 4) ? (P2 & 0xc0c0c0c0) >> 6 : (P1 & 0xc0c0c0c0) >> 6;
	RetValue += (Interpoler & 2) ? (P2 & 0x80808080) >> 7 : (P1 & 0x80808080) >> 7;
	return RetValue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCheckList::DrawCheckMark(CDC *pDC, int x, int y, COLORREF color)
{
	pDC->SetPixel(x, y + 2, color);
	pDC->SetPixel(x, y + 3, color);
	pDC->SetPixel(x, y + 4, color);

	pDC->SetPixel(x + 1, y + 3, color);
	pDC->SetPixel(x + 1, y + 4, color);
	pDC->SetPixel(x + 1, y + 5, color);

	pDC->SetPixel(x + 2, y + 4, color);
	pDC->SetPixel(x + 2, y + 5, color);
	pDC->SetPixel(x + 2, y + 6, color);

	pDC->SetPixel(x + 3, y + 3, color);
	pDC->SetPixel(x + 3, y + 4, color);
	pDC->SetPixel(x + 3, y + 5, color);

	pDC->SetPixel(x + 4, y + 2, color);
	pDC->SetPixel(x + 4, y + 3, color);
	pDC->SetPixel(x + 4, y + 4, color);

	pDC->SetPixel(x + 5, y + 1, color);
	pDC->SetPixel(x + 5, y + 2, color);
	pDC->SetPixel(x + 5, y + 3, color);

	pDC->SetPixel(x + 6, y, color);
	pDC->SetPixel(x + 6, y + 1, color);
	pDC->SetPixel(x + 6, y + 2, color);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static COLORREF MidColor(COLORREF ColA, COLORREF ColB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int Red = MulDiv(7, GetRValue(ColA), 10) + MulDiv(3, GetRValue(ColB), 10);
	int Green = MulDiv(7, GetGValue(ColA), 10) + MulDiv(3, GetGValue(ColB), 10);
	int Blue = MulDiv(7, GetBValue(ColA), 10) + MulDiv(3, GetBValue(ColB), 10);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	return RGB(Red, Green, Blue);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCheckList::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION	pos, pos1, pos2;
	CString		o_Str;
	CPaintDC	tdc(this);
	CRect		o_Pos, o_Pos1, o_Pos2;
	CRect		o_Pos3;
	int			height;
	int			index, drawindex;
	int			selindex;
	CFont		*poldfont;
	int			icon;
	BOOL		enabled;
	int			flag;
	CPoint		pt;
	COLORREF	hi, face, sel, txt, dk;
	COLORREF	face1;
	BOOL		selected;
	CDC			dc;
	CBitmap		bitmap;
	CBitmap		*pOldBitmap;
	COLORREF	mid;
	BOOL		b_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Anti flick debut */
	GetClientRect(&o_Pos1);
	dc.CreateCompatibleDC(&tdc);
	bitmap.CreateCompatibleBitmap(&tdc, o_Pos1.Width()+60, o_Pos1.Height()+60);
	pOldBitmap = dc.SelectObject(&bitmap);

	GetCursorPos(&pt);
	ScreenToClient(&pt);
	hi = GetSysColor(COLOR_3DHILIGHT);
	sel = GetSysColor(COLOR_HIGHLIGHT);
	txt = GetSysColor(COLOR_MENUTEXT);
	dk = GetSysColor(COLOR_3DSHADOW);
	face = GetSysColor(COLOR_MENU);
	face1 = ABGR_u32_Interpol2Colors(face, dk, 0.1f);
	mid = MidColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_HIGHLIGHT));

	index = 0;
	drawindex = 0;
	selindex = 0;
	poldfont = dc.SelectObject(&mo_Font);

	mo_ListRects.RemoveAll();
	pos = mo_ListItems.GetHeadPosition();
	pos1 = mo_ListIcons.GetHeadPosition();
	pos2 = mo_ListEnabled.GetHeadPosition();

	GetClientRect(&o_Pos);
	o_Pos.top -= mi_PosY;
	o_Pos.left += 3;
	o_Pos.top += 3;

	mi_Selected = -1;
	while(pos)
	{
		o_Str = mo_ListItems.GetNext(pos);
		icon = mo_ListIcons.GetNext(pos1);
		enabled = mo_ListEnabled.GetNext(pos2);

		/* Position courante */
		height = 16;
		if(o_Str.IsEmpty()) height = 8;
		o_Pos.bottom = o_Pos.top + height;
		o_Pos1 = o_Pos;
		mo_ListRects.AddTail(o_Pos);
		if(o_Pos.top >= 0)
		{
			/* Separateur */
			if(o_Str.IsEmpty())
			{
				o_Pos2 = o_Pos1;
				o_Pos2.right = o_Pos1.left + 16;
				if(mpo_ImageList) o_Pos2.right += 24;
				dc.FillSolidRect(o_Pos2, face1);
				o_Pos2.left = o_Pos2.right;
				o_Pos2.right = o_Pos1.right;
				dc.FillSolidRect(o_Pos2, face);

				o_Pos1.top += (o_Pos1.Height() / 2) - 1;
				o_Pos1.bottom = o_Pos1.top + 2;
				o_Pos1.left++;
				o_Pos1.right--;
				dc.Draw3dRect(&o_Pos1, dk, hi);
				mo_ListRects.RemoveTail();
				selindex--;
			}
			else
			{
				/* Souris dessus ? */
				selected = o_Pos.PtInRect(pt);
				if(selected) mi_Selected = selindex;

				/* Checkbox */
				o_Pos2 = o_Pos1;
				o_Pos2.right = o_Pos2.left + o_Pos2.Height();
				flag = DFCS_BUTTONCHECK;
				if(enabled)
					flag |= mpb_Checks[index] ? DFCS_CHECKED : 0;
				else
					flag |= DFCS_INACTIVE;

				dc.FillSolidRect(&o_Pos2, face1);
				if(flag & DFCS_CHECKED)
				{
					DrawCheckMark(&dc, o_Pos2.left + 4, o_Pos2.top + 2, txt);
				}

				if(icon != -1 && mpo_ImageList)
				{
					o_Pos3 = o_Pos2;
					o_Pos3.left = o_Pos2.right;
					o_Pos3.right = o_Pos3.left + 24;
					dc.FillSolidRect(o_Pos3, face1);
					mpo_ImageList->Draw(&dc, icon, CPoint(o_Pos3.left + 4, o_Pos3.top), ILD_TRANSPARENT);
					o_Pos2.right += 24;
				}
				else if(mpo_ImageList)
				{
					o_Pos3 = o_Pos2;
					o_Pos3.left = o_Pos2.right;
					o_Pos3.right = o_Pos3.left + 24;
					dc.FillSolidRect(o_Pos3, face1);
					o_Pos2.right += 24;
				}

				/* Texte */
				o_Pos1.left = o_Pos2.right;
				o_Pos1.right -= 3;
				o_Pos2 = o_Pos1;
				o_Pos2.right = o_Pos2.left + 3;
				dc.FillSolidRect(&o_Pos2, face);
				o_Pos1.left += 3;
				if(enabled)
				{
					dc.SetTextColor(txt);
					dc.SetBkColor(selected ? mid : face);
					dc.ExtTextOut(o_Pos1.left + 6, o_Pos2.top, ETO_OPAQUE, &o_Pos1, o_Str, NULL);
					if(selected)
						dc.Draw3dRect(o_Pos1, GetSysColor(COLOR_HIGHLIGHT), GetSysColor(COLOR_HIGHLIGHT));
				}
				else
				{
					dc.SetBkColor(selected ? sel : face);
					dc.SetTextColor(hi);
					dc.ExtTextOut(o_Pos1.left + 7, o_Pos2.top + 1, ETO_OPAQUE, &o_Pos1, o_Str, NULL);
					dc.SetTextColor(GetSysColor(COLOR_3DSHADOW));
					dc.SetBkMode(TRANSPARENT);
					dc.ExtTextOut(o_Pos1.left + 6, o_Pos2.top, 0, &o_Pos1, o_Str, NULL);
				}

				index++;
			}
		}
		else if(!o_Str.IsEmpty())
			index++;

		selindex++;
		drawindex++;

		/* Nouvelle ligne */
		o_Pos.top += height;

		/* On dépasse en bas */
		if(drawindex == mi_MaxLines + (mi_PosY / i_GetHeight())) break;
	}

	/* Bord */
	GetClientRect(&o_Pos1);
	o_Pos1.bottom -= 20;
	dc.Draw3dRect(&o_Pos1, face, face);
	o_Pos1.InflateRect(-1, -1);
	dc.Draw3dRect(&o_Pos1, face, face);
	o_Pos1.InflateRect(-1, -1);
	dc.Draw3dRect(&o_Pos1, face, face);
	o_Pos1.InflateRect(-1, -1);

	/* Le reste */
	GetClientRect(&o_Pos1);
	o_Pos1.top = o_Pos.bottom;
	o_Pos1.bottom -= 20;
	if(o_Pos1.bottom > o_Pos1.top) dc.FillSolidRect(&o_Pos1, face);

	/* Boutons */
	GetClientRect(&o_Pos1);
	o_Pos1.bottom -= 20;
	o_Pos1.top = o_Pos1.bottom - 20;
	o_Pos1.top++;
	o_Pos1.bottom--;

	/* All */
	o_Pos2 = o_Pos1;
	o_Pos2.right /= 3;
	o_Pos2.top++;
	o_Pos2.left += 2;
	o_Pos2.right -= 2;
	b_Sel = o_Pos2.PtInRect(pt);
	dc.Draw3dRect(o_Pos2, b_Sel ? sel : dk, b_Sel ? sel : dk);
	dc.SetTextAlign(TA_CENTER);
	o_Pos2.InflateRect(-1, -1);
	dc.SetBkColor(b_Sel ? mid : face);
	mi_Selected = b_Sel ? -2 : mi_Selected;
	dc.SetTextColor(txt);
	dc.ExtTextOut(o_Pos2.left + o_Pos2.Width() / 2, o_Pos2.top, ETO_OPAQUE, &o_Pos2, "All", NULL);
	o_Pos2.InflateRect(1, 1);
	dc.ExcludeClipRect(&o_Pos2);
	mo_RectAll = o_Pos2;

	/* None */
	o_Pos2.left = o_Pos2.right + 4;
	o_Pos2.right = o_Pos2.left + (o_Pos1.Width() / 3) - 4;
	b_Sel = o_Pos2.PtInRect(pt);
	dc.Draw3dRect(o_Pos2, b_Sel ? sel : dk, b_Sel ? sel : dk);
	dc.SetTextAlign(TA_CENTER);
	o_Pos2.InflateRect(-1, -1);
	dc.SetBkColor(b_Sel ? mid : face);
	mi_Selected = o_Pos2.PtInRect(pt) ? -3 : mi_Selected;
	dc.SetTextColor(txt);
	dc.ExtTextOut(o_Pos2.left + o_Pos2.Width() / 2, o_Pos2.top, ETO_OPAQUE, &o_Pos2, "None", NULL);
	o_Pos2.InflateRect(1, 1);
	dc.ExcludeClipRect(&o_Pos2);
	mo_RectNone = o_Pos2;

	/* Invert */
	o_Pos2.left = o_Pos2.right + 4;
	o_Pos2.right = o_Pos2.left + (o_Pos1.Width() / 3) - 4;
	b_Sel = o_Pos2.PtInRect(pt);
	dc.Draw3dRect(o_Pos2, b_Sel ? sel : dk, b_Sel ? sel : dk);
	dc.SetTextAlign(TA_CENTER);
	o_Pos2.InflateRect(-1, -1);
	dc.SetBkColor(b_Sel ? mid : face);
	mi_Selected = o_Pos2.PtInRect(pt) ? -4 : mi_Selected;
	dc.SetTextColor(txt);
	dc.ExtTextOut(o_Pos2.left + o_Pos2.Width() / 2, o_Pos2.top, ETO_OPAQUE, &o_Pos2, "Invert", NULL);
	o_Pos2.InflateRect(1, 1);
	dc.ExcludeClipRect(&o_Pos2);
	mo_RectInvert = o_Pos2;

	/* OK */
	o_Pos2.left = o_Pos2.right - 3*((o_Pos1.Width() / 3)) + 4;
	o_Pos2.right = o_Pos2.left + (o_Pos1.Width() / 3) - 4;
	o_Pos2.top = o_Pos2.top + 20;
	o_Pos2.bottom = o_Pos2.bottom + 20;
	b_Sel = o_Pos2.PtInRect(pt);
	dc.Draw3dRect(o_Pos2, b_Sel ? sel : dk, b_Sel ? sel : dk);
	dc.SetTextAlign(TA_CENTER);
	o_Pos2.InflateRect(-1, -1);
	dc.SetBkColor(b_Sel ? mid : face);
	mi_Selected = o_Pos2.PtInRect(pt) ? -5 : mi_Selected;
	dc.SetTextColor(txt);
	dc.ExtTextOut(o_Pos2.left + o_Pos2.Width() / 2, o_Pos2.top, ETO_OPAQUE, &o_Pos2, "OK", NULL);
	o_Pos2.InflateRect(1, 1);
	dc.ExcludeClipRect(&o_Pos2);
	mo_RectOK = o_Pos2;

	/* Cancel */
	o_Pos2.left = o_Pos2.right + 4;
	o_Pos2.right = o_Pos2.left + (o_Pos1.Width() / 3) - 4;
	b_Sel = o_Pos2.PtInRect(pt);
	dc.Draw3dRect(o_Pos2, b_Sel ? sel : dk, b_Sel ? sel : dk);
	dc.SetTextAlign(TA_CENTER);
	o_Pos2.InflateRect(-1, -1);
	dc.SetBkColor(b_Sel ? mid : face);
	mi_Selected = o_Pos2.PtInRect(pt) ? -6 : mi_Selected;
	dc.SetTextColor(txt);
	dc.ExtTextOut(o_Pos2.left + o_Pos2.Width() / 2, o_Pos2.top, ETO_OPAQUE, &o_Pos2, "Cancel", NULL);
	o_Pos2.InflateRect(1, 1);
	dc.ExcludeClipRect(&o_Pos2);
	mo_RectCancel = o_Pos2;

	/* Entre les boutons */
	GetClientRect(&o_Pos1);
	o_Pos1.top = o_Pos1.bottom - 20;
	dc.FillSolidRect(o_Pos1, face);

	dc.SelectObject(poldfont);

	/* Anti flick fin */
	GetClientRect(&o_Pos1);
	tdc.BitBlt(o_Pos1.left, o_Pos1.top, o_Pos1.Width(), o_Pos1.Height(), &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOldBitmap);
	dc.DeleteDC();
	bitmap.DeleteObject();
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCheckList::OnLButtonDown(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~*/
	POSITION	pos;
	CRect		o_Rect;
	int			index;
	/*~~~~~~~~~~~~~~~*/

	mb_Save = 0;

	/* Dans une checkbox ? */
	index = 0;
	pos = mo_ListRects.GetHeadPosition();
	while(pos)
	{
		o_Rect = mo_ListRects.GetNext(pos);
		if(o_Rect.PtInRect(pt))
		{
			if(mo_ListEnabled.GetAt(mo_ListEnabled.FindIndex(index)))
			{
				mpb_Checks[index] = mpb_Checks[index] ? FALSE : TRUE;
				Invalidate();
			}

			return;
		}

		index++;
	}

	/* Boutons */
	if(mo_RectAll.PtInRect(pt))
	{
		for(index = 0; index < mo_ListEnabled.GetCount(); index++)
		{
			if(mo_ListEnabled.GetAt(mo_ListEnabled.FindIndex(index))) mpb_Checks[index] = TRUE;
		}

		Invalidate();
		return;
	}

	if(mo_RectNone.PtInRect(pt))
	{
		for(index = 0; index < mo_ListEnabled.GetCount(); index++)
		{
			if(mo_ListEnabled.GetAt(mo_ListEnabled.FindIndex(index))) mpb_Checks[index] = FALSE;
		}

		Invalidate();
		return;
	}

	if(mo_RectInvert.PtInRect(pt))
	{
		for(index = 0; index < mo_ListEnabled.GetCount(); index++)
		{
			if(mo_ListEnabled.GetAt(mo_ListEnabled.FindIndex(index)))
				mpb_Checks[index] = mpb_Checks[index] ? FALSE : TRUE;
		}

		Invalidate();
		return;
	}

	if(mo_RectOK.PtInRect(pt))
	{
		mb_Save = 1;
		return;
	}

	if(mo_RectCancel.PtInRect(pt))
	{
		mb_Save = 2;
		return;
	}

	CFrameWnd::OnLButtonDown(n, pt);
}
#endif