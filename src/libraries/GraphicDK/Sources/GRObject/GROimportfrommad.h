/*$T GROimportfrommad.h GC!1.39 06/24/99 10:39:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS
#pragma once
#include "BASe/BAStypes.h"


#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GRObject/GROstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


/*$4
 ***************************************************************************************************
    Constants
 ***************************************************************************************************
 */

#define GRO_C_Import_MadObject     0
#define GRO_C_Import_MadMaterial   1

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

GRO_tdst_Struct *GRO_pst_ImportFromMad(MAD_NodeID *, WOR_tdst_World *, LONG);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* ACTIVE_EDITORS */
