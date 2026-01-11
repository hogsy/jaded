/*$T LOGframe.h GC!1.26 04/22/99 14:40:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include <string>
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "LOGframe_act.h"

#define ELOG_Csz_FileName	"Jade.log"
#define LOG_iLOG_NB 6
#define ELOG_MaxStringNb    10000
/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */
struct ELOGString
{
    int mi_Canal;
    COLORREF mx_Color;
    std::basic_string<char> ms_String;
};

class ELOG_cl_Frame : public EDI_cl_BaseFrame
{
    DECLARE_DYNCREATE(ELOG_cl_Frame)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    ELOG_cl_Frame(void);
    ~ELOG_cl_Frame(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    CRichEditCtrl	*mpo_Edit;                  // Current log window
    ELOGString      ma_Strings[ELOG_MaxStringNb];
    int             mi_StringIndex;
    BOOL            mb_IsBufferFull;
	CFont			mo_Fnt;
	BOOL			mb_ValidFont;
	int				mi_SizeFont;
	char			masz_NameFont[100];
	CString			mao_Filter[10];
	BOOL			mb_FilterOut;
    UINT            mui_ActiveCanal;            // Which canal is active ?
	
    struct
    {
        BOOL    b_Freeze;
		BOOL	b_LogToFile;
    } mst_Ini;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    BOOL    PreTranslateMessage( MSG *);

    void    AddText(const char *, BOOL , COLORREF,int);
	void	DefineFont(char *, int);
	void	LogRasters(char *);
	BOOL	StringIsSkipped(CString & str);

/*$2
 ---------------------------------------------------------------------------------------------------
    INTERFACE.
 ---------------------------------------------------------------------------------------------------
 */

public:
	void	OnFilter(void);
    BOOL    b_CanDuplicate(void) { return FALSE; };
    BOOL    b_CanBeLinked(void) { return FALSE; };
    BOOL    b_CanBeLinkedToEngine(void) { return FALSE; };
    int     i_OnMessage(ULONG, ULONG, ULONG);
    char    *psz_OnActionGetBase(void) { return ELOG_asz_ActionBase; };
    void    OnAction(ULONG);
    BOOL    b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
    UINT    ui_OnActionState(ULONG);
    BOOL    b_KnowsKey(USHORT);
    void    ReinitIni(void);
    void    LoadIni(void);
    void    TreatIni(void);
    void    SaveIni(void);
    void    OnToolBarCommand(UINT);
    void    OnToolBarCommandUI(UINT, CCmdUI *);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnSize(UINT, int, int);
	afx_msg BOOL	OnMouseWheel(UINT, short, CPoint);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
