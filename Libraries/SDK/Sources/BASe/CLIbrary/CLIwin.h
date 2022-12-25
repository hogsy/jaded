/*$T CLIwin.h GC!1.55 01/19/00 16:59:45 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#if !defined(PSX2_TARGET)
    #pragma once
#endif

#ifndef __CLIWIN_H__
#define __CLIWIN_H__

#if !defined PSX2_TARGET && !defined _GAMECUBE


#if defined(ACTIVE_EDITORS) && defined(__cplusplus)
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MFC
 -----------------------------------------------------------------------------------------------------------------------
 */
#if !defined(POINTER_64)
#define POINTER_64
#endif


#define      OEMRESOURCE
typedef long SHANDLE_PTR;
#include     "afxwin.h"
#include     "afxext.h"
#include     "afxcmn.h"
#include     "afxcoll.h"
#include     "afxpriv.h"
#include     "afxrich.h"
#include     "afxtempl.h"
#include     "shlobj.h"

#else 
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Windows
 -----------------------------------------------------------------------------------------------------------------------
 */
#if !defined(_XBOX) && !defined(_XENON)
#if !defined(POINTER_64)
#define POINTER_64
#endif
#include     "windows.h"
#endif

#endif 



#if !defined(_XBOX) && !defined(_XENON)
#include "mmsystem.h"
#endif

#endif

#define L_timeGetTime   timeGetTime

#if defined(_XBOX) && !defined(_XENON)
//#define HWND unsigned long
#include <xtl.h>

#endif


#endif /* __CLIWIN_H__ */
