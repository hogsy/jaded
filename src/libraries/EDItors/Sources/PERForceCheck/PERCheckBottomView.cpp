// PERCheckBottomView.cpp : implementation of the EPERCheckBottom_cl_View class
//

#include "precomp.h"
#include "PERCheckBottomTreeListCtrl.h"
#include "PERCheckBottomView.h"
#include "PERCheck.h"

#include "Res\Res.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "EDImainframe.h"

#include "DATaControl/DATCPerforce.h"
#include "DATaControl/DATCUtils.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define ID_EDIT_FIELD   333
#define ID_TREE_LIST_HEADER 337
#define ID_TREE_LIST_CTRL   373
#define ID_TREE_LIST_SCROLLBAR   377

/////////////////////////////////////////////////////////////////////////////
// EPERCheckBottom_cl_View

IMPLEMENT_DYNCREATE(EPERCheckBottom_cl_View, CView)

BEGIN_MESSAGE_MAP(EPERCheckBottom_cl_View, CView)
	//{{AFX_MSG_MAP(EPERCheckBottom_cl_View)
		ON_WM_SIZE()
		ON_WM_HSCROLL()		
		ON_WM_CONTEXTMENU()
                ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands	
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// EPERCheckBottom_cl_View construction/destruction

EPERCheckBottom_cl_View::EPERCheckBottom_cl_View()
//	: CFormView(EPERCheckBottom_cl_View::IDD)
{
	m_TLInitialized=FALSE;	
}

EPERCheckBottom_cl_View::~EPERCheckBottom_cl_View()
{
}

BOOL EPERCheckBottom_cl_View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheckBottom_cl_View drawing

void EPERCheckBottom_cl_View::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheckBottom_cl_View diagnostics

#ifdef _DEBUG
void EPERCheckBottom_cl_View::AssertValid() const
{
	CView::AssertValid();
}

void EPERCheckBottom_cl_View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// EPERCheck_cl_View data handling

void EPERCheckBottom_cl_View::InitializeTreeCtrl()
{
	m_hItemRootNotInP4 = m_tree.InsertItem("FILE(S) NEW IN BF:");
	m_tree.SetItemImage(m_hItemRootNotInP4, 0, 0);
	m_tree.SetItemData(m_hItemRootNotInP4, (DWORD)eRootNotInP4);
	m_tree.SetItemChildren(m_hItemRootNotInP4, TRUE);
	m_tree.SetItemFolder(m_hItemRootNotInP4, TRUE);
		
	m_hItemRootDeleted = m_tree.InsertItem("FILE(S) DELETED IN BF:");
	m_tree.SetItemImage(m_hItemRootDeleted, 20, 20);
	m_tree.SetItemData(m_hItemRootDeleted, (DWORD)eRootDeleted);
	m_tree.SetItemChildren(m_hItemRootDeleted, TRUE);
	m_tree.SetItemFolder(m_hItemRootDeleted, TRUE);

	m_hItemRootNotInBF = m_tree.InsertItem("FILE(S) NEW IN SERVER:");
	m_tree.SetItemImage(m_hItemRootNotInBF, 0, 0);
	m_tree.SetItemData(m_hItemRootNotInBF, (DWORD)eRootNotInBF);
	m_tree.SetItemChildren(m_hItemRootNotInBF, TRUE);
	m_tree.SetItemFolder(m_hItemRootNotInBF, TRUE);
}

