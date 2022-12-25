// SynchronizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Synchronize.h"
#include "SynchronizeDlg.h"
#include ".\synchronizedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define C_STATE  0
#define C_NAME   1
#define C_SIZE   2
#define C_WDATE  3
#define C_PATH   4

#define M_GetBuffer(a) a.GetBuffer(a.GetLength())


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


// CSynchronizeDlg dialog



CSynchronizeDlg::CSynchronizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSynchronizeDlg::IDD, pParent)
{
    char az_Init[1024];

    GetCurrentDirectory(1024, (LPTSTR)az_Init);
    mo_SyncFileName = CString(az_Init) + CString("\\") + CString(AfxGetAppName()) + CString(".syn") ;

    mo_InPath=CString("");
    mo_RefPath=CString("");

    mo_ExecSame=CString("");
    mo_ExecNew=CString("");
    mo_ExecModif=CString("");
    mo_ExecDel=CString("");

    mpo_RefRoot = NULL;
    mpo_InRoot = NULL;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSynchronizeDlg::~CSynchronizeDlg(void)
{
    if(mpo_RefRoot) delete mpo_RefRoot ;
    if(mpo_InRoot) delete mpo_InRoot ;
}

void CSynchronizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSynchronizeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BTN_IN, OnBnClickedBtnIn)
    ON_BN_CLICKED(IDC_BTN_REF, OnBnClickedBtnRef)
    ON_BN_CLICKED(IDC_BTN_ONDEL, OnBnClickedBtnDel)
    ON_BN_CLICKED(IDC_BTN_ONSAME, OnBnClickedBtnOnsame)
    ON_BN_CLICKED(IDC_BTN_ONMODIF, OnBnClickedBtnOnmodif)
    ON_BN_CLICKED(IDC_BTN_ONNEW, OnBnClickedBtnOnnew)
    ON_BN_CLICKED(IDC_BTN_SYNC, OnBnClickedBtnSync)
    ON_BN_CLICKED(IDC_BTN_SYNCSEL, OnBnClickedBtnSyncsel)
    ON_BN_CLICKED(IDC_BTN_SELALL, OnBnClickedBtnSelall)
    ON_BN_CLICKED(IDC_BTN_SELNONE, OnBnClickedBtnSelnone)
    ON_BN_CLICKED(IDC_BTN_REVERSESEL, OnBnClickedBtnReversesel)
    ON_BN_CLICKED(ID_BTN_OPEN, OnBnClickedBtnOpen)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(ID_BTN_QUIT, OnBnClickedBtnQuit)
    ON_BN_CLICKED(IDC_BTN_OPEN2, OnBnClickedBtnOpen2)
	ON_BN_CLICKED(IDC_BTN_SYNCSEL2, OnBnClickedBtnSyncsel2)
	ON_BN_CLICKED(IDC_BTN_SYNCSEL3, OnBnClickedBtnSyncsel3)
END_MESSAGE_MAP()


// CSynchronizeDlg message handlers

BOOL CSynchronizeDlg::OnInitDialog()
{
    CListCtrl *po_List; 
    CBitmap		o_Bmp;

   
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
    po_List = (CListCtrl *) GetDlgItem(IDC_LIST);
    po_List->SetExtendedStyle(LVS_EX_FULLROWSELECT);

	mo_ImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 10);
	o_Bmp.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP1));
	mo_ImageList.Add(&o_Bmp, RGB(192, 192, 192));
	po_List->SetImageList(&mo_ImageList, LVSIL_SMALL);

    po_List->InsertColumn(C_STATE, "State", LVCFMT_LEFT, 40);
    po_List->InsertColumn(C_NAME, "Name", LVCFMT_LEFT, 150);
    po_List->InsertColumn(C_SIZE, "Size", LVCFMT_LEFT, 60);
    po_List->InsertColumn(C_WDATE, "Last write", LVCFMT_LEFT, 120);
    po_List->InsertColumn(C_PATH, "Relative path", LVCFMT_LEFT, 250);

    CFileFind    o_Finder;
    if(o_Finder.FindFile(LPCTSTR(mo_SyncFileName)))
    {
        LoadSyncFile(mo_SyncFileName);
    }
    
    GetDlgItem(IDC_BTN_SYNC)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SYNCSEL)->EnableWindow(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSynchronizeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == SC_CLOSE)
	{
		OnOK();
	}
	else if ((nID & 0xFFF0) == IDM_ABOUTBOX)
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

void CSynchronizeDlg::OnPaint() 
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
HCURSOR CSynchronizeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CSynchronizeDlg::GetDispFilter(void)
{
    int iFlag=0;

    iFlag |= 1<<CMyFile::Diff_NotCompared;

    if(((CButton*)GetDlgItem(IDC_CK_UNCHANGED))->GetCheck())
        iFlag |= 1<<CMyFile::Diff_Unchanged;

    if(((CButton*)GetDlgItem(IDC_CK_MODIFIED))->GetCheck())
        iFlag |= 1<<CMyFile::Diff_Modified;

    if(((CButton*)GetDlgItem(IDC_CK_DELETED))->GetCheck())
        iFlag |= 1<<CMyFile::Diff_Deleted;

    if(((CButton*)GetDlgItem(IDC_CK_NEW))->GetCheck())
        iFlag |= 1<<CMyFile::Diff_New;

    return iFlag;
}

