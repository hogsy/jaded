/*$T DYN.c GC!1.52 10/15/99 16:07:56 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Initialisation and close of the module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNvars.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

 /*
 ===================================================================================================
    Aim:    Initialisation of the World module
 ===================================================================================================
 */
void DYN_InitModule(void)
{
    DYN_gf_AutoOrientLowInertiaValue = -10.0f;
    DYN_gf_AutoOrientMediumInertiaValue = -3.0f;
    DYN_gf_AutoOrientHiInertiaValue = -1.0f;
}

/*
 ===================================================================================================
    Aim:    Close of the module
 ===================================================================================================
 */
void DYN_CloseModule(void)
{
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
