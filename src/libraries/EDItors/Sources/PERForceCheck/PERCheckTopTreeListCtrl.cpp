// PERCheckTreeListCtrl.cpp : implementation file
//

#include "precomp.h"
#include "PERCheckTopView.h"
#include "PERCheckframe.h"
#include "PERCheck.h"
#include "windowsx.h"  // required for GET_X_LPARAM, GET_Y_LPARAM)
#include <time.h>
#include "BIGfiles/BIGfat.h"
#include "EDImsg.h"
#include "LINks\LINKtoed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TREE_LIST_HEADER 370

/////////////////////////////////////////////////////////////////////////////
// Helper function
BOOL DirHasChildren(BIG_INDEX DirIndex)
{
	if ( BIG_SubDir(DirIndex) != BIG_C_InvalidIndex )
		return TRUE;

	if ( BIG_FirstFile(DirIndex) != BIG_C_InvalidIndex )
		return TRUE;

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_TreeListCtrl

EPERCheckTop_cl_TreeListCtrl::EPERCheckTop_cl_TreeListCtrl()
{
	m_nColumns = m_nColumnsWidth = 0;
	m_nOffset = 0;
	m_wndColor = RGB(250,254,218);
	m_bHasGridLines=FALSE;
	m_GridColor=RGB(0,0,0);
	m_GridStyle=BS_HATCHED;
	m_bPressedTick=FALSE;
	pEdit=NULL;
	pCombo=NULL;

	m_bIsExpanded = FALSE;
}

EPERCheckTop_cl_TreeListCtrl::~EPERCheckTop_cl_TreeListCtrl()
{		
}

BEGIN_MESSAGE_MAP(EPERCheckTop_cl_TreeListCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(EPERCheckTop_cl_TreeListCtrl)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()	
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()	
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_VSCROLL()
	ON_MESSAGE(UWM_TV_CHECKBOX, OnTvCheckbox)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnTvnItemexpanded)
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_TreeListCtrl message handlers

int EPERCheckTop_cl_TreeListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{	
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
} 

HTREEITEM EPERCheckTop_cl_TreeListCtrl::GetTreeItem(int nItem)
{
	HTREEITEM m_ParentItem = GetRootItem();
	int m_nCount = 0;

	while((m_ParentItem!=NULL) && (m_nCount<nItem))
	{
		m_nCount ++ ;
		GetNextSiblingItem(m_ParentItem);
	}

	return m_ParentItem;
}

int EPERCheckTop_cl_TreeListCtrl::GetListItem(HTREEITEM hItem)
{
	HTREEITEM m_ParentItem = GetRootItem();
	int m_nCount = 0;

	while((m_ParentItem!=NULL) && (m_ParentItem!=hItem))
	{
		m_nCount ++ ;
		GetNextSiblingItem(m_ParentItem);
	}

	return m_nCount;
}

int EPERCheckTop_cl_TreeListCtrl::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	HD_ITEM hdi;
	hdi.mask = HDI_TEXT | HDI_FORMAT;
	if(nWidth!=-1)
	{
		hdi.mask |= HDI_WIDTH;
		hdi.cxy = nWidth;
	}
	
	hdi.pszText = (LPTSTR)lpszColumnHeading;
	hdi.fmt = HDF_OWNERDRAW;

	if(nFormat == LVCFMT_RIGHT)
		hdi.fmt |= HDF_RIGHT;
	else
	if(nFormat == LVCFMT_CENTER)
		hdi.fmt |= HDF_CENTER;
	else
		hdi.fmt |= HDF_LEFT;

	m_nColumns ++ ;

	int m_nReturn = m_wndHeader.InsertItem(nCol, &hdi);

	if(m_nColumns==1)
	{
		m_wndHeader.SetItemImage(m_nReturn, 0);
	}

	RecalcColumnsWidth();
	SetBkColor(m_wndColor);
	return m_nReturn;
}

int EPERCheckTop_cl_TreeListCtrl::GetColumnWidth(int nCol)
{
	HD_ITEM hItem;
	hItem.mask = HDI_WIDTH;
	if(!m_wndHeader.GetItem(nCol, &hItem))
		return 0;

	return hItem.cxy;
}

int EPERCheckTop_cl_TreeListCtrl::GetColumnAlign(int nCol)
{
	HD_ITEM hItem;
	hItem.mask = HDI_FORMAT;
	if(!m_wndHeader.GetItem(nCol, &hItem))
		return LVCFMT_LEFT;

	if(hItem.fmt & HDF_RIGHT)
		return LVCFMT_RIGHT;
	else
	if(hItem.fmt & HDF_CENTER)
		return LVCFMT_CENTER;
	else
		return LVCFMT_LEFT;
}

void EPERCheckTop_cl_TreeListCtrl::RecalcColumnsWidth()
{
	m_nColumnsWidth = 0;
	for(int i=0;i<m_nColumns;i++)
		m_nColumnsWidth += GetColumnWidth(i);
}

