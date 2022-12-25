// PERCheckBottomCtrlView.h : interface of the EPERCheckBottom_cl_View class
//

#if !defined(AFX_BOTTOMTREELISTCTRLVIEW_H__6279AD9C_3B25_47AB_BE59_20255873C9AF__INCLUDED_)
#define AFX_BOTTOMTREELISTCTRLVIEW_H__6279AD9C_3B25_47AB_BE59_20255873C9AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
#include "PERCheckBottomTreeListCtrl.h"

#include "DATaControl/DATCP4ClientInfo.h"

class EPERCheckBottom_cl_View : public CView
{
protected: // create from serialization only
	EPERCheckBottom_cl_View();
	DECLARE_DYNCREATE(EPERCheckBottom_cl_View)

// ----------------------------------------------------------------
// STRUCTURE AND METHODS TO HANDLE DATA IN BOTH TREE
public:
	enum
	{
		eRootNotInP4,
		eRootNotInBF,
		eRootDeleted
	};

	void InitializeTreeCtrl();

	void FillNotInP4TreeCtrl();
	void ClearNotInP4TreeCtrl();
	void ExpandNotInP4TreeCtrl() { GetTree()->Expand(m_hItemRootNotInP4, TVE_EXPAND); } ;

	void FillNotInBFTreeCtrl(std::vector<DAT_CP4ClientInfoHeader*>& _vFileInfo);
	void ClearNotInBFTreeCtrl();
	void ExpandNotInBFTreeCtrl() { GetTree()->Expand(m_hItemRootNotInBF, TVE_EXPAND); } ;

	void FillDeletedTreeCtrl(std::vector<DAT_CP4ClientInfoHeader*>& _vFileInfo);
	void ClearDeletedTreeCtrl();
	void ExpandDeletedTreeCtrl() { GetTree()->Expand(m_hItemRootDeleted, TVE_EXPAND); } ;

	EPERCheckBottom_cl_TreeListCtrl* GetTree() { return &m_tree ; } ;

	void SetType(ePERCheckViewType eType) { m_eType = eType ; } ;
	ePERCheckViewType GetType()	{ return m_eType ; } ;

private:
	ePERCheckViewType m_eType;

	HTREEITEM m_hItemRootNotInP4;
	HTREEITEM m_hItemRootNotInBF;
	HTREEITEM m_hItemRootDeleted;
// ----------------------------------------------------------------

// Attributes
public:
	CImageList m_cImageList;
	CImageList m_StatusImageList;

//attributes
public:
	BOOL m_RTL;
	CFont m_headerFont;
	EPERCheckBottom_cl_TreeListCtrl m_tree;
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
	//{{AFX_VIRTUAL(EPERCheckBottom_cl_View)
	public:
		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		virtual void OnDraw(CDC* pDC);  // overridden to draw this view
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
		virtual void OnInitialUpdate();		
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~EPERCheckBottom_cl_View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(EPERCheckBottom_cl_View)
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

#endif // !defined(AFX_NEWTREELISTCTRLVIEW_H__6279AD9C_3B25_47AB_BE59_20255873C9AF__INCLUDED_)
