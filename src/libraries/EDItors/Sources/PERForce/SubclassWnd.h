/** @file SubclassWnd.h
 *
 * Definition of CSubclassWnd.h.
 *
 * @author William E. Kempf
 * @version 2.0
 *
 * Copyright &copy 2000 NEWare Software.
 *
 * This code may be used in compiled form in any way you desire (including
 * commercial use). The code may be redistributed unmodified by any means providing
 * it is not sold for profit without the authors written consent, and providing that
 * this notice and the authors name and all copyright notices remains intact. However,
 * this file and the accompanying source code may not be hosted on a website or bulletin
 * board without the authors written permission.
 *
 * <b><i>This software is provided "as is" without express or implied warranty. Use it
 * at your own risk!</i></b>
 */

#if !defined(_SUBCLASSWND_H_199B246E_AE5D_11D3_A459_000629B2F85_INCLUDED_)
#define _SUBCLASSWND_H_199B246E_AE5D_11D3_A459_000629B2F85_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/**
 * Pluggable window message handler.
 *
 * This class can be used to "subclass" a window in the C API sense of the term.
 * It is a base class, meant to be derived from.  Derived classes should provide
 * an interface that will call CSubclassWnd::SubclassWindow in order to hook into
 * a window's message handling interface.  Typically this will be done in the
 * derived's constructor and the OnFinalMessage method will be overriden to delete
 * the object's instance (delete this).
 */

class CSubclassWnd
{
private:
	class WndProcThunk
	{
	public:
#if defined(_M_IX86)
#pragma pack(push,1)
		struct Thunk
		{
			DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
			DWORD   m_this;         //
			BYTE    m_jmp;          // jmp WndProc
			DWORD   m_relproc;      // relative jmp
		};
#pragma pack(pop)
#elif defined (_M_ALPHA)
	// For ALPHA we will stick the this pointer into a0, which is where
	// the HWND is.  However, we don't actually need the HWND so this is OK.
#pragma pack(push,4)
		struct Thunk //this should come out to 20 bytes
		{
			DWORD ldah_at;      //  ldah    at, HIWORD(func)
			DWORD ldah_a0;      //  ldah    a0, HIWORD(this)
			DWORD lda_at;       //  lda     at, LOWORD(func)(at)
			DWORD lda_a0;       //  lda     a0, LOWORD(this)(a0)
			DWORD jmp;          //  jmp     zero,(at),0
		};
#pragma pack(pop)
#else
#error Only Alpha and X86 supported
#endif

		Thunk thunk;

		void Init(WNDPROC proc, void* pThis)
		{
#if defined (_M_IX86)
			thunk.m_mov = 0x042444C7;  //C7 44 24 0C
			thunk.m_this = (DWORD)pThis;
			thunk.m_jmp = 0xe9;
			thunk.m_relproc = (int)proc - ((int)this+sizeof(Thunk));
#elif defined (_M_ALPHA)
			thunk.ldah_at = (0x279f0000 | HIWORD(proc)) + (LOWORD(proc)>>15);
			thunk.ldah_a0 = (0x261f0000 | HIWORD(pThis)) + (LOWORD(pThis)>>15);
			thunk.lda_at = 0x239c0000 | LOWORD(proc);
			thunk.lda_a0 = 0x22100000 | LOWORD(pThis);
			thunk.jmp = 0x6bfc0000;
#endif
			// write block from data cache and
			//  flush from instruction cache
			FlushInstructionCache(GetCurrentProcess(), &thunk, sizeof(thunk));
		}
	};

protected:
	CSubclassWnd();
	virtual ~CSubclassWnd();

	virtual BOOL ProcessWindowMessage(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT& lResult);
	virtual void OnFinalMessage();

	LRESULT DefWindowProc();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

#ifdef _MFC_VER
	BOOL SubclassWindow(CWnd* pWnd, BOOL bReflect=TRUE);
#endif
	BOOL SubclassWindow(HWND hWnd, BOOL bReflect=TRUE);
	HWND UnsubclassWindow();

public:
	const MSG* GetCurrentMessage() const;

	HWND GetHandle() const;

	LRESULT SendMessage(UINT message, WPARAM wParam=0, LPARAM lParam=0);
	BOOL PostMessage(UINT message, WPARAM wParam=0, LPARAM lParam=0);
	void SendMessageToDescendants(UINT message, WPARAM wParam, LPARAM lParam,
		BOOL bDeep=TRUE);

private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);

