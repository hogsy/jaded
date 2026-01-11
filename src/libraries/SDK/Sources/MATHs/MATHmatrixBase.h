/*$T MATHmatrixBase.h GC!1.52 10/13/99 10:56:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Handle the basic matrix operations £
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHMATRIXBASE_H_
#define _MATHMATRIXBASE_H_

#ifdef PSX2_TARGET
#include "MATHs/PS2/ps2MATHmatrixBase.h"
#else
#include "MATHs/MATHmatrixTrans.h"
#include "BASe/BAStypes.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern  "C"
{
#endif

/*
 ===================================================================================================
    Aim:    Test if a matrix has valid values (no NaN)
 ===================================================================================================
 */
#if 0 // #ifdef _DEBUG // if there are NaN in your matrix, use the MATH_b_CheckMatrixValidity function.
int MATH_b_CheckMatrixValidity ( MATH_tdst_Matrix    *_pst_M);
#else //  _DEBUG
#define MATH_b_CheckMatrixValidity(_pst_M) 1
#endif // _DEBUG


/*$4
 ***************************************************************************************************
    Functions handling the types:: £
    £
    MATH_l_CalculateCorrectType £
    MATH_ClearIdentityType £
    MATH_ClearRotationType £
    MATH_ClearScaleType £
    MATH_ClearOScaleType £
    MATH_ClearAnyScaleType £
    MATH_l_GetType £
    MATH_Set33Type £
    MATH_SetAnyScaleType £
    MATH_SetCorrectType £
    MATH_SetIdentityType £
    MATH_SetFullIdentityType £
    MATH_SetType £
    MATH_SetRotationType £
    MATH_SetScaleType £
    MATH_SetOScaleType £
    MATH_b_TestScaleType £
    MATH_b_TestOScaleType £
    MATH_b_TestAnyScaleType £
    MATH_b_TestIdentityType £
    MATH_b_TestFullIdentityType £
    MATH_b_TestRotationType £
    MATH_b_TypeIsCorrect £
 ***************************************************************************************************
 */


/*
 ===================================================================================================
    Aim:    Get the type of the matrix
 ===================================================================================================
 */
_inline_ LONG MATH_l_GetType(MATH_tdst_Matrix *M)
{
    return(M->lType);
}

/*
 ===================================================================================================
    Aim:    Set the type of the matrix
 ===================================================================================================
 */
_inline_ void MATH_SetType(MATH_tdst_Matrix *M, LONG _lType)
{
    M->lType = _lType;
}

/*
 ===================================================================================================
    Aim:    Handle the Identity type of the Matrix

    Note:   The identity type is set if there is no scale and no rotation, (even if there is a
            translation) !
 ===================================================================================================
 */
_inline_ LONG MATH_b_TestIdentityType(MATH_tdst_Matrix *M)
{
    return((M->lType & (MATH_Ci_Rotation + MATH_Ci_AnyFormOfScale)) == 0);
}

/**/
_inline_ void MATH_SetIdentityType(MATH_tdst_Matrix *M)
{
    (M->lType) &= MATH_Ci_Translation;
}

/*
 ===================================================================================================
    Aim:    Test if the matrix is completely identity
 ===================================================================================================
 */
_inline_ LONG MATH_b_TestFullIdentityType(MATH_tdst_Matrix *M)
{
    return((M->lType & MATH_Ci_AnyFlag) == 0);
}

/*
 ===================================================================================================
    Aim:    Clears all the flags, even the translation flag
 ===================================================================================================
 */
_inline_ void MATH_SetFullIdentityType(MATH_tdst_Matrix *M)
{
    (M->lType) = 0;
}

/*
 ===================================================================================================
    Aim:    Handle the rotation type of the matrix
 ===================================================================================================
 */
_inline_ LONG MATH_b_TestRotationType( MATH_tdst_Matrix *M)
{
    return((M->lType) & MATH_Ci_Rotation);
}

/**/
_inline_ void MATH_SetRotationType(MATH_tdst_Matrix *M)
{
    (M->lType) |= MATH_Ci_Rotation;
}

/**/
_inline_ void MATH_ClearRotationType(MATH_tdst_Matrix *M)
{
    (M->lType) &= MATH_Ci_NoRotation;
}

/*
 ===================================================================================================
    Aim:    Handle the scale type of the matrix
 ===================================================================================================
 */
_inline_ char MATH_b_TestScaleType(MATH_tdst_Matrix *M)
{
    return ((char)((M->lType) & MATH_Ci_Scale));
}

/**/
_inline_ void MATH_SetScaleType(MATH_tdst_Matrix *M)
{
    (M->lType) |= MATH_Ci_Scale;
}

