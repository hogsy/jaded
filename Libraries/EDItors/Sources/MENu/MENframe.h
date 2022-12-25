/*$T MENframe.h GC!1.20 03/31/99 11:23:16 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "MENin.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EVAV_cl_View;
class EMEN_cl_Frame : public EDI_cl_BaseFrame
{
    DECLARE_DYNCREATE(EMEN_cl_Frame)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EMEN_cl_Frame(void);
    ~EMEN_cl_Frame(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:

    /* The list of all inside frames : That's the list of all present menus */
    CList<EMEN_cl_FrameIn *, EMEN_cl_FrameIn *>     mo_List;

    /* The current visible (expanded) frame */
    EMEN_cl_FrameIn                                 *mpo_CurrentExpand;
	BOOL											mb_ChangePane;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EMEN_cl_FrameIn *po_AddMenu
                    (
                        char *,
                        EDI_cl_ActionList *,
                        EDI_cl_BaseFrame *,
                        int,
                        BOOL _b_Sel = FALSE
                    );
    void            RemoveMenu(EMEN_cl_FrameIn *);
    void            MoveMenu(EMEN_cl_FrameIn *, EMEN_cl_Frame *);
    void            NcClick(EMEN_cl_FrameIn *);
    void            Refresh(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    INTERFACE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    virtual BOOL    b_CanBeLinked(void) { return FALSE; };
    virtual BOOL    b_CanBeLinkedToEngine(void) { return FALSE; };
    virtual BOOL    b_HasMenu(void) { return FALSE; };
    virtual int     i_OnMessage(ULONG, ULONG, ULONG);
    virtual BOOL    b_KnowsKey(USHORT);
    virtual BOOL    b_CanActivate(void) { return FALSE; };
	void			SaveIni(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnDestroy(void);
    afx_msg void    OnSize(UINT, int, int);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
