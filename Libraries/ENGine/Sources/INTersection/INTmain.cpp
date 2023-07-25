/*$T INTmain.c GC! 1.081 07/30/02 15:16:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "INTstruct.h"
#include "COLlision/COLconst.h"
#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"


#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "MATHs/MATH.h"
#include "LINks/LINKmsg.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/EOT/EOT.h"

#include "TIMer/TIMdefs.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "SOFT/SOFTHelper.h"
#include "BIGfiles/BIGread.h"
#include "EDIpaths.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"


#ifdef ACTIVE_EDITORS
#include "SDK/Sources/LINKs/LINKmsg.h"
#endif

/*$F
					------------------
					--   Functions  --
					------------------

INT_PointSphere
INT_PointAABBox
INT_AABBoxAABBox 
INT_SphereSphere 
INT_SphereBox

INT_RaySphere			// Haines's method 
INT_RayOBBox			// Haines's method 
INT_RayAABBox			// Haines's method 

INT_FullRaySphere		// Haines's method
INT_FullRayAABBox		// Woo's method 

INT_FullRayTriangle		// Möller & Trumbore's Method

INT_PointInTriangle
INT_FullSegmentTriangle

*/
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INT_PointSphere(MATH_tdst_Vector *pst_Center, float _f_Radius, MATH_tdst_Vector *_pst_Point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	stDif;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&stDif, pst_Center, _pst_Point);

	return(fInf(MATH_f_SqrVector(&stDif), _f_Radius * _f_Radius));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INT_PointAABBox
(
	MATH_tdst_Vector	*_pst_GMin,
	MATH_tdst_Vector	*_pst_GMax,
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Matrix	*_pst_AbsoluteMatrix
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_AbsolutePoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* First, we transform the point in the global coordinate system. */
	MATH_TransformVector(&st_AbsolutePoint, _pst_AbsoluteMatrix, _pst_Point);

	return
		(
			fSup (_pst_GMin->x, st_AbsolutePoint.x)
		||	fInf (_pst_GMax->x, st_AbsolutePoint.x)
		||	fSup (_pst_GMin->y, st_AbsolutePoint.y)
		||	fInf (_pst_GMax->y, st_AbsolutePoint.y)
		||	fSup (_pst_GMin->z, st_AbsolutePoint.z)
		||	fInf (_pst_GMax->z, st_AbsolutePoint.z)
		) ? FALSE : TRUE;
}

/*
 =======================================================================================================================
    Aim:    Detects whether 2 AABBoxes are colliding.The Boxes are assumed to be in the same coordinate system. TAKE
            CARE, don't forget that this function must be re-arranged if you want to deal with the bounding AABBoxes,
            because the function does not take care of an hypothetic translation of the boxes which is the case for the
            bounding AABBoxes!
 =======================================================================================================================
 */
BOOL INT_AABBoxAABBox(INT_tdst_Box *_pst_Box1, INT_tdst_Box *_pst_Box2)
{
	return
		(
			fSup (_pst_Box1->st_Min.x, _pst_Box2->st_Max.x)
		||	fInf (_pst_Box1->st_Max.x, _pst_Box2->st_Min.x)
		||	fSup (_pst_Box1->st_Min.y, _pst_Box2->st_Max.y)
		||	fInf (_pst_Box1->st_Max.y, _pst_Box2->st_Min.y)
		||	fSup (_pst_Box1->st_Min.z, _pst_Box2->st_Max.z)
		||	fInf (_pst_Box1->st_Max.z, _pst_Box2->st_Min.z)
		) ? FALSE : TRUE;
}

#ifdef JADEFUSION
/*
 =======================================================================================================================
    Aim:    Detects whether 2 AABBoxes are colliding.The Boxes are assumed to be in the same coordinate system. TAKE
            CARE, don't forget that this function must be re-arranged if you want to deal with the bounding AABBoxes,
            because the function does not take care of an hypothetic translation of the boxes which is the case for the
            bounding AABBoxes!
 =======================================================================================================================
 */
BOOL INT_AABBoxAABBox( MATH_tdst_Vector	*_pst_MinBox1,
                       MATH_tdst_Vector	*_pst_MaxBox1,
                       MATH_tdst_Vector	*_pst_MinBox2,
                       MATH_tdst_Vector	*_pst_MaxBox2 )
                      
{
    INT_tdst_Box box1;
    INT_tdst_Box box2;

    box1.st_Min = *_pst_MinBox1;
    box1.st_Max = *_pst_MaxBox1;
    box2.st_Min = *_pst_MinBox2;
    box2.st_Max = *_pst_MaxBox2;

	return INT_AABBoxAABBox( &box1, &box2 );
}
#endif

/*
 =======================================================================================================================
    Aim:    Detects whether 2 spheres are colliding
 =======================================================================================================================
 */
extern "C" BOOL INT_SphereSphere( MATH_tdst_Vector *_pst_Center1, float f_Radius1, MATH_tdst_Vector *_pst_Center2, float f_Radius2 )
{
	return INT_PointSphere(_pst_Center1, f_Radius1 + f_Radius2, _pst_Center2);
}

/*$F
 =======================================================================================================================
    Aim:    Detects whether 2 spheres are colliding and returns colliding info.

    Note:   Do NOT deal with hypothetic scaled spheres.
			1 is the ZDx
			2 is the Cob
 =======================================================================================================================
 */

extern "C" BOOL INT_FullSphereSphere(
	MATH_tdst_Vector	*_pst_Center1,
	float				_f_Radius1,
	MATH_tdst_Vector	*_pst_Center2,
	float				_f_Radius2,
	MATH_tdst_Vector	*_pst_Normal,
	MATH_tdst_Vector	*_pst_Hit	/* Hit point */
)
{
	if(!INT_SphereSphere(_pst_Center1, _f_Radius1, _pst_Center2, _f_Radius2))
		return FALSE;
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Unit;
		MATH_tdst_Vector	st_CobToZDM;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_SubVector(&st_CobToZDM, _pst_Center1, _pst_Center2);
		MATH_NormalizeVector(&st_Unit, &st_CobToZDM);
		MATH_MulVector(_pst_Hit, &st_Unit, _f_Radius2);
		MATH_AddEqualVector(_pst_Hit, _pst_Center2);
		MATH_CopyVector(_pst_Normal, &st_Unit);
		return TRUE;
	}
}

/*
 =======================================================================================================================
    Aim:    Detects whether the sphere collides the AABBox.
 =======================================================================================================================
 */
extern "C" BOOL INT_SphereAABBox(
	MATH_tdst_Vector	*_pst_Center,
	float				f_Radius,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max
)
{
	/*~~~~~~~~~~~*/
#ifdef PSX2_TARGET
	ULONG	TAB[6];
	/*~~~~~~~~~~~*/

	*(float *) &TAB[0] = _pst_Center->x - (_pst_Min->x - f_Radius);
	*(float *) &TAB[1] = _pst_Center->y - (_pst_Min->y - f_Radius);
	*(float *) &TAB[2] = _pst_Center->z - (_pst_Min->z - f_Radius);
	*(float *) &TAB[3] = (_pst_Max->x + f_Radius) - _pst_Center->x;
	*(float *) &TAB[4] = (_pst_Max->y + f_Radius) - _pst_Center->y;
	*(float *) &TAB[5] = (_pst_Max->z + f_Radius) - _pst_Center->z;
	return(((TAB[0] | TAB[1] | TAB[2] | TAB[3] | TAB[4] | TAB[5]) & 0x80000000) ^ 0x80000000);

	/*~~~~~~~~~~~~~~~~~*/
#else
	float	f_Min, f_Max;
	/*~~~~~~~~~~~~~~~~~*/

	/* X rejection */
	if(fSup(_pst_Center->x, _pst_Max->x))
	{
		f_Min = _pst_Max->x;
		f_Max = _pst_Center->x;
		if(fSup(f_Max - f_Min, f_Radius)) return FALSE;
	}
	else
	{
		if(fInf(_pst_Center->x, _pst_Min->x))
		{
			f_Max = _pst_Min->x;
			f_Min = _pst_Center->x;
			if(fSup(f_Max - f_Min, f_Radius)) return FALSE;
		}
	}

	/* Y rejection */
	if(fSup(_pst_Center->y, _pst_Max->y))
	{
		f_Min = _pst_Max->y;
		f_Max = _pst_Center->y;
		if(fSup(f_Max - f_Min, f_Radius)) return FALSE;
	}
	else
	{
		if(fInf(_pst_Center->y, _pst_Min->y))
		{
			f_Max = _pst_Min->y;
			f_Min = _pst_Center->y;
			if(fSup(f_Max - f_Min, f_Radius)) return FALSE;
		}
	}

	/* Z rejection */
	if(fSup(_pst_Center->z, _pst_Max->z))
	{
		f_Min = _pst_Max->z;
		f_Max = _pst_Center->z;
		if(fSup(f_Max - f_Min, f_Radius)) return FALSE;
	}
	else
	{
		if(fInf(_pst_Center->z, _pst_Min->z))
		{
			f_Max = _pst_Min->z;
			f_Min = _pst_Center->z;
			if(fSup(f_Max - f_Min, f_Radius)) return FALSE;
		}
	}

	return TRUE;
#endif
}

/*
 =======================================================================================================================
    Aim:    Detects whether the Ray collides the sphere. If you want the get the distance of the collided point and the
            collided point itself, use the INT_FullRaySphere function.
 =======================================================================================================================
 */
BOOL INT_RaySphere(MATH_tdst_Vector *_pst_Origin, MATH_tdst_Vector *_pst_Direction, INT_tdst_Sphere *_pst_Sphere)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectOC;	/* O: Origin of the Ray; C: Center of the Sphere */
	float				f_SqrNormOC, f_NormOA, f_SqrNormOA;
	float				f_SqrRadius, f_SqrNormCA;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_VectOC, &_pst_Sphere->st_Center, _pst_Origin);

	/* A: Projection of C onto the Direction of the Ray */
	f_NormOA = MATH_f_DotProduct(&st_VectOC, _pst_Direction);
	f_SqrNormOA = fSqr(f_NormOA);
	f_SqrNormOC = MATH_f_SqrNormVector(&st_VectOC);
	f_SqrRadius = fSqr(_pst_Sphere->f_Radius);

	/* If the origin of the ray is inside the sphere, there is collision. */
	if(fSup(f_SqrRadius, f_SqrNormOC)) return TRUE;

	/*
	 * The sphere is behind the ray AND the origin is not in the Sphere, there cannot
	 * be any collision
	 */
	if((f_NormOA < 0) && fSup(f_SqrNormOC, f_SqrRadius)) return FALSE;

	/* Pythagorean theorem */
	f_SqrNormCA = fSub(f_SqrNormOC, f_SqrNormOA);

	/* If the Collision point is out of the sphere, there is no collision */
	if(fSup(f_SqrNormCA, f_SqrRadius))
		return FALSE;
	else
		return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Detects whether the Ray collides the sphere. This function also compute the distance of the origin of the
            Ray and the collided point and the exact position of the collided point.

    Note:   !!! WORKS !!!
 =======================================================================================================================
 */
extern "C" BOOL INT_FullRaySphere(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	INT_tdst_Sphere		*_pst_Sphere,
	MATH_tdst_Vector	*_pst_CollidedPoint,	/* Collision point */
	float				f_MaxDist,				/* Max acceptable distance. Beyond, we reject */
	float				*_f_Dist				/* Distance between the origin of the ray and the Collision point */
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectOC;	/* O: Origin of the Ray; C: Center of the Sphere */
	float				f_SqrNormOC, f_NormOA, f_SqrNormOA;
	float				f_SqrRadius, f_SqrNormCA, f_NormAM;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_VectOC, &_pst_Sphere->st_Center, _pst_Origin);

	/* A: Projection of C onto the Direction of the Ray */
	f_NormOA = MATH_f_DotProduct(&st_VectOC, _pst_Direction);
	f_SqrNormOA = fSqr(f_NormOA);
	f_SqrNormOC = MATH_f_SqrNormVector(&st_VectOC);
	f_SqrRadius = fSqr(_pst_Sphere->f_Radius);

	/* If the origin of the ray is inside the sphere, there is collision. */
	if(fSup(f_SqrRadius, f_SqrNormOC))
	{
		*_f_Dist = -1.0f;
		return TRUE;
	}

	/*
	 * The sphere is behind the ray AND the origin is not in the Sphere, there cannot
	 * be any collision
	 */
	if((f_NormOA < 0) && fSup(f_SqrNormOC, f_SqrRadius)) return FALSE;

	/* Pythagorean theorem */
	f_SqrNormCA = fSub(f_SqrNormOC, f_SqrNormOA);

	/* If the Collision point is out of the sphere, there is no collision */
	if(fSup(f_SqrNormCA, f_SqrRadius)) return FALSE;

	/* M: Intersection between the ray and the sphere */
	f_NormAM = fSqrt(fSub(f_SqrRadius, f_SqrNormCA));

	if(fSup(f_SqrNormOC, f_SqrRadius))
		*_f_Dist = f_NormOA - f_NormAM;
	else
		*_f_Dist = f_NormOA - f_NormAM;

	if(fSup(*_f_Dist, f_MaxDist)) return FALSE;

	/* We may not want the collided point info. */
	if(_pst_CollidedPoint)
	{
		MATH_MulVector(_pst_CollidedPoint, _pst_Direction, *_f_Dist);
		MATH_AddVector(_pst_CollidedPoint, _pst_Origin, _pst_CollidedPoint);
	}

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    This function computes the 2 distances (T1 et T2) between the origin of the Ray and a slab.(A slab is
            simply 2 parallel planes. A box is actually defined by 3 slabs). It also compares those values to the TMin
            and TMax to finally find

    Note:   For additional information, look in the Real Time Rendering book, p 301.
 =======================================================================================================================
 */
