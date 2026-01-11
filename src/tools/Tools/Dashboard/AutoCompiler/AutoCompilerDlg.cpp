// AutoCompilerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompiler.h"
#include "AutoCompilerDlg.h"
#include "Dialog_ChangeString.h"
#include "DesdacSettings.h"
#include "BigFileUtils.h"
#include ".\autocompilerdlg.h"
#include "MapTestingConfig.h"
#include "hangeNumbersDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoCompilerDlg dialog



CAutoCompilerDlg::CAutoCompilerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoCompilerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoCompilerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAutoCompilerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_CLOSE()

    ON_BN_CLICKED(IDC_BUTTON_Suspend, OnBnClickedButtonSuspend)
    ON_BN_CLICKED(IDC_BUTTON_CHANGESETTINGS, OnBnClickedButtonChangesettings)
    ON_BN_CLICKED(IDC_BUTTON_ForceUpdateVersion, OnBnClickedButtonForceupdateversion)
    ON_BN_CLICKED(IDC_CHECK_MapTestingEnable, OnBnClickedCheckMaptestingEnable)
    ON_BN_CLICKED(IDC_BUTTON_MapTestingConfiguration, OnBnClickedButtonMaptestingconfiguration)
    ON_BN_CLICKED(IDC_CHECK_MapTestingXenonEnable, OnBnClickedCheckMaptestingXenonEnable)
    ON_BN_CLICKED(IDC_CHECK_SendMails, OnBnClickedCheckSendmails)
    ON_BN_CLICKED(IDC_CHECK_SendMailsUser, OnBnClickedCheckSendmailsuser)
    ON_BN_CLICKED(IDC_CHECK_TestLastCLonly, OnBnClickedCheckTestlastCLonly)	
	ON_BN_CLICKED(IDC_CHECK_AITOC, OnBnClickedCheckAitoc)
END_MESSAGE_MAP()


// CAutoCompilerDlg message handlers

BOOL CAutoCompilerDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
    SetTimer(1, ki_DESDAC_TimerValue, 0);	
    SetWindowText(kaz_MainWindowName " : running");	
    UpdateDisplay();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAutoCompilerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoCompilerDlg::OnPaint() 
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoCompilerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoCompilerDlg::OnTimer(UINT nIDEvent) 
{	
    mo_Desdac.RunOneFrame();
    UpdateDisplay();
}



void CAutoCompilerDlg::OnBnClickedButtonServerport()
{
    Suspend();
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(BIG_pz_GetPerforceServerPort());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        BIG_SetPerforceServerPort(az_NewString);
        ((CButton*)GetDlgItem(IDC_BUTTON_ServerPort  ))->SetWindowText(BIG_pz_GetPerforceServerPort()     );
    }
}

void CAutoCompilerDlg::OnBnClickedButtonServerbranch()
{
    Suspend();
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(BIG_pz_GetPerforceDepotBranch());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        BIG_SetPerforceDepotBranch(az_NewString);
        ((CButton*)GetDlgItem(IDC_BUTTON_ServerBranch))->SetWindowText(BIG_pz_GetPerforceDepotBranch()    );
    }
}

void CAutoCompilerDlg::OnBnClickedButtonUsername()
{
    Suspend();
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(BIG_pz_GetPerforceCurrentUser());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        BIG_SetPerforceCurrentUser(az_NewString);
        ((CButton*)GetDlgItem(IDC_BUTTON_UserName    ))->SetWindowText(BIG_pz_GetPerforceCurrentUser()    );
    }
}

void CAutoCompilerDlg::OnBnClickedButtonSuspend()
{
    bool bSuspended = mo_Desdac.bGetSuspendedStatus();
    if ( bSuspended )
    {
        mo_Desdac.Suspend(false);
        GetDlgItem(IDC_BUTTON_Suspend)->SetWindowText("Suspend");
        SetWindowText(kaz_MainWindowName " : running");
    }
    else
    {
        mo_Desdac.Suspend(true);
        GetDlgItem(IDC_BUTTON_Suspend)->SetWindowText("Resume");
        SetWindowText(kaz_MainWindowName " : suspended");
    }
}

