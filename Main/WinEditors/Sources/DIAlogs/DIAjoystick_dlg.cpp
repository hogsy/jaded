/*$T DIAjoystick_dlg.cpp GC! 1.081 06/19/00 10:43:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAjoystick_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "F3DFrame/F3Dview.h"
#include "SOFT/SOFTbackgroundImage.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "INOut/INOjoystick.h"

BEGIN_MESSAGE_MAP(EDIA_cl_Joystick, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_SETRECT0, OnButton_SetRect0)
	ON_BN_CLICKED(IDC_CHECK_SETRECT1, OnButton_SetRect1)
	ON_BN_CLICKED(IDC_BUTTON_CALIBRATE, OnButton_Calibrate)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Joystick::EDIA_cl_Joystick(void) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_JOYSTICK)
{
	mui_Timer = 0xffffffff;
	mi_SetRect = -1;
	mi_Calibrate = 0;
	mi_CurPad = 0;

	for(int i = 0; i < 2; i++)
	{
		mal_OldValues[i][0] = win32INO_l_Joystick_YDownStart[i];
		mal_OldValues[i][1] = win32INO_l_Joystick_XRightStart[i];
		mal_OldValues[i][2] = win32INO_l_Joystick_YUpStart[i];
		mal_OldValues[i][3] = win32INO_l_Joystick_XLeftStart[i];
		mal_OldValues[i][4] = win32INO_l_Joystick_YDown[i];
		mal_OldValues[i][5] = win32INO_l_Joystick_XRight[i];
		mal_OldValues[i][6] = win32INO_l_Joystick_YUp[i];
		mal_OldValues[i][7] = win32INO_l_Joystick_XLeft[i];
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Joystick::~EDIA_cl_Joystick(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Joystick::OnInitDialog(void)
{
	GetDlgItem(IDC_STATIC_JOYPOS)->GetWindowRect(&mo_JoyPos);
	ScreenToClient(&mo_JoyPos);
	mi_WindowWidth = mo_JoyPos.Width();
	mi_WindowHeight = mo_JoyPos.Height();
	GetDlgItem(IDC_STATIC_JOYPOS)->ShowWindow(SW_HIDE);

	mi_OX = mo_JoyPos.left + 2;
	mi_OY = mo_JoyPos.top + 2;
	mf_RatioX = (float) (mi_WindowWidth - 4) / 65536.0f;
	mf_RatioY = (float) (mi_WindowHeight - 4) / 65536.0f;

	mui_Timer = SetTimer(1, 50, NULL);

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::OnOK(void)
{
	if(mui_Timer != 0xFFFFFFFF) KillTimer(mui_Timer);
    EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::OnCancel(void)
{
	if(mui_Timer != 0xFFFFFFFF) KillTimer(mui_Timer);

	for(int i = 0; i < 2; i++)
	{
		win32INO_l_Joystick_YDownStart[i] = mal_OldValues[i][0];
		win32INO_l_Joystick_XRightStart[i] = mal_OldValues[i][1];
		win32INO_l_Joystick_YUpStart[i] = mal_OldValues[i][2];
		win32INO_l_Joystick_XLeftStart[i] = mal_OldValues[i][3];
		win32INO_l_Joystick_YDown[i] = mal_OldValues[i][4];
		win32INO_l_Joystick_XRight[i] = mal_OldValues[i][5];
		win32INO_l_Joystick_YUp[i] = mal_OldValues[i][6];
		win32INO_l_Joystick_XLeft[i] = mal_OldValues[i][7];
	}

    EDIA_cl_BaseDialog::OnCancel();
}

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::OnTimer(UINT)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				sz_Text[32];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	INO_Joystick_Update();
	mo_JoyPos.InflateRect(2, 2);
	InvalidateRect(&mo_JoyPos);
	mo_JoyPos.DeflateRect(2, 2);
	((CButton *) GetDlgItem(IDC_CHECK_A))->SetCheck(INO_b_Joystick_IsButtonDown(0) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_B))->SetCheck(INO_b_Joystick_IsButtonDown(1) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_C))->SetCheck(INO_b_Joystick_IsButtonDown(2) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_D))->SetCheck(INO_b_Joystick_IsButtonDown(3) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_E))->SetCheck(INO_b_Joystick_IsButtonDown(4) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_F))->SetCheck(INO_b_Joystick_IsButtonDown(5) ? 1 : 0);

	sprintf(sz_Text, "%.3f (%d)", (INO_l_Joystick_X[mi_CurPad] / 32767.0f) - 1.0f, INO_l_Joystick_X[mi_CurPad]);
	GetDlgItem(IDC_EDIT_X)->SetWindowText(sz_Text);
	sprintf(sz_Text, "%.3f (%d)", (INO_l_Joystick_Y[mi_CurPad] / 32767.0f) - 1.0f, INO_l_Joystick_Y[mi_CurPad]);
	GetDlgItem(IDC_EDIT_Y)->SetWindowText(sz_Text);

	if(mi_SetRect == 0)
	{
		if(INO_b_Joystick_IsButtonDown(0)) win32INO_l_Joystick_YDownStart[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		if(INO_b_Joystick_IsButtonDown(1)) win32INO_l_Joystick_XRightStart[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_b_Joystick_IsButtonDown(2)) win32INO_l_Joystick_YUpStart[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		if(INO_b_Joystick_IsButtonDown(3)) win32INO_l_Joystick_XLeftStart[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
	}
	else if(mi_SetRect == 1)
	{
		if(INO_b_Joystick_IsButtonDown(0)) win32INO_l_Joystick_YDown[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		if(INO_b_Joystick_IsButtonDown(1)) win32INO_l_Joystick_XRight[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_b_Joystick_IsButtonDown(2)) win32INO_l_Joystick_YUp[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		if(INO_b_Joystick_IsButtonDown(3)) win32INO_l_Joystick_XLeft[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
	}

	switch(mi_Calibrate)
	{
	case 1:
	case 6:
		if(INO_b_Joystick_IsButtonDown(0))
		{
			mi_Calibrate++;
			mi_Counter = 0;
			UpdateCalibrateText();
		}
		break;
	case 2:
	case 7:
		if(mi_Counter++ == 100)
		{
			mi_Calibrate++;
			UpdateCalibrateText();
		}

		if(INO_l_Joystick_X[mi_CurPad] < win32INO_l_Joystick_XLeftStart[mi_CurPad]) win32INO_l_Joystick_XLeftStart[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_l_Joystick_X[mi_CurPad] > win32INO_l_Joystick_XRightStart[mi_CurPad]) win32INO_l_Joystick_XRightStart[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_l_Joystick_Y[mi_CurPad] < win32INO_l_Joystick_YUpStart[mi_CurPad]) win32INO_l_Joystick_YUpStart[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		if(INO_l_Joystick_Y[mi_CurPad] > win32INO_l_Joystick_YDownStart[mi_CurPad]) win32INO_l_Joystick_YDownStart[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		break;
	case 3:
	case 8:
		if(INO_b_Joystick_IsButtonDown(0))
		{
			mi_Calibrate++;
			mi_Counter = 0;
			UpdateCalibrateText();
		}

		if(INO_l_Joystick_X[mi_CurPad] < win32INO_l_Joystick_XLeft[mi_CurPad]) 
			win32INO_l_Joystick_XLeft[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_l_Joystick_X[mi_CurPad] > win32INO_l_Joystick_XRight[mi_CurPad]) 
			win32INO_l_Joystick_XRight[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_l_Joystick_Y[mi_CurPad] < win32INO_l_Joystick_YUp[mi_CurPad]) 
			win32INO_l_Joystick_YUp[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		if(INO_l_Joystick_Y[mi_CurPad] > win32INO_l_Joystick_YDown[mi_CurPad]) 
			win32INO_l_Joystick_YDown[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		break;
	case 4:
	case 9:
		if(INO_b_Joystick_IsButtonDown(0))
		{
			mi_Calibrate++;
			mi_Counter = 0;
			UpdateCalibrateText();
		}

	case 5:
	case 10:
		if(mi_Counter++ == 100)
		{
			if(mi_Calibrate == 5)
			{
				mi_CurPad = 1;
				mi_Calibrate++;
			    mi_SetRect = -1;
				UpdateCalibrateText();
			}
			else
			{
				mi_CurPad = 0;
				mi_Calibrate = 0;
				UpdateCalibrateText();
				GetDlgItem(IDC_BUTTON_CALIBRATE)->EnableWindow(TRUE);
			}

            win32INO_l_Joystick_XLeftStart[mi_CurPad] -= 1000;
            win32INO_l_Joystick_XRightStart[mi_CurPad] += 1000;
            win32INO_l_Joystick_YUpStart[mi_CurPad] -= 1000;
            win32INO_l_Joystick_YDownStart[mi_CurPad] += 1000;
		}

		if(INO_l_Joystick_X[mi_CurPad] < win32INO_l_Joystick_XLeftStart[mi_CurPad]) win32INO_l_Joystick_XLeftStart[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_l_Joystick_X[mi_CurPad] > win32INO_l_Joystick_XRightStart[mi_CurPad]) win32INO_l_Joystick_XRightStart[mi_CurPad] = INO_l_Joystick_X[mi_CurPad];
		if(INO_l_Joystick_Y[mi_CurPad] < win32INO_l_Joystick_YUpStart[mi_CurPad]) win32INO_l_Joystick_YUpStart[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		if(INO_l_Joystick_Y[mi_CurPad] > win32INO_l_Joystick_YDownStart[mi_CurPad]) win32INO_l_Joystick_YDownStart[mi_CurPad] = INO_l_Joystick_Y[mi_CurPad];
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	CDC		*pDC;
	int		x1, x2, y1, y2;
	CPen	o_RedPen[2], o_BluePen[2], o_GreenPen[2];
	void	*p_OldPen;
	CRgn	o_Region;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	pDC = GetDC();
	o_RedPen[0].CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	o_BluePen[0].CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	o_GreenPen[0].CreatePen(PS_SOLID, 1, RGB(0, 0xFF, 0));
	o_RedPen[1].CreatePen(PS_SOLID, 1, RGB(0x8F, 0, 0));
	o_BluePen[1].CreatePen(PS_SOLID, 1, RGB(0, 0, 0x8F));
	o_GreenPen[1].CreatePen(PS_SOLID, 1, RGB(0, 0x8F, 0));

	GetDlgItem(IDC_STATIC_JOYPOS)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	o_Rect.InflateRect(2, 2);
	o_Region.CreateRectRgn(o_Rect.left, o_Rect.top, o_Rect.right, o_Rect.bottom);
	pDC->SelectClipRgn(&o_Region);

	for(int pad = 0; pad < 2; pad++)
	{
		p_OldPen = pDC->SelectObject(o_BluePen[pad]);
		x1 = mi_OX + (int) ((float) win32INO_l_Joystick_XLeft[pad] * mf_RatioX);
		y1 = mi_OY + (int) ((float) win32INO_l_Joystick_YUp[pad] * mf_RatioY);
		x2 = mi_OX + (int) ((float) win32INO_l_Joystick_XRight[pad] * mf_RatioX);
		y2 = mi_OY + (int) ((float) win32INO_l_Joystick_YDown[pad] * mf_RatioY);
		pDC->MoveTo(x1, y1);
		pDC->LineTo(x2, y1);
		pDC->LineTo(x2, y2);
		pDC->LineTo(x1, y2);
		pDC->LineTo(x1, y1);

		pDC->SelectObject(o_GreenPen[pad]);
		x1 = mi_OX + (int) ((float) win32INO_l_Joystick_XLeftStart[pad] * mf_RatioX);
		y1 = mi_OY + (int) ((float) win32INO_l_Joystick_YUpStart[pad] * mf_RatioY);
		x2 = mi_OX + (int) ((float) win32INO_l_Joystick_XRightStart[pad] * mf_RatioX);
		y2 = mi_OY + (int) ((float) win32INO_l_Joystick_YDownStart[pad] * mf_RatioY);
		pDC->MoveTo(x1, y1);
		pDC->LineTo(x2, y1);
		pDC->LineTo(x2, y2);
		pDC->LineTo(x1, y2);
		pDC->LineTo(x1, y1);

		pDC->SelectObject(o_RedPen[pad]);
		x1 = mi_OX + (int) ((float) INO_l_Joystick_X[pad] * mf_RatioX);
		y1 = mi_OY + (int) ((float) INO_l_Joystick_Y[pad] * mf_RatioY);
		pDC->MoveTo(x1 + 3, y1);
		pDC->LineTo(x1 - 3, y1);
		pDC->MoveTo(x1, y1 + 3);
		pDC->LineTo(x1, y1 - 3);

		if(mi_SetRect != -1)
		{
			pDC->SetTextColor(mi_SetRect ? RGB(0, 0, 0xFF) : RGB(0, 0xFF, 0));
			pDC->SetBkMode(TRANSPARENT);

			x1 = (int) (mi_OX + 0.5f * (float) mi_WindowWidth);
			y1 = (int) (mi_OY + 0.75f * (float) mi_WindowHeight);
			o_Rect = CRect(x1 - 8, y1 - 8, x1 + 8, y1 + 8);
			pDC->DrawText("A", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			x1 = (int) (mi_OX + 0.75f * (float) mi_WindowWidth);
			y1 = (int) (mi_OY + 0.5f * (float) mi_WindowHeight);
			o_Rect = CRect(x1 - 8, y1 - 8, x1 + 8, y1 + 8);
			pDC->DrawText("B", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			x1 = (int) (mi_OX + 0.5f * (float) mi_WindowWidth);
			y1 = (int) (mi_OY + 0.25f * (float) mi_WindowHeight);
			o_Rect = CRect(x1 - 8, y1 - 8, x1 + 8, y1 + 8);
			pDC->DrawText("C", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			x1 = (int) (mi_OX + 0.25f * (float) mi_WindowWidth);
			y1 = (int) (mi_OY + 0.5f * (float) mi_WindowHeight);
			o_Rect = CRect(x1 - 8, y1 - 8, x1 + 8, y1 + 8);
			pDC->DrawText("D", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
	}

	pDC->SelectClipRgn(NULL);
	pDC->SelectObject(p_OldPen);
	DeleteObject(o_RedPen);
	DeleteObject(o_BluePen);
	DeleteObject(o_GreenPen);
	ReleaseDC(pDC);
	o_Region.DeleteObject();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::OnButton_SetRect0(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_SETRECT0))->GetCheck())
	{
		((CButton *) GetDlgItem(IDC_CHECK_SETRECT1))->SetCheck(0);
		mi_SetRect = 0;
	}
	else
	{
		mi_SetRect = -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::OnButton_SetRect1(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_SETRECT1))->GetCheck())
	{
		((CButton *) GetDlgItem(IDC_CHECK_SETRECT0))->SetCheck(0);
		mi_SetRect = 1;
	}
	else
	{
		mi_SetRect = -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::OnButton_Calibrate(void)
{
	GetDlgItem(IDC_BUTTON_CALIBRATE)->EnableWindow(FALSE);
	mi_Calibrate = 1;
	UpdateCalibrateText();
    mi_SetRect = -1;
	mi_CurPad = 0;

	for(int i = 0; i < 2; i++)
	{
		win32INO_l_Joystick_YDownStart[i] = 0;
		win32INO_l_Joystick_XRightStart[i] = 0;
		win32INO_l_Joystick_YUpStart[i] = 65536;
		win32INO_l_Joystick_XLeftStart[i] = 65536;
		win32INO_l_Joystick_YDown[i] = 32768;
		win32INO_l_Joystick_XRight[i] = 32768;
		win32INO_l_Joystick_YUp[i] = 32768;
		win32INO_l_Joystick_XLeft[i] = 32768;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Joystick::UpdateCalibrateText(void)
{
	switch(mi_Calibrate)
	{
	case 0:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("");
		break;
	case 1:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Release joy 0 (then click A)");
		break;
	case 2:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Wait...");
		break;
	case 3:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Move joy 0 in all dir (then click A)");
		break;
	case 4:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Release joy 0 (then click A)");
		break;
	case 5:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Wait...");
		break;

	case 6:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Release joy 1 (then click A)");
		break;
	case 7:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Wait...");
		break;
	case 8:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Move joy 1 in all dir (then click A)");
		break;
	case 9:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Release joy 1 (then click A)");
		break;
	case 10:
		GetDlgItem(IDC_STATIC_CALIBRATE)->SetWindowText("Wait...");
		break;
	}
}

#endif /* ACTIVE_EDITORS */
