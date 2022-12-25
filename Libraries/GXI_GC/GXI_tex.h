/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */




#ifndef __GXI_TEX_H__
#define __GXI_TEX_H__

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "BASe/BAStypes.h"

typedef struct 
{
	u32 r : 5;
	u32 g : 6;
	u32 b : 5;
} tdst_Palette_RGB565;

typedef struct 
{
	u32 flag : 1;
	u32 a : 3;
	u32 r : 4;
	u32 g : 4;
	u32 b : 4;
} tdst_Palette_RGB5A3;

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */

void    GXI_Texture_Load( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
void    GXI_Palette_Load( GDI_tdst_DisplayData *, TEX_tdst_Palette *p_PAL, ULONG );
void    GXI_Set_Texture_Palette( GDI_tdst_DisplayData *, ULONG ulTexNum, ULONG IdexTex, ULONG IndexPal );
void    GXI_Texture_Unload( GDI_tdst_DisplayData * );
LONG    GXI_l_Texture_Init( GDI_tdst_DisplayData *, ULONG );


#endif 