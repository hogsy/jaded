/*$T WAYsave.h GC!1.41 08/20/99 10:59:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif


#ifndef __WAYSAVE_H__
#define __WAYSAVE_H__

#include "ENGine/Sources/WAYs/WAYstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void		WAY_SaveOneNetwork(WOR_tdst_World *, WAY_tdst_Network *);
extern void		WAY_SaveWorldNetworks(WOR_tdst_World *);
extern ULONG    WAY_ul_SaveStruct(OBJ_tdst_GameObject *, WAY_tdst_Struct *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WAYSAVE_H__ */
