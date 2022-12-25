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
#include "DATCP4ClientUserDelete.h"


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

#define P4_DEFAULT_DELETE_ARG 2

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   void DAT_CPerforce::P4Delete(std::vector<std::string>& _vFiles,BOOL _bAutoSubmit /* = FALSE */))
/// \author    YCharbonneau
/// \date      03-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Delete(const std::vector<std::string>& _vFiles,BOOL _bAutoSubmit /* = FALSE */)
{
	DAT_CP4ClientUserDelete P4Client;

	// Calculated # of file to edit
	P4Client.SetNumberFilesToHandle(_vFiles.size());

	ExecuteOperation(P4_DEFAULT_DELETE_ARG,&P4Client, &DAT_CPerforce::P4Delete,_vFiles,_bAutoSubmit);
	return !P4Client.IsError();
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4Delete(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Delete(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	// setting up default parameters
	char asz_Changelist[MAX_PATH] = {0};
	sprintf(asz_Changelist,"%d",GetWorkingChangelist());
	_pszArgs[0] = "-c";
	_pszArgs[1] = 	asz_Changelist;

	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_DELETE,_pClientUser );

	DAT_CP4ClientUserDelete* pClient = (DAT_CP4ClientUserDelete*)(_pClientUser);
	assert(pClient);

	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}
