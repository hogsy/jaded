// PERCheckView.cpp : implementation of the EPERCheckTop_cl_View class
//

#include "precomp.h"
#include "Res\Res.h"
#include "PERCheckTopView.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "EDImainframe.h"
#include ".\perchecktopview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TREE_LIST_HEADER			337
#define ID_TREE_LIST_CTRL			373
#define ID_TREE_LIST_SCROLLBAR		377

std::map<BIG_INDEX, EPERCheckTop_cl_View::TreeReference> EPERCheckTop_cl_View::m_mapFileToTreeReference;
std::map<BIG_INDEX, EPERCheckTop_cl_View::TreeReference> EPERCheckTop_cl_View::m_mapFolderToTreeReference;

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_View

IMPLEMENT_DYNCREATE(EPERCheckTop_cl_View, CView)

BEGIN_MESSAGE_MAP(EPERCheckTop_cl_View, CView)
	//{{AFX_MSG_MAP(EPERCheckTop_cl_View)
	ON_WM_SIZE()
	ON_WM_HSCROLL()		
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	// Standard printing commands	
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_View construction/destruction

EPERCheckTop_cl_View::EPERCheckTop_cl_View()
//: CFormView(EPERCheckTop_cl_View::IDD)
{
	m_TLInitialized=FALSE;	

	m_dwFilters = 	ePERCheckViewFilterModify |
						ePERCheckViewFilterRename |
							ePERCheckViewFilterMove |
								ePERCheckViewFilterNew ;
}

EPERCheckTop_cl_View::~EPERCheckTop_cl_View()
{
}

BOOL EPERCheckTop_cl_View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_View drawing

void EPERCheckTop_cl_View::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_View diagnostics

#ifdef _DEBUG
void EPERCheckTop_cl_View::AssertValid() const
{
	CView::AssertValid();
}

void EPERCheckTop_cl_View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_View data handling

void EPERCheckTop_cl_View::ExpandTreeCtrl(BIG_INDEX DirIndex, BOOL bRecursive/*=FALSE*/)
{
	HTREEITEM hRootLeft, hRootRight;

	// Client PANE
	if ( m_eType == ePERCheckTopLeftView )
	{
		if (m_mapFolderToTreeReference.find(DirIndex) == m_mapFolderToTreeReference.end())
		{
			hRootLeft = GetTree()->InsertItem(BIG_NameDir(DirIndex));
			GetTree()->SetItemData(hRootLeft, DirIndex);
			GetTree()->SetItemImage(hRootLeft, 0, 0);
			GetTree()->SetItemFolder(hRootLeft,TRUE);
			GetTree()->SetItemChildren(hRootLeft, TRUE);
			
			hRootRight = GetTwinTree()->InsertItem(BIG_NameDir(DirIndex));
			GetTwinTree()->SetItemData(hRootRight, DirIndex);
			GetTwinTree()->SetItemImage(hRootRight, 0, 0);
			GetTwinTree()->SetItemFolder(hRootRight,TRUE);
			GetTwinTree()->SetItemChildren(hRootRight, TRUE);

			m_mapFolderToTreeReference.insert(std::make_pair(DirIndex, TreeReference(hRootLeft, hRootRight)));
		}
		else
		{
			RemoveFolderTreeCtrl(DirIndex);

			hRootLeft = m_mapFolderToTreeReference[DirIndex].m_TreeItemClient;
			hRootRight = m_mapFolderToTreeReference[DirIndex].m_TreeItemServer;

			GetTree()->DeleteSubItemsData(hRootLeft);
			GetTree()->Expand(hRootLeft, TVE_COLLAPSERESET|TVE_COLLAPSE);

			GetTwinTree()->DeleteSubItemsData(hRootRight);
			GetTwinTree()->Expand(hRootRight, TVE_COLLAPSERESET|TVE_COLLAPSE);
		}
	}
	// Server PANE
	else
	{
		if (m_mapFolderToTreeReference.find(DirIndex) == m_mapFolderToTreeReference.end())
		{
			hRootRight = GetTree()->InsertItem(BIG_NameDir(DirIndex));
			GetTree()->SetItemData(hRootRight, DirIndex);
			GetTree()->SetItemImage(hRootRight, 0, 0);
			GetTree()->SetItemFolder(hRootRight,TRUE);
			GetTree()->SetItemChildren(hRootRight, TRUE);

			hRootLeft = GetTwinTree()->InsertItem(BIG_NameDir(DirIndex));
			GetTwinTree()->SetItemData(hRootLeft, DirIndex);
			GetTwinTree()->SetItemImage(hRootLeft, 0, 0);
			GetTwinTree()->SetItemFolder(hRootLeft,TRUE);
			GetTwinTree()->SetItemChildren(hRootLeft, TRUE);

			m_mapFolderToTreeReference.insert(std::make_pair(DirIndex, TreeReference(hRootLeft, hRootRight)));
		}
		else
		{
			RemoveFolderTreeCtrl(DirIndex);

			hRootRight = m_mapFolderToTreeReference[DirIndex].m_TreeItemServer;
			hRootLeft = m_mapFolderToTreeReference[DirIndex].m_TreeItemClient;

			GetTree()->DeleteSubItemsData(hRootRight);
			GetTree()->Expand(hRootRight, TVE_COLLAPSERESET|TVE_COLLAPSE);
			
			GetTwinTree()->DeleteSubItemsData(hRootLeft);
			GetTwinTree()->Expand(hRootLeft, TVE_COLLAPSERESET|TVE_COLLAPSE);
		}
	}
	
	if ( bRecursive )
	{
		ExpandTreeCtrlRecurse(DirIndex, BIG_NextDir(DirIndex));
	}
	else
	{
		// Client PANE
		if ( m_eType == ePERCheckTopLeftView )
			GetTree()->Expand(hRootLeft, TVE_EXPAND);
		// Server PANE
		else
			GetTree()->Expand(hRootRight, TVE_EXPAND);
	}

	// Client PANE
	if ( m_eType == ePERCheckTopLeftView )
	{
		GetTree()->SelectSetFirstVisible(hRootLeft);
		GetTwinTree()->SelectSetFirstVisible(hRootRight);
	}
	// Server PANE
	else
	{
		GetTree()->SelectSetFirstVisible(hRootRight);
		GetTwinTree()->SelectSetFirstVisible(hRootLeft);
	}
}

