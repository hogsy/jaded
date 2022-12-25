//------------------------------------------------------------------------------
//   DATCP4ClientUserSync.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERRESOLVE_H__
#define __DATCP4CLIENTUSERRESOLVE_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "PerforceUI.h"
#include "DATCP4FileSysSync.h"
#include "DATCP4Message.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserResolve : public DAT_CP4Message, public PerforceSyncUI
{
public:

	DAT_CP4ClientUserResolve( ):
	DAT_CP4Message("Resolve")
	{		
	}

	virtual ~DAT_CP4ClientUserResolve()
	{
	}

	virtual void Message(Error* pError)
	{
		PerforceSyncUI::Message(pError);
		DAT_CP4Message::Message(pError);
		//GetLastFileInfo().ulCmdRevision = P4_INVALIDREVISION;
		
		
		
		if ( pError->IsWarning() ) 
		{
				/*char aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName] = { 0 };
				DAT_CUtils::GetBFFileFromKey(GetLastFileInfo().ulKey,aszBFFilename);
				*/
				StrBuf buf;
				pError->Fmt(buf, EF_PLAIN);
				OutputInfoMessage(buf.Value(),TRUE);

		}
		
		FlashProgress();
	}

	virtual FileSys* File( FileSysType type )
	{
		return new DAT_CP4FileSysSync("");
	}

	void HandleError(Error* pError)
	{
		StrBuf buf;
		pError->Fmt(buf, EF_PLAIN);
		OutputInfoMessage(buf.Value(),TRUE);
	}


private:

};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERSYNC_H__

