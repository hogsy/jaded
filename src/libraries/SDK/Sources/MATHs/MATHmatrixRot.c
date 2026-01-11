/*$T MATHmatrixRot.c GC! 1.081 04/20/01 10:01:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

/*
 * Aim: Matrix operation functions Note: Most functions are declared inline in the
 * Matrix.h file. Only the function sthat need no particular speed are here
 */
#include "MATHs/MATHmatrixBase.h"
#include "MATHs/MATHdebug.h"

#ifdef _DEBUG
#include <float.h>
#include "BASe/ERRors/ERRasser.h"
#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 * Define this to correct some precision errors, when using optimised math
 * functions
 */

/* #define _MATH_LOWPRECISION */
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
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Quaternion    q;  /* Only used if no quaternion pointer is given... */
    float                   f_SinHalfAngle;
    float                   xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f_SinHalfAngle = fSin(fHalf(_f_Angle));

    if(_pst_Q == 0) _pst_Q = &q;

    _pst_Q->x = fMul(_pst_Axis->x, f_SinHalfAngle);
    _pst_Q->y = fMul(_pst_Axis->y, f_SinHalfAngle);
    _pst_Q->z = fMul(_pst_Axis->z, f_SinHalfAngle);
    _pst_Q->w = fCos(fHalf(_f_Angle));

    xs = fTwice(_pst_Q->x);
    ys = fTwice(_pst_Q->y);
    zs = fTwice(_pst_Q->z);

    wx = fMul(_pst_Q->w, xs);
    wy = fMul(_pst_Q->w, ys);
    wz = fMul(_pst_Q->w, zs);

    xx = fMul(_pst_Q->x, xs);
    xy = fMul(_pst_Q->x, ys);
    xz = fMul(_pst_Q->x, zs);

    yy = fMul(_pst_Q->y, ys);
    yz = fMul(_pst_Q->y, zs);

    zz = fMul(_pst_Q->z, zs);

    /* Fill destination matrix */
    _pst_MDst->Ix = fSub(Cf_One, fAdd(yy, zz));
    _pst_MDst->Iy = fAdd(xy, wz);
    _pst_MDst->Iz = fSub(xz, wy);

    _pst_MDst->Jx = fSub(xy, wz);
    _pst_MDst->Jy = fSub(Cf_One, fAdd(xx, zz));
    _pst_MDst->Jz = fAdd(yz, wx);

    _pst_MDst->Kx = fAdd(xz, wy);
    _pst_MDst->Ky = fSub(yz, wx);
    _pst_MDst->Kz = fSub(Cf_One, fAdd(xx, yy));

    if(_b_InitRestOfMatrix)
    {
        MATH_ClearScale(_pst_MDst, 1);
        MATH_ClearTranslation(_pst_MDst);
    }

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_MDst);
    
	MATH_b_CheckMatrixValidity(_pst_MDst);        
}

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
)
{
    MATH_MakeRotationMatrix_AxisAngle(_pst_MDst, _pst_Axis, _f_Angle, _pst_Q, 0);
    _pst_MDst->T.x = _pst_Point->x -
        fMul(_pst_MDst->Ix, _pst_Point->x) -
        fMul(_pst_MDst->Jx, _pst_Point->y) -
        fMul(_pst_MDst->Kx, _pst_Point->z);
    _pst_MDst->T.y = _pst_Point->y -
        fMul(_pst_MDst->Iy, _pst_Point->x) -
        fMul(_pst_MDst->Jy, _pst_Point->y) -
        fMul(_pst_MDst->Ky, _pst_Point->z);
    _pst_MDst->T.z = _pst_Point->z -
        fMul(_pst_MDst->Iz, _pst_Point->x) -
        fMul(_pst_MDst->Jz, _pst_Point->y) -
        fMul(_pst_MDst->Kz, _pst_Point->z);
    if(_b_InitRestOfMatrix)
    {
        MATH_ClearScale(_pst_MDst, 1);
    }

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_MDst);

    /* Sets the Translation Type */
    MATH_SetTranslationType(_pst_MDst);
    
	MATH_b_CheckMatrixValidity(_pst_MDst);        
}

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
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_Diff, st_Axis;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SubVector(&st_Diff, _pst_Point2, _pst_Point1);
    if(!MATH_b_NulVector(&st_Diff))
    {
        MATH_NormalizeVector(&st_Axis, &st_Diff);
        MATH_MakeRotationMatrix_PointAxisAngle
        (
            _pst_MDst,
            _pst_Point1,
            &st_Axis,
            _f_Angle,
            _pst_Q,
            _b_InitRestOfMatrix
        );
    }
    else
        /* Blindage... */
        MATH_SetIdentityMatrix(_pst_MDst);
        
	MATH_b_CheckMatrixValidity(_pst_MDst);            
}


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
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    s = fSin(_f_Angle);
    c = fCos(_f_Angle);

    fSetLongToFloat(_pst_MDst->Ix, Cl_1f);
    fSetLongToFloat(_pst_MDst->Iy, Cl_0f);
    fSetLongToFloat(_pst_MDst->Iz, Cl_0f);

    fSetLongToFloat(_pst_MDst->Jx, Cl_0f);
    vCopy(&_pst_MDst->Jy, &c);
    vCopy(&_pst_MDst->Jz, &s);

    vNeg(&s);
    fSetLongToFloat(_pst_MDst->Kx, Cl_0f);
    vCopy(&_pst_MDst->Ky, &s);
    vCopy(&_pst_MDst->Kz, &c);

    if(_b_InitRestOfMatrix)
    {
        MATH_ClearTranslation(_pst_MDst);
        MATH_ClearScale(_pst_MDst, 1);
    }

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_MDst);
    
	MATH_b_CheckMatrixValidity(_pst_MDst);                
}

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
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    s = fSin(_f_Angle);
    c = fCos(_f_Angle);

    vCopy(&_pst_MDst->Kx, &s);
    vNeg(&s);

    vCopy(&_pst_MDst->Ix, &c);
    fSetLongToFloat(_pst_MDst->Iy, Cl_0f);
    vCopy(&_pst_MDst->Iz, &s);

    fSetLongToFloat(_pst_MDst->Jx, Cl_0f);
    fSetLongToFloat(_pst_MDst->Jy, Cl_1f);
    fSetLongToFloat(_pst_MDst->Jz, Cl_0f);

    fSetLongToFloat(_pst_MDst->Ky, Cl_0f);
    vCopy(&_pst_MDst->Kz, &c);

    if(_b_InitRestOfMatrix)
    {
        MATH_ClearTranslation(_pst_MDst);
        MATH_ClearScale(_pst_MDst, 1);
    }

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_MDst);
    
    MATH_b_CheckMatrixValidity(_pst_MDst);            
}

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
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    s = fSin(_f_Angle);
    c = fCos(_f_Angle);

    vCopy(&_pst_MDst->Iy, &s);
    vNeg(&s);
    vCopy(&_pst_MDst->Ix, &c);
    fSetLongToFloat(_pst_MDst->Iz, Cl_0f);

    vCopy(&_pst_MDst->Jx, &s);
    vCopy(&_pst_MDst->Jy, &c);
    fSetLongToFloat(_pst_MDst->Jz, Cl_0f);

    fSetLongToFloat(_pst_MDst->Kx, Cl_0f);
    fSetLongToFloat(_pst_MDst->Ky, Cl_0f);
    fSetLongToFloat(_pst_MDst->Kz, Cl_1f);

    if(_b_InitRestOfMatrix)
    {
        MATH_ClearTranslation(_pst_MDst);
        MATH_ClearScale(_pst_MDst, 1);
    }

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_MDst);
    
    MATH_b_CheckMatrixValidity(_pst_MDst);            
}

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
void MATH_RotateMatrix_AroundGlobalXAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s, t, f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    c = fCos(_f_Angle);
    s = fSin(_f_Angle);

    t = _pst_Dst->Iy;
    f = _pst_Dst->Iz;
    _pst_Dst->Iy = fMulSubMul(t, c, f, s);
    _pst_Dst->Iz = fMulAddMul(t, s, f, c);
    
    t = _pst_Dst->Jy;
    f = _pst_Dst->Jz;
    _pst_Dst->Jy = fMulSubMul(t, c, f, s);
    _pst_Dst->Jz = fMulAddMul(t, s, f, c);
    
    t = _pst_Dst->Ky;
    f = _pst_Dst->Kz;
    _pst_Dst->Ky = fMulSubMul(t, c, f, s);
    _pst_Dst->Kz = fMulAddMul(t, s, f, c);

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_Dst);

	MATH_b_CheckMatrixValidity(_pst_Dst);            
}

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
void MATH_RotateMatrix_AroundGlobalYAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s, t,f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    c = fCos(_f_Angle);
    s = fSin(_f_Angle);

    t = _pst_Dst->Ix;
    f=_pst_Dst->Iz;
    _pst_Dst->Ix = fMulAddMul(t, c, f, s);
    _pst_Dst->Iz = fMulSubMul(f, c, t, s);
    
    t = _pst_Dst->Jx;
    f=_pst_Dst->Jz;
    _pst_Dst->Jx = fMulAddMul(t, c, f, s);
    _pst_Dst->Jz = fMulSubMul(f, c, t, s);
    
    t = _pst_Dst->Kx;
    f=_pst_Dst->Kz;
    _pst_Dst->Kx = fMulAddMul(t, c, f, s);
    _pst_Dst->Kz = fMulSubMul(f, c, t, s);

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_Dst);

	MATH_b_CheckMatrixValidity(_pst_Dst);            
}

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
void MATH_RotateMatrix_AroundGlobalZAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s, t,f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*$F
		|Ix Iy Iz|		| c s 0 |	| cIx-sIy sIx+cIy Iz | 
		|Jx Jy Jz| *	|-s c 0 | = | cJx-sJy sJx+cJy Jz | 
		|Kx Ky Kz|		| 0 0 1 |	| cKx-sKy sKx+cKy Kz |
     */
    c = fCos(_f_Angle);
    s = fSin(_f_Angle);

    t = _pst_Dst->Ix;
    f=_pst_Dst->Iy;
    _pst_Dst->Ix = fMulSubMul(t, c, f, s);
    _pst_Dst->Iy = fMulAddMul(t, s, f, c);
    
    t = _pst_Dst->Jx;
    f=_pst_Dst->Jy;
    _pst_Dst->Jx = fMulSubMul(t, c, f, s);
    _pst_Dst->Jy = fMulAddMul(t, s, f, c);
    
    t = _pst_Dst->Kx;
    f=_pst_Dst->Ky;
    _pst_Dst->Kx = fMulSubMul(t, c, f, s);
    _pst_Dst->Ky = fMulAddMul(t, s, f, c);

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_Dst);

	MATH_b_CheckMatrixValidity(_pst_Dst);            
}

