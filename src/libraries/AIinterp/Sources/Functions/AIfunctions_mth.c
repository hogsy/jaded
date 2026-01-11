/*$T AIfunctions_mth.c GC 1.134 05/05/04 16:00:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "TEXture/TEX_MESH.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
float	AI_gf_Epsilon = Cf_EpsilonBig;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int Find
(
	MATH_tdst_Vector	*rkP0,
	MATH_tdst_Vector	*rkD0,
	MATH_tdst_Vector	*rkP1,
	MATH_tdst_Vector	*rkD1,
	MATH_tdst_Vector	*rkDiff,
	float				*rfD0SqrLen,
	int					*riQuantity,
	float				afT[2]
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	/*
	 * Intersection is a solution to P0+s*D0 = P1+t*D1. Rewrite as £
	 * s*D0 - t*D1 = P1 - P0, a 2x2 system of equations. If D0 = (x0,y0) £
	 * and D1 = (x1,y1) and P1 - P0 = (c0,c1), then the system is £
	 * x0*s - x1*t = c0 and y0*s - y1*t = c1. The error tests are relative £
	 * to the size of the direction vectors, |Cross(D0,D1)| >= e*|D0|*|D1| £
	 * rather than absolute tests |Cross(D0,D1)| >= e. The quantities £
	 * P1-P0, |D0|^2, and |D1|^2 are returned for use by calling functions.
	 */
	float				fDet;
	MATH_tdst_Vector	vv;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CrossProduct(&vv, rkD1, rkD0);
	fDet = vv.z;

	MATH_SubVector(rkDiff, rkP1, rkP0);
	*rfD0SqrLen = MATH_f_DotProduct(rkD0, rkD0);

	if(fDet * fDet > Cf_Epsilon **rfD0SqrLen * MATH_f_DotProduct(rkD1, rkD1))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		/*
		 * Lines intersect in a single point. Return both s and t values for £
		 * use by calling functions.
		 */
		float	fInvDet = ((float) 1.0) / fDet;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		*riQuantity = 1;

		MATH_CrossProduct(&vv, rkD1, rkDiff);
		afT[0] = vv.z * fInvDet;
		MATH_CrossProduct(&vv, rkD0, rkDiff);
		afT[1] = vv.z * fInvDet;
	}
	else
	{
		/*~~~~~~~~~*/
		float	fRHS;
		/*~~~~~~~~~*/

		/* lines are parallel */
		MATH_CrossProduct(&vv, rkD0, rkDiff);
		fDet = vv.z;
		fRHS = Cf_Epsilon * (*rfD0SqrLen) * MATH_f_DotProduct(rkDiff, rkDiff);

		if(fDet * fDet > fRHS)
		{
			/* lines are disjoint */
			*riQuantity = 0;
		}
		else
		{
			/* lines are the same */
			*riQuantity = 2;
		}
	}

	return *riQuantity != 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int FindIntersectionRaySeg
(
	MATH_tdst_Vector	*rayorigin,
	MATH_tdst_Vector	*raydir,
	MATH_tdst_Vector	*segorigin,
	MATH_tdst_Vector	*segdest,
	float				afT[2]
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	kDiff;
	float				fD0SqrLen;
	int					bIntersects;
	int					riQuantity;
	MATH_tdst_Vector	segdir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&segdir, segdest, segorigin);
	bIntersects = Find(rayorigin, raydir, segorigin, &segdir, &kDiff, &fD0SqrLen, &riQuantity, afT);
	if(bIntersects)
	{
		if(riQuantity == 1)
		{
			if(afT[0] < (float) 0.0 || afT[1] < (float) 0.0 || afT[1] > (float) 1.0)
			{
				/* lines intersect, but ray and segment do not */
				riQuantity = 0;
			}
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			/* ray and segment are on the same line */
			float	fDotRS = MATH_f_DotProduct(raydir, &segdir);
			float	fDot0, fDot1;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(fDotRS > (float) 0.0)
			{
				fDot0 = MATH_f_DotProduct(&kDiff, raydir);
				fDot1 = fDot0 + fDotRS;
			}
			else
			{
				fDot1 = MATH_f_DotProduct(raydir, raydir);
				fDot0 = fDot1 + fDotRS;
			}

			/* compute intersection of [t0,t1] and [0,+infinity] */
			if(fDot0 >= (float) 0.0)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				/* complete overlap */
				float	fInvLen = ((float) 1.0) / fD0SqrLen;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				afT[0] = fDot0 * fInvLen;
				afT[1] = fDot1 * fInvLen;
			}
			else if(fDot1 > (float) 0.0)
			{
				/* partial overlap */
				afT[0] = (float) 0.0;
				afT[1] = fDot1 / fD0SqrLen;
			}
			else if(fDot1 < (float) 0.0)
			{
				/* no overlap */
				riQuantity = 0;
			}
			else
			{
				/* overlap at a single end point */
				riQuantity = 1;
				afT[0] = (float) 0.0;
			}
		}
	}

	return riQuantity;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATHRaySeg_C
