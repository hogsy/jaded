/*$T DX8tex.h GC!1.52 11/26/99 13:55:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */




#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __DX8TEXT_H__
#define __DX8TEXT_H__

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "DX8init.h"
#include "GDInterface/GDInterface.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */
void    DX8_Texture_Load( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
LONG    DX8_l_Texture_Store( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
void    DX8_Palette_Load( GDI_tdst_DisplayData *, TEX_tdst_Palette *, ULONG );
void    DX8_Set_Texture_Palette( GDI_tdst_DisplayData *, ULONG _ulTexNum, ULONG IdexTex, ULONG IndexPal );

void    DX8_Texture_Unload( GDI_tdst_DisplayData * );
void    DX8_Texture_Set(struct GDI_tdst_DisplayData_ *, LONG);
LONG    DX8_l_Texture_Init( GDI_tdst_DisplayData *, ULONG );

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ::: Texture Compression :::::::::::::::::::::::::::::::::::::::::::::::::::
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#if defined( ACTIVE_EDITORS )

/**
 * Fuctions included in this file are intended for texture compression.
 * These functions are used only during editor, so while in Engine mode
 * they are not compiled.
 */

/**
 * TEX_Compress is the function used to compress a texture in a given
 * format.
 *
 * @param textureDescription the texture to compress.
 * @param format the requested compression format.
 * @return the DX8 texture.
 */

PDIRECT3DTEXTURE8*
DX8_Compress( TEX_tdst_File_Desc* textureDescription, D3DFORMAT fmtTo );

/**
 * TEX_DtcCompress performs a DTC texture compression. It returns a buffer
 * containing the DTC file to dump on disc.
 * This function is needed only in editor, so it is not included when
 * compiling for engines.
 *
 * @param textureDescription the description of the texture.
 * @param newSize the size of the buffer to dump on disk.
 * @return a pointer to a buffer containing the texture file as it has to
 *         be stored on disk. The buffer has been allocated via malloc and
 *         it is up to the caller its release through free.
 */


void*
DX8_DtcCompress( const TEX_tdst_File_Desc* textureDescription, size_t* newSize );

#endif      // defined( ACTIVE_EDITORS )



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif



#endif  /* __DX8TEXT_H__ */