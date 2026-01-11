/*$T BROtreectrl.h GC!1.36 05/18/99 10:54:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#include <vector>
#include "PERForce/PERCDataCtrl.h"
/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */
typedef std::vector<HTREEITEM> SEL_ITEM_LIST;
class EPER_cl_Frame;

#ifdef ACTIVE_EDITORS
class EPER_cl_TreeCtrl : public CTreeCtrl,public PER_CDataCtrl
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
	EPER_cl_TreeCtrl(EPER_cl_Frame* _pFrame);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseView *mpo_MyView;
	EPER_cl_Frame* mpo_frame;
	BOOL			mb_LockSel;

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
 	
	

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point) ;
	afx_msg void	OnRButtonDown(UINT nFlags, CPoint point);

	
	void SelectItem(HTREEITEM hItem,int iState );
	void		ClearSelection();
	const SEL_ITEM_LIST& GetSelectedItems()
	{
		return m_vSelItem;
	}
	HTREEITEM GetLastSelectedItem() const
	{ 
		return m_hLastSelItem; 
	} 

	virtual ULONG			GetItemReference(HDATACTRLITEM _hItem) { return BIG_ul_SearchKeyToFat(CTreeCtrl::GetItemData((HTREEITEM)_hItem));}
	virtual HDATACTRLITEM	GetFirstSelectedItem() const { 	return (HDATACTRLITEM)GetLastSelectedItem(); }
	virtual HDATACTRLITEM	GetNextSelectedItem	(HDATACTRLITEM _hItem) const { return -1; }
	virtual BOOL			ItemIsDirectory		(HDATACTRLITEM _hItem) const {	return GetParentItem(GetLastSelectedItem()) == NULL;	}
	virtual ULONG			GetItemReferenceData(HDATACTRLITEM _hItem)  
	{ 
		return CTreeCtrl::GetItemData((HTREEITEM)_hItem);
	} 

	virtual HDATACTRLITEM	GetParentReference			(HDATACTRLITEM _hItem) { return (HDATACTRLITEM) GetParentItem((HTREEITEM)_hItem); }


protected:
	BOOL		m_bDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;
	HTREEITEM	m_hLastSelItem;
	CImageList*	m_pDragImage;
	CImageList* m_pImageList;
	SEL_ITEM_LIST m_vSelItem;
	
	HTREEITEM	MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter);
	void		OnControlKeyPress(HTREEITEM hCurItem);
	void		OnShiftKeyPress(HTREEITEM hCurItem);
	void		SelectItems(HTREEITEM hItemFrom,HTREEITEM hItemTo);
	
	void		OnDrag(CPoint point);
	void		RemoveFromSelectionList(HTREEITEM hItem);

	
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point) ;
	afx_msg void	OnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void    OnBeginDrag(NMHDR *, LRESULT *);
	afx_msg void    OnExpand(NMHDR *, LRESULT *);
	afx_msg void    OnSetFocus(CWnd *);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
