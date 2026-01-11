/*$T TEXfile.h GC! 1.081 04/06/00 09:28:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __TEXTFILE_H__
#define __TEXTFILE_H__

#include "BIGfiles/BIGkey.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
#define TEX_Csz_X360TexturesPath "ROOT/X360Pack/01"
#endif
#define TEX_Cuw_DF_Info					0x0001
#define TEX_Cuw_DF_Content				0x0002
#define TEX_Cuw_DF_OnlyParams           0x0004
#define TEX_Cuw_DF_AllocBuffers			0x0008
#define TEX_Cuw_DF_VeryBadBoy			0x0010
#define TEX_Cuw_DF_SpecialNoLoad        0x0020
#define TEX_Cuw_DF_Referenced           0x0040
#define TEX_Cuw_DF_ConvertTo32BPP       0x0080
#define TEX_Cuw_DF_XenonFileAsBitmap    0x0100
#define TEX_Cuw_DF_EditorUseXenonFile   0x0200

/* flags */
#define TEX_FP_QualityVeryLow		0x0000
#define TEX_FP_QualityLow			0x0001
#define TEX_FP_QualityMedium		0x0002
#define TEX_FP_QualityHigh			0x0003
#define TEX_FP_QualityVeryHigh		0x0004
#define TEX_FP_QualityMask			0x0007
#define TEX_FP_MipmapOn			    0x0008
#define TEX_FP_MipmapUseAlpha		0x0010
#define TEX_FP_MipmapUseColor		0x0020
#define TEX_FP_ThisIsAFont			0x0040
#define TEX_FP_MipmapKeepBorder	    0x0080
#define TEX_FP_KeepPaletteInMem     0x0100
#define TEX_FP_Interface            0x0200
#define TEX_FP_BumpMap              0x0400
#define TEX_FP_NormaleMap1			0x0800

/* type */
enum TextureFileType
{
	TEX_FP_TgaFile    = 0x01,
	TEX_FP_BmpFile    = 0x02,
	TEX_FP_JpgFile    = 0x03,
	TEX_FP_SprFile    = 0x04,
	TEX_FP_ProFile    = 0x05,
	TEX_FP_RawFile    = 0x06,
	TEX_FP_RawPalFile = 0x07,
	TEX_FP_PalFile    = 0x08,
	TEX_FP_AniFile    = 0x09,
	TEX_FP_DdsFile    = 0x0A,// XBox compressed texture.
	TEX_FP_XeDDSFile  = 0x0B,
	// hogsy: new formats
	TEX_FP_GifFile = 0x0C,
	TEX_FP_PsdFile = 0x0D,
	TEX_FP_PngFile = 0x0E,
};

/* format */
#define TEX_FP_32bpp                0x10
#define TEX_FP_24bpp                0x20
#define TEX_FP_16bpp                0x30
#define TEX_FP_8bpp                 0x40
#define TEX_FP_4bpp                 0x50

#define TEX_FP_bppMask              0xF0

#define TEX_FP_1555                 0x30
#define TEX_FP_4444                 0x31

/* flags for TEX file */
#define TEX_TakeRawPal              0x0001
#define TEX_TakeTrueColor           0x0002
#define TEX_ChangeToTrueColor       0x0004
#define TEX_XenonTexture            0x0008
#define TEX_XenonNeedsConversion    0x0010
#define TEX_XenonConvert            0x0020

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
extern char *TEX_gasz_QualityName[5];
#endif

/*$4
 ***********************************************************************************************************************
    Special memory management macros
 ***********************************************************************************************************************
 */
#ifdef MEM_OPT		
#define TEX_M_File_Alloc(p, l, t) \
	{ \
		p = (t *) MEM_p_AllocAlign(l, 32); \
	}
#define TEX_M_File_AllocTmp(p, l, t) \
    { \
        p = (t *) MEM_p_AllocTmp(l); \
    }
#elif defined(ACTIVE_EDITORS)
#define TEX_M_File_Alloc(p, l, t) \
	{ \
		p = (t *) MEM_p_AllocAlign(l, 64); \
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated += MEM_ul_GetRealSize((void *)p); \
	}
#define TEX_M_File_AllocTmp(p, l, t) \
    { \
        p = (t *) MEM_p_AllocTmp(l); \
        MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated += MEM_ul_GetRealSize(p); \
    }
#else
#define TEX_M_File_Alloc(p, l, t) \
	{ \
		p = (t *) MEM_p_AllocAlign(l, 64); \
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated += MEM_ul_GetRealSize((void *) *((ULONG **) p - 1)); \
	}
#define TEX_M_File_AllocTmp(p, l, t) \
    { \
        p = (t *) MEM_p_AllocTmp(l); \
        MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated += MEM_ul_GetRealSize((void *) *((ULONG **) p - 1)); \
    }
#endif

#ifdef MEM_OPT		
#define TEX_M_File_Realloc(p, l, t) \
	{ \
		p = (t *) MEM_p_ReallocAlign(p, l, 32); \
	}
