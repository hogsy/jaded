/*$T MATHmatrixRot.h GC!1.52 10/21/99 12:28:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MATHMatrixscale.h: All scale matrix operations £
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHMATRIXROT_H_
#define _MATHMATRIXROT_H_

#include "MATHs/MATHmatrixBase.h"
#include "BASe/BAStypes.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    ROTATION MATRIX CREATIONS, £
    £
    MATH_MakeRotationMatrix_AxisAngle £
    MATH_MakeRotationMatrix_PointAxisAngle £
    MATH_MakeRotationMatrix_PointsAngle £
    MATH_MakeRotationMatrix_UsingSight £
    MATH_MakeRotationMatrix_UsingBanking £
    MATH_MakeRotationMatrix_XAxis £
    MATH_MakeRotationMatrix_YAxis £
    MATH_MakeRotationMatrix_ZAxis £
    Because it is not useful right now, the creation using euler angles (1 angle around each axis)
    is not done yet...
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Create a rotation matrix around a unit axis going thru the origin, and returns the
            corresponding Quaternion if asked

    Ex:     // We make a rotation matrix of Pi/4 radians around vector J // and initialise the rest
            of the matrix. MATH_MakeRotationMatrix_AxisAngle( &M, &MATH_st_BaseVectorJ, Cf_PiBy4,
            0, 1 );

    In:     _pst_MDst               Destination 4x4 matrix
            _pst_Axis               Unit axis to rotate around
            _f_Angle                Angle (in radians) to rotate
            _pst_Q                  Quaternion filled by the funtion, except if 0
            _b_InitRestOfMatrix     Set to 1 if you want the rest of the 4x4 matrix tobe cleared 

    Note:   This function performs the rotation around an axis countaining the origin. if you want
            to rotate around an axis defined by a point (local coordinate) and an vector use the
            function MATH_MakeRotationMatrix_PointAxisAngle for a clear understanding of what the
            function does, here is an explaination: Assuming you want to rotate vectors around the
            origin of your coordinate system. (If you want to rotate around some other point,
            subtract its coordinates from the point you are rotating, do the rotation, and then add
            back what you subtracted.) In 3-D, you need not only an angle, but also an axis.
            Actually, you need 3 independent numbers, and these come in a variety of flavors. The
            flavor chosen is unit quaternions: 4 numbers that square and add up to +1. You can
            write these as [(x,y,z),w], with 4 real numbers, or [v,w], with v, a 3-D vector
            pointing aLONG the axis. The concept of an axis is unique to 3-D. It is a line through
            the origin containing all the points which do not move during the rotation. So we know
            if we are turning forwards or back, we use a vector pointing out aLONG the line.
            Suppose you want to use unit vector u as your axis, and rotate by 2t degrees. (Yes,
            that's twice t.) Make a quaternion [u sin t, cos t]. You can use the quaternion -- call
            it q -- directly on a vector v with quaternion multiplication, q v q^-1, or just
            convert the quaternion to a 3x3 matrix M. If the components of q are {(x,y,z),w], then
            you want the matrix £
            1-2(yy+zz), 2(xy-wz), 2(xz+wy), £
            2(xy+wz),1-2(xx+zz), 2(yz-wx), £
            2(xz-wy), 2(yz+wx),1-2(xx+yy). £
            Rotations, translations, and much more are explained in all basic computer graphics
            texts. Quaternions are covered briefly in [Foley], and more extensively in several
            Graphics Gems, and the SIGGRAPH 85 proceedings.
 ===================================================================================================
 */
void MATH_MakeRotationMatrix_AxisAngle
(
    MATH_tdst_Matrix        *_pst_MDst,
    MATH_tdst_Vector        *_pst_Axis,
    float                   _f_Angle,
    MATH_tdst_Quaternion    *_pst_Q,
    char                    _b_InitRestOfMatrix
);

/*
 ===================================================================================================
    Aim:    Create a rotation matrix around a unit axis not necessary going thru the origin,and
            returns the corresponding Quaternion if asked

    In:     _pst_MDst               Destination 4x4 matrix
            _pst_Point              Point of the axis
            _pst_Axis               Unit axis to rotate around
            _f_Angle                Angle (in radians) to rotate
            _pst_Q                  Quaternion filled by the funtion, except if 0
            _b_InitRestOfMatrix     Set to 1 if you want the rest of the 4x4 matrix to be cleared
 ===================================================================================================
 */
