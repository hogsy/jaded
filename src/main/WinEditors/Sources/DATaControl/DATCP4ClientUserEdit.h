//------------------------------------------------------------------------------
//   DATCP4ClientUserEdit.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSEREDIT_H__
#define __DATCP4CLIENTUSEREDIT_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4FileSys.h"
#include "BIGfiles\BIGfat.h"
#include "DATCPerforce.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserEdit : public DAT_CP4Message,public PerforceEditUI
{
public:
	DAT_CP4ClientUserEdit( ) :
	DAT_CP4Message("CheckOut(Edit)")
	{	
	}

	virtual void Message(Error* pError)
	{
		PerforceEditUI::Message(pError);
		DAT_CP4Message::Message(pError);

		char	aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };
		if ( GetLastFileInfo().IsValid() ) 
		{
			DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);
		}

		std::string strMessage;
		if ( ( IsOpenedExclusively() || IsCurrentlyOpened() )&& GetLastFileInfo().IsValid()) 
		{
			strMessage += " - Is already opened by another user - " ;
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
        }
		else if ( IsOpened() && GetLastFileInfo().IsValid()) 
		{
			m_ulAffectedFiles ++ ;
			strMessage += "file opened for edit - ";
			strMessage += aszBFFilename;
			OutputInfoMessage(strMessage.c_str());

			// Make sur the FAT is up to date when the Edit action succeeded
			BIG_INDEX ulIndex= BIG_ul_SearchKeyToFat(GetLastFileInfo().ulKey);
			strcpy(BIG_P4Action(ulIndex), P4_ACTION_EDIT); 
			strcpy(BIG_P4OwnerName(ulIndex), DAT_CPerforce::GetInstance()->GetUser());
		}
		else if ( IsOpenedAtOlderRevision() && GetLastFileInfo().IsValid()) 
		{
			strMessage += " Is already opened from an old revision - ";
			strMessage += aszBFFilename;
			OutputWarningMessage(strMessage.c_str());
		}
		else if ( IsCurrentlyOpened()&& GetLastFileInfo().IsValid() ) 
		{
			strMessage += "Currently opened by another user";
			strMessage += "- ";
			strMessage += aszBFFilename;
			OutputWarningMessage(strMessage.c_str());
		}
		else 
		{
			StrBuf buf;
			pError->Fmt(buf, EF_PLAIN);
			OutputErrorMessage(buf.Value());
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
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSEREDIT_H__

