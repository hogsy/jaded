/*$T ps2MATHvector.h GC! 1.081 07/07/00 14:20:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* MATHVector.h: All vector operations */
#if !defined(__ps2MATHVECTOR_H__) && defined(PSX2_TARGET)
#define __ps2MATHVECTOR_H__


#include "MATHs/PS2/ps2MATHfloat.h"
#include "MATHs/PS2/ps2MATHopt.h"
#include "BASe/BAStypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 ===================================================================================================
    Aim:    Sacales (or multiplies) a vector

    Note:   Makes A*=f
 ===================================================================================================
 */
_inline_ void MATH_ScaleEqualVector(MATH_tdst_Vector *A, float f)
{
    A->x *= f;
    A->y *= f;
    A->z *= f;
}

#define MATH_MulEqualVector MATH_ScaleEqualVector

/*
 =======================================================================================================================
    Aim:    Normalize a non nul vector

    Time:   K6/2-333 71 clocks

    Note:   The vector must be different from zero If you are not sure, you can use MATH_NormalizeAnyVector (slower);
 =======================================================================================================================
 */
void MATH_NormalizeVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A);
/*
 =======================================================================================================================
    Aim:    Take the absolute value for each coordinate of a vector
 =======================================================================================================================
 */
void MATH_AbsVector(MATH_tdst_Vector *_pst_Mdst, MATH_tdst_Vector *_pst_Msrc);

/*
 =======================================================================================================================
    Aim:    Initialize a vector to 0
 =======================================================================================================================
 */
void MATH_InitVectorToZero(MATH_tdst_Vector *VDst);

#define MATH_SetNulVector	MATH_InitVectorToZero

/*
 =======================================================================================================================
    Aim:    Initialize a vector with 3 floats using values for the floats

    Time:   K6/2-333 15 clocks
 =======================================================================================================================
 */
void MATH_InitVector(MATH_tdst_Vector *VDst, float x, float y, float z);

#define MATH_SetVector	MATH_InitVector


/*
 =======================================================================================================================
    Aim:    Invert a vector

    Note:   (1/x,1/y,1/z); is performed, NOT (-x,-y,-z);...
 =======================================================================================================================
 */
void MATH_InvVector(MATH_tdst_Vector *_pst_Vdst, MATH_tdst_Vector *_pst_VSrc);


/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_InvEqualVector(MATH_tdst_Vector *_pst_VSrc)
{
    _pst_VSrc->x = fInv(_pst_VSrc->x);
    _pst_VSrc->y = fInv(_pst_VSrc->y);
    _pst_VSrc->z = fInv(_pst_VSrc->z);
}

/*
 =======================================================================================================================
    Aim:    Scale a vector (multiplies each coordinate with same value);

    Time:   K6/2-333 27 clocks
 =======================================================================================================================
 */
_inline_ void MATH_ScaleVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f)
{
    Dest->x = fMul(A->x, f);
    Dest->y = fMul(A->y, f);
    Dest->z = fMul(A->z, f);
}


#define MATH_MulVector	MATH_ScaleVector

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_DivVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f);


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_DivEqualVector(MATH_tdst_Vector *A, float f);


/*
 =======================================================================================================================
    Aim:    Cross product between two vectors

    Time:   K6/2-333 50 clocks
 =======================================================================================================================
 */
void MATH_CrossProduct(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, MATH_tdst_Vector *B);


/*
 =======================================================================================================================
    Aim:    Returns the invert of the norm of a vector

    Time:   K6/2-333 -Not valid until debugged... (should be 50 clocks);
 =======================================================================================================================
 */
float MATH_f_InvNormVector(MATH_tdst_Vector *A);



/*
 =======================================================================================================================
    Aim:    Normalize a non nul vector £

    Note:   The vector must be different from zero. (use MATH_NormalizeEqualAnyVector if you are not sure);
 =======================================================================================================================
 */
void MATH_NormalizeEqualVector(MATH_tdst_Vector *_pst_V);


/*
 =======================================================================================================================
    Aim:    Set the norm of a vector
 =======================================================================================================================
 */
void MATH_SetNormVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *Src, float _f_Norm);


/*
 =======================================================================================================================
    Aim:    
     a vector with a precision of 0.0001
 =======================================================================================================================
 */
void MATH_RoundVector(MATH_tdst_Vector *_pst_Vector);

/*
 =======================================================================================================================
    Aim:    Normalize and round a non nul vector (to avoid precision drift)
 =======================================================================================================================
 */
