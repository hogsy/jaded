/*$T DIACOLOR_dlg.cpp GC!1.52 12/02/99 15:50:33 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIA_SmoothSel_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "DIAlogs/DIAname_dlg.h"

extern ULONG SmoothSelIsActivate;
extern ULONG InvertSel;
extern float DistanceMax;
extern ULONG SmoothSelMustBeUpdate;
extern float topologicCare;
extern ULONG SmoothSelMode;
#ifndef JADEFUSION 
extern "C" 
{
#endif
float BaseAngle = 0.0f;
float EndAngle = 0.0f;
#ifndef JADEFUSION 
}
#endif

BEGIN_MESSAGE_MAP(EDIA_cl_SmoothSelectionDialog, EDIA_cl_BaseDialog)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
	//ON_WM_PAINT()//*/
    //ON_WM_ERASEBKGND()
    ON_WM_KEYDOWN()
    ON_WM_MOUSEWHEEL()
    ON_BN_CLICKED(IDC_CHECK1, OnCheck_InvertSel)
    ON_BN_CLICKED(IDC_CHECK2, OnCheck_Radius)
    ON_BN_CLICKED(IDC_CHECK3, OnCheck_TopoCare)
	ON_EN_CHANGE(IDC_EDIT1, OnEditChangeRadius)
	ON_EN_CHANGE(IDC_EDIT12, OnEditChangeTC)
END_MESSAGE_MAP()

#define GOURAUDDISCRET  50

