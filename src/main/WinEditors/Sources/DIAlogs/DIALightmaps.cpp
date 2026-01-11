/*$T DIALightmaps_dlg.cpp GC 1.129 09/14/01 17:23:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "selection/selection.h"
#include "engine/sources/world/worstruct.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIALightmaps.h"
#include "Res/Res.h"
#include "EDImainframe.h"


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_LightmapsDialog, EDIA_cl_BaseDialog)
	//ON_BN_CLICKED(IDC_BUTTON_COMPUTE, OnBtCompute)	
	ON_WM_HSCROLL()	
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_LightmapsDialog::EDIA_cl_LightmapsDialog(void) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_LIGHTMAPS)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void	EDIA_cl_LightmapsDialog::SetUIFromParams(void)
{
	((CButton *) GetDlgItem(IDC_LM_CHECK_SUPERSAMPLING))->SetCheck(mOptions.doSuperSampling);

	((CSliderCtrl*)GetDlgItem(IDC_LM_SLIDER_SHADOWOPACITY))->SetPos(mOptions.shadowOpacity * 100);

	CString texelPerMeter;
	texelPerMeter.Format("%f", mOptions.texelPerMeter);
	((CEdit *) GetDlgItem(IDC_LM_EDIT_TEXELPERMETER))->SetWindowText(texelPerMeter);

	CString superSamplingFactor;
	superSamplingFactor.Format("%f", mOptions.superSamplingFactor); 
	((CEdit *) GetDlgItem(IDC_LM_EDIT_SUPERSAMPLINGFACTOR))->SetWindowText(texelPerMeter);

	CString shadowOpacity;
	shadowOpacity.Format("Shadow Opacity: %i %%", (int)(mOptions.shadowOpacity * 100));
	((CEdit *) GetDlgItem(IDC_LM_STATIC_SHADOWOPACITY))->SetWindowText(shadowOpacity);
}

void	EDIA_cl_LightmapsDialog::GetParamsFromUI(void)
{
	mOptions.doSuperSampling = ((CButton *) GetDlgItem(IDC_LM_CHECK_SUPERSAMPLING))->GetCheck() ? 1 : 0;
	mOptions.computeShadows = 1;
	mOptions.computePS2Lightmaps = 0;
	mOptions.shadowMapMode = 1;
	mOptions.fixBackFaceBug = 1;

	mOptions.shadowOpacity = (float)((CSliderCtrl*)GetDlgItem(IDC_LM_SLIDER_SHADOWOPACITY))->GetPos() / 100.0f;

	CString superSamplingFactor;
	((CEdit *) GetDlgItem(IDC_LM_EDIT_SUPERSAMPLINGFACTOR))->GetWindowText(superSamplingFactor);
	sscanf((LPCTSTR)superSamplingFactor, "%f", &mOptions.superSamplingFactor);

	CString texelPerMeter;
	((CEdit *) GetDlgItem(IDC_LM_EDIT_TEXELPERMETER))->GetWindowText(texelPerMeter);
	sscanf((LPCTSTR)texelPerMeter, "%f", &mOptions.texelPerMeter);

}

void EDIA_cl_LightmapsDialog::OnOK(void)
{
	GetParamsFromUI();

	EDIA_cl_BaseDialog::OnOK();
}

void	EDIA_cl_LightmapsDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	mOptions.shadowOpacity = (float)((CSliderCtrl*)GetDlgItem(IDC_LM_SLIDER_SHADOWOPACITY))->GetPos() / 100.0f;

	CString shadowOpacity;
	shadowOpacity.Format("Shadow Opacity: %i %%", (int)(mOptions.shadowOpacity * 100));
	((CEdit *) GetDlgItem(IDC_LM_STATIC_SHADOWOPACITY))->SetWindowText(shadowOpacity);
}	

BOOL EDIA_cl_LightmapsDialog::OnInitDialog(void)
{
	EDIA_cl_BaseDialog::OnInitDialog();

	CenterWindow();

	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_LM_SLIDER_SHADOWOPACITY);

	pSlider->SetTicFreq(10);
	pSlider->SetRange(0, 100, TRUE);

	SetUIFromParams();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LightmapsDialog::OnDestroy(void)
{	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

#endif /* ACTIVE_EDITORS */
