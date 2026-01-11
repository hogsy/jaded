/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef ACTIVE_EDITORS

//#include "BASe/BAStypes.h"
//#include "BASE/CLIbrary/CLIwin.h"
//#include "BIGfiles/BIGdefs.h"
//#include "EDIbaseframe.h"
//#include "Res/Res.h"
#include "TEXture/TEXfile.h"


/*
 ---------------------------------------------------------------------------------------------------
    Description of a bitmap
 ---------------------------------------------------------------------------------------------------
 */
enum MAIEDITEX_TextureType
{
	MAIEDITEX_C_INVALID,
	MAIEDITEX_C_BMP,
	MAIEDITEX_C_TGA,
	MAIEDITEX_C_JPG,
	MAIEDITEX_C_PALETTE,
	MAIEDITEX_C_RAW,
	MAIEDITEX_C_TEX,
	MAIEDITEX_C_DDS,
	MAIEDITEX_C_PSD,
	MAIEDITEX_C_PNG,
	MAIEDITEX_C_GIF
};

typedef struct  MAIEDITEX_tdst_Bitmap_
{
    HBITMAP             h_Bitmap;
    HBITMAP             h_BitmapAlpha;
    BITMAPINFOHEADER    st_Header;
	BOOL				b_Compress;
} MAIEDITEX_tdst_Bitmap;

typedef struct MAIEDITEX_tdst_BitmapDes_
{
    int                     i_Type;
    BIG_INDEX               ul_FatFile;
    CRect                   o_Rect;
    MAIEDITEX_tdst_Bitmap   st_Bmp;
	TEX_tdst_File_Tex		st_Tex;
	ULONG					ul_PaletteFlags;
} MAIEDITEX_tdst_BitmapDes;

typedef struct MAIEDITEX_tdst_SelBitmapDes_
{
    int                     i_Type;
    BIG_INDEX               ul_FatFile;
    CRect                   o_Rect;
    MAIEDITEX_tdst_Bitmap   st_Bmp;
	TEX_tdst_File_Tex		st_Tex;
	ULONG					ul_PaletteFlags;

	char				c_Quality;
	short				uw_Flags;
	ULONG				ul_Color;
	ULONG				ul_FontDescKey;
} MAIEDITEX_tdst_SelBitmapDes;

/*
 ---------------------------------------------------------------------------------------------------
    functions
 ---------------------------------------------------------------------------------------------------
 */

MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertBitmap(CView *, BIG_INDEX, UCHAR *);
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertRaw( CView	*, BIG_INDEX, UCHAR *, UCHAR *, short, short, UCHAR, int, BOOL );
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertJPG(CView *, BIG_INDEX , UCHAR *, ULONG );
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertTGA(CView *, BIG_INDEX , UCHAR *);
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertPalette(CView *, BIG_INDEX , UCHAR *);
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertRawFile(CView *, BIG_INDEX , UCHAR *);
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertStbiFile( CView *view, BIG_INDEX fatFile, uint8_t *buffer, unsigned int length, int format );

#if defined(_XENON_RENDER)
MAIEDITEX_tdst_BitmapDes* TEXUtil_ConvertDDS(CView*, BIG_INDEX, UCHAR*, ULONG);
void Xe_GenerateAllDds();
#endif

#endif  /* ACTIVE_EDITORS */
