/*$T INTinit.h GC!1.5 10/11/99 10:02:34 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Init functions of the Intersection module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __INT_INIT__
#define __INT_INIT__
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "INTstruct.h"

USHORT  INT_SnP_GetFirstFreeIndex(INT_tdst_SnP *);
void    INT_SnP_Alloc(struct WOR_tdst_World_ *);
void    INT_SnP_DesAlloc(struct WOR_tdst_World_ *);
void    INT_SnP_AddBox
        (
            INT_tdst_SnP *,
            OBJ_tdst_GameObject *,
            MATH_tdst_Vector *,
            MATH_tdst_Vector *
        );

void    INT_SnP_AddSphere(INT_tdst_SnP *, OBJ_tdst_GameObject *, MATH_tdst_Vector *, float);

void    INT_SnP_Reinit(struct WOR_tdst_World_ *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __INT_INIT__ */