CMyDir* ParseDirectory(LPCTSTR asz_Path, CMyDir*po_Father);

void CSynchronizeDlg::OnBnClickedButton1()
{
    CString      o_Line;
    CListCtrl *po_List;

    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    po_List = (CListCtrl *) GetDlgItem(IDC_LIST);
    po_List->SetRedraw(FALSE);
    po_List->DeleteAllItems();

    if(mpo_RefRoot) delete mpo_RefRoot;    
    mpo_RefRoot = NULL;

    if(mpo_InRoot) delete mpo_InRoot;    
    mpo_InRoot = NULL;

    GetDlgItem(IDC_ED_INPUT)->GetWindowText(mo_InPath);
    GetDlgItem(IDC_ED_REFERENCE)->GetWindowText(mo_RefPath);

    
    mpo_RefRoot = ParseDirectory(M_GetBuffer(mo_RefPath), NULL);
    mpo_InRoot = ParseDirectory(M_GetBuffer(mo_InPath), NULL);
    
    if(mpo_RefRoot && mpo_InRoot)
    {
        mpo_RefRoot->Compare(mpo_InRoot);
        mpo_RefRoot->DisplayDir(po_List, GetDispFilter());
    }

    po_List->SetRedraw(TRUE);
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

    GetDlgItem(IDC_BTN_SYNC)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_SYNCSEL)->EnableWindow(TRUE);
}


CMyDir* ParseDirectory(LPCTSTR asz_Path, CMyDir*po_Father)
{
    CFileFind    o_Finder;
    CMyDir       *po_Root;
    BOOL         bWorking ;
    CString o_Name;
    CTime o_Time;
    unsigned int uiSize ;
    CString o_CompletePath;

    po_Root = po_Father;

    if(po_Root)
    {
        CMyDir*po_Father;
        
        o_CompletePath = po_Root->mo_Name + CString("\\*.*");
        po_Father = po_Root->mp_Father;
        while(po_Father)
        {
            o_CompletePath = po_Father->mo_Name + CString("\\") + o_CompletePath ;
            po_Father = po_Father->mp_Father;
        }

    }
    else
    {
        o_CompletePath = CString(asz_Path) + CString("\\*.*");
    }

    if(!po_Root) 
    {
        po_Root = new CMyDir;
        po_Root->mo_Name = CString(asz_Path) ;
        po_Root->mi_DiffState = CMyFile::Diff_Modified;
    }

    bWorking = o_Finder.FindFile(M_GetBuffer(o_CompletePath));
    while (bWorking)
    {
        bWorking = o_Finder.FindNextFile();

        if(o_Finder.IsHidden()) continue;
        if(o_Finder.IsSystem()) continue;
        if(o_Finder.IsDots()) continue;
        if(o_Finder.IsTemporary()) continue;
        

        o_Name = o_Finder.GetFileName();
        o_Finder.GetLastWriteTime(o_Time);
        uiSize =(unsigned int) (o_Finder.GetLength() & 0x00000000FFFFFFFF);

        if(o_Finder.IsDirectory())
        {
            CMyDir *poDir;

            poDir = new CMyDir(o_Name, o_Time);
            po_Root->AddSubdirChild(poDir);
            ParseDirectory(NULL, poDir);
        }
        else
        {
            CMyFile*poFile;
            poFile = new CMyFile(o_Name, o_Time, uiSize);
            po_Root->AddFileChild(poFile);
        }
    }

    return po_Root;
}



////////////////////////////////////////////////////////////////////////

CMyFile::CMyFile(void)
{
    mo_Name = CString("");
    mo_LastWriteTime = CTime(2000, 01, 01, 0, 0, 0);
    mp_Father = NULL;
    mui_Size = 0;
    mi_DiffState=Diff_NotCompared;
}

CMyFile::CMyFile(CString&oName, CTime&oTime, unsigned int uiSize)
{
    mo_Name = oName;
    mo_LastWriteTime = oTime;
    mp_Father = NULL;
    mui_Size = uiSize;
    mi_DiffState=Diff_NotCompared;
}

CMyFile::~CMyFile(void)
{
    mp_Father = NULL;
}

void CMyFile::GetCompletePath(CString & o_Complete)
{
    CString oTmp;
    CMyDir*po_Father;

    o_Complete = mo_Name;
    po_Father = mp_Father;

    while(po_Father)
    {
        oTmp = po_Father->mo_Name + CString("\\") + o_Complete ;
        o_Complete = oTmp;
        po_Father = po_Father->mp_Father;
    }
}

void CMyFile::GetRelativePath(CString & o_Complete)
{
    CString oTmp;
    CMyDir*po_Father;

    o_Complete = mo_Name;
    po_Father = mp_Father;

    while(po_Father && po_Father->mp_Father)
    {
        oTmp = po_Father->mo_Name + CString("\\") + o_Complete ;
        o_Complete = oTmp;
        po_Father = po_Father->mp_Father;
    }
}