void CAutoCompilerDlg::Suspend()
{
    mo_Desdac.Suspend(true);
    GetDlgItem(IDC_BUTTON_Suspend)->SetWindowText("Resume");
    SetWindowText(kaz_MainWindowName " : suspended");
}

void CAutoCompilerDlg::UpdateDisplay()
{
    ////////////////////
    // CHANGE LIST STACK
    bool bIsServer = mo_Desdac.bIsServer();

    BOOL bEnableWindow = bIsServer ? TRUE : FALSE;
    GetDlgItem(IDC_STATIC_NbClInStack           )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NextCl                )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_LastSubmittedCl       )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NbClInStackValue      )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NextClValue           )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_LastSubmittedClValue  )->EnableWindow(bEnableWindow);

    char sz_Tmp[15];

    // First
    int iCL = mo_Desdac.iGetClInTopOfStack();
    bool bFirstCL = ( iCL != -1 );
    if ( bFirstCL )
        sprintf(sz_Tmp, "%d", iCL );
    else
        sprintf(sz_Tmp, "NONE");

    GetDlgItem(IDC_STATIC_NextClValue)->SetWindowText(sz_Tmp);


    // Last
    iCL = mo_Desdac.iGetLastClInStack();
    bool bLastCL = ( iCL != -1 );
    if ( bLastCL )
        sprintf(sz_Tmp, "%d", iCL );
    else
        sprintf(sz_Tmp, "NONE");

    GetDlgItem(IDC_STATIC_LastSubmittedClValue)->SetWindowText(sz_Tmp);
    
    // Nb
    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbClOnStack());
    GetDlgItem(IDC_STATIC_NbClInStackValue)->SetWindowText(sz_Tmp);

    // Next to test
    bool bTestLastCLonly =  mo_Desdac.bGetTestLastClonly();
    ((CButton*)GetDlgItem(IDC_CHECK_TestLastCLonly))->SetCheck(bTestLastCLonly ? BST_CHECKED : BST_UNCHECKED);
    GetDlgItem(IDC_STATIC_TestFirstCl)->ShowWindow( (bIsServer && bFirstCL && !bTestLastCLonly) ? SW_SHOW : SW_HIDE );
    GetDlgItem(IDC_STATIC_TestLastCl)-> ShowWindow( (bIsServer && bLastCL  &&  bTestLastCLonly) ? SW_SHOW : SW_HIDE );


    ////////////////////////////
    // CHANGE LIST RESULTS STACK
    bEnableWindow = mo_Desdac.iGetLastClResult() != -1 ? TRUE : FALSE;
    GetDlgItem(IDC_STATIC_ResultGroup           )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_LastCL                )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_LastCLValue           )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_Owner                 )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_OwnerValue            )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_AiResult              )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_AiResultValue         )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_DbResult              )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_DbResultValue         )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_MapTestResult         )->EnableWindow(bEnableWindow && mo_Desdac.bGetGlobalMapTesting()  );
    GetDlgItem(IDC_STATIC_MapTestResultValue    )->EnableWindow(bEnableWindow && mo_Desdac.bGetGlobalMapTesting()  );
    GetDlgItem(IDC_STATIC_GlobalResult          )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_GlobalResultValue     )->EnableWindow(bEnableWindow);
        

    if ( bEnableWindow )
    {
        sprintf(sz_Tmp, "%d", mo_Desdac.iGetLastClResult());
        GetDlgItem(IDC_STATIC_LastCLValue)->SetWindowText(sz_Tmp);
        GetDlgItem(IDC_STATIC_OwnerValue )->SetWindowText(mo_Desdac.pz_GetLastClResultOwner());

        // IA
        if ( mo_Desdac.bGetLastClResultAiFailed() )
            GetDlgItem(IDC_STATIC_AiResultValue)->SetWindowText("FAILED");
        else
            GetDlgItem(IDC_STATIC_AiResultValue)->SetWindowText("OK");

        // DB
        if ( mo_Desdac.bGetLastClResultDbFailed() )
            GetDlgItem(IDC_STATIC_DbResultValue)->SetWindowText("FAILED");
        else
            GetDlgItem(IDC_STATIC_DbResultValue)->SetWindowText("OK");

        // Map testing
        if( mo_Desdac.bGetGlobalMapTesting() )
        {
            if ( mo_Desdac.bGetLastClResultMapTestFailed() )
                GetDlgItem(IDC_STATIC_MapTestResultValue)->SetWindowText("FAILED");
            else
                GetDlgItem(IDC_STATIC_MapTestResultValue)->SetWindowText("OK");
        }
        else
        {   GetDlgItem(IDC_STATIC_MapTestResultValue)->SetWindowText("N/A");
        }

        // Global Status
        if ( mo_Desdac.bGetLastClResultGlobalFailed() )
            GetDlgItem(IDC_STATIC_GlobalResultValue)->SetWindowText("FAILED");
        else
            GetDlgItem(IDC_STATIC_GlobalResultValue)->SetWindowText("OK");

    }

    ////////////////
    // CLIENTS INFOS
    bEnableWindow = bIsServer ? TRUE : FALSE;
    GetDlgItem(IDC_STATIC_CLIENTSINFOS             )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBCLIENTS                )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBCLIENTSVALUE           )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBRUNNINGCLIENTS         )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBRUNNINGCLIENTSVALUE    )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBUPDATINGCLIENTS        )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBUPDATINGCLIENTSVALUE   )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBIDDLECLIENTS           )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBIDDLECLIENTSVALUE      )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBDOWNCLIENTS            )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBDOWNCLIENTSVALUE       )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBNOTLAUNCHEDCLIENTS     )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBNOTLAUNCHEDCLIENTSVALUE)->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBSUSPENDEDCLIENTS       )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NBSUSPENDEDCLIENTSVALUE  )->EnableWindow(bEnableWindow);

    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbClients());
    GetDlgItem(IDC_STATIC_NBCLIENTSVALUE           )->SetWindowText(sz_Tmp);

    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbBusyClients());
    GetDlgItem(IDC_STATIC_NBRUNNINGCLIENTSVALUE    )->SetWindowText(sz_Tmp);

    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbUpdatingClients());
    GetDlgItem(IDC_STATIC_NBUPDATINGCLIENTSVALUE   )->SetWindowText(sz_Tmp);

    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbDownClients());
    GetDlgItem(IDC_STATIC_NBDOWNCLIENTSVALUE       )->SetWindowText(sz_Tmp);

    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbNotLaunchedClients());
    GetDlgItem(IDC_STATIC_NBNOTLAUNCHEDCLIENTSVALUE)->SetWindowText(sz_Tmp);

    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbSuspendedClients());
    GetDlgItem(IDC_STATIC_NBSUSPENDEDCLIENTSVALUE)->SetWindowText(sz_Tmp);

    int iNbIddle = mo_Desdac.iGetNbClients() - mo_Desdac.iGetNbBusyClients() - mo_Desdac.iGetNbDownClients() - mo_Desdac.iGetNbNotLaunchedClients() - mo_Desdac.iGetNbSuspendedClients() - mo_Desdac.iGetNbUpdatingClients();
    sprintf(sz_Tmp, "%d", iNbIddle);
    GetDlgItem(IDC_STATIC_NBIDDLECLIENTSVALUE      )->SetWindowText(sz_Tmp);


    ////////
    // MAILS
    GetDlgItem(IDC_CHECK_SendMails)->EnableWindow(bEnableWindow);
    ((CButton*)GetDlgItem(IDC_CHECK_SendMails))->SetCheck(mo_Desdac.bGetSendMails() ? BST_CHECKED : BST_UNCHECKED);
    GetDlgItem(IDC_CHECK_SendMailsUser)->EnableWindow(bEnableWindow & BOOL(mo_Desdac.bGetSendMails()) );
    ((CButton*)GetDlgItem(IDC_CHECK_SendMailsUser))->SetCheck(mo_Desdac.bGetSendMailsUser() ? BST_CHECKED : BST_UNCHECKED);


    ////////////////////////////
    // VERSION/PATCH/LAST UPDATE
    bGetExeVersion(sz_Tmp);
    GetDlgItem(IDC_STATIC_DesdacVersion)->SetWindowText(sz_Tmp);
    sprintf(sz_Tmp, "%d.%d", mo_Desdac.iGetYetiVersion(), mo_Desdac.iGetYetiPatch());
    GetDlgItem(IDC_STATIC_PatchValue)->SetWindowText(sz_Tmp);
    char sz_LastUpdate[64];
    sprintf(sz_LastUpdate, "%02d/%02d/%04d at %02d:%02d:%02d", 
            mo_Desdac.iGetYetiLastUpdateDay(), mo_Desdac.iGetYetiLastUpdateMonth(), mo_Desdac.iGetYetiLastUpdateYear(),
            mo_Desdac.iGetYetiLastUpdateHour(), mo_Desdac.iGetYetiLastUpdateMinute(), mo_Desdac.iGetYetiLastUpdateSecond());
    GetDlgItem(IDC_STATIC_LastUpdatedValue)->SetWindowText(sz_LastUpdate);
    bEnableWindow = bIsServer ? TRUE : FALSE;
    GetDlgItem(IDC_BUTTON_ForceUpdateVersion)->EnableWindow(bEnableWindow);


    //////////////////////
    // CHANGE LIST RESULTS
    if ( BIG_iGetLastClTested() != -1 )
    {
        char az_ChangeList[256];
        sprintf(az_ChangeList, "%d", BIG_iGetLastClTested() );
        ((CStatic*)GetDlgItem(IDC_STATIC_LastCLValue))->SetWindowText(az_ChangeList);
        ((CStatic*)GetDlgItem(IDC_STATIC_OwnerValue ))->SetWindowText(BIG_pz_GetLastUser());

        if ( BIG_bGetAiResultStatus() )
            ((CStatic*)GetDlgItem(IDC_STATIC_AiResultValue))->SetWindowText("FAILED");
        else
            ((CStatic*)GetDlgItem(IDC_STATIC_AiResultValue))->SetWindowText("OK");

        if ( BIG_bGetDatabaseCheckStatus() )
            ((CStatic*)GetDlgItem(IDC_STATIC_DbResultValue))->SetWindowText("FAILED");
        else
            ((CStatic*)GetDlgItem(IDC_STATIC_DbResultValue))->SetWindowText("OK");

        if ( BIG_bGetGlobalResultStatus() )
            ((CStatic*)GetDlgItem(IDC_STATIC_GlobalResultValue))->SetWindowText("FAILED");
        else
            ((CStatic*)GetDlgItem(IDC_STATIC_GlobalResultValue))->SetWindowText("OK");
    }


    /////////////////////
    // STATUS INFORMATION
    if ( mo_Desdac.bIsYetiRunning() )
    {
        char sz_Caption[256];
        sprintf(sz_Caption, "TESTING CL %d, by %s", mo_Desdac.iGetCurrentCl(), mo_Desdac.pz_GetCurrentClOwner());
        
        if( mo_Desdac.bIsCopyToXENONrunning() )
        {
            char szCopying[256];
            sprintf(szCopying, " : Xenon copy on %s (%2.1f%%)", mo_Desdac.szGetXenonKitName(), mo_Desdac.fCopyProgress() );
            strcat( sz_Caption, szCopying );
        }
        else if( mo_Desdac.bIsYetiXENONrunning() )
        {
            strcat( sz_Caption, " : Xenon run on " );
            strcat( sz_Caption, mo_Desdac.szGetXenonKitName() ); 
        }

        ((CStatic*)GetDlgItem(IDC_STATIC_Working))->SetWindowText(sz_Caption);
    }
    else
    {
        ((CStatic*)GetDlgItem(IDC_STATIC_Working))->SetWindowText("WAITING");
    }


    //////////////
    // MAP TESTING
    ((CButton*)GetDlgItem(IDC_CHECK_MapTestingEnable))->SetCheck(mo_Desdac.bGetMapTestingPC() ? BST_CHECKED : BST_UNCHECKED);
    ((CButton*)GetDlgItem(IDC_CHECK_MapTestingXenonEnable))->SetCheck(mo_Desdac.bGetMapTestingXenon() ? BST_CHECKED : BST_UNCHECKED);
    sprintf(sz_Tmp, "%d", mo_Desdac.iGetNbMapToTest());
    GetDlgItem(IDC_STATIC_NbMapToTestValue)->SetWindowText(sz_Tmp);
    
    bEnableWindow = bIsServer ? TRUE : FALSE;
    GetDlgItem(IDC_CHECK_MapTestingEnable     )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_CHECK_MapTestingXenonEnable)->EnableWindow(bEnableWindow); 

    bEnableWindow = ( mo_Desdac.bGetMapTestingPC() || mo_Desdac.bGetMapTestingXenon() ) ? TRUE : FALSE;
    GetDlgItem(IDC_STATIC_NbMapToTest     )->EnableWindow(bEnableWindow);
    GetDlgItem(IDC_STATIC_NbMapToTestValue)->EnableWindow(bEnableWindow);   // nb of maps to test


    //////////////
    // Client list
    UpdateClientDisplay();
}


