/*$T PRO.c GC!1.26 04/21/99 14:08:44 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "PROdefs.h"
#include "PROPS2.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*
 ===================================================================================================
 ===================================================================================================
 */
void PRO_InitModule(void)
{
#ifdef RASTERS_ON
    _PRO_gpst_FirstTrameRaster = NULL;
#endif
     PROPS2_Init(1);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
