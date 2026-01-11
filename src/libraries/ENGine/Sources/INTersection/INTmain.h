/*$T INTmain.h GC! 1.081 02/13/01 14:46:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __INT_MAIN__
#define __INT_MAIN__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "INTstruct.h"

BOOL	INT_SphereSphere(MATH_tdst_Vector *, float, MATH_tdst_Vector *, float);
BOOL	INT_SphereBox(INT_tdst_Box *, INT_tdst_Sphere *);
BOOL	INT_SphereAABBox(MATH_tdst_Vector *, float, MATH_tdst_Vector *, MATH_tdst_Vector *);

BOOL	INT_PointSphere(MATH_tdst_Vector *, float, MATH_tdst_Vector *);
BOOL	INT_PointAABBox(MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Matrix *);

BOOL	INT_FullSphereSphere
		(
			MATH_tdst_Vector *,
			float,
			MATH_tdst_Vector *,
			float,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *
		);

BOOL	INT_FullRayAABBox(MATH_tdst_Vector *, MATH_tdst_Vector *, INT_tdst_Box *, MATH_tdst_Vector *, float, float *);
BOOL	INT_FullRaySphere
		(
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			INT_tdst_Sphere *,
			MATH_tdst_Vector *,
			float,
			float *
		);
BOOL	INT_FullSegmentTriangle
		(
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			UCHAR *
		);
BOOL	INT_FullSphereTriangle
		(
			MATH_tdst_Vector *,
			float,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			ULONG *
		);

BOOL	INT_FullRayTriangle
		(
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			float *,
			MATH_tdst_Vector *,
			BOOL
		);

BOOL	INT_HighLevel_OBBoxOBBox
		(
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Matrix *,
			MATH_tdst_Vector *
		);

BOOL	INT_SphereOBBox(MATH_tdst_Vector *, float, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *);

BOOL	INT_FullSphereOBBox
		(
			MATH_tdst_Vector *,
			float,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			ULONG *
		);

BOOL	INT_FullSphereCylinder
		(
			MATH_tdst_Vector *,
			float,
			MATH_tdst_Vector *,
			float,
			float,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			ULONG *
		);

BOOL	INT_DynamicHit
		(
			MATH_tdst_Vector *,
			float,
			MATH_tdst_Vector *,
			float,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			ULONG *
		);

BOOL	INT_b_FullRayCylinder
		(
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			MATH_tdst_Vector *,
			float,
			float,
			float,
			float *,
			MATH_tdst_Vector *
		);

BOOL INT_FullTriangleTriangle
		(
			MATH_tdst_Vector	*, 
			MATH_tdst_Vector	*, 
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*, 
			MATH_tdst_Vector	*, 
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*
		);

BOOL INT_FullAABBoxTriangle
		(
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*,
			MATH_tdst_Vector	*,
			ULONG				*,
			ULONG				*
		);

BOOL INT_AABBoxAABBox( MATH_tdst_Vector	*_pst_MinBox1,
                      MATH_tdst_Vector	*_pst_MaxBox1,
                      MATH_tdst_Vector	*_pst_MinBox2,
                      MATH_tdst_Vector	*_pst_MaxBox2 );

BOOL INT_SegmentSphere( MATH_tdst_Vector	*_pst_A,
                        MATH_tdst_Vector	*_pst_B,
                        MATH_tdst_Vector	*_pst_Center,
                        float				_f_Radius,
                        MATH_tdst_Vector	*_pst_Hit );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __INT_MAIN__ */