void CAutoCompilerDlg::OnClose()
{
    mo_Desdac.SaveInfoInIniFile();
    CDialog::OnClose();
}

void CAutoCompilerDlg::OnBnClickedButtonChangesettings()
{
    // save status
    bool bWasRunningStatus = ! mo_Desdac.bGetSuspendedStatus();

    // Suspend server
    Suspend();

    // Open dialog
    CDesdacSettings     ChangeSettings;
    ChangeSettings.SetDesdac(&mo_Desdac);

    if ( ChangeSettings.DoModal() == IDOK )
    {
        mo_Desdac.SaveInfoInIniFile();
    }

    // restore suspended/running status if needed
    if(bWasRunningStatus)
    {   OnBnClickedButtonSuspend();
    }
}


void CAutoCompilerDlg::OnBnClickedButtonForceupdateversion()
{
    // Only server can update version on all clients

    // Update list and status of clients
    mo_Desdac.UpdateListOfAwareClients();
    mo_Desdac.UpdateListOfAvailableClients();

    //Check if an update request isn't already pending
    int iPendingUpdateNb = mo_Desdac.iGetNbOfPendingUpdateRequests();
    if(iPendingUpdateNb > 0)
    {
        MessageBox("An auto-update is already activated", "Auto-Update Version", MB_OK);
        return;
    }

    // ask for new version number
    ChangeNumbersDlg oChangeNumbers;
    oChangeNumbers.m_iVersion   = mo_Desdac.iGetYetiVersion();
    oChangeNumbers.m_iPatch     = mo_Desdac.iGetYetiPatch();

    if ( IDOK == oChangeNumbers.DoModal() )
    {
        unsigned long uiUpdateTypeFlags = mo_Desdac.bGetMapTestingXenon() ? (kuiEnginePCUpdate | kuiEngineXenonUpdate) : (kuiEnginePCUpdate);
        mo_Desdac.SendUpdateEngineVersionRequest(uiUpdateTypeFlags, oChangeNumbers.m_iVersion, oChangeNumbers.m_iPatch, 0);
        UpdateDisplay();
    }
    else
    {
        MessageBox("Update Engine Canceled", "Update Version", MB_OK);
    }
}

