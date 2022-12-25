/*$T LINKmsg.h GC!1.41 09/14/99 10:43:40 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern BOOL     LINK_gb_EscapeDetected;
extern BOOL		LINK_gb_IgnoreEscape;
extern BOOL     LINK_gb_CanLog;
extern BOOL     LINK_gb_CanDisplay;
extern BOOL     LINK_gb_CanAbort;
extern BOOL     LINK_gb_UseSecond;
extern BOOL     LINK_gb_OnSameLine;
extern ULONG	LINK_gul_ColorTxt;
extern void     LINK_PrintStatusMsg(char *);
extern void     LINK_PrintStatusMsgCanal(char *,int);
extern void     LINK_PrintStatusMsgEOL(char *);
extern void     LINK_PrintStatusMsgEOLCanal(char *,int);
extern void     LINK_BeginLongOperation(char *);
extern void     LINK_EndLongOperation(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#else
#define LINK_PrintStatusMsg(a)
#define LINK_BeginLongOperation(a)
#define LINK_EndLongOperation()
#endif /* ACTIVE_EDITORS */
