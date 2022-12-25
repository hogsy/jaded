// NumEdit.cpp : implementation file
//

#include "precomp.h"
#include "NumEdit.h"

#include <errno.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit

CNumEdit::CNumEdit()
{
	myType = TypeDouble;
	myRejectingChange = false;
}

CNumEdit::~CNumEdit()
{
}


BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_BACK:
	case VK_RETURN:
	case 0x0A: // Shift+Enter (= linefeed)
	case VK_ESCAPE:
	case VK_TAB:
		break;
	default:
		myLastSel = GetSel();
		GetWindowText(myLastValidValue);
		break;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CNumEdit::OnUpdate() 
{
	if (! myRejectingChange)
	{
		CString aValue;
		GetWindowText(aValue);
		LPTSTR aEndPtr;
		union
		{
			long aLongValue;
			double aDoubleValue;
		};

		errno = 0;
		switch (myType)
		{
		case TypeLong:
			aLongValue = _tcstol(aValue, &aEndPtr, 10);
			break;
		case TypeDouble:
			aDoubleValue = _tcstod(aValue, &aEndPtr);
			break;
		}

		if (! (*aEndPtr) && errno != ERANGE)
		{
			myLastValidValue = aValue;
		}
		else
		{
			myRejectingChange = true;
			SetWindowText(myLastValidValue);
			myRejectingChange = false;
			SetSel(myLastSel);
		}
	}
}
