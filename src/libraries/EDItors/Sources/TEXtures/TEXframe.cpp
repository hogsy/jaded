/*$T TEXframe.cpp GC! 1.081 05/30/01 08:57:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLibrary/CLIMEM.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"

#define ACTION_GLOBAL

#include "TEXframe_act.h"
#include "TEXscroll.h"
#include "TEXframe.h"
#include "TEXutil.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "ENGine/Sources/ENGinit.h"
#include "GraphicDK/Sources/TEXture/TEXfile.h"
#include "GraphicDK/Sources/TEXture/TEXconvert.h"
#include "GraphicDK/Sources/TEXture/TEXeditorfct.h"
#include "DIAlogs/DIAchecklist_dlg.h"
#include "VAVview/VAVview.h"

#ifdef JADEFUSION
#include "GraphicDK/Sources/TEXture/TEXxenon.h"
#endif
#if defined(_XENON_RENDER)
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
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

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(ETEX_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(ETEX_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEX_cl_Frame::ETEX_cl_Frame(void)
{
	mul_DirIndex = BIG_C_InvalidIndex;
	mul_FileIndex = BIG_C_InvalidIndex;
	mpo_Splitter = new CSplitterWnd;
	mpo_DataView = new EVAV_cl_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEX_cl_Frame::~ETEX_cl_Frame(void)
{
	mpo_Splitter->DestroyWindow();
	delete mpo_Splitter;

	if ( mpo_DataView )
	{
		mpo_DataView->ResetList();
		delete mpo_DataView;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEX_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CCreateContext	o_Context;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_ScrollView = NULL;
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1)
	{
		return -1;
	}


	/* create splitter view */
	mpo_Splitter->CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE);

	mpo_Splitter->CreateView(0, 1, RUNTIME_CLASS(ETEX_cl_InsideScroll), CSize(0, 0), NULL);
	mpo_ScrollView = ( ETEX_cl_InsideScroll * ) mpo_Splitter->GetPane(0, 1);

	mpo_DataView->mb_CanDragDrop = FALSE;
	mst_DataView.po_ListItems = &mo_ListItems;
	mst_DataView.psz_NameCol1 = "Name";
	mst_DataView.i_WidthCol1 = 100;
	mst_DataView.psz_NameCol2 = "Value";
	mpo_DataView->MyCreate(mpo_Splitter, &mst_DataView, this, mpo_Splitter->IdFromRowCol(0, 0));
	mpo_DataView->ShowWindow(SW_SHOW);
	mpo_Splitter->SetColumnInfo( 0, 200, 200 );
	mpo_ScrollView->mul_Selected = BIG_C_InvalidIndex;
	mpo_ScrollView->mpo_DialogBar = mpo_DialogBar;
	mpo_ScrollView->mpo_Parent = this;
	mpo_ScrollView->mo_Size = CSize(1, 1);
	mpo_ScrollView->SetScrollSizes(MM_TEXT, CSize(1, 1));

	/* Redraw window */
	RecalcLayout();

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	/* Move statics inside dialog */
	M_MF()->SizePanesDialogBar(this);

	/* Change scroll view size */
	if(mpo_ScrollView)
	{
		mpo_ScrollView->Invalidate();
		mpo_ScrollView->SendMessage(WM_PAINT);
		mpo_ScrollView->SetScrollSizes(MM_TEXT, mpo_ScrollView->mo_Size);
	}
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::DeleteList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION					pos;
	MAIEDITEX_tdst_BitmapDes	*pst_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mpo_ScrollView->mo_BmpList.GetHeadPosition();
	while(pos)
	{
		pst_Des = mpo_ScrollView->mo_BmpList.GetNext(pos);
		DeleteObject(pst_Des->st_Bmp.h_Bitmap);
		if(pst_Des->st_Bmp.h_BitmapAlpha) DeleteObject(pst_Des->st_Bmp.h_BitmapAlpha);
		delete pst_Des;
	}

	mpo_ScrollView->mo_BmpList.RemoveAll();
	mpo_ScrollView->SetScrollSizes(MM_TEXT, CSize(0, 0));

	mpo_DialogBar->GetDlgItem(IDC_PATH)->SetWindowText("");
	mpo_DialogBar->GetDlgItem(IDC_FILE)->SetWindowText("");
	mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText("");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if 0
