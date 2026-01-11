/*$T OBJBoundingVolume.h GC!1.71 01/21/00 11:10:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifdef ODE_INSIDE
#include "MATHs/MATH.h"
#include "OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern BOOL OBJ_ODE_IsBox(OBJ_tdst_GameObject *);
//extern BOOL OBJ_ODE_IsSphere(void *);
//extern BOOL OBJ_ODE_IsPlane(void *);
//extern BOOL OBJ_ODE_IsCylinder(void *);
extern BOOL OBJ_HasNoODE(OBJ_tdst_GameObject *);
extern void OBJ_ODE_Move(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
extern void OBJ_ODE_Scale(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
extern void OBJ_ODE_Rotate(OBJ_tdst_GameObject *, MATH_tdst_Vector *, MATH_tdst_Vector *);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif //ODE_INSIDE
