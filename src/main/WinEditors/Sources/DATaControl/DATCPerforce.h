
//------------------------------------------------------------------------------
//   DATCPerforce.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class DAT_CPerforce
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCPERFORCE_H__
#define __DATCPERFORCE_H__

#if 0
 
//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4ClientApi.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "DATCP4Message.h"
#include "LINks/LINKmsg.h"
#include "DATCUserBreak.h"
#include "DATCUtils.h"
#include "perforceui.h"
#include "assert.h"


typedef std::map<ULONG,PerforceChangelistInfo*>  DAT_TChangelistInfo  ;

class DAT_CP4ClientInfoFStat;
typedef std::map<BIG_KEY, DAT_CP4ClientInfoFStat*> DAT_TP4FilesNotInBF ;

class DAT_CP4ClientInfoHeader;

struct DAT_SHistoryInfo;

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
#pragma warning( disable : 4996 )

#define FILENAMELENGHT			10	// strlen("0xFFFFFFFF");
#define KEYLENGHT				8			// strlen("FFFFFFFF");

#define P4_FILE_SYNC_NB	        1000
#define P4_MAX_FILE				500
#define P4_MAX_CHANGELIST_FILE	8000
#define P4_MAX_LABEL			30
#define P4_DIR_TOTAL			256
#define P4_MAX_DIR_LIMIT		1

// chanelist and file status
#define P4_STATUS_PENDING			"pending"
#define P4_STATUS_SUBMITTED			"submitted"
#define P4_STATUS_UNRESOLVED		"unresolved"
#define P4_STATUS_LOCAL_CHECKOUT	"local checkout"
#define P4_DEFAULT_CHANGELIST_DESCRIPTION	"New changelist"

// Connection & client specs
#define DEFAULT_ROOT_PATH "c:\\\\program files\\\\perforce\\\\temp"

// commands
#define P4_EDIT			"edit"
#define P4_ATTRIBUTE	"attribute"
#define P4_SUBMIT		"submit"
#define P4_FILES		"files"
#define P4_FSTAT		"fstat"
#define P4_SYNC			"sync"
#define P4_FLUSH		"flush"
#define P4_ADD			"add"
#define P4_CHANGE		"change"
#define P4_DELETE		"delete"
#define P4_REVERT		"revert"
#define P4_CHANGES		"changes"
#define P4_OPENED		"opened"
#define P4_REOPEN		"reopen"
#define P4_DIRS			"dirs"
#define P4_CLIENT		"client"
#define P4_CLIENTS		"clients"
#define P4_DESCRIBE		"describe"
#define P4_FILELOG		"filelog"
#define P4_PRINT		"print"
#define P4_USERS		"users"
#define P4_DIFF			"diff"
#define P4_RESOLVE		"resolve"
#define P4_DEPOTS		"depots"
#define P4_LOGIN		"login"
#define P4_INTEGRATE	"integrate"
#define P4_MONITOR		"monitor"


//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
#if 0
class DAT_CPerforce
{
	   //--------------------------------------------------------------------------
	   // public definitions 
	   //--------------------------------------------------------------------------

public:	
		static DAT_CPerforce* GetInstance() { return &ms_Perforce; }
		void Disable() { m_bEnable = FALSE; } 
        void PermanentDisable() { m_bEnable = FALSE; m_bPermanentDisable = TRUE; } 
		void Enable() { m_bEnable = TRUE; } 
		BOOL IsEnabled() { return m_bEnable; }		
        BOOL IsPermanentDisabled() { return m_bPermanentDisable; }	

	    DAT_CPerforce ();
		virtual ~DAT_CPerforce ();

		static void DAT_CPerforce::Initialize	(	const std::string& _strPort,
													const std::string& _strHostName,
													const std::string& _strUsername,
													const std::string& _strPassword
												);

		virtual int 	P4Connect			(BOOL bDisableAbort = FALSE);
		virtual int 	P4Disconnect		();
		virtual int		IsConnected()		{ return m_bIsConnected; }
		
		const char*		GetClient(){ return GetClientApi()->GetClient().Value(); }
		const char*		GetHost(){ return GetClientApi()->GetHost().Value(); }
		const char*		GetPort(){ return GetClientApi()->GetPort().Value(); }
		const char*		GetUser(){ return GetClientApi()->GetUser().Value(); }
		const char*		GetBFVersion() { return m_strBFVersion.c_str(); } 
		void			SetBFVersion(const std::string& _strBFVersion 	) { m_strBFVersion = _strBFVersion; } 

		const std::string&	GetP4Depot() { return m_strP4Depot; } 
		void			SetP4Depot(const std::string& _strP4Depot 	) { m_strP4Depot = _strP4Depot; } 

		const char*		GetPassword(){ return m_strPassword.c_str(); }
		void			SetPassword(const std::string& _strPassword ){ m_strPassword = _strPassword; }

		// P4 commands
		virtual int		ExecuteOperationOnDir(	BIG_INDEX _ulDirIndex,
												int (DAT_CPerforce::*_fpOperation)(BIG_INDEX* ul_index,ULONG _ul_Changelist),
												BOOL _bRecursive,
												BOOL _bFirst,
												BOOL _bAutoSubmit,
												BOOL (*_fpUiFileCallback)(BIG_INDEX _ulIndex)
												);