void EPERCheckTop_cl_TreeListCtrl::DrawItemText (CDC* pDC, CString text, CRect rect, int nWidth, int nFormat)
{
    //
    // Make sure the text will fit in the prescribed rectangle, and truncate
    // it if it won't.
    //
    BOOL bNeedDots = FALSE;
    int nMaxWidth = nWidth - 4;

	while ((text.GetLength()>0) && (pDC->GetTextExtent((LPCTSTR) text).cx > (nMaxWidth - 4))) {
		text = text.Right (text.GetLength () - 1);
		bNeedDots = TRUE;
	}

	if (bNeedDots) {
		if (text.GetLength () >= 1)
			text = text.Right (text.GetLength () - 1);
		text.Insert(0, "...");
	}

    //
    // Draw the text into the rectangle using MFC's handy CDC::DrawText
    // function.
    //
    rect.right = rect.left + nMaxWidth;

    UINT nStyle = DT_VCENTER | DT_SINGLELINE;
    if (nFormat == LVCFMT_LEFT)
        nStyle |= DT_LEFT;
    else if (nFormat == LVCFMT_CENTER)
        nStyle |= DT_CENTER;
    else 
        nStyle |= DT_RIGHT;

	if((text.GetLength()>0) && (rect.right>rect.left))
		pDC->DrawText (text, rect, nStyle);
}

void EPERCheckTop_cl_TreeListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	dc.SelectClipRgn(&rgn);
	rgn.DeleteObject();	

	dc.SetViewportOrg(m_nOffset, 0);

	dc.SetTextColor(m_wndColor);
	 
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );

	HTREEITEM hItem = GetFirstVisibleItem();

	int n = GetVisibleCount(), m_nWidth;

	dc.FillSolidRect(GetColumnWidth(0),1,rcClient.Width(),rcClient.Height(),m_wndColor);

	CTLItem *pItem;

	// the most urgent thing is to erase the labels that were drawn by the tree
	while(hItem!=NULL && n>=0)
	{
		CRect rect;

		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
	
		CRect m_labelRect;
		GetItemRect( hItem, &m_labelRect, TRUE );
		GetItemRect( hItem, &rect, FALSE );
		if(GetColumnsNum()>1)
			rect.left = min(m_labelRect.left, GetColumnWidth(0));
		else
			rect.left = m_labelRect.left;
		rect.right = m_nColumnsWidth;
		dc.FillSolidRect(rect.left,rect.top,rect.Width(),rect.Height(),m_wndColor);

		hItem = GetNextVisibleItem( hItem );
		n--;

		// draw horizontal grid lines
		if (m_bHasGridLines)
		{				
			LOGBRUSH logBrush;
			logBrush.lbStyle = m_GridStyle;
			logBrush.lbColor = m_GridColor;
			logBrush.lbHatch = HS_CROSS;
			CBrush Gridbrush;
			Gridbrush.CreateBrushIndirect(&logBrush);					
			
			CRect rect(0,rect.top-1,rect.right,rect.top);								
			dc.FillRect (rect, &Gridbrush);			
		}
	}

	// create the font
	CFont *pFontDC;
	CFont fontDC, boldFontDC;
	LOGFONT logfont;

	CFont *pFont = GetFont();
	pFont->GetLogFont( &logfont );

	fontDC.CreateFontIndirect( &logfont );
	pFontDC = dc.SelectObject( &fontDC );

	logfont.lfWeight = 700;
	boldFontDC.CreateFontIndirect( &logfont );

	// and now let's get to the painting itself

	hItem = GetFirstVisibleItem();
	n = GetVisibleCount();
	while(hItem!=NULL && n>=0)
	{
		CRect rect;

		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;

		if ( !(GetItemState( hItem, selflag ) & selflag ))
		{
			dc.SetBkMode(TRANSPARENT);

			pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
			CString sItem = pItem->GetItemText();

			CRect m_labelRect;
			GetItemRect( hItem, &m_labelRect, TRUE );
			GetItemRect( hItem, &rect, FALSE );
			if(GetColumnsNum()>1)
				rect.left = min(m_labelRect.left, GetColumnWidth(0));
			else
				rect.left = m_labelRect.left;
			rect.right = m_nColumnsWidth;

			dc.SetBkColor( m_wndColor );
			dc.SetTextColor( pItem->m_Color );

			if(pItem->m_Bold)
			{
				dc.SelectObject( &boldFontDC );
			}

			DrawItemText(&dc, sItem, CRect(rect.left+2, rect.top, GetColumnWidth(0), rect.bottom), GetColumnWidth(0)-rect.left-2, GetColumnAlign(0));

			// draw item text in columns 1 and more
			m_nWidth = 0;
			for(int i=1;i<m_nColumns;i++)
			{
				m_nWidth += GetColumnWidth(i-1);
				DrawItemText(&dc, pItem->GetSubstring(i), CRect(m_nWidth, rect.top, m_nWidth+GetColumnWidth(i), rect.bottom), GetColumnWidth(i), GetColumnAlign(i));				
			}

			// draw vertical grid lines	
			if (m_bHasGridLines)
			{						
				LOGBRUSH logBrush;
				logBrush.lbStyle = m_GridStyle;
				logBrush.lbColor = m_GridColor;
				logBrush.lbHatch = HS_CROSS;
				CBrush Gridbrush;
				Gridbrush.CreateBrushIndirect(&logBrush);			
				m_nWidth = 0;
				for(int i=1;i<m_nColumns+1;i++)
				{
					m_nWidth += GetColumnWidth(i-1);				
					CRect rect(m_nWidth-1,rect.top,m_nWidth,rect.bottom);								
					dc.FillRect (rect, &Gridbrush);
				}			
			}
			
			dc.SetTextColor(::GetSysColor (COLOR_WINDOWTEXT ));
			
			if(pItem->m_Bold)
			{
				dc.SelectObject( &fontDC );
			}
		}
		else
		{

			CRect m_labelRect;
			GetItemRect( hItem, &m_labelRect, TRUE );
			GetItemRect( hItem, &rect, FALSE );
			if(GetColumnsNum()>1)
				rect.left = min(m_labelRect.left, GetColumnWidth(0));
			else
				rect.left = m_labelRect.left;
			rect.right = m_nColumnsWidth;


			// If the item is selected, paint the rectangle with the system color
			// COLOR_HIGHLIGHT

			COLORREF m_highlightColor = ::GetSysColor (COLOR_HIGHLIGHT);

			CBrush brush(m_highlightColor);
			dc.FillRect (rect, &brush);

			// draw a dotted focus rectangle

			dc.DrawFocusRect (rect);
			
			pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
			CString sItem = pItem->GetItemText();

			dc.SetBkColor(m_highlightColor);

			dc.SetTextColor(::GetSysColor (COLOR_HIGHLIGHTTEXT));
			
			if(pItem->m_Bold)
			{
				dc.SelectObject( &boldFontDC );
			}

			DrawItemText(&dc, sItem, CRect(rect.left+2, rect.top, GetColumnWidth(0), rect.bottom), GetColumnWidth(0)-rect.left-2, GetColumnAlign(0));

			m_nWidth = 0;
			for(int i=1;i<m_nColumns;i++)
			{
				m_nWidth += GetColumnWidth(i-1);
				DrawItemText(&dc, pItem->GetSubstring(i), CRect(m_nWidth, rect.top, m_nWidth+GetColumnWidth(i), rect.bottom), GetColumnWidth(i), GetColumnAlign(i));
			}

			if(pItem->m_Bold)
			{
				dc.SelectObject( &fontDC );
			}
		}

		hItem = GetNextVisibleItem( hItem );
		n--;
	}

	dc.SelectObject( pFontDC );
}

