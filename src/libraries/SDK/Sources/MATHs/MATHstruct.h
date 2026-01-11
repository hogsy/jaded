/*$T MATHstruct.h GC!1.52 10/13/99 10:47:47 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Structures
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHSTRUCT_H_
#define _MATHSTRUCT_H_

#include "BASe/BAStypes.h"

/* Types pour la matrice */
#define MATH_Ci_Translation     2
#define MATH_Ci_NoTranslation   (0xFFFFFFFF - MATH_Ci_Translation)

#define MATH_Ci_Rotation        4
#define MATH_Ci_NoRotation      (0xFFFFFFFF - MATH_Ci_Rotation)

/*
 * MATH_Ci_Scale indicates that there is scale on one or more of the local axis of the matrix.
 * This is the most common used form of scale £
 * In this case the scale coeffs for each axis are stored in the last column of the matrix
 */
#define MATH_Ci_Scale   8
#define MATH_Ci_NoScale (0xFFFFFFFF - MATH_Ci_Scale)

/*
 * MATH_Ci_OScale is used to indicate that there a scale oriented in one direction only (but a
 * non "local-axis aligned" direction )£
 * in this case, the scale is stored in the last column of the matrix, as the direction and norm
 * of the scale £
 * This flag must not be set with the MATH_Ci_Scale
 */
#define MATH_Ci_OScale      16
#define MATH_Ci_NoOScale    (0xFFFFFFFF - MATH_Ci_OScale)

/*
 * MATH_Ci_AnyScale that there is scale in one ore more non "local-axis aligned" directions £
 * in this case, 3 coeffs are not enough, the scale needs a whole matrix. £
 * This is the only case where we choose to store the scale "mixed" within the transformation
 * matrix
 */
#define MATH_Ci_AnyScale                    32
#define MATH_Ci_NoAnyScale                  (0xFFFFFFFF - MATH_Ci_AnyScale)

#define MATH_Ci_Complex                     (MATH_Ci_Scale + MATH_Ci_Rotation)
#define MATH_Ci_AnyFormOfScale              (MATH_Ci_Scale + MATH_Ci_OScale + MATH_Ci_AnyScale)
#define MATH_Ci_AnyFlag                     (MATH_Ci_AnyFormOfScale + MATH_Ci_Rotation + MATH_Ci_Translation)
#define MATH_Ci_NoRotationNoAnyFormOfScale  (0xFFFFFFFF - MATH_Ci_Rotation - MATH_Ci_AnyFormOfScale)
#define MATH_Ci_FullIdentityType            (0xFFFFFFFF - MATH_Ci_AnyFlag)

/*
 ---------------------------------------------------------------------------------------------------
    Standard 3D vector
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MATH_tdst_Vector_
{
    float   x;
    float   y;
    float   z;
} MATH_tdst_Vector;

typedef struct  MATH_tdst_CompressedVector_
{
    short   x;
    short   y;
    short   z;
} MATH_tdst_CompressedVector;

typedef struct  MATH_tdst_UltraCompressedVector_
{
    short   s;
    char    c1;
    char    c2;
} MATH_tdst_UltraCompressedVector;

/*
 ---------------------------------------------------------------------------------------------------
    Vecteur homogene (4 coordonnees)
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MATH_tdst_HomVector_
{
    float   x, y, z, w;
} MATH_tdst_HomVector;

/*
 ---------------------------------------------------------------------------------------------------
    Quaternion (4 coordonnees, idem vecteur homogène)
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MATH_tdst_Quaternion_
{
    float   x, y, z, w;
} MATH_tdst_Quaternion;

typedef struct  MATH_tdst_CompressedQuaternion_
{
    short   x, y, z;
} MATH_tdst_CompressedQuaternion;

typedef struct  MATH_tdst_UltraCompressedQuaternion_
{
    short   s;                          /* One short for one coordinate */
    char    c1, c2;                     /* Two char for the two other coordinates */
} MATH_tdst_UltraCompressedQuaternion;

/*
 ---------------------------------------------------------------------------------------------------
    Matrice 33
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MATH_tdst_33Matrix_
{
    MATH_tdst_Vector    I;
    MATH_tdst_Vector    J;
    MATH_tdst_Vector    K;
    LONG                lType;
} MATH_tdst_33Matrix;

/*
 ---------------------------------------------------------------------------------------------------
    Matrice 4*4, système "left-handed", c'est à dire que les axes sont en lignes et pas en colonne
    !!
 ---------------------------------------------------------------------------------------------------
 */
#if defined(_XBOX) || defined(_M_X86) || defined(_XENON)
typedef struct MATH_tdst_Matrix_
{
    __declspec(align(16)) float               Ix;
	float Iy, Iz, Sx; /* I,J,K sont les vecteurs qui definissent les axes locaux
                                         * dans le repere global. */
    float               Jx, Jy, Jz, Sy; /* S le vecteur scale */
    float               Kx, Ky, Kz, Sz;
    MATH_tdst_Vector    T;              /* T pour Translation */
    float               w;
    LONG                lType;
}__declspec(align(16))MATH_tdst_Matrix;

#else
typedef struct MATH_tdst_Matrix_
{
    float               Ix, Iy, Iz, Sx; /* I,J,K sont les vecteurs qui definissent les axes locaux
                                         * dans le repere global. */
    float               Jx, Jy, Jz, Sy; /* S le vecteur scale */
    float               Kx, Ky, Kz, Sz;
    MATH_tdst_Vector    T;              /* T pour Translation */
    float               w;
    LONG                lType;
}MATH_tdst_Matrix;
#endif
#endif
