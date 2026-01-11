/*$T ps2MATHvector.h GC! 1.081 07/07/00 14:20:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* MATHVector.h: All vector operations */
#ifdef PSX2_TARGET
#include "MATHs/MATHfloat.h"
#include "MATHs/PS2/ps2MATHopt.h"
#include "MATHs/PS2/ps2MATHvector.h"
#include "BASe/BAStypes.h"

#ifdef __cplusplus
extern "C"
{
#endif



/*
 =======================================================================================================================
    Aim:    Invert a vector

    Note:   (1/x,1/y,1/z) is performed, NOT (-x,-y,-z)...
 =======================================================================================================================
 */
void MATH_InvVector(MATH_tdst_Vector *_pst_Vdst, MATH_tdst_Vector *_pst_VSrc)
{
	_pst_Vdst->x = fInv(_pst_VSrc->x);
	_pst_Vdst->y = fInv(_pst_VSrc->y);
	_pst_Vdst->z = fInv(_pst_VSrc->z);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_DivVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f)
{
	/*~~~~~~~~~~~*/
	float	f_invf;
	/*~~~~~~~~~~~*/

	f_invf = fInv(f);

	Dest->x = fMul(A->x, f_invf);
	Dest->y = fMul(A->y, f_invf);
	Dest->z = fMul(A->z, f_invf);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_DivEqualVector(MATH_tdst_Vector *A, float f)
{
	/*~~~~~~~~~~~*/
	float	f_invf;
	/*~~~~~~~~~~~*/

	f_invf = fInv(f);

	A->x *= f_invf;
	A->y *= f_invf;
	A->z *= f_invf;
}

/*
 =======================================================================================================================
    Aim:    Cross product between two vectors

    Time:   K6/2-333 50 clocks
 =======================================================================================================================
 */



/*
 =======================================================================================================================
    Aim:    Returns the invert of the norm of a vector

    Time:   K6/2-333 -Not valid until debugged... (should be 50 clocks)
 =======================================================================================================================
 */
float MATH_f_InvNormVector(MATH_tdst_Vector *A)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	f = fSqr(A->x) + fSqr(A->y) + fSqr(A->z);
	f = 1 / fSqrt(f);

	/* FOptInvSqrt(f); vOptInvSqrt(&f); */
	return f;
}

/*
 =======================================================================================================================
    Aim:    Normalize a non nul vector

    Time:   K6/2-333 71 clocks

    Note:   As been assemblysed 
 =======================================================================================================================
 */



/*
 =======================================================================================================================
    Aim:    Normalize a non nul vector £

    Note:   The vector must be different from zero. (use MATH_NormalizeEqualAnyVector if you are not sure)
 =======================================================================================================================
 */


/*
 =======================================================================================================================
    Aim:    Normalize and round a non nul vector (to avoid precision drift)
 =======================================================================================================================
 */
void MATH_NormalizeAndRoundVector(MATH_tdst_Vector *_pst_DstVector, MATH_tdst_Vector *_pst_SrcVector)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	float	f10KInvNorm;
	LONG	l_Sx, l_Sy, l_Sz;	/* Coordinates converted to LONG */
	/*~~~~~~~~~~~~~~~~~~~~~*/

	f10KInvNorm = MATH_f_InvNormVector(_pst_SrcVector) * Cf_10K;
	l_Sx = lFloatToLong(fMul(_pst_SrcVector->x, f10KInvNorm));
	l_Sy = lFloatToLong(fMul(_pst_SrcVector->y, f10KInvNorm));
	l_Sz = lFloatToLong(fMul(_pst_SrcVector->z, f10KInvNorm));
	_pst_DstVector->x = fLongToFloat(l_Sx) * Cf_EpsilonBig;
	_pst_DstVector->y = fLongToFloat(l_Sy) * Cf_EpsilonBig;
	_pst_DstVector->z = fLongToFloat(l_Sz) * Cf_EpsilonBig;
}

/*
 =======================================================================================================================
    Aim:    Set the norm of a vector
 =======================================================================================================================
 */

/*
 =======================================================================================================================
    Aim:    Round a vector with a precision of 0.0001
 =======================================================================================================================
 */
void MATH_RoundVector(MATH_tdst_Vector *_pst_Vector)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_Sx, l_Sy, l_Sz;	/* Coordinates converted to LONG */
	/*~~~~~~~~~~~~~~~~~~~~~*/

	l_Sx = lFloatToLong(_pst_Vector->x * Cf_10K);
	l_Sy = lFloatToLong(_pst_Vector->y * Cf_10K);
	l_Sz = lFloatToLong(_pst_Vector->z * Cf_10K);
	_pst_Vector->x = fLongToFloat(l_Sx) * Cf_EpsilonBig;
	_pst_Vector->y = fLongToFloat(l_Sy) * Cf_EpsilonBig;
	_pst_Vector->z = fLongToFloat(l_Sz) * Cf_EpsilonBig;
}

