/*$T EDIFileFilters.cpp GC!1.40 09/02/99 11:16:34 */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDIFileFilters.h"
#include "Greta/regexpr2.h"



/*
===================================================================================================
===================================================================================================
*/
EDI_cl_FileFilter::EDI_cl_FileFilter():
m_bFilterEnable(FALSE),
m_FilterType(Filter_ShowOnly)
{
}

/*
===================================================================================================
===================================================================================================
*/
EDI_cl_FileFilter::~EDI_cl_FileFilter()
{
}

/*
===================================================================================================
===================================================================================================
*/
void EDI_cl_FileFilter::SetFilter(const CString& strFilter, eFilterType FilterType)
{
	if ( FilterType == Filter_RegularExp)
	{
		m_strFilterDialogRegEx = strFilter ; 
		m_strRegularEx = strFilter ;
		m_FilterType = Filter_RegularExp;
	}
	else
	{
		if ( FilterType == Filter_ShowOnly)
			m_strFilterDialogShowOnly = strFilter;
		if ( FilterType == Filter_RemoveOnly)
			m_strFilterDialogRemoveOnly = strFilter;

		if ( strFilter.IsEmpty() )
		{	
			m_strRegularEx = strFilter;
			return;
		}

		m_strRegularEx = "\\.(";
		m_FilterType = FilterType;

		int iToken = 0;
		CString strToken;

		strToken = strFilter.Tokenize(". ;", iToken);
		while ( ! strToken.IsEmpty() )
		{
			m_strRegularEx.Append(strToken);

			strToken = strFilter.Tokenize(". ;", iToken);

			if ( ! strToken.IsEmpty() )
				m_strRegularEx.Append("|");
		}
		m_strRegularEx.Append(")");
	}
}

/*
===================================================================================================
===================================================================================================
*/
BOOL EDI_cl_FileFilter::RunFilter( const char* szFileName )
{
	if ( m_bFilterEnable && ! m_strRegularEx.IsEmpty() )
	{
		regex::rpattern patternPending(m_strRegularEx.GetBuffer());
		regex::match_results results;

		BOOL bResults = patternPending.match(szFileName, results).matched;

		if ( m_FilterType == Filter_RemoveOnly)
			return ! bResults;
		
		return bResults;
	}
	return TRUE;
}

#endif /* ACTIVE_EDITORS */
