/*$T EDImainframe.cpp GC! 1.100 03/05/01 12:18:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/VERsion/VERsion_Check.h"
#include "BIGfiles/VERsion/VERsion_Number.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "EDIapp.h"
#include "EDIstrings.h"
#include "EDIerrid.h"
#include "EDIaction.h"
#include "EDIapp_options.h"
#include "MATHs/MATH.h"

#define ACTION_GLOBAL

#include "EDImainframe_act.h"
#include "DIAlogs/DIAmessage_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDIpaths.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/ENGvars.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKstruct.h"
#include "EDImsg.h"
#include "EDIicons.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/RASters/RASframe.h"
#include "DIAlogs/DIAchecklist_dlg.h"
#include "DIAlogs/DIAjoystick_dlg.h"
#include "DIAlogs/DIAlang_dlg.h"
#include "INOut/INOjoystick.h"
#include "INOut/INO.h"
#include "BIGFiles/STReams/STReamaccess.h"
#include "BIGFiles/STReams/STReamBigFile.h"

#include "DATaControl/DATCPerforce.h"
/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

ERAS_cl_Frame	*gpo_RasterEditor = NULL;
extern BOOL		ERR_gb_Log;
extern char		EDI_gaz_Message[4096];

/* Additionnal bitmap list for files */
CMapPtrToPtr	EDI_mo_FileIcons16;
CMapPtrToPtr	EDI_mo_FileIcons32;
CMapPtrToPtr	EDI_mo_FileIconsKey16;
CMapPtrToPtr	EDI_mo_FileIconsKey32;
CMapPtrToPtr	EDI_mo_FileKeyBmp;
BOOL			EDI_gb_HasClosed = FALSE;
EDI_cl_MainFrame *EDI_gpstMainFrame = NULL;

#ifdef JADEFUSION
//Feature text to display in the menu Option
char*			EDI_Feature_Text[EDI_MTL_MAX_FEATURES] =
				{
				"CheckWorld Dialog",
				"SubObject Keyboard ShortCut",
				"Advance Rename",
				"Auto-Rename Create WayPoint",
				"PreFaB instance rename",
				"Duplicate Extended"
				};
#endif
extern BOOL EDI_gb_Automated;
extern BOOL EDI_gb_ExportToKeys;
extern BOOL EDI_gb_ImportFromKeys;
#ifdef MONTREAL_SPECIFIC
extern BOOL EDI_gb_DontCheckVersion;
extern BOOL EDI_gb_DontCheckKeyServer;
#endif 