/*
 =======================================================================================================================
    Aim:    Round a vector with a given precision
 =======================================================================================================================
 */
void MATH_RoundVectorWithPrecision(MATH_tdst_Vector *_pst_Vector, float f_precision)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_Sx, l_Sy, l_Sz;	/* Coordinates converted to LONG */
	float	f_InvPrec;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	f_InvPrec = (fInv(f_precision));

	l_Sx = lFloatToLong(_pst_Vector->x * f_InvPrec);
	l_Sy = lFloatToLong(_pst_Vector->y * f_InvPrec);
	l_Sz = lFloatToLong(_pst_Vector->z * f_InvPrec);
	_pst_Vector->x = fLongToFloat(l_Sx) * f_precision;
	_pst_Vector->y = fLongToFloat(l_Sy) * f_precision;
	_pst_Vector->z = fLongToFloat(l_Sz) * f_precision;
}

/*$4
 ***********************************************************************************************************************
    VECTOR TESTS £
    £
    MATH_b_NulVector £
    MATH_b_NulVectorWithEpsilon £
    MATH_b_EqVector £
    MATH_b_EqVector2 £
    MATH_b_EqVectorWithEpsilon £
    MATH_b_VecColinear £
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Tests if a vector is exactly nul

    Time:   K6/2-333 6-13 clocks

    Note:   The classical method "return ((A->x==0) && (A->y==0) && (A->z==0))" takes from 24 to 63 clocks
 =======================================================================================================================
 */
int MATH_b_NulVector(const MATH_tdst_Vector *pst_Vector)
{
	/*~~~~~~~~*/
	LONG	*pl;
	/*~~~~~~~~*/

	pl = (LONG *) pst_Vector;

	if(MATH_l_fAbsl(*pl) != 0) return 0;
	if(MATH_l_fAbsl(*(pl + 1)) != 0) return 0;
	if(MATH_l_fAbsl(*(pl + 2)) != 0) return 0;
	return 1;
}

/*
 =======================================================================================================================
    Aim:    Tests if a vector is nul with an epsilon

    Time:   K6/2-333 6-13 clocks

    Note:   The classical method "return (fAbs(A->x)<fEps) && (fAbs(A->y)<fEps) && (fAbs(A->z)<fEps);" takes from 25 to
            64 clocks This function, enables to quickly test if a vector is within a cube with edges of epsilon size =>
            useful for quick collision tests
 =======================================================================================================================
 */
char MATH_b_NulVectorWithEpsilon(MATH_tdst_Vector *A, float fEps)
{
	/*~~~~~~~~~*/
	LONG	*pA;
	LONG	lEps;
	/*~~~~~~~~~*/

	pA = (LONG *) A;
	lEps = *(LONG *) &fEps;

	/*
	 * On prend la valeur absolue des floatants et on fait au plus 3 comparaisons
	 * entieres
	 */
	if(MATH_l_fAbsl(*pA) > lEps) return 0;
	if(MATH_l_fAbsl(*(pA + 1)) > lEps) return 0;
	if(MATH_l_fAbsl(*(pA + 2)) > lEps) return 0;
	return 1;
}

/*
 =======================================================================================================================
    Aim:    Tests if two vectors are exactly identical

    Time:   K6/2-333 17-33 clocks

    Note:   The classical method "return ((A->x==B->x) && (A->y==B->y) && (A->z==B->z))" takes 23-65 clocks (65 if
            equal) See also MATH_b_EqVector2
 =======================================================================================================================
 */
char MATH_b_EqVector(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	stTmp;

	LONG				*pA;
	LONG				*pB;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pA = (LONG *) A;
	pB = (LONG *) B;

	/* Pour assurer l'egalité */
	MATH_SubVector(&stTmp, A, B);
	return(MATH_b_NulVector(&stTmp));
}

/*
 =======================================================================================================================
    Aim:    Tests if two vectors are exactly identical, with memory comparison

    Time:   K6/2-333 6-12 clocks

    Note:   12 clocks happen when the vectors are identical. This function compares directly the memory. So it is very
            fast. But it is not 100% sure if one of the vectors has a nul coordinate, because +0 and -0 exist and are
            different... use MATH_b_EqVector if you want to be 100% sure
 =======================================================================================================================
 */
