/*$T EOTinit.h GC!1.37 06/08/99 09:37:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Initialisations
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#include "ENGine/Sources/EOT/EOTstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
void EOT_SetOfEOT_Init(EOT_tdst_SetOfEOT *pst_SetOfEOT);
void EOT_SetOfEOT_Close(EOT_tdst_SetOfEOT *pst_SetOfEOT);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
