// BaseTabCtrl.cpp : implementation file
//

#include "Precomp.h"
//#include "tabcontrol.h"
#include "BaseTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl

CBaseTabCtrl::CBaseTabCtrl(int nType)
{
	m_crBack = (COLORREF)-1; // use default color
	m_nDrawType = BTC_NONE;

	EnableDraw(nType);
}

CBaseTabCtrl::~CBaseTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CBaseTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CBaseTabCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl message handlers

void CBaseTabCtrl::SetBkgndColor(COLORREF color)
{
	// set new color
	m_crBack = color;

	// redraw
	if (GetSafeHwnd())
		Invalidate();
}

BOOL CBaseTabCtrl::EnableDraw(int nType)
{
	ASSERT (nType >= BTC_NONE && nType <= BTC_ALL);

	if (nType < BTC_NONE || nType > BTC_ALL)
		return FALSE;

	m_nDrawType = nType;

	if (GetSafeHwnd())
	{
		if (m_nDrawType != BTC_NONE) 
			ModifyStyle(0, TCS_OWNERDRAWFIXED);
		else
			ModifyStyle(TCS_OWNERDRAWFIXED, 0);

		Invalidate();
	}

	return TRUE;
}

void CBaseTabCtrl::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

void CBaseTabCtrl::DrawItemBorder(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

void CBaseTabCtrl::DrawMainBorder(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

COLORREF CBaseTabCtrl::GetTabColor(BOOL /*bSelected*/)
{
	return ::GetSysColor(COLOR_3DFACE);
}

COLORREF CBaseTabCtrl::GetTabTextColor(BOOL /*bSelected*/)
{
	return ::GetSysColor(COLOR_WINDOWTEXT);
}

void CBaseTabCtrl::PreSubclassWindow() 
{
	CTabCtrl::PreSubclassWindow();

	if (m_nDrawType != BTC_NONE) 
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

BOOL CBaseTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rClient, rTab, rTotalTab, rBkgnd, rEdge;
	COLORREF crBack;
	int nTab, nTabHeight = 0;

	GetClientRect(rClient);
	CTabCtrl::OnEraseBkgnd(pDC);

	// calc total tab width
	nTab = GetItemCount();
	rTotalTab.SetRectEmpty();

	while (nTab--)
	{
		GetItemRect(nTab, rTab);
		rTotalTab.UnionRect(rTab, rTotalTab);
	}

	nTabHeight = rTotalTab.Height();

	// add a bit
	rTotalTab.InflateRect(2, 3);
	rEdge = rTotalTab;

	// then if background color is set, paint the visible background
	// area of the tabs in the bkgnd color
	// note: the mfc code for drawing the tabs makes all sorts of assumptions
	// about the background color of the tab control being the same as the page
	// color - in some places the background color shows thru' the pages!!
	// so we must only paint the background color where we need to, which is that
	// portion of the tab area not excluded by the tabs themselves
	crBack = (m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack;
	
	// full width of tab ctrl above top of tabs
	rBkgnd = rClient;
	rBkgnd.bottom = rTotalTab.top + 3;
	pDC->SetBkColor(crBack);
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);
	
	// width of tab ctrl visible bkgnd including bottom pixel of tabs to left of tabs
	rBkgnd = rClient;
	rBkgnd.right = 2;
	rBkgnd.bottom = rBkgnd.top + (nTabHeight + 2);
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);
	
	// to right of tabs
	rBkgnd = rClient;
	rBkgnd.left += rTotalTab.Width() - 2;
	rBkgnd.bottom = rBkgnd.top + (nTabHeight + 2);
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);

	return TRUE;
}

void CBaseTabCtrl::OnPaint() 
{
	if (m_nDrawType == BTC_NONE)
	{
		Default();
	}
	else if (m_nDrawType == BTC_TABS)
	{
		ASSERT (GetStyle() & TCS_OWNERDRAWFIXED);
		Default();
	}
	else // all
	{
		CPaintDC dc(this); // device context for painting

		// prepare dc
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

		DrawMainBorder(&dis);

		// paint the tabs first and then the borders
		int nTab = GetItemCount();
		int nSel = GetCurSel();

		if (!nTab) // no pages added
			return;

		while (nTab--)
		{
			if (nTab != nSel)
			{
				dis.itemID = nTab;
				dis.itemState = 0;

				VERIFY(GetItemRect(nTab, &dis.rcItem));

				dis.rcItem.bottom -= 2;
				DrawItem(&dis);
				DrawItemBorder(&dis);
			}
		}

		// now selected tab
		dis.itemID = nSel;
		dis.itemState = ODS_SELECTED;

		VERIFY(GetItemRect(nSel, &dis.rcItem));

		dis.rcItem.bottom += 2;
		dis.rcItem.top -= 2;
		DrawItem(&dis);
		DrawItemBorder(&dis);
	}
}

