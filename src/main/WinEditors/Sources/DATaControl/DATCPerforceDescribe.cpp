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
#include "DATCP4ClientUserDescribe.h"

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
//   int DAT_CPerforce::P4Describe(ULONG _ul_Changelist,std::string& _strDescription)
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Describe(ULONG _ul_Changelist,std::string& _strDescription)
{
	PerforceDescribeUI UserClient(NULL);

	// lists the opened file within the requested asz_Changelist
	{
		int argCount = 1;

		char asz_Changelist[MAX_PATH];
		sprintf(asz_Changelist,"%d",_ul_Changelist);
		char *argValues[] = 
		{ 
			asz_Changelist
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_DESCRIBE, &UserClient );
	}
	_strDescription = UserClient.GetDescription();

	return ! UserClient.IsError();
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Describe(ULONG _ul_Changelist,PerforceChangelistInfo* _pChangelistInfot)
/// \author    Nbeaufils
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Describe(ULONG _ul_Changelist, PerforceChangelistInfo* _pChangelistInfo)
{
	PerforceDescribeUI UserClient(_pChangelistInfo);

	// lists the opened file within the requested asz_Changelist
	{
		int argCount = 1;

		char asz_Changelist[MAX_PATH];
		sprintf(asz_Changelist,"%d",_ul_Changelist);
		char *argValues[] = 
		{ 
			asz_Changelist
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_DESCRIBE, &UserClient );
	}
	return ! UserClient.IsError();
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Describe(ULONG _ul_Changelist, std::vector<DAT_CP4ClientInfo*>* _pvFileInfo)
/// \author    Nbeaufils
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Describe(ULONG _ul_Changelist, std::vector<DAT_CP4ClientInfo>* _pvFileInfo)
{
	DAT_CP4ClientUserDescribe ClientUserDescribe(_pvFileInfo);

	// lists the opened file within the requested asz_Changelist
	{
		int argCount = 2;

		char asz_Changelist[MAX_PATH];
		sprintf(asz_Changelist,"%d",_ul_Changelist);
		char *argValues[] = 
		{ 
			"-s",
				asz_Changelist
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_DESCRIBE, &ClientUserDescribe);
	}
	return 0;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4DescribeIntegral(ULONG _ul_Changelist,std::string& _strWholeText)
/// \author    FFerland
/// \date      2005-02-03
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4DescribeIntegral(ULONG _ul_Changelist,std::string& _strWholeText)
{
	PerforceDescribeUI UserClient(NULL);

	// lists the opened file within the requested asz_Changelist
	{
		int argCount = 1;

		char asz_Changelist[MAX_PATH];
		sprintf(asz_Changelist,"%d",_ul_Changelist);
		char *argValues[] = 
		{ 
			asz_Changelist
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_DESCRIBE, &UserClient );
	}
	_strWholeText = UserClient.GetIntegralText();

	return ! UserClient.IsError();
}
