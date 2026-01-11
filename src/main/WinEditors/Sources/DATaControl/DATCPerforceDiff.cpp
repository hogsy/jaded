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

#include "DATCCompression.h"
#include "DATCP4BFObject.h"

// client users
#include "DATCP4ClientUserDiff.h"

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
#define P4_DEFAULT_DIFF_ARG 1
//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   int DAT_CPerforce::P4Diff()
/// \author    FFerland
/// \date      2005-02-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Diff( BIG_KEY _ulKey  )
{
	BOOL bIsSame = FALSE;
	std::vector<DAT_CP4ClientInfo*> vFileInfo;
	DAT_CP4ClientUserDiff UserClient(&vFileInfo);

	// get the content of the file
	{
		int argCount = 2;

		char szP4FilePath[ MAX_PATH + 1 ];
		DAT_CUtils::GetP4FileFromKey( _ulKey, szP4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str() );

		char* argValues[] =
		{
			"-f",
			szP4FilePath
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_DIFF, &UserClient );
	}

	assert(vFileInfo.size() == 1 && "DAT_CPerforce::P4Diff - Vector contains more than one result after a diff on one key.");

	DAT_CP4ClientInfoDiff* pInfoDiff = (DAT_CP4ClientInfoDiff*)vFileInfo.back();
	bIsSame = DAT_CP4ClientInfoDiff::eFileSame & pInfoDiff->dwFileDiff ;

	// Clean up memory
	delete pInfoDiff;

	return bIsSame;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Diff(std::vector<std::string>& vP4Files, std::vector<DAT_CP4ClientInfo>* _pvFileInfo)
/// \author    Nbeaufils
/// \date      2005-02-16
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Diff(std::vector<std::string>& _vFiles, std::vector<DAT_CP4ClientInfo*>* _pvFileInfo)
{
	DAT_CP4ClientUserDiff P4Client(_pvFileInfo);

	// Calculated # of file to edit
	P4Client.SetNumberFilesToHandle(_vFiles.size());

	ExecuteOperation(P4_DEFAULT_DIFF_ARG,&P4Client, &DAT_CPerforce::P4Diff,_vFiles,FALSE);

	return TRUE;
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4Edit(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Diff(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	// setting up default parameters
	_pszArgs[0] = "-f";

	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_DIFF,_pClientUser );

	DAT_CP4ClientUserDiff* pClient = (DAT_CP4ClientUserDiff*)(_pClientUser);
	assert(pClient);

	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}