void EPERCheckTop_cl_TreeListCtrl::ResetVertScrollBar()
{
	EPERCheckTop_cl_View *pFrame = (EPERCheckTop_cl_View*)GetParent();

	CRect m_treeRect;
	GetClientRect(&m_treeRect);

	CRect m_wndRect;
	pFrame->GetClientRect(&m_wndRect);

	CRect m_headerRect;
	m_wndHeader.GetClientRect(&m_headerRect);	

	CRect m_barRect;
	pFrame->m_horScrollBar.GetClientRect(&m_barRect);

	if(!pFrame->HorizontalScrollVisible())
		SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_headerRect.Height(), SWP_NOMOVE);
	else
		SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

	if(pFrame->HorizontalScrollVisible())
	{
		if(!pFrame->VerticalScrollVisible())
		{
			pFrame->m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_barRect.Height(), SWP_NOMOVE);
			
			int nMin, nMax;
			pFrame->m_horScrollBar.GetScrollRange(&nMin, &nMax);
			if((nMax-nMin) == (GetColumnsWidth()-m_treeRect.Width()+GetSystemMetrics(SM_CXVSCROLL)))
				// i.e. it disappeared because of calling
				// SetWindowPos
			{
				if(nMax - GetSystemMetrics(SM_CXVSCROLL) > 0)
					pFrame->m_horScrollBar.SetScrollRange(nMin, nMax - GetSystemMetrics(SM_CXVSCROLL));
				else
					// hide the horz scroll bar and update the tree
				{
					pFrame->m_horScrollBar.EnableWindow(FALSE);

					// we no longer need it, so hide it!
					{
						pFrame->m_horScrollBar.ShowWindow(SW_HIDE);

						SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_headerRect.Height(), SWP_NOMOVE);
						// the tree takes scroll's place
					}

					pFrame->m_horScrollBar.SetScrollRange(0, 0);

					// set scroll offset to zero
					{
						m_nOffset = 0;
						Invalidate();
						m_wndHeader.GetWindowRect(&m_headerRect);
						m_wndHeader.SetWindowPos(&wndTop, m_nOffset, 0, max(pFrame->StretchWidth(GetColumnsWidth(),m_wndRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
					}
				}
			}
		}
		else
		{
			pFrame->m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);

			int nMin, nMax;
			pFrame->m_horScrollBar.GetScrollRange(&nMin, &nMax);
			if((nMax-nMin) == (GetColumnsWidth()-m_treeRect.Width()-GetSystemMetrics(SM_CXVSCROLL)))
				// i.e. it appeared because of calling
				// SetWindowPos
			{
				pFrame->m_horScrollBar.SetScrollRange(nMin, nMax + GetSystemMetrics(SM_CXVSCROLL));
			}
		}
	}
	else
	if(pFrame->VerticalScrollVisible())
	{
		if(GetColumnsWidth()>m_treeRect.Width())
			// the vertical scroll bar takes some place
			// and the columns are a bit bigger than the client
			// area but smaller than (client area + vertical scroll width)
		{
			// show the horz scroll bar
			{
				pFrame->m_horScrollBar.EnableWindow(TRUE);

				pFrame->m_horScrollBar.ShowWindow(SW_SHOW);

				// the tree becomes smaller
				SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

				pFrame->m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
			}

			pFrame->m_horScrollBar.SetScrollRange(0, GetColumnsWidth()-m_treeRect.Width());
		}
	}
}