BOOL INT_OBBSlabComputation
(
	MATH_tdst_Vector	*_pst_Direction,
	MATH_tdst_Vector	*_pst_VectOC,
	MATH_tdst_Vector	*_pst_HalfU,
	float				*_pf_TMax,
	float				*_pf_TMin
)
{
	/*~~~~~~~~~~~~~~~~~~*/
	float	T1, T2, TTemp;
	float	e, f, N;
	/*~~~~~~~~~~~~~~~~~~*/

	e = MATH_f_DotProduct(_pst_HalfU, _pst_VectOC);
	f = MATH_f_DotProduct(_pst_HalfU, _pst_Direction);
	N = MATH_f_SqrNormVector(_pst_HalfU);

	if(fSup(f, 1E-9f))
	{
		/* The Mul with 1/f must be faster ... */
		T1 = fDiv(fAdd(e, N), f);
		T2 = fDiv(fSub(e, N), f);

		/* We want T1 ([T2]) to contain the max ([min]) of (T1, T2) */
		if(fInf(T1, T2))
		{
			TTemp = T1;
			T1 = T2;
			T2 = TTemp;
		}

		*_pf_TMax = (T1 > *_pf_TMax) ? T1 : *_pf_TMax;
		*_pf_TMin = (T2 < *_pf_TMin) ? T2 : *_pf_TMin;

		/* What a pity, the ray missed the box ! */
		if(fSup(*_pf_TMin, *_pf_TMax)) return FALSE;

		/* If the Box is "behind" the Ray, we return ... */
		if(fInfZero(*_pf_TMax)) return FALSE;
	}

	/*
	 * The following TRUE does NOT mean that the Ray collides the Box. We could just
	 * not conclude yet. The Algorithm will go on.
	 */
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Detects whether the ray collides the Oriented box The Algo seems to be quite good but it must receive the
            center of the box and the 3 Half Length vectors which need to be computed before. If you want to get the
            distance between the origin of the ray and the collided point, and the exact position of the collided
            point, use the INT_FullRayOBBox.

    Note:   Scheme presented by Haines // Real Time Rendering p 300.
 =======================================================================================================================
 */
BOOL INT_RayOBBox
(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	MATH_tdst_Vector	*_pst_BoxCenter,
	MATH_tdst_Vector	*_pst_HalfU,
	MATH_tdst_Vector	*_pst_HalfV,
	MATH_tdst_Vector	*_pst_HalfW
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectOC;	/* O: Origin of the Ray and C: Center of the Box; */
	float				f_TMin, f_TMax;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_TMin = Cf_Infinit;
	f_TMax = Cf_Zero;
	MATH_SubVector(&st_VectOC, _pst_BoxCenter, _pst_Direction);

	if(!(INT_OBBSlabComputation(_pst_Direction, &st_VectOC, _pst_HalfU, &f_TMin, &f_TMax))) return FALSE;
	if(!(INT_OBBSlabComputation(_pst_Direction, &st_VectOC, _pst_HalfV, &f_TMin, &f_TMax))) return FALSE;
	if(!(INT_OBBSlabComputation(_pst_Direction, &st_VectOC, _pst_HalfW, &f_TMin, &f_TMax))) return FALSE;

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Detects whether the Ray collided the Oriented Box. We also compute the distance between the Origin of the
            Ray and the collided point, and the exact position of the collided point.

    Note:   Scheme presented by Haines // Real Time Rendering p.300
 =======================================================================================================================
 */
BOOL INT_FullRayOBBox
(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	MATH_tdst_Vector	*_pst_BoxCenter,
	MATH_tdst_Vector	*_pst_HalfU,
	MATH_tdst_Vector	*_pst_HalfV,
	MATH_tdst_Vector	*_pst_HalfW,
	float				*_f_Dist,
	MATH_tdst_Vector	*_pst_CollidedPoint
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectOC;	/* O: Origin of the Ray and C: Center of the Box; */
	float				f_TMin, f_TMax;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_TMin = Cf_Infinit;
	f_TMax = Cf_Zero;

	MATH_SubVector(&st_VectOC, _pst_BoxCenter, _pst_Direction);

	if(!(INT_OBBSlabComputation(_pst_Direction, &st_VectOC, _pst_HalfU, &f_TMin, &f_TMax))) return FALSE;
	if(!(INT_OBBSlabComputation(_pst_Direction, &st_VectOC, _pst_HalfV, &f_TMin, &f_TMax))) return FALSE;
	if(!(INT_OBBSlabComputation(_pst_Direction, &st_VectOC, _pst_HalfW, &f_TMin, &f_TMax))) return FALSE;

	if(fSupZero(f_TMin))
		*_f_Dist = f_TMin;
	else
		*_f_Dist = f_TMax;

	MATH_MulVector(_pst_CollidedPoint, _pst_Direction, *_f_Dist);
	MATH_AddVector(_pst_CollidedPoint, _pst_Origin, _pst_CollidedPoint);
	return TRUE;
}

/*
 =======================================================================================================================
    Note:   Algo by Andrew Woo. Graphic Gems I (p.736)
 =======================================================================================================================
 */
BOOL INT_FullRayAABBox2
(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	INT_tdst_Box		*_pst_AABB,
	MATH_tdst_Vector	*_pst_CollidedPoint,
	float				f_MaxDist,
	float				*pf_Dist
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_ClosestPlane;
	float					f_TMax, f_TTemp;
	MATH_tdst_Vector		*pst_Min, *pst_Max;
	MATH_tdst_Vector		st_CollidedPoint;
	ULONG					Flag;
	float					X, Y, Z;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Min = &_pst_AABB->st_Min;
	pst_Max = &_pst_AABB->st_Max;
	
	Flag = 0;

	Flag += (_pst_Origin->x < pst_Min->x);
	if((Flag & 0x1) && (_pst_Direction->x < 0.0f)) return FALSE;
	Flag += (_pst_Origin->x > pst_Max->x) << 1;
	if((Flag & 0x2) && (_pst_Direction->x > 0.0f)) return FALSE;
	Flag += (_pst_Origin->y < pst_Min->y) << 2;
	if((Flag & 0x4) && (_pst_Direction->y < 0.0f)) return FALSE;
	Flag += (_pst_Origin->y > pst_Max->y) << 3;
	if((Flag & 0x8) && (_pst_Direction->y > 0.0f)) return FALSE;
	Flag += (_pst_Origin->z < pst_Min->z) << 4;
	if((Flag & 0x10) && (_pst_Direction->z < 0.0f)) return FALSE;
	Flag += (_pst_Origin->z > pst_Max->z) << 5;
	if((Flag & 0x20) && (_pst_Direction->z > 0.0f)) return FALSE;

	if(!Flag)
	{
		if(_pst_CollidedPoint)
			*_pst_CollidedPoint = *_pst_Origin;
		if(pf_Dist) *pf_Dist = -1.0f;

		return TRUE;
	}

	X = (Flag & 0x1) ? pst_Min->x : pst_Max->x;
	Y = (Flag & 0x4) ? pst_Min->y : pst_Max->y;
	Z = (Flag & 0x10) ? pst_Min->z : pst_Max->z;


	f_TMax = -1.0;

	/* Axis X */
	if((Flag & 0x3) && (_pst_Direction->x != 0.0f))
	{
		f_TMax = (X - _pst_Origin->x) / _pst_Direction->x;
		ul_ClosestPlane = INT_Cul_AxisX;
	}

	/* Axis Y */
	if((Flag & 0xC) && (_pst_Direction->y != 0.0f))
	{
		f_TTemp = (Y - _pst_Origin->y ) / _pst_Direction->y;
		if(f_TTemp > f_TMax)
		{
			f_TMax = f_TTemp;
			ul_ClosestPlane = INT_Cul_AxisY;
		}
	}

	/* Axis Z */
	if((Flag & 0x30) && (_pst_Direction->z != 0.0f))
	{
		f_TTemp = (Z - _pst_Origin->z) / _pst_Direction->z;
		if(f_TTemp > f_TMax)
		{
			f_TMax = f_TTemp;
			ul_ClosestPlane = INT_Cul_AxisZ;
		}
	}

	if((f_TMax < 0.0f) || (f_TMax > f_MaxDist)) return FALSE;

	if(pf_Dist) *pf_Dist = f_TMax;

	if(!_pst_CollidedPoint)
		_pst_CollidedPoint = &st_CollidedPoint;

	if(ul_ClosestPlane != INT_Cul_AxisX)
	{
		_pst_CollidedPoint->x = _pst_Origin->x + f_TMax * _pst_Direction->x;
		if((_pst_CollidedPoint->x < pst_Min->x) || (_pst_CollidedPoint->x > pst_Max->x)) return FALSE;
	}
	else
		_pst_CollidedPoint->x = X;

	if(ul_ClosestPlane != INT_Cul_AxisY)
	{
		_pst_CollidedPoint->y = _pst_Origin->y + f_TMax * _pst_Direction->y;
		if((_pst_CollidedPoint->y < pst_Min->y) || (_pst_CollidedPoint->y > pst_Max->y)) return FALSE;
	}
	else
		_pst_CollidedPoint->y = Y;

	if(ul_ClosestPlane != INT_Cul_AxisZ)
	{
		_pst_CollidedPoint->z = _pst_Origin->z + f_TMax * _pst_Direction->z;
		if((_pst_CollidedPoint->z < pst_Min->z) || (_pst_CollidedPoint->z > pst_Max->z)) return FALSE;
	}
	else
		_pst_CollidedPoint->x = Z;


	return TRUE;
}

extern "C" BOOL INT_FullRayAABBox(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	INT_tdst_Box		*_pst_AABB,
	MATH_tdst_Vector	*_pst_CollidedPoint,
	float				f_MaxDist,
	float				*pf_Dist
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Inside;
	ULONG				ul_Quadrant[3], ul_ClosestPlane;
	float				f_Plane[3], f_TMax, f_TTemp;
	MATH_tdst_Vector	st_Min, st_Max;
	MATH_tdst_Vector	st_CollidedPoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);

	/* Init, must be a negative value < -1.0 */
	f_TMax = -2.0;

	st_Min = _pst_AABB->st_Min;
	st_Max = _pst_AABB->st_Max;

	b_Inside = TRUE;

	/*
	 * We first find the candidate planes. The following tests can be avoided if rays
	 * cast all from the eye.
	 */

	/* Unrolling Loop ... Axis X */
	if(fInf(_pst_Origin->x, st_Min.x))
	{
		ul_Quadrant[INT_Cul_AxisX] = INT_Cul_LEFT;
		f_Plane[INT_Cul_AxisX] = st_Min.x;
		b_Inside = FALSE;
	}
	else
	{
		if(fSup(_pst_Origin->x, st_Max.x))
		{
			ul_Quadrant[INT_Cul_AxisX] = INT_Cul_RIGHT;
			f_Plane[INT_Cul_AxisX] = st_Max.x;
			b_Inside = FALSE;
		}
		else
			ul_Quadrant[INT_Cul_AxisX] = INT_Cul_MIDDLE;
	}

	/* Axis Y */
	if(fInf(_pst_Origin->y, st_Min.y))
	{
		ul_Quadrant[INT_Cul_AxisY] = INT_Cul_LEFT;
		f_Plane[INT_Cul_AxisY] = st_Min.y;
		b_Inside = FALSE;
	}
	else
	{
		if(fSup(_pst_Origin->y, st_Max.y))
		{
			ul_Quadrant[INT_Cul_AxisY] = INT_Cul_RIGHT;
			f_Plane[INT_Cul_AxisY] = st_Max.y;
			b_Inside = FALSE;
		}
		else
			ul_Quadrant[INT_Cul_AxisY] = INT_Cul_MIDDLE;
	}

	/* Axis Z */
	if(fInf(_pst_Origin->z, st_Min.z))
	{
		ul_Quadrant[INT_Cul_AxisZ] = INT_Cul_LEFT;
		f_Plane[INT_Cul_AxisZ] = st_Min.z;
		b_Inside = FALSE;
	}
	else
	{
		if(fSup(_pst_Origin->z, st_Max.z))
		{
			ul_Quadrant[INT_Cul_AxisZ] = INT_Cul_RIGHT;
			f_Plane[INT_Cul_AxisZ] = st_Max.z;
			b_Inside = FALSE;
		}
		else
			ul_Quadrant[INT_Cul_AxisZ] = INT_Cul_MIDDLE;
	}

	/* If the Ray origin is inside the box */
	if(!_pst_CollidedPoint) _pst_CollidedPoint = &st_CollidedPoint;
	if(b_Inside)
	{
		*_pst_CollidedPoint = *_pst_Origin;

		if(pf_Dist) *pf_Dist = -1.0f;
//		PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
		return TRUE;
	}

	ul_ClosestPlane = INT_Cul_AxisX;

	/* Unrolling loop ... Axis X */
	if((ul_Quadrant[INT_Cul_AxisX] != INT_Cul_MIDDLE) && (_pst_Direction->x != 0))
		f_TMax = fDiv(fSub(f_Plane[INT_Cul_AxisX], _pst_Origin->x), _pst_Direction->x);
	else
		f_TMax = -1.0;

	/* Axis Y */
	if((ul_Quadrant[INT_Cul_AxisY] != INT_Cul_MIDDLE) && (_pst_Direction->y != 0))
	{
		f_TTemp = fDiv(fSub(f_Plane[INT_Cul_AxisY], _pst_Origin->y), _pst_Direction->y);
		if(fSup(f_TTemp, f_TMax))
		{
			f_TMax = f_TTemp;

			ul_ClosestPlane = INT_Cul_AxisY;
		}
	}

	/* Axis Z */
	if((ul_Quadrant[INT_Cul_AxisZ] != INT_Cul_MIDDLE) && (_pst_Direction->z != 0))
	{
		f_TTemp = fDiv(fSub(f_Plane[INT_Cul_AxisZ], _pst_Origin->z), _pst_Direction->z);
		if(fSup(f_TTemp, f_TMax))
		{
			f_TMax = f_TTemp;

			ul_ClosestPlane = INT_Cul_AxisZ;
		}
	}

	if((fInfZero(f_TMax)) || (fSup(f_TMax, f_MaxDist))) 
	{
//		PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
		return FALSE;
	}

	if(pf_Dist) *pf_Dist = f_TMax;
	if(ul_ClosestPlane != INT_Cul_AxisX)
	{
		_pst_CollidedPoint->x = _pst_Origin->x + f_TMax * _pst_Direction->x;
		if((fInf(_pst_CollidedPoint->x, st_Min.x)) || (fSup(_pst_CollidedPoint->x, st_Max.x))) 
		{
//			PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
			return FALSE;
		}
	}
	else
		_pst_CollidedPoint->x = f_Plane[INT_Cul_AxisX];

	if(ul_ClosestPlane != INT_Cul_AxisY)
	{
		_pst_CollidedPoint->y = _pst_Origin->y + f_TMax * _pst_Direction->y;
		if((fInf(_pst_CollidedPoint->y, st_Min.y)) || (fSup(_pst_CollidedPoint->y, st_Max.y))) 
		{

//			PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
			return FALSE;
		}
	}
	else
		_pst_CollidedPoint->y = f_Plane[INT_Cul_AxisY];

	if(ul_ClosestPlane != INT_Cul_AxisZ)
	{
		_pst_CollidedPoint->z = _pst_Origin->z + f_TMax * _pst_Direction->z;
		if((fInf(_pst_CollidedPoint->z, st_Min.z)) || (fSup(_pst_CollidedPoint->z, st_Max.z))) 
		{
//			PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
			return FALSE;
		}

	}
	else
		_pst_CollidedPoint->z = f_Plane[INT_Cul_AxisZ];

//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
	return ul_ClosestPlane + 1;
}

/*
 =======================================================================================================================
    Aim:    Detects whether a ray intersects a triangle. The functions also returns the distance between the origin of
            the ray and the collided point and the exact position of the collided point.

    Note:   This algo. computes everything in the Triangle barycentric coordinates system. For more info, look in the
            "Real Time Rendering" book, p.305.
 =======================================================================================================================
 */
extern "C" BOOL INT_FullRayTriangle(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_C,
	float				*_f_Dist,
	MATH_tdst_Vector	*_pst_CollidedPoint,
	BOOL				_b_HasScale /* obsolethe */
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_AB, st_AC, st_AO;
	MATH_tdst_Vector	st_Cross_Ray_AC, st_Cross_AO_AB;
	MATH_tdst_Vector	st_Buffer;
	float				f_Det, f_InvDet, f_U, f_V;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);

	MATH_SubVector(&st_AB, _pst_B, _pst_A);
	MATH_SubVector(&st_AC, _pst_C, _pst_A);
	MATH_CrossProduct(&st_Cross_Ray_AC, _pst_Direction, &st_AC);
	f_Det = MATH_f_DotProduct(&st_AB, &st_Cross_Ray_AC);
	if(fInf(f_Det, (float) 1E-5) && fSup(f_Det, (float) - 1E-5)) 
	{
//		PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
		return FALSE;
	}
	f_InvDet = fInv(f_Det);
	MATH_SubVector(&st_AO, _pst_Origin, _pst_A);
	f_U = fMul(f_InvDet, MATH_f_DotProduct(&st_AO, &st_Cross_Ray_AC));
	if(fInfZero(f_U) || (fSup(f_U, 1.0f))) 
	{
//		PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
		return FALSE;
	}
	MATH_CrossProduct(&st_Cross_AO_AB, &st_AO, &st_AB);
	f_V = fMul(f_InvDet, MATH_f_DotProduct(_pst_Direction, &st_Cross_AO_AB));
	if(fInfZero(f_V) || (fSup(fAdd(f_U, f_V), 1.0f))) 
	{
//		PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
		return FALSE;
	}

	/* OM = (1 - u - v)OA + uOB + vOC with O:origin of the Ray and M: Collided Point */
	if(_pst_CollidedPoint)
	{
		MATH_MulVector(&st_Buffer, _pst_C, f_V);
		MATH_MulVector(_pst_CollidedPoint, _pst_B, f_U);
		MATH_AddEqualVector(_pst_CollidedPoint, &st_Buffer);
		MATH_MulVector(&st_Buffer, _pst_A, 1 - f_U - f_V);
		MATH_AddEqualVector(_pst_CollidedPoint, &st_Buffer);
	}

	*_f_Dist = fMul(f_InvDet, MATH_f_DotProduct(&st_AC, &st_Cross_AO_AB));
	if(*_f_Dist < 0.0f) 
	{
//		PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
		return FALSE;
	}

//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Returns if a point is inside a triangle or not.

    Note:   Base sur la theorie des parcours.
 =======================================================================================================================
 */
extern "C" BOOL INT_PointInTriangle(
	MATH_tdst_Vector	*_pst_M,
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_C,
	MATH_tdst_Vector	*_pst_N,
	UCHAR				*_pc_Code
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectMA, st_VectMB, st_VectMC;
	float				Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_VectMA, _pst_A, _pst_M);
	MATH_SubVector(&st_VectMB, _pst_B, _pst_M);
	MATH_SubVector(&st_VectMC, _pst_C, _pst_M);

	if(fSup(fAbs(_pst_N->x), Cf_InvSqrt3))
	{
		Val = st_VectMA.y * st_VectMB.z - st_VectMA.z * st_VectMB.y;
		if(fDifferentSign(_pst_N->x, Val))
		{
			if(_pc_Code) *_pc_Code = 0;
			return FALSE;
		}
		else
		{
			Val = st_VectMB.y * st_VectMC.z - st_VectMB.z * st_VectMC.y;
			if(fDifferentSign(_pst_N->x, Val))
			{
				if(_pc_Code) *_pc_Code = 1;
				return FALSE;
			}
			else
			{
				Val = st_VectMC.y * st_VectMA.z - st_VectMC.z * st_VectMA.y;

				if(fSameSign(_pst_N->x, Val))
					return TRUE;
				else
				{
					if(_pc_Code) *_pc_Code = 2;
					return FALSE;
				}
			}
		}
	}
	else
	{
		if(fSup(fAbs(_pst_N->y), Cf_InvSqrt3))
		{
			Val = st_VectMA.z * st_VectMB.x - st_VectMA.x * st_VectMB.z;
			if(fDifferentSign(_pst_N->y, Val))
			{
				if(_pc_Code) *_pc_Code = 0;
				return FALSE;
			}
			else
			{
				Val = st_VectMB.z * st_VectMC.x - st_VectMB.x * st_VectMC.z;
				if(fDifferentSign(_pst_N->y, Val))
				{
					if(_pc_Code) *_pc_Code = 1;
					return FALSE;
				}
				else
				{
					Val = st_VectMC.z * st_VectMA.x - st_VectMC.x * st_VectMA.z;
					if(fSameSign(_pst_N->y, Val))
						return TRUE;
					else
					{
						if(_pc_Code) *_pc_Code = 2;
						return FALSE;
					}
				}
			}
		}
		else
		{
			Val = st_VectMA.x * st_VectMB.y - st_VectMA.y * st_VectMB.x;
			if(fDifferentSign(_pst_N->z, Val))
			{
				if(_pc_Code) *_pc_Code = 0;
				return FALSE;
			}
			else
			{
				Val = st_VectMB.x * st_VectMC.y - st_VectMB.y * st_VectMC.x;
				if(fDifferentSign(_pst_N->z, Val))
				{
					if(_pc_Code) *_pc_Code = 1;
					return FALSE;
				}
				else
				{
					Val = st_VectMC.x * st_VectMA.y - st_VectMC.y * st_VectMA.x;
					if(fSameSign(_pst_N->z, Val))
						return TRUE;
					else
					{
						if(_pc_Code) *_pc_Code = 2;
						return FALSE;
					}
				}
			}
		}
	}
}

