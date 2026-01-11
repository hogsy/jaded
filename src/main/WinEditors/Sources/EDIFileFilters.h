/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#pragma once
#ifdef ACTIVE_EDITORS

/*$4
***************************************************************************************************
***************************************************************************************************
*/

class EDI_cl_FileFilter
{
	/*$2
	---------------------------------------------------------------------------------------------------
	CONSTRUCT
	---------------------------------------------------------------------------------------------------
	*/
public:
	EDI_cl_FileFilter();
	~EDI_cl_FileFilter();

	enum eFilterType
	{
		Filter_ShowOnly,
		Filter_RemoveOnly,
		Filter_RegularExp
	};

	/*$2
	---------------------------------------------------------------------------------------------------
	ATTRIBUTES
	---------------------------------------------------------------------------------------------------
	*/
protected:
	BOOL m_bFilterEnable ;
	CString m_strRegularEx ;
	eFilterType m_FilterType ;

	CString m_strFilterDialogShowOnly ;
	CString m_strFilterDialogRemoveOnly ;
	CString m_strFilterDialogRegEx ;
		
	/*$2
	---------------------------------------------------------------------------------------------------
	FUNCTIONS
	---------------------------------------------------------------------------------------------------
	*/
public:
	void Enable(BOOL bFilterEnable) { m_bFilterEnable = bFilterEnable ; } ;
	BOOL IsEnabled() { return m_bFilterEnable ; } ;

	void SetFilter(const CString& strFilterFileType, eFilterType FilterType);
	void ClearFilter(eFilterType FilterType); 

	CString& GetFilterDialogShowOnly() { return m_strFilterDialogShowOnly ; } ;	
	CString& GetFilterDialogRemoveOnly() { return m_strFilterDialogRemoveOnly ; } ;	
	CString& GetFilterDialogRegEx() { return m_strFilterDialogRegEx ; } ;	
	
	eFilterType GetFilterType() { return m_FilterType; };

	BOOL RunFilter( const char* szFileName );
};

#endif /* ACTIVE_EDITORS */
