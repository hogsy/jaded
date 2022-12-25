/*$T TEXconvert.h GC! 1.081 10/11/00 17:46:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 bitmap conversion fonction header
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef __TEXCONVERT_H__
#define __TEXCONVERT_H__

#ifndef PSX2_TARGET
#pragma once
#endif
#include "TEXture/TEXfile.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    "general" Functions
 ***********************************************************************************************************************
 */

LONG	TEX_l_ConvertPF(TEX_tdst_File_Desc *, TEX_tdst_File_Desc *);
LONG	TEX_l_DimensionAreGood(TEX_tdst_File_Desc *, LONG, LONG, BOOL, TEX_tdst_File_Desc *);
LONG	TEX_l_ConvertSize(TEX_tdst_File_Desc *, TEX_tdst_File_Desc *);
LONG	TEX_l_ConvertSizeFast(TEX_tdst_File_Desc *);

/*$4
 ***********************************************************************************************************************
    24 to 32 operations
 ***********************************************************************************************************************
 */

LONG	TEX_l_ConvertSize_24To32(TEX_tdst_File_Desc *, TEX_tdst_File_Desc *);
void	TEX_Convert_24To32(ULONG *, ULONG);
void	TEX_Convert_24To32SwapRB(ULONG *, ULONG);

/*$4
 ***********************************************************************************************************************
    32 to 24 operations
 ***********************************************************************************************************************
 */
void	TEX_Convert_32To24(ULONG *, ULONG);

/*$4
 ***********************************************************************************************************************
    32 to 32 bit operation
 ***********************************************************************************************************************
 */

LONG	TEX_l_ConvertSize_32(TEX_tdst_File_Desc *, TEX_tdst_File_Desc *);
void	TEX_Compress_Xo2(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Compress_Yo2(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Compress_Yo2InvertColor(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Expand_XBy2(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Expand_YBy2(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Convert_32SwapRB(ULONG *, ULONG);
void	TEX_Blend_AlphaColor50(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Blend_AlphaColor25(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Blend_Alpha50(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Blend_Alpha25(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Blend_Color50(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Blend_Color25(ULONG *, ULONG *, ULONG, ULONG);
void	TEX_Blend_KeepBorder(ULONG *, ULONG, ULONG);

/*$4
 ***********************************************************************************************************************
    24 to 24 bit operation
 ***********************************************************************************************************************
 */

void	TEX_Compress24_Xo2(unsigned char *, unsigned char *, ULONG, ULONG);
void	TEX_Compress24_Yo2(unsigned char *, unsigned char *, ULONG, ULONG);
void	TEX_Compress24_Yo2InvertColor(unsigned char *, unsigned char *, ULONG, ULONG);
void	TEX_Expand24_XBy2(unsigned char *, unsigned char *, ULONG, ULONG);
void	TEX_Expand24_YBy2(unsigned char *, unsigned char *, ULONG, ULONG);
void	TEX_Blend24_Color50(unsigned char *, unsigned char *, ULONG, ULONG);
void	TEX_Blend24_Color25(unsigned char *, unsigned char *, ULONG, ULONG);
void	TEX_Blend24_KeepBorder(unsigned char *, ULONG, ULONG);

/*$4
 ***********************************************************************************************************************
    8 to 8 operation
 ***********************************************************************************************************************
 */

void TEX_Compress8_Xo2(unsigned char *, unsigned char *, ULONG , ULONG );
void TEX_Compress8_Yo2(unsigned char *, unsigned char *, ULONG , ULONG );
void TEX_Expand8_XBy2(unsigned char *, unsigned char *, ULONG , ULONG );
void TEX_Expand8_YBy2(unsigned char *, unsigned char *, ULONG , ULONG );

/*$4
 ***********************************************************************************************************************
    4 to 4 bits operations
 ***********************************************************************************************************************
 */

void TEX_Compress4_Xo2(unsigned char *, unsigned char *, ULONG , ULONG );
void TEX_Compress4_Yo2(unsigned char *, unsigned char *, ULONG , ULONG );
void TEX_Expand4_XBy2(unsigned char *, unsigned char *, ULONG , ULONG );
void TEX_Expand4_YBy2(unsigned char *, unsigned char *, ULONG , ULONG );

/*$4
 ***********************************************************************************************************************
    4 to 8 bits operations
 ***********************************************************************************************************************
 */
void TEX_Convert_4To8( unsigned char *, unsigned char *, ULONG, ULONG );

/*$4
 ***********************************************************************************************************************
    8 to 4 bits operations
 ***********************************************************************************************************************
 */
void TEX_Convert_8To4( unsigned char *, unsigned char *, ULONG, ULONG );


/*$4
 ***********************************************************************************************************************
    4/8 to 32 operation
 ***********************************************************************************************************************
 */

void	TEX_Convert_8To32(ULONG *, UCHAR *, ULONG *, ULONG);
void	TEX_Convert_8To32SwapRB(ULONG *, UCHAR *, ULONG *, ULONG);
void	TEX_Convert_4To32(ULONG *, UCHAR *, ULONG *, ULONG);
void	TEX_Convert_4To32SwapRB(ULONG *, UCHAR *, ULONG *, ULONG);
void    TEX_Convert_8To24(UCHAR *, UCHAR *, ULONG *, int, int, int );
void    TEX_Convert_4To24(UCHAR *, UCHAR *, ULONG *, int, int, int );
void    TEX_Convert_8To24_ChannelAlpha(UCHAR *, UCHAR *, ULONG *, int, int, int );
void    TEX_Convert_4To24_ChannelAlpha(UCHAR *, UCHAR *, ULONG *, int, int, int );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXCONVERT_H */