/*$
 ===================================================================================================
    Aim:    Rotates the transformation matrix around local X-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*R)*T to have a local rotation so we calculate directly R*T where R
            is the rotation matrix around X-axis
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundLocalXAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s, t,f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*$F
	   |1 0  0|   | Ix Iy Iz |   | Ix      Iy       Iz      | 
	   |0 c  s| * | Jx Jy Jz | = | cJx+sKx cJy+sKy  cJz+sKz |
       |0 -s c|   | Kx Ky Kz |   | cKx-sJx c Ky-sJy cKz-sJz |
     */
    c = fCos(_f_Angle);
    s = fSin(_f_Angle);

    t = _pst_Dst->Jx;
    f=_pst_Dst->Kx;
    _pst_Dst->Jx = fMulAddMul(t, c, f, s);
    _pst_Dst->Kx = fMulSubMul(f, c, t, s);

    t = _pst_Dst->Jy;
    f=_pst_Dst->Ky;
    _pst_Dst->Jy = fMulAddMul(t, c, f, s);
    _pst_Dst->Ky = fMulSubMul(f, c, t, s);

    t = _pst_Dst->Jz;
    f=_pst_Dst->Kz;
    _pst_Dst->Jz = fMulAddMul(t, c, f, s);
    _pst_Dst->Kz = fMulSubMul(f, c, t, s);

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_Dst);
    
	MATH_b_CheckMatrixValidity(_pst_Dst);            
}

