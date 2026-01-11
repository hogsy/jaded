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
#include "DATCP4ClientUserAdd.h"
#include "DATCP4Message.h"


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
#define P4_DEFAULT_ADD_ARG 4
//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   void DAT_CPerforce::P4Add(std::vector<std::string>& _vFiles,BOOL _bAutoSubmit /* = FALSE */))
/// \author    YCharbonneau
/// \date      03-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Add(const std::vector<std::string>& _vFiles,BOOL _bAutoSubmit /* = FALSE */)
{
	std::vector<DAT_CP4ClientInfo> vInfo;
	DAT_CP4ClientUserAdd P4Client(&vInfo);

	// Calculated # of file to edit
	P4Client.SetNumberFilesToHandle(_vFiles.size());

	ExecuteOperation(P4_DEFAULT_ADD_ARG,&P4Client, &DAT_CPerforce::P4Add,_vFiles,_bAutoSubmit);

	UpdateBF(vInfo);

	return (_vFiles.size() == P4Client.GetNumberAffectedFiles()) ;
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4Add(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Add(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	// setting up default parameters
	char asz_Changelist[MAX_PATH] = {0};
	sprintf(asz_Changelist,"%d",GetWorkingChangelist());

	_pszArgs[0] = 	"-t";
	_pszArgs[1] = 	"binary+Fl";
	_pszArgs[2] = 	"-c";
	_pszArgs[3] = 	asz_Changelist;

	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_ADD,_pClientUser );

	DAT_CP4ClientUserAdd* pClient = (DAT_CP4ClientUserAdd*)(_pClientUser);
	assert(pClient);

	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	//pClient->ResetAffectedFiles();

	return ulAffectedFiles;
}

//------------------------------------------------------------------------------