void EPERCheckTop_cl_TreeListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void EPERCheckTop_cl_TreeListCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnRButtonUp(nFlags, point);
}

void EPERCheckTop_cl_TreeListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem = HitTest(point,&nFlags);

	if ( hItem == NULL )
		return;

	if( (nFlags & TVHT_ONITEMRIGHT) || (nFlags & TVHT_ONITEMINDENT) || (nFlags & TVHT_ONITEM) )
	{
		SelectItem(hItem);
		SelectTwinTreeItem(hItem);
	}	
	CTreeCtrl::OnLButtonDown(nFlags,point);
}

void EPERCheckTop_cl_TreeListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	// find what item is selected
	HTREEITEM hItem = HitTest(point, &nFlags);

	if((nFlags & TVHT_ONITEMRIGHT) || (nFlags & TVHT_ONITEMINDENT) || (nFlags & TVHT_ONITEM))
	{
		SelectItem(hItem);
		SelectTwinTreeItem(hItem);

		ClientToScreen(&point);
		((EPERCheck_cl_Frame*)GetParent()->GetParent()->GetParent()->GetParent())->OnCtrlPopup(point, ((EPERCheckTop_cl_View*)GetParent())->GetType());
	}
	CTreeCtrl::OnRButtonDown(nFlags, point);
}

void EPERCheckTop_cl_TreeListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	ResetVertScrollBar();
	HTREEITEM hItem = GetSelectedItem();
	SelectTwinTreeItem(hItem);
}

void EPERCheckTop_cl_TreeListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	POINT pt = point;
	HTREEITEM hItem = HitTest(point,&nFlags);

	if ( ItemIsFolder(hItem) == FALSE ) 
	{
		BIG_INDEX ulIndex = GetItemData(hItem);

		if ( ulIndex != BIG_C_InvalidIndex ) 
		{
			LINK_SendMessageToEditors(EDI_MESSAGE_SELFILE, BIG_ParentFile(ulIndex),ulIndex);		
		}
	}
	else 
	{
		CTreeCtrl::OnLButtonDblClk(nFlags,point);
	}
}

BOOL EPERCheckTop_cl_TreeListCtrl::SetItemData(HTREEITEM hItem, DWORD dwData)
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	if(!pItem)
		return FALSE;
	pItem->itemData = dwData;
	return CTreeCtrl::SetItemData(hItem, (LPARAM)pItem);
}

DWORD EPERCheckTop_cl_TreeListCtrl::GetItemData(HTREEITEM hItem) const
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	if(!pItem)
		return NULL;
	return pItem->itemData;
}				

HTREEITEM EPERCheckTop_cl_TreeListCtrl::InsertItem(LPCTSTR lpszItem, BOOL bIsEditable, BOOL bHasCombo, HTREEITEM hParent, HTREEITEM hInsertAfter)
{	
	CTLItem *pItem = new CTLItem;
	pItem->InsertItem(lpszItem);
	pItem->SetEditable(bIsEditable);
	pItem->SetCombo(bHasCombo);
	
	m_nItems++;
	((EPERCheckTop_cl_View*)GetParent())->ResetScrollBar();
	
	return CTreeCtrl::InsertItem(TVIF_PARAM|TVIF_TEXT, "", 0, 0, 0, 0, (LPARAM)pItem, hParent, hInsertAfter);
}

HTREEITEM EPERCheckTop_cl_TreeListCtrl::InsertItem( LPCTSTR lpszItem, int nImage, int nSelectedImage, BOOL bIsEditable, BOOL bHasCombo, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CTLItem *pItem = new CTLItem;
	pItem->InsertItem(lpszItem);
	pItem->SetEditable(bIsEditable);
	pItem->SetCombo(bHasCombo);
	
	m_nItems++;	
	((EPERCheckTop_cl_View*)GetParent())->ResetScrollBar();
	
	return CTreeCtrl::InsertItem(TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE, "", nImage, nSelectedImage, 0, 0, (LPARAM)pItem, hParent, hInsertAfter);
}

HTREEITEM EPERCheckTop_cl_TreeListCtrl::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter , BOOL bIsEditable, BOOL bHasCombo)
{
	CTLItem *pItem = new CTLItem;
	pItem->InsertItem(lpszItem);
	pItem->itemData = lParam;
	pItem->SetEditable(bIsEditable);
	pItem->SetCombo(bHasCombo);
	
	m_nItems++;
	((EPERCheckTop_cl_View*)GetParent())->ResetScrollBar();	
	
	return CTreeCtrl::InsertItem(nMask, "", nImage, nSelectedImage, nState, nStateMask, (LPARAM)pItem, hParent, hInsertAfter); 
}


