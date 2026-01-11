/*$T WORimportfrommad.h GC!1.39 06/23/99 09:51:27 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Initialisations du module World
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

#pragma once
#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    create a world describe by content of mad file
 ===================================================================================================
 */
BOOL WOR_b_World_ExportMadFile(WOR_tdst_World *, char *, char *, unsigned char, unsigned char, BOOL);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /*ACTIVE_EDITORS*/