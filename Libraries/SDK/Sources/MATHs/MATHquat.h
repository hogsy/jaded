/*$T MATHquat.h GC!1.53 11/25/99 10:18:12 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once 

#ifndef __MATHquat_h__
#define __MATHquat_h__

#include "MATHs/MATHstruct.h"/* For the quaternion structures */
#include "MATHs/MATHfloat.h"
#include "MATHs/MATHvector.h"
#include "BASe/BASsys.h"/* For definition of _inline_ */
#include "BASe/BAStypes.h"


/*
 ===================================================================================================
    Aim:    Initialize a quaternion to 0
 ===================================================================================================
 */
_inline_ void MATH_InitQuaternionToZero(MATH_tdst_Quaternion *_pst_Q)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *pl;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pl = (LONG *) _pst_Q;

    pl[3] = pl[2] = pl[1] = pl[0] = Cl_0f;
}

/*
 ===================================================================================================
    Aim:    Initialize a quaternion with 4 floats using values for the floats

    Note:   When possible use MATH_SetQuaternion wich is faster
 ===================================================================================================
 */
_inline_ void MATH_InitQuaternion(MATH_tdst_Quaternion *_pst_Q, float x, float y, float z, float w)
{
    _pst_Q->x = x;
    _pst_Q->y = y;
    _pst_Q->z = z;
    _pst_Q->w = w;
}

/*
 ===================================================================================================
    Aim:    Initialize a quaternion with 4 floats using pointers for the floats
 ===================================================================================================
 */
_inline_ void MATH_SetQuaternion
(
    MATH_tdst_Quaternion    *_pst_Q,
    float                   *px,
    float                   *py,
    float                   *pz,
    float                   *pw
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *pl;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pl = (LONG *) _pst_Q;

    pl[0] = *(LONG *) px;
    pl[1] = *(LONG *) py;
    pl[2] = *(LONG *) pz;
    pl[3] = *(LONG *) pw;
}

/*
 ===================================================================================================
    Aim:    Copy a quaternion
 ===================================================================================================
 */
_inline_ void MATH_CopyQuaternion(MATH_tdst_Quaternion *QDst, MATH_tdst_Quaternion *QSrc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src;
    LONG    *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Dst = (LONG *) QDst;
    Src = (LONG *) QSrc;

    Dst[0] = Src[0];
    Dst[1] = Src[1];
    Dst[2] = Src[2];
    Dst[3] = Src[3];
}

/*
 ===================================================================================================
    Aim:    Convert a vector to a quaternion (with w = 0)
 ===================================================================================================
 */
_inline_ void MATH_ConvertVectorToQuaternion
(
    MATH_tdst_Quaternion    *_pst_QDst,
    MATH_tdst_Vector        *_pst_VSrc
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src;
    LONG    *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Dst = (LONG *) _pst_QDst;
    Src = (LONG *) _pst_VSrc;

    Dst[0] = Src[0];
    Dst[1] = Src[1];
    Dst[2] = Src[2];
    Dst[3] = Cl_0f;
}

/*
 ===================================================================================================
    Aim:    Add two quaternions
 ===================================================================================================
 */
_inline_ void MATH_AddQuaternion
(
    MATH_tdst_Quaternion    *QDst,
    MATH_tdst_Quaternion    *A,
    MATH_tdst_Quaternion    *B
)
{
    QDst->x = fAdd(A->x, B->x);
    QDst->y = fAdd(A->y, B->y);
    QDst->z = fAdd(A->z, B->z);
    QDst->w = fAdd(A->w, B->w);
}

/*
 ===================================================================================================
    Aim:    Substract two quaternions
 ===================================================================================================
 */
_inline_ void MATH_SubQuaternion
(
    MATH_tdst_Quaternion    *QDst,
    MATH_tdst_Quaternion    *A,
    MATH_tdst_Quaternion    *B
)
{
    QDst->x = fSub(A->x, B->x);
    QDst->y = fSub(A->y, B->y);
    QDst->z = fSub(A->z, B->z);
    QDst->w = fSub(A->w, B->w);
}

/*
 ===================================================================================================
    Aim:    Negate a quaternion

    Note:   Warning, with this function the quaternion is directly negated.
 ===================================================================================================
 */