bool g_bNewlyCreatedBF = false;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDI_cl_MainFrame, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(EDI_ID_NEW, OnProjectNew)
	ON_COMMAND(EDI_ID_OPEN, OnProjectOpen)
	ON_COMMAND(EDI_ID_EXIT, OnExit)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_MainFrame::EDI_cl_MainFrame(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION	pos;
	int			i_Index, i_Index1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_gpstMainFrame = this;

	*mst_Ini.asz_CurrentRefFile = 0;

	EDI_gaz_Message[0] = 0;

	mpo_MenuView = new EDI_cl_BaseView(0);
	mpo_LeftView = new EDI_cl_BaseView(1);
	mpo_RightView = new EDI_cl_BaseView(2);
	mpo_CornerView = new EDI_cl_BaseView(3);
	mpo_BottomView = new EDI_cl_BaseView(4);
	mpo_PipeFrame = NULL;
	x_TimeVSS = 0;
	x_TimeREF = 0;

	mpo_MaxView = NULL;
	mpo_Menu = NULL;
	mpo_MenuBar = NULL;

	mpo_MenuFrame = NULL;

	mi_TotalEditors = 0;
	mi_NumLockDisplay = 0;
	L_memset(mai_LockCount, 0, sizeof(mai_LockCount));
	L_memset(mapo_LockWnd, 0, sizeof(mapo_LockWnd));

	mi_VssCanWrite = FALSE;
	EDI_gst_DragDrop.b_BeginDragDrop = FALSE;

	/* Additionnal bmp list */
	pos = EDI_mo_FileKeyBmp.GetStartPosition();
	while(pos)
	{
		EDI_mo_FileKeyBmp.GetNextAssoc(pos, (void * &) i_Index, (void * &) i_Index1);
		DeleteObject((HBITMAP) i_Index1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_MainFrame::~EDI_cl_MainFrame(void)
{
	mo_BigSplitter.DestroyWindow();

	/* Delete common font */
	mo_Fnt.DeleteObject();
	mo_Fnt1.DeleteObject();
	mo_Fnt2.DeleteObject();

	/* Delete views */
	delete mpo_MenuView;
	delete mpo_LeftView;
	delete mpo_RightView;
	delete mpo_CornerView;
	delete mpo_BottomView;

	/* Delete toolbar */
	if(mpo_MenuBar)
	{
		mpo_MenuBar->DestroyWindow();
		delete mpo_MenuBar;
	}

	DeleteObject(mh_MainIcon);

	EDI_gpstMainFrame = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	LOGFONT *plf;
	int		i;
	UINT	ar[3] = { 0, 1, 2 };
	L_FILE	x_File;
	CBitmap o_Bmp, o_Bmp1;
	char	asz_Path[L_MAX_PATH], *psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* App icon */
	mh_MainIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(mh_MainIcon, FALSE);

	if(!CFrameWnd::OnCreateClient(lpcs, pContext)) return FALSE;

	/* Init main window */
	MAI_gh_MainWindow = m_hWnd;

	/* Create the icon image list for files */
	mo_FileImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 10);
	mo_FileImageList1.Create(32, 32, ILC_COLOR16 | ILC_MASK, 0, 10);
	o_Bmp.LoadBitmap(MAKEINTRESOURCE(EDI_IDR_IMAGELIST));
	mo_FileImageList.Add(&o_Bmp, RGB(192, 192, 192));
	o_Bmp1.LoadBitmap(MAKEINTRESOURCE(EDI_IDR_IMAGELIST1));
	mo_FileImageList1.Add(&o_Bmp1, RGB(192, 192, 192));

	/* Create action list */
	mpo_Actions = new EDI_cl_ActionList;

	/* Create font. */
	plf = (LOGFONT *) LocalAlloc(LPTR, sizeof(LOGFONT));
	lstrcpy(plf->lfFaceName, EDI_STR_Csz_Font);
	plf->lfHeight = 14;
	plf->lfWeight = 500;
	plf->lfEscapement = 0;
	mo_Fnt.CreateFontIndirect(plf);
	LocalFree((LOCALHANDLE) plf);

	plf = (LOGFONT *) LocalAlloc(LPTR, sizeof(LOGFONT));
	lstrcpy(plf->lfFaceName, EDI_STR_Csz_Font);
	plf->lfHeight = 14;
	plf->lfWeight = 500;
	plf->lfEscapement = plf->lfOrientation = 900;
	mo_Fnt1.CreateFontIndirect(plf);
	LocalFree((LOCALHANDLE) plf);

	plf = (LOGFONT *) LocalAlloc(LPTR, sizeof(LOGFONT));
	lstrcpy(plf->lfFaceName, EDI_STR_Csz_Font);
	plf->lfHeight = 13;
	plf->lfWeight = 500;
	plf->lfEscapement = 0;
	mo_Fnt2.CreateFontIndirect(plf);
	LocalFree((LOCALHANDLE) plf);

	/* Create splitters. */
	mo_BigSplitter.CreateStatic(this, 1, 2, WS_VISIBLE | WS_CHILD);
	mo_Splitter.CreateStatic(&mo_BigSplitter, 1, 2, WS_CHILD | WS_VISIBLE, mo_BigSplitter.IdFromRowCol(0, 1));
	mo_Splitter1.CreateStatic(&mo_Splitter, 2, 1, WS_CHILD | WS_VISIBLE, mo_Splitter.IdFromRowCol(0, 0));
	mo_Splitter2.CreateStatic(&mo_Splitter, 2, 1, WS_CHILD | WS_VISIBLE, mo_Splitter.IdFromRowCol(0, 1));

	/* Create status bar. */
	mo_Status.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | WS_CLIPCHILDREN);
	mo_Status.SetFont(&mo_Fnt);
	mo_Status.SetIndicators(ar, 3);
	mo_Status.SetPaneInfo(0, 0, SBPS_STRETCH, 10);
	mo_Status.SetPaneInfo(1, 1, SBPS_STRETCH, 10);
	mo_Status.SetPaneInfo(2, 2, SBPS_NORMAL, 300);
	mo_Status.UpdateWindow();

	/* Create the menu view */
	if
	(
		mpo_MenuView->Create
			(
				WS_VISIBLE | WS_CLIPCHILDREN | TCS_OWNERDRAWFIXED,
				CRect(0, 0, 0, 0),
				&mo_BigSplitter,
				mo_BigSplitter.IdFromRowCol(0, 0)
			) == FALSE
	) return FALSE;

	mpo_MenuView->SetFont(&mo_Fnt);
	mpo_MenuView->mpsz_Name = "M";

	/* Create the left view */
	if
	(
		mpo_LeftView->Create
			(
				WS_VISIBLE | WS_CLIPCHILDREN | TCS_OWNERDRAWFIXED,
				CRect(0, 0, 0, 0),
				&mo_Splitter1,
				mo_Splitter1.IdFromRowCol(0, 0)
			) == FALSE
	) return FALSE;

	mpo_LeftView->SetFont(&mo_Fnt);
	mpo_LeftView->mpsz_Name = "L";

	/* Create the right view */
	if
	(
		mpo_RightView->Create
			(
				WS_VISIBLE | WS_CLIPCHILDREN | TCS_OWNERDRAWFIXED,
				CRect(0, 0, 0, 0),
				&mo_Splitter2,
				mo_Splitter2.IdFromRowCol(0, 0)
			) == FALSE
	) return FALSE;

	mpo_RightView->SetFont(&mo_Fnt);
	mpo_RightView->mpsz_Name = "R";

	/* Create the corner view */
	if
	(
		mpo_CornerView->Create
			(
				WS_VISIBLE | WS_CLIPCHILDREN | TCS_OWNERDRAWFIXED,
				CRect(0, 0, 0, 0),
				&mo_Splitter1,
				mo_Splitter1.IdFromRowCol(1, 0)
			) == FALSE
	) return FALSE;

	mpo_CornerView->SetFont(&mo_Fnt);
	mpo_CornerView->mpsz_Name = "C";

	/* Create the bottom view */
	if
	(
		mpo_BottomView->Create
			(
				WS_VISIBLE | WS_CLIPCHILDREN | TCS_OWNERDRAWFIXED,
				CRect(0, 0, 0, 0),
				&mo_Splitter2,
				mo_Splitter2.IdFromRowCol(1, 0)
			) == FALSE
	) return FALSE;

	mpo_BottomView->SetFont(&mo_Fnt);
	mpo_BottomView->mpsz_Name = "B";

	/* Set size of each pane item */
	for(i = 0; i < EDI_C_MaxViews; i++)
		po_GetViewWithNum(i)->SetItemSize(CSize(70, 16));

	/* First init all default values. */
	mst_ExternIni.asz_DataBigFileName[0] = '\0';
	mst_ExternIni.st_Placement.length = sizeof(WINDOWPLACEMENT);
	mst_ExternIni.st_Placement.flags = WPF_RESTORETOMAXIMIZED;
	mst_ExternIni.st_Placement.showCmd = SW_SHOWMAXIMIZED;
	mst_ExternIni.st_Placement.ptMinPosition.x = -1;
	mst_ExternIni.st_Placement.ptMinPosition.y = -1;
	mst_ExternIni.st_Placement.ptMaxPosition.x = -1;
	mst_ExternIni.st_Placement.ptMaxPosition.y = -1;
	mst_ExternIni.st_Placement.rcNormalPosition.left = 0;
	mst_ExternIni.st_Placement.rcNormalPosition.top = 0;
	mst_ExternIni.st_Placement.rcNormalPosition.right = 300;
	mst_ExternIni.st_Placement.rcNormalPosition.bottom = 300;
	mst_ExternIni.b_SynchroEditorsData = FALSE;
	L_strcpy(mst_ExternIni.asz_PathHelp, "Help/");

	/* Load external ini file. */
	L_strcpy(asz_Path, EDI_go_TheApp.m_pszHelpFilePath);
	psz_Temp = L_strrchr(asz_Path, '\\');
	if(!psz_Temp) psz_Temp = L_strrchr(asz_Path, '/');
	if(psz_Temp)
		L_strcpy(psz_Temp + 1, EDI_Csz_ExternalIniFile);
	else
		L_strcpy(asz_Path, EDI_Csz_ExternalIniFile);

	if(!L_access(asz_Path, 0))
	{
		x_File = L_fopen(asz_Path, L_fopen_RB);
		if(CLI_FileOpen(x_File))
		{
			L_fread(&mst_ExternIni, sizeof(mst_ExternIni), 1, x_File);
			L_fclose(x_File);
		}
	}

	/* Check structure */
_Try_
	ERR_X_Error
	(
		mst_ExternIni.st_Placement.length == sizeof(WINDOWPLACEMENT),
		EDI_ERR_Csz_IniCorrupt,
		EDI_Csz_ExternalIniFile
	);

_Catch_
_End_
	/* Open last project */
_Try_
	ENG_gb_GlobalLock = TRUE;

	// If we are running in automatic mode and importing from keys in P4, 
	// we need to create a new bf...
	if (EDI_gb_Automated && EDI_gb_ImportFromKeys)
	{
		BIG_CreateEmpty(EDI_go_TheApp.masz_ToOpen);
		g_bNewlyCreatedBF = true;
	}

	if(*EDI_go_TheApp.masz_ToOpen)
	{
		OpenProject(EDI_go_TheApp.masz_ToOpen);
	}
	else
	{
		OpenProject(mst_ExternIni.asz_DataBigFileName);
	}

	ENG_gb_GlobalLock = FALSE;
_Catch_
	mst_ExternIni.asz_DataBigFileName[0] = '\0';
_End_
	RecalcLayout();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnSize(UINT n, int x, int y)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	CFrameWnd::OnSize(n, x, y);

	if(mpo_Menu)
	{
		mpo_MenuBar->GetClientRect(&o_Rect);
		o_Rect.top += 2;
		o_Rect.bottom -= 1;
		mpo_Menu->MoveWindow(o_Rect);
	}

	RecalcLayout();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_TabCtrlH, i_CaptionH, i_FrameH, i_FrameW;
	EDI_cl_BaseFrame	*po_Editor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CFrameWnd::OnGetMinMaxInfo(lpMMI);

	/* Special mode for a very maximized editor... */
	if(mpo_MaxView && mst_Desktop.b_VeryMaximized)
	{
		i_FrameW = GetSystemMetrics(SM_CXSIZEFRAME);
		i_FrameH = GetSystemMetrics(SM_CYSIZEFRAME) - 2;
		i_CaptionH = GetSystemMetrics(SM_CYCAPTION);
		i_TabCtrlH = 16;

		lpMMI->ptMaxPosition.x -= i_FrameW;
		lpMMI->ptMaxPosition.y -= i_CaptionH + i_TabCtrlH;
		lpMMI->ptMaxSize.x += (4 * i_FrameW);
		lpMMI->ptMaxSize.y += i_CaptionH + i_TabCtrlH;

		/* Status bar */
		lpMMI->ptMaxPosition.y -= 20;
		lpMMI->ptMaxSize.y += 40 + i_FrameH;

		/* Top menu of main frame */
		if(mpo_MenuBar)
		{
			lpMMI->ptMaxPosition.y -= 20;
			lpMMI->ptMaxSize.y += 20;
		}

		/* Top menu of maximized editor */
		po_Editor = mpo_MaxView->po_GetActivatedEditor();
		if(po_Editor && po_Editor->mpo_Menu && po_Editor->mst_BaseIni.b_TopMenu)
		{
			if(po_Editor->mst_BaseIni.mb_LeftMode)
			{
				lpMMI->ptMaxPosition.x -= 20;
				lpMMI->ptMaxSize.x += 20;
			}
			else
			{
				lpMMI->ptMaxPosition.y -= 20;
				lpMMI->ptMaxSize.y += 20;
			}
		}

		/* Editor top dialog bar */
		if(po_Editor && po_Editor->mpo_DialogBar && po_Editor->mst_BaseIni.b_DialogBarVisible)
		{
			lpMMI->ptMaxPosition.y -= 20;
			lpMMI->ptMaxSize.y += 20;
		}

		lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x;
		lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y;
	}
}

/*$4
 ***********************************************************************************************************************
    ENGINE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::BeforeEngine(void)
{
	/*~~~~~~*/
	UINT	i;
	/*~~~~~~*/

	LINK_PrintStatusMsg("Engine Running...");
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->BeforeEngine();
	mo_BigSplitter.Invalidate(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::AfterEngine(void)
{
	/*~~~~~~*/
	UINT	i;
	/*~~~~~~*/

	LINK_PrintStatusMsg("...Engine Stopped");
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->AfterEngine();
	mo_BigSplitter.Invalidate(FALSE);
}

extern ULONG	ulRealIdleCounter;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OneTrameEnding(void)
{
	/*~~~~~~*/
	UINT	i;
	/*~~~~~~*/

	if(ENG_gb_ForcePauseEngine) return;
	if(EDI_go_TheApp.EDI_gpo_EnterWnd2->ulSpeed && (ulRealIdleCounter != 0xFFFFFFFF))
	{
		if(timeGetTime() - ulRealIdleCounter >= EDI_go_TheApp.EDI_gpo_EnterWnd2->ulSpeed)
		{
			ulRealIdleCounter = 0xFFFFFFFF;
			EDI_go_TheApp.EDI_gpo_EnterWnd2->Display();
		}
	}

	/* Informs editors */
	gpo_RasterEditor = NULL;
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->OneTrameEnding();

	/* Force painting for focus pane */
	EDI_go_TheApp.SetFocusPaneColor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::RefreshCheckDlg(void)
{
	/*~~~~~~~~~~~~~*/
	POSITION	pos;
	HWND		hwnd;
	EDIA_cl_BaseDialog *po_Dlg;
	/*~~~~~~~~~~~~~*/

	pos = APP_go_Modeless.GetHeadPosition();
	while(pos)
	{
		hwnd = APP_go_Modeless.GetNext(pos);
		if(APP_go_ModelessNoRef.Find(hwnd) == NULL)
		{
			po_Dlg = (EDIA_cl_BaseDialog *) CWnd::FromHandle(hwnd);
			po_Dlg->i_OnMessage(EDI_MESSAGE_REFRESHDLG, 0, 0);
			::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Only to set an exception handling before process all messages.
 =======================================================================================================================
 */
LRESULT EDI_cl_MainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~*/
	LRESULT lResult;
	/*~~~~~~~~~~~~*/

_Try_
	lResult = CFrameWnd::WindowProc(message, wParam, lParam);
_Catch_
	lResult = 0;
_End_
	return lResult;
}

/*
 =======================================================================================================================
    Aim:    This MFC function is called for every messages. Normal call in editor mode. Engine call in engine mode
            (engine is running).
 =======================================================================================================================
 */
LRESULT EDI_cl_MainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~*/
	LRESULT result;
	/*~~~~~~~~~~~*/

	// awful hack to sort out anim panel ~hogsy
	if ( message == WM_MOVING )
	{
		for ( unsigned int i = 0; i < mi_TotalEditors; ++i )
		{
			if ( mast_ListOfEditors[ i ].po_Instance == nullptr )
			{
				continue;
			}
			mast_ListOfEditors[ i ].po_Instance->SendMessage( WM_MOVING, wParam, lParam );
		}
	}

	if(MAI_b_TreatMainWndMessages(m_hWnd, message, wParam, lParam, &result)) return result;
	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_MainFrame::MessageBox(char *lpszText, char *lpszCaption, UINT nType)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_MessageDialog	o_Dialog(lpszText, lpszCaption, nType);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	return o_Dialog.DoModal();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::LockDisplay(CWnd *_po_Wnd)
{
	/*~~~~~~~~~~~~*/
	int		i;
	int		i_Res;
	CWnd	*po_Wnd;
	/*~~~~~~~~~~~~*/

	if(_po_Wnd == NULL) return;

	i_Res = mi_NumLockDisplay;
	for(i = 0; i < mi_NumLockDisplay; i++)
	{
		if(mapo_LockWnd[i] == _po_Wnd)
		{
			mai_LockCount[i]++;
			return;
		}

		/* Search if there's a locked parent */
		po_Wnd = _po_Wnd->GetParent();
		while(po_Wnd)
		{
			if(po_Wnd == mapo_LockWnd[i]) return;
			po_Wnd = po_Wnd->GetParent();
		}

		if(mapo_LockWnd[i] == NULL) i_Res = i;
	}

	if(!_po_Wnd->IsWindowVisible()) return;
	po_Wnd = _po_Wnd->GetParent();
	while(po_Wnd)
	{
		if(!po_Wnd->IsWindowVisible()) return;
		po_Wnd = po_Wnd->GetParent();
	}

	mai_LockCount[i_Res] = 1;
	mapo_LockWnd[i_Res] = _po_Wnd;
	_po_Wnd->HideCaret();
	_po_Wnd->SetRedraw(FALSE);

	if(i_Res == mi_NumLockDisplay) mi_NumLockDisplay++;
}

/*
 =======================================================================================================================
    Aim:    To unlock display in a given window.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::UnlockDisplay(CWnd *_po_Wnd, CRect *_po_Rect, BOOL _b_Update)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < mi_NumLockDisplay; i++)
	{
		if(mapo_LockWnd[i] == _po_Wnd)
		{
			mai_LockCount[i]--;
			if(mai_LockCount[i] == 0)
			{
				mapo_LockWnd[i] = NULL;
				if(_b_Update)
				{
					_po_Wnd->SetRedraw(TRUE);
					_po_Wnd->RedrawWindow(_po_Rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
					_po_Wnd->ShowCaret();
				}

				if(i == mi_NumLockDisplay - 1) mi_NumLockDisplay--;
			}

			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::LockWindowUpdate(CWnd *_po_Wnd)
{
	/*~~~~~~~~~~~~*/
	CWnd	*po_Wnd;
	/*~~~~~~~~~~~~*/

	if(!_po_Wnd->IsWindowVisible()) return;
	po_Wnd = _po_Wnd->GetParent();
	while(po_Wnd)
	{
		if(!po_Wnd->IsWindowVisible()) return;
		po_Wnd = po_Wnd->GetParent();
	}

	_po_Wnd->LockWindowUpdate();
}

/*
 =======================================================================================================================
    Aim:    To update frame title of the application depending on current data file.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::UpdateMainFrameTitle(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	/*~~~~~~~~~~~~~~~~~~*/

	sprintf(asz_Temp, "JADED Editor (Version %03d-%03d)", BIG_Cu4_AppVersion, BIG_Cu4_Version);

#ifdef SPEED_EDITORS
	L_strcat( asz_Temp, " (R)" );
#endif
	L_strcat( asz_Temp, " -- " );
	L_strcat(asz_Temp, __DATE__);
	L_strcat(asz_Temp, "  --  ");
	if(mst_ExternIni.asz_DataBigFileName[0] == '\0')
	{
		L_strcat(asz_Temp, "No Open Project");
	}
	else
	{
		L_strcat(asz_Temp, mst_ExternIni.asz_DataBigFileName);
	}

#if 0 // removing perforce ~hogsy
	L_strcat(asz_Temp, " -- [Perforce: ");
	if (DAT_CPerforce::GetInstance()->IsEnabled())
	{	
		L_strcat(asz_Temp, DAT_CPerforce::GetInstance()->GetPort());
		L_strcat(asz_Temp, " - ClientView: ");
		L_strcat(asz_Temp, DAT_CPerforce::GetInstance()->GetClient());
	}
	else
	{
		L_strcat(asz_Temp, "DISABLED");
	}
	L_strcat(asz_Temp, "]");
#endif

	SetWindowText(asz_Temp);
}

/*
 =======================================================================================================================
    Aim:    Call to size the 3 panes of the standard dialog bar PANES_IDD_DIALOGBAR. Each pane will have the same width
            depending on editor width. (except for texture editor : path take half, two others take quarter
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SizePanesDialogBar(EDI_cl_BaseFrame *_po_Edit)
{
	/*~~~~~~~~~~~~*/
	CRect	o_Rect;
	int		cxx, w1, w2;
	CWnd	*po_Wnd;
	/*~~~~~~~~~~~~*/

	_po_Edit->mpo_DialogBar->GetWindowRect(o_Rect);
	cxx = o_Rect.right - o_Rect.left;
    
    if (_po_Edit->mst_Def.i_Type == EDI_IDEDIT_TEXTURE)
    {
        w1 = 2;
        w2 = 4;
    }
    else
    {
        w1 = w2 = 3;
    }

	/* Move statics inside dialog */
	po_Wnd = _po_Edit->mpo_DialogBar->GetDlgItem(IDC_PATH);
	po_Wnd->GetWindowRect(&o_Rect);
	_po_Edit->mpo_DialogBar->ScreenToClient(&o_Rect);
	o_Rect.left = 2;
	o_Rect.right = o_Rect.left + cxx / w1;
	po_Wnd->MoveWindow(&o_Rect);

	po_Wnd = _po_Edit->mpo_DialogBar->GetDlgItem(IDC_FILE);
	o_Rect.left = o_Rect.right + 2;
	o_Rect.right = o_Rect.left + cxx / w2;
	po_Wnd->MoveWindow(&o_Rect);

	po_Wnd = _po_Edit->mpo_DialogBar->GetDlgItem(IDC_INFOS);
	o_Rect.left = o_Rect.right + 2;
	o_Rect.right = cxx;
	po_Wnd->MoveWindow(&o_Rect);
}

/*
 =======================================================================================================================
    Aim:    Erase background if no project open, else not (cause the splitter fill all the window client area).
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::OnEraseBkgnd(CDC *pDC)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	DWORD	x_Col;
	/*~~~~~~~~~~~*/

	x_Col = GetSysColor(COLOR_3DSHADOW);
	if(mst_ExternIni.asz_DataBigFileName[0] == '\0')
	{
		GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		pDC->FillSolidRect(o_Rect, x_Col);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ForceClose(void)
{
	LINK_gb_AllRefreshEnable = FALSE;
	CFrameWnd::OnClose();
}
/**/
void EDI_cl_MainFrame::OnClose(void)
{
	/*~~~~~~*/
	UINT	i;
	/*~~~~~~*/

	//if(GetAsyncKeyState(VK_SHIFT) < 0) ExitProcess(0);

	/* Request user */
	if(mst_Ini.mst_Options.b_AskExit && !ERR_gb_Log)
	{
		if(MessageBox(EDI_STR_Csz_SureExit, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDNO) return;
	}

	/* Close project */
	ENG_gb_ExitApplication = TRUE;
	if(b_CloseProject(FALSE) == FALSE)
	{
		ENG_gb_ExitApplication = FALSE;
		return;
	}

	mo_Splitter.ShowWindow(SW_HIDE);
	mo_Splitter1.ShowWindow(SW_HIDE);
	mo_Splitter2.ShowWindow(SW_HIDE);

	/* Inform editors */
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->WhenAppClose();

	/* Save infos in ini file. */
	SaveExternIni();

	/*
	 * Delete action list. £
	 * We must delete action list here (and not in destructor) cause the destructor of
	 * action list will delete itself from mo_ActionList. If we call the destructor in
	 * mainframe destructor, the list will be invalid. £
	 * Here we are sure that mo_ActionList is still correct.
	 */
	delete mpo_Actions;

	/* Close */
	PostQuitMessage(0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SaveExternIni(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_FILE	x_File;
	char	asz_Path[L_MAX_PATH];
	char	*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetWindowPlacement(&mst_ExternIni.st_Placement);

	L_strcpy(asz_Path, EDI_go_TheApp.m_pszHelpFilePath);
	psz_Temp = L_strrchr(asz_Path, '\\');
	if(!psz_Temp) psz_Temp = L_strrchr(asz_Path, '/');
	if(psz_Temp)
		L_strcpy(psz_Temp + 1, EDI_Csz_ExternalIniFile);
	else
		L_strcpy(asz_Path, EDI_Csz_ExternalIniFile);

	x_File = L_fopen(asz_Path, L_fopen_WB);
	if(CLI_FileOpen(x_File))
	{
		L_fwrite(&mst_ExternIni, sizeof(mst_ExternIni), 1, x_File);
		L_fclose(x_File);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnExit(void)
{
	if(EDI_go_TheApp.mb_RunEngine)
		ENG_gb_ExitApplication = TRUE;
	else
		PostMessage(WM_CLOSE);

	LINK_gb_CanDisplay = FALSE;
	LINK_gb_CanLog = FALSE;
}

/*$4
 ***********************************************************************************************************************
    PROJECT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    To open an existing project.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OpenProject(char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Name[255];
	char	asz_NameR[L_MAX_PATH];
	char	*psz_Temp;
	char	az[512];
	FILE	*f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Invalid project name */
	if((_psz_Name == NULL) || (*_psz_Name == '\0'))
	{
		mo_BigSplitter.ShowWindow(SW_HIDE);
		return;
	}

	// read BF platform
	char szPlatform[ 64 ] = {0};
	GetStreamByName( _psz_Name, STREAM_BFPLATFORM, szPlatform );

	// set platform if not set
	if( strlen( szPlatform ) == 0 )
	{
		// set platform to this editor's platform and read back to make sure it worked
		SetStreamByName( _psz_Name, STREAM_BFPLATFORM, BFPLATFORM_CURRENT );
		GetStreamByName( _psz_Name, STREAM_BFPLATFORM, szPlatform );

		if( stricmp( szPlatform, BFPLATFORM_CURRENT ) == 0 && !EDI_gb_Automated)
		{
			char szMessage[ 256 ];
			sprintf( szMessage, "BF is not yet associated to a platform. Associating to %s.", BFPLATFORM_CURRENT );

			if( !g_bNewlyCreatedBF )
#ifdef JADEFUSION
			M_MF( )->MessageBox( szMessage, "Info", MB_OK | MB_ICONERROR );
#else
			LINK_PrintStatusMsg(szMessage);
#endif
		}
	}

	// validate that platform is set on BF
	if( strlen( szPlatform ) == 0 )
	{
		M_MF( )->MessageBox( "Could not determine target platform! Make sure the drive where your BF is stored is an NTFS drive!", "Error", MB_OK | MB_ICONERROR );
		return;
	}

#ifndef JADEFUSION //POPOWARNING TEST PLATEFORME
	// make sure platform matches editor
	if( stricmp( szPlatform, BFPLATFORM_CURRENT ) != 0 )
	{
		char szMessage[ 256 ];
		sprintf( szMessage, "Can not open file. This BF is intended to be used with the %s editor!", szPlatform );
		M_MF( )->MessageBox( szMessage, "Platform mismatch!", MB_OK | MB_ICONERROR );
		return;
	}
#endif
	// at this point, platform is ok

	psz_Temp = L_strrchr(_psz_Name, '/');
	if(!psz_Temp) psz_Temp = L_strrchr(_psz_Name, '\\');
	if(psz_Temp) 
		sprintf(asz_Name, "Loading project %s", psz_Temp + 1);
	else
		sprintf(asz_Name, "Loading project");
	if(EDI_gpo_EnterWnd)
		EDI_gpo_EnterWnd->DisplayMessage(asz_Name);
	else
	{
		LINK_gb_CanLog = FALSE;
		LINK_PrintStatusMsg(asz_Name);
		LINK_gb_CanLog = TRUE;
	}

	/* Set global init flat */
	EDI_gb_DuringInit = TRUE;

_Try_
	/* To lock */
	ComputeLockedFileOpen(_psz_Name, asz_NameR);
#if 0
	if(!L_access(asz_NameR, 0))
	{
		ERR_X_ForceError("This bigfile is already open by someone else !!!", NULL);
		return;
	}

#endif
	f = L_fopen(asz_NameR, "w");
	if(!f)
	{
		sprintf(az, "Unable to create %s file. Is it read only ?", asz_NameR);
		ERR_X_ForceError(az, NULL);
		ExitProcess(-1);
	}

	L_fclose(f);

	/* Open bigfile */
	BIG_Open(_psz_Name);

	/* Create base hierarchy */
	BIG_ul_CreateDir(EDI_Csz_Path_GameData);
	BIG_ul_CreateDir(EDI_Csz_Path_EditData);
	BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	BIG_ul_CreateDir(EDI_Csz_Path_AnimationAni);
	BIG_ul_CreateDir(EDI_Csz_Path_Audio);
	BIG_ul_CreateDir(EDI_Csz_Path_AnimationAct);
	BIG_ul_CreateDir(EDI_Csz_Path_ObjModels);
	BIG_ul_CreateDir(EDI_Csz_Path_AIModels);
	BIG_ul_CreateDir(EDI_Csz_Path_AILib);
	BIG_ul_CreateDir(EDI_Csz_Path_COLModels);
	BIG_ul_CreateDir(EDI_Csz_Path_COLGameMaterials);
	BIG_ul_CreateDir(EDI_Csz_Path_Objects);
	BIG_ul_CreateDir(EDI_Csz_Path_Levels);
	BIG_ul_CreateDir(EDI_Csz_Ini_Keyboard);
	BIG_ul_CreateDir(EDI_Csz_Ini_Desktop);
	BIG_ul_CreateDir(EDI_Csz_Path_Texts);
	BIG_ul_CreateDir(EDI_Csz_Ini_NameKits);

	/* Editors data */
	SynchronizeEditorsData(TRUE);

	/* New project name */
	L_strcpy(mst_ExternIni.asz_DataBigFileName, _psz_Name);

	/* Init for engine */
	if(EDI_gpo_EnterWnd) 
		EDI_gpo_EnterWnd->DisplayMessage("Engine Init");
	else
		LINK_PrintStatusMsg("Engine Init");
	ENG_InitEngine();

	/* Register editors */
	RegisterEditors();

	/* Open default desktop */
	OpenWorkspace();

	/* Menu is hidden */
	::SetMenu(GetSafeHwnd(), NULL);

	/* Set focus to mainframe to avoid a dummy focus on an unkown window */
	SetFocus();

	/* Update title */
	UpdateMainFrameTitle();

	/* If an error occurs while opening bigfile, force to close project */
_Catch_
	b_CloseProject(TRUE);
_End_
	/* Reset global init flat */
	EDI_gb_DuringInit = FALSE;

	/* Open the base ? */

	// NOTE: Since we are using perforce, this check is not needed anymore.
	//if(*mst_Ini.asz_CurrentRefFile && !L_strcmpi(mst_Ini.asz_CurrentRefFile, _psz_Name))
	//{
	//	if
	//	(
	//		MessageBox
	//			(
	//				"You are trying to open the BASE !!!\nDo you want to continue ?",
	//				"Open the base",
	//				MB_YESNO
	//			) == IDNO
	//	) ExitProcess(0);
	//}

}

/*
 =======================================================================================================================
    Aim:    To close current project.

    In:     _mb_Update TRUE to see the close.
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_CloseProject(BOOL _b_Update)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_NameR[L_MAX_PATH];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Close current desktop */
	if(b_CloseWorkspace(_b_Update) == FALSE) return FALSE;

	EDI_gb_HasClosed = TRUE;

	/* Is there a project ? */
	if(mst_ExternIni.asz_DataBigFileName[0] != '\0')
	{
		/* Close bigfile */
		if(BIG_Handle() != NULL)
		{
_Try_
			BIG_Close();
			ENG_CloseEngine();
_Catch_
_End_
			BIG_Handle() = NULL;
		}
	}

	DestroyVssFile();
	DestroyRefFile();

	if(_b_Update)
	{
		/* Menu is the simple one */
		::SetMenu(GetSafeHwnd(), ::LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(MAINFRAME_IDM_MENU)));

		/* No more file name project */
		mst_ExternIni.asz_DataBigFileName[0] = '\0';

		/* Update title */
		UpdateMainFrameTitle();
	}

	/* General... */
	EDI_go_TheApp.mpo_LastWnd = NULL;	/* For focus */
	EDI_go_TheApp.mpo_FocusWndWhenDlg = NULL;
	EDI_go_TheApp.mpo_LastView = NULL;

	mi_TotalEditors = 0;

	/* Unlock */
	ComputeLockedFileOpen(BIG_gst.asz_Name, asz_NameR);
	L_unlink(asz_NameR);

	/* Force window to redraw */
	Invalidate();
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    To create a new project.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnProjectNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_Dlg(EDI_STR_Csz_TitleChooseProject, 1);
	char				asz_Temp[BIG_C_MaxLenPath];
	char				*psz_Temp;
	CString				o_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dlg.DoModal() == IDOK)
	{
		/* Get file name */
		o_Dlg.GetItem(o_Dlg.mo_File, 1, o_Temp);
		psz_Temp = (char *) (LPCSTR) o_Temp;

		/* Compute full name on disk of current file */
		L_strcpy(asz_Temp, o_Dlg.masz_FullPath);
		if(asz_Temp[L_strlen(asz_Temp) - 1] != '\\') L_strcat(asz_Temp, "\\");
		L_strcat(asz_Temp, psz_Temp);
		L_strcat(asz_Temp, BIG_Csz_BigExt);

		/* Check if file already exists */
		if(!L_access(asz_Temp, 0))
		{
			if(MessageBox(EDI_STR_Csz_ConfirmOverwrite, EDI_STR_Csz_TitleConfirm, MB_YESNO | MB_ICONQUESTION) == IDNO)
			{
				return;
			}
		}

		M_MF()->LockDisplay(&mo_BigSplitter);
		AfxGetApp()->DoWaitCursor(1);

		if(b_CloseProject(FALSE))
		{
			/* Create and open project */
			BIG_CreateEmpty(asz_Temp);
			g_bNewlyCreatedBF = true;
			OpenProject(asz_Temp);
			g_bNewlyCreatedBF = false;
			OnDesktopEqualize();
		}

		M_MF()->UnlockDisplay(&mo_BigSplitter);
		AfxGetApp()->DoWaitCursor(-1);
	}
}

/*
 =======================================================================================================================
    Aim:    To open an existing project.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnProjectOpen(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	mo_File(EDI_STR_Csz_TitleChooseProjectOpen, 1);
	char				asz_Temp[BIG_C_MaxLenPath];
	char				*psz_Temp;
	CString				o_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mo_File.DoModal() == IDOK)
	{
		M_MF()->LockDisplay(&mo_BigSplitter);
		AfxGetApp()->DoWaitCursor(1);

		/* Get file name */
		mo_File.GetItem(mo_File.mo_File, 1, o_Temp);
		psz_Temp = (char *) (LPCSTR) o_Temp;

		/* Compute full name on disk of current file */
		L_strcpy(asz_Temp, mo_File.masz_FullPath);
		if(asz_Temp[L_strlen(asz_Temp) - 1] != '\\') L_strcat(asz_Temp, "\\");
		L_strcat(asz_Temp, psz_Temp);

		/* Test if file exists */
		if(L_access(asz_Temp, 0))
		{
			ERR_X_ForceError(EDI_ERR_Csz_UnknowFile, asz_Temp);
		}

		/* If valid, open project */
		else
		{
			if(b_CloseProject(FALSE))
			{
				OpenProject(asz_Temp);
			}
		}

		/*
		 * Unlock display. Of there's no current bigfile, that means that there just have
		 * been an error while opening. In case of error, the main splitter must be
		 * hidden, so we must unlock display without updating.
		 */
		if(BIG_Handle())
		{
			M_MF()->UnlockDisplay(&mo_BigSplitter);
			AfxGetApp()->DoWaitCursor(-1);
		}
		else
		{
			M_MF()->UnlockDisplay(&mo_BigSplitter, NULL, FALSE);
			AfxGetApp()->DoWaitCursor(-1);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    To close current projects.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnProjectClose(void)
{
	/* Close project */
	if(b_CloseProject(TRUE) == FALSE) return;
}

/*
 =======================================================================================================================
    Aim:    Construct a menu with all editors.

    Note:   _i_Type determins for what we construct the menu. £
            _i_Type 0 Show/Hide editors £
            _i_Type 1 Link To £
            _i_Type 2 Fill with an editor type. £
            _i_Param is the ID.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::EditorMenu(CMenu *po_Menu, EDI_cl_BaseFrame *_po_Owner, int _i_Type, int _i_Of, int _i_Param)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j, i_Pos, i_ID;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	BOOL				b_Ok;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Add all editors that can be activated */
	i_Pos = 0;
	i_ID = WM_USER + _i_Of;
	b_Ok = FALSE;
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		for(j = 0; j < M_CurNumEd(po_View->mi_NumPane); j++)
		{
			po_Editor = M_CurEd(po_View->mi_NumPane, j).po_Editor;
			if(po_Editor && po_Editor->b_CanActivate())
			{
				switch(_i_Type)
				{
				/* Menu to construct list of visible/unvisible editors */
				case 0:
					break;

				/* Menu to construct list of editors to link to */
				case 1:
					if((_po_Owner == po_Editor) || (!po_Editor->b_CanBeLinked()))
					{
						i_ID++;
						continue;
					}
					break;

				/* Menu to show editors with a given type */
				case 2:
					if(po_Editor == _po_Owner)
					{
						i_ID++;
						continue;
					}

					if(!(po_Editor->mst_Def.i_Type & _i_Param))
					{
						i_ID++;
						continue;
					}
					break;
				}

				/* Insert a separator */
				if(b_Ok) po_Menu->InsertMenu(i_Pos++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCSTR) NULL);
				po_Menu->InsertMenu(i_Pos, MF_BYPOSITION, i_ID, po_Editor->mst_Def.asz_Name);
				b_Ok = FALSE;

				switch(_i_Type)
				{
				/* Menu to construct list of visible/unvisible editors */
				case 0:
					if(po_Editor->mst_BaseIni.b_IsVisible) po_Menu->CheckMenuItem(i_Pos, MF_CHECKED | MF_BYPOSITION);
					break;

				/* Menu to construct list of editors to link to */
				case 1:
					if(_po_Owner->mst_BaseIni.ab_LinkTo[po_Editor->mi_PosInGArray][po_Editor->mi_NumEdit])
						po_Menu->CheckMenuItem(i_Pos, MF_CHECKED | MF_BYPOSITION);
					break;
				}

				i_Pos++;
			}

			i_ID++;
		}

		if(po_Menu->GetMenuItemCount()) b_Ok = TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_MainFrame::po_GetEditorByMenu(int _i_ID, int _i_Of)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, mi_Num;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_Num = 0;
	_i_ID -= _i_Of;
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		if(_i_ID < M_CurNumEd(po_View->mi_NumPane))
		{
			po_Editor = M_CurEd(po_View->mi_NumPane, _i_ID).po_Editor;
			return po_Editor;
		}

		_i_ID -= M_CurNumEd(po_View->mi_NumPane);
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnHistory(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint				pt;
	EDI_cl_BaseFrame	*po_Ed;
	EDI_cl_BaseView		*po_View;
	EMEN_cl_SubMenu		o_Menu(FALSE);
	int					i_Pos, i;
	BIG_KEY				ul_Key;
	BIG_INDEX			ul_Sel, ul_File;
	char				asz_Path[BIG_C_MaxLenPath];
	UINT				ui_State;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&pt);
	po_Ed = po_EditorUnderPoint(pt, &po_View);
	if(po_Ed)
	{
		/* Create popup menu */
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "History", -1);
		i_Pos = 0;
		for(i = 0; i < EDI_C_MaxHistory; i++)
		{
			ul_Key = po_Ed->mst_BaseIni.aul_History[i];
			if(ul_Key != BIG_C_InvalidIndex)
			{
				ul_File = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_File != BIG_C_InvalidIndex)
				{
					/* Special file ? */
					ui_State = -1;
					if
					(
						BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtWorldList)
					||	BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtAIEditorModel)
					) ui_State = 0x80000000;
					if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtSoundBank)) ui_State = 0x40000000;
					if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtSModifier)) ui_State = 0x80000000;
					
					M_MF()->AddPopupMenuAction(NULL, &o_Menu, ul_File, TRUE, BIG_NameFile(ul_File), ui_State);
					i_Pos++;
				}
				else
				{
					/* A dir ? */
					ul_File = ul_Key;
					if(ul_File < BIG_MaxDir())
					{
						L_strcpy(asz_Path, "/");
						L_strcat(asz_Path, BIG_NameDir(ul_File));
						M_MF()->AddPopupMenuAction(NULL, &o_Menu, ul_File | 0x80000000, TRUE, asz_Path, -1);
						i_Pos++;
					}
				}
			}
		}

		/* Select file */
		if(i_Pos)
		{
			ul_Sel = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			if((int) ul_Sel != -1)
			{
				if(ul_Sel & 0x80000000)
				{
					if(!po_Ed->i_OnMessage(EDI_MESSAGE_CANSELDIR, ul_Sel & 0x7FFFFFFF, 0)) return;
					po_Ed->i_OnMessage(EDI_MESSAGE_SELDIR, ul_Sel & 0x7FFFFFFF, 0);
				}
				else
				{
					if(!po_Ed->i_OnMessage(EDI_MESSAGE_CANSELFILE, 0, ul_Sel)) return;
					po_Ed->i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_Sel);
				}
			}
		}

		if(ul_Sel == -1) return;

		/* Swap file with first */
		for(i = 0; i < EDI_C_MaxHistory; i++)
		{
			ul_Key = po_Ed->mst_BaseIni.aul_History[i];
			if(ul_Key != BIG_C_InvalidIndex)
			{
				ul_File = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_File == BIG_C_InvalidIndex) ul_File = ul_Key | 0x80000000;
				if(i && (ul_File == ul_Sel))
				{
					L_memmove(&po_Ed->mst_BaseIni.aul_History[1], &po_Ed->mst_BaseIni.aul_History[0], i * 4);
					po_Ed->mst_BaseIni.aul_History[0] = ul_Key;
					break;
				}
			}
		}

		/* Force focus */
		if(IsWindow(po_Ed->m_hWnd)) po_Ed->SetFocus();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::AddHistoryFile(EDI_cl_BaseFrame *_po_Ed, BIG_KEY _ul_Key)
{
	/*~~*/
	int i;
	BIG_KEY h;
	/*~~*/

	/* Search for the same key */
	for(i = 0; i < EDI_C_MaxHistory; i++)
	{
		if(_po_Ed->mst_BaseIni.aul_History[i] == _ul_Key) 
		{
			h = _po_Ed->mst_BaseIni.aul_History[0];
			_po_Ed->mst_BaseIni.aul_History[0] = _po_Ed->mst_BaseIni.aul_History[i];
			_po_Ed->mst_BaseIni.aul_History[i] = h;
			return;
		}
	}

	L_memmove(&_po_Ed->mst_BaseIni.aul_History[1], &_po_Ed->mst_BaseIni.aul_History[0], (EDI_C_MaxHistory - 2) * 4);
	_po_Ed->mst_BaseIni.aul_History[0] = _ul_Key;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::DrawTransparentBmp
(
	UINT		_ui_Res,
	CDC			*pDC,
	int			x,
	int			y,
	int			wd,
	int			hd,
	int			ws,
	int			hs,
	COLORREF	_x_Tranp
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	CBitmap bmp, *oldbmp;
	CDC		dc, bmpdc, newdc;
	CDC		maskDC;
	CBitmap maskBitmap;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	newdc.CreateCompatibleDC(pDC);
	bmp.LoadBitmap(MAKEINTRESOURCE(_ui_Res));
	oldbmp = newdc.SelectObject(&bmp);

	maskDC.CreateCompatibleDC(pDC);

	maskBitmap.CreateBitmap(32, 32, 1, 1, NULL);
	maskDC.SelectObject(&maskBitmap);
	newdc.SetBkColor(RGB(192, 192, 192));

	maskDC.BitBlt(0, 0, 32, 32, &newdc, 0, 0, SRCCOPY);

	newdc.SetBkColor(RGB(0, 0, 0));
	newdc.SetTextColor(RGB(255, 255, 255));
	newdc.BitBlt(0, 0, 32, 32, &maskDC, 0, 0, SRCAND);

	pDC->SetBkColor(RGB(255, 255, 255));
	pDC->SetTextColor(RGB(0, 0, 0));

	pDC->StretchBlt(x, y, wd, hd, &maskDC, 0, 0, ws, hs, SRCAND);
	pDC->StretchBlt(x, y, wd, hd, &newdc, 0, 0, ws, hs, SRCPAINT);

	newdc.SelectObject(oldbmp);
	DeleteObject(&bmp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDI_cl_MainFrame::u4_Interpol2PackedColor(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned long	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(fZClipLocalCoef >= 0.98f) return ulP2;
	if(fZClipLocalCoef <= 0.02f) return ulP1;
	*((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
	RetValue = (Interpoler & 128) ? (ulP2 & 0xfefefefe) >> 1 : (ulP1 & 0xfefefefe) >> 1;
	RetValue += (Interpoler & 64) ? (ulP2 & 0xfcfcfcfc) >> 2 : (ulP1 & 0xfcfcfcfc) >> 2;
	RetValue += (Interpoler & 32) ? (ulP2 & 0xf8f8f8f8) >> 3 : (ulP1 & 0xf8f8f8f8) >> 3;
	RetValue += (Interpoler & 16) ? (ulP2 & 0xf0f0f0f0) >> 4 : (ulP1 & 0xf0f0f0f0) >> 4;
	RetValue += (Interpoler & 8) ? (ulP2 & 0xe0e0e0e0) >> 5 : (ulP1 & 0xe0e0e0e0) >> 5;
	RetValue += (Interpoler & 4) ? (ulP2 & 0xc0c0c0c0) >> 6 : (ulP1 & 0xc0c0c0c0) >> 6;
	RetValue += (Interpoler & 2) ? (ulP2 & 0x80808080) >> 7 : (ulP1 & 0x80808080) >> 7;
	return RetValue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::FlashJade1(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC				*pdc;
	BITMAPINFO		st_BitmapInfo;
	long			*p;
	int				a;
	long			K, YYY;
	unsigned long	XX, YY, lastYY, YY2, Zoom1;
	long			*p_currentptrA, *p_currentptrB;
	long			DEX, DEX2;
	int				Zoom;
	long			*TABLEA;
	long			*TABLEB;
	long			*PICTURETABLE;
	ULONG			windowsizex, windowsizey;
	CDC				memdc;
	CBitmap			bmp, *old;
	int				first;
	void			*p_RawBitsDst;
	HBITMAP			mh_Bitmap;
	HBITMAP			mh_Bitmap1;
	int				i;
	CPoint			pt;
	int				mouseX, mouseY;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	windowsizex = 256;
	windowsizey = 256;

	TABLEA = (long *) malloc(windowsizex * windowsizey * 4);
	memset(TABLEA, 0, windowsizex * windowsizey * 4);
	TABLEB = (long *) malloc(windowsizex * windowsizey * 4);
	memset(TABLEB, 0, windowsizex * windowsizey * 4);

	GetCursorPos(&pt);
	ScreenToClient(&pt);

	pdc = GetWindowDC();
	memdc.CreateCompatibleDC(pdc);
	bmp.CreateCompatibleBitmap(pdc, windowsizex, windowsizey);
	old = memdc.SelectObject(&bmp);
	memdc.BitBlt(0, 0, windowsizex, windowsizey, pdc, pt.x - (windowsizex / 2), pt.y - (windowsizey / 2), SRCCOPY);
	first = 0;

	st_BitmapInfo.bmiHeader.biWidth = windowsizex;
	st_BitmapInfo.bmiHeader.biHeight = windowsizey;
	st_BitmapInfo.bmiHeader.biPlanes = 1;
	st_BitmapInfo.bmiHeader.biBitCount = 32;
	st_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	st_BitmapInfo.bmiHeader.biCompression = BI_RGB;
	st_BitmapInfo.bmiHeader.biSizeImage = 0;
	st_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	st_BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	st_BitmapInfo.bmiHeader.biClrUsed = 0;
	st_BitmapInfo.bmiHeader.biClrImportant = 0;
	mh_Bitmap = CreateDIBSection(pdc->m_hDC, &st_BitmapInfo, DIB_RGB_COLORS, (void **) &p_RawBitsDst, NULL, 0);
	mh_Bitmap1 = CreateDIBSection(pdc->m_hDC, &st_BitmapInfo, DIB_RGB_COLORS, (void **) &PICTURETABLE, NULL, 0);

	p = (long *) malloc(windowsizex * windowsizey * 4);
	a = GetDIBits(memdc.m_hDC, (HBITMAP) bmp, 0, windowsizey, p, &st_BitmapInfo, DIB_RGB_COLORS);

	L_memcpy(PICTURETABLE, p, windowsizex * windowsizey * 4);
	free(p);

	for(i = 0; i < 50; i++)
	{
		if(i == 0 || i == 25)
		{
			mouseX = windowsizex / 2;
			mouseY = windowsizey / 2 - windowsizey / 4;
		}
		else
		{
			mouseX = mouseY = 0;
		}

		Zoom = 1;

		first++;
		if(first & 1)
		{
			p_currentptrA = TABLEA;
			p_currentptrB = TABLEB;
		}
		else
		{
			p_currentptrA = TABLEB;
			p_currentptrB = TABLEA;
		}

		if(mouseX && mouseY)
		{
			mouseX >>= Zoom;
			mouseY >>= Zoom;
			for(K = 0; K < 4; K++)
			{
				for(YYY = 0; YYY < 4; YYY++)
				{
/*$off*/
#pragma warning(disable : 4554)
					*(p_currentptrA + ((mouseX + YYY) % (windowsizex>> Zoom )) + (((mouseY + K) % (windowsizey>> Zoom )) * (windowsizex>> Zoom ))) += 0x4FFFF;
					*(p_currentptrA + ((mouseX + YYY + 5) % (windowsizex>> Zoom )) + (((mouseY + K) % (windowsizey>> Zoom )) * (windowsizex>> Zoom ))) -= 0x4FFFF;
/*$on*/
				}
			}
		}

		lastYY = ((windowsizey >> Zoom) - 1) * (windowsizex >> Zoom);
		for(YY = windowsizex >> Zoom; YY < lastYY; YY += windowsizex >> Zoom)
		{
			for(XX = 1; XX < (windowsizex >> Zoom) - 1; XX++)
			{
/*$off*/
				*(p_currentptrA + XX + YY) = (*(p_currentptrB + XX - 1 + YY) + *(p_currentptrB + XX + 1 + YY) + *(p_currentptrB + XX + YY + (windowsizex>>Zoom)) + *(p_currentptrB + XX + YY - (windowsizex>>Zoom)) >> 1) - (*(p_currentptrA + XX + YY));
				*(p_currentptrA + XX + YY) -= *(p_currentptrA + XX + YY) >> 8;
				*(p_currentptrB + XX + YY) -= *(p_currentptrB + XX + YY) >> 8;
/*$on*/
			}
		}

		K = 8;
		lastYY = (windowsizey) * windowsizex;
		Zoom1 = Zoom;
		for(YY2 = 0, YY = 0; YY < lastYY; YY += windowsizex)
		{
			for(XX = 0; XX < windowsizex; XX++)
			{
				DEX2 = DEX = *(p_currentptrB + (XX >> Zoom) + YY2) >> 4;
				if(DEX2 < 0) DEX2 = 0;
				DEX2 >>= 5;
				if(DEX2 > 0x800)
					*(((unsigned long *) p_RawBitsDst) + XX + YY) = 0x00FFFFFF;
				else
					*(((unsigned long *) p_RawBitsDst) + XX + YY) = *(PICTURETABLE + XX + (DEX >> 11) + YY);
			}

			if(!Zoom1) YY2 += windowsizex >> Zoom;
			Zoom1++;
			if(Zoom1 > (ULONG) (1 << Zoom)) Zoom1 = 0;
		}

		memdc.SelectObject(mh_Bitmap);
		pdc->BitBlt(pt.x - windowsizex / 2, pt.y - windowsizey / 2, windowsizex, windowsizey, &memdc, 0, 0, SRCCOPY);
	}

	free(TABLEA);
	free(TABLEB);
	memdc.SelectObject(old);
	DeleteObject(bmp);
	DeleteObject(mh_Bitmap);
	DeleteObject(mh_Bitmap1);
	ReleaseDC(pdc);
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_FRAME | RDW_ALLCHILDREN);
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::FlashJade(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	ast_Pos[1000];
	MATH_tdst_Vector	ast_Vit[1000];
	MATH_tdst_Vector	st_Vec;
	float				fNorm;
	int					i, k, imax;
	CPoint				pt;
	CDC					*pdc;
	int					delay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	imax = 1;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	pt.y += 16;
	L_memset(ast_Pos, 0, sizeof(MATH_tdst_Vector));
	L_memset(ast_Vit, 0, sizeof(MATH_tdst_Vector));

	imax = 0;
	delay = 0;

	pdc = GetWindowDC();

	for(k = 0; k < 40; k++)
	{
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if(++delay == 2)
		{
			for(i = 0; i < 10; i++)
			{
                ast_Vit[imax + i].x = fRand(-3.0f, 3.0f); 
                ast_Vit[imax + i].y = fRand(-3.0f, 3.0f);                
                ast_Pos[imax + i].x = (float) pt.x;
				ast_Pos[imax + i].y = (float) pt.y;
			}

			imax += i;
			delay = 0;
		}

		/* Speed */
		for(i = 0; i < imax; i++)
		{
			if(MATH_f_NormVector(&ast_Vit[i]) == 0.0f) continue;
            MATH_NormalizeVector(&st_Vec, &ast_Vit[i]);
			fNorm = MATH_f_NormVector(&ast_Vit[i]);
			MATH_ScaleVector(&ast_Vit[i], &st_Vec, fNorm * fRand(0.98f, 0.999f));
			if(i & 1)
			{
				ast_Vit[i].y = 2 * fSin((float) k);
			}
			else
			{
				ast_Vit[i].x = 2 * fCos((float) k);
			}

			MATH_AddVector(&ast_Pos[i], &ast_Pos[i], &ast_Vit[i]);
		}

		/* Draw */
		for(i = 0; i < imax; i++)
		{
			pdc->SetPixel((int) ast_Pos[i].x - 1, (int) ast_Pos[i].y - 1, 0x00EEEEEE);
			pdc->SetPixel((int) ast_Pos[i].x, (int) ast_Pos[i].y, 0x0000E0FF);
			pdc->SetPixel((int) ast_Pos[i].x + 1, (int) ast_Pos[i].y + 1, 0x00000090);
		}
	}

	ReleaseDC(pdc);
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_FRAME | RDW_ALLCHILDREN);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_MainFrame::i_GetIconImage(char *_psz_Path, char *_psz_Name, int _i_Size)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char		*psz_Temp;
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~~*/

	/* File */
	if(_psz_Name)
	{
		psz_Temp = L_strrchr(_psz_Name, '.');
		if(!psz_Temp) return EDI_IMAGE_BASEFILE;

		/*$off*/
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineInstance))	return EDI_IMAGE_AIINSTANCE;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtCOLInstance))		return EDI_IMAGE_AIINSTANCE;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorModel))		return EDI_IMAGE_AIMODEL;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineModel))		return EDI_IMAGE_AIMODEL;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtCOLSetModel))		return EDI_IMAGE_AIMODEL;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))		return EDI_IMAGE_AIFCT;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineFct))		return EDI_IMAGE_AIFCT;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorVars))		return EDI_IMAGE_AIVAR;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineVars))		return EDI_IMAGE_AIVAR;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtGraphicObject))		return EDI_IMAGE_GRO;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtCOLObject))			return EDI_IMAGE_GRO;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtGraphicMaterial))	return EDI_IMAGE_GRM;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtCOLGMAT))			return EDI_IMAGE_GRM;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtTexture1))			return EDI_IMAGE_TGA;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtTexture2))			return EDI_IMAGE_TGA;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtTexture3))			return EDI_IMAGE_TGA;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtGameObject))			return EDI_IMAGE_GAO;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtWorld))				return EDI_IMAGE_WORLD;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtObjModels))			return EDI_IMAGE_OBJMODEL;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAnimation))			return EDI_IMAGE_ANIM;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtEventAllsTracks))	return EDI_IMAGE_ANIM;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtGraphicLight))		return EDI_IMAGE_LIGHT;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtShape))				return EDI_IMAGE_SHAPE;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtObjGroups))			return EDI_IMAGE_GRP;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtWorldList))			return EDI_IMAGE_WOL;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundFile))			return EDI_IMAGE_SND;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtLoadingSound))		return EDI_IMAGE_LOADINGSND;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundMusic))			return EDI_IMAGE_MUSIC;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundAmbience))		return EDI_IMAGE_AMBIENCE;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundDialog))		return EDI_IMAGE_DIALOG;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundBank))		    return EDI_IMAGE_SNDBANK;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundMetaBank))	    return EDI_IMAGE_SNDMETABANK;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSModifier))	        return EDI_IMAGE_SMODIFIER;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundInsert))		return EDI_IMAGE_SNDINSERT;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtSoundFade))			return EDI_IMAGE_SNDFADE;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtActionKit))	        return EDI_IMAGE_ACK;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAction))				return EDI_IMAGE_ACT;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtVideo1))				return EDI_IMAGE_IPU;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtVideo3))				return EDI_IMAGE_MTX;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib))		return EDI_IMAGE_FCTLIB;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineFctLib))		return EDI_IMAGE_FCTLIB;
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtPrefab))				return EDI_IMAGE_PREFAB;
		/*$on*/
		return EDI_IMAGE_BASEFILE;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Folder
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Trachcan */
	psz_Temp = _psz_Path;
	if(!L_strncmp(_psz_Path, EDI_Csz_Path_TrashCan, L_strlen(EDI_Csz_Path_TrashCan)))
	{
		psz_Temp += L_strlen(EDI_Csz_Path_TrashCan);
		if(!(*psz_Temp)) return EDI_IMAGE_TRASHCAN;
	}

	/* World */
	psz_Temp = _psz_Path;
	if(!L_strncmp(_psz_Path, EDI_Csz_Path_Levels, L_strlen(EDI_Csz_Path_Levels)))
	{
		psz_Temp += L_strlen(EDI_Csz_Path_Levels);
		if(!(*psz_Temp)) return EDI_IMAGE_FOLDERWORLD;
	}

	/* AI global lib */
	psz_Temp = _psz_Path;
	if(!L_strncmp(_psz_Path, EDI_Csz_Path_AILib, L_strlen(EDI_Csz_Path_AILib)))
	{
		psz_Temp += L_strlen(EDI_Csz_Path_AILib);
		if(!(*psz_Temp)) return EDI_IMAGE_GLOBALLIB;
	}

	/* AI model */
	psz_Temp = _psz_Path;
	if(!L_strncmp(_psz_Path, EDI_Csz_Path_AIModels, L_strlen(EDI_Csz_Path_AIModels)))
	{
		psz_Temp += L_strlen(EDI_Csz_Path_AIModels);
		if(*psz_Temp)
		{
			ul_Index = BIG_ul_SearchDir(_psz_Path);
			if(ul_Index == BIG_C_InvalidIndex || BIG_SubDir(ul_Index) == BIG_C_InvalidIndex)
				return EDI_IMAGE_FOLDERAIMODEL;
		}
		else
			return EDI_IMAGE_FOLDERAIMODEL;
	}

	/* AI instance */
	psz_Temp = _psz_Path;
	if(!L_strncmp(_psz_Path, EDI_Csz_Path_Levels, L_strlen(EDI_Csz_Path_Levels)))
	{
		psz_Temp += L_strlen(EDI_Csz_Path_Levels);
		if(*psz_Temp)
		{
			psz_Temp++;
			psz_Temp = L_strrchr(psz_Temp, '/');
			if(psz_Temp)
			{
				psz_Temp++;
				if(!L_strncmp(psz_Temp, EDI_Csz_Path_AIInstances, L_strlen(EDI_Csz_Path_AIInstances)))
				{
					psz_Temp += L_strlen(EDI_Csz_Path_AIInstances);
					if(*psz_Temp)
					{
						psz_Temp++;
						if(!L_strchr(psz_Temp, '/')) return EDI_IMAGE_FOLDERAIINSTANCE;
					}
					else
						return EDI_IMAGE_FOLDERAIINSTANCE;
				}
			}
		}
	}

	return EDI_IMAGE_FOLDER1;
}