void EPERCheckBottom_cl_View::FillNotInP4TreeCtrl()
{
	char aszLabel[MAX_PATH] = {0};
	char aszSize[MAX_PATH] = {0};;
	char aszTime[MAX_PATH] = {0};

	HTREEITEM hChild;
	BOOL bNewFiles = FALSE;

	EDIA_cl_UPDATEDialog* pProgress = new EDIA_cl_UPDATEDialog("Updating info- Please wait...");
	pProgress->DoModeless();

	M_MF()->BeginWaitCursor();

	// Files not present in P4
	for ( BIG_INDEX index=0 ; index < BIG_MaxFile() ; index++ )
	{
		if ( BIG_FileKey(index) == BIG_C_InvalidIndex )
			continue;

		if ( BIG_P4RevisionClient(index) != 0 )
			continue;

		// Get file name
		char aszBFPathName[BIG_C_MaxLenPath];
		char aszBFFileName[BIG_C_MaxLenPath+BIG_C_MaxLenName];
		BIG_ComputeFullName(BIG_ParentFile(index), aszBFPathName);

		strcpy(aszBFFileName, aszBFPathName);
		strcat(aszBFFileName, "/");
		strcat(aszBFFileName, BIG_NameFile(index));

		struct tm* newtime;
		newtime = localtime(&BIG_TimeFile(index));

		int iImage = M_MF()->i_GetIconImage(0, BIG_NameFile(index), 0);
		sprintf(aszLabel, "%s#%d", aszBFFileName, BIG_P4RevisionClient(index));
		sprintf(aszSize, "%d", BIG_ul_GetLengthFile(BIG_PosFile(index)));
		strftime(aszTime, 20, "%m/%d/%y %H:%M", newtime);
		
		hChild = GetTree()->InsertItem("", FALSE, FALSE, m_hItemRootNotInP4, TVI_LAST, FALSE);
		GetTree()->SetItemText(hChild,0, aszLabel);
		GetTree()->SetItemText(hChild,1,aszSize);
		GetTree()->SetItemText(hChild,2,aszTime);
		GetTree()->SetItemImage(hChild, iImage, iImage);
		GetTree()->SetItemData(hChild, BIG_FileKey(index));
		GetTree()->SetItemColor(hChild, RGB(255,0,0));
		GetTree()->SetItemBold(hChild, TRUE);

		bNewFiles = TRUE ;

		if ( index % ((BIG_MaxFile()/50)+1) == 0 )
		{
			pProgress->OnRefreshBar((float)index/(float)BIG_MaxFile());
			M_MF()->BeginWaitCursor();
		}

		// Did the user press escape ?
		if ( LINK_gb_EscapeDetected )
			break;
	}

	if ( ! bNewFiles )
	{
		hChild = GetTree()->InsertItem("NO NEW FILE IN BF...", FALSE, FALSE, m_hItemRootNotInP4);
		GetTree()->SetItemText(hChild,1, "--");
		GetTree()->SetItemText(hChild,2,"--");
		GetTree()->SetItemImage(hChild, 100, 100);
		GetTree()->SetItemData(hChild, BIG_C_InvalidIndex);
		GetTree()->SetItemBold(hChild, TRUE);
	}

	if ( pProgress )
		delete pProgress;

	M_MF()->EndWaitCursor();

	GetTree()->SortItems(0, TRUE, m_hItemRootNotInP4);
}

void EPERCheckBottom_cl_View::ClearNotInP4TreeCtrl()
{
	M_MF()->BeginWaitCursor();
	GetTree()->DeleteSubItemsData(m_hItemRootNotInP4);
	GetTree()->Expand(m_hItemRootNotInP4, TVE_COLLAPSE|TVE_COLLAPSERESET);
	M_MF()->EndWaitCursor();
}

void EPERCheckBottom_cl_View::FillNotInBFTreeCtrl(std::vector<DAT_CP4ClientInfoHeader*>& vFileInfo)
{
	char aszLabel[MAX_PATH] = {0};
	char aszSize[MAX_PATH] = {0};;
	char aszTime[MAX_PATH] = {0};

	UINT iImage;
	HTREEITEM hChild;
	BOOL bNewFiles = FALSE;

	M_MF()->BeginWaitCursor();

	EDIA_cl_UPDATEDialog* pProgress = new EDIA_cl_UPDATEDialog("Updating info- Please wait...");
	pProgress->DoModeless();

	UINT index;
	std::vector<DAT_CP4ClientInfoHeader*>::iterator Iter;
	for ( index=0, Iter = vFileInfo.begin(); Iter != vFileInfo.end(); Iter++, index++ )
	{
		iImage = M_MF()->i_GetIconImage(NULL, (*Iter)->aszBFFilename, 0);
		sprintf(aszLabel, "%s", (*Iter)->aszBFFilename);
		sprintf(aszSize, "%s", SIZE_UNAVAILABLE);
		sprintf(aszTime, "%s", TIME_UNAVAILABLE);

		hChild = GetTree()->InsertItem("", FALSE, FALSE, m_hItemRootNotInBF);
		GetTree()->SetItemText(hChild,0, aszLabel);
		GetTree()->SetItemText(hChild,1,aszSize);
		GetTree()->SetItemText(hChild,2,aszTime);
		GetTree()->SetItemImage(hChild, iImage, iImage);
		GetTree()->SetItemData(hChild, (*Iter)->ulKey);
		GetTree()->SetItemColor(hChild, RGB(255,128,64));
		GetTree()->SetItemBold(hChild, TRUE);

		bNewFiles = TRUE ;

		if ( index % ((vFileInfo.size()/50)+1) == 0 )
		{
			pProgress->OnRefreshBar((float)index/(float)vFileInfo.size());
			M_MF()->BeginWaitCursor();
		}

		// Did the user press escape ?
		if ( LINK_gb_EscapeDetected )
			break;
	}
	
	if ( pProgress )
		delete pProgress;

	M_MF()->EndWaitCursor();

	if ( ! bNewFiles )
	{
		hChild = GetTree()->InsertItem("NO NEW FILE IN PERFORCE...", FALSE, FALSE, m_hItemRootNotInBF);
		GetTree()->SetItemText(hChild,1, "--");
		GetTree()->SetItemText(hChild,2,"--");
		GetTree()->SetItemImage(hChild, 100, 100);
		GetTree()->SetItemData(hChild, BIG_C_InvalidIndex);
		GetTree()->SetItemBold(hChild, TRUE);
	}
	GetTree()->SortItems(0, TRUE, m_hItemRootNotInBF);
}

