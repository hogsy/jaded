//------------------------------------------------------------------------------
//   DATCP4ClientUserDelete.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERDELETE_H__
#define __DATCP4CLIENTUSERDELETE_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "perforceui.h"
#include "DATCPerforce.h"
#include "BIGfiles/BIGfat.h"
#include "DATCP4Message.h"

#include "DATCP4FileSysDelete.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserDelete : public DAT_CP4Message,public PerforceDeleteUI
{
public:

	DAT_CP4ClientUserDelete() : 
    DAT_CP4Message("Delete")
	{
	}

	virtual void Message(Error* pError)
	{
		PerforceDeleteUI::Message(pError);
		DAT_CP4Message::Message(pError);
		
		std::string strMessage;
		char	aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };
		if ( GetLastFileInfo().IsValid() ) 
		{
			DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);
		}

		if ( IsOpenedDelete() && GetLastFileInfo().IsValid())
		{
			m_ulAffectedFiles++;
			strMessage += " [file is opened for delete] " ;
			strMessage += aszBFFilename;
			OutputInfoMessage(strMessage.c_str());

			// Make sure the FAT is up to date when the Delete action succeeded
			BIG_INDEX ulIndex= BIG_ul_SearchKeyToFat(GetLastFileInfo().ulKey);
			strcpy(BIG_P4Action(ulIndex), P4_ACTION_DELETE); 
			strcpy(BIG_P4OwnerName(ulIndex), DAT_CPerforce::GetInstance()->GetUser());
		}
		else if ( IsAlreadyDeleted() && GetLastFileInfo().IsValid()) 
		{
			strMessage += "- [file already deleted] - ";
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
		}
		else if ( IsAlreadyOpened() && GetLastFileInfo().IsValid()) 
		{
			strMessage += "- [file already opened] - "; 
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


	virtual FileSys* File( FileSysType type )
	{
		return new DAT_CP4FileSysDelete();
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
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERDELETE_H__

