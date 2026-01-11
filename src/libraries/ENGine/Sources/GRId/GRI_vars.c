/*$T GRI_vars.c GC!1.68 01/06/00 12:04:16 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_vars.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

GRID_tdst_Best      GRID_gast_Best[GRID_MAX_BEST];
GRID_tdst_Explored  GRID_gast_Explored[GRID_MAX_EXPLORED];

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