char MATH_b_EqVector2(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	/*~~~~~~~~*/
	LONG	*pA;
	LONG	*pB;
	/*~~~~~~~~*/

	pA = (LONG *) A;
	pB = (LONG *) B;

	return((*pA == *pB) && (*(pA + 1) == *(pB + 1)) && (*(pA + 2) == *(pB + 2)));
}

/*
 =======================================================================================================================
    Aim:    Tests if two vectors are identical (with an epsilon)

    Time:   K6/2-333 20-28 clocks

    Note:   The classical method "return (fAbs(A->x-B->x)<fEps) && (fAbs(A->y-B->y)<fEps) && (fAbs(A->z-B->z)<fEps);"
            takes from 28 to 90 clocks
 =======================================================================================================================
 */
char MATH_b_EqVectorWithEpsilon(MATH_tdst_Vector *A, MATH_tdst_Vector *B, float fEps)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	stTmp;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&stTmp, A, B);
	return(MATH_b_NulVectorWithEpsilon(&stTmp, fEps));
}

/*
 =======================================================================================================================
    Aim:    Returns true if 2 vectors are colinear
 =======================================================================================================================
 */
char MATH_b_VecColinear(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_TmpVec;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CrossProduct(&st_TmpVec, A, B);
	return(MATH_b_NulVectorWithEpsilon(&st_TmpVec, Cf_Epsilon));
}

/*$4
 ***********************************************************************************************************************
    ADVANCED VECTOR OPERATIONS £
    £
    MATH_NormalizeAnyVector (vector*,vector*) £
    MATH_NormalizeEqualAnyVector (vector*) £
    MATH_VectorCombine(vector*,vector*,vector*,float) £
    MATH_f_PointLineDistance(vector*,vector*,vector*) £
    MATH_PointLineProjection(vector*,vector*,vector*,vector*) £
    MATH_PointSegmentProjection £
    MATH_PointParabolProjection2D £
    MATH_PointParabolProjection3D £
    MATH_f_VecCos(vector*,vector*) £
    MATH_f_VecSin(vector*,vector*,vector*) £
    MATH_f_VecAngle(vector*,vector*,vector*) £
    MATH_f_VecAbsAngle(vector*,vector*) £
    MATH_f_VectorAbsAngle £
    MATH_AddScaleVector(Dst, A, B, f) : Dst = A + f * B £
    MATH_SubScaleVector(Dst, A, B, f) : Dst = A - f * B £
    MATH_f_SubSubDotProduct(A, B, C, D) : (A - B) . (C - D) £
    MATH_f_SubSubCrossProduct(Dst, A, B, C, D) : Dst = (A - B) ^ (C - D) £
    MATH_MulTwoVectors(vector*,vector*,vector*) £
    MATH_MulEqualTwoVectors(vector* vector*) £
    MATH_ExpEqualVector (vector*) £
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Calculates the distance from point "_pst_Point" to line defined by the two points "_pst_LineA" and
            "_pst_LineB"
 =======================================================================================================================
 */
float MATH_f_PointLineDistance
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_u;
	MATH_tdst_Vector	st_AM;
	MATH_tdst_Vector	st_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_u, _pst_LineB, _pst_LineA);
	MATH_SubVector(&st_AM, _pst_Point, _pst_LineA);
	MATH_NormalizeEqualVector(&st_u);
	MATH_CrossProduct(&st_Tmp, &st_u, &st_AM);
	return(MATH_f_NormVector(&st_Tmp));
}

/*
 =======================================================================================================================
    Aim:    Calculates the coordinates of the projection "_pst_Proj" of the point "_pst_Point" on the line defined by
            the two points "_pst_LineA" and "_pst_LineB"
 =======================================================================================================================
 */
void MATH_PointLineProjection
(
	MATH_tdst_Vector	*_pst_Proj,
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_u;
	MATH_tdst_Vector	st_AM;
	float				f_AH;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_u, _pst_LineB, _pst_LineA);
	MATH_SubVector(&st_AM, _pst_Point, _pst_LineA);
	MATH_NormalizeEqualVector(&st_u);
	f_AH = MATH_f_DotProduct(&st_u, &st_AM);
	MATH_MulEqualVector(&st_u, f_AH);
	MATH_AddVector(_pst_Proj, &st_u, _pst_LineA);
}

/*
 =======================================================================================================================
    Aim:    Calculates the cos between two vectors
 =======================================================================================================================
 */
float MATH_f_VecCos(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_uV1;
	MATH_tdst_Vector	st_uV2;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_NormalizeVector(&st_uV1, _pst_V1);
	MATH_NormalizeVector(&st_uV2, _pst_V2);
	return(MATH_f_DotProduct(&st_uV1, &st_uV2));
}

/*
 =======================================================================================================================
    Aim:    Calculates the sin between two vectors
 =======================================================================================================================
 */