HTREEITEM EPERCheckTop_cl_TreeListCtrl::CopyItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{	
	if(ItemHasChildren(hItem))
		return NULL;
	
	TV_ITEM item;
	item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
	item.hItem = hItem;	
	GetItem(&item);
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	CTLItem *pNewItem = new CTLItem(*pItem);

	item.lParam = (LPARAM)pNewItem;

	TV_INSERTSTRUCT insStruct;
	insStruct.item = item;
	insStruct.hParent = hParent;
	insStruct.hInsertAfter = hInsertAfter;	
	
	return CTreeCtrl::InsertItem(&insStruct);
}

HTREEITEM EPERCheckTop_cl_TreeListCtrl::MoveItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	if(ItemHasChildren(hItem))
		return NULL;
		
	TV_ITEM item;
	item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
	item.hItem = hItem;	
	GetItem(&item);
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	CTLItem *pNewItem = new CTLItem(*pItem);
	DeleteItem(hItem);
	
	item.lParam = (LPARAM)pNewItem;

	TV_INSERTSTRUCT insStruct;	
	insStruct.item = item;
	insStruct.hParent = hParent;
	insStruct.hInsertAfter = hInsertAfter;
	

	return CTreeCtrl::InsertItem(&insStruct);
}

BOOL EPERCheckTop_cl_TreeListCtrl::SetItemText( HTREEITEM hItem, int nCol ,LPCTSTR lpszItem )
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	if(!pItem)
		return FALSE;
	pItem->SetSubstring(nCol, lpszItem);	
	return CTreeCtrl::SetItemData(hItem, (LPARAM)pItem);
}

BOOL EPERCheckTop_cl_TreeListCtrl::SetItemFolder( HTREEITEM hItem, BOOL _bIsFolder )
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	if(!pItem)
		return FALSE;
	pItem->SetFolder(_bIsFolder);	
	return CTreeCtrl::SetItemData(hItem, (LPARAM)pItem);
}

BOOL EPERCheckTop_cl_TreeListCtrl::ItemIsFolder( HTREEITEM hItem )
{ 
	return ((CTLItem*)CTreeCtrl::GetItemData(hItem ))->IsFolder() ; 
} 

BOOL EPERCheckTop_cl_TreeListCtrl::SetItemChildren( HTREEITEM hItem, BOOL _bChildren )
{
	TV_ITEM item;
	item.mask = TVIF_CHILDREN | TVIF_HANDLE;
	item.hItem = hItem;
	GetItem(&item);
	item.cChildren = _bChildren;
	return CTreeCtrl::SetItem(&item);
}

BOOL EPERCheckTop_cl_TreeListCtrl::SetItemColor( HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate )
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	if(!pItem)
		return FALSE;
	pItem->m_Color = m_newColor;
	if(!CTreeCtrl::SetItemData(hItem, (LPARAM)pItem))
		return FALSE;
	if(m_bInvalidate)
		Invalidate();
	return TRUE;	
}

BOOL EPERCheckTop_cl_TreeListCtrl::SetItemBold( HTREEITEM hItem, BOOL m_Bold, BOOL m_bInvalidate )
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	if(!pItem)
		return FALSE;
	pItem->m_Bold = m_Bold;
	if(!CTreeCtrl::SetItemData(hItem, (LPARAM)pItem))
		return FALSE;
	if(m_bInvalidate)
		Invalidate();
	return TRUE;
}

CString EPERCheckTop_cl_TreeListCtrl::GetItemText( HTREEITEM hItem, int nSubItem )
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
		
	if(!pItem)
		return _T("");
	return pItem->GetSubstring(nSubItem);
}

CString EPERCheckTop_cl_TreeListCtrl::GetItemText( int nItem, int nSubItem )
{
	return GetItemText(GetTreeItem(nItem), nSubItem);
}

BOOL EPERCheckTop_cl_TreeListCtrl::DeleteItem( HTREEITEM hItem )
{
	CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
	if(!pItem)
		return FALSE;
	
	delete pItem;

	m_nItems --;
	
	return CTreeCtrl::DeleteItem(hItem);
}

BOOL EPERCheckTop_cl_TreeListCtrl::DeleteItem( int nItem )
{
	return DeleteItem(GetTreeItem(nItem));
}

