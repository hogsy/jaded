/*$T EDIapp.h GC! 1.076 03/08/00 16:12:05 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "BIGfiles/BIGdefs.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDI_cl_EnterWnd : public CFrameWnd
{
public:
	ULONG			windowsizex, windowsizey;
	CDC				memdc;
	CBitmap			bmp, *old;
	CString			mo_Msg;

public:
	void			DisplayMessage(char *);
	BOOL			PreCreateWindow(CREATESTRUCT &);
	void			Paint(void);
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg void	OnDestroy(void);
	DECLARE_MESSAGE_MAP()
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDI_cl_ToolTipWnd : public CFrameWnd
{
public:
	char 			mo_Msg[20][256]; /* 20 lines maximum */
	ULONG			ulNumberOfLines;
	ULONG			ulSpeed;

public:
	void			DisplayMessage(char *,ULONG ulSpeed);
	void			Display();
	BOOL			PreCreateWindow(CREATESTRUCT &);
	void			Paint(void);
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg void	OnDestroy(void);
	DECLARE_MESSAGE_MAP()
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EDI_cl_MainFrame;
class	EDI_cl_BaseFrame;
class	EDI_cl_BaseView;
class EDI_cl_App : public CWinApp
{
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDI_cl_MainFrame	*mpo_MainFrame; /* Main application frame */
	BOOL				mb_RunEngine;
	HHOOK				mh_HookMsg;
	HHOOK				mh_HookKeyboard;
	HHOOK				mh_HookMouse;
	CWnd				*mpo_LastWnd;
	CWnd				*mpo_FocusWndWhenDlg;
	EDI_cl_BaseView		*mpo_LastView;

	MSG					*mp_SimuMsg;
	int					mi_NumSimuMsg;
	int					mi_MaxSimuMsg;
	char				masz_ToOpen[L_MAX_PATH];

	EDI_cl_ToolTipWnd	*EDI_gpo_EnterWnd2; /* Philippe Tipwindow */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDI_cl_BaseFrame	*GetFocusedEditor(void);
	void				SetFocusPaneColor(void);
	BOOL				b_DispatchKey(USHORT, BOOL, BOOL b = FALSE);
	BOOL				b_ProcessMouseWheel(MSG *);
	void				AddModeless(HWND, BOOL = FALSE);
	void				RemoveModeless(HWND);
	EDI_cl_BaseFrame	*po_EditorUnderMouse(void);

	BOOL				SearchHelp(char *, BOOL = TRUE);

	void				SaveSimuMsg(MSG *, int, char *);
	void				LoadSimuMsg(MSG **, int *, int *, char *);
	void				AddSimuMsg(MSG **, int *, int *, MSG *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	virtual BOOL	InitInstance(void);
	virtual int		ExitInstance(void);
	virtual BOOL	PumpMessage(void);
	virtual int		Run(void);
	virtual BOOL	OnIdle(LONG);
	virtual void	OnRealIdle(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	DECLARE_MESSAGE_MAP()

private:
	bool editorMode{ false };

public:
	bool IsEditorMode() const { return editorMode; }
};
extern EDI_cl_App			EDI_go_TheApp;
extern BOOL					EDI_gb_LockKeyboard;
extern BOOL					EDI_gb_LockMouse;
extern BOOL					EDI_gb_LockHook;
extern BOOL					EDI_gb_TopMenuMode;
extern HWND					EDI_gh_TopMenuMode;
extern BOOL					EDI_gb_DuringInit;
extern CList<HWND, HWND>	APP_go_Modeless;
extern CList<HWND, HWND>	APP_go_ModelessDia;
extern CList<HWND, HWND>	APP_go_ModelessNoRef;
extern EDI_cl_EnterWnd		*EDI_gpo_EnterWnd;
extern BIG_tdst_BigFile		EDI_gst_HelpFile;

#endif /* ACTIVE_EDITORS */
