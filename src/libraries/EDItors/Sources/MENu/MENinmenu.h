/*$T MENinmenu.h GC! 1.081 05/30/00 09:20:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#include "BASe/CLIbrary/CLIwin.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EMEN_cl_ToolBar : public CToolBar
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	LoadFullColorToolBar(int nResID);
	BOOL	LoadFullColorToolBar(LPCTSTR lpszResourceName);
	BOOL	LoadFullColorBitmap(LPCTSTR lpszResourceName);
	HBITMAP LoadMappedFullColorBitmap(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono);
	HBITMAP LoadMappedFullColorBitmap1(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono);
	void	EraseNonClient(void);
	void	ComputeDisabledBar(void);
	CImageList	mo_DisabledIL;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnSysColorChange(void);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnPaint(void);
	DECLARE_MESSAGE_MAP()
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EMEN_cl_Menu;

class EMEN_cl_InMenu : public CFrameWnd
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EMEN_cl_InMenu(UINT _ui_Res){ mui_ResTB = _ui_Res; };
	~EMEN_cl_InMenu(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EMEN_cl_ToolBar mo_Toolbar;
	int				mi_MinPos;
	UINT			mui_ResTB;
	EMEN_cl_Menu	*mpo_Parent;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg LRESULT	OnNcHitTest(CPoint);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnSizing(UINT, LPRECT);
	afx_msg void	OnCommand(UINT);
	afx_msg void	OnCommandUI(CCmdUI *);
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
