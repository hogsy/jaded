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
#include "DATCP4ClientUserFiles.h"
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
//   int DAT_CPerforce::P4Files(char* _psz_P4FilePath,std::vector<std::string>& _vFile )
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Files(const char* _psz_P4DirPath)
{
	DAT_CP4ClientUserFiles ClientUserFiles;

	// ------------------------------------------------------
	// For each P4 directories, we get the keys inside. This
	// we don't bust the limit (maxresults) of perforce
	char** ppsz_P4DirPath = new char*[P4_DIR_TOTAL];
	std::vector<char*>& vcP4Directories = GetP4Directories(_psz_P4DirPath);
	UINT iP4Directories = 0;
	for ( iP4Directories = 0 ; iP4Directories < P4_DIR_TOTAL ; iP4Directories++ )
	{
		ppsz_P4DirPath[iP4Directories] = vcP4Directories[iP4Directories];
	}
	m_Client.SetArgv( iP4Directories, ppsz_P4DirPath );
	m_Client.Run( P4_FILES, &ClientUserFiles );

	delete [] ppsz_P4DirPath ;

	return !ClientUserFiles.IsError();
}