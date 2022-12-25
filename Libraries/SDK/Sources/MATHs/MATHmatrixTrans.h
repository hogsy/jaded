/*$T MATHmatrixScale.h GC!1.52 10/08/99 11:03:06 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MATHMatrixscale.h: All scale matrix operations £
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHMATRIXTRANS_H_
#define _MATHMATRIXTRANS_H_

#include "MATHs/MATHvector.h"
#include "BASe/BAStypes.h"

/*$4
 ***************************************************************************************************
    HANDLE THE TRANSLATION PART OF THE 4x4 MATRIX £
    £
    Available functions: £
	MATH_b_TestTranslationType £
	MATH_SetTranslationType £
	MATH_ClearTranslationType £
	MATH_UpdateTranslationType(matrix *) £
    MATH_pst_GetTranslation £
    MATH_SetTranslation £
    MATH_SetTranslationNoType £
    MATH_ClearTranslation £
    MATH_CopyTranslation (from one matrix to another)£
    MATH_CopyTranslationNoType £
    MATH_CopyTranslationToVector (from one matrix to one vector) £
    MATH_AddTranslation
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Handle the translation type of the matrix
 ===================================================================================================
 */
_inline_ LONG MATH_b_TestTranslationType(MATH_tdst_Matrix *M)
{
    return((M->lType) & MATH_Ci_Translation);
}

/**/
_inline_ void MATH_SetTranslationType(MATH_tdst_Matrix *M)
{
    (M->lType) |= MATH_Ci_Translation;
}

/**/
_inline_ void MATH_ClearTranslationType(MATH_tdst_Matrix *M)
{
    (M->lType) &= MATH_Ci_NoTranslation;
}

/*
 ===================================================================================================
    Aim:    Update the translation type of a 4*4 matrix

    In:     _pst_MSrc   Matrix to update
 ===================================================================================================
 */
_inline_ void MATH_UpdateTranslationType(MATH_tdst_Matrix *_pst_MSrc)
{
    if(MATH_b_NulVector(&(_pst_MSrc->T)))
        MATH_ClearTranslationType(_pst_MSrc);
    else
        MATH_SetTranslationType(_pst_MSrc);
}

/*
 ===================================================================================================
    Aim:    Returns the Translation of a 4x4 matrix in a standard vector

    Out:    Pointer to the translation
 ===================================================================================================
 */
_inline_ MATH_tdst_Vector *MATH_pst_GetTranslation(MATH_tdst_Matrix *M)
{
    return &(M->T);
}

/*
 ===================================================================================================
    Aim:    Set the translation (to a non nul vector) and affect the type
 ===================================================================================================
 */
_inline_ void MATH_SetTranslation(MATH_tdst_Matrix *M, MATH_tdst_Vector *T)
{
    MATH_CopyVector(&(M->T), T);
    (M->lType) |= MATH_Ci_Translation;
}

/*
 ===================================================================================================
    Aim:    Set the translation without changing the type
 ===================================================================================================
 */
_inline_ void MATH_SetTranslationNoType(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Vector *T)
{
    MATH_CopyVector(&(_pst_MDst->T), T);
}

/*
 ===================================================================================================
    Aim:    Clear the translation and change the bit
 ===================================================================================================
 */
_inline_ void MATH_ClearTranslation(MATH_tdst_Matrix *_pst_MDst)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *l;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    l = (LONG *) &(_pst_MDst->T);

    *l = Cl_0f;
    *(l + 1) = Cl_0f;
    *(l + 2) = Cl_0f;
    (_pst_MDst->lType) &= MATH_Ci_NoTranslation;
}

/*
 ===================================================================================================
    Aim:    Copy the translation and change the destination type

    In:     _pst_MDst   Destination Matrix
            _pst_MSrc   Source Matrix
 ===================================================================================================
 */
_inline_ void MATH_CopyTranslation(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc)
{
    MATH_CopyVector(&(_pst_MDst->T), &(_pst_MSrc->T));
    if (MATH_b_TestTranslationType(_pst_MSrc)) MATH_SetTranslationType(_pst_MDst);
}

/*
 ===================================================================================================
    Aim:    Copy the translation without changing the destination type

    In:     _pst_MDst   Destination Matrix
            _pst_MSrc   Source Matrix
 ===================================================================================================
 */
_inline_ void MATH_CopyTranslationNoType(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc)
{
    MATH_CopyVector(&(_pst_MDst->T), &(_pst_MSrc->T));
}

/*
 ===================================================================================================
    Aim:    Copy the translation of a matrix into a given vector
 ===================================================================================================
 */
_inline_ void MATH_CopyTranslationToVector
(
    MATH_tdst_Vector    *_pst_DstVector,
    MATH_tdst_Matrix    *_pst_MSrc
)
{
    MATH_CopyVector(_pst_DstVector, &(_pst_MSrc->T));
}

/*
 ===================================================================================================
    Aim:    Add the translations of two matrix (optimized using the types)

    In:     _pst_MDst   Destination Matrix
            _pst_M1     First matrix
            _pst_M2     Second matrix
 ===================================================================================================
 */
_inline_ void MATH_AddTranslation
(
    MATH_tdst_Matrix    *_pst_MDst,
    MATH_tdst_Matrix    *_pst_M1,
    MATH_tdst_Matrix    *_pst_M2
)
{
    if(MATH_b_TestTranslationType(_pst_M1))
    {
        if(MATH_b_TestTranslationType(_pst_M2))
        {
            /* Both matrix have translation */
            MATH_AddVector(&(_pst_MDst->T), &(_pst_M1->T), &(_pst_M2->T));
            MATH_UpdateTranslationType(_pst_MDst);
        }
        else
        {
            /* Only M1 has translation */
            MATH_CopyVector(&(_pst_MDst->T), &(_pst_M1->T));
            MATH_SetTranslationType(_pst_MDst);
        }
    }
    else
    {
        if(MATH_b_TestTranslationType(_pst_M2))
        {
            /* Only M2 has translation */
            MATH_CopyVector(&(_pst_MDst->T), &(_pst_M2->T));
            MATH_SetTranslationType(_pst_MDst);
        }
        else
        {
            MATH_InitVectorToZero(&(_pst_MDst->T));
            MATH_ClearTranslationType(_pst_MDst);
        }
    }
}

#endif
