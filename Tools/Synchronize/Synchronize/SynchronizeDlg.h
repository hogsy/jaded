// SynchronizeDlg.h : header file
//

#pragma once

class CMyDir;

// CSynchronizeDlg dialog
class CSynchronizeDlg : public CDialog
{
// Construction
public:
    CString mo_SyncFileName;
    CString mo_InPath;
    CString mo_RefPath;

    CString mo_ExecSame;
    CString mo_ExecModif;
    CString mo_ExecNew;
    CString mo_ExecDel;

    CMyDir  *mpo_RefRoot;
    CMyDir  *mpo_InRoot;


	CSynchronizeDlg(CWnd* pParent = NULL);	// standard constructor
	~CSynchronizeDlg(void);	
    int GetDispFilter(void);
    void LoadSyncFile(CString&);
    void SaveSyncFile(CString&);

// Dialog Data
	enum { IDD = IDD_SYNCHRONIZE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    CImageList						mo_ImageList;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedBtnIn();
    afx_msg void OnBnClickedBtnRef();
    afx_msg void OnBnClickedBtnDel();
    afx_msg void OnBnClickedBtnOnsame();
    afx_msg void OnBnClickedBtnOnmodif();
    afx_msg void OnBnClickedBtnOnnew();
    afx_msg void OnBnClickedBtnSync();
    afx_msg void OnBnClickedBtnSyncsel();
    afx_msg void OnBnClickedBtnSelall();
    afx_msg void OnBnClickedBtnSelnone();
    afx_msg void OnBnClickedBtnReversesel();
    afx_msg void OnBnClickedBtnOpen();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedBtnQuit();
    afx_msg void OnBnClickedBtnOpen2();
	afx_msg void OnBnClickedBtnSyncsel2();
	afx_msg void OnBnClickedBtnSyncsel3();
};








class CMyDir;

class CMyFile
{
public:
    CString     mo_Name;
    CTime       mo_LastWriteTime;
    unsigned int        mui_Size;
    CMyDir      *mp_Father;
	enum { Diff_NotCompared=0, Diff_Unchanged=1, Diff_Modified=2, Diff_New=3, Diff_Deleted=4, Diff_Nb=5 };
    int         mi_DiffState;

public:
                CMyFile(void);
                CMyFile(CString&, CTime&, unsigned int=0);
                ~CMyFile(void);
    void PrintFile(CFile*);
    void GetCompletePath(CString&);
    void GetRelativePath(CString&);
    int operator==(const CMyFile & oFile) ;
    void Synchronize(CString&o_ExecUnchanged,CString&o_ExecModified,CString&o_ExecNew,CString&o_ExecDeleted);
};


class CMyDir : public CMyFile
{
public:
    int         mi_ChildrenDirNb;
    CMyDir      **mp_ChildrenDirList;
    int         mi_ChildrenFileNb;
    CMyFile     **mp_ChildrenFileList;

public:
                CMyDir(void);
                CMyDir(CMyDir*);
                CMyDir(CString&, CTime&);
                ~CMyDir(void);
public:
        void AddSubdirChild(CMyDir*);
        void AddFileChild(CMyFile*);
        void PrintDir(CFile*);
        void DisplayDir(CListCtrl *, int);
        void DisplayDir(CListCtrl*, int, int*);
        void Compare(CMyDir*pDir);
        int operator==(const CMyDir & oDir) ;
        void Synchronize(CString&o_ExecUnchanged,CString&o_ExecModified,CString&o_ExecNew,CString&o_ExecDeleted);
};

typedef CMyDir* LPCMyDir;
typedef CMyFile* LPCMyFile;