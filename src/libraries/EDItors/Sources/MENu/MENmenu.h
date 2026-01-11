/*$T MENmenu.h GC!1.28 04/30/99 16:00:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#include "BASe/CLIbrary/CLIwin.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_BaseFrame;
class   EMEN_cl_FrameIn;
class   EMEN_cl_InMenu;
class   EDI_cl_ActionList;

class EMEN_cl_Menu : public CFrameWnd
{
    DECLARE_DYNCREATE(EMEN_cl_Menu)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT
 ---------------------------------------------------------------------------------------------------
 */

public:
    EMEN_cl_Menu(void);
    ~EMEN_cl_Menu(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_ActionList   *mpo_Actions;
    EMEN_cl_InMenu      *mpo_InMenu;
    EDI_cl_BaseFrame    *mpo_Editor;
    EMEN_cl_FrameIn     *mpo_MenuFrame;
    int                 mi_Sel;
    int                 mi_LastSel;
    int                 mi_PosXInMenu;
    int                 mi_MinPosInMenu;
    CRect               mo_SelRect;
    BOOL                mb_SelOK;
	BOOL				mb_LeftMode;
    UINT                mui_ResToolBar;
	BOOL				mb_DragMode;
	CRect				mo_DragRect;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    DrawBar(void);
    int     i_GetDrawSel(CDC *, BOOL, CRect &);
	void	SetTop(BOOL);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    afx_msg void    OnLButtonDblClk(UINT, CPoint);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnSize(UINT, int, int);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
