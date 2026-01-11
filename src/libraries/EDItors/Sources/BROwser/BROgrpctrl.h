/*$T BROgrpctrl.h GC!1.40 07/15/99 14:42:50 */

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
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "Dialogs/ListCtrlStyled.h"
class EBRO_cl_GrpCtrl : public CListCtrlStyled
{
/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EBRO_cl_GrpCtrl (void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseView *mpo_MyView;
    int             mi_SortColumn;
	BOOL			mb_LockSel;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
	void	AddGameObjectSub(void *);
    void    FillGameObject(EDI_cl_BaseView *, OBJ_tdst_GameObject *);
    void    FillGrp(EDI_cl_BaseView *, ULONG);
    CString GetItemText(int, int);

	virtual ULONG	GetItemReference(HDATACTRLITEM _hItem) { return BIG_ul_SearchKeyToFat(CListCtrlStyled::GetItemData(_hItem));}

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

protected:
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnSetFocus(CWnd *);
    afx_msg void    OnItemChanged(NMHDR *, LRESULT *);
    afx_msg void    OnBeginDrag(NMHDR *, LRESULT *);
    afx_msg void    OnColumnClick(NMHDR *, LRESULT *);
    afx_msg void    OnParentNotify(UINT, LONG);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnKeyDown(UINT, UINT, UINT);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
