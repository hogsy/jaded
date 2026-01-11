/*$T DIAname_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DATCP4ChangelistComment.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles\LOAding\LOAdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "F3Dframe/F3Dview.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "Res/Res.h"

#include "DATaControl/DATCPerforce.h"
#include "DIAlogs/DATCP4SettingsDlg.h"

BEGIN_MESSAGE_MAP(EDIA_cl_P4ChangelistCommentDialog,EDIA_cl_BaseDialog)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED		(IDC_P4ENABLE_ADDITIONNAL_VERSION,OnP4EnableAdditionnalVersionChanged)
END_MESSAGE_MAP()
/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_P4ChangelistCommentDialog::EDIA_cl_P4ChangelistCommentDialog(	const char *_psz_Title,
																		const char * _pszSubmitType,
																		const char* _psz_comment,
																		BOOL _bOnSubmit,
																		BOOL _bRevertUnchangedFiles,
																		BOOL _bVerifyLinks,
																		BOOL _bSelectAllText ) :
EDIA_cl_BaseDialog(DIALOGS_IDD_P4CHANGELIST_COMMENT),
mo_Title(_psz_Title),
mo_Comment(_psz_comment),
mo_SubmitType(_pszSubmitType),
m_bSelectAllText(_bSelectAllText),
m_bOnSubmit(_bOnSubmit),
m_bRevertUnchanged(_bRevertUnchangedFiles),
m_bDefaultVerifyLinks(_bVerifyLinks)
{
	//mo_Title = _psz_Title;
	//mo_Comment = _psz_comment;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int EDIA_cl_P4ChangelistCommentDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int iReturn ;
	if(  (iReturn = EDIA_cl_BaseDialog::OnCreate(lpCreateStruct)) == -1)
	{
		return -1;
	}

	GetDlgItem(IDC_P4ENABLE_ADDITIONNAL_VERSION)->EnableWindow(m_bOnSubmit);
	GetDlgItem(IDC_P4ADDITIONNAL_VERSION)->EnableWindow(FALSE);
	GetDlgItem(IDC_P4CHECKLINKS)->EnableWindow(m_bDefaultVerifyLinks);
	((CButton*)GetDlgItem(IDC_P4CHECKLINKS))->SetCheck(m_bDefaultVerifyLinks);

	
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRichEditCtrl	*po_Edit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	po_Edit = ((CRichEditCtrl *) GetDlgItem(IDC_P4CHANGELIST_COMMENT));
	po_Edit->SetFocus();

	if ( m_bSelectAllText ) 
	{
		po_Edit->SetSel(0,mo_Comment.GetLength());
	}
	else
	{
		po_Edit->SetSel(0,0);
	}
	return iReturn;
}

BOOL EDIA_cl_P4ChangelistCommentDialog::OnInitDialog()
{
	EDIA_cl_BaseDialog::OnInitDialog();
	GetDlgItem(IDC_P4ENABLE_ADDITIONNAL_VERSION)->EnableWindow(m_bOnSubmit);
	GetDlgItem(IDC_P4_MAPLIST)->EnableWindow(m_bOnSubmit);
	GetDlgItem(IDC_P4_BUGFIX)->EnableWindow(m_bOnSubmit);
	GetDlgItem(IDC_P4_DEMO_TAG)->EnableWindow(m_bOnSubmit);
	GetDlgItem(IDC_P4ADDITIONNAL_VERSION)->EnableWindow(FALSE);
	GetDlgItem(IDC_P4FORCENOREVERT)->EnableWindow(FALSE);
	GetDlgItem(IDC_P4CHECKLINKS)->EnableWindow(m_bDefaultVerifyLinks);
	((CButton*)GetDlgItem(IDC_P4CHECKLINKS))->SetCheck(m_bDefaultVerifyLinks);

	CComboBox* pP4AdditionnalVersion	= (CComboBox*)	GetDlgItem(IDC_P4ADDITIONNAL_VERSION);
	// client view has not been set wet.
	DAT_CPerforce Perforce;
	if ( Perforce.P4Connect() )
	{
		std::list<std::string> lstDirs;

		std::string strDir = "//";
		strDir += DAT_CPerforce::GetInstance()->GetP4Depot() ;
		strDir += "/";
		strDir += P4_VERSIONROOT;
		strDir += "*";
		Perforce.P4Dirs(strDir.c_str(),lstDirs);

		///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~«~~~~~~~~~~~~*/

		STL::list<STL::string>::iterator iter= lstDirs.begin();
		UINT ui = 0;

		for ( iter; iter != lstDirs.end(); iter++ ) 
		{
			char* pszVersion = const_cast<char*>((*iter).c_str()) ;
			pszVersion += strDir.length() - strlen("*") ;
			if ( stricmp(pszVersion,DAT_CPerforce::GetInstance()->GetBFVersion()) != 0 )
			pP4AdditionnalVersion->AddString(pszVersion);
		}
		Perforce.P4Disconnect();
	}

	return TRUE;
}

void EDIA_cl_P4ChangelistCommentDialog::OnP4EnableAdditionnalVersionChanged()
{
	GetDlgItem(IDC_P4ADDITIONNAL_VERSION)->EnableWindow(((CButton*)GetDlgItem(IDC_P4ENABLE_ADDITIONNAL_VERSION))->GetCheck());
	GetDlgItem(IDC_P4FORCENOREVERT)->EnableWindow(((CButton*)GetDlgItem(IDC_P4ENABLE_ADDITIONNAL_VERSION))->GetCheck());
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_P4ChangelistCommentDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRichEditCtrl	*po_Edit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);

	po_Edit = ((CRichEditCtrl *) GetDlgItem(IDC_P4CHANGELIST_COMMENT));

	if ( ((CButton*)GetDlgItem(IDC_P4ENABLE_ADDITIONNAL_VERSION))->GetCheck() ) 
	{

		CComboBox* pP4AdditionnalVersion	= (CComboBox*)	GetDlgItem(IDC_P4ADDITIONNAL_VERSION);
		CString strP4AdditionnalVersion; 
		if ( pP4AdditionnalVersion->GetCurSel() >  CB_ERR )
		{
			pP4AdditionnalVersion->GetLBText(pP4AdditionnalVersion->GetCurSel(),mo_AdditionnalVersion);
		}

		m_bRevertUnchanged = !((CButton*)GetDlgItem(IDC_P4FORCENOREVERT))->GetCheck();

	}
	
	m_bVerifyLinks = ((CButton*)GetDlgItem(IDC_P4CHECKLINKS))->GetCheck();

	SetWindowText(mo_Title);
	DDX_Text(pDX, IDC_P4CHANGELIST_COMMENT, mo_Comment);
	DDX_Text(pDX, IDC_P4_MAPLIST, mo_SubmitType);
	DDX_Text(pDX, IDC_P4_BUGFIX, mo_BugFix);
	m_bTagForDemo = ((CButton*)GetDlgItem(IDC_P4_DEMO_TAG))->GetCheck();
	po_Edit->SetFocus();
}

#endif /* ACTIVE_EDITORS */
