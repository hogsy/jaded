/*$T WAY.c GC!1.52 10/25/99 10:04:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BASarray.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/* Waypoints for recurse test */
BAS_tdst_barray WAY_gst_Seen;
BAS_tdst_barray WAY_gst_SeenDist;

/*
 ===================================================================================================
 ===================================================================================================
 */
void WAY_InitModule(void)
{
    BAS_binit(&WAY_gst_Seen, 1000);
    BAS_binit(&WAY_gst_SeenDist, 1000);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void WAY_CloseModule(void)
{
    BAS_bfree(&WAY_gst_Seen);
    BAS_bfree(&WAY_gst_SeenDist);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