void EPERCheckTop_cl_View::ExpandTreeCtrlRecurse(BIG_INDEX DirIndex, BIG_INDEX StopDirIndex)
{
	HTREEITEM hItem;

	// Client PANE
	if ( m_eType == ePERCheckTopLeftView )
	{
		while ( DirIndex != BIG_C_InvalidIndex )
		{
			hItem = m_mapFolderToTreeReference[DirIndex].m_TreeItemClient;
			if ( hItem == NULL )	
				return;

			m_tree.Expand(hItem, TVE_EXPAND);

			// Did the user press escape ?
			if ( LINK_gb_EscapeDetected )
				return;

			ExpandTreeCtrlRecurse(BIG_SubDir(DirIndex), StopDirIndex);

			if ( BIG_NextDir(DirIndex) == StopDirIndex)
				break;

			DirIndex = BIG_NextDir(DirIndex);
		}
	}
	// Server PANE
	else 
	{
		while ( DirIndex != BIG_C_InvalidIndex )
		{
			hItem = m_mapFolderToTreeReference[DirIndex].m_TreeItemServer;
			if ( hItem == NULL )	
				return;

			m_tree.Expand(hItem, TVE_EXPAND);

			// Did the user press escape ?
			if ( LINK_gb_EscapeDetected )
				return;

			ExpandTreeCtrlRecurse(BIG_SubDir(DirIndex), StopDirIndex);

			if ( BIG_NextDir(DirIndex) == StopDirIndex)
				break;

			DirIndex = BIG_NextDir(DirIndex);
		}
	}
}

