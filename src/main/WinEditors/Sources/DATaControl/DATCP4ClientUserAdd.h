//------------------------------------------------------------------------------
//   DATCP4ClientUserAdd.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERADD_H__
#define __DATCP4CLIENTUSERADD_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4Message.h"
#include "DATCPerforce.h"

// Bigfile support
#include "BIGfiles\BIGfat.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserAdd : public DAT_CP4Message,public PerforceAddUI
{
public:

	DAT_CP4ClientUserAdd(std::vector<DAT_CP4ClientInfo>* _pvFile) : 
    DAT_CP4Message("Add"),
	m_pvFiles(_pvFile)
	{
	
	}
	
	virtual ~DAT_CP4ClientUserAdd()
	{
	}

	virtual void InputData(StrBuf* strbuf, Error* e)
	{}

		
	virtual void Message(Error* pError)
	{
		// we do not want to output infos
		PerforceAddUI::Message(pError);
		DAT_CP4Message::Message(pError);
		
		std::string strMessage;
		char	aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };
		if ( GetLastFileInfo().IsValid() ) 
		{
			DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);
		}

		if ( IsOpenedAdd() && GetLastFileInfo().IsValid()) 
		{
			m_ulAffectedFiles ++;
			// no errors where found for this file 
			strMessage += "- [file opened for add] - ";
			strMessage += aszBFFilename;
			OutputInfoMessage(strMessage.c_str());

			if ( m_pvFiles  ) 
			{
				m_pvFiles->push_back(GetLastFileInfo());
			}
			
			// Make sure the FAT is up to date when the Add action succeeded
			BIG_INDEX ulIndex= BIG_ul_SearchKeyToFat(GetLastFileInfo().ulKey);
			strcpy(BIG_P4Action(ulIndex), P4_ACTION_ADD); 
			strcpy(BIG_P4OwnerName(ulIndex), DAT_CPerforce::GetInstance()->GetUser());
		}
		else if ( IsCurrentlyOpened() && GetLastFileInfo().IsValid()) 
		{
			strMessage += "- [file already opened] - "; 
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
		}
		else if ( IsExistingFile() && GetLastFileInfo().IsValid()) 
		{
			strMessage += "- [file already exists] - "; 
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
		}
		else 
		{
			StrBuf buf;
			pError->Fmt(buf, EF_PLAIN);
			OutputInfoMessage(buf.Value(),TRUE);
		}

		FlashProgress();
	}

	void HandleError(Error* pError)
	{
		if ( pError->IsError() )
		{
			StrBuf buf;
			pError->Fmt(buf, EF_PLAIN); 
			OutputDebugString(buf.Value());
		}
	}

private:
	std::vector<DAT_CP4ClientInfo>* m_pvFiles;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERADD_H__

