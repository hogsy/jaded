/*$T COLsave.h GC!1.59 12/22/99 10:23:25 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __COL_SAVE__
#define __COL_SAVE__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplus */

#include "BIGfiles/SAVing/SAVdefs.h"/* BIG_KEY */

BIG_KEY COL_ul_CreateInstanceFile(struct WOR_tdst_World_ *, BIG_INDEX *, char *, char *);
BIG_KEY COL_ul_CreateColMapFile(struct WOR_tdst_World_ *, BIG_INDEX, char *);
BIG_KEY COL_ul_CreateCobFile(struct WOR_tdst_World_ *, char *);

void    COL_SaveInstance(struct COL_tdst_Instance_ *, BIG_KEY);
void    COL_SaveColSet(struct COL_tdst_ColSet_ *, BIG_KEY);
void    COL_SaveColMap(struct COL_tdst_ColMap_ *, BIG_KEY);
void    COL_SaveCob(struct COL_tdst_Cob_ *, BIG_KEY);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplcus */
#endif /* __COL_SAVE__ */
