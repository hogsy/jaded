/*$T BIGexport.h GC!1.20 04/01/99 14:36:04 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern BOOL BIG_gb_ExportRecurse;

extern void BIG_ExportFileToDisk(char *, char *, char *);
extern void BIG_ExportDirToDisk(char *, char *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
