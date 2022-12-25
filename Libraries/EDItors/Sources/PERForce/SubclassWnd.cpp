/** @file SubclassWnd.cpp
 *
 * Implementation file for CSubclassWnd.
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

#include "precomp.h"
#include "SubclassWnd.h"

namespace
{
	/*
	 * Sends a message to all child controls of a specified window.
	 *
	 * @param message [in] Specifies the message id.
	 * @param wParam [in] Specifies additional message-specific information.
	 * @param lParam [in] Specifies additional message-specific information.
	 * @param bDeep [in] Specifies whether or not the message should be sent
	 *		recursively to all children.
	 */

	void SendMessageToDescendantsImp(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam, BOOL bDeep)
	{
		// walk through HWNDs to avoid creating temporary CWnd objects
		// unless we need to call this function recursively
		for (HWND hWndChild = ::GetTopWindow(hWnd); hWndChild != NULL;
			hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
		{
			// send message with Windows SendMessage API
			::SendMessage(hWndChild, message, wParam, lParam);
			if (bDeep && ::GetTopWindow(hWndChild) != NULL)
			{
				// send to child windows after parent
				SendMessageToDescendantsImp(hWndChild, message, wParam, lParam,
					bDeep);
			}
		}
	}
};

/*
 * Message used to unsubclass CSubclassWnd objects.
 */
const UINT WM_UNSUBCLASS = RegisterWindowMessage("{207D9568-FF3C-11D3-A469-000629B2F855}");
/*
 * Message used to reflect messages back to child controls.
 */
const UINT WM_REFLECT = RegisterWindowMessage("{9688DAE5-3736-11D4-A48C-000629B2F855}");
/*
 * Message used to get the reflector plugin from a window if one exists.
 */
const UINT WM_GETREFLECTOR = RegisterWindowMessage("{9688DAE6-3736-11D4-A48C-000629B2F855}");

/*
 * Specifies the reflected message information.  Used by WM_REFLECT.
 */
struct MSGREFLECTSTRUCT
{
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	BOOL bHandled;
};

/*
 * Message handler used to reflect messages from a parent window to a child window.
 */
class CMessageReflector : public CSubclassWnd
{
private:
	CMessageReflector(HWND hWnd);

public:
	static CMessageReflector* FromHandle(HWND hWnd);
	virtual BOOL ProcessWindowMessage(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT& lResult);
	virtual void OnFinalMessage();
};

/*
 * Constructor.  Subclasses the specified window.
 *
 * @param hWnd [in] Handle to the window to be subclassed.
 */

CMessageReflector::CMessageReflector(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	SubclassWindow(hWnd, FALSE);
}

/*
 * This method retrieves a pointer to a CMessageReflector object from a handle to a
 * window.  If the specified window has not yet been subclassed by a CMessageReflector
 * instance then a new instance is created on the heap.
 *
 * @param hWnd [in] Handle to the window to retrieve the CMessageReflector from.
 *
 * @return A pointer to the message reflector associated with the window.
 */

CMessageReflector* CMessageReflector::FromHandle(HWND hWnd)
{
	CMessageReflector* pReflector =
		(CMessageReflector*)::SendMessage(hWnd, WM_GETREFLECTOR, 0, 0);
	if (!pReflector)
		pReflector = new CMessageReflector(hWnd);
	return pReflector;
}

/*
 * Called to handle window messages.  Derived classes can override this
 * method to handle any window messages sent to the subclassed window.
 * Normally the programmer won't code this directly, but will instead use
 * the BEGIN_MSG_DISPATCH/END_MSG_DISPATCH macros.
 *
 * @param message [in] Specifies the message id.
 * @param wParam [in] Specifies additional message-specific information.
 * @param lParam [in] Specifies additional message-specific information.
 * @param lResult [in] A reference to the result value to be returned to
 *		the caller.
 *
 * @return TRUE if the message was handled, otherwise FALSE.
 */