/*$off*/
/*
 ===================================================================================================
				*** INT_FullSegmentTriangle ***

						---------------
						- Declaration -
						---------------
	 0 : Origin of the world
	 A, B: The 2 extremities of the segment
	 T1, T2, T3 : The 3 points of the triangle.
	 N : Normalized normal vector of the Triangle plan.
						---------------
						- Explanation -
						---------------
     First, We compute f = AM / AB where M is the intersection between (AB) and the triangle plan.

			AM/AB = (AM.N)/(AB.N) = (AT1.N)/(AB.N) = N / D.

	 Then, we check if M is inside the triangle or not.
 ===================================================================================================
 */
/*$on*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INT_FullSegmentTriangle
(
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_T1,
	MATH_tdst_Vector	*_pst_T2,
	MATH_tdst_Vector	*_pst_T3,
	MATH_tdst_Vector	*_pst_N,
	MATH_tdst_Vector	*_pst_Hit,
	UCHAR				*_pc_Code
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectAT1, st_VectAB;
	float				f, Num, Den;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Float f_Min, f_Max; */

/*$off*/

									/* --- X rejection ---*/
/*$F
    f_Min = fSup(_pst_A->x, _pst_B->x) ? _pst_B->x : _pst_A->x;
    f_Max = fSup(_pst_T1->x, _pst_T2->x) && fSup(_pst_T1->x, _pst_T3->x)    ?
                 _pst_T1->x :
                 fSup(_pst_T2->x,_pst_T3->x) ?
                 _pst_T2->x :
                 _pst_T3->x;

    if(fInf(f_Max, f_Min)) return FALSE;

    f_Max = fSup(_pst_A->x, _pst_B->x) ? _pst_A->x : _pst_B->x;
    f_Min = fInf(_pst_T1->x, _pst_T2->x) && fInf(_pst_T1->x, _pst_T3->x) ? _pst_T1->x : fInf
        (
            _pst_T2->x,
            _pst_T3->x
        ) ? _pst_T2->x : _pst_T3->x;

    if(fInf(f_Max, f_Min)) return FALSE;
*/

									/* --- Y rejection ---*/
/*$F
    f_Min = fSup(_pst_A->y, _pst_B->y) ? _pst_B->y : _pst_A->y;
    f_Max = fSup(_pst_T1->y, _pst_T2->y) && fSup(_pst_T1->y, _pst_T3->y)    ?
                 _pst_T1->y :
                 fSup(_pst_T2->y,_pst_T3->y) ?
                 _pst_T2->y :
                 _pst_T3->y;

    if(fInf(f_Max, f_Min)) return FALSE;

    f_Max = fSup(_pst_A->y, _pst_B->y) ? _pst_A->y : _pst_B->y;
    f_Min = fInf(_pst_T1->y, _pst_T2->y) && fInf(_pst_T1->y, _pst_T3->y) ? _pst_T1->y : fInf
        (
            _pst_T2->y,
            _pst_T3->y
        ) ? _pst_T2->y : _pst_T3->y;

    if(fInf(f_Max, f_Min)) return FALSE;
*/

									/* --- Z rejection ---*/
/*$F
    f_Min = fSup(_pst_A->z, _pst_B->z) ? _pst_B->z : _pst_A->z;
    f_Max = fSup(_pst_T1->z, _pst_T2->z) && fSup(_pst_T1->z, _pst_T3->z)    ?
                 _pst_T1->z :
                 fSup(_pst_T2->z,_pst_T3->z) ?
                 _pst_T2->z :
                 _pst_T3->z;

    if(fInf(f_Max, f_Min)) return FALSE;

    f_Max = fSup(_pst_A->z, _pst_B->z) ? _pst_A->z : _pst_B->z;
    f_Min = fInf(_pst_T1->z, _pst_T2->z) && fInf(_pst_T1->z, _pst_T3->z) ? _pst_T1->z : fInf
        (
            _pst_T2->z,
            _pst_T3->z
        ) ? _pst_T2->z : _pst_T3->z;

    if(fInf(f_Max, f_Min)) return FALSE;
*/
	/*$on*/
	MATH_SubVector(&st_VectAT1, _pst_T1, _pst_A);
	MATH_SubVector(&st_VectAB, _pst_B, _pst_A);
	Num = MATH_f_DotProduct(&st_VectAT1, _pst_N);

	/* BackFace case : Treated before this function call ---> NOT DONE THERE */
	Den = MATH_f_DotProduct(&st_VectAB, _pst_N);

	/* If the segment is almost parallel to the triangle, we return FALSE; */
	if(fInf(Den, (float) 1E-5) && fSup(Den, (float) - 1E-5)) return FALSE;

	f = fDiv(Num, Den);

	/* M is not between A and B, we return FALSE. */
	if((f <= 0.0f) || (f >= 1.0f)) return FALSE;

	/* O-Hit = OA + fAB */
	MATH_MulVector(_pst_Hit, &st_VectAB, f);
	MATH_AddVector(_pst_Hit, _pst_A, _pst_Hit);

	return(INT_PointInTriangle(_pst_Hit, _pst_T1, _pst_T2, _pst_T3, _pst_N, _pc_Code));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INT_FullSegmentSphere
(
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_Center,
	float				_f_Radius,
	MATH_tdst_Vector	*_pst_Hit
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectCA, st_VectCB, st_VectAB;
	MATH_tdst_Vector	st_Dir;
	float				f_NormAB, f_NormAM, f_NormMP, f_SqrNormCA;
	float				f_SqrRadius, f_SqrNormCM, f_SqrNormAM;
	float				f_Dist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_VectCA, _pst_A, _pst_Center);
	MATH_SubVector(&st_VectCB, _pst_B, _pst_Center);
	MATH_SubVector(&st_VectAB, _pst_B, _pst_A);

	/* --- X rejection */

	/*$F
    if(fSupZero(st_VectAB.x))
    {
        if(fSup(st_VectCA.x, _f_Radius)) return FALSE;
        if(fInf(st_VectCB.x, -_f_Radius)) return FALSE;
    }
    else
    {
        if(fSup(st_VectCB.x, _f_Radius)) return FALSE;
        if(fInf(st_VectCA.x, -_f_Radius)) return FALSE;
    }
*/

	/* --- Y rejection */

	/*$F
    if(fSupZero(st_VectAB.y))
    {
        if(fSup(st_VectCA.y, _f_Radius)) return FALSE;
        if(fInf(st_VectCB.y, -_f_Radius)) return FALSE;
    }
    else
    {
        if(fSup(st_VectCB.y, _f_Radius)) return FALSE;
        if(fInf(st_VectCA.y, -_f_Radius)) return FALSE;
    }
*/

	/* --- Z rejection */

	/*$F
    if(fSupZero(st_VectAB.z))
    {
        if(fSup(st_VectCA.z, _f_Radius)) return FALSE;
        if(fInf(st_VectCB.z, -_f_Radius)) return FALSE;
    }
    else
    {
        if(fSup(st_VectCB.z, _f_Radius)) return FALSE;
        if(fInf(st_VectCA.z, -_f_Radius)) return FALSE;
    }
*/
	f_NormAB = MATH_f_NormVector(&st_VectAB);

	/* Anti Bug */	
	if(f_NormAB == 0.0f)
	{
#if defined(_GAMECUBE) && defined(_DEBUG)
		OSReport("Old collision bug detection\n");
#endif	
		return FALSE;
	}
	st_Dir.x = fMul(st_VectAB.x, fInv(f_NormAB));
	st_Dir.y = fMul(st_VectAB.y, fInv(f_NormAB));
	st_Dir.z = fMul(st_VectAB.z, fInv(f_NormAB));

	/* M: Projection of C onto the (AB) line */
	f_NormAM = -MATH_f_DotProduct(&st_VectCA, &st_Dir);
	f_SqrNormAM = fSqr(f_NormAM);
	f_SqrNormCA = MATH_f_SqrNormVector(&st_VectCA);
	f_SqrRadius = fSqr(_f_Radius);

	/*
	 * The sphere is behind the segment AND the segment first point is not in the
	 * Sphere, there cannot be any collision
	 */
	if((f_NormAM < 0) && fSup(f_SqrNormCA, f_SqrRadius)) return FALSE;

	/* Pythagorean theorem */
	f_SqrNormCM = fSub(f_SqrNormCA, f_SqrNormAM);

	/* If the Collision point is out of the sphere, there is no collision */
	if(fSup(f_SqrNormCM, f_SqrRadius)) return FALSE;

	/* P: Intersection between the (AB) line and the sphere */
	f_NormMP = fSqrt(fSub(f_SqrRadius, f_SqrNormCM));

	/* TEMP */
	if((f_NormAM - f_NormMP) > f_NormAB) return FALSE;

	if(f_NormAM > f_NormAB)
	{
		f_Dist = f_NormAB;
		*_pst_Hit = *_pst_B;
		return TRUE;
	}

	/* Fin Temp. */
	if(fSup(f_SqrNormCA, f_SqrRadius))
	{
		/* The hit point is M. */
		MATH_MulVector(_pst_Hit, &st_Dir, f_NormAM);
		MATH_AddVector(_pst_Hit, _pst_A, _pst_Hit);
		return TRUE;
	}
	else
	{
		f_Dist = 0.0f;
		*_pst_Hit = *_pst_A;
		return TRUE;
	}
}

