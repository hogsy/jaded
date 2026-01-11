/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "DIAlogs/DIAradiosity_dlg.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
extern BOOL AdjustRadiosityMode;
BEGIN_MESSAGE_MAP(EDIA_cl_RadiosityDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(IDOK, OnGO)
	ON_COMMAND(IDC_CHECK1, OnSO)
	ON_COMMAND(IDC_CHECK19, OnOP)
	ON_COMMAND(IDC_CHECK23, OnABE)
	ON_COMMAND(IDC_CHECK20 , OnEP)
	ON_COMMAND(IDC_CHECK22 , OnSmoothFront)
	ON_COMMAND(IDC_CHECK21 , OnComputeRadiosity)
	ON_COMMAND(IDC_CHECK24 , OnUBCKCLR)
	ON_COMMAND(IDC_CHECK25 , OnSmoothRes)
	ON_COMMAND(IDC_CHECK26 , OnFasthRes)
	ON_CBN_SELCHANGE(IDC_COMBO_LCT1, OnQualityCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_LCT2, OnBET)
END_MESSAGE_MAP()
void EDIA_cl_RadiosityDialog::OnQualityCombo(void)
{
	CComboBox			*pcombo;
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT1);
	if(pcombo->GetCurSel() == -1) return;
	*Quality = (ULONG) pcombo->GetCurSel() + 1;
}
void EDIA_cl_RadiosityDialog::OnUBCKCLR(void)
{
	*Flags ^= 128;
}

void EDIA_cl_RadiosityDialog::OnSmoothRes(void)
{
	*Flags ^= 256;
}
void EDIA_cl_RadiosityDialog::OnFasthRes(void)
{
	*Flags ^= 512;
}
void EDIA_cl_RadiosityDialog::OnGO(void)
{
	*Flags |= 1;
	PostMessage(WM_CLOSE);
}
void EDIA_cl_RadiosityDialog::OnSO(void)
{
	*Flags ^= 2;
}

void EDIA_cl_RadiosityDialog::OnEP(void)
{
	*Flags ^= 32;
}
void EDIA_cl_RadiosityDialog::OnSmoothFront(void)
{
	*Flags ^= 8;
}

void EDIA_cl_RadiosityDialog::OnComputeRadiosity(void)
{
	CButton *po_Button;
	*Flags ^= 64;
	po_Button = (CButton *) GetDlgItem(IDC_CHECK24);
	if (*Flags & 64)
		po_Button->EnableWindow(TRUE);
	else
		po_Button->EnableWindow(FALSE);
	po_Button = (CButton *) GetDlgItem(IDC_CHECK25);
	if (*Flags & 64)
		po_Button->EnableWindow(TRUE);
	else
		po_Button->EnableWindow(FALSE);
	po_Button = (CButton *) GetDlgItem(IDC_CHECK26);
	if (*Flags & 64)
		po_Button->EnableWindow(TRUE);
	else
		po_Button->EnableWindow(FALSE);
}

void EDIA_cl_RadiosityDialog::OnBET(void)
{
	CComboBox			*pcombo;
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT2);
	if(pcombo->GetCurSel() == -1) return;
	*BreakEdgeThresh = (ULONG) pcombo->GetCurSel();
}
void EDIA_cl_RadiosityDialog::OnABE(void)
{
	CComboBox			*pcombo;
	*Flags ^= 16;
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT2);
	pcombo->EnableWindow(*Flags & 16);
}

void EDIA_cl_RadiosityDialog::OnOP(void)
{
	CComboBox			*pcombo;
	*Flags ^= 4;
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT1);
	pcombo->EnableWindow(*Flags & 4);
}