int CMyFile::operator==(const CMyFile & oFile) 
{ 
    if(mo_Name!=oFile.mo_Name) return 0;
    if(mo_LastWriteTime!=oFile.mo_LastWriteTime) return 0;
    if(mui_Size!=oFile.mui_Size) return 0;
    return 1;
}

void CMyFile::Synchronize(CString&o_ExecUnchanged,CString&o_ExecModified,CString&o_ExecNew,CString&o_ExecDeleted)
{
    CString o_Complete;
    CString o_Command;
    
    GetRelativePath(o_Complete);
    o_Complete = CString(" \"") + o_Complete + CString("\"");
    o_Command = CString("");
    

    if((o_ExecUnchanged != CString("")) && (mi_DiffState == Diff_Unchanged) )
        o_Command = o_ExecUnchanged + CString(" file ") + o_Complete;	    
    else if((o_ExecModified != CString("")) && (mi_DiffState == Diff_Modified) )
        o_Command = o_ExecModified + CString(" file ") + o_Complete;	    
    else if((o_ExecNew != CString("")) && (mi_DiffState == Diff_New) )
        o_Command = o_ExecNew + CString(" file ") + o_Complete;	    
    else if((o_ExecDeleted != CString("")) && (mi_DiffState == Diff_Deleted) )
        o_Command = o_ExecDeleted + CString(" file ") + o_Complete;	    
    else
        return;

    if(o_Command != CString(""))
        system(M_GetBuffer(o_Command));

    mi_DiffState = Diff_NotCompared;
}

///////////////////////////////////////////////////////////////////////
CMyDir::CMyDir(void) : CMyFile()
{
    mi_ChildrenDirNb = 0;
    mp_ChildrenDirList = NULL;
    mi_ChildrenFileNb = 0;
    mp_ChildrenFileList = NULL;
}

CMyDir::CMyDir(CString&oName, CTime&oTime) : CMyFile(oName, oTime)
{
    mp_Father = NULL;
    mi_ChildrenDirNb = 0;
    mp_ChildrenDirList = NULL;
    mi_ChildrenFileNb = 0;
    mp_ChildrenFileList = NULL;
}

CMyDir::CMyDir(CMyDir*pDir) : CMyFile(pDir->mo_Name, pDir->mo_LastWriteTime)
{
    int     i;

    mp_Father = NULL;

    mi_ChildrenDirNb = pDir->mi_ChildrenDirNb;
    mp_ChildrenDirList = new LPCMyDir[mi_ChildrenDirNb];
    for(i=0; i<mi_ChildrenDirNb ; i++)
    {
        mp_ChildrenDirList[i] = new CMyDir(pDir->mp_ChildrenDirList[i]);
        mp_ChildrenDirList[i]->mi_DiffState= Diff_New;
        mp_ChildrenDirList[i]->mp_Father = this;
    }

    mi_ChildrenFileNb = pDir->mi_ChildrenFileNb ;
    mp_ChildrenFileList = NULL;
    mp_ChildrenFileList= new LPCMyFile[mi_ChildrenFileNb];
    for(i=0; i<mi_ChildrenFileNb; i++)
    {
        mp_ChildrenFileList[i] = new CMyFile(pDir->mp_ChildrenFileList[i]->mo_Name, pDir->mp_ChildrenFileList[i]->mo_LastWriteTime, pDir->mp_ChildrenFileList[i]->mui_Size);
        mp_ChildrenFileList[i]->mi_DiffState = Diff_New;
        mp_ChildrenFileList[i]->mp_Father = this;
    }
}

CMyDir::~CMyDir(void)
{
    int i;
    mp_Father = NULL;
    
    for(i=0; i<mi_ChildrenDirNb; i++)
        if(mp_ChildrenDirList[i]) delete mp_ChildrenDirList[i];
    
    if(mp_ChildrenDirList) delete mp_ChildrenDirList;
    mp_ChildrenDirList = NULL;
    
    for(i=0; i<mi_ChildrenFileNb; i++)
        if(mp_ChildrenFileList[i]) delete mp_ChildrenFileList[i];

    if(mp_ChildrenFileList) delete mp_ChildrenFileList;
    mp_ChildrenFileList = NULL;
}

