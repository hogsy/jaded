/*$T COLset.h GC!1.65 12/30/99 12:22:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __COL_SET__
#define __COL_SET__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplus */

#include "MATHs/MATH.h"

#ifdef ACTIVE_EDITORS
void    COL_ColSet_AddSphere(struct COL_tdst_ZDx_ *, struct COL_tdst_ZDx_ *, MATH_tdst_Vector *);
void    COL_ColSet_AddBox(struct COL_tdst_ZDx_ *, struct COL_tdst_ZDx_ *, MATH_tdst_Vector *);
void    COL_ColSet_RemoveZDxWithIndex(struct COL_tdst_ColSet_ *, UCHAR);
void    COL_ColSet_AddZDx(struct COL_tdst_ColSet_ *, USHORT, void *, void *);
void    COL_ColSet_RemoveInstance(struct COL_tdst_ColSet_ *, struct COL_tdst_Instance_ *);
#endif /* ACTIVE_EDITORS */

void    COL_ColSet_AddInstance(struct COL_tdst_ColSet_ *, struct COL_tdst_Instance_ *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplcus */
#endif /* __COL_SET__ */