/**/
_inline_ void MATH_ClearScaleType(MATH_tdst_Matrix *M)
{
    (M->lType) &= MATH_Ci_NoScale;
}

/*
 ===================================================================================================
    Aim:    Handle the "OScale"(oriented scale) type of the matrix

    Note:   The oriented scale is a scale in one direction, not aligned with any of the local axis
            of the matrix. (In a base where the matrix is diagonal, only one axis is scaled)
 ===================================================================================================
 */
_inline_ char MATH_b_TestOScaleType(MATH_tdst_Matrix *M)
{
    return ((char)((M->lType) & MATH_Ci_OScale));
}

/**/
_inline_ void MATH_SetOScaleType(MATH_tdst_Matrix *M)
{
    (M->lType) |= MATH_Ci_OScale;
}

/**/
_inline_ void MATH_ClearOScaleType(MATH_tdst_Matrix *M)
{
    (M->lType) &= MATH_Ci_NoOScale;
}

/*
 ===================================================================================================
    Aim:    Handle the "AnyScale" () type of the matrix

    Note:   The Any Scale is a scale in any possible directions, not aligned with any of the local
            axis of the matrix. (In a base where the matrix is diagonal, 2 or 3 axis are scaled)
 ===================================================================================================
 */
_inline_ char MATH_b_TestAnyScaleType(MATH_tdst_Matrix *M)
{
    return ((char)((M->lType) & MATH_Ci_AnyScale));
}

/**/
_inline_ void MATH_SetAnyScaleType(MATH_tdst_Matrix *M)
{
    (M->lType) |= MATH_Ci_AnyScale;
}

/**/
_inline_ void MATH_ClearAnyScaleType(MATH_tdst_Matrix *M)
{
    (M->lType) &= MATH_Ci_NoAnyScale;
}

/*
 ===================================================================================================
    Aim:    Handle the "AnyFormOfScale" () type of the matrix

 ===================================================================================================
 */
_inline_ char MATH_b_TestAnyFormOfScaleType(MATH_tdst_Matrix *M)
{
    return ((char)((M->lType) & MATH_Ci_AnyFormOfScale));
}

/*
 ===================================================================================================
    Aim:    Calculate the correct type for one matrix
 ===================================================================================================
 */
LONG    MATH_l_CalculateCorrectType(MATH_tdst_Matrix *_pst_M);

/*
 ===================================================================================================
    Aim:    Check the matrix type. Return 1 if the type is correct, 0 if the type is incorrect
 ===================================================================================================
 */
BOOL    MATH_b_TypeIsCorrect(MATH_tdst_Matrix *_pst_M);

/*
 ===================================================================================================
    Aim:    Sets a correct type to a matrix, whatever the type was before...
 ===================================================================================================
 */
void    MATH_SetCorrectType(MATH_tdst_Matrix *M);

/*$4
 ***************************************************************************************************
    Available functions:£
    £
    MATH_ClearRotation £
    MATH_ClearScale £
    MATH_CopyMatrix £
    MATH_GetRotationMatrix £
    MATH_pst_GetXAxis £
    MATH_pst_GetYAxis £
    MATH_pst_GetZAxis £
    MATH_SetIdentityMatrix £
    MATH_SetIdentity33Matrix £
    MATH_SetMatrixWithType £
    MATH_SetXAxis £
    MATH_SetYAxis £
    MATH_SetZAxis £
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Returns a pointer to the X axis of the transform matrix

    In:     _pst_MSrc   Source Matrix 

    Out:    Pointer to a vector (X-axis)
 ===================================================================================================
 */
_inline_ MATH_tdst_Vector *MATH_pst_GetXAxis(MATH_tdst_Matrix *_pst_MSrc)
{
    return (MATH_tdst_Vector *) &(_pst_MSrc->Ix);
}

_inline_ const MATH_tdst_Vector *MATH_pst_GetXAxisConst(const MATH_tdst_Matrix *_pst_MSrc)
{
    return (const MATH_tdst_Vector *) &(_pst_MSrc->Ix);
}


/*
 ===================================================================================================
    Aim:    Returns a pointer to the Y axis of the transform matrix

    In:     _pst_MSrc   Source Matrix 

    Out:    Pointer to a vector (Y-axis)
 ===================================================================================================
 */
_inline_ MATH_tdst_Vector *MATH_pst_GetYAxis(MATH_tdst_Matrix *_pst_MSrc)
{
    return (MATH_tdst_Vector *) &(_pst_MSrc->Jx);
}

_inline_ const MATH_tdst_Vector *MATH_pst_GetYAxisConst(const MATH_tdst_Matrix *_pst_MSrc)
{
    return (const MATH_tdst_Vector *) &(_pst_MSrc->Jx);
}