void EPERCheckBottom_cl_View::ClearNotInBFTreeCtrl()
{
	GetTree()->DeleteSubItemsData(m_hItemRootNotInP4);
	GetTree()->Expand(m_hItemRootNotInBF, TVE_COLLAPSE|TVE_COLLAPSERESET);
}

void EPERCheckBottom_cl_View::FillDeletedTreeCtrl(std::vector<DAT_CP4ClientInfoHeader*>& vFileInfo)
{
	char aszLabel[MAX_PATH] = {0};
	char aszSize[MAX_PATH] = {0};;
	char aszTime[MAX_PATH] = {0};
	
	UINT iImage;
	HTREEITEM hChild;
	BOOL bNewFiles = FALSE;

	M_MF()->BeginWaitCursor();

	std::vector<BIG_KEY>::iterator IterFindKey;

	std::vector<DAT_CP4ClientInfoHeader*>::iterator Iter;
	for ( Iter = vFileInfo.begin(); Iter != vFileInfo.end(); Iter++ )
	{
		iImage = M_MF()->i_GetIconImage(NULL, (*Iter)->aszBFFilename, 0);
		sprintf(aszLabel, "%s", (*Iter)->aszBFFilename);
		sprintf(aszSize, "%s", SIZE_UNAVAILABLE);
		sprintf(aszTime, "%s", TIME_UNAVAILABLE);

		hChild = GetTree()->InsertItem((*Iter)->aszBFFilename, FALSE, FALSE, m_hItemRootDeleted);
		GetTree()->SetItemText(hChild,0, aszLabel);
		GetTree()->SetItemText(hChild,1,aszSize);
		GetTree()->SetItemText(hChild,2,aszTime);
		GetTree()->SetItemImage(hChild, iImage, iImage);
		GetTree()->SetItemData(hChild, (*Iter)->ulKey);
		GetTree()->SetItemColor(hChild, RGB(128,128,128));
		GetTree()->SetItemBold(hChild, TRUE);
		bNewFiles = TRUE ;
	}

	M_MF()->EndWaitCursor();

	if ( ! bNewFiles )
	{
		hChild = GetTree()->InsertItem("NO FILES DELETED...", FALSE, FALSE, m_hItemRootDeleted);
		GetTree()->SetItemText(hChild,1, "--");
		GetTree()->SetItemText(hChild,2,"--");
		GetTree()->SetItemImage(hChild, 100, 100);
		GetTree()->SetItemData(hChild, BIG_C_InvalidIndex);
		GetTree()->SetItemBold(hChild, TRUE);
	}
	GetTree()->SortItems(0, TRUE, m_hItemRootDeleted);
}

