/*$T DIA_UPDATE_dlg.cpp GC! 1.081 05/24/00 16:13:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "EDIapp.h"

BEGIN_MESSAGE_MAP(EDIA_cl_UPDATEDialog, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CANCEL_MRM, OnButton_Cancel)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_UPDATEDialog::EDIA_cl_UPDATEDialog(char *_psz_Title) :
	EDIA_cl_BaseDialog(IDD_COMPUTEMRM)
{
	CurrentPos = 0.0f;
	bIsCanceled = FALSE;
	ul_TextLenght = 0;
	RealFirstTime = timeGetTime();
	if(_psz_Title != NULL)
	{
		while(*(_psz_Title) && (ul_TextLenght != (DIA_UPDT_MAX_TEXT_LEN - 1)))
		{
			uc_Text[ul_TextLenght++] = *(_psz_Title++);
		}
	}

	uc_Text[ul_TextLenght] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_UPDATEDialog::~EDIA_cl_UPDATEDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UPDATEDialog::OnRefreshBar(float Advance)
{
	/*~~~~*/
	MSG msg;
	/*~~~~*/

	CurrentPos = Advance;
	if(CurrentPos > 1.0f) CurrentPos = 1.0f;
	if(CurrentPos < 0.0f) CurrentPos = 0.0f;

	ul_AccesCounter++;
	switch((ul_AccesCounter) & 3)
	{
	case 0:
		SetDlgItemText(IDC_TURNTURN, "---");
		break;
	case 1:
		SetDlgItemText(IDC_TURNTURN, "\\");
		break;
	case 2:
		SetDlgItemText(IDC_TURNTURN, "|");
		break;
	case 3:
		SetDlgItemText(IDC_TURNTURN, "/");
		break;
	}

	Invalidate();
	UpdateWindow();

	while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// Note: WM_ADDTEXT is used by the EditLog. A very quick edit control (used by Perforce Frame).
		const long WM_ADDTEXT = WM_USER + 500;
		if((::GetParent(msg.hwnd) == m_hWnd) || (msg.hwnd == m_hWnd) || (msg.message == WM_PAINT) || (msg.message == WM_ADDTEXT))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return bIsCanceled;
}

