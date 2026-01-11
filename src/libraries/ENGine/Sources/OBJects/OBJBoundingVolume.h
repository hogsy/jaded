/*$T OBJBoundingVolume.h GC!1.71 01/21/00 11:10:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __OBJ_BV__
#define __OBJ_BV__

#include "MATHs/MATH.h"
#include "OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern BOOL OBJ_BV_IsSphere(void *);
extern BOOL OBJ_BV_IsAABBox(void *);
extern BOOL OBJ_HasNoBV(OBJ_tdst_GameObject *);
extern void OBJ_BV_Move(OBJ_tdst_GameObject *, MATH_tdst_Vector *, BOOL);
extern void OBJ_BV_OBBoxMove(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
extern void OBJ_BV_ComputeCenter(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
extern void OBJ_ComputeAABBoxFromOBBox(OBJ_tdst_GameObject *);
extern void OBJ_ComputeSphereFromOBBox(OBJ_tdst_GameObject *);
extern void OBJ_ComputeGeometricObjectBV(OBJ_tdst_GameObject *, BOOL);
extern void OBJ_ComputeBV(OBJ_tdst_GameObject *, BOOL, BOOL);
extern void OBJ_ReComputeRealTimeBVs(TAB_tdst_PFtable *);
extern void OBJ_DuplicateBV(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float OBJ_f_BV_GetRadius(void *_pst_BV)
{
    return(((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.x);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void OBJ_v_BV_SetRadius(void *_pst_BV, float f_Radius)
{
    ((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.x = f_Radius;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_BV_GetCenter(void *_pst_BV)
{
    return(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMin);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_BV_GetGMax(void *_pst_BV)
{
    return(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_BV_GetGMin(void *_pst_BV)
{
    return(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMin);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_BV_GetGMaxInit(void *_pst_BV)
{
    return(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMaxIn);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_BV_GetGMinInit(void *_pst_BV)
{
    return(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMinIn);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_BV_GetLMax(void *_pst_BV)
{
    return(&((OBJ_tdst_ComplexBV *) _pst_BV)->st_LMax);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_BV_GetLMin(void *_pst_BV)
{
    return(&((OBJ_tdst_ComplexBV *) _pst_BV)->st_LMin);
}

#ifdef ACTIVE_EDITORS
void    OBJ_CheckAllBVs(struct TAB_tdst_PFtable_ *, ULONG);
BOOL    OBJ_CheckOneBV(OBJ_tdst_GameObject *, ULONG);
void    OBJ_ReComputeAllBVs(struct TAB_tdst_PFtable_ *);
void    OBJ_HideAllBVs(struct TAB_tdst_PFtable_ *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* ___OBJ_BV__ */