BOOL CMessageReflector::ProcessWindowMessage(UINT message, WPARAM wParam, LPARAM lParam,
											 LRESULT& lResult)
{
	if (message == WM_GETREFLECTOR)
	{
		lResult = (LRESULT)this;
		return TRUE;
	}

	HWND hWndChild = NULL;

	switch (message)
	{
	case WM_COMMAND:
		if (lParam != NULL)	// not from a menu
			hWndChild = (HWND)lParam;
		break;
	case WM_NOTIFY:
		hWndChild = ((LPNMHDR)lParam)->hwndFrom;
		break;
	case WM_PARENTNOTIFY:
		switch (LOWORD(wParam))
		{
		case WM_CREATE:
		case WM_DESTROY:
			hWndChild = (HWND)lParam;
			break;
		default:
			hWndChild = GetDlgItem(GetHandle(), HIWORD(wParam));
			break;
		}
		break;
	case WM_DRAWITEM:
		if (wParam)		// not from a menu
			hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
		break;
	case WM_MEASUREITEM:
		if (wParam)		// not from a menu
			hWndChild = GetDlgItem(GetHandle(), ((LPMEASUREITEMSTRUCT)lParam)->CtlID);
		break;
	case WM_COMPAREITEM:
		if (wParam)		// not from a menu
			hWndChild = GetDlgItem(GetHandle(), ((LPCOMPAREITEMSTRUCT)lParam)->CtlID);
		break;
	case WM_DELETEITEM:
		if (wParam)		// not from a menu
			hWndChild = GetDlgItem(GetHandle(), ((LPDELETEITEMSTRUCT)lParam)->CtlID);
		break;
	case WM_VKEYTOITEM:
	case WM_CHARTOITEM:
	case WM_HSCROLL:
	case WM_VSCROLL:
		hWndChild = (HWND)lParam;
		break;
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
		hWndChild = (HWND)lParam;
		break;
	default:
		break;
	}

	if (hWndChild == NULL)
		return FALSE;

	ASSERT(::IsWindow(hWndChild));

	MSGREFLECTSTRUCT rs;
	rs.message = OCM__BASE + message;
	rs.wParam = wParam;
	rs.lParam = lParam;
	rs.bHandled = FALSE;

	lResult = ::SendMessage(hWndChild, WM_REFLECT, 0, (LPARAM)&rs);

	return rs.bHandled;
}

/*
 * Called after the last message has been dispatched to the window.
 */

void CMessageReflector::OnFinalMessage()
{
	delete this;
}

/**
 * Default contructor.
 */

CSubclassWnd::CSubclassWnd()
	: m_hWnd(0), m_pfnSuperWindowProc(::DefWindowProc), m_pCurrentMsg(0)
{
}

/**
 * Destructor.
 */

CSubclassWnd::~CSubclassWnd()
{
	if (m_hWnd != NULL)
		UnsubclassWindow();
}

/**
 * Called to subclass the specified window.  If the @a bReflect
 * parameter is non-zero then the parent window will be subclassed to
 * support message reflection.
 *
 * @param hWnd [in] Specifies the handle of the window to be subclassed.
 * @param bReflect [in] Specifies whether or not the parent should
 *		reflect messages back to the window.
 *
 * @return If the window was subclassed, the return value is non-zero.  If the
 *		window was not subclassed, the return value is zero.
 */

BOOL CSubclassWnd::SubclassWindow(HWND hWnd, BOOL bReflect)
{
	if (m_hWnd != NULL || !::IsWindow(hWnd))
		return FALSE;

	m_thunk.Init(WindowProc, this);
	WNDPROC pProc = (WNDPROC)&(m_thunk.thunk);
	WNDPROC pfnWndProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pProc);
	if (pfnWndProc == NULL)
		return FALSE;

	m_pfnSuperWindowProc = pfnWndProc;
	m_hWnd = hWnd;

	if (bReflect && (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) == WS_CHILD)
	{
		HWND hWndParent = GetParent(hWnd);
		ASSERT(::IsWindow(hWndParent));
		CMessageReflector::FromHandle(hWndParent);
	}

	return TRUE;
}

/**
 * Called to unsubclass the specified window.
 *
 * @return The handle to the window previously subclassed.
 */

HWND CSubclassWnd::UnsubclassWindow()
{
	return (HWND)SendMessage(WM_UNSUBCLASS, 0, (LPARAM)this);
}

/**
 * Implementation detail.  This is the actual WindowProc installed by
 * the thunk.
 *
 * @param hWnd [in] Handle to the window.
 * @param message [in] Specifies the message id.
 * @param wParam [in] Specifies additional message-specific information.
 * @param lParam [in] Specifies additional message-specific information.
 *
 * @return The return value is the result of the message processing and
 *		depends on the message sent.
 */

