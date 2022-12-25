/*$T PERCframe.h GC 1.138 10/14/04 14:40:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"
#include "PERcframe.h"
#include "PERcframe_act.h"
#include "PERmsg.h"
#include	"PERCDataCtrl.h"

#ifdef JADEFUSION
#include "EDIFileFilters.h"
#endif

#include "DATaControl\DATCP4ClientUserChanges.h"

enum EPER_EIcon
{
	eICON_NOTUSED1,
	eICON_DEPOT,
	eICON_FOLDER,
	eICON_USER_CHANGELISTS,
	eICON_OTHERUSERS_CHANGELISTS,
	eICON_USER_CHANGELIST,
	eICON_OTHERUSER_CHANGELIST,
	eICON_OTHERCLIENT_CHANGELIST,
	eICON_SUBMITTED_CHANGELIST,
	eICON_BRANCHSPECS,
	eICON_NOTUSED2,
	eICON_NOTUSED3,
	eICON_FILEONCLIENT,
	eICON_NOTUSED4,
	eICON_WARNING,
	eICON_ERROR,
	eICON_REFRESH,
	eICON_NOTUSED6,
	eICON_RESET,
	eICON_NOTUSED8,
	eICON_CLIENTVIEW,
	eICON_USER,
	eICON_NOTONCLIENTFILE,
	eICON_FILE,
	eICON_FILE2,
	eICON_NOTUSED9,
	eICON_EDIT,
	eICON_ADD,
	eICON_DELETE,
	eICON_ADD_OTHERUSER,
	eICON_EDIT_OTHERUSER,
	eICON_DELETE_OTHERUSER,
	eICON_WORKING_CHANGELIST,
	eICON_LOCKED,
	eICON_NOTUSED12,
	eICON_UNRESOLVED,
	eMAX_ICON
};

const char P4_USRMSG_SYNC_BEFORE_OPERATION [] = "File(s) are outdated with server, Get server version ? %s";

class EPER_cl_TreeCtrl;
class CEditLog;
class EDIA_cl_MsgLinkDialog;
class DAT_CP4ClientInfoChanges;

/*$4
 ***********************************************************************************************************************
    CONSTANTS FOR STRING
 ***********************************************************************************************************************
 */

