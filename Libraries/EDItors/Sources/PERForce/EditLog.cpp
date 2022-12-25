/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
\ editlog.cpp,  Fast asynchronous text logging using a Windows Edit-Control
/
\ Version:	1.0, created 2000-04-02
/           1.1, 2000-07-11: Now *really* supports Win9x, uses V2.0 of SubclassWnd
\			1.2  2003-08-20: Fixed some problems with VC.NET 2003
/
\
/ Author:	Daniel Lohmann (daniel@losoft.de)
\
/ Please send comments and bugfixes to the above email address.
\
/ This code is provided AS IS, you use it at your own risk! 
\ You may use it for whatever you want.
/
\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#include "precomp.h"
#include "EditLog.h"
#include <malloc.h>		// for the _alloca() function

const long WM_ADDTEXT	= WM_USER + 500;

// Converts a UNICODE string to ANSI using the stack for buffering
#define _W2A(lpw) ( (lpw == NULL ) ? NULL : _W2AHelp(lpw, (LPSTR) _alloca( wcslen( lpw ) + 1 ) ) )
inline LPSTR _W2AHelp(LPCWSTR pwsz, LPSTR psz ) {
	int n = wcslen( pwsz ) + 1;
	WideCharToMultiByte( CP_ACP, 0, pwsz, n, psz, n, NULL, NULL );
	return psz;
}

// true, if we are running on a NT/2k System
bool g_bIsWinNT = ( GetVersion() < 0x80000000 ); 


CEditLog::CEditLog( 
		HWND hEdit,
		UINT nTimerDelay
		)
{
	m_bMessagePending = false;
	m_nTimerID		= 0;
	m_nTimerDelay	= nTimerDelay;
	::InitializeCriticalSection( &m_csLock );
	SetEditCtrl( hEdit );
}

CEditLog::~CEditLog(
		)
{
	UnsubclassWindow();
	::DeleteCriticalSection( &m_csLock );
}


void CEditLog::AddText( 
		LPCWSTR pwszAdd,
		bool bLFtoCRLF	// = false
		)
{
	if( pwszAdd ) {
		::EnterCriticalSection( &m_csLock );
		if( bLFtoCRLF ) {
			// Add text to our buffer, but convert each LF to a CRLF pair
			int cchAdd = ::wcslen( pwszAdd );
			// Ensure no buffer enlargement operations are necessary 			
			m_wsStore.reserve( m_wsStore.length() + cchAdd * 2 );
			for( int i = 0; i < cchAdd; ++i ) {
				if( pwszAdd[ i ] == L'\n' )
					m_wsStore += L'\r';
				m_wsStore += pwszAdd[ i ];
			}
		}
		else
			// Simly add text
			m_wsStore += pwszAdd;

		// If the log is filled really fast or the UI thread is doing something
		// else, the WM_ADDTEXT messages may not get processed for a long period.
		// To avoid floating the UI threads message queue, we post only one message.
		if( !m_bMessagePending && GetHandle() != NULL ) {
			::PostMessage( GetHandle(), WM_ADDTEXT, 0, 0 );
			m_bMessagePending = true;
		}
		::LeaveCriticalSection( &m_csLock );
	}
}

void CEditLog::AddText( 
		LPCSTR pszAdd, 
		bool bLFtoCRLF	// = false
		)
{
	if( pszAdd ) {
		int n = ::strlen( pszAdd ) + 1;

		if( n <= 1024 ) {
			// Allocate conversion buffer for 1k max on the stack (very fast!)
			LPWSTR pwsz = (LPWSTR) ::_alloca( n * 2 );
			::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pszAdd, n, pwsz, n );
			AddText( pwsz, bLFtoCRLF );
		}
		else {
			// Allocate conversion buffer on the heap
			LPWSTR pwsz = new WCHAR[ n ];
			::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pszAdd, n, pwsz, n );
			AddText( pwsz, bLFtoCRLF );
			delete pwsz;
		}
	}
}

void CEditLog::SetEditCtrl(
		HWND hEdit
		)
{
	if( hEdit != GetHandle() && GetHandle() != NULL )
		UnsubclassWindow();
	
	if( SubclassWindow( hEdit ) ) {
		// We want to support a lot of text...
		// Note that Win9x supports no more than 64kb of text! - just too bad...
		// Tip: Use NT/2k :-)
		::SendMessage( hEdit, EM_LIMITTEXT, 0, 0 );
	}
}

BOOL CEditLog::ProcessWindowMessage(
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		LRESULT& lResult 
		)
{
	lResult = 0;
	switch( uMsg ) {
		case WM_DESTROY:
			OnDestroy();
			return TRUE;
		case WM_TIMER:
			OnTimer( (UINT) wParam );
			return TRUE;
		case WM_ADDTEXT:
			OnAddText(true);
//			if( !OnAddText() && m_nTimerID == 0 )
//				m_nTimerID = ::SetTimer( GetHandle(), 4711, m_nTimerDelay, NULL );
			return TRUE;
	}

	return root_type::ProcessWindowMessage( uMsg, wParam, lParam, lResult );
}


void CEditLog::OnDestroy(
		)
{
	UnsubclassWindow();
}

void CEditLog::OnTimer(
		UINT id
		)
{
	if( id == m_nTimerID && OnAddText( true ) ) {
		::KillTimer( GetHandle(), m_nTimerID );
		m_nTimerID = 0;
	}
		
}