_inline_ void MATH_NegQuaternion(MATH_tdst_Quaternion *_pst_Q)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *pl;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pl = (LONG *) _pst_Q;

    pl[0] += 0x80000000;
    pl[1] += 0x80000000;
    pl[2] += 0x80000000;
    pl[3] += 0x80000000;
}

/*
 ===================================================================================================
    Aim:    Scale a quaternion (multiplies each coordinate with same value)
 ===================================================================================================
 */
_inline_ void MATH_ScaleQuaternion(MATH_tdst_Quaternion *QDst, MATH_tdst_Quaternion *A, float f)
{
    QDst->x = fMul(A->x, f);
    QDst->y = fMul(A->y, f);
    QDst->z = fMul(A->z, f);
    QDst->w = fMul(A->w, f);
}

/*
 ===================================================================================================
    Aim:    Divide a quaternion (divide each coordinate with same value)
 ===================================================================================================
 */
_inline_ void MATH_DivQuaternion(MATH_tdst_Quaternion *QDst, MATH_tdst_Quaternion *A, float f)
{
    vOptInv(&f);

    QDst->x = fMul(A->x, f);
    QDst->y = fMul(A->y, f);
    QDst->z = fMul(A->z, f);
    QDst->w = fMul(A->w, f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_MulQuaternion
(
    MATH_tdst_Quaternion    *QDst,
    MATH_tdst_Quaternion    *A,
    MATH_tdst_Quaternion    *B
)
{
	QDst->x = (A->w * B->x) + (B->w * A->x) + (A->y * B->z) - (A->z * B->y);
	QDst->y = (A->w * B->y) + (B->w * A->y) + (A->z * B->x) - (A->x * B->z);
	QDst->z = (A->w * B->z) + (B->w * A->z) + (A->x * B->y) - (A->y * B->x);
    QDst->w = (A->w * B->w) - (A->x * B->x + A->y * B->y + A->z * B->z);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ float MATH_f_QuaternionDotProduct(MATH_tdst_Quaternion *A, MATH_tdst_Quaternion *B)
{
   return (A->x * B->x + A->y * B->y + A->z * B->z + A->w * B->w);
}

/*
 ===================================================================================================
    Aim:    Returns the norm of a quaternion
 ===================================================================================================
 */
_inline_ float MATH_f_NormQuaternion(MATH_tdst_Quaternion *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f = fSqr(A->x) + fSqr(A->y) + fSqr(A->z) + fSqr(A->w);
    return (fNormalSqrt(f));
}

/*
 ===================================================================================================
    Aim:    Returns the invert of the norm of a quaternion
 ===================================================================================================
 */
_inline_ float MATH_f_InvNormQuaternion(MATH_tdst_Quaternion *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f = fSqr(A->x) + fSqr(A->y) + fSqr(A->z) + fSqr(A->w);
    f = 1.0f / fNormalSqrt(f);
    return f;
}

/*
 ===================================================================================================
    Aim:    Normalize a non nul quaternion

    Note:   The vector must be different from zero
 ===================================================================================================
 */
_inline_ void MATH_NormalizeQuaternion(MATH_tdst_Quaternion *QDst, MATH_tdst_Quaternion *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   fInvNorm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    fInvNorm = MATH_f_InvNormQuaternion(A);
    QDst->x = fMul(A->x, fInvNorm);
    QDst->y = fMul(A->y, fInvNorm);
    QDst->z = fMul(A->z, fInvNorm);
    QDst->w = fMul(A->w, fInvNorm);
}

#ifdef JADEFUSION
_inline_ void MATH_QuaternionTransformVector( MATH_tdst_Vector * _pst_Out, MATH_tdst_Quaternion *Q, MATH_tdst_Vector * _pst_In )
{
	MATH_tdst_Quaternion qIn;
    MATH_tdst_Quaternion qRes;

	// Rotate both vector by quaternion
    MATH_tdst_Quaternion qConj;
    MATH_InitQuaternion( &qConj, -Q->x, -Q->y, -Q->z, Q->w);

    MATH_ConvertVectorToQuaternion( &qIn, _pst_In );

    MATH_MulQuaternion( &qRes, &qIn, &qConj);
    MATH_MulQuaternion( &qRes, Q, &qRes );

    _pst_Out->x = qRes.x;
    _pst_Out->y = qRes.y;
    _pst_Out->z = qRes.z;
}
#endif
/*
 ===================================================================================================
 ===================================================================================================
 */
void MATH_ConvertMatrixToQuaternion(MATH_tdst_Quaternion *QDst, MATH_tdst_Matrix *M);

/*
 ===================================================================================================
 ===================================================================================================
 */
void MATH_ConvertQuaternionToMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Quaternion *Q);

/*
 ===================================================================================================
    Aim:    Spherical lineaire interpolation between two quaternions

    In:     Q0              = Start quaternion
            Q1              = End quaternion
            T               = Time between 0 and 1
            Theta           = Angle between the two quaternions 

    Out:    QDst = [ Q0 . sin ((1-T).Theta) + Q1 . sin (T.Theta) ] / sin Theta
 ===================================================================================================
 */
void MATH_Slerp
(
    MATH_tdst_Quaternion    *QDst,
    float                   T,
    MATH_tdst_Quaternion    *Q0,
    MATH_tdst_Quaternion    *Q1,
    float                   Theta,
    float                   InvSinTheta
);

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_CompressUnitQuaternion
(
    MATH_tdst_CompressedQuaternion  *CDst,
    MATH_tdst_Quaternion            *Q
)
{
    MATH_tdst_Quaternion st_Q;

    /* As Q and -Q represent the same rotation, we can have Q->w >= 0 */
    MATH_CopyQuaternion(&st_Q, Q);
    if(Q->w < 0.0f)
        MATH_NegQuaternion(&st_Q);

    CDst->x = (short) (st_Q.x * 32767.0f + 0.5f);
    CDst->y = (short) (st_Q.y * 32767.0f + 0.5f);
    CDst->z = (short) (st_Q.z * 32767.0f + 0.5f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UncompressUnitQuaternion
(
    MATH_tdst_Quaternion            *QDst,
    MATH_tdst_CompressedQuaternion  *C
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   fSum;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    QDst->x = (C->x / 32767.0f);
    QDst->y = (C->y / 32767.0f);
    QDst->z = (C->z / 32767.0f);

    fSum = fSqr(QDst->x) + fSqr(QDst->y) + fSqr(QDst->z);
    QDst->w = fSum < 1.0f ? fSqrt(1.0f - fSum) : 0.0f;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressXUnitQuaternion
(
    MATH_tdst_Quaternion                *QDst,
    MATH_tdst_UltraCompressedQuaternion *C
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   fSum;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    QDst->x += (C->s / 32767.0f);
    QDst->y += (C->c1 / 32767.0f);
    QDst->z += (C->c2 / 32767.0f);

    fSum = fSqr(QDst->x) + fSqr(QDst->y) + fSqr(QDst->z);
    QDst->w = fSum < 1.0f ? fSqrt(1.0f - fSum) : 0.0f;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressYUnitQuaternion
(
    MATH_tdst_Quaternion                *QDst,
    MATH_tdst_UltraCompressedQuaternion *C
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   fSum;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    QDst->x += (C->c1 / 32767.0f);
    QDst->y += (C->s / 32767.0f);
    QDst->z += (C->c2 / 32767.0f);

    fSum = fSqr(QDst->x) + fSqr(QDst->y) + fSqr(QDst->z);
    QDst->w = fSum < 1.0f ? fSqrt(1.0f - fSum) : 0.0f;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressZUnitQuaternion
(
    MATH_tdst_Quaternion                *QDst,
    MATH_tdst_UltraCompressedQuaternion *C
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   fSum;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    QDst->x += (C->c1 / 32767.0f);
    QDst->y += (C->c2 / 32767.0f);
    QDst->z += (C->s / 32767.0f);

    fSum = fSqr(QDst->x) + fSqr(QDst->y) + fSqr(QDst->z);
    QDst->w = fSum < 1.0f ? fSqrt(1.0f - fSum) : 0.0f;
}

#endif