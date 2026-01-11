/*$T SOFTMatrixStack.h GC!1.52 10/11/99 14:48:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __SOFTMATRIXSTACK_H__
#define __SOFTMATRIXSTACK_H__
#include "BASe/BAStypes.h"


#include "MATHs/MATH.h"

#ifndef PSX2_TARGET
#pragma once
#endif


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Structures
 ***************************************************************************************************
 */

#define MATRIX_STACK_DEPTH	16

typedef struct  SOFT_tdst_MatrixStack_
{
	MATH_tdst_Matrix    dst_Matrix[MATRIX_STACK_DEPTH];
	MATH_tdst_Matrix    *pst_CurrentMatrix;
    unsigned short      uw_NumberMaxOfMatrix;
    unsigned short      uw_IndexOfCurrentMatrix;
} SOFT_tdst_MatrixStack;

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */
LONG    SOFT_l_MatrixStack_Create(SOFT_tdst_MatrixStack *);
void    SOFT_MatrixStack_Destroy(SOFT_tdst_MatrixStack *);
void    SOFT_MatrixStack_Reset(SOFT_tdst_MatrixStack *, MATH_tdst_Matrix *);
LONG    SOFT_l_MatrixStack_Push(SOFT_tdst_MatrixStack *, MATH_tdst_Matrix *);
LONG    SOFT_l_MatrixStack_Pop(SOFT_tdst_MatrixStack *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTMATRIXSTACK_H */