/*
 ===================================================================================================
    Aim:    Returns a pointer to the Z axis of the transform matrix

    In:     _pst_MSrc   Source Matrix 

    Out:    Pointer to a vector (Z-axis)
 ===================================================================================================
 */
_inline_ MATH_tdst_Vector *MATH_pst_GetZAxis(MATH_tdst_Matrix *_pst_MSrc)
{
    return (MATH_tdst_Vector *) &(_pst_MSrc->Kx);
}

_inline_ const MATH_tdst_Vector *MATH_pst_GetZAxisConst(const MATH_tdst_Matrix *_pst_MSrc)
{
    return (const MATH_tdst_Vector *) &(_pst_MSrc->Kx);
}

/*
 ===================================================================================================
    Aim:    Set the X axis of a matrix (1st row) with a given vector
 ===================================================================================================
 */
_inline_ void MATH_SetXAxis(MATH_tdst_Matrix *M, const MATH_tdst_Vector *_pst_I)
{
    MATH_CopyVector((MATH_tdst_Vector *) &M->Ix, _pst_I);
	MATH_b_CheckMatrixValidity(M);    
}

/*
 ===================================================================================================
    Aim:    Set the Y axis of a matrix (2nd row) with a given vector
 ===================================================================================================
 */
_inline_ void MATH_SetYAxis(MATH_tdst_Matrix *M, const MATH_tdst_Vector *_pst_J)
{
    MATH_CopyVector((MATH_tdst_Vector *) &M->Jx, _pst_J);
	MATH_b_CheckMatrixValidity(M);    
}

/*
 ===================================================================================================
    Aim:    Set the Z axis of a matrix (3rd row) with a given vector
 ===================================================================================================
 */
_inline_ void MATH_SetZAxis(MATH_tdst_Matrix *M, const MATH_tdst_Vector *_pst_K)
{
    MATH_CopyVector((MATH_tdst_Vector *) &M->Kx, _pst_K);
	MATH_b_CheckMatrixValidity(M);    
}

/*
 ===================================================================================================
    Aim:    Clear the scale (Sets the last column of the 4x4 matrix to (0,0,0,1))
 ===================================================================================================
 */
_inline_ void MATH_ClearScale(MATH_tdst_Matrix *_pst_M, char _b_cleartype)
{
    fSetLongToFloat(_pst_M->Sx, Cl_0f);
    fSetLongToFloat(_pst_M->Sy, Cl_0f);
    fSetLongToFloat(_pst_M->Sz, Cl_0f);
    //fSetLongToFloat(_pst_M->w, Cl_1f);
    if(_b_cleartype)
        MATH_ClearScaleType(_pst_M);
}

/*
 ===================================================================================================
    Aim:    Initialize a 4x4 matrix with given axis, translation, scale and type

    In:     M       Matrix to be initialized
            I       X-axis
            J       Y-axis
            K       Z-axis
            T       Translation vector
            S       Scale vector
            w       Last coordinate of homogenous projection vector (usually 1)
            _lType  Type of the matrix
 ===================================================================================================
 */
void    MATH_SetMatrixWithType
        (
            MATH_tdst_Matrix    *M,
            MATH_tdst_Vector    *I,
            MATH_tdst_Vector    *J,
            MATH_tdst_Vector    *K,
            MATH_tdst_Vector    *T,
            MATH_tdst_Vector    *S,
            float               w,
            LONG                _lType
        );

/*
 ===================================================================================================
    Aim:    Initialize the 3x3 matrix with the 3 axis vectors and the type
 ===================================================================================================
 */
void    MATH_Set33Matrix
        (
            MATH_tdst_Matrix    *M,
            MATH_tdst_Vector    *I,
            MATH_tdst_Vector    *J,
            MATH_tdst_Vector    *K
        );

/*
 ===================================================================================================
    Aim:    Copy the whole 4x4 matrix (including type, translation, projection)

    Time:   K6/2-333 25 clocks
 ===================================================================================================
 */
_inline_ void MATH_CopyMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *lSrc, *lDst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    lSrc = (LONG *) M;
    lDst = (LONG *) MDst;

    if(MDst != M)
    {
    	*MDst = *M;
    }
	MATH_b_CheckMatrixValidity(MDst);    
}

/*
 ===================================================================================================
    Aim:    Copy the 3x3 matrix

    Note:   Copy the rotation type
 ===================================================================================================
 */
