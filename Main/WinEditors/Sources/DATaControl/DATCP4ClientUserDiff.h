//------------------------------------------------------------------------------
//   DATCP4ClientUserDiff.h
/// \author    NBeaufils
/// \date      11-Jan-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERDIFF_H__
#define __DATCP4CLIENTUSERDIFF_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4Message.h"
#include "DATCP4FileSysDiff.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserDiff : public DAT_CP4Message, public ClientUser
{
public:
	DAT_CP4ClientUserDiff( std::vector<DAT_CP4ClientInfo*>* _pvFiles = NULL ) : 
	DAT_CP4Message("Diff"),
	m_pvFiles(_pvFiles)
	{
		
	}

	virtual ~DAT_CP4ClientUserDiff()
	{
	}

	virtual void Message( Error *err )
	{
		DAT_CP4Message::Message(err);
		if ( m_pvFiles && GetLastFileInfo().IsValid() ) 
		{
			DAT_CP4ClientInfoDiff* pInfoDiff = new DAT_CP4ClientInfoDiff;
			pInfoDiff->ulKey		= GetLastFileInfo().ulKey;
			pInfoDiff->uiFileInfo	= GetLastFileInfo().uiFileInfo;
			
			if ( GetLastFileInfo().uiFileInfo & eMSG_NOSUCHFILE )
				pInfoDiff->dwFileDiff = DAT_CP4ClientInfoDiff::eFileNew;
			else
				pInfoDiff->dwFileDiff = DAT_CP4ClientInfoDiff::eFileSame;

			m_pvFiles->push_back(pInfoDiff);
		}
		FlashProgress();
	}

	void Diff(FileSys *f1, FileSys *f2, int doPage, char *diffFlags, Error *e);

	virtual FileSys* File( FileSysType type )
	{
		return new DAT_CP4FileSysDiff;
	}

private:
	std::vector<DAT_CP4ClientInfo*>* m_pvFiles;

};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERDIFF_H__

