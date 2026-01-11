/*$T AIsave.h GC!1.41 08/20/99 09:49:41 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __AISAVE_H__
#define __AISAVE_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "BASe/BAStypes.h"
#include "AIstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#ifdef ACTIVE_EDITORS
extern BIG_KEY  AI_ul_CreateInstance(WOR_tdst_World *, BIG_INDEX, BIG_INDEX, char *);
extern void     AI_SaveInstance(AI_tdst_Instance *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __AISAVE_H__ */