class EPER_cl_Frame :
	public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(EPER_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPER_cl_Frame(void);
	~EPER_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPER_cl_TreeCtrl*		mpo_TreeView;
	CSplitterWnd*			mpo_InsideSplitter;
	CEdit*					mpo_Edit;

	CMapPtrToPtr			m_IndexDirToTree;
	BOOL					mb_Locked;

	CImageList				m_ImageList;
	DAT_TChangelistInfo		m_mChangelist;

	std::list<std::string>	m_lstVersion;
	CEditLog*				m_pEditLog;
#ifdef JADEFUSION
	EDI_cl_FileFilter		m_FileFilter;
#endif
	struct
	{
		int i_Version;
	} mst_Ini;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	HTREEITEM	GetTreeItemFromlParam(LONG);
	void		OnCtrlPopup(CPoint);

	void	GetSelectedItems(CTreeCtrl* _pTree,HTREEITEM _hItem,std::vector<HTREEITEM>& _vSelected );


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	OpenProject(void);
	BOOL	SetupPerforce();
	void	CloseProject(void);
	BOOL	b_KnowsKey(USHORT _uw_Key);


	void	Refresh(ULONG _ul_Changelist = -1);
	void	MoveItems(std::vector<BIG_KEY>& _vFileIndex,ULONG _ulChangelist );
	void	RevertSelected(HTREEITEM hParentItem);


	BOOL	b_CanActivate(void);
	char	*psz_OnActionGetBase(void)	{ return EPERC_asz_ActionBase; };

	void	OnAction(ULONG);
	void	RemoveSelectedItem(HTREEITEM _hSelectedItem);
	BOOL	SelectBigFileVersion(char* _pszBfVersion);
	BOOL	SelectPerforceServer(char* _pszPerforceServer);
	void	OnSubmit();
	void	OnSubmitEdit();
	void	OnHistory();
	void	OnRevert();
	void	OnRevertUnchanged();
	void	OnRevertUnchanged(ULONG _ulChangelist);
	void	OnDeleteEmpty();
	void	OnEdit();
	void	OnResolve();
	void	ReplicateSubmitOnVersion(ULONG _ulChangelist, const std::string& _strAdditionnalVersion);

	void	AddSelected			(PER_CDataCtrl* _pItemCtrl);
	void	EditSelected		(PER_CDataCtrl* _pItemCtrl);
	void	DeleteSelected		(PER_CDataCtrl* _pItemCtrl);
	void	SyncDeleteSelected	(PER_CDataCtrl* _pItemCtrl);
	void	SubmitSelected		(PER_CDataCtrl* _pItemCtrl);
	void	SubmitEditSelected	(PER_CDataCtrl* _pItemCtrl);
	void	SyncSelected		(PER_CDataCtrl* _pItemCtrl,BOOL _bForceSync);
	void	ServerSync			(PER_CDataCtrl* _pItemCtrl);
	void	RevertSelected		(PER_CDataCtrl* _pItemCtrl);
	void	ShowHistorySelected	(PER_CDataCtrl* _pItemCtrl);
	void	DiffSelected		(PER_CDataCtrl* _pItemCtrl);
	void	IntegrateSelected	(PER_CDataCtrl* _pItemCtrl);
	void	FlushFiles			(std::vector<ULONG >& _vFiles, ULONG rev);
	void	FlushDir			(BIG_INDEX _ulIndex, ULONG rev);
	
	static BOOL	PerforceCheckFileUpToDate(std::vector<BIG_INDEX>& _vFileIndex,BOOL _bDiffDetectedChanged = TRUE );
	
	// Check if a world is open (if so close it)
	static BOOL	IsWorldCurrentlyOpened();
	
	void	OnSyncChangelistFiles(BOOL _bPreviousRevision = FALSE);
	void	OnSyncBfToChangelist();
	void	OnOpenChangelistFiles();

	BOOL	EditChangelist(ULONG _ulChangelist,BOOL _bActivateAdditionnalVersionning, std::string& _strAdditionVersion,BOOL& _bVerifyLinks, BOOL& _bRevertUnchanged);
	void	OnSetWorking();
	void	OnAdd();

	void	OnAutoCheckout( BIG_INDEX _ulIndex,BOOL* _pIsCheckout );

	void	OnClearLogWindow();

	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	UINT	ui_OnActionState(ULONG);
	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);

	void        OnTreeCtrlSelChange(void);
	void        OnTreeCtrlExpanded(NM_TREEVIEW *);
	void        OnTreeCtrlBeginDrag(CPoint);

	void    OnRealIdle(void);

	BOOL GetFilter(UINT _ulFilter,std::string& _strFilter,BOOL _AddIfNotThere = FALSE);
	void ClearChangelist(ULONG _ulChangelist);
	void ClearChangelistTree(ULONG _ulChangelist);
	BOOL FillChangelistTreeFiles(const PerforceChangelistInfo* _pChangelistInfo,HTREEITEM hChangelist,BOOL _bFull	);
	BOOL FillChangelistTreeExpand( const std::vector<PerforceFileInfo*>& in_vFiles, std::vector<DAT_CP4ClientInfoChanges>& out_vFile);
	void FillChangelistTree(const PerforceChangelistInfo* pChangelistInfo,BOOL _bFull);
	void FillChangelistTree(ULONG _ulChangelist,BOOL _bFull);
	void ClearChangelistFiles(HTREEITEM hChangelist);

	ULONG ArrangeIntoChangelist( const std::vector< ULONG >& lstIndexes );

	EPER_EIcon GetIconFile(const std::string& _strStatus, const std::string& _strUser,const std::string& _strCommand,const std::string& _strVersion);
	EPER_EIcon GetIconChangelist(ULONG _ulChangelist,const std::string& _strStatus, const std::string& _strUser,const std::string& _strVersion);

	void OutputMessage(const char* _pszMessage,COLORREF _dwColor = 0 );

 private:
	void ComboBoxResetContentAndDeleteDataPtr( int ctrlID );
	void Lock(BOOL _bLock, BIG_INDEX ulDir,BIG_INDEX ulFile ,char* _pszPath  );
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnClose(void);
	afx_msg void	OnRefresh(void) { Refresh(); }
	afx_msg void	OnDefault(void);
#ifdef JADEFUSION
	afx_msg void	OnCheckFilterSettings();
	afx_msg void	OnClickFilterSettings();
#endif
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
