/*$T COLactor.h GC!1.5 11/22/99 15:04:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __COL_ACTOR__
#define __COL_ACTOR__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplcus */

// GC Dependencies 11/22/99
#include "COLvars.h"


BOOL    COL_Actor_Actor(COL_tdst_GlobalVars *);
BOOL    COL_Actor_Decor(COL_tdst_GlobalVars *);
BOOL    COL_Actor_GameObject(COL_tdst_GlobalVars *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* _cplusplcus */

#endif /* __COL_ACTOR_ */
