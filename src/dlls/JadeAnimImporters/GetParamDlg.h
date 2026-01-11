/*****************************************************************

 Gilles jaffier @ Ubisoft Pictures - 04/2006

	Ask for parameters to the user.
	Only one parameter now : Skip translation.

 *****************************************************************/

#ifndef GETPARAMDLG_H
#define GETPARAMDLG_H

class CGetParamDlg
{
public:
	CGetParamDlg(Interface& _ip);
	~CGetParamDlg();

	int DoModal();

private:
	// Methods
	static BOOL CALLBACK	DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void					Init(HWND hWnd);

	// Attributes
    Interface& ip;
};


#endif