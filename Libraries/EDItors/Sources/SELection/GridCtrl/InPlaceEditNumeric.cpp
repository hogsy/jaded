/**
 *	InPlaceEditNumeric.cpp - CInPlaceEditNumeric implementation
 *	
 *	Written By Gurmeet S. Kochar <gomzygotit@hotmail.com>
 *	Copyright (c) 2004. All rights reserved.
 *
 *	This code may be used in compiled form in any way you desire 
 *	(including commercial use). The code may be redistributed 
 *	unmodified by any means PROVIDING it is not sold for profit 
 *	without the authors written consent, and providing that this 
 *	notice and the authors name and all copyright notices remains 
 *	intact. However, this file and the accompanying source code may 
 *	not be hosted on a website or bulletin board without the authors 
 *	written permission.
 *	
 *	This file is provided "AS IS" with no expressed or implied warranty.
 *	The author accepts no liability for any damage/loss of business that
 *	this product may cause.
 *
 *	HISTORY:
 *		Mar 03, 2004 (Gurmeet S. Kochar)
 *			-> First release.
 *		Mar 04, 2004 (Gurmeet S. Kochar)
 *			-> Added WM_PASTE message handling.
 *			   Suggested by snakeware (www.codeproject.com).
 *			-> Minor code changes to extract decimal and 
 *			   negative symbols from default user locale. 
 *			   Suggested By BishBosh02 (www.codeproject.com).
 */
#include "precomp.h"
#include "InPlaceEditNumeric.h"

#ifndef _INC_TCHAR
#	include <tchar.h>
#endif	// !_INC_TCHAR

/**
 *	Removed By Gurmeet S. Kochar (Mar 04, 2004)
 */
//#ifndef VK_OEM_PERIOD
//#	define VK_OEM_PERIOD	0xBE
//#endif	// !VK_OEM_PERIOD

//#ifndef VK_OEM_MINUS
//#	define VK_OEM_MINUS		0xBD
//#endif	// !VK_OEM_MINUS

// initialize static variable(s)
const CInPlaceEditNumeric::_LocaleInfo CInPlaceEditNumeric::DefUserLocale;

BEGIN_MESSAGE_MAP(CInPlaceEditNumeric, CInPlaceEdit)
	ON_MESSAGE(WM_PASTE, OnPaste)
END_MESSAGE_MAP()

/**
 *	GetClipboardText - clipboard text extraction helper
 *	Checks to see if any textual data is available on the clipboard. If 
 *	yes, extracts it, parses only numeric data out of it, and returns it.
 *
 *	Added By Gurmeet S. Kochar (Mar 04, 2004)
 */
#pragma warning(disable: 4018)	// signed/unsigned mismatch
CString CInPlaceEditNumeric::GetClipboardText(void) const
{
	CString	strClipBrdText;

	// clipboard contains textual data?
	if (::IsClipboardFormatAvailable(CF_TEXT))
	{
		// open the clipboard to get clipboard text
		if (::OpenClipboard(m_hWnd))
		{
			HANDLE	hClipBrdData = NULL;
			if ((hClipBrdData = ::GetClipboardData(CF_TEXT)) != NULL)
			{
				LPTSTR	lpClipBrdText = (LPTSTR)::GlobalLock(hClipBrdData);
				if (lpClipBrdText)
				{
					strClipBrdText = lpClipBrdText;
					::GlobalUnlock(hClipBrdData);
				}
			}

			VERIFY(::CloseClipboard());

			/**
			 *	parse a number out of the retrieved text
			 */
			bool	bHasDecimal = false;	// decimal symbol flag
			int		iStart = 0, iCount = 0;
			LPTSTR	lpsz = strClipBrdText.LockBuffer();

			// skip leading whitespaces (including tabs)
			for (; *lpsz == _T(' ') || *lpsz == _T('\t'); iStart++)
				lpsz = ::_tcsinc(lpsz);

			// is the first character a negative symbol?
			if (*lpsz == DefUserLocale.chNegationSymbol)
			{
				// negative values are not allowed?
				if (!m_bAllowNegativeValues)
					return ((LPCTSTR)NULL);

				++iCount;
				lpsz = ::_tcsinc(lpsz);
			}

			while (*lpsz != _T('\0'))
			{
				if (!::_istdigit(*lpsz))
				{
					if ( (m_chDigitsAfterDecimal) && 
						 (*lpsz != DefUserLocale.chDecimalSymbol) )
						break;

					// a decimal symbol is already there?
					if (bHasDecimal)
						break;
					bHasDecimal = true;
				}

				++iCount;
				lpsz = ::_tcsinc(lpsz);
			}

			strClipBrdText.UnlockBuffer();

			if ( (!iStart) && (!iCount) )
				strClipBrdText.Empty();
			else
				strClipBrdText = strClipBrdText.Mid(iStart, iCount);
		}
	}

	return (strClipBrdText);
}

/**
 *	OnPaste - WM_PASTE message handler
 *	Parses a numeric value from the clipboard text, if available, and 
 *	displays it appropriately in the edit control
 *
 *	Added By Gurmeet S. Kochar (Mar 04, 2004)
 */