private:
	WndProcThunk m_thunk;
	WNDPROC m_pfnSuperWindowProc;
	const MSG* m_pCurrentMsg;
	HWND m_hWnd;
};

#ifdef	_MFC_VER
/**
 * Called to subclass the specified window.  If the @a bReflect
 * parameter is TRUE then the parent window will be subclassed to
 * support message reflection.
 *
 * @param pWnd [in] Points to the CWnd to be subclassed.
 * @param bReflect [in] Specifies whether or not the parent should
 *		reflect messages back to the window.
 *
 * @return TRUE if successful; otherwise FALSE.
 */

inline BOOL CSubclassWnd::SubclassWindow(CWnd* pWnd, BOOL bReflect)
{
	return SubclassWindow(pWnd->GetSafeHwnd(), bReflect);
}
#endif

/**
 * Retrieves a pointer to the "current" MSG structure being handled.
 *
 * @return Returns a pointer to the "current" MSG structure being handled.
 */
inline const MSG* CSubclassWnd::GetCurrentMessage() const
{
	return m_pCurrentMsg;
}

/**
 * Calls the "default" window procedure with the current message.  The
 * "default" procedure is what ever procedure was subclassed by this
 * class.
 *
 * @return The return value is the result of the message processing and
 *		depends on the message.
 */

inline LRESULT CSubclassWnd::DefWindowProc()
{
	const MSG* pMsg = m_pCurrentMsg;
	LRESULT lRes = 0;
	if (pMsg != NULL)
		lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
	return lRes;
}

/**
 * Calls the "default" window procedure with the specified message.  The
 * "default" procedure is what ever procedure was subclassed by this
 * class.
 *
 * @param message [in] Specifies the message id.
 * @param wParam [in] Specifies additional message-specific information.
 * @param lParam [in] Specifies additional message-specific information.
 *
 * @return The return value is the result of the message processing and
 *		depends on the message.
 */
inline LRESULT CSubclassWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef STRICT
	return ::CallWindowProc(m_pfnSuperWindowProc, m_hWnd, message, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)m_pfnSuperWindowProc, m_hWnd, message, wParam, lParam);
#endif
}

/**
 * Called to get the handle of the subclassed window.
 *
 * @return The handle to the currently subclassed window.
 */

inline HWND CSubclassWnd::GetHandle() const
{
	return m_hWnd;
}

/**
 * Sends a message to the subclassed window.
 *
 * @param message [in] Specifies the message id.
 * @param wParam [in] Specifies additional message-specific information.
 * @param lParam [in] Specifies additional message-specific information.
 *
 * @return The return value is the result of the message processing and
 *		depends on the message.
 */

inline LRESULT CSubclassWnd::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(GetHandle(), message, wParam, lParam);
}

/**
 * Posts a message to the subclassed window.
 *
 * @param message [in] Specifies the message id.
 * @param wParam [in] Specifies additional message-specific information.
 * @param lParam [in] Specifies additional message-specific information.
 *
 * @return If the function succeeds, the return value is nonzero. If the
 *		function fails, the return value is zero. To get extended error
 *		information, call GetLastError. 
 */

inline BOOL CSubclassWnd::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::PostMessage(GetHandle(), message, wParam, lParam);
}

template <typename T>
struct DISPATCH_TRAITS
{
	typedef T type;
};

/** @def BEGIN_MSG_DISPATCH(className, rootClass)
 *
 * Begins the message dispatch map.
 */
#define	BEGIN_MSG_DISPATCH(className, rootClass)	\
	virtual BOOL ProcessWindowMessage(UINT message, WPARAM wParam, \
		LPARAM lParam, LRESULT& lResult) {	\
		typedef DISPATCH_TRAITS<##rootClass>::type root_type;	\
		if (FALSE) ;
/**
 * Ends the message dispatch map.
 */
#define	END_MSG_DISPATCH()	\
		return root_type::ProcessWindowMessage(message, wParam, lParam, lResult); }

/**
 * Dispatches a message to a member function from within the message map.  If you
 * want to dispatch a user define message you must declare a dispatch macro
 * for it.  See the header file <b>SubclassWnd.h</b> for examples of how to declare
 * such macros.
 */
