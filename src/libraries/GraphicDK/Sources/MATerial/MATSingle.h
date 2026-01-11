/*$T MATSingle.h GC!1.39 06/29/99 15:57:34 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __MATSINGLE_H__
#define __MATSINGLE_H__



#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#endif
#endif
#endif
#include "TEXture/TEXstruct.h"
#include "MATerial/MATstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"

/*
 ===================================================================================================
    $4 functions
 ===================================================================================================
 */
MAT_tdst_Single     *MAT_pst_CreateSingleFromBuffer(GRO_tdst_Struct *, char **, WOR_tdst_World *);
MAT_tdst_Single     *MAT_pst_CreateSingle(char *);
void                *MAT_p_CreateSingleDefault( void );
LONG                MAT_l_SaveSingleInBuffer(MAT_tdst_Single *, TEX_tdst_List *);


#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
MAT_tdst_Single     *MAT_p_CreateSingleFromMad(MAD_StandarMaterial *);
MAD_StandarMaterial *MAT_p_SingleToMad(MAT_tdst_Single *, WOR_tdst_World *);
#endif
#endif
#endif

void                *MAT_p_SingleDuplicate(MAT_tdst_Single *, char *, char *, ULONG);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif



#endif /* __MATSINGLE_H__ */