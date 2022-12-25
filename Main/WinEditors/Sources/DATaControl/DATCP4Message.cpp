//------------------------------------------------------------------------------
// Filename   :DATCP4ClientUser.cpp
/// \author    YCharbonneau
/// \date      2005-01-11
/// \par       Description: Implementation of DATCP4ClientUser
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCP4Message.h"
#include "DATCPerforce.h"
#include "LINks/LINKmsg.h"
#include "DATCPerforce.h"
#include "bigfiles\bigfat.h"

#include "DATCP4ClientApi.h"

// editor refresh and break
#include "EDImainframe.h"
#include "EDImsg.h"
#include "EDItors\Sources\PERForce\PERmsg.h"
#include "LINks\LINKtoed.h"
#include "Greta/regexpr2.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

#ifdef JADEFUSION
extern BOOL BIG_b_CheckUserAbortCommand(void);
#else
extern "C" BOOL BIG_b_CheckUserAbortCommand(void);
#endif
//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------
DAT_CP4ClientInfo DAT_CP4Message::ms_LastFileInfo;
BOOL DAT_CP4Message::ms_bLoggedError = FALSE;

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   DAT_CP4Message::DAT_CP4Message(const char* clientName /* = NULL */ , ULONG ulFilesToHandle /* = P4_MAX_FILESTOHANDLE */  ):
/// \author    YCharbonneau
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DAT_CP4Message::DAT_CP4Message(const char* clientName /* = NULL */ , 
							   ULONG ulFilesToHandle /* = P4_MAX_FILESTOHANDLE */	   ):
m_strClientName(clientName),
m_ulAffectedFiles(0),
m_ulFilesToHandle(ulFilesToHandle),
m_ulProgressCount(0),
m_bRefreshProgress(TRUE),
mpo_Progress(NULL)
{
	char tmpStr[MAX_PATH];
	char tmpTimeStr[MAX_PATH];
	L_time_t x_Time;

	L_time(&x_Time);
	L_strftime(tmpTimeStr, MAX_PATH, "%H:%M:%S", L_localtime(&x_Time));
	sprintf(tmpStr, "BEGIN --%s-- [%s]", m_strClientName.c_str(), tmpTimeStr);
	
	DAT_CP4Message::OutputHeaderMessage(tmpStr);

	char tmpProgress[MAX_PATH];
	sprintf(tmpProgress, "%s - Please wait...", m_strClientName.c_str());

	mpo_Progress = new EDIA_cl_UPDATEDialog(tmpProgress);
	mpo_Progress->DoModeless();
	
	ClearProgress();
}

//------------------------------------------------------------
//   DAT_CP4Message::~DAT_CP4Message()
/// \author    YCharbonneau
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DAT_CP4Message::~DAT_CP4Message()
{
	char tmpStr[MAX_PATH];
	char tmpTimeStr[MAX_PATH];
	L_time_t	x_Time;

	L_time(&x_Time);
	L_strftime(tmpTimeStr, MAX_PATH, "%H:%M:%S", L_localtime(&x_Time));

	sprintf(tmpStr, "END --%s-- [%s]", m_strClientName.c_str(), tmpTimeStr);

	DAT_CP4Message::OutputHeaderMessage(tmpStr);

	ClearProgress();
	
	if ( mpo_Progress )
	{
		delete mpo_Progress;
		mpo_Progress = NULL;
	}
}

