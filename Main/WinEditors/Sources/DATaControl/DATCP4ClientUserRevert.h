//------------------------------------------------------------------------------
//   DAT_CP4ClientUserRevert.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERREVERT_H__
#define __DATCP4CLIENTUSERREVERT_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "BIGfiles\BIGfat.h"
#include "DATCP4FileSysSync.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserRevert : public DAT_CP4Message,public PerforceRevertUI
{
public:
	DAT_CP4ClientUserRevert(std::vector<DAT_CP4ClientInfo>* _pvFiles = NULL,BOOL _bRevertUnchanged=FALSE) : 
	DAT_CP4Message("Revert"),
	PerforceRevertUI(),
	m_bRevertUnchanged(_bRevertUnchanged),
	m_pvFiles(_pvFiles)
	{
	
	}

	virtual ~DAT_CP4ClientUserRevert()
	{
	}

	virtual void Message(Error* pError)
	{
		PerforceRevertUI::Message(pError);
		DAT_CP4Message::Message(pError);
		
		std::string strMessage;

		char	aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };
		if ( GetLastFileInfo().IsValid() ) 
		{
			DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);
		}


		if ( IsNotOpened() && GetLastFileInfo().IsValid()) 
		{
			strMessage += " - ";
			strMessage += "File not open "; 
			strMessage += " - ";
			strMessage += aszBFFilename;
			OutputErrorMessage(strMessage.c_str());
		}
		else if ( IsReverted() && GetLastFileInfo().IsValid()) 
		{
			strMessage += " - ";
			strMessage += "File was reverted"; 
			strMessage += " - ";
			strMessage += aszBFFilename;
			OutputInfoMessage(strMessage.c_str(),TRUE);

			BIG_INDEX index = BIG_ul_SearchKeyToFat(GetLastFileInfo().ulKey);
			if ( index != BIG_C_InvalidIndex ) 
			{
				strcpy(BIG_P4Action(index), "");
			}

			m_ulAffectedFiles ++;

			if ( m_pvFiles )
				m_pvFiles->push_back(GetLastFileInfo());
		}
		else if ( !IsError() ) 
		{
			StrBuf buf;
			pError->Fmt(buf, EF_PLAIN);
			OutputInfoMessage(buf.Value(),TRUE);
		}

		FlashProgress();
	}

	std::vector<DAT_CP4ClientInfo>* m_pvFiles;
	BOOL GetRevertUnchanged() { return m_bRevertUnchanged; }

	// If you want to have direct perforce behavior for revert, enable these lines
	// We decided to disable auto get on revert and ask if you want to sync afterwords
	// in order to avoid work loss. It should not happen but if you switch from one big file 
	// to another, you can revert without loosing your work. ( if you actually know what you are doing ) 
	
	//virtual FileSys* File( FileSysType type )
	//{
	//	return new DAT_CP4FileSysSync("");
	//}

private:
	BOOL	m_bRevertUnchanged;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERREVERT_H__

