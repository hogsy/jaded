//------------------------------------------------------------------------------
//   DATCP4ClientUserFlush.h
/// \author    NBeaufils
/// \date      19-Jan-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERFLUSH_H__
#define __DATCP4CLIENTUSERFLUSH_H__

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
class DAT_CP4ClientUserFlush : public DAT_CP4Message,public ClientUser
{
public:
	DAT_CP4ClientUserFlush() : 
	DAT_CP4Message("Updating server info")
	{		
	}

	virtual void Message( Error *err )
	{
		if ( err->IsError() )
		{
			StrBuf buf;
			err->Fmt(buf,EF_PLAIN);
			OutputErrorMessage(buf.Value());
		}
	}
};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERFLUSH_H__

