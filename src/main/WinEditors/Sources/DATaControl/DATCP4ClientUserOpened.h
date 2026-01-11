//------------------------------------------------------------------------------
//   DATCP4ClientUser.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSEROPENED_H__
#define __DATCP4CLIENTUSEROPENED_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

#include "PerforceUI.h"
#include "DATCP4Message.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserOpened : public DAT_CP4Message,public PerforceOpenedUI
{
public:
	DAT_CP4ClientUserOpened(std::vector<DAT_CP4ClientInfo>* _pvFiles) : 
	DAT_CP4Message("Opened"),
	m_vFiles(_pvFiles)
	{
	}

	virtual void Message(Error* error)
	{
		DAT_CP4Message::Message(error);
		if ( GetLastFileInfo().IsValid() ) 
		{
			m_vFiles->push_back(GetLastFileInfo());
		}

		FlashProgress();
	}

	std::vector<DAT_CP4ClientInfo>* m_vFiles;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSEROPENED_H__

