/*$T COLmain.h GC! 1.081 02/07/01 10:33:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __COL_H__
#define __COL_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$F GC Dependencies 11/22/99 */
#include "BASe/BAStypes.h"

void				COL_MainCall(struct WOR_tdst_World_ *);
BOOL				COL_b_SingleBVOverlap(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *);
BOOL				COL_b_BVOverlap(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *, BOOL, BOOL, BOOL);

BOOL				COL_b_BVIn(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *);

ULONG				COL_ListCreate
					(
						struct OBJ_tdst_GameObject_ *,
						ULONG,
						ULONG,
						ULONG,
						UCHAR,
						struct WOR_tdst_World_ *,
						ULONG *,
						UCHAR
					);

ULONG				COL_ListCreateDistance
					(
						ULONG,				
						ULONG,
						ULONG,
						struct WOR_tdst_World_ *,
						ULONG *,
						MATH_tdst_Vector *,
						float,
						ULONG
					);

BOOL				COL_ZDE_ZDECollide(struct OBJ_tdst_GameObject_ *, UCHAR, UCHAR, struct OBJ_tdst_GameObject_ *);
BOOL				COL_ZDE_BVCollide(struct OBJ_tdst_GameObject_ *, UCHAR, struct OBJ_tdst_GameObject_ *, BOOL);
BOOL				COL_ZDE_PivotCollide(struct OBJ_tdst_GameObject_ *, UCHAR, struct OBJ_tdst_GameObject_ *, BOOL);
BOOL				COL_BV_PivotCollide(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *, BOOL);
BOOL				COL_ZDE_BVCenterCollide(struct OBJ_tdst_GameObject_ *, UCHAR, struct OBJ_tdst_GameObject_ *, BOOL);

OBJ_tdst_GameObject *COL_GetFirstObjectInRange
					(
						struct WOR_tdst_World_ *,
						MATH_tdst_Vector *,
						float,
						ULONG,
						ULONG,
						ULONG
					);

ULONG				COL_VisibleObjectListCreate(struct WOR_tdst_World_ *, ULONG, ULONG, ULONG, ULONG *);
void				COL_RecomputeAllVisualOK3(struct WOR_tdst_World_ *);


#ifdef ODE_INSIDE
void				COL_ODEPrimitive_Callback(void *data, dGeomID o1, dGeomID o2);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __COL_H__ */
