// Dx9tex.h

#ifndef __DX9TEX_H__
#define __DX9TEX_H__

#include "Dx9struct.h"

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "GDInterface/GDInterface.h"


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void	Dx9_Texture_Load( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
LONG	Dx9_l_Texture_Store( GDI_tdst_DisplayData *, TEX_tdst_Data *, TEX_tdst_File_Desc *, ULONG );
void	Dx9_Palette_Load( GDI_tdst_DisplayData *, TEX_tdst_Palette *, ULONG );
void	Dx9_Set_Texture_Palette( GDI_tdst_DisplayData *, ULONG _ulTexNum, ULONG IdexTex, ULONG IndexPal );
void	Dx9_Texture_Unload( GDI_tdst_DisplayData * );
LONG	Dx9_l_Texture_Init( GDI_tdst_DisplayData *, ULONG );



#ifdef __cplusplus
}
#endif

#endif /* __DX9TEX_H__ */
