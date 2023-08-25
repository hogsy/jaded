/*$T TEXfile.c GC! 1.081 10/11/00 15:30:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "INOut/INOfile.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXanimated.h"
#include "TEXture/TEXfile.h"
#ifdef JADEFUSION
#include "TEXture/TEXxenon.h"
#endif
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "../../Dlls/JPEGLIB/INTERFCE.H"


#ifdef ACTIVE_EDITORS
#include "BIGfiles/IMPort/IMPbase.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "EDIpaths.h"
#endif
#include "GDInterface/GDInterface.h"
#include "MATerial/MATSprite.h"
#include "STRing/STRstruct.h"

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeGDInterface.h"
#include "XenonGraphics/XeRenderer.h"
#endif

#if defined(_XBOX) || defined(_XENON)
typedef struct	tagBITMAPINFOHEADER
{
	DWORD	biSize : 32;
	LONG	biWidth : 32;
	LONG	biHeight : 32;
	WORD	biPlanes : 16;
	WORD	biBitCount : 16;
	DWORD	biCompression : 32;
	DWORD	biSizeImage : 32;
	LONG	biXPelsPerMeter : 32;
	LONG	biYPelsPerMeter : 32;
	DWORD	biClrUsed : 32;
	DWORD	biClrImportant : 32;
} BITMAPINFOHEADER;

typedef struct	tagBITMAPFILEHEADER
{
	WORD	bfType : 16;
	DWORD	bfSize : 32;
	WORD	bfReserved1 : 16;
	WORD	bfReserved2 : 16;
	DWORD	bfOffBits : 32;
} BITMAPFILEHEADER;
#endif
#if (defined(PSX2_TARGET) && defined(__cplusplus))
extern "C"
{
#endif
#if (defined(PSX2_TARGET) || defined(_GAMECUBE))
#ifdef __CW__

/* CodeWarrior encoding form */
#pragma pack(1)
typedef struct	tagBITMAPINFOHEADER
{
	DWORD	biSize : 32;
	LONG	biWidth : 32;
	LONG	biHeight : 32;
	WORD	biPlanes : 16;
	WORD	biBitCount : 16;
	DWORD	biCompression : 32;
	DWORD	biSizeImage : 32;
	LONG	biXPelsPerMeter : 32;
	LONG	biYPelsPerMeter : 32;
	DWORD	biClrUsed : 32;
	DWORD	biClrImportant : 32;
} BITMAPINFOHEADER;

typedef struct	tagBITMAPFILEHEADER
{
	WORD	bfType : 16;
	DWORD	bfSize : 32;
	WORD	bfReserved1 : 16;
	WORD	bfReserved2 : 16;
	DWORD	bfOffBits : 32;
} BITMAPFILEHEADER;
#pragma pack(0)
#else

/*
 -----------------------------------------------------------------------------------------------------------------------
    GNU encoding form
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	tagBITMAPINFOHEADER
{
	DWORD biSize			__attribute__((packed));
	LONG biWidth			__attribute__((packed));
	LONG biHeight			__attribute__((packed));
	WORD biPlanes			__attribute__((packed));
	WORD biBitCount			__attribute__((packed));
	DWORD biCompression		__attribute__((packed));
	DWORD biSizeImage		__attribute__((packed));
	LONG biXPelsPerMeter	__attribute__((packed));
	LONG biYPelsPerMeter	__attribute__((packed));
	DWORD biClrUsed			__attribute__((packed));
	DWORD biClrImportant	__attribute__((packed));
}
__attribute__((packed))
BITMAPINFOHEADER;

typedef struct	tagBITMAPFILEHEADER
{
	WORD bfType			__attribute__((packed));
	DWORD bfSize		__attribute__((packed));
	WORD bfReserved1	__attribute__((packed));
	WORD bfReserved2	__attribute__((packed));
	DWORD bfOffBits		__attribute__((packed));
}
__attribute__((packed))
BITMAPFILEHEADER;
#endif /* __CW__ */
#endif /* PSX2_TARGET */

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
char	*TEX_gasz_QualityName[5] = { "Very low", "Low", "Medium", "High", "Very high" };

#ifdef JADEFUSION
extern int  GDI_gi_GDIType; 

extern BOOL EDI_gb_ComputeMap;
#endif
#endif


/*$4
 ***********************************************************************************************************************
    palette functions
 ***********************************************************************************************************************
 */
extern void BIG_ReadNoSeek(ULONG _ul_Pos, void *_p_Buffer, ULONG _ul_Length);


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_LoadPalette( TEX_tdst_Palette *_pst_Palette, BOOL _b_SwapRB )
{
    ULONG   ul_Key, ul_Pos, ul_Size, ul_BufSize;
    ULONG   *pul_32;
    UCHAR   *puc_24;
	CHAR	*pc_Buffer;

	
	ul_Key = _pst_Palette->ul_Key;
    ul_Pos = BIG_ul_SearchKeyToPos( ul_Key );
	if(ul_Pos == (ULONG) - 1) return;

	L_memset( _pst_Palette, 0, sizeof( TEX_tdst_Palette ) );
	_pst_Palette->ul_Key = ul_Key;

#ifdef ACTIVE_EDITORS
	ul_Size = BIG_ul_SearchKeyToFat( _pst_Palette->ul_Key );
    if(ul_Size != BIG_C_InvalidIndex) BIG_FileChanged(ul_Size) |= EDI_FHC_Loaded;
#endif

	pc_Buffer = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);

    if (ul_Size == 1024)
    {
        _pst_Palette->uc_Flags |= TEX_uc_AlphaPalette;
        ul_BufSize = 1024;
    }
    else if (ul_Size == 768)
    {
        ul_BufSize = 1024;
    }
    else if (ul_Size == 64)
    {
        _pst_Palette->uc_Flags |= TEX_uc_Palette16 | TEX_uc_AlphaPalette;
        ul_BufSize = 64;
    }
    else if (ul_Size == 48)
    {
        _pst_Palette->uc_Flags |= TEX_uc_Palette16;
        ul_BufSize = 64;
    }
    else
    {
        _pst_Palette->uc_Flags |= TEX_uc_InvalidPalette;
        return;
    }

    TEX_M_File_AllocTmp(_pst_Palette->pul_Color, ul_BufSize, ULONG);     

	LOA_ReadCharArray(&pc_Buffer, (CHAR*) _pst_Palette->pul_Color, ul_Size);

    if (_pst_Palette->uc_Flags & TEX_uc_AlphaPalette)
    {
        if (_b_SwapRB)
        {
            pul_32 = _pst_Palette->pul_Color + (ul_BufSize >> 2);
            while (ul_Size)
            {
                pul_32--;
                puc_24 = (UCHAR *) pul_32;
                ul_Size -= 4;

                *pul_32 = (puc_24[0]<< 16) | (puc_24[1]<< 8) | (puc_24[2]) | (puc_24[3] << 24);
            }
        }
#if defined(_XENON)
	else
	{
            pul_32 = _pst_Palette->pul_Color + (ul_BufSize >> 2);
            while (ul_Size)
            {
                pul_32--;
                puc_24 = (UCHAR *) pul_32;
                ul_Size -= 4;

                *pul_32 = (puc_24[0]) | (puc_24[1]<< 8) | (puc_24[2]<<16) | (puc_24[3] << 24);
            }
	}
#endif
	}
    else
    {   // expand the palette by filling in the missing alpha
		// Start from the end of the palette (last 25% of the buffer is empty because of the missing alpha)
        pul_32 = _pst_Palette->pul_Color + (ul_BufSize >> 2);
        puc_24 = ((UCHAR *) _pst_Palette->pul_Color) + ul_Size;

        if (_b_SwapRB)
        {
            while (ul_Size)
            {
                pul_32--;
                puc_24 -= 3;
                ul_Size -= 3;

                *pul_32 = 0xFF000000 | (puc_24[0]<< 16) | (puc_24[1]<< 8) | (puc_24[2]);
            }
        }
        else
        {
            while (ul_Size)
            {
                pul_32--;
                puc_24 -= 3;
                ul_Size -= 3;

                *pul_32 = 0xFF000000 | (puc_24[0]) | (puc_24[1]<< 8) | (puc_24[2]<<16);
            }
        }
    }
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_SaveNewPaletteBF( TEX_tdst_Palette *_pst_Pal, char *_sz_Path, char *_sz_Name, BOOL _b_SwapRB )
{
    char    *pc_Buffer, *pc_Cur, *pc_Src;
    LONG    l_Size;
    int     i, i_BufferAlloc;

    l_Size = (_pst_Pal->uc_Flags & TEX_uc_Palette16) ? 16 : 256;
    i_BufferAlloc = 0;
    pc_Buffer = (char *) _pst_Pal->pul_Color;
    
    if (_pst_Pal->uc_Flags & TEX_uc_AlphaPalette)
    {
        if (_b_SwapRB)
        {
            pc_Src = pc_Buffer;
            pc_Cur = pc_Buffer = (char*)L_malloc( 1024 );
            i_BufferAlloc = 1;
            pc_Src = (char *) _pst_Pal->pul_Color;
            for (i = 0; i < l_Size; i++, pc_Src += 4)
            {
                *pc_Cur++ = pc_Src[2];
                *pc_Cur++ = pc_Src[1];
                *pc_Cur++ = pc_Src[0];
                *pc_Cur++ = pc_Src[3];
            }
            //BIG_UpdateFileFromBuffer(_sz_Path, _sz_Name, pc_Buffer, l_Size * 4);
            //L_free( pc_Buffer );
        }
        else
        {
            //pc_Buffer = (char *) _pst_Pal->pul_Color;
            //BIG_UpdateFileFromBuffer(_sz_Path, _sz_Name, pc_Buffer, l_Size * 4);
        }
        l_Size *= 4;
    }
    else
    {
        i_BufferAlloc = 1;
        pc_Src = pc_Buffer;
        pc_Cur = pc_Buffer = (char*)L_malloc( 1024 );

        if (_b_SwapRB)
        {
            for (i = 0; i < l_Size; i++, pc_Src += 4)
            {
                *pc_Cur++ = pc_Src[2];
                *pc_Cur++ = pc_Src[1];
                *pc_Cur++ = pc_Src[0];
            }
        }
        else
        {
            for (i = 0; i < l_Size; i++)
            {
                *pc_Cur++ = *pc_Src++;
                *pc_Cur++ = *pc_Src++;
                *pc_Cur++ = *pc_Src++;
                pc_Src++;
            }
        }
        l_Size *= 3;
        //BIG_UpdateFileFromBuffer(_sz_Path, _sz_Name, pc_Buffer, l_Size * 3);
        //L_free( pc_Buffer );
    }

    SAV_Begin(_sz_Path, _sz_Name);
	SAV_Buffer(pc_Buffer, l_Size);
    if (i_BufferAlloc) L_free( pc_Buffer );
	return SAV_ul_End();
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_SavePaletteBF( TEX_tdst_Palette *_pst_Pal, BOOL _b_SwapRB )
{
    ULONG   ul_Index;
    char    *sz_Name, sz_Path[ BIG_C_MaxLenPath ];

    if ( _pst_Pal->ul_Key == BIG_C_InvalidKey) return BIG_C_InvalidIndex;
    ul_Index = BIG_ul_SearchKeyToFat( _pst_Pal->ul_Key );
    if ( ul_Index == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;
    sz_Name = BIG_NameFile( ul_Index );
    BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Path );
    return TEX_ul_File_SaveNewPaletteBF( _pst_Pal, sz_Path, sz_Name, _b_SwapRB );
}
#endif

/*$4
 ***********************************************************************************************************************
    JPEG format
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_VSwap(char *dst, char *src, ULONG ulW, ULONG ulL, ULONG ulBpp)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	ulLineLenght;
	/*~~~~~~~~~~~~~~~~~*/

	ulLineLenght = (ulW * ulBpp);
	src += ulLineLenght * ulL - ulLineLenght;
	while(ulL--)
	{
		L_memcpy(dst, src, ulLineLenght);
		dst += ulLineLenght;
		src -= ulLineLenght;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_LoadJpeg(char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc, ULONG ulFileLenght)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	JPEGDATA		stJI;
	ULONG			ul_Length;
	unsigned char	*ucVSwap;
	unsigned char	*ucPermut, *ucPermutLast, ucSwap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&stJI, 0, sizeof(stJI));

	if(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Info)
	{

		lJADE_JPEG_INPOUT_FILE_LENGHT = ulFileLenght;
		ucJADE_JPEG_INPOUT_FILE = (unsigned char *) _pc_Buffer;
		JpegInfo(&stJI);

		_pst_TexDesc->uc_PaletteBPC = 0;
		_pst_TexDesc->uw_PaletteLength = 0;
		_pst_TexDesc->uw_Width = stJI.width;
		_pst_TexDesc->uw_Height = stJI.height;
		_pst_TexDesc->uc_BPP = _pst_TexDesc->uc_FinalBPP = stJI.components << 3;

		/* _pst_TexDesc->uc_FinalBPP = _pst_TexDesc->uc_BPP; */
		_pst_TexDesc->ul_RMask = 0xFF;
		_pst_TexDesc->ul_GMask = 0xFF00;
		_pst_TexDesc->ul_BMask = 0xFF0000;
		_pst_TexDesc->ul_AMask = 0;

		if(_pst_TexDesc->st_Params.uw_Flags & TEX_FP_MipmapOn) _pst_TexDesc->uw_FileFlags |= TEX_uw_Mipmap;

		/*$F
        if(_pst_TexDesc->uc_BPP == 8)
			_pst_TexDesc->uc_FinalBPP = 24;
		else if(_pst_TexDesc->uc_BPP != 24)
			return 0;
        */
	}

	if((_pst_TexDesc->uc_BPP != 8) && (_pst_TexDesc->uc_BPP != 24)) return 0;

	if(!(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Content)) return 1;

	if((_pst_TexDesc->uc_BPP != 8) && (_pst_TexDesc->uc_BPP != 24)) return 0;

	ul_Length = ((ULONG) _pst_TexDesc->uw_Width * _pst_TexDesc->uw_Height);

	/* ul_Length *= _pst_TexDesc->uc_FinalBPP >> 3; */
	ul_Length *= 3;
	if(!ul_Length) return 0;

	TEX_M_File_Alloc(_pst_TexDesc->p_Bitmap, ul_Length, void);
	stJI.ptr = ucVSwap = (unsigned char *) MEM_p_Alloc(ul_Length);

	lJADE_JPEG_INPOUT_FILE_LENGHT = ulFileLenght;
	ucJADE_JPEG_INPOUT_FILE = (unsigned char *) _pc_Buffer;
	JpegRead(&stJI);

	if(_pst_TexDesc->uc_BPP == 8)
	{
		TEX_File_VSwap
		(
			(char *) _pst_TexDesc->p_Bitmap,
			(char *) ucVSwap,
			_pst_TexDesc->uw_Width,
			_pst_TexDesc->uw_Height,
			1
		);
		ucPermut = (unsigned char *) ((ULONG) _pst_TexDesc->p_Bitmap + ul_Length - (ULONG) 3);
		ucPermutLast = (unsigned char *) ((ULONG) _pst_TexDesc->p_Bitmap + (_pst_TexDesc->uw_Width * _pst_TexDesc->uw_Height) - (LONG) 1);
		while(ucPermut >= ucPermutLast)
		{
			*(ucPermut) = *(ucPermutLast);
			*(ucPermut + 1) = *(ucPermutLast);
			*(ucPermut + 2) = *(ucPermutLast);
			ucPermut -= 3;
			ucPermutLast--;
		}
	}
	else if(_pst_TexDesc->uc_BPP == 24)
	{
		ucPermut = ucVSwap;
		ucPermutLast = ucPermut + ul_Length;
		while(ucPermut < ucPermutLast)
		{
			ucSwap = *(ucPermut);
			*(ucPermut) = *(ucPermut + 2);
			*(ucPermut + 2) = ucSwap;
			ucPermut += 3;
		}

		TEX_File_VSwap
		(
			(char *) _pst_TexDesc->p_Bitmap,
			(char *) ucVSwap,
			_pst_TexDesc->uw_Width,
			_pst_TexDesc->uw_Height,
			3
		);
	}

	MEM_Free(ucVSwap);
	return 1;
}