int CALLBACK EPERCheckTop_cl_TreeListCtrl::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CTLItem *pItem1 = (CTLItem *)lParam1;
	CTLItem *pItem2 = (CTLItem *)lParam2;
	
	CString str1;
	CString str2;

	// NOTE: Comparing Folder - Files
	// (Folders always have priority over files)
	if ( pItem1->IsFolder() && ! pItem2->IsFolder() )
		return -1 ;
	if ( pItem2->IsFolder() && ! pItem1->IsFolder())
		return 1 ;

	SSortType *pSortType = (SSortType *)lParamSort;

	// NOTE: Comparing Folder - Folder
	if ( pItem1->IsFolder() && pItem2->IsFolder() )
	{
		str1 = pItem1->GetSubstring(pSortType->nCol);
		str2 = pItem2->GetSubstring(pSortType->nCol);
	}
	// NOTE: Comparing Files - Files
	// (Files are already ordered when inserted in tree)
	else
		return 0 ;

	int m_nComp;
	// compare the two strings, but
	// notice:
	// in this case, "xxxx10" comes after "xxxx2"
	{
		CString tmpStr1, tmpStr2;
		int index = str1.FindOneOf("0123456789");
		if(index!=-1)
			tmpStr1 = str1.Right(str1.GetLength()-index);
		index = str2.FindOneOf("0123456789");
		if(index!=-1)
			tmpStr2 = str2.Right(str2.GetLength()-index);
		
		tmpStr1 = tmpStr1.SpanIncluding("0123456789");
		tmpStr2 = tmpStr2.SpanIncluding("0123456789");

		if((tmpStr1=="") && (tmpStr2==""))
			m_nComp = str1.CompareNoCase(str2);
		else
		{
			int num1 = atoi(tmpStr1);
			int num2 = atoi(tmpStr2);

			tmpStr1 = str1.SpanExcluding("0123456789");
			tmpStr2 = str2.SpanExcluding("0123456789");

			if(tmpStr1 == tmpStr2)
			{
				if(num1 > num2)
					m_nComp = 1;
				else
				if(num1 < num2)
					m_nComp = -1;
				else
					m_nComp = str1.CompareNoCase(str2);
			}
			else
				m_nComp = str1.CompareNoCase(str2);			
		}
	}

	if(!pSortType->bAscending)
	{
		if(m_nComp == 1)
			m_nComp = -1;
		else
		if(m_nComp == -1)
			m_nComp = 1;
	}

	return m_nComp;
}

BOOL EPERCheckTop_cl_TreeListCtrl::SortItems( int nCol, BOOL bAscending, HTREEITEM low)
{
	TV_SORTCB tSort;

	tSort.hParent = low;	
	tSort.lpfnCompare = CompareFunc;

	SSortType *pSortType = new SSortType;
	pSortType->nCol = nCol;
	pSortType->bAscending = bAscending;
	tSort.lParam = (LPARAM)pSortType;
	
	BOOL m_bReturn = SortChildrenCB(&tSort);
	
	delete pSortType;	
		
	return m_bReturn;
}

void EPERCheckTop_cl_TreeListCtrl::MemDeleteAllItems(HTREEITEM hParent)
{
	HTREEITEM hItem = hParent;	
	CTLItem *pItem;	
	
	while(hItem!=NULL)
	{			
		pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
		MemItemArray.Add(pItem);
		
		if(ItemHasChildren(hItem))
			MemDeleteAllItems(GetChildItem(hItem));			
				
		hItem = GetNextSiblingItem(hItem);		
	}	
}

void EPERCheckTop_cl_TreeListCtrl::DeleteSubItemsData(HTREEITEM hParent)
{
	HTREEITEM hItem = GetChildItem(hParent);
	while(hItem != NULL)
	{			
		CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);	
		delete pItem;
		CTreeCtrl::SetItemData(hItem, NULL);
		DeleteSubItemsData(hItem);
		hItem = GetNextSiblingItem(hItem);		
	}
}

BOOL EPERCheckTop_cl_TreeListCtrl::DeleteAllItems()
{
	LockWindowUpdate();
	BeginWaitCursor();
	
	MemDeleteAllItems(GetRootItem());		
	BOOL m_bReturn = CTreeCtrl::DeleteAllItems();
	
	for (int i=0;i<MemItemArray.GetSize();i++)
		delete MemItemArray.GetAt(i);
	MemItemArray.RemoveAll();	

	EndWaitCursor();
	UnlockWindowUpdate();
	return m_bReturn;
}

void EPERCheckTop_cl_TreeListCtrl::OnDestroy()
{	
	MemDeleteAllItems(GetRootItem());		
	CTreeCtrl::DeleteAllItems();
	for (int i=0;i<MemItemArray.GetSize();i++)
		delete MemItemArray.GetAt(i);
	MemItemArray.RemoveAll();

	CTreeCtrl::OnDestroy();
}

BOOL EPERCheckTop_cl_TreeListCtrl::DeleteSubItems( HTREEITEM rootItem )
{
	HTREEITEM hItem = GetChildItem(rootItem);
	while(hItem!=NULL)
	{			
		HTREEITEM childItem = GetChildItem(hItem);
		if (childItem)
			DeleteSubItems(childItem);

		DeleteItem(hItem);
		hItem = GetChildItem(rootItem);		
	}
	return TRUE;
}

CString EPERCheckTop_cl_TreeListCtrl::GetNextLine(CString &message)
{
	CString result;
	BOOL bOK = FALSE;
	char k = 13;
	char j;
	int nSize = message.GetLength();
	int nIndex = 0;
	
	//Search for the return character in the string
	while(nIndex < nSize && !bOK)
	{
		j = message.GetAt(nIndex);
		if(j == k)
		{
			result = message.Left(nIndex);
			//Drop the carriage return and line feed characters
			message = message.Right(nSize - (nIndex + 2));
			bOK = TRUE;
		}
		nIndex++;
	}
	return result;
}

LRESULT EPERCheckTop_cl_TreeListCtrl::OnTvCheckbox(WPARAM wp, LPARAM lp)
{ 
	// this method is called twice when user ticks but we only want to 
	// change state once so boolean flag is used
	if (m_bPressedTick)
	{
		HTREEITEM hitem = (HTREEITEM) lp;
  
  
		// get what state the checkbox is in    
		int checked=WhichCheck(hitem);

		// change state to next one up
		if (checked!=3)
		{
  			SetChecked(hitem,checked+1);
		}
		else if (checked==3)
		{
			SetChecked(hitem,1);
		}
		m_bPressedTick=FALSE;
	}
	else
	{
		m_bPressedTick=TRUE;
	}

	return 0;
}

