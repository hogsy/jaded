#include "StdAfx.h"
#include "GetParamDlg.h"
#include "resource.h"


CGetParamDlg::CGetParamDlg(Interface& _ip):
ip (_ip)
{
}

CGetParamDlg::~CGetParamDlg()
{
}

int CGetParamDlg::DoModal()
{
	return (DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_ASK_PARAM), ip.GetMAXHWnd(), DlgProc, (LPARAM) this));
}

BOOL CALLBACK CGetParamDlg::DlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	CGetParamDlg* dlg = NULL;

	if (msg==WM_INITDIALOG)
	{
		dlg = reinterpret_cast<CGetParamDlg*>(lParam);
		SetWindowLong(hWnd, GWL_USERDATA, lParam);
	}
	else
	{
        dlg=reinterpret_cast<CGetParamDlg*>(GetWindowLong(hWnd, GWL_USERDATA));
		if (!dlg)
			return FALSE;
	}

    switch (msg)
    {
    case WM_INITDIALOG:
        dlg->Init( hWnd );
        break;

    case WM_DESTROY:
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_PARAM_YES:
			EndDialog(hWnd, 1);
            break;
        case IDC_PARAM_NO:
            EndDialog(hWnd, 0);
            break;
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

void CGetParamDlg::Init( HWND hWnd )
{
	CenterWindow(hWnd, ip.GetMAXHWnd());
}