void EPERCheckBottom_cl_View::ClearDeletedTreeCtrl()
{
	GetTree()->DeleteSubItemsData(m_hItemRootNotInP4);
	GetTree()->Expand(m_hItemRootDeleted, TVE_COLLAPSE|TVE_COLLAPSERESET);
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheckBottom_cl_View message handlers

void EPERCheckBottom_cl_View::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	Initialize();	

	int iRow, iCol;
	CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();
	assert(pSplitter->IsChildPane(this, &iRow, &iCol ));

	if ( iRow == 1 && iCol == 0 )
		SetType(ePERCheckBottomView);

	m_tree.SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);

	// create image list for check boxes assocaited with tree
	m_StatusImageList.Create(IDB_P4CHECK_TREE, 16, 2, RGB (255,255,255));	
	m_tree.SetImageList(&m_StatusImageList, TVSIL_STATE );

	CRect m_wndRect;
	GetClientRect(&m_wndRect);
	MoveWindow(0, 0, m_wndRect.Width(), m_wndRect.Height());

	m_tree.InsertColumn(0, "FILES", LVCFMT_LEFT, 450);
	m_tree.InsertColumn(1, "Size", LVCFMT_RIGHT, 60);
	m_tree.InsertColumn(2, "Date", LVCFMT_RIGHT, 100);		

	m_TLInitialized = TRUE;		

	m_tree.SetBackgroundColor(RGB(255,255,255));
}


/////////////////////////////////////////////////////////////////////////////
// EPERCheckBottom_cl_View message handlers

void EPERCheckBottom_cl_View::Initialize()
{
	// creates all the objects in frame -
	// header, tree, horizontal scroll bar

	CRect m_wndRect;
	GetWindowRect(&m_wndRect);
	CRect m_headerRect;

	// create the header
	{
		m_headerRect.left = m_headerRect.top = -1;
		m_headerRect.right = m_wndRect.Width();

		m_tree.m_wndHeader.Create(WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ, m_headerRect, this, ID_TREE_LIST_HEADER);
	}

	CSize textSize;
	// set header's pos, dimensions and image list
	{
		CDC *pDC = m_tree.m_wndHeader.GetDC();
		pDC->SelectObject(&m_tree.m_headerFont);
		textSize = pDC->GetTextExtent("A");
		m_tree.m_wndHeader.ReleaseDC(pDC); 

		m_tree.m_wndHeader.SetWindowPos(&wndTop, -1, -1, m_headerRect.Width(), textSize.cy+4, SWP_SHOWWINDOW);

		m_tree.m_cImageList.Create(IDB_P4CHECK_SORT, 16, 10, 0);
		m_tree.m_wndHeader.SetImageList(&m_tree.m_cImageList);		
	}

	CRect m_treeRect;

	// create the tree itself
	{
		GetWindowRect(&m_wndRect);

		m_treeRect.left=0;
		m_treeRect.top = textSize.cy+4;
		m_treeRect.right = m_headerRect.Width()-5;
		m_treeRect.bottom = m_wndRect.Height()-GetSystemMetrics(SM_CYHSCROLL)-4;

		m_tree.Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,m_treeRect, this, ID_TREE_LIST_CTRL);
	}

	// finally, create the horizontal scroll bar
	{
		CRect m_scrollRect;
		m_scrollRect.left=0;
		m_scrollRect.top = m_treeRect.bottom;
		m_scrollRect.right = m_treeRect.Width()-GetSystemMetrics(SM_CXVSCROLL);
		m_scrollRect.bottom = m_wndRect.bottom;

		m_horScrollBar.Create(WS_CHILD | WS_VISIBLE | WS_DISABLED | SBS_HORZ | SBS_TOPALIGN, m_scrollRect, this, ID_TREE_LIST_SCROLLBAR);
	}

	SortTree(0, TRUE, m_tree.GetRootItem());	
}

BOOL EPERCheckBottom_cl_View::VerticalScrollVisible()
{	
	int sMin, sMax;
	m_tree.GetScrollRange(SB_VERT, &sMin, &sMax);	
	return sMax!=0;
}

BOOL EPERCheckBottom_cl_View::HorizontalScrollVisible()
{
	int sMin, sMax;
	m_horScrollBar.GetScrollRange(&sMin, &sMax);
	return sMax!=0;
}

int EPERCheckBottom_cl_View::StretchWidth(int m_nWidth, int m_nMeasure)
{	
	if ( ! m_nMeasure )
		return 0;
	return ((m_nWidth/m_nMeasure)+1)*m_nMeasure;//put the fixed for +1 in brackets f/0 error
}

