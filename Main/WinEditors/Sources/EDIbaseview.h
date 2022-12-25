/*$T EDIbaseview.h GC!1.52 11/05/99 10:54:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "MATHs/MATH.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class CFloating : public CWnd
{
    DECLARE_DYNCREATE(CFloating)
public:
    EDI_cl_BaseView *mpo_View;
	BOOL			mb_Minimize;
    CRect			mo_BeforeMin;
public:
    CFloating (void);
    afx_msg void    OnSize(UINT n, int cx, int cy);
    afx_msg void    OnNcLButtonDblClk(UINT, CPoint);
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    afx_msg void    OnClose(void);
    DECLARE_MESSAGE_MAP()
};

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

#define M_CurSelScr(a)  (M_MF()->mst_Desktop.ast_View[a].i_CurrentSelScreen)
#define M_CurNumEd(a)   (M_MF()->mst_Desktop.ast_View[a].i_NumEditors)
#define M_CurEd(a, b)   (M_MF()->mst_Desktop.ast_View[a].ast_Editors[b])

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EDI_cl_BaseView : public CTabCtrl
{
    DECLARE_DYNCREATE(EDI_cl_BaseView)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseView (void);
    EDI_cl_BaseView (int);
    ~EDI_cl_BaseView(void);
	BOOL PreCreateWindow(CREATESTRUCT &);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    char                *mpsz_Name;
    BOOL                mb_DragDropCopy;
    BOOL                mb_Outside;
    int                 mi_PaneDragDrop;
    EDI_cl_BaseView     *mpo_DragView;
    int                 mi_PopupMenu;
    int                 mi_NumPane;
    BOOL                mb_TabFill;
    BOOL                mb_Floating;

    CFloating           *p1;
    CEdit               *p;
    CWnd                *po_Parent;
    UINT                uiID;

    MATH_tdst_Vector    mst_GamePos;
	int					mi_Score;
    MATH_tdst_Vector    mst_GameMove;
    POINT               mst_GameLMouse;
    BOOL                mb_GameOut;
    BOOL                mb_GameKey;
    int                 mi_GameOn;
    POINT               mast_GamePos[1024];

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseFrame    *po_AddOneEditor(int, int, int);
    void                CreateMainEditors(void);
    void                CreateDuplicateEditors(void);
    int                 i_NumEditFromPane(int);
    void                DisactivateEditor(EDI_cl_BaseFrame *);
    virtual void        IWantToBeActive(EDI_cl_BaseFrame *);
    virtual void        ChangePane(void) { };
    void                ActivateDisactivateEditors(void);
    void                DrawItem(LPDRAWITEMSTRUCT);
    EDI_cl_BaseFrame    *po_GetActivatedEditor(void);
    void                GameTest(void);
    void                GameTick1(void);
    void                GameTick2(void);

    BOOL                DestroyEditor(EDI_cl_BaseFrame *);
    void                ActivateNewPane(void);

    void                SetFloating(BOOL);

/*$2
 ---------------------------------------------------------------------------------------------------
    INTERFACE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    virtual void    SaveIni(void);
    virtual void    LoadIniDesktop(void);
    virtual void    TreatIni(void);
    virtual void    OpenProject(void);
    virtual void    CloseProject(void);
    virtual BOOL    b_AcceptToCloseProject(void);
    virtual void    WhenAppClose(void);
    virtual void    BeforeEngine(void);
    virtual void    AfterEngine(void);
    virtual void    OneTrameEnding(void);
    virtual void    SendMessageToEditors(ULONG, ULONG, ULONG, int = 3);
    virtual void    OnRealIdle(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    afx_msg void    OnNcPaint(void);
    afx_msg void    OnPaint(void);
    afx_msg void    OnSize(UINT, int, int);
    afx_msg void    OnChangePane(NMHDR *, LRESULT *);
    afx_msg LRESULT OnNcHitTest(CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnPopupMenu(UINT);
    afx_msg void    OnLButtonDblClk(UINT, CPoint);
    afx_msg void    OnLinkTo(UINT);
    afx_msg void    OnInitFrom(UINT);
    afx_msg void    OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS *);
	afx_msg void	OnKeyDown(UINT, UINT, UINT);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
