//------------------------------------------------------------------------------
//   DAT_CP4ClientUserReopen.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERREOPEN_H__
#define __DATCP4CLIENTUSERREOPEN_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserReopen : public DAT_CP4Message,public PerforceReopenUI
{
public:
	DAT_CP4ClientUserReopen( ) : 
	DAT_CP4Message("Reopen")
	{		
	}

	virtual void Message(Error* pError)
	{
		PerforceReopenUI::Message(pError);
		DAT_CP4Message::Message(pError);

		m_ulAffectedFiles ++;
		FlashProgress();
	}
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERREOPEN_H__