#define DISPATCH_MSG(msg, mfn)    \
	else if (message == msg)  { lResult = DISPATCH_##msg((wParam), (lParam), (mfn)); return TRUE; }

/* void Class::OnCompacting(UINT compactRatio) */
#define DISPATCH_WM_COMPACTING(wParam, lParam, fn) \
    ((fn)((UINT)(wParam)), 0L)

/* void Class::OnWinIniChange(LPCTSTR lpszSectionName) */
#define DISPATCH_WM_WININICHANGE(wParam, lParam, fn) \
    ((fn)((LPCTSTR)(lParam)), 0L)

/* void Class::OnSysColorChange() */
#define DISPATCH_WM_SYSCOLORCHANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Class::OnQueryNewPalette() */
#define DISPATCH_WM_QUERYNEWPALETTE(wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(), 0L)

/* void Class::OnPaletteIsChanging(HWND hwndPaletteChange) */
#define DISPATCH_WM_PALETTEISCHANGING(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Class::OnPaletteChanged(HWND hwndPaletteChange) */
#define DISPATCH_WM_PALETTECHANGED(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Class::OnFontChange() */
#define DISPATCH_WM_FONTCHANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnSpoolerStatus(UINT status, int cJobInQueue) */
#define DISPATCH_WM_SPOOLERSTATUS(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Class::OnDevModeChange(LPCTSTR lpszDeviceName) */
#define DISPATCH_WM_DEVMODECHANGE(wParam, lParam, fn) \
    ((fn)((LPCTSTR)(lParam)), 0L)

/* void Class::OnTimeChange() */
#define DISPATCH_WM_TIMECHANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnPower(int code) */
#define DISPATCH_WM_POWER(wParam, lParam, fn) \
    ((fn)((int)(wParam)), 0L)

/* BOOL Class::OnQueryEndSession() */
#define DISPATCH_WM_QUERYENDSESSION(wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(), 0L)

/* void Class::OnEndSession(BOOL fEnding) */
#define DISPATCH_WM_ENDSESSION(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam)), 0L)

/* void Class::OnQuit(int exitCode) */
#define DISPATCH_WM_QUIT(wParam, lParam, fn) \
    ((fn)((int)(wParam)), 0L)

/* This message is in Windows 3.1 only */
/* void Class::OnSystemError(int errCode) */
#define DISPATCH_WM_SYSTEMERROR(wParam, lParam, fn) 0L

/* BOOL Class::OnCreate(LPCREATESTRUCT lpCreateStruct) */
#define DISPATCH_WM_CREATE(wParam, lParam, fn) \
    ((fn)((LPCREATESTRUCT)(lParam)) ? 0L : (LRESULT)-1L)

/* BOOL Class::OnNCCreate(LPCREATESTRUCT lpCreateStruct) */
#define DISPATCH_WM_NCCREATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((LPCREATESTRUCT)(lParam))

/* void Class::OnDestroy() */
#define DISPATCH_WM_DESTROY(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnNCDestroy() */
#define DISPATCH_WM_NCDESTROY(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnShowWindow(BOOL fShow, UINT status) */
#define DISPATCH_WM_SHOWWINDOW(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam), (UINT)(lParam)), 0L)

/* void Class::OnSetRedraw(BOOL fRedraw) */
#define DISPATCH_WM_SETREDRAW(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam)), 0L)

/* void Class::OnEnable(BOOL fEnable) */
#define DISPATCH_WM_ENABLE(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam)), 0L)

/* void Class::OnSetText(LPCTSTR lpszText) */
#define DISPATCH_WM_SETTEXT(wParam, lParam, fn) \
    ((fn)((LPCTSTR)(lParam)), 0L)

/* int Class::OnGetText(int cchTextMax, LPTSTR lpszText) */
#define DISPATCH_WM_GETTEXT(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((int)(wParam), (LPTSTR)(lParam))

/* int Class::OnGetTextLength() */
#define DISPATCH_WM_GETTEXTLENGTH(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)()

/* BOOL Class::OnWindowPosChanging(LPWINDOWPOS lpwpos) */
#define DISPATCH_WM_WINDOWPOSCHANGING(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((LPWINDOWPOS)(lParam))

/* void Class::OnWindowPosChanged(const LPWINDOWPOS lpwpos) */
#define DISPATCH_WM_WINDOWPOSCHANGED(wParam, lParam, fn) \
    ((fn)((const LPWINDOWPOS)(lParam)), 0L)

/* void Class::OnMove(int x, int y) */
#define DISPATCH_WM_MOVE(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Class::OnSize(UINT state, int cx, int cy) */
#define DISPATCH_WM_SIZE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Class::OnClose() */
#define DISPATCH_WM_CLOSE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Class::OnQueryOpen() */
#define DISPATCH_WM_QUERYOPEN(wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(), 0L)

/* void Class::OnGetMinMaxInfo(LPMINMAXINFO lpMinMaxInfo) */
#define DISPATCH_WM_GETMINMAXINFO(wParam, lParam, fn) \
    ((fn)((LPMINMAXINFO)(lParam)), 0L)

/* void Class::OnPaint() */
#define DISPATCH_WM_PAINT(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Class::OnEraseBkgnd(HDC hdc) */
#define DISPATCH_WM_ERASEBKGND(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((HDC)(wParam))

/* BOOL Class::OnIconEraseBkgnd(HDC hdc) */
#define DISPATCH_WM_ICONERASEBKGND(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((HDC)(wParam))

/* void Class::OnNCPaint(HRGN hrgn) */
#define DISPATCH_WM_NCPAINT(wParam, lParam, fn) \
    ((fn)((HRGN)(wParam)), 0L)

/* UINT Class::OnNCCalcSize(BOOL fCalcValidRects, NCCALCSIZE_PARAMS * lpcsp) */
#define DISPATCH_WM_NCCALCSIZE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((BOOL)(0), (NCCALCSIZE_PARAMS *)(lParam))

/* UINT Class::OnNCHitTest(int x, int y) */
#define DISPATCH_WM_NCHITTEST(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))

/* HICON Class::OnQueryDragIcon() */
#define DISPATCH_WM_QUERYDRAGICON(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)()

#ifdef _INC_SHELLAPI
/* void Class::OnDropFiles(HDROP hdrop) */
#define DISPATCH_WM_DROPFILES(wParam, lParam, fn) \
    ((fn)((HDROP)(wParam)), 0L)
#endif  /* _INC_SHELLAPI */

/* void Class::OnActivate(UINT state, HWND hwndActDeact, BOOL fMinimized) */
#define DISPATCH_WM_ACTIVATE(wParam, lParam, fn) \
    ((fn)((UINT)LOWORD(wParam), (HWND)(lParam), (BOOL)HIWORD(wParam)), 0L)

/* void Class::OnActivateApp(BOOL fActivate, DWORD dwThreadId) */
#define DISPATCH_WM_ACTIVATEAPP(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam), (DWORD)(lParam)), 0L)

/* BOOL Class::OnNCActivate(BOOL fActive, HWND hwndActDeact, BOOL fMinimized) */
#define DISPATCH_WM_NCACTIVATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((BOOL)(wParam), 0L, 0L)

/* void Class::OnSetFocus(HWND hwndOldFocus) */
#define DISPATCH_WM_SETFOCUS(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Class::OnKillFocus(HWND hwndNewFocus) */
#define DISPATCH_WM_KILLFOCUS(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Class::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define DISPATCH_WM_KEYDOWN(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), TRUE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Class::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define DISPATCH_WM_KEYUP(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), FALSE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Class::OnChar(TCHAR ch, int cRepeat) */
#define DISPATCH_WM_CHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Class::OnDeadChar(TCHAR ch, int cRepeat) */
#define DISPATCH_WM_DEADCHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Class::OnSysKey(UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define DISPATCH_WM_SYSKEYDOWN(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), TRUE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Class::OnSysKey(UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define DISPATCH_WM_SYSKEYUP(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), FALSE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Class::OnSysChar(TCHAR ch, int cRepeat) */
#define DISPATCH_WM_SYSCHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Class::OnSysDeadChar(TCHAR ch, int cRepeat) */
#define DISPATCH_WM_SYSDEADCHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Class::OnMouseMove(int x, int y, UINT keyFlags) */
#define DISPATCH_WM_MOUSEMOVE(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define DISPATCH_WM_LBUTTONDOWN(wParam, lParam, fn) \
    ((fn)(FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define DISPATCH_WM_LBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)(TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnLButtonUp(int x, int y, UINT keyFlags) */
#define DISPATCH_WM_LBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define DISPATCH_WM_RBUTTONDOWN(wParam, lParam, fn) \
    ((fn)(FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define DISPATCH_WM_RBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)(TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnRButtonUp(int x, int y, UINT flags) */
#define DISPATCH_WM_RBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnMButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define DISPATCH_WM_MBUTTONDOWN(wParam, lParam, fn) \
    ((fn)(FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnMButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define DISPATCH_WM_MBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)(TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnMButtonUp(int x, int y, UINT flags) */
#define DISPATCH_WM_MBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCMouseMove(int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCMOUSEMOVE(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCLButtonDown(BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCLBUTTONDOWN(wParam, lParam, fn) \
    ((fn)(FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCLButtonDown(BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCLBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)(TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCLButtonUp(int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCLBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCRButtonDown(BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCRBUTTONDOWN(wParam, lParam, fn) \
    ((fn)(FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCRButtonDown(BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCRBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)(TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCRButtonUp(int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCRBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCMButtonDown(BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCMBUTTONDOWN(wParam, lParam, fn) \
    ((fn)(FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCMButtonDown(BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCMBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)(TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Class::OnNCMButtonUp(int x, int y, UINT codeHitTest) */
#define DISPATCH_WM_NCMBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* int Class::OnMouseActivate(HWND hwndTopLevel, UINT codeHitTest, UINT msg) */
#define DISPATCH_WM_MOUSEACTIVATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((HWND)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam))

/* void Class::OnCancelMode() */
#define DISPATCH_WM_CANCELMODE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnTimer(UINT id) */
#define DISPATCH_WM_TIMER(wParam, lParam, fn) \
    ((fn)((UINT)(wParam)), 0L)

/* void Class::OnInitMenu(HMENU hMenu) */
#define DISPATCH_WM_INITMENU(wParam, lParam, fn) \
    ((fn)((HMENU)(wParam)), 0L)

/* void Class::OnInitMenuPopup(HMENU hMenu, UINT item, BOOL fSystemMenu) */
#define DISPATCH_WM_INITMENUPOPUP(wParam, lParam, fn) \
    ((fn)((HMENU)(wParam), (UINT)LOWORD(lParam), (BOOL)HIWORD(lParam)), 0L)

/* void Class::OnMenuSelect(HMENU hmenu, int item, HMENU hmenuPopup, UINT flags) */
#define DISPATCH_WM_MENUSELECT(wParam, lParam, fn)                  \
    ((fn)((HMENU)(lParam),  \
    (HIWORD(wParam) & MF_POPUP) ? 0L : (int)(LOWORD(wParam)),           \
    (HIWORD(wParam) & MF_POPUP) ? GetSubMenu((HMENU)lParam, LOWORD(wParam)) : 0L, \
    (UINT)(((short)HIWORD(wParam) == -1) ? 0xFFFFFFFF : HIWORD(wParam))), 0L)

/* DWORD Class::OnMenuChar(UINT ch, UINT flags, HMENU hmenu) */
#define DISPATCH_WM_MENUCHAR(wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((UINT)(LOWORD(wParam)), (UINT)HIWORD(wParam), (HMENU)(lParam))

/* void Class::OnCommand(int id, HWND hwndCtl, UINT codeNotify) */
#define DISPATCH_WM_COMMAND(wParam, lParam, fn) \
    ((fn)((int)(LOWORD(wParam)), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)

/* void Class::OnCommandReflect(UINT codeNotify) */
#define	DISPATCH_OCM_COMMAND(wParam, lParam, fn)	\
	((fn)((int)((UINT)HIWORD(wParam)), 0L)

/* LRESULT Class::OnNotify(int id, LPNMHDR pnmh) */
#define	DISPATCH_WM_NOTIFY(wParam, lParam, fn)	\
	((fn)((int)wParam, (LPNMHDR)lParam))

/* LRESULT Class::OnNotifyReflect(LPNMHDR pnmh) */
#define DISPATCH_OCM_NOTIFY(wParam, lParam, fn)	\
	((fn)((LPNMHDR)lParam))

/* void Class::OnHScroll(HWND hwndCtl, UINT code, int pos) */
#define DISPATCH_WM_HSCROLL(wParam, lParam, fn) \
    ((fn)((HWND)(lParam), (UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam)), 0L)

/* void Class::OnHScrollReflect(UINT code, int pos) */
#define DISPATCH_OCM_HSCROLL(wParam, lParam, fn) \
    ((fn)((UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam)), 0L)

/* void Class::OnVScroll(HWND hwndCtl, UINT code, int pos) */
#define DISPATCH_WM_VSCROLL(wParam, lParam, fn) \
    ((fn)((HWND)(lParam), (UINT)(LOWORD(wParam)),  (int)(short)HIWORD(wParam)), 0L)

/* void Class::OnVScrollReflect(UINT code, int pos) */
#define DISPATCH_OCM_VSCROLL(wParam, lParam, fn) \
    ((fn)((UINT)(LOWORD(wParam)),  (int)(short)HIWORD(wParam)), 0L)

/* void Class::OnCut() */
#define DISPATCH_WM_CUT(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnCopy() */
#define DISPATCH_WM_COPY(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnPaste() */
#define DISPATCH_WM_PASTE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnClear() */
#define DISPATCH_WM_CLEAR(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnUndo() */
#define DISPATCH_WM_UNDO(wParam, lParam, fn) \
    ((fn)(), 0L)

/* HANDLE Class::OnRenderFormat(UINT fmt) */
#define DISPATCH_WM_RENDERFORMAT(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HANDLE)(fn)((UINT)(wParam))

/* void Class::OnRenderAllFormats() */
#define DISPATCH_WM_RENDERALLFORMATS(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnDestroyClipboard() */
#define DISPATCH_WM_DESTROYCLIPBOARD(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnDrawClipboard() */
#define DISPATCH_WM_DRAWCLIPBOARD(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Class::OnPaintClipboard(HWND hwndCBViewer, const LPPAINTSTRUCT lpPaintStruct) */
#define DISPATCH_WM_PAINTCLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (const LPPAINTSTRUCT)GlobalLock((HGLOBAL)(lParam))), GlobalUnlock((HGLOBAL)(lParam)), 0L)

/* void Class::OnSizeClipboard(HWND hwndCBViewer, const LPRECT lprc) */
#define DISPATCH_WM_SIZECLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (const LPRECT)GlobalLock((HGLOBAL)(lParam))), GlobalUnlock((HGLOBAL)(lParam)), 0L)

/* void Class::OnVScrollClipboard(HWND hwndCBViewer, UINT code, int pos) */
#define DISPATCH_WM_VSCROLLCLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (UINT)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Class::OnHScrollClipboard(HWND hwndCBViewer, UINT code, int pos) */
#define DISPATCH_WM_HSCROLLCLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (UINT)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Class::OnAskCBFormatName(int cchMax, LPTSTR rgchName) */
#define DISPATCH_WM_ASKCBFORMATNAME(wParam, lParam, fn) \
    ((fn)((int)(wParam), (LPTSTR)(lParam)), 0L)

/* void Class::OnChangeCBChain(HWND hwndRemove, HWND hwndNext) */
#define DISPATCH_WM_CHANGECBCHAIN(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (HWND)(lParam)), 0L)

/* BOOL Class::OnSetCursor(HWND hwndCursor, UINT codeHitTest, UINT msg) */
#define DISPATCH_WM_SETCURSOR(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((HWND)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam))

/* void Class::OnSysCommand(UINT cmd, int x, int y) */
#define DISPATCH_WM_SYSCOMMAND(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* HWND Class::MDICreate(const LPMDICREATESTRUCT lpmcs) */
#define DISPATCH_WM_MDICREATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((LPMDICREATESTRUCT)(lParam))

/* void Class::MDIDestroy(HWND hwndDestroy) */
#define DISPATCH_WM_MDIDESTROY(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* NOTE: Usable only by MDI client windows */
/* void Class::MDIActivate(BOOL fActive, HWND hwndActivate, HWND hwndDeactivate) */
#define DISPATCH_WM_MDIACTIVATE(wParam, lParam, fn) \
    ((fn)((BOOL)(lParam == (LPARAM)GetHandle()), (HWND)(lParam), (HWND)(wParam)), 0L)

/* void Class::MDIRestore(HWND hwndRestore) */
#define DISPATCH_WM_MDIRESTORE(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* HWND Class::MDINext(HWND hwndCur, BOOL fPrev) */
#define DISPATCH_WM_MDINEXT(wParam, lParam, fn) \
    (LRESULT)(HWND)(fn)((HWND)(wParam), (BOOL)lParam)

/* void Class::MDIMaximize(HWND hwndMaximize) */
#define DISPATCH_WM_MDIMAXIMIZE(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* BOOL Class::MDITile(UINT cmd) */
#define DISPATCH_WM_MDITILE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((UINT)(wParam))

/* BOOL Class::MDICascade(UINT cmd) */
#define DISPATCH_WM_MDICASCADE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((UINT)(wParam))

/* void Class::MDIIconArrange() */
#define DISPATCH_WM_MDIICONARRANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* HWND Class::MDIGetActive() */
#define DISPATCH_WM_MDIGETACTIVE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)()

/* HMENU Class::MDISetMenu(BOOL fRefresh, HMENU hmenuFrame, HMENU hmenuWindow) */
#define DISPATCH_WM_MDISETMENU(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((BOOL)(wParam), (HMENU)(wParam), (HMENU)(lParam))

/* void Class::OnChildActivate() */
#define DISPATCH_WM_CHILDACTIVATE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Class::OnInitDialog(HWND hwndFocus, LPARAM lParam) */
#define DISPATCH_WM_INITDIALOG(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(BOOL)(fn)((HWND)(wParam), lParam)

/* HWND Class::OnNextDlgCtl(HWND hwndSetFocus, BOOL fNext) */
#define DISPATCH_WM_NEXTDLGCTL(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HWND)(fn)((HWND)(wParam), (BOOL)(lParam))

/* void Class::OnParentNotify(UINT msg, HWND hwndChild, int idChild) */
#define DISPATCH_WM_PARENTNOTIFY(wParam, lParam, fn) \
    ((fn)((UINT)LOWORD(wParam), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)

/* void Class:OnParentNotifyReflect(UINT msg) */
#define DISPATCH_OCM_PARENTNOTIFY(wParam, lParam, fn) \
	((fn)((UINT)LOWORD(wParam)), 0L)

/* void Class::OnEnterIdle(UINT source, HWND hwndSource) */
#define DISPATCH_WM_ENTERIDLE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (HWND)(lParam)), 0L)

/* UINT Class::OnGetDlgCode(LPMSG lpmsg) */
#define DISPATCH_WM_GETDLGCODE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((LPMSG)(lParam))

/* HBRUSH Class::OnCtlColor(HDC hdc, HWND hwndChild, int type) */
#define DISPATCH_WM_CTLCOLORMSGBOX(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_MSGBOX)

#define DISPATCH_WM_CTLCOLOREDIT(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_EDIT)

#define DISPATCH_WM_CTLCOLORLISTBOX(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_LISTBOX)

#define DISPATCH_WM_CTLCOLORBTN(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_BTN)

#define DISPATCH_WM_CTLCOLORDLG(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_DLG)

#define DISPATCH_WM_CTLCOLORSCROLLBAR(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_SCROLLBAR)

#define DISPATCH_WM_CTLCOLORSTATIC(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_STATIC)

/* HBRUSH Class::OnCtlColorReflect(HDC hdc, int type) */
#define DISPATCH_OCM_CTLCOLORMSGBOX(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), CTLCOLOR_MSGBOX)

#define DISPATCH_OCM_CTLCOLOREDIT(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), CTLCOLOR_EDIT)

#define DISPATCH_OCM_CTLCOLORLISTBOX(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), CTLCOLOR_LISTBOX)

#define DISPATCH_OCM_CTLCOLORBTN(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), CTLCOLOR_BTN)

#define DISPATCH_OCM_CTLCOLORDLG(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), CTLCOLOR_DLG)

#define DISPATCH_OCM_CTLCOLORSCROLLBAR(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), CTLCOLOR_SCROLLBAR)

#define DISPATCH_OCM_CTLCOLORSTATIC(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), CTLCOLOR_STATIC)

/* void Class::OnSetFont(HFONT hfont, BOOL fRedraw) */
#define DISPATCH_WM_SETFONT(wParam, lParam, fn) \
    ((fn)((HFONT)(wParam), (BOOL)(lParam)), 0L)

/* HFONT Class::OnGetFont() */
#define DISPATCH_WM_GETFONT(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HFONT)(fn)()

/* void Class::OnDrawItem(const DRAWITEMSTRUCT * lpDrawItem) */
#define DISPATCH_WM_DRAWITEM(wParam, lParam, fn) \
    ((fn)((const DRAWITEMSTRUCT *)(lParam)), 0L)

/* void Class::OnDrawItemReflect(const DRAWITEMSTRUCT * lpDrawItem) */
#define DISPATCH_OCM_DRAWITEM(wParam, lParam, fn) \
    ((fn)((const DRAWITEMSTRUCT *)(lParam)), 0L)

/* void Class::OnMeasureItem(MEASUREITEMSTRUCT * lpMeasureItem) */
#define DISPATCH_WM_MEASUREITEM(wParam, lParam, fn) \
    ((fn)((MEASUREITEMSTRUCT *)(lParam)), 0L)

/* void Class::OnMeasureItemReflect(MEASUREITEMSTRUCT * lpMeasureItem) */
#define DISPATCH_OCM_MEASUREITEM(wParam, lParam, fn) \
    ((fn)((MEASUREITEMSTRUCT *)(lParam)), 0L)

/* void Class::OnDeleteItem(const DELETEITEMSTRUCT * lpDeleteItem) */
#define DISPATCH_WM_DELETEITEM(wParam, lParam, fn) \
    ((fn)((const DELETEITEMSTRUCT *)(lParam)), 0L)

/* void Class::OnDeleteItemReflect(const DELETEITEMSTRUCT * lpDeleteItem) */
#define DISPATCH_OCM_DELETEITEM(wParam, lParam, fn) \
    ((fn)((const DELETEITEMSTRUCT *)(lParam)), 0L)

/* int Class::OnCompareItem(const COMPAREITEMSTRUCT * lpCompareItem) */
#define DISPATCH_WM_COMPAREITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((const COMPAREITEMSTRUCT *)(lParam))

/* int Class::OnCompareItemReflect(const COMPAREITEMSTRUCT * lpCompareItem) */
#define DISPATCH_OCM_COMPAREITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((const COMPAREITEMSTRUCT *)(lParam))

/* int Class::OnVkeyToItem(UINT vk, HWND hwndListbox, int iCaret) */
#define DISPATCH_WM_VKEYTOITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((UINT)LOWORD(wParam), (HWND)(lParam), (int)(short)HIWORD(wParam))

/* int Class::OnVkeyToItemReflect(UINT vk, int iCaret) */
#define DISPATCH_OCM_VKEYTOITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((UINT)LOWORD(wParam), (int)(short)HIWORD(wParam))

/* int Class::OnCharToItem(UINT ch, HWND hwndListbox, int iCaret) */
#define DISPATCH_WM_CHARTOITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((UINT)LOWORD(wParam), (HWND)(lParam), (int)(short)HIWORD(wParam))

/* int Class::OnCharToItemReflect(UINT ch, int iCaret) */
#define DISPATCH_OCM_CHARTOITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((UINT)LOWORD(wParam), (int)(short)HIWORD(wParam))

/* void Class::OnQueueSync() */
#define DISPATCH_WM_QUEUESYNC(wParam, lParam, fn) \
    ((fn)(), 0L)

#if (WINVER >= 0x030a)
/* void Class::OnCommNotify(int cid, UINT flags) */
#define DISPATCH_WM_COMMNOTIFY(wParam, lParam, fn) \
    ((fn)((int)(wParam), (UINT)LOWORD(lParam)), 0L)
#endif

/* void Class::OnDisplayChange(UINT bitsPerPixel, UINT cxScreen, UINT cyScreen) */
#define DISPATCH_WM_DISPLAYCHANGE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(wParam)), 0L)

/* BOOL Class::OnDeviceChange(UINT uEvent, DWORD dwEventData) */
#define DISPATCH_WM_DEVICECHANGE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((UINT)(wParam), (DWORD)(wParam))

/* void Class::OnContextMenu(HWND hwndContext, UINT xPos, UINT yPos) */
#define DISPATCH_WM_CONTEXTMENU(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

#endif // !defined(_SUBCLASSWND_H_199B246E_AE5D_11D3_A459_000629B2F85_INCLUDED_)
