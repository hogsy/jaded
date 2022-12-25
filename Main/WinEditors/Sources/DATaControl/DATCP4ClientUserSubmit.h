//------------------------------------------------------------------------------
//   DATCP4ClientUserSubmit.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERSUBMIT_H__
#define __DATCP4CLIENTUSERSUBMIT_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4Message.h"
#include "DATCP4FileSysSubmit.h"
#include "BIGfiles/BIGdefs.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserSubmit : public DAT_CP4Message, public PerforceSubmitUI
{
public:
	DAT_CP4ClientUserSubmit( ) : 
	DAT_CP4Message("CheckIn(submit)")
	{		
	}

	virtual ~DAT_CP4ClientUserSubmit()
	{
	}

	virtual void Message(Error* pError)
	{
		PerforceSubmitUI::Message(pError);
		DAT_CP4Message::Message(pError);

		char	aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };

		if ( GetLastFileInfo().IsValid() ) 
		{
			DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);
		}

		std::string strMessage;
		if ( IsNotFoundLocal() && GetLastFileInfo().IsValid()) 
		{
			strMessage += "- [file is not found localy] - "; 	
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
		}
		else if ( IsEmptyChange() && GetLastFileInfo().IsValid()) 
		{
			strMessage += "- [change list is empty ]- "; 	
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
		}
		else if ( IsMustResolve() ) 
		{
			const std::list<std::string>& vFile = GetMustResolveFiles();
			std::list<std::string>::const_reference ref = vFile.back();
				
			strMessage += "- [submitting file involves conflict] - "; 
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
			// (NOTE: remove any action that was previously there)
			BIG_INDEX ulIndex= BIG_ul_SearchKeyToFat(GetLastFileInfo().ulKey);
			strcpy(BIG_P4Action(ulIndex),"unresolved") ; 
		}
		else if ( !IsError() || pError->IsError() ) 
		{
			StrBuf buf;
			pError->Fmt(buf, EF_PLAIN);
			OutputInfoMessage(buf.Value(),TRUE);
		}

		FlashProgress();
	}


	void HandleError(Error* pError)
	{
		PerforceSubmitUI::HandleError(pError);

		std::string strMessage;
		if ( IsNotFoundLocal() ) 
		{
			StrBuf buf;
			pError->Fmt(buf, EF_PLAIN);
			OutputErrorMessage(buf.Value());
		}
	}

	virtual FileSys* File( FileSysType type )
	{
		return new DAT_CP4FileSysSubmit;
	}

private : 
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERSUBMIT_H__

