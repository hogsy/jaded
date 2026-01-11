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
#include "DATCP4ClientUserSubmit.h"
#include "EDIstrings.h"
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
//   int DAT_CPerforce::P4Submit(ULONG _ul_Changelist)
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Submit(ULONG& _ul_Changelist,BOOL _bVerifyLinks /* = TRUE  */ )
{
	DAT_CP4ClientUserSubmit ClientUserSubmit;

	std::vector<DAT_CP4ClientInfo> vFileInChangeList;
	std::vector<DAT_CP4ClientInfo>::iterator it;

	// Get the list of files in the changelist
	P4Describe(_ul_Changelist, &vFileInChangeList);

	// before submitting a changelist we must make sure that all the files
	// referenced by the files in this changelist are already in the BF
	// or are in the changelist
	// This way, we wont checkin a file with broken links
	if ( _bVerifyLinks && !VerifyFileLinks(vFileInChangeList))
	{
		M_MF()->MessageBox("Submit failed: Some files where missing in changelist.\n See the Perforce Log pane for more info.", EDI_STR_Csz_OperationAborted, MB_ICONEXCLAMATION | MB_OK);
		return 0;	
	}

	// actual submit 
	{
		int argCount = 2;
		char asz_Changelist[MAX_PATH] = {0};
		sprintf(asz_Changelist,"%d",_ul_Changelist);

		char *argChangeList[]=
		{
			"-c",
				asz_Changelist
		};

		ULONG ulBufferSize = 0 ;
		m_Client.SetArgv( argCount, argChangeList );
		m_Client.Run( P4_SUBMIT, &ClientUserSubmit );
	}

	if ( _ul_Changelist == m_ulWorkingChangelist ) // && !ClientUserSubmit.IsError() ) 
	{
		m_ulWorkingChangelist = -1;
	}

	// --------------------------------------------------------------
	// If no error occured during submit update BF with information
	if ( !ClientUserSubmit.IsError() )
	{
		vFileInChangeList.clear();

		// Get the list of files in the changelist
		P4Describe(ClientUserSubmit.GetChangelistNumber(), &vFileInChangeList);

		_ul_Changelist = ClientUserSubmit.GetChangelistNumber();

		// Update in BF all the files that were submitted
		UpdateBF(vFileInChangeList);

		return 1;
	}

	return 0 ;
}

