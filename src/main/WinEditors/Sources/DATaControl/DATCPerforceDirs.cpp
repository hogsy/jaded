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
//   int DAT_CPerforce::P4Dirs(const char* _psz_P4DirPath,std::list<std::string>& _lstDirs)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Dirs(const char* _psz_P4DirPath,std::list<std::string>& _lstDirs)
{
	PerforceDirsUI UserClient;

	int argCount = 1;

	char *argValues[]=
	{
		const_cast<char*>(_psz_P4DirPath)
	};

	m_Client.SetArgv( argCount, argValues );
	m_Client.Run( P4_DIRS, &UserClient );

	_lstDirs = UserClient.GetDirs();
	return !UserClient.IsError();
}
