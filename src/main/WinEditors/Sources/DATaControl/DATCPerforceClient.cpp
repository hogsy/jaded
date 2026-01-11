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
#include "DATCP4Message.h"



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
//   int DAT_CPerforce::P4Client(const char* _psz_version)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Client(const char* _pszVersion, const std::list<std::string>& _vDirs,std::string& _strDescription,BOOL _bCreate)
{
	std::string strClientViewName = m_Client.GetHost().Value();
	strClientViewName += "-";
	strClientViewName += _pszVersion;

	std::string strView;
	if ( _bCreate ) 
	{
		std::list<std::string>::const_iterator iter;

		for ( iter = _vDirs.begin(); iter != _vDirs.end(); iter ++ ) 
		{
			strView += (*iter).c_str();
			strView += "/... //";
			strView += strClientViewName;
			strView += "/version/";
			std::string strVersionTemp;
			strVersionTemp.assign( strrchr((*iter).c_str(),'/') + 1);
			strView += strVersionTemp;
			strView += "/... ";
			strView += '\n';
			strView += ' ';
		}
	}


	PerforceClientUI UserClient (	strClientViewName.c_str(),
		m_Client.GetHost().Value(),
		m_Client.GetUser().Value(),
		DEFAULT_ROOT_PATH,
		_strDescription.c_str(),
		strView.c_str()
		);



	if ( _bCreate ) 
	{
		int argCount = 1;
		char *argValues[]=
		{
			"-i",
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_CLIENT, &UserClient );
	}
	else
	{
		int argCount =2;
		char *argValues[]=
		{
			"-o",
			const_cast<char*>(strClientViewName.c_str())
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_CLIENT, &UserClient );
		_strDescription = UserClient.GetDescription();
	}

	m_Client.SetClient(strClientViewName.c_str());

	return !UserClient.IsError();
}

// client users
int DAT_CPerforce::P4Clients(std::list<std::string>& _vClients )
{
	PerforceClientsUI UserClient;
	m_Client.Run( P4_CLIENTS, &UserClient );

	_vClients = UserClient.GetClients();
	return !UserClient.IsError();
}