LRESULT CALLBACK CSubclassWnd::WindowProc(HWND hWnd, UINT message, WPARAM wParam,
										  LPARAM lParam)
{
	CSubclassWnd* pThis = (CSubclassWnd*)hWnd;

	// set a ptr to this message and save the old value
	MSG msg = { pThis->m_hWnd, message, wParam, lParam, 0, { 0, 0 } };

	// check to see if this is a reflected message and adjust accordingly
	MSGREFLECTSTRUCT* rs = 0;
	if (message == WM_REFLECT)
	{
		rs = (MSGREFLECTSTRUCT*)lParam;
		ASSERT(rs);
		msg.message = rs->message;
		msg.wParam = rs->wParam;
		msg.lParam = rs->lParam;
	}

	// save the message information and set the new current
	const MSG* pOldMsg = pThis->m_pCurrentMsg;
	pThis->m_pCurrentMsg = &msg;

	// pass to the message map to process
	LRESULT lRes;
	BOOL bRet = pThis->ProcessWindowMessage(msg.message, msg.wParam, msg.lParam, lRes);

	// if this is a reflected message let parent know if it was handled
	if (rs)
		rs->bHandled = bRet;

	// restore saved value for the current message
	pThis->m_pCurrentMsg = pOldMsg;

	// do the default processing if message was not handled
	// note that we need to use the original values, so that reflected
	// messages can properly set the 'bHandled' flag.
	if (!bRet)
		lRes = pThis->DefWindowProc(message, wParam, lParam);

	if (message == WM_NCDESTROY)
	{
		// unsubclass, if needed
		pThis->UnsubclassWindow();

		// clean up after window is destroyed
		pThis->OnFinalMessage();
	}

	return lRes;
}

/**
 * Sends a message to all child windows of the subclassed window.
 *
 * @param message [in] Specifies the message id.
 * @param wParam [in] Specifies additional message-specific information.
 * @param lParam [in] Specifies additional message-specific information.
 * @param bDeep [in] Specifies whether or not the message should be sent
 *		recursively to all children.
 */

void CSubclassWnd::SendMessageToDescendants(UINT message, WPARAM wParam, LPARAM lParam,
											BOOL bDeep)
{
	SendMessageToDescendantsImp(GetHandle(), message, wParam, lParam, bDeep);
}

/**
 * Called after the last message has been dispatched to the window.
 */

void CSubclassWnd::OnFinalMessage()
{
}

/**
 * Called to handle window messages.  Derived classes can override this
 * method to handle any window messages sent to the subclassed window.
 * Normally the programmer won't code this directly, but will instead use
 * the #BEGIN_MSG_DISPATCH/#END_MSG_DISPATCH macros.
 *
 * @param message [in] Specifies the message id.
 * @param wParam [in] Specifies additional message-specific information.
 * @param lParam [in] Specifies additional message-specific information.
 * @param lResult [out] A reference to the result value to be returned to
 *		the caller.
 *
 * @return If the message was handled, the return value is non-zero.  If the
 *		message was not handled, the return value is zero.
 */

BOOL CSubclassWnd::ProcessWindowMessage(UINT message, WPARAM wParam, LPARAM lParam,
										LRESULT& lResult)
{
	if (message == WM_UNSUBCLASS)
	{
		if ((CSubclassWnd*)lParam == this)
		{
			if (m_hWnd == NULL)
				lResult = NULL;
			else
			{
				if (wParam)
				{
					CSubclassWnd* pPrevious = (CSubclassWnd*)wParam;
					ASSERT(pPrevious->m_pfnSuperWindowProc == (WNDPROC)&(m_thunk.thunk));
					pPrevious->m_pfnSuperWindowProc = m_pfnSuperWindowProc;
					lResult = (LRESULT)m_hWnd;
					m_pfnSuperWindowProc = ::DefWindowProc;
					m_hWnd = NULL;
				}
				else
				{
					WNDPROC pOurProc = (WNDPROC)&(m_thunk.thunk);
					WNDPROC pActiveProc = (WNDPROC)::GetWindowLong(m_hWnd, GWL_WNDPROC);
					ASSERT(pOurProc == pActiveProc);
					if (!::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_pfnSuperWindowProc))
						lResult = NULL;
					else
					{
						m_pfnSuperWindowProc = ::DefWindowProc;
						lResult = (LRESULT)m_hWnd;
						m_hWnd = NULL;
					}
				}
			}
		}
		else
			lResult = DefWindowProc(message, (WPARAM)this, lParam);

		return TRUE;
	}

	return FALSE;
}
