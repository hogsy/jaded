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
#include "DATCP4ClientUserRevert.h"

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

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   int DAT_CPerforce::P4Revert(ULONG ul_Key,ULONG _ul_Changelist);
/// \author    YCharbonneau
/// \date      2005-01-13
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Revert(BIG_KEY ul_Key,BOOL _bSync /* = TRUE */ )
{
	std::vector<DAT_CP4ClientInfo> vFiles;
	DAT_CP4ClientUserRevert UserClient(&vFiles) ;

	// reverting a file 
	char asz_P4FilePath[MAX_PATH] = {0};
	DAT_CUtils::GetP4FileFromKey(ul_Key,asz_P4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

	int argCount = 1;
	char *argValues[] = 
	{ 
		asz_P4FilePath
	};

	m_Client.SetArgv( argCount, argValues );
	m_Client.Run( P4_REVERT, &UserClient );


	// ------------------------------------------------------
	// If no error occured during revert
	if ( !UserClient.IsError() && _bSync)
	{
		std::vector<std::string> vFileToSync;
		for ( UINT ui = 0; ui < vFiles.size();ui++ ) 
		{
			char aszFilename[MAX_PATH] = {0};
			DAT_CUtils::GetP4FileFromKey(vFiles[ui].ulKey,aszFilename,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

			char aszFile[MAX_PATH] = {0};
			sprintf(aszFile,"%s#%d",aszFilename,vFiles[ui].ulCmdRevision);
			vFileToSync.push_back(aszFile);
		}
		if ( vFileToSync.size() > 0 ) 
		{
			// forcing sync of the reverted files
			P4Sync(vFileToSync,"",TRUE);
		}
		UpdateBF(vFiles);
		return 1;
	}

	return 0 ;
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4Revert	( const std::vector<std::string>& _vFiles ,std::vector<DAT_CP4ClientInfo>* _pvInfo /* = NULL */ )
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Revert	( const std::vector<std::string>& _vFiles ,std::vector<DAT_CP4ClientInfo>* _pvInfo /* = NULL */,BOOL _bRevertUnchanged /* = FALSE */ )
{
	DAT_CP4ClientUserRevert P4Client(_pvInfo,_bRevertUnchanged) ;

	// Calculated # of file to revert
	P4Client.SetNumberFilesToHandle(_vFiles.size());

	UINT uiDefaultArg = 0; 
	if ( _bRevertUnchanged ) 
	{
		uiDefaultArg = 1;
	}

	ExecuteOperation(uiDefaultArg,&P4Client, &DAT_CPerforce::P4Revert,_vFiles,FALSE);
	return !P4Client.IsError();
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Revert(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Revert(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	DAT_CP4ClientUserRevert* pClient = (DAT_CP4ClientUserRevert*)(_pClientUser);
	assert(pClient);

	if ( pClient->GetRevertUnchanged() )
	{
		_pszArgs[0] = "-a";
	}
	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_REVERT,_pClientUser );

	
	
	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4RevertChangelist(ULONG _ul_Changelist);
/// \author    YCharbonneau
/// \date      2005-01-13
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4RevertChangelist(ULONG _ul_Changelist,std::vector<std::string>& _vRevertedFiles,BOOL _bSync /* = TRUE */ )
{
	std::vector<DAT_CP4ClientInfo> vFiles;
	DAT_CP4ClientUserRevert UserClient(&vFiles) ;

	// reverting a changelists 
	{
		char asz_Changelist[MAX_PATH] = {0};
		sprintf(asz_Changelist,"%d",_ul_Changelist);

		int argCount = 3;
		char *argValues[] = 
		{ 
			"-c",
				asz_Changelist,
				"//..."
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_REVERT, &UserClient );
	}

	// ------------------------------------------------------
	// If no error occured during revert
	if ( !UserClient.IsError() )
	{
		for ( UINT ui = 0; ui < vFiles.size();ui++ ) 
		{
			char aszFilename[MAX_PATH] = {0};
			DAT_CUtils::GetP4FileFromKey(vFiles[ui].ulKey,aszFilename,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

			char aszFile[MAX_PATH] = {0};
			sprintf(aszFile,"%s#%d",aszFilename,vFiles[ui].ulCmdRevision);
			_vRevertedFiles.push_back(aszFile);
		}
	}

	return !UserClient.IsError();
}