void ETEX_cl_Frame::ConvertBitmap(BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
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
		return;
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
	pdc = mpo_ScrollView->GetDC();
	pst_Des->st_Bmp.h_Bitmap = CreateDIBitmap
		(
			pdc->m_hDC,
			pst_Info,
			CBM_INIT,
			p_Bits,
			(BITMAPINFO *) pst_Info,
			DIB_RGB_COLORS
		);

	ReleaseDC(pdc);

	/* Add to list */
	mpo_ScrollView->mo_BmpList.AddTail(pst_Des);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::ConvertRaw
(
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
	pdc = mpo_ScrollView->GetDC();
	pst_Des->st_Bmp.h_Bitmap = CreateDIBSection(pdc->m_hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawBits, NULL, 0);

	ReleaseDC(pdc);
	if(pst_Des->st_Bmp.h_Bitmap == NULL)
	{
		delete pst_Des;
		return;
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
		pdc = mpo_ScrollView->GetDC();
		pst_Des->st_Bmp.h_BitmapAlpha = CreateDIBSection
			(
				pdc->m_hDC,
				&st_BitmapInfo,
				ui_Usage,
				(void **) &p_RawBits1,
				NULL,
				0
			);

		ReleaseDC(pdc);

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
	mpo_ScrollView->mo_BmpList.AddTail(pst_Des);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::ConvertJPG(BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf, ULONG ulLenght)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
	st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	TEX_l_File_LoadJpeg((char *) _puc_Buf, &st_Des, ulLenght);
	ConvertRaw
	(
		_ul_FatFile,
		(UCHAR *) st_Des.p_Bitmap,
		(UCHAR *) st_Des.p_Palette,
		st_Des.uw_Width,
		st_Des.uw_Height,
		st_Des.uc_BPP,
		MAIEDITEX_C_JPG
	);
	MEM_Free(st_Des.p_Bitmap);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::ConvertTGA(BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
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
		return;
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

	ConvertRaw
	(
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
void ETEX_cl_Frame::ConvertPalette(BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
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
	if(st_Palette.uc_Flags & TEX_uc_InvalidPalette) return;

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

	ConvertRaw(_ul_FatFile, (UCHAR *) pul_Buffer, NULL, 64, 64, 32, MAIEDITEX_C_PALETTE);
	MEM_Free(pul_Buffer);
    TEX_M_File_Free( st_Palette.pul_Color );

	pst_Des = mpo_ScrollView->mo_BmpList.GetTail();
	//pst_Des->st_Tex.ul_Palette = st_Palette.uc_Flags;
	pst_Des->ul_PaletteFlags = st_Palette.uc_Flags;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::ConvertRawFile(BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
	st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	TEX_l_File_GetInfoAndContent(BIG_FileKey(_ul_FatFile), &st_Des);

	ConvertRaw
	(
		_ul_FatFile,
		(UCHAR *) st_Des.p_Bitmap,
		(UCHAR *) st_Des.p_Palette,
		st_Des.uw_Width,
		st_Des.uw_Height,
		st_Des.uc_BPP,
		MAIEDITEX_C_RAW
	);
	MEM_Free(st_Des.p_Bitmap);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::ConvertTex(BIG_INDEX _ul_FatFile, UCHAR *_puc_Buf, ULONG _ul_Size, MAIEDITEX_tdst_BitmapDes *pDes)
{
#ifdef JADEFUSION
#define NB_ROWS 5
#else
#define NB_ROWS 4
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc			st_Des;
	TEX_tdst_Palette			st_Palette;
	MAIEDITEX_tdst_BitmapDes	*pst_Des;
	BOOL						b_Alpha;
	short						w, h;
	BITMAPINFO					st_BitmapInfo;
	UINT						ui_Usage;
	CDC							*pdc, dc;
	UCHAR						*p_RawBits, *p_RawBits1, *p_RawBits2;
	RECT						st_Rect;
	CBitmap						*oldBmp, oBmp;
	int							i, j, i_Result, i_Slot;
	CFont						*oldFnt;
	ULONG						ul_Key, ul_Index, ul_ShowFlag;
	unsigned char				*puc_Tgt, *puc_Src;
	int							LineWidth, LinePitch;
	TEX_tdst_File_Desc			st_TexDesc;

#ifdef JADEFUSION
	int							W[NB_ROWS][2], H[NB_ROWS][2], row[ NB_ROWS ], col[ 2 ], x, y, sloty[ NB_ROWS ];
    char						sz_Etiket[NB_ROWS][2][20] = { {"PS2 (Raw)", "PS2 (TC)" }, {"GC (Raw)", "GC (TC)" }, {"XBOX (Raw)", "XBOX (TC)" }, {"PC (Raw)", "PC (TC)" }, {"Xenon (Original)", "Xenon (Native)" } };
#else
	int							W[4][2], H[4][2], row[ 4 ], col[ 2 ], x, y, sloty[ 4 ];
	char						sz_Etiket[4][2][12] = { {"PS2 (Raw)", "PS2 (TC)" }, {"GC (Raw)", "GC (TC)" }, {"XBOX (Raw)", "XBOX (TC)" }, {"PC (Raw)", "PC (TC)" } };
#endif
	char						c_SlotOrder[ 4 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_ShowFlag = mst_Ini.l_ShowFlag;
	c_SlotOrder[ 0 ] = 0;
	c_SlotOrder[ 1 ] = 1;
	c_SlotOrder[ 2 ] = 2;
	c_SlotOrder[ 3 ] = 3;
	if ( mst_Ini.l_ShowFlag & ETEX_C_TexShowAllSlot )
		ul_ShowFlag |= ETEX_C_TexShowRawPal | ETEX_C_TexShowTga | ETEX_C_TexShowSlotMask;
	/*
	if ( mst_Ini.l_ShowFlag & ETEX_C_TexUseTMSlotOrder ) 
	{
		EOUT_cl_Frame		*po_3DView;
		TEX_tdst_Manager	*pst_TM;
		po_3DView = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		pst_TM = &po_3DView->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->st_TexManager;
		c_SlotOrder[ 0 ] = pst_TM->c_TexSlotOrder[ 0 ];
		c_SlotOrder[ 1 ] = pst_TM->c_TexSlotOrder[ 1 ];
		c_SlotOrder[ 2 ] = pst_TM->c_TexSlotOrder[ 2 ];
		c_SlotOrder[ 3 ] = pst_TM->c_TexSlotOrder[ 3 ];
	}
	*/

	if ((ul_ShowFlag & 0xF00)== 0)
		ul_ShowFlag |= ETEX_C_TexShowRawPal | ETEX_C_TexShowTga;
	if ((ul_ShowFlag & 0xF000)== 0)
		ul_ShowFlag |= ETEX_C_TexShowSlotMask;

	if(pDes == NULL)
	{
		pst_Des = new MAIEDITEX_tdst_BitmapDes;
		pst_Des->i_Type = MAIEDITEX_C_TEX;
		pst_Des->ul_FatFile = _ul_FatFile;
	}
	else
	{
		pst_Des = pDes;
		DeleteObject(pst_Des->st_Bmp.h_Bitmap);
		if(pst_Des->st_Bmp.h_BitmapAlpha) DeleteObject(pst_Des->st_Bmp.h_BitmapAlpha);
		_ul_FatFile = pDes->ul_FatFile;
	}

	/* init */
	b_Alpha = FALSE;

	/* get Tex file content */
#ifdef JADEFUSION
	L_memset(&st_TexDesc, 0, sizeof(TEX_tdst_File_Desc));
#endif
	TEX_l_File_GetInfoAndContent( BIG_FileKey( _ul_FatFile ), &st_TexDesc );
	L_memcpy( &pst_Des->st_Tex, &st_TexDesc.st_Tex, sizeof( TEX_tdst_File_Tex ) );

	/* get size of data */
	L_memset( &W, 0, sizeof( W ) );
	L_memset( &H, 0, sizeof( H ) );
	L_memset( &st_Des, 0, sizeof(TEX_tdst_File_Desc));
	for ( i_Slot = 0; i_Slot < 4; i_Slot++ )
	{
		if ( ! (ul_ShowFlag & (ETEX_C_TexShowSlotPS2 << i_Slot ) ) )
			continue;
		
		ul_Key = pst_Des->st_Tex.ast_Slot[ i_Slot ].ul_Raw;
		if ( !(ul_ShowFlag & ETEX_C_TexShowRawPal) )
			ul_Key = BIG_C_InvalidKey;

		if ( ul_Key != BIG_C_InvalidKey)
		{
			ul_Index = BIG_ul_SearchKeyToFat( ul_Key );
			if(ul_Index == BIG_C_InvalidIndex)
			{
				W[ i_Slot ][ 0 ] = -1;
			}
			else
			{
				st_Des.uw_DescFlags = TEX_Cuw_DF_Info;
				i_Result = TEX_l_File_GetInfoAndContent( ul_Key, &st_Des);
				if ( !i_Result )
					W[ i_Slot ][ 0 ] = -2;
				else
				{
					TEX_File_FreeDescription( &st_Des );
					W[ i_Slot ][ 0 ] = st_Des.uw_Width;
					H[ i_Slot ][ 0 ] = st_Des.uw_Height;

					if (!b_Alpha)
					{
						ul_Key = pst_Des->st_Tex.ast_Slot[ i_Slot ].ul_Pal;
						if( ul_Key != BIG_C_InvalidKey )
						{
							L_memset( &st_Palette, 0, sizeof( TEX_tdst_Palette ) );
							st_Palette.ul_Key = ul_Key;
							TEX_File_LoadPalette(&st_Palette, FALSE);
							if ( st_Palette.uc_Flags & TEX_uc_AlphaPalette )
								b_Alpha = TRUE;
							if (st_Palette.pul_Color) 
								TEX_M_File_Free(st_Palette.pul_Color);
						}
					}
				}
			}
		}

		ul_Key = pst_Des->st_Tex.ast_Slot[ i_Slot ].ul_TC;
		if ( !(ul_ShowFlag & ETEX_C_TexShowTga) )
			ul_Key = BIG_C_InvalidKey;
		if( ul_Key != BIG_C_InvalidKey )
		{
			ul_Index = BIG_ul_SearchKeyToFat( ul_Key  );
			if( ul_Index == BIG_C_InvalidIndex )
				W[ i_Slot ][ 1 ] = -1;
			else
			{
				st_Des.uw_DescFlags = TEX_Cuw_DF_Info;
				i_Result = TEX_l_File_GetInfoAndContent( ul_Key, &st_Des );
				if (!i_Result)
					W[ i_Slot ][ 1 ] = -2;
				else if(st_Des.st_Params.uc_Type == TEX_FP_RawPalFile)
					W[ i_Slot ][ 1 ] = -3;
				else
				{
					W[ i_Slot ][ 1 ] = st_Des.uw_Width;
					H[ i_Slot ][ 1 ] = st_Des.uw_Height;
					if(st_Des.uc_BPP == 32) 
						b_Alpha = TRUE;
				}
			}
		}
	}
#if defined(_XENON_RENDER)
    // Original texture
    if (GDI_b_IsXenonGraphics() && (pst_Des->st_Tex.st_XeProperties.ul_OriginalTexture != BIG_C_InvalidKey))
    {
        ul_Index = BIG_ul_SearchKeyToFat(pst_Des->st_Tex.st_XeProperties.ul_OriginalTexture);
        if (ul_Index != BIG_C_InvalidIndex)
        {
            st_Des.uw_DescFlags = TEX_Cuw_DF_Info;
            i_Result = TEX_l_File_GetInfoAndContent(pst_Des->st_Tex.st_XeProperties.ul_OriginalTexture, &st_Des);
            if (i_Result)
            {
                TEX_File_FreeDescription(&st_Des);
                W[4][0] = st_Des.uw_Width;
                H[4][0] = st_Des.uw_Height;

                b_Alpha = TRUE;

                // Automatic compression
                if (pst_Des->st_Tex.st_XeProperties.ul_NativeTexture == BIG_C_InvalidKey)
                {
                    W[4][1] = pst_Des->st_Tex.st_XeProperties.ul_OutputWidth;
                    H[4][1] = pst_Des->st_Tex.st_XeProperties.ul_OutputHeight;
                }
            }
        }
    }

    // Native texture
    if (GDI_b_IsXenonGraphics() && (pst_Des->st_Tex.st_XeProperties.ul_NativeTexture != BIG_C_InvalidKey))
    {
        ul_Index = BIG_ul_SearchKeyToFat(pst_Des->st_Tex.st_XeProperties.ul_NativeTexture);
        if (ul_Index != BIG_C_InvalidIndex)
        {
            st_Des.uw_DescFlags = TEX_Cuw_DF_Info;
            i_Result = TEX_l_File_GetInfoAndContent(pst_Des->st_Tex.st_XeProperties.ul_NativeTexture, &st_Des);
            if (i_Result)
            {
                TEX_File_FreeDescription(&st_Des);
                W[4][1] = st_Des.uw_Width;
                H[4][1] = st_Des.uw_Height;

                b_Alpha = TRUE;
            }
        }
    }
#endif

	/* compute size of finale texture */
	col[0] = col[1] = 0;
	h = 0;
	for (i_Slot = 0; i_Slot < NB_ROWS; i_Slot++)
	{
		if ( W[i_Slot][0] > col[0] ) col[0] = W[i_Slot][0];
		if ( W[i_Slot][1] > col[1] ) col[1] = W[i_Slot][1];

		row[ i_Slot ] = __max( H[i_Slot][0], H[i_Slot][1] );
		if ( row[ i_Slot ] == 0)
			sloty[ i_Slot ] = -1;
		else
		{
			sloty[ i_Slot ] = h;
			h += 12 + row[ i_Slot ];
		}
	}

	w = col[0] + 4 + col[1];

	/* invert y coordinates */
	for (i_Slot = 0; i_Slot < NB_ROWS; i_Slot++ )
	{
		if( sloty[ i_Slot ] != -1 ) 
			sloty[ i_Slot ] = h - sloty[ i_Slot ];
	}

	/* Bitmap info header */
	st_BitmapInfo.bmiHeader.biWidth = w;
	st_BitmapInfo.bmiHeader.biHeight = h;
	st_BitmapInfo.bmiHeader.biPlanes = 1;
	st_BitmapInfo.bmiHeader.biBitCount = 24;
	st_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	st_BitmapInfo.bmiHeader.biCompression = BI_RGB;
	st_BitmapInfo.bmiHeader.biSizeImage = 0;
	st_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	st_BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	st_BitmapInfo.bmiHeader.biClrUsed = 0;
	st_BitmapInfo.bmiHeader.biClrImportant = 0;

	ui_Usage = DIB_RGB_COLORS;
	pdc = mpo_ScrollView->GetDC();
	pst_Des->st_Bmp.h_Bitmap = CreateDIBSection(pdc->m_hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawBits, NULL, 0);
	pst_Des->st_Bmp.b_Compress = FALSE;
	L_memcpy(&pst_Des->st_Bmp.st_Header, &st_BitmapInfo, sizeof(BITMAPINFOHEADER));

	if(pst_Des->st_Bmp.h_Bitmap == NULL)
	{
		ReleaseDC(pdc);
		delete pst_Des;
		return;
	}

	/* alpha bitmap */
	pst_Des->st_Bmp.h_BitmapAlpha = NULL;
	p_RawBits1 = NULL;
	if (b_Alpha)
		pst_Des->st_Bmp.h_BitmapAlpha = CreateDIBSection( pdc->m_hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawBits1, NULL, 0 );

	/* copy bitmap */
	for ( i_Slot = 0; i_Slot < 4; i_Slot++ )
	{
		if (sloty[ i_Slot ] == -1) 
			continue;
		
		x = 4;
		y = (sloty[ i_Slot ] - (row[ i_Slot ] + 12)) + ((row[i_Slot] - H[i_Slot][0]) >> 1);
		if ( W[ i_Slot ][ 0 ] > 0 )
		{
			/* get bitmap */
			L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
			st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
			TEX_l_File_GetInfoAndContent(pst_Des->st_Tex.ast_Slot[ i_Slot ].ul_Raw, &st_Des);

			/* get palette */
			L_memset( &st_Palette, 0, sizeof( TEX_tdst_Palette ) );
			st_Palette.ul_Key = pst_Des->st_Tex.ast_Slot[ i_Slot ].ul_Pal;
			TEX_File_LoadPalette(&st_Palette, FALSE);

			puc_Tgt = p_RawBits + ( (y*w + x) * 3 ) + ( ((col[0] - W[i_Slot][0]) >> 1) * 3);
			if(st_Des.uc_FinalBPP == 8)
				TEX_Convert_8To24( puc_Tgt, (UCHAR *) st_Des.p_Bitmap, st_Palette.pul_Color, W[i_Slot][0], H[i_Slot][0], (w - W[i_Slot][0]) * 3 );
			else if(st_Des.uc_FinalBPP == 4)
				TEX_Convert_4To24( puc_Tgt, (UCHAR *) st_Des.p_Bitmap, st_Palette.pul_Color, W[i_Slot][0], H[i_Slot][0], (w - W[i_Slot][0]) * 3 );

			if (p_RawBits1 && (st_Palette.uc_Flags & TEX_uc_AlphaPalette) )
			{
				puc_Tgt = p_RawBits1 + ( (y*w + x) * 3 ) + ( ((col[0] - W[i_Slot][0]) >> 1) * 3);
				if(st_Des.uc_FinalBPP == 8)
					TEX_Convert_8To24_ChannelAlpha( puc_Tgt, (UCHAR *) st_Des.p_Bitmap, st_Palette.pul_Color, W[i_Slot][0], H[i_Slot][0], (w - W[i_Slot][0]) * 3 );
				else if(st_Des.uc_FinalBPP == 4)
					TEX_Convert_4To24_ChannelAlpha( puc_Tgt, (UCHAR *) st_Des.p_Bitmap, st_Palette.pul_Color, W[i_Slot][0], H[i_Slot][0], (w - W[i_Slot][0]) * 3 );
			}

			if (st_Palette.pul_Color) 
				TEX_M_File_Free(st_Palette.pul_Color);
			if (st_Des.p_Bitmap)
				TEX_M_File_Free(st_Des.p_Bitmap);
		}

		x+= col[0] + 4;
		y = (sloty[ i_Slot ] - (row[ i_Slot ] + 12)) + ((row[i_Slot] - H[i_Slot][1]) >> 1);
		if (W[ i_Slot ][ 1 ] > 0 )
		{
			L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
			st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
			i_Result = TEX_l_File_GetInfoAndContent(pst_Des->st_Tex.ast_Slot[ i_Slot ].ul_TC, &st_Des);

			puc_Tgt = p_RawBits + ( (y*w + x) * 3 ) + (((col[1] - st_Des.uw_Width) >> 1) * 3);
			puc_Src = (UCHAR *) st_Des.p_Bitmap;
			LinePitch = (w - st_Des.uw_Width) * 3;

			if( st_Des.uc_FinalBPP == 32)
			{
				for(j = 0; j < st_Des.uw_Height; j++)
				{
					for(i = 0; i < st_Des.uw_Width; i++)
					{
						*puc_Tgt++ = *puc_Src++;
						*puc_Tgt++ = *puc_Src++;
						*puc_Tgt++ = *puc_Src++;
						puc_Src++;
					}
					puc_Tgt += LinePitch;
				}

				if(p_RawBits1)
				{
					puc_Tgt = p_RawBits1 + ( (y*w + x) * 3 ) + (((col[1] - st_Des.uw_Width) >> 1) * 3);
					puc_Src = ((UCHAR *) st_Des.p_Bitmap) + 3;
					for(j = 0; j < st_Des.uw_Height; j++)
					{
						for(i = 0; i < st_Des.uw_Width; i++)
						{
							*puc_Tgt++ = *puc_Src;
							*puc_Tgt++ = *puc_Src;
							*puc_Tgt++ = *puc_Src;
							puc_Src += 4;
						}
						puc_Tgt += LinePitch;
					}
				}
			}
			else if(st_Des.uc_FinalBPP == 24)
			{
				LineWidth = st_Des.uw_Width * 3;
				LinePitch = w * 3;
				for(i = 0; i < st_Des.uw_Height; i++)
				{
					L_memcpy(puc_Tgt, puc_Src, LineWidth);
					puc_Tgt += LinePitch;
					puc_Src += LineWidth;
				}
			}

			if (st_Des.p_Bitmap) 
				TEX_M_File_Free(st_Des.p_Bitmap);
		}
	}
#if defined(_XENON_RENDER)
    // Xenon original texture
    if (W[4][0] > 0)
    {
        ULONG ulLength;
        char* pcDDSBuf;

        ul_Index = BIG_ul_SearchKeyToFat(pst_Des->st_Tex.st_XeProperties.ul_OriginalTexture);
        pcDDSBuf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_Index), &ulLength);
        if (pcDDSBuf != NULL)
        {
            L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
            st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content | TEX_Cuw_DF_ConvertTo32BPP;
            if (TEX_l_File_LoadDDS((char *)pcDDSBuf, &st_Des, ulLength))
            {
                x         = 0;
                y         = (sloty[4] - (row[4] + 12)) + ((row[4] - H[4][0]) >> 1);
                puc_Tgt   = p_RawBits + ((y * w + x) * 3) + (((col[0] - st_Des.uw_Width) >> 1) * 3);
                puc_Src   = (UCHAR*)st_Des.p_Bitmap;
                LinePitch = (w - st_Des.uw_Width) * 3;

                for(j = 0; j < st_Des.uw_Height; j++)
                {
                    for(i = 0; i < st_Des.uw_Width; i++)
                    {
                        *puc_Tgt++ = *puc_Src++;
                        *puc_Tgt++ = *puc_Src++;
                        *puc_Tgt++ = *puc_Src++;
                        puc_Src++;
                    }
                    puc_Tgt += LinePitch;
                }

                if(p_RawBits1)
                {
                    puc_Tgt = p_RawBits1 + ((y * w + x) * 3) + (((col[0] - st_Des.uw_Width) >> 1) * 3);
                    puc_Src = ((UCHAR *) st_Des.p_Bitmap) + 3;
                    for(j = 0; j < st_Des.uw_Height; j++)
                    {
                        for(i = 0; i < st_Des.uw_Width; i++)
                        {
                            *puc_Tgt++ = *puc_Src;
                            *puc_Tgt++ = *puc_Src;
                            *puc_Tgt++ = *puc_Src;
                            puc_Src += 4;
                        }
                        puc_Tgt += LinePitch;
                    }
                }
            }

            if (st_Des.p_Bitmap)
                TEX_M_File_Free(st_Des.p_Bitmap);

            L_free(pcDDSBuf);
        }
    }

    // Xenon native texture
    if (W[4][1] > 0)
    {
        ULONG ulLength;
        char* pcDDSBuf;

        if (pst_Des->st_Tex.st_XeProperties.ul_NativeTexture != BIG_C_InvalidKey)
        {
            ul_Index = BIG_ul_SearchKeyToFat(pst_Des->st_Tex.st_XeProperties.ul_NativeTexture);
        }
        else
        {
            ul_Index = BIG_ul_SearchKeyToFat(pst_Des->st_Tex.st_XeProperties.ul_OriginalTexture);
        }

        pcDDSBuf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_Index), &ulLength);
        if (pcDDSBuf != NULL)
        {
            L_memset(&st_Des, 0, sizeof(TEX_tdst_File_Desc));
            L_memcpy(&st_Des.st_Tex.st_XeProperties, &pst_Des->st_Tex.st_XeProperties, sizeof(TEX_tdst_XenonFileTex));
            st_Des.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content | TEX_Cuw_DF_ConvertTo32BPP | TEX_Cuw_DF_EditorUseXenonFile;
            if (TEX_l_File_LoadDDS(pcDDSBuf, &st_Des, ulLength))
            {
                if (col[0] == 0)
                    x = 0;
                else
                    x = col[0] + 4;
                y         = (sloty[4] - (row[4] + 12)) + ((row[4] - H[4][1]) >> 1);
                puc_Tgt   = p_RawBits + ((y * w + x) * 3) + (((col[1] - st_Des.uw_Width) >> 1) * 3);
                puc_Src   = (UCHAR*)st_Des.p_Bitmap;
                LinePitch = (w - st_Des.uw_Width) * 3;

                for(j = 0; j < st_Des.uw_Height; j++)
                {
                    for(i = 0; i < st_Des.uw_Width; i++)
                    {
                        *puc_Tgt++ = *puc_Src++;
                        *puc_Tgt++ = *puc_Src++;
                        *puc_Tgt++ = *puc_Src++;
                        puc_Src++;
                    }
                    puc_Tgt += LinePitch;
                }

                if(p_RawBits1)
                {
                    puc_Tgt = p_RawBits1 + ((y * w + x) * 3) + (((col[1] - st_Des.uw_Width) >> 1) * 3);
                    puc_Src = ((UCHAR *) st_Des.p_Bitmap) + 3;
                    for(j = 0; j < st_Des.uw_Height; j++)
                    {
                        for(i = 0; i < st_Des.uw_Width; i++)
                        {
                            *puc_Tgt++ = *puc_Src;
                            *puc_Tgt++ = *puc_Src;
                            *puc_Tgt++ = *puc_Src;
                            puc_Src += 4;
                        }
                        puc_Tgt += LinePitch;
                    }
                }
            }

            if (st_Des.p_Bitmap)
                TEX_M_File_Free(st_Des.p_Bitmap);

            L_free(pcDDSBuf);
        }
    }
#endif
	dc.CreateCompatibleDC(pdc);
	oBmp.CreateCompatibleBitmap(pdc, w, 12 );
	oldBmp = dc.SelectObject(&oBmp);
	oldFnt = dc.SelectObject(&M_MF()->mo_Fnt);
	dc.SetTextColor(0xFFFFFF);
	dc.SetBkMode(TRANSPARENT);
	p_RawBits2 = (UCHAR *) L_malloc( 12 * (w + 4) * 3);
	L_memset(p_RawBits2, 0x75, 12 * (w + 4) * 3);
	st_BitmapInfo.bmiHeader.biHeight = 12;

	for (i_Slot = 0; i_Slot < NB_ROWS; i_Slot++ )
	{
		if (sloty[ i_Slot ] == -1 ) 
			continue;

		dc.FillSolidRect( 0, 0, w, 12, 0x7F0000 );

		st_Rect.left = 4;
		st_Rect.right = st_Rect.left+ col[0];
		st_Rect.top = 0;
		st_Rect.bottom = 12;
		dc.DrawText( sz_Etiket[ i_Slot ][ 0 ], -1, &st_Rect, DT_SINGLELINE);

		st_Rect.left = st_Rect.right + 4;
		st_Rect.right = st_Rect.left + col[1];
		dc.DrawText( sz_Etiket[ i_Slot ][ 1 ], -1, &st_Rect, DT_SINGLELINE);

		i_Result = GetDIBits(dc.m_hDC, (HBITMAP) oBmp.m_hObject, 0, 12, p_RawBits2, &st_BitmapInfo, ui_Usage);
		L_memcpy(p_RawBits + (sloty[ i_Slot ] - 12) * w * 3, p_RawBits2, 12 * w * 3);
		if(p_RawBits1) (p_RawBits1 + (sloty[ i_Slot ] - 12) * w * 3, p_RawBits2, 12 * w * 3);
	}

	L_free(p_RawBits2);
	dc.SelectObject(oldFnt);
	dc.SelectObject(oldBmp);
	ReleaseDC(pdc);

	if(!pDes) mpo_ScrollView->mo_BmpList.AddTail(pst_Des);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEX_cl_Frame::b_UpdateTex(CPoint *_po_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAIEDITEX_tdst_BitmapDes	*pst_Des;
	int							type;
	char						*sz_Name;
	TEX_tdst_File_Params		st_Tex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sz_Name = BIG_NameFile(mul_FileIndex);

	/*
	 * mise à jour des textures de type fontes avec un drag and drop de fichier fod
	 * (FOnte Descriptor)
	 */
	if(TEX_l_File_IsFormatSupported(sz_Name, 3))
	{
		pst_Des = mpo_ScrollView->GetDesUnderPoint(_po_Pt);
		if(!pst_Des) return FALSE;

		TEX_File_GetParams(&st_Tex, pst_Des->ul_FatFile);
		if(!(st_Tex.uw_Flags & TEX_FP_ThisIsAFont)) return FALSE;
		st_Tex.ul_Params[0] = BIG_FileKey(mul_FileIndex);
		TEX_File_SetParams(&st_Tex, pst_Des->ul_FatFile, FALSE);
		return TRUE;
	}

	/* mise à jour d'un .TEX avec raw, pal ou tga */
	for(type = 0; type < 3; type++)
	{
		if(TEX_l_File_IsFormatSupported(sz_Name, type)) break;
	}

	if(type == 3) return FALSE;

	pst_Des = mpo_ScrollView->GetDesUnderPoint(_po_Pt);
	if(!pst_Des) return FALSE;
	if(pst_Des->i_Type != MAIEDITEX_C_TEX) return FALSE;

	if(type == 0)
	{
		if(pst_Des->st_Tex.ast_Slot[0].ul_Raw != BIG_FileKey(mul_FileIndex))
		{
			pst_Des->st_Tex.ast_Slot[0].ul_Raw = BIG_FileKey(mul_FileIndex);
			mpo_ScrollView->SaveTex(pst_Des);
		}
	}
	else if(type == 1)
	{
		if(pst_Des->st_Tex.ast_Slot[0].ul_Pal != BIG_FileKey(mul_FileIndex))
		{
			pst_Des->st_Tex.ast_Slot[0].ul_Pal = BIG_FileKey(mul_FileIndex);
			mpo_ScrollView->SaveTex(pst_Des);
		}
	}
	else if(type == 2)
	{
		if(pst_Des->st_Tex.ast_Slot[0].ul_TC != BIG_FileKey(mul_FileIndex))
		{
			pst_Des->st_Tex.ast_Slot[0].ul_TC = BIG_FileKey(mul_FileIndex);
			mpo_ScrollView->SaveTex(pst_Des);
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::Refresh(void)
{
	/* Force a dummy paint to get new size of scroll size */
	mpo_ScrollView->SetRedraw(FALSE);
	mpo_ScrollView->Invalidate();
	mpo_ScrollView->SendMessage(WM_PAINT);
	mpo_ScrollView->SetRedraw(TRUE);

	/* And then force a paint with that new size */
	mpo_ScrollView->Invalidate();
	mpo_ScrollView->SetScrollSizes(MM_TEXT, mpo_ScrollView->mo_Size);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::Browse(BOOL _b_Del)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX					ul_IndexFile;
	void						*p_Buf;
	char						*psz_Name, *psz_Temp;
	int							i_Format, i_Num;
	BOOL						b_KeepSelection;
	POSITION					pos;
	MAIEDITEX_tdst_BitmapDes    *pdes;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->AddHistoryFile(this, mul_DirIndex);
	if(mul_FileIndex != BIG_C_InvalidIndex) M_MF()->AddHistoryFile(this, BIG_FileKey(mul_FileIndex));

	/* Delete current list */
	b_KeepSelection = FALSE;

	if(_b_Del) DeleteList();

	/* Remember index */
	mpo_ScrollView->mul_DirIndex = mul_DirIndex;
	if(mul_DirIndex != BIG_C_InvalidIndex)
	{
		AfxGetApp()->DoWaitCursor(1);

		/* Browse a path or a single file */
		ul_IndexFile = mul_FileIndex;
		if(ul_IndexFile == BIG_C_InvalidIndex) ul_IndexFile = BIG_FirstFile(mul_DirIndex);

		i_Num = 0;
		while(ul_IndexFile != BIG_C_InvalidIndex)
		{
			/* look into current data if file already there */
		    pos = mpo_ScrollView->mo_BmpList.GetHeadPosition();
			pdes = NULL;
			while(pos)
			{
				pdes = mpo_ScrollView->mo_BmpList.GetNext(pos);
				if (pdes->ul_FatFile == ul_IndexFile )
					break;
				pdes = NULL;
			}
			
			if (!pdes )
			{
				/* Is the extension valid ? */
				psz_Name = BIG_NameFile(ul_IndexFile);
				psz_Temp = L_strrchr(psz_Name, '.');
				if(psz_Temp)
				{
					/* Get the format */
					psz_Temp++;
					if ( !L_strcmpi( psz_Temp, "bmp" ) )
						i_Format = MAIEDITEX_C_BMP;
					else if ( !L_strcmpi( psz_Temp, "tga" ) )
						i_Format = MAIEDITEX_C_TGA;
					else if ( !L_strcmpi( psz_Temp, "jpg" ) )
						i_Format = MAIEDITEX_C_JPG;
					else if ( !L_strcmpi( psz_Temp, "pal" ) )
						i_Format = MAIEDITEX_C_PALETTE;
					else if ( !L_strcmpi( psz_Temp, "raw" ) )
						i_Format = MAIEDITEX_C_RAW;
					else if ( !L_strcmpi( psz_Temp, "tex" ) )
						i_Format = MAIEDITEX_C_TEX;
					else if ( !L_strcmpi( psz_Temp, "psd" ) )
						i_Format = MAIEDITEX_C_PSD;
					else if ( !L_strcmpi( psz_Temp, "png" ) )
						i_Format = MAIEDITEX_C_PNG;
					else if ( !L_strcmpi( psz_Temp, "gif" ) )
						i_Format = MAIEDITEX_C_GIF;
#if defined(_XENON_RENDER)
                    else if(!L_strcmpi(psz_Temp, "dds"))
                        i_Format = MAIEDITEX_C_DDS;
#endif
					else
						i_Format = MAIEDITEX_C_INVALID;

					if(i_Format != MAIEDITEX_C_INVALID)
					{
						/*~~~~~~~~~~~~~*/
						ULONG	ulLenght;
						/*~~~~~~~~~~~~~*/

						i_Num++;

						/* Keep selection ? */
						if(ul_IndexFile == mpo_ScrollView->mul_Selected) b_KeepSelection = TRUE;

						/* Read the file */
						p_Buf = (void *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_IndexFile), &ulLenght);
						if(p_Buf)
						{
							/* Treat the file */
							switch(i_Format)
							{
							case MAIEDITEX_C_BMP:
								pdes = TEXUtil_ConvertBitmap(mpo_ScrollView, ul_IndexFile, (UCHAR *) p_Buf);
								break;

							case MAIEDITEX_C_TGA:
								pdes = TEXUtil_ConvertTGA(mpo_ScrollView, ul_IndexFile, (UCHAR *) p_Buf);
								break;

							case MAIEDITEX_C_JPG:
								pdes = TEXUtil_ConvertJPG(mpo_ScrollView, ul_IndexFile, (UCHAR *) p_Buf, ulLenght);
								break;
#if defined(_XENON_RENDER)
                            case MAIEDITEX_C_DDS:
                                if (GDI_b_IsXenonGraphics())
                                {
                                    pdes = TEXUtil_ConvertDDS(mpo_ScrollView, ul_IndexFile, (UCHAR*)p_Buf, ulLenght);
                                }
                                break;
#endif
							case MAIEDITEX_C_PALETTE:
								pdes = TEXUtil_ConvertPalette(mpo_ScrollView, ul_IndexFile, (UCHAR *) p_Buf);
								break;

							case MAIEDITEX_C_RAW:
								pdes = TEXUtil_ConvertRawFile(mpo_ScrollView, ul_IndexFile, (UCHAR *) p_Buf);
								break;

							case MAIEDITEX_C_PSD:
							case MAIEDITEX_C_PNG:
							case MAIEDITEX_C_GIF:
								pdes = TEXUtil_ConvertStbiFile( mpo_ScrollView, ul_IndexFile, ( UCHAR * ) p_Buf, ulLenght, i_Format );
								break;

							case MAIEDITEX_C_TEX:
								ConvertTex(ul_IndexFile, (UCHAR *) p_Buf, ulLenght, NULL);
								break;
							}
							
							if (pdes)
								mpo_ScrollView->mo_BmpList.AddTail(pdes);

							L_free(p_Buf);
						}
					}
				}
			}

			/* Pass to next file in dir */
			if(mul_FileIndex != BIG_C_InvalidIndex) break;
			ul_IndexFile = BIG_NextFile(ul_IndexFile);
		}

		/* Reset selection if invalid */
		if(b_KeepSelection == FALSE) mpo_ScrollView->mul_Selected = BIG_C_InvalidIndex;

		AfxGetApp()->DoWaitCursor(-1);
	}

	/* No texture in the directory */
	if(mpo_ScrollView->mo_BmpList.GetCount() == 0)
	{
		mul_DirIndex = BIG_C_InvalidIndex;
		mul_FileIndex = BIG_C_InvalidIndex;
	}

	/* Refresh */
	Refresh();
}

/*$4
 ***********************************************************************************************************************
    COMMANDS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::OnAlpha(void)
{
	mst_Ini.b_AlphaShow = mst_Ini.b_AlphaShow ? FALSE : TRUE;
	mpo_ScrollView->mb_AlphaShow = mst_Ini.b_AlphaShow;

	/* Refresh */
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::OnClose(void)
{
	DeleteList();
	mul_DirIndex = BIG_C_InvalidIndex;
	mul_FileIndex = BIG_C_InvalidIndex;
	mpo_ScrollView->Invalidate();
	mpo_ScrollView->SendMessage(WM_PAINT);
}

/*
 =======================================================================================================================
    Aim:    Call to change the resolution to display images.
 =======================================================================================================================
 */
void ETEX_cl_Frame::OnChooseResolution(int _i_ID)
{
	mst_Ini.i_Res = _i_ID;
	mpo_ScrollView->mi_Res = mst_Ini.i_Res;
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::OnForceRes(void)
{
	mst_Ini.b_ForceRes = !mst_Ini.b_ForceRes;
	mpo_ScrollView->mb_ForceRes = mst_Ini.b_ForceRes;

	/* Refresh */
	Refresh();
}

/*$4
 ***********************************************************************************************************************
    INTERFACE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEX_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::CloseProject(void)
{
	DeleteList();
}

/*$4
 ***********************************************************************************************************************
    Check textures
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::ParseFile(BIG_INDEX _ul_Dir, void (*pfnb_Callback) (void *, ULONG))
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Sub;
	/*~~~~~~~~~~~~~~~*/

	/* Recurse call for each dirs */
	ul_Sub = BIG_SubDir(_ul_Dir);
	while(ul_Sub != BIG_C_InvalidIndex)
	{
		ParseFile(ul_Sub, pfnb_Callback);
		ul_Sub = BIG_NextDir(ul_Sub);
	}

	/* Call for each files */
	ul_Sub = BIG_FirstFile(_ul_Dir);
	while(ul_Sub != BIG_C_InvalidIndex)
	{
		pfnb_Callback((void *) this, ul_Sub);
		ul_Sub = BIG_NextFile(ul_Sub);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Check_DoubleName_CB(void *Frame, ULONG ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			*pul_Data;
	ETEX_cl_Frame	*po_Frame;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_Frame = (ETEX_cl_Frame *) Frame;

	if(po_Frame->mo_TextureCheck.Lookup(BIG_NameFile(ul_Index), (void * &) pul_Data))
	{
		pul_Data[0]++;
		pul_Data = (ULONG *) L_realloc(pul_Data, (pul_Data[0] + 1) * 4);
		pul_Data[pul_Data[0]] = ul_Index;

		po_Frame->mo_TextureCheck.SetAt(BIG_NameFile(ul_Index), (void *) pul_Data);
	}
	else
	{
		pul_Data = (ULONG *) L_malloc(8);
		pul_Data[0] = 1;
		pul_Data[1] = ul_Index;
		po_Frame->mo_TextureCheck.SetAt(BIG_NameFile(ul_Index), (void *) pul_Data);
	}
}
/**/
void ETEX_cl_Frame::Check_DoubleName(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Texture;
	POSITION	x_Pos;
	ULONG		*pul_Data;
	CString		o_String;
	int			i;
	char		sz_Path[BIG_C_MaxLenPath + 15];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	DeleteList();
	ul_Texture = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	ParseFile(ul_Texture, Check_DoubleName_CB);

	strcpy(sz_Path, "....[        ] ");
	x_Pos = mo_TextureCheck.GetStartPosition();
	while(x_Pos)
	{
		mo_TextureCheck.GetNextAssoc(x_Pos, o_String, (void * &) pul_Data);
		if(pul_Data[0] > 1)
		{
			LINK_PrintStatusMsg((char *) (LPCTSTR) o_String);
			for(i = 1; i <= (int) pul_Data[0]; i++)
			{
				BIG_ComputeFullName(BIG_ParentFile(pul_Data[i]), sz_Path + 15);
				sprintf(sz_Path + 5, "%08X", BIG_FileKey(pul_Data[i]));
				sz_Path[13] = ']';
				LINK_PrintStatusMsg(sz_Path);

				mul_DirIndex = BIG_ParentFile(pul_Data[i]);
				mul_FileIndex = pul_Data[i];
				Browse(FALSE);
			}
		}

		L_free(pul_Data);
	}

	mo_TextureCheck.RemoveAll();
}

static int	sai_FileType[10];
static char *sasz_Ext[9] = { ".tga", ".bmp", ".jpg", ".spr", ".pro", ".raw", ".tex", ".pal", ".ant" };

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Check_Loaded_CB(void *Frame, ULONG ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			*pul_Data;
	ETEX_cl_Frame	*po_Frame;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(BIG_FileChanged(ul_Index) & EDI_FHC_Loaded)) return;

	for(i = 0; i < 8; i++)
	{
		if(BIG_b_IsFileExtension(ul_Index, sasz_Ext[i])) break;
	}

	if(!sai_FileType[i]) return;

	pul_Data = (ULONG *) L_malloc(8);
	pul_Data[0] = 1;
	pul_Data[1] = ul_Index;

	po_Frame = (ETEX_cl_Frame *) Frame;
	po_Frame->mo_TextureCheck.SetAt(BIG_NameFile(ul_Index), (void *) pul_Data);
}
/**/
void ETEX_cl_Frame::Check_Loaded(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Texture;
	POSITION				x_Pos;
	ULONG					*pul_Data;
	CString					o_String;
	char					sz_Path[BIG_C_MaxLenPath + 15];
	EDIA_cl_CheckListDialog *po_Dialog;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_CheckListDialog("Type of file", CPoint(0, 0));
	for(i = 0; i < 8; i++)
	{
		sai_FileType[i] = 1;
		po_Dialog->AddItem(sasz_Ext[i] + 1, EVAV_EVVIT_Bool, &sai_FileType[i]);
	}

	sai_FileType[9] = 0;

	i = po_Dialog->DoModal();
	delete po_Dialog;

	DeleteList();
	ul_Texture = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	ParseFile(ul_Texture, Check_Loaded_CB);

	strcpy(sz_Path, "....[        ] ");
	x_Pos = mo_TextureCheck.GetStartPosition();
	while(x_Pos)
	{
		mo_TextureCheck.GetNextAssoc(x_Pos, o_String, (void * &) pul_Data);
		LINK_PrintStatusMsg((char *) (LPCTSTR) o_String);

		mul_DirIndex = BIG_ParentFile(pul_Data[1]);
		mul_FileIndex = pul_Data[1];
		Browse(FALSE);

		L_free(pul_Data);
	}

	mo_TextureCheck.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int	sai_TexCheckType[11] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1};
static char *sasz_TexCheckType[11] = 
{ 
    "validity of keys",
    "raw without pal", 
    "pal without raw", 
    "with no data", 
    "with no raw/pal", 
    "with no true color",
    "correct bad key",
	"size",
	"correct bad size",
	"type",
	"correct bad type"
};
/**/
void Check_TexFile_CB(void *Frame, ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_File_Desc		D;
    char					sz_Text[256];
    char					sz_Path[ BIG_C_MaxLenPath ];
    ULONG					ul_Index, ul_Key, ul_Size;
    BOOL					b_Save;
	char					*puc_Buffer;
	TEX_tdst_File_Params	*pst_Params;
	int						i_Slot;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if( !BIG_b_IsFileExtension(_ul_Index, ".tex"))
        return;

	/* check size */
	if ( sai_TexCheckType[7] )
	{
		ul_Size = BIG_ul_GetLengthFile(BIG_PosFile( _ul_Index ));
		if ( ( ul_Size > 80 ) || (ul_Size & 0x3) )
		{
			sprintf( sz_Text, "[%08X] %s has an invalid size (%d)", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), ul_Size );
			LINK_PrintStatusMsg(sz_Text);
			if ( sai_TexCheckType[8] )
			{
				puc_Buffer = BIG_pc_ReadFileTmp( BIG_PosFile( _ul_Index ), &ul_Size );
				BIG_ComputeFullName( BIG_ParentFile( _ul_Index ), sz_Path );
				SAV_Begin( sz_Path, BIG_NameFile( _ul_Index ) );
				SAV_Buffer( puc_Buffer, 80);
				SAV_ul_End();
			}
		}
	}

	/* check type */
	if ( sai_TexCheckType[9] )
	{
		puc_Buffer = BIG_pc_ReadFileTmp( BIG_PosFile( _ul_Index ), &ul_Size );
		pst_Params = (TEX_tdst_File_Params *) (puc_Buffer + (ul_Size - 32));
		if (pst_Params->uc_Type != TEX_FP_RawPalFile)
		{
			sprintf( sz_Text, "[%08X] %s has an invalid type", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index) );
			LINK_PrintStatusMsg(sz_Text);
			if ( sai_TexCheckType[10] )
			{
				pst_Params->uc_Type = TEX_FP_RawPalFile;
				BIG_ComputeFullName( BIG_ParentFile( _ul_Index ), sz_Path );
				SAV_Begin( sz_Path, BIG_NameFile( _ul_Index ) );
				SAV_Buffer( puc_Buffer, 44);
				SAV_ul_End();
			}
		}
	}


    L_memset(&D, 0, sizeof(TEX_tdst_File_Desc));
	D.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	if(!TEX_l_File_GetInfoAndContent(BIG_FileKey(_ul_Index), &D)) 
    {
        sprintf( sz_Text, "[%08X] %s is invalid", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index) );
        LINK_PrintStatusMsg(sz_Text);
        return;
    }


    if( sai_TexCheckType[ 0 ] )
    {
        b_Save = FALSE;

		for ( i_Slot = 0; i_Slot < 4; i_Slot++)
		{
			/* test palette */
			ul_Key = D.st_Tex.ast_Slot[ i_Slot ].ul_Pal;
			if (ul_Key != BIG_C_InvalidKey)
			{
				ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_Index == BIG_C_InvalidKey)
				{
					sprintf( sz_Text, "[%08X] %s (slot %d) use a bad key : %08X", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot, ul_Key );
					LINK_PrintStatusMsg(sz_Text);
					if( sai_TexCheckType[ 6 ] )
					{
						D.st_Tex.ast_Slot[ i_Slot ].ul_Pal = BIG_C_InvalidKey;
						b_Save = TRUE;
					}
				}
				else
				{
					if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( ul_Index ), 1) )
					{
						sprintf( sz_Text, "[%08X] %s (slot %d) has a bad palette file : %08X, %s", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot, ul_Key, BIG_NameFile( ul_Index) );
						LINK_PrintStatusMsg(sz_Text);
						if( sai_TexCheckType[ 6 ] )
						{
							D.st_Tex.ast_Slot[ i_Slot ].ul_Pal = BIG_C_InvalidKey;
	                        b_Save = TRUE;
		                }
			        }
				}
			}
        
			/* test TrueColor key */
			ul_Key = D.st_Tex.ast_Slot[ i_Slot ].ul_TC;
			if (ul_Key != BIG_C_InvalidKey)
			{
				ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_Index == BIG_C_InvalidKey)
				{
					sprintf( sz_Text, "[%08X] %s (slot %d) use a bad key : %08X", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot, ul_Key );
					LINK_PrintStatusMsg(sz_Text);
					if( sai_TexCheckType[ 6 ] )
					{
						D.st_Tex.ast_Slot[ i_Slot ].ul_TC = BIG_C_InvalidKey;
						b_Save = TRUE;
					}
				}
	            else
		        {
			        if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( ul_Index ), 2) )
					{
						sprintf( sz_Text, "[%08X] %s (slot %d) has a bad true color file : %08X, %s", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot, ul_Key, BIG_NameFile( ul_Index) );
						LINK_PrintStatusMsg(sz_Text);
						if( sai_TexCheckType[ 6 ] )
						{
							D.st_Tex.ast_Slot[ i_Slot ].ul_TC = BIG_C_InvalidKey;
							b_Save = TRUE;
	                    }
		            }
			    }
			}

			/* test raw key */
			ul_Key = D.st_Tex.ast_Slot[ i_Slot ].ul_Raw;
			if (ul_Key != BIG_C_InvalidKey)
			{
				ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_Index == BIG_C_InvalidKey)
				{
					sprintf( sz_Text, "[%08X] %s (slot %d) use a bad key : %08X", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot, ul_Key );
					LINK_PrintStatusMsg(sz_Text);
					if( sai_TexCheckType[ 6 ] )
					{
						D.st_Tex.ast_Slot[ i_Slot ].ul_Raw = BIG_C_InvalidKey;
						b_Save = TRUE;
					}
				}
				else
				{
					if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( ul_Index ), 0) )
					{
						sprintf( sz_Text, "[%08X] %s (slot %d) has a bad raw file : %08X, %s", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot, ul_Key, BIG_NameFile( ul_Index) );
						LINK_PrintStatusMsg(sz_Text);
						if( sai_TexCheckType[ 6 ] )
						{
							D.st_Tex.ast_Slot[ i_Slot ].ul_Raw = BIG_C_InvalidKey;
							b_Save = TRUE;
						}
					}
				}
			}
		}

        if (b_Save)
        {
            BIG_ComputeFullName( BIG_ParentFile( _ul_Index ), sz_Path );
            TEX_ul_File_CreatePaletteRawTexture(  sz_Path, BIG_NameFile( _ul_Index ), D.st_Tex.ast_Slot[0].ul_Raw, D.st_Tex.ast_Slot[0].ul_Pal, D.st_Tex.ast_Slot[0].ul_TC );
        }
    }

    if ( sai_TexCheckType[ 1 ] )
    {
		for (i_Slot = 0; i_Slot < 4; i_Slot++ )
		{
			if ( (D.st_Tex.ast_Slot[ i_Slot ].ul_Raw != BIG_C_InvalidKey) && (D.st_Tex.ast_Slot[ i_Slot ].ul_Pal == BIG_C_InvalidKey) )
	        {
		        sprintf( sz_Text, "[%08X] %s (slot %d) has raw but no pal", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot );
			    LINK_PrintStatusMsg(sz_Text);
			}
		}
    }

    if ( sai_TexCheckType[ 2 ] )
    {
		for (i_Slot = 0; i_Slot < 4; i_Slot++ )
		{
			if ( (D.st_Tex.ast_Slot[ i_Slot ].ul_Raw == BIG_C_InvalidKey) && (D.st_Tex.ast_Slot[ i_Slot ].ul_Pal != BIG_C_InvalidKey) )
			{
				sprintf( sz_Text, "[%08X] %s (slot %d) has pal but no raw", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot );
				LINK_PrintStatusMsg(sz_Text);
			}
		}
    }

    if ( sai_TexCheckType[ 3 ] )
    {
		i_Slot = 0;
		if ( (D.st_Tex.ast_Slot[ i_Slot ].ul_Raw == BIG_C_InvalidKey) && (D.st_Tex.ast_Slot[ i_Slot ].ul_Pal == BIG_C_InvalidKey) && (D.st_Tex.ast_Slot[ i_Slot ].ul_TC == BIG_C_InvalidKey) )
		{
			sprintf( sz_Text, "[%08X] %s (slot %d) has no data", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot );
			LINK_PrintStatusMsg(sz_Text);
		}
    }
    
    if ( sai_TexCheckType[ 4 ] )
    {
		for (i_Slot = 0; i_Slot < 4; i_Slot++ )
		{
			if ( (D.st_Tex.ast_Slot[ i_Slot ].ul_Raw == BIG_C_InvalidKey) || (D.st_Tex.ast_Slot[ i_Slot ].ul_Pal == BIG_C_InvalidKey)  )
			{
				sprintf( sz_Text, "[%08X] %s (slot %d) has no raw/pal", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot );
				LINK_PrintStatusMsg(sz_Text);
			}
		}
    }

    if ( sai_TexCheckType[ 5 ] )
    {
		for (i_Slot = 0; i_Slot < 4; i_Slot++ )
		{
			if ( D.st_Tex.ast_Slot[ i_Slot ].ul_TC == BIG_C_InvalidKey )
			{
				sprintf( sz_Text, "[%08X] %s (slot %d) has no true color", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i_Slot );
				LINK_PrintStatusMsg(sz_Text);
			}
		}
    }
}
/**/
void ETEX_cl_Frame::Check_TexFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Texture;
	EDIA_cl_CheckListDialog *po_Dialog;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_CheckListDialog("Type of check", CPoint(0, 0));
	for(i = 0; i < 11; i++)
		po_Dialog->AddItem(sasz_TexCheckType[i], EVAV_EVVIT_Bool, &sai_TexCheckType[i]);

	i = po_Dialog->DoModal();
	delete po_Dialog;

    LINK_PrintStatusMsg( "Check tex file content");
	ul_Texture = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	ParseFile(ul_Texture, Check_TexFile_CB);
    LINK_PrintStatusMsg( "Finished");
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Check_Palette_CB(void *Frame, ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
    char				sz_Text[256];
    TEX_tdst_Palette			st_Palette;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if( !BIG_b_IsFileExtension(_ul_Index, ".pal"))
        return;

    L_memset( &st_Palette, 0, sizeof( TEX_tdst_Palette ) );
    st_Palette.ul_Key = BIG_FileKey( _ul_Index );
	TEX_File_LoadPalette(&st_Palette, FALSE);

    if( st_Palette.uc_Flags & TEX_uc_InvalidPalette )
	{
        sprintf( sz_Text, "[%08X] %s is an invalid palette", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index) );
		LINK_PrintStatusMsg(sz_Text);
    }
}
/**/
void ETEX_cl_Frame::Check_Palette(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Texture;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    LINK_PrintStatusMsg( "Check validity of palette");
	ul_Texture = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	ParseFile(ul_Texture, Check_Palette_CB);
    LINK_PrintStatusMsg( "Finished");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Check_BadParams_CB(void *Frame, ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
    char				sz_Text[256];
    TEX_tdst_File_Params st_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
    
    if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( _ul_Index ),-1) )
    {
        if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( _ul_Index ), 0) )
            return;
    }

	TEX_File_GetParams( &st_Params, _ul_Index );
    if ( !TEX_b_File_CheckParams( &st_Params ) )
    {
        sprintf( sz_Text, "[%08X] %s has bad params", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index) );
		LINK_PrintStatusMsg(sz_Text);
    }
}
/**/
void ETEX_cl_Frame::Check_BadParams(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Texture;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    LINK_PrintStatusMsg( "Check parameters of textures");
	ul_Texture = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	ParseFile(ul_Texture, Check_BadParams_CB);
    LINK_PrintStatusMsg( "Finished");
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Check_Font_CB(void *Frame, ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
    char				sz_Text[256];
    TEX_tdst_File_Params st_Params;
    ULONG               ul_Key, ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
    
    if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( _ul_Index ),-1) )
    {
        if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( _ul_Index ), 0) )
            return;
    }

	TEX_File_GetParams( &st_Params, _ul_Index );
    if ( !TEX_b_File_CheckParams( &st_Params ) )
    {
        sprintf( sz_Text, "[%08X] %s has bad params", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index) );
		LINK_PrintStatusMsg(sz_Text);
        return;
    }

    if (st_Params.uw_Flags & TEX_FP_ThisIsAFont)
    {
        ul_Key = st_Params.ul_Params[0];
        if ( (ul_Key == BIG_C_InvalidKey) || (ul_Key == 0 ) )
        {
            sprintf( sz_Text, "[%08X] %s is a font without descriptor", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index) );
            LINK_PrintStatusMsg(sz_Text);
            return;
        }
        ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
        if (ul_Index == BIG_C_InvalidIndex)
        {
            sprintf( sz_Text, "[%08X] %s is a font with bad descriptor [%08X]", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), ul_Key );
            LINK_PrintStatusMsg(sz_Text);
            return;
        }
    }
}
/**/
void ETEX_cl_Frame::Check_Font(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Texture;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    LINK_PrintStatusMsg( "Check parameters of font");
	ul_Texture = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	ParseFile(ul_Texture, Check_Font_CB);
    LINK_PrintStatusMsg( "Finished");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::Check_User(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG						ul_Index;
	TEX_tdst_4Edit_IndexList	st_Data;
	int							i;
	char						sz_Text[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = mpo_ScrollView->mul_Selected;
    if (ul_Index == BIG_C_InvalidIndex) return;
	if(TEX_ul_4Edit_WhoUseTexture(ul_Index, &st_Data))
	{
		sprintf
		(
			sz_Text,
			"%d struct use texture %s [%08X]",
			st_Data.ul_Number,
			BIG_NameFile(ul_Index),
			BIG_FileKey(ul_Index)
		);
		LINK_PrintStatusMsg(sz_Text);

		for(i = 0; i < (int) st_Data.ul_Number; i++)
		{
			ul_Index = st_Data.pul_Indexes[i];
			sprintf(sz_Text, "....[%08X] %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index));
			LINK_PrintStatusMsg(sz_Text);
		}
	}
	else
	{
		sprintf(sz_Text, "No struct use texture %s [%08X]", BIG_NameFile(ul_Index), BIG_FileKey(ul_Index));
		LINK_PrintStatusMsg(sz_Text);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_Frame::Check_DuplicateRaw(void)
{
	TEX_4Edit_SearchDuplicateRaw();
}



#endif /* ACTIVE_EDITORS */
