/*$T AIleftframe.h GC!1.41 09/21/99 15:34:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"

#include "EDItors/Sources/PERForce/PERCDataCtrl.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EAI_cl_InsideFrame : public CWnd
{
/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_Frame    *mpo_Parent;

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    virtual int OnToolHitTest(CPoint, TOOLINFO *) const;

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnSize(UINT n, int cx, int cy);
    afx_msg BOOL    OnMouseWheel(UINT, short, CPoint);
    afx_msg BOOL    OnToolTipText(UINT, NMHDR *, LRESULT *);
    DECLARE_MESSAGE_MAP()
};

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EAI_cl_Frame;

class EAI_cl_LeftFrame : public CListCtrl, public PER_CDataCtrl
{
/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_Frame    *mpo_Parent;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    SelectItem(int);
    void    SortList(void);
    void    SelectFromFile(void);

	// DATA CONTROL Functions
	virtual BOOL			ItemIsDirectory (HDATACTRLITEM _hItem)const	{ return FALSE; }
	virtual ULONG			GetItemReference(HDATACTRLITEM _hItem)	{ return GetItemData(_hItem);	}
	virtual HDATACTRLITEM	GetFirstSelectedItem()const { return GetNextItem(-1, LVNI_SELECTED);}
	virtual HDATACTRLITEM	GetNextSelectedItem	(HDATACTRLITEM _hItem)const {	return GetNextItem(_hItem, LVNI_SELECTED);}


/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnColumnClick(NMHDR *, LRESULT *);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
