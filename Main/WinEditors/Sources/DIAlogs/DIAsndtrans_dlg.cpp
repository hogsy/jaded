/*$T DIAsndtrans_dlg.cpp GC 1.138 06/19/03 17:28:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAsndtrans_dlg.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDvolume.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SndTranslate, EDIA_cl_BaseDialog)
	ON_COMMAND(IDC_CHECK_ALL, OnAllLanguagues)
END_MESSAGE_MAP()
static int	ai_CheckList[EDIA_Cte_LangNb] =
{
	IDC_LANG0,
	IDC_LANG1,
	IDC_LANG2,
	IDC_LANG3,
	IDC_LANG4,
	IDC_LANG5,
	IDC_LANG6,
	IDC_LANG7,
	IDC_LANG8,
	IDC_LANG9,
	IDC_LANG10
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndTranslate::EDIA_cl_SndTranslate(void *pv) :
	EDIA_cl_BaseDialog(ESON_IDD_TRANSLATE)
{
	L_memset(mi_ChoosenLang, 0, EDIA_Cte_LangNb * sizeof(int));
	mb_AllLanguages = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndTranslate::Reset(void)
{
	L_memset(mi_ChoosenLang, 0, EDIA_Cte_LangNb * sizeof(int));
	mb_AllLanguages = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndTranslate::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Button;
	int		i;
	/*~~~~~~~~~~~~~~~*/

	LINK_gb_CanRefresh = FALSE;
	CDialog::DoDataExchange(pDX);
	if(pDX->m_bSaveAndValidate)
	{
		for(i = 0; i < EDIA_Cte_LangNb; i++)
		{
			po_Button = (CButton *) GetDlgItem(ai_CheckList[i]);
			mi_ChoosenLang[i] = po_Button->GetCheck();
		}
	}
	else
	{
		po_Button = (CButton *) GetDlgItem(IDC_LANG1);
		po_Button->SetCheck(1);
	}

	LINK_gb_CanRefresh = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndTranslate::OnAllLanguagues(void)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Button;
	int		i;
	/*~~~~~~~~~~~~~~~*/

	mb_AllLanguages = !mb_AllLanguages;

	if(mb_AllLanguages)
	{
		for(i = 0; i < EDIA_Cte_LangNb; i++)
		{
			po_Button = (CButton *) GetDlgItem(ai_CheckList[i]);
			po_Button->SetCheck(1);
			mi_ChoosenLang[i] = 1;
		}
	}
	else
	{
		for(i = 0; i < EDIA_Cte_LangNb; i++)
		{
			po_Button = (CButton *) GetDlgItem(ai_CheckList[i]);
			po_Button->SetCheck(0);
			mi_ChoosenLang[i] = 0;
		}
	}

	po_Button = (CButton *) GetDlgItem(IDC_LANG1);
	po_Button->SetCheck(1);
	mi_ChoosenLang[1] = 1;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