bool CEditLog::OnAddText(
		bool bForce
		)
{
	bool		 bAdded = false;
	std::wstring wsText;
	int			 cchText;

	HWND hEdit = GetHandle();

	// Retrieve the text from our buffer
	// We store it in a local variable to prevent long time locking
	::EnterCriticalSection( &m_csLock );
	wsText = m_wsStore;
	m_bMessagePending = false; 
	::LeaveCriticalSection( &m_csLock );

	cchText = wsText.length();


	// Get Edit contents dimensions and cursor position
	int nStartChar, nEndChar, nSize;
	::SendMessage( hEdit, EM_GETSEL, (WPARAM) &nStartChar, (LPARAM) &nEndChar );
	nSize = (int) ::SendMessage( hEdit, WM_GETTEXTLENGTH, 0, 0 );


	if( nStartChar == nSize && nSize == nEndChar  ) {
		// The cursor resides at the end of text
	
		SCROLLINFO si;
		si.cbSize	= sizeof si;
		si.fMask	= SIF_ALL;
		::GetScrollInfo( GetHandle(), SB_VERT, &si );
		if( si.nPos >= si.nMax - (int) si.nPage + 1  ) {
			// Not scrolled away
			::SendMessage( hEdit, EM_SETSEL, (WPARAM) nSize, (LPARAM) nSize );
			if( g_bIsWinNT )
				::SendMessageW( hEdit, EM_REPLACESEL, 0, (LPARAM) wsText.c_str() );
			else 
				::SendMessage( hEdit, EM_REPLACESEL, 0, (LPARAM) _W2A( wsText.c_str() ) );
			bAdded = true;
		}
		else if( bForce ) {
			int nFirstLine, nFirstLineNow;

			// Reduce flicker by ignoring WM_PAINT
			::SendMessage( hEdit, WM_SETREDRAW, false, 0 );

			// Remember where we are
			nFirstLine = (int)::SendMessage( hEdit, EM_GETFIRSTVISIBLELINE, 0, 0 );
		
			// Select at the end of text and replace the selection
			// This is a very fast way to add text to an edit control
			::SendMessage( hEdit, EM_SETSEL, (WPARAM) nSize, (LPARAM) nSize );
			if( g_bIsWinNT )
				::SendMessageW( hEdit, EM_REPLACESEL, 0, (LPARAM) wsText.c_str() );
			else 
				::SendMessage( hEdit, EM_REPLACESEL, 0, (LPARAM) _W2A( wsText.c_str() ) );

			// Restore our previous selection
			::SendMessage( hEdit, EM_SETSEL, (WPARAM) nStartChar, (LPARAM) nEndChar );
			nFirstLineNow = (int)::SendMessage( hEdit, EM_GETFIRSTVISIBLELINE, 0, 0 );
			::SendMessage( hEdit, EM_LINESCROLL, 0, (LPARAM) nFirstLine - nFirstLineNow);

			::SendMessage( hEdit, WM_SETREDRAW, true, 0 );
			bAdded = true;
		}
	}
	else if( bForce ) {
		// We should add the text anyway...

		POINT pt;
		int nTotaLineBefore, nTotalLineAfter, nCaretPos;

		// Reduce flicker by ignoring WM_PAINT
		//::SendMessage( hEdit, WM_SETREDRAW, false, 0 );

		nTotaLineBefore = (int)::SendMessage( hEdit, EM_GETLINECOUNT, 0, 0 );
	
		if( nStartChar != nEndChar ) {
			// If we are currently selecting some text, we have to find out
			// if the caret is near the beginning of this block or near the end.
			// Note that this does not always work. Because of the EM_CHARFROMPOS
			// message returning only 16 bits this will fail if the user has selected 
			// a block with a length dividable by 64k.
			
			::GetCaretPos( &pt );
			nCaretPos = (int) LOWORD(::SendMessage( hEdit, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y) ) );
			if( abs( (nStartChar % 0xffff - nCaretPos) ) < abs( (nEndChar % 0xffff - nCaretPos) ) ) {
				nCaretPos	= nStartChar;
				nStartChar	= nEndChar;
				nEndChar	= nCaretPos;
			}
		}

		// Note: This will flicker, if someone has a good idea how to prevent this - let me know
		
		// Select at the end of text and replace the selection
		// This is a very fast way to add text to an edit control
		::SendMessage( hEdit, EM_SETSEL, (WPARAM) nSize, (LPARAM) nSize );
		if( g_bIsWinNT )
			::SendMessageW( hEdit, EM_REPLACESEL, 0, (LPARAM) wsText.c_str() );
		else 
			::SendMessage( hEdit, EM_REPLACESEL, 0, (LPARAM) _W2A( wsText.c_str() ) );

		// Restore our previous selection
		::SendMessage( hEdit, EM_SETSEL, (WPARAM) nStartChar, (LPARAM) nEndChar );
		nTotalLineAfter = (int)::SendMessage( hEdit, EM_GETLINECOUNT, 0, 0 );
		
		::SendMessage( hEdit, EM_LINESCROLL, 0, (LPARAM) nTotalLineAfter - nTotaLineBefore);

		//::SendMessage( hEdit, WM_SETREDRAW, true, 0 );
		bAdded = true;
	}

	if( bAdded ) {
		// Delete the processed text from our buffer
		::EnterCriticalSection( &m_csLock );
		m_wsStore.erase( 0, cchText );
		::LeaveCriticalSection( &m_csLock );
	}
	return bAdded;
}




