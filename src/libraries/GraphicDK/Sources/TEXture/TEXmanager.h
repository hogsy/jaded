/*$T TEXmanager.h GC!1.52 11/26/99 09:46:12 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __TEXMANAGER_H__
#define __TEXMANAGER_H__


#ifndef PSX2_TARGET
#pragma once
#endif

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define TEX_Manager_AcceptNonSquare     0x00000001
#define TEX_Manager_ForceReload         0x00000002
#define TEX_Manager_FastCompression     0x00000004
#define TEX_Manager_UseMipmap           0x00000008
#define TEX_Manager_InvertMipmap        0x00000010
#define TEX_Manager_DumpBench           0x00000020
#define TEX_Manager_Accept32bpp         0x00000100
#define TEX_Manager_Accept24bpp         0x00000200
#define TEX_Manager_Accept16bpp_1555    0x00000400
#define TEX_Manager_Accept16bpp_4444    0x00000800
#define TEX_Manager_Accept8bpp          0x00001000
#define TEX_Manager_Accept4bpp          0x00002000
#define TEX_Manager_AcceptAlphaPalette  0x00004000
#define TEX_Manager_OneTexForRawPal     0x00008000
#define TEX_Manager_FixVRam				0x00010000
#define TEX_Manager_RGB2BGR             0x00100000
#define TEX_Manager_StoreInterfaceTex   0x00200000
#define TEX_Manager_LoadStoredITex      0x00400000

#define TEX_Manager_AcceptAllPalette            (TEX_Manager_Accept8bpp | TEX_Manager_Accept4bpp | TEX_Manager_AcceptAlphaPalette)
#define TEX_Manager_AcceptAllPaletteButAlpha    (TEX_Manager_Accept8bpp | TEX_Manager_Accept4bpp )

#define TEX_Manager_SlotPS2				0
#define TEX_Manager_SlotGC				1
#define TEX_Manager_SlotXBOX			2
#define TEX_Manager_SlotPC				3
#define TEX_Manager_SlotNone			-1

/*$4
 ***************************************************************************************************
    Structures
 ***************************************************************************************************
 */

typedef struct  TEX_tdst_Manager_
{
    ULONG   ul_Flags;
    LONG    l_MaxTextureSize;
    LONG    l_MinTextureSize;

    LONG    l_VRamAvailable;
    LONG    l_VRamLeft;
    LONG    l_VRamNeeded;

    LONG    l_RamAvailable;
    LONG    l_RamNeeded;
    LONG    l_RamLeft;

    LONG    l_MemoryTakenByLoading;
    LONG    l_TextureNumber;
    LONG    l_VRamTextureNumber;
    LONG    l_RamTextureNumber;

	char	c_TexSlotOrder[ 4 ];

#ifdef ACTIVE_EDITORS
    LONG    al_TextureNumberByBpp[5];
    LONG    aaal_TextureNumberByBppWidthHeight[5][8][8];
    LONG    l_NbTextureInterface;
    ULONG   aul_TextureInterface[ 64 ];
    USHORT  auw_TIH[ 64 ];
    USHORT  auw_TIW[ 64 ];
    USHORT  auw_TICH[ 64 ];
    USHORT  auw_TICW[ 64 ];

	//int		i_Console;
#endif

} TEX_tdst_Manager;

/*$4
 ***************************************************************************************************
    Function header
 ***************************************************************************************************
 */

void    TEX_Manager_Init(TEX_tdst_Manager *);
void    TEX_Manager_Reinit(TEX_tdst_Manager *);

struct TEX_tdst_File_Desc_;
void    TEX_Manager_ComputeCompression( TEX_tdst_Manager *, struct TEX_tdst_File_Desc_ *, LONG, char );
void    TEX_Manager_ValidateTextures( TEX_tdst_Manager *, struct TEX_tdst_File_Desc_ *, LONG );
SHORT   TEX_w_Manager_ChooseGoodInTex( TEX_tdst_Manager *, struct TEX_tdst_File_Desc_ * );

#ifdef ACTIVE_EDITORS
void TEX_Manager_AddInfo( TEX_tdst_Manager *, struct TEX_tdst_Data_ *, struct TEX_tdst_File_Desc_ * );
#else
#define TEX_Manager_AddInfo(a,b,c);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif __TEXMANAGER_H__ 
 
