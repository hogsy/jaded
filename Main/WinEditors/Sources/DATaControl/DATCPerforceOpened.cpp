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
#include "DATCP4ClientUserOpened.h"
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
//   int DAT_CPerforce::P4Opened(ULONG _ul_Changelist)
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Opened(ULONG _ul_Changelist,std::vector<DAT_CP4ClientInfo>* _pvFiles)
{
	DAT_CP4ClientUserOpened UserClient(_pvFiles);

	// lists the opened file within the requested asz_Changelist
	{
		int argCount = 3;

		char asz_Changelist[MAX_PATH];
		sprintf(asz_Changelist,"%d",_ul_Changelist);
		char *argValues[] = 
		{ 
			"-a",
			"-c",
			asz_Changelist
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_OPENED, &UserClient );
	}

	return ! UserClient.IsError();
}

int DAT_CPerforce::P4Opened(std::vector<DAT_CP4ClientInfo>* _pvFiles)
{
	DAT_CP4ClientUserOpened UserClient(_pvFiles);

	m_Client.Run( P4_OPENED, &UserClient );
	return ! UserClient.IsError();
}