/*
 =======================================================================================================================
    Node: When we test the edges, we DO NOT try them randomly. The first one is closest edge of the projected Sphere
    center onto the Triangle plan. The other 2 have no order.
 =======================================================================================================================
 */
extern "C" BOOL INT_FullSphereTriangle(
	MATH_tdst_Vector	*_pst_Center,
	float				_f_Radius,
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_C,
	MATH_tdst_Vector	*_pst_Normal,	/* Normal of the triangle. */
	MATH_tdst_Vector	*_pst_Hit,
	ULONG				*_pul_CollisionType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Start, st_End, st_Segment, st_VectAT1;
	UCHAR				uc_Code;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_MulVector(&st_Start, _pst_Normal, _f_Radius);
	MATH_AddVector(&st_Start, &st_Start, _pst_Center);

	MATH_MulVector(&st_Segment, _pst_Normal, -fTwice(_f_Radius));
	MATH_AddVector(&st_End, &st_Start, &st_Segment);

	/* We want to now if the triangle is a backface in order not to try the edges. */
	MATH_SubVector(&st_VectAT1, _pst_A, _pst_Center);

	uc_Code = 0;

	if(!(INT_FullSegmentTriangle(&st_Start, &st_End, _pst_A, _pst_B, _pst_C, _pst_Normal, _pst_Hit, &uc_Code)))
	{
		switch(uc_Code)
		{
		case 0:
			if(INT_FullSegmentSphere(_pst_A, _pst_B, _pst_Center, _f_Radius, _pst_Hit))
				*_pul_CollisionType = COL_Cul_Edge12;
			else
			{
				if(INT_FullSegmentSphere(_pst_B, _pst_C, _pst_Center, _f_Radius, _pst_Hit))
					*_pul_CollisionType = COL_Cul_Edge23;
				else
				{
					if(INT_FullSegmentSphere(_pst_C, _pst_A, _pst_Center, _f_Radius, _pst_Hit))
						*_pul_CollisionType = COL_Cul_Edge13;
					else
						return FALSE;
				}
			}

			return TRUE;
		case 1:
			if(INT_FullSegmentSphere(_pst_B, _pst_C, _pst_Center, _f_Radius, _pst_Hit))
				*_pul_CollisionType = COL_Cul_Edge23;
			else
			{
				if(INT_FullSegmentSphere(_pst_A, _pst_B, _pst_Center, _f_Radius, _pst_Hit))
					*_pul_CollisionType = COL_Cul_Edge12;
				else
				{
					if(INT_FullSegmentSphere(_pst_C, _pst_A, _pst_Center, _f_Radius, _pst_Hit))
						*_pul_CollisionType = COL_Cul_Edge13;
					else
						return FALSE;
				}
			}

			return TRUE;
		case 2:
			if(INT_FullSegmentSphere(_pst_C, _pst_A, _pst_Center, _f_Radius, _pst_Hit))
				*_pul_CollisionType = COL_Cul_Edge13;
			else
			{
				if(INT_FullSegmentSphere(_pst_A, _pst_B, _pst_Center, _f_Radius, _pst_Hit))
					*_pul_CollisionType = COL_Cul_Edge12;
				else
				{
					if(INT_FullSegmentSphere(_pst_B, _pst_C, _pst_Center, _f_Radius, _pst_Hit))
						*_pul_CollisionType = COL_Cul_Edge23;
					else
						return FALSE;
				}
			}

			return TRUE;
		}
	}
	else
		*_pul_CollisionType = COL_Cul_Triangle;

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    En Anglais dans le texte.

    Note:   From the Graphics Gems IV
 =======================================================================================================================
 */
BOOL INT_SegmentCylinder
(
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_CylBase,
	MATH_tdst_Vector	*_pst_Axis, /* Unit */
	float				f_Radius,
	float				f_Length
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectAB, st_VectCylA, st_Cross;
	float				f_SqrNorm, f_SqrDot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_VectAB, _pst_B, _pst_A);
	MATH_SubVector(&st_VectCylA, _pst_A, _pst_CylBase);
	MATH_CrossProduct(&st_Cross, &st_VectAB, _pst_Axis);

	f_SqrDot = fSqr(MATH_f_DotProduct(&st_VectCylA, &st_Cross));
	f_SqrNorm = MATH_f_SqrNormVector(&st_Cross);

	return(fInf(f_SqrDot, fMul(fSqr(f_Radius), f_SqrNorm)));
}

/*
 =======================================================================================================================
    Aim:    Detects wether 2 OBBox are overlapping.

    Note:   B is assumed to be in the A local coordinate system. Mat is the matrix to go from the B coordinate system
            to the A one. For more information, look in the Real Time Rendering Book p.327
 =======================================================================================================================
 */
BOOL INT_LowLevel_OBBoxOBBox
(
	float				h_Au,
	float				h_Av,
	float				h_Aw,
	float				h_Bu,
	float				h_Bv,
	float				h_Bw,
	MATH_tdst_Matrix	*Mat
)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*T;
	/*~~~~~~~~~~~~~~~~~~~*/

	/*$off*/
	T = MATH_pst_GetTranslation(Mat);

	/* Au Rejection Test */
	if(fSup(fAbs(T->x), h_Au + h_Bu * fAbs(Mat->Ix) + h_Bv * fAbs(Mat->Jx) + h_Bw * fAbs(Mat->Kx)))
		return FALSE;

	/* Av Rejection Test */
	if(fSup(fAbs(T->y), h_Av + h_Bu * fAbs(Mat->Iy) + h_Bv * fAbs(Mat->Jy) + h_Bw * fAbs(Mat->Ky)))
		return FALSE;

	/* Aw Rejection Test */
	if(fSup(fAbs(T->z), h_Aw + h_Bu * fAbs(Mat->Iz) + h_Bv * fAbs(Mat->Jz) + h_Bw * fAbs(Mat->Kz)))
		return FALSE;

	/* Bu Rejection Test */
	if(fSup(fAbs(T->x * Mat->Ix + T->y * Mat->Iy + T->z * Mat->Iz), h_Au * fAbs(Mat->Ix) + h_Av * fAbs(Mat->Iy) + h_Aw * fAbs(Mat->Iz) + h_Bu))
		return FALSE;

	/* Bv Rejection Test */
	if(fSup(fAbs(T->x * Mat->Jx + T->y * Mat->Jy + T->z * Mat->Jz), h_Au * fAbs(Mat->Jx) + h_Av * fAbs(Mat->Jy) + h_Aw * fAbs(Mat->Jz) + h_Bv))
		return FALSE;

	/* Bw Rejection Test */
	if(fSup(fAbs(T->x * Mat->Kx + T->y * Mat->Ky + T->z * Mat->Kz), h_Au * fAbs(Mat->Kx) + h_Av * fAbs(Mat->Ky) + h_Aw * fAbs(Mat->Kz) + h_Bw))
		return FALSE;

/*$F
					----------------- DO NOT DELETE ----------------------
					---------- ALGO OTHER AXIS REJECTION TEST ------------
					--------------- ??? No Need for now ??? --------------

    if(fSup(fAbs(T->z * Mat->Iy - T->y * Mat->Iz), h_Av * fAbs(Mat->Iz) + h_Aw * fAbs(Mat->Iy) + h_Bv * fAbs(Mat->Kx) + h_Bw * fAbs(Mat->Jx))) return FALSE;
    if(fSup(fAbs(T->z * Mat->Jy - T->y * Mat->Jz), h_Av * fAbs(Mat->Jz) + h_Aw * fAbs(Mat->Jy) + h_Bu * fAbs(Mat->Kx) + h_Bw * fAbs(Mat->Ix))) return FALSE;
    if(fSup(fAbs(T->z * Mat->Ky - T->y * Mat->Kz), h_Av * fAbs(Mat->Kz) + h_Aw * fAbs(Mat->Ky) + h_Bu * fAbs(Mat->Jx) + h_Bv * fAbs(Mat->Ix))) return FALSE;
    if(fSup(fAbs(T->x * Mat->Iz - T->z * Mat->Ix), h_Au * fAbs(Mat->Iz) + h_Aw * fAbs(Mat->Ix) + h_Bv * fAbs(Mat->Ky) + h_Bw * fAbs(Mat->Jy))) return FALSE;
    if(fSup(fAbs(T->x * Mat->Jz - T->z * Mat->Jx), h_Au * fAbs(Mat->Jz) + h_Aw * fAbs(Mat->Jx) + h_Bu * fAbs(Mat->Ky) + h_Bw * fAbs(Mat->Iy))) return FALSE;
    if(fSup(fAbs(T->x * Mat->Kz - T->z * Mat->Kx), h_Au * fAbs(Mat->Kz) + h_Aw * fAbs(Mat->Kx) + h_Bu * fAbs(Mat->Jy) + h_Bv * fAbs(Mat->Iy))) return FALSE;
    if(fSup(fAbs(T->y * Mat->Ix - T->x * Mat->Iy), h_Au * fAbs(Mat->Iy) + h_Av * fAbs(Mat->Ix) + h_Bv * fAbs(Mat->Kz) + h_Bw * fAbs(Mat->Jz))) return FALSE;
    if(fSup(fAbs(T->y * Mat->Jx - T->x * Mat->Jy), h_Au * fAbs(Mat->Jy) + h_Av * fAbs(Mat->Jx) + h_Bu * fAbs(Mat->Kz) + h_Bw * fAbs(Mat->Iz))) return FALSE;
    if(fSup(fAbs(T->y * Mat->Kx - T->x * Mat->Ky), h_Au * fAbs(Mat->Ky) + h_Av * fAbs(Mat->Kx) + h_Bu * fAbs(Mat->Jz) + h_Bv * fAbs(Mat->Iz))) return FALSE;

*/

	return TRUE;
/*$on*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" BOOL INT_HighLevel_OBBoxOBBox(
	MATH_tdst_Vector	*_pst_A_Min,
	MATH_tdst_Vector	*_pst_A_Max,
	MATH_tdst_Vector	*_pst_B_Min,
	MATH_tdst_Vector	*_pst_B_Max,
	MATH_tdst_Matrix	*_pst_Mat,
	MATH_tdst_Vector	*_pst_A_Scale
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Scale;
	float				h_Au, h_Av, h_Aw, h_Bu, h_Bv, h_Bw;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$F ---- Stuff Computation -------- */
	if(_pst_A_Scale)
	{
		h_Au = (_pst_A_Max->x - _pst_A_Min->x) * 0.5f * _pst_A_Scale->x;
		h_Av = (_pst_A_Max->y - _pst_A_Min->y) * 0.5f * _pst_A_Scale->y;
		h_Aw = (_pst_A_Max->z - _pst_A_Min->z) * 0.5f * _pst_A_Scale->z;
	}
	else
	{
		h_Au = (_pst_A_Max->x - _pst_A_Min->x) * 0.5f;
		h_Av = (_pst_A_Max->y - _pst_A_Min->y) * 0.5f;
		h_Aw = (_pst_A_Max->z - _pst_A_Min->z) * 0.5f;
	}

	h_Bu = (_pst_B_Max->x - _pst_B_Min->x) * 0.5f;
	h_Bv = (_pst_B_Max->y - _pst_B_Min->y) * 0.5f;
	h_Bw = (_pst_B_Max->z - _pst_B_Min->z) * 0.5f;

	/* If the Matrix to go from BCS to ACS has scale, we must deal with it */
	if(MATH_b_TestScaleType(_pst_Mat))
	{
		MATH_GetScale(&st_Scale, _pst_Mat);

		h_Bu = h_Bu * st_Scale.x;
		h_Bv = h_Bv * st_Scale.y;
		h_Bw = h_Bw * st_Scale.z;

		return INT_LowLevel_OBBoxOBBox(h_Au, h_Av, h_Aw, h_Bu, h_Bv, h_Bw, _pst_Mat);
	}
	else
		return INT_LowLevel_OBBoxOBBox(h_Au, h_Av, h_Aw, h_Bu, h_Bv, h_Bw, _pst_Mat);
}

/*$F
 =======================================================================================================================
    Aim:    Detects if an OBBox is colliding a Sphere

    Note:   A is the Sphere and B is the Box.
			ACS: A Coordinate system.
			BCS: B Coordinate system.
 =======================================================================================================================
 */

