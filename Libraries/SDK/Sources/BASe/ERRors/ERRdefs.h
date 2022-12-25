/*$T ERRdefs.h GC!1.41 08/16/99 13:50:05 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __ERRDEFS_H__
#define __ERRDEFS_H__

#include "BASe/BASsys.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIxxx.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Name of log file where errors and assertions are always wrote */
#define ERR_Csz_AssertLogName   "ERRasser.log"

// Global Buffer used by ERR_vFormatMessage
// When you use ERR_vFormatMessage, the message is formatted in this
// Buffer and is returned.
#define ERR_BUFSIZEMSG (2048)
extern CHAR ERR_g_szFormatBuffer[ERR_BUFSIZEMSG];

#if defined(ACTIVE_EDITORS)

/*
 ---------------------------------------------------------------------------------------------------
    Structure to define an exception. Exceptions are only present in editor mode.
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  _ERR_tdst_Exception_
{
    /* Buffer for L_setjmp */
    L_jmp_buf                   st_Buf;

    /*
     -----------------------------------------------------------------------------------------------
        Previous exception in the list
     -----------------------------------------------------------------------------------------------
     */
    struct _ERR_tdst_Exception_ *pst_Previous;
} _ERR_tdst_Exception;

extern _ERR_tdst_Exception  *_ERR_gpst_LastException;
extern void                 ERR_LastException(void);
#endif /* ACTIVE_EDITORS */

/*$2
 ---------------------------------------------------------------------------------------------------
    Functions
 ---------------------------------------------------------------------------------------------------
 */

extern BOOL					ERR_gb_Warning;
extern BOOL					ERR_gb_WarningBox;
extern BOOL                 _ERR_fnb_AssertFailed(BOOL, char *, int, char *, char *, char *, BOOL);
extern struct OBJ_tdst_GameObject_	*ERR_gpst_ContextGAO;
extern char							*ERR_gpsz_ContextString;

// Format a message using the printf format, put it in ERR_g_szFormatBuffer and return the string.
CHAR * ERR_szFormatMessage(CHAR  *_szFormat, ...);

#ifdef ACTIVE_EDITORS
extern void					ERR_TestWarning(void);
extern void                 ERR_LogPrint(const char *_s_Msg);
#else
#define ERR_TestWarning()
#define ERR_LogPrint(_s_Msg)
#endif

#ifdef JADEFUSION
#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL) || (defined(_XENON) && defined(_DEBUG))
void __stdcall ERR_OutputDebugString(CHAR* _sz_Format, ...);
#else
_inline_ void ERR_OutputDebugString(CHAR* _sz_Format, ...) { }
#endif // defined(ACTIVE_EDITORS) || (defined(_XENON) && defined(_DEBUG))
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __ERRDEFS_H__ */