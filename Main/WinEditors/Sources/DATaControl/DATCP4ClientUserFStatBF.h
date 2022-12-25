//------------------------------------------------------------------------------
//   DATCP4ClientUserFStatBF.h
/// \author    NBeaufils
/// \date      17-Jan-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERFSTATBF_H__
#define __DATCP4CLIENTUSERFSTATBF_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4Message.h"
#include "BIGfiles\BIGfat.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "BIGfiles\BIGmerge.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserFstatBF : public DAT_CP4Message, public PerforceFstatUI
{
public:

	DAT_CP4ClientUserFstatBF( ) :
	DAT_CP4Message("Refreshing(Stat)")
	{		
	}

	virtual void Message(Error* pError)
	{
		PerforceFstatUI::Message(pError);

		std::string strMessage;

		// We were not able to access P4
		if ( IsAccessDenied() ) 
		{
			strMessage += "- [Access Denied] - Server may not be accessible "; 
			OutputErrorMessage(strMessage.c_str());
		}
	}

	virtual void OutputStat( StrDict *varList )
	{
		m_ulAffectedFiles ++;

		StrPtr* value;
		BIG_KEY ul_Key = BIG_C_InvalidKey;
		BIG_INDEX ul_Index = BIG_C_InvalidIndex; 

		// Get File value
		value = varList->GetVar("depotFile");
		ul_Key = DAT_CUtils::GetKeyFromString(value->Text());
		ul_Index = BIG_ul_SearchKeyToFat(ul_Key);

		// Check if key exist in BF
		if ( ul_Index != BIG_C_InvalidIndex )
		{
			// Get Owner value
			value = varList->GetVar("actionOwner") ;
			if ( value )
				strcpy(BIG_P4OwnerName(ul_Index), value->Text());
			else
			{
				value =  varList->GetVar("otherOpen0") ;
				if ( value )
				{
					strcpy(BIG_P4OwnerName(ul_Index), value->Text());
					*strchr(BIG_P4OwnerName(ul_Index), '@') = '\0';
				}
				else
					BIG_P4OwnerName(ul_Index)[0] = P4_INVALIDSTRING;
			}

			// Get Server Revision value
			value = varList->GetVar("headRev") ;
			if ( value )
				BIG_P4RevisionServer(ul_Index) = value->Atoi();
			else
				BIG_P4RevisionServer(ul_Index) = 0;

			// Get Client Revision value
			value = varList->GetVar("haveRev") ;
			if ( value )
				BIG_P4RevisionClient(ul_Index) = value->Atoi();	
			else
				BIG_P4RevisionClient(ul_Index) = 0;

			// Get Server Time value
			value = varList->GetVar("headTime") ;
			if ( value )
				BIG_P4Time(ul_Index) = value->Atoi();	
			else
				BIG_P4Time(ul_Index) = P4_INVALIDTIME;	

			// Get Server Changelist value
			value = varList->GetVar("change") ;
			if ( value )
				BIG_P4ChangeList(ul_Index) = value->Atoi();	
			else	
				BIG_P4ChangeList(ul_Index) = P4_INVALIDCHANGELIST;	

			// Get Server head action value
			value = varList->GetVar("headAction") ;
			if ( value )
				strcpy(BIG_P4HeadAction(ul_Index), value->Text());
			else	
				BIG_P4HeadAction(ul_Index)[0] = P4_INVALIDSTRING;

			// Get Server action value
			value = varList->GetVar("action") ;
			if ( value )
				strcpy(BIG_P4Action(ul_Index), value->Text());
			else	
				BIG_P4Action(ul_Index)[0] = P4_INVALIDSTRING;

			// overwriting the value of the action if we need to resolve
			value = varList->GetVar(P4_STATUS_UNRESOLVED) ;
			if ( value )
				strcpy(BIG_P4Action(ul_Index), P4_STATUS_UNRESOLVED);
		}
		// Key doesn't exist in BF. We need to keep it to tell the user
		else
		{
			DAT_CP4ClientInfoFStat* pInfo = new DAT_CP4ClientInfoFStat; 

			// Get Key value
			pInfo->ulKey = ul_Key;

			// Get Owner value
			value  = varList->GetVar("actionOwner") ;
			if ( value )
				strcpy(pInfo->aszOwnerName, value->Text());
			else
			{
				value =  varList->GetVar("otherOpen0") ;
				if ( value )
				{
					strcpy(pInfo->aszOwnerName, value->Text());
					*strchr(pInfo->aszOwnerName, '@') = '\0';
				}
				else
					pInfo->aszOwnerName[0] = P4_INVALIDSTRING;
			}

			// Get Server Revision value
			value = varList->GetVar("headRev") ;
			if ( value )
				pInfo->ulServerRevision = value->Atoi();	
			else
				pInfo->ulServerRevision = 0;

			// Get Client Revision value
			value = varList->GetVar("haveRev") ;
			if ( value )
				pInfo->ulClientRevision = value->Atoi();	
			else
				pInfo->ulClientRevision = 0;	

			// Get Server Time value
			value = varList->GetVar("headTime") ;
			if ( value )
				pInfo->ulTime = value->Atoi();	
			else
				pInfo->ulTime = P4_INVALIDTIME;	

			// Get Server Changelist value
			value = varList->GetVar("headChange") ;
			if ( value )
				pInfo->ulChangeList = value->Atoi();	
			else	
				pInfo->ulChangeList = P4_INVALIDCHANGELIST;	

			// Get Server head action value
			value = varList->GetVar("headAction") ;
			if ( value )
				strcpy(pInfo->aszHeadAction, value->Text());
			else	
				pInfo->aszHeadAction[0] = P4_INVALIDSTRING;

			// Get Server action value
			value = varList->GetVar("action") ;
			if ( value )
				strcpy(pInfo->aszAction, value->Text());
			else	
				pInfo->aszAction[0] = P4_INVALIDSTRING;

			DAT_CPerforce::GetInstance()->AddP4FileNotInBF(ul_Key, pInfo);
		}

		FlashProgress();
	}
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERFSTATBF_H__

