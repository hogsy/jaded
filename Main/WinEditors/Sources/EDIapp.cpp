/*$T EDIapp.cpp GC 1.134 04/30/04 09:59:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include <string>
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGcheck.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "EDIapp.h"
#include "EDImainframe.h"
#include "EDIstrings.h"
#include "EDIerrid.h"
#include "EDIeditors_infos.h"
#include "Res/Res.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "INOut/INOkeyboard.h"
#include "EDItors/Sources/EVEnts/EVEframe_act.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDItors/Sources/MENu/MENin.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDImainframe_act.h"
#include "F3DFrame/F3Dview.h"
#include "LINKs/LINKtoed.h"
#include "DIAlogs/DIAchecklist_dlg.h"
#include "DIAlogs/DIA_SmoothSel_dlg.h"
#include "EDIpaths.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMLog.h"
#include "EDIapp_options.h"
#include "BIGfiles/VERsion/VERsion_number.h"
#include "SouND/sources/SND.h"
#include "BASe/MEMory/MEMpro.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "INOut/INO.h"
#include "DATaControl/DATCUtils.h"
#include "DATaControl/DATCPerforce.h"

#if defined(_XENON_RENDER)
#include "GraphicDK/Sources/TEXture/TEXfile.h"
#include "GraphicDK/Sources/TEXture/TEXxenon.h"
#endif

#ifdef WIN32
#include <afxsock.h>		// MFC socket extensions
#endif // #ifdef WIN32

// to satisfy p4 crap - should remove p4 ~hogsy
#undef _tzname
extern "C" { char* _tzname[2]; }

/*$4
 ***********************************************************************************************************************
    OPTIONS
 ***********************************************************************************************************************
 */

#define EDI_MSG_MaxLines	20
#define EDI_MSG_MaxLenLine	200
BOOL				EDI_gb_AutoHideTip = TRUE;
char				mo_Msg[EDI_MSG_MaxLines][EDI_MSG_MaxLenLine];	/* 20 lines maximum */
ULONG				ulNumberOfLines;
ULONG				ulRealIdleCounter = 0;
POINT				stOldCurPos;
BOOL				EDI_gb_SlashQ = FALSE;
#ifdef JADEFUSION
extern BOOL		EDI_gb_SlashC ;

BOOL                EDI_gb_SlashLXMP = FALSE;
BOOL                EDI_gb_SlashLXPK = FALSE;
BOOL                EDI_gb_SlashXPK = FALSE;
BOOL                EDI_gb_SlashXTX = FALSE;
BOOL                EDI_gb_XeQuickLoad = FALSE;
extern char			EDI_gaz_SlashL[1024];


#else
extern "C" BOOL		EDI_gb_SlashC ;
#endif
char				EDI_gaz_SlashC[512];
BOOL				EDI_gb_SlashL = FALSE;
BOOL                EDI_gb_SlashLKTM = FALSE;
BOOL				EDI_gb_SlashLoadAndQuit = FALSE;
#ifdef JADEFUSION
extern char		EDI_gaz_SlashL[1024];
#else
extern "C" char		EDI_gaz_SlashL[1024];
#endif
char				EDI_gaz_SlashY[512];
BOOL				EDI_gb_SX = FALSE;
BOOL				EDI_gb_CanBin = FALSE;
#ifdef JADEFUSION
extern BOOL		EDI_gb_ComputeMap;
#else
extern "C" BOOL		EDI_gb_ComputeMap;
#endif
BOOL				EDI_gb_CleanBinFiles = FALSE;
BOOL				EDI_gb_CleanFinal;
BOOL				EDI_gb_Clean = FALSE;
BOOL				EDI_gb_CleanByFat = FALSE;
BOOL				EDI_gb_EdiImport = FALSE;
BOOL				EDI_gb_EdiMerge = FALSE;
char				EDI_gaz_EdiMergeDir[L_MAX_PATH];
char				EDI_gaz_EdiBigfileSource[L_MAX_PATH];
char				EDI_gaz_EdiImportSource[L_MAX_PATH];
char				EDI_gaz_EdiImportDestination[L_MAX_PATH];
BOOL				EDI_gb_MakeMapList = FALSE;
char				EDI_gaz_EdiMapList[L_MAX_PATH];
extern BOOL			ERR_gb_Log;
#ifdef JADEFUSION
extern int		EDI_gi_GenSpe;
extern int		WOR_gi_CurrentConsole;
extern void		BIG_MakeWolList(char *_asz_Path);
extern ULONG	AI2C_gul_MinInstanceNbForGeneration;
#else
extern "C" int		EDI_gi_GenSpe;
extern "C" int		WOR_gi_CurrentConsole;
extern "C" void		BIG_MakeWolList(char *_asz_Path);
extern "C" ULONG	AI2C_gul_MinInstanceNbForGeneration;
#endif

char				EDI_gpz_SlashPbuffer[1024] ;
char                EDI_az_LogFileName[L_MAX_PATH];
char				*EDI_gpz_SlashP = NULL;
char				*EDI_gpz_SlashPG = NULL;
#ifdef JADEFUSION
extern char		LOA_gaz_EdiExFileKey[L_MAX_PATH];
extern BOOL		LOA_gb_EdiExFileKey;
#else
extern "C" char		LOA_gaz_EdiExFileKey[L_MAX_PATH];
extern "C" BOOL		LOA_gb_EdiExFileKey;
#endif
BOOL				EDI_gb_SaveSoundReport = FALSE;
BOOL				EDI_gb_SoundCheck = FALSE;
HANDLE				EDI_gh_ErrorFile;
BOOL				EDI_gb_Check = FALSE;
ULONG				EDI_gul_EdiBigfileSyncChangelist = 0;
ULONG				EDI_gul_EdiBigfileSyncChangelistClean = 0;
#ifdef JADEFUSION
BOOL				EDI_gb_EdiBigfileRefreshServerInfo = FALSE;
CHAR                EDI_gsz_StartupPath[L_MAX_PATH];
BOOL                EDI_gb_XeQuiet = FALSE;
#endif
ULONG				EDI_gul_EdiBigfileSyncForce = 0;
ULONG               EDI_gul_EdiBigfileFlushChangelist = 0;
ULONG				EDI_gul_EdiLock = 0;
BOOL				EDI_gb_BatchModeWithoutPerforce = FALSE;
extern "C" BOOL		AI_gb_Optim2;
extern "C" BOOL		AI_gb_GenOptim2;
#ifdef JADEFUSION
extern BOOL		EDI_gb_MorphLog = FALSE;
BOOL                EDI_gb_XeWipeTGAs = FALSE;
BOOL                EDI_gb_XeWipeDDSs = FALSE;
#else
extern "C" BOOL		EDI_gb_MorphLog = FALSE;
#endif

#if MONTREAL_SPECIFIC
BOOL                EDI_gb_DontCheckVersion = FALSE;
BOOL				EDI_gb_DontCheckKeyServer = FALSE;
#endif

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

EDI_cl_App			EDI_go_TheApp;					/* Main application */
BOOL				EDI_gb_LockKeyboard = FALSE;	/* To lock keyboard hook */
BOOL				EDI_gb_LockMouse = FALSE;		/* To lock mouse hook */
BOOL				EDI_gb_LockHook = FALSE;		/* To lock all hooks */
BOOL				EDI_gb_TopMenuMode = FALSE;		/* Top menu selection mode */
HWND				EDI_gh_TopMenuMode = 0;			/* Top menu selection mode (menu) */
BOOL				EDI_gb_DuringInit = FALSE;		/* We are during init of application */
#ifdef JADEFUSION
extern BOOL		EDI_gb_NoVerbose;
#else
extern "C" BOOL		EDI_gb_NoVerbose;
#endif
LRESULT CALLBACK	x_KeyboardHook(int, WPARAM, LPARAM);
LRESULT CALLBACK	x_MouseHook(int, WPARAM, LPARAM);
CList<HWND, HWND>	APP_go_Modeless;
CList<HWND, HWND>	APP_go_ModelessDia;
CList<HWND, HWND>	APP_go_ModelessNoRef;
MSG					*gpParam = NULL;
int					giParam = 0;
BOOL				gbSimuEnd = TRUE;
BOOL EDI_gb_Automated    = FALSE;
BOOL EDI_gb_ExportToKeys = FALSE;
BOOL EDI_gb_ImportFromKeys = FALSE;
std::string EDI_gb_ExportToKeys_Path;
std::string EDI_gb_ImportFromKeys_Path;

