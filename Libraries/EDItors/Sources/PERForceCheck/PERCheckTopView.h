// PERCheckView.h : interface of the EPERCheckTop_cl_View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOPTREELISTCTRLVIEW_H__6279AD9C_3B25_47AB_BE59_20255873C9AF__INCLUDED_)
#define AFX_TOPTREELISTCTRLVIEW_H__6279AD9C_3B25_47AB_BE59_20255873C9AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PERCheckTopTreeListCtrl.h"

#include "DATaControl/DATCPerforce.h"
#include "DATaControl/DATCUtils.h"

#ifdef JADEFUSION
#include "EDIFileFilters.h"
#endif

#include "PERCheck.h"

#include <string>
#include <map>
#include <set>
#include <vector>

class EPERCheckTop_cl_View : public CView
{
protected: // create from serialization only
	EPERCheckTop_cl_View();
	DECLARE_DYNCREATE(EPERCheckTop_cl_View)

// ----------------------------------------------------------------
// STRUCTURE AND METHODS TO HANDLE DATA IN BOTH TREE
public:

	enum ePERCheckViewFilter
	{
		ePERCheckViewFilterModify		= 0x00000001,
		ePERCheckViewFilterRename		= 0x00000002,
		ePERCheckViewFilterMove			= 0x00000004,
		ePERCheckViewFilterNew			= 0x00000008,

		ePERCheckViewFilterSame			= 0x00000010
	};

	class TreeReference
	{
		public:
			TreeReference(HTREEITEM TreeItemClient=NULL, HTREEITEM TreeItemServer=NULL):m_TreeItemClient(TreeItemClient), m_TreeItemServer(TreeItemServer) {};

			HTREEITEM m_TreeItemClient;
			HTREEITEM m_TreeItemServer;
	};

	// Used to map File to the corresponding HTREETIEM in both tree
	static std::map<BIG_INDEX, TreeReference> m_mapFileToTreeReference;

	// Used to map Folder to the corresponding HTREETIEM in both tree
	static std::map<BIG_INDEX, TreeReference> m_mapFolderToTreeReference;

	void ExpandTreeCtrl(BIG_INDEX DirIndex, BOOL bRecursive=FALSE);

	BOOL FillFolderTreeCtrl(HTREEITEM hItemParent, BIG_INDEX DirIndex);
	BOOL FillFilesTreeCtrl(HTREEITEM hItemParent, BIG_INDEX DirIndex);
	
	void RemoveFolderTreeCtrl(BIG_INDEX DirIndex);
	void RemoveFilesTreeCtrl(BIG_INDEX DirIndex);

	void SetType(ePERCheckViewType eType) { m_eType = eType ; } ;
	ePERCheckViewType GetType()	{ return m_eType ; } ;

	EPERCheckTop_cl_TreeListCtrl* GetTree() { return &m_tree ; } ;
	
	EPERCheckTop_cl_View* GetTwinView()
	{ 
		if ( m_eType == ePERCheckTopLeftView )	
			return (EPERCheckTop_cl_View*)((CSplitterWnd*)GetParent())->GetPane(0,1);
		else if ( m_eType == ePERCheckTopRightView )
			return (EPERCheckTop_cl_View*)((CSplitterWnd*)GetParent())->GetPane(0,0);

		return NULL ; 
	} ;

	EPERCheckTop_cl_TreeListCtrl* GetTwinTree() { return &GetTwinView()->m_tree ; } ;

	HTREEITEM GetTwinFolderItem(BIG_INDEX FolderIndex) 
	{
		if ( m_eType == ePERCheckTopLeftView )
			return m_mapFolderToTreeReference[FolderIndex].m_TreeItemServer ;
		else if ( m_eType == ePERCheckTopRightView )
			return m_mapFolderToTreeReference[FolderIndex].m_TreeItemClient ;

		return NULL ;
	} ;

	HTREEITEM GetTwinFileItem(BIG_INDEX FileIndex) 
	{
		if ( m_eType == ePERCheckTopLeftView )
			return m_mapFileToTreeReference[FileIndex].m_TreeItemServer ;
		else if ( m_eType == ePERCheckTopRightView )
			return m_mapFileToTreeReference[FileIndex].m_TreeItemClient ;

		return NULL ;
	} ;

	void AddFilter(DWORD dwFilters) { m_dwFilters |= dwFilters ; } ;
	void RemFilter(DWORD dwFilters) { m_dwFilters &= ~dwFilters ; } ;

#ifdef JADEFUSION
	void SetFileFilter(EDI_cl_FileFilter* pFileFilter) { m_pFileFilter = pFileFilter ; } ;
#endif

private:
	BOOL m_bIsExpanded;
	ePERCheckViewType m_eType;
	DWORD m_dwFilters;
#ifdef JADEFUSION
	EDI_cl_FileFilter* m_pFileFilter;
#endif
	void SetColorsForItem(EPERCheck_cl_Manager::EPERCheck_cl_Diff* pDiff, HTREEITEM hItem);

	void ExpandTreeCtrlRecurse(BIG_INDEX DirIndex, BIG_INDEX StopDirIndex);
// ----------------------------------------------------------------

// Attributes
public:
	CImageList m_cImageList;
	CImageList m_StatusImageList;

	CFont m_headerFont;
	EPERCheckTop_cl_TreeListCtrl m_tree;
	CScrollBar m_horScrollBar;

private:	
	BOOL m_TLInitialized;	

// Operations
public:
	void Initialize();

	void SortTree(int nCol, BOOL bAscending, HTREEITEM hParent);	

	void ResetScrollBar();
	BOOL VerticalScrollVisible();
	BOOL HorizontalScrollVisible();
	int StretchWidth(int m_nWidth, int m_nMeasure);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EPERCheckTop_cl_View)
public:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();		
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~EPERCheckTop_cl_View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(EPERCheckTop_cl_View)
	afx_msg void OnSize(UINT nType, int cx, int cy);		
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	    
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOPTREELISTCTRLVIEW_H__6279AD9C_3B25_47AB_BE59_20255873C9AF__INCLUDED_)
