/*$T MATHmatrixAdvanced.h GC! 1.081 05/03/00 14:28:50 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Aim: Handle the various advanced matrix operations £
 */
#ifndef _MATHMATRIXADVANCED_H_
#define _MATHMATRIXADVANCED_H_

#include "BASe/BAStypes.h"

#include "MATHs/MATHmatrixBase.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Advanced operations on the 3x3 part of the matrix £
    functions: £
    £
    MATH_f_Det33Matrix(matrix *) £
    MATH_Invert33Matrix(matrix*) £
    MATH_TranspEq33Matrix £
    MATH_Transp33MatrixWithoutBuffer £
    MATH_Orthonormalize £
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Calculates the determinant of the 3x3 part of the matrix

    Time:   K6/2-333 59 clocks
 =======================================================================================================================
 */
float MATH_f_Det33Matrix(MATH_tdst_Matrix *M);

/*
 =======================================================================================================================
    Aim:    Inverts the 3x3 part of a 4x4 matrix without changing the translation

    Time:   K6/2-333 196 clocks (really optimized)

    Note:   The destination matrix MUST be different from the source matrix
 =======================================================================================================================
 */
void MATH_Invert33Matrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M);

/*
 =======================================================================================================================
    Aim:    Transpose the 3x3 part of a 4x4 matrix

    Time:   K6/2-333 18 clocks

    Note:   This function works only if source is different from destination use MATH_TranspEq33Matrix if source =
            destination
 =======================================================================================================================
 */
void MATH_Transp33MatrixWithoutBuffer(MATH_tdst_Matrix *Dest, MATH_tdst_Matrix *M);

/*
 =======================================================================================================================
    Aim:    Transpose the 3x3 part of a 4x4 matrix

    Time:   K6/2-333 14 clocks

    Note:   This function transposes directly the Matrix use MATH_Transp33MatrixWithoutBuffer if source is different
            from destination
 =======================================================================================================================
 */
void MATH_TranspEq33Matrix(MATH_tdst_Matrix *M);

/* Aim: Orthonormalize a rotation matrix */
void	MATH_Orthonormalize(MATH_tdst_Matrix *);

/*$4
 ***********************************************************************************************************************
    Available functions:£
    £
    MATH_CalculateEigenValues £
    MATH_f_Trace £
    MATH_f_Det £
    MATH_MatrixBlend £
 ***********************************************************************************************************************
 */

/*
 * Aim: Calculate the eigenvalues (valeurs propres) of the matrix Out:
 * _pst_Vector:: This vector holds the 3 eigenvalues of the Matrix
 */
void	MATH_CalculateEigenValues(MATH_tdst_Matrix *, MATH_tdst_Vector *);

/*
 =======================================================================================================================
    Aim returns the trace of the Matrix
 =======================================================================================================================
 */
_inline_ float MATH_f_Trace(MATH_tdst_Matrix *M)
{
	MATH_b_CheckMatrixValidity(M);
	return(M->Ix + M->Jy + M->Kz);
}

#define MATH_f_Det	MATH_f_Det33Matrix

void	MATH_MatrixBlend(MATH_tdst_Matrix *, MATH_tdst_Matrix *, MATH_tdst_Matrix *, float f_Coeff, char _b_InitBlend);

void	MATH_QuaternionBlend
		(
			MATH_tdst_Matrix *,
			MATH_tdst_Quaternion *,
			MATH_tdst_Quaternion *,
			float f_Coeff,
			char _b_InitBlend
		);
#ifdef JADEFUSION
void	MATH_QuaternionBlend
        (
            MATH_tdst_Quaternion *,
            MATH_tdst_Quaternion *,
            MATH_tdst_Quaternion *,
            float f_Coeff,
            char _b_InitBlend
         );
#endif
void MATH_ConvertMatrixToQuaternion(MATH_tdst_Quaternion *QDst, MATH_tdst_Matrix *M);
void MATH_ConvertQuaternionToMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Quaternion *Q);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
