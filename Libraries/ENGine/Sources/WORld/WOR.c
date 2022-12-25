/*$T WOR.c GC!1.39 06/23/99 10:22:19 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Initialisation and close of the module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/WORld/WORinit.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/IMPort/IMPbase.h"
#include "ENGine/Sources/WORld/WORimportfrommad.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*
 ===================================================================================================
    Aim:    Initialisation of the World module
 ===================================================================================================
 */
void WOR_InitModule(void)
{
    /* Initialises the universe */
    WOR_Universe_Init();
#ifdef ACTIVE_EDITORS
    IMP_b_AddImportCallback("mad", WOR_b_World_Import);
#endif
}

/*
 ===================================================================================================
    Aim:    Close of the module
 ===================================================================================================
 */
void WOR_CloseModule(void)
{
    WOR_Universe_Destroy();
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
