/*$T AIload.h GC! 1.078 03/17/00 10:36:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __AILOAD_H__
#define __AILOAD_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "BASe/BAStypes.h"
#include "AIstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Prototypes
 -----------------------------------------------------------------------------------------------------------------------
 */

extern BOOL				AI_b_IsARef(int);

extern void				AI_ResolveFctRefSimple(AI_tdst_Node *);
extern void				AI_ResolveModelRef(AI_tdst_Model *);
extern void				AI_ResolveInstanceRef(AI_tdst_Instance *);
extern ULONG			AI_ul_ResolveOneRef(int, ULONG);
extern BOOL				AI_SpecialWParam(char);

extern ULONG			AI_ul_CallbackLoadFunction(ULONG);
extern ULONG			AI_ul_CallbackLoadVars(ULONG);
extern ULONG			AI_ul_CallbackLoadModel(ULONG);
extern ULONG			AI_ul_CallbackLoadInstance(ULONG);

extern void				AI_AllocateSaveBuffer(AI_tdst_Instance *);
extern AI_tdst_Instance *AI_pst_LoadInstance(BIG_KEY);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __AILOAD_H__ */
