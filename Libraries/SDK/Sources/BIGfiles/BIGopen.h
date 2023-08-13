/*$T BIGopen.h GC!1.52 11/10/99 11:56:06 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern bool             BIG_Open(char *);
extern void             BIG_Close(void);
extern BIG_tdst_BigFile BIG_gst;

#ifdef ACTIVE_EDITORS
extern void             BIG_CreateEmptyPriv(char *, ULONG, ULONG, ULONG, ULONG, ULONG);
extern void             BIG_CreateEmpty(char *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
