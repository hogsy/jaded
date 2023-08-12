//------------------------------------------------------------------------------
//   DATCP4ClientUser.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSER_H__
#define __DATCP4CLIENTUSER_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include <list>
#include "perforceui.h"
#include "DATCP4ClientInfo.h"

#include "EDImainframe.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "BASe\ERRors\ERRasser.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
#define P4_INVALIDREVISION		0xFFFFFFFF
#define P4_INVALIDCHANGELIST	0xFFFFFFFF
#define P4_INVALIDTIME			0xFFFFFFFF
#define P4_INVALIDSTRING		'\0'

#define P4_ACTION_NEW		"new"
#define P4_ACTION_DELETE	"delete"
#define P4_ACTION_ADD		"add"
#define P4_ACTION_EDIT		"edit"
#define P4_ACTION_REMOVE	"removed"

#define P4_MAX_FILESTOHANDLE 5000

extern void EPER_OutputMessage(const char* _pszMessage);
extern "C" BOOL LINK_gb_EscapeDetected;

#if 0

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4Message 
{
public:

	DAT_CP4Message(const char* clientName = NULL, ULONG ulFilesToHandle = P4_MAX_FILESTOHANDLE);

	virtual ~DAT_CP4Message();

	virtual void ResetAffectedFiles()
	{
		m_ulAffectedFiles = 0; 
	}

	virtual ULONG GetNumberAffectedFiles() {return m_ulAffectedFiles;}
	
	virtual void SetNumberFilesToHandle(ULONG ulFilesToHandle) { m_ulFilesToHandle = ulFilesToHandle ; } ;

	virtual void Message( Error *err ) ;
	virtual BOOL CheckLoggedState( Error *err );

	virtual void ClearProgress(); 
	virtual void FlashProgress(BOOL _bForceProgress = FALSE );

	
	static void OutputCriticalErrorMessage(const char* _psz_Message)
	{
		std::string strMessage = "[CRITICAL] -";
		strMessage += _psz_Message;
		strMessage += "\n";
		EPER_OutputMessage(strMessage.c_str());
		M_MF()->MessageBox("Critical error, please consult your data manager ASAP.","[P4:CRITICAL - ABORT]",MB_OK);
		LINK_gb_EscapeDetected = TRUE;
		ms_bLoggedError = TRUE;
	}

	static void OutputErrorKeyMismatchMessage(const char* _psz_Message)
	{
		std::string strMessage = "[CRITICAL] -";
		strMessage += _psz_Message;
		strMessage += "\n";
		EPER_OutputMessage(strMessage.c_str());
		M_MF()->MessageBox(const_cast<char*>(strMessage.c_str()),"[P4:CRITICAL - Key mismatch detected]",MB_OK);
		ms_bLoggedError = TRUE;
	}

	static void OutputErrorMessage(const char* _psz_Message)
	{
		
		ERR_X_Warning(0, _psz_Message, NULL);

		std::string strMessage = "[ERROR] -";
		strMessage += _psz_Message;
		strMessage += "\n";
		
		
		EPER_OutputMessage(strMessage.c_str());
		ms_bLoggedError = TRUE;
	}

	static void OutputWarningMessage(const char* _psz_Message)
	{
		std::string strMessage = "[WARNING] -";
		strMessage += _psz_Message;
		strMessage += "\n";
		EPER_OutputMessage(strMessage.c_str());
		ms_bLoggedError = TRUE;
	}

	static void OutputInfoMessage(const char* _psz_Message, BOOL bForce = FALSE)
	{
		std::string strMessage = "\t[INFO] -";
		strMessage += _psz_Message;
		strMessage += "\n";
		EPER_OutputMessage(strMessage.c_str());
	}

	static void OutputHeaderMessage(const char* _psz_Message, BOOL bForce = FALSE)
	{
		std::string strMessage(_psz_Message);
		strMessage += "\n";
		EPER_OutputMessage(strMessage.c_str());
	}

	static DAT_CP4ClientInfo&	GetLastFileInfo()	{ return ms_LastFileInfo;}
	static void					ResetLoggedError()	{ ms_bLoggedError = FALSE; } 
	static BOOL					GetLoggedError()	{ return ms_bLoggedError; } 
	void SetRefreshProgress		(BOOL _bRefreshProgress) { m_bRefreshProgress = _bRefreshProgress; }

protected:

	std::string m_strClientName;
	static DAT_CP4ClientInfo ms_LastFileInfo;
	ULONG m_ulAffectedFiles;
	static BOOL ms_bLoggedError;
	EDIA_cl_UPDATEDialog* mpo_Progress;
	UINT m_ulProgressCount;
	ULONG m_ulFilesToHandle;
	BOOL m_bRefreshProgress;
};
//------------------------------------------------------------------------------

#endif

#endif //#ifndef __DATCP4CLIENTUSER_H__