extern "C" BOOL INT_SphereOBBox(
	MATH_tdst_Vector	*_pst_BCS_Center,
	float				f_BCS_Radius,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max,
	MATH_tdst_Vector	*_pst_ClosestPoint
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ClosestPoint, st_Dist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* --- X Closest Computation */
	if(_pst_BCS_Center->x < _pst_Min->x)
		st_ClosestPoint.x = _pst_Min->x;
	else
	{
		if(_pst_BCS_Center->x > _pst_Max->x)
			st_ClosestPoint.x = _pst_Max->x;
		else
			st_ClosestPoint.x = _pst_BCS_Center->x;
	}

	/* --- Y Closest Computation */
	if(_pst_BCS_Center->y < _pst_Min->y)
		st_ClosestPoint.y = _pst_Min->y;
	else
	{
		if(_pst_BCS_Center->y > _pst_Max->y)
			st_ClosestPoint.y = _pst_Max->y;
		else
			st_ClosestPoint.y = _pst_BCS_Center->y;
	}

	/* --- Z Closest Computation */
	if(_pst_BCS_Center->z < _pst_Min->z)
		st_ClosestPoint.z = _pst_Min->z;
	else
	{
		if(_pst_BCS_Center->z > _pst_Max->z)
			st_ClosestPoint.z = _pst_Max->z;
		else
			st_ClosestPoint.z = _pst_BCS_Center->z;
	}

	MATH_SubVector(&st_Dist, _pst_BCS_Center, &st_ClosestPoint);
	if(_pst_ClosestPoint) MATH_CopyVector(_pst_ClosestPoint, &st_ClosestPoint);

	return fInf(MATH_f_SqrNormVector(&st_Dist), fSqr(f_BCS_Radius));
}

/*
 =======================================================================================================================
		0 - min X | 1 - Max X | 2 - min Y | 3 - Max Y | 4 - min z | 5 - Max z 
 =======================================================================================================================
 */
int	INT_FindClosedBoxPlanFromInsidePoint(MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max, MATH_tdst_Vector *_pst_Point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	Test, TestM;
	int		iTest;	/*0 - min X | 1 - Max X | 2 - min Y | 3 - Max Y | 4 - min z | 5 - Max z */
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	TestM = 10000000.0f;

	/* X */
	Test = _pst_Max->x - _pst_Point->x;
	if(Test < TestM)
	{
		TestM = Test;
		iTest = 0;
	}

	Test =  _pst_Point->x - _pst_Min->x;
	if(Test < TestM)
	{
		TestM = Test;
		iTest = 1;
	}

	/* Y */
	Test = _pst_Max->y - _pst_Point->y;
	if(Test < TestM)
	{
		TestM = Test;
		iTest = 2;
	}

	Test =  _pst_Point->y - _pst_Min->y;
	if(Test < TestM)
	{
		TestM = Test;
		iTest = 3;
	}


	/* Z */
	Test = _pst_Max->z - _pst_Point->z;
	if(Test < TestM)
	{
		TestM = Test;
		iTest = 4;
	}

	Test =  _pst_Point->z - _pst_Min->z;
	if(Test < TestM)
	{
		TestM = Test;
		iTest = 5;
	}

	return iTest;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" BOOL INT_FullSphereOBBox(
	MATH_tdst_Vector	*_pst_A_BCS_Center,
	float				_f_A_GCS_Radius,
	MATH_tdst_Vector	*_pst_B_ScaledMin,
	MATH_tdst_Vector	*_pst_B_ScaledMax,
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_MoveUnit,
	MATH_tdst_Vector	*_pst_BCS_Normal,
	MATH_tdst_Vector	*_pst_BCS_Hit,
	ULONG				*pul_CollisionType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_Box		st_Box;
	MATH_tdst_Vector	st_Move;
	float				f_Move;
	float				f_Dist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Move, _pst_A_BCS_Center, _pst_Origin);
	f_Move = MATH_f_NormVector(&st_Move) + _f_A_GCS_Radius;
	MATH_CopyVector(&st_Box.st_Min, _pst_B_ScaledMin);
	MATH_CopyVector(&st_Box.st_Max, _pst_B_ScaledMax);

	/* Did the Move Ray touched the Box ? */
	if
	(
		!MATH_b_NulVector(_pst_MoveUnit)
	&&	INT_FullRayAABBox(_pst_Origin, _pst_MoveUnit, &st_Box, _pst_BCS_Hit, f_Move, &f_Dist)
	)
	{
		if(pul_CollisionType)
		{
			*pul_CollisionType = COL_Cul_Box;
			*pul_CollisionType |= COL_Cul_Static;
		}

		if(f_Dist != -1.0f)
		{
			if(fEqWithEpsilon(_pst_BCS_Hit->x, _pst_B_ScaledMax->x, 1E-3f))
			{
				MATH_InitVector(_pst_BCS_Normal, 1.0f, 0.0f, 0.0f);
				return TRUE;
			}
			if(fEqWithEpsilon(_pst_BCS_Hit->y, _pst_B_ScaledMax->y, 1E-3f))
			{
				MATH_InitVector(_pst_BCS_Normal, 0.0f, 1.0f, 0.0f);
				return TRUE;
			}
			if(fEqWithEpsilon(_pst_BCS_Hit->z, _pst_B_ScaledMax->z, 1E-3f))
			{
				MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, 1.0f);
				return TRUE;
			}
			if(fEqWithEpsilon(_pst_BCS_Hit->x, _pst_B_ScaledMin->x, 1E-3f))
			{
				MATH_InitVector(_pst_BCS_Normal, -1.0f, 0.0f, 0.0f);
				return TRUE;
			}
			if(fEqWithEpsilon(_pst_BCS_Hit->y, _pst_B_ScaledMin->y, 1E-3f))
			{
				MATH_InitVector(_pst_BCS_Normal, 0.0f, -1.0f, 0.0f);
				return TRUE;
			}
			if(fEqWithEpsilon(_pst_BCS_Hit->z, _pst_B_ScaledMin->z, 1E-3f))
			{
				MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, -1.0f);
				return TRUE;
			}
		}
		else
		{
			/*~~~~~~~~~~~~*/
			int		iTest;
			/*~~~~~~~~~~~~*/

			iTest = INT_FindClosedBoxPlanFromInsidePoint(_pst_B_ScaledMin, _pst_B_ScaledMax, _pst_BCS_Hit);
			switch(iTest)
			{
				case 0:
				_pst_BCS_Hit->x = _pst_B_ScaledMin->x;
				MATH_InitVector(_pst_BCS_Normal, -1.0f, 0.0f, 0.0f);
				return TRUE;

				case 1:
				_pst_BCS_Hit->x = _pst_B_ScaledMax->x;
				MATH_InitVector(_pst_BCS_Normal, 1.0f, 0.0f, 0.0f);
				return TRUE;

				case 2:
				_pst_BCS_Hit->y = _pst_B_ScaledMin->y;
				MATH_InitVector(_pst_BCS_Normal, 0.0f, -1.0f, 0.0f);
				return TRUE;

				case 3:
				_pst_BCS_Hit->y = _pst_B_ScaledMax->y;
				MATH_InitVector(_pst_BCS_Normal, 1.0f, 1.0f, 0.0f);
				return TRUE;

				case 4:
				_pst_BCS_Hit->z = _pst_B_ScaledMin->z;
				MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, -1.0f);
				return TRUE;

				case 5:
				_pst_BCS_Hit->z = _pst_B_ScaledMax->z;
				MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, 1.0f);
				return TRUE;

			}
		}
	}

	/* Ray didnt detect the collision. We try statically. */
	if(INT_SphereOBBox(_pst_A_BCS_Center, _f_A_GCS_Radius, _pst_B_ScaledMin, _pst_B_ScaledMax, _pst_BCS_Hit))
	{
		if(pul_CollisionType)
		{
			*pul_CollisionType = COL_Cul_Box;
			*pul_CollisionType |= COL_Cul_Static;
		}

		/* If the Final point INSIDE the Box ??? */
#ifdef ACTIVE_EDITORS
		if
		(
			(_pst_A_BCS_Center->x > _pst_B_ScaledMin->x)
		&&	(_pst_A_BCS_Center->x < _pst_B_ScaledMax->x)
		&&	(_pst_A_BCS_Center->y > _pst_B_ScaledMin->y)
		&&	(_pst_A_BCS_Center->y < _pst_B_ScaledMax->y)
		&&	(_pst_A_BCS_Center->z > _pst_B_ScaledMin->z)
		&&	(_pst_A_BCS_Center->z < _pst_B_ScaledMax->z)
		)
		{
			LINK_PrintStatusMsg("Inside");
		}

#endif
		MATH_SubVector(_pst_BCS_Normal, _pst_A_BCS_Center, _pst_BCS_Hit);

		if(MATH_b_NulVector(_pst_BCS_Normal)) return FALSE;

		MATH_NormalizeEqualVector(_pst_BCS_Normal);
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" BOOL INT_b_FullRayCylinder(
	MATH_tdst_Vector	*_pst_O,	/* Origin of the Ray */
	MATH_tdst_Vector	*_pst_Ray,	/* Ray */
	MATH_tdst_Vector	*_pst_A,	/* Basis of the Cylinder */
	MATH_tdst_Vector	*_pst_Axis, /* Axis of the Cylinder */
	float				_f_Radius,
	float				_f_Height,
	float				_f_MaxDist,
	float				*_f_Dist,
	MATH_tdst_Vector	*_pst_Hit	/* Hit point (computed here) */
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_AO;
	float				a, b, c, A, B, C, D, R, Delta, Alpha, Beta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InitVector(_pst_Hit, 0.0f, 0.0f, 0.0f);

	R = _f_Radius;
	MATH_SubVector(&st_AO, _pst_O, _pst_A);

	A = MATH_f_DotProduct(&st_AO, _pst_Axis);
	B = MATH_f_DotProduct(_pst_Ray, _pst_Axis);
	C = MATH_f_DotProduct(&st_AO, _pst_Ray);
	D = MATH_f_SqrNormVector(&st_AO);

	/* If Ray and Cylinder Axis are almost colinear. */
	if(fEqWithEpsilon(fabs(B), 1.0f, 1E-5f))
	{
		return FALSE;
	}

	a = 1.0f - (B * B);
	b = 2.0f * (C - A * B);
	c = -(A * A + R * R - D);

	Delta = b * b - 4.0f * a * c;

	if(Delta < 0.0f)
	{
		return FALSE;
	}

	Alpha = (-b - fSqrt(fAbs(Delta))) / (2.0f * a);

	if((Alpha < 0.0f) || (Alpha > _f_MaxDist))
	{
		return FALSE;
	}

	MATH_MulVector(_pst_Hit, _pst_Ray, Alpha);
	MATH_AddEqualVector(_pst_Hit, _pst_O);

	Beta = A + Alpha * B;

	if(_f_Dist) *_f_Dist = Alpha;

	if(fAbs(Beta) > _f_Height)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" BOOL INT_FullSphereCylinder(
	MATH_tdst_Vector	*_pst_A_BCS_Center,
	float				_f_A_GCS_Radius,
	MATH_tdst_Vector	*_pst_B_Center,
	float				_f_B_BCS_Radius,
	float				_f_B_BCS_Height,
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_MoveUnit,
	MATH_tdst_Vector	*_pst_BCS_Normal,
	MATH_tdst_Vector	*_pst_BCS_Hit,
	ULONG				*pul_CollisionType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_AxisPoint, st_AxisToCenter, st_Unit, st_Move;
	float				f_Move, f_Dist, f_ZOffset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Move, _pst_A_BCS_Center, _pst_Origin);
	f_Move = MATH_f_NormVector(&st_Move);

	MATH_CopyVector(&st_AxisPoint, _pst_B_Center);

	/* Are we colliding with the Upside or Downside of the Cylinder ? */
	if(0)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float				f_Dot1, f_Dot2, f_Alpha;
		MATH_tdst_Vector	st_Z, st_Vector;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InitVector(&st_Z, 0.0f, 0.0f, 1.0f);

		if(_pst_MoveUnit->z < 0.0f)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Up;
			/*~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_MulVector(&st_Up, &st_Z, _f_B_BCS_Height);
			MATH_AddEqualVector(&st_Up, _pst_B_Center);
			MATH_SubVector(&st_Vector, _pst_Origin, &st_Up);

			f_Dot1 = MATH_f_DotProduct(_pst_MoveUnit, &st_Z);
			f_Dot2 = MATH_f_DotProduct(&st_Vector, &st_Z);

			if(fAbs(f_Dot1) < 1E-3) goto RayCast;
			f_Alpha = -f_Dot2 / f_Dot1;

			if((f_Alpha < 0.0f) || (f_Alpha > f_Move)) goto RayCast;

			MATH_MulVector(_pst_BCS_Hit, _pst_MoveUnit, f_Alpha);
			MATH_AddEqualVector(_pst_BCS_Hit, _pst_Origin);
			MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, 1.0f);
			return TRUE;
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Down;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_MulVector(&st_Down, &st_Z, -_f_B_BCS_Height);
			MATH_AddEqualVector(&st_Down, _pst_B_Center);
			MATH_SubVector(&st_Vector, _pst_Origin, &st_Down);

			f_Dot1 = MATH_f_DotProduct(_pst_MoveUnit, &st_Z);
			f_Dot2 = MATH_f_DotProduct(&st_Vector, &st_Z);

			if(fAbs(f_Dot1) < 1E-3) goto RayCast;
			f_Alpha = -f_Dot2 / f_Dot1;

			if(f_Alpha > f_Move) goto RayCast;

			MATH_MulVector(_pst_BCS_Hit, _pst_MoveUnit, f_Alpha);
			MATH_AddEqualVector(_pst_BCS_Hit, _pst_Origin);
			MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, -1.0f);
			return TRUE;
		}
	}

RayCast:
	if
	(
		INT_b_FullRayCylinder
		(
			_pst_Origin,
			_pst_MoveUnit,
			_pst_B_Center,
			&MATH_gst_BaseVectorK,
			_f_B_BCS_Radius,
			_f_B_BCS_Height,
			f_Move,
			NULL,
			_pst_BCS_Hit
		)
	)
	{
		/*
		st_AxisPoint.z = _pst_BCS_Hit->z;
		MATH_SubVector(&st_AxisToCenter, _pst_BCS_Hit, &st_AxisPoint);
		if(MATH_b_NulVector(&st_AxisToCenter)) MATH_InitVector(&st_AxisToCenter, 0.0f, 0.0f, 1.0f);
		*/
		MATH_SubVector(&st_AxisToCenter, _pst_BCS_Hit, _pst_A_BCS_Center);
		MATH_NormalizeVector(_pst_BCS_Normal, &st_AxisToCenter);

		if(pul_CollisionType)
		{
			*pul_CollisionType = COL_Cul_Cylinder;
			*pul_CollisionType |= COL_Cul_Static;
		}

		/* #ifdef ACTIVE_EDITORS LINK_PrintStatusMsg("Cylinder Ray"); #endif */
		return TRUE;
	}

	st_AxisPoint.z = _pst_A_BCS_Center->z;
	f_ZOffset = st_AxisPoint.z - _pst_B_Center->z;

	if(fAbs(f_ZOffset) > (_f_A_GCS_Radius + _f_B_BCS_Height))
	{
		return FALSE;
	}

	MATH_SubVector(&st_AxisToCenter, _pst_A_BCS_Center, &st_AxisPoint);

	f_Dist = MATH_f_NormVector(&st_AxisToCenter);

	if(f_Dist > (_f_A_GCS_Radius + _f_B_BCS_Radius))
	{
		return FALSE;
	}

	if(!MATH_b_NulVector(&st_AxisToCenter))
		MATH_NormalizeVector(_pst_BCS_Normal, &st_AxisToCenter);
	else
	{
		if(_pst_A_BCS_Center->z > _pst_B_Center->z)
			MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, 1.0f);
		else
			MATH_InitVector(_pst_BCS_Normal, 0.0f, 0.0f, -1.0f);
	}

	MATH_MulVector(&st_Unit, _pst_BCS_Normal, _f_B_BCS_Radius);
	MATH_AddVector(_pst_BCS_Hit, &st_AxisPoint, &st_Unit);

	if(pul_CollisionType)
	{
		*pul_CollisionType = COL_Cul_Cylinder;
		*pul_CollisionType |= COL_Cul_Static;
	}

	/* #ifdef ACTIVE_EDITORS LINK_PrintStatusMsg("Cylinder Classical"); #endif */
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Returns the point of the line that is closest to the Point.

    Note:   To OPTIMIZZZZEEE !!!
 =======================================================================================================================
 */
