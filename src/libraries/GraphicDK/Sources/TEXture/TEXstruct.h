/*$T TEXstruct.h GC! 1.081 10/11/00 14:40:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Name : TEXstruct.h Description : */
#ifndef __TEXSTRUCT_H__
#define __TEXSTRUCT_H__

#include "BIGfiles/BIGkey.h"
#include "BASe/BAStypes.h"

#ifndef PSX2_TARGET
#pragma once
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* For texture flags */
#define TEX_uw_WrapU		0x0001
#define TEX_uw_WrapV		0x0002
#define TEX_uw_AlphaTest	0x0004
#define TEX_uw_Alpha		0x0008
#define TEX_uw_Mipmap		0x0010
#define TEX_uw_IsSpriteGen	0x0020
#define TEX_uw_RawPal       0x0040
#define TEX_uw_Ani          0x0080
#define TEX_uw_Shared       0x0100

/* palette flags */
#define TEX_uc_Palette16	    0x01
#define TEX_uc_AlphaPalette     0x02
#define TEX_uc_InvalidPalette   0x04
#define TEX_uc_UpdatablePal     0x08
#define TEX_uc_KeepPalInMem     0x10
#define TEX_uc_IndexInverted    0x80


/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    A texture
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEX_tdst_Data_
{
	BIG_KEY			ul_Key;
	union
	{
		short			w_Width;
		short			w_TexPal;
		short			w_Dummy;
	};
	union
	{
		short			w_Height;
		short			w_TexRaw;
		short			w_TexTC;
	};
	unsigned short	uw_Flags;
	short			w_Index;
} TEX_tdst_Data;

typedef struct	TEX_tdst_Palette_
{
	BIG_KEY			ul_Key;
    ULONG           *pul_Color;
	unsigned char	uc_Flags;
	unsigned char	uc_BitPerColor;
	short			w_Index;
} TEX_tdst_Palette;

/*
 -----------------------------------------------------------------------------------------------------------------------
    graphic data list list
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEX_tdst_List_
{
    LONG                l_NumberMaxOfTextures;
	LONG				l_NumberOfTextures;
	TEX_tdst_Data		*dst_Texture;

    LONG                l_NumberMaxOfPalettes;
	LONG				l_NumberOfPalettes;
	TEX_tdst_Palette	*dst_Palette;
} TEX_tdst_List;

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

extern TEX_tdst_List	TEX_gst_GlobalList;
#ifdef ACTIVE_EDITORS
extern LONG				TEX_gal_MadToGlobalIndex[1024];
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void					TEX_List_Init(TEX_tdst_List *);
void					TEX_List_Free(TEX_tdst_List *);

short			        TEX_w_List_AddTexture(TEX_tdst_List *, BIG_KEY, char);
TEX_tdst_Data			*TEX_pst_List_FindTexture(TEX_tdst_List *, BIG_KEY);
BIG_KEY					TEX_ul_List_FindTexture(TEX_tdst_List *, short);

short			        TEX_w_List_AddPalette(TEX_tdst_List *, BIG_KEY);
TEX_tdst_Palette		*TEX_pst_List_FindPalette(TEX_tdst_List *, BIG_KEY);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXSTRUCT_H__ */ 
 