extern ULONG	ulRealIdleCounter;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnActivateApp(BOOL bActive, LOCAL_HTASK hTask)
{
	/* Force tips to close */
	if(!bActive && EDI_go_TheApp.EDI_gpo_EnterWnd2)
	{
		EDI_go_TheApp.EDI_gpo_EnterWnd2->DisplayMessage("", 1);
		EDI_go_TheApp.EDI_gpo_EnterWnd2->SetWindowPos(NULL, 1, 1, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME);
		ulRealIdleCounter = timeGetTime();
	}

	CFrameWnd::OnActivateApp(bActive, hTask);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::Joystick_Calibrate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_Joystick	o_Dialog;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Dialog.DoModal();
	{
		/*~~~~~~~~~~~~~*/
		FILE	*hp_File;
		/*~~~~~~~~~~~~~*/

		hp_File = fopen("joy.ini", "wt");
		if(hp_File)
		{
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_YDownStart);
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_XRightStart);
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_YUpStart);
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_XLeftStart);
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_YDown);
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_XRight);
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_YUp);
			fprintf(hp_File, "%d\n", win32INO_l_Joystick_XLeft);
		}
	}

	return;
}

#ifdef JADEFUSION
extern void MEMpro_PrintMemRaster(void);
#else
extern "C" void MEMpro_PrintMemRaster(void);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::RefStats(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j;
	ULONG				first, last;
	char				az[1000];
	char				az1[10000];
	BIG_INDEX			ul_Fat;
	BIG_KEY				ul_Key;
	BAS_tdst_barray		*p;
	EDIA_cl_NameDialog	o_Dialog1("Enter First Key");
	EDIA_cl_NameDialog	o_Dialog2("Enter Last Key");
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(az, "%X", 0);
	o_Dialog1.mo_Name = az;
	if(o_Dialog1.DoModal() != IDOK) return;
	sscanf(o_Dialog1.mo_Name, "%x", &first);
	sprintf(az, "%X", -1);
	o_Dialog2.mo_Name = az;
	if(o_Dialog2.DoModal() != IDOK) return;
	sscanf(o_Dialog2.mo_Name, "%x", &last);

	for(i = 0; i < LOA_gst_RefArray.num; i++)
	{
		ul_Key = LOA_gst_RefArray.base[i].ul_Key;
		if(ul_Key < first) continue;
		if(ul_Key > last) break;

		ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
		sprintf(az, "%s (%x)", BIG_NameFile(ul_Fat), ul_Key);
		j = strlen(az);
		while(j < 50)
		{
			strcat(az, " ");
			j++;
		}

		p = (BAS_tdst_barray *) LOA_gst_RefArray.base[i].ul_Val;
		for(j = 0; j < p->num; j++)
		{
			ul_Key = p->base[j].ul_Key;
			ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
			sprintf(az1, "%s (%x) - ", BIG_NameFile(ul_Fat), ul_Key);
			strcat(az, az1);
		}

		if(p->num) LINK_PrintStatusMsg(az);
		if(GetAsyncKeyState(VK_ESCAPE) < 0) break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::UnrefStats(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j;
	ULONG				first, last;
	char				az[1000];
	char				az1[10000];
	BIG_INDEX			ul_Fat;
	BIG_KEY				ul_Key;
	BAS_tdst_barray		*p;
	int					ok;
	EDIA_cl_NameDialog	o_Dialog1("Enter First Key");
	EDIA_cl_NameDialog	o_Dialog2("Enter Last Key");
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(az, "%X", 0);
	o_Dialog1.mo_Name = az;
	if(o_Dialog1.DoModal() != IDOK) return;
	sscanf(o_Dialog1.mo_Name, "%x", &first);
	sprintf(az, "%X", -1);
	o_Dialog2.mo_Name = az;
	if(o_Dialog2.DoModal() != IDOK) return;
	sscanf(o_Dialog2.mo_Name, "%x", &last);

	for(i = 0; i < LOA_gst_RefArray.num; i++)
	{
		ul_Key = LOA_gst_RefArray.base[i].ul_Key;
		if(ul_Key < first) continue;
		if(ul_Key > last) break;

		ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
		sprintf(az, "%s (%x)", BIG_NameFile(ul_Fat), ul_Key);
		j = strlen(az);
		while(j < 50)
		{
			strcat(az, " ");
			j++;
		}

		ok = 0;
		for(j = 0; j < LOA_gst_RefArray.num; j++)
		{
			p = (BAS_tdst_barray *) LOA_gst_RefArray.base[j].ul_Val;
			if(BAS_bsearch(ul_Key, p) != BIG_C_InvalidIndex)
			{
				ok = 1;
				ul_Key = LOA_gst_RefArray.base[j].ul_Key;
				ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
				sprintf(az1, "%s (%x) - ", BIG_NameFile(ul_Fat), ul_Key);
				strcat(az, az1);
			}
		}

		if(ok) LINK_PrintStatusMsg(az);
		if(GetAsyncKeyState(VK_ESCAPE) < 0) break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::MemoryStats(void)
{
	MEMpro_PrintMemRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern void TEXT_ChangeLang(int);
#else
extern "C" void TEXT_ChangeLang(int);
#endif
void EDI_cl_MainFrame::OnSetLanguage(void)
{
	EDIA_cl_Lang *po_Dialog;

	po_Dialog= new EDIA_cl_Lang;
	
	if(IDOK==po_Dialog->DoModal())
	{
		TEXT_ChangeLang(po_Dialog->mi_Lang);
	}
	delete po_Dialog;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::TextureStats(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i, j;
	BIG_INDEX	ul_File;
	ULONG		ul_Size;
	ULONG		ul_Size1;
	char		az_Msg[1024];
	int			ai_Treated[1024];
	int			i_numtreated;
	ULONG		min;
	int			minind;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("Loaded textures");
	LINK_PrintStatusMsg("---------------");
	ul_Size = 0;
	i_numtreated = 0;

	while(1)
	{
		min = 0xFFFFFFFF;
		for(i = 0; i < TEX_gst_GlobalList.l_NumberOfTextures; i++)
		{
			for(j = 0; j < i_numtreated; j++)
			{
				if(i == ai_Treated[j]) goto zap;
			}

			ul_File = BIG_ul_SearchKeyToFat(TEX_gst_GlobalList.dst_Texture[i].ul_Key);
			if(ul_File != BIG_C_InvalidIndex)
			{
				ul_Size1 = BIG_ul_GetLengthFile(BIG_PosFile(ul_File));
				if(ul_Size1 < min)
				{
					min = ul_Size1;
					minind = i;
				}
			}

zap: ;
		}

		if(min == 0xFFFFFFFF) break;
		ai_Treated[i_numtreated++] = minind;
		ul_Size += min;
		sprintf
		(
			az_Msg,
			"%s (%d kb)",
			BIG_NameFile(BIG_ul_SearchKeyToFat(TEX_gst_GlobalList.dst_Texture[minind].ul_Key)),
			min / 1024
		);
		LINK_PrintStatusMsg(az_Msg);
	}

	LINK_PrintStatusMsg("----------------");
	sprintf(az_Msg, "Total = %d kb", ul_Size / 1024);
	LINK_PrintStatusMsg(az_Msg);
}

#endif /* ACTIVE_EDITORS */