/*$
 ===================================================================================================
    Aim:    Rotates the transformation matrix around local Y-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*R)*T to have a local rotation so we calculate directly R*T where R
            is the rotation matrix around Y-axis
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundLocalYAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s, t,f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*$F
     |c 0 -s|	|Ix Iy Iz|		|cIx-sKx	cIy-sKy cIz-sKz	|
	 |0 1  0| * |Jx Jy Jz| =	|Jx			Jy		Jz		|
	 |s 0  c|	|Kx Ky Kz|		|sIx+cKx	sIy+cKy sIz+cKz	|
     */
    c = fCos(_f_Angle);
    s = fSin(_f_Angle);

    t = _pst_Dst->Ix;
    f = _pst_Dst->Kx;
    _pst_Dst->Ix = fMulSubMul(t, c, f, s);
    _pst_Dst->Kx = fMulAddMul(f, c, t, s);

    t = _pst_Dst->Iy;
    f = _pst_Dst->Ky;
    _pst_Dst->Iy = fMulSubMul(t, c, f, s);
    _pst_Dst->Ky = fMulAddMul(f, c, t, s);

    t = _pst_Dst->Iz;
    f = _pst_Dst->Kz;
    _pst_Dst->Iz = fMulSubMul(t, c, f, s);
    _pst_Dst->Kz = fMulAddMul(f, c, t, s);

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_Dst);

	MATH_b_CheckMatrixValidity(_pst_Dst);            
}

