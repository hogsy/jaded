//------------------------------------------------------------------------------
//   DATCP4ClientUserChanges.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERCHANGES_H__
#define __DATCP4CLIENTUSERCHANGES_H__

#if 0

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
class DAT_CP4ClientUserChanges : public DAT_CP4Message,public ClientUser
{
public:

	DAT_CP4ClientUserChanges ( DAT_TChangelistInfo* _pChangelistInfo ) : 
    DAT_CP4Message("Changes"),
	m_pChangelistInfo(_pChangelistInfo)
	{
	}

	virtual void	OutputInfo( char level, const_char *data );
	virtual void	Message(Error *err )
	{
		CheckLoggedState(err);
		ClientUser::Message(err);

		if ( err->IsError() ) 
		{
			StrBuf Message;
			err->Fmt(Message,EF_PLAIN);
			DAT_CP4Message::OutputErrorMessage(Message.Value());
		}
	}

private:

	DAT_TChangelistInfo* m_pChangelistInfo;


};

#endif

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERCHANGES_H__