		virtual int		P4Depots			(std::vector<std::string>& _vDepots);
		virtual int 	P4SyncBF			(const char* _psz_P4Root, const char* _psz_BfPath = NULL, BOOL _bForceSync = FALSE, char* _psz_P4Label = NULL,ULONG _ul_Changelist = P4_INVALIDCHANGELIST);
		virtual int 	P4SyncBFFiles		(std::vector<std::string> &_vP4Files);
		virtual int 	P4SyncDir			(BIG_INDEX _ul_DirIndex, BOOL _bForceSync = FALSE);
		virtual int 	P4Sync				(BIG_INDEX _ul_KeyIndex, BOOL _bForceSync = FALSE, ULONG _ul_RevisionToSync = P4_INVALIDREVISION);
		virtual int 	P4Sync				(const std::vector<std::string>& _vFiles,std::string _strDir = "",BOOL _bForceSync = FALSE );
		virtual int		P4SyncBFPreview		(const char* _psz_P4Root, std::vector<std::string>& vP4Files);

		virtual int		P4Diff				(BIG_KEY ul_Key);
		virtual int		P4Diff				(std::vector<std::string>& vP4Files, std::vector<DAT_CP4ClientInfo*>* _pvFileInfo);

		virtual int 	P4FlushBF			(const char* _psz_P4Root);
		virtual int		P4Flush				(std::vector<ULONG >& _vFiles, ULONG rev);
		virtual int		P4Flush				(std::vector<std::string>& _vFiles, ULONG rev);

		virtual int		P4FStatBF			(const char* _psz_P4Root);
		virtual int		P4Fstat				(const std::vector<std::string>& _vFiles);
		
		virtual int 	P4Add				(const std::vector<std::string>& vP4Files,BOOL _bAutoSubmit = FALSE);
		virtual int 	P4Edit				(const std::vector<std::string>& vP4Files,BOOL _bAutoSubmit = FALSE);
		virtual int 	P4Delete			(const std::vector<std::string>& vP4Files,BOOL _bAutoSubmit = FALSE);
		virtual int 	P4Reopen			(const std::vector<std::string>& vP4Files,ULONG _ulChangelist);


		virtual int 	P4Revert			(BIG_KEY ul_Key,BOOL _bSync = TRUE );
		virtual int 	P4Revert			(const std::vector<std::string>& vP4Files , std::vector<DAT_CP4ClientInfo>* _pvInfo = NULL,BOOL _bRevertUnchanged = FALSE );
		

		virtual int		P4FileInfo			(BIG_KEY _ulKey,std::vector<DAT_CP4ClientInfoHeader*>* _pvFileInfo);
		virtual int		P4FileInfo			(const char* _psz_P4FilePath,std::vector<DAT_CP4ClientInfoHeader*>* _pvFileInfo);
		virtual int		P4FileInfo			(const std::vector<std::string>& _vFiles, std::vector<DAT_CP4ClientInfoHeader*>* _pvFileInfo);
		virtual int 	P4History			(ULONG _ul_Key,std::vector<DAT_SHistoryInfo*>& _lstEntries);
		virtual int     P4Print				(ULONG _ul_Key, ULONG _ul_Rev, StrBuf& _DataBuffer, bool bGetRawData = false); // raw data = compressed file with header
		virtual int		P4Resolve			(BIG_KEY ul_Key,BOOL _bAcceptYours /* else accept theirs */ );
		virtual int		P4Resolve			(const std::vector<std::string>& _vFiles,BOOL _bAcceptYours /* else accept theirs */ );
		virtual int		P4Integrate			(const std::string& _strBranchSpec,const std::string& _strChangelistToIntegrate,BOOL _bReverse );
		virtual int		P4Integrate			(const std::string& _strBranchSpec,const std::vector<std::string>& _vFiles ,BOOL _bReverse);
		
		// change list related command
		virtual int 	P4Submit			(ULONG &_ul_Changelist,BOOL _bVerifyLinks = TRUE );
		virtual int 	P4Opened			(ULONG _ul_Changelist,std::vector<DAT_CP4ClientInfo>* _pFiles);
		virtual int 	P4Opened			(std::vector<DAT_CP4ClientInfo>* _pFiles);
		virtual int 	P4RevertChangelist	(ULONG _ul_Changelist,std::vector<std::string>& _vRevertedFiles,BOOL _bSync = TRUE );
		virtual int		P4Describe			(ULONG _ul_Changelist,std::string& _strDescription);
		virtual int		P4Describe			(ULONG _ul_Changelist,PerforceChangelistInfo* _pChangelistInfo);
		virtual int		P4Describe			(ULONG _ul_Changelist,std::vector<DAT_CP4ClientInfo>* _pvFileInfo);
		virtual int		P4DescribeIntegral	(ULONG _ul_Changelist,std::string& _strWholeText);

