// DIAComboBoxCompletion.cpp : implementation file
//

#include "precomp.h"
#include "BIGfiles/BIGfat.h"
#include "EDIpaths.h"
#include "DIAComboBoxCompletion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBoxCompletion

CComboBoxCompletion::CComboBoxCompletion()
{
	m_bAutoComplete = TRUE;
}

CComboBoxCompletion::~CComboBoxCompletion()
{
}


BEGIN_MESSAGE_MAP(CComboBoxCompletion, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxCompletion)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxCompletion message handlers

BOOL CComboBoxCompletion::PreTranslateMessage(MSG* pMsg)
{
	// Need to check for backspace/delete. These will modify the text in
	// the edit box, causing the auto complete to just add back the text
	// the user has just tried to delete. 

	if (pMsg->message == WM_KEYDOWN)
	{
		m_bAutoComplete = TRUE;

		int nVirtKey = (int) pMsg->wParam;
		if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
			m_bAutoComplete = FALSE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CComboBoxCompletion::OnEditUpdate() 
{
  // if we are not to auto update the text, get outta here
  if (!m_bAutoComplete) 
      return;

  // Get the text in the edit box
  CString str;
  GetWindowText(str);
  int nLength = str.GetLength();
  
  // Currently selected range
  DWORD dwCurSel = GetEditSel();
  WORD dStart = LOWORD(dwCurSel);
  WORD dEnd   = HIWORD(dwCurSel);

  // Search for, and select in, and string in the combo box that is prefixed
  // by the text in the edit box
  if (SelectString(-1, str) == CB_ERR)
  {
      SetWindowText(str);		// No text selected, so restore what was there before
      if (dwCurSel != CB_ERR)
        SetEditSel(dStart, dEnd);	//restore cursor postion
  }

  // Set the text selection as the additional text that we have added
  if (dEnd < nLength && dwCurSel != CB_ERR)
      SetEditSel(dStart, dEnd);
  else
      SetEditSel(nLength, -1);
}

void CComboBoxCompletion::FillCombo(ULONG ul_File)
{
	CString strTmp;

	while(ul_File != BIG_C_InvalidIndex)
	{
		strTmp = BIG_NameFile(ul_File);
			
		// Remove file extension if present
		// Remove file extension if present
		if (strTmp.Find(EDI_Csz_ExtGameObject) != -1)
			strTmp.Delete(strTmp.GetLength()-strlen(EDI_Csz_ExtGameObject), strlen(EDI_Csz_ExtGameObject));

		AddString(strTmp);

		ul_File = BIG_NextFile(ul_File);
	}
}

BOOL CComboBoxCompletion::IsNamePresent(CString& strName)
{
	int nIndex = 0;
	char* pstrName;
	BOOL bIsNamePresent = FALSE ;
	
	// Loop through the files in the directory
	while((nIndex = FindStringExact(nIndex, LPCTSTR(strName))) != CB_ERR)
	{
		BOOL bTwoDigit;
		int iMaxIncrement = 0;

		bIsNamePresent = TRUE;

		GetLBText(nIndex, strName);

		pstrName = strName.GetBuffer();

		// Check if we have two digit at the end of the string
		bTwoDigit = isdigit(int(pstrName[L_strlen(pstrName)-2]));
		bTwoDigit &= isdigit(int(pstrName[L_strlen(pstrName)-1]));

		if ( bTwoDigit )
		{
			iMaxIncrement = atoi(pstrName + L_strlen(pstrName)-2) + 1; 
			sprintf(pstrName + L_strlen(pstrName)-2, "%02d", iMaxIncrement);
			strName.ReleaseBuffer();
		}
		else
		{
			iMaxIncrement++ ;

			char strMaxIncrement[3];
			sprintf(strMaxIncrement,"%02d", iMaxIncrement);

			strName.ReleaseBuffer();
			strName.Append(strMaxIncrement);
		}
	}
	return bIsNamePresent ;
}