void CAutoCompilerDlg::OnBnClickedCheckMaptestingEnable()
{
    CButton * pButton = (CButton*)GetDlgItem(IDC_CHECK_MapTestingEnable);
    mo_Desdac.SetMapTesting(pButton->GetCheck() ? true : false);
    UpdateDisplay();
}

void CAutoCompilerDlg::OnBnClickedButtonMaptestingconfiguration()
{
    // save status
    bool bWasRunningStatus = ! mo_Desdac.bGetSuspendedStatus();

    // Suspend server
    Suspend();

    // Open dialog
    CMapTestingConfig oMapTesting;
    oMapTesting.SetDesdac(&mo_Desdac);

    if ( oMapTesting.DoModal() == IDOK )
    {
    }

    // restore suspended/running status if needed
    if(bWasRunningStatus)
    {   OnBnClickedButtonSuspend();
    }
}

void CAutoCompilerDlg::OnBnClickedCheckMaptestingXenonEnable()
{
    CButton * pButton = (CButton*)GetDlgItem(IDC_CHECK_MapTestingXenonEnable);
    mo_Desdac.SetMapTestingXenon(pButton->GetCheck() ? true : false);
    UpdateDisplay();
}

void CAutoCompilerDlg::OnBnClickedCheckTestlastCLonly()
{
    CButton * pButton = (CButton*)GetDlgItem(IDC_CHECK_TestLastCLonly);
    mo_Desdac.SetTestLastClonly(pButton->GetCheck() ? true : false);
    UpdateDisplay();
}