BOOL EDIA_OnlyTotal = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UPDATEDialog::OnRefreshBarText(float Advance, char *Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL			RefreshPlise;
	unsigned long	OldLenght;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(Text != NULL)
	{
		RefreshPlise = FALSE;
		OldLenght = ul_TextLenght;
		ul_TextLenght = 0;
		while(*(Text) && (ul_TextLenght != (DIA_UPDT_MAX_TEXT_LEN - 1)))
		{
			if(uc_Text[ul_TextLenght] != *Text) RefreshPlise = TRUE;
			uc_Text[ul_TextLenght++] = *(Text++);
		}

		uc_Text[ul_TextLenght] = 0;

		if(OldLenght != ul_TextLenght) RefreshPlise = TRUE;

		if(RefreshPlise) SetDlgItemText(IDC_COMPUTINGMRMTEXT, Text - ul_TextLenght);
	}

	if ((LastPos > Advance))
	{
		FirstTime = timeGetTime();
		SetDlgItemText(IDC_COMPUTINGMRMTEXTtime, "");
		SetDlgItemText(IDC_COMPUTINGMRMTEXTtime2, "");
		LastSeconds = 0;
	}

	LastPos = Advance;
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	Seconds, Minutes, Hours, E_Minutes, E_Hours, E_Seconds;
		ULONG	Seconds_C, Minutes_C, Hours_C;
		char	Time[64];
		char	Time2[64];
		char	Time_E[64];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		Seconds = timeGetTime() - FirstTime;
		Seconds_C = (ULONG)((float)Seconds * (1.0f / Advance));
		Seconds_C -= Seconds;
		Seconds /= 1000;
		Seconds_C /= 1000;
//		if(LastSeconds != Seconds)
		{
			LastSeconds = Seconds;
			E_Seconds = timeGetTime() - RealFirstTime;
			E_Seconds /= 1000;
			Minutes = Seconds / 60;
			Hours = Minutes / 60;
			Minutes_C = Seconds_C / 60;
			Hours_C = Minutes_C / 60;
			E_Minutes = E_Seconds / 60;
			E_Hours = E_Minutes / 60;
			Seconds %= 60;
			Minutes %= 60;
			Seconds_C %= 60;
			Minutes_C %= 60;
			E_Seconds %= 60;
			E_Minutes %= 60;

			Time[0] = 0;
			Time_E[0] = 0;

//			if((E_Seconds > 5) || E_Minutes || E_Hours)
			{
				if(E_Hours)
					sprintf(Time_E, "(Total %d h %d m %d s)", E_Hours, E_Minutes, E_Seconds);
				else
					sprintf(Time_E, "(Total %d m %d s)", E_Minutes, E_Seconds);
			}

			if(Hours)
				sprintf(Time, "%d h %d m %d s", Hours, Minutes, Seconds);
			else if(Minutes)
				sprintf(Time, "%d m %d s", Minutes, Seconds);
			else if(Seconds) sprintf(Time, "%d Second(s)", Seconds);

			strcpy(Time2 , Time);

			if(EDIA_OnlyTotal)
			{
				sprintf(Time, "%s ", Time2);
			}
			else
			{
				if(Hours_C)
					sprintf(Time, "(still approx. %d h %d m)", Hours_C, Minutes_C);
				else if(Minutes_C)
				{
					sprintf(Time, "(still approx. %d m %d s)", Minutes_C, Seconds_C);
				}
				else if(Seconds_C) sprintf(Time, "(still approx. %d Seconds)", Seconds_C);
			}
	
			if(Time[0])
			{
				if(!EDIA_OnlyTotal) SetDlgItemText(IDC_COMPUTINGMRMTEXTtime, Time);
				SetDlgItemText(IDC_COMPUTINGMRMTEXTtime2, Time_E);
			}
		}
	}

	EDIA_OnlyTotal = FALSE;
	return OnRefreshBar(Advance);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UPDATEDialog::OnButton_Cancel(void)
{
	bIsCanceled = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UPDATEDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect, o_Rect1;
	int		iDiv;
	/*~~~~~~~~~~~~~~~~~~~~*/

	SetDlgItemText(IDC_COMPUTINGMRMTEXT, uc_Text);
	SetDlgItemText(IDC_COMPUTINGMRMTEXTtime, "");
	SetDlgItemText(IDC_COMPUTINGMRMTEXTtime2, "");

	if(EDI_gpo_EnterWnd)
	{
		EDI_gpo_EnterWnd->GetClientRect(&o_Rect);
		o_Rect.top = o_Rect.bottom - 17;
		EDI_gpo_EnterWnd->ClientToScreen(&o_Rect);
	}
	else
	{
		M_MF()->mo_Status.GetItemRect(1, &o_Rect);
		M_MF()->mo_Status.ClientToScreen(&o_Rect);
	}


	o_Rect1 = o_Rect;
	MoveWindow(&o_Rect);
	ScreenToClient(&o_Rect);
	if(!EDI_gpo_EnterWnd) o_Rect.InflateRect(-1, -1);

	o_Rect1 = o_Rect;

	if(!EDI_gpo_EnterWnd) o_Rect1.right -= 50;

	GetDlgItem(IDC_COMPUTINGMRM)->MoveWindow(&o_Rect1);

	if(EDI_gpo_EnterWnd)
	{
		GetDlgItem(IDC_CANCEL_MRM)->EnableWindow(FALSE);
	}
	else
	{
		o_Rect1 = o_Rect;
		o_Rect1.left = o_Rect1.right - 50;
		GetDlgItem(IDC_CANCEL_MRM)->MoveWindow(&o_Rect1);
	}

	o_Rect1 = o_Rect;
	o_Rect1.left += 5;
	iDiv = o_Rect.Width() / 3;
	o_Rect1.right = iDiv;
	GetDlgItem(IDC_COMPUTINGMRMTEXT)->MoveWindow(o_Rect1);
	o_Rect1.left = o_Rect1.right;
	o_Rect1.right = o_Rect1.left + iDiv;
	GetDlgItem(IDC_COMPUTINGMRMTEXTtime)->MoveWindow(o_Rect1);
	o_Rect1.left = o_Rect1.right;
	o_Rect1.right = o_Rect1.left + iDiv;
	GetDlgItem(IDC_COMPUTINGMRMTEXTtime2)->MoveWindow(o_Rect1);

	LastPos = 100.0f;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UPDATEDialog::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC			*PDC;
	PAINTSTRUCT ps;
	RECT		LocalRect, LocalRect2;
	HBRUSH		brh;
	CRect		o_Rect;
	CString		o_Text;
	CFont		*FNT;
	COLORREF	col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->SetFocus();

	BeginPaint(&ps);

	GetDlgItem(IDC_COMPUTINGMRM)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);

	PDC = GetDC();
	PDC->Draw3dRect(&LocalRect, 0, GetSysColor(COLOR_3DHILIGHT));

	ReleaseDC(PDC);

	if(!EDI_gpo_EnterWnd)
	{
		LocalRect.bottom--;
		LocalRect.top++;
	}
	LocalRect.right--;
	LocalRect.top++;
	LocalRect2 = LocalRect;

	LocalRect.right = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * CurrentPos);
	LocalRect2.left = LocalRect.right;

	brh = CreateSolidBrush(((unsigned long) ((1.0f - CurrentPos) * 255.0f) << 16) | (unsigned long) (CurrentPos * 255.0f));
	FillRect(ps.hdc, &LocalRect, brh);
	DeleteObject(brh);

	if(EDI_gpo_EnterWnd)
		brh = CreateSolidBrush(0xFFFFFFFF);
	else
		brh = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	FillRect(ps.hdc, &LocalRect2, brh);
	DeleteObject(brh);

	PDC = GetDC();
	FNT = PDC->SelectObject(&M_MF()->mo_Fnt);
	PDC->SetBkMode(TRANSPARENT);

	GetDlgItem(IDC_COMPUTINGMRMTEXT)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	GetDlgItem(IDC_COMPUTINGMRMTEXT)->GetWindowText(o_Text);
	if(!EDI_gpo_EnterWnd && (o_Rect.left > LocalRect.right))
		col = 0;
	else 
		col = 0x00FFFFFF;
	PDC->SetTextColor(col);
	PDC->ExtTextOut(o_Rect.left, o_Rect.top, 0, &o_Rect, o_Text, NULL);

	GetDlgItem(IDC_COMPUTINGMRMTEXTtime)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	GetDlgItem(IDC_COMPUTINGMRMTEXTtime)->GetWindowText(o_Text);
	if(!EDI_gpo_EnterWnd && (o_Rect.left > LocalRect.right))
		col = 0;
	else 
		col = 0x00FFFFFF;
	PDC->SetTextColor(col);
	PDC->ExtTextOut(o_Rect.left, o_Rect.top, 0, &o_Rect, o_Text, NULL);

	GetDlgItem(IDC_COMPUTINGMRMTEXTtime2)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	GetDlgItem(IDC_COMPUTINGMRMTEXTtime2)->GetWindowText(o_Text);
	if(!EDI_gpo_EnterWnd && (o_Rect.left > LocalRect.right))
		col = 0;
	else 
		col = 0x00FFFFFF;
	PDC->SetTextColor(col);
	PDC->ExtTextOut(o_Rect.left, o_Rect.top, 0, &o_Rect, o_Text, NULL);

	PDC->SelectObject(FNT);
	ReleaseDC(PDC);

	EndPaint(&ps);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UPDATEDialog::OnEraseBkgnd(CDC *pdc)
{
	return TRUE;
}

#endif /* ACTIVE_EDITORS */
