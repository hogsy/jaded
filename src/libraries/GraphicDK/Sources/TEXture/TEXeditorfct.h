/*$T TEXeditorfct.h GC! 1.081 05/22/01 09:37:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifdef ACTIVE_EDITORS

/*$F
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 bitmap conversion fonction header
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef __TEXEDITORFCT_H__
#define __TEXEDITORFCT_H__

#ifndef PSX2_TARGET
#pragma once
#endif
#include "TEXture/TEXfile.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

typedef struct TEX_tdst_4Edit_IndexList_
{
    ULONG   *pul_Indexes;
    ULONG   ul_Number;
    ULONG   ul_Max;
    ULONG   ul_Index;
} TEX_tdst_4Edit_IndexList;

typedef struct TEX_tdst_4Edit_CreateBitmapResult_
{
    HBITMAP             *ph_Colors;
    HBITMAP             *ph_Alpha;
    HBITMAP             *ph_PaletteColors;
    HBITMAP             *ph_PaletteAlpha;
    int                 i_PalWidth;
    int                 i_PalHeight;
    int                 *pi_Width;
    int                 *pi_Height;
    char                **ppc_Raw;
    TEX_tdst_Palette	*pst_Pal;
    ULONG               ul_PalKey;
    ULONG               ul_RawKey;
    BOOL                b_RawPalPrio;
} TEX_tdst_4Edit_CreateBitmapResult;


int     TEX_i_4Edit_CreateBitmap(ULONG, int, HDC, TEX_tdst_4Edit_CreateBitmapResult * ); //HBITMAP *, HBITMAP *, int *, int *);
int		TEX_i_4Edit_SaveBitmap( HBITMAP, int, int, char *);
ULONG   TEX_ul_4Edit_ParseTexture( BIG_INDEX, int, int (*pfni_Callback) (ULONG, ULONG *), ULONG * );
ULONG   TEX_ul_4Edit_FindPaletteForRaw( BIG_INDEX );
ULONG   TEX_ul_4Edit_WhoUseTexture( BIG_INDEX, TEX_tdst_4Edit_IndexList *);
void	TEX_4Edit_SearchDuplicateRaw( void );
void    TEX_4Edit_CheckMatTexture( void );
void    TEX_4Edit_CheckMultiMat( void );
void    TEX_4Edit_CheckSoundID( void );
void    TEX_4Edit_WhoUseMaterial( ULONG );

void	TEX_4Edit_ExportTga( ULONG, char *, int , int , int *, char ** );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXCONVERT_H */

#endif /* ACTIVE_EDITORS */

