/*$T MATmulti.h GC!1.39 06/29/99 10:22:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __MATMULTI_H__
#define __MATMULTI_H__




#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifndef PSX2_TARGET
#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#endif
#endif
#include "MATerial/MATstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

void                MAT_AllocMultiMaterial(MAT_tdst_Material *_pst_Material, LONG _l_NbSubMat);
void                MAT_FreeMultiMaterial(MAT_tdst_Material *_pst_Material);
MAT_tdst_Multi      *MAT_pst_CreateMultiFromBuffer(GRO_tdst_Struct *, char **, WOR_tdst_World *);
MAT_tdst_Multi      *MAT_pst_CreateMulti(char *);
void                *MAT_p_CreateMultiDefault( void );
LONG                MAT_l_SaveMultiInBuffer(MAT_tdst_Multi *, void *);

#ifndef PSX2_TARGET
#ifdef ACTIVE_EDITORS
MAT_tdst_Multi      *MAT_p_CreateMultiFromMad(MAD_MultiMaterial *);
MAD_MultiMaterial   *MAT_p_MultiToMad(MAT_tdst_Multi *, WOR_tdst_World *);
#endif
#endif

void                *MAT_p_MultiDuplicate(MAT_tdst_Multi *, char *, char *,ULONG);

void                MAT_MultiAddRef(MAT_tdst_Multi *, LONG );

void                *MAT_p_MultiDuplicate(MAT_tdst_Multi  *, char *, char *, ULONG);


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __MATMULTI_H__ */