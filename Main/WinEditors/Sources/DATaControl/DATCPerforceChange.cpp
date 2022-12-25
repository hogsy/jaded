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
#include "DATCP4ClientUserChange.h"
#include "DATCP4ClientUserChanges.h"

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
//   ULONG DAT_CPerforce::P4Change(char* _psz_ChangeListName)
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG DAT_CPerforce::P4Change(ULONG _ul_Changelist,const char* _psz_descritpion,const char* _Files)
{
	char asz_Changelist[MAX_PATH];

	if( _ul_Changelist == -1 ) 
	{
		sprintf(asz_Changelist,"new");
	}
	else
	{
		sprintf(asz_Changelist,"%d",_ul_Changelist);
	}

	DAT_CP4ClientUserChange UserClient(asz_Changelist,m_Client.GetClient().Value(),m_Client.GetUser().Value(),_psz_descritpion,_Files);

	// renaming the default asz_Changelist a file 
	{
		int argCount = 1;
		char *argValues[] = 
		{ 
			"-i"
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_CHANGE, &UserClient );
	}

	if ( _ul_Changelist == -1 ) 
		return UserClient.GetNewChangelist();
	else 
		return _ul_Changelist;
}


//------------------------------------------------------------
//   int DAT_CPerforce::P4Change(char* _psz_ChangeListName)
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Changes(const char* _psz_username,
							 const char* _psz_status,
							 const char*  _psz_MaxChangelist,
							 DAT_TChangelistInfo* _pChangelist )
{
	DAT_CP4ClientUserChanges UserClient(_pChangelist);


	// renaming the default asz_Changelist a file 
	{
		int argCount = 0;
		char *argValues[7];

		if (  _psz_status && _psz_status[0] != 0 )
		{
			argValues[argCount++] = "-s";
			argValues[argCount++] = const_cast<char*>(_psz_status);
		}


		if (  _psz_username && _psz_username[0] != 0 ) 
		{
			argValues[argCount++] = "-u";
			argValues[argCount++] = const_cast<char*>(_psz_username);
		}

		if ( _psz_MaxChangelist && _psz_MaxChangelist[0] != 0 ) 
		{
			argValues[argCount++] = "-m";
			argValues[argCount++] = const_cast<char*>(_psz_MaxChangelist);
		}

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_CHANGES, &UserClient );
	}

	return 0;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4ChangeDelete(ULONG _ul_Changelist)
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4ChangeDelete(ULONG _ul_Changelist)
{
	PerforceChangeDeleteUI UserClient;

	// renaming the default asz_Changelist a file 
	{
		int argCount = 2;
		char asz_Changelist[MAX_PATH];
		sprintf(asz_Changelist,"%d",_ul_Changelist);
		char *argValues[] = 
		{ 
			"-d",
				asz_Changelist
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_CHANGE, &UserClient );
	}

	if ( _ul_Changelist == m_ulWorkingChangelist ) 
	{
		m_ulWorkingChangelist = -1;
	}

	return !UserClient.IsError();
}
