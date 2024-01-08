/*$T TEXutil.cpp GC! 1.081 05/30/01 08:57:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#ifdef ACTIVE_EDITORS

#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGfat.h"
#include "TEXutil.h"
#include "ENGine/Sources/ENGinit.h"
#include "GraphicDK/Sources/TEXture/TEXfile.h"
#include "GraphicDK/Sources/TEXture/TEXconvert.h"
#include "GraphicDK/Sources/TEXture/TEXeditorfct.h"

#ifdef JADEFUSION
#include <vector>
#include "GraphicDK/Sources/TEXture/TEXxenon.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#endif
/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

typedef struct
{
	unsigned char	uc_Size;
	unsigned char	uc_ColorMapType;
	unsigned char	uc_ImageTypeCode;
	unsigned short	uw_Origin;
	unsigned short	uw_PaletteLength;
	unsigned char	uc_BPCInPalette;
	unsigned short	uw_Left;
	unsigned short	uw_Top;
	unsigned short	uw_Width;
	unsigned short	uw_Height;
	unsigned char	uc_BPP;
	unsigned char	ucDescriptorByte;
} TEX_tdst_File_TgaHeader;


/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertBitmap(CView *_po_View, BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BITMAPFILEHEADER			*pst_Header;
	BITMAPINFOHEADER			*pst_Info;
	DWORD						*p_Bits;
	MAIEDITEX_tdst_BitmapDes	*pst_Des;
	CDC							*pdc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get bitmap header */
	pst_Header = (BITMAPFILEHEADER *) _puc_Buf;
	if(pst_Header->bfType != 0x4D42)	/* "BM" */
	{
		return NULL;
	}

	p_Bits = (DWORD *) ((char *) pst_Header + pst_Header->bfOffBits);

	/* Get info header */
	pst_Info = (BITMAPINFOHEADER *) ((char *) pst_Header + sizeof(BITMAPFILEHEADER));

	/* New entry for list */
	pst_Des = new MAIEDITEX_tdst_BitmapDes;
	pst_Des->ul_FatFile = _ul_FatFile;
	pst_Des->st_Bmp.h_BitmapAlpha = NULL;
	pst_Des->i_Type = MAIEDITEX_C_BMP;
	L_memcpy(&pst_Des->st_Bmp.st_Header, pst_Info, sizeof(BITMAPINFOHEADER));

	/* Create bitmap */
	pdc = _po_View->GetDC();
	pst_Des->st_Bmp.h_Bitmap = CreateDIBitmap
		(
			pdc->m_hDC,
			pst_Info,
			CBM_INIT,
			p_Bits,
			(BITMAPINFO *) pst_Info,
			DIB_RGB_COLORS
		);
	_po_View->ReleaseDC(pdc);
	
	return pst_Des;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertRaw
(
	CView		*_po_View, 
	BIG_INDEX	_ul_FatFile,
	UCHAR		*_puc_Buf,
	UCHAR		*_puc_Palette,
	short		w_TgaWidth,
	short		w_TgaHeight,
	UCHAR		uc_BBP,
	int			_i_Format,
	BOOL		_b_Compress
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UCHAR						*p_RawBits, *p_RawBits1;
	ULONG						ul_Value;
	MAIEDITEX_tdst_BitmapDes	*pst_Des;
	CDC							*pdc;
	int							i, j, k;
	UINT						ui_Usage;
	BITMAPINFO					st_BitmapInfo;
	ULONG						*pul_Dest;
	UCHAR						*puc_Src;
	UCHAR						*puc_Buf1;
	LONG						l_WidthInByte;
	LONG						l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Bitmap info header */
	st_BitmapInfo.bmiHeader.biWidth = w_TgaWidth;
	st_BitmapInfo.bmiHeader.biHeight = w_TgaHeight;
	st_BitmapInfo.bmiHeader.biPlanes = 1;
	st_BitmapInfo.bmiHeader.biBitCount = uc_BBP;
	st_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	st_BitmapInfo.bmiHeader.biCompression = BI_RGB;
	st_BitmapInfo.bmiHeader.biSizeImage = 0;
	st_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	st_BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	st_BitmapInfo.bmiHeader.biClrUsed = 0;
	st_BitmapInfo.bmiHeader.biClrImportant = 0;

	ui_Usage = DIB_RGB_COLORS;

	/* New entry for list */
	pst_Des = new MAIEDITEX_tdst_BitmapDes;
	pst_Des->i_Type = _i_Format;
	pst_Des->st_Bmp.b_Compress = FALSE;

	/* Create bitmap */
	pdc = _po_View->GetDC();
	pst_Des->st_Bmp.h_Bitmap = CreateDIBSection(pdc->m_hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawBits, NULL, 0);
	_po_View->ReleaseDC(pdc);
	
	if(pst_Des->st_Bmp.h_Bitmap == NULL)
	{
		delete pst_Des;
		return NULL;
	}

	l_WidthInByte = (w_TgaWidth * uc_BBP) >> 3;
	l_Size = l_WidthInByte * w_TgaHeight;

	/* Convert palette to 32 bits */
	if(_puc_Palette)
	{
		puc_Src = _puc_Buf + 256 * 3;
		pul_Dest = (ULONG *) p_RawBits;

		for(i = 0; i < w_TgaWidth * w_TgaHeight; i++)
		{
			ul_Value = *puc_Src * 3;
			ul_Value = (_puc_Palette[ul_Value + 2] << 16) + (_puc_Palette[ul_Value + 1] << 8) + _puc_Palette[ul_Value];

			*pul_Dest = ul_Value;
			pul_Dest++;
			puc_Src++;
		}
	}

	/* Copy the tga raw data in the image's bits area */
	else
	{
		/* (CBitmap::FromHandle( pst_Des->h_Bitmap ))->GetBitmap( &st_Bmp ); */

		/*
		 * Special copy if bitmap is odd in width (add one dummy value at the end of the
		 * line)
		 */

		/* if(!_b_Compress && (w_TgaWidth & 3)) */
		if(!_b_Compress && (l_WidthInByte & 3))
		{
			p_RawBits1 = p_RawBits;
			puc_Buf1 = _puc_Buf;
			k = 4 - (l_WidthInByte & 3);
			if(k == 0)
			{
				for(i = 0; i < w_TgaHeight; i++)
				{
					for(j = 0; j < l_WidthInByte; j++) *p_RawBits1++ = *puc_Buf1++;
				}
			}
			else if(k == 1)
			{
				for(i = 0; i < w_TgaHeight; i++)
				{
					for(j = 0; j < l_WidthInByte; j++) *p_RawBits1++ = *puc_Buf1++;
					*p_RawBits1++ = 0;
				}
			}
			else if(k == 2)
			{
				for(i = 0; i < w_TgaHeight; i++)
				{
					for(j = 0; j < l_WidthInByte; j++) *p_RawBits1++ = *puc_Buf1++;
					*p_RawBits1++ = 0;
					*p_RawBits1++ = 0;
				}
			}
			else if(k == 3)
			{
				for(i = 0; i < w_TgaHeight; i++)
				{
					for(j = 0; j < l_WidthInByte; j++) *p_RawBits1++ = *puc_Buf1++;
					*p_RawBits1++ = 0;
					*p_RawBits1++ = 0;
					*p_RawBits1++ = 0;
				}
			}
		}
		else
		{
			if(_b_Compress)
			{
				TEX_l_File_TgaUncompress(uc_BBP, (char *) p_RawBits, (char *) _puc_Buf, l_Size);
				pst_Des->st_Bmp.b_Compress = TRUE;
			}
			else
			{
				L_memcpy(p_RawBits, _puc_Buf, l_Size);
			}
		}
	}

	pst_Des->st_Bmp.h_BitmapAlpha = NULL;
	pst_Des->ul_FatFile = _ul_FatFile;
	L_memcpy(&pst_Des->st_Bmp.st_Header, &st_BitmapInfo, sizeof(BITMAPINFOHEADER));
	if(_puc_Palette)
	{
		pst_Des->st_Bmp.st_Header.biBitCount = 8;
	}

	/* Alpha channel */
	if(uc_BBP == 32)
	{
		pdc = _po_View->GetDC();
		pst_Des->st_Bmp.h_BitmapAlpha = CreateDIBSection
			(
				pdc->m_hDC,
				&st_BitmapInfo,
				ui_Usage,
				(void **) &p_RawBits1,
				NULL,
				0
			);
		_po_View->ReleaseDC(pdc);

		for(i = 0; i < w_TgaHeight * w_TgaWidth; i++)
		{
			ul_Value = *((LONG *) p_RawBits);
			ul_Value &= 0xFF000000;
			ul_Value >>= 8;
			ul_Value |= ((ul_Value & 0x00FF0000) >> 8) | ((ul_Value & 0x00FF0000) >> 16);
			*((LONG *) p_RawBits1) = ul_Value;
			p_RawBits += 4;
			p_RawBits1 += 4;
		}
	}

	/* Add to list */
	return pst_Des;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertJPG(CView *_po_View, BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf, ULONG ulLenght)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc			st_Des;
	MAIEDITEX_tdst_BitmapDes	*pst_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
	st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	TEX_l_File_LoadJpeg((char *) _puc_Buf, &st_Des, ulLenght);
	pst_Des = TEXUtil_ConvertRaw
	(
		_po_View,
		_ul_FatFile,
		(UCHAR *) st_Des.p_Bitmap,
		(UCHAR *) st_Des.p_Palette,
		st_Des.uw_Width,
		st_Des.uw_Height,
		st_Des.uc_BPP,
		MAIEDITEX_C_JPG,
		0
	);
	MEM_Free(st_Des.p_Bitmap);
	return pst_Des;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertTGA(CView *_po_View, BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	short					w_TgaWidth, w_TgaHeight;
	UCHAR					uc_BBP;
	TEX_tdst_File_TgaHeader *pst_TGA;
	BOOL					b_Pal;
	UCHAR					*puc_Pal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_TGA = (TEX_tdst_File_TgaHeader *) _puc_Buf;
	b_Pal = FALSE;

	w_TgaWidth = (_puc_Buf[13] << 8) + _puc_Buf[12];
	w_TgaHeight = (_puc_Buf[15] << 8) + _puc_Buf[14];
	uc_BBP = _puc_Buf[16];
	if(uc_BBP < 8)
	{
		return NULL;
	}

	if(uc_BBP == 8)
	{
		uc_BBP = 32;
		b_Pal = TRUE;
	}

	/* Pass header */
	_puc_Buf += 18;

	/* Convert raw */
	if(b_Pal)
		puc_Pal = _puc_Buf;
	else
		puc_Pal = NULL;

	return TEXUtil_ConvertRaw
	(
		_po_View, 
		_ul_FatFile,
		_puc_Buf,
		puc_Pal,
		w_TgaWidth,
		w_TgaHeight,
		uc_BBP,
		MAIEDITEX_C_TGA,
		pst_TGA->uc_ImageTypeCode >= 9
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertPalette(CView *_po_View, BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Palette			st_Palette;
	ULONG						*pul_Buffer, *pul_Line, ul_Color;
	int							i, j, k;
	MAIEDITEX_tdst_BitmapDes	*pst_Des;
	ULONG						ul_Mask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    L_memset( &st_Palette, 0, sizeof( TEX_tdst_Palette ) );
	st_Palette.ul_Key = BIG_FileKey(_ul_FatFile);
	TEX_File_LoadPalette(&st_Palette, FALSE);
	if(st_Palette.uc_Flags & TEX_uc_InvalidPalette) return NULL;

	pul_Buffer = (ULONG *) MEM_p_Alloc(64 * 64 * 4);

	ul_Mask = 0xFFFFFF;
	if(st_Palette.uc_Flags & TEX_uc_AlphaPalette) ul_Mask |= 0xFF000000;

	if(st_Palette.uc_Flags & TEX_uc_Palette16)
	{
		for(j = 0; j < 64; j += 16)
		{
			pul_Line = pul_Buffer + (j << 6);
			for(i = 0; i < 64; i += 16)
			{
				ul_Color = st_Palette.pul_Color[(3 - (j >> 4)) * 4 + (i >> 4)] & ul_Mask;
				for(k = 0; k < 16; k++) pul_Line[i + k] = ul_Color;
			}

			for(k = 1; k < 16; k++) L_memcpy(pul_Line + (k << 6), pul_Line, 256);
		}
	}
	else
	{
		for(j = 0; j < 64; j += 4)
		{
			pul_Line = pul_Buffer + (j << 6);
			for(i = 0; i < 64; i += 4)
			{
				ul_Color = st_Palette.pul_Color[(15 - (j >> 2)) * 16 + (i >> 2)] & ul_Mask;
				pul_Line[i] = ul_Color;
				pul_Line[i + 1] = ul_Color;
				pul_Line[i + 2] = ul_Color;
				pul_Line[i + 3] = ul_Color;
			}

			for(k = 1; k < 4; k++) L_memcpy(pul_Line + (k << 6), pul_Line, 256);
		}
	}

	pst_Des = TEXUtil_ConvertRaw(_po_View, _ul_FatFile, (UCHAR *) pul_Buffer, NULL, 64, 64, 32, MAIEDITEX_C_PALETTE, 0);
	MEM_Free(pul_Buffer);
    TEX_M_File_Free( st_Palette.pul_Color );

	if (pst_Des)
		pst_Des->ul_PaletteFlags = st_Palette.uc_Flags;
	return pst_Des;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertRawFile(CView *_po_View, BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc		st_Des;
	MAIEDITEX_tdst_BitmapDes *pst_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
	st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	TEX_l_File_GetInfoAndContent(BIG_FileKey(_ul_FatFile), &st_Des);

	pst_Des = TEXUtil_ConvertRaw
	(
		_po_View, 
		_ul_FatFile,
		(UCHAR *) st_Des.p_Bitmap,
		(UCHAR *) st_Des.p_Palette,
		st_Des.uw_Width,
		st_Des.uw_Height,
		st_Des.uc_BPP,
		MAIEDITEX_C_RAW,
		0
	);
	MEM_Free(st_Des.p_Bitmap);
	return pst_Des;
}
#if defined(_XENON_RENDER)
MAIEDITEX_tdst_BitmapDes* TEXUtil_ConvertDDS(CView* _po_View, BIG_INDEX _ul_FatFile, UCHAR* _puc_Buf, ULONG _ul_Length)
{
    MAIEDITEX_tdst_BitmapDes* pst_Des = NULL;
    TEX_tdst_File_Desc        st_Des;

    L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
    st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content | TEX_Cuw_DF_ConvertTo32BPP;

    if (TEX_l_File_LoadDDS((char *) _puc_Buf, &st_Des, _ul_Length))
    {
        pst_Des = TEXUtil_ConvertRaw(_po_View, _ul_FatFile, (UCHAR*)st_Des.p_Bitmap,
                                     NULL, st_Des.uw_Width, st_Des.uw_Height,
                                     32, MAIEDITEX_C_DDS, 0);
        MEM_Free(st_Des.p_Bitmap);
    }

    return pst_Des;
}

static void Xe_GenerateDds( BIG_INDEX ulDir, BIG_INDEX ulFile )
{
	CHAR  szNewFileName[L_MAX_PATH];
    CHAR  szDirName[L_MAX_PATH];
	CHAR* pcPos;
	TEX_tdst_File_Desc st_TexDesc;
	TEX_tdst_File_Tex  st_Tex;

	// Create name for new DDS file
	strcpy(szNewFileName, BIG_NameFile(ulFile));
	pcPos = strrchr(szNewFileName, '.');
	if (pcPos != NULL)
		*pcPos = '\0';
	strcat(szNewFileName, "_Xenon.dds");

	// Make sure the file does not exist, or prompt for overwrite
//	ulDdsFileIndex = BIG_ul_SearchFile(ulFile, szNewFileName);
//	if (ulDdsFileIndex != BIG_C_InvalidIndex)

	BIG_ComputeFullName(ulDir, szDirName);

	/* get Tex file content */
	L_memset(&st_TexDesc, 0, sizeof(TEX_tdst_File_Desc));
	TEX_l_File_GetInfoAndContent( BIG_FileKey( ulFile ), &st_TexDesc );
	L_memcpy( &st_Tex, &st_TexDesc.st_Tex, sizeof( TEX_tdst_File_Tex ) );

	// Convert 
	if (TEX_XeGenerateDDS(&st_Tex.st_XeProperties, szDirName, szNewFileName))
	{
		TEX_ul_File_SaveTexWithIndex(ulFile, &st_Tex);
	}
}

typedef std::pair<BIG_INDEX, BIG_INDEX> t_ulPair;
static void Xe_BuildTexList( std::vector<t_ulPair>& texList, BIG_INDEX ulDir )
{
	// process files in this folder
	BIG_INDEX ulFile = BIG_FirstFile( ulDir );
	while( ulFile != BIG_C_InvalidIndex )
	{
		if (BIG_b_IsFileExtensionIn(ulFile, ".tex"))
		{
			texList.push_back(t_ulPair(ulDir, ulFile));
		}
		ulFile = BIG_NextFile( ulFile );
	}

	// recursively process sub-folders
	BIG_INDEX ulSubDir = BIG_SubDir(ulDir);
	while(ulSubDir != BIG_C_InvalidIndex)
	{
		Xe_BuildTexList( texList, ulSubDir );
		ulSubDir = BIG_NextDir(ulSubDir);
	}
}

void Xe_GenerateAllDds()
{
	static EDIA_cl_UPDATEDialog* mpo_Progress;
	std::vector<t_ulPair>		 texList;
	std::vector<t_ulPair>::iterator it;
	ULONG pos;

	mpo_Progress = new EDIA_cl_UPDATEDialog("Generating DDS...");
	mpo_Progress->DoModeless();

	Xe_BuildTexList(texList, BIG_Root());

	for (it = texList.begin(), pos = 0; 
		 it != texList.end(); 
		 it++, pos++)
	{	
		BIG_INDEX ulDir, ulFile;
		char str[MAX_PATH];

		t_ulPair aPair = *it;
		ulDir  = aPair.first;
		ulFile = aPair.second;

		Xe_GenerateDds(ulDir, ulFile);
		sprintf(str, "Generating DDS for %s...", BIG_NameFile(ulFile));
		mpo_Progress->OnRefreshBarText((float)pos/(float)texList.size(), str);
	}

	delete mpo_Progress;
}
#endif

MAIEDITEX_tdst_BitmapDes *TEXUtil_ConvertStbiFile( CView *view, BIG_INDEX fatFile, uint8_t *buffer, unsigned int length, int format )
{
	TEX_tdst_File_Desc desc{};
	desc.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	TEX_File_LoadStbiFile( buffer, length, &desc );

	MAIEDITEX_tdst_BitmapDes *pDesc = TEXUtil_ConvertRaw(
	        view,
	        fatFile,
	        ( uint8_t * ) desc.p_Bitmap,
	        ( uint8_t * ) desc.p_Palette,
	        desc.uw_Width,
	        desc.uw_Height,
	        desc.uc_BPP,
	        format,
	        false );

	MEM_Free( desc.p_Bitmap );

	return pDesc;
}

#endif /* ACTIVE_EDITORS */
