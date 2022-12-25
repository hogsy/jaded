/*$T DIAcompletion_dlg.cpp GC!1.71 01/19/00 09:46:32 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAXeMergeFlags.h"
#include "Res/Res.h"
#include "EDImainframe.h"

BEGIN_MESSAGE_MAP(EDIA_cl_XeMergeFlags, EDIA_cl_BaseDialog)
    ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK, OnCheckALL)
	ON_BN_CLICKED(IDC_CHECK1, OnCheckPostion)
	ON_BN_CLICKED(IDC_CHECK2, OnCheckBV)
	ON_BN_CLICKED(IDC_CHECK3, OnCheckKeyGRORLI)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_XeMergeFlags::EDIA_cl_XeMergeFlags(EDIA_cl_ToolBox_GROView* _pst_GROView, OBJ_tdst_GameObject* _pstGAO) :
EDIA_cl_BaseDialog(DIALOGS_IDD_XEMERGEFLAGS),
m_pstGROView(_pst_GROView),
m_pstGAO(_pstGAO),
m_ulXeMergeFlags(NULL)
{
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_XeMergeFlags::~EDIA_cl_XeMergeFlags(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_XeMergeFlags::OnInitDialog(void)
{
    CDialog::OnInitDialog();
    
	// Update data in window
	Refresh(m_pstGAO);

	// Disable Apply button since no changes have been made
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	CenterWindow();

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_XeMergeFlags::OnOK(void)
{
	m_pstGAO->ul_XeMergeFlags = NULL;

	if ( IsDlgButtonChecked(IDC_CHECK1) )
		m_pstGAO->ul_XeMergeFlags |= OBJ_C_Merge_KeepXe_Position;

	if ( IsDlgButtonChecked(IDC_CHECK2) )
		m_pstGAO->ul_XeMergeFlags |= OBJ_C_Merge_KeepXe_BV;
	
	if ( IsDlgButtonChecked(IDC_CHECK3) )
	{
		m_pstGAO->ul_XeMergeFlags |= OBJ_C_Merge_KeepXe_KeyGRO;
		m_pstGAO->ul_XeMergeFlags |= OBJ_C_Merge_KeepXe_KeyRLI;
	}

	EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_XeMergeFlags::OnCancel(void)
{
    EDIA_cl_BaseDialog::OnCancel();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_XeMergeFlags::OnCheckALL()
{
	// Disable Apply button since no changes have been made
	GetDlgItem(IDOK)->EnableWindow(TRUE);

	if ( IsDlgButtonChecked(IDC_CHECK) == BST_CHECKED )
	{
		CheckDlgButton(IDC_CHECK, BST_UNCHECKED);
		CheckDlgButton(IDC_CHECK1, BST_UNCHECKED);
		CheckDlgButton(IDC_CHECK2, BST_UNCHECKED);
		CheckDlgButton(IDC_CHECK3, BST_UNCHECKED);

		m_ulXeMergeFlags = NULL ;
	}
	else
	{
		CheckDlgButton(IDC_CHECK, BST_CHECKED);
		CheckDlgButton(IDC_CHECK1, BST_CHECKED);
		CheckDlgButton(IDC_CHECK2, BST_CHECKED);
		CheckDlgButton(IDC_CHECK3, BST_CHECKED);

		m_ulXeMergeFlags = (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_XeMergeFlags::OnCheckPostion()
{
	// Disable Apply button since no changes have been made
	GetDlgItem(IDOK)->EnableWindow(TRUE);

	if ( IsDlgButtonChecked(IDC_CHECK1) )
		m_ulXeMergeFlags |= OBJ_C_Merge_KeepXe_Position;
	else
		m_ulXeMergeFlags &= ~OBJ_C_Merge_KeepXe_Position;

	if ( m_ulXeMergeFlags == (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
		CheckDlgButton(IDC_CHECK, BST_CHECKED);
	else if ( m_ulXeMergeFlags & (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
		CheckDlgButton(IDC_CHECK, BST_INDETERMINATE);
	else
		CheckDlgButton(IDC_CHECK, BST_UNCHECKED);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_XeMergeFlags::OnCheckBV()
{
	// Disable Apply button since no changes have been made
	GetDlgItem(IDOK)->EnableWindow(TRUE);

	if ( IsDlgButtonChecked(IDC_CHECK2) )
		m_ulXeMergeFlags |= OBJ_C_Merge_KeepXe_BV;
	else
		m_ulXeMergeFlags &= ~OBJ_C_Merge_KeepXe_BV;

	if ( m_ulXeMergeFlags == (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
		CheckDlgButton(IDC_CHECK, BST_CHECKED);
	else if (m_ulXeMergeFlags & (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
		CheckDlgButton(IDC_CHECK, BST_INDETERMINATE);
	else
		CheckDlgButton(IDC_CHECK, BST_UNCHECKED);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_XeMergeFlags::OnCheckKeyGRORLI()
{
	// Disable Apply button since no changes have been made
	GetDlgItem(IDOK)->EnableWindow(TRUE);

	if ( IsDlgButtonChecked(IDC_CHECK3) )
	{
		m_ulXeMergeFlags |= OBJ_C_Merge_KeepXe_KeyGRO;
		m_ulXeMergeFlags |= OBJ_C_Merge_KeepXe_KeyRLI;
	}
	else
	{
		m_ulXeMergeFlags &= ~OBJ_C_Merge_KeepXe_KeyGRO;
		m_ulXeMergeFlags &= ~OBJ_C_Merge_KeepXe_KeyRLI;
	}

	if ( m_ulXeMergeFlags == (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
		CheckDlgButton(IDC_CHECK, BST_CHECKED);
	else if ( m_ulXeMergeFlags & (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
		CheckDlgButton(IDC_CHECK, BST_INDETERMINATE);
	else
		CheckDlgButton(IDC_CHECK, BST_UNCHECKED);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_XeMergeFlags::OnDestroy()
{
	m_pstGROView->Remove_MergePS2Data();

	EDIA_cl_BaseDialog::OnDestroy();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_XeMergeFlags::Refresh(OBJ_tdst_GameObject* _pst_Gao)
{
	if ( _pst_Gao != NULL )
	{
		// Disable Apply button since no changes have been made
		if ( m_pstGAO != _pst_Gao )
			GetDlgItem(IDOK)->EnableWindow(FALSE);

		m_pstGAO = _pst_Gao;
		m_ulXeMergeFlags = _pst_Gao->ul_XeMergeFlags;

		GetDlgItem(IDC_STATIC_GONAME)->SetWindowText(m_pstGAO->sz_Name);

		GetDlgItem(IDC_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(TRUE);

		CheckDlgButton(IDC_CHECK1, m_ulXeMergeFlags & OBJ_C_Merge_KeepXe_Position);
		CheckDlgButton(IDC_CHECK2, m_ulXeMergeFlags & OBJ_C_Merge_KeepXe_BV);
		CheckDlgButton(IDC_CHECK3, m_ulXeMergeFlags & (OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI));

		if ( m_ulXeMergeFlags == (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
			CheckDlgButton(IDC_CHECK, BST_CHECKED);
		else if ( m_ulXeMergeFlags & (OBJ_C_Merge_KeepXe_Position|OBJ_C_Merge_KeepXe_BV|OBJ_C_Merge_KeepXe_KeyGRO|OBJ_C_Merge_KeepXe_KeyRLI) )
			CheckDlgButton(IDC_CHECK, BST_INDETERMINATE);
		else
			CheckDlgButton(IDC_CHECK, BST_UNCHECKED);
	}
	else
	{
		m_ulXeMergeFlags = NULL;

		GetDlgItem(IDC_STATIC_GONAME)->SetWindowText("No selection");

		GetDlgItem(IDC_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(FALSE);

		// Disable Apply button since no changes have been made
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

#endif /* ACTIVE_EDITORS */

