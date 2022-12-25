//------------------------------------------------------------------------------
// Filename   :DATCPerforceSync.cpp
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
int DAT_CPerforce::P4Users(std::list<std::string>& _vUser )
{
	PerforceUsersUI UserClient;
	m_Client.Run( P4_USERS, &UserClient );

	_vUser = UserClient.GetUsers();
	return !UserClient.IsError();
}