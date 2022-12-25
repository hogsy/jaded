// CerbereDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VersionReader.h"

#include "resource.h"
#include "SystemTray.h"
#define	WM_ICON_NOTIFY			WM_APP+10

#include "defines.h"
#include "Cerbere.h"
#include "CerbereDlg.h"
#include "FileWatch.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



/////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

    virtual BOOL OnInitDialog();

// Dialog Data
	enum    { IDD = IDD_ABOUTBOX };
    char    m_szVer[32];

// Implementation
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    char sz_Name[ki_MaxNameLength];
    sprintf(sz_Name, kaz_CERBERE_Name " v%s", m_szVer );
    GetDlgItem(IDC_STATIC_Version)->SetWindowText(sz_Name);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



///////////////////////////////////////////////////////////////////////////
// clList
BEGIN_MESSAGE_MAP(clList, CListCtrl)
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void clList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CRect oListRect;
    GetClientRect(oListRect);

    if(oListRect.PtInRect(point))
    {
        int iIndex;
        iIndex = HitTest(point);
        if (iIndex >= 0)
        {
            mpo_Parent->OpenReport(iIndex);
        }
    }
}






///////////////////////////////////////////////////////////////////////////
// CCerbereDlg : main dialog

CCerbereDlg::CCerbereDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCerbereDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_ulCurrentStatus = kul_FlagInitializing;
}

void CCerbereDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    ////{{AFX_DATA_MAP(LGT_cl_AddRemoveSectorsDlg)
    // main block
    DDX_Control(pDX, IDC_LIST_PC,          m_oList );

    ////}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCerbereDlg, CDialog)

    ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_REGISTERED_MESSAGE(CFileWatch::m_msgFileWatchNotify, OnFileWatchNotification)
    ON_REGISTERED_MESSAGE(CFileWatch::m_msgFileWatchDelete, OnFileWatchDelete)

    ON_COMMAND(ID_POPUP_MAXIMIZE,   OnMaximize)
    ON_COMMAND(ID_POPUP_MINIMIZE,   OnMinimize)
    ON_COMMAND(ID_POPUP_QUIT,       OnPopupQuit)
    ON_COMMAND(ID_POPUP_ABOUTBOX,   OnAboutbox)

END_MESSAGE_MAP()




int CCerbereDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create the tray icon
    if (!m_TrayIcon.Create(
        NULL,                            // Let icon deal with its own messages
        WM_ICON_NOTIFY,                  // Icon notify message to use
        _T(kaz_CERBERE_Name ": " kaz_CERBERE_GlobalStatusInitializing), // tool-tip
        m_hIcon,                         // tray icon used
        IDR_POPUP_MENU                   // popup menu id
        ))
    {
        return -1;
    }


    m_TrayIcon.SetMenuDefaultItem(3, TRUE);

    // show splash during init
    OpenSplashWnd();

    return 0;
}

void CCerbereDlg::OpenSplashWnd()
{
    m_oSplashBmp.LoadBitmap(IDB_SPLASH);
    m_oSplash.SetTransparentColor(RGB(255, 0, 255));
    m_oSplash.SetBitmap( m_oSplashBmp );
    m_oSplash.ShowSplash();
}

void CCerbereDlg::CloseSplashWnd()
{
    m_oSplash.CloseSplash();
}

BOOL CCerbereDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon



	// Extra initialization
    char szExeFileName[MAX_PATH];
    GetModuleFileName(NULL, szExeFileName, MAX_PATH);
    VREAD_VersionReader::bGetFileVersion(szExeFileName, m_szVer);



    // Init Cerbere
    mo_Cerbere.Init(m_hWnd);

    // Init timers
    SetTimer(kui_UpdateTimerID,   ki_UpdateTimerValue,   0);    // Watchdog frequency
    SetTimer(kui_IconAnimTimerID, ki_IconAnimTimerValue, 0);    // animated taskbar icon freq

    // Default animation list for task-bar icon
    SetIconGreen();

    // init the PC list
    InitList();

    // refresh
    UpdateDisplay();
    UpdatePCSettings();

    // end of init
    CloseSplashWnd();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CCerbereDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
        OnAboutbox();
    }
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCerbereDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CCerbereDlg::OnClose()
{
    int iRes = MessageBox("Sure to Quit?", kaz_CERBERE_Name, MB_YESNO  );

    if( IDYES == iRes)
    {
        m_TrayIcon.RemoveIcon();
        CDialog::OnClose();
    }
}


#define MINWNDSIZE_X    30
#define MINWNDSIZE_Y    50