#elif defined(ACTIVE_EDITORS)
#define TEX_M_File_Realloc(p, l, t) \
	{ \
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated -= MEM_ul_GetRealSize((void *) p); \
		p = (t *) MEM_p_ReallocAlign(p, l, 64); \
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated += MEM_ul_GetRealSize((void *) p); \
	}
#else
#define TEX_M_File_Realloc(p, l, t) \
	{ \
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated -= MEM_ul_GetRealSize((void *) *((ULONG **) p - 1)); \
		p = (t *) MEM_p_ReallocAlign(p, l, 64); \
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated += MEM_ul_GetRealSize((void *) *((ULONG **) p - 1)); \
	}
#endif	

#ifdef MEM_OPT		
#define TEX_M_File_Free(p) \
	{ \
		MEM_FreeAlign(p); \
	}
#elif defined(ACTIVE_EDITORS)
#define TEX_M_File_Free(p) \
	{ \
        if(MEM_IsTmpPointer((char*)(p)))\
            MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated -= ((ULONG*)(p))[-1]; \
        else\
		    MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated -= MEM_ul_GetRealSize((void *)p); \
		MEM_FreeAlign(p); \
	}
#else
#define TEX_M_File_Free(p) \
	{ \
        if(MEM_IsTmpPointer((char*)(p)))\
            MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated -= ((ULONG*)(p))[-1]; \
        else\
		    MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated -= MEM_ul_GetRealSize((void *) *((ULONG **) p - 1)); \
		MEM_FreeAlign(p); \
	}
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	TEX_tdst_File_Params_
{
    ULONG           ul_Mark;
    USHORT          uw_Flags;                   /// Flags:
                                                /// - TEX_FP_MipmapOn
                                                /// - TEX_FP_Interface
                                                /// - TEX_FP_QualityMedium
                                                /// - TEX_FP_ThisIsAFont
                                                /// - TEX_FP_BumpMap

    unsigned char   uc_Type;                    /// file type:
                                                /// - TEX_FP_TgaFile
                                                /// - TEX_FP_BmpFile
                                                /// - TEX_FP_DdsFile
                                                /// - TEX_FP_JpgFile
                                                /// - TEX_FP_RawFile
                                                /// - TEX_FP_SprFile
                                                /// - TEX_FP_ProFile
                                                /// - TEX_FP_AniFile
                                                /// - TEX_FP_RawPalFile
    unsigned char   uc_Format;
    USHORT          uw_Width;
    USHORT          uw_Height;
	ULONG	        ul_Color;
	
    ULONG	        ul_Params[4];
} TEX_tdst_File_Params;

typedef struct	TEX_tdst_File_TexSlot_
{
	ULONG	ul_Raw;
	ULONG	ul_Pal;
	ULONG	ul_TC;
} TEX_tdst_File_TexSlot;

enum TEX_XenonTextureFormat
{
    TEX_XTF_AUTO               = 0,
    TEX_XTF_DXT1               = 1,
    TEX_XTF_DXT5               = 2,
    TEX_XTF_A8                 = 3,
    TEX_XTF_A8R8G8B8           = 4,
    TEX_XTF_DXN                = 5,
    TEX_XTF_CTX1               = 6,

    TEX_XTF_COUNT,

    TEX_XTF_FORMAT_FORCE_DWORD = 0xffffffff
};

#define TEX_Xe_IsNormalMap      0x00000001
#define	TEX_Xe_IsCubeMap		0x00000002

#define CUBEMAP_FACE_NUM	6


typedef struct TEX_tdst_XenonFileTex_
{
    ULONG   ul_Flags;
    BIG_KEY ul_OriginalTexture;
    BIG_KEY ul_NativeTexture;
    ULONG   ul_OutputWidth;
    ULONG   ul_OutputHeight;
    ULONG   ul_OutputFormat;
    ULONG   ul_NbLevels;
    ULONG   ul_FileSize;
} TEX_tdst_XenonFileTex;

typedef struct TEX_tdst_XenonCubeMapFileTex_
{
	ULONG   ul_Flags;
	BIG_KEY ul_OriginalTextures[CUBEMAP_FACE_NUM];
	BIG_KEY ul_NativeTexture;
	ULONG   ul_OutputSize;
	ULONG   ul_OutputFormat;
	ULONG   ul_NbLevels;
	ULONG   ul_FileSize;
} TEX_tdst_XenonCubeMapFileTex;

typedef struct	TEX_tdst_File_Tex_
{
	TEX_tdst_File_TexSlot	ast_Slot[ 4 ];
	//ULONG					ul_Flags;
#if defined(_XENON_RENDER)
    TEX_tdst_XenonFileTex st_XeProperties;
#endif
} TEX_tdst_File_Tex;

