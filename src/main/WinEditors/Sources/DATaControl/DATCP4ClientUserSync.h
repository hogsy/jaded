//------------------------------------------------------------------------------
//   DATCP4ClientUserSync.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERSYNC_H__
#define __DATCP4CLIENTUSERSYNC_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "PerforceUI.h"
#include "DATCP4FileSysSync.h"
#include "DATCP4Message.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserSync : public DAT_CP4Message, public PerforceSyncUI
{
public:

	DAT_CP4ClientUserSync(	std::vector<DAT_CP4ClientInfo>* _pvFiles = NULL, 
							const std::string& _strBigDir = "",
							BOOL _bForceSync = FALSE):
	DAT_CP4Message("GetLatest(sync)"),
	m_pvFiles(_pvFiles),
	m_strBigDir(_strBigDir),
	m_bForceSync(_bForceSync)
	{		
	}

	virtual ~DAT_CP4ClientUserSync()
	{
	}

	virtual void Message(Error* pError)
	{
		PerforceSyncUI::Message(pError);
		DAT_CP4Message::Message(pError);
		GetLastFileInfo().ulCmdRevision = P4_INVALIDREVISION;
		
		char aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };
		
		if ( GetLastFileInfo().IsValid() ) 
		{
			DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);

			// Are we syncing just a directory ?
			if ( ! m_strBigDir.empty() )
			{
				// File is not in directory, so we don't take it in account
				std::string strBFFileName(aszBFFilename);
				if ( strBFFileName.find(m_strBigDir) == -1 ) 
				{
					FlashProgress();
					return;
				}
			}
		}

		if ( IsUpToDate() && GetLastFileInfo().IsValid() ) 
		{
			std::string strMessage ;
			strMessage += "- [file is up to date] - "; 	
			strMessage += aszBFFilename;
			OutputWarningMessage(strMessage.c_str());
		}
		else if ( IsSync() && GetLastFileInfo().IsValid() )
		{
			m_ulAffectedFiles ++;

			// message is sent directly by filesys sync since the message is comming 
			// before we even get the file.
			if ( m_pvFiles  ) 
			{
				m_pvFiles->push_back(GetLastFileInfo());
			}
		}
		else if ( IsNotOnServer() && GetLastFileInfo().IsValid() ) 
		{
			std::string strMessage ;
			strMessage += "- [file not on server] - "; 	
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
		}
		else if (IsDeleted() && GetLastFileInfo().IsValid() )
		{
			std::string strMessage ;
			strMessage += "- [file has been deleted] - "; 	
			strMessage += aszBFFilename;
			OutputWarningMessage(strMessage.c_str());

			m_ulAffectedFiles ++;
			if ( m_pvFiles  ) 
			{
				m_pvFiles->push_back(GetLastFileInfo());
			}
		}
		else if ( GetLastFileInfo().IsValid() ) 
		{
			m_ulAffectedFiles ++;
			if ( m_pvFiles  ) 
			{
				m_pvFiles->push_back(GetLastFileInfo());
			}
		}
		FlashProgress();
	}

	virtual FileSys* File( FileSysType type )
	{
		return new DAT_CP4FileSysSync(m_strBigDir);
	}

	void HandleError(Error* /*err*/)
	{
	}

	BOOL GetForceSync(){return m_bForceSync;}

private:

	std::vector<DAT_CP4ClientInfo>* m_pvFiles;
	std::string m_strBigDir;
	BOOL m_bForceSync;

};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERSYNC_H__