LRESULT CInPlaceEditNumeric::OnPaste(WPARAM wParam, LPARAM lParam)
{
	UNUSED_ALWAYS(wParam);
	UNUSED_ALWAYS(lParam);

	CString	strClipBrdText,		// text available on clipboard
			strCtrlText;		// text in the edit control
	DWORD	dwSelStart = 0,		// selection range starting position
			dwSelEnd = 0;		// selection range ending position
	int		iDecimalPos[2];		// 0 = in control, 1 = in clipboard text

	strClipBrdText = GetClipboardText();
	// no (valid) clipboard data available?
	if (strClipBrdText.IsEmpty())
	{
		TRACE0("(Warning) CInPlaceEditNumeric::OnPaste:"
				" Unknown clipboard data format or clipboard data unavailable.\n");
		::MessageBeep(MB_ICONEXCLAMATION);
		return (0L);
	}

	// get control's current text and selection range
	GetWindowText(strCtrlText);
	SendMessage(EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);

	// pasting a negative value somewhere in between the current text?
	if ( (strClipBrdText[0] == DefUserLocale.chNegationSymbol) && (dwSelStart) )
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		return (0L);
	}

	if (strCtrlText.GetLength() && 
		strCtrlText[0] == DefUserLocale.chNegationSymbol)
	{
		if (dwSelEnd == dwSelStart)
		{
			::MessageBeep(MB_ICONEXCLAMATION);
			return (0L);
		}
	}

	if (m_chDigitsAfterDecimal)
	{
		iDecimalPos[0] = strCtrlText.Find(DefUserLocale.chDecimalSymbol);
		iDecimalPos[1] = strClipBrdText.Find(DefUserLocale.chDecimalSymbol);
		// both control and clipboard text contain a decimal symbol and 
		// decimal symbol doesn't fall within the current selection range
		if ( (iDecimalPos[0] >= 0 && iDecimalPos[1] >= 0) && 
			 (iDecimalPos[0] < dwSelStart || iDecimalPos[0] > dwSelEnd) )
		{
			// extract only that much data from clipboard text 
			// that comes before the decimal symbol position
			strClipBrdText = strClipBrdText.Left(iDecimalPos[1]);
			
			// set iDecimalPos[1] to -1 to indicate that now 
			// there is no decimal symbol in the clipboard text!
			iDecimalPos[1] = -1;
		}
	}

	strCtrlText = strCtrlText.Left(dwSelStart) + 
					strClipBrdText + 
					strCtrlText.Mid(dwSelEnd);
	dwSelEnd = dwSelStart + strClipBrdText.GetLength();

	// if count of digits after decimal symbol exceeds the 
	// value specified by m_chDigitsAfterDecimal, then, 
	// truncate the rest of the string
	iDecimalPos[0] = strCtrlText.Find(DefUserLocale.chDecimalSymbol);
	if (iDecimalPos[0] >= 0)
	{
		if (strCtrlText.Mid(iDecimalPos[0] + 1).GetLength() > m_chDigitsAfterDecimal)
			strCtrlText = strCtrlText.Left(iDecimalPos[0] + m_chDigitsAfterDecimal + 1);
	}

	SetWindowText(strCtrlText);
	SetSel(dwSelEnd, dwSelEnd);	// position caret, scroll if necessary!

	// Do not call CEdit::Paste from within this function!!!

	return (0L);
}

/**
 *	PreTranslateMessage
 *	Handles WM_KEYDOWN message
 *
 *	Author: Gurmeet S. Kochar
 *	Date: Mar 03, 2004
 */
