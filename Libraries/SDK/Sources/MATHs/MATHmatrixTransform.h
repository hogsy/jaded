/*$T MATHmatrixTransform.h GC!1.52 10/19/99 11:18:09 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MATHMatrix.h: All matrix operations
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHMATRIXTRANSFORM_H_
#define _MATHMATRIXTRANSFORM_H_

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "MATHs/MATHmatrixAdvanced.h"
#include "MATHs/MATHmatrixBase.h"

/*$4
 ***************************************************************************************************
    TRANSFORMATIONS (for 4*4 matrix): £
    £
    MATH_TransformVector £
    MATH_TransformVertex £
    MATH_TransformHomVector £
    MATH_MulMatrixMatrix £
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Transforms a 3 coordinates vector of the space (tranformation only, no translation)

    Time:   K6/2-333 79 - 108 (79 if destination vector is different from source vector) (108 if
            source = destination)

    Note:   The transformation is done by the operation V*M we are in a left-handed system where
            the vector is multiplied left to the matrix. So, the transformation matrix is: M =
            S.R.T (S=scale, R=rotation, T= tanslation)
 ===================================================================================================
 */
void    MATH_TransformVector(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc);

/*
 ===================================================================================================
    Aim:    Transforms a 3 coordinates vector of the space (tranformation only, no translation and
            no scale)
 ===================================================================================================
 */
void    MATH_TransformVectorNoScale
        (
            MATH_tdst_Vector    *VDst,
            MATH_tdst_Matrix    *M,
            MATH_tdst_Vector    *VSrc
        );

/*
 ===================================================================================================
    Aim:    Transforms a 3 coordinates point of the space (tranformation + translation)

    Time:   K6/2-333 95 - 135 (95 if destination vector is different from source vector) (135 if
            source = destination)

    Note:   The transformation is done by the operation V*M we are in a left-handed system where
            the vector is multiplied left to the matrix. So, the transformation matrix is: M =
            S.R.T (S=scale, R=rotation, T= tanslation)
 ===================================================================================================
 */
void    MATH_TransformVertex(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc);

/*
 ===================================================================================================
    Aim:    Transforms a 3 coordinates vector of the space (tranformation only, no translation and
            no scale)
 ===================================================================================================
 */
void    MATH_TransformVertexNoScale
        (
            MATH_tdst_Vector    *VDst,
            MATH_tdst_Matrix    *M,
            MATH_tdst_Vector    *VSrc
        );

/*
 ===================================================================================================
    Aim:    Transforms a 4 coordinates point or vector of the space

    Note:   - The 4th coordinate always indicates if it's a point or a vector £
            (x,y,z,0) ==> vector (x,y,z,1) ==> point £
            this function works only if the 4th coordinate is 0 or 1 and if the last column of the
            4x4 matrix is (0,0,0,1) if not, use MATH_TransformHomVector2 £
            - the transformation is done by the operation V*M we are in a left-handed system where
            the vector is multiplied left to the matrix. So, the transformation matrix is: M =
            S.R.T (S=scale, R=rotation, T= tanslation)
 ===================================================================================================
 */
void    MATH_TransformHomVector
        (
            MATH_tdst_HomVector *VDst,
            MATH_tdst_Matrix    *M,
            MATH_tdst_HomVector *VSrc
        );

/*
 ===================================================================================================
    Aim:    4x4 Matrix multiplication (without taking care of the last column)

    Time:   K6/2-333 37-291 clocks

    Note:   - The destination matrix MUST be different from both source matix £
            - Precisions about timing: £
            37 clocks if M2 is an identity matrix £
            105 clocks if M1 is identity £
            291 clocks if no identity matrix £
            - The type is always updated (it's different from MATH_Mul33MatrixMatrix)
 ===================================================================================================
 */
void    MATH_MulMatrixMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2);

/*$4
 ***************************************************************************************************
    MATH_Mul33MatrixMatrix £
    MATH_InvertMatrix £
    MATH_VertexGlobalToLocal (vectorDst*,matrix*,vectorSrc*)£
    MATH_VertexLocalToGlobal (vectorDst*,matrix*,vectorSrc*)£
    MATH_VectorLocalToGlobal (vectorDst*,matrix*,vectorSrc*)£
    MATH_VectorGlobalToLocal (vectorDst*,matrix*,vectorSrc*)£
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Multiply the 3x3 part of the matrix without changing the translation

    Time:   K6/2-333 25-221 clocks

    Note:   Destination matrix must be different from both source matrixes the type of the matrix
            is updated if _bUpdateType is true - £
            timing precisions: £
            25 clocks if M1 is identity £
            27 clocks if M2 is identity £
            214 clocks without update £
            221 clocks with update
 ===================================================================================================
 */
void    MATH_Mul33MatrixMatrix
        (
            MATH_tdst_Matrix    *MDst,
            MATH_tdst_Matrix    *M1,
            MATH_tdst_Matrix    *M2,
            char                _bUpdateType
        );

/*
 ===================================================================================================
    Aim:    4*4 Matrix inversion

    Note:   The destination matrix MUST be different from the source matrix £
            1. transposes the 3x3 part £
            2. inverts the scale vector £
            3. neg the trabslation and trabsform it £
 ===================================================================================================
 */
void    MATH_InvertMatrix(MATH_tdst_Matrix *_pst_Dst, MATH_tdst_Matrix *_pst_Src);

/*
 ===================================================================================================
    Aim:    Transform a vector (not a vertex!) from a global to a local coodinates
 ===================================================================================================
 */
void    MATH_VectorGlobalToLocal
        (
            MATH_tdst_Vector    *_pst_VDst,
            MATH_tdst_Matrix    *_pst_MSrc,
            MATH_tdst_Vector    *_pst_VSrc
        );

/*
 ===================================================================================================
    Aim:    Transform a vector (not a vertex!) from a local to a global coodinates
 ===================================================================================================
 */
#ifdef PSX2_TARGET
#define MATH_VectorLocalToGlobal MATH_TransformVectorNoScale
#else
void    MATH_VectorLocalToGlobal
        (
            MATH_tdst_Vector    *_pst_VDst,
            MATH_tdst_Matrix    *_pst_MSrc,
            MATH_tdst_Vector    *_pst_VSrc
        );
#endif

/*
 ===================================================================================================
    Aim:    Transform a vertex from a global to a local coodinate
 ===================================================================================================
 */
void    MATH_VertexGlobalToLocal
        (
            MATH_tdst_Vector    *_pst_VDst,
            MATH_tdst_Matrix    *_pst_MSrc,
            MATH_tdst_Vector    *_pst_VSrc
        );
/*
 ===================================================================================================
    Aim:    Transform a vertex from a local to a global coodinate
 ===================================================================================================
 */
#ifdef PSX2_TARGET
#define MATH_VertexLocalToGlobal MATH_TransformVertex
#else
void    MATH_VertexLocalToGlobal
        (
            MATH_tdst_Vector    *_pst_VDst,
            MATH_tdst_Matrix    *_pst_MSrc,
            MATH_tdst_Vector    *_pst_VSrc
        );
#endif


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _MATHMATRIXTRANSFORM_H_ */