EDIA_cl_SmoothSelectionDialog *p_TheOnlyOneSmoothSelDiag = NULL;
/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_SmoothSelectionDialog::EDIA_cl_SmoothSelectionDialog
(
    char            *_psz_Title,
    unsigned long   *ColorExt,
    void (*RefrechClbk) (unsigned long, unsigned long),
    unsigned long USRPARAM1,
    unsigned long USRPARAM2
) :
    EDIA_cl_BaseDialog(IDD_DIALOG_SMOOTH_SELECTION)
{
	SmoothSelIsActivate = 1;
	p_TheOnlyOneSmoothSelDiag = this;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_SmoothSelectionDialog::~EDIA_cl_SmoothSelectionDialog(void)
{
	SmoothSelIsActivate = 0;
	p_TheOnlyOneSmoothSelDiag = NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_SmoothSelectionDialog::OnInitDialog(void)
{
    SetWindowText(mo_Title);
    CenterWindow(AfxGetMainWnd());
   ((CButton *) GetDlgItem(IDC_CHECK1))->SetCheck(InvertSel);
   ((CButton *) GetDlgItem(IDC_CHECK2))->SetCheck(SmoothSelMode & 1);
   ((CButton *) GetDlgItem(IDC_CHECK3))->SetCheck(SmoothSelMode & 2);
   SetDlgItemInt( IDC_EDIT1, (int)(DistanceMax * 100.0f));
   SetDlgItemInt( IDC_EDIT12, (int)(topologicCare * 100.0f));
   UpdateEnability();
/*   SetDlgItemInt( IDC_EDIT2, (int)(BaseAngle * 100.0f));
   SetDlgItemInt( IDC_EDIT3, (int)(EndAngle * 100.0f));*/

/*   ((CEdit *) GetDlgItem(IDC_EDIT1))->SetDlgItemInt((int)(DistanceMax * 100.0f), FALSE);
   ((CEdit *) GetDlgItem(IDC_EDIT2))->SetDlgItemInt((int)(BaseAngle * 100.0f), FALSE);
   ((CEdit *) GetDlgItem(IDC_EDIT3))->SetDlgItemInt((int)(EndAngle * 100.0f), FALSE);*/
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_SmoothSelectionDialog::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        OnKeyDown(pMsg->wParam, 0, 0);
    }
    return 0;
}


BOOL EDIA_cl_SmoothSelectionDialog::OnMouseWheel(UINT a, short b, CPoint cp)
{
	CRect DS;
	//DS.PtInRect
	(((CWnd *) GetDlgItem(IDC_EDIT1))->GetWindowRect(&DS));
	if (DS.PtInRect(cp))
	{
		/*~~~~~~~~~~~~~~~~~~*/
		char	asz_Temp[512];
		int RadiusinCentimeters;
		int Delta;
		/*~~~~~~~~~~~~~~~~~~*/

		GetDlgItem(IDC_EDIT1)->GetWindowText(asz_Temp, 512);
		sscanf(asz_Temp, "%d", &RadiusinCentimeters);
		Delta = RadiusinCentimeters >> 3;
		if (!Delta) Delta = 1;
		if (b > 0)
			RadiusinCentimeters += Delta;
		else
			RadiusinCentimeters -= Delta;
		if (RadiusinCentimeters < 0) RadiusinCentimeters = 0;
		SetDlgItemInt( IDC_EDIT1, RadiusinCentimeters);
		OnEditChangeRadius();
		return TRUE;
	}

	(((CWnd *) GetDlgItem(IDC_EDIT12))->GetWindowRect(&DS));
	if (DS.PtInRect(cp))
	{
		/*~~~~~~~~~~~~~~~~~~*/
		char	asz_Temp[512];
		int RadiusinCentimeters;
		int Delta;
		/*~~~~~~~~~~~~~~~~~~*/

		GetDlgItem(IDC_EDIT12)->GetWindowText(asz_Temp, 512);
		sscanf(asz_Temp, "%d", &RadiusinCentimeters);
		Delta = RadiusinCentimeters >> 3;
		if (!Delta) Delta = 1;
		if (b > 0)
			RadiusinCentimeters += Delta;
		else
			RadiusinCentimeters -= Delta;
		if (RadiusinCentimeters < 0) RadiusinCentimeters = 0;
		SetDlgItemInt( IDC_EDIT12, RadiusinCentimeters);

		OnEditChangeTC();
		return TRUE;
	}

    return 0;
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
/*
 ===================================================================================================
 ===================================================================================================
 */


/*
 ===================================================================================================
 ===================================================================================================
 */


/*
 ===================================================================================================
 ===================================================================================================
 */

/*
 ===================================================================================================
 ===================================================================================================
 */

/*
 ===================================================================================================
 ===================================================================================================
 */


/*
 ===================================================================================================
 ===================================================================================================
 */


/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_SmoothSelectionDialog::OnKeyDown(UINT _ui_Char, UINT _ui_Rep, UINT _ui_Flags )
{
    EDIA_cl_BaseDialog::OnKeyDown( _ui_Char, _ui_Rep, _ui_Flags );
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_SmoothSelectionDialog::OnMouseMove(UINT, CPoint )
{
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_SmoothSelectionDialog::OnLButtonDown(UINT, CPoint)
{
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_SmoothSelectionDialog::OnLButtonUp(UINT, CPoint)
{
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_SmoothSelectionDialog::OnRButtonDown(UINT, CPoint)
{
}

void EDIA_cl_SmoothSelectionDialog::OnCheck_InvertSel(void)
{
	InvertSel ^= 1;
	SmoothSelMustBeUpdate = 1;
	LINK_Refresh();
	SmoothSelMustBeUpdate = 0;
}

void EDIA_cl_SmoothSelectionDialog::OnCheck_TopoCare(void)
{
	SmoothSelMode ^= 2;
	UpdateEnability();
	SmoothSelMustBeUpdate = 1;
	LINK_Refresh();
	SmoothSelMustBeUpdate = 0;
}

void EDIA_cl_SmoothSelectionDialog::OnCheck_Radius(void)
{
	SmoothSelMode ^= 1;
	UpdateEnability();
	SmoothSelMustBeUpdate = 1;
	LINK_Refresh();
	SmoothSelMustBeUpdate = 0;
}
void EDIA_cl_SmoothSelectionDialog::UpdateEnability(void)
{
	if (SmoothSelMode & 1)  
		GetDlgItem(IDC_EDIT1)->EnableWindow( 1 );
	else
		GetDlgItem(IDC_EDIT1)->EnableWindow( 0 );
	if (SmoothSelMode & 2)  
		GetDlgItem(IDC_EDIT12)->EnableWindow( 1 );
	else
		GetDlgItem(IDC_EDIT12)->EnableWindow( 0 );
}
void EDIA_cl_SmoothSelectionDialog::OnEditChangeTC(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	int RadiusinCentimeters;
	/*~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT12)->GetWindowText(asz_Temp, 512);
	sscanf(asz_Temp, "%d", &RadiusinCentimeters);
	topologicCare = (float)RadiusinCentimeters / 100.0f;
	UpdateEnability();
	SmoothSelMustBeUpdate = 1;
	LINK_Refresh();
	SmoothSelMustBeUpdate = 0;
}
void EDIA_cl_SmoothSelectionDialog::OnEditChangeRadius(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	int RadiusinCentimeters;
	/*~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT1)->GetWindowText(asz_Temp, 512);
	sscanf(asz_Temp, "%d", &RadiusinCentimeters);
	DistanceMax = (float)RadiusinCentimeters / 100.0f;
	UpdateEnability();
	SmoothSelMustBeUpdate = 1;
	LINK_Refresh();
	SmoothSelMustBeUpdate = 0;
}


/*
 ===================================================================================================
 ===================================================================================================
 */

/*void EDIA_cl_SmoothSelectionDialog::OnPaint(void)
{
}*/

/*
 ===================================================================================================
 ===================================================================================================
 */
/*BOOL EDIA_cl_SmoothSelectionDialog::OnEraseBkgnd(CDC *pdc)
{
    return TRUE;
}*/

#endif /* ACTIVE_EDITORS */