(
	MATH_tdst_Vector	*raypos,
	MATH_tdst_Vector	*raydir,
	MATH_tdst_Vector	*segpos1,
	MATH_tdst_Vector	*segpos2,
	MATH_tdst_Vector	*res
)
{
	/*~~~~~~~~~~~*/
	int		result;
	float	atf[2];
	/*~~~~~~~~~~~*/

	res->x = res->y = res->z = 0;
	result = FindIntersectionRaySeg(raypos, raydir, segpos1, segpos2, atf);
	if(result)
	{
		MATH_ScaleVector(res, raydir, atf[0]);
		MATH_AddEqualVector(res, raypos);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHRaySeg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*raypos, *raydir, *segpos1, *segpos2;
	MATH_tdst_Vector	res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	segpos2 = AI_PopVectorPtr();
	segpos1 = AI_PopVectorPtr();
	raydir = AI_PopVectorPtr();
	raypos = AI_PopVectorPtr();

	AI_EvalFunc_MATHRaySeg_C(raypos, raydir, segpos1, segpos2, &res);
	AI_PushVector(&res);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATHBitSet_C(int i, int bit)
{
	i |= (1 << bit);
	return i;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHBitSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	int i, bit;
	/*~~~~~~~*/

	bit = AI_PopInt();
	i = AI_PopInt();

	AI_PushInt(AI_EvalFunc_MATHBitSet_C(i, bit));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATHBitChange_C(int i, int bit)
{
	if(i & (1 << bit))
		i &= ~(1 << bit);
	else
		i |= (1 << bit);

	return(i);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHBitChange(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	int i, bit;
	/*~~~~~~~*/

	bit = AI_PopInt();
	i = AI_PopInt();

	AI_PushInt(AI_EvalFunc_MATHBitChange_C(i, bit));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATHBitReset_C(int i, int bit)
{
	i &= ~(1 << bit);
	return(i);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHBitReset(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	int i, bit;
	/*~~~~~~~*/

	bit = AI_PopInt();
	i = AI_PopInt();

	AI_PushInt(AI_EvalFunc_MATHBitReset_C(i, bit));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATHBitTest_C(int i, int bit)
{
	return(i & (1 << bit) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHBitTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	int i, bit;
	/*~~~~~~~*/

	bit = AI_PopInt();
	i = AI_PopInt();

	AI_PushInt(AI_EvalFunc_MATHBitTest_C(i, bit));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHSinus_C(float _f_Angle)
{
	return(fSin(_f_Angle));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHSinus(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	float	f_Angle;
	/*~~~~~~~~~~~~*/

	f_Angle = AI_PopFloat();

	AI_PushFloat(AI_EvalFunc_MATHSinus_C(f_Angle));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHCosinus_C(float _f_Angle)
{
	return(fCos(_f_Angle));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHCosinus(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	float	f_Angle;
	/*~~~~~~~~~~~~*/

	f_Angle = AI_PopFloat();

	AI_PushFloat(AI_EvalFunc_MATHCosinus_C(f_Angle));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHArcSinus_C(float _f_Angle)
{
	return(fAsin(_f_Angle));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHArcSinus(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_MATHArcSinus_C(AI_PopFloat()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHArcCosinus_C(float _f_Angle)
{
	return(fAcos(_f_Angle));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHArcCosinus(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_MATHArcCosinus_C(AI_PopFloat()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHExp_C(float _f_Angle)
{
	return(fExp(_f_Angle));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHExp(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_MATHExp_C(AI_PopFloat()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHRandFloat_C(float _f1, float _f2)
{
	if(_f1 == _f2) return _f1;
	return(fRand(_f1, _f2));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHRandFloat(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	float	fmax, fmin;
	/*~~~~~~~~~~~~~~~*/

	fmax = AI_PopFloat();
	fmin = AI_PopFloat();
	AI_PushFloat(AI_EvalFunc_MATHRandFloat_C(fmin, fmax));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATHRandInt_C(int _i1, int _i2)
{
	if(_i1 == _i2) return _i1;
	return(lRand(_i1, _i2));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHRandInt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~*/
	int imin, imax;
	/*~~~~~~~~~~~*/

	imax = AI_PopInt();
	imin = AI_PopInt();
	AI_PushInt(AI_EvalFunc_MATHRandInt_C(imin, imax));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHAbsFloat(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~*/
	float	fVal;
	/*~~~~~~~~~*/

	fVal = AI_PopFloat();
	AI_PushFloat(fAbs(fVal));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHAbsInt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~*/
	int iVal;
	/*~~~~~*/

	iVal = AI_PopInt();
	AI_PushInt(lAbs(iVal));
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Vectors
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecDistance(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v1, *v2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v1 = AI_PopVectorPtr();
	v2 = AI_PopVectorPtr();

	AI_PushFloat(MATH_f_Distance(v1, v2));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecNorm(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(MATH_f_NormVector(AI_PopVectorPtr()));
	return ++_pst_Node;
}


float AI_EvalFunc_MATHVecSquareDistance_C(MATH_tdst_Vector *_pst_v1, MATH_tdst_Vector *_pst_v2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector	vResult;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SubVector(&vResult, _pst_v1, _pst_v2);
    return MATH_f_SqrNormVector(&vResult);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
AI_tdst_Node *AI_EvalFunc_MATHVecSquareDistance(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v1, *v2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v1 = AI_PopVectorPtr();
	v2 = AI_PopVectorPtr();

	AI_PushFloat(AI_EvalFunc_MATHVecSquareDistance_C(v1,v2));

	return ++_pst_Node;}

/*
=======================================================================================================================
=======================================================================================================================
*/
AI_tdst_Node *AI_EvalFunc_MATHVecSquareNorm(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(MATH_f_SqrNormVector(AI_PopVectorPtr()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecNormalize_C(MATH_tdst_Vector *v, MATH_tdst_Vector *dest)
{
	AI_Check(v->x != 0.0f || v->y != 0.0f || v->z != 0.0f, "Normalizing a null vector");
	MATH_NormalizeVector(dest, v);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecNormalize(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	AI_EvalFunc_MATHVecNormalize_C(AI_PopVectorPtr(), &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Change the norm of a vector
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecSetNormVector_C(MATH_tdst_Vector *pst_Vector, float f_norm)
{
	AI_Check(!(MATH_b_NulVector(pst_Vector)), "Changing the norm of a null vector");
	MATH_SetNormVector(pst_Vector, pst_Vector, f_norm);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecSetNormVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Vector;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	float				f_norm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_norm = AI_PopFloat();
	AI_PopVar(&Val1, &st_Var1); /* Vector to normalize */
	pst_Vector = (MATH_tdst_Vector *) st_Var1.pv_Addr;
	AI_EvalFunc_MATHVecSetNormVector_C(pst_Vector, f_norm);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Change the norm of a vector
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecSetNormalize(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Vector;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1); /* Vector to normalize */
	pst_Vector = (MATH_tdst_Vector *) st_Var1.pv_Addr;
	AI_Check(!(MATH_b_NulVector(pst_Vector)), "Normalizing a null vector");
	MATH_NormalizeEqualVector(pst_Vector);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Change the norm of a vector
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecSetHorzNormalize_C(MATH_tdst_Vector *pst_Vector)
{
	AI_Check(!(MATH_b_NulVector(pst_Vector)), "Normalizing a null vector");
	pst_Vector->z = 0.0f;
	AI_Check(!(MATH_b_NulVector(pst_Vector)), "Normalizing a vertical vector");
	MATH_NormalizeEqualVector(pst_Vector);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecSetHorzNormalize(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	MATH_tdst_Vector	*pst_Vector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1); /* Vector to normalize */
	pst_Vector = (MATH_tdst_Vector *) st_Var1.pv_Addr;
	AI_EvalFunc_MATHVecSetHorzNormalize_C(pst_Vector);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecDotProduct(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v1, *v2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v1 = AI_PopVectorPtr();
	v2 = AI_PopVectorPtr();
	AI_PushFloat(MATH_f_DotProduct(v2, v1));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecCrossProduct_C(MATH_tdst_Vector *v2, MATH_tdst_Vector *v1, MATH_tdst_Vector *v3)
{
	MATH_CrossProduct(v3, v2, v1);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecCrossProduct(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v1, *v2, v3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v1 = AI_PopVectorPtr();
	v2 = AI_PopVectorPtr();
	AI_EvalFunc_MATHVecCrossProduct_C(v2, v1, &v3);
	AI_PushVector(&v3);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecGlobalToLocal_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_V,
	MATH_tdst_Vector	*_pst_VDest
)
{
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		OBJ_ComputeGlobalWithLocal(_pst_GO, &st_Temp, 1);
		MATH_VectorGlobalToLocal(_pst_VDest, &st_Temp, _pst_V);
	}
	else
	{
		MATH_VectorGlobalToLocal(_pst_VDest, OBJ_pst_GetAbsoluteMatrix(_pst_GO), _pst_V);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecGlobalToLocal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v, v2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&v);

	AI_EvalFunc_MATHVecGlobalToLocal_C(pst_GO, &v, &v2);

	AI_PushVector(&v2);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecLocalToGlobal_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_V,
	MATH_tdst_Vector	*_pst_VDest
)
{
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		OBJ_ComputeGlobalWithLocal(_pst_GO, &st_Temp, 1);
		MATH_VectorLocalToGlobal(_pst_VDest, &st_Temp, _pst_V);
	}
	else
	{
		MATH_VectorLocalToGlobal(_pst_VDest, OBJ_pst_GetAbsoluteMatrix(_pst_GO), _pst_V);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecLocalToGlobal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v, v2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&v);

	AI_EvalFunc_MATHVecLocalToGlobal_C(pst_GO, &v, &v2);

	AI_PushVector(&v2);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_MTHVecNull_C(MATH_tdst_Vector *_pst_V)
{
	return(MATH_b_NulVector(_pst_V) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHVecNull(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~*/

	v = AI_PopVectorPtr();
	AI_PushInt(AI_EvalFunc_MTHVecNull_C(v));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_MTHVecNullEpsilon_C(MATH_tdst_Vector *_pst_V)
{
	return(MATH_b_NulVectorWithEpsilon(_pst_V, AI_gf_Epsilon) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHVecNullEpsilon(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~*/

	v = AI_PopVectorPtr();
	AI_PushInt(AI_EvalFunc_MTHVecNullEpsilon_C(v));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_MTHVecNullToler_C(MATH_tdst_Vector *_pst_V, float _f_Toler)
{
	return(MATH_b_NulVectorWithEpsilon(_pst_V, _f_Toler) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHVecNullToler(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Toler;
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Toler = AI_PopFloat();
	v = AI_PopVectorPtr();
	AI_PushInt(AI_EvalFunc_MTHVecNullToler_C(v, f_Toler));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Calculates the distance from point "_pst_Point" to line defined by the two points "_pst_LineA" and
    "_pst_LineB"
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHDistPointLine(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Point;
	MATH_tdst_Vector	st_LineA;
	MATH_tdst_Vector	st_LineB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_LineB);
	AI_PopVector(&st_LineA);
	AI_PopVector(&st_Point);

	AI_PushFloat(MATH_f_PointLineDistance(&st_Point, &st_LineA, &st_LineB));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Calculates the coordinates of the projection "_pst_Proj" of the point "_pst_Point" on the line defined by the
    two points "_pst_LineA" and "_pst_LineB"
 =======================================================================================================================
 */
void AI_EvalFunc_MATHProjPointLine_C
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB,
	MATH_tdst_Vector	*_pst_Proj
)
{
	MATH_PointLineProjection(_pst_Proj, _pst_Point, _pst_LineA, _pst_LineB);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHProjPointLine(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Point;
	MATH_tdst_Vector	st_LineA;
	MATH_tdst_Vector	st_LineB;
	MATH_tdst_Vector	st_Proj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_LineB);
	AI_PopVector(&st_LineA);
	AI_PopVector(&st_Point);

	AI_EvalFunc_MATHProjPointLine_C(&st_Point, &st_LineA, &st_LineB, &st_Proj);

	AI_PushVector(&st_Proj);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Calculates the coordinates of the projection "_pst_Proj" of the point "_pst_Point" on the line defined by the
    two points "_pst_LineA" and "_pst_LineB"
 =======================================================================================================================
 */
void AI_EvalFunc_MATHProjPointSegment_C
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB,
	MATH_tdst_Vector	*_pst_Proj
)
{
	MATH_PointSegmentProjection(_pst_Proj, _pst_Point, _pst_LineA, _pst_LineB);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHProjPointSegment(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Point;
	MATH_tdst_Vector	st_LineA;
	MATH_tdst_Vector	st_LineB;
	MATH_tdst_Vector	st_Proj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_LineB);
	AI_PopVector(&st_LineA);
	AI_PopVector(&st_Point);

	AI_EvalFunc_MATHProjPointSegment_C(&st_Point, &st_LineA, &st_LineB, &st_Proj);

	AI_PushVector(&st_Proj);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Find the projection on a 3D parabol Note: The parabol is defined by 3 vertices (point A, point B, origin) and
    one float (distance OA) Ex: MATH_PointParabolProjection(v_M,v_A,v_B,v_O,10)
 =======================================================================================================================
 */
void AI_EvalFunc_MATHProjPointParabol_C
(
	MATH_tdst_Vector	*_pst_X0,
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_O,
	float				_f_r,
	MATH_tdst_Vector	*_pst_X1
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tangent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_PointParabolProjection3D(_pst_X1, &st_Tangent, _pst_X0, _pst_A, _pst_B, _pst_O, _f_r);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHProjPointParabol(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_A;
	MATH_tdst_Vector	st_B;
	MATH_tdst_Vector	st_O;
	float				f_r;
	MATH_tdst_Vector	st_X0;
	MATH_tdst_Vector	st_X1;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	f_r = AI_PopFloat();
	AI_PopVector(&st_O);
	AI_PopVector(&st_B);
	AI_PopVector(&st_A);
	AI_PopVector(&st_X0);	/* Point to project to the parabol */

	AI_EvalFunc_MATHProjPointParabol_C(&st_X0, &st_A, &st_B, &st_O, f_r, &st_X1);

	AI_PushVector(&st_X1);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Make a blend between two matrixes using Slerp between quaternions Ex:
    MATH_MatrixBlend(SightingDest,BankingDest,Sighting1,Banking1,Sighting2,Banking2,coeff) Note: Coeff must be between
    0 and 1
 =======================================================================================================================
 */
void AI_EvalFunc_MATHMatrixBlend_C
(
	MATH_tdst_Vector	*_pst_SightingDest,
	MATH_tdst_Vector	*_pst_BankingDest,
	MATH_tdst_Vector	*_pst_Sighting1,
	MATH_tdst_Vector	*_pst_Banking1,
	MATH_tdst_Vector	*_pst_Sighting2,
	MATH_tdst_Vector	*_pst_Banking2,
	float				_f_coeff,
	char				_b_InitBlend
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static MATH_tdst_Matrix st_M1;
	static MATH_tdst_Matrix st_M2;
	MATH_tdst_Matrix		st_Mdst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f_coeff < 0.0f) _f_coeff = 0.0f;
	if(_f_coeff > 1.0f) _f_coeff = 1.0f;

	if(_b_InitBlend)
	{
		/* Build the 2 matrix from sighting and banking */
		MATH_CrossProduct(MATH_pst_GetXAxis(&st_M1), _pst_Banking1, _pst_Sighting1);
		MATH_CrossProduct(MATH_pst_GetXAxis(&st_M2), _pst_Banking2, _pst_Sighting2);
		MATH_CopyVector(MATH_pst_GetZAxis(&st_M2), _pst_Banking2);
		MATH_CopyVector(MATH_pst_GetZAxis(&st_M1), _pst_Banking1);
		MATH_NegVector(MATH_pst_GetYAxis(&st_M2), _pst_Sighting2);
		MATH_NegVector(MATH_pst_GetYAxis(&st_M1), _pst_Sighting1);
	}

	MATH_MatrixBlend(&st_Mdst, &st_M1, &st_M2, _f_coeff, _b_InitBlend);
	MATH_NegVector(_pst_SightingDest, MATH_pst_GetYAxis(&st_Mdst));
	MATH_CopyVector(_pst_BankingDest, MATH_pst_GetZAxis(&st_Mdst));
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHMatrixBlend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	MATH_tdst_Vector	st_S1, st_S2, st_B1, st_B2;
	float				f_coeff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_coeff = AI_PopFloat();
	AI_PopVector(&st_B2);
	AI_PopVector(&st_S2);
	AI_PopVector(&st_B1);
	AI_PopVector(&st_S1);
	AI_PopVar(&Val2, &st_Var2); /* Banking Dest */
	AI_PopVar(&Val1, &st_Var1); /* Sighting Dest */

	AI_EvalFunc_MATHMatrixBlend_C
	(
		(MATH_tdst_Vector *) st_Var1.pv_Addr,
		(MATH_tdst_Vector *) st_Var2.pv_Addr,
		&st_S1,
		&st_B1,
		&st_S2,
		&st_B2,
		f_coeff,
		1
	);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Ex: VDst=MATH_MatrixVecLocalToGlobal(VSrc,Sight,Bank)
 =======================================================================================================================
 */
void AI_EvalFunc_MATHMatrixVecLocalToGlobal_C
(
	MATH_tdst_Vector	*_pst_Vdst,
	MATH_tdst_Vector	*_pst_Vsrc,
	MATH_tdst_Vector	*_pst_S1,
	MATH_tdst_Vector	*_pst_B1
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_M1;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Build the matrix from sighting and banking */
	MATH_CrossProduct(MATH_pst_GetXAxis(&st_M1), _pst_B1, _pst_S1);
	MATH_CopyVector(MATH_pst_GetZAxis(&st_M1), _pst_B1);
	MATH_NegVector(MATH_pst_GetYAxis(&st_M1), _pst_S1);

	MATH_VectorLocalToGlobal(_pst_Vdst, &st_M1, _pst_Vsrc);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHMatrixVecLocalToGlobal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_S1, st_B1, st_Vsrc, st_Vdst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_B1);
	AI_PopVector(&st_S1);
	AI_PopVector(&st_Vsrc);

	AI_EvalFunc_MATHMatrixVecLocalToGlobal_C(&st_Vdst, &st_Vsrc, &st_S1, &st_B1);

	AI_PushVector(&st_Vdst);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Ex: F_Limited=MATH_FloatLimit(f_In,f_Min,f_Max);
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHFloatLimit(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_In, f_Min, f_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Max = AI_PopFloat();
	f_Min = AI_PopFloat();
	f_In = AI_PopFloat();

	AI_PushFloat(MATH_f_FloatLimit(f_In, f_Min, f_Max));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Ex: F_Min=MATH_FloatMin(f_A,f_B);
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHFloatMin(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	float	f_A, f_B;
	/*~~~~~~~~~~~~~*/

	f_B = AI_PopFloat();
	f_A = AI_PopFloat();

	AI_PushFloat(fMin(f_A, f_B));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Ex: F_Min=MATH_FloatMax(f_A,f_B);
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHFloatMax(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	float	f_A, f_B;
	/*~~~~~~~~~~~~~*/

	f_B = AI_PopFloat();
	f_A = AI_PopFloat();

	AI_PushFloat(fMax(f_A, f_B));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_MATHFloatNullEps_C(float _f)
{
	return(fNulWithEpsilon(_f, AI_gf_Epsilon) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHFloatNullEps(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	f = AI_PopFloat();
	AI_PushInt(AI_EvalFunc_MATHFloatNullEps_C(f));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_MTHFloatNullToler_C(float _f, float _f_Toler)
{
	return(fNulWithEpsilon(_f, _f_Toler) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHFloatNullToler(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	float	f;
	float	f_Toler;
	/*~~~~~~~~~~~~*/

	f_Toler = AI_PopFloat();
	f = AI_PopFloat();
	AI_PushInt(AI_EvalFunc_MTHFloatNullToler_C(f, f_Toler));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Ex: MATH_EpsilonSet(0.00001)
 =======================================================================================================================
 */
void AI_EvalFunc_MATHEpsilonSet_C(float _f_Epsilon)
{
	AI_gf_Epsilon = _f_Epsilon;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHEpsilonSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~*/
	float	f_Epsilon;
	/*~~~~~~~~~~~~~~*/

	f_Epsilon = AI_PopFloat();
	AI_EvalFunc_MATHEpsilonSet_C(f_Epsilon);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Ex: F_Eps=MATH_EpsilonGet()
 =======================================================================================================================
 */
float AI_EvalFunc_MATHEpsilonGet_C(void)
{
	return(AI_gf_Epsilon);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHEpsilonGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_MATHEpsilonGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Linear blends of two vector Note: If factor == 0, destination is the vector 1
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecBlend_C
(
	MATH_tdst_Vector	*st_Vec1,
	MATH_tdst_Vector	*st_Vec2,
	float				f_Factor,
	MATH_tdst_Vector	*pdest
)
{
	MATH_BlendVector(pdest, st_Vec1, st_Vec2, f_Factor);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecBlend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp, *pst_Vec1, *pst_Vec2;
	float				f_Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Factor = AI_PopFloat();
	pst_Vec2 = AI_PopVectorPtr();
	pst_Vec1 = AI_PopVectorPtr();
	AI_EvalFunc_MATHVecBlend_C(pst_Vec1, pst_Vec2, f_Factor, &st_Tmp);
	AI_PushVector(&st_Tmp);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Rotate a vector
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecRotate_C
(
	MATH_tdst_Vector	*pst_Vec,
	MATH_tdst_Vector	*pst_Axe,
	float				f_Angle,
	MATH_tdst_Vector	*pst_Dest
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp, st_Vec2, W;
	MATH_tdst_Vector	st_VecS;
	float				f, f_Norm2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&st_VecS, pst_Vec);
	f = MATH_f_DotProduct(pst_Axe, &st_VecS);
	f_Norm2 = MATH_f_DotProduct(pst_Axe, pst_Axe);

#ifdef JADEFUSION
    if(fAbs(f_Norm2)>Cf_Epsilon)
    {
        f /= f_Norm2;
    }
    else
    {
        MATH_CopyVector(pst_Dest, pst_Vec);
        return;
    }
#else
	f /= f_Norm2;
#endif	

	MATH_ScaleVector(&W, pst_Axe, f);
	MATH_SubEqualVector(&st_VecS, &W);
	MATH_ScaleVector(&st_Tmp, &st_VecS, fCos(f_Angle));
	MATH_NormalizeEqualVector(pst_Axe);
	MATH_CrossProduct(&st_Vec2, pst_Axe, &st_VecS);
	MATH_AddScaleVector(&st_Tmp, &st_Tmp, &st_Vec2, fSin(f_Angle));
	MATH_AddEqualVector(&st_Tmp, &W);
	MATH_CopyVector(pst_Dest, &st_Tmp);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecRotate(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Angle;
	MATH_tdst_Vector	st_Tmp, st_Axe, st_Vec;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Angle = AI_PopFloat();
	AI_PopVector(&st_Axe);
	AI_PopVector(&st_Vec);
	AI_EvalFunc_MATHVecRotate_C(&st_Vec, &st_Axe, f_Angle, &st_Tmp);
	AI_PushVector(&st_Tmp);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Sets all vector coordinates to zero Ex: MATH_VecSetNull(v)
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecSetNull(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	MATH_tdst_Vector	*pst_Vector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	pst_Vector = (MATH_tdst_Vector *) st_Var1.pv_Addr;
	MATH_InitVectorToZero(pst_Vector);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHFloatRound_C(float f_FloatToRound, float f_Precision)
{
	return MATH_f_FloatRound(f_FloatToRound, f_Precision);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHFloatRound(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_Precision, f_FloatToRound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Precision = AI_PopFloat();
	f_FloatToRound = AI_PopFloat();
	AI_PushFloat(MATH_f_FloatRound(f_FloatToRound, f_Precision));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Rounds a vector with a given precision Ex: MATH_VecRound(v,e)
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecRound(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	MATH_tdst_Vector	*pst_Vector;
	float				f_Precision;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Precision = AI_PopFloat();
	AI_PopVar(&Val1, &st_Var1);
	pst_Vector = (MATH_tdst_Vector *) st_Var1.pv_Addr;
	MATH_RoundVectorWithPrecision(pst_Vector, f_Precision);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecCos(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*st_Vec1, *st_Vec2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Vec2 = AI_PopVectorPtr();
	st_Vec1 = AI_PopVectorPtr();
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec2, Cf_Epsilon), "Vector 2 is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec1, Cf_Epsilon), "Vector 1 is null");
	AI_PushFloat(MATH_f_VecCos(st_Vec1, st_Vec2));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecSin(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*st_Vec1, *st_Vec2, *st_Axis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Axis = AI_PopVectorPtr();
	st_Vec2 = AI_PopVectorPtr();
	st_Vec1 = AI_PopVectorPtr();
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Axis, Cf_Epsilon), "Vector 3 is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec2, Cf_Epsilon), "Vector 2 is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec1, Cf_Epsilon), "Vector 1 is null");
	AI_PushFloat(MATH_f_VecSin(st_Vec1, st_Vec2, st_Axis));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecAngle(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*st_Vec1, *st_Vec2, *st_Axis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Axis = AI_PopVectorPtr();
	st_Vec2 = AI_PopVectorPtr();
	st_Vec1 = AI_PopVectorPtr();
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Axis, Cf_Epsilon), "Vector 3 is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec2, Cf_Epsilon), "Vector 2 is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec1, Cf_Epsilon), "Vector 1 is null");
	AI_PushFloat(MATH_f_VecAngle(st_Vec1, st_Vec2, st_Axis));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHVecAbsAngle(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*st_Vec1, *st_Vec2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Vec2 = AI_PopVectorPtr();
	st_Vec1 = AI_PopVectorPtr();
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec2, Cf_Epsilon), "Vector 2 is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Vec1, Cf_Epsilon), "Vector 1 is null");
	AI_PushFloat(MATH_f_VecAbsAngle(st_Vec1, st_Vec2));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Makes a blend between two vectors, using rotation interpolation Note: Not optimized
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecBlendRotate_C
(
	MATH_tdst_Vector	*_pst_1,
	MATH_tdst_Vector	*_pst_2,
	float				_f_Factor,
	MATH_tdst_Vector	*_pst_Dest
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_RotMat;
	MATH_tdst_Vector	st_Axis, st_Tmp;
	float				f_Angle;
	float				f_Norm1, f_Norm2;
	char				b_BlendNormOnly;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f_Factor < 0.0f) _f_Factor = 0.0f;
	if(_f_Factor > 1.0f) _f_Factor = 1.0f;

	b_BlendNormOnly = 0;

	/* Check cases with nul vectors */
	if(MATH_b_NulVectorWithEpsilon(_pst_1, Cf_Epsilon))
	{
		b_BlendNormOnly = 1;
		MATH_CopyVector(_pst_Dest, _pst_2);
	}

	if(MATH_b_NulVectorWithEpsilon(_pst_2, Cf_Epsilon))
	{
		b_BlendNormOnly = 1;
		MATH_CopyVector(_pst_Dest, _pst_1);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Angle interpolation
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!b_BlendNormOnly)
	{
		/* Calculate unit axis */
		MATH_CrossProduct(&st_Axis, _pst_1, _pst_2);

		if(MATH_b_NulVectorWithEpsilon(&st_Axis, 0.00001f))
		{
			/*
			 * We have two colinear vectors £
			 * Oposite or same direction ?
			 */
			if(MATH_f_DotProduct(_pst_1, _pst_2) > 0)
			{
				/* Same direction, we don't interpolate the angle */
				MATH_CopyVector(_pst_Dest, _pst_1);
				b_BlendNormOnly = 1;
			}
			else
			{
				/* Oposite direction, the problem is to find a axis... so we choose one */
#if 0
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					OBJ_tdst_GameObject *pst_GO;
					char				sz_Text[200];
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					AI_M_GetCurrentObject(pst_GO);
					sprintf(sz_Text, "MATH_VecBlendRotate : blending opposite vector (%s)", pst_GO->sz_Name);
					ERR_X_Warning(0, sz_Text, NULL);
					AI_Check(0, sz_Text);
				}
#endif
				MATH_CopyVector(&st_Tmp, &MATH_gst_BaseVectorI);
				MATH_CrossProduct(&st_Axis, _pst_1, &st_Tmp);
				if(MATH_b_NulVectorWithEpsilon(&st_Axis, 0.001f))
				{
					MATH_CopyVector(&st_Tmp, &MATH_gst_BaseVectorJ);
					MATH_CrossProduct(&st_Axis, _pst_1, &st_Tmp);
				}
			}
		}

		if(!b_BlendNormOnly)
		{
 #ifdef JADEFUSION
			if(MATH_b_NulVectorWithEpsilon(&st_Axis, 0.001f))
            {
                // no possibly valid rotation from that input
                //MATH_InitVector(_pst_Dest, 0.0f, 0.0f, 1.0f);
                MATH_CopyVector(_pst_Dest, _pst_1);
                return;
            }
            else
            {
			    MATH_NormalizeEqualVector(&st_Axis);

			    /* Calculate angle between two vectors */
			    f_Angle = MATH_f_VecAngle(_pst_1, _pst_2, &st_Axis) * _f_Factor;

			    /* Create the rotation matrix */
			    MATH_MakeRotationMatrix_AxisAngle(&st_RotMat, &st_Axis, f_Angle, 0, 1);

			    /* Make the rotation */
			    MATH_TransformVector(_pst_Dest, &st_RotMat, _pst_1);
            }
#else
			MATH_NormalizeEqualVector(&st_Axis);

			/* Calculate angle between two vectors */
			f_Angle = MATH_f_VecAngle(_pst_1, _pst_2, &st_Axis) * _f_Factor;

			/* Create the rotation matrix */
			MATH_MakeRotationMatrix_AxisAngle(&st_RotMat, &st_Axis, f_Angle, 0, 1);

			/* Make the rotation */
			MATH_TransformVector(_pst_Dest, &st_RotMat, _pst_1);
#endif
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Norm interpolation
	 -------------------------------------------------------------------------------------------------------------------
	 */

	f_Norm1 = MATH_f_NormVector(_pst_1);
	f_Norm2 = MATH_f_NormVector(_pst_2);
	if(!MATH_b_NulVectorWithEpsilon(_pst_Dest, Cf_Epsilon))
		MATH_SetNormVector(_pst_Dest, _pst_Dest, MATH_f_FloatBlend(f_Norm1, f_Norm2, _f_Factor));
	else
		MATH_InitVector(_pst_Dest, 0.0f, 0.0f, 0.0f);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecBlendRotate(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp, *st_Vec1, *st_Vec2;
	float				f_Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Factor = AI_PopFloat();
	st_Vec2 = AI_PopVectorPtr();
	st_Vec1 = AI_PopVectorPtr();
	AI_EvalFunc_MATHVecBlendRotate_C(st_Vec1, st_Vec2, f_Factor, &st_Tmp);
	AI_PushVector(&st_Tmp);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATHFloatBlend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_Factor, f_Param1, f_Param2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Factor = AI_PopFloat();
	f_Param2 = AI_PopFloat();
	f_Param1 = AI_PopFloat();
	AI_PushFloat(MATH_f_FloatBlend(f_Param1, f_Param2, f_Factor));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Returns the sign of a float Notes: F>0 return +1 £
    f<0 returns -1 £
    f=0 returns 0 £
    the returned value is a float
 =======================================================================================================================
 */
float AI_EvalFunc_MTHFloatSign_C(float f)
{
	if(fNul(f)) return 1.0f;
	vSign(&f);
	return f;
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHFloatSign(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	f = AI_PopFloat();
	AI_PushFloat(AI_EvalFunc_MTHFloatSign_C(f));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Returns the square root of a float
 =======================================================================================================================
 */
float AI_EvalFunc_MTHFloatSqrt_C(float f)
{
	return fSqrt(f);
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHFloatSqrt(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_MTHFloatSqrt_C(AI_PopFloat()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Returns the sign of an int Notes: i>0 return +1 £
    i<0 returns -1 £
    i=0 returns 0 £
    the returned value is an int
 =======================================================================================================================
 */
int AI_EvalFunc_MTHIntSign_C(int i)
{
	if(i > 0)
		i = 1;
	else if(i < 0)
		i = -1;
	return i;
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHIntSign(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_MTHIntSign_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MTHLn_C( float f )
{
	return fLn(f);
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHLn(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	f = AI_PopFloat();
	AI_PushFloat(AI_EvalFunc_MTHLn_C(f));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MTHLog10(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	f = AI_PopFloat();
	AI_PushFloat(fLog10(f));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MTHModulo_C(int i1, int i2)
{
	if(!i2) return 0;
	return i1 % i2;
}
/**/
AI_tdst_Node *AI_EvalFunc_MTHModulo(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	int i1, i2;
	/*~~~~~~~*/

	i2 = AI_PopInt();
	i1 = AI_PopInt();
	AI_PushInt(AI_EvalFunc_MTHModulo_C(i1, i2));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MATHRandBino_C(float moyenne, float precision, float grand, float petit)
{
	/*~~~~~~~~~~~~~~~~*/
	float	compteur;
	int		numbcl, bcl;
	/*~~~~~~~~~~~~~~~~*/

	moyenne = (moyenne - petit) / precision;
	numbcl = (int) ((grand - petit) / precision);

	for(bcl = 0, compteur = 0; bcl < numbcl; bcl++)
	{
		if(fRand(0, (float) numbcl) < moyenne) compteur += precision;
	}

	return compteur + petit;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHRandBino(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	petit, grand, precision, moyenne;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	petit = AI_PopFloat();
	grand = AI_PopFloat();
	precision = AI_PopFloat();
	moyenne = AI_PopFloat();

	AI_PushFloat(AI_EvalFunc_MATHRandBino_C(moyenne, precision, grand, petit));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATHVecInCone_C
(
	MATH_tdst_Vector	*v2,
	MATH_tdst_Vector	*v1,
	float				f_Angle,
	LONG				l_Flag,
	MATH_tdst_Vector	*dest
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				cos, cosref, nv2;
	MATH_tdst_Vector	v3, vTemp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	nv2 = 1.0f;
	if(MATH_b_NulVectorWithEpsilon(v1, 0.000001f)) return;

	if(!(l_Flag & 1)) MATH_NormalizeEqualVector(v1);

	cos = MATH_f_DotProduct(v1, v2);

	if(!(l_Flag & 2))
	{
		nv2 = MATH_f_NormVector(v2);
		cos /= nv2;
	}

	cosref = fCos(f_Angle);
	if(cos > cosref)
	{
		if((l_Flag & 6) == 4) MATH_ScaleEqualVector(v2, fInv(nv2));
		MATH_CopyVector(dest, v2);
		return;
	}

	if(!fEqWithEpsilon(cos, -1.0f, 1E-5f))
		MATH_CrossProduct(&vTemp, v1, v2);
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	stTemp;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		if(v2->y != 0.0f)
			MATH_InitVector(&stTemp, 1.0f, 0.0f, 0.0f);
		else
			MATH_InitVector(&stTemp, 0.0f, 1.0f, 0.0f);

		MATH_CrossProduct(&vTemp, v1, &stTemp);
	}

	MATH_CrossProduct(&v3, &vTemp, v1);
	MATH_NormalizeEqualVector(&v3);
	MATH_ScaleEqualVector(&v3, fSin(f_Angle));
	MATH_AddScaleVector(&v3, &v3, v1, cosref);

	if(!(l_Flag & 6)) MATH_ScaleEqualVector(&v3, nv2);

	MATH_CopyVector(dest, &v3);
}
/**/
AI_tdst_Node *AI_EvalFunc_MATHVecInCone(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v1, *v2, v3;
	LONG				l_Flag;
	float				f_Angle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Flag = AI_PopInt();
	f_Angle = AI_PopFloat();
	v1 = AI_PopVectorPtr();
	v2 = AI_PopVectorPtr();

	AI_EvalFunc_MATHVecInCone_C(v2, v1, f_Angle, l_Flag, &v3);
	AI_PushVector(&v3);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    WATER FUNCTIONS
 =======================================================================================================================
 */
void AI_EvalFunc_WTR_GetDif_C(MATH_tdst_Vector *pSrc, MATH_tdst_Vector *pDst)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern void WTR_Get_A_DifVector(MATH_tdst_Vector *pSrc, MATH_tdst_Vector *pDst, ULONG Converge);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	WTR_Get_A_DifVector(pSrc, pDst, 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WTR_GetDif(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*SRC, DST;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SRC = AI_PopVectorPtr();

	AI_EvalFunc_WTR_GetDif_C(SRC, &DST);

	AI_PushVector(&DST);
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_WTR_GetNrm_C(MATH_tdst_Vector *pSrc, MATH_tdst_Vector *pDst)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern void WTR_Get_A_MrmVector(MATH_tdst_Vector *pSrc, MATH_tdst_Vector *pDst);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	WTR_Get_A_MrmVector(pSrc, pDst);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WTR_GetNrm(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*SRC, DST;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SRC = AI_PopVectorPtr();

	AI_EvalFunc_WTR_GetNrm_C(SRC, &DST);

	AI_PushVector(&DST);
	return ++_pst_Node;
}

extern WATER_Export_Struct	stExportWaterParrams;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void AI_EvalFunc_WTR_Enable_C(ULONG Enable)
{
	if(stExportWaterParrams.Activate && Enable) return;
	if((!stExportWaterParrams.Activate) && (!Enable)) return;
	stExportWaterParrams.Activate = Enable;
	WTR_SetExportParams(&stExportWaterParrams);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WTR_SetF_C(ULONG NumToSet, float Value)
{
	/*~~~~~~~~~~~~~~*/
	float	*Faddress;
	/*~~~~~~~~~~~~~~*/

	Faddress = ((float *) &stExportWaterParrams.ZFactor) + NumToSet;
	if((ULONG) & stExportWaterParrams.ulLastValue > (ULONG) Faddress)
	{
		*Faddress = Value;
		WTR_SetExportParams(&stExportWaterParrams);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WTR_SetL_C(ULONG NumToSet, ULONG Value)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	*Faddress;
	/*~~~~~~~~~~~~~~*/

	Faddress = ((ULONG *) &stExportWaterParrams.SkyCol) + NumToSet;
	if((ULONG) & stExportWaterParrams.ZFactor > (ULONG) Faddress)
	{
		*Faddress = Value;
		WTR_SetExportParams(&stExportWaterParrams);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WTR_Enable(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_WTR_Enable_C(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WTR_SetF(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	float	fSet;
	ULONG	NumToSet;
	/*~~~~~~~~~~~~~*/

	fSet = AI_PopFloat();
	NumToSet = AI_PopInt();
	AI_EvalFunc_WTR_SetF_C(NumToSet, fSet);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WTR_SetL(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	ULONG	fSet;
	ULONG	NumToSet;
	/*~~~~~~~~~~~~~*/

	fSet = AI_PopInt();
	NumToSet = AI_PopInt();
	AI_EvalFunc_WTR_SetL_C(NumToSet, fSet);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
