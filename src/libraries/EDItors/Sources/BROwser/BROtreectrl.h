/*$T BROtreectrl.h GC!1.36 05/18/99 10:54:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

#include "EDItors/Sources/PERForce/PERCDataCtrl.h"

class EBRO_cl_TreeCtrl : public CTreeCtrl, public PER_CDataCtrl
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
	EBRO_cl_TreeCtrl(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseView *mpo_MyView;
	BOOL			mb_LockSel;

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    DeleteAllItems(void);
    void    Expand(HTREEITEM, UINT);
    void    EnsureVisible(HTREEITEM);

	virtual ULONG			GetItemReference(HDATACTRLITEM _hItem) { return CTreeCtrl::GetItemData((HTREEITEM)_hItem);}
	virtual HDATACTRLITEM	GetFirstSelectedItem() const { 	return (HDATACTRLITEM)CTreeCtrl::GetSelectedItem(); }
	virtual HDATACTRLITEM	GetNextSelectedItem	(HDATACTRLITEM _hItem) const { return -1; }
	virtual BOOL			ItemIsDirectory		(HDATACTRLITEM _hItem) const {	return TRUE;	}

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    FillDirRecurse(int, HTREEITEM, BIG_INDEX, BIG_INDEX, char *);
    void    FillDir(EDI_cl_BaseView *, int, HTREEITEM, BIG_INDEX, BIG_INDEX);
    CString GetItemText(HTREEITEM);
	void	Collapse(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

protected:
    afx_msg void    OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
    afx_msg void    OnSetFocus(CWnd *);
    afx_msg void    OnSelChange(NMHDR *, LRESULT *);
    afx_msg void    OnExpanded(NMHDR *, LRESULT *);
    afx_msg void    OnExpanding(NMHDR *, LRESULT *);
    afx_msg void    OnBeginDrag(NMHDR *, LRESULT *);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