void EPERCheckTop_cl_TreeListCtrl::ScrollTwinTreeItem()
{
	HTREEITEM hItemFirstVisible = GetFirstVisibleItem();

	// Folder
	if ( ItemHasChildren(hItemFirstVisible) )
	{
		BIG_INDEX DirIndex = GetItemData(hItemFirstVisible);

		HTREEITEM hItemTwin = GetPERCheckView()->GetTwinFolderItem(DirIndex);
		if ( hItemTwin != NULL )
			GetPERCheckView()->GetTwinTree()->SelectSetFirstVisible(hItemTwin);
	}
	// File
	else
	{
		BIG_INDEX FileIndex = GetItemData(GetFirstVisibleItem());

		HTREEITEM hItemTwin = GetPERCheckView()->GetTwinFileItem(FileIndex);
		if ( hItemTwin != NULL )
			GetPERCheckView()->GetTwinTree()->SelectSetFirstVisible(hItemTwin);
		else
		{
			// It might be an empty directory ?
			HTREEITEM hItemTwin = GetPERCheckView()->GetTwinFolderItem(FileIndex);
			if ( hItemTwin != NULL )
				GetPERCheckView()->GetTwinTree()->SelectSetFirstVisible(hItemTwin);
		}
	}
}

void EPERCheckTop_cl_TreeListCtrl::SelectTwinTreeItem(HTREEITEM hItem)
{
	// Folder
	if ( ItemHasChildren(hItem) )
	{
		BIG_INDEX DirIndex = GetItemData(hItem);
		
		HTREEITEM hItemTwin = GetPERCheckView()->GetTwinFolderItem(DirIndex);
		if ( hItemTwin != NULL )
			GetPERCheckView()->GetTwinTree()->SelectItem(hItemTwin);
	}
	// File
	else
	{
		BIG_INDEX FileIndex = GetItemData(hItem);
		
		HTREEITEM hItemTwin = GetPERCheckView()->GetTwinFileItem(FileIndex);
		if ( hItemTwin != NULL )
			GetPERCheckView()->GetTwinTree()->SelectItem(hItemTwin);
		else
		{
			// It might be an empty directory ?
			HTREEITEM hItemTwin = GetPERCheckView()->GetTwinFolderItem(FileIndex);
			if ( hItemTwin != NULL )
				GetPERCheckView()->GetTwinTree()->SelectItem(hItemTwin);
		}
	}
}


void EPERCheckTop_cl_TreeListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//SetRedraw(FALSE);
	
	switch(nSBCode)
	{
		case SB_ENDSCROLL:
			ScrollTwinTreeItem();
			
			//SetRedraw(TRUE);
			//GetPERCheckView()->GetTwinTree()->SetRedraw();
			break;
	}
	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL EPERCheckTop_cl_TreeListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ScrollTwinTreeItem();
	return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void EPERCheckTop_cl_TreeListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ScrollTwinTreeItem();
	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void EPERCheckTop_cl_TreeListCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	BIG_INDEX DirIndex = GetItemData(pNMTreeView->itemNew.hItem);
	HTREEITEM hTwinFolder = GetPERCheckView()->GetTwinFolderItem(DirIndex);

	// --------------------------------------------------------------------
	// Expand

	if ( ! IsExpanded() && pNMTreeView->action == TVE_EXPAND )	
	{
		if ( EPERCheck_cl_Manager::GetInstance()->IsDiffStructEmpty(DirIndex) )
		{
			EPERCheck_cl_Manager::GetInstance()->FillDiffStruct(DirIndex);
			EPERCheck_cl_Manager::GetInstance()->SortDiffStruct();
		}

		if ( ! DirHasChildren(DirIndex) )
		{
			SetItemChildren(pNMTreeView->itemNew.hItem, FALSE);
			GetPERCheckView()->GetTwinTree()->SetItemChildren(hTwinFolder, FALSE);
			*pResult = 1;
			return;
		}

		// Expand TREE
		Expanded(TRUE);

		if ( ! GetPERCheckView()->FillFolderTreeCtrl(pNMTreeView->itemNew.hItem, DirIndex) )
		{
			Expanded(FALSE);
			*pResult = 1;
			return;
		}

		// Expand TWIN TREE
		GetPERCheckView()->GetTwinTree()->Expanded(TRUE);
			
		if ( ! GetPERCheckView()->GetTwinView()->FillFolderTreeCtrl(hTwinFolder, DirIndex) )
		{
			GetPERCheckView()->GetTwinTree()->Expanded(FALSE);
			*pResult = 1;
			return;
		}

		GetPERCheckView()->GetTwinTree()->Expand(hTwinFolder, TVE_EXPAND);
	}
	
	// -----------------------------------------------------------------------
	// Collapse
	
	if ( pNMTreeView->action == TVE_COLLAPSE )
	{
		M_MF()->BeginWaitCursor();
		GetPERCheckView()->RemoveFolderTreeCtrl(DirIndex);

		std::vector<BIG_INDEX> vDirIndex;
		GetDirIndexVisible(pNMTreeView->itemNew.hItem, vDirIndex);
		EPERCheck_cl_Manager::GetInstance()->RemoveDiffStruct(vDirIndex);

		DeleteSubItemsData(pNMTreeView->itemNew.hItem);
		Expand(pNMTreeView->itemNew.hItem, TVE_COLLAPSERESET|TVE_COLLAPSE);

		GetPERCheckView()->GetTwinTree()->DeleteSubItemsData(hTwinFolder);
		GetPERCheckView()->GetTwinTree()->Expand(hTwinFolder, TVE_COLLAPSERESET|TVE_COLLAPSE);

		*pResult = 1;
		M_MF()->EndWaitCursor();
		return ;
	}

	*pResult = 0;
}

