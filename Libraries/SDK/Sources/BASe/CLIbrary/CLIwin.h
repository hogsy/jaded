/*$T CLIwin.h GC!1.55 01/19/00 16:59:45 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#if defined( _WIN32 )

#	define _WIN32_WINNT 0x0A00

/* hogsy :  I tried to be rid of this; some suggest that it's a fault 
 *          with the include order due to Windows SDK / DirectX SDK, but
 *          it straight-up looks like an issue with the Windows SDK headers, 
 *          full-stop. So we'll keep it for now, but uh, TODO!!!            */
#	pragma warning( disable : 4005 )
#	ifndef POINTER_64
#		define POINTER_64
#	endif

#	if defined( ACTIVE_EDITORS ) && defined( __cplusplus )
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MFC
 -----------------------------------------------------------------------------------------------------------------------
 */

#		define OEMRESOURCE
typedef long SHANDLE_PTR;
#		include "afxwin.h"
#		include "afxext.h"
#		include "afxcmn.h"
#		include "afxcoll.h"
#		include "afxpriv.h"
#		include "afxrich.h"
#		include "afxtempl.h"
#		include "shlobj.h"

#	else

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Windows
 -----------------------------------------------------------------------------------------------------------------------
 */

#		include <Windows.h>

#	endif

#	if defined( _MSC_VER )
#		define L_access  _access
#		define L_itoa    _itoa
#		define L_stricmp _stricmp
#	else
#		define L_access  access
#		define L_itoa    itoa
#		define L_stricmp stricmp
#	endif

#	include "mmsystem.h"

#	define L_timeGetTime timeGetTime

#endif
