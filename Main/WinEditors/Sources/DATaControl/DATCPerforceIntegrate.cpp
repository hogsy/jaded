//------------------------------------------------------------------------------
// Filename   :DATCPerforceIntegrate.cpp
/// \author    YCharbonneau
/// \date      2005-06-29
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
#include "DATCP4ClientUserIntegrate.h"
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
#define P4_DEFAULT_INTEGRATE_ARG 6
//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


//------------------------------------------------------------
//   void DAT_CPerforce::P4Integrate(std::vector<std::string>& _vFiles))
/// \author    YCharbonneau
/// \date      2005-06-29
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Integrate(const std::string& _strBranchSpec,
							   const std::vector<std::string>& _vFiles,
							   BOOL _bReverse)
{
	DAT_CP4ClientUserIntegrate P4Client(_strBranchSpec,"",_bReverse);
	// setting up default parameters
	char asz_Changelist[MAX_PATH] = {0};
	sprintf(asz_Changelist,"%d",GetWorkingChangelist());

	// Calculated # of file to edit
	P4Client.SetNumberFilesToHandle(_vFiles.size());

	UINT uiArg = P4_DEFAULT_INTEGRATE_ARG;
	if ( _bReverse ) 
	{
		uiArg++;
	}

	ExecuteOperation(uiArg,&P4Client, &DAT_CPerforce::P4Integrate,_vFiles,FALSE);

	return !P4Client.IsError();
}

//------------------------------------------------------------
//   void DAT_CPerforce::P4Integrate(std::vector<std::string>& _vFiles))
/// \author    YCharbonneau
/// \date      2005-06-29
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Integrate(const std::string& _strBranchSpec,
							   const std::string& _strChangelist,BOOL _bReverse )
{
	DAT_CP4ClientUserIntegrate P4Client(_strBranchSpec,_strChangelist,_bReverse);
	
	// for changelist : integ -b T-PS2-Production->T-PS2-Release -s //...@79,@79  

	// setting up default parameters
	char asz_Changelist[MAX_PATH] = {0};
	sprintf(asz_Changelist,"%d",GetWorkingChangelist());
	std::string strPath = "//...@" + _strChangelist + ",@" + _strChangelist;
	
	char** aszArgs = new char*[7];

	UINT uiArgCount = 0;

	aszArgs[uiArgCount++] = "-d";
	aszArgs[uiArgCount++] = "-c";
	aszArgs[uiArgCount++] = 	asz_Changelist;
	aszArgs[uiArgCount++] = "-b";
	aszArgs[uiArgCount++] = const_cast<char*>(_strBranchSpec.c_str());
	aszArgs[uiArgCount++] = const_cast<char*>(strPath.c_str());
	
	if ( _bReverse ) 
	{
		aszArgs[uiArgCount++] = "-r";
	}

	m_Client.SetArgv( uiArgCount, aszArgs );
	m_Client.Run(P4_INTEGRATE,&P4Client );

	delete[] aszArgs;
	ULONG ulAffectedFiles = P4Client.GetNumberAffectedFiles();
	P4Client.ResetAffectedFiles();

	return !P4Client.IsError();
}



//------------------------------------------------------------
//   int DAT_CPerforce::P4Integrate(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
/// \author    YCharbonneau
/// \date      2005-06-29
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Integrate(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser)
{
	char asz_Changelist[MAX_PATH] = {0};
	sprintf(asz_Changelist,"%d",GetWorkingChangelist());
	
	DAT_CP4ClientUserIntegrate* pClient = (DAT_CP4ClientUserIntegrate*)(_pClientUser);
	assert(pClient);

	// for files : integ -b T-PS2-Production->T-PS2-Release -s //kk-bf-test/version/T-PS2-Production/0x00/0000/0x00000010 //...
	// for all : integ -b T-PS2-Production->T-PS2-Release //...
	_pszArgs[0] = "-c";
	_pszArgs[1] = asz_Changelist;
	_pszArgs[2] = "-b";
	_pszArgs[3] = const_cast<char*>(pClient->GetBranchSpec().c_str());
	_pszArgs[4] = "-d";
	_pszArgs[5] = "-s";

	if ( pClient->IsReverse() ) 
	{
		_pszArgs[6] = "-r";
	}


	m_Client.SetArgv( _uiArgCount, _pszArgs );
	m_Client.Run(P4_INTEGRATE,_pClientUser );


	ULONG ulAffectedFiles = pClient->GetNumberAffectedFiles();
	pClient->ResetAffectedFiles();
	return ulAffectedFiles;
}