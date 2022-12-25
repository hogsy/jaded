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
#include "DATCP4ClientUserFstatBF.h"
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
#define P4_DEFAULT_FSTAT_ARG 0
//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


//------------------------------------------------------------
//   int DAT_CPerforce::P4FStat(std::vector<DAT_CP4ClientInfo>* _pvP4Files)
/// \author    Nbeaufils
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Fstat(const std::vector<DAT_CP4ClientInfo>& _vFiles)
{
	DAT_CP4ClientUserFstatBF ClientUserFstatBF;

	// Calculated # of file to fstat
	ClientUserFstatBF.SetNumberFilesToHandle(_vFiles.size());

	std::vector<DAT_CP4ClientInfo>::const_iterator Iter;
	char** ppsz_P4FilePathAndRevision = new char*[P4_MAX_FILE];

	UINT iP4MaxFile = 0;
	for ( Iter = _vFiles.begin() ; Iter < _vFiles.end() ; Iter++ )
	{
		// Once we have accumulated enough files, send data to Perforce
		if ( (iP4MaxFile != 0) && (iP4MaxFile == P4_MAX_FILE) )
		{
			m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
			m_Client.Run( P4_FSTAT, &ClientUserFstatBF);

			// delete allocated string
			for(ULONG index = 0 ; index < iP4MaxFile ; index++)
				delete[] ppsz_P4FilePathAndRevision[index];

			iP4MaxFile = 0;
		}

		// Get P4 file path
		char asz_P4FilePath[MAX_PATH];
		DAT_CUtils::GetP4FileFromKey((*Iter).ulKey, asz_P4FilePath, DAT_CPerforce::GetInstance()->GetP4Root().c_str());

		char* psz_P4FilePathAndRevision = new char[MAX_PATH];
		
		if ( (*Iter).ulCmdRevision != P4_INVALIDREVISION ) 
		{
			// Get P4 file path and revision
			sprintf(psz_P4FilePathAndRevision, "%s#%d", asz_P4FilePath, (*Iter).ulCmdRevision);
		}
		else 
		{
			// Get P4 file path
			sprintf(psz_P4FilePathAndRevision, "%s", asz_P4FilePath);
		}

		ppsz_P4FilePathAndRevision[iP4MaxFile] = psz_P4FilePathAndRevision;
		iP4MaxFile++;
	}

	// send remaining data to Perforce
	if ( iP4MaxFile != 0 )
	{
		m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
		m_Client.Run( P4_FSTAT, &ClientUserFstatBF);
	}

	// delete allocated string
	for(ULONG index = 0 ; index < iP4MaxFile ; index++)
		delete[] ppsz_P4FilePathAndRevision[index];

	delete [] ppsz_P4FilePathAndRevision ;

	return	0;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4FStatBF(char* _psz_P4Root)
/// \author    Nbeaufils
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4FStatBF(const char* _psz_P4Root)
{
	// Wipe out the map containing all the files not in the BF but in Perforce
	CleanP4FilesNotInBF();

	DAT_CP4ClientUserFstatBF ClientUserFstatBF;

	// ------------------------------------------------------
	// For each P4 directories, we get the keys inside. This
	// we don't bust the limit (maxresults) of perforce
	char** ppsz_P4DirPath = new char*[P4_DIR_TOTAL];
	std::vector<char*>& vcP4Directories = GetP4Directories(_psz_P4Root);
	
	ClientUserFstatBF.SetNumberFilesToHandle( vcP4Directories.size() / P4_MAX_DIR_LIMIT );
	ClientUserFstatBF.SetRefreshProgress( FALSE );

	UINT uiArgCount =0;
	for ( UINT iP4Directories = 0 ; iP4Directories < P4_DIR_TOTAL ; iP4Directories++ )
	{
		ppsz_P4DirPath[uiArgCount] = vcP4Directories[iP4Directories];
		uiArgCount ++;
		if ( uiArgCount % P4_MAX_DIR_LIMIT == 0)
		{
			ClientUserFstatBF.FlashProgress(TRUE);
			m_Client.SetArgv( uiArgCount, ppsz_P4DirPath );
			m_Client.Run( P4_FSTAT, &ClientUserFstatBF);
			uiArgCount = 0;
		}
	}

	if ( uiArgCount > 0 ) 
	{
		m_Client.SetArgv( uiArgCount, ppsz_P4DirPath );
		m_Client.Run( P4_FSTAT, &ClientUserFstatBF);
		ClientUserFstatBF.FlashProgress(TRUE);
	}

	delete [] ppsz_P4DirPath ;

	return	0;
}

//------------------------------------------------------------
//   void DAT_CPerforce::P4Fstat(std::vector<std::string>& _vFiles)
/// \author    YCharbonneau
/// \date      03-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Fstat(const std::vector<std::string>& _vFiles)
{
	DAT_CP4ClientUserFstatBF P4Client;

	// Calculated # of file to fstat
	P4Client.SetNumberFilesToHandle(_vFiles.size());

	std::vector<std::string> vP4Dirs;
	for ( UINT uiFiles = 0; uiFiles < _vFiles.size(); uiFiles ++ ) 
	{
		UINT uiDirs = 0 ;
		for (  uiDirs ; uiDirs < vP4Dirs.size() ; uiDirs ++ ) 
		{
			if ( _vFiles[uiFiles].find(vP4Dirs[uiDirs]) != -1 ) 
			{ 
				break; 
			}
		}

		if ( uiDirs == vP4Dirs.size() ) 
		{
			std::string strDir(_vFiles[uiFiles],0,_vFiles[uiFiles].length() - FILENAMELENGHT );
			vP4Dirs.push_back(strDir);
		}
	}

	for (  UINT uiDirs = 0 ; uiDirs < vP4Dirs.size() ; uiDirs ++ ) 
	{
		vP4Dirs[uiDirs] += "*";
	}

	ExecuteOperation(P4_DEFAULT_FSTAT_ARG,&P4Client, &DAT_CPerforce::P4Fstat,vP4Dirs,FALSE);
	return !P4Client.IsError();
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4Fstat(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      05/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Fstat(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_FSTAT,_pClientUser );

	DAT_CP4ClientUserFstatBF* pClient = (DAT_CP4ClientUserFstatBF*)(_pClientUser);
	assert(pClient);

	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}