void MATH_NormalizeAndRoundVector(MATH_tdst_Vector *_pst_DstVector, MATH_tdst_Vector *_pst_SrcVector);

/*
 =======================================================================================================================
    Aim:    Round a vector with a given precision
 =======================================================================================================================
 */
void MATH_RoundVectorWithPrecision(MATH_tdst_Vector *_pst_Vector, float f_precision);


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

    Note:   The classical method "return ((A->x==0); && (A->y==0); && (A->z==0););" takes from 24 to 63 clocks
 =======================================================================================================================
 */
int MATH_b_NulVector(const MATH_tdst_Vector *pst_Vector);


/*
 =======================================================================================================================
    Aim:    Tests if a vector is nul with an epsilon

    Time:   K6/2-333 6-13 clocks

    Note:   The classical method "return (fAbs(A->x);<fEps); && (fAbs(A->y);<fEps); && (fAbs(A->z);<fEps);" takes from 25 to
            64 clocks This function, enables to quickly test if a vector is within a cube with edges of epsilon size =>
            useful for quick collision tests
 =======================================================================================================================
 */
char MATH_b_NulVectorWithEpsilon(MATH_tdst_Vector *A, float fEps);


/*
 =======================================================================================================================
    Aim:    Tests if two vectors are exactly identical

    Time:   K6/2-333 17-33 clocks

    Note:   The classical method "return ((A->x==B->x); && (A->y==B->y); && (A->z==B->z););" takes 23-65 clocks (65 if
            equal); See also MATH_b_EqVector2
 =======================================================================================================================
 */
char MATH_b_EqVector(MATH_tdst_Vector *A, MATH_tdst_Vector *B);


/*
 =======================================================================================================================
    Aim:    Tests if two vectors are exactly identical, with memory comparison

    Time:   K6/2-333 6-12 clocks

    Note:   12 clocks happen when the vectors are identical. This function compares directly the memory. So it is very
            fast. But it is not 100% sure if one of the vectors has a nul coordinate, because +0 and -0 exist and are
            different... use MATH_b_EqVector if you want to be 100% sure
 =======================================================================================================================
 */
char MATH_b_EqVector2(MATH_tdst_Vector *A, MATH_tdst_Vector *B);


/*
 =======================================================================================================================
    Aim:    Tests if two vectors are identical (with an epsilon);

    Time:   K6/2-333 20-28 clocks

    Note:   The classical method "return (fAbs(A->x-B->x);<fEps); && (fAbs(A->y-B->y);<fEps); && (fAbs(A->z-B->z);<fEps);"
            takes from 28 to 90 clocks
 =======================================================================================================================
 */
char MATH_b_EqVectorWithEpsilon(MATH_tdst_Vector *A, MATH_tdst_Vector *B, float fEps);


/*
 =======================================================================================================================
    Aim:    Returns true if 2 vectors are colinear
 =======================================================================================================================
 */
char MATH_b_VecColinear(MATH_tdst_Vector *A, MATH_tdst_Vector *B);


/*$4
 ***********************************************************************************************************************
    ADVANCED VECTOR OPERATIONS £
    £
    MATH_NormalizeAnyVector (vector*,vector*); £
    MATH_NormalizeEqualAnyVector (vector*); £
    MATH_VectorCombine(vector*,vector*,vector*,float); £
    MATH_f_PointLineDistance(vector*,vector*,vector*); £
    MATH_PointLineProjection(vector*,vector*,vector*,vector*); £
    MATH_PointSegmentProjection £
    MATH_PointParabolProjection2D £
    MATH_PointParabolProjection3D £
    MATH_f_VecCos(vector*,vector*); £
    MATH_f_VecSin(vector*,vector*,vector*); £
    MATH_f_VecAngle(vector*,vector*,vector*); £
    MATH_f_VecAbsAngle(vector*,vector*); £
    MATH_f_VectorAbsAngle £
    MATH_AddScaleVector(Dst, A, B, f); : Dst = A + f * B £
    MATH_SubScaleVector(Dst, A, B, f); : Dst = A - f * B £
    MATH_f_SubSubDotProduct(A, B, C, D); : (A - B); . (C - D); £
    MATH_f_SubSubCrossProduct(Dst, A, B, C, D); : Dst = (A - B); ^ (C - D); £
    MATH_MulTwoVectors(vector*,vector*,vector*); £
    MATH_MulEqualTwoVectors(vector* vector*); £
    MATH_ExpEqualVector (vector*); £
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Normalize a vector, even if nul

    Note:   If the vector is nul, a nul vector is returned
 =======================================================================================================================
 */