void CCerbereDlg::OnSize(UINT nType, int cx, int cy)
{
    // min size
    cx = __max(cx, MINWNDSIZE_X);
    cy = __max(cy, MINWNDSIZE_Y);

    CDialog::OnSize(nType, cx, cy);

    switch(nType)
    {
        case SIZE_MAXIMIZED:
            OnMaximize();
            break;

        case SIZE_MINIMIZED:
            OnMinimize();
            return;
            //break;
    }


    // recompute listbox size
    CListCtrl * pList = (CListCtrl*)GetDlgItem(IDC_LIST_PC);

    if(pList)
    {
        CRect oListRect;

        m_oList.GetWindowRect(&oListRect);    
        ScreenToClient(&oListRect);

        oListRect.right  = cx - oListRect.left;
        oListRect.bottom = cy - oListRect.top;

        m_oList.MoveWindow(oListRect);
    }
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCerbereDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCerbereDlg::OnTimer(UINT nIDEvent)
{
    if(kui_UpdateTimerID == nIDEvent)
    {
        mo_Cerbere.RunOneFrame();
        UpdateDisplay();
    }
    else
    {
        m_TrayIcon.StepAnimation();
    }
}

void CCerbereDlg::OnMaximize()
{   
    m_TrayIcon.MaximiseFromTray(this);
    m_TrayIcon.SetMenuDefaultItem(ID_POPUP_MINIMIZE, FALSE);
}

void CCerbereDlg::OnMinimize()
{   
    m_TrayIcon.MinimiseToTray(this);
    m_TrayIcon.SetMenuDefaultItem(ID_POPUP_MAXIMIZE, FALSE);
}

void CCerbereDlg::OnPopupQuit()
{
    SendMessage(WM_CLOSE);
}

void CCerbereDlg::OnAboutbox()
{
    CAboutDlg dlgAbout;
    strcpy(dlgAbout.m_szVer, m_szVer) ;
    dlgAbout.DoModal();
}


//
LRESULT CCerbereDlg::OnFileWatchNotification(WPARAM wParam, LPARAM lParam)
{
    mo_Cerbere.OnFileWatchNotification( (LPCTSTR)lParam, (DWORD)wParam );

    if( E_FileTypeIni == (tde_FileType)HIWORD(wParam) )
    {   // update settings only when needed
        UpdatePCSettings();
    }

    return 0;
}

LRESULT CCerbereDlg::OnFileWatchDelete(WPARAM wParam, LPARAM lParam)
{
    mo_Cerbere.OnFileWatchDelete( (LPCTSTR)lParam, (DWORD)wParam );

    if( E_FileTypeIni == (tde_FileType)HIWORD(wParam) )
    {   // update settings only when needed
        UpdatePCSettings();
    }

    return 0;
}



//////////////////////////////////////////
void CCerbereDlg::OpenReport(int _iPCIndex)
{
    const cl_PCwatch& rPC = mo_Cerbere.roGetPC(_iPCIndex);

    if(rPC.m_oSettings.mb_Server)
    {
        char szFile[ki_MaxNameLength];
        if( rPC.m_oSettings.mb_MapTestingEnable || rPC.m_oSettings.mb_MapTestingXenonEnable )
        {   sprintf( szFile, kaz_DESDAC_RemoteHtmlMapFile, rPC.m_szPCname );    
        }
        else
        {   sprintf( szFile, kaz_DESDAC_RemoteHtmlFile, rPC.m_szPCname );    
        }

        // open Html file in external window (with default app)
        ShellExecute(0, _T("open"), szFile, 0, 0, SW_SHOWNORMAL);
    }
}

//
void CCerbereDlg::UpdateDisplay()
{
    UpdateGlobalStatus();
    UpdatePCStatus();
    UpdateTitle();
}

void CCerbereDlg::UpdateGlobalStatus()
{
    DWORD ulNewStatus = mo_Cerbere.ulGetGlobalStatus();
    if( m_ulCurrentStatus == ulNewStatus )    
        return; // no change: no need to go further

    // store new status
    m_ulCurrentStatus = ulNewStatus;  // store new status

    // update tooltip
    char sz_Status[ki_MaxNameLength];
    sprintf(sz_Status, kaz_CERBERE_Name ": %s", mo_Cerbere.szGlobalStatus() );
    m_TrayIcon.SetTooltipText( sz_Status );


    // update icon according to status
    if( ulNewStatus == 0  )
    {   
        // status ok
        SetIconGreen();
    }
    else
    {
        // status not ok: test different cases

        SetIconRed();
    }
}



void CCerbereDlg::UpdateTitle()
{
    char sz_WinTitle[ki_MaxNameLength];
    sprintf(sz_WinTitle, kaz_CERBERE_Name "   -   Status: %s", mo_Cerbere.szGlobalStatus() );
    SetWindowText(sz_WinTitle);
}


void CCerbereDlg::InitList()
{
    m_oList.SetParent(this);
    m_oList.DeleteAllItems();

    // create columns
    UINT k = 0;
    m_oColumns.mi_PCname = k;
    m_oList.InsertColumn(k++,   kaz_COLUMNTITLE_PCname,         LVCFMT_LEFT, ki_COLUMNDEFAULTWIDTH_PCname);

    m_oColumns.mi_TestType = k;
    m_oList.InsertColumn(k++,   kaz_COLUMNTITLE_TestType,       LVCFMT_LEFT, ki_COLUMNDEFAULTWIDTH_TestType);
    
    m_oColumns.mi_Status = k;
    m_oList.InsertColumn(k++,   kaz_COLUMNTITLE_Status,         LVCFMT_LEFT, ki_COLUMNDEFAULTWIDTH_Status);
    
    m_oColumns.mi_Results = k;
    m_oList.InsertColumn(k++,   kaz_COLUMNTITLE_Results,        LVCFMT_LEFT, ki_COLUMNDEFAULTWIDTH_Results);

    m_oColumns.mi_ProjectName = k;
    m_oList.InsertColumn(k++,   kaz_COLUMNTITLE_ProjectName,    LVCFMT_LEFT, ki_COLUMNDEFAULTWIDTH_ProjectName);

    m_oColumns.mi_EngineVersion = k;
    m_oList.InsertColumn(k++,   kaz_COLUMNTITLE_EngineVersion,  LVCFMT_LEFT, ki_COLUMNDEFAULTWIDTH_EngineVersion);


    // Create items
    UINT uiPCnb = mo_Cerbere.uiGetNbPC();
    m_oList.SetItemCount( uiPCnb );
    for ( UINT i = 0; i < uiPCnb; i++ )
    {
        const cl_PCwatch& rPC = mo_Cerbere.roGetPC(i);
        m_oList.InsertItem(i, rPC.m_szPCname);
    }
}

void CCerbereDlg::UpdatePCStatus()
{
    for ( UINT i = 0; i < mo_Cerbere.uiGetNbPC(); i++ )
    {
        const cl_PCwatch& rPC = mo_Cerbere.roGetPC(i);

        // Status
        char szStatus[ki_MaxNameLength];
        sprintf( szStatus, "%s%s", rPC.szStatus(), rPC.szActivity() );
        m_oList.SetItemText(i, m_oColumns.mi_Status, szStatus );

        
        // Data tests Results
        m_oList.SetItemText(i, m_oColumns.mi_Results, rPC.m_oSettings.mb_Server ? "_" : "" );
    }

    UpdateData(false);
}


void CCerbereDlg::UpdatePCSettings()
{
    for ( UINT i = 0; i < mo_Cerbere.uiGetNbPC(); i++ )
    {
        char szBuf[ki_MaxNameLength];

        const cl_PCwatch& rPC = mo_Cerbere.roGetPC(i);
        const cl_PCsettings& roSettings = rPC.m_oSettings;
        bool bNoSettings  = rPC.bIsFileDeleted(E_FileTypeIni);

        // Type of test performed
        if(roSettings.mb_Server)
        {
            if( roSettings.mb_MapTestingEnable )
            {   strcpy(szBuf, kaz_CERBERE_TESTMAP_PC);
            }
            else if( roSettings.mb_MapTestingXenonEnable )
            {   strcpy(szBuf, kaz_CERBERE_TESTMAP_XENON);
            }
            else
            {   strcpy(szBuf, kaz_CERBERE_TEST_IADB);
            }
        }
        else
        {   strcpy(szBuf, kaz_CERBERE_TEST_CLIENT);
        }

        m_oList.SetItemText(i, m_oColumns.mi_TestType, bNoSettings ? "" : szBuf );

        // Project tested
        m_oList.SetItemText(i, m_oColumns.mi_ProjectName, bNoSettings ? "" : roSettings.msz_ProjectName );
        

        // Engine version used for test
        sprintf(szBuf, "%d.%02d", roSettings.mi_EnginePCVersion, roSettings.mi_EnginePCPatch);
        m_oList.SetItemText(i, m_oColumns.mi_EngineVersion, bNoSettings ? "" : szBuf );
        
    }

    UpdateData(false);
}


