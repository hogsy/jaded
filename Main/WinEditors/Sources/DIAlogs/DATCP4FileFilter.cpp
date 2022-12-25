/*$T DIAname_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DATCP4FileFilter.h"
#include "Res/Res.h"
#include ".\datcp4filefilter.h"

/*
 =======================================================================================================================
 =======================================================================================================================
*/
BEGIN_MESSAGE_MAP(EDIA_cl_P4FileFilterDialog,EDIA_cl_BaseDialog)
END_MESSAGE_MAP()

/*
=======================================================================================================================
=======================================================================================================================
*/

EDIA_cl_P4FileFilterDialog::EDIA_cl_P4FileFilterDialog(EDI_cl_FileFilter* pFileFilter):
EDIA_cl_BaseDialog(DIALOGS_IDD_P4FILEFILTER),
m_pFileFilter(pFileFilter)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/

BOOL EDIA_cl_P4FileFilterDialog::OnInitDialog()
{
	EDIA_cl_BaseDialog::OnInitDialog();

	switch( m_pFileFilter->GetFilterType() )
	{
		case EDI_cl_FileFilter::Filter_ShowOnly:
			GetDlgItem(IDC_P4FILTER_COMBO_SHOWONLY)->SetWindowText(m_pFileFilter->GetFilterDialogShowOnly());
			CheckDlgButton(IDC_P4FILTER_CHECK_SHOWONLY, TRUE);
			break;
		case EDI_cl_FileFilter::Filter_RemoveOnly:
			GetDlgItem(IDC_P4FILTER_COMBO_REMOVEONLY)->SetWindowText(m_pFileFilter->GetFilterDialogRemoveOnly());
			CheckDlgButton(IDC_P4FILTER_CHECK_REMOVEONLY, TRUE);
			break;
		case EDI_cl_FileFilter::Filter_RegularExp:
			GetDlgItem(IDC_P4FILTER_COMBO_REGULAREXP)->SetWindowText(m_pFileFilter->GetFilterDialogRegEx());
			CheckDlgButton(IDC_P4FILTER_CHECK_REGULAREXP, TRUE);
			break;
	}
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_P4FileFilterDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_P4FileFilterDialog::OnOK()
{
	CString strFilter;
	
	int iRadioID = GetCheckedRadioButton(IDC_P4FILTER_CHECK_SHOWONLY, IDC_P4FILTER_CHECK_REGULAREXP);

	switch ( iRadioID )
	{
		case IDC_P4FILTER_CHECK_SHOWONLY:
			((CComboBox*)GetDlgItem(IDC_P4FILTER_COMBO_SHOWONLY))->GetWindowText(strFilter);
			m_pFileFilter->SetFilter(strFilter, EDI_cl_FileFilter::Filter_ShowOnly);

			break;
		case IDC_P4FILTER_CHECK_REMOVEONLY:
			((CComboBox*)GetDlgItem(IDC_P4FILTER_COMBO_REMOVEONLY))->GetWindowText(strFilter);
			m_pFileFilter->SetFilter(strFilter, EDI_cl_FileFilter::Filter_RemoveOnly);
			break;
		case IDC_P4FILTER_CHECK_REGULAREXP:
			((CComboBox*)GetDlgItem(IDC_P4FILTER_COMBO_REGULAREXP))->GetWindowText(strFilter);
			m_pFileFilter->SetFilter(strFilter, EDI_cl_FileFilter::Filter_RegularExp);
			break;
	}
	
	EDIA_cl_BaseDialog::OnOK();
}

#endif /* ACTIVE_EDITORS */