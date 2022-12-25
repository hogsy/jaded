/*$T DIAtextfile_dlg.cpp GC 1.138 03/16/04 09:51:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAtextfile_dlg.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "EDIpaths.h"
#include "EDImainframe.h"
#include "EDIstrings.h"
#include "INOut/INO.h"

#include "Res/Res.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_TextFile, EDIA_cl_BaseDialog)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_TextFile::EDIA_cl_TextFile(void) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_NEWTEXTFILE)
{
	mo_FileName = CString("");
	mi_Country = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_TextFile::~EDIA_cl_TextFile(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_TextFile::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int			i, item;
	BOOL		ret;
	CComboBox	*pListBox;
	/*~~~~~~~~~~~~~~~~~~*/

	ret = EDIA_cl_BaseDialog::OnInitDialog();

	pListBox = (CComboBox *) GetDlgItem(IDC_COMBO_COUNTRY);
	for(i = 0; i < INO_e_MaxLangNb; i++)
	{
		item = pListBox->AddString(INO_dst_LanguageName[i].az_LongName);
        pListBox->SetItemData(item, i);
	}

    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TextFile::DoDataExchange(CDataExchange *pDX)
{
    CComboBox	*pListBox;
    CEdit*pEdit;

	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_COUNTRY, mi_Country);
	DDX_Text(pDX, IDC_EDIT_NAME2, mo_FileName);
    
    if(pDX->m_bSaveAndValidate)
    {
        pListBox = (CComboBox *) GetDlgItem(IDC_COMBO_COUNTRY);
        mi_Country = pListBox->GetItemData(mi_Country);
    }

    pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NAME2);
    pEdit->ShowWindow(SW_SHOW);
    pEdit->SetFocus();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
