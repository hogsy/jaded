/*$T COL.c GC!1.41 08/11/99 15:45:31 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "OBJects/OBJstruct.h"
#include "COLlision/COLvars.h"
#include "MATHs/MATH.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

extern COL_tdst_GlobalVars  COL_gst_GlobalVars;

/*
 ===================================================================================================
 ===================================================================================================
 */
void COL_InitModule(void)
{
	COL_gst_GlobalVars.f_WallCosAngle=Cf_Cos45;
}

/*
 ===================================================================================================
    Aim:    Close of the Intersection module
 ===================================================================================================
 */
void COL_CloseModule(void)
{
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
