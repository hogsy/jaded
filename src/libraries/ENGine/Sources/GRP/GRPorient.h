/*$T GRPorient.h GC!1.71 02/22/00 16:11:41 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GRPORIENT_H__
#define __GRPORIENT_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
void    GRP_SetAbsolutePosition(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void    GRP_SetAbsoluteInitialPosition(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void    GRP_AddAbsoluteVector(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void    GRP_AddAbsoluteInitialVector(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void    GRP_SetRelativePosition(OBJ_tdst_Group *, MATH_tdst_Vector *);
void    GRP_SetAbsoluteMatrix(OBJ_tdst_GameObject *, MATH_tdst_Matrix *);
void    GRP_SetAbsoluteInitialMatrix(OBJ_tdst_GameObject *, MATH_tdst_Matrix *);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GRPORIENT_H__ */