void MATH_MakeRotationMatrix_PointAxisAngle
(
    MATH_tdst_Matrix        *_pst_MDst,
    MATH_tdst_Vector        *_pst_Point,
    MATH_tdst_Vector        *_pst_Axis,
    float                   _f_Angle,
    MATH_tdst_Quaternion    *_pst_Q,
    char                    _b_InitRestOfMatrix
);

/*
 ===================================================================================================
    Aim:    Create a rotation matrix around a unit axis not necessary going thru the origin,and
            returns the corresponding Quaternion if asked

    In:     _pst_MDst               Destination 4x4 matrix
            _pst_Point1             1st point of the axis
            _pst_Point2             2nd point of the axis
            _f_Angle                Angle (in radians) to rotate
            _pst_Q                  Quaternion filled by the funtion, except if 0
            _b_InitRestOfMatrix     Set to 1 if you want the rest of the 4x4 matrix to be cleared 

    Note:   This function is the same as MATH_MakeRotationMatrix_PointAxisAngle except that the
            axis is defined by two points
 ===================================================================================================
 */
void MATH_MakeRotationMatrix_PointsAngle
(
    MATH_tdst_Matrix        *_pst_MDst,
    MATH_tdst_Vector        *_pst_Point1,
    MATH_tdst_Vector        *_pst_Point2,
    float                   _f_Angle,
    MATH_tdst_Quaternion    *_pst_Q,
    char                    _b_InitRestOfMatrix
);

/*
 ===================================================================================================
    Aim:    Create a rotation matrix using a sighting vector (imposed) and a banking vector (shows
            up and is only proposed)
 ===================================================================================================
 */
void    MATH_MakeRotationMatrix_UsingSight
        (
            MATH_tdst_Matrix    *_pst_MDst,         /* Destination 4x4 matrix */
            MATH_tdst_Vector    *_pst_Sight,        /* SightVector (imposed, no need to be
                                                     * normalized) */
            MATH_tdst_Vector    *_pst_Banking,      /* Vector that gives the banking (whished, but
                                                     * not imposed) */
            char                _b_InitRestOfMatrix /* Set to 1 if you want the rest of the 4x4
                                                     * matrix to be cleared */
        );

/*
 ===================================================================================================
    Aim:    Create a rotation matrix using a sighting vector (imposed) and a banking vector (shows
            up and is only proposed)
 ===================================================================================================
 */
void    MATH_MakeRotationMatrix_UsingBanking
        (
            MATH_tdst_Matrix    *_pst_MDst,         /* Destination 4x4 matrix */
            MATH_tdst_Vector    *_pst_Sight,        /* SightVector (whished, but not imposed) */
            MATH_tdst_Vector    *_pst_Banking,      /* Banking vector(imposed, no need to be
                                                     * normalized) */
            char                _b_InitRestOfMatrix /* Set to 1 if you want the rest of the 4x4
                                                     * matrix to be cleared */
        );

/*
 ===================================================================================================
    Aim:    Make a rotation matrix without changing the current X,Y or Z direction
 ===================================================================================================
 */
void    MATH_MakeRotationMatrix_KeepingXAxis(MATH_tdst_Matrix *_pst_M, char _b_InitRestOfMatrix);
void    MATH_MakeRotationMatrix_KeepingYAxis(MATH_tdst_Matrix *_pst_M, char _b_InitRestOfMatrix);
void    MATH_MakeRotationMatrix_KeepingZAxis(MATH_tdst_Matrix *_pst_M, char _b_InitRestOfMatrix);

/*$F
 ===================================================================================================
    Aim:    Create a rotation matrix around X axis

    In:     _pst_MDst               Destination Matrix
            _f_Angle                Angle of rotation
            _b_InitRestOfMatrix     Set to 1 to clear the rest of the 4x4 matrix 

    Note:   | 1  0 0 | is the rotation matrix around X-axis 
			| 0  c s | 
			| 0 -s c |
 ===================================================================================================
 */
void MATH_MakeRotationMatrix_XAxis
(
    MATH_tdst_Matrix    *_pst_MDst,
    float               _f_Angle,
    char                _b_InitRestOfMatrix
);