#ifdef JADEFUSION
extern void BIG_CleanByFat();
#else
extern "C" void BIG_CleanByFat();
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if _MSC_VER == 1200
#define GE_MFC_MSG	m_msgCur
#else
#define GE_MFC_MSG	pState->m_msgCur
#endif
BEGIN_MESSAGE_MAP(EDI_cl_EnterWnd, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_EnterWnd::DisplayMessage(char *_psz_Msg)
{
	/*~~~~~~~~~~~~~~~~~*/
	CDC		*pdc;
	CRect	o_Rect;
	char	asz_Msg[100];
	/*~~~~~~~~~~~~~~~~~*/

	mo_Msg = _psz_Msg;
	pdc = GetWindowDC();

	GetClientRect(&o_Rect);
	o_Rect.left = 10;
	o_Rect.top = o_Rect.bottom - 17;
	o_Rect.right -= 10;
	pdc->SelectObject(&M_MF()->mo_Fnt);
	pdc->SetTextColor(0x00FFFFFF);
	pdc->SetBkColor(0x00000000);
	pdc->ExtTextOut(o_Rect.left, o_Rect.top, ETO_CLIPPED | ETO_OPAQUE, o_Rect, _psz_Msg, L_strlen(_psz_Msg), NULL);

	/* App version */
	GetClientRect(&o_Rect);
	pdc->SetBkMode(TRANSPARENT);
#if MONTREAL_SPECIFIC
	// For the builds in Montreal, display the Montreal build number as well as the Montpellier build number.
	sprintf(asz_Msg, "Version %03d(%03d)-%03d", BIG_Cu4_MontrealAppVersion, BIG_Cu4_AppVersion, BIG_Cu4_Version);
#else
#ifdef JADEFUSION
	// For the builds 360, display the 360 build number as well as the Montpellier build number.
	sprintf(asz_Msg, "Version Xe-%03d(%03d,%03d)-%03d", BIG_Cu4_MontrealXeAppVersion, BIG_Cu4_MontrealAppVersion, BIG_Cu4_AppVersion, BIG_Cu4_Version);
#else
	sprintf(asz_Msg, "Version %03d-%03d", BIG_Cu4_AppVersion, BIG_Cu4_Version);
#endif
#endif
	pdc->SetTextColor(0x00FFFFFF);
	pdc->ExtTextOut(o_Rect.left + 10, o_Rect.top, 0, NULL, asz_Msg, L_strlen(asz_Msg), NULL);

	ReleaseDC(pdc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_EnterWnd::PreCreateWindow(CREATESTRUCT &cs)
{
	CFrameWnd::PreCreateWindow(cs);
	cs.dwExStyle = 0;
	return TRUE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_EnterWnd::Paint(void)
{
	/*~~~~~*/
	CDC *pdc;
	/*~~~~~*/

	memdc.SelectObject(bmp);
	pdc = GetDC();
	pdc->BitBlt(0, 0, windowsizex, windowsizey, &memdc, 0, 0, SRCCOPY);
	ReleaseDC(pdc);
}

#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_InitialP4RefreshServerInfo()
{
    ULONG ulBFSize = BIG_ul_GetBFSize();
    ULONG ulRegBFSize = BIG_ul_GetRegistryBfSize();

    if (!EDI_gb_BatchModeWithoutPerforce && 
        ulBFSize != ulRegBFSize && 
        DAT_CPerforce::GetInstance()->P4Connect(FALSE)) 
	{
	    DAT_CPerforce::GetInstance()->P4FlushBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
		DAT_CPerforce::GetInstance()->P4Disconnect();
        BIG_SetRegistryBfSize();
    }
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SetTransparentWnd(CWnd *po_Wnd, int ftrans)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes) (HWND hWnd, COLORREF cr, BYTE bAlpha, DWORD dwFlags);
	lpfnSetLayeredWindowAttributes	g_pSetLayeredWindowAttributes;
	HMODULE							hUser32 = GetModuleHandle(_T("USER32.DLL"));
#define WS_EX_LAYERED	0x00080000
//#define LWA_COLORKEY	1	/* Use color as the transparency color. */
//#define LWA_ALPHA		2	/* Use bAlpha to determine the opacity of the layer */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	g_pSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes) GetProcAddress
		(
			hUser32,
			"SetLayeredWindowAttributes"
		);
	if(g_pSetLayeredWindowAttributes)
	{
		if(ftrans < 255)
		{
			SetWindowLong(po_Wnd->m_hWnd, GWL_EXSTYLE, GetWindowLong(po_Wnd->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			g_pSetLayeredWindowAttributes(po_Wnd->m_hWnd, 0, (BYTE) ftrans, LWA_ALPHA);
		}
		else
		{
			SetWindowLong(po_Wnd->m_hWnd, GWL_EXSTYLE, GetWindowLong(po_Wnd->m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_EnterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{ 
	/*~~~~~~~~~*/
	CRect	r;
	CDC		*pdc;
	/*~~~~~~~~~*/

	CFrameWnd::OnCreate(lpCreateStruct);

	GetClientRect(&r);
	windowsizex = r.right;
	windowsizey = r.bottom;
	bmp.LoadBitmap(MAKEINTRESOURCE(BMP_EDI_INTRO));
	pdc = GetDC();
	memdc.CreateCompatibleDC(pdc);
	old = memdc.SelectObject(&bmp);
	ReleaseDC(pdc);
	ShowWindow(SW_SHOW);
	Paint();
	return 0;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_EnterWnd::OnEraseBkgnd(CDC *pDC)
{
	Paint();
	DisplayMessage((char *) (LPCSTR) mo_Msg);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_EnterWnd::OnDestroy(void)
{
	memdc.SelectObject(old);
	DeleteObject(bmp);
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDI_cl_ToolTipWnd, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_Tooltip_DisplayMessage(char *p_str, ULONG ulSpeed = 200)
{
	if(EDI_go_TheApp.EDI_gpo_EnterWnd2) EDI_go_TheApp.EDI_gpo_EnterWnd2->DisplayMessage(p_str, ulSpeed);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_ToolTipWnd::DisplayMessage(char *_psz_Msg, ULONG ulSpeedEx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*psz_Dec1, *psz_Dec2;
	int		i_Len;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulNumberOfLines = 0;
	mo_Msg[0][0] = 0;
	if(!_psz_Msg) return;
	ulSpeed = ulSpeedEx;
	psz_Dec1 = _psz_Msg;
	psz_Dec2 = &mo_Msg[0][0];
	*psz_Dec2 = 0;
	i_Len = 0;

	/* Count number of '10' == number of lines */
	while(*psz_Dec1 != 0)
	{
		*psz_Dec2 = *psz_Dec1;
		if(*psz_Dec2 == 13) *psz_Dec2 = 0;
		if(*psz_Dec2 == 10)
		{
			*psz_Dec2 = 0;
			if(ulNumberOfLines == EDI_MSG_MaxLines - 1) break;
			psz_Dec2 = &mo_Msg[++ulNumberOfLines][0];
			i_Len = 0;
		}
		else
			psz_Dec2++;

		i_Len++;
		if(i_Len == EDI_MSG_MaxLenLine - 1)
		{
			*psz_Dec2 = 0;
			break;
		}

		psz_Dec1++;
	}

	*psz_Dec2 = 0;
	ulNumberOfLines++;

	/* Force refresh is 1 in delay */
	if(ulSpeedEx == 1) ulRealIdleCounter = timeGetTime();
	if(ulSpeedEx == 0) Display();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_ToolTipWnd::Display(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		*pdc;
	CRect	o_Rect, o_BigRect;
	CBrush	brh;
	CSize	TxtLenMax;
	CSize	TxtLen;
	POINT	pt;
	ULONG	ulLineCounter;
	ULONG	ulRealLineNumber;
	CWnd	*po_Focus;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(mo_Msg[0][0] == 0) return;

	pdc = GetWindowDC();
	GetCursorPos(&pt);
	pdc->SelectObject(&M_MF()->mo_Fnt);
	TxtLen.cx = TxtLen.cy = 0;
	ulRealLineNumber = 0;
	for(ulLineCounter = 0; ulLineCounter < ulNumberOfLines; ulLineCounter++)
	{
		if(mo_Msg[ulLineCounter][0] != 0)
		{
			TxtLenMax = pdc->GetTextExtent(&mo_Msg[ulLineCounter][0]);
			if(TxtLenMax.cx > TxtLen.cx) TxtLen.cx = TxtLenMax.cx;
			if(TxtLenMax.cy > TxtLen.cy) TxtLen.cy = TxtLenMax.cy;
			ulRealLineNumber++;
		}
	}

	o_Rect.top = pt.y + 20;
	o_Rect.bottom = ulRealLineNumber * TxtLen.cy + 4;
	o_Rect.left = pt.x + 10;
	o_Rect.right = TxtLen.cx + 10;
	GetDesktopWindow()->GetClientRect(&o_BigRect);
	if(o_BigRect.right < o_Rect.left + o_Rect.right) o_Rect.left = o_BigRect.right - o_Rect.right;
	if(o_BigRect.bottom < o_Rect.top + o_Rect.bottom) o_Rect.top = pt.y - 20 - o_Rect.bottom;
	SetWindowPos
	(
		NULL,
		o_Rect.left,
		o_Rect.top,
		o_Rect.right,
		o_Rect.bottom,
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME
	);
	po_Focus = GetFocus();
	ReleaseDC(pdc);
	ShowWindow(SW_SHOW);
	if(po_Focus) po_Focus->SetFocus();
	GetClientRect(&o_Rect);
	brh.CreateSolidBrush(0xB0f0f0);

	pdc = GetWindowDC();
	pdc->SelectObject(&M_MF()->mo_Fnt);

	pdc->FillRect(&o_Rect, &brh);
	brh.DeleteObject();
	pdc->Draw3dRect(&o_Rect, 0, 0);

	o_Rect.left += 5;
	o_Rect.top += 2;
	pdc->SetTextColor(0x0000000);
	pdc->SetBkMode(TRANSPARENT);
	for(ulLineCounter = 0; ulLineCounter < ulNumberOfLines; ulLineCounter++)
	{
		if(mo_Msg[ulLineCounter][0] != 0)
		{
			pdc->ExtTextOut
				(
					o_Rect.left,
					o_Rect.top,
					0,
					NULL,
					&mo_Msg[ulLineCounter][0],
					L_strlen(&mo_Msg[ulLineCounter][0]),
					NULL
				);
			o_Rect.top += TxtLen.cy;
		}
	}

	ReleaseDC(pdc);

	M_MF()->UpdateWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_ToolTipWnd::PreCreateWindow(CREATESTRUCT &cs)
{
	CFrameWnd::PreCreateWindow(cs);
	cs.dwExStyle = WS_EX_TOPMOST;
	return TRUE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_ToolTipWnd::Paint(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_ToolTipWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ulSpeed = 200;
	ulNumberOfLines = 0;
	CFrameWnd::OnCreate(lpCreateStruct);
	memset(mo_Msg, 0, 20 * 256);
	return 0;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_ToolTipWnd::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_ToolTipWnd::OnDestroy(void)
{
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDI_cl_App, CWinApp)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

EDI_cl_EnterWnd *EDI_gpo_EnterWnd = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int HookMem
(
	int					allocType,
	void				*userData,
	size_t				size,
	int					blockType,
	long				requestNumber,
	const unsigned char *filename,
	int					lineNumber
)
{
	if(ENG_gb_ExitApplication) return TRUE;
	switch(allocType)
	{
	case _HOOK_ALLOC:
		MEM_gi_AllocatedMemory += size;
		break;
	case _HOOK_REALLOC:
		MEM_gi_AllocatedMemory -= _msize_dbg(userData, blockType);
		MEM_gi_AllocatedMemory += size;
		break;
	case _HOOK_FREE:
		MEM_gi_AllocatedMemory -= _msize_dbg(userData, blockType);
		break;
	}

	return TRUE;
}

CWnd* GetSplashWindow( )
{
	return EDI_gpo_EnterWnd;
}

#ifdef JADEFUSION
extern BOOL					LOA_gb_SpeedMode;
extern BOOL					LOA_gb_SlashY;
extern int					BIG_gi_SpecialRead;
extern int					TEX_gi_ForceText;
#else
extern "C" BOOL					LOA_gb_SpeedMode;
extern "C" BOOL					LOA_gb_SlashY;
extern "C" int					BIG_gi_SpecialRead;
extern "C" int					TEX_gi_ForceText;
#endif
extern CList<CString, CString>	go_IfDefConsts;
extern char						*AI_PATH;

#ifdef JADEFUSION
extern void					AI2C_LoadFixModelList(void);
extern void					AI2C_MakeMapFixList(void);
extern void					AI2C_UnloadFixModelList(void);
#else
extern "C" void					AI2C_LoadFixModelList(void);
extern "C" void					AI2C_MakeMapFixList(void);
extern "C" void					AI2C_UnloadFixModelList(void);
#endif

#if defined(XML_CONV_TOOL)
ULONG gXmlConvRetVal = 0;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_App::InitInstance(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*psz_Scan;
	char				*psz_Beg;
	char				c_mem;
	char				az[1024];
	_AFX_THREAD_STATE	*pState = AfxGetThreadState();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEMpro_Init();

#ifdef JADEFUSION
    // Remember the directory in which the editor was executed
    {
        CHAR szWindowsDir[L_MAX_PATH];
        CHAR szProgramFilesDir[L_MAX_PATH];

        GetWindowsDirectory(szWindowsDir, L_MAX_PATH);
        strcpy(szProgramFilesDir, ":\\Program Files");

        GetCurrentDirectory(L_MAX_PATH, EDI_gsz_StartupPath);

        if (!IsDebuggerPresent())
        {
            CHAR szFullPath[L_MAX_PATH];
            CHAR szDrive[_MAX_DRIVE];
            CHAR szDirectory[_MAX_DIR];
            GetModuleFileName(GetModuleHandle(NULL), szFullPath, L_MAX_PATH);
            _splitpath(szFullPath, szDrive, szDirectory, NULL, NULL);
            _makepath(EDI_gsz_StartupPath, szDrive, szDirectory, NULL, NULL);

            // Use the right directory when running with Ctrl+F5 under MSVC
            if (strstr(EDI_gsz_StartupPath, "Output") != NULL)
            {
                GetCurrentDirectory(L_MAX_PATH, EDI_gsz_StartupPath);
            }
        }

        ERR_OutputDebugString("[Info] Default path is set to '%s'\n", EDI_gsz_StartupPath);
    }
#endif

	_CrtSetAllocHook(HookMem);
	AfxEnableMemoryTracking(FALSE);

#ifdef WIN32
	AfxSocketInit();
	AfxInitRichEdit2();
#endif // #ifdef WIN32

	HKEY h_Key;
	if ( RegOpenKey( HKEY_CURRENT_USER, KEY_ROOT, &h_Key ) != ERROR_SUCCESS )
	{
		BIG_SetKeyID( BIG_uc_ComputeKeyID() );
	}
	else
	{
		RegCloseKey( h_Key );
	}

#if !defined(XML_CONV_TOOL)
    strcpy(EDI_az_LogFileName,"binerr.log");

    ULONG ulRegBFSize = BIG_ul_GetBFSize();

	/* Test command line */
	*masz_ToOpen = 0;
	for(psz_Scan = m_lpCmdLine; psz_Scan && *psz_Scan; psz_Scan++)
	{
		while(*psz_Scan == ' ') psz_Scan++;

		if(*psz_Scan == '/')
		{
			psz_Scan++;
			switch(*psz_Scan)
			{
			case '6':
				AI_gb_GenOptim2 = TRUE;
				break;

			case 'b':
			case 'B':	/* /B : binarization mode */
				LOA_gb_SpeedMode = TRUE;
#ifdef JADEFUSION
				EDI_gb_Automated = TRUE;
#endif
				break;

			case 'c':
			case 'C':
				/*
				 * /cleanbinfile(<wol file>) : delete all generated .bin files for the .wol <wol
				 * file>
				 */
				if(L_strnicmp(psz_Scan, "cleanbinfiles", 13) == 0)
				{
					/*~~~~~~~~~~~~~~~*/
					char	*psz_Start;
					int		ii = 0;
					/*~~~~~~~~~~~~~~~*/

					psz_Start = L_strchr(psz_Scan, '(');
					if(psz_Start++)
					{
						while(*psz_Start != ')')
						{
							EDI_gaz_SlashL[ii] = *psz_Start;
							ii++;
							psz_Start++;
						}

						EDI_gaz_SlashL[ii] = '\0';
						EDI_gb_CleanBinFiles = TRUE;
						psz_Scan = psz_Start + 1;
					}
#ifdef JADEFUSION
					EDI_gb_Automated = TRUE;
#endif
				}
				else if(L_strnicmp(psz_Scan, "cleanbyfat", 10) == 0)
				{
					EDI_gb_CleanByFat = TRUE;
				}
				else if(L_strnicmp(psz_Scan, "clean", 5) == 0)
				{
					EDI_gb_Clean = TRUE;
				}

				/* /C<wol file> : compil AI after loading <wol file> */
				else
				{
					L_strcpy(EDI_gaz_SlashC, psz_Scan + 1);
					EDI_gb_SlashC = TRUE;
					if(L_strchr(EDI_gaz_SlashC, ' ')) *L_strchr(EDI_gaz_SlashC, ' ') = 0;
				}
				break;

			case 'd':
			case 'D':
				/* /D<macro> : define the macro <macro> for AI compilation */
				L_strcpy(az, psz_Scan + 1);
				if(L_strchr(az, ' ')) *L_strchr(az, ' ') = 0;
				go_IfDefConsts.AddTail(az);
				break;

			case 'e':
			case 'E':
				if(L_strnicmp(psz_Scan, "exfilekey", 9) == 0)
				{
					/*~~~~~~~~~~~~~~~*/
					/* /exfilekey(<file name>) : exchanging key during loading */
					char	*psz_Start;
					int		ii = 0;
					/*~~~~~~~~~~~~~~~*/

					psz_Start = L_strchr(psz_Scan, '[');
					if(psz_Start++)
					{
						while(*psz_Start != ']')
						{
							LOA_gaz_EdiExFileKey[ii] = *psz_Start;
							ii++;
							psz_Start++;
						}

						LOA_gaz_EdiExFileKey[ii] = '\0';
						LOA_gb_EdiExFileKey = TRUE;
						psz_Scan = psz_Start + 1;
					}
				}
				else if(!L_strnicmp(psz_Scan, "exportToKeys", 12))
				{
					EDI_gb_ExportToKeys = TRUE;
					EDI_gb_Automated    = TRUE;

					psz_Scan +=12;
					while(*psz_Scan == ' ') psz_Scan++;
					EDI_gb_ExportToKeys_Path = psz_Scan;
					EDI_gb_ExportToKeys_Path.erase(EDI_gb_ExportToKeys_Path.find_first_of(' '));
					EDI_gb_ExportToKeys_Path += '/';
				}
				else
				{
					/* /E<n> : set the enabling level for AI source code generation */
					sscanf(psz_Scan + 1, "%d", &AI2C_gul_MinInstanceNbForGeneration);
				}
				break;

			case 'f':
			case 'F':
				if(L_strnicmp(psz_Scan, "flush", 5) == 0) 
				{
					EDI_gul_EdiBigfileFlushChangelist = 1;
				}
                else
                {
    				/* /F : do the clean final operation */
	    			EDI_gb_CleanFinal = TRUE;
                }
				break;

			case 'G':
#ifdef JADEFUSION
				if(!L_strnicmp(psz_Scan, "GL", 2))
				{
					GDI_gi_GDIType = 0;
					psz_Scan += 2;
				}
				else
#endif
				{
				/* /G<console id> : set the internal console id (0=PC, 1=PS2, 2=GC, 3=Xbox) */
				WOR_gi_CurrentConsole = L_atoi(psz_Scan + 1);
				}
				break;

			case 'i':
			case 'I':
				if(!L_strnicmp(psz_Scan, "importFromKeys", 14))
				{
					EDI_gb_ImportFromKeys = TRUE;
					EDI_gb_Automated      = TRUE;

					psz_Scan +=14;
					while(*psz_Scan == ' ') psz_Scan++;
					EDI_gb_ImportFromKeys_Path = psz_Scan;
					EDI_gb_ImportFromKeys_Path.erase(EDI_gb_ImportFromKeys_Path.find_first_of(' '));
					EDI_gb_ImportFromKeys_Path += '/';
				}
				/* /import(<repsource>)to(<repdestination>) : Link to path + import from path */
				else if(L_strnicmp(psz_Scan, "import", 6) == 0)
				{
					/*~~~~~~~~~~~~~~~*/
					char	*psz_Start;
					int		ii = 0;
					/*~~~~~~~~~~~~~~~*/

					EDI_gaz_EdiImportSource[0] = 0;
					EDI_gaz_EdiImportDestination[0] = 0;

					psz_Start = L_strchr(psz_Scan, '(');
					if(psz_Start++)
					{
						while(*psz_Start != ')')
						{
							EDI_gaz_EdiImportSource[ii] = *psz_Start;
							ii++;
							psz_Start++;
						}

						EDI_gaz_EdiImportSource[ii] = '\0';

						psz_Start = L_strchr(psz_Start, '(');
						if(psz_Start++)
						{
							ii = 0;
							while(*psz_Start != ')')
							{
								if(*psz_Start == '\\') *psz_Start = '/';
								EDI_gaz_EdiImportDestination[ii] = *psz_Start;
								ii++;
								psz_Start++;
							}

							EDI_gaz_EdiImportDestination[ii] = '\0';
							EDI_gb_EdiImport = TRUE;
                            EDI_gb_EdiMerge = FALSE;
							psz_Scan = psz_Start + 1;
						}
					}
				}
				break;

			case 'k':
			case 'K':
				/*
				 * /K[fr,en,da,nl,fi, de, it, es, pt, sv, pl, ru, ja, zh, sq, ar, bg, be, el, ko,
				 * no, ro, sr, sk, sl, tr]
				 */
				if(L_strnicmp(psz_Scan, "k[", 2) == 0)
				{
					psz_Scan = INO_p_StoreLanguage(psz_Scan + 2);
				}
				break;

			case 'l':
			case 'L':
				if(L_strnicmp(psz_Scan, "lock:\"", 6) == 0)
				{
					L_strcpy(EDI_gaz_SlashL, psz_Scan + 6);
					if(L_strchr(EDI_gaz_SlashL, '"')) *(L_strchr(EDI_gaz_SlashL, '"')) = 0;
					psz_Scan += L_strlen(EDI_gaz_SlashL);
					EDI_gul_EdiLock = 1;
					break;
				}

				/* /L<wol file> : load one map */
				EDI_gb_SlashL = TRUE;
				EDI_gb_SlashLoadAndQuit = FALSE;
                EDI_gb_SlashLKTM = FALSE;
#ifdef JADEFUSION
				EDI_gb_SlashLXMP = FALSE;
                EDI_gb_SlashLXPK = FALSE;
#endif
				if((*(psz_Scan + 1) == 'q') || (*(psz_Scan + 1) == 'Q'))
				{
					EDI_gb_SlashLoadAndQuit = TRUE;
					psz_Scan++;
				}
                // Do this test in multiple step for avoiding overflow
                else if((*(psz_Scan + 1) == 'k') || (*(psz_Scan + 1) == 'K'))
                {
                    if((*(psz_Scan + 2) == 't') || (*(psz_Scan + 2) == 'T'))
                    {
                        if((*(psz_Scan + 3) == 'm') || (*(psz_Scan + 3) == 'M'))
                        {
                            EDI_gb_SlashLKTM = TRUE;
                            psz_Scan+=3;                                                		
                        }
                    }
                }
#ifdef JADEFUSION
                // Check consistency of XMP (Xenon Mesh Processing)
                else if((*(psz_Scan + 1) == 'x') || (*(psz_Scan + 1) == 'X'))
                {
                    if((*(psz_Scan + 2) == 'm') || (*(psz_Scan + 2) == 'M'))
                    {
                        if((*(psz_Scan + 3) == 'p') || (*(psz_Scan + 3) == 'P'))
                        {
                            EDI_gb_SlashLXMP = TRUE;
                            psz_Scan+=3;
                        }
                    }
                    else if ((*(psz_Scan + 2) == 'p') || (*(psz_Scan + 2) == 'P'))
                    {
                        if((*(psz_Scan + 3) == 'k') || (*(psz_Scan + 3) == 'K'))
                        {
                            EDI_gb_SlashLXPK = TRUE;
                            psz_Scan+=3;
                        }
                    }
                }
#endif

				L_strcpy(EDI_gaz_SlashL, psz_Scan + 1);
//				if(L_strchr(EDI_gaz_SlashL, ' ')) *L_strchr(EDI_gaz_SlashL, ' ') = 0;
				if(L_strchr(EDI_gaz_SlashL, '.')) *(L_strchr(EDI_gaz_SlashL, '.')+4) = 0;
				{
					/*~~~~~~~*/
					FILE	*f;
					/*~~~~~~~*/

					f = fopen(EDI_az_LogFileName, "at");
					if(!f) f = fopen(EDI_az_LogFileName, "wt");
					fprintf(f, "\n\n*********** MAP %s ************\n\n", EDI_gaz_SlashL);
					fclose(f);
				}
                psz_Scan += L_strlen(EDI_gaz_SlashL);
				break;

			case 'm':
			case 'M':
				/* /makemaplist(<file name>) : generate the list of the map */
				if(L_strnicmp(psz_Scan, "makemaplist", 11) == 0)
				{
					/*~~~~~~~~~~~~~~~*/
					char	*psz_Start;
					int		ii = 0;
					/*~~~~~~~~~~~~~~~*/

					psz_Start = L_strchr(psz_Scan, '(');
					if(psz_Start++)
					{
						while(*psz_Start != ')')
						{
							EDI_gaz_EdiMapList[ii] = *psz_Start;
							ii++;
							psz_Start++;
						}

						EDI_gaz_EdiMapList[ii] = '\0';
						EDI_gb_MakeMapList = TRUE;
						psz_Scan = psz_Start + 1;
					}
				}
				/* /merge(<bigfile source>):(<folder>) : import merge filtered */
				else if(L_strnicmp(psz_Scan, "merge", 5) == 0)
				{
					/*~~~~~~~~~~~~~~~*/
					char	*psz_Start;
					int		ii = 0;
					/*~~~~~~~~~~~~~~~*/


                    EDI_gaz_EdiMergeDir[0] = 0;
					EDI_gaz_EdiBigfileSource[0] = 0;

					psz_Start = L_strchr(psz_Scan, '(');
					if(psz_Start++)
					{
						while(*psz_Start != ')')
						{
							EDI_gaz_EdiBigfileSource[ii] = *psz_Start;
							ii++;
							psz_Start++;
						}
						EDI_gaz_EdiBigfileSource[ii] = '\0';

						psz_Start = L_strchr(psz_Start, '(');
						if(psz_Start++)
						{
							ii = 0;
							while(*psz_Start != ')')
							{
								if(*psz_Start == '\\') *psz_Start = '/';
								EDI_gaz_EdiMergeDir[ii] = *psz_Start;
								ii++;
								psz_Start++;
							}
                            EDI_gaz_EdiMergeDir[ii] = '\0';
                            psz_Start++;

                                                              
                            EDI_gb_EdiMerge = TRUE;
							psz_Scan = psz_Start + 1;
						}
					}
                }
#ifdef JADEFUSION
				else if(L_strnicmp(psz_Scan, "morphlog", 8) == 0)
                {
                    EDI_gb_MorphLog = TRUE;
                }
#endif
				break;

			case 'n':
			case 'N':
				/* /nosound */
				if(L_strnicmp(psz_Scan, "nosound", 7) == 0) SND_gc_NoSound = 1;
#ifdef MONTREAL_SPECIFIC
                 else if(L_strnicmp(psz_Scan, "nocheck", 7) == 0) 
					EDI_gb_DontCheckVersion = TRUE;
				else if(L_strnicmp(psz_Scan, "noperforce", 10) == 0) 
					DAT_CPerforce::GetInstance()->PermanentDisable();
				else if(L_strnicmp(psz_Scan, "nokeyserver", 11) == 0)
					EDI_gb_DontCheckKeyServer = TRUE;
#endif
				else if(L_strnicmp(psz_Scan, "nop4", 4) == 0) EDI_gb_BatchModeWithoutPerforce = TRUE;
				break;

			case 'o':
			case 'O':
				/* /O : activate the preprocess mode */
				EDI_gb_ComputeMap = TRUE;
#ifdef JADEFUSION
				EDI_gb_Automated  = TRUE;
#endif
				break;

			case 'P':

				/* Generation */
				if(psz_Scan[1] == 'G')
				{
					EDI_gpz_SlashP = EDI_gpz_SlashPbuffer;
                    
                    psz_Scan += 2; // skip PG
                    psz_Scan ++;   // skip '"'
                    
                    while(*psz_Scan != '"') *EDI_gpz_SlashP++ = *psz_Scan++;
                    *EDI_gpz_SlashP = 0;

                    psz_Scan ++;   // skip '"'
                    EDI_gpz_SlashP = EDI_gpz_SlashPbuffer;
				}

				/* Log */
				else
				{
					EDI_gpz_SlashPG = psz_Scan;
                    psz_Scan += 2; // skip PP
				}

				//while(*psz_Scan) psz_Scan++;
				break;

			case 'q':
			case 'Q':
				/* /Q */
#ifdef JADEFUSION
                if (L_strnicmp(psz_Scan, "quiet", 5) == 0)
                {
                    EDI_gb_XeQuiet = TRUE;
                }
                else if (L_strnicmp(psz_Scan, "quickload", 9) == 0)
                {
                    EDI_gb_XeQuickLoad = TRUE;
                }
                else
 #endif
				{
				EDI_gb_SlashQ = TRUE;
				}
				break;

			case 'r':
			case 'R':
#ifdef JADEFUSION
				if (L_strnicmp(psz_Scan, "refreshserverinfo", 17) == 0)
					EDI_gb_EdiBigfileRefreshServerInfo = TRUE;
				else
#endif
				{
				/* /R : binarization of the fix map */
				BIG_gi_SpecialRead = 1;
				LOA_gb_SpeedMode = TRUE;
				if(psz_Scan[1] == '(')
				{
					/* /R(<num>) : bin du fix # <num> */
					sscanf(psz_Scan+1, "(%d)", &BIG_gi_SpecialRead);
				}
				}
				break;

			case 'S':
			case 's':
				if(L_strnicmp(psz_Scan, "soundreport", 11) == 0)
					EDI_gb_SaveSoundReport = TRUE;
				else if(L_strnicmp(psz_Scan, "soundcheck", 10) == 0)
					EDI_gb_SoundCheck= TRUE;
				else if( L_strnicmp(psz_Scan, "sync", 4) == 0 || 
                    L_strnicmp(psz_Scan, "syncclean", 9) == 0 || 
                    L_strnicmp(psz_Scan, "syncforce", 9) == 0 )
				{
					/*~~~~~~~~~~~~~~~*/
					char	*psz_Start;
					int		ii = 0;
					char	szChangelist[64];
					/*~~~~~~~~~~~~~~~*/

					psz_Start = L_strchr(psz_Scan, ':');
					if(psz_Start++)
					{
						while(*psz_Start && *psz_Start != ' ')
						{
							szChangelist[ii] = *psz_Start;
							ii++;
							psz_Start++;
						}
						szChangelist[ii] = '\0';
					}

					const ULONG ulChangelist = atol( szChangelist );

					if( ulChangelist > 0 )
					{
						EDI_gul_EdiBigfileSyncChangelist = ulChangelist;
					}

					if ( L_strnicmp(psz_Scan, "syncclean", 9) == 0 ) 
					{
						EDI_gul_EdiBigfileSyncChangelistClean = 1;
					}

					if ( L_strnicmp(psz_Scan, "syncforce", 9) == 0 ) 
					{
						EDI_gul_EdiBigfileSyncForce = 1;
					}

				}
				else
				{
					/* /S : generation of the key list of the fix map */
					EDI_gi_GenSpe = 1;
					if(psz_Scan[1] == '(')
					{
						/* /S(<num>) : bin du fix # <num> */
						sscanf(psz_Scan+1, "(%d)", &EDI_gi_GenSpe);
					}
#ifdef JADEFUSION
					EDI_gb_Automated = TRUE;
#endif
				}
				break;

			case 't':
			case 'T':
				/*$F
				 *
				 * /T<tag> : force the capacities of texture manager 
				 *   <tag> : 4 enable 4bpp loading
				 *           8 enable 8bpp loading
				 *           A enable alpha palette loading
				 *           M enable ??
				 *           X = enable all
				 */
				if(psz_Scan[1] == '4')
					TEX_gi_ForceText |= TEX_Manager_Accept4bpp;
				else if(psz_Scan[1] == '8')
					TEX_gi_ForceText |= TEX_Manager_Accept8bpp;
				else if((psz_Scan[1] == 'A') || (psz_Scan[1] == 'a'))
					TEX_gi_ForceText |= TEX_Manager_AcceptAlphaPalette;
				else if((psz_Scan[1] == 'M') || (psz_Scan[1] == 'm'))
					TEX_gi_ForceText |= TEX_Manager_OneTexForRawPal;
				else if((psz_Scan[1] == 'X') || (psz_Scan[1] == 'x'))
					TEX_gi_ForceText |= TEX_Manager_AcceptAllPalette | TEX_Manager_OneTexForRawPal;
				break;

			case 'u':
			case 'U':
				if(L_strnicmp(psz_Scan, "unlock:\"", 8) == 0)
				{
					L_strcpy(EDI_gaz_SlashL, psz_Scan + 8);
					if(L_strchr(EDI_gaz_SlashL, '"')) *(L_strchr(EDI_gaz_SlashL, '"')) = 0;
					psz_Scan += L_strlen(EDI_gaz_SlashL);
					EDI_gul_EdiLock = 2;
					break;
				}

				EDI_gb_Check = TRUE;
				break;

			case 'x':
			case 'X':
#ifdef JADEFUSION
                if((*(psz_Scan + 1) == 'p') || (*(psz_Scan + 1) == 'P'))
                {
                    if((*(psz_Scan + 2) == 'k') || (*(psz_Scan + 2) == 'K'))
                    {
                        EDI_gb_SlashXPK = TRUE;
                    }
                }
                else if((*(psz_Scan + 1) == 't') || (*(psz_Scan + 1) == 'T'))
                {
                    if((*(psz_Scan + 2) == 'x') || (*(psz_Scan + 2) == 'X'))
                    {
                        EDI_gb_SlashXTX = TRUE;
                    }
                }
                else
#endif
				{
				EDI_gb_SX = TRUE;
				}
				break;

			case 'y':
			case 'Y':
				/* /Y(<path>) : set the path for generating AI source code */
				{
					/*~~~~~~~~~~~~~~~*/
					char	*psz_Start;
					int		ii = 0;
					/*~~~~~~~~~~~~~~~*/

					EDI_gaz_SlashY[0] = 0;

					psz_Start = L_strchr(psz_Scan, '(');
					if(psz_Start++)
					{
						while(*psz_Start != ')')
						{
							EDI_gaz_SlashY[ii] = *psz_Start;
							ii++;
							psz_Start++;
						}

						EDI_gaz_SlashY[ii] = '\0';
						LOA_gb_SlashY = TRUE;
						AI_PATH = EDI_gaz_SlashY;
					}
				}
				break;

			case 'w':
			case 'W':
#ifdef JADEFUSION
                if (L_strnicmp(psz_Scan, "wipetga", 7) == 0)
                {
                    EDI_gb_XeWipeTGAs = TRUE;
                }
                else if (L_strnicmp(psz_Scan, "wipedds", 7) == 0)
                {
                    EDI_gb_XeWipeDDSs = TRUE;
                }
#endif
				break;

			case 'z':
			case 'Z':
                {
    				/* /Z : log all warnings into a file whose name is binerr.log or can be read here*/
	    			ERR_gb_Log = TRUE;
		    		EDI_gb_NoVerbose = TRUE;

                    // Read log file name
                    psz_Scan++;
                    if (*psz_Scan == ':')
                    {
                        psz_Scan++;
                        int i = 0;
            			while(*psz_Scan && *psz_Scan != ' ') 
                        {
                            EDI_az_LogFileName[i] = *psz_Scan;
                            psz_Scan++;
                            i++;
                        }
                        EDI_az_LogFileName[i] = 0;
                    }

			    	break;
                }
			}

			while(*psz_Scan && *psz_Scan != ' ') psz_Scan++;
			while(*psz_Scan == ' ') psz_Scan++;
			psz_Scan--;
		}
		else
		{
			psz_Beg = psz_Scan;
			while(*psz_Scan && *psz_Scan != ' ') psz_Scan++;
			c_mem = *psz_Scan;
			*psz_Scan = 0;
			L_strcpy(masz_ToOpen, psz_Beg);
			*psz_Scan = c_mem;
			while(*psz_Scan == ' ') psz_Scan++;
			psz_Scan--;
		}
	}

	EDI_gh_ErrorFile = INVALID_HANDLE_VALUE;
	if(EDI_gb_NoVerbose)
	{
		EDI_gh_ErrorFile = CreateFile
			(
				EDI_az_LogFileName,
				GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);

		if(EDI_gh_ErrorFile != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(EDI_gh_ErrorFile, 0, NULL, FILE_END);
			_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_ERROR, EDI_gh_ErrorFile);
			_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_ASSERT, EDI_gh_ErrorFile);
		}
	}

	/* Create logo window */
	EDI_gpo_EnterWnd = new EDI_cl_EnterWnd;
	EDI_gpo_EnterWnd->Create
		(
			NULL,
			"Jade",
			WS_VISIBLE | WS_POPUP,
			CRect
			(
				(GetSystemMetrics(SM_CXSCREEN) / 2) - 160,
				(GetSystemMetrics(SM_CYSCREEN) / 2) - 274,
				(GetSystemMetrics(SM_CXSCREEN) / 2) + 160,
				(GetSystemMetrics(SM_CYSCREEN) / 2) + 274
			),
			NULL,
			NULL
		);
#endif // XML_CONV_TOOL

	/*
	 * To break system when an allocation with a given number is made. When they are
	 * memory leaks, the allocation number is given into braces. £
	 * Set "dbgbreak" to 1 and "val" to the number of the allocation you want to stop.
	 */

	/*~~~~*/
#ifdef _DEBUG
	int val;
	/*~~~~*/

	val = 0;
	if(val)
	{
		_CrtSetBreakAlloc(val);
		_crtBreakAlloc = val;
	}
#endif \
 \
	/*~~~~~~~~~~~~~~~~~~~~~~*/ \
	/* _DEBUG */
	_Try_
	/* Init SDK */
	ENG_InitApplication();
	/*~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(XML_CONV_TOOL)
	int BFObjectXmlConv(HINSTANCE hInstance, LPTSTR    lpCmdLine);

	gXmlConvRetVal = BFObjectXmlConv(AfxGetInstanceHandle(), m_lpCmdLine);

	return FALSE;
#endif // XML_CONV_TOOL

	/* Init application instance */
	MAI_gh_MainInstance = AfxGetInstanceHandle();

	/* Global inits */
	mb_RunEngine = FALSE;
	mpo_LastWnd = NULL;
	mpo_LastView = NULL;
	mpo_FocusWndWhenDlg = NULL;
	EDI_gb_LockKeyboard = FALSE;

	/* Simu */
	mp_SimuMsg = NULL;
	mi_NumSimuMsg = mi_MaxSimuMsg = 0;

	/* Create and display main frame. Window if forced to be maximized. */
	m_pMainWnd = mpo_MainFrame = new(EDI_cl_MainFrame);
	mpo_MainFrame->Create
		(
			NULL,
			"Jade",
			WS_OVERLAPPEDWINDOW,
			CRect(0, 0, 400, 400),
			NULL,
			MAKEINTRESOURCE(MAINFRAME_IDM_MENU)
		);
	MAI_gst_MainHandles.h_OwnerWindow = mpo_MainFrame->m_hWnd;

#ifdef JADEFUSION
	// Force 3D view to be active so that the renderer is initialized.
	EOUT_cl_Frame	*po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

		if(po_Out)
		{
			po_Out->mpo_MyView->IWantToBeActive(po_Out);
		}

#else	
	/*
	 * en cas de binarisation : force la vue 3D à étre visible sinon le monde ne sera
	 * pas attaché à la vue et donc les textures ne seront pas chargées
	 */
	if(LOA_gb_SpeedMode)
	{
		/*~~~~~~~~~~~~~~~~~~~~*/
		EOUT_cl_Frame	*po_Out;
		/*~~~~~~~~~~~~~~~~~~~~*/

		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

		if(po_Out)
		{
			po_Out->mpo_MyView->IWantToBeActive(po_Out);
		}
	}
#endif

	/* Clean final */
	if(EDI_gb_CleanFinal)
	{
		BIG_Clean(TRUE);
		ExitProcess(0);
	}

	if ( EDI_gb_CleanByFat ) 
	{
		BIG_CleanByFat();
		ExitProcess(0);
	}

	if ( EDI_gb_Clean ) 
	{
		BIG_Clean(FALSE);
		ExitProcess(0);
	}

	/* Check */
	if(EDI_gb_Check)
	{
		BIG_b_CheckFile(FALSE);
		ExitProcess(0);
	}

#ifdef JADEFUSION
    if (EDI_gb_XeWipeTGAs)
    {
#if defined(_XENON_RENDER)
        TEX_ReplaceAllTGAs();
#endif
        ExitProcess(0);
    }

    if (EDI_gb_XeWipeDDSs)
    {
#if defined(_XENON_RENDER) && !defined(XML_CONV_TOOL)
        TEX_ReplaceAllDDSs();
#endif
        ExitProcess(0);
    }
#endif

	if(EDI_gb_MakeMapList)
	{
		BIG_MakeWolList(EDI_gaz_EdiMapList);
		ExitProcess(0);
	}

	if (EDI_gb_Automated)
	{
		if (EDI_gb_ExportToKeys)
		{
			DAT_CUtils::Export(EDI_gb_ExportToKeys_Path, std::string(""));
			ExitProcess(0);			
		}
		if (EDI_gb_ImportFromKeys)
		{
			DAT_CUtils::Import(EDI_gb_ImportFromKeys_Path, std::string(""));
			ExitProcess(0);			
		}
	}

	if(EDI_gi_GenSpe)
	{
		/*~~~~~~~~~~~~~*/
		ULONG	ul_Index;
		/*~~~~~~~~~~~~~*/

		ul_Index = BIG_ul_SearchDir(BIG_Csz_Root "/Bin");
		if(ul_Index != BIG_C_InvalidIndex) BIG_DelDir(BIG_Csz_Root "/Bin");

		if(WOR_gi_CurrentConsole == 2)
		{
			/* only for gamecube */
			AI2C_LoadFixModelList();
			AI2C_MakeMapFixList();
			AI2C_UnloadFixModelList();
		}
	}

    // Refresh P4 server info if the size of the bf is not the same 
    // as it was last time it was closed (so that bf and server are coherent).
#ifdef JADEFUSION
    EDI_InitialP4RefreshServerInfo();
#endif
	
	ULONG ulBFSize = BIG_ul_GetBFSize();
    if (!EDI_gb_BatchModeWithoutPerforce && 
        ulBFSize != ulRegBFSize && 
        DAT_CPerforce::GetInstance()->P4Connect(FALSE)) 
	{
	    DAT_CPerforce::GetInstance()->P4FlushBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
		DAT_CPerforce::GetInstance()->P4Disconnect();
    }

	/* One idle */
	OnRealIdle();

	/* Delete intro window */
	EDI_gpo_EnterWnd->DestroyWindow();
	EDI_gpo_EnterWnd = NULL;

	/* Set icon and title */
	mpo_MainFrame->UpdateMainFrameTitle();

	/* Show main window */
	mpo_MainFrame->SetWindowPlacement(&mpo_MainFrame->mst_ExternIni.st_Placement);
	m_nCmdShow = mpo_MainFrame->mst_ExternIni.st_Placement.showCmd;
	::ShowWindow(mpo_MainFrame->GetSafeHwnd(), m_nCmdShow);
	mpo_MainFrame->SetIcon(LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);
	EDI_gb_CanBin = TRUE;

	/* Create hook for keyboard messages */
	mh_HookKeyboard = SetWindowsHookEx(WH_KEYBOARD, x_KeyboardHook, NULL, AfxGetThread()->m_nThreadID);

	/* Create hook for mouses messages */
	mh_HookMouse = SetWindowsHookEx(WH_MOUSE, x_MouseHook, NULL, AfxGetThread()->m_nThreadID);

	while(::PeekMessage(&(GE_MFC_MSG), NULL, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&(GE_MFC_MSG));
		DispatchMessage(&(GE_MFC_MSG));
	}

	/* Inform editors about all preregisters datas (during init) */
	LINK_UpdatePointers();

	/*~~~~~~~~~~~~~~~~~~~~*/
	_Catch_ _Return_(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~*/

	_End_

	/* Auto check bigfile ? */
	if(BIG_Handle())
	{
		if(!EDI_gb_NoVerbose)
		{
			mpo_MainFrame->mst_Ini.ui_NumOpen++;
			if(mpo_MainFrame->mst_Ini.ui_NumOpen >= 10)
			{
				mpo_MainFrame->mst_Ini.ui_NumOpen = 0;
				mpo_MainFrame->SaveIni();
				AfxGetApp()->DoWaitCursor(1);
				if(mpo_MainFrame->MessageBox("Do you want to check the bigfile ?", "Jade", MB_YESNO) == IDYES)
					BIG_b_CheckFile(FALSE);
				AfxGetApp()->DoWaitCursor(-1);
			}
			else
			{
				mpo_MainFrame->SaveIni();
			}
		}
	}

	MEM_gi_AddAllocatedMemory = MEM_gi_AllocatedMemory;

	/* Create logo window */
	EDI_gpo_EnterWnd2 = new EDI_cl_ToolTipWnd;
	EDI_gpo_EnterWnd2->Create(NULL, NULL, WS_POPUP, CRect(0, 0, 1, 1), mpo_MainFrame, NULL);

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern EDI_cl_MainFrame *EDI_gpstMainFrame;
int EDI_cl_App::ExitInstance(void)
{
#if defined(XML_CONV_TOOL)
	return gXmlConvRetVal;
#endif

	LINK_gb_AllRefreshEnable = FALSE;
	if(EDI_gpo_EnterWnd2)
	{
		EDI_gpo_EnterWnd2->DestroyWindow();
		EDI_gpo_EnterWnd2 = NULL;
	}

	if (mh_HookKeyboard) UnhookWindowsHookEx(mh_HookKeyboard);
	if (mh_HookMouse) UnhookWindowsHookEx(mh_HookMouse);
	if (EDI_gpstMainFrame) delete mpo_MainFrame;
	mpo_MainFrame = NULL;
	ENG_CloseApplication();

	if(EDI_gh_ErrorFile != INVALID_HANDLE_VALUE) CloseHandle(EDI_gh_ErrorFile);
	EDI_gh_ErrorFile = INVALID_HANDLE_VALUE;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_App::po_EditorUnderMouse(void)
{
	/*~~~~~~~~~~~~*/
	CPoint	pt;
	CWnd	*po_Wnd;
	/*~~~~~~~~~~~~*/

	GetCursorPos(&pt);
	po_Wnd = M_MF()->WindowFromPoint(pt);
	while(po_Wnd && !po_Wnd->IsKindOf(RUNTIME_CLASS(EDI_cl_BaseFrame))) po_Wnd = po_Wnd->GetParent();
	return (EDI_cl_BaseFrame *) po_Wnd;
}

/*
 =======================================================================================================================
    Aim: Special process for mouse wheel. We dispatch the message to the 3D view under mouse.
 =======================================================================================================================
 */
BOOL EDI_cl_App::b_ProcessMouseWheel(MSG *_p_Msg)
{
	/*~~~~~~~~~~~~*/
	CPoint	pt;
	CWnd	*po_Wnd;
	/*~~~~~~~~~~~~*/

	if(_p_Msg->message == WM_MOUSEWHEEL)
	{
		extern EDIA_cl_SmoothSelectionDialog *p_TheOnlyOneSmoothSelDiag;
		GetCursorPos(&pt);
		/* Philippe le hacker */
		if (p_TheOnlyOneSmoothSelDiag && p_TheOnlyOneSmoothSelDiag->OnMouseWheel(LOWORD(_p_Msg->wParam), (short)HIWORD(_p_Msg->wParam), pt)) return TRUE;

		po_Wnd = M_MF()->WindowFromPoint(pt);
		if(po_Wnd && IsWindow(po_Wnd->m_hWnd))
		{
			if(po_Wnd->IsKindOf(RUNTIME_CLASS(F3D_cl_View)))
			{
				po_Wnd->ScreenToClient(&pt);
				po_Wnd->SendMessage(WM_MOUSEWHEEL, _p_Msg->wParam, pt.x + (pt.y << 16));
				return TRUE;
			}

			if(po_Wnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd))) return TRUE;
			if(po_Wnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) return TRUE;
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_LogBug(char *_psz_Add)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
#if 0
	FILE		*f;
	char		asz_Name[128];
	char		asz_Msg[512];
	char		asz_Buf[512];
	ULONG		i_Len;
	int			i_T;
	static int	i_Time = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	i_T = timeGetTime();
	if(i_T - i_Time < 3000) return;
	i_Time = i_T;

	i_Len = 128;
	GetComputerName(asz_Name, &i_Len);
	_strtime(asz_Buf);
#if MONTREAL_SPECIFIC
	// For the builds in Montreal, display the Montreal build number as well as the Montpellier build number.
	sprintf(asz_Msg, "%s  Version %d(%d)  %s  %s", asz_Name, BIG_Cu4_MontrealAppVersion, BIG_Cu4_AppVersion, asz_Buf, _psz_Add);
#else
#ifdef JADEFUSION
	sprintf(asz_Msg, "%s  Version Xe-%d(%d,%d)  %s  %s", asz_Name, BIG_Cu4_MontrealXeAppVersion, BIG_Cu4_MontrealAppVersion, BIG_Cu4_AppVersion, asz_Buf, _psz_Add);
#else
	sprintf(asz_Msg, "%s  Version %d  %s  %s", asz_Name, BIG_Cu4_AppVersion, asz_Buf, _psz_Add);
#endif
#endif

	f = fopen("\\\\mtp-cbeaudet\\bigfiles\\jade.bug", "r+b");
	if(!f)
	{
		f = fopen("\\\\mtp-cbeaudet\\bigfiles\\jade.bug", "wb");
		if(f)
		{
			fprintf(f, "%s\n", asz_Msg);
			fclose(f);
		}
	}
	else
	{
		fseek(f, 0, SEEK_END);
		fprintf(f, "%s\n", asz_Msg);
		fclose(f);
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_i_Exception(void)
{
	/*~~~~~~*/
	int i_Res;
	/*~~~~~~*/

	if(EDI_gb_NoVerbose)
	{
        ERR_X_Warning(0, "*** Exception ***", NULL);
		ExitProcess(-1);
	}
	else
	{
		EDI_LogBug("Exception");
		i_Res = MessageBox
			(
				NULL,
				"Un bug ä été détecté.\nAppuyez sur YES pour debugger.\nAppuyer sur NO pour tenter de continuer.",
				"Jade Meditation !!!",
				MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2
			);
		if(i_Res == IDYES) return 0;
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_App::PumpMessage(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor, *po_OneEd, *po_FirstEd;
	_AFX_THREAD_STATE	*pState = AfxGetThreadState();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* If keyboard is locked, considered it's for a normal dialog box */
	if(EDI_gb_LockKeyboard) return CWinApp::PumpMessage();

	/* Exit */
	if(!::GetMessage(&(GE_MFC_MSG), NULL, NULL, NULL)) return FALSE;

	/*
	 * Send mousewheel to 3D view under mouse. This is done here and not if mouse hook
	 * cause I don't know of to get message wParam and lParam in hook !!!
	 */
	if(b_ProcessMouseWheel(&(GE_MFC_MSG))) return TRUE;

	/* Translate and dispatch message */
	if((GE_MFC_MSG).message != WM_KICKIDLE && !PreTranslateMessage(&(GE_MFC_MSG)))
	{
		__try
		{
			_Try_
			::TranslateMessage(&(GE_MFC_MSG));
			::DispatchMessage(&(GE_MFC_MSG));
			_Catch_ 
			_End_
		}

		__except(EDI_i_Exception())
		{
		}
	}

	/* Update the CPU Clock Frequency if necessary */
	TIM_UpdateCPUClockFrequency();

	/* Test to run engine */
	if(mb_RunEngine)
	{
		/* Force a 3D view to have the focus */
		po_FirstEd = GetFocusedEditor();
		if(!po_FirstEd || po_FirstEd->mst_Def.i_Type != EDI_IDEDIT_OUTPUT)
		{
			po_FirstEd = po_OneEd = NULL;
			for(i = 0; i < EDI_C_MaxViews; i++)
			{
				po_View = M_MF()->po_GetViewWithNum(i);
				for(j = 0; j < M_CurNumEd(po_View->mi_NumPane); j++)
				{
					po_Editor = M_CurEd(po_View->mi_NumPane, j).po_Editor;
					if(po_Editor && po_Editor->mst_Def.i_Type == EDI_IDEDIT_OUTPUT)
					{
						if(po_Editor->mb_IsPresent)
						{
							po_OneEd = po_Editor;
							if(!po_Editor->mpo_Original) po_FirstEd = po_Editor;
						}
					}
				}
			}
		}

		if(!po_FirstEd) po_FirstEd = po_OneEd;
		if(po_FirstEd)
		{
			po_FirstEd->mpo_MyView->IWantToBeActive(po_FirstEd);
			po_FirstEd->mpo_MyView->SetFocus();
		}

		/* Run engine */
		mpo_MainFrame->BeforeEngine();
		ENG_Init();
		__try
		{
			ENG_Loop();
		}

		__except(EDI_i_Exception())
		{
		}

		mb_RunEngine = FALSE;
		mpo_MainFrame->AfterEngine();
		LINK_UpdatePointers();
		LINK_Refresh();

		/* If ENG_gb_ExitApplication is TRUE, close application */
		if(ENG_gb_ExitApplication) mpo_MainFrame->PostMessage(WM_CLOSE);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_App::Run(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				bIdle;
	LONG				lIdleCount;
	_AFX_THREAD_STATE	*pState = AfxGetThreadState();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	__try
	{
		bIdle = TRUE;
		lIdleCount = 0;

		for(;;)
		{
			while(bIdle && !::PeekMessage(&(GE_MFC_MSG), NULL, NULL, NULL, PM_NOREMOVE))
			{
				if(!OnIdle(lIdleCount++)) bIdle = FALSE;
			}

			if(!::PeekMessage(&(GE_MFC_MSG), NULL, NULL, NULL, PM_NOREMOVE))
			{
				OnRealIdle();
			}

			do
			{
				/* Process message */
				if(!PumpMessage()) return ExitInstance();

				/* Idle init */
				if(IsIdleMessage(&(GE_MFC_MSG)))
				{
					bIdle = TRUE;
					lIdleCount = 0;
				}
			} while(::PeekMessage(&(GE_MFC_MSG), NULL, NULL, NULL, PM_NOREMOVE));
		}
	}

	__except(EDI_i_Exception())
	{
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_App::GetFocusedEditor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CWnd			*po_Wnd, *po_Wnd1;
	EDI_cl_BaseView *po_View;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Retreive pane that contains current focus wnd */
	if(!BIG_Handle()) return NULL;

	po_Wnd = M_MF()->GetFocus();

	if(po_Wnd && po_Wnd->IsKindOf(RUNTIME_CLASS(CFloating))) po_Wnd = po_Wnd->GetTopWindow();

	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_Wnd1 = po_Wnd;
		while((po_Wnd1) && (po_Wnd1 != M_MF()->po_GetViewWithNum(i))) po_Wnd1 = po_Wnd1->GetParent();
		if(po_Wnd1) break;
	}

	if(!po_Wnd1) return NULL;

	po_View = (EDI_cl_BaseView *) po_Wnd1;
	i = po_View->i_NumEditFromPane(po_View->GetCurSel());
	return M_CurEd(po_View->mi_NumPane, i).po_Editor;
}

/*
 =======================================================================================================================
    Aim: A very strange function. This function is used to highlight a tabcontrol when a child of it has the focus. We
    look the window that has currently the input focus, and we search if one of its parent is a tabcontrol.
 =======================================================================================================================
 */
void EDI_cl_App::SetFocusPaneColor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CWnd	*po_Wnd, *po_Wnd1;
	int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_Wnd = M_MF()->GetFocus();
	if(po_Wnd != mpo_LastWnd)
	{
		mpo_LastWnd = M_MF()->GetFocus();

		/* Retreive pane that contains current focus wnd */
		for(i = 0; i < EDI_C_MaxViews; i++)
		{
			po_Wnd1 = po_Wnd;
			while((po_Wnd1) && (po_Wnd1 != M_MF()->po_GetViewWithNum(i))) po_Wnd1 = po_Wnd1->GetParent();
			if(po_Wnd1) break;
		}

		if(po_Wnd1 != mpo_LastView)
		{
			if(po_Wnd1)
			{
				po_Wnd1->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_FRAME);
				if(((EDI_cl_BaseView *) po_Wnd1)->po_GetActivatedEditor())
				{
					if ( ((EDI_cl_BaseView*) po_Wnd1)->po_GetActivatedEditor()->mpo_Menu != NULL)
					((EDI_cl_BaseView *) po_Wnd1)->po_GetActivatedEditor()->mpo_Menu->Invalidate();
			}
			}

			if(mpo_LastView)
			{
				mpo_LastView->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_FRAME);
				if(((EDI_cl_BaseView *) mpo_LastView)->po_GetActivatedEditor())
				{
					if ( ((EDI_cl_BaseView *) mpo_LastView)->po_GetActivatedEditor()->mpo_Menu != NULL )
					((EDI_cl_BaseView *) mpo_LastView)->po_GetActivatedEditor()->mpo_Menu->Invalidate();
			}
			}

			mpo_LastView = (EDI_cl_BaseView *) po_Wnd1;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_App::OnIdle(LONG _l_Count)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString			o_Str;
//	char			*psz_Tmp;
//	char			asz_Tmp[255];
//	char			asz_Buf[255];
	static ULONG	ul_LastMem = 0;
	static int		i_LastRealMem = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ENG_gb_ForcePauseEngine) return CWinApp::OnIdle(_l_Count);

	/* Test if there's warnings */
	if(!EDI_gb_NoVerbose) ERR_TestWarning();

	/* Display allocated memory in window title */
/*	if
	(
		((ul_LastMem & 0xFFFFC000) != (MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated & 0xFFFFC000))
	||	((i_LastRealMem & 0xFFFFC000) != (MEM_gi_AllocatedMemory & 0xFFFFC000))
	)
	{
		ul_LastMem = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
		i_LastRealMem = MEM_gi_AllocatedMemory;

		M_MF()->GetWindowText(o_Str);
		psz_Tmp = (char *) (LPCSTR) o_Str;
		L_strcpy(asz_Tmp, psz_Tmp);
		psz_Tmp = L_strchr(asz_Tmp, '[');
		if(psz_Tmp) *(psz_Tmp - 5) = 0;
		MEM_MakeAllocatedMemoryString(asz_Buf);
		o_Str = asz_Tmp;
		o_Str += asz_Buf;
		M_MF()->SetWindowText(o_Str);
	}*/

#ifdef _DEBUG
	if(ENG_gb_TestMemory) MEM_CheckAllocatedBlocks(0);
#endif

	/*
	 * Force pane to be ready, and to erase last message. That means that status must
	 * not be used to set persistent text. A long operation will terminate here...
	 */
	LINK_EndLongOperation();

	/* Force drawing tabcontrols for focus aspect. A bit complicated... */
	if(_l_Count == 0) SetFocusPaneColor();

	return CWinApp::OnIdle(_l_Count);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_App::OnRealIdle(void)
{
	/*~~*/
	int i;
	/*~~*/

	/* MSG */
	M_MF()->DisplayMsg();

	if(ENG_gb_ForcePauseEngine) return;

	if(EDI_go_TheApp.EDI_gpo_EnterWnd2)
	{
		if(EDI_go_TheApp.EDI_gpo_EnterWnd2->ulSpeed && (ulRealIdleCounter != 0xFFFFFFFF))
		{
			if(timeGetTime() - ulRealIdleCounter >= EDI_go_TheApp.EDI_gpo_EnterWnd2->ulSpeed)
			{
				ulRealIdleCounter = 0xFFFFFFFF;
				EDI_go_TheApp.EDI_gpo_EnterWnd2->Display();
			}
		}
	}

	M_MF()->VssUpdateStatus();

	/* Inform all editors about idle */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		if(M_MF()->po_GetViewWithNum(i)) M_MF()->po_GetViewWithNum(i)->OnRealIdle();
	}
}

/*
 =======================================================================================================================
    Aim: Function that took a key in parameter, and that request an action to editors and application. Order is focus
    editor, application, and activated editors. In: _uw_Key The virtual key code of the key _b_Pressed TRUE if the key
    is pressed, FALSE else. If key is not pressed but released, the action will not be called, but function will return
    TRUE if the action is known. That is, the hook procedure will not threw the key. Out: TRUE to preserve windows from
    sending the key, FALSE if the key is not known as an action.
 =======================================================================================================================
 */
BOOL EDI_cl_App::b_DispatchKey(USHORT _uw_Key, BOOL _b_Pressed, BOOL _b_Glob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Action;
	EDI_cl_BaseFrame	*po_Focus;
	POSITION			pos;
	EDI_cl_ActionList	*po_List;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Test focus editor action */
    po_Focus = EDI_go_TheApp.GetFocusedEditor();
	if(!_b_Glob)
	{
		if(po_Focus)
		{
			/* Is the editor knows the key (it's a single key, not an action) */
			if(po_Focus->b_KnowsKey(_uw_Key)) return FALSE;

			/* Else test the action */
			ul_Action = po_Focus->mpo_Actions->ul_KeyToAction(_uw_Key);
			if(ul_Action && po_Focus->b_OnActionValidate(ul_Action, FALSE))
			{
				if(_b_Pressed)
				{
					po_Focus->OnAction(ul_Action);
					if(po_Focus->mpo_MenuFrame) po_Focus->mpo_MenuFrame->FillList();
				}

				return TRUE;
			}
		}
	}

	/* _b_Glob. Allow edit */
	if(_b_Glob && M_MF()->b_EditKey(_uw_Key)) return FALSE;

	/* This is the end in fullscreen mode */
	if(_b_Glob || (M_MF()->mpo_MaxView && M_MF()->mst_Desktop.b_VeryMaximized))
	{
		/* Scan for persistent editors */
		pos = M_MF()->mo_ActionList.GetHeadPosition();
		while(pos)
		{
			po_List = M_MF()->mo_ActionList.GetNext(pos);
			if
			(
				(po_List->mpo_Editor)
			&&	(po_List->mpo_Editor != po_Focus)
			&&	(po_List->mpo_Editor->mb_IsActivate)
			&&	(po_List->mpo_Editor->mb_Persist)
			)
			{
				ul_Action = po_List->ul_KeyToAction(_uw_Key);
				if(ul_Action && po_List->mpo_Editor->b_OnActionValidate(ul_Action, FALSE))
				{
					switch(po_List->mpo_Editor->mst_Def.i_Type)
					{
					case EDI_IDEDIT_EVENTS:
						switch(ul_Action)
						{
						case EEVE_ACTION_PLAY:
						case EEVE_ACTION_PLAY60:
						case EEVE_ACTION_PLAY30:
						case EEVE_ACTION_PLAY25:
						case EEVE_ACTION_PLAY15:
						case EEVE_ACTION_ADDFRAMEALL:
						case EEVE_ACTION_ADDXFRAMEALL:
						case EEVE_ACTION_DUPLICATEFRAMEALL:
						case EEVE_ACTION_SHOWALLROTCURVE:
						case EEVE_ACTION_DELALLCURVE:
						case EEVE_ACTION_DELKEYFRAME:
						case EEVE_ACTION_GOTOORIGIN:
						case EEVE_ACTION_GOTOEND:
							break;
						default:
							return TRUE;
						}
					}

					if(_b_Pressed)
					{
						po_List->mpo_Editor->OnAction(ul_Action);
						if(po_List->mpo_Editor->mpo_MenuFrame) po_List->mpo_Editor->mpo_MenuFrame->FillList();
					}

					return TRUE;
				}
			}
		}

		/* MF */
		ul_Action = M_MF()->mpo_Actions->ul_KeyToAction(_uw_Key);
		switch(ul_Action)
		{
		case EDI_ACTION_DESKFULLMAX:
		case EDI_ACTION_DESKMAX:
		case EDI_ACTION_TOGGLEENGINE:
		case EDI_ACTION_ENGINESTEP:
		case EDI_ACTION_REINITENGINE:
		case EDI_ACTION_SPEEDDRAW:
			break;
		default:
			return TRUE;
		}
	}

	/* Test global actions */
	ul_Action = M_MF()->mpo_Actions->ul_KeyToAction(_uw_Key);
	if(ul_Action && M_MF()->b_OnActionValidate(ul_Action, FALSE))
	{
		if(_b_Pressed)
		{
			M_MF()->OnAction(ul_Action);
			if(M_MF()->mpo_MenuFrame) M_MF()->mpo_MenuFrame->FillList();
		}

		return TRUE;
	}

	if(_b_Glob) return FALSE;

	/* Test editors actions */
	pos = M_MF()->mo_ActionList.GetHeadPosition();
	while(pos)
	{
		po_List = M_MF()->mo_ActionList.GetNext(pos);
		if((po_List->mpo_Editor) && (po_List->mpo_Editor != po_Focus) && (po_List->mpo_Editor->mb_IsActivate))
		{
			ul_Action = po_List->ul_KeyToAction(_uw_Key);
			if(ul_Action && po_List->mpo_Editor->b_OnActionValidate(ul_Action, FALSE))
			{
				if(_b_Pressed)
				{
					po_List->mpo_Editor->OnAction(ul_Action);
					if(po_List->mpo_Editor->mpo_MenuFrame) po_List->mpo_Editor->mpo_MenuFrame->FillList();
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim: Global hook to treat keyboard operations.
 =======================================================================================================================
 */
LRESULT CALLBACK x_KeyboardHook(int code, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POINT			pt;
	USHORT			uw_Key;
	BOOL			b_Res;
	HWND			xWnd;
	BOOL			bGlob;
	static BOOL		b_Locked = FALSE;
	static WPARAM	w_MemoLock = 0;
	static BOOL		b_Recurse = FALSE;
	CPoint			point;
	CWnd			*po_Wnd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* No space if mouse on a 3D view */
	if(wParam == VK_SPACE)
	{
		GetCursorPos(&point);
		po_Wnd = M_MF()->WindowFromPoint(point);
		if(po_Wnd)
		{
			if(po_Wnd->IsKindOf(RUNTIME_CLASS(F3D_cl_View)))
			{
				if
				(
					(GetAsyncKeyState(VK_SHIFT) >= 0)
				&&	(GetAsyncKeyState(VK_CONTROL) >= 0)
				&&	(GetAsyncKeyState(VK_MENU) >= 0)
				) return TRUE;
			}
		}
	}

	if(b_Recurse) return FALSE;

	/* Is the global process of key locked ? */
	if(EDI_gb_LockKeyboard) return FALSE;
	if(EDI_gb_LockHook) return FALSE;

	/* Hook only if no dialog */
	bGlob = FALSE;
	if(M_MF()->GetActiveWindow() != M_MF())
	{
		po_Wnd = M_MF()->GetFocus();
		if(po_Wnd && !po_Wnd->IsKindOf(RUNTIME_CLASS(CFloating)))
		{
			do
			{
				if(po_Wnd->IsKindOf(RUNTIME_CLASS(EDI_cl_BaseFrame))) goto testkanmeme;
				if(po_Wnd && po_Wnd->IsKindOf(RUNTIME_CLASS(CFloating))) goto testkanmeme;
				po_Wnd = po_Wnd->GetParent();
			} while(po_Wnd);

			xWnd = ::GetFocus();
			while(xWnd)
			{
				if(APP_go_Modeless.Find(xWnd))
				{
					/* Special case for a single return : Windows does not dispatch this message !! */
					uw_Key = (wParam != VK_SHIFT) && GetAsyncKeyState(VK_SHIFT) & 0x8000 ? SHIFT : 0;
					uw_Key |= (wParam != VK_CONTROL) && GetAsyncKeyState(VK_CONTROL) & 0x8000 ? CONTROL : 0;
					uw_Key |= (wParam != VK_MENU) && GetAsyncKeyState(VK_MENU) & 0x8000 ? ALT : 0;
					uw_Key |= (wParam & 0x00FF);
					if(uw_Key == VK_RETURN)
					{
						po_Wnd = M_MF()->GetFocus();
						po_Wnd->SendMessage(WM_CHAR, VK_RETURN, 0);
						return FALSE;
					}

					if(uw_Key == VK_ESCAPE)
					{
						po_Wnd = M_MF()->GetFocus();
						po_Wnd->SendMessage(WM_CHAR, VK_ESCAPE, 0);
						return FALSE;
					}

					bGlob = TRUE;
					goto testkanmeme;
				}

				xWnd = GetParent(xWnd);
			}

			return FALSE;
		}
	}

testkanmeme:
	/*
	 * When drag&drop, we must pass CTRL and SHIFT cause they can change the Drag&Drop
	 * operation.
	 */
	if(EDI_gst_DragDrop.b_BeginDragDrop)
	{
		if((wParam == VK_CONTROL) || (wParam == VK_SHIFT))
		{
			GetCursorPos(&pt);
			EDI_gst_DragDrop.po_CaptureWnd->ScreenToClient(&pt);
			EDI_gst_DragDrop.po_CaptureWnd->SendMessage(WM_MOUSEMOVE, 0, (pt.y << 16) + pt.x);
		}
	}

	/* Get key depending on flags (combined keys) */
	uw_Key = (wParam != VK_SHIFT) && GetAsyncKeyState(VK_SHIFT) & 0x8000 ? SHIFT : 0;
	uw_Key |= (wParam != VK_CONTROL) && GetAsyncKeyState(VK_CONTROL) & 0x8000 ? CONTROL : 0;
	uw_Key |= (wParam != VK_MENU) && GetAsyncKeyState(VK_MENU) & 0x8000 ? ALT : 0;
	uw_Key |= (wParam & 0x00FF);

	/* To be sure that an action will not be sent more than one time */
	if(b_Locked && w_MemoLock != uw_Key) b_Locked = FALSE;
	if(b_Locked && lParam < 0)
	{
		b_Locked = FALSE;
		return FALSE;
	}

	if(lParam < 0) b_Locked = FALSE;
	if(b_Locked) return TRUE;

	b_Recurse = TRUE;

	/*~~~~~~~~~~~~~~~~~*/
	_Try_	
	b_Res = TRUE;
	/*~~~~~~~~~~~~~~~~~*/

	b_Res = EDI_go_TheApp.b_DispatchKey(uw_Key, lParam >= 0, bGlob);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_Catch_ _End_	b_Recurse = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Lock call if action is valid */
	if(b_Res && (lParam >= 0))
	{
		w_MemoLock = uw_Key;
		b_Locked = TRUE;
	}

	return b_Res;
}

extern BOOL NNN_ForceNow;
extern BOOL NNN_Local;
extern BOOL NNN_Deb;

/*
 =======================================================================================================================
    Aim: Global hook to treat mouse operations.
 =======================================================================================================================
 */
LRESULT CALLBACK x_MouseHook(int code, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MOUSEHOOKSTRUCT		*pStruct;
	HWND				xWnd;
	CWnd				oWnd, *po_Wnd;
	EMEN_cl_Menu		*po_Menu;
	int					iNewSel;
	CDC					*pDC;
	CRect				o_SelRect;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	int					i;
	POINT				pt;
	POSITION			pos;
	LONG				lDistX, lDistY;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(EDI_gb_LockMouse) return FALSE;
	if(EDI_gb_LockHook) return FALSE;

	pStruct = (MOUSEHOOKSTRUCT *) lParam;

	/* Send message ? */
	if(wParam == WM_LBUTTONDBLCLK)
	{
		GetCursorPos(&pt);
		M_MF()->mo_Status.GetItemRect(2, &o_SelRect);
		M_MF()->mo_Status.ClientToScreen(&o_SelRect);
		if(o_SelRect.PtInRect(pt))
		{
			M_MF()->SendMsg();
			return TRUE;
		}
	}

	/*
	 * Special process for focus. We want to force focus on a pane by clicking inside
	 * it
	 */
	if((wParam == WM_LBUTTONDOWN) || (wParam == WM_NCLBUTTONDOWN))
	{
		for(i = 0; i < EDI_C_MaxViews; i++)
		{
			po_View = M_MF()->po_GetViewWithNum(i);
			po_View->GetWindowRect(&o_SelRect);
			if(o_SelRect.PtInRect(pStruct->pt))
			{
				po_Editor = EDI_go_TheApp.GetFocusedEditor();
				if(!po_Editor || po_Editor->mpo_MyView != po_View)
				{
					pos = APP_go_Modeless.GetHeadPosition();
					while(pos)
					{
						xWnd = APP_go_Modeless.GetNext(pos);
						::GetWindowRect(xWnd, &o_SelRect);
						if(o_SelRect.PtInRect(pStruct->pt)) goto nofocus;
					}

					/* Window under mouse must be child of view */
					GetCursorPos(&pt);
					po_Wnd = M_MF()->WindowFromPoint(pt);
					while(po_Wnd)
					{
						if(po_Wnd == po_View) goto focus;
						po_Wnd = po_Wnd->GetParent();
					}

					goto nofocus;
focus: ;
					EDI_go_TheApp.mpo_FocusWndWhenDlg = M_MF()->GetFocus();
					GetCursorPos(&pt);
					po_View->SetFocus();
nofocus: ;
				}
			}
		}
	}

	/*
	 * Special process when selected a top menu. We want menu to be selected by a
	 * simple mouse move
	 */
	if(EDI_gb_TopMenuMode)
	{
		xWnd = WindowFromPoint(pStruct->pt);
		if(xWnd && (EDI_gh_TopMenuMode == xWnd))
		{
			po_Wnd = oWnd.FromHandle(xWnd);
			if(po_Wnd->IsKindOf(RUNTIME_CLASS(EMEN_cl_Menu)))
			{
				/* Get new selected item in menu */
				po_Menu = (EMEN_cl_Menu *) po_Wnd;
				pDC = po_Menu->GetWindowDC();
				iNewSel = po_Menu->i_GetDrawSel(pDC, FALSE, po_Menu->mo_SelRect);

				/* If that new item is different of previous one, simulate a left button down */
				if((iNewSel != po_Menu->mi_LastSel) && (iNewSel != po_Menu->mi_Sel) && (iNewSel != -1))
				{
					po_Menu->mi_LastSel = po_Menu->mi_Sel;
					po_Menu->mi_Sel = iNewSel;
					po_Menu->ScreenToClient(&pStruct->pt);
					mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, pStruct->pt.x, pStruct->pt.y, 0, 1);
				}

				po_Menu->ReleaseDC(pDC);
			}
		}
	}

	/* Tooltips */
	GetCursorPos(&pt);
	lDistX = pt.x - stOldCurPos.x;
	if(lDistX < 0) lDistX = -lDistX;
	lDistY = pt.y - stOldCurPos.y;
	if(lDistY < 0) lDistY = -lDistY;
	if((lDistX > 2) || (lDistY > 2) || (wParam == WM_LBUTTONDOWN) || (wParam == WM_RBUTTONDOWN))
	{
		if(EDI_go_TheApp.EDI_gpo_EnterWnd2 && EDI_gb_AutoHideTip)
		{
			NNN_ForceNow = FALSE;
			NNN_Local = 0;
			NNN_Deb = 0;
			EDI_go_TheApp.EDI_gpo_EnterWnd2->DisplayMessage("", 1);
			EDI_go_TheApp.EDI_gpo_EnterWnd2->SetWindowPos
				(
					NULL,
					1,
					1,
					0,
					0,
					SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME
				);
			ulRealIdleCounter = timeGetTime();
			GetCursorPos(&stOldCurPos);
		}
	}

	if(ENG_gb_EngineRunning) ulRealIdleCounter = timeGetTime();
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_App::AddModeless(HWND _hwnd, BOOL _b_Dialog)
{
	APP_go_Modeless.AddTail(_hwnd);
	if(_b_Dialog) APP_go_ModelessDia.AddTail(_hwnd);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_App::RemoveModeless(HWND _hwnd)
{
	/*~~~~~~~~~~~~*/
	POSITION	pos;
	/*~~~~~~~~~~~~*/

	pos = APP_go_Modeless.Find(_hwnd);
	if(pos) APP_go_Modeless.RemoveAt(pos);
	pos = APP_go_ModelessDia.Find(_hwnd);
	if(pos) APP_go_ModelessDia.RemoveAt(pos);
}
#endif /* ACTIVE_EDITORS */
