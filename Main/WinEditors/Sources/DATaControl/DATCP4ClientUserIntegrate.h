//------------------------------------------------------------------------------
//   DATCP4ClientUserIntegrate.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERINTEGRATE_H__
#define __DATCP4CLIENTUSERINTEGRATE_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4FileSys.h"
#include "BIGfiles\BIGfat.h"
#include "DATCPerforce.h"
#include "DATCP4FileSysSync.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserIntegrate : public DAT_CP4Message,public PerforceUI
{
public:
	DAT_CP4ClientUserIntegrate( const std::string& _strBranchSpec,const std::string& _strChangelistToIntegrate,BOOL _bReverse ) :
	DAT_CP4Message("Integrate"),
	m_strBranchSpec(_strBranchSpec),
	m_strChangelistToIntegrate(_strChangelistToIntegrate),
	m_bReverse(_bReverse)

	{	
	}

	virtual void Message(Error* pError)
	{
		DAT_CP4Message::Message(pError);
	
		StrBuf buf;
		pError->Fmt(buf, EF_PLAIN); 
		DAT_CP4Message::OutputInfoMessage(buf.Value());

		FlashProgress();
	}

	void HandleError(Error* pError)
	{
		StrBuf buf;
		pError->Fmt(buf, EF_PLAIN); 
		DAT_CP4Message::OutputInfoMessage(buf.Value());
	}

	virtual FileSys* File( FileSysType type )
	{
		return new DAT_CP4FileSysSync("");
	}
	const std::string& GetBranchSpec() { return m_strBranchSpec; }
	const std::string& GetChangelistToIntegrate() { return m_strChangelistToIntegrate; }
	BOOL IsReverse() { return m_bReverse; }
private:
	std::string m_strBranchSpec;
	std::string	m_strChangelistToIntegrate;
	BOOL m_bReverse;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERINTEGRATE_H__

