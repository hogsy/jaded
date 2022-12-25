/*$T DIAbase.cpp GC 1.134 04/30/04 15:03:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Base/ERRors/ERRasser.h"
#include "EDIerrid.h"
#include "EDIapp.h"
#include "DIAlogs/DIAbase.h"
#include "DIAlogs/BtnST.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "ProSliderCtrl.h"
#include "FlatHeaderCtrl.h"
#include "EnTabCtrl.h"

extern BOOL EDI_gb_SX;
extern void SetTransparentWnd(CWnd *, int);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_BaseDialog, CDialog)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDYES, OnYes)
	ON_COMMAND(IDNO, OnNo)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_BaseDialog::EDIA_cl_BaseDialog(UINT nIDTemplate, CWnd* pParentWnd) :
	CDialog(nIDTemplate, pParentWnd)
{
	mui_Res = nIDTemplate;
	mb_Modeless = FALSE;
	mb_Minimize = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_BaseDialog::~EDIA_cl_BaseDialog(void)
{
	EDI_go_TheApp.RemoveModeless(m_hWnd);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::SetTheme(CWnd *po_Parent)
{
	/*~~~~~~~~~~~~~~~~*/
	CWnd		*po;
	CButtonST	*pb;
	char		az[100];
	int			id;
	DWORD		style;
	/*~~~~~~~~~~~~~~~~*/

	if(!EDI_gb_SX) return;
	po = po_Parent->GetWindow(GW_CHILD);
	if(po)
	{
		po = po->GetWindow(GW_HWNDFIRST);
		while(po)
		{
			SetTheme(po);
			GetClassName(po->m_hWnd, az, 100);
			if(FromHandlePermanent(po->m_hWnd) == NULL)
			{
				if(!L_strcmpi(az, "SysTabControl32"))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					CEnTabCtrl	*pb = new CEnTabCtrl;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pb->SubclassDlgItem(po->GetDlgCtrlID(), po_Parent);
					pb->EnableCustomLook(TRUE, ETC_GRADIENT);
				}
				else if(!L_strcmpi(az, "SysHeader32"))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					CFlatHeaderCtrl *pb = new CFlatHeaderCtrl;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pb->SubclassDlgItem(po->GetDlgCtrlID(), po_Parent);
				}
				else if(!L_strcmpi(az, "msctls_trackbar32"))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					CProSliderCtrl	*pb = new CProSliderCtrl;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pb->SubclassDlgItem(po->GetDlgCtrlID(), po_Parent);
				}
				else if(!L_strcmpi(az, "Button"))
				{
					style = ((CButton *) po)->GetButtonStyle();
					style &= SS_TYPEMASK;

					if(style == BS_GROUPBOX)
					{
						pb = new CButtonST;
						pb->m_bIsXP = FALSE;
						pb->m_bIsGrpBox = TRUE;
						pb->SubclassWindow(po->m_hWnd);
						pb->EnableWindow(FALSE);
					}
					else
					{
						id = po->GetDlgCtrlID();
						if(id != -1)
						{
							if
							(
								style == BS_RADIOBUTTON
							||	style == BS_AUTORADIOBUTTON
							||	style == BS_PUSHBUTTON
							||	style == BS_DEFPUSHBUTTON
							||	style == BS_AUTOCHECKBOX
							||	style == BS_3STATE
							||	style == BS_AUTO3STATE
							||	po->GetStyle() & BS_ICON
							)
							{
								if(style == BS_RADIOBUTTON || style == BS_AUTORADIOBUTTON)
								{
									if(po->GetStyle() & BS_PUSHLIKE)
									{
										po->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);
										pb = new CButtonST;
										pb->SubclassDlgItem(po->GetDlgCtrlID(), po_Parent);
									}
									else
									{
										po->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);
										pb = new CButtonST;
										pb->m_bIsXP = FALSE;
										pb->m_bIsRealCheckBox = TRUE;
										pb->SubclassDlgItem(po->GetDlgCtrlID(), po_Parent);
										pb->SetBitmaps
											(
												IDB_RADIOON,
												RGB(192, 192, 192),
												IDB_RADIOOFF,
												RGB(192, 192, 192)
											);
									}
								}
								else if
									(
										style != BS_AUTOCHECKBOX
									||	po->GetStyle() & (BS_PUSHLIKE | BS_BITMAP | BS_ICON)
									)
								{
									po->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);
									pb = new CButtonST;
									pb->SubclassDlgItem(po->GetDlgCtrlID(), po_Parent);
								}
								else	/* Check */
								{
									po->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);
									pb = new CButtonST;
									pb->m_bIsXP = FALSE;
									pb->m_bIsRealCheckBox = TRUE;
									pb->SubclassDlgItem(po->GetDlgCtrlID(), po_Parent);
									pb->SetBitmaps(IDB_CHECKON, RGB(192, 192, 192), IDB_CHECKOFF, RGB(192, 192, 192));
								}
							}
						}
					}
				}
			}

			po = po->GetWindow(GW_HWNDNEXT);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BaseDialog::OnInitDialog(void)
{
	SetTheme(this);
	return CDialog::OnInitDialog();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnSize(UINT n, int cx, int cy)
{
	mb_Minimize = FALSE;
	CDialog::OnSize(n, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HBRUSH EDIA_cl_BaseDialog::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnNcLButtonDown(UINT n, CPoint pt)
{
	/* SetTransparentWnd(this, 230); */
	CDialog::OnNcLButtonDown(n, pt);

	/* SetTransparentWnd(this, 255); */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnNcLButtonUp(UINT n, CPoint pt)
{
	CDialog::OnNcLButtonUp(n, pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BaseDialog::OnEraseBkgnd(CDC *pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_BaseDialog::DoModal(void)
{
	/*~~~~~~~~~~~*/
	int		mi_Res;
	HWND	hFocus;
	/*~~~~~~~~~~~*/

	mb_Modeless = FALSE;
	EDI_gb_LockKeyboard = TRUE;
	hFocus = ::GetFocus();
	EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
	mi_Res = CDialog::DoModal();
	if(hFocus && ::IsWindow(hFocus)) ::SetFocus(hFocus);
    EDI_go_TheApp.mpo_FocusWndWhenDlg = NULL;
	EDI_gb_LockKeyboard = FALSE;

	return mi_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_BaseDialog::DoModeless(void)
{
	HWND	hFocus;

	mb_Modeless = TRUE;
	hFocus = ::GetFocus();
	Create(MAKEINTRESOURCE(mui_Res), NULL);
	EDI_go_TheApp.AddModeless(m_hWnd, TRUE);
	if(hFocus && ::IsWindow(hFocus)) ::SetFocus(hFocus);
	ShowWindow(SW_SHOW);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnClose(void)
{
	EDI_go_TheApp.RemoveModeless(m_hWnd);
	CDialog::OnClose();
	if(mb_Modeless) DestroyWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnOK(void)
{
	if(!mb_Modeless)
		CDialog::OnOK();
	else
		DestroyWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnYes(void)
{
	if(!mb_Modeless)
		EndDialog(IDYES);
	else
		DestroyWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnNo(void)
{
	if(!mb_Modeless)
		EndDialog(IDNO);
	else
		DestroyWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnCancel(void)
{
	if(!mb_Modeless)
		EndDialog(IDCANCEL);
	else
		DestroyWindow();
}

/*
 =======================================================================================================================
    Aim: Only to set an exception handling before process all messages.
 =======================================================================================================================
 */
LRESULT EDIA_cl_BaseDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~*/
	LRESULT lResult;
	/*~~~~~~~~~~*/

_Try_
    lResult = CDialog::WindowProc(message, wParam, lParam);
_Catch_
    lResult = 0;
_End_

    return lResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BaseDialog::OnDestroy(void)
{
	EDI_go_TheApp.RemoveModeless(m_hWnd);
	if(mb_Modeless) delete this;
}
#endif /* ACTIVE_EDITORS */