_inline_ void MATH_Copy33Matrix(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *pl_Src, *pl_Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_b_CheckMatrixValidity(_pst_MSrc);    
    pl_Src = (LONG *) &(_pst_MSrc->Ix);
    pl_Dst = (LONG *) &(_pst_MDst->Ix);

    *pl_Dst = *pl_Src;
    *(pl_Dst + 1) = *(pl_Src + 1);
    *(pl_Dst + 2) = *(pl_Src + 2);

    *(pl_Dst + 4) = *(pl_Src + 4);
    *(pl_Dst + 5) = *(pl_Src + 5);
    *(pl_Dst + 6) = *(pl_Src + 6);

    *(pl_Dst + 8) = *(pl_Src + 8);
    *(pl_Dst + 9) = *(pl_Src + 9);
    *(pl_Dst + 10) = *(pl_Src + 10);

    if(MATH_b_TestRotationType(_pst_MSrc))
        MATH_SetRotationType(_pst_MDst);
}

/*
 ===================================================================================================
    Aim:    Makes a pure rotation matrix (without translation or scale) from a 4x4 matrix
 ===================================================================================================
 */
_inline_ void MATH_GetRotationMatrix(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc)
{
    MATH_Copy33Matrix(_pst_MDst, _pst_MSrc);
    MATH_ClearScale(_pst_MDst, 1);
    MATH_ClearTranslation(_pst_MDst);
    MATH_SetRotationType(_pst_MDst);

	MATH_b_CheckMatrixValidity(_pst_MDst);    
}

/*
 ===================================================================================================
    Aim:    Set matrix to identity and clear the translation & projection

    Time:   K6/2-333 20 clocks
 ===================================================================================================
 */
_inline_ void MATH_SetIdentityMatrix(MATH_tdst_Matrix *_pst_MSrc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *lPointer;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    lPointer = (LONG *) &(_pst_MSrc->Ix);

    *lPointer = Cl_1f;
    *(lPointer + 1) = Cl_0f;
    *(lPointer + 2) = Cl_0f;
    *(lPointer + 3) = Cl_0f;

    *(lPointer + 4) = Cl_0f;
    *(lPointer + 5) = Cl_1f;
    *(lPointer + 6) = Cl_0f;
    *(lPointer + 7) = Cl_0f;

    *(lPointer + 8) = Cl_0f;
    *(lPointer + 9) = Cl_0f;
    *(lPointer + 10) = Cl_1f;
    *(lPointer + 11) = Cl_0f;

    *(lPointer + 12) = Cl_0f;
    *(lPointer + 13) = Cl_0f;
    *(lPointer + 14) = Cl_0f;
    *(lPointer + 15) = Cl_1f;

    MATH_SetIdentityType(_pst_MSrc);
}

/*
 ===================================================================================================
    Aim:    Set the 3x3 matrix to identity

    Note:   The rotation flag is cleared, but the scale and the translation don't change
 ===================================================================================================
 */
_inline_ void MATH_SetIdentity33Matrix(MATH_tdst_Matrix *_pst_M)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *pl_Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pl_Dst = (LONG *) &(_pst_M->Ix);

    *pl_Dst = Cl_1f;
    *(pl_Dst + 1) = Cl_0f;
    *(pl_Dst + 2) = Cl_0f;

    *(pl_Dst + 4) = Cl_0f;
    *(pl_Dst + 5) = Cl_1f;
    *(pl_Dst + 6) = Cl_0f;

    *(pl_Dst + 8) = Cl_0f;
    *(pl_Dst + 9) = Cl_0f;
    *(pl_Dst + 10) = Cl_1f;

    MATH_ClearRotationType(_pst_M);
}

/*
 ===================================================================================================
    Aim:    Clears the rotation from the matrix (doesn't change the scale or translation)
 ===================================================================================================
 */
_inline_ void MATH_ClearRotation(MATH_tdst_Matrix *_pst_MSrc)
{
    MATH_SetIdentity33Matrix(_pst_MSrc);
}

/*$4
 ***************************************************************************************************
    Test between matrixes £
    £
    MATH_b_EqMatrixWithEpsilon(matrix*,matrix*,epsilon)
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Test if two matrixes are equal
 ===================================================================================================
 */
_inline_ int MATH_b_EqMatrixWithEpsilon
(
    const MATH_tdst_Matrix    *_pst_M1,
    const MATH_tdst_Matrix    *_pst_M2,
    float               f_epsilon
)
{
    return MATH_b_EqVectorWithEpsilon( MATH_pst_GetXAxisConst(_pst_M1),
                                       MATH_pst_GetXAxisConst(_pst_M2),
                                       f_epsilon ) &&
           MATH_b_EqVectorWithEpsilon( MATH_pst_GetYAxisConst(_pst_M1),
                                       MATH_pst_GetYAxisConst(_pst_M2),
                                       f_epsilon ) &&
           MATH_b_EqVectorWithEpsilon( MATH_pst_GetZAxisConst(_pst_M1),
                                       MATH_pst_GetZAxisConst(_pst_M2),
                                       f_epsilon );
}


	
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif
#endif