/*
 ===================================================================================================
    Aim:    Rotates the transformation matrix around local Z-Axis

    Note:   If V is the vector to rotate, T the current transformation matrix and R the rotation
            matrix, we want (V*R)*T to have a local rotation so we calculate directly R*T where R
            is the rotation matrix around Z-axis £
 ===================================================================================================
 */
void MATH_RotateMatrix_AroundLocalZAxis(MATH_tdst_Matrix *_pst_Dst, float _f_Angle)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   c, s, t,f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*$F
      | c s 0|		|Ix Iy Iz|		|cIx+sJx c	Iy+sJy	cIz-sJz	| 
	  |-s c 0| *	|Jx Jy Jz| =	|cJx-sIx	cJy-sIy cJz-sIz	| 
	  | 0 0 1|		|Kx Ky Kz|		|Kx			Ky		Kz		|
     */
    c = fCos(_f_Angle);
    s = fSin(_f_Angle);

    t = _pst_Dst->Ix;
    f = _pst_Dst->Jx;
    _pst_Dst->Ix = fMulAddMul(t, c, f, s);
    _pst_Dst->Jx = fMulSubMul(f, c, t, s);

    t = _pst_Dst->Iy;
    f = _pst_Dst->Jy;
    _pst_Dst->Iy = fMulAddMul(t, c, f, s);
    _pst_Dst->Jy = fMulSubMul(f, c, t, s);

    t = _pst_Dst->Iz;
    f = _pst_Dst->Jz;
    _pst_Dst->Iz = fMulAddMul(t, c, f, s);
    _pst_Dst->Jz = fMulSubMul(f, c, t, s);

    /* Sets the Rotation Type. */
    MATH_SetRotationType(_pst_Dst);
    
	MATH_b_CheckMatrixValidity(_pst_Dst);            
}

/*
 =======================================================================================================================
    Aim:    Create a rotation matrix using a sighting vector (imposed) and a banking vector (shows up and is only
            proposed)
 =======================================================================================================================
 */
void MATH_MakeRotationMatrix_UsingSight
(
	MATH_tdst_Matrix	*_pst_MDst,			/* Destination 4x4 matrix */
	MATH_tdst_Vector	*_pst_Sight,		/* SightVector (no need to be normalized) */
	MATH_tdst_Vector	*_pst_Banking,		/* Vector that gives the banking (whished, but not imposed) */
	char				_b_InitRestOfMatrix /* Set to 1 if you want the scale and translation of the 4x4 matrix to be
											 * cleared */
)
{

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the Sight
	 -------------------------------------------------------------------------------------------------------------------
	 */

	MATH_NormalizeVector(MATH_pst_GetYAxis(_pst_MDst), _pst_Sight);
	MATH_NegEqualVector(MATH_pst_GetYAxis(_pst_MDst));

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the banking
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_pst_Banking)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Banking;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_NormalizeVector(&st_Banking, _pst_Banking);

		MATH_CrossProduct(MATH_pst_GetXAxis(_pst_MDst), _pst_Sight, &st_Banking);

		/*
		 * If Sighting and Banking are colinear, it won't work ! in this case we change a
		 * little the wished banking and calculate X again...
		 */
		if(MATH_b_NulVectorWithEpsilon(MATH_pst_GetXAxis(_pst_MDst), Cf_EpsilonLow))
		{
			st_Banking.x += Cf_EpsilonBig;
			MATH_CrossProduct(MATH_pst_GetXAxis(_pst_MDst), _pst_Sight, &st_Banking);
		}

		/* Store the proposed Z vector */
		MATH_CopyVector(MATH_pst_GetZAxis(_pst_MDst), &st_Banking);

		/* Negate X, (because Y = - Sight) */
		MATH_NegEqualVector(MATH_pst_GetXAxis(_pst_MDst));
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the case that Keep the current banking
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!_pst_Banking)
	{
		MATH_CrossProduct(MATH_pst_GetXAxis(_pst_MDst), MATH_pst_GetYAxis(_pst_MDst), MATH_pst_GetZAxis(_pst_MDst));
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Normalize X and recalculate the final Banking
	 -------------------------------------------------------------------------------------------------------------------
	 */

	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(_pst_MDst));
	MATH_CrossProduct(MATH_pst_GetZAxis(_pst_MDst), MATH_pst_GetXAxis(_pst_MDst), MATH_pst_GetYAxis(_pst_MDst));
