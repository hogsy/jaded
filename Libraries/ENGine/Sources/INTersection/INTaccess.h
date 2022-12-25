/*$T INTaccess.h GC!1.41 09/15/99 09:36:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __INT_ACCESS__
#define __INT_ACCESS__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "BASe/BAStypes.h"

extern void INT_SnP_ResetFlags(ULONG *);
extern BOOL INT_SnP_GetFlag(ULONG *, ULONG, ULONG);
extern void INT_SnP_SetFlag(ULONG *, ULONG , ULONG , BOOL);
extern BOOL INT_SnP_FullGetFlag(ULONG *, ULONG, ULONG, ULONG, ULONG *, ULONG *);
extern BOOL INT_SnP_FullSetFlag(ULONG *, ULONG, ULONG, ULONG, ULONG *, ULONG *, BOOL);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _INT_ACCESS__ */ 
 