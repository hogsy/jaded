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
#include "DATCP4ClientUserHistory.h"

// dialog 
#include "DIAlogs/DIAhistory_dlg.h"

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
//   int DAT_CPerforce::P4History()
/// \author    FFerland
/// \date      2005-01-31
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4History( ULONG _ul_Key, std::vector<DAT_SHistoryInfo*>& _lstEntries )
{
	DAT_CP4ClientUserHistory UserClient(_lstEntries);

	// lists the opened file within the requested asz_Changelist
	{
		int argCount = 3;

		char szP4FilePath[ MAX_PATH + 1 ];
		DAT_CUtils::GetP4FileFromKey( _ul_Key, szP4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str() );

		char* argValues[] =
		{
			"-t",
				"-l",
				szP4FilePath
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_FILELOG, &UserClient );
	}

	return ! UserClient.IsError();
}

//------------------------------------------------------------
//   int DAT_CPerforce::ShowHistory()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4ShowHistory(ULONG ul_Key)
{
	EDIA_cl_HistoryDialog o_HistoryDlg(ul_Key);
	o_HistoryDlg.DoModal();

	return 0;
}
