//------------------------------------------------------------------------------
//   DATCP4ClientUserDescribe.h
/// \author    NBeaufils
/// \date      10-Feb-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERDESCRIBE_H__
#define __DATCP4CLIENTUSERDESCRIBE_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4Message.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserDescribe : public DAT_CP4Message, public ClientUser
{
public:

	DAT_CP4ClientUserDescribe(std::vector<DAT_CP4ClientInfo>* _pvFiles = NULL):
    DAT_CP4Message("Describe"),
	m_pvFiles(_pvFiles)
	{
	}

	virtual ~DAT_CP4ClientUserDescribe()
	{
	}

	virtual void Message(Error* pError)
	{
		DAT_CP4Message::Message(pError);

		if ( GetLastFileInfo().IsValid()) 
		{
			if ( m_pvFiles  ) 
			{
				m_pvFiles->push_back(GetLastFileInfo());
			}
		}

		FlashProgress();
	}

	virtual void OutputText(const_char* data, int /*length*/)
	{
	}

private:
	std::vector<DAT_CP4ClientInfo>* m_pvFiles;

};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERDESCRIBE_H__

