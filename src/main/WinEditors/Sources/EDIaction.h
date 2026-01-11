/*$T EDIaction.h GC!1.41 08/04/99 12:25:52 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"

#define SHIFT   0x0100
#define CONTROL 0x0200
#define ALT     0x0400

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_Action
{
public:
    CString mo_DisplayName;
	CString	mo_RealDisplayName;
    CString mo_Key;
    UINT    mui_Resource;
    ULONG   mul_Action;
    USHORT  muw_Key;
    UINT    mui_State;
    BOOL    mb_Dyn;
    UINT    mui_NumDown;
	UINT	mui_AddState;
	BOOL	mb_Disabled;
	EDI_cl_Action(void)
	{
		mb_Disabled = FALSE;
	};
};

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_ConfigList
{
public:
    CList<EDI_cl_Action *, EDI_cl_Action *> mo_List;
};

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_BaseFrame;
class   EDI_cl_ActionList
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_ActionList (void);
    ~EDI_cl_ActionList(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseFrame                                *mpo_Editor;
    CString                                         mo_Name;
    int                                             mi_CurrentList;
    CList<EDI_cl_ConfigList *, EDI_cl_ConfigList *> mo_List;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    BOOL            b_ActionKnowsKey(USHORT);
    void            ReadFile(char *);
    void            SaveUser(ULONG, USHORT, CString &);
    void            ParseBuffer(char *);
    USHORT          uw_StringToKey(char *);
    void            DeleteAll(void);
    CString         o_KeyToString(char, USHORT &);
    ULONG           ul_KeyToAction(USHORT);
    EDI_cl_Action   *po_GetActionById(ULONG);
    USHORT          uw_GetKeyOfAction(ULONG);
};
#endif /* ACTIVE_EDITORS */
