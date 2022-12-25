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
#include "DATCP4ClientUserReopen.h"
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
#define P4_DEFAULT_REOPEN_ARG 2
//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


//------------------------------------------------------------
//   int DAT_CPerforce::P4Reopen	( const std::vector<std::string>& _vFiles ,std::vector<DAT_CP4ClientInfo>* _pvInfo /* = NULL */ )
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Reopen	( const std::vector<std::string>& _vFiles,ULONG _ulChangelist  )
{
	ULONG ulOldChangelist = GetWorkingChangelist(FALSE);
	SetWorkingChangelist(_ulChangelist);

	DAT_CP4ClientUserReopen P4Client ;

	// Calculated # of file to revert
	P4Client.SetNumberFilesToHandle(_vFiles.size());

	ExecuteOperation(P4_DEFAULT_REOPEN_ARG,&P4Client, &DAT_CPerforce::P4Reopen,_vFiles,FALSE);
	
	SetWorkingChangelist(ulOldChangelist);

	return (_vFiles.size() == P4Client.GetNumberAffectedFiles()) ;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Reopen(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Reopen(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{

	char asz_Changelist[MAX_PATH] = {0};
	sprintf(asz_Changelist,"%d",GetWorkingChangelist());

	_pszArgs[0] = "-c";
	_pszArgs[1] = asz_Changelist;

	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_REOPEN,_pClientUser );

	DAT_CP4ClientUserReopen* pClient = (DAT_CP4ClientUserReopen*)(_pClientUser);
	assert(pClient);

	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	//pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}
