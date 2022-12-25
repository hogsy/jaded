//------------------------------------------------------------------------------
// Filename   :DATCPerforceSync.cpp
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
#include "DATCP4ClientUserSync.h"
#include "DATCP4ClientUserSyncPreview.h"
#include "DATCP4Message.h"


//------------------------------------------------------------------------------
//   int DAT_CPerforce::P4SyncBFFiles(std::vector<std::string> )
/// \author    NBeaufils
/// \date      20-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4SyncBFFiles(std::vector<std::string> &_vP4Files)
{
	static DWORD dwTime;
	dwTime = GetTickCount();

    std::vector<DAT_CP4ClientInfo> vFileInSync ;
	char aszBFPath[MAX_PATH] = {0};

	DAT_CP4ClientUserSync ClientUserSync(&vFileInSync,aszBFPath,0);

	// ------------------------------------------------------
	// For each P4 file, we get the key.
	int argCount = 0;
    char** ppsz_P4File = new char*[P4_FILE_SYNC_NB+1];

    unsigned int i;
    for (i=0; i<_vP4Files.size(); i++,argCount++)
    {
        ppsz_P4File[argCount] = const_cast < char *>(_vP4Files[i].c_str());
    }

    ClientUserSync.SetNumberFilesToHandle(_vP4Files.size() );
    ClientUserSync.SetRefreshProgress( TRUE );

    if ( argCount > 0 ) 
    {
        m_Client.SetArgv( argCount, ppsz_P4File );
        m_Client.Run( P4_SYNC, &ClientUserSync );
    }

    delete [] ppsz_P4File ;

	// ------------------------------------------------------
	// If no error occured during sync, update BF fat
	if ( ! ClientUserSync.IsError())
	{
		// Update in BF all the files that were synced
		UpdateBF(vFileInSync);
	}

	char aszMessage[MAX_PATH] = {0};
	dwTime = GetTickCount() - dwTime;
	int iSecond = (dwTime / 1000) % 60;
	int iMinute = ((dwTime / 1000) / 60) % 60;
	int iHour   = ((dwTime / 1000) / 60) / 60 ;

	sprintf (aszMessage," - Time for Sync: %dh:%dm:%ds",iHour,iMinute,iSecond);
	DAT_CP4Message::OutputInfoMessage(aszMessage,TRUE);

    return 0;
}


