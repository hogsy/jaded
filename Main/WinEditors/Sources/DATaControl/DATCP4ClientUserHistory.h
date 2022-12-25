//------------------------------------------------------------------------------
//   DATCP4ClientUserHistory.h
/// \author    FFerland
/// \date      2005-01-31
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERHISTORY_H__
#define __DATCP4CLIENTUSERHISTORY_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

#include "PerforceUI.h"
#include "DATCP4Message.h"

//------------------------------------------------------------------------------
// Structures
//------------------------------------------------------------------------------

struct DAT_SHistoryInfo
{
	std::string		strRevision;
	std::string		strFilename;
	std::string		strChangelist;
	std::string		strDate;
	std::string		strUser;
	std::string		strAction;
	std::string		strDescription;
	std::string		strFileType;
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserHistory : public DAT_CP4Message, public PerforceFilelogUI
{
public:
	DAT_CP4ClientUserHistory( std::vector< DAT_SHistoryInfo* >& _lstEntries )
		: DAT_CP4Message("History"),		
		  m_lstEntries( _lstEntries )
	  {
	  }

	  virtual void Message(Error* error)
	  {
		  PerforceFilelogUI::Message(error);
		  DAT_CP4Message::Message(error);

		  if( IsError( ) )
		  {
			  SetError( true );
		  }
		  else if( IsLine( ) )
		  {
			  DAT_SHistoryInfo* pInfo = new DAT_SHistoryInfo;
				pInfo->strChangelist  = GetChangelist( );
				pInfo->strRevision    = GetRevision( );
				pInfo->strAction      = GetAction( );
				pInfo->strDate        = GetDate( );
				pInfo->strDescription = GetDescription( );
				pInfo->strFilename    = GetFilename( );
				pInfo->strUser        = GetUser( );
				pInfo->strFileType    = GetFileType( );

			  m_lstEntries.push_back( pInfo );
		  }

		  FlashProgress();
	  }

private:
	std::vector< DAT_SHistoryInfo* >&	m_lstEntries;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSEROPENED_H__

