//------------------------------------------------------------------------------
//   DATCP4ClientUserSyncPreview.h
/// \author    Nbeaufils
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERSYNCPREVIEW_H__
#define __DATCP4CLIENTUSERSYNCPREVIEW_H__

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
class DAT_CP4ClientUserSyncPreview : public DAT_CP4Message, public PerforceSyncUI
{
public:

	DAT_CP4ClientUserSyncPreview( std::vector<std::string>* _pvP4Files = NULL )  : 
	DAT_CP4Message("Fetching file information(sync preview)"),
	m_pvP4Files(_pvP4Files),
	m_strBigDir("")
	{		
	}

	virtual ~DAT_CP4ClientUserSyncPreview()
	{
	}

	virtual void Message(Error* pError)
	{
		PerforceSyncUI::Message(pError);
		DAT_CP4Message::Message(pError);
		GetLastFileInfo().ulCmdRevision = P4_INVALIDREVISION;

		char	aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };
		if ( GetLastFileInfo().IsValid() ) 
		{
			DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);
		}

		if ( IsUpToDate() && GetLastFileInfo().IsValid() ) 
		{
		}
		else if ( IsNotOnServer() && GetLastFileInfo().IsValid() ) 
		{
		}
		else if (IsDeleted() && GetLastFileInfo().IsValid() )
		{
		}
		else if ( IsSync() && GetLastFileInfo().IsValid() )
		{
			// message is sent directly by filesys sync since the message is comming 
			// before we even get the file.
			if ( m_pvP4Files  ) 
			{
				m_pvP4Files->push_back(aszBFFilename);
			}
		}
		else if ( GetLastFileInfo().IsValid() ) 
		{
			if ( m_pvP4Files  ) 
			{
				m_pvP4Files->push_back(aszBFFilename);
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

private:
	std::vector<std::string>* m_pvP4Files;
	std::string m_strBigDir;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERSYNC_H__