#ifdef _MATH_LOWPRECISION
	MATH_NormalizeEqualVector(MATH_pst_GetZAxis(_pst_MDst));
#endif
	if(_b_InitRestOfMatrix)
	{
		MATH_ClearScale(_pst_MDst, 1);
		MATH_ClearTranslation(_pst_MDst);
	}

	/* Sets the Rotation Type. */
	MATH_SetRotationType(_pst_MDst);
	
	MATH_b_CheckMatrixValidity(_pst_MDst);            
}

/*
 =======================================================================================================================
    Aim:    Create a rotation matrix using a banking vector (imposed, shows up) and a sighting vector (shows in front
            and is only proposed)
 =======================================================================================================================
 */
void MATH_MakeRotationMatrix_UsingBanking
(
	MATH_tdst_Matrix	*_pst_MDst,			/* Destination 4x4 matrix */
	MATH_tdst_Vector	*_pst_Sight,		/* Sight vector (proposed) */
	MATH_tdst_Vector	*_pst_Banking,		/* Banking vector(imposed) */
	char				_b_InitRestOfMatrix /* Set to 1 if you want the rest of the 4x4 matrix to be cleared */
)
{
	/* Store the wanted Z vector (will be recalculated after) */
	MATH_NormalizeVector(MATH_pst_GetZAxis(_pst_MDst), _pst_Banking);

	/*
	 * Set the Y vector (-Sight), except if a null sight pointer is givent, which
	 * means we don't change the Y vector (not even normalized)
	 */
	if(_pst_Sight)
	{
		MATH_NegVector(MATH_pst_GetYAxis(_pst_MDst), _pst_Sight);
		MATH_NormalizeEqualVector(MATH_pst_GetYAxis(_pst_MDst));
	}

	/* Calculate the X vector */
	MATH_CrossProduct(MATH_pst_GetXAxis(_pst_MDst), MATH_pst_GetYAxis(_pst_MDst), MATH_pst_GetZAxis(_pst_MDst));

	/* if X is a nul vector, Y and Z are colinear, so we change Y and recalculate X */
	if(MATH_b_NulVectorWithEpsilon(MATH_pst_GetXAxis(_pst_MDst), Cf_Epsilon))
	{
		MATH_AddEqualVector(MATH_pst_GetYAxis(_pst_MDst), &MATH_gst_BaseVectorI);

		/* Calculate the X vector */
		MATH_CrossProduct(MATH_pst_GetXAxis(_pst_MDst), MATH_pst_GetYAxis(_pst_MDst), MATH_pst_GetZAxis(_pst_MDst));
	}

	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(_pst_MDst));

	/* Finally recalculate the Y vector (Z imposed doesn't change) */
	MATH_CrossProduct(MATH_pst_GetYAxis(_pst_MDst), MATH_pst_GetZAxis(_pst_MDst), MATH_pst_GetXAxis(_pst_MDst));
#ifdef _MATH_LOWPRECISION
	MATH_NormalizeEqualVector(MATH_pst_GetYAxis(_pst_MDst));
#endif
	if(_b_InitRestOfMatrix)
	{
		MATH_ClearScale(_pst_MDst, 1);
		MATH_ClearTranslation(_pst_MDst);
	}

	/* Sets the Rotation Type. */
	MATH_SetRotationType(_pst_MDst);

	MATH_b_CheckMatrixValidity(_pst_MDst);            
}

/*
 =======================================================================================================================
    Aim:    Make a rotation matrix without changing the current X Axis
 =======================================================================================================================
 */
void MATH_MakeRotationMatrix_KeepingXAxis(MATH_tdst_Matrix *_pst_M, char _b_InitRestOfMatrix)
{
	if(MATH_b_NulVector(MATH_pst_GetXAxis(_pst_M)))
	{
		MATH_CopyVector(MATH_pst_GetXAxis(_pst_M), &MATH_gst_BaseVectorI);
	}

	MATH_CrossProduct(MATH_pst_GetZAxis(_pst_M), MATH_pst_GetXAxis(_pst_M), MATH_pst_GetYAxis(_pst_M));

	MATH_CrossProduct(MATH_pst_GetYAxis(_pst_M), MATH_pst_GetZAxis(_pst_M), MATH_pst_GetXAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetYAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetZAxis(_pst_M));

	if(_b_InitRestOfMatrix)
	{
		MATH_ClearScale(_pst_M, 1);
		MATH_ClearTranslation(_pst_M);
	}

	/* Sets the Rotation Type. */
	MATH_SetRotationType(_pst_M);
	
	MATH_b_CheckMatrixValidity(_pst_M);            
}

