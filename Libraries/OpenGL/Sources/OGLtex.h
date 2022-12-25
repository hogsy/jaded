/*$T OGLtex.h GC!1.52 11/26/99 13:55:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */




#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __OGLTEXT_H__
#define __OGLTEXT_H__

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "OGLinit.h"
#include "BASe/BAStypes.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */
void    OGL_Texture_Load( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
LONG    OGL_l_Texture_Store( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
void    OGL_Palette_Load( GDI_tdst_DisplayData *, TEX_tdst_Palette *p_PAL,ULONG );
void    OGL_Set_Texture_Palette( GDI_tdst_DisplayData *, ULONG _ulTexNum, ULONG IdexTex, ULONG IndexPal );

void    OGL_Texture_SetPalette(int _i_Palette);

void    OGL_Texture_Unload( GDI_tdst_DisplayData * );
void    OGL_Texture_UnLoadCompare( void );
void    OGL_Texture_Set(struct GDI_tdst_DisplayData_ *, LONG);
LONG    OGL_l_Texture_Init( GDI_tdst_DisplayData *, ULONG );

void    OGL_Texture_LoadInterfaceTex( GDI_tdst_DisplayData * );
void    OGL_Texture_UnloadInterfaceTex( GDI_tdst_DisplayData *);
void    *OGL_p_Texture_GetInterfaceTexBuffer( GDI_tdst_DisplayData *, ULONG );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif



#endif  /* __OGLTEXT_H__ */