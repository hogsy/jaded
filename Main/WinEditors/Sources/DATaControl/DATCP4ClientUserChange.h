//------------------------------------------------------------------------------
//   DATCP4ClientUserChange.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERCHANGE_H__
#define __DATCP4CLIENTUSERCHANGE_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCPerforce.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserChange : public DAT_CP4Message,public PerforceChangeUI
{
public:

	DAT_CP4ClientUserChange( const char* _pszChange,const char* _pszClient, const char* _pszUser,const char* _pszDescription,const char* _Files ) : 
	DAT_CP4Message("Change"),
	PerforceChangeUI(_pszChange,_pszClient,_pszUser,_pszDescription,_Files)
	{
	}
	
	virtual void Message( Error *pError )
	{
		DAT_CP4Message::CheckLoggedState( pError );

		PerforceChangeUI::Message(pError);
		if ( pError->IsError() ) 
		{
			StrBuf buf;
			pError->Fmt(buf, EF_PLAIN);
			OutputErrorMessage(buf.Value());
		}
		FlashProgress();
	}



	ULONG GetNewChangelist() { return atol(GetChangeNum().c_str()) ; } 

private:

};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERCHANGE_H__