//------------------------------------------------------------------------------
//   int DAT_CPerforce::P4SyncBF(const char* _psz_P4Root, BOOL _bForceSync/*=FALSE*/, char* _psz_P4Label/*= NULL*/)
/// \author    NBeaufils
/// \date      20-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4SyncBF(	const char* _psz_P4Root, 
								const char* _psz_BFPath /* = NULL */,
								BOOL _bForceSync/*=FALSE*/,
								char* _psz_P4Label/*= NULL*/,
								ULONG _ul_Changelist /* = P4_INVALIDCHANGELIST*/)
{
	static DWORD dwTime;
	dwTime = GetTickCount();

	char asz_Tmp[P4_MAX_LABEL] = {0};
	if ( _ul_Changelist != P4_INVALIDCHANGELIST ) 
		sprintf(asz_Tmp, "@%d", _ul_Changelist);
	else if ( _psz_P4Label != NULL)
		sprintf(asz_Tmp, "@%s",  _psz_P4Label);

	std::vector<DAT_CP4ClientInfo> vFileInSync ;
	char aszBFPath[MAX_PATH] = {0};
	if ( _psz_BFPath ) 
	{
		strcpy(aszBFPath,_psz_BFPath);
	}

	DAT_CP4ClientUserSync ClientUserSync(&vFileInSync,aszBFPath,_bForceSync);

	// ------------------------------------------------------
	// Calculated # of file to sync. We only set it if BIG_MaxFile() = 0 because only then do we
	// know how many files will be synced. 
	if ( BIG_MaxFile() == 0 )
		ClientUserSync.SetNumberFilesToHandle(DAT_CPerforce::GetInstance()->GetP4FilesNotInBF().size());

	// ------------------------------------------------------
	// For each P4 directories, we get the keys inside. This
	// we don't bust the limit (maxresults) of perforce
	int argCount = 0;
	char** ppsz_P4DirPath = new char*[P4_DIR_TOTAL+1];
	
	if (_bForceSync)
	{
		ppsz_P4DirPath[0] = "-f";
		argCount++; 
	}

    std::vector<char*>& vcP4Directories = GetP4Directories(_psz_P4Root, asz_Tmp);

    ClientUserSync.SetNumberFilesToHandle( vcP4Directories.size() / P4_MAX_DIR_LIMIT );
    ClientUserSync.SetRefreshProgress( FALSE );

    for ( UINT iP4Directories = 0 ; iP4Directories < P4_DIR_TOTAL ; iP4Directories++ )
    {
        ppsz_P4DirPath[argCount] = vcP4Directories[iP4Directories];
        argCount++; 

        // limite max results 
        if ( argCount % P4_MAX_DIR_LIMIT == 0)
        {
            ClientUserSync.FlashProgress( TRUE );
            m_Client.SetArgv( argCount, ppsz_P4DirPath );
            m_Client.Run( P4_SYNC, &ClientUserSync );
            argCount = 0;

            if (_bForceSync)
            {
                ppsz_P4DirPath[0] = "-f";
                argCount++; 
            }
        }
    }

    ClientUserSync.FlashProgress( TRUE );

    if ( argCount > 0 ) 
    {
        m_Client.SetArgv( argCount, ppsz_P4DirPath );
        m_Client.Run( P4_SYNC, &ClientUserSync );
    }

    delete [] ppsz_P4DirPath ;

	// ------------------------------------------------------
	// If no error occured during sync, update BF fat
	if ( ! ClientUserSync.IsError())
	{
		// Update in BF all the files that were synced
		UpdateBF(vFileInSync);
	}
	
	char aszMessage[MAX_PATH] = {0};
	dwTime = GetTickCount() - dwTime;
	int iSecond = (dwTime / 1000) % 60;
	int iMinute = ((dwTime / 1000) / 60) % 60;
	int iHour   = ((dwTime / 1000) / 60) / 60 ;

	sprintf (aszMessage," - Time for Sync: %dh:%dm:%ds",iHour,iMinute,iSecond);
	DAT_CP4Message::OutputInfoMessage(aszMessage,TRUE);

	return 0;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4SyncDir(BIG_INDEX _ul_DirIndex, BOOL _bForceSync/*=FALSE*/)
/// \author    Nbeaufils
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4SyncDir(BIG_INDEX _ul_DirIndex, BOOL _bForceSync/*=FALSE*/)
{
	char    aszBFFilePath[BIG_C_MaxLenPath];
	BIG_ComputeFullName(_ul_DirIndex, aszBFFilePath);

	std::vector<DAT_CP4ClientInfo> vFileInSync ;
	DAT_CP4ClientUserSync ClientUserSync(&vFileInSync,aszBFFilePath);

	// ------------------------------------------------------
	// For each P4 directories, we get the keys inside. This
	// we don't bust the limit (maxresults) of perforce
	int argCount = 0;
	char** ppsz_P4DirPath = new char*[P4_DIR_TOTAL+1];

	if (_bForceSync)
	{
		ppsz_P4DirPath[0] = "-f";
		argCount++; 
	}

	std::vector<char*>& vcP4Directories = GetP4Directories(GetP4Root().c_str());
	ClientUserSync.SetNumberFilesToHandle( vcP4Directories.size() / P4_MAX_DIR_LIMIT );
	ClientUserSync.SetRefreshProgress( FALSE );

	for ( UINT iP4Directories = 0 ; iP4Directories < P4_DIR_TOTAL ; iP4Directories++ )
	{
		ppsz_P4DirPath[argCount] = vcP4Directories[iP4Directories];
		argCount++; 
		
		if ( argCount % P4_MAX_DIR_LIMIT == 0)
		{
			ClientUserSync.FlashProgress(TRUE);
			m_Client.SetArgv( argCount, ppsz_P4DirPath );
			m_Client.Run( P4_SYNC, &ClientUserSync );
			argCount = 0;
		}
	}
	if ( argCount > 0 ) 
	{
		ClientUserSync.FlashProgress(TRUE);
		m_Client.SetArgv( argCount, ppsz_P4DirPath );
		m_Client.Run( P4_SYNC, &ClientUserSync );
	}

	delete [] ppsz_P4DirPath ;

	// --------------------------------------------------
	// If no error occured during sync, update BF fat
	if ( ! ClientUserSync.IsError())
	{
		UpdateBF(vFileInSync);
	}

	return 0;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Sync(BIG_INDEX _ul_KeyIndex, BOOL _bForceSync/*=FALSE*/)
/// \author    Nbeaufils
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Sync(	BIG_INDEX _ul_KeyIndex, 
							BOOL _bForceSync/*=FALSE*/, 
							ULONG _ul_RevisionToSync/*= P4_INVALIDREVISION*/)
{
	char asz_P4FilePath[MAX_PATH];
	DAT_CUtils::GetP4FileFromKey(BIG_FileKey(_ul_KeyIndex), asz_P4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

	if ( _ul_RevisionToSync != P4_INVALIDREVISION)
	{
		char aszP4FilePathAndRevision[MAX_PATH];
		sprintf(aszP4FilePathAndRevision, "%s#%d", asz_P4FilePath, _ul_RevisionToSync);
		strcpy(asz_P4FilePath, aszP4FilePathAndRevision);
	}

	ULONG ulFilesToSync = 1;

	std::vector<DAT_CP4ClientInfo> vFileInSync ;
	DAT_CP4ClientUserSync ClientUserSync(&vFileInSync, "");

	int argCount = 0;
	char* argValues[2];
	if (_bForceSync)
	{
		argValues[0] = "-f";
		argValues[1] = asz_P4FilePath;
		argCount = 2;
	}
	else
	{ 
		argValues[0] = asz_P4FilePath;
		argCount = 1;
	};

	m_Client.SetArgv( argCount, argValues );
	m_Client.Run( P4_SYNC, &ClientUserSync );

	// --------------------------------------------------
	// If no error occured during sync, update BF fat
	if ( ! ClientUserSync.IsError())
	{
		// Update in BF all the files that were synced
		UpdateBF(vFileInSync);
	}
	return 0;
}

//------------------------------------------------------------
//   void DAT_CPerforce::P4Sync(const std::vector<std::string>& _vFiles,std::string _strDir /* = ""*/,BOOL _bForceSync /* = FALSE */ )
/// \author    NBeaufils
/// \date      03-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Sync(const std::vector<std::string>& _vFiles,std::string _strDir /* = ""*/,BOOL _bForceSync /* = FALSE */ )
{
	std::vector<DAT_CP4ClientInfo> vInfoFiles;
	DAT_CP4ClientUserSync P4Client(&vInfoFiles, _strDir,_bForceSync);

	// Calculated # of file to sync
	if ( _vFiles.size() > 0 )
	{
		P4Client.SetRefreshProgress(FALSE);
		P4Client.SetNumberFilesToHandle( _vFiles.size() / P4_MAX_FILE );
	}

	UINT uiDefaultArg = _bForceSync;

	ExecuteOperation(uiDefaultArg,&P4Client, &DAT_CPerforce::P4Sync,_vFiles,FALSE);

	UpdateBF(vInfoFiles);

	return !P4Client.IsError();
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
int DAT_CPerforce::P4Sync(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	DAT_CP4ClientUserSync* pClient = (DAT_CP4ClientUserSync*)(_pClientUser);
	assert(pClient);

	// setting up default parameters
	if ( pClient->GetForceSync() ) 
		_pszArgs[0] = "-f";

	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_SYNC,_pClientUser );

	pClient->FlashProgress(TRUE);
	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}

//------------------------------------------------------------------------------
//   int DAT_CPerforce::P4SyncBFPreview(const char* _psz_P4Root, std::vector<std::string>& _vP4Files)
/// \author    NBeaufils
/// \date      20-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4SyncBFPreview(const char* _psz_P4Root, std::vector<std::string>& _vP4Files)
{
	DAT_CP4ClientUserSyncPreview ClientUserSync(&_vP4Files);

	// ------------------------------------------------------
	// For each P4 directories, we get the keys inside. This
	// we don't bust the limit (maxresults) of perforce
	char** ppsz_P4DirPath = new char*[P4_DIR_TOTAL];
	std::vector<char*>& vcP4Directories = GetP4Directories(_psz_P4Root);
	UINT iP4Directories = 0;
	for ( iP4Directories = 0 ; iP4Directories < P4_DIR_TOTAL ; iP4Directories++ )
	{
		ppsz_P4DirPath[iP4Directories] = vcP4Directories[iP4Directories];
	}
	m_Client.SetArgv( iP4Directories, ppsz_P4DirPath );
	m_Client.Run( P4_SYNC, &ClientUserSync );

	delete [] ppsz_P4DirPath ;

	return 0;
}

//------------------------------------------------------------------------------

