/*$T MATHvars.c GC!1.5 10/06/99 17:39:30 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Déclaration des variables globales du moddule Maths
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "MATHstruct.h"
#include "BASe/BAStypes.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

LONG                MATH_gl_TableRacine[2048];

LONG                MATH_gl_TableInverse[1024];
LONG                MATH_gl_TableInverseRacine[2048];

 
float               MATH_gf_TableSin[1024];
float               MATH_gf_1024by2Pi;
#ifndef PSX2_TARGET
double              MATH_gd_Decal;
double              MATH_gd_Tmp;
#endif

MATH_tdst_Matrix    MATH_gst_IdentityMatrix ONLY_PSX2_ALIGNED(16);
MATH_tdst_Matrix	MATH_gst_TmpIdentityMatrix ONLY_PSX2_ALIGNED(16);
MATH_tdst_Vector    MATH_gst_IdentityVector ONLY_PSX2_ALIGNED(16);
MATH_tdst_Vector    MATH_gst_BaseVectorI ONLY_PSX2_ALIGNED(16);
MATH_tdst_Vector    MATH_gst_BaseVectorJ ONLY_PSX2_ALIGNED(16);
MATH_tdst_Vector    MATH_gst_BaseVectorK ONLY_PSX2_ALIGNED(16);
MATH_tdst_Vector    MATH_gst_NulVector ONLY_PSX2_ALIGNED(16);


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