BOOL EPERCheckTop_cl_View::FillFolderTreeCtrl(HTREEITEM hItemParent, BIG_INDEX DirIndex)
{
	UINT index = -1;

	BOOL bHasBeenFilled = FALSE;

	HTREEITEM hItem = NULL;

	std::string strFolder, strFullFolder;
	char aszBFPathName[BIG_C_MaxLenPath];

	BIG_INDEX SubDirIndex = BIG_SubDir(DirIndex);
	while(SubDirIndex != BIG_C_InvalidIndex)
	{
		bHasBeenFilled = TRUE;
		
		// Get path name
		BIG_ComputeFullName(SubDirIndex, aszBFPathName);
		strFullFolder = aszBFPathName;	

		if ( (index = strFullFolder.rfind('/')) != -1 )
			strFolder = strFullFolder.substr(index+1);
		else
			strFolder = strFullFolder;

		// Client PANE
		if ( m_eType == ePERCheckTopLeftView )
		{
			if (m_mapFolderToTreeReference.find(SubDirIndex) != m_mapFolderToTreeReference.end())
			{
				hItem = m_mapFolderToTreeReference[SubDirIndex].m_TreeItemClient;

				if ( hItem == NULL )
				{
					hItem = m_tree.InsertItem( strFolder.c_str(), FALSE, FALSE, hItemParent);
					m_mapFolderToTreeReference[SubDirIndex].m_TreeItemClient = hItem;
				}
			}
			else
			{
				hItem = m_tree.InsertItem( strFolder.c_str(), FALSE, FALSE, hItemParent);
				m_mapFolderToTreeReference.insert(std::make_pair(SubDirIndex, TreeReference(hItem, NULL)));
			}
		}
		// Server PANE
		else
		{
			if (m_mapFolderToTreeReference.find(SubDirIndex) != m_mapFolderToTreeReference.end())
			{
				hItem = m_mapFolderToTreeReference[SubDirIndex].m_TreeItemServer;

				if ( hItem == NULL )
				{
					hItem = m_tree.InsertItem( strFolder.c_str(), FALSE, FALSE, hItemParent);
					m_mapFolderToTreeReference[SubDirIndex].m_TreeItemServer = hItem;
				}
			}
			else
			{
				hItem = m_tree.InsertItem( strFolder.c_str(), FALSE, FALSE, hItemParent);
				m_mapFolderToTreeReference.insert(std::make_pair(SubDirIndex, TreeReference(NULL, hItem)));
			}
		}

		int iImage = M_MF()->i_GetIconImage(const_cast<char*>(strFullFolder.c_str()), NULL, 0);

		m_tree.SetItemData(hItem, SubDirIndex);
		m_tree.SetItemImage(hItem, iImage, iImage);
		m_tree.SetItemFolder(hItem,TRUE);
		m_tree.SetItemChildren(hItem, TRUE);

		SubDirIndex = BIG_NextDir(SubDirIndex);
	}

	bHasBeenFilled |= FillFilesTreeCtrl(hItemParent, DirIndex);

	return bHasBeenFilled;
}

void EPERCheckTop_cl_View::RemoveFolderTreeCtrl(BIG_INDEX DirIndex)
{
	HTREEITEM hFolderItem;

	// Client PANE
	if ( m_eType == ePERCheckTopLeftView )
	{
		hFolderItem = m_mapFolderToTreeReference[DirIndex].m_TreeItemClient ;
	}
	// Server PANE
	else
	{
		hFolderItem = m_mapFolderToTreeReference[DirIndex].m_TreeItemServer ;
	}		

	std::vector<BIG_INDEX> vDirIndexVisible;
	m_tree.GetDirIndexVisible(hFolderItem, vDirIndexVisible);	

	for ( UINT index=0 ; index < vDirIndexVisible.size() ; index++ )
	{
		if ( DirIndex != vDirIndexVisible[index] )
			m_mapFolderToTreeReference.erase(vDirIndexVisible[index]);
	}

	RemoveFilesTreeCtrl(DirIndex);
}