BOOL EDIA_cl_RadiosityDialog::OnInitDialog(void)
{
	CButton *po_Button;
	CComboBox			*pcombo;
	po_Button = (CButton *) GetDlgItem(IDC_CHECK1);
	if (*Flags & 2)
		po_Button->SetCheck(1);
	else
		po_Button->SetCheck(0);

	po_Button = (CButton *) GetDlgItem(IDC_CHECK19);
	if (*Flags & 4)
		po_Button->SetCheck(1);
	else
		po_Button->SetCheck(0);

	po_Button = (CButton *) GetDlgItem(IDC_CHECK22);
	if (*Flags & 8)
		po_Button->SetCheck(1);
	else
		po_Button->SetCheck(0);

	po_Button = (CButton *) GetDlgItem(IDC_CHECK23);
	if (*Flags & 16)
		po_Button->SetCheck(1);
	else
		po_Button->SetCheck(0);

	po_Button = (CButton *) GetDlgItem(IDC_CHECK20);
	if (*Flags & 32)
		po_Button->SetCheck(1);
	else
		po_Button->SetCheck(0);

	po_Button = (CButton *) GetDlgItem(IDC_CHECK21);
	if (*Flags & 64)
	{
		po_Button->SetCheck(1);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK24);
		po_Button->EnableWindow(TRUE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK25);
		po_Button->EnableWindow(TRUE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK26);
		po_Button->EnableWindow(TRUE);
	}
	else
	{
		po_Button->SetCheck(0);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK24);
		po_Button->EnableWindow(FALSE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK25);
		po_Button->EnableWindow(FALSE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK26);
		po_Button->EnableWindow(FALSE);
	}

	po_Button = (CButton *) GetDlgItem(IDC_CHECK24);
	if (*Flags & 128)
	{
		po_Button->SetCheck(1);
	}
	else
		po_Button->SetCheck(0);

	po_Button = (CButton *) GetDlgItem(IDC_CHECK25);
	if (*Flags & 256)
	{
		po_Button->SetCheck(1);
	}
	else
		po_Button->SetCheck(0);


	po_Button = (CButton *) GetDlgItem(IDC_CHECK26);
	if (*Flags & 512)
	{
		po_Button->SetCheck(1);
	}
	else
		po_Button->SetCheck(0);


	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT1);
	pcombo->InsertString(0, "Quality 1 (hight)");
	pcombo->InsertString(1, "Quality 2 ");
	pcombo->InsertString(2, "Quality 3 ");
	pcombo->InsertString(3, "Quality 4 ");
	pcombo->InsertString(4, "Quality 5 ");
	pcombo->InsertString(5, "Quality 6");
	pcombo->InsertString(6, "Quality 7");
	pcombo->InsertString(7, "Quality 8");
	pcombo->InsertString(8, "Quality 9");
	pcombo->InsertString(9, "Quality 10 (low)");
	pcombo->InsertString(10, "Quality 11 ");
	pcombo->InsertString(11, "Quality 12 ");
	pcombo->InsertString(12, "Quality 13 ");
	pcombo->InsertString(13, "Quality 14 ");
	pcombo->InsertString(14, "Quality 15 ");
	pcombo->InsertString(15, "Quality 16 ");
	pcombo->InsertString(16, "Quality 17 ");
	pcombo->InsertString(17, "Quality 18 ");
	pcombo->InsertString(19, "Quality 19 ");
	pcombo->InsertString(20, "Quality 20 ");
	pcombo->SetCurSel(*Quality - 1);
	pcombo->EnableWindow(*Flags & 4);

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT2);
	pcombo->InsertString(0, "Thresh 0 (plane)");
	pcombo->InsertString(1, "Thresh 1");
	pcombo->InsertString(2, "Thresh 2");
	pcombo->InsertString(3, "Thresh 3");
	pcombo->InsertString(4, "Thresh 4");
	pcombo->InsertString(5, "Thresh 5");
	pcombo->InsertString(6, "Thresh 6 ");
	pcombo->InsertString(7, "Thresh 7 (90°)");
	pcombo->InsertString(8, "Thresh 8 ");
	pcombo->InsertString(9, "Thresh 9 ");
	pcombo->InsertString(10, "Thresh 10 ");
	pcombo->InsertString(11, "Thresh 11 ");
	pcombo->InsertString(12, "Thresh 12 ");
	pcombo->InsertString(13, "Thresh 13 ");
	pcombo->InsertString(14, "Thresh 14 ");
	pcombo->InsertString(15, "Thresh 15 (180°)");
	pcombo->SetCurSel(*BreakEdgeThresh);
	pcombo->EnableWindow(*Flags & 16);

	if (AdjustRadiosityMode)
	{
		po_Button = (CButton *) GetDlgItem(IDC_CHECK23);
		po_Button->EnableWindow(FALSE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK22);
		po_Button->EnableWindow(FALSE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK20);
		po_Button->EnableWindow(FALSE);
		pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT2);
		pcombo->EnableWindow(FALSE);
	} else
	{
		po_Button = (CButton *) GetDlgItem(IDC_CHECK23);
		po_Button->EnableWindow(TRUE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK22);
		po_Button->EnableWindow(TRUE);
		po_Button = (CButton *) GetDlgItem(IDC_CHECK20);
		po_Button->EnableWindow(TRUE);
		pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LCT2);
		pcombo->EnableWindow(TRUE); 
	}

	return FALSE;
}
/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_RadiosityDialog::EDIA_cl_RadiosityDialog(ULONG *TheFlags , ULONG *TheQulity, ULONG *BEQ) :
    EDIA_cl_BaseDialog(IDD_DIALOG_RADIOSITY)
{
		Flags = TheFlags;
		Quality = TheQulity;
		BreakEdgeThresh = BEQ;
		*Flags &= ~1;
}


#endif  /* ACTIVE_EDITORS */
