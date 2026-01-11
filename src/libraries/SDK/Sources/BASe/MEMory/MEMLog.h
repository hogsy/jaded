/*$T MEMLog.h GC! 1.089 08/01/00 15:33:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MEMLOG_H__
#define __MEMLOG_H__
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifdef ACTIVE_EDITORS
extern int MEM_gi_AllocatedMemory;
extern int MEM_gi_AddAllocatedMemory;
#endif

#if defined(_DEBUG) || defined(ACTIVE_EDITORS)
#define MEM_LOG
#endif // #if defined(_DEBUG) || defined(ACTIVE_EDITORS)

#ifdef MEM_LOG
extern void MEM_Log(char *, void *, ULONG, ULONG, char *, int);
extern void MEM_LogString(char *);
#endif // MEM_LOG

extern void MEM_MakeAllocatedMemoryString(char *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