void MATH_NormalizeAnyVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A);


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_NormalizeEqualAnyVector(MATH_tdst_Vector *_pst_V);


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
);


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
);


extern void MATH_PointParabolProjection2D(MATH_tdst_Vector *, MATH_tdst_Vector *, float a, float b);
extern void MATH_PointParabolProjection3D
			(
				MATH_tdst_Vector *, /* Projection global coordinates */
				MATH_tdst_Vector *, /* Tangent global coordinates */
				MATH_tdst_Vector *, /* Point global coordinates (before projection); */
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				float _f_r
			);
extern void MATH_PointSegmentProjection
			(
				MATH_tdst_Vector *, /* Projection global coordinates */
				MATH_tdst_Vector *, /* Point global coordinates (before projection); */
				MATH_tdst_Vector *,
				MATH_tdst_Vector *
			);

/*
 =======================================================================================================================
    Aim:    Calculates the cos between two vectors
 =======================================================================================================================
 */
float MATH_f_VecCos(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2);


/*
 =======================================================================================================================
    Aim:    Calculates the sin between two vectors
 =======================================================================================================================
 */
float MATH_f_VecSin(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis);


/*
 =======================================================================================================================
    Aim:    Calculates the angle between two vectors of the space

    Note:   The axis defines the normal of the plan for the sign of the angle. If no axis is given (nul pointer);, then
            the axis is calculated using the cross product Clocks: To be optimized, (too many normalizations are done);
 =======================================================================================================================
 */
float MATH_f_VecAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis);

/*
 =======================================================================================================================
    Aim:    Calculates the absolute value of the angle between two vectors of the space

    Note:   Faster than MATH_f_VecAngle, no cross product to find the sign of the angle
 =======================================================================================================================
 */
float MATH_f_VecAbsAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2);


/*
 =======================================================================================================================
    Aim:    Scale a vector and add another one

    Note:   Dst=A+B*f
 =======================================================================================================================
 */
void MATH_AddScaleVector(MATH_tdst_Vector *Dst, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float f);


/*
 =======================================================================================================================
    Aim Scale a vector and subs the result to another one

    Note:   Makes:: Dst = A - f * B;
 =======================================================================================================================
 */
void MATH_SubScaleVector(MATH_tdst_Vector *Dst, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float f);


/*
 =======================================================================================================================
    Aim:    Makes (A - B);.(C - D);
 =======================================================================================================================
 */
float MATH_f_SubSubDotProduct
(
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	MATH_tdst_Vector	*C,
	MATH_tdst_Vector	*D
);


/*
 =======================================================================================================================
    Aim:    Makes (A - B);^(C - D);
 =======================================================================================================================
 */
void MATH_f_SubSubCrossProduct
(
	MATH_tdst_Vector	*Dst,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	MATH_tdst_Vector	*C,
	MATH_tdst_Vector	*D
);

/*
 =======================================================================================================================
    Aim:    Combine two vectors with the following formula: VDst = r * V1 + (1-r); * V2

    Note:   We use r * V1 + (1-r); * V2 = r * (V1 - V2); + V2, faster way for calculation
 =======================================================================================================================
 */
void MATH_VectorCombine
(
	MATH_tdst_Vector	*_pst_VDst,
	MATH_tdst_Vector	*_pst_V1,
	MATH_tdst_Vector	*_pst_V2,
	float				_f_r
);


/*
 =======================================================================================================================
    Aim:    Linear blends of two vector

    Note:   If factor == 0, destination is the vector 1
 =======================================================================================================================
 */
void MATH_BlendVector_asm
(
	MATH_tdst_Vector	*_pst_Dest,
	MATH_tdst_Vector	*_pst_1,
	MATH_tdst_Vector	*_pst_2,
	float				_f_Factor
);

_inline_ void MATH_BlendVector
(
	MATH_tdst_Vector	*_pst_Dest,
	MATH_tdst_Vector	*_pst_1,
	MATH_tdst_Vector	*_pst_2,
	float				_f_Factor
)
{
	if(_f_Factor>1.0f)	_f_Factor = 1.0f;
	if(_f_Factor<0.0f)	_f_Factor = 0.0f;
	MATH_BlendVector_asm(_pst_Dest, _pst_1, _pst_2, _f_Factor);
}

/*
 ===================================================================================================
    Aim:    Take the absolute value for each coordinate of a vector
 ===================================================================================================
 */