void INT_PointLineClosestPoint
(
	MATH_tdst_Vector	*_pst_M,
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_MA,
	MATH_tdst_Vector	*_pst_Closest
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_AB, st_NormAB;
	float				f_AB, f_Dot, f_U;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_AB, _pst_B, _pst_A);
	MATH_NormalizeVector(&st_NormAB, &st_AB);
	f_AB = MATH_f_NormVector(&st_AB);
	f_Dot = MATH_f_DotProduct(_pst_MA, &st_NormAB);
	f_U = -fDiv(f_Dot, f_AB);

	if(f_U < 0.0f)
	{
		*_pst_Closest = *_pst_A;
		return;
	}

	if(f_U > 1.0f)
	{
		*_pst_Closest = *_pst_B;
		return;
	}

	MATH_MulVector(_pst_Closest, &st_AB, f_U);
	MATH_AddEqualVector(_pst_Closest, _pst_A);
	return;
}

/*
 =======================================================================================================================
    Aim:    Returns the point of the triangle that is closest to the Point.

    Note:   Returns TRUE if the point is inside the Triangle.
 =======================================================================================================================
 */
void INT_PointTriangleClosestPointGet
(
	MATH_tdst_Vector	*_pst_T1,
	MATH_tdst_Vector	*_pst_T2,
	MATH_tdst_Vector	*_pst_T3,
	MATH_tdst_Vector	*_pst_N,
	MATH_tdst_Vector	*_pst_M,
	MATH_tdst_Vector	*_pst_Closest,
	ULONG				*_pul_CollisionType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_MT1, st_MT2, st_MT3;
	float				f_SqrMT1, f_SqrMT2, f_SqrMT3, f_Max3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(INT_PointInTriangle(_pst_M, _pst_T1, _pst_T2, _pst_T3, _pst_N, NULL))
	{
		*_pst_Closest = *_pst_M;
		*_pul_CollisionType = COL_Cul_Triangle;
		return;
	}

	MATH_SubVector(&st_MT1, _pst_T1, _pst_M);
	MATH_SubVector(&st_MT2, _pst_T2, _pst_M);
	MATH_SubVector(&st_MT3, _pst_T3, _pst_M);

	f_SqrMT1 = MATH_f_SqrNormVector(&st_MT1);
	f_SqrMT2 = MATH_f_SqrNormVector(&st_MT2);
	f_SqrMT3 = MATH_f_SqrNormVector(&st_MT3);

	f_Max3 = fMax3(f_SqrMT1, f_SqrMT2, f_SqrMT3);

	if(f_Max3 == f_SqrMT1)
	{
		INT_PointLineClosestPoint(_pst_M, _pst_T2, _pst_T3, &st_MT2, _pst_Closest);
		*_pul_CollisionType = COL_Cul_Edge23;
		return;
	}

	if(f_Max3 == f_SqrMT2)
	{
		INT_PointLineClosestPoint(_pst_M, _pst_T1, _pst_T3, &st_MT1, _pst_Closest);
		*_pul_CollisionType = COL_Cul_Edge13;
		return;
	}

	if(f_Max3 == f_SqrMT3)
	{
		INT_PointLineClosestPoint(_pst_M, _pst_T1, _pst_T2, &st_MT1, _pst_Closest);
		*_pul_CollisionType = COL_Cul_Edge12;
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" BOOL INT_DynamicHit(
	MATH_tdst_Vector	*_pst_Dynamic_Center,
	float				_f_Dynamic_Radius,
	MATH_tdst_Vector	*_pst_Static_New_Center,
	float				_f_Static_Radius,
	MATH_tdst_Vector	*_pst_MoveUnit,
	MATH_tdst_Vector	*_pst_T1,
	MATH_tdst_Vector	*_pst_T2,
	MATH_tdst_Vector	*_pst_T3,
	MATH_tdst_Vector	*_pst_N,
	MATH_tdst_Vector	*_pst_Hit,
	ULONG				*_pul_CollisionType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_CenterT1, st_OT1;
	MATH_tdst_Vector	st_BCS_Proj, st_HitCenter;
	MATH_tdst_Vector	st_Radius, st_MinusRadius, st_Origin;
	MATH_tdst_Vector	st_Move;
	float				f_Dot, Den, Num, f;
	BOOL				b_FirstPass;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_FirstPass = TRUE;

	/* First Pass. Ray Cast to touch the Triangle. */
	MATH_MulVector(&st_MinusRadius, _pst_MoveUnit, -_f_Dynamic_Radius);
	MATH_MulVector(&st_Radius, _pst_MoveUnit, _f_Dynamic_Radius);
	MATH_AddVector(&st_Origin, _pst_Dynamic_Center, &st_MinusRadius);
	MATH_SubVector(&st_Move, _pst_Dynamic_Center, &st_Origin);
	MATH_AddEqualVector(&st_Move, &st_Radius);

	MATH_SubVector(&st_OT1, _pst_T1, &st_Origin);

	Num = MATH_f_DotProduct(&st_OT1, _pst_N);
	Den = MATH_f_DotProduct(&st_Move, _pst_N);

#ifdef JADEFUSION
	BOOL bDenIsAlmostZero = (fAbs(Den) < (float) 1E-5);

	if((Den > 0.0f) || bDenIsAlmostZero )
	{
		b_FirstPass = FALSE;
	}

	if( bDenIsAlmostZero )
	{
 		f = 0.0f;
	}
	else
	{
		f = fDiv(Num, Den);
	}
#else

	if((Den > 0.0f) || (fAbs(Den) < (float) 1E-5))
	{
		b_FirstPass = FALSE;
	}

	f = fDiv(Num, Den);
#endif

	if(f <= 0.0f)
		b_FirstPass = FALSE;
	else
	{
		MATH_MulVector(&st_BCS_Proj, &st_Move, f);
		MATH_AddEqualVector(&st_BCS_Proj, &st_Origin);

		INT_PointTriangleClosestPointGet(_pst_T1, _pst_T2, _pst_T3, _pst_N, &st_BCS_Proj, _pst_Hit, _pul_CollisionType);

		if(MATH_f_Distance(_pst_Hit, _pst_Dynamic_Center) > _f_Dynamic_Radius)
		{
			b_FirstPass = FALSE;
		}
		else
		{
			/* --- Cylinder Culling --- */
			MATH_SubVector(&st_HitCenter, _pst_Static_New_Center, _pst_Hit);
			f_Dot = MATH_f_DotProduct(&st_HitCenter, _pst_MoveUnit);

			MATH_MulVector(&st_HitCenter, _pst_MoveUnit, f_Dot);
			MATH_AddVector(&st_BCS_Proj, _pst_Hit, &st_HitCenter);

			if(MATH_f_Distance(&st_BCS_Proj, _pst_Static_New_Center) > _f_Static_Radius)
			{
				b_FirstPass = FALSE;
			}

			/* --- End Cylinder Culling --- */
		}
	}

	/* Second Pass. Projection of the Dynamic Center onto the Triangle. */
	if(!b_FirstPass)
	{
		MATH_SubVector(&st_CenterT1, _pst_T1, _pst_Dynamic_Center);
		f_Dot = MATH_f_DotProduct(&st_CenterT1, _pst_N);

		MATH_MulVector(&st_CenterT1, _pst_N, f_Dot);
		MATH_AddVector(&st_BCS_Proj, _pst_Dynamic_Center, &st_CenterT1);

		INT_PointTriangleClosestPointGet(_pst_T1, _pst_T2, _pst_T3, _pst_N, &st_BCS_Proj, _pst_Hit, _pul_CollisionType);

		if(MATH_f_Distance(_pst_Hit, _pst_Dynamic_Center) > _f_Dynamic_Radius)
		{
			return FALSE;
		}

		/* --- Cylinder Culling --- */
		MATH_SubVector(&st_HitCenter, _pst_Static_New_Center, _pst_Hit);
		f_Dot = MATH_f_DotProduct(&st_HitCenter, _pst_MoveUnit);

		MATH_MulVector(&st_HitCenter, _pst_MoveUnit, f_Dot);
		MATH_AddVector(&st_BCS_Proj, _pst_Hit, &st_HitCenter);

		if(MATH_f_Distance(&st_BCS_Proj, _pst_Static_New_Center) > _f_Static_Radius)
		{
			return FALSE;
		}

		/* --- End Cylinder Culling --- */
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" BOOL INT_FullTriangleTriangle(
	MATH_tdst_Vector	*_pst_A1,
	MATH_tdst_Vector	*_pst_A2,
	MATH_tdst_Vector	*_pst_A3,
	MATH_tdst_Vector	*_pst_B1,
	MATH_tdst_Vector	*_pst_B2,
	MATH_tdst_Vector	*_pst_B3,
	MATH_tdst_Vector	*_pst_CollidedPoint
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_T1T2, st_T1T3, st_Vector, st_Normal, st_RayUnit;
	float				Val, RayMax, RayDist;
	LONG				Test;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Rejection Test with Triangle 1 */
	MATH_SubVector(&st_T1T2, _pst_A2, _pst_A1);
	MATH_SubVector(&st_T1T3, _pst_A3, _pst_A1);
	MATH_CrossProduct(&st_Normal, &st_T1T2, &st_T1T3);

	MATH_SubVector(&st_Vector, _pst_B1, _pst_A1);
	Val = MATH_f_DotProduct(&st_Vector, &st_Normal);

	Test = 1;
	Test <<= ((*((LONG *) &Val)) & 0x80000000) ? 0 : 1;

	MATH_SubVector(&st_Vector, _pst_B2, _pst_A1);
	Val = MATH_f_DotProduct(&st_Vector, &st_Normal);

	Test <<= ((*((LONG *) &Val)) & 0x80000000) ? 0 : 1;

	MATH_SubVector(&st_Vector, _pst_B3, _pst_A1);
	Val = MATH_f_DotProduct(&st_Vector, &st_Normal);

	Test <<= ((*((LONG *) &Val)) & 0x80000000) ? 0 : 1;

	if(!(Test & 0x0E)) return FALSE;

	/* Rejection Test with Triangle 2 */
	MATH_SubVector(&st_T1T2, _pst_B2, _pst_B1);
	MATH_SubVector(&st_T1T3, _pst_B3, _pst_B1);
	MATH_CrossProduct(&st_Normal, &st_T1T2, &st_T1T3);

	/* Rejection Test */
	MATH_SubVector(&st_Vector, _pst_A1, _pst_B1);
	Val = MATH_f_DotProduct(&st_Vector, &st_Normal);

	Test = 1;
	Test <<= ((*((LONG *) &Val)) & 0x80000000) ? 0 : 1;

	MATH_SubVector(&st_Vector, _pst_A2, _pst_B1);
	Val = MATH_f_DotProduct(&st_Vector, &st_Normal);

	Test <<= ((*((LONG *) &Val)) & 0x80000000) ? 0 : 1;

	MATH_SubVector(&st_Vector, _pst_A3, _pst_B1);
	Val = MATH_f_DotProduct(&st_Vector, &st_Normal);

	Test <<= ((*((LONG *) &Val)) & 0x80000000) ? 0 : 1;

	if(!(Test & 0x0E)) return FALSE;

	/* Edge 1 */
	MATH_SubVector(&st_RayUnit, _pst_B2, _pst_B1);
	RayMax = MATH_f_NormVector(&st_RayUnit);
	MATH_DivEqualVector(&st_RayUnit, RayMax);

	if
	(
		INT_FullRayTriangle(_pst_B1, &st_RayUnit, _pst_A1, _pst_A2, _pst_A3, &RayDist, _pst_CollidedPoint, FALSE)
	&&	(RayDist < RayMax)
	)
	{
		return TRUE;
	}

	/* Edge 2 */
	MATH_SubVector(&st_RayUnit, _pst_B3, _pst_B2);
	RayMax = MATH_f_NormVector(&st_RayUnit);
	MATH_DivEqualVector(&st_RayUnit, RayMax);

	if
	(
		INT_FullRayTriangle(_pst_B2, &st_RayUnit, _pst_A1, _pst_A2, _pst_A3, &RayDist, _pst_CollidedPoint, FALSE)
	&&	(RayDist < RayMax)
	)
	{
		return TRUE;
	}

	/* Edge 3 */
	MATH_SubVector(&st_RayUnit, _pst_B3, _pst_B1);
	RayMax = MATH_f_NormVector(&st_RayUnit);
	MATH_DivEqualVector(&st_RayUnit, RayMax);

	if
	(
		INT_FullRayTriangle(_pst_B1, &st_RayUnit, _pst_A1, _pst_A2, _pst_A3, &RayDist, _pst_CollidedPoint, FALSE)
	&&	(RayDist < RayMax)
	)
	{
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:

    Note:
 =======================================================================================================================
 */
ULONG INT_ul_GetPositionPointWithBox
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max
)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ulOutCode;
	/*~~~~~~~~~~~~~~*/

	ulOutCode = 0;

	if(_pst_Point->x < _pst_Min->x)
		ulOutCode |= 0x1;
	else if(_pst_Point->x > _pst_Max->x) 
		ulOutCode |= 0x2;

	if(_pst_Point->y < _pst_Min->y)
		ulOutCode |= 0x4;
	else if(_pst_Point->y > _pst_Max->y) 
		ulOutCode |= 0x8;

	if(_pst_Point->z < _pst_Min->z)
		ulOutCode |= 0x10;
	else if(_pst_Point->z > _pst_Max->z) 
		ulOutCode |= 0x20;


	/* We want to test all */
	ulOutCode |= 0x3F; 

	return ulOutCode;
}

/*
 =======================================================================================================================
    Aim:

    Note:
 =======================================================================================================================
 */
ULONG INT_ul_GetPositionPointWithEdgeBox
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max,
	MATH_tdst_Vector	*_pst_Delta,
	MATH_tdst_Vector	*_pst_NegDelta
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulOutCode;
	MATH_tdst_Vector	st_MulDeltaPoint;
	MATH_tdst_Vector	st_MulNegDeltaPoint;
	MATH_tdst_Vector	st_MulDeltaMax;
	MATH_tdst_Vector	st_MulNegDeltaMin;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulOutCode = 0;

	st_MulNegDeltaPoint.x = _pst_NegDelta->x * _pst_Point->x;
	st_MulNegDeltaPoint.y = _pst_NegDelta->y * _pst_Point->y;
	st_MulNegDeltaPoint.z = _pst_NegDelta->z * _pst_Point->z;

	st_MulDeltaMax.x = _pst_Delta->x * _pst_Max->x;
	st_MulDeltaMax.y = _pst_Delta->y * _pst_Max->y;
	st_MulDeltaMax.z = _pst_Delta->z * _pst_Max->z;

	st_MulDeltaPoint.x = _pst_Delta->x * _pst_Point->x;
	st_MulDeltaPoint.y = _pst_Delta->y * _pst_Point->y;
	st_MulDeltaPoint.z = _pst_Delta->z * _pst_Point->z;

	st_MulNegDeltaMin.x = _pst_NegDelta->x * _pst_Min->x;
	st_MulNegDeltaMin.y = _pst_NegDelta->y * _pst_Min->y;
	st_MulNegDeltaMin.z = _pst_NegDelta->z * _pst_Min->z;

	if((st_MulDeltaPoint.x + st_MulDeltaPoint.y) > (st_MulDeltaMax.x + st_MulDeltaMax.y)) ulOutCode |= 0x001;
	if((st_MulDeltaPoint.x + st_MulNegDeltaPoint.y) > (st_MulDeltaMax.x + st_MulNegDeltaMin.y)) ulOutCode |= 0x002;
	if((st_MulNegDeltaPoint.x + st_MulDeltaPoint.y) > (st_MulNegDeltaMin.x + st_MulDeltaMax.y)) ulOutCode |= 0x004;
	if((st_MulNegDeltaPoint.x + st_MulNegDeltaPoint.y) > (st_MulNegDeltaMin.x + st_MulNegDeltaMin.y))
		ulOutCode |= 0x008;

	if((st_MulDeltaPoint.x + st_MulDeltaPoint.z) > (st_MulDeltaMax.x + st_MulDeltaMax.z)) ulOutCode |= 0x010;
	if((st_MulDeltaPoint.x + st_MulNegDeltaPoint.z) > (st_MulDeltaMax.x + st_MulNegDeltaMin.z)) ulOutCode |= 0x020;
	if((st_MulNegDeltaPoint.x + st_MulDeltaPoint.z) > (st_MulNegDeltaMin.x + st_MulDeltaMax.z)) ulOutCode |= 0x040;
	if((st_MulNegDeltaPoint.x + st_MulNegDeltaPoint.z) > (st_MulNegDeltaMin.x + st_MulNegDeltaMin.z))
		ulOutCode |= 0x080;

	if((st_MulDeltaPoint.y + st_MulDeltaPoint.z) > (st_MulDeltaMax.y + st_MulDeltaMax.z)) ulOutCode |= 0x100;
	if((st_MulDeltaPoint.y + st_MulNegDeltaPoint.z) > (st_MulDeltaMax.y + st_MulNegDeltaMin.z)) ulOutCode |= 0x200;
	if((st_MulNegDeltaPoint.y + st_MulDeltaPoint.z) > (st_MulNegDeltaMin.y + st_MulDeltaMax.z)) ulOutCode |= 0x400;
	if((st_MulNegDeltaPoint.y + st_MulNegDeltaPoint.z) > (st_MulNegDeltaMin.y + st_MulNegDeltaMin.z))
		ulOutCode |= 0x800;

	/* We want to test all */
	ulOutCode |= 0xFFF;

	return ulOutCode;
}

/*
 =======================================================================================================================
    Aim:

    Note:
 =======================================================================================================================
 */
BOOL INT_b_InXYRange(MATH_tdst_Vector *_pst_Point, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	if(_pst_Point->x < _pst_Min->x) return FALSE;
	if(_pst_Point->x > _pst_Max->x) return FALSE;

	if(_pst_Point->y < _pst_Min->y) return FALSE;
	if(_pst_Point->y > _pst_Max->y) return FALSE;

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:

    Note:
 =======================================================================================================================
 */
BOOL INT_b_InYZRange(MATH_tdst_Vector *_pst_Point, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	if(_pst_Point->y < _pst_Min->y) return FALSE;
	if(_pst_Point->y > _pst_Max->y) return FALSE;

	if(_pst_Point->z < _pst_Min->z) return FALSE;
	if(_pst_Point->z > _pst_Max->z) return FALSE;

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:

    Note:
 =======================================================================================================================
 */
BOOL INT_b_InXZRange(MATH_tdst_Vector *_pst_Point, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	if(_pst_Point->x < _pst_Min->x) return FALSE;
	if(_pst_Point->x > _pst_Max->x) return FALSE;

	if(_pst_Point->z < _pst_Min->z) return FALSE;
	if(_pst_Point->z > _pst_Max->z) return FALSE;

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:

    Note:   Which of the twelve edge plane(s) is point P outside of?
 =======================================================================================================================
 */
int INT_i_IntersectEdgeWithBoxWithHit
(
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_AB,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max,
	ULONG				ulExploredPlan,
	MATH_tdst_Vector	*_apst_hit
)
{
	/*~~~~~~~*/
	float	xT;
	int 	Col;
	float	f_1, f_2;
	/*~~~~~~~*/

	Col = 0;

	if(ulExploredPlan & 0x1)
	{
#ifdef JADEFUSION 
		if(_pst_AB->x != 0.0f)
#endif
		{
		xT = (_pst_Min->x - _pst_A->x) / _pst_AB->x;
		_apst_hit[Col].x = _pst_Min->x;
		_apst_hit[Col].y = (_pst_AB->y * xT) + _pst_A->y;
		_apst_hit[Col].z = (_pst_AB->z * xT) + _pst_A->z;

		if(INT_b_InYZRange(&_apst_hit[Col], _pst_Min, _pst_Max))
		{

			{
				f_1 = _apst_hit[Col].y - _pst_Min->y;
				f_2 = _pst_Max->y - _apst_hit[Col].y;

				_apst_hit[Col].y = (f_1 < f_2) ? _pst_Min->y : _pst_Max->y;

				f_1 = _apst_hit[Col].z - _pst_Min->z;
				f_2 = _pst_Max->z - _apst_hit[Col].z;

				_apst_hit[Col].z = (f_1 < f_2) ? _pst_Min->z : _pst_Max->z;
			}

			Col ++;
			}
		}
	}

	if(ulExploredPlan & 0x2)
	{
#ifdef JADEFUSION
		if(_pst_AB->x != 0.0f)
#endif        
		{

		xT = (_pst_Max->x - _pst_A->x) / _pst_AB->x;
		_apst_hit[Col].x = _pst_Max->x;
		_apst_hit[Col].y = (_pst_AB->y * xT) + _pst_A->y;
		_apst_hit[Col].z = (_pst_AB->z * xT) + _pst_A->z;

		if(INT_b_InYZRange(&_apst_hit[Col], _pst_Min, _pst_Max)) 
		{
			{
				f_1 = _apst_hit[Col].y - _pst_Min->y;
				f_2 = _pst_Max->y - _apst_hit[Col].y;

				_apst_hit[Col].y = (f_1 < f_2) ? _pst_Min->y : _pst_Max->y;

				f_1 = _apst_hit[Col].z - _pst_Min->z;
				f_2 = _pst_Max->z - _apst_hit[Col].z;

				_apst_hit[Col].z = (f_1 < f_2) ? _pst_Min->z : _pst_Max->z;
			}

			Col ++;
		}
	}
	}
	if(ulExploredPlan & 0x4)
	{
#ifdef JADEFUSION
		if(_pst_AB->y != 0.0f)
#endif
		{
		xT = (_pst_Min->y - _pst_A->y) / _pst_AB->y;

		_apst_hit[Col].x = (_pst_AB->x * xT) + _pst_A->x;
		_apst_hit[Col].y = _pst_Min->y;
		_apst_hit[Col].z = (_pst_AB->z * xT) + _pst_A->z;

		if(INT_b_InXZRange(&_apst_hit[Col], _pst_Min, _pst_Max)) 
		{
			{
				f_1 = _apst_hit[Col].x - _pst_Min->x;
				f_2 = _pst_Max->x - _apst_hit[Col].x;

				_apst_hit[Col].x = (f_1 < f_2) ? _pst_Min->x : _pst_Max->x;

				f_1 = _apst_hit[Col].z - _pst_Min->z;
				f_2 = _pst_Max->z - _apst_hit[Col].z;

				_apst_hit[Col].z = (f_1 < f_2) ? _pst_Min->z : _pst_Max->z;
			}

			Col ++;
		}
	}
	}
	if(ulExploredPlan & 0x8)
	{
#ifdef JADEFUSION
		if(_pst_AB->y != 0.0f)
#endif 
		{
		xT = (_pst_Max->y - _pst_A->y) / _pst_AB->y;
		_apst_hit[Col].x = (_pst_AB->x * xT) + _pst_A->x;
		_apst_hit[Col].y = _pst_Max->y;
		_apst_hit[Col].z = (_pst_AB->z * xT) + _pst_A->z;

		if(INT_b_InXZRange(&_apst_hit[Col], _pst_Min, _pst_Max)) 
		{
			{
				f_1 = _apst_hit[Col].x - _pst_Min->x;
				f_2 = _pst_Max->x - _apst_hit[Col].x;

				_apst_hit[Col].x = (f_1 < f_2) ? _pst_Min->x : _pst_Max->x;

				f_1 = _apst_hit[Col].z - _pst_Min->z;
				f_2 = _pst_Max->z - _apst_hit[Col].z;

				_apst_hit[Col].z = (f_1 < f_2) ? _pst_Min->z : _pst_Max->z;
			}

			Col++;
		}
	}
	}
	if(ulExploredPlan & 0x10)
	{
#ifdef JADEFUSION
		if(_pst_AB->z != 0.0f)
#endif
		{
		xT = (_pst_Min->z - _pst_A->z) / _pst_AB->z;
		_apst_hit[Col].x = (_pst_AB->x * xT) + _pst_A->x;
		_apst_hit[Col].y = (_pst_AB->y * xT) + _pst_A->y;
		_apst_hit[Col].z = _pst_Min->z;

		if(INT_b_InXYRange(&_apst_hit[Col], _pst_Min, _pst_Max)) 
		{
			{
				f_1 = _apst_hit[Col].x - _pst_Min->x;
				f_2 = _pst_Max->x - _apst_hit[Col].x;

				_apst_hit[Col].x = (f_1 < f_2) ? _pst_Min->x : _pst_Max->x;

				f_1 = _apst_hit[Col].y - _pst_Min->y;
				f_2 = _pst_Max->y - _apst_hit[Col].y;

				_apst_hit[Col].y = (f_1 < f_2) ? _pst_Min->y : _pst_Max->y;
			}

			Col++;
		}
	}
	}
	if(ulExploredPlan & 0x20)
	{
 #ifdef JADEFUSION
		if(_pst_AB->z != 0.0f)
 #endif
		{
		xT = (_pst_Max->z - _pst_A->z) / _pst_AB->z;
		_apst_hit[Col].x = (_pst_AB->x * xT) + _pst_A->x;
		_apst_hit[Col].y = (_pst_AB->y * xT) + _pst_A->y;
		_apst_hit[Col].z = _pst_Max->z;

		if(INT_b_InXYRange(&_apst_hit[Col], _pst_Min, _pst_Max)) 
		{
			{
				f_1 = _apst_hit[Col].x - _pst_Min->x;
				f_2 = _pst_Max->x - _apst_hit[Col].x;

				_apst_hit[Col].x = (f_1 < f_2) ? _pst_Min->x : _pst_Max->x;

				f_1 = _apst_hit[Col].y - _pst_Min->y;
				f_2 = _pst_Max->y - _apst_hit[Col].y;

				_apst_hit[Col].y = (f_1 < f_2) ? _pst_Min->y : _pst_Max->y;
			}

			Col++;;
		}
	}
	}
	/*
	if(Col == 2)
	{
		MATH_AddEqualVector(&_apst_hit[Col - 2], &_apst_hit[Col - 1]);
		MATH_MulEqualVector(&_apst_hit[Col - 2], 0.5f);

		Col = 1;
	}
	*/

	return Col;
}

/*
 =======================================================================================================================
    Aim:

    Note:
 =======================================================================================================================
 */
extern "C" BOOL INT_FullAABBoxTriangle(
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_C,
	MATH_tdst_Vector	*_pst_Normal,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max,
	MATH_tdst_Vector	*_apst_Hit,
	ULONG				*_pul_NumberOfCollisions,
	ULONG				*_apul_CollisionType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Delta, st_NegDelta, st_Edge;
	MATH_tdst_Vector	st_Hit;
	ULONG				ul_V1Test, ul_V2Test, ul_V3Test;
	float				fPlan, fDot, fNum, fDiag;
	BOOL				b_Collision;
	int					numcol, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pul_NumberOfCollisions = 0;
	_apul_CollisionType[0] = 0;


	b_Collision = FALSE;

	/*
	ul_V1Test = INT_ul_GetPositionPointWithBox(_pst_A, _pst_Min, _pst_Max);
	ul_V2Test = INT_ul_GetPositionPointWithBox(_pst_B, _pst_Min, _pst_Max);
	ul_V3Test = INT_ul_GetPositionPointWithBox(_pst_C, _pst_Min, _pst_Max);
	*/
	ul_V1Test = 0x3F;
	ul_V2Test = 0x3F;
	ul_V3Test = 0x3F;


	/* les trois points sont dans le meme demi espace exterieur */
//	if(ul_V1Test & ul_V2Test & ul_V3Test) return FALSE;

	/* Delta de la boite */
	MATH_SubVector(&st_Delta, _pst_Max, _pst_Min);
	MATH_NegVector(&st_NegDelta, &st_Delta);

	/* Test sur les demi espaces des aretes */
	/*
	ul_V1Test |= INT_ul_GetPositionPointWithEdgeBox(_pst_A, _pst_Min, _pst_Max, &st_Delta, &st_NegDelta) << 8;
	ul_V2Test |= INT_ul_GetPositionPointWithEdgeBox(_pst_B, _pst_Min, _pst_Max, &st_Delta, &st_NegDelta) << 8;
	ul_V3Test |= INT_ul_GetPositionPointWithEdgeBox(_pst_C, _pst_Min, _pst_Max, &st_Delta, &st_NegDelta) << 8;
	*/

	ul_V1Test |= 0xFFF;
	ul_V2Test |= 0xFFF;
	ul_V3Test |= 0xFFF;

//	if(ul_V1Test & ul_V2Test & ul_V3Test) return FALSE;

	/* plan du triangle */
	fPlan = -(MATH_f_DotProduct(_pst_Normal, _pst_A));

	/*$F-------------------------------------------------

		 Intersections avec les quatre diagonales 

	   -------------------------------------------------*/
	fDot = (_pst_Normal->x * st_Delta.x) + (_pst_Normal->y * st_Delta.y) + (_pst_Normal->z * st_Delta.z);
	if(fDot != 0.0f)
	{
		fNum = (_pst_Normal->x * _pst_Min->x) + (_pst_Normal->y * _pst_Min->y) + (_pst_Normal->z * _pst_Min->z) + fPlan;
		fDiag = -(fNum / fDot);

		if((fDiag > 0.0f) && (fDiag < 1.0f))
		{
			st_Hit.x = (st_Delta.x * fDiag) + _pst_Min->x;
			st_Hit.y = (st_Delta.y * fDiag) + _pst_Min->y;
			st_Hit.z = (st_Delta.z * fDiag) + _pst_Min->z;

			if(INT_PointInTriangle(&st_Hit, _pst_A, _pst_B, _pst_C, _pst_Normal, NULL))
			{
				if(fDiag > 0.5f)
				{
					st_Hit.x = _pst_Max->x;
					st_Hit.y = _pst_Max->y;
					st_Hit.z = _pst_Max->z;
				}
				else
				{
					st_Hit.x = _pst_Min->x;
					st_Hit.y = _pst_Min->y;
					st_Hit.z = _pst_Min->z;
				}

				if(_apul_CollisionType) _apul_CollisionType[(*_pul_NumberOfCollisions)] |= COL_Cul_ZDMBox + COL_Cul_Triangle;
				MATH_CopyVector(&_apst_Hit[(*_pul_NumberOfCollisions)++], &st_Hit);
				_apul_CollisionType[(*_pul_NumberOfCollisions)] = 0;
				b_Collision = TRUE;
			}
		}
	}

	fDot = (_pst_Normal->x * st_Delta.x) + (_pst_Normal->y * st_NegDelta.y) + (_pst_Normal->z * st_Delta.z);
	if(fDot != 0.0f)
	{
		fNum = (_pst_Normal->x * _pst_Min->x) + (_pst_Normal->y * _pst_Max->y) + (_pst_Normal->z * _pst_Min->z) + fPlan;
		fDiag = -(fNum / fDot);

		if((fDiag > 0.0f) && (fDiag < 1.0f))
		{
			st_Hit.x = (st_Delta.x * fDiag) + _pst_Min->x;
			st_Hit.y = (st_NegDelta.y * fDiag) + _pst_Max->y;
			st_Hit.z = (st_Delta.z * fDiag) + _pst_Min->z;

			if(INT_PointInTriangle(&st_Hit, _pst_A, _pst_B, _pst_C, _pst_Normal, NULL))
			{
				if(fDiag > 0.5f)
				{
					st_Hit.x = _pst_Max->x;
					st_Hit.y = _pst_Min->y;
					st_Hit.z = _pst_Max->z;
				}
				else
				{
					st_Hit.x = _pst_Min->x;
					st_Hit.y = _pst_Max->y;
					st_Hit.z = _pst_Min->z;
				}

				if(_apul_CollisionType) _apul_CollisionType[(*_pul_NumberOfCollisions)] |= COL_Cul_ZDMBox + COL_Cul_Triangle;
				MATH_CopyVector(&_apst_Hit[(*_pul_NumberOfCollisions)++], &st_Hit);
				_apul_CollisionType[(*_pul_NumberOfCollisions)] = 0;
				b_Collision = TRUE;
			}
		}
	}

	fDot = (_pst_Normal->x * st_Delta.x) + (_pst_Normal->y * st_Delta.y) + (_pst_Normal->z * st_NegDelta.z);
	if(fDot != 0.0f)
	{
		fNum = (_pst_Normal->x * _pst_Min->x) + (_pst_Normal->y * _pst_Min->y) + (_pst_Normal->z * _pst_Max->z) + fPlan;
		fDiag = -(fNum / fDot);

		if((fDiag > 0.0f) && (fDiag < 1.0f))
		{
			st_Hit.x = (st_Delta.x * fDiag) + _pst_Min->x;
			st_Hit.y = (st_Delta.y * fDiag) + _pst_Min->y;
			st_Hit.z = (st_NegDelta.z * fDiag) + _pst_Max->z;

			if(INT_PointInTriangle(&st_Hit, _pst_A, _pst_B, _pst_C, _pst_Normal, NULL))
			{
				if(fDiag > 0.5f)
				{
					st_Hit.x = _pst_Max->x;
					st_Hit.y = _pst_Max->y;
					st_Hit.z = _pst_Min->z;
				}
				else
				{
					st_Hit.x = _pst_Min->x;
					st_Hit.y = _pst_Min->y;
					st_Hit.z = _pst_Max->z;
				}

				if(_apul_CollisionType) _apul_CollisionType[(*_pul_NumberOfCollisions)] |= COL_Cul_ZDMBox + COL_Cul_Triangle;
				MATH_CopyVector(&_apst_Hit[(*_pul_NumberOfCollisions)++], &st_Hit);
				_apul_CollisionType[(*_pul_NumberOfCollisions)] = 0;
				b_Collision = TRUE;
			}
		}
	}

	fDot = (_pst_Normal->x * st_Delta.x) + (_pst_Normal->y * st_NegDelta.y) + (_pst_Normal->z * st_NegDelta.z);
	if(fDot != 0.0f)
	{
		fNum = (_pst_Normal->x * _pst_Min->x) + (_pst_Normal->y * _pst_Max->y) + (_pst_Normal->z * _pst_Max->z) + fPlan;
		fDiag = -(fNum / fDot);

		if((fDiag > 0.0f) && (fDiag < 1.0f))
		{
			st_Hit.x = (st_Delta.x * fDiag) + _pst_Min->x;
			st_Hit.y = (st_NegDelta.y * fDiag) + _pst_Max->y;
			st_Hit.z = (st_NegDelta.z * fDiag) + _pst_Max->z;

			if(INT_PointInTriangle(&st_Hit, _pst_A, _pst_B, _pst_C, _pst_Normal, NULL))
			{
				if(fDiag > 0.5f)
				{
					st_Hit.x = _pst_Max->x;
					st_Hit.y = _pst_Min->y;
					st_Hit.z = _pst_Min->z;
				}
				else
				{
					st_Hit.x = _pst_Min->x;
					st_Hit.y = _pst_Max->y;
					st_Hit.z = _pst_Max->z;
				}

				if(_apul_CollisionType) _apul_CollisionType[(*_pul_NumberOfCollisions)] |= COL_Cul_ZDMBox + COL_Cul_Triangle;
				MATH_CopyVector(&_apst_Hit[(*_pul_NumberOfCollisions)++], &st_Hit);
				_apul_CollisionType[(*_pul_NumberOfCollisions)] = 0;
				b_Collision = TRUE;
			}
		}
	}

//	if(!b_Collision)
	{
//		if((ul_V1Test & ul_V2Test) == 0)
		{
			MATH_SubVector(&st_Edge, _pst_B, _pst_A);
			numcol = 				
				INT_i_IntersectEdgeWithBoxWithHit
				(
					_pst_A,
					_pst_B,
					&st_Edge,
					_pst_Min,
					_pst_Max,
					ul_V1Test | ul_V2Test,
					&_apst_Hit[*_pul_NumberOfCollisions]
				);

			if(numcol)
			{
				b_Collision = TRUE;
				for(i = 0; i < numcol; i++)
				{
					_apul_CollisionType[(*_pul_NumberOfCollisions)] |= COL_Cul_ZDMBox + COL_Cul_Triangle ; //COL_Cul_Edge12;
					(*_pul_NumberOfCollisions)++;	
					_apul_CollisionType[(*_pul_NumberOfCollisions)] = 0;
				}
			}
		}

//		if((ul_V1Test & ul_V3Test) == 0)
		{
			MATH_SubVector(&st_Edge, _pst_C, _pst_A);
			numcol = 
				INT_i_IntersectEdgeWithBoxWithHit
				(
					_pst_A,
					_pst_C,
					&st_Edge,
					_pst_Min,
					_pst_Max,
					ul_V1Test | ul_V3Test,
					&_apst_Hit[*_pul_NumberOfCollisions]
				);

			if(numcol)
			{
				b_Collision = TRUE;
				for(i = 0; i < numcol; i++)
				{
					_apul_CollisionType[(*_pul_NumberOfCollisions)] |= COL_Cul_ZDMBox + COL_Cul_Triangle ; //COL_Cul_Edge13;
					(*_pul_NumberOfCollisions)++;	
					_apul_CollisionType[(*_pul_NumberOfCollisions)] = 0;
				}
			}
		}

//		if((ul_V2Test & ul_V3Test) == 0)
		{
			MATH_SubVector(&st_Edge, _pst_C, _pst_B);
			numcol = 
				INT_i_IntersectEdgeWithBoxWithHit
				(
					_pst_B,
					_pst_C,
					&st_Edge,
					_pst_Min,
					_pst_Max,
					ul_V2Test | ul_V3Test,
					&_apst_Hit[*_pul_NumberOfCollisions]
				);

			if(numcol)
			{
				b_Collision = TRUE;
				for(i = 0; i < numcol; i++)
				{
					_apul_CollisionType[(*_pul_NumberOfCollisions)] |= COL_Cul_ZDMBox + COL_Cul_Triangle; //COL_Cul_Edge23;
					(*_pul_NumberOfCollisions)++;	
					_apul_CollisionType[(*_pul_NumberOfCollisions)] = 0;
				}
			}
		}
	}

	return b_Collision;
}
BOOL INT_SegmentSphere( MATH_tdst_Vector * _pst_A,
                        MATH_tdst_Vector * _pst_B,
                        MATH_tdst_Vector * _pst_Center,
                        float              _f_Radius,
                        MATH_tdst_Vector * _pst_Hit )
{
    MATH_tdst_Vector st_Direction;
    MATH_tdst_Vector st_OriginCenter;
	float a, b, c,t,fDenum;
	float fDeterminant;
	BOOL b_Collision;

    MATH_SubVector( &st_Direction, _pst_B, _pst_A );
    MATH_SubVector( &st_OriginCenter, _pst_Center, _pst_A );

    a = MATH_f_DotProduct( &st_Direction, &st_Direction );
    b = 2.0f * MATH_f_DotProduct( &st_Direction, &st_OriginCenter );
    c = MATH_f_DotProduct( &st_OriginCenter, &st_OriginCenter ) - (_f_Radius * _f_Radius);

    //
    // Solve quadratic
    //
    fDeterminant =  b*b - 4.0f * a * c;
    if( fDeterminant < 0.0f )
    {
        // No solution
        return FALSE;
    }

    b_Collision = FALSE;
    t = 0.0f;

    fDenum = 2.0f * a;

    if( fabs(fDeterminant) <= 1E-3f ) 
    {
        // There is only one solution, the line catches the edge of the sphere
        t = -b / fDenum;

        // See if the intersection is within the segment
        if( t >= 0.0f && t <= 1.0f )
        {
            b_Collision = TRUE;
        }
    }
    else
    {
        // There might be 2 solutions
        float fDeterminantSqrt = fSqrt( fDeterminant );
        float t1 = (-b +  fDeterminantSqrt) / fDenum;
        float t2 = (-b -  fDeterminantSqrt) / fDenum;

        if( t1 >= 0.0f && t1 <= 1.0f )
        {
            b_Collision = TRUE;
            t = t1;
        }

        if( t2 >= 0.0f && t2 <= 1.0f )
        {
            // If t1 was also good, we will return the closest one to the origin
            if( b_Collision )
            {
                if( t2 < t1 )
                {
                    t = t2;
                }
            }
            else
            {
                b_Collision = TRUE;
                t = t2;
            }
        }
    }

    if( b_Collision && _pst_Hit != NULL )
    {
        MATH_AddScaleVector( _pst_Hit, _pst_A, &st_Direction, t );
    }

    return b_Collision;
}
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