void CAutoCompilerDlg::OnBnClickedCheckSendmails()
{
    CButton * pButton = (CButton*)GetDlgItem(IDC_CHECK_SendMails);
    mo_Desdac.SetSendMails(pButton->GetCheck() ? true : false);
    UpdateDisplay();
}

void CAutoCompilerDlg::OnBnClickedCheckSendmailsuser()
{
    CButton * pButton = (CButton*)GetDlgItem(IDC_CHECK_SendMailsUser);
    mo_Desdac.SetSendMailsUser(pButton->GetCheck() ? true : false);
    UpdateDisplay();
}

void CAutoCompilerDlg::UpdateClientDisplay()
{
    CListBox * pList = (CListBox*)GetDlgItem(IDC_LIST_CLIENTS);
    pList->ResetContent();

    if( mo_Desdac.bIsServer() )
    {   // serveur : display clients
        // update client status
        //    mo_Desdac.UpdateListOfAwareClients();
        //    mo_Desdac.UpdateListOfAvailableClients();

        // List of all clients
        for ( int i = 0; i < mo_Desdac.iGetNbClients(); ++i )
        {
            char sz_ClientInfos[256];
            mo_Desdac.GetClientStatInfos(i, sz_ClientInfos);

            char sz_ClientAndStatus[1024];
            sprintf(sz_ClientAndStatus, "%02d : %s (%s) %s", i, mo_Desdac.pz_GetClient(i), mo_Desdac.pz_GetClientStatus(i), sz_ClientInfos);
            pList->AddString(sz_ClientAndStatus);
        }
    }
    else
    {   // client
        char sz_ServerName[256];
        sprintf(sz_ServerName, "(Server is %s)", mo_Desdac.pz_GetPcServer() );
        pList->AddString(sz_ServerName);
        pList->EnableWindow(FALSE);
    }
}

void CAutoCompilerDlg::OnBnClickedCheckAitoc()
{
    CButton * pButton = (CButton*)GetDlgItem(IDC_CHECK_AITOC);
    mo_Desdac.SetAiToCTesting(pButton->GetCheck() ? true : false);
    UpdateDisplay();
}
