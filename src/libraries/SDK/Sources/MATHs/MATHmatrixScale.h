/*$T MATHmatrixScale.h GC!1.52 10/11/99 10:10:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MATHMatrixscale.h: All scale matrix operations £
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHMATRIXSCALE_H_
#define _MATHMATRIXSCALE_H_

#if defined (__cplusplus) && !defined(JADEFUSION)
extern  "C"
{
#endif


#include "MATHs/MATHmatrixBase.h"

/*$4
 ***************************************************************************************************
    HANDLE THE SCALE PART OF THE 4x4 MATRIX (last colum is used for scale)£
    £
    MATH_GetScale £
    MATH_SetScale £
    MATH_SetZoomMatrix £
    MATH_MakeOGLMatrix £
 ***************************************************************************************************
 */

#define MATH_GetScaleVector MATH_GetScale

extern void MATH_GetScale(MATH_tdst_Vector *, MATH_tdst_Matrix *);
extern void MATH_SetScale(MATH_tdst_Matrix *, MATH_tdst_Vector *);
extern void MATH_SetZoom(MATH_tdst_Matrix *, float );
extern void MATH_MakeOGLMatrix(MATH_tdst_Matrix *, MATH_tdst_Matrix *);
#ifdef JADEFUSION
extern float MATH_GetMaxScale(MATH_tdst_Matrix *);
#endif
/*$4
 ***************************************************************************************************
    Handle Scale/Zoom of the 3x3 part of a 4x4 matrix £
    £
    Functions: £
    MATH_Zoom33Matrix £
    MATH_Scale33Matrix £
    MATH_GetScaleFrom33Matrix £
    MATH_GetRotationFromScaled33Matrix £

    Note:   These functions should normally only be used by the display, since in the rest of the
            engine, the scale is stored in the last column of the 4x4 matrix
 ***************************************************************************************************
 */

#define MATH_Zoom33Matrix   MATH_Mul33Matrix

extern void MATH_Mul33Matrix(MATH_tdst_Matrix *, MATH_tdst_Matrix *, float );
extern void MATH_Scale33Matrix( MATH_tdst_Matrix    *, MATH_tdst_Matrix    *, MATH_tdst_Vector    *);
extern void MATH_GetScaleFrom33Matrix(MATH_tdst_Vector *, MATH_tdst_Matrix *);
extern void MATH_GetRotationFromScaled33Matrix(MATH_tdst_Matrix *, MATH_tdst_Matrix *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif
