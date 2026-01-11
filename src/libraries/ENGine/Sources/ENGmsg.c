/*$T ENGmsg.c GC! 1.081 04/21/00 16:23:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/MEMory/MEMlog.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "GraphicDK/Sources/SOFT/SOFTzlist.h"

#include "INOut/INOjoystick.h"
#include "GraphicDK/Sources/GDInterface/GDIrasters.h"

#ifdef ACTIVE_EDITORS
#include "GraphicDK/Sources/SOFT/SOFTpickingbuffer.h"
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
BOOL	ENG_gb_GlobalLock = FALSE;	/* To tell that main display is locked (do not update size) */
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern ULONG	EDI_OUT_gl_ForceSetMode;
#if defined (__cplusplus) && !defined(JADEFUSION)
};
#endif
#endif

GDI_tdst_DisplayData *GDI_pst_GlobalDD;
#ifndef _DX8
extern int      GDI_gi_GDIType;
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL MAI_b_TreatMainWndMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	/*~~~~~~~~~~~~~~~~~*/
#ifndef ACTIVE_EDITORS
#ifndef _FINAL_
	char	asz_Buf[512];
#endif
#endif
	/*~~~~~~~~~~~~~~~~~*/

	switch(message)
	{
	case WM_CLOSE:
	case WM_QUIT:
		if(ENG_gb_EngineRunning)
		{
			ENG_gb_ExitApplication = TRUE;
			*pResult = 0;
			return TRUE;
		}
		return FALSE;

	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_CLOSE:
			if(ENG_gb_EngineRunning)
			{
				ENG_gb_ExitApplication = TRUE;
				*pResult = 0;
				return TRUE;
			}
			return FALSE;
		}
		break;

	case WM_ACTIVATEAPP:
		if(!wParam)
		{
			ENG_gb_ForcePauseEngine = TRUE;
			INO_Joystick_Unacquire();
		}
		else
		{
			ENG_gb_ForcePauseEngine = FALSE;
			INO_Joystick_Acquire();
		}

	case WM_NCLBUTTONDOWN:
#ifndef ACTIVE_EDITORS
#ifndef _FINAL_
		MEM_MakeAllocatedMemoryString(asz_Buf);
		SetWindowText(hWnd, asz_Buf);
#endif
#endif
		break;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL MAI_b_TreatOwnerWndMessages