typedef struct	TEX_tdst_File_Desc_
{
	unsigned short			uw_DescFlags;       /// description flags (?)
                                                /// - TEX_Cuw_DF_Info - request to load header only
                                                /// - TEX_Cuw_DF_OnlyParams - ?
                                                /// - TEX_Cuw_DF_Content - request to load texture content
                                                /// - TEX_Cuw_DF_SpecialNoLoad

	unsigned short			uw_FileFlags;       /// Texture properties (?)
                                                /// - TEX_uw_Mipmap
	
    unsigned short			uw_Width;
	unsigned short			uw_Height;
    unsigned short			uw_ValidWidth;
	unsigned short			uw_ValidHeight;
    unsigned short			uw_CompressedWidth;
	unsigned short			uw_CompressedHeight;
	
    unsigned char			uc_BPP;
    unsigned char			uc_FinalBPP;
	
    unsigned char			uc_PaletteBPC;
	unsigned short			uw_PaletteLength;
    
	ULONG					ul_RMask;           /// mask to extract/set color components
	ULONG					ul_GMask;           /// from image (if true color).
	ULONG					ul_BMask;
	ULONG					ul_AMask;

    void					*p_Palette;         /// palette pointer (if palettized)
	void					*p_Bitmap;          /// the texture bits.
    void                    *p_TmpPalette;  
    void                    *p_TmpBitmap;

    /*
	struct 
    {
        LONG                l_Raw24or32;
        LONG                l_Raw4or8;
        LONG                l_Palette;
        ULONG               ul_Flags;
    } st_Tex;
	*/


	TEX_tdst_File_Tex		st_Tex;
	short					w_TexRaw;
	short					w_TexPal;
	short					w_TexTC;
	short					w_TexFlags;
	
    TEX_tdst_File_Params	st_Params;
} TEX_tdst_File_Desc;

/*$4
 ***********************************************************************************************************************
    Function header
 ***********************************************************************************************************************
 */



LONG	TEX_l_File_GetContent(BIG_KEY, TEX_tdst_File_Desc *);
LONG	TEX_l_File_GetInfo(BIG_KEY, TEX_tdst_File_Desc *, BOOL);
LONG	TEX_l_File_GetInfoAndContent(BIG_KEY, TEX_tdst_File_Desc *);
void	TEX_File_FreeDescription(TEX_tdst_File_Desc *);

void	TEX_l_File_TgaUncompress(char, char *, char *, ULONG);
LONG	TEX_l_File_LoadJpeg(char *, TEX_tdst_File_Desc *, ULONG);
LONG    TEX_l_File_LoadTga(char *, TEX_tdst_File_Desc *);
LONG	TEX_l_File_SaveTga(char *, TEX_tdst_File_Desc *);
ULONG   TEX_ul_File_SaveTgaInBF( char *, char *, TEX_tdst_File_Desc *);

LONG	TEX_l_MemoryNeeded(TEX_tdst_File_Desc *);

BOOL    TEX_b_File_CheckParams( TEX_tdst_File_Params *);
void    TEX_File_TransformParams( TEX_tdst_File_Params *);
void    TEX_File_DevelopParams( TEX_tdst_File_Desc * );

void    TEX_File_LoadPalette( struct TEX_tdst_Palette_ *, BOOL );
ULONG   TEX_ul_File_SaveNewPaletteBF( struct TEX_tdst_Palette_ *, char *, char *, BOOL );
ULONG   TEX_ul_File_SavePaletteBF( struct TEX_tdst_Palette_ *, BOOL );

LONG    TEX_l_File_LoadRaw( char *, TEX_tdst_File_Desc * );
ULONG   TEX_ul_File_SaveRawInBF(char *, char *, TEX_tdst_File_Desc *, BOOL);
LONG    TEX_l_File_LoadRawPalette(char *, TEX_tdst_File_Desc *, ULONG );

LONG    TEX_l_File_LoadSpriteGen(ULONG, char *, TEX_tdst_File_Desc *, ULONG );

bool TEX_File_LoadStbiFile( uint8_t *buffer, unsigned int length, TEX_tdst_File_Desc *fileDesc );

#ifdef ACTIVE_EDITORS

BOOL    TEX_b_File_GetName(int, char *);
void	TEX_File_Init(void);
LONG	TEX_l_File_IsFormatSupported(char *, int);
int     TEX_i_File_DefaultParams(TEX_tdst_File_Params *, char *, char *, char *, LONG );
void	TEX_File_SetParams(TEX_tdst_File_Params *, BIG_INDEX, BOOL );
void    TEX_File_InitParams( TEX_tdst_File_Params * );
void	TEX_File_GetParams(TEX_tdst_File_Params *, BIG_INDEX);

ULONG   TEX_ul_File_CreatePaletteRawTexture( char *, char *, ULONG, ULONG, ULONG );
ULONG	TEX_ul_File_SaveTexWithName( char *, char *, TEX_tdst_File_Tex *, BOOL );
ULONG	TEX_ul_File_SaveTexWithIndex( ULONG, TEX_tdst_File_Tex * );

#else
#define TEX_b_File_GetName(a,b) 0
#define TEX_File_Init()
#define TEX_l_File_IsFormatSupported(a, b) 0
#define TEX_File_DefautParams(a, b, c, d, e) 0
#define TEX_File_SetParams(a, b, c)
#define TEX_File_InitParams(a)
#define TEX_File_GetParams(a, b);
#define TEX_ul_File_CreatePaletteRawTexture( a,b,c,d,e );
#endif ACTIVE_EDITORS
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXTFILE_H__ */
