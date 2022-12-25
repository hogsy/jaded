/*$T DIAlang_dlg.cpp GC 1.138 03/25/04 15:57:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAlang_dlg.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"
#include "INOut/INO.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_Lang, EDIA_cl_BaseDialog)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Lang::EDIA_cl_Lang(void) :
	EDIA_cl_BaseDialog(IDD_LANG)
{
	mi_Lang = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Lang::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int			i, item;
	BOOL		ret;
	CComboBox	*pListBox;
	/*~~~~~~~~~~~~~~~~~~*/

	ret = EDIA_cl_BaseDialog::OnInitDialog();

	pListBox = (CComboBox *) GetDlgItem(IDC_COMBO1);
	for(i = 0; i < INO_e_MaxLangNb; i++)
	{
		item = pListBox->AddString(INO_dst_LanguageName[i].az_LongName);
		pListBox->SetItemData(item, i);
	}

	i = pListBox->FindString(0, INO_dst_LanguageName[mi_Lang].az_LongName);
	pListBox->SetCurSel(i);
	pListBox->SetFocus();

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Lang::DoDataExchange(CDataExchange *pDX)
{
	CComboBox	*pListBox;
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO1, mi_Lang);
    
    if(pDX->m_bSaveAndValidate)
    {
        pListBox = (CComboBox *) GetDlgItem(IDC_COMBO1);
        mi_Lang = pListBox->GetItemData(mi_Lang);
    }
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
