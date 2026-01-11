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
#include "DATCP4Message.h"
#include "DATCP4ClientUserSync.h"
#include "DATCP4ClientUserResolve.h"


//------------------------------------------------------------------------------
static BOOL s_bAcceptYours = TRUE;
//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------
#define P4_DEFAULT_RESOLVE_ARG 1
//------------------------------------------------------------
//   int DAT_CPerforce::P4Resolve(ULONG ul_Key)
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Resolve(BIG_KEY ul_Key,BOOL _bAcceptYours /* else accept theirs */ )
{
	DAT_CP4ClientUserSync UserClient(NULL);

	// adding a file 
	{
		char asz_P4FilePath[MAX_PATH] = {0};
		DAT_CUtils::GetP4FileFromKey(ul_Key,asz_P4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

		char aszAcceptParam[5] = {0};
		if ( _bAcceptYours ) 
		{	
			strcpy(aszAcceptParam,"-ay");
		}
		else 
		{
			strcpy(aszAcceptParam,"-at");
		}

		int argCount = 2;
		char *argValues[] = 
		{ 
			aszAcceptParam,
			asz_P4FilePath
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_RESOLVE, &UserClient );
	}

	return !UserClient.IsError();
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4Resolve(const std::vector<std::string>& _vFiles,BOOL _bAcceptYours /* else accept theirs */ )
/// \author    YCharbonneau
/// \date      05/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Resolve( const std::vector<std::string>& _vFiles,BOOL _bAcceptYours /* else accept theirs */ )
{
	DAT_CP4ClientUserResolve UserClient;
	s_bAcceptYours = _bAcceptYours;

	// Calculated # of file to edit
	UserClient.SetNumberFilesToHandle(_vFiles.size());


	ExecuteOperation(P4_DEFAULT_RESOLVE_ARG,&UserClient, &DAT_CPerforce::P4Resolve,_vFiles,FALSE);
	return !UserClient.IsError();
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Resolve(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Resolve(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	// setting up default parameters
	char asz_Changelist[MAX_PATH] = {0};
	sprintf(asz_Changelist,"%d",GetWorkingChangelist());
	if ( s_bAcceptYours ) 
	{
		_pszArgs[0] = "-ay";
	}
	else 
	{
		_pszArgs[0] = "-at";
	}

	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_RESOLVE,_pClientUser );

	DAT_CP4ClientUserResolve* pClient = (DAT_CP4ClientUserResolve*)(_pClientUser);
	assert(pClient);

	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}
//------------------------------------------------------------------------------

