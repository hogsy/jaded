/*$T MENin.h GC!1.41 08/26/99 18:56:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "VAVview/VAVview.h"
#include "EDIaction.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

#define SIZE_CAPTIONY   15
class   EDI_cl_ActionList;
class   EDI_cl_BaseFrame;
class   EMEN_cl_FrameList;
class   EMEN_cl_Frame;

class EMEN_cl_FrameIn : public CFrameWnd
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EMEN_cl_FrameIn (EDI_cl_ActionList *, EDI_cl_BaseFrame *);
    ~EMEN_cl_FrameIn(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:

    /* We must keep the trace of the contener frame */
    EMEN_cl_Frame                                   *mpo_Parent;

    /* The editor associated to that menu. Can be NULL if the menu if for application */
    EDI_cl_BaseFrame                                *mpo_Editor;

    /* The action list associated with that menu */
    EDI_cl_ActionList                               *mpo_Actions;

    int                                             mi_Num;     /* The priority (0 is the top) */
    int                                             mi_Y;       /* The top coord of the frame */
    BOOL                                            mb_Locked;  /* Is the frame locked ? */
    BOOL                                            mb_MyCanDD; /* Can drag & drop menu */

    /* The 2 lists inside a splitter */
    CSplitterWnd                                    mo_Splitter;
    EMEN_cl_FrameList                               *mpo_ListUp;
    EMEN_cl_FrameList                               *mpo_ListDown;

    /* A propertie view */
    EVAV_cl_View                                    *mpo_VarsView;
    EVAV_tdst_VarsViewStruct                        mst_VarsViewStruct;
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   mo_ListItems;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_Action   *FillList(void);
    void            CreateTopMenu(EDI_cl_BaseFrame *, BOOL _b_Refresh = TRUE);
    void            DestroyTopMenu(EDI_cl_BaseFrame *);
	void			SaveIni(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    afx_msg LRESULT OnNcHitTest(CPoint);
    afx_msg void    OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS *);
    afx_msg void    OnNcPaint(void);
    afx_msg void    OnSize(UINT, int, int);
    afx_msg void    OnNcLButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
	afx_msg void	OnDestroy(void);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
