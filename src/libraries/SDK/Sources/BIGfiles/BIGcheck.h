/*$T BIGcheck.h GC!1.52 12/09/99 10:00:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGopen.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern BOOL BIG_gb_CanClean;
extern void BIG_RestoreDeleted(void);
extern BOOL BIG_b_CheckFile(BOOL);
extern void BIG_Clean(BOOL);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
