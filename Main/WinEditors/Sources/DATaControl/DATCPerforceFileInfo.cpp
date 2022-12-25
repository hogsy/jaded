//------------------------------------------------------------------------------
// Filename   :DATCPerforce.cpp
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: Implementation of DATCPerforce
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCPerforce.h"

// client users
#include "DATCP4ClientUserHeader.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------
#define P4_DEFAULT_FILELOG_ARG 0
//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


//------------------------------------------------------------
//   int	DAT_CPerforce::P4FileInfo(BIG_KEY _ulKey,std::vector<DAT_CP4ClientInfoFStat*>* _pvFileInfo)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int	DAT_CPerforce::P4FileInfo(BIG_KEY _ulKey,std::vector<DAT_CP4ClientInfoHeader*>* _pvFileInfo)
{
	// Get P4 file path for key
	char asz_P4FilePath[MAX_PATH];
	DAT_CUtils::GetP4FileFromKey(_ulKey, asz_P4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

	return P4FileInfo(asz_P4FilePath,_pvFileInfo);
}

//------------------------------------------------------------
//   int	DAT_CPerforce::P4FileInfo(const char* _psz_P4FilePath,std::vector<DAT_CP4ClientInfoFStat*>* _pvFileInfo)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int	DAT_CPerforce::P4FileInfo(const char* _psz_P4FilePath,std::vector<DAT_CP4ClientInfoHeader*>* _pvFileInfo)
{
	// This client user will not realy sync the file, 
	// it will abort the sync when reaching header size
	// You can then user DAT_CP4BFObject to retreive the 
	// file info you need from the pushed back buffers 
	//
	// Usage:	DAT_CP4BFObject object(char*); 
	//			object.GetPath();
	//
	// NOTE: You need to destroy pushed back buffer after usage.
	//
	DAT_CP4ClientUserHeader UserClient(_pvFileInfo);
	char *argValues[] = 
	{ 
		"-a",
			const_cast<char*>(_psz_P4FilePath)
	};

	m_Client.SetArgv(2, argValues );
	m_Client.Run( P4_PRINT, &UserClient);

	return !UserClient.IsError();

}

//------------------------------------------------------------
//   void DAT_CPerforce::P4FileInfo(std::vector<std::string>& _vP4Files)
/// \author    YCharbonneau
/// \date      03-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4FileInfo(const std::vector<std::string>& _vP4Files, std::vector<DAT_CP4ClientInfoHeader*>* _pvFileInfo)
{
	DAT_CP4ClientUserHeader P4Client(_pvFileInfo);

	ExecuteOperation(P4_DEFAULT_FILELOG_ARG, &P4Client, &DAT_CPerforce::P4FileInfo, _vP4Files, FALSE);
	return !P4Client.IsError();
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4FileInfo(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4FileInfo(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_PRINT,_pClientUser );

	return 0;
}