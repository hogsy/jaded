/*$T COLload.h GC!1.71 03/07/00 18:43:48 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __COL_LOAD__
#define __COL_LOAD__

/*$F GC Dependencies 11/22/99 */
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplus */

extern ULONG						COL_ul_CallBackLoadGameMaterial(ULONG);
extern ULONG                        COL_ul_CallBackLoadCob(ULONG);
extern ULONG                        COL_ul_CallBackLoadColMap(ULONG);
extern ULONG                        COL_ul_CallBackLoadColSet(ULONG);
extern ULONG                        COL_ul_CallBackLoadInstance(ULONG);
extern struct COL_tdst_ColMap_      *COL_pst_LoadColMap(BIG_KEY);
extern struct COL_tdst_Instance_    *COL_pst_LoadInstance(BIG_KEY);
extern void                         COL_ResolveRefs(OBJ_tdst_GameObject *_pst_GO);
extern void                         COL_OK3_Build(void *, BOOL , BOOL );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplcus */
#endif /* __COL_LOAD__ */
