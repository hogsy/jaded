/*$T MATHvars.h GC!1.41 09/28/99 10:15:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Inclusion des variables globales du module maths
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern LONG             MATH_gl_TableRacine[2048];
extern LONG             MATH_gl_TableInverse[1024];
extern LONG             MATH_gl_TableInverseRacine[2048];
extern float            MATH_gf_TableSin[1024];
extern float            MATH_gf_1024by2Pi;
#ifndef PSX2_TARGET
extern double           MATH_gd_Decal;
extern double           MATH_gd_Tmp;
#endif

extern MATH_tdst_Matrix MATH_gst_IdentityMatrix;
extern MATH_tdst_Matrix	MATH_gst_TmpIdentityMatrix;
extern MATH_tdst_Vector MATH_gst_IdentityVector;
extern MATH_tdst_Vector MATH_gst_BaseVectorI;
extern MATH_tdst_Vector MATH_gst_BaseVectorJ;
extern MATH_tdst_Vector MATH_gst_BaseVectorK;
extern MATH_tdst_Vector MATH_gst_NulVector;
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