void CMyDir::Synchronize(CString&o_ExecUnchanged,CString&o_ExecModified,CString&o_ExecNew,CString&o_ExecDeleted)
{
    int i;

    CString o_Complete;
    CString o_Command;
    
    GetRelativePath(o_Complete);
    o_Complete = CString(" \"") + o_Complete + CString("\"");
    o_Command = CString("");
    

    if((o_ExecUnchanged != CString("")) && (mi_DiffState == Diff_Unchanged) )
        o_Command = o_ExecUnchanged + CString(" dir ") + o_Complete  ;	    
    else if((o_ExecModified != CString("")) && (mi_DiffState == Diff_Modified) )
        o_Command = o_ExecModified +  CString(" dir ") + o_Complete ;	    
    else if((o_ExecNew != CString("")) && (mi_DiffState == Diff_New) )
        o_Command = o_ExecNew +  CString(" dir ") + o_Complete ;	    
    else if((o_ExecDeleted != CString("")) && (mi_DiffState == Diff_Deleted) )
        o_Command = o_ExecDeleted + CString(" dir ") + o_Complete ;	    
    else
        return;

    if( (o_Command != CString("")) && mp_Father )
    {
        system(M_GetBuffer(o_Command));
    }


    for(i=0; i<mi_ChildrenFileNb; i++)
    {
        mp_ChildrenFileList[i]->Synchronize(o_ExecUnchanged,o_ExecModified,o_ExecNew,o_ExecDeleted);
    }

    for(i=0; i<mi_ChildrenDirNb; i++)
    {
        mp_ChildrenDirList[i]->Synchronize(o_ExecUnchanged,o_ExecModified,o_ExecNew,o_ExecDeleted);
    }
    
    mi_DiffState = Diff_NotCompared;
}

void CMyDir::AddSubdirChild(CMyDir*po_Child)
{
    if(mp_ChildrenDirList)
    {
        CMyDir**pTmp = mp_ChildrenDirList ;
        mp_ChildrenDirList = new LPCMyDir[mi_ChildrenDirNb+1];
        memcpy(mp_ChildrenDirList, pTmp, mi_ChildrenDirNb*sizeof(CMyDir*));
        delete [] pTmp;
    }
    else
    {
        mp_ChildrenDirList = new LPCMyDir[1];
    }

    po_Child->mp_Father = this;
    mp_ChildrenDirList[mi_ChildrenDirNb++] = po_Child;
}

void CMyDir::AddFileChild(CMyFile*po_Child)
{
    if(mp_ChildrenFileList)
    {
        CMyFile**pTmp = mp_ChildrenFileList;
        mp_ChildrenFileList = new LPCMyFile[mi_ChildrenFileNb+1];
        memcpy(mp_ChildrenFileList, pTmp, mi_ChildrenFileNb*sizeof(CMyFile*));
        delete [] pTmp;
    }
    else
    {
        mp_ChildrenFileList= new LPCMyFile[1];
    }

    po_Child->mp_Father = this;
    mp_ChildrenFileList[mi_ChildrenFileNb++] = po_Child;
}

void CMyFile::PrintFile(CFile*poFile)
{
    CString o_Line;

    o_Line.Format
    (
        "F %04d-%02d-%02d %02d:%02d:%02d\t%s\t%d\n", 
        mo_LastWriteTime.GetYear(),
        mo_LastWriteTime.GetMonth(),
        mo_LastWriteTime.GetDay(),
        mo_LastWriteTime.GetHour(),
        mo_LastWriteTime.GetMinute(),
        mo_LastWriteTime.GetSecond(),  
        mo_Name.GetBuffer(mo_Name.GetLength()),
        mui_Size
    );
    poFile->Write(o_Line.GetBuffer(o_Line.GetLength()), o_Line.GetLength());
}

void CMyDir::PrintDir(CFile*poFile)
{
    CString o_Line;
    int i;

    o_Line.Format
    (
        "D %04d-%02d-%02d %02d:%02d:%02d\t%s\t%d\n", 
        mo_LastWriteTime.GetYear(),
        mo_LastWriteTime.GetMonth(),
        mo_LastWriteTime.GetDay(),
        mo_LastWriteTime.GetHour(),
        mo_LastWriteTime.GetMinute(),
        mo_LastWriteTime.GetSecond(),  
        mo_Name.GetBuffer(mo_Name.GetLength()),
        mui_Size
    );
    poFile->Write(o_Line.GetBuffer(o_Line.GetLength()), o_Line.GetLength());

    for(i=0; i<mi_ChildrenFileNb; i++)
        mp_ChildrenFileList[i]->PrintFile(poFile);

    for(i=0; i<mi_ChildrenDirNb; i++)
        mp_ChildrenDirList[i]->PrintDir(poFile);

}