_inline_ void MATH_AbsVector(MATH_tdst_Vector *_pst_Mdst, MATH_tdst_Vector *_pst_Msrc)
{
    _pst_Mdst->x = fAbs(_pst_Msrc->x);
    _pst_Mdst->y = fAbs(_pst_Msrc->y);
    _pst_Mdst->z = fAbs(_pst_Msrc->z);
}

/*
 ===================================================================================================
    Aim:    Initialize a vector to 0
 ===================================================================================================
 */
_inline_ void MATH_InitVectorToZero(MATH_tdst_Vector *VDst)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Dst = (LONG *) VDst;

    Dst[2] = Dst[1] = *Dst = Cl_0f;
}

#define MATH_SetNulVector   MATH_InitVectorToZero

/*
 ===================================================================================================
    Aim:    Initialize a vector with 3 floats using values for the floats

    Time:   K6/2-333 15 clocks
 ===================================================================================================
 */
_inline_ void MATH_InitVector(MATH_tdst_Vector *VDst, float x, float y, float z)
{
    VDst->x = x;
    VDst->y = y;
    VDst->z = z;
}

#define MATH_SetVector  MATH_InitVector

/*
 ===================================================================================================
    Aim:    Add two vectors

    Time:   K6/2-333 26 clocks

    Note:   Makes Vdest=A+B;
 ===================================================================================================
 */
_inline_ void MATH_AddVector(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    VDst->x = fAdd(A->x, B->x);
    VDst->y = fAdd(A->y, B->y);
    VDst->z = fAdd(A->z, B->z);
}

/*
 ===================================================================================================
    Aim:    Add two vectors

    Note:   Makes A+=B
 ===================================================================================================
 */
_inline_ void MATH_AddEqualVector(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    A->x += B->x;
    A->y += B->y;
    A->z += B->z;
}

/*
 ===================================================================================================
    Aim:    Substract two vectors

    Time:   K6/2-333 26 clocks

    Note:   Vdst = A-B
 ===================================================================================================
 */
_inline_ void MATH_SubVector(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    VDst->x = fSub(A->x, B->x);
    VDst->y = fSub(A->y, B->y);
    VDst->z = fSub(A->z, B->z);
}

/*
 ===================================================================================================
    Aim:    Sub two vectors

    Note:   Makes A-=B
 ===================================================================================================
 */
_inline_ void MATH_SubEqualVector(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    A->x -= B->x;
    A->y -= B->y;
    A->z -= B->z;
}

/*
 ===================================================================================================
    Aim:    Copy a vector

    Time:   K6/2-333 9 clocks
 ===================================================================================================
 */
_inline_ void MATH_CopyVector(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src;
    LONG    *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Dst = (LONG *) VDst;
    Src = (LONG *) A;

    *Dst = *Src;
    *(Dst + 1) = *(Src + 1);
    *(Dst + 2) = *(Src + 2);
}

/*
 ===================================================================================================
    Aim:    Negate a vector

    Time:   K6/2-333 6 clocks

    Note:   Warning, with this function the vector is directly negated.
 ===================================================================================================
 */
_inline_ void MATH_NegEqualVector(MATH_tdst_Vector *_pst_Vector)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Src = (LONG *) _pst_Vector;

    *Src += 0x80000000;
    *(Src + 1) += 0x80000000;
    *(Src + 2) += 0x80000000;
}

/*
 ===================================================================================================
    Aim:    Negate a vector

    Time:   K6/2-333 6 clocks

    Note:   Warning, with this function the vector is directly negated.
 ===================================================================================================
 */
_inline_ void MATH_NegVector(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Vector *_pst_VSrc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src, *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Src = (LONG *) _pst_VSrc;
    Dst = (LONG *) _pst_VDst;

    *Dst = *Src + 0x80000000;
    *(Dst + 1) = *(Src + 1) + 0x80000000;
    *(Dst + 2) = *(Src + 2) + 0x80000000;
}


/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_OptInvEqualVector(MATH_tdst_Vector *pst_Vector)
{
    vOptInv(&(pst_Vector->x));
    vOptInv(&(pst_Vector->y));
    vOptInv(&(pst_Vector->z));
}



/*
 ===================================================================================================
    Aim:    Dot product between two vectors

    Time:   K6/2-333 26 clocks
 ===================================================================================================
 */
_inline_ float MATH_f_DotProduct(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    return((A->x) * (B->x) + (A->y) * (B->y) + (A->z) * (B->z));
}



/*
 ===================================================================================================
    Aim:    Returns the square of the norm of a vector

    Time:   Not tested
 ===================================================================================================
 */
