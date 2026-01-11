/*$T DIAorder_dlg.h GC!1.71 01/18/00 11:01:44 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "VAVview/VAVview.h"

typedef void (*EDIA_tdpfnv_OrderCB) (ULONG, ULONG);

class EDIA_cl_OrderDialog : public EDIA_cl_BaseDialog
{
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_OrderDialog (char *_psz_Title, BOOL = FALSE, BOOL = TRUE);
    ~EDIA_cl_OrderDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    CList<CString, CString> mo_StrList;
    CList<ULONG, ULONG>     mo_DataList;
    EDIA_tdpfnv_OrderCB     mpfn_CB;
	BOOL					mb_LessPlus;
	BOOL					mb_SetContent;
	char					*mpz_Title;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    void    InitList(void);
    void    Swap(ULONG, ULONG);
    void    AddItem(char *, ULONG);
    void    SetCallback(EDIA_tdpfnv_OrderCB);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    BOOL    OnInitDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg void	OnSize(UINT, int, int);
    afx_msg void    OnUp(void);
    afx_msg void    OnDown(void);
    afx_msg void    OnPlus(void);
    afx_msg void    OnLess(void);
    afx_msg void    OnSetContent(void);
    afx_msg void    OnResetContent(void);
    afx_msg void    OnInsertContent(void);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