BOOL EPERCheckTop_cl_View::FillFilesTreeCtrl(HTREEITEM hItemParent, BIG_INDEX DirIndex)
{
	HTREEITEM hItem;

	BOOL bHasBeenFilled = FALSE;
	
	std::vector<EPERCheck_cl_Manager::EPERCheck_cl_Diff*>::const_iterator Iter;
	for ( Iter = EPERCheck_cl_Manager::GetInstance()->GetDiffStruct(DirIndex).begin() ; Iter != EPERCheck_cl_Manager::GetInstance()->GetDiffStruct(DirIndex).end() ; Iter++ )
	{
		// FILTER STATUS (new, modified, moved, renamed)		
		 if ( ! ( (*Iter)->m_dwFileDiff & m_dwFilters ) )
			continue;
#ifdef JADEFUSION
		// FILTER NAME
		if ( ! m_pFileFilter->RunFilter( (*Iter)->m_ClientFileName.c_str()) )
			continue;
#endif

		bHasBeenFilled = TRUE;

		char aszLabel[MAX_PATH] = {0};
		char aszSize[MAX_PATH] = {0};;
		char aszTime[MAX_PATH] = {0};

		int iImage = 0;

		// Client PANE
		if ( m_eType == ePERCheckTopLeftView )
		{
			struct tm* newtime;
			newtime = localtime(&((*Iter)->m_ClientTime));

			sprintf(aszLabel, "%s#%d", (*Iter)->m_ClientFileName.c_str(), (*Iter)->m_ClientRevision);
			sprintf(aszSize, "%d", (*Iter)->m_ClientSize);
			strftime(aszTime, 20, "%m/%d/%y %H:%M", newtime);

			// Check if file is already inserted in tree
			if (m_mapFileToTreeReference.find((*Iter)->m_Index) != m_mapFileToTreeReference.end())
			{
				hItem = m_mapFileToTreeReference[(*Iter)->m_Index].m_TreeItemClient;
				if ( hItem == NULL )
				{
					hItem = m_tree.InsertItem("", FALSE, FALSE, hItemParent);
					m_mapFileToTreeReference[(*Iter)->m_Index].m_TreeItemClient = hItem ;
				}
			}
			else
			{
				hItem = m_tree.InsertItem("", FALSE, FALSE, hItemParent);
				m_mapFileToTreeReference.insert(std::make_pair((*Iter)->m_Index, TreeReference(hItem, NULL)));
			}

			iImage = M_MF()->i_GetIconImage(NULL, const_cast<char*>((*Iter)->m_ClientFileName.c_str()), 0);
		}
		// Server PANE
		else 
		{
			if ( ! (*Iter)->m_ServerFileName.empty() )
			{
				if ( (*Iter)->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileRenamed )
				{
					char szPath[BIG_C_MaxLenPath];
					char szName[BIG_C_MaxLenName];
					DAT_CUtils::ExtractPathAndFileName((*Iter)->m_ServerFileName.c_str(), szPath, szName);
					sprintf(aszLabel, "%s#%d", szName, (*Iter)->m_ServerRevision);
				}
				else
					sprintf(aszLabel, "%s#%d", (*Iter)->m_ServerFileName.c_str(), (*Iter)->m_ServerRevision);
			}
			else
				sprintf(aszLabel, "%s", NAME_UNAVAILABLE);

			if ( (*Iter)->m_ServerSize != -1 )
				sprintf(aszSize, "%d", (*Iter)->m_ServerSize);
			else
				sprintf(aszSize, "%s", SIZE_UNAVAILABLE);
				
			struct tm* newtime;
			newtime = localtime(&((*Iter)->m_ServerTime));
			if ( newtime )			
				strftime(aszTime, 255, "%m/%d/%y %H:%M", newtime);
			else
				sprintf(aszTime, "%s", TIME_UNAVAILABLE);

			// Check if file is already inserted in tree
			if (m_mapFileToTreeReference.find((*Iter)->m_Index) != m_mapFileToTreeReference.end())
			{
				hItem = m_mapFileToTreeReference[(*Iter)->m_Index].m_TreeItemServer;
				if ( hItem == NULL )
				{
					hItem = m_tree.InsertItem("", FALSE, FALSE, hItemParent);
					m_mapFileToTreeReference[(*Iter)->m_Index].m_TreeItemServer = hItem ;
				}
			}
			else
			{
				hItem = m_tree.InsertItem("", FALSE, FALSE, hItemParent);
				m_mapFileToTreeReference.insert(std::make_pair((*Iter)->m_Index, TreeReference(NULL, hItem)));
			}

			iImage = M_MF()->i_GetIconImage(NULL, const_cast<char*>((*Iter)->m_ServerFileName.c_str()), 0);
		}

		m_tree.SetItemText(hItem,0, aszLabel);
		m_tree.SetItemText(hItem,1,aszSize);
		m_tree.SetItemText(hItem,2,aszTime);
		m_tree.SetItemData(hItem,(*Iter)->m_Index);
		m_tree.SetItemImage(hItem, iImage, iImage);
		SetColorsForItem((*Iter), hItem);
	}

	return bHasBeenFilled;
}

void EPERCheckTop_cl_View::RemoveFilesTreeCtrl(BIG_INDEX DirIndex)
{
	HTREEITEM hFolderItem;
	std::vector<HTREEITEM> vTreeItemVisible;

	// Client PANE
	if ( m_eType == ePERCheckTopLeftView )
	{
		hFolderItem = m_mapFolderToTreeReference[DirIndex].m_TreeItemClient ;
	}
	// Server PANE
	else
	{
		hFolderItem = m_mapFolderToTreeReference[DirIndex].m_TreeItemServer ;
	}		

	m_tree.GetFileItemVisible(hFolderItem, vTreeItemVisible);	

	for ( UINT index=0 ; index < vTreeItemVisible.size() ; index++ )
	{
		BIG_INDEX FileIndex = m_tree.GetItemData(vTreeItemVisible[index]);
		m_mapFileToTreeReference.erase(FileIndex);
	}
}