_inline_ float MATH_f_SqrNormVector(MATH_tdst_Vector *A)
{
    return(A->x * A->x + A->y * A->y + A->z * A->z);
}

/*
 ===================================================================================================
    Aim:    Returns the norm of a vector
 ===================================================================================================
 */
_inline_ float MATH_f_NormVector(MATH_tdst_Vector *_pst_V)
{
    return(fSqrt(MATH_f_SqrNormVector(_pst_V)));
}

/*
 ===================================================================================================
    Aim:    Returns the distance between two points

    Time:   Not tested
 ===================================================================================================
 */
_inline_ float MATH_f_Distance(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    V;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SubVector(&V, A, B);
    return(MATH_f_NormVector(&V));
}

/*
 ===================================================================================================
    Aim:    Return the square of the vector

    Time:   Not tested
 ===================================================================================================
 */
_inline_ float MATH_f_SqrVector(MATH_tdst_Vector *A)
{
    return(fSqr(A->x) + fSqr(A->y) + fSqr(A->z));
}



/*
 ===================================================================================================
    Aim:    Normalize a vector, even if nul

    Note:   If the vector is nul, a nul vector is returned
 ===================================================================================================
 */
_inline_ void MATH_NormalizeAnyVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A)
{
    if(MATH_b_NulVector(A))
        MATH_InitVectorToZero(Dest);
    else
        MATH_NormalizeVector(Dest, A);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_NormalizeEqualAnyVector(MATH_tdst_Vector *_pst_V)
{
    if(!MATH_b_NulVector(_pst_V))
        MATH_NormalizeEqualVector(_pst_V);
}




/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UncompressAbsoluteVector(MATH_tdst_Vector *Dst, MATH_tdst_CompressedVector *C)
{
    Dst->x = (C->x / 1024.0f);
    Dst->y = (C->y / 1024.0f);
    Dst->z = (C->z / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UncompressRelativeVector(MATH_tdst_Vector *Dst, MATH_tdst_CompressedVector *C)
{
    Dst->x += (C->x / 1024.0f);
    Dst->y += (C->y / 1024.0f);
    Dst->z += (C->z / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressXVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
    Dst->x += (C->s / 1024.0f);
    Dst->y += (C->c1 / 1024.0f);
    Dst->z += (C->c2 / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressYVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
    Dst->x += (C->c1 / 1024.0f);
    Dst->y += (C->s / 1024.0f);
    Dst->z += (C->c2 / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressZVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
    Dst->x += (C->c1 / 1024.0f);
    Dst->y += (C->c2 / 1024.0f);
    Dst->z += (C->s / 1024.0f);
}

/*
 ===================================================================================================
    Aim:    Multiplies each coordinate of a vector by the same coordinate of another vector
 ===================================================================================================
 */
_inline_ void MATH_MulTwoVectors
(
    MATH_tdst_Vector    *_pst_Dst,
    MATH_tdst_Vector    *_pst_Src,
    MATH_tdst_Vector    *_pst_Mul
)
{
    _pst_Dst->x = fMul(_pst_Src->x, _pst_Mul->x);
    _pst_Dst->y = fMul(_pst_Src->y, _pst_Mul->y);
    _pst_Dst->z = fMul(_pst_Src->z, _pst_Mul->z);
}

/*
 ===================================================================================================
    Aim:    Multiplies each coordinate of a vector by the same coordinate of another vector
 ===================================================================================================
 */
_inline_ void MATH_MulEqualTwoVectors(MATH_tdst_Vector *_pst_Dst, MATH_tdst_Vector *_pst_Mul)
{
    _pst_Dst->x = fMul(_pst_Dst->x, _pst_Mul->x);
    _pst_Dst->y = fMul(_pst_Dst->y, _pst_Mul->y);
    _pst_Dst->z = fMul(_pst_Dst->z, _pst_Mul->z);
}

/*
 ===================================================================================================
    Aim:    Calculates the exponential of each coordinate of a vector
 ===================================================================================================
 */
_inline_ void MATH_ExpEqualVector(MATH_tdst_Vector *_pst_Dst)
{
    _pst_Dst->x = fExp(_pst_Dst->x);
    _pst_Dst->y = fExp(_pst_Dst->y);
    _pst_Dst->z = fExp(_pst_Dst->z);
}



#include "MATHs/ps2/ps2MATHvector_ALIGNED.h"


#ifdef __cplusplus
}
#endif
#endif /* #ifndef __ps2MATHVECTOR_H__ */