/*
 =======================================================================================================================
    Aim:    Make a rotation matrix without changing the current X Axis
 =======================================================================================================================
 */
void MATH_MakeRotationMatrix_KeepingYAxis(MATH_tdst_Matrix *_pst_M, char _b_InitRestOfMatrix)
{
	if(MATH_b_NulVector(MATH_pst_GetYAxis(_pst_M)))
	{
		MATH_CopyVector(MATH_pst_GetYAxis(_pst_M), &MATH_gst_BaseVectorJ);
	}

	MATH_CrossProduct(MATH_pst_GetXAxis(_pst_M), MATH_pst_GetYAxis(_pst_M), MATH_pst_GetZAxis(_pst_M));

	MATH_CrossProduct(MATH_pst_GetZAxis(_pst_M), MATH_pst_GetXAxis(_pst_M), MATH_pst_GetYAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetYAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetZAxis(_pst_M));
	if(_b_InitRestOfMatrix)
	{
		MATH_ClearScale(_pst_M, 1);
		MATH_ClearTranslation(_pst_M);
	}

	/* Sets the Rotation Type. */
	MATH_SetRotationType(_pst_M);
	
	MATH_b_CheckMatrixValidity(_pst_M);            
}

/*
 =======================================================================================================================
    Aim:    Make a rotation matrix without changing the current X Axis
 =======================================================================================================================
 */
void MATH_MakeRotationMatrix_KeepingZAxis(MATH_tdst_Matrix *_pst_M, char _b_InitRestOfMatrix)
{
	if(MATH_b_NulVector(MATH_pst_GetZAxis(_pst_M)))
	{
		MATH_CopyVector(MATH_pst_GetZAxis(_pst_M), &MATH_gst_BaseVectorK);
	}

	MATH_CrossProduct(MATH_pst_GetYAxis(_pst_M), MATH_pst_GetZAxis(_pst_M), MATH_pst_GetXAxis(_pst_M));

	MATH_CrossProduct(MATH_pst_GetXAxis(_pst_M), MATH_pst_GetYAxis(_pst_M), MATH_pst_GetZAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetYAxis(_pst_M));
	MATH_NormalizeEqualVector(MATH_pst_GetZAxis(_pst_M));
	if(_b_InitRestOfMatrix)
	{
		MATH_ClearScale(_pst_M, 1);
		MATH_ClearTranslation(_pst_M);
	}

	/* Sets the Rotation Type. */
	MATH_SetRotationType(_pst_M);
	
	MATH_b_CheckMatrixValidity(_pst_M);            
}

/*
 =======================================================================================================================
    Aim:    Change the orientation of a matrix using a given sighting and banking

    Note:   With this function, the sighting is imposed and the banking only proposed
 =======================================================================================================================
 */
void MATH_OrientMatrix_UsingSight
(
	MATH_tdst_Matrix	*_pst_Matrix,
	MATH_tdst_Vector	*_pst_Sighting,
	MATH_tdst_Vector	*_pst_Banking
)
{
	/* We just make a rotation matrix without clearing the scale and translation */
	MATH_MakeRotationMatrix_UsingSight(_pst_Matrix, _pst_Sighting, _pst_Banking, 0);
	
	MATH_b_CheckMatrixValidity(_pst_Matrix);            
}

/*
 =======================================================================================================================
    Aim:    Change the orientation of a matrix using a given sighting and banking

    Note:   With this function, the sighting is imposed and the banking only proposed
 =======================================================================================================================
 */
void MATH_OrientMatrix_UsingBanking
(
	MATH_tdst_Matrix	*_pst_Matrix,
	MATH_tdst_Vector	*_pst_Sighting,
	MATH_tdst_Vector	*_pst_Banking
)
{
	/* We just make a rotation matrix without clearing the scale and translation */
	MATH_MakeRotationMatrix_UsingBanking(_pst_Matrix, _pst_Sighting, _pst_Banking, 0);

	MATH_b_CheckMatrixValidity(_pst_Matrix);            
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