void EPERCheckTop_cl_TreeListCtrl::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	GetPERCheckView()->SortTree(0, TRUE, pNMTreeView->itemNew.hItem);

	Expanded(FALSE);

	*pResult = 0;
}

void EPERCheckTop_cl_TreeListCtrl::SetChecked(HTREEITEM hItem, int Checked)
{
	SetItemState(hItem,INDEXTOSTATEIMAGEMASK(Checked), TVIS_STATEIMAGEMASK);	    	  
}

int EPERCheckTop_cl_TreeListCtrl::WhichCheck(HTREEITEM hItem)
{
	int checked = (GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12);    
	return checked;
}

void EPERCheckTop_cl_TreeListCtrl::SetBackgroundColor(COLORREF color)
{
	m_wndColor=color;
	SetBkColor(color);
}

void EPERCheckTop_cl_TreeListCtrl::SetGridLines(BOOL hasLines)
{
	m_bHasGridLines=hasLines;
}

BOOL EPERCheckTop_cl_TreeListCtrl::HasGridLines()
{
	return m_bHasGridLines;
}

void EPERCheckTop_cl_TreeListCtrl::SetGridColor(COLORREF color)
{
	m_GridColor=color;
}

COLORREF EPERCheckTop_cl_TreeListCtrl::GetGridColor()
{
	return m_GridColor;
}

void EPERCheckTop_cl_TreeListCtrl::SetGridStyle(UINT style)
{
	m_GridStyle=style;
}

void EPERCheckTop_cl_TreeListCtrl::GetFileItemVisible(HTREEITEM hParent, std::vector<HTREEITEM>& vTreeFileItem)
{
	if ( ! ItemIsFolder(hParent) )
		vTreeFileItem.push_back(hParent);

	// now recurse on all children of this node
	HTREEITEM hchild = GetNextItem(hParent,TVGN_CHILD);
	for ( ; hchild != NULL ; ) 
	{
		GetFileItemVisible(hchild, vTreeFileItem);
		hchild = GetNextSiblingItem(hchild);
	}
}

void EPERCheckTop_cl_TreeListCtrl::GetDirIndexVisible(HTREEITEM hParent, std::vector<BIG_INDEX>& vDirIndexVisible)
{
	if ( ItemIsFolder(hParent) )
		vDirIndexVisible.push_back(GetItemData(hParent));

	// now recurse on all children of this node
	HTREEITEM hchild = GetNextItem(hParent,TVGN_CHILD);
	for ( ; hchild != NULL ; ) 
	{
		GetDirIndexVisible(hchild, vDirIndexVisible);
		hchild = GetNextSiblingItem(hchild);
	}
}

// ------------------------------------------------------------
// DATA Control functions
void EPERCheckTop_cl_TreeListCtrl::InitializeFileItemVisisble()
{
	m_vTreeFileItemsVisible.clear();
	m_TreeFileItemsVisibleIndex = 0;

	GetFileItemVisible(GetSelectedItem(), m_vTreeFileItemsVisible);
}

ULONG EPERCheckTop_cl_TreeListCtrl::GetItemReference(HDATACTRLITEM hItem)
{
	// return the data of the index in the vector m_vTreeFileItemsVisible
	return GetItemData(m_vTreeFileItemsVisible[(UINT)hItem]) ; 
}

HDATACTRLITEM EPERCheckTop_cl_TreeListCtrl::GetFirstSelectedItem() const
{ 
	// return the first index in the vector m_vTreeFileItemsVisible
	if ( m_vTreeFileItemsVisible.size() == 0 )
		return (HDATACTRLITEM)-1; 

	return (HDATACTRLITEM)0; 
}

HDATACTRLITEM EPERCheckTop_cl_TreeListCtrl::GetNextSelectedItem(HDATACTRLITEM _hItem) const
{
	// return the increment of the index in the vector m_vTreeFileItemsVisible
	_hItem++ ;
		
	if ( (UINT)_hItem >= m_vTreeFileItemsVisible.size() )
		_hItem = -1 ;

	return (HDATACTRLITEM)_hItem; 
} 

BOOL EPERCheckTop_cl_TreeListCtrl::ItemIsDirectory(HDATACTRLITEM _hItem) const
{ 
	return FALSE ; 
} 

BOOL EPERCheckTop_cl_TreeListCtrl::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

// ------------------------------------------------------------
