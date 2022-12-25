// ------------------------------------------------------------------------------------------------
// File   : TEXcubemap.h
// Date   : 2005-01-18
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_TEXCUBEMAP_H
#define GUARD_TEXCUBEMAP_H

#if defined(_XENON_RENDER)
#define TEX_USE_CUBEMAPS
#endif // defined(_XENON_RENDER)

#if defined(TEX_USE_CUBEMAPS)

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
#define TEX_CubeMap_InvalidIndex -1

// ------------------------------------------------------------------------------------------------
// STRUCTURES
// ------------------------------------------------------------------------------------------------
// Information about a loaded cube map
typedef struct TEX_tdst_CubeMapInfo_
{
    BIG_KEY ul_Key;
    ULONG   ul_NbReferences;
    SHORT   s_Index;
    BOOL    b_HWLoaded;
} TEX_tdst_CubeMapInfo;

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

void TEX_CubeMap_Init(ULONG _ul_InitCubeMapCount);
void TEX_CubeMap_Free(BOOL _b_Shutdown);

void TEX_CubeMap_HardwareLoad(void);
void TEX_CubeMap_HardwareUnload(void);

SHORT                  TEX_CubeMap_Add(BIG_KEY _ul_Key);
void                   TEX_CubeMap_Remove(SHORT _s_Index);
TEX_tdst_CubeMapInfo_* TEX_CubeMap_GetInfo(SHORT _s_Index);
BIG_KEY                TEX_CubeMap_GetKey(SHORT _s_Index);

void TEX_CubeMap_CheckUsedTextures(CHAR* _pc_UsedIndex);

#endif // defined(TEX_USE_CUBEMAPS)

#endif // #ifdef GUARD_TEXCUBEMAP_H