		virtual void	SetWorkingChangelist(ULONG _ul_Changelist) { m_ulWorkingChangelist = _ul_Changelist; } 
		virtual ULONG	GetWorkingChangelist(BOOL _bCreate = TRUE );

		
		virtual int		P4Files				(const char* _psz_P4DirPath);
		virtual int		P4Dirs				(const char* _psz_P4DirPath,std::list<std::string>& _lstDirs);
		virtual int		P4Client			(const char* _pszVersion,const std::list<std::string>& _vDirs,std::string& _strDescription,BOOL _bCreate);
		virtual int		P4Users				(std::list<std::string>& _vUser );
		virtual int		P4Clients			(std::list<std::string>& _vClients );
		
		virtual ULONG	P4Change			(ULONG _ul_Changelist,const char* _psz_descritpion,const char* _Files);
		virtual int		P4Changes			(const char* _psz_username,const char* _psz_status,const char* _psz_MaxChangelist ,DAT_TChangelistInfo* _pChangelist);

		virtual int 	P4ChangeDelete		(ULONG _ul_Changelist);

		virtual int 	P4LocalCheckout		();
		virtual int 	P4ShowHistory		(ULONG ul_Key);
		virtual int		P4MonitorShow();
		virtual int		CountFiles			(ULONG ul_Key,ULONG _ul_Changelist){return 0;}
		virtual ULONG	GetCount()			{return m_ulTotalOperationFile;}
		virtual void	SetRelog			(BOOL _bRelog){ m_bRelog = _bRelog; }

		// List of files not present in BF but present on Perforce (filled after a P4FStat)
		virtual void	AddP4FileNotInBF(BIG_KEY _ul_Key, DAT_CP4ClientInfoFStat* pFStatInfo);
		virtual DAT_TP4FilesNotInBF& GetP4FilesNotInBF() { return m_mapP4FilesNotInBF ; }

		std::string&	GetP4Root() { return m_strRoot; }
		void			SetP4Root(const std::string& _strRoot) { m_strRoot = _strRoot; DAT_CP4ClientApi::Log(std::string("New P4 root : ") + _strRoot); }

		std::vector<char*>&	GetP4Directories(const char* _szP4Root, const char* _szValueToAppend = NULL);

		static ULONG UpdateBFFileBuffer(BIG_KEY _ulKey, void* _pBuffer,ULONG _ulBufferSize,const char* _pszBFFilepath);

		void SetCreateChangelistDescription(const std::string& strDescription ) { m_strCreateChangelistDescription = strDescription;}

		std::string GetCreateChangelistDescription() 
		{ 
			if ( m_strCreateChangelistDescription != "" ) 
			return m_strCreateChangelistDescription;
			
			return P4_DEFAULT_CHANGELIST_DESCRIPTION;
		}

		//--------------------------------------------------------------------------
		// protected definitions
		//--------------------------------------------------------------------------
protected:

		virtual int		CheckForErrors(Error& _Error);
	
		// Keeps BF fat in up to date with Perforce
		virtual void	UpdateBF(const std::vector<DAT_CP4ClientInfo>& _vFiles);

		// Verify that all links within a file point to valid files
		int VerifyFileLinks(std::vector<DAT_CP4ClientInfo>& vFileList);

	   //--------------------------------------------------------------------------
	   // private definitions
	   //--------------------------------------------------------------------------

private:
		DAT_TP4FilesNotInBF m_mapP4FilesNotInBF;		
		
		virtual void	CleanP4FilesNotInBF();
		virtual void	RemP4FileNotInBF(BIG_KEY _ul_Key);


		virtual BOOL	ExecuteOperation(	const UINT _uiDefaultArgCount,
											ClientUser* _pClientUser,
											int (DAT_CPerforce::*_fpOperation)(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser),
											const std::vector<std::string>& _vFiles,
											BOOL _bAutoSubmit  = FALSE  ) ;

		virtual int		P4Edit				(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4Revert			(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4Reopen			(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4Delete			(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4Add				(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4Fstat				(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual	int		P4Sync				(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual	int		P4Diff				(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4FileInfo			(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4Integrate			(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);
		virtual int		P4Resolve			(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser);

		virtual int		P4Fstat				(const std::vector<DAT_CP4ClientInfo>& _vFiles);



		ClientApi*			GetClientApi()	{return &m_Client;}
		DAT_CP4ClientApi	m_Client;
		DAT_CUserBreak		m_UserBreak;
		Error				m_Error;
		ULONG				m_ulWorkingChangelist;


		BOOL				m_bIsConnected;
		BOOL				m_bEnable;
        BOOL            	m_bPermanentDisable;
		BOOL				m_bRelog;

		ULONG 				m_ulTotalFile;
		ULONG 				m_ulOperationFile;
		ULONG 				m_ulTotalOperationFile;
		std::string			m_strRoot;
		std::string			m_strCreateChangelistDescription;
		std::string			m_strPassword;
		std::string			m_strBFVersion;
		std::string			m_strP4Depot;

		std::vector<char*>	m_vcP4Directories;
		
		static 	DAT_CPerforce ms_Perforce;
};
#endif

#endif

//------------------------------------------------------------------------------
#endif