void EPERCheckTop_cl_View::SetColorsForItem(EPERCheck_cl_Manager::EPERCheck_cl_Diff* pDiff, HTREEITEM hItem)
{
	if ( (pDiff->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileMoved) && (m_dwFilters & DAT_CP4ClientInfoDiff::eFileMoved) )
	{
		m_tree.SetItemColor(hItem, RGB(128,0,255), FALSE);
		m_tree.SetItemBold(hItem, TRUE, FALSE);
	}
	else if ( (pDiff->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileRenamed) && (m_dwFilters & DAT_CP4ClientInfoDiff::eFileRenamed) )
	{
		m_tree.SetItemColor(hItem, RGB(0,128,0), FALSE);
		m_tree.SetItemBold(hItem, TRUE, FALSE);
	}
	else if ( (pDiff->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileModified) && (m_dwFilters & DAT_CP4ClientInfoDiff::eFileModified) )
	{
		m_tree.SetItemColor(hItem, RGB(0,0,255), FALSE);
		m_tree.SetItemBold(hItem, TRUE, FALSE);
	}
	else if ( (pDiff->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileNew) && (m_dwFilters & DAT_CP4ClientInfoDiff::eFileNew) )
	{
		m_tree.SetItemColor(hItem, RGB(255,0,0), FALSE);
		m_tree.SetItemBold(hItem);
	}
	else
	{
		m_tree.SetItemColor(hItem, RGB(0,0,0), FALSE);
		m_tree.SetItemBold(hItem, FALSE);
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_View methods

void EPERCheckTop_cl_View::Initialize()
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

BOOL EPERCheckTop_cl_View::VerticalScrollVisible()
{	
	int sMin, sMax;
	m_tree.GetScrollRange(SB_VERT, &sMin, &sMax);	
	return sMax!=0;
}

BOOL EPERCheckTop_cl_View::HorizontalScrollVisible()
{
	int sMin, sMax;
	m_horScrollBar.GetScrollRange(&sMin, &sMax);
	return sMax!=0;
}

int EPERCheckTop_cl_View::StretchWidth(int m_nWidth, int m_nMeasure)
{	
	if ( ! m_nMeasure )
		return 0;
	return ((m_nWidth/m_nMeasure)+1)*m_nMeasure;//put the fixed for +1 in brackets f/0 error
}

void EPERCheckTop_cl_View::ResetScrollBar()
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

void EPERCheckTop_cl_View::SortTree(int nCol, BOOL bAscending, HTREEITEM hParent)
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

/////////////////////////////////////////////////////////////////////////////
// EPERCheckTop_cl_View message handlers

void EPERCheckTop_cl_View::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	Initialize();	

	int iRow, iCol;
	CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();
	BOOL bIsChildPane = pSplitter->IsChildPane(this, &iRow, &iCol );
	assert(bIsChildPane && "EPERCheckTop_cl_View::OnInitialUpdate() - Child pane not properly created.");

	if ( iRow == 0 && iCol == 0 )
		SetType(ePERCheckTopLeftView);
	else if ( iRow == 0 && iCol == 1 )
		SetType(ePERCheckTopRightView);

	m_tree.SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);

	// create image list for check boxes assocaited with tree
	m_StatusImageList.Create(IDB_P4CHECK_TREE, 16, 2, RGB (255,255,255));	
	m_tree.SetImageList(&m_StatusImageList, TVSIL_STATE );

	CRect m_wndRect;
	GetClientRect(&m_wndRect);
	MoveWindow(0, 0, m_wndRect.Width(), m_wndRect.Height());

	// insert Columns in list for tree
	if ( m_eType == ePERCheckTopLeftView )
	{
		m_tree.InsertColumn(0, "CLIENT", LVCFMT_LEFT, 320);
		m_tree.InsertColumn(1, "Size", LVCFMT_RIGHT, 60);
		m_tree.InsertColumn(2, "Date", LVCFMT_RIGHT, 100);		
	}
	else 
	{
		m_tree.InsertColumn(0, "SERVER", LVCFMT_LEFT, 320);
		m_tree.InsertColumn(1, "Size", LVCFMT_RIGHT, 60);
		m_tree.InsertColumn(2, "Date", LVCFMT_RIGHT, 100);		
	}

	m_TLInitialized = TRUE;		

	m_tree.SetBackgroundColor(RGB(255,255,255));
}

void EPERCheckTop_cl_View::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

BOOL EPERCheckTop_cl_View::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void EPERCheckTop_cl_View::OnContextMenu(CWnd* pWnd, CPoint point) 
{	
	GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd, MAKELPARAM(point.x, point.y) ); 
}

void EPERCheckTop_cl_View::OnSize(UINT nType, int cx, int cy) 
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

BOOL EPERCheckTop_cl_View::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;//CView::OnEraseBkgnd(pDC);
}