float MATH_f_VecSin(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_CrossProduct;
	float				f_Sin;
	MATH_tdst_Vector	st_uV1;
	MATH_tdst_Vector	st_uV2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_NormalizeVector(&st_uV1, _pst_V1);
	MATH_NormalizeVector(&st_uV2, _pst_V2);

	MATH_CrossProduct(&st_CrossProduct, &st_uV1, &st_uV2);
	f_Sin = MATH_f_NormVector(&st_CrossProduct);

	/*
	 * If an axis is given, than we use it to calculate the sign of the sinus, else it
	 * is positive
	 */
	if(_pst_Axis != 0)
	{
		if(MATH_f_DotProduct(&st_CrossProduct, _pst_Axis) < 0.0f) vNeg(&f_Sin);
	}

	return(f_Sin);
}

/*
 =======================================================================================================================
    Aim:    Calculates the angle between two vectors of the space

    Note:   The axis defines the normal of the plan for the sign of the angle. If no axis is given (nul pointer), then
            the axis is calculated using the cross product Clocks: To be optimized, (too many normalizations are done)
 =======================================================================================================================
 */
float MATH_f_VecAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_Cos, f_Sin, f_Angle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Cos = MATH_f_VecCos(_pst_V1, _pst_V2);
	if(f_Cos >= 1.0f) return 0;
	if(f_Cos <= -1.0f)
		f_Angle = Cf_Pi;
	else
		f_Angle = fAcos(f_Cos);

	f_Sin = MATH_f_VecSin(_pst_V1, _pst_V2, _pst_Axis);
	if(f_Sin < 0) f_Angle = -f_Angle;

	return(f_Angle);
}

/*
 =======================================================================================================================
    Aim:    Calculates the absolute value of the angle between two vectors of the space

    Note:   Faster than MATH_f_VecAngle, no cross product to find the sign of the angle
 =======================================================================================================================
 */
float MATH_f_VecAbsAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	float	f_Cos, f_Angle;
	/*~~~~~~~~~~~~~~~~~~~*/

	f_Cos = MATH_f_VecCos(_pst_V1, _pst_V2);
	f_Angle = fAcos(f_Cos);

	return(f_Angle);
}

/*
 =======================================================================================================================
    Aim:    Scale a vector and add another one

    Note:   Dst=A+B*f
 =======================================================================================================================
 */

/*
 =======================================================================================================================
    Aim Scale a vector and subs the result to another one

    Note:   Makes:: Dst = A - f * B;
 =======================================================================================================================
 */
void MATH_SubScaleVector(MATH_tdst_Vector *Dst, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_ScaleVector(&st_Tmp, B, f);
	MATH_SubVector(Dst, A, &st_Tmp);
}

/*
 =======================================================================================================================
    Aim:    Makes (A - B).(C - D)
 =======================================================================================================================
 */
float MATH_f_SubSubDotProduct
(
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	MATH_tdst_Vector	*C,
	MATH_tdst_Vector	*D
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp1;
	MATH_tdst_Vector	st_Tmp2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Tmp1, A, B);
	MATH_SubVector(&st_Tmp2, C, D);

	return(MATH_f_DotProduct(&st_Tmp1, &st_Tmp2));
}

/*
 =======================================================================================================================
    Aim:    Makes (A - B)^(C - D)
 =======================================================================================================================
 */
void MATH_f_SubSubCrossProduct
(
	MATH_tdst_Vector	*Dst,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	MATH_tdst_Vector	*C,
	MATH_tdst_Vector	*D
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp1;
	MATH_tdst_Vector	st_Tmp2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Tmp1, A, B);
	MATH_SubVector(&st_Tmp2, C, D);

	MATH_CrossProduct(Dst, &st_Tmp1, &st_Tmp2);
}

/*
 =======================================================================================================================
    Aim:    Combine two vectors with the following formula: VDst = r * V1 + (1-r) * V2

    Note:   We use r * V1 + (1-r) * V2 = r * (V1 - V2) + V2, faster way for calculation
 =======================================================================================================================
 */
void MATH_VectorCombine
(
	MATH_tdst_Vector	*_pst_VDst,
	MATH_tdst_Vector	*_pst_V1,
	MATH_tdst_Vector	*_pst_V2,
	float				_f_r
)
{
	MATH_SubVector(_pst_VDst, _pst_V1, _pst_V2);
	MATH_AddScaleVector(_pst_VDst, _pst_V2, _pst_VDst, _f_r);
}


/*
 =======================================================================================================================
    Aim:    Linear blends of two vector

    Note:   If factor == 0, destination is the vector 1
 =======================================================================================================================
 */


#ifdef __cplusplus
}
#endif
#endif