//------------------------------------------------------------
//   BOOL DAT_CP4Message::CheckLoggedState( Error *err )
/// \author    YCharbonneau
/// \date      23/05/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL DAT_CP4Message::CheckLoggedState( Error *err )
{
	// Check whether or not Expired login was denied
	static const regex::rpattern ExpiredPattern("Your session has expired, please login again.");
	static const regex::rpattern InvalidPasswordPattern("Perforce password \\(P4PASSWD\\) invalid or unset.");
	regex::match_results results;

	StrBuf buf;
	err->Fmt(buf, EF_PLAIN);

	std::string str = buf.Text();

	if (	ExpiredPattern.match(str, results).matched || 
			InvalidPasswordPattern.match(str, results).matched	) 
	{
		
		DAT_CPerforce::GetInstance()->SetRelog(TRUE);
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------
//   void DAT_CP4Message::Message( Error *err )
/// \author    YCharbonneau
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4Message::Message( Error *err )
{
	CheckLoggedState( err );

	ms_LastFileInfo.uiFileInfo = 0;

	// Process the message returned by the command
	StrBuf buf;
	err->Fmt(buf, EF_PLAIN);

	DAT_CP4ClientApi::Log(buf.Text());

	std::string strbuf(buf.Text());
	
	if ( strchr(buf.Value(),'#') )
	{
		// Pattern for P4 edit/add/delete/sync/flush/opened/files/revert/reopen/print
		static const regex::rpattern PatternAction(".+/\\w{4}/0x(\\w{8})#([0-9]+|none)(\\s-\\s|\\s)(add|delete|edit|.+ not on client|no such file|sync|flush|opened|files|revert|reopen|print|refreshing|updating|was .+, reverted|was .+, abandoned).+");
		static const regex::rpattern PatternOther(".+/\\w{4}/0x(\\w{8})#([0-9]+|none)(\\s-\\s|\\s)(.+)");

		static regex::match_results versionResult;
		if ( PatternAction.match(strbuf, versionResult).matched ) 
		{
			ms_LastFileInfo.ulKey = DAT_CUtils::GetKeyFromExactString(versionResult.backref(1).str().c_str());
			ms_LastFileInfo.uiFileInfo |= eVALID_KEY;
			
			ms_LastFileInfo.ulCmdRevision = P4_INVALIDREVISION;
			if ( versionResult.backref(2).str().c_str() != "none" )
			{
				ms_LastFileInfo.ulCmdRevision = atol(versionResult.backref(2).str().c_str());
				ms_LastFileInfo.uiFileInfo |= eVALID_REVISION;
			}
			
			strcpy(ms_LastFileInfo.aszAction,versionResult.backref(4).str().c_str());
			ms_LastFileInfo.uiFileInfo |= eVALID_ACTION;

			if ( strstr(ms_LastFileInfo.aszAction, "file(s) not on client.") )
				ms_LastFileInfo.uiFileInfo |= eMSG_NOSUCHFILE;
			if ( strstr(ms_LastFileInfo.aszAction, "no such file(s)") ) 
				ms_LastFileInfo.uiFileInfo |= eMSG_NOSUCHFILE;
			
			ms_LastFileInfo.uiFileInfo |= eVALID_INFO;
		}
		else if ( PatternOther.match(strbuf, versionResult).matched )
		{
			ms_LastFileInfo.ulKey = DAT_CUtils::GetKeyFromExactString(versionResult.backref(1).str().c_str());
			ms_LastFileInfo.uiFileInfo |= eVALID_KEY;

			ms_LastFileInfo.ulCmdRevision = P4_INVALIDREVISION;
			if ( versionResult.backref(2).str().c_str() != "none" )
			{
				ms_LastFileInfo.uiFileInfo |= eVALID_REVISION;
				ms_LastFileInfo.ulCmdRevision = atol(versionResult.backref(2).str().c_str());
			}

			if ( strstr(versionResult.backref(4).str().c_str(), "file(s) not on client.") )
				ms_LastFileInfo.uiFileInfo |= eMSG_NOSUCHFILE;
			if ( strstr(versionResult.backref(4).str().c_str(), "no such file(s)") )
				ms_LastFileInfo.uiFileInfo |= eMSG_NOSUCHFILE;

			ms_LastFileInfo.uiFileInfo |= eVALID_INFO;
		}
	}
	else 
	{
		static const regex::rpattern filePattern(".+/\\w{4}/0x(\\w{8}) - (.+)");
		static regex::match_results::backref_type fileRef;
		static regex::match_results fileResult;
		fileRef	= filePattern.match(strbuf, fileResult); // Keep only the file name

		if (fileRef.matched)
		{
			ms_LastFileInfo.ulKey = DAT_CUtils::GetKeyFromExactString(fileResult.backref(1).str().c_str());
			ms_LastFileInfo.uiFileInfo |= eVALID_KEY;

			ms_LastFileInfo.ulCmdRevision = P4_INVALIDREVISION;

			if ( strstr(fileResult.backref(2).str().c_str(), "file(s) not on client.") )
					ms_LastFileInfo.uiFileInfo |= eMSG_NOSUCHFILE;
			if ( strstr(fileResult.backref(2).str().c_str(), "no such file(s).") ) 
				ms_LastFileInfo.uiFileInfo |= eMSG_NOSUCHFILE;
			
			ms_LastFileInfo.uiFileInfo |= eVALID_INFO;
		}
	}
}


//------------------------------------------------------------
//   void DAT_CP4Message::ClearProgress()
/// \author    YCharbonneau
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4Message::ClearProgress()
{
	mpo_Progress->OnRefreshBar(0);
}

//------------------------------------------------------------
//   void DAT_CP4Message::FlashProgress()
/// \author    YCharbonneau
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4Message::FlashProgress(BOOL _bForceProgress /* = FALSE */)
{
	if ( !m_bRefreshProgress && ! _bForceProgress) return;

	m_ulProgressCount++;

	if ( m_ulProgressCount % ((m_ulFilesToHandle/50)+1) == 0)
	{
		// If m_ulFilesToHandle is equal to default value, then don't approximate time
		// because we are using a default value
		if ( m_ulFilesToHandle != P4_MAX_FILESTOHANDLE)
		{
			static char ProgressText[MAX_PATH];
			sprintf(ProgressText, "%s...", m_strClientName.c_str());
			mpo_Progress->OnRefreshBarText((float)m_ulProgressCount/(float)m_ulFilesToHandle, ProgressText);
		}
		else
			mpo_Progress->OnRefreshBar((float)m_ulProgressCount/(float)m_ulFilesToHandle);

		// 360 Removed waitcursor because nobody is cancelling it
#ifndef JADEFUSION
		AfxGetApp()->DoWaitCursor(1);
#endif
	}

	if ( m_ulProgressCount == m_ulFilesToHandle )
		m_ulProgressCount=0;
}

//------------------------------------------------------------------------------
