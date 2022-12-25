//------------------------------------------------------------------------------
//   DATCP4ClientUserFiles.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERFILES_H__
#define __DATCP4CLIENTUSERFILES_H__

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
class DAT_CP4ClientUserFiles : public DAT_CP4Message,public PerforceFilesUI
{
public:

	DAT_CP4ClientUserFiles() : 
	DAT_CP4Message("Files"),
	PerforceFilesUI()
	{
	}

	virtual void Message(Error* pError)
	{
		//PerforceFilesUI::Message(pError);
		DAT_CP4Message::Message(pError);

		FlashProgress();
	}

};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERFILES_H__