void CMyDir::DisplayDir(CListCtrl *po_List, int iFlag)
{
    
    CString o_Line, o_Line1, o_Line2;
    int i,line, j;
    
    for(i=j=0; i<mi_ChildrenFileNb; i++)
    {
        if( (iFlag & (1<<mp_ChildrenFileList[i]->mi_DiffState)) == 0) continue;

        line = po_List->InsertItem(j++, "", mp_ChildrenFileList[i]->mi_DiffState);
        po_List->SetItemData(line, (DWORD_PTR)mp_ChildrenFileList[i]);
        po_List->SetItem(line, C_NAME, LVIF_TEXT, M_GetBuffer(mp_ChildrenFileList[i]->mo_Name), 0, 0, 0, 0);

        mp_ChildrenFileList[i]->GetRelativePath(o_Line1);
        po_List->SetItem(line, C_PATH, LVIF_TEXT, M_GetBuffer(o_Line1), 0, 0, 0, 0);
        
        o_Line.Format("%d",mp_ChildrenFileList[i]->mui_Size);
        po_List->SetItem(line, C_SIZE, LVIF_TEXT, M_GetBuffer(o_Line), 0, 0, 0, 0);
        
        o_Line2.Format("%04d-%02d-%02d %02d:%02d:%02d",
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetYear(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetMonth(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetDay(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetHour(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetMinute(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetSecond()
            );
        po_List->SetItem(line, C_WDATE, LVIF_TEXT, M_GetBuffer(o_Line2), 0, 0, 0, 0);
    }

    for(i=0; i<mi_ChildrenDirNb; i++)
    {
        mp_ChildrenDirList[i]->DisplayDir(po_List, iFlag, &j);
    }
       
}

void CMyDir::DisplayDir(CListCtrl *po_List, int iFlag, int * pj)
{
    CString o_Line, o_Line1, o_Line2;
    int i, line;
    
    for(i=0; i<mi_ChildrenFileNb; i++)
    {
        if( (iFlag & (1<<mp_ChildrenFileList[i]->mi_DiffState)) == 0) continue;

        line = po_List->InsertItem(*pj, "", mp_ChildrenFileList[i]->mi_DiffState);
        po_List->SetItemData(line, (DWORD_PTR)mp_ChildrenFileList[i]);

        *pj += 1;

        po_List->SetItem(line, C_NAME, LVIF_TEXT, M_GetBuffer(mp_ChildrenFileList[i]->mo_Name), 0, 0, 0, 0);

        mp_ChildrenFileList[i]->GetRelativePath(o_Line);
        po_List->SetItem(line, C_PATH, LVIF_TEXT, M_GetBuffer(o_Line), 0, 0, 0, 0);
        
        o_Line1.Format("%d",mp_ChildrenFileList[i]->mui_Size);
        po_List->SetItem(line, C_SIZE, LVIF_TEXT, M_GetBuffer(o_Line1), 0, 0, 0, 0);
        
        o_Line2.Format("%04d-%02d-%02d %02d:%02d:%02d",
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetYear(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetMonth(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetDay(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetHour(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetMinute(),
            mp_ChildrenFileList[i]->mo_LastWriteTime.GetSecond()
            );
        po_List->SetItem(line, C_WDATE, LVIF_TEXT, M_GetBuffer(o_Line2), 0, 0, 0, 0);
    }

    for(i=0; i<mi_ChildrenDirNb; i++)
    {
        mp_ChildrenDirList[i]->DisplayDir(po_List,iFlag, pj);
    }
}

void CMyDir::Compare(CMyDir*poDir)
{
    int i,j;
    char *ai_NewFile;
    CMyFile*poFile;
    CMyDir*poNewDir;

    ai_NewFile = new char[poDir->mi_ChildrenFileNb];
    memset(ai_NewFile, -1, poDir->mi_ChildrenFileNb);
    
    for(i=0; i<mi_ChildrenFileNb; i++)
    {
        mp_ChildrenFileList[i]->mi_DiffState = Diff_Deleted;

        for(j=0; j<poDir->mi_ChildrenFileNb; j++)
        {
            if(poDir->mp_ChildrenFileList[j]->mo_Name != mp_ChildrenFileList[i]->mo_Name) continue;

            if( *poDir->mp_ChildrenFileList[j] == *mp_ChildrenFileList[i]) 
                mp_ChildrenFileList[i]->mi_DiffState = Diff_Unchanged;
            else
                mp_ChildrenFileList[i]->mi_DiffState = Diff_Modified;

            ai_NewFile[j] = 0;
            break;
        }
    }
    for(j=0; j<poDir->mi_ChildrenFileNb; j++)
    {
        if(ai_NewFile[j] == 0) continue;

        poFile = new CMyFile(poDir->mp_ChildrenFileList[j]->mo_Name, poDir->mp_ChildrenFileList[j]->mo_LastWriteTime, poDir->mp_ChildrenFileList[j]->mui_Size);
        poFile->mi_DiffState = Diff_New;
        this->AddFileChild(poFile);
    }
    delete [] ai_NewFile;

    ////////////////////////////////////////////////////////////////
    
    ai_NewFile = new char[poDir->mi_ChildrenDirNb];
    memset(ai_NewFile, -1, poDir->mi_ChildrenDirNb);

    for(i=0; i<mi_ChildrenDirNb; i++)
    {
        mp_ChildrenDirList[i]->mi_DiffState = Diff_Deleted;

        for(j=0; j<poDir->mi_ChildrenDirNb; j++)
        {
            if(poDir->mp_ChildrenDirList[j]->mo_Name != mp_ChildrenDirList[i]->mo_Name) continue;
            mp_ChildrenDirList[i]->mi_DiffState = Diff_Modified;

            mp_ChildrenDirList[i]->Compare(poDir->mp_ChildrenDirList[j]);
            ai_NewFile[j] = 0;
            break;
        }
    }
    for(j=0; j<poDir->mi_ChildrenDirNb; j++)
    {
        if(ai_NewFile[j] == 0) continue;

        poNewDir = new CMyDir(poDir->mp_ChildrenDirList[j]);
        this->AddSubdirChild(poNewDir);
        poNewDir->mi_DiffState = Diff_New;
        poNewDir->mp_Father = this;
    }

    delete [] ai_NewFile;
}

int CMyDir::operator==(const CMyDir & oDir) 
{
    int i;

    if(mi_ChildrenDirNb != oDir.mi_ChildrenDirNb) return 0;
    if(mi_ChildrenFileNb!= oDir.mi_ChildrenFileNb) return 0;

    for(i=0; i<mi_ChildrenDirNb; i++)
    {
        if(*mp_ChildrenDirList[i] == *oDir.mp_ChildrenDirList[i]) continue;
        return 0;
    }

    for(i=0; i<mi_ChildrenFileNb; i++)
    {
        if(*mp_ChildrenFileList[i] == *oDir.mp_ChildrenFileList[i]) continue;
        return 0;
    }

    return 1;
}


void CSynchronizeDlg::OnBnClickedBtnIn()
{
   CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, NULL, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_InPath = fileDlg.GetPathName();
      mo_InPath = mo_InPath.Left(mo_InPath.ReverseFind('\\'));
      GetDlgItem(IDC_ED_INPUT)->SetWindowText(mo_InPath);
   }
}

void CSynchronizeDlg::OnBnClickedBtnRef()
{
   CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, NULL, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_RefPath = fileDlg.GetPathName();
      mo_RefPath = mo_RefPath.Left(mo_RefPath.ReverseFind('\\'));
      GetDlgItem(IDC_ED_REFERENCE)->SetWindowText(mo_RefPath);
   }
}

void CSynchronizeDlg::OnBnClickedBtnDel()
{
   CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST| OFN_FILEMUSTEXIST, NULL, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_ExecDel = fileDlg.GetPathName();
      GetDlgItem(IDC_ED_ONDEL)->SetWindowText(mo_ExecDel);
   }
}


void CSynchronizeDlg::OnBnClickedBtnOnsame()
{
   CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST| OFN_FILEMUSTEXIST, NULL, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_ExecSame = fileDlg.GetPathName();
      GetDlgItem(IDC_ED_ONSAME)->SetWindowText(mo_ExecSame);
   }
}

void CSynchronizeDlg::OnBnClickedBtnOnmodif()
{
   CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST| OFN_FILEMUSTEXIST, NULL, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_ExecModif = fileDlg.GetPathName();
      GetDlgItem(IDC_ED_ONMODIF)->SetWindowText(mo_ExecModif);
   }
}

void CSynchronizeDlg::OnBnClickedBtnOnnew()
{
    CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST, NULL, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_ExecNew= fileDlg.GetPathName();
      GetDlgItem(IDC_ED_ONNEW)->SetWindowText(mo_ExecNew);
   }
}

void CSynchronizeDlg::OnBnClickedBtnSync()
{
    GetDlgItem(IDC_BTN_SYNC)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SYNCSEL)->EnableWindow(FALSE);

    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    GetDlgItem(IDC_ED_ONNEW)->GetWindowText(mo_ExecNew);
    GetDlgItem(IDC_ED_ONDEL)->GetWindowText(mo_ExecDel);
    GetDlgItem(IDC_ED_ONSAME)->GetWindowText(mo_ExecSame);
    GetDlgItem(IDC_ED_ONMODIF)->GetWindowText(mo_ExecModif);

    mpo_RefRoot->Synchronize(mo_ExecSame, mo_ExecModif, mo_ExecNew, mo_ExecDel);
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CSynchronizeDlg::OnBnClickedBtnSyncsel()
{
    UINT i;
    int  nItem = -1;
    CListCtrl*po_List;
    CMyFile*poFile;

    GetDlgItem(IDC_BTN_SYNC)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SYNCSEL)->EnableWindow(FALSE);

    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    po_List = (CListCtrl*)GetDlgItem(IDC_LIST);
    
    for(i=0; i<po_List->GetSelectedCount(); i++)
    {
      nItem = po_List->GetNextItem(nItem, LVNI_SELECTED);
      ASSERT(nItem != -1);

      poFile = (CMyFile*)po_List->GetItemData(nItem);

      if(poFile->mp_Father) 
      {
        CString o_Complete;
        CString o_Command;
        
        poFile->mp_Father->GetRelativePath(o_Complete);
        o_Complete = CString(" \"") + o_Complete + CString("\"");
        o_Command = CString("");
        

        if((mo_ExecSame != CString("")) && (poFile->mp_Father->mi_DiffState == CMyFile::Diff_Unchanged) )
            o_Command = mo_ExecSame + CString(" dir ") + o_Complete ;	    
        else if((mo_ExecModif!= CString("")) && (poFile->mp_Father->mi_DiffState == CMyFile::Diff_Modified) )
            o_Command = mo_ExecModif + CString(" dir ") + o_Complete;
        else if((mo_ExecNew!= CString("")) && (poFile->mp_Father->mi_DiffState == CMyFile::Diff_New) )
            o_Command = mo_ExecNew  + CString(" dir ") + o_Complete;
        else if((mo_ExecDel!= CString("")) && (poFile->mp_Father->mi_DiffState == CMyFile::Diff_Deleted) )
            o_Command = mo_ExecDel  + CString(" dir ") + o_Complete;
        

        if( (o_Command != CString("")) && poFile->mp_Father->mp_Father )
            system(M_GetBuffer(o_Command));

        poFile->mp_Father->mi_DiffState = CMyFile::Diff_NotCompared;
      }

      poFile->Synchronize(mo_ExecSame, mo_ExecModif, mo_ExecNew, mo_ExecDel);
      
   }
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CSynchronizeDlg::OnBnClickedBtnSelall()
{
    int i ;
    CListCtrl*po_List;

    po_List = (CListCtrl*)GetDlgItem(IDC_LIST);

    for(i=0; i<po_List->GetItemCount(); i++)
    {
        po_List->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);   
        po_List->Update(i);
    }
}

void CSynchronizeDlg::OnBnClickedBtnSelnone()
{
    int i, iSelected ;
    CListCtrl*po_List;

    po_List = (CListCtrl*)GetDlgItem(IDC_LIST);
    iSelected = (int)po_List->GetSelectedCount();

    for(i=0; i<iSelected; i++)
    {
        po_List->SetItemState(i, 0, LVIS_SELECTED);    
        po_List->Update(i);
    }
}

void CSynchronizeDlg::OnBnClickedBtnReversesel()
{
    int i, iSelected ;
    CListCtrl*po_List;
    UINT iState;

    po_List = (CListCtrl*)GetDlgItem(IDC_LIST);
    iSelected = po_List->GetItemCount();
    for(i=0; i<iSelected ; i++)
    {
        iState = po_List->GetItemState(i, LVIS_SELECTED);    
        po_List->SetItemState(i, iState?0:LVIS_SELECTED, LVIS_SELECTED);    
        po_List->Update(i);
    }
    
}

void CSynchronizeDlg::OnBnClickedBtnOpen()
{
/*
old button code
static char BASED_CODE szFilter[] = "Synchro Files (*.syn)|*.syn|All Files (*.*)|*.*||";

    CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST, szFilter, this);
  
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_SyncFileName = fileDlg.GetPathName();
      LoadSyncFile(mo_SyncFileName);
   }
   */
}

void CSynchronizeDlg::OnBnClickedOk()
{
    SaveSyncFile(mo_SyncFileName);
    OnOK();
}

void CSynchronizeDlg::LoadSyncFile(CString& o_FileName)
{
    CString o_Title;
    char sz_Buffer[MAX_PATH];

    o_Title = CString(AfxGetAppName()) + CString(" - ") + o_FileName;
    AfxGetMainWnd()->SetWindowText(o_Title);

    GetPrivateProfileString("Folder", "Input", "", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    mo_InPath = CString(sz_Buffer);
    GetPrivateProfileString("Folder", "Reference", "", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    mo_RefPath = CString(sz_Buffer);
    GetDlgItem(IDC_ED_INPUT)->SetWindowText(mo_InPath);
    GetDlgItem(IDC_ED_REFERENCE)->SetWindowText(mo_RefPath);

    GetPrivateProfileString("Command", "Same", "", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    mo_ExecSame= CString(sz_Buffer);
    GetPrivateProfileString("Command", "Del", "", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    mo_ExecDel= CString(sz_Buffer);
    GetPrivateProfileString("Command", "Modif", "", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    mo_ExecModif= CString(sz_Buffer);
    GetPrivateProfileString("Command", "New", "", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    mo_ExecNew= CString(sz_Buffer);
    GetDlgItem(IDC_ED_ONDEL)->SetWindowText(mo_ExecDel);
    GetDlgItem(IDC_ED_ONMODIF)->SetWindowText(mo_ExecModif);
    GetDlgItem(IDC_ED_ONNEW)->SetWindowText(mo_ExecNew);
    GetDlgItem(IDC_ED_ONSAME)->SetWindowText(mo_ExecSame);

    GetPrivateProfileString("Display", "Same", "1", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    ((CButton*)GetDlgItem(IDC_CK_UNCHANGED))->SetCheck(*sz_Buffer == '1');
    GetPrivateProfileString("Display", "Del", "1", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    ((CButton*)GetDlgItem(IDC_CK_DELETED))->SetCheck(*sz_Buffer == '1');
    GetPrivateProfileString("Display", "Modif", "1", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    ((CButton*)GetDlgItem(IDC_CK_MODIFIED))->SetCheck(*sz_Buffer == '1');
    GetPrivateProfileString("Display", "New", "1", sz_Buffer, MAX_PATH, LPCTSTR(o_FileName));
    ((CButton*)GetDlgItem(IDC_CK_NEW))->SetCheck(*sz_Buffer == '1');
}

void CSynchronizeDlg::SaveSyncFile(CString& o_FileName)
{
    GetDlgItem(IDC_ED_INPUT)->GetWindowText(mo_InPath);
    GetDlgItem(IDC_ED_REFERENCE)->GetWindowText(mo_RefPath);
    WritePrivateProfileString("Folder", NULL, NULL, LPCTSTR(o_FileName));
    WritePrivateProfileString("Folder", "Input", LPCTSTR(mo_InPath), LPCTSTR(o_FileName));
    WritePrivateProfileString("Folder", "Reference", LPCTSTR(mo_RefPath), LPCTSTR(o_FileName));


    GetDlgItem(IDC_ED_ONDEL)->GetWindowText(mo_ExecDel);
    GetDlgItem(IDC_ED_ONMODIF)->GetWindowText(mo_ExecModif);
    GetDlgItem(IDC_ED_ONNEW)->GetWindowText(mo_ExecNew);
    GetDlgItem(IDC_ED_ONSAME)->GetWindowText(mo_ExecSame);
    WritePrivateProfileString("Command", NULL, NULL, LPCTSTR(o_FileName));
    WritePrivateProfileString("Command", "Same", LPCTSTR(mo_ExecSame), LPCTSTR(o_FileName));
    WritePrivateProfileString("Command", "Del", LPCTSTR(mo_ExecDel), LPCTSTR(o_FileName));
    WritePrivateProfileString("Command", "Modif", LPCTSTR(mo_ExecModif), LPCTSTR(o_FileName));
    WritePrivateProfileString("Command", "New", LPCTSTR(mo_ExecNew), LPCTSTR(o_FileName));

    WritePrivateProfileString("Display", NULL, NULL, LPCTSTR(o_FileName));
    WritePrivateProfileString("Display", "Same", ((CButton*)GetDlgItem(IDC_CK_UNCHANGED))->GetCheck() ? "1" : "0", LPCTSTR(o_FileName));
    WritePrivateProfileString("Display", "Del", ((CButton*)GetDlgItem(IDC_CK_DELETED))->GetCheck() ? "1" : "0", LPCTSTR(o_FileName));
    WritePrivateProfileString("Display", "Modif", ((CButton*)GetDlgItem(IDC_CK_MODIFIED))->GetCheck() ? "1" : "0", LPCTSTR(o_FileName));
    WritePrivateProfileString("Display", "New", ((CButton*)GetDlgItem(IDC_CK_NEW))->GetCheck() ? "1" : "0", LPCTSTR(o_FileName));
}



void CSynchronizeDlg::OnBnClickedBtnQuit()
{
    OnOK();
}

void CSynchronizeDlg::OnBnClickedBtnOpen2()
{
    static char BASED_CODE szFilter[] = "Synchro Files (*.syn)|*.syn|All Files (*.*)|*.*||";

    CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST, szFilter, this);
  
   if( fileDlg.DoModal ()==IDOK )
   {
      mo_SyncFileName = fileDlg.GetPathName();
      LoadSyncFile(mo_SyncFileName);
   }
}

void CSynchronizeDlg::OnBnClickedBtnSyncsel2()
{
	int i = 0;
	CString o_Folder, mo_ExecModif;
	CString o_Complete;
	CString o_Command;
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, NULL, this);
	char *pz1, *pz2;
   
	if( fileDlg.DoModal ()==IDOK )
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

		o_Folder = fileDlg.GetPathName();
		o_Folder = o_Folder.Left(o_Folder.ReverseFind('\\'));      
	    
		pz1 = M_GetBuffer(mo_InPath);
		pz2 = M_GetBuffer(o_Folder);

		while(pz1[i] == pz2[i])
		{
			i++;
			if(!pz1[i]) break;
			if(!pz2[i]) break;
		}
		if(!pz2[i]) 
			o_Complete = CString(".");
		else
			o_Complete = CString(pz2 + i);
		
		
		GetDlgItem(IDC_ED_ONMODIF)->GetWindowText(mo_ExecModif);
		o_Command = mo_ExecModif +  CString(" forcedir ") + o_Complete ;	    
		system(M_GetBuffer(o_Command));

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
}

void CSynchronizeDlg::OnBnClickedBtnSyncsel3()
{
	char *pz1, *pz2;
	int i=0;
	CString o_Folder, mo_ExecModif;
	CString o_Complete;
	CString o_Command;
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, NULL, this);
   
	if( fileDlg.DoModal ()==IDOK )
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

		o_Folder = fileDlg.GetPathName();

	    
		pz1 = M_GetBuffer(mo_InPath);
		pz2 = M_GetBuffer(o_Folder);

		while(pz1[i] == pz2[i])
		{
			i++;
			if(!pz1[i]) break;
			if(!pz2[i]) break;
		}
		if(!pz2[i]) 
			o_Complete = CString(".");
		else
			o_Complete = CString(pz2 + i );


		GetDlgItem(IDC_ED_ONMODIF)->GetWindowText(mo_ExecModif);
		o_Command = mo_ExecModif +  CString(" forcefile ") + o_Complete ;	    

		system(M_GetBuffer(o_Command));
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
}
