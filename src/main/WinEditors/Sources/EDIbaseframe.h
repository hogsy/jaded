/*$T EDIbaseframe.h GC!1.40 09/13/99 16:38:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "EDIaction.h"
#include "VAVview/VAVview.h"
#include "GDInterface/GDIresolution.h"
#include "BASe/BAStypes.h"

#define EDI_C_MaxDuplicate  4               /* Max duplication */
#define EDI_C_MaxEditors    20              /* Max editors per view */
#define EDI_C_MaxHistory    20              /* Max history files */

/*
 ---------------------------------------------------------------------------------------------------
    To save/restore informations on pos and duplication of editor
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  tdst_PosDupEdit_
{
    int     i_NumView;                      /* The number of the view */
    BOOL    b_Occupy;                       /* Is there an editor with that number ? */
} tdst_PosDupEdit;

/*$4
 ***************************************************************************************************
    To define an editor
 ***************************************************************************************************
 */

class   EDI_cl_BaseFrame;
typedef struct
{
    int                 i_Type;             /* Type of the editor */
    CRuntimeClass       *po_Class;          /* Runtime class to duplicate the editor */
    char                asz_Name[100];      /* Name of editor */
    UINT                ui_ResDialogBar;    /* Resource dialog bar ID */
    UINT                ui_ResToolBar;      /* Resource tool bar ID */
    EDI_cl_BaseFrame    *po_Instance;       /* Instance of the editor in app */
} EDI_tdst_DefEdit;

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_BaseView;
class   EMEN_cl_FrameIn;
class   EMEN_cl_FrameList;
class   EMEN_cl_Menu;
class   EMEN_cl_SubMenu;

class EDI_cl_BaseFrame : public CFrameWnd
{
    DECLARE_DYNCREATE(EDI_cl_BaseFrame)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseFrame (void);
    ~EDI_cl_BaseFrame(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_tdst_DefEdit    mst_Def;
    int                 mi_PosInGArray;
    BOOL                mb_IsPresent;       /* Is the editor here ? */
    BOOL                mb_IsActivate;      /* Is the editor activated ? */
    int                 mi_NumPane;         /* Number of the pane the editor is */
    EDI_cl_BaseFrame    *mpo_Original;      /* Address of the first original editor */
    EDI_cl_BaseView     *mpo_MyView;        /* Address of the view where the editor is now */
    EDI_cl_ActionList   *mpo_Actions;       /* Address of action list associated */
    int                 mi_NumEdit;
	BOOL				mb_Persist;			/* Keys persist in full screen */

    BOOL                mab_PresentEdit[EDI_C_MaxDuplicate];

    CDialogBar          *mpo_DialogBar;     /* Address of eventual dialog bar */

    /*
     * For MENu editor. Keep the trace of the menu frame in MENu editor, and of the top menu
     * above the editor frame
     */
    EMEN_cl_FrameIn     *mpo_MenuFrame;     /* Address of left frame menu */
    EMEN_cl_Menu        *mpo_Menu;          /* The eventual top menu */

    LONG                mi_FullScreenResolution; /* full screen resolution */

    struct
    {
        BOOL                b_DialogBarVisible;
        int                 i_NumCopy;
        BOOL                b_IsVisible;
        BOOL                ab_LinkTo[EDI_C_MaxEditors][EDI_C_MaxDuplicate];    /* Linked list */
        BOOL                b_EngineRefresh;                                    /* Need to be
                                                                                 * refresh when
                                                                                 * engine is
                                                                                 * running ? */
        BOOL                b_EditorRefresh;                                    /* Need to be
                                                                                 * refresh when
                                                                                 * engine is not
                                                                                 * running ? */
        int                 i_CountMenu;                                        /* The editor copy
                                                                                 * of MEN */
        BOOL                b_TopMenu;                                          /* Top menu visible
                                                                                 * ? */
        int                 i_TopInMenu;                                        /* Position of
                                                                                 * toolbar in
                                                                                 * topmenu */
        BOOL                mb_LeftMode;                                        /* Menu if at left */
        BIG_KEY             aul_History[EDI_C_MaxHistory];                      /* History files */
        GDI_tdst_Resolution st_FullScreenResolution;                            /* Full screen
                                                                                 * resolution */
		int					YSplitMenu;
		int					ISplitMenu;
        LONG                al_Dummy[6];
    } mst_BaseIni;

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    virtual BOOL    PreCreateWindow(CREATESTRUCT &);

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    int     OnCreate(LPCREATESTRUCT);
    void    OnSize(UINT, int, int);
    void    RefreshMenu(void);
    int     i_GetPosDup(void);
    void    BaseLoadIni(char *, UCHAR *, int);
    char    *BaseLoadIniDesktop(char *, UCHAR *, int);
    void    BaseSaveIni(char *, UCHAR *, int);
    void    GetClientInsideRect(CRect &);

    void    InitPopupMenuAction(EMEN_cl_SubMenu *);
    BOOL	AddPopupMenuAction(EMEN_cl_SubMenu *, ULONG);
    void    TrackPopupMenuAction(CPoint, EMEN_cl_SubMenu *);

/*$2
 ---------------------------------------------------------------------------------------------------
    INTERFACE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    virtual BOOL            b_CanActivate(void) { return TRUE; };
    virtual BOOL            b_CanDuplicate(void) { return TRUE; };

    virtual void            OnActivate(void);
    virtual void            OnDisactivate(void);
    virtual BOOL            b_CanBeLinked(void) { return TRUE; };
    virtual BOOL            b_CanBeLinkedToEngine(void) { return TRUE; };
    virtual BOOL            b_HasMenu(void) { return TRUE; };

    virtual void            ReinitIni(void);
    virtual void            LoadIni(void);
    virtual void            LoadIniDesktop(void);
    virtual void            TreatIni(void) { };
    virtual void            SaveIni(void);

    virtual void            ForceRefresh(void) { };

    virtual char            *psz_OnActionGetBase(void) { return NULL; };
    virtual void            OnAction(ULONG) { };
    virtual BOOL            b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE) { return TRUE; };
    virtual BOOL            b_KnowsKey(USHORT) { return FALSE; };
    virtual void            OnActionUI(ULONG, CString &o, CString &r) { r = o; };
    virtual UINT            ui_OnActionState(ULONG) { return (UINT) -1; };
    virtual UINT            ui_ActionFillDynamic(EDI_cl_ConfigList *, POSITION) { return 0; };
    virtual int             i_IsItVarAction(ULONG, EVAV_cl_View *) { return 0; };
    virtual void            OneActionKeyHasChanged(void) { };
    virtual EDI_cl_Action   *po_ActionGetSetDynKey(EDI_cl_ActionList *, EDI_cl_Action *)
    {
        return NULL;
    };

    virtual void    OpenProject(void);
    virtual void    CloseProject(void) { };
    virtual BOOL    b_AcceptToCloseProject(void) { return TRUE; };
    virtual void    WhenAppClose(void) { };
    virtual void    BeforeEngine(void) { };
    virtual void    AfterEngine(void) { };
    virtual void    OneTrameEnding(void) { };
    virtual int     i_OnMessage(ULONG, ULONG, ULONG) { return 1; };

    virtual void    OnRealIdle(void) { };

    virtual void    OnToolBarCommand(UINT) { };
    virtual void    OnToolBarCommandUI(UINT, CCmdUI *) { };

    virtual BOOL    b_CanHaveMultipleFullScreenResolution(void) { return FALSE; };

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
