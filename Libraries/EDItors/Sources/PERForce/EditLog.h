/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
\ editlog.h,  Fast asynchronous text logging using a Windows Edit-Control
/
\ Version:	1.0, created 2000-04-02
/           1.1, 2000-07-11: Now *really* supports Win9x, uses V2.0 of SubclassWnd
\
/ Author:	Daniel Lohmann (daniel@uni-koblenz.de)
\
/ Please send comments and bugfixes to the above email address.
\
/ This code is provided AS IS, you use it at your own risk! 
\ You may use it for whatever you want.
/
\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#ifndef __EDITLOG_H__
#define __EDITLOG_H__

// We use the CSubclassWnd class, created by William E. Kempf (sirwillard@my-Deja.com)
// You can find the complete article at http://www.codeproject.com/miscctrl/subclasswnd.asp
#include <string>
#include "SubclassWnd.h"

///////////////////////////////////
// CEditLog class 
// Implemets a fast, asynchronous and threadsave text logging for Edit-Controls
//
//

class CEditLog : protected CSubclassWnd
{
public:
	typedef CSubclassWnd root_type;

	// Constructs the CEditLog. You can pass the edit controls handle
	// here or set it later using the SetEditCtrl() function.
	// To increase performance CEditLog repaints the edit-control
	// only if the caret resides in the last line. Otherwise the
	// control will be refreshed only every nMaxRefreshDelay msec.
	CEditLog( HWND hEdit = NULL, UINT nMaxRefreshDelay = 500 );
	
	virtual ~CEditLog();

	
	// Adds some text to the end of the edit control. Works asynchronously
	// (using PostMessage()) and is save to be called by multiple threads.
	// If you pass true for bLFtoCRLF every LF (ASCII 10) (as it is used in
	// most cases for end-of-line) will be converted to a CR/LF (ASCII 10/13)
	// sequence as it is needed by the windows edit control. 
	virtual void AddText( LPCWSTR pwszAdd, bool bLFtoCRLF = false );
	
	// Converts pszAdd to UNICODE and calls the above
	void AddText( LPCSTR pszAdd, bool bLFtoCRLF = false );

	// Sets the edit-control to be used for logging.
	// Pass NULL to stop text logging.
	virtual void SetEditCtrl( HWND hEdit );

	HWND GetEditCtrl() const
	{
		return GetHandle();
	}
protected:

	virtual BOOL ProcessWindowMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult );

	void OnDestroy();
	void OnTimer(UINT id);

	bool OnAddText( bool bForce = false);
	
	// This members are used by the GUI thread only
	UINT			m_nTimerID;
	UINT			m_nTimerDelay;


	// This members are used by the GUI thread and 
	// every thread calling AddText, so we protect them with
	// a Critical Section.
	CRITICAL_SECTION	m_csLock;
	bool				m_bMessagePending;
	std::wstring		m_wsStore;
};


#endif // __EDITLOG_H__