
//------------------------------------------------------------------------------
// Filename   :DATCP4ClientUserChanges.cpp
/// \author    YCharbonneau
/// \date      2005-01-13
/// \par       Description: Implementation of DATCP4ClientUserChanges
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCP4ClientUserChanges.h"


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void DAT_CP4ClientUserChanges::OutputInfo( char level, const_char *data )
{
	// this is the output format we are getting from perforce
	// "Change 91 on 2005/01/12 by ycharbonneau@PC-YCHARBONNEAU *pending* 'Working changelist '"

	static const regex::rpattern patternPending("Change (.+) on ([0-9]*/[0-9]*/[0-9]*) by (.+)@(.+) \\*(pending)\\* '(.+)'");
	static const regex::rpattern patternSubmitted("Change (.+) on ([0-9]*/[0-9]*/[0-9]*) by (.+)@(.+) '(.+)'");
	static const regex::rpattern FormatPattern("(\n\r|\n)", "-", regex::GLOBAL);
	regex::match_results results;
	regex::subst_results substResults;

	std::string str = data;

	if (patternPending.match(str, results).matched)
	{
		ULONG ulChangelist = atol(results.backref(1).str().c_str());
		DAT_TChangelistInfo::iterator iter = m_pChangelistInfo->find(ulChangelist);
		PerforceChangelistInfo* pChangelistInfo = NULL;
		
		// trying to find changelist within the map 
		if ( iter != m_pChangelistInfo->end() ) 
		{
			pChangelistInfo = iter->second;
		}
		else 
		{
			pChangelistInfo = new PerforceChangelistInfo;
		}

		pChangelistInfo->strChangelist = results.backref(1).str();
		pChangelistInfo->strDate = results.backref(2).str();
		pChangelistInfo->strUser = results.backref(3).str();
		pChangelistInfo->strClient = results.backref(4).str();
		pChangelistInfo->strStatus = results.backref(5).str();
		pChangelistInfo->strDescription = results.backref(6).str();

		FormatPattern.substitute(pChangelistInfo->strDescription, substResults);

		
		if ( iter == m_pChangelistInfo->end() ) 
		{
			m_pChangelistInfo->insert(std::make_pair(ulChangelist,pChangelistInfo));
		}

	}
	else if (patternSubmitted.match(str, results).matched)
	{
		ULONG ulChangelist = atol(results.backref(1).str().c_str());
		DAT_TChangelistInfo::iterator iter = m_pChangelistInfo->find(ulChangelist);
		PerforceChangelistInfo* pChangelistInfo = NULL;

		// trying to find changelist within the map 
		if ( iter != m_pChangelistInfo->end() ) 
		{
			pChangelistInfo = iter->second;
		}
		else 
		{
			pChangelistInfo = new PerforceChangelistInfo;
		}

		pChangelistInfo->strChangelist = results.backref(1).str();
		pChangelistInfo->strDate = results.backref(2).str();
		pChangelistInfo->strUser = results.backref(3).str();
		pChangelistInfo->strClient = results.backref(4).str();
		pChangelistInfo->strStatus = "submitted";
		pChangelistInfo->strDescription = results.backref(5).str();
		FormatPattern.substitute(pChangelistInfo->strDescription, substResults);


		if ( iter == m_pChangelistInfo->end() ) 
		{
			m_pChangelistInfo->insert(std::make_pair(ulChangelist,pChangelistInfo));
		}
	}

	FlashProgress();
}