/*$4
 ***********************************************************************************************************************
    TGA format
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
#ifdef __CW__

/* CodeWarrior encoding form */
#pragma pack(1)
typedef struct
{
	u_long64	uc_Size : 8;
	u_long64	uc_ColorMapType : 8;
	u_long64	uc_ImageTypeCode : 8;
	u_long64	uw_Origin : 16;
	u_long64	uw_PaletteLength : 16;
	u_long64	uc_BPCInPalette : 8;
	u_long64	uw_Left : 16;
	u_long64	uw_Top : 16;
	u_long64	uw_Width : 16;
	u_long64	uw_Height : 16;
	u_long64	uc_BPP : 8;
	u_long64	ucDescriptorByte : 8;
} TEX_tdst_File_TgaHeader;
#pragma pack(0)
#else

/*
 -----------------------------------------------------------------------------------------------------------------------
    GNU encoding form
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
	unsigned char uc_Size			__attribute__((packed));
	unsigned char uc_ColorMapType	__attribute__((packed));
	unsigned char uc_ImageTypeCode	__attribute__((packed));
	unsigned short uw_Origin		__attribute__((packed));
	unsigned short uw_PaletteLength __attribute__((packed));
	unsigned char uc_BPCInPalette	__attribute__((packed));
	unsigned short uw_Left			__attribute__((packed));
	unsigned short uw_Top			__attribute__((packed));
	unsigned short uw_Width			__attribute__((packed));
	unsigned short uw_Height		__attribute__((packed));
	unsigned char uc_BPP			__attribute__((packed));
	unsigned char ucDescriptorByte	__attribute__((packed));
}
__attribute__((packed))
TEX_tdst_File_TgaHeader;
#endif /* __CW__ */

#else /* PSX2_TARGET */