(
	HWND				hWnd,
	MAI_tdst_WinHandles *ph,
	UINT				message,
	WPARAM				wParam,
	LPARAM				lParam,
	LRESULT				*pResult
)
{
	/*~~~~~~~~~~~~~~~*/
	RECT		rect;
	POINT		pt1;
	MINMAXINFO	*lpmmi;
	/*~~~~~~~~~~~~~~~*/

	switch(message)
	{
	case WM_ERASEBKGND:
#ifndef ACTIVE_EDITORS
		{
			/*~~~~~~~~~*/
			HDC		hdc;
			RECT	rect;
			/*~~~~~~~~~*/

			hdc = GetDC(hWnd);
			GetClientRect(hWnd, &rect);
			SetBkColor(hdc, 0);
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			ReleaseDC(hWnd, hdc);
		}

#endif
		*pResult = TRUE;
		return TRUE;

	case WM_CREATE:
		ph->h_OwnerWindow = hWnd;
		break;

	case WM_GETMINMAXINFO:
#ifndef ACTIVE_EDITORS
		if(sgb_FullScreen)
		{
			/*~~~~~~~~~~~~~~~*/
			LPMINMAXINFO	lp;
			/*~~~~~~~~~~~~~~~*/

			DefWindowProc(hWnd, message, wParam, lParam);
			lp = (LPMINMAXINFO) lParam;
			lp->ptMaxPosition.y -= GetSystemMetrics(SM_CYCAPTION);
			lp->ptMaxSize.y += GetSystemMetrics(SM_CYCAPTION);
			lp->ptMaxTrackSize.x = lp->ptMaxSize.x;
			lp->ptMaxTrackSize.y = lp->ptMaxSize.y;
			*pResult = 0;
			return TRUE;
		}

#endif
		lpmmi = (LPMINMAXINFO) lParam;
		lpmmi->ptMinTrackSize.x = 100;
		lpmmi->ptMinTrackSize.y = 100;
		return TRUE;

	case WM_SIZE:
		if(!ph->h_DisplayWindow) return FALSE;

#ifdef _PC_RETAIL
		{
			HANDLE kernel32 = LoadLibrary("kernel32");
		    BOOL (__stdcall *IsDebuggerPresent)(void) = GetProcAddress(kernel32, "IsDebuggerPresent");
			if(IsDebuggerPresent && IsDebuggerPresent())
				return FALSE;
		}
#endif

		GetWindowRect(ph->h_DisplayWindow, &rect);
		pt1.x = rect.left;
		pt1.y = rect.top;
		ScreenToClient(ph->h_OwnerWindow, &pt1);
		GetClientRect(ph->h_OwnerWindow, &rect);
		MoveWindow(ph->h_DisplayWindow, pt1.x, pt1.y, rect.right, rect.bottom - pt1.y, FALSE);
		return FALSE;

	case WM_MOVE:
		break;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL MAI_b_TreatDisplayWndMessages
(
	HWND				hWnd,
	MAI_tdst_WinHandles *ph,
	UINT				message,
	WPARAM				wParam,
	LPARAM				lParam,
	LRESULT				*pResult
)
{
	/*~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	ULONG	Keep;
#endif
	/*~~~~~~~~~*/

	switch(message)
	{
#ifndef ACTIVE_EDITORS
	case WM_MOUSEMOVE:
		SetCursor(NULL);
		break;
#endif

	case WM_ERASEBKGND:
#ifndef ACTIVE_EDITORS
		GDI_BeforeDisplay(ph->pst_DisplayData);
		if((ph->pst_World) && (ph->pst_World == ph->pst_DisplayData->pst_World))
			WOR_Render(ph->pst_World, ph->pst_DisplayData);
		else
		{
			/*~~~~~~~~~*/
			HDC		hdc;
			RECT	rect;
			/*~~~~~~~~~*/

			hdc = GetDC(hWnd);
			GetClientRect(hWnd, &rect);
			SetBkColor(hdc, 0);
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			ReleaseDC(hWnd, hdc);
		}

		GDI_AfterDisplay(ph->pst_DisplayData);
#else
		if((ph->pst_World) && (ph->pst_World == ph->pst_DisplayData->pst_World))
			LINK_Refresh();
		else
		{
			/*~~~~~~~~~*/
			HDC		hdc;
			RECT	rect;
			/*~~~~~~~~~*/

			hdc = GetDC(hWnd);
			GetClientRect(hWnd, &rect);
			SetBkColor(hdc, 0);
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			ReleaseDC(hWnd, hdc);
		}
#endif
		return TRUE;

	case WM_CREATE:
		if(ph)
		{
			ph->h_DisplayWindow = hWnd;
			ph->pst_DisplayData = GDI_pst_GlobalDD = GDI_fnpst_CreateDisplayData();

			GDI_fnl_InitInterface(&ph->pst_DisplayData->st_GDI, GDI_gi_GDIType);

			ph->pst_DisplayData->pv_SpecificData = OGL_pst_CreateDevice();

#ifndef ACTIVE_EDITORS
#ifdef RASTERS_ON
			GDI_Rasters_Init(ph->pst_DisplayData->pst_Raster, "Display Data");
#endif
#endif
		}
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		if(ph && ph->pst_DisplayData)
		{
			GDI_DetachDisplay(ph->pst_DisplayData);
#ifdef ACTIVE_EDITORS
            SOFT_PickingBuffer_Close(ph->pst_DisplayData->pst_PickingBuffer);
#endif
			ph->pst_DisplayData->st_GDI.pfnv_DesinitDisplay(ph->pst_DisplayData->pv_SpecificData);
			GDI_fnv_DestroyDisplayData(ph->pst_DisplayData);
		}
		break;

#ifndef ACTIVE_EDITORS
	case WM_DISPLAYCHANGE:
#endif

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_SIZE:
#ifdef ACTIVE_EDITORS
		if(ENG_gb_GlobalLock) return FALSE;
#endif
		if(ph && ph->pst_DisplayData && LOWORD(lParam) && HIWORD(lParam))
		{
#ifdef ACTIVE_EDITORS
			Keep = EDI_OUT_gl_ForceSetMode;
			EDI_OUT_gl_ForceSetMode = 1;
			GDI_ReadaptDisplay(ph->pst_DisplayData, hWnd);
			EDI_OUT_gl_ForceSetMode = Keep;
			LINK_Refresh();
#else
			GDI_ReadaptDisplay(ph->pst_DisplayData, hWnd);
			SOFT_ZList_Init();
			SOFT_ZList_Clear();
			GDI_BeforeDisplay(ph->pst_DisplayData);
			if((ph->pst_World) && (ph->pst_World == ph->pst_DisplayData->pst_World))
				WOR_Render(ph->pst_World, ph->pst_DisplayData);
			GDI_AfterDisplay(ph->pst_DisplayData);
#endif
		}
		break;
	}

	return FALSE;
}

