/*$T WORvars.c GC!1.41 08/02/99 13:16:20 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/WORld/WORvars.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif


/*$4
 ***************************************************************************************************
    Constants
 ***************************************************************************************************
 */

/* The universe of the game */
WOR_tdst_Universe   WOR_gst_Universe;

/* The world curently handled by the engine */
WOR_tdst_World      *WOR_gpst_CurrentWorld;


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
