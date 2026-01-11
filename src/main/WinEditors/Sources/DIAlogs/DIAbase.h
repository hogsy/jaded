/*$T DIAbase.h GC 1.134 04/28/04 09:29:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_BaseDialog :
	public CDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_BaseDialog(UINT,CWnd* pParentWnd = NULL);
	~EDIA_cl_BaseDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	UINT	mui_Res;
	BOOL	mb_Modeless;
	CRect	mo_BeforeMin;
	BOOL	mb_Minimize;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	virtual int DoModal(void);
	virtual int DoModeless(void);
	LRESULT		WindowProc(UINT, WPARAM, LPARAM);
	virtual int i_OnMessage(ULONG, ULONG, ULONG)	{ return 1; };
	static void SetTheme(CWnd *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg void	OnOK(void);
	afx_msg void	OnYes(void);
	afx_msg void	OnNo(void);
	afx_msg void	OnCancel(void);
	afx_msg void	OnDestroy(void);
	afx_msg void	OnClose(void);
	afx_msg void	OnNcLButtonDown(UINT, CPoint);
	afx_msg void	OnNcLButtonUp(UINT, CPoint);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg BOOL	OnInitDialog(void);
	afx_msg HBRUSH	OnCtlColor(CDC *, CWnd *, UINT);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
