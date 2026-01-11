//------------------------------------------------------------------------------
//   DATCP4ClientUserLogin.h
/// \author    YCharbonneau
/// \date      2005-02-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERLOGIN_H__
#define __DATCP4CLIENTUSERLOGIN_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

#include "PerforceUI.h"
#include "DATCP4Message.h"

//------------------------------------------------------------------------------
// Structures
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserLogin : public PerforceUI
{
public:
	DAT_CP4ClientUserLogin(const std::string& _strPassword ) : 
	m_strPassword(_strPassword)
	{		
	}

	virtual void	Prompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e )
	{
		rsp.Set(m_strPassword.c_str());
	}

	virtual void Message(Error* err)
	{
		StrBuf buf;
		err->Fmt(buf,EF_PLAIN);

		if ( err->IsError() ) 
		{
			SetError(true);
			DAT_CP4Message::OutputErrorMessage(buf.Text());
		}
		else 
		{
			DAT_CP4Message::OutputInfoMessage(buf.Text());
		}
	}

	virtual void HandleError(Error* err)
	{
		StrBuf buf;
		err->Fmt(buf,EF_PLAIN);
		DAT_CP4Message::OutputErrorMessage(buf.Text());

		SetError(true);
	}




private:

	std::string m_strPassword;

};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERLOGIN_H__

