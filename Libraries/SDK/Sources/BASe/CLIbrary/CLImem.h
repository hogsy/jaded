/*$T CLImem.h GC! 1.081 07/15/02 18:32:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __CLIMEM_H__
#define __CLIMEM_H__

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    GAMECUBE
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif // __cplusplus

#ifdef _GAMECUBE
#ifndef GC2RVL_NO_ARAM
#include <dolphin/os.h>
#define L_malloc(a) OSAlloc(a)
#define L_free(a)	OSFree(a)
#else // GC2RVL_NO_ARAM
#include <revolution/os.h>
EXTERN void* L_malloc(ULONG _ul_size);
EXTERN void L_free(void *p);
#endif // GC2RVL_NO_ARAM

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    XBOX
 -----------------------------------------------------------------------------------------------------------------------
 */

#elif defined(_XBOX) || defined(_XENON)
#if !defined(_XENON)
#include <xtl.h>
#else
#include <Xbdm.h>
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif

extern HANDLE	g_hHeap;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#ifdef JADEFUSION
#if defined(_FINAL_)
// SC: With the removal of debug information and the of unused XMP LOD levels we can 
//     remove 8 MB of Jade memory (for binarized versions)
#define DM_MEM_SIZE     112*1024*1024
#else
#define DM_MEM_SIZE     120*1024*1024
#endif
#endif

#ifdef JADEFUSION
#define L_mallocsize(a)	HeapSize(g_hHeap, HEAP_GENERATE_EXCEPTIONS, a)
#endif
#define L_malloc(a)		HeapAlloc(g_hHeap, HEAP_GENERATE_EXCEPTIONS, a)
#define L_realloc(a, b) HeapRealloc(g_hHeap, HEAP_GENERATE_EXCEPTIONS, a, b)
#define L_free(a)		HeapFree(g_hHeap, HEAP_NO_SERIALIZE, a)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OTHERS
 -----------------------------------------------------------------------------------------------------------------------
 */

#else /* _XBOX */
#include "malloc.h"
#define L_malloc(a)		malloc(a)
#define L_realloc(a, b) realloc(a, b)
#define L_free(a)		free(a)

#endif
#endif /* __CLIMEM_H__ */ 
 