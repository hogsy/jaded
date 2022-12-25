/*$T EACTframe.h GC!1.71 02/24/00 12:19:52 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "BASE/CLIbrary/CLIfile.h"
#include "EACTframe_act.h"
#include "EDIbaseframe.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"

class   EACT_cl_View;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EACT_cl_Frame : public EDI_cl_BaseFrame
{
    DECLARE_DYNCREATE(EACT_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EACT_cl_Frame (void);
    ~EACT_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    struct
    {
    }
    mst_Ini;

    EACT_cl_View        *mpo_View;  /* Edition view */

	BIG_INDEX			mul_CurrentActionKit;
    ACT_st_ActionKit    *mpst_CurrentActionKit;
    ACT_st_Action       *mpst_CurrentAction;
	USHORT				muw_CurrentAction;
	USHORT				muw_SaveAction;
    ACT_st_ActionItem   *mpst_CurrentActionItem;
	UCHAR				muc_CurrentItem;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	OnClose(void);
	void	LoadActionKit(BIG_INDEX);
	void	LoadAnim(BIG_INDEX);
    void    ResetTransition(void);
	void	DuplicateKit(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    BOOL    b_CanDuplicate(void)        { return TRUE; };
    BOOL    b_CanBeLinked(void)         { return TRUE; };
    BOOL    b_CanBeLinkedToEngine(void) { return TRUE; };
    BOOL    b_CanActivate(void)         { return TRUE; }
    char    *psz_OnActionGetBase(void)  { return EACT_asz_ActionBase; };
    void    OnAction(ULONG);
    BOOL    b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
    UINT    ui_OnActionState(ULONG);
    int     i_OnMessage(ULONG, ULONG, ULONG);
    void    ReinitIni(void);
    void    LoadIni(void);
    void    TreatIni(void);
    void    SaveIni(void);
	BOOL	b_KnowsKey(USHORT);
    void    OnRealIdle(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnSize(UINT, int, int);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