/*$F
 ===================================================================================================
    Aim:    Create a rotation matrix around Y axis

    In:     _pst_MDst               Destination Matrix
            _f_Angle                Angle of rotation
            _b_InitRestOfMatrix     Set to 1 to clear the rest of the 4x4 matrix 

    Note:   | C 0 -s | is the rotation matrix around Y-axis
			| 0 1  0 |
			| s 0  c |
 ===================================================================================================
 */
void MATH_MakeRotationMatrix_YAxis
(
    MATH_tdst_Matrix    *_pst_MDst,
    float               _f_Angle,
    char                _b_InitRestOfMatrix
);

/*$F
 ===================================================================================================
    Aim:    Create a rotation matrix around Z axis

    In:     _pst_MDst               Destination Matrix
            _f_Angle                Angle of rotation
            _b_InitRestOfMatrix     Set to 1 to clear the rest of the 4x4 matrix 

    Note:   | C s 0 | is the rotation matrix around Z-axis
			|-s c 0 | 
			| 0 0 1 |
 ===================================================================================================
 */
void MATH_MakeRotationMatrix_ZAxis
(
    MATH_tdst_Matrix    *_pst_MDst,
    float               _f_Angle,
    char                _b_InitRestOfMatrix
);

/*$4
 ***************************************************************************************************
    DIRECT ROTATIONS AROUND GLOBAL AND LOCAL AXES (method called "direct post-concatenation") £
    MATH_RotateMatrix_AroundGlobalXAxis £
    MATH_RotateMatrix_AroundGlobalYAxis £
    MATH_RotateMatrix_AroundGlobalZAxis £
    MATH_RotateMatrix_AroundLocalXAxis £
    MATH_RotateMatrix_AroundLocalYAxis £
    MATH_RotateMatrix_AroundLocalZAxis £
    MATH_OrientMatrix_UsingSight £
    MATH_OrientMatrix_UsingBanking £
 ***************************************************************************************************
 */

/*$F
 ===================================================================================================
    Aim:    Rotates the transformation matrix around global X-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*T)*R to have a global rotation so we calculate directly T*R where
				| 1  0 0 | 
			R=	| 0  c s | is the rotation matrix around X-axis 
				| 0 -s c |
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundGlobalXAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle);

/*$F
 ===================================================================================================
    Aim:    Rotates the transformation matrix around global Y-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*T)*R to have a global rotation so we calculate directly T*R where
				| c 0 -s | 
			R=	| 0 1  0 | is the rotation matrix around X-axis
				| s 0  c |
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundGlobalYAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle);

/*$F
 ===================================================================================================
    Aim:    Rotates the transformation matrix around global Z-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*T)*R to have a global rotation so we calculate directly T*R where
				| c s 0 | 
			R=  |-s c 0 | is the rotation matrix around X-axis 
				| 0 0 1 |
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundGlobalZAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle);

/*$
 ===================================================================================================
    Aim:    Rotates the transformation matrix around local X-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*R)*T to have a local rotation so we calculate directly R*T where R
            is the rotation matrix around X-axis
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundLocalXAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle);

/*$
 ===================================================================================================
    Aim:    Rotates the transformation matrix around local Y-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*R)*T to have a local rotation so we calculate directly R*T where R
            is the rotation matrix around Y-axis
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundLocalYAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle);

/*
 ===================================================================================================
    Aim:    Rotates the transformation matrix around local Z-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*R)*T to have a local rotation so we calculate directly R*T where R
            is the rotation matrix around Z-axis £
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundLocalZAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle);

/*
 ===================================================================================================
    Aim:    Orient a matrix using a sight or a banking vector
 ===================================================================================================
 */
void    MATH_OrientMatrix_UsingSight
        (
            MATH_tdst_Matrix    *_pst_Matrix,
            MATH_tdst_Vector    *_pst_Sighting,
            MATH_tdst_Vector    *_pst_Banking
        );

void    MATH_OrientMatrix_UsingBanking
        (
            MATH_tdst_Matrix    *_pst_Matrix,
            MATH_tdst_Vector    *_pst_Sighting,
            MATH_tdst_Vector    *_pst_Banking
        );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