BOOL CInPlaceEditNumeric::PreTranslateMessage(MSG *pMSG)
{
	CString	strBuffer;			// edit control's text buffer
	DWORD	dwSelStart = 0,		// selection starting position
			dwSelEnd = 0;		// selection ending position
	int		iPos = 0;			// to hold the return value of CString::Find
	UINT	nKeyCode = pMSG->wParam;	// virtual key code of the key pressed

	// not a WM_KEYDOWN message?
	if (pMSG->message != WM_KEYDOWN)
		// forward for default processing!
		goto LForwardMsg;

	// CTRL+C, CTRL+X, or, CTRL+V?
	if ( (nKeyCode == _T('C') || nKeyCode == _T('X') || nKeyCode == _T('V')) && 
		 (::GetKeyState(VK_CONTROL) & 0x8000) )
		goto LForwardMsg;

	//TRACE1("(Info) CInPlaceEditNumeric::PreTranslateMessage:"
	//		" Virtual key code of the key pressed = 0x%04x\n", nKeyCode);

	switch (nKeyCode)
	{

		// subtract key?
		case VK_SUBTRACT:
		case VK_OEM_MINUS:	// Removed By Gurmeet S. Kochar (Mar 04, 2004)
		{
			// get into this only if negative values are allowed!
			if (m_bAllowNegativeValues)
			{
				GetWindowText(strBuffer);
				if (strBuffer.IsEmpty())
					break;

				// retrieve selection range
				SendMessage(EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);

				// is the selection beginning from the very first character?
				if (!dwSelStart)
				{
					// NOTE: if text buffer starts from a decimal symbol, 
					//		 then we cannot accept a negative symbol until the 
					//		 decimal points symbol falls within the current 
					//		 selection range

					/**
					 *	Removed By Gurmeet S. Kochar (Mar 04, 2004)
					 */
					// if (strBuffer[0] == _T('.'))

					if (strBuffer[0] == DefUserLocale.chDecimalSymbol)
					{
						// decimal symbol falls within the current selection range?
						if (dwSelEnd != dwSelStart)
							// accept the negative symbol as it is going to 
							// replace the currently selected text
							break;

						::MessageBeep(MB_ICONEXCLAMATION);
						return (TRUE);
					}

					// check to make sure the first character is not 
					// already a negative sign
					if (strBuffer[0] != DefUserLocale.chNegationSymbol)
						break;
					
					// the first character is a negative sign but 
					// is it within the current selection range?
					if (dwSelEnd != dwSelStart)
						break;
				}
			}

			::MessageBeep(MB_ICONEXCLAMATION);
			return (TRUE);
		}

	// digits on the main section of keyboard?
	case 0x30:	case 0x31:	case 0x32:	case 0x33:	case 0x34:
	case 0x35:	case 0x36:	case 0x37:	case 0x38:	case 0x39:
	// digits on the numeric keypad?
	case 0x60:	case 0x61:	case 0x62:	case 0x63:	case 0x64:
	case 0x65:	case 0x66:	case 0x67:	case 0x68:	case 0x69:
	// decimal symbol?
	case VK_DECIMAL:
	case VK_OEM_PERIOD:	// Removed By Gurmeet S. Kochar (Mar 04, 2004)
		{
			GetWindowText(strBuffer);
			if (strBuffer.IsEmpty())
				break;

			/**
			 *	Removed By Gurmeet S. Kochar (Mar 04, 2004)
			 */
			iPos = strBuffer.Find(_T('.'));

			// get the current selection range
			SendMessage(EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);
			
			//TRACE2("(Info) CInPlaceEditNumeric::PreTranslateMessage:"
			//		" SelStart = %ld, SelEnd = %ld\n", dwSelStart, dwSelEnd);

			// decimal symbol?
			if (nKeyCode == VK_DECIMAL /*|| nKeyCode == VK_OEM_PERIOD*/)
			{
				ASSERT(m_chDigitsAfterDecimal >= 0 && m_chDigitsAfterDecimal <= 9);

				// accept integer values only?
				if (!m_chDigitsAfterDecimal)
				{
					::MessageBeep(MB_ICONEXCLAMATION);
					return (TRUE);
				}

				// a decimal symbol is already there?
				if (iPos >= 0)
				{
					if ( (dwSelEnd == dwSelStart) && 
						 (iPos < dwSelStart || iPos > dwSelEnd) )
					{
						// there can be only one decimal symbol in a numeric value
						::MessageBeep(MB_ICONEXCLAMATION);
						return (TRUE);
					}
				}
			}
			
			// one of the digit keys has been pressed?
			else
			{
				// iPos >= 0 : value contains a decimal symbol?
				// dwSelStart > iPos : user is typing text after the decimal symbol?
				// dwSelStart == dwSelEnd : the selection is empty?
				// strBuffer.Mid... : number of digits after decimal symbol is 
				//					  going to exceed than the value specified  
				//					  by m_chDigitsAfterDecimal?
				if ( (iPos >= 0) && 
					 (dwSelStart > iPos) && 
					 (dwSelStart == dwSelEnd) &&
					 (strBuffer.Mid(iPos+1).GetLength() >= m_chDigitsAfterDecimal) )
				{
					::MessageBeep(MB_ICONEXCLAMATION);
					return (TRUE);
				}
			}

			// neither a digit nor a decimal symbol is allowed 
			// at the starting of the text when the value in the 
			// text buffer is a negative value
			
			/**
			 *	Removed By Gurmeet S. Kochar (Mar 04, 2004)
			 */
			//if ( (!dwSelStart) && (strBuffer[0] == _T('-')) )

			if ( (!dwSelStart) && (strBuffer[0] == DefUserLocale.chNegationSymbol) )
			{
				// negative sign is within the current selection range?
				if (dwSelEnd == dwSelStart)
				{
					::MessageBeep(MB_ICONEXCLAMATION);
					return (TRUE);
				}
			}

			break;
		}

	default:
		{
			if ((nKeyCode >= 0x01 && nKeyCode <= 0x40) || 
				(nKeyCode >= 0x5B && nKeyCode <= 0x69) || 
				(nKeyCode >= 0x70 && nKeyCode <= 0xB9) || 
				(nKeyCode >= 0xE5 && nKeyCode <= 0xFE))
			{
				// spaces are not allowed in numeric values!
				if (nKeyCode == VK_SPACE)
				{
					::MessageBeep(MB_ICONEXCLAMATION);
					return (TRUE);
				}

				break;
			}

			return (TRUE);	// eat message; ignore the key!!!
		}
	}

LForwardMsg:
	return (CInPlaceEdit::PreTranslateMessage(pMSG));
}
#pragma warning(default: 4018)