void EPERCheckBottom_cl_View::ResetScrollBar()
{
	// resetting the horizontal scroll bar

	int m_nTotalWidth=0, m_nPageWidth;

	CRect m_treeRect;
	m_tree.GetClientRect(&m_treeRect);

	CRect m_wndRect;
	GetClientRect(&m_wndRect);

	CRect m_headerRect;
	m_tree.m_wndHeader.GetClientRect(&m_headerRect);

	CRect m_barRect;
	m_horScrollBar.GetClientRect(m_barRect);

	m_nPageWidth = m_treeRect.Width();

	m_nTotalWidth = m_tree.GetColumnsWidth();

	if(m_nTotalWidth > m_nPageWidth)
	{
		// show the scroll bar and adjust it's size
		{
			m_horScrollBar.EnableWindow(TRUE);

			m_horScrollBar.ShowWindow(SW_SHOW);

			// the tree becomes smaller
			m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

			if(!VerticalScrollVisible())
				// i.e. vertical scroll bar isn't visible
			{
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_barRect.Height(), SWP_NOMOVE);
			}
			else
			{
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
			}
		}

		m_horScrollBar.SetScrollRange(0, m_nTotalWidth-m_nPageWidth);

		// recalculate the offset
		{
			CRect m_wndHeaderRect;
			m_tree.m_wndHeader.GetWindowRect(&m_wndHeaderRect);
			ScreenToClient(&m_wndHeaderRect);

			m_tree.m_nOffset = m_wndHeaderRect.left;
			m_horScrollBar.SetScrollPos(-m_tree.m_nOffset);
		}
	}
	else
	{
		m_horScrollBar.EnableWindow(FALSE);

		// we no longer need it, so hide it!
		{
			m_horScrollBar.ShowWindow(SW_HIDE);

			m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_headerRect.Height(), SWP_NOMOVE);
			// the tree takes scroll's place
		}

		m_horScrollBar.SetScrollRange(0, 0);

		// set scroll offset to zero
		{
			m_tree.m_nOffset = 0;
			m_tree.Invalidate();
			CRect m_headerRect;
			m_tree.m_wndHeader.GetWindowRect(&m_headerRect);
			CRect m_wndRect;
			GetClientRect(&m_wndRect);
			m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0, max(StretchWidth(m_tree.GetColumnsWidth(),m_wndRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
		}
	}
}

void EPERCheckBottom_cl_View::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CRect m_treeRect;
	m_tree.GetClientRect(&m_treeRect);
	
	// if horizontal scroll bar 
	if(pScrollBar == &m_horScrollBar)
	{
		int m_nCurPos = m_horScrollBar.GetScrollPos();
		int m_nPrevPos = m_nCurPos;
		// decide what to do for each diffrent scroll event
		switch(nSBCode)
		{
			case SB_LEFT:			m_nCurPos = 0;
									break;
			case SB_RIGHT:			m_nCurPos = m_horScrollBar.GetScrollLimit()-1;
									break;
			case SB_LINELEFT:		m_nCurPos = max(m_nCurPos-6, 0);
									break;
			case SB_LINERIGHT:		m_nCurPos = min(m_nCurPos+6, m_horScrollBar.GetScrollLimit()-1);
									break;
			case SB_PAGELEFT:		m_nCurPos = max(m_nCurPos-m_treeRect.Width(), 0);
									break;
			case SB_PAGERIGHT:		m_nCurPos = min(m_nCurPos+m_treeRect.Width(), m_horScrollBar.GetScrollLimit()-1);
									break;
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:  if(nPos==0)
									    m_nCurPos = 0;
								    else
									    m_nCurPos = min(StretchWidth(nPos, 6), m_horScrollBar.GetScrollLimit()-1);
								    break;
		}		

		m_horScrollBar.SetScrollPos(m_nCurPos);
		m_tree.m_nOffset = -m_nCurPos;

		// smoothly scroll the tree control
		{
			CRect m_scrollRect;
			m_tree.GetClientRect(&m_scrollRect);
			m_tree.ScrollWindow(m_nPrevPos - m_nCurPos, 0, &m_scrollRect, &m_scrollRect);
		}

		CRect m_headerRect;
		m_tree.m_wndHeader.GetWindowRect(&m_headerRect);
		CRect m_wndRect;
		GetClientRect(&m_wndRect);		

		m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0, max(StretchWidth(m_tree.GetColumnsWidth(),m_treeRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
	}
	
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void EPERCheckBottom_cl_View::SortTree(int nCol, BOOL bAscending, HTREEITEM hParent)
{		
	HTREEITEM hChild=NULL;
	HTREEITEM hPa = hParent;
	
	while(hPa!=NULL)
	{
		if(m_tree.ItemHasChildren(hPa))
		{			
			m_tree.SortItems(nCol, bAscending, hPa);
			SortTree(nCol, bAscending, hChild);			
		}			

		hPa = m_tree.GetNextSiblingItem(hPa);
	}
}

BOOL EPERCheckBottom_cl_View::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;	

	if((wParam == ID_TREE_LIST_HEADER) && (pHDN->hdr.code == HDN_ITEMCHANGED))
	{
		int m_nPrevColumnsWidth = m_tree.GetColumnsWidth();
		m_tree.RecalcColumnsWidth();
		ResetScrollBar();

		// in case we were at the scroll bar's end,
		// and some column's width was reduced,
		// update header's position (move to the right).
		CRect m_treeRect;
		m_tree.GetClientRect(&m_treeRect);

		CRect m_headerRect;
		m_tree.m_wndHeader.GetClientRect(&m_headerRect);

		if((m_nPrevColumnsWidth > m_tree.GetColumnsWidth()) &&
		   (m_horScrollBar.GetScrollPos() == m_horScrollBar.GetScrollLimit()-1) &&
		   (m_treeRect.Width() < m_tree.GetColumnsWidth()))
		{
			m_tree.m_nOffset = -m_tree.GetColumnsWidth()+m_treeRect.Width();
			m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0, 0, 0, SWP_NOSIZE);
		}

		m_tree.Invalidate();
	}
	return CView::OnNotify(wParam, lParam, pResult);
}

