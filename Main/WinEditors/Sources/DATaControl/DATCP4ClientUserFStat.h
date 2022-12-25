//------------------------------------------------------------------------------
//   DATCP4ClientUserFStat.h
/// \author    NBeaufils
/// \date      17-Jan-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERFSTAT_H__
#define __DATCP4CLIENTUSERFSTAT_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCPerforce.h"

// Bigfile support
#include "BIGfiles\BIGfat.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "BIGfiles\BIGmerge.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserFstat : public DAT_CP4ClientUser, public PerforceFstatUI
{
public:
	DAT_CP4ClientUserFstat( ) :
	DAT_CP4ClientUser("Refresh"),
	m_pInfo( NULL )
	{
	}

	virtual ~DAT_CP4ClientUserFstat( )
	{
		if( m_pInfo )
			delete m_pInfo;
	}

	const DAT_CP4ClientInfoFStat* GetInfo( ) const
	{
		return m_pInfo;
	}

	virtual void Message(Error* pError)
	{
		PerforceFstatUI::Message(pError);

		std::string strMessage;

		// We were not able to access P4
		if ( IsAccessDenied() ) 
		{
			strMessage += "- [Access Denied] - Server may not be accessible"; 
			OutputErrorMessage(strMessage.c_str());
		}
	}

	virtual void OutputStat( StrDict *varList )
	{
		StrPtr* value;
		BIG_KEY ul_Key = BIG_C_InvalidKey;
		BIG_INDEX ul_Index = BIG_C_InvalidIndex; 

		// Get BF key value
		value = varList->GetVar("depotFile");
		ul_Key = DAT_CUtils::GetKeyFromString(value->Text());
		ul_Index = BIG_ul_SearchKeyToFat(ul_Key);

		m_pInfo = new DAT_CP4ClientInfoFStat; 

		// Get Key value
		m_pInfo->ulKey = ul_Key;

		// Get Owner value
		value  = varList->GetVar("actionOwner") ;
		if ( value )
			strcpy(m_pInfo->aszOwnerName, value->Text());
		else
		{
			value =  varList->GetVar("otherOpen0") ;
			if ( value )
			{
				strcpy(m_pInfo->aszOwnerName, value->Text());
				*strchr(m_pInfo->aszOwnerName, '@') = '\0';
			}
			else
				m_pInfo->aszOwnerName[0] = P4_INVALIDSTRING;
		}

		// Get Server Revision value
		value = varList->GetVar("headRev") ;
		if ( value )
			m_pInfo->ulServerRevision = value->Atoi();	
		else
			m_pInfo->ulServerRevision = 0;

		// Get Client Revision value
		value = varList->GetVar("haveRev") ;
		if ( value )
			m_pInfo->ulClientRevision = value->Atoi();	
		else
			m_pInfo->ulClientRevision = 0;	

		// Get Server Time value
		value = varList->GetVar("headTime") ;
		if ( value )
			m_pInfo->ulTime = value->Atoi();	
		else
			m_pInfo->ulTime = P4_INVALIDTIME;	

		// Get Server Changelist value
		value = varList->GetVar("headChange") ;
		if ( value )
			m_pInfo->ulChangeList = value->Atoi();	
		else	
			m_pInfo->ulChangeList = P4_INVALIDCHANGELIST;	

		// Get Server head action value
		value = varList->GetVar("headAction") ;
		if ( value )
			strcpy(m_pInfo->aszHeadAction, value->Text());
		else	
			m_pInfo->aszHeadAction[0] = P4_INVALIDSTRING;

		// Get Server action value
		value = varList->GetVar("action") ;
		if ( value )
			strcpy(m_pInfo->aszAction, value->Text());
		else	
			m_pInfo->aszAction[0] = P4_INVALIDSTRING;

		// overwriting the value of the action if we need to resolve
		value = varList->GetVar(P4_STATUS_UNRESOLVED) ;
		if ( value )
			strcpy(m_pInfo->aszAction, P4_STATUS_UNRESOLVED);

		FlashProgress();
	}

private:
	DAT_CP4ClientInfoFStat*	m_pInfo;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERFSTAT_H__

