/*$T DIAname_dlg.h GC! 1.078 03/16/00 10:29:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

class EDIA_cl_P4ChangelistCommentDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_P4ChangelistCommentDialog (	const char *_psz_Title, 
										const char * _pszSubmitType,
										const char*_psz_comment,
										BOOL _bActivateAdditionnalVersionning,
										BOOL _bRevertUnchanged,
										BOOL _bVerifyLinks,
										BOOL _bSelectAllText);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CString mo_Comment;
	CString mo_Title;
	CString mo_AdditionnalVersion;
	CString mo_SubmitType;
	CString mo_BugFix;
	bool	mb_EnableAdditionnalVersion;
	BOOL m_bTagForDemo;
	BOOL m_bSelectAllText;
	BOOL m_bOnSubmit;
	BOOL m_bRevertUnchanged;
	BOOL m_bVerifyLinks;
	BOOL m_bDefaultVerifyLinks;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */
	const char* GetAdditionnalVersion() 
	{ 
		return mo_AdditionnalVersion.GetBuffer();
	}

	BOOL GetRevertUnchanged() { return m_bRevertUnchanged; }
	BOOL GetVerifyLinks() { return m_bVerifyLinks; }


public:
	void	DoDataExchange(CDataExchange *);
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnP4AdditionnalVersionChanged();
	afx_msg void	OnP4EnableAdditionnalVersionChanged();
	afx_msg BOOL	OnInitDialog();
	
		
	DECLARE_MESSAGE_MAP()
};


#endif /* ACTIVE_EDITORS */
