/*$T MATmultitexture.h GC!1.55 12/30/99 18:08:28 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __MATMULTITEXTURE_H__
#define __MATMULTITEXTURE_H__



#include "BASe/BAStypes.h"


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


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

MAT_tdst_MultiTexture   *MAT_pst_CreateMultiTextureFromBuffer
                        (
                            struct GRO_tdst_Struct_ *,
                            char **,
                            struct WOR_tdst_World_ *
                        );
MAT_tdst_MultiTexture   *MAT_pst_CreateMultiTexture(char *);
void                    MAT_FreeMultiTexture(MAT_tdst_Material *_pst_Material);
void                    *MAT_p_MultiTextureDuplicate(MAT_tdst_MultiTexture *, char *, char *, ULONG);

LONG                    MAT_l_SaveMultiTextureInBuffer(MAT_tdst_MultiTexture *, TEX_tdst_List*);

#ifdef JADEFUSION
MAT_tdst_MTLevel*       MAT_pst_CreateMTLevel(MAT_tdst_MultiTexture* _pst_MultiTex);
void                    MAT_DestroyMTLevel(MAT_tdst_MTLevel* _pst_Level);
#endif

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
MAT_tdst_MultiTexture   *MAT_p_CreateMultiTextureFromMad(struct MAD_StandarMaterial_ *);
MAD_MultiTexMaterial    *MAT_p_MultiTextureToMad(MAT_tdst_MultiTexture *, WOR_tdst_World *);
#endif
#endif
#endif

#if defined(_XENON_RENDER)
// Xenon extended material properties
MAT_tdst_XeMTLevel* MAT_pst_CreateXeMTLevel(void);
MAT_tdst_XeMTLevel* MAT_pst_DuplicateXeMTLevel(MAT_tdst_XeMTLevel*);
void                MAT_FreeXeMTLevel(MAT_tdst_XeMTLevel*);
void                MAT_LoadXeMTLevelFromBuffer(MAT_tdst_XeMTLevel*, MAT_tdst_MultiTexture*, char**);
LONG                MAT_l_SaveXeMTLevelInBuffer(MAT_tdst_XeMTLevel*, TEX_tdst_List*);

#if defined(ACTIVE_EDITORS)
void MAT_ValidateXeMTLevel(MAT_tdst_XeMTLevel*);
#endif // defined(ACTIVE_EDITORS)

#endif // defined(_XENON_RENDER)


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __MATMULTITEXTURE_H__ */