/* MSVC encoding form */
#pragma pack(push, 1)
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
#pragma pack(pop)
#endif /* PSX2_TARGET */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_l_File_TgaUncompress(char uc_BPP, char *pc_Dest, char *_pc_Buffer, ULONG ul_L)
{
	/*~~~~~~~~~~~*/
	char	uc_Num;
	ULONG	ul_Val;
	/*~~~~~~~~~~~*/

	while((int) ul_L > 0)
	{
		uc_Num = (*_pc_Buffer & 0x7F) + 1;

		if(*_pc_Buffer & 0x80)
		{
			_pc_Buffer++;
			ul_Val = *(int *) _pc_Buffer;
			if(uc_BPP == 24)
				_pc_Buffer += 3;
			else
				_pc_Buffer += 4;
			while(uc_Num--)
			{
				if(uc_BPP == 24)
				{
					*(pc_Dest++) = (char) (ul_Val);
					*(pc_Dest++) = (char) (ul_Val >> 8);
					*(pc_Dest++) = (char) (ul_Val >> 16);
					ul_L -= 3;
				}
				else
				{
					*(int *) pc_Dest = ul_Val;
					ul_L -= 4;
				}
			}
		}
		else
		{
			_pc_Buffer++;
			while(uc_Num--)
			{
				if(uc_BPP == 24)
				{
					*(pc_Dest++) = *(_pc_Buffer++);
					*(pc_Dest++) = *(_pc_Buffer++);
					*(pc_Dest++) = *(_pc_Buffer++);
					ul_L -= 3;
				}
				else
				{
					*(int *) pc_Dest = *(int *) _pc_Buffer;
					_pc_Buffer += 4;
					pc_Dest += 4;
					ul_L -= 4;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Read bitmap content of a TGA file
 =======================================================================================================================
 */
LONG TEX_l_File_LoadTga(char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_TgaHeader st_TgaHeader;
	ULONG					ul_Length;
	LONG					l_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Result = 0;

	st_TgaHeader.uc_Size = LOA_ReadUChar(&_pc_Buffer);
	st_TgaHeader.uc_ColorMapType = LOA_ReadUChar(&_pc_Buffer);
	st_TgaHeader.uc_ImageTypeCode = LOA_ReadUChar(&_pc_Buffer);
	st_TgaHeader.uw_Origin = LOA_ReadUShort(&_pc_Buffer);
	st_TgaHeader.uw_PaletteLength = LOA_ReadUShort(&_pc_Buffer);
	st_TgaHeader.uc_BPCInPalette = LOA_ReadUChar(&_pc_Buffer);
	st_TgaHeader.uw_Left = LOA_ReadUShort(&_pc_Buffer);
	st_TgaHeader.uw_Top = LOA_ReadUShort(&_pc_Buffer);
	st_TgaHeader.uw_Width = LOA_ReadUShort(&_pc_Buffer);
	st_TgaHeader.uw_Height = LOA_ReadUShort(&_pc_Buffer);
	st_TgaHeader.uc_BPP = LOA_ReadUChar(&_pc_Buffer);
	st_TgaHeader.ucDescriptorByte = LOA_ReadUChar(&_pc_Buffer);

	/* Copy bitmap parameters */
	if(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Info)
	{
		_pst_TexDesc->uw_Width = st_TgaHeader.uw_Width;
		_pst_TexDesc->uw_Height = st_TgaHeader.uw_Height;
		_pst_TexDesc->uw_PaletteLength = st_TgaHeader.uw_PaletteLength;
		_pst_TexDesc->uc_BPP = _pst_TexDesc->uc_FinalBPP = st_TgaHeader.uc_BPP;
		_pst_TexDesc->uc_PaletteBPC = st_TgaHeader.uc_BPCInPalette;
		// p_TmpBitmap and p_TmpPalette are used only in editor mode to convert to raw/pal
		_pst_TexDesc->p_TmpBitmap = _pc_Buffer;
		_pst_TexDesc->p_TmpPalette = NULL;

		if(_pst_TexDesc->st_Params.uw_Flags & TEX_FP_MipmapOn) _pst_TexDesc->uw_FileFlags |= TEX_uw_Mipmap;

		if(_pst_TexDesc->uc_BPP == 32)
		{
			_pst_TexDesc->ul_AMask = 0xFF000000;
			_pst_TexDesc->ul_RMask = 0xFF0000;
			_pst_TexDesc->ul_GMask = 0xFF00;
			_pst_TexDesc->ul_BMask = 0xFF;
		}
		else if(_pst_TexDesc->uc_BPP == 24)
		{
			_pst_TexDesc->ul_RMask = 0xFF0000;
			_pst_TexDesc->ul_GMask = 0xFF00;
			_pst_TexDesc->ul_BMask = 0xFF;
			_pst_TexDesc->ul_AMask = 0;
		}
		else if(_pst_TexDesc->uc_BPP == 16)
		{
			_pst_TexDesc->ul_AMask = 0;
			_pst_TexDesc->ul_RMask = 0xFF0000;
			_pst_TexDesc->ul_GMask = 0xFF00;
			_pst_TexDesc->ul_BMask = 0xFF;
		}
		else if(_pst_TexDesc->uc_BPP == 8)
		{
			_pst_TexDesc->p_TmpPalette = _pc_Buffer;
			_pst_TexDesc->p_TmpBitmap = _pc_Buffer + st_TgaHeader.uw_PaletteLength * (_pst_TexDesc->uc_PaletteBPC >> 3);
		}
		else
			return 0;
	}

	if(!(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Content)) return 1;

	ul_Length = ((ULONG) _pst_TexDesc->uw_Width * _pst_TexDesc->uw_Height);
	if(!ul_Length) return 0;

	LOA_ReadCharArray_Ed(&_pc_Buffer, NULL, st_TgaHeader.uw_PaletteLength * (_pst_TexDesc->uc_PaletteBPC >> 3)); // skip the palette, it is not needed right now

	TEX_M_File_AllocTmp(_pst_TexDesc->p_Bitmap, ul_Length * (_pst_TexDesc->uc_FinalBPP >> 3), void);    

	ul_Length *= _pst_TexDesc->uc_BPP >> 3;

	/* Compress version */
	if(st_TgaHeader.uc_ImageTypeCode >= 9)
	{
		TEX_l_File_TgaUncompress(st_TgaHeader.uc_BPP, (char *) _pst_TexDesc->p_Bitmap, _pc_Buffer, ul_Length);
		LOA_ReadCharArray(&_pc_Buffer, NULL, ul_Length); // read the compressed bitmap so it will be written to disk
	}

	/* Uncompress version */
	else
	{
		LOA_ReadCharArray(&_pc_Buffer, (CHAR*)_pst_TexDesc->p_Bitmap, ul_Length); // read the bitmap
#if defined(_XENON)
		if (st_TgaHeader.uc_BPP == 32)
		{
			FlipEndian_32(_pst_TexDesc->p_Bitmap, ul_Length);
		}
#endif
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_SaveTga(char *_psz_FileName, TEX_tdst_File_Desc *_pst_TexDesc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if defined(_XBOX) || defined(_XENON)
	HANDLE i_File;
#else // _XBOX
	L_FILE						i_File;
#endif // _XBOX
	TEX_tdst_File_TgaHeader st_TgaHeader;
	LONG					l_Write;
	ULONG					ul_Length;
	LONG					l_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Result = 0;

#if defined(_XBOX) || defined(_XENON)
	i_File = CreateFile (_psz_FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
#else // _XBOX
	i_File = L_fopen(_psz_FileName, L_fopen_WB);
	if(!i_File) return 0;
#endif // _XBOX

	/* Fill tga header with graphic data description */
	st_TgaHeader.uc_Size = 0;
    if((_pst_TexDesc->uw_PaletteLength) && (_pst_TexDesc->p_Palette))
    {
        st_TgaHeader.uc_ColorMapType = 1;
	    st_TgaHeader.uc_ImageTypeCode = 1;
    }
    else
    {
    	st_TgaHeader.uc_ColorMapType = 0;
    	st_TgaHeader.uc_ImageTypeCode = 2;
    }
	st_TgaHeader.uw_Origin = 0;
	st_TgaHeader.uw_PaletteLength = _pst_TexDesc->uw_PaletteLength;
	st_TgaHeader.uc_BPCInPalette = _pst_TexDesc->uc_PaletteBPC;
	st_TgaHeader.uw_Left = 0;
	st_TgaHeader.uw_Top = 0;
	st_TgaHeader.uw_Width = _pst_TexDesc->uw_Width;
	st_TgaHeader.uw_Height = _pst_TexDesc->uw_Height;
	st_TgaHeader.uc_BPP = _pst_TexDesc->uc_BPP;
	st_TgaHeader.ucDescriptorByte = 0;

	l_Write = L_fwriteA((void *) &st_TgaHeader, sizeof(TEX_tdst_File_TgaHeader), 1, i_File);
#if !defined(_XBOX) && !defined(_XENON)
	if(l_Write != 0)
#endif // _XBOX
	{
		/* Write eventually palette */
		if((st_TgaHeader.uw_PaletteLength) && (_pst_TexDesc->p_Palette))
        {
            ul_Length = (st_TgaHeader.uw_PaletteLength * st_TgaHeader.uc_BPCInPalette) >> 3;
			L_fwriteA((void*)_pst_TexDesc->p_Palette, ul_Length, 1, i_File);
        }

		/* Compute bitmap length and write it */
		if(_pst_TexDesc->p_Bitmap)
		{
			ul_Length = (_pst_TexDesc->uw_Width * _pst_TexDesc->uc_BPP) >> 3;
			ul_Length = (ul_Length + 3) &~3;
			ul_Length *= _pst_TexDesc->uw_Height;

			L_fwriteA((void*)_pst_TexDesc->p_Bitmap, ul_Length, 1, i_File);
		}

		l_Result = 1;
	}

	L_fclose(i_File);
	return l_Result;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_SaveTgaInBF(char *_sz_Path, char *_sz_Name, TEX_tdst_File_Desc *_pst_TexDesc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_TgaHeader st_TgaHeader;
	ULONG					ul_Length;
	LONG					l_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Result = 0;

    SAV_Begin( _sz_Path, _sz_Name );

	/* Fill tga header with graphic data description */
	st_TgaHeader.uc_Size = 0;
	if((_pst_TexDesc->uw_PaletteLength) && (_pst_TexDesc->p_Palette))
    {
        st_TgaHeader.uc_ColorMapType = 1;
	    st_TgaHeader.uc_ImageTypeCode = 1;
    }
    else
    {
    	st_TgaHeader.uc_ColorMapType = 0;
    	st_TgaHeader.uc_ImageTypeCode = 2;
    }
	st_TgaHeader.uw_Origin = 0;
	st_TgaHeader.uw_PaletteLength = _pst_TexDesc->uw_PaletteLength;
	st_TgaHeader.uc_BPCInPalette = _pst_TexDesc->uc_PaletteBPC;
	st_TgaHeader.uw_Left = 0;
	st_TgaHeader.uw_Top = 0;
	st_TgaHeader.uw_Width = _pst_TexDesc->uw_Width;
	st_TgaHeader.uw_Height = _pst_TexDesc->uw_Height;
	st_TgaHeader.uc_BPP = _pst_TexDesc->uc_BPP;
	st_TgaHeader.ucDescriptorByte = 8;

    SAV_Buffer( &st_TgaHeader, sizeof(TEX_tdst_File_TgaHeader));
	
    /* Write eventually palette */
    if((st_TgaHeader.uw_PaletteLength) && (_pst_TexDesc->p_Palette))
    {
        ul_Length = (st_TgaHeader.uw_PaletteLength * st_TgaHeader.uc_BPCInPalette) >> 3;
        SAV_Buffer( (char*) _pst_TexDesc->p_Palette, ul_Length);
    }

    /* Compute bitmap length and write it */
	if(_pst_TexDesc->p_Bitmap)
	{
	    ul_Length = (_pst_TexDesc->uw_Width * _pst_TexDesc->uc_BPP) >> 3;
		ul_Length = (ul_Length + 3) &~3;
		ul_Length *= _pst_TexDesc->uw_Height;

		SAV_Buffer( (char*)_pst_TexDesc->p_Bitmap, ul_Length);
    }


	TEX_File_InitParams(&_pst_TexDesc->st_Params);
	_pst_TexDesc->st_Params.uc_Type = TEX_FP_TgaFile;
	_pst_TexDesc->st_Params.uc_Format = (_pst_TexDesc->uc_BPP == 24) ? TEX_FP_24bpp : TEX_FP_32bpp;
    _pst_TexDesc->st_Params.uw_Width = _pst_TexDesc->uw_Width;
    _pst_TexDesc->st_Params.uw_Height = _pst_TexDesc->uw_Height;
	
    SAV_Buffer( &_pst_TexDesc->st_Params, 32 );

	return SAV_ul_End();
}

#endif

/*$4
 ***********************************************************************************************************************
    BMP file
 ***********************************************************************************************************************
 */

#if 0
#pragma pack(push, 1)
typedef struct	tagBITMAPFILEHEADER
{
	short	bfType;
	LONG	bfSize;
	short	bfReserved1;
	short	bfReserved2;
	LONG	bfOffBits;
} BITMAPFILEHEADER;
typedef struct	tagBITMAPINFOHEADER
{
	LONG	biSize;
	LONG	biWidth;
	LONG	biHeight;
	short	biPlanes;
	short	biBitCount;
	LONG	biCompression;
	LONG	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	LONG	biClrUsed;
	LONG	biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_LoadBmp(char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Length;
	LONG				l_Result;
	BITMAPINFOHEADER	*stBmInfoHeader;
	BITMAPFILEHEADER	*stBmFileHeader;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Result = 0;
	stBmFileHeader = (BITMAPFILEHEADER *) _pc_Buffer;
	stBmInfoHeader = (BITMAPINFOHEADER *) (_pc_Buffer + sizeof(BITMAPFILEHEADER));
	_pc_Buffer += sizeof(*stBmFileHeader) + sizeof(*stBmInfoHeader);

	/* Copy bitmap parameters */
	if(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Info)
	{
		_pst_TexDesc->uw_Width = (unsigned short) stBmInfoHeader->biWidth;
		_pst_TexDesc->uw_Height = (unsigned short) stBmInfoHeader->biHeight;
		_pst_TexDesc->uc_BPP = _pst_TexDesc->uc_FinalBPP = (unsigned char) stBmInfoHeader->biBitCount;

		/* _pst_TexDesc->uc_FinalBPP = _pst_TexDesc->uc_BPP; */
		if(_pst_TexDesc->st_Params.uw_Flags & TEX_FP_MipmapOn) _pst_TexDesc->uw_FileFlags |= TEX_uw_Mipmap;

		/* Get bitmap */
		if(_pst_TexDesc->uc_BPP == 32)
		{
			_pst_TexDesc->ul_AMask = 0xFF000000;
			_pst_TexDesc->ul_RMask = 0xFF0000;
			_pst_TexDesc->ul_GMask = 0xFF00;
			_pst_TexDesc->ul_BMask = 0xFF;
		}
		else if(_pst_TexDesc->uc_BPP == 24)
		{
			_pst_TexDesc->ul_RMask = 0xFF0000;
			_pst_TexDesc->ul_GMask = 0xFF00;
			_pst_TexDesc->ul_BMask = 0xFF;
			_pst_TexDesc->ul_AMask = 0;
		}
		else if(_pst_TexDesc->uc_BPP == 16)
		{
			_pst_TexDesc->ul_RMask = 0x7C00;
			_pst_TexDesc->ul_GMask = 0x03E0;
			_pst_TexDesc->ul_BMask = 0x1F;
			_pst_TexDesc->ul_AMask = 0x8000;
		}
		else if(_pst_TexDesc->uc_BPP != 8) return 0;
	}

	if(!(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Content)) return 1;

	ul_Length = ((ULONG) _pst_TexDesc->uw_Width * _pst_TexDesc->uw_Height);

	/* ul_Length *= _pst_TexDesc->uc_BPP >> 3; */
	if(!ul_Length) return 0;

	/* Get bitmap */
	if
	(
		(_pst_TexDesc->uc_BPP == 32)
	||	(_pst_TexDesc->uc_BPP == 24)
	||	(_pst_TexDesc->uc_BPP == 16)
	||	(_pst_TexDesc->uc_BPP == 8)
	)
	{
		TEX_M_File_Alloc(_pst_TexDesc->p_Bitmap, ul_Length * (_pst_TexDesc->uc_FinalBPP >> 3), void);
		L_memcpy(_pst_TexDesc->p_Bitmap, _pc_Buffer, ul_Length * (_pst_TexDesc->uc_BPP >> 3));
		return 1;
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
    Procedural texture file
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_LoadProcedural(char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc, ULONG _ul_Size)
{
	/*~~~~~~~~~~~~*/
	int		i_Type;
	LONG	l_Size;
	short	w_Flags;
	/*~~~~~~~~~~~~*/

	l_Size = 0;

	if(_ul_Size == 0)
	{
		_pst_TexDesc->uw_Width = 128;
		_pst_TexDesc->uw_Height = 128;
		i_Type = 0;
		w_Flags = 0;
	}
	else
	{
		l_Size = LOA_ReadLong(&_pc_Buffer);
		w_Flags = LOA_ReadShort(&_pc_Buffer);
		_pst_TexDesc->uw_Width = LOA_ReadUShort(&_pc_Buffer);
		_pst_TexDesc->uw_Height = LOA_ReadUShort(&_pc_Buffer);
		i_Type = LOA_ReadUShort(&_pc_Buffer);
	}

	_pst_TexDesc->uc_FinalBPP = _pst_TexDesc->uc_BPP = 32;
	_pst_TexDesc->uw_DescFlags |= TEX_Cuw_DF_SpecialNoLoad;
	_pst_TexDesc->ul_AMask = 0xFF000000;
	_pst_TexDesc->ul_RMask = 0xFF0000;
	_pst_TexDesc->ul_GMask = 0xFF00;
	_pst_TexDesc->ul_BMask = 0xFF;

    if (w_Flags & TEXPRO_Interface)
    {
        _pst_TexDesc->st_Params.uw_Flags |= TEX_FP_Interface;
    }

	_pst_TexDesc->st_Params.ul_Params[0] = i_Type;
	_pst_TexDesc->st_Params.ul_Params[1] = (ULONG) TEX_gast_ProceduralInterface[i_Type].pfnpv_Load
		(
			_pc_Buffer,
			_ul_Size - l_Size,
			_pst_TexDesc->uw_Width,
			_pst_TexDesc->uw_Height
		);
	_pst_TexDesc->st_Params.ul_Params[2] = (int) w_Flags;

	return 1;
}

/*$4
 ***********************************************************************************************************************
    Animated texture file
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_LoadAnimated(char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc, ULONG _ul_Size)
{
	/*~~~~~~~~~~~~*/
	int		            i;
	LONG	            l_Size;
	USHORT              uw_Flags;
    UCHAR               uc_Number;
    TEX_tdst_AnimTex    *pst_Data;
	/*~~~~~~~~~~~~*/

	l_Size = 0;

	if(_ul_Size == 0)
	{
		
		uw_Flags = 0;
        uc_Number = 0;
	}
	else
	{
		l_Size = LOA_ReadLong(&_pc_Buffer);
		uw_Flags = LOA_ReadUShort(&_pc_Buffer);
        uc_Number = LOA_ReadUChar( &_pc_Buffer );
        
        LOA_ReadUChar_Ed(&_pc_Buffer, NULL);

	}

	_pst_TexDesc->uw_DescFlags |= TEX_Cuw_DF_SpecialNoLoad;

	pst_Data = NULL;
    if (uc_Number)
    {
        pst_Data = (TEX_tdst_AnimTex *) MEM_p_Alloc( uc_Number * sizeof( TEX_tdst_AnimTex ) );
        for (i = 0; i < uc_Number; i++)
        {
            pst_Data[i].ul_Key = LOA_ReadULong(&_pc_Buffer);
            pst_Data[i].w_Time = LOA_ReadUShort(&_pc_Buffer);
            pst_Data[i].w_Index = -1;

            LOA_ReadUShort_Ed(&_pc_Buffer, NULL);
        }
    }
    
    _pst_TexDesc->st_Params.ul_Params[0] = (int) uc_Number;
	_pst_TexDesc->st_Params.ul_Params[1] = (ULONG) pst_Data;
	_pst_TexDesc->st_Params.ul_Params[2] = (int) uw_Flags;

	return 1;
}

/*$4
 ***********************************************************************************************************************
    raw texture
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_LoadRaw(char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc )
{
    ULONG ul_Length;
    
    if( !(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Content) ) return 1;

	ul_Length = ((ULONG) _pst_TexDesc->uw_Width * _pst_TexDesc->uw_Height);
	if(!ul_Length) return 0;

    ul_Length = (ul_Length * _pst_TexDesc->uc_FinalBPP) >> 3;

	TEX_M_File_Alloc(_pst_TexDesc->p_Bitmap, ul_Length, void);
	LOA_ReadCharArray(&_pc_Buffer, (CHAR*)_pst_TexDesc->p_Bitmap, ul_Length);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS 
ULONG TEX_ul_File_SaveRawInBF(char *_sz_Path, char *_sz_Name, TEX_tdst_File_Desc *_pst_TexDesc, BOOL update )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Length, ul_Index;
	LONG					l_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Result = 0;

    if ( (_pst_TexDesc->uc_BPP != 4) && (_pst_TexDesc->uc_BPP != 8) ) return BIG_C_InvalidKey;
    if(!_pst_TexDesc->p_Bitmap) return BIG_C_InvalidKey;

    ul_Index = BIG_ul_SearchFileExt( _sz_Path, _sz_Name );
    if ( ul_Index != BIG_C_InvalidKey )
        TEX_File_GetParams( &_pst_TexDesc->st_Params, ul_Index );
    else
    {
		if(!update)
		{
	        TEX_File_InitParams(&_pst_TexDesc->st_Params);
		    _pst_TexDesc->st_Params.uw_Flags = TEX_FP_QualityMedium;
		}
    }
    _pst_TexDesc->st_Params.uc_Type = TEX_FP_RawFile;
    _pst_TexDesc->st_Params.uc_Format = (_pst_TexDesc->uc_BPP == 4) ? TEX_FP_4bpp : TEX_FP_8bpp;
    _pst_TexDesc->st_Params.uw_Width = _pst_TexDesc->uw_Width;
    _pst_TexDesc->st_Params.uw_Height = _pst_TexDesc->uw_Height;

    SAV_Begin( _sz_Path, _sz_Name );

    /* Compute bitmap length and write it */
	ul_Length = (_pst_TexDesc->uw_Height * _pst_TexDesc->uw_Width * _pst_TexDesc->uc_BPP) >> 3;
	SAV_Buffer( (char*)_pst_TexDesc->p_Bitmap, ul_Length);
    SAV_Buffer( &_pst_TexDesc->st_Params, 32 );

	return SAV_ul_End();
}
#endif


/*$4
 ***********************************************************************************************************************
    raw palette texture
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
	Cherche si une texture de remplacement existe (quand on binarise, a cause du preprocess
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
ULONG TEX_l_FileFindReplacement(ULONG _ul_FileKey)
{
#if defined(_XENON_RENDER)

    CHAR       sz_FileName[32];
    BIG_INDEX  ul_FileIndex;

    sprintf(sz_FileName, "0x%08x.xtx", _ul_FileKey);

    ul_FileIndex = BIG_ul_SearchFileExt(TEX_Csz_X360TexturesPath, sz_FileName);
    if (ul_FileIndex != BIG_C_InvalidIndex)
        _ul_FileKey = BIG_FileKey(ul_FileIndex);

    return _ul_FileKey;

#else
	ULONG			ul_World;
#if defined (JADEFUSION)
	extern unsigned int	WOR_gul_WorldKey;
#else
	extern ULONG	WOR_gul_WorldKey;
#endif
	char			az_Path[BIG_C_MaxLenPath];
	char			az_File[BIG_C_MaxLenName];
	ULONG			ul_File;

	ul_World = BIG_ul_SearchKeyToFat(WOR_gul_WorldKey);
	if(ul_World != BIG_C_InvalidIndex)
	{
		BIG_ComputeFullName(BIG_ParentFile(ul_World), az_Path);
		L_strcat(az_Path, "/BinTextures");
		sprintf(az_File, "%x", _ul_FileKey);
		ul_File = BIG_ul_SearchFileExt(az_Path, az_File);
		if(ul_File != BIG_C_InvalidIndex) 
			_ul_FileKey = BIG_FileKey(ul_File);
	}
	return _ul_FileKey;
#endif
}

void TEX_4Edit_FileReplaceInTexDesc( TEX_tdst_File_Desc *_pst_TexDesc )
{
	int i_Slot;
	for (i_Slot = 0; i_Slot < 4; i_Slot++ )
	{
		_pst_TexDesc->st_Tex.ast_Slot[ i_Slot ].ul_Raw = TEX_l_FileFindReplacement(_pst_TexDesc->st_Tex.ast_Slot[ i_Slot ].ul_Raw);
		_pst_TexDesc->st_Tex.ast_Slot[ i_Slot ].ul_TC = TEX_l_FileFindReplacement(_pst_TexDesc->st_Tex.ast_Slot[ i_Slot ].ul_TC );
	}
}

#else

#define TEX_4Edit_FileReplaceInTexDesc(a) 

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_LoadRawPalette(char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc, ULONG _ul_Size )
{
	ULONG	*pul_Keys;
#ifdef JADEFUSION
	ULONG ul_ReadSize = _ul_Size;
#endif

#if defined(_XENON_RENDER)
    TEX_XeResetProperties(&_pst_TexDesc->st_Tex.st_XeProperties);

    if (_ul_Size > 80)
        ul_ReadSize = 80;
#endif

	L_memset( &_pst_TexDesc->st_Tex, 0xFF, sizeof( TEX_tdst_File_Tex ) );
#ifdef JADEFUSION
	if ( ( ul_ReadSize > 80 ) || (ul_ReadSize & 3) )
#else
	if ( ( _ul_Size > 80 ) || (_ul_Size & 3) )
#endif
		return 0;

	pul_Keys = (ULONG *) &_pst_TexDesc->st_Tex;
#ifdef JADEFUSION
	ul_ReadSize = (ul_ReadSize - 32) >> 2;
	while (ul_ReadSize--)
#else
	_ul_Size = (_ul_Size - 32) >> 2;
	while (_ul_Size--)
#endif
		*pul_Keys++ = LOA_ReadLong(&_pc_Buffer);

#if defined(_XENON_RENDER)
    // Read the Xenon properties if any
    if (_ul_Size > 80)
    {
        TEX_XeLoadProperties(&_pst_TexDesc->st_Tex.st_XeProperties, &_pc_Buffer);
    }

	for (ULONG i = 0; i < 4; ++i)
	{
		_pst_TexDesc->st_Tex.ast_Slot[i].ul_TC = BIG_C_InvalidKey;
	}
#endif


	TEX_4Edit_FileReplaceInTexDesc( _pst_TexDesc );
	

	//if ( _ul_Size == 40)
	//{
     //   _pst_TexDesc->st_Tex.ast_Slot[0].ul_Raw = LOA_ReadLong(&_pc_Buffer);          /* raw */
      //  _pst_TexDesc->st_Tex.ast_Slot[0].ul_Pal = LOA_ReadLong(&_pc_Buffer);    /* palette */
	//}
//    else if ( _ul_Size == 44 )
  //  {
    //    _pst_TexDesc->st_Tex.ast_Slot[0].ul_Raw = LOA_ReadLong(&_pc_Buffer);            /* raw */
      //  _pst_TexDesc->st_Tex.ast_Slot[0].ul_Pal = LOA_ReadLong(&_pc_Buffer);      /* palette */
//        _pst_TexDesc->st_Tex.ast_Slot[0].ul_TC = LOA_ReadLong(&_pc_Buffer);    /* 32 / 24 bit */
  //  
    //}

    //_pst_TexDesc->st_Params.ul_Params[0] = _pst_TexDesc->st_Tex.ast_Slot[0].ul_Raw;
    //_pst_TexDesc->st_Params.ul_Params[1] = _pst_TexDesc->st_Tex.ast_Slot[0].ul_Pal;
    //_pst_TexDesc->st_Params.ul_Params[2] = _pst_TexDesc->st_Tex.ast_Slot[0].ul_TC;

	return 1;
}

/*$4
 ***********************************************************************************************************************
    Sprite generator functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_LoadSpriteGen(ULONG _ul_Key, char *_pc_Buffer, TEX_tdst_File_Desc *_pst_TexDesc, ULONG _ul_Size)
{
	_pst_TexDesc->st_Params.ul_Params[0] = (ULONG) MAT_pst_SpriteGen_Load(_ul_Key, _pc_Buffer, _ul_Size);
	return 1;
}

/*$4
 ***********************************************************************************************************************
    General functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Warning(char _c_Warn, BIG_KEY _ul_FileKey)
{
	if(_c_Warn)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Index;
		char		*sz_Name, sz_Unknow[20];
		char		sz_Message[512];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Index = BIG_ul_SearchKeyToFat(_ul_FileKey);
		if(ul_Index != BIG_C_InvalidIndex)
			sz_Name = BIG_NameFile(ul_Index);
		else
		{
			sprintf(sz_Unknow, "0x%08X", _ul_FileKey);
			sz_Name = sz_Unknow;
		}

		sprintf(sz_Message, "Can't get content of texture %s : bad format", sz_Name);
		ERR_X_Warning(0, sz_Message, NULL);
	}
}

#else
#define TEX_Warning(a, b)
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */

LONG TEX_l_File_Read(BIG_KEY _ul_FileKey, TEX_tdst_File_Desc *_pst_TexDesc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Size;
	char			*pc_Buffer = NULL;
	LONG			l_Result;
	ULONG			ul_Pos;
	unsigned short	uw_SaveFlags;
	BOOL			b_BadParams;
	extern void BIG_UpdateCache(void);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	_ul_FileKey = TEX_l_FileFindReplacement(_ul_FileKey);
#endif

    BIG_UpdateCache();

	if(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Info)
	{
		uw_SaveFlags = _pst_TexDesc->uw_DescFlags;
		L_memset(_pst_TexDesc, 0, sizeof(TEX_tdst_File_Desc));
		_pst_TexDesc->uw_DescFlags = uw_SaveFlags;
	}

	ul_Pos = BIG_ul_SearchKeyToPos(_ul_FileKey);
	if(ul_Pos == (ULONG) - 1) 
	{
#ifdef ACTIVE_EDITORS
		char az[64];
		sprintf(az, "[%08X] Unknown texture", _ul_FileKey);
		ERR_X_Warning(0, az, NULL);
#endif
		return 0;
	}

	{
		CHAR * pc_ParamBuf;

		pc_Buffer = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);
		pc_ParamBuf = pc_Buffer;
		// If we're loading the non binary data (i.e. editor data), we need to skip the data ahead of the tex params
		// However, in binary data, they will be saved in the required order, so no skipping is required
		if(!LOA_IsBinaryData())
			pc_ParamBuf += ul_Size - sizeof(TEX_tdst_File_Params);

		// Load the TEX_tdst_File_Params structure
		_pst_TexDesc->st_Params.ul_Mark = LOA_ReadULong(&pc_ParamBuf);
		if(_pst_TexDesc->st_Params.ul_Mark != 0xFFFFFFFF)
		{
#ifdef ACTIVE_EDITORS
			char az[64];
			sprintf(az, "[%08X] Texture with bad parameters", _ul_FileKey);
			ERR_X_Warning(0, az, NULL);
			return 0;
#else
			_breakpoint_;
#endif
		}
		_pst_TexDesc->st_Params.uw_Flags = LOA_ReadUShort(&pc_ParamBuf);
#ifdef _XBOX
		//_pst_TexDesc->st_Params.uw_Flags |= TEX_FP_MipmapOn; Test Phil
#endif
		_pst_TexDesc->st_Params.uc_Type = LOA_ReadUChar(&pc_ParamBuf);
		_pst_TexDesc->st_Params.uc_Format = LOA_ReadUChar(&pc_ParamBuf);
		_pst_TexDesc->st_Params.uw_Width = LOA_ReadUShort(&pc_ParamBuf);
		_pst_TexDesc->st_Params.uw_Height = LOA_ReadUShort(&pc_ParamBuf);
		_pst_TexDesc->st_Params.ul_Color = LOA_ReadULong(&pc_ParamBuf);
		_pst_TexDesc->st_Params.ul_Params[0] = LOA_ReadULong(&pc_ParamBuf);
		_pst_TexDesc->st_Params.ul_Params[1] = LOA_ReadULong(&pc_ParamBuf);
		_pst_TexDesc->st_Params.ul_Params[2] = LOA_ReadULong(&pc_ParamBuf);
		_pst_TexDesc->st_Params.ul_Params[3] = LOA_ReadULong(&pc_ParamBuf);

		if(pc_Buffer != NULL && LOA_IsBinaryData())
			pc_Buffer = pc_ParamBuf; // in the binary data, the content follows the header so advance the pointer.

	}

	if(!TEX_b_File_CheckParams(&_pst_TexDesc->st_Params))
	{
		TEX_File_TransformParams(&_pst_TexDesc->st_Params);
		_pst_TexDesc->uw_DescFlags &= ~TEX_Cuw_DF_OnlyParams;
		b_BadParams = TRUE;
	}
	else
	{
		if
		(
			(_pst_TexDesc->st_Params.uc_Type == TEX_FP_SprFile)
		||	(_pst_TexDesc->st_Params.uc_Type == TEX_FP_ProFile)
        ||	(_pst_TexDesc->st_Params.uc_Type == TEX_FP_AniFile)
		||	(_pst_TexDesc->st_Params.uc_Type == TEX_FP_RawPalFile)
		) _pst_TexDesc->uw_DescFlags &= ~TEX_Cuw_DF_OnlyParams;
		b_BadParams = FALSE;
	}
#if defined(_XENON_RENDER)
    // Keep the size of the file since we will need it in the texture manager
    _pst_TexDesc->st_Tex.st_XeProperties.ul_FileSize = ul_Size - sizeof(TEX_tdst_File_Params);

    // DDS files are loaded entirely in memory
    if (_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_XenonFileAsBitmap)
    {
        _pst_TexDesc->st_Params.uc_Type = TEX_FP_XeDDSFile;
        _pst_TexDesc->uc_BPP      = 0x80;
        _pst_TexDesc->uc_FinalBPP = 0x80;
    }

#endif
    if (_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Info)
    {
        TEX_File_DevelopParams(_pst_TexDesc);
	
        if(_pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_OnlyParams)
		{
		    return 1;
		}
    }

	switch(_pst_TexDesc->st_Params.uc_Type)
	{
	case TEX_FP_TgaFile:
		l_Result = TEX_l_File_LoadTga(pc_Buffer, _pst_TexDesc);
		break;

	case TEX_FP_BmpFile:
		l_Result = TEX_l_File_LoadBmp(pc_Buffer, _pst_TexDesc);
		break;

	case TEX_FP_JpgFile:
		l_Result = TEX_l_File_LoadJpeg(pc_Buffer, _pst_TexDesc, ul_Size);
		break;

	case TEX_FP_SprFile:
		l_Result = TEX_l_File_LoadSpriteGen(_ul_FileKey, pc_Buffer, _pst_TexDesc, ul_Size - sizeof(TEX_tdst_File_Params));
		break;

	case TEX_FP_ProFile:
		l_Result = TEX_l_File_LoadProcedural(pc_Buffer, _pst_TexDesc, ul_Size - sizeof(TEX_tdst_File_Params));
		break;

    case TEX_FP_RawFile:
		l_Result = TEX_l_File_LoadRaw(pc_Buffer, _pst_TexDesc );
        break;
	
    case TEX_FP_RawPalFile:
		l_Result = TEX_l_File_LoadRawPalette(pc_Buffer, _pst_TexDesc, ul_Size );
		break;

    case TEX_FP_AniFile:
		l_Result = TEX_l_File_LoadAnimated(pc_Buffer, _pst_TexDesc, ul_Size - sizeof(TEX_tdst_File_Params));
		break;

#if defined(_XENON_RENDER)
    case TEX_FP_XeDDSFile:
        l_Result = TEX_l_File_LoadDDS(pc_Buffer, _pst_TexDesc, ul_Size - sizeof(TEX_tdst_File_Params));
        break;
#endif

	default: l_Result = 0;
	}

    if (l_Result)
    {
        /* mark file as loaded */
#ifdef ACTIVE_EDITORS
    	ul_Size = BIG_ul_SearchKeyToFat( _ul_FileKey );
	    if(ul_Size != BIG_C_InvalidIndex) BIG_FileChanged(ul_Size) |= EDI_FHC_Loaded;
#endif

	    /* Special treatment for font texture */
	    if( _pst_TexDesc->uw_DescFlags & TEX_Cuw_DF_Content )
	    {
		    if(_pst_TexDesc->st_Params.uw_Flags & TEX_FP_ThisIsAFont) STR_AddFont(_ul_FileKey, _pst_TexDesc);
	    }

        /* bad params in texture, set good one */
	    if(b_BadParams)
	    {
		    _pst_TexDesc->st_Params.uc_Format = (_pst_TexDesc->uc_BPP == 32) ? TEX_FP_32bpp : TEX_FP_24bpp;
		    _pst_TexDesc->st_Params.uw_Height = _pst_TexDesc->uw_Height;
		    _pst_TexDesc->st_Params.uw_Width = _pst_TexDesc->uw_Width;
		    TEX_File_SetParams(&_pst_TexDesc->st_Params, _ul_FileKey, TRUE);
	    }
    }

	TEX_Warning((char) (l_Result == 0), _ul_FileKey);
	return l_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_GetInfo(BIG_KEY _ul_FileKey, TEX_tdst_File_Desc *_pst_TexDesc, BOOL _b_OnlyParams)
{
	_pst_TexDesc->uw_DescFlags |= TEX_Cuw_DF_Info;
	_pst_TexDesc->uw_DescFlags &= ~TEX_Cuw_DF_Content;
	if(_b_OnlyParams)
		_pst_TexDesc->uw_DescFlags |= TEX_Cuw_DF_OnlyParams;
	else
		_pst_TexDesc->uw_DescFlags &= ~TEX_Cuw_DF_OnlyParams;
	return TEX_l_File_Read(_ul_FileKey, _pst_TexDesc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_GetContent(BIG_KEY _ul_FileKey, TEX_tdst_File_Desc *_pst_TexDesc)
{
	_pst_TexDesc->uw_DescFlags &= ~TEX_Cuw_DF_Info;
	_pst_TexDesc->uw_DescFlags |= TEX_Cuw_DF_Content;
	_pst_TexDesc->uw_DescFlags &= ~TEX_Cuw_DF_OnlyParams;
	return TEX_l_File_Read(_ul_FileKey, _pst_TexDesc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_File_GetInfoAndContent(BIG_KEY _ul_FileKey, TEX_tdst_File_Desc *_pst_TexDesc)
{
	_pst_TexDesc->uw_DescFlags |= TEX_Cuw_DF_Info;
	_pst_TexDesc->uw_DescFlags |= TEX_Cuw_DF_Content;
	_pst_TexDesc->uw_DescFlags &= ~TEX_Cuw_DF_OnlyParams;
	return TEX_l_File_Read(_ul_FileKey, _pst_TexDesc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_FreeDescription(TEX_tdst_File_Desc *_pst_TexDesc)
{
	if((_pst_TexDesc->p_Palette) && (_pst_TexDesc->uw_PaletteLength))
	{
		MEM_Free(_pst_TexDesc->p_Palette);
		_pst_TexDesc->p_Palette = NULL;
	}

	if(_pst_TexDesc->p_Bitmap)
	{
		TEX_M_File_Free(_pst_TexDesc->p_Bitmap);
		_pst_TexDesc->p_Bitmap = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL TEX_b_File_CheckParams(TEX_tdst_File_Params *_pst_FP)
{
	if(_pst_FP->ul_Params[3] != 0xC0DEC0DE) return FALSE;
	if(_pst_FP->ul_Params[2] != 0x00FF00FF) return FALSE;
	if(_pst_FP->ul_Params[1] != 0xCAD01234) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_TransformParams(TEX_tdst_File_Params *_pst_FP)
{
	_pst_FP->ul_Color = *(ULONG *) &_pst_FP->uw_Flags;
	_pst_FP->uw_Flags = *(USHORT *) &_pst_FP->ul_Mark;
	_pst_FP->uc_Type = (UCHAR) (_pst_FP->ul_Mark >> 28);
	_pst_FP->uc_Format = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_DevelopParams(TEX_tdst_File_Desc *_pst_Desc)
{
	_pst_Desc->uw_Width = _pst_Desc->st_Params.uw_Width;
	_pst_Desc->uw_Height = _pst_Desc->st_Params.uw_Height;

	switch(_pst_Desc->st_Params.uc_Format)
	{
	case TEX_FP_32bpp:
		_pst_Desc->uc_BPP = 32;
		_pst_Desc->ul_AMask = 0xFF000000;
		_pst_Desc->ul_RMask = 0xFF0000;
		_pst_Desc->ul_GMask = 0xFF00;
		_pst_Desc->ul_BMask = 0xFF;
		break;
	case TEX_FP_24bpp:
		_pst_Desc->uc_BPP = 24;
		_pst_Desc->ul_AMask = 0;
		_pst_Desc->ul_RMask = 0xFF0000;
		_pst_Desc->ul_GMask = 0xFF00;
		_pst_Desc->ul_BMask = 0xFF;
		break;
	case TEX_FP_1555:
		_pst_Desc->uc_BPP = 16;
		_pst_Desc->ul_AMask = 0X8000;
		_pst_Desc->ul_RMask = 0x7C00;
		_pst_Desc->ul_GMask = 0x03E0;
		_pst_Desc->ul_BMask = 0x001F;
		break;
	case TEX_FP_4444:
		_pst_Desc->uc_BPP = 16;
		_pst_Desc->ul_AMask = 0XF000;
		_pst_Desc->ul_RMask = 0x0F00;
		_pst_Desc->ul_GMask = 0x00F0;
		_pst_Desc->ul_BMask = 0x000F;
		break;
	case TEX_FP_8bpp:
		_pst_Desc->uc_BPP = 8;
		break;
	case TEX_FP_4bpp:
		_pst_Desc->uc_BPP = 4;
		break;
	}

	_pst_Desc->uc_FinalBPP = _pst_Desc->uc_BPP;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_CreatePalette( TEX_tdst_Palette *_pst_Pal, char *_sz_Path, char *_sz_Name, BOOL _b_32, BOOL _b_SwapRB )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*pc_Pal, *pc_Src, *pc_Buffer;
	char	sz_PalName[BIG_C_MaxLenName];
    BOOL    b_4Bit;
    int     i;
    ULONG   ul_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pc_Pal = strrchr(_sz_Name, '.');
	if(pc_Pal) *pc_Pal = 0;
	strcpy(sz_PalName, _sz_Name);
	strcat(sz_PalName, ".pal");
	if(pc_Pal) 
    {
        *pc_Pal = '.';
        b_4Bit = (L_strnicmp( pc_Pal - 5, "4bits", 5) == 0);
    }
    else
        b_4Bit = FALSE;

    pc_Src = (char *) _pst_Pal->pul_Color;
    pc_Pal = NULL;
    if (!_b_32)
    {
        pc_Pal = pc_Buffer = (char*)L_malloc( 1024 );
        for (i = 0; i < 256; i++)
        {
            *pc_Pal++ = *pc_Src++;
            *pc_Pal++ = *pc_Src++;
            *pc_Pal++ = *pc_Src++;
            *pc_Pal++ = (char) 0;
        }
        pc_Src = (char *) _pst_Pal->pul_Color;
        _pst_Pal->pul_Color = (ULONG *) pc_Buffer;
    }

    if (b_4Bit )
    {
        _pst_Pal->uc_Flags = TEX_uc_Palette16;
        for (i = 0; i< 16; i++)
            if ( _pst_Pal->pul_Color[ i ] != 0) break;
         if (i == 16)
         {
             _pst_Pal->uc_Flags |= TEX_uc_IndexInverted;
            for ( i = 0; i < 16; i++)
                _pst_Pal->pul_Color[ i ] = _pst_Pal->pul_Color[ 255 - i ];
         }
    }
    else
    {
        _pst_Pal->uc_Flags = 0;
    }
    
    ul_Result = TEX_ul_File_SaveNewPaletteBF( _pst_Pal, _sz_Path, sz_PalName, _b_SwapRB );

    if (pc_Pal) L_free( pc_Buffer );
    _pst_Pal->pul_Color = (ULONG *) pc_Pal;

    return ul_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_CreateRaw(char *_sz_Path, char *_sz_Name, TEX_tdst_File_Desc *_pst_Desc, TEX_tdst_File_Params *_pst_Params, BOOL _b_Invert )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Ext, *pc_Src, *pc_Dest;
	char					sz_RawName[BIG_C_MaxLenName];
	TEX_tdst_File_Params	st_Params;
	ULONG					ul_File;
    LONG                    l_Size, l_Count;
    BOOL                    b_4Bit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_4Bit = FALSE;
	pc_Ext = strrchr(_sz_Name, '.');
	if(pc_Ext) *pc_Ext = 0;
	strcpy(sz_RawName, _sz_Name);
	strcat(sz_RawName, ".raw");
	if(pc_Ext) 
    {
        *pc_Ext = '.';
        b_4Bit = (L_strnicmp( pc_Ext - 5, "4bits", 5) == 0);
    }

    if (_pst_Params == NULL)
    {
	    TEX_File_InitParams(&st_Params);
	    st_Params.uc_Type = TEX_FP_RawFile;
	    st_Params.uw_Flags = TEX_FP_QualityMedium;
        st_Params.uc_Format = (b_4Bit) ? TEX_FP_4bpp : TEX_FP_8bpp;
        st_Params.uw_Width = _pst_Desc->uw_Width;
        st_Params.uw_Height = _pst_Desc->uw_Height;
        _pst_Params = &st_Params;
    }

    l_Size = _pst_Desc->uw_Height * _pst_Desc->uw_Width;
    if (_b_Invert)
    {
        pc_Ext = (char*)_pst_Desc->p_TmpBitmap;
        l_Count = l_Size;
        while (l_Count-- )
        {
            *pc_Ext = 255 - *pc_Ext;
            pc_Ext++;
        }
    }

    if (b_4Bit)
    {
        l_Size >>= 1;
        l_Count = l_Size;
        pc_Src = pc_Dest = (char *) _pst_Desc->p_TmpBitmap;
        while (l_Count--)
        {
            *pc_Dest++ = (*pc_Src << 4) | (*(pc_Src + 1) & 0xF);
            pc_Src += 2;
        }
    }
    
    SAV_Begin(_sz_Path, sz_RawName);
	SAV_Buffer(_pst_Desc->p_TmpBitmap, l_Size);
    SAV_Buffer(_pst_Params, 32);
    ul_File = SAV_ul_End();

	if(ul_File == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;
	return BIG_FileKey(ul_File);
}

/*
 =======================================================================================================================
    compute tex name
 =======================================================================================================================
 */
void TEX_ul_File_TexComputeName( char *_sz_Name, char *_sz_TexName, int *pi_Slot )
{
	char	*pc_Ext,c_Save;
	int		i_Slot;

	pc_Ext = strrchr(_sz_Name, '.');
	if ( !pc_Ext ) pc_Ext = _sz_Name + strlen( _sz_Name );

	// detection suffixe _4bits et _8bits
	if ( !L_strnicmp( pc_Ext - 6, "_4bits", 6 ) || !L_strnicmp( pc_Ext - 6, "_8bits", 6 ) )
		pc_Ext -= 6;

	// dettection suffixe console (_PS2 / _GC / _XBOX / _XENON / _PC)
	i_Slot = 0;
	if ( !L_strncmp( pc_Ext - 4, "_PS2", 4) )
	{
		pc_Ext -= 4;
	}
	else if ( !L_strncmp( pc_Ext - 3, "_GC", 3) )
	{
		pc_Ext -= 3;
		i_Slot = 1; 
	}
	else if ( !L_strncmp( pc_Ext - 5, "_XBOX", 5) )
	{
		pc_Ext -= 5;
		i_Slot = 2; 
	}
	else if ( !L_strncmp( pc_Ext - 6, "_XENON", 6) )
	{
		pc_Ext -= 6;
		i_Slot = 2; 
	}
	else if ( !L_strncmp( pc_Ext - 3, "_PC", 3) )
	{
		pc_Ext -= 3;
		i_Slot = 3; 
	}

	c_Save = *pc_Ext;
	*pc_Ext = 0;
	strcpy( _sz_TexName, _sz_Name);
	strcat( _sz_TexName, ".tex" );
	*pc_Ext = c_Save;

	if (pi_Slot) *pi_Slot = i_Slot;
}

/*
 =======================================================================================================================
    Save a tex file 
        a key set to -2 indicate that if key exist value does not change from the one in file
                        (if file doesn't exist -1 replace -2)
 =======================================================================================================================
 */
ULONG TEX_ul_File_CreatePaletteRawTexture(char *_sz_Path, char *_sz_Name, ULONG _ul_Raw, ULONG _ul_Palette, ULONG _ul_TrueColor )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Ext;  //, *pc_End;
	char					sz_TexName[BIG_C_MaxLenName];
	TEX_tdst_File_Params	st_Params;
	ULONG					ul_File, ul_TextureDir, ul_Size;
    UCHAR                   *puc_Buf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	TEX_ul_File_TexComputeName( _sz_Name, sz_TexName, NULL );

    ul_File = BIG_ul_SearchFileExt( _sz_Path, sz_TexName);
    if (ul_File != BIG_C_InvalidIndex)
    {
        puc_Buf = (UCHAR *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Size);
       
        L_memcpy( &st_Params, puc_Buf + ul_Size - 32, 32);
        if (_ul_Raw == -2) _ul_Raw = *(ULONG *) puc_Buf;
        if (_ul_Palette == -2) _ul_Palette = *(ULONG *) (puc_Buf + 4);
        if (_ul_TrueColor == -2) 
        {
            if (ul_Size > 40)
                _ul_TrueColor = *(ULONG *) (puc_Buf + 8);
            else
                _ul_TrueColor = -1;
        }
    }
    else
    {
        if (_ul_Raw == (ULONG) -2) _ul_Raw = -1;
        if (_ul_Palette == (ULONG) -2) _ul_Palette = -1;
        if (_ul_TrueColor == (ULONG) -2) 
        {
            _ul_TrueColor = -1;

            // try to associate automatically a tga
            ul_TextureDir = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
            pc_Ext = L_strrchr( sz_TexName, '.' );
            L_strcpy( pc_Ext + 1, "tga" );
            ul_File = BIG_ul_SearchFileInDirRec(ul_TextureDir, sz_TexName );
            if (ul_File == BIG_C_InvalidIndex)
            {
                L_strcpy( pc_Ext + 1, "bmp" );
                ul_File = BIG_ul_SearchFileInDirRec(ul_TextureDir, sz_TexName );
                if (ul_File == BIG_C_InvalidIndex)
                {
                    L_strcpy( pc_Ext + 1, "jpg" );
                    ul_File = BIG_ul_SearchFileInDirRec(ul_TextureDir, sz_TexName );
                }
            }
            L_strcpy( pc_Ext, ".tex" );

            if (ul_File != BIG_C_InvalidKey)
                _ul_TrueColor = BIG_FileKey( ul_File );
            
        }
	    TEX_File_InitParams(&st_Params);
	    st_Params.uc_Type = TEX_FP_RawPalFile;
    }

	SAV_Begin(_sz_Path, sz_TexName);
	SAV_Buffer(&_ul_Raw, 4);
	SAV_Buffer(&_ul_Palette, 4);
    SAV_Buffer(&_ul_TrueColor, 4);
	SAV_Buffer(&st_Params, 32);
	ul_File = SAV_ul_End();

	return ul_File;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_SaveTexWithName( char *_sz_Path, char *_sz_Name, TEX_tdst_File_Tex *_pst_Tex, BOOL _b_ComputeName )
{
	char					sz_TexName[ 256 ];
	TEX_tdst_File_Params	st_Params;

	if ( _b_ComputeName )
	{
		TEX_ul_File_TexComputeName( _sz_Name, sz_TexName, NULL );
		_sz_Name = sz_TexName;
	}

	SAV_Begin( _sz_Path, _sz_Name );

	/* save data */
#if defined(_XENON_RENDER)
    // Save the usual PS2 structure
    SAV_Buffer( _pst_Tex, sizeof( TEX_tdst_File_Tex ) - sizeof(TEX_tdst_XenonFileTex) );

    // Save the Xenon structure
    TEX_XeSaveProperties(&_pst_Tex->st_XeProperties);
#else
	SAV_Buffer( _pst_Tex, sizeof( TEX_tdst_File_Tex ) );
#endif
	/* save params */
	TEX_File_InitParams(&st_Params);
    st_Params.uc_Type = TEX_FP_RawPalFile;
	SAV_Buffer( &st_Params, 32 );

	return SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_SaveTexWithIndex( ULONG _ul_Index, TEX_tdst_File_Tex *_pst_Tex )
{
	char sz_Path[ BIG_C_MaxLenPath ];

	BIG_ComputeFullName(BIG_ParentFile(_ul_Index), sz_Path);
	return TEX_ul_File_SaveTexWithName( sz_Path, BIG_NameFile( _ul_Index ), _pst_Tex, FALSE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_File_CreateTexRawPal( char *_sz_Path, char *_sz_Name, TEX_tdst_File_Desc *_pst_Desc )
{
    TEX_tdst_File_Desc      st_TexDesc;
    TEX_tdst_Palette        st_Palette;
    ULONG                   ul_Pal, ul_NewPal, ul_Raw, ul_Tex, ul_Tga, ul_TC;
    TEX_tdst_File_Params	st_Params, *pst_Params;
    char                    *psz_Ext, *psz_End;
    char                    sz_Msg[ 512 ], sz_Name[ 256 ];

    /* 1 - cherche un .tex avec le même nom */
    psz_Ext = L_strrchr( _sz_Name, '.' );
    if( !psz_Ext ) return 0;
    L_strcpy( psz_Ext, ".tex" );
    ul_Tex = BIG_ul_SearchFileExt( _sz_Path, _sz_Name );

    /* 2 - cherche un .pal avec le même nom */
    L_strcpy( psz_Ext, ".pal" );
    ul_Pal = BIG_ul_SearchFileExt( _sz_Path, _sz_Name );

    /* 3 - cherche un .raw avec le même nom */
    L_strcpy( psz_Ext, ".raw" );
    ul_Raw = BIG_ul_SearchFileExt( _sz_Path, _sz_Name );

    /* 4 - créé la palette si elle n'existe pas */
    L_memset( &st_Palette, 0, sizeof( TEX_tdst_Palette ) );
    st_Palette.pul_Color = (ULONG*)_pst_Desc->p_TmpPalette;
    if ( (ul_Pal == BIG_C_InvalidIndex) || (ul_Tex == BIG_C_InvalidIndex ) )
        ul_NewPal = TEX_ul_File_CreatePalette( &st_Palette, _sz_Path, _sz_Name, FALSE, FALSE );

    /* 5 - création du raw £
        si il existe on récupére les paramètres et on remplace le bmp par le nouveau */
    if (ul_Raw != BIG_C_InvalidIndex )
    {
        pst_Params = &st_Params;
        TEX_File_GetParams( &st_Params, ul_Raw );
        st_Params.uw_Height = _pst_Desc->uw_Height;
        st_Params.uw_Width = _pst_Desc->uw_Width;
    }
    else
        pst_Params = NULL;
    ul_Raw = TEX_ul_File_CreateRaw(_sz_Path, _sz_Name, _pst_Desc, pst_Params, st_Palette.uc_Flags & TEX_uc_IndexInverted);
    
    /* 6 - récupération / création du .tex */
    ul_TC = (ULONG) -2;
    if (ul_Tex != BIG_C_InvalidIndex)
    {
        TEX_l_File_GetContent( BIG_FileKey(ul_Tex), &st_TexDesc );

        if ( (ul_Pal != BIG_C_InvalidIndex) && (st_TexDesc.st_Tex.ast_Slot[0].ul_Pal == BIG_FileKey( ul_Pal ) ) )
        {
            ul_NewPal = TEX_ul_File_CreatePalette( &st_Palette, _sz_Path, _sz_Name, FALSE, FALSE );
            st_TexDesc.st_Tex.ast_Slot[0].ul_Pal = BIG_FileKey( ul_NewPal );
        }
        else 
		{
			ul_NewPal = BIG_ul_SearchKeyToFat( st_TexDesc.st_Tex.ast_Slot[0].ul_Pal );
		}
    }
    /* 7 - essaye de trouver un .tga avec le même nom */
    else
    {
        L_strcpy( psz_Ext, ".tex" );
        L_strcpy( sz_Name, _sz_Name );
        psz_End = psz_Ext - 6;
        if ( (psz_End > _sz_Name) && (*psz_End == '_') && ((psz_End[1] == '4') || (psz_End[1] == '8')) && (L_strnicmp(psz_End + 2, "bits", 4) == 0) )
        {
            L_strcpy( psz_End, ".tga" );
            ul_Tga = BIG_ul_SearchFileExt( _sz_Path, _sz_Name );
        
            if (ul_Tga != BIG_C_InvalidIndex)
            {
                sprintf( sz_Msg, "%s file found. Insert into %s?\n(tex and tga exchange key)", _sz_Name, sz_Name );
                if(MessageBox( NULL, sz_Msg, "Tex creation", MB_ICONQUESTION | MB_YESNO) == IDYES)
                    ul_TC = BIG_FileKey( ul_Tga );
            }
        }
        L_strcpy( _sz_Name, sz_Name );
    }
    ul_Tex = TEX_ul_File_CreatePaletteRawTexture(_sz_Path, _sz_Name, ul_Raw, BIG_FileKey( ul_NewPal ), ul_TC );

    if (ul_TC != (ULONG) -2)
    {
        /* swap tga and tex key */
        ul_Pal = BIG_FileKey( ul_Tga );
        BIG_FileKey( ul_Tga ) = BIG_FileKey( ul_Tex );
        BIG_FileKey( ul_Tex ) = ul_Pal;

        /* update files */
        BIG_UpdateOneFileInFat( ul_Tga );
        BIG_FileChanged( ul_Tga ) |= EDI_FHC_Touch;
        BIG_InsertKeyToFat( BIG_FileKey( ul_Tga ), ul_Tga );

        BIG_UpdateOneFileInFat( ul_Tex );
        BIG_FileChanged( ul_Tex) |= EDI_FHC_Touch;
        BIG_InsertKeyToFat( BIG_FileKey( ul_Tex ), ul_Tex );

        ul_Tex = TEX_ul_File_CreatePaletteRawTexture(_sz_Path, _sz_Name, (ULONG) -2, (ULONG) -2, BIG_FileKey( ul_Tga ) );
    }
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_PutIntoATex( char *_sz_Path, char *_sz_Name )
{
    TEX_tdst_File_Desc  st_TexDesc;
    char                *psz_Ext;
    ULONG               ul_Index, ul_Tex;
    char                sz_TexName[ BIG_C_MaxLenName ];
    char                sz_Msg[ 512 ];

    ul_Index = BIG_ul_SearchFileExt( _sz_Path, _sz_Name );
    if (ul_Index ==  BIG_C_InvalidIndex) 
        return;

    L_strcpy( sz_TexName, _sz_Name );
    psz_Ext = L_strrchr( sz_TexName, '.' );
    if (!psz_Ext) return;
    L_strcpy( psz_Ext, ".tex" );

    ul_Tex = BIG_ul_SearchFileExt( _sz_Path, sz_TexName );
    if (ul_Tex == BIG_C_InvalidIndex) 
        return;

    TEX_l_File_GetContent( BIG_FileKey(ul_Tex), &st_TexDesc );
    if ( st_TexDesc.st_Tex.ast_Slot[0].ul_TC == BIG_FileKey( ul_Index) )
        return;

    sprintf( sz_Msg, "%s file found. Insert %s into it?", sz_TexName, _sz_Name );
    if(MessageBox( NULL, sz_Msg, "Texture importation", MB_ICONQUESTION | MB_YESNO) == IDNO) 
        return;

    TEX_ul_File_CreatePaletteRawTexture(_sz_Path, _sz_Name, (ULONG) -2, (ULONG) -2, BIG_FileKey( ul_Index ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL TEX_b_File_Import(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Buffer, *sz_Ext;
	LONG					l_Size;
	L_FILE					h_File;
	ULONG					ul_File;
	TEX_tdst_File_Params	st_TextureParams, st_TexParams2;
    char                    sz_Text[256];
    TEX_tdst_Palette        st_Palette;
    BOOL                    b_CheckTex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Open texture file */
	h_File = L_fopen(_psz_ExternName, "rb");
	if(!CLI_FileOpen(h_File)) return 0;

	L_fseek(h_File, 0, SEEK_END);
	l_Size = L_ftell(h_File);
	L_fseek(h_File, 0, SEEK_SET);

	pc_Buffer = (char*)L_malloc(l_Size + 32);
	L_fread(pc_Buffer, 1, l_Size, h_File);
	L_fclose(h_File);

	/* Put extension into lower case */
	sz_Ext = strrchr(_psz_BigFileName, '.');
	if(sz_Ext)
	{
		while(*(++sz_Ext)) *sz_Ext = tolower(*sz_Ext);
	}

	/* importing palette */
	sz_Ext = strrchr(_psz_BigFileName, '.');
	if( (sz_Ext) && (L_strcmp(sz_Ext + 1, "pal") == 0) )
	{
        if (l_Size != 1048)
        {
            if ( (l_Size == 48) || (l_Size == 64) || (l_Size == 768) || (l_Size == 1024) )
			{
	            L_free( pc_Buffer );
                return 1;
			}
            sprintf( sz_Text, "Invalid palette : %s", _psz_ExternName );
            ERR_X_Warning( l_Size == 1048, "Import error", sz_Text );
            L_free( pc_Buffer );
            return 0;
        }
        
        st_Palette.pul_Color = (ULONG * ) (pc_Buffer + 24);
        TEX_ul_File_CreatePalette( &st_Palette, _psz_BigPathName, _psz_BigFileName, TRUE, TRUE );
        L_free( pc_Buffer );
		return 0;
	}

    b_CheckTex = FALSE;

    ul_File = BIG_ul_SearchDir(_psz_BigPathName);
	if(ul_File != BIG_C_InvalidIndex) ul_File = BIG_ul_SearchFile(ul_File, _psz_BigFileName);
	if(ul_File != BIG_C_InvalidIndex)
	{
		TEX_File_GetParams(&st_TextureParams, ul_File);
		if(!TEX_b_File_CheckParams(&st_TextureParams))
		{
			if(!TEX_i_File_DefaultParams(&st_TextureParams, _psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size))
            {
                L_free( pc_Buffer );
				return 0;
            }
		}
        else
        {
            if(TEX_i_File_DefaultParams(&st_TexParams2, _psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size))
            {
                st_TextureParams.uw_Height = st_TexParams2.uw_Height;
                st_TextureParams.uw_Width = st_TexParams2.uw_Width;
            }
        }
	}
	else
	{
		if(!TEX_i_File_DefaultParams(&st_TextureParams, _psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size))
        {
            L_free( pc_Buffer );
			return 0;
        }
        b_CheckTex = TRUE;
	}

    l_Size -= 32;
    if ( !TEX_b_File_CheckParams((TEX_tdst_File_Params *) (pc_Buffer + l_Size )))
		l_Size += 32;
	L_memcpy(pc_Buffer + l_Size, &st_TextureParams, 32);

	BIG_UpdateFileFromBuffer(_psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size + 32);
	L_free(pc_Buffer);

    if (b_CheckTex)
        TEX_File_PutIntoATex( _psz_BigPathName, _psz_BigFileName );

#if defined(_XENON_RENDER)
    const CHAR BASE_TEXTURE_PATH[]  = "ROOT/EngineDatas/01 Texture Bank/";
    const CHAR XENON_TEXTURE_PATH[] = "ROOT/EngineDatas/01 Texture Bank/_Xenon/";

    CHAR szBigFileNameLwr[512];
    strcpy(szBigFileNameLwr, _psz_BigFileName);
    strlwr(szBigFileNameLwr);

    // Update the corresponding .tex if importing a Xenon .tga [diffuse map]
    if ((strstr(szBigFileNameLwr, "_xe_c.tga") != NULL) &&
        (strstr(_psz_BigPathName, XENON_TEXTURE_PATH) == _psz_BigPathName))
    {
        CHAR  szTexPath[512];
        CHAR  szTexFileName[256];
        CHAR* szExtraPath = _psz_BigPathName + strlen(XENON_TEXTURE_PATH);

        ULONG ul_XenonFileIndex = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);
        if (ul_XenonFileIndex != BIG_C_InvalidIndex)
        {
            // Build the .tex path name
            strcpy(szTexPath, BASE_TEXTURE_PATH);
            strcat(szTexPath, szExtraPath);

            // Build the .tex file name
            strcpy(szTexFileName, szBigFileNameLwr);
            if (strstr(szTexFileName, "_xe_c.tga") != NULL)
            {
                bool bFoundTEX = false;

                *strstr(szTexFileName, "_xe_c.tga") = 0;
                strcat(szTexFileName, ".tex");

                // Open the .tex file and update the Xenon information
                ul_File = BIG_ul_SearchDir(szTexPath);
                if (ul_File != BIG_C_InvalidIndex)
                    ul_File = BIG_ul_SearchFile(ul_File, szTexFileName);
                if (ul_File != BIG_C_InvalidIndex)
                {
                    bFoundTEX = true;

                    TEX_tdst_File_Desc stTexFileInfo;
                    L_memset(&stTexFileInfo, 0, sizeof(TEX_tdst_File_Desc));

                    if (TEX_l_File_GetContent( BIG_FileKey(ul_File), &stTexFileInfo ))
                    {
                        stTexFileInfo.st_Tex.st_XeProperties.ul_OriginalTexture = BIG_FileKey(ul_XenonFileIndex);
                        stTexFileInfo.st_Tex.st_XeProperties.ul_OutputWidth     = st_TextureParams.uw_Width;
                        stTexFileInfo.st_Tex.st_XeProperties.ul_OutputHeight    = st_TextureParams.uw_Height;
                        stTexFileInfo.st_Tex.st_XeProperties.ul_NbLevels        = 0;

                        // Update the .tex file and we're done
                        TEX_ul_File_SaveTexWithIndex(ul_File, &stTexFileInfo.st_Tex);

                        CHAR szMessage[2048];
                        sprintf(szMessage, "Import File - Diffuse map - Updated %s/%s", szTexPath, szTexFileName);
                        LINK_PrintStatusMsg(szMessage);
                    }
                }

                if (!bFoundTEX)
                {
                    CHAR szNewTexName[256];
                    strcpy(szNewTexName, _psz_BigFileName);
                    *strrchr(szNewTexName, '.') = 0;
                    strcat(szNewTexName, ".tex");

                    TEX_tdst_File_Tex stTexFileInfo;
                    L_memset(&stTexFileInfo, 0xff, sizeof(TEX_tdst_File_Tex));

                    TEX_XeResetProperties(&stTexFileInfo.st_XeProperties);

                    stTexFileInfo.st_XeProperties.ul_OriginalTexture = BIG_FileKey(ul_XenonFileIndex);
                    stTexFileInfo.st_XeProperties.ul_OutputWidth     = st_TextureParams.uw_Width;
                    stTexFileInfo.st_XeProperties.ul_OutputHeight    = st_TextureParams.uw_Height;
                    stTexFileInfo.st_XeProperties.ul_NbLevels        = 0;
                    stTexFileInfo.st_XeProperties.ul_Flags           = 0;

                    // Update the .tex file and we're done
                    TEX_ul_File_SaveTexWithName(_psz_BigPathName, szNewTexName, &stTexFileInfo, FALSE);

                    CHAR szMessage[2048];
                    sprintf(szMessage, "Import File - Diffuse map - Created %s/%s", _psz_BigPathName, szNewTexName);
                    LINK_PrintStatusMsg(szMessage);
                }
            }
        }
    }
    else if ((strstr(_psz_BigPathName, XENON_TEXTURE_PATH) == _psz_BigPathName) &&
             (strstr(szBigFileNameLwr, "_xe_n.tga") != NULL))
    {
        // Create a .tex if we imported a normal map

        ULONG ul_XenonFileIndex = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);
        if (ul_XenonFileIndex != BIG_C_InvalidIndex)
        {
            CHAR szNewTexName[256];
            strcpy(szNewTexName, _psz_BigFileName);
            *strrchr(szNewTexName, '.') = 0;
            strcat(szNewTexName, ".tex");

            TEX_tdst_File_Tex stTexFileInfo;
            L_memset(&stTexFileInfo, 0xff, sizeof(TEX_tdst_File_Tex));

            TEX_XeResetProperties(&stTexFileInfo.st_XeProperties);

            stTexFileInfo.st_XeProperties.ul_OriginalTexture = BIG_FileKey(ul_XenonFileIndex);
            stTexFileInfo.st_XeProperties.ul_OutputWidth     = st_TextureParams.uw_Width;
            stTexFileInfo.st_XeProperties.ul_OutputHeight    = st_TextureParams.uw_Height;
            stTexFileInfo.st_XeProperties.ul_NbLevels        = 0;
            stTexFileInfo.st_XeProperties.ul_Flags           = TEX_Xe_IsNormalMap;

            // Update the .tex file and we're done
            TEX_ul_File_SaveTexWithName(_psz_BigPathName, szNewTexName, &stTexFileInfo, FALSE);

            CHAR szMessage[2048];
            sprintf(szMessage, "Import File - Normal map - Created %s/%s", _psz_BigPathName, szNewTexName);
            LINK_PrintStatusMsg(szMessage);
        }
    }
    else if ((strstr(_psz_BigPathName, XENON_TEXTURE_PATH) == _psz_BigPathName) &&
             (strstr(szBigFileNameLwr, "_xe_s.tga") != NULL))
    {
        // Create a .tex if we imported a specular map

        ULONG ul_XenonFileIndex = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);
        if (ul_XenonFileIndex != BIG_C_InvalidIndex)
        {
            CHAR szNewTexName[256];
            strcpy(szNewTexName, _psz_BigFileName);
            *strrchr(szNewTexName, '.') = 0;
            strcat(szNewTexName, ".tex");

            TEX_tdst_File_Tex stTexFileInfo;
            L_memset(&stTexFileInfo, 0xff, sizeof(TEX_tdst_File_Tex));

            TEX_XeResetProperties(&stTexFileInfo.st_XeProperties);

            stTexFileInfo.st_XeProperties.ul_OriginalTexture = BIG_FileKey(ul_XenonFileIndex);
            stTexFileInfo.st_XeProperties.ul_OutputWidth     = st_TextureParams.uw_Width;
            stTexFileInfo.st_XeProperties.ul_OutputHeight    = st_TextureParams.uw_Height;
            stTexFileInfo.st_XeProperties.ul_NbLevels        = 0;
            stTexFileInfo.st_XeProperties.ul_Flags           = 0;
            stTexFileInfo.st_XeProperties.ul_OutputFormat    = TEX_XTF_A8;

            // Update the .tex file and we're done
            TEX_ul_File_SaveTexWithName(_psz_BigPathName, szNewTexName, &stTexFileInfo, FALSE);

            CHAR szMessage[2048];
            sprintf(szMessage, "Import File - Specular map (Alpha only) - Created %s/%s", _psz_BigPathName, szNewTexName);
            LINK_PrintStatusMsg(szMessage);
        }
    }
    // SC: Uncomment for automatic .tex conversion when a .tga is imported
    /*
    else if (strstr(szBigFileNameLwr, ".tga") != NULL)
    {
        // Create a .tex if we imported a .tga

        ULONG ul_XenonFileIndex = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);
        if (ul_XenonFileIndex != BIG_C_InvalidIndex)
        {
            CHAR szNewTexName[256];
            strcpy(szNewTexName, _psz_BigFileName);
            *strrchr(szNewTexName, '.') = 0;
            strcat(szNewTexName, ".tex");

            TEX_tdst_File_Tex stTexFileInfo;
            L_memset(&stTexFileInfo, 0xff, sizeof(TEX_tdst_File_Tex));

            TEX_XeResetProperties(&stTexFileInfo.st_XeProperties);

            stTexFileInfo.st_XeProperties.ul_OriginalTexture = BIG_FileKey(ul_XenonFileIndex);
            stTexFileInfo.st_XeProperties.ul_OutputWidth     = st_TextureParams.uw_Width;
            stTexFileInfo.st_XeProperties.ul_OutputHeight    = st_TextureParams.uw_Height;
            stTexFileInfo.st_XeProperties.ul_NbLevels        = 0;
            stTexFileInfo.st_XeProperties.ul_Flags           = 0;

            // Update the .tex file and we're done
            TEX_ul_File_SaveTexWithName(_psz_BigPathName, szNewTexName, &stTexFileInfo, FALSE);

            CHAR szMessage[2048];
            sprintf(szMessage, "Import File - Special - Created %s/%s", _psz_BigPathName, szNewTexName);
            LINK_PrintStatusMsg(szMessage);
        }
    }
    */
#endif
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_Init(void)
{
	IMP_b_AddImportCallback("tga", TEX_b_File_Import);
	IMP_b_AddImportCallback("jpg", TEX_b_File_Import);
	IMP_b_AddImportCallback("bmp", TEX_b_File_Import);
	IMP_b_AddImportCallback("pal", TEX_b_File_Import);
#if defined(_XENON_RENDER)
    IMP_b_AddImportCallback("dds", TEX_b_File_Import);
#endif
}

/*
 =======================================================================================================================
    i_Type indique le type de données voulues
        -1 (textures) toutes sauf palette et raw
        0   raw
        1   palette
        2   texture (seulement texture pure => pas de spr, pro ou tex)
		3	font descriptor
		4	file type Drag'n dropable dans l'éditeur de texture (depuis un autre éditeur )
		5	file type Drag'n dropable dans l'éditeur de texture (depuis le meme éditeur )
 =======================================================================================================================
 */
LONG TEX_l_File_IsFormatSupported(char *_psz_Filename, int i_Type)
{
	/*~~~~~~~~~~~~~*/
	char	*psz_Ext;
	/*~~~~~~~~~~~~~*/

	psz_Ext = strrchr(_psz_Filename, '.');
	if(psz_Ext == NULL) return 0;
    if (i_Type == -1)
    {
	    if(L_stricmp(psz_Ext, ".tga") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".bmp") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".jpg") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".spr") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".pro") == 0) return 1;
        if(L_stricmp(psz_Ext, ".ant") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".tex") == 0) return 1;
 #if defined(_XENON_RENDER)
        if(L_stricmp(psz_Ext, ".dds") == 0) return 1;
#endif
       return 0;
    }

    if (i_Type == 0)
    {
	    if(L_stricmp(psz_Ext, ".raw") == 0) return 1;
        return 0;
    }

    if (i_Type == 1)
    {
	    if(L_stricmp(psz_Ext, ".pal") == 0) return 1;
        return 0;
    }

    if (i_Type == 2)
    {
	    if(L_stricmp(psz_Ext, ".tga") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".bmp") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".jpg") == 0) return 1;
        return 0;
    }

    if (i_Type == 3)
    {
        if (L_stricmp( psz_Ext, ".fod") == 0) return 1;
        return 0;
    }

	if (i_Type == 4)
	{
		if(L_stricmp(psz_Ext, ".tga") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".bmp") == 0) return 1;
	    if(L_stricmp(psz_Ext, ".jpg") == 0) return 1;
		if(L_stricmp(psz_Ext, ".tex") == 0) return 1;
		if(L_stricmp(psz_Ext, ".raw") == 0) return 1;
		if(L_stricmp(psz_Ext, ".pal") == 0) return 1;
#if defined(_XENON_RENDER)
        if(L_stricmp(psz_Ext, ".dds") == 0) return 1;
#endif
	}

	if (i_Type == 5)
	{
		if(L_stricmp(psz_Ext, ".tex") == 0) return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_File_DefaultParams
(
	TEX_tdst_File_Params	*_pst_FP,
	char					*_sz_Path,
	char					*_sz_Name,
	char					*_pc_Buffer,
	LONG					_l_Size
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*_psz_Ext;
	TEX_tdst_File_Desc	st_Desc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	TEX_File_InitParams(_pst_FP);

    L_memset( &st_Desc, 0, sizeof( TEX_tdst_File_Desc ) );
	st_Desc.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;

	_psz_Ext = strrchr(_sz_Name, '.');
	if(_psz_Ext)
	{
		_psz_Ext++;
		if(L_stricmp(_psz_Ext, "tga") == 0)
		{
			if(TEX_l_File_LoadTga(_pc_Buffer, &st_Desc)) _pst_FP->uc_Type = TEX_FP_TgaFile;
		}
		else if(L_stricmp(_psz_Ext, "bmp") == 0)
		{
			if(TEX_l_File_LoadBmp(_pc_Buffer, &st_Desc)) _pst_FP->uc_Type = TEX_FP_BmpFile;
		}
		else if(L_stricmp(_psz_Ext, "jpg") == 0)
		{
			if(TEX_l_File_LoadJpeg(_pc_Buffer, &st_Desc, _l_Size)) _pst_FP->uc_Type = TEX_FP_JpgFile;
		}
#if defined(_XENON_RENDER)
        else if(L_stricmp(_psz_Ext, "dds") == 0)
        {
            if(TEX_l_File_LoadDDS(_pc_Buffer, &st_Desc, _l_Size)) _pst_FP->uc_Type = TEX_FP_XeDDSFile;
        }
#endif
	}

	if(_pst_FP->uc_Type)
	{
		/* test for a palette */
		if(st_Desc.p_TmpPalette != NULL)
		{
            TEX_ul_File_CreateTexRawPal( _sz_Path, _sz_Name, &st_Desc );
            if ( st_Desc.p_Bitmap )
				TEX_M_File_Free(st_Desc.p_Bitmap);
			return 0;
		}
        
		_pst_FP->uw_Width = st_Desc.uw_Width;
		_pst_FP->uw_Height = st_Desc.uw_Height;
		_pst_FP->uc_Format = (st_Desc.uc_BPP == 32) ? TEX_FP_32bpp : TEX_FP_24bpp;

        if (st_Desc.p_Bitmap)
            TEX_M_File_Free(st_Desc.p_Bitmap);
	}

	_pst_FP->uw_Flags = TEX_FP_QualityMedium | TEX_FP_MipmapOn;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_GetParams(TEX_tdst_File_Params *_pst_FP, BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Offset;
	ULONG	ul_Size;
	char	*pc_Buffer;
	char	sz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if ( TEX_l_File_IsFormatSupported( BIG_NameFile( _ul_Index ), 1 ) )
		return;

	/* Ul_Offset = (BIG_LengthDiskFile(_ul_Index) + 4) - 32; */
	ul_Size = BIG_ul_GetLengthFile(BIG_PosFile(_ul_Index));
	ul_Offset = (ul_Size + 4) - 32;

	BIG_ReadNoSeek(BIG_PosFile(_ul_Index) + ul_Offset, _pst_FP, 32);

	/* change old format to new format */
	if((_pst_FP->ul_Params[3] == 0) && (_pst_FP->ul_Params[2] == 0))
	{
		pc_Buffer = (char*)L_malloc(ul_Size);
		BIG_ReadNoSeek(BIG_PosFile(_ul_Index) + 4, pc_Buffer, ul_Size);

		BIG_ComputeFullName(BIG_ParentFile(_ul_Index), sz_Path);
		TEX_i_File_DefaultParams(_pst_FP, sz_Path, BIG_NameFile(_ul_Index), pc_Buffer, ul_Size);
		TEX_File_SetParams(_pst_FP, _ul_Index, FALSE);

		L_free(pc_Buffer);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_SetParams(TEX_tdst_File_Params *_pst_FP, BIG_INDEX _ul_Index, BOOL _b_Key)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	    ul_Offset;
    L_time_t    x_Time;
	int i;
	/*~~~~~~~~~~~~~~*/

	_pst_FP->ul_Mark = 0xFFFFFFFF;
	_pst_FP->ul_Params[1] = 0xCAD01234;
	_pst_FP->ul_Params[2] = 0x00FF00FF;
	_pst_FP->ul_Params[3] = 0xC0DEC0DE;

	if(_b_Key) _ul_Index = BIG_ul_SearchKeyToFat(_ul_Index);

	ul_Offset = BIG_PosFile(_ul_Index) + BIG_ul_GetLengthFile(BIG_PosFile(_ul_Index)) + 4 - 32;
	i = L_fseek(BIG_Handle(), ul_Offset, L_SEEK_SET);
	ERR_X_Error( i == 0, L_ERR_Csz_FSeek, NULL);
	i = BIG_fwrite((UCHAR *) _pst_FP, 32, BIG_Handle());
	ERR_X_Error( i == 1, L_ERR_Csz_FWrite, NULL);

    L_time(&x_Time);
    BIG_TimeFile( _ul_Index ) = x_Time;
    BIG_UpdateOneFileInFat( _ul_Index );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_File_InitParams(TEX_tdst_File_Params *_pst_FP)
{
	_pst_FP->ul_Mark = 0xFFFFFFFF;
	_pst_FP->uw_Flags = 0;
	_pst_FP->uc_Type = 0;
	_pst_FP->uc_Format = 0;
	_pst_FP->uw_Height = 0;
	_pst_FP->uw_Width = 0;
	_pst_FP->ul_Params[0] = 0;
	_pst_FP->ul_Params[1] = 0xCAD01234;
	_pst_FP->ul_Params[2] = 0x00FF00FF;
	_pst_FP->ul_Params[3] = 0xC0DEC0DE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL TEX_b_File_GetName(int _i_Index, char *_sz_Name )
{
    BIG_KEY ul_Key;

    *_sz_Name = 0;
    if ( (_i_Index < 0) || (_i_Index >= TEX_gst_GlobalList.l_NumberOfTextures) )
        return FALSE;

    ul_Key = TEX_gst_GlobalList.dst_Texture[ _i_Index ].ul_Key;
    if (ul_Key == BIG_C_InvalidKey)
        return FALSE;

    ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
    if (ul_Key == BIG_C_InvalidIndex )
        return FALSE;
            
    strcpy( _sz_Name, BIG_NameFile(ul_Key));
    return TRUE;
}


#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_MemoryNeeded(TEX_tdst_File_Desc *_pst_FD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_Size, l_SizeRef, l_Ref;
extern BOOL EDI_gb_ComputeMap;
extern BOOL WOR_gi_CurrentConsole;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Size = (LONG) _pst_FD->uw_CompressedHeight * (LONG) _pst_FD->uw_CompressedWidth;
    l_Size = (l_Size * _pst_FD->uc_FinalBPP) >> 3;

	if(!(_pst_FD->st_Params.uw_Flags & TEX_FP_MipmapOn)) return l_Size;
#ifdef ACTIVE_EDITORS
	if(EDI_gb_ComputeMap && WOR_gi_CurrentConsole == 1) return l_Size;
#endif

	l_SizeRef = l_Size;
	l_Ref = _pst_FD->uw_CompressedHeight;
	if(_pst_FD->uw_CompressedWidth < l_Ref) l_Ref = _pst_FD->uw_CompressedWidth;

	while(l_Ref >>= 1)
	{
		l_SizeRef >>= 2;
		l_Size += l_SizeRef;
	}

	while(l_SizeRef > 4)
	{
		l_SizeRef >>= 1;
		l_Size += l_SizeRef;
	}

	return l_Size;
}

#ifdef ACTIVE_EDITORS
BOOL TEX_b_ImportCallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*pc_Buffer;
	ULONG				l_Size;
	L_FILE				h_File;
	char				*sz_Ext;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_File = L_fopen(_psz_ExternName, "rb");
	if(!CLI_FileOpen(h_File)) return 0;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    read the file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	L_fseek(h_File, 0, SEEK_END);
	l_Size = L_ftell(h_File);
	L_fseek(h_File, 0, SEEK_SET);

	pc_Buffer = (char *) L_malloc(l_Size);
	if(pc_Buffer == NULL)
	{
		/*~~~~~~~~~~~~~*/
		char	az[1024];
		/*~~~~~~~~~~~~~*/

		sprintf(az, "There no enougth memory to import the file %s", _psz_BigFileName);
		MessageBox(NULL, az, "File Import Failed !!", MB_OK | MB_ICONSTOP);
		ERR_X_Warning(0, az, NULL);
		L_fclose(h_File);
		return 0;
	}

	L_fread(pc_Buffer, 1, l_Size, h_File);
	L_fclose(h_File);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Put extension into lower case
	 -------------------------------------------------------------------------------------------------------------------
	 */

	sz_Ext = strrchr(_psz_BigFileName, '.');
	if(sz_Ext)
	{
		while(*(++sz_Ext)) *sz_Ext = tolower(*sz_Ext);
	}
	/* save file */
    BIG_UpdateFileFromBuffer(_psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size);
    L_free(pc_Buffer);
    return 0;
}
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