void EPERCheckBottom_cl_View::OnContextMenu(CWnd* pWnd, CPoint point) 
{	
	GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd, MAKELPARAM(point.x, point.y) ); 
}

void EPERCheckBottom_cl_View::OnSize(UINT nType, int cx, int cy) 
{
	SetScrollRange(SB_HORZ, 0, 0);
	SetScrollRange(SB_VERT, 0, 0);

	if(m_TLInitialized)
	{	
		// resize all the controls	
		CRect m_wndRect;
		GetClientRect(&m_wndRect);

		CRect m_headerRect;
		m_tree.m_wndHeader.GetClientRect(&m_headerRect);
		m_tree.m_wndHeader.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_headerRect.Height(), SWP_NOMOVE);

		CRect m_scrollRect;
		m_horScrollBar.GetClientRect(&m_scrollRect);

		m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_scrollRect.Height(), SWP_NOMOVE);

		CRect m_treeRect;
		m_tree.GetClientRect(&m_treeRect);
		m_horScrollBar.MoveWindow(0, m_treeRect.bottom, m_wndRect.Width(), m_scrollRect.Height());

		if(m_tree.GetColumnsWidth() > m_treeRect.Width())
		{
			// show the horz scroll bar
			{
				CRect m_barRect;
				m_horScrollBar.GetClientRect(&m_barRect);			

				m_horScrollBar.EnableWindow(TRUE);

				m_horScrollBar.ShowWindow(SW_SHOW);

				// the tree becomes smaller
				m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

				int tfdsfsd=m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL);
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
				
			}

			m_horScrollBar.SetScrollRange(0, m_tree.GetColumnsWidth()-m_treeRect.Width());

		}
		else
		{
			// hide the scroll bar
			{
				m_horScrollBar.EnableWindow(FALSE);

				m_horScrollBar.ShowWindow(SW_HIDE);

				// the tree becomes larger
				m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_headerRect.Height(), SWP_NOMOVE);				
			}

			m_horScrollBar.SetScrollRange(0, 0);
		}

		m_tree.ResetVertScrollBar();
		
		// Reset the header control position and scroll
		// the tree control as required.
		int m_nCurPos = m_horScrollBar.GetScrollPos();
		m_tree.m_nOffset = -m_nCurPos;
	
		// smoothly scroll the tree control
	    {
		CRect m_scrollRect;
		m_tree.GetClientRect(&m_scrollRect);
		m_tree.ScrollWindow(m_nCurPos, 0, &m_scrollRect, &m_scrollRect);
		}
		m_tree.m_wndHeader.GetWindowRect(&m_headerRect);	
		GetClientRect(&m_wndRect);

		m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0,max(StretchWidth(m_tree.GetColumnsWidth(),m_treeRect.Width()),m_wndRect.Width()), m_headerRect.Height(),SWP_SHOWWINDOW);			
	}	
}

BOOL EPERCheckBottom_cl_View::OnEraseBkgnd(CDC* pDC)
{
	return FALSE ; //CView::OnEraseBkgnd(pDC);
}
