/*$T OGLtex.h GC!1.52 11/26/99 13:55:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */




#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GSPTEXT_H__
#define __GSPTEXT_H__
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "GS_PS2/GSPinit.h"
#include "BASe/BAStypes.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */

void    GSP_Texture_Load( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
LONG    GSP_l_Texture_Store( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
void    GSP_Palette_Load( GDI_tdst_DisplayData *, TEX_tdst_Palette *p_PAL, ULONG );
void    GSP_Set_Texture_Palette( GDI_tdst_DisplayData *, ULONG ulTexNum, ULONG IdexTex, ULONG IndexPal );
void    GSP_Texture_Unload( GDI_tdst_DisplayData * );
void    GSP_Texture_Set(struct GDI_tdst_DisplayData_ *, LONG);
LONG    GSP_l_Texture_Init( GDI_tdst_DisplayData *, ULONG );


#ifdef __cplusplus
}
#endif



#endif  /* __OGLTEXT_H__ */