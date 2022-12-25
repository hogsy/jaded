/*$T Gx8tex.h GC!1.52 11/26/99 13:55:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */




#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GX8TEXT_H__
#define __GX8TEXT_H__

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "Gx8init.h"
#include "BASe/BAStypes.h"
#include "GDInterface/GDInterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */
void    Gx8_Texture_Load( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
LONG    Gx8_l_Texture_Store( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
void    Gx8_Palette_Load( GDI_tdst_DisplayData *, TEX_tdst_Palette *, ULONG );
void    Gx8_Set_Texture_Palette( GDI_tdst_DisplayData *, ULONG _ulTexNum, ULONG IdexTex, ULONG IndexPal );

void    Gx8_Texture_Unload( GDI_tdst_DisplayData * );
void    Gx8_Texture_Set(struct GDI_tdst_DisplayData_ *, LONG);
LONG    Gx8_l_Texture_Init( GDI_tdst_DisplayData *, ULONG );

void	Gx8_ReloadPalette(GDI_tdst_DisplayData *_pst_DD, ULONG ulPaletteIndex);
void	Gx8_CreateAfterFXTextures(Gx8_tdst_SpecificData *_pst_SD);

void	Gx8_ReleasePalette(void);
// initializa manual palette generation
//void	Gx8_InitPaletteMemory(void);
void _MakeSpecularExponent32b( ULONG* buffer, int width, int height ,int exp);

#ifdef __cplusplus
}
#endif



#endif  /* __GX8TEXT_H__ */
