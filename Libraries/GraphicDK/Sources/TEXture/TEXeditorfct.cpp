/*$T TEXeditorfct.c GC! 1.081 05/30/01 13:06:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Bitmap conversion fonction */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXeditorfct.h"
#include "MATerial/MATstruct.h"
#include "MATerial/MATsprite.h"
#include "GEOmetric/GEOload.h"

#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGexport.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/WORld/WORcheck.h"

#include "EDItors/Sources/MATerial/MuTex.h"





#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern "C" BAS_tdst_barray WOR_ListAllKeys;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_4Edit_CreateBitmap(ULONG _ul_Index, int _i_Slot, HDC hDC, TEX_tdst_4Edit_CreateBitmapResult *_pst_Res )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc	D;
	UCHAR				*p_RawRGB, *p_RawA, *p_Bit;
	int					i, j, x, y, X, Y, PalDecal;
	UINT				ui_Usage;
	BITMAPINFO			st_BitmapInfo;
	BOOL				b_Alpha;
	ULONG				ul_Key, *pul_Pal, ul_Color;
	TEX_tdst_Palette	st_Pal, *pst_Pal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_Key = BIG_FileKey(_ul_Index);
    if (ul_Key == BIG_C_InvalidKey) return 0;

	L_memset( &st_Pal, 0, sizeof( TEX_tdst_Palette ) );
    pst_Pal = (_pst_Res->pst_Pal) ? _pst_Res->pst_Pal : &st_Pal;
	pst_Pal->ul_Key = BIG_C_InvalidKey;
    pul_Pal = NULL;
    b_Alpha = 0;
	
    L_memset(&D, 0, sizeof(TEX_tdst_File_Desc));
	D.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	if(!TEX_l_File_GetInfoAndContent(ul_Key, &D)) return 0;

    if (
            (D.st_Params.uc_Type == TEX_FP_ProFile) ||
            (D.st_Params.uc_Type == TEX_FP_PalFile) ||
            (D.st_Params.uc_Type == TEX_FP_AniFile)
        ) return 0;

	if(D.st_Params.uc_Type == TEX_FP_RawPalFile)
	{
#ifdef JADEFUSION
        ul_Key = BIG_C_InvalidKey;

#if defined(_XENON_RENDER)
        if ((D.st_Tex.st_XeProperties.ul_OriginalTexture != 0) && 
            (D.st_Tex.st_XeProperties.ul_OriginalTexture != BIG_C_InvalidKey))
        {
            ul_Key = D.st_Tex.st_XeProperties.ul_OriginalTexture;
        }
#endif

        if (ul_Key == BIG_C_InvalidKey)
        {
            _pst_Res->ul_PalKey = D.st_Tex.ast_Slot[ _i_Slot ].ul_Pal;
            _pst_Res->ul_RawKey = D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw;

            L_memset( pst_Pal, 0, sizeof( TEX_tdst_Palette ) );
            pst_Pal->ul_Key = D.st_Tex.ast_Slot[ _i_Slot ].ul_Pal;

            if ( _pst_Res->b_RawPalPrio)
                ul_Key = (D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw != BIG_C_InvalidKey) ? D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw : D.st_Tex.ast_Slot[ _i_Slot ].ul_TC ;
            else
                ul_Key = (D.st_Tex.ast_Slot[ _i_Slot ].ul_TC != BIG_C_InvalidKey) ? D.st_Tex.ast_Slot[ _i_Slot ].ul_TC : D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw;
        }
#else		
		_pst_Res->ul_PalKey = D.st_Tex.ast_Slot[ _i_Slot ].ul_Pal;
        _pst_Res->ul_RawKey = D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw;

        L_memset( pst_Pal, 0, sizeof( TEX_tdst_Palette ) );
        pst_Pal->ul_Key = D.st_Tex.ast_Slot[ _i_Slot ].ul_Pal;

        if ( _pst_Res->b_RawPalPrio)
            ul_Key = (D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw != BIG_C_InvalidKey) ? D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw : D.st_Tex.ast_Slot[ _i_Slot ].ul_TC ;
        else
            ul_Key = (D.st_Tex.ast_Slot[ _i_Slot ].ul_TC != BIG_C_InvalidKey) ? D.st_Tex.ast_Slot[ _i_Slot ].ul_TC : D.st_Tex.ast_Slot[ _i_Slot ].ul_Raw;
#endif        
		if(ul_Key != BIG_C_InvalidKey) TEX_l_File_GetInfoAndContent(ul_Key, &D);
	}
	else if (D.st_Params.uc_Type == TEX_FP_RawFile )
		_pst_Res->ul_RawKey = ul_Key;

	if( _pst_Res->pi_Width ) *_pst_Res->pi_Width = D.uw_Width;
    if( _pst_Res->pi_Height ) *_pst_Res->pi_Height = D.uw_Height;

	/* search for a palette */
	if(D.uc_BPP <= 8)
	{
		if(pst_Pal->ul_Key == BIG_C_InvalidKey) pst_Pal->ul_Key = TEX_ul_4Edit_FindPaletteForRaw(_ul_Index);
		if(pst_Pal->ul_Key != BIG_C_InvalidIndex)
		{
			TEX_File_LoadPalette( pst_Pal, FALSE);
			b_Alpha |= ( pst_Pal->uc_Flags & TEX_uc_AlphaPalette);
			pul_Pal = pst_Pal->pul_Color;
		}
	}
    else
    {
        pst_Pal->ul_Key = BIG_C_InvalidKey;
        b_Alpha = (D.uc_BPP == 32);
    }

	/* Bitmap info header */
	st_BitmapInfo.bmiHeader.biWidth = D.uw_Width;
	st_BitmapInfo.bmiHeader.biHeight = D.uw_Height;
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

	/* Create bitmap */
    p_RawRGB = NULL;
    p_RawA = NULL;
    if (_pst_Res->ph_Colors)
    {
	    (*_pst_Res->ph_Colors) = CreateDIBSection(hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawRGB, NULL, 0);
        if ( (*_pst_Res->ph_Colors) == NULL) return 0;
    }
    if (_pst_Res->ph_Alpha )
    {
    	(*_pst_Res->ph_Alpha) = NULL;
	    if(b_Alpha) 
        {
            (*_pst_Res->ph_Alpha) = CreateDIBSection(hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawA, NULL, 0);
	        if ( *_pst_Res->ph_Alpha == NULL ) return 0;
        }
    }

	p_Bit = (UCHAR *) D.p_Bitmap;

	/* 32 bit */
	if(D.uc_BPP == 32)
	{
		for(i = 0; i < D.uw_Width * D.uw_Height; i++)
		{
			*p_RawRGB++ = *p_Bit++;
			*p_RawRGB++ = *p_Bit++;
			*p_RawRGB++ = *p_Bit++;

			*p_RawA++ = *p_Bit;
			*p_RawA++ = *p_Bit;
			*p_RawA++ = *p_Bit++;
		}
	}

	/* 24 bit */
	else if(D.uc_BPP == 24)
	{
		L_memcpy(p_RawRGB, D.p_Bitmap, (D.uw_Width * D.uw_Height * 3));
	}

	/* 8 bit */
	else if(D.uc_BPP == 8)
	{
		TEX_Convert_8To24(p_RawRGB, p_Bit, pul_Pal, D.uw_Width, D.uw_Height, 0);
		if(b_Alpha) TEX_Convert_8To24_ChannelAlpha(p_RawA, p_Bit, pul_Pal, D.uw_Width, D.uw_Height, 0);
	}

	/* 4 bit */
	else if(D.uc_BPP == 4)
	{
		TEX_Convert_4To24(p_RawRGB, p_Bit, pul_Pal, D.uw_Width, D.uw_Height, 0);
		if(b_Alpha) TEX_Convert_4To24_ChannelAlpha(p_RawA, p_Bit, pul_Pal, D.uw_Width, D.uw_Height, 0);
	}

    /* Bitmap info header */
    PalDecal = (pst_Pal->uc_Flags & TEX_uc_Palette16) ? 2 : 0;
  	st_BitmapInfo.bmiHeader.biWidth = _pst_Res->i_PalWidth;
    st_BitmapInfo.bmiHeader.biHeight = _pst_Res->i_PalHeight;
	if(_pst_Res->ph_PaletteColors) (*_pst_Res->ph_PaletteColors) = NULL;
    if(_pst_Res->ph_PaletteAlpha ) (*_pst_Res->ph_PaletteAlpha) = NULL;
    if (pst_Pal->ul_Key != BIG_C_InvalidKey)
    {
        if (_pst_Res->ph_PaletteColors )
        {
            (*_pst_Res->ph_PaletteColors) = CreateDIBSection(hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawRGB, NULL, 0);
            if (*_pst_Res->ph_PaletteColors)
            {
                X = _pst_Res->i_PalWidth >> 4;
                Y = _pst_Res->i_PalHeight >> 4;
                for (j = 0; j < 16; j++)
                {
                    for (i = 0; i < 16; i++)
                    {
                        p_Bit = p_RawRGB + ( ( ((15 - j) * Y * _pst_Res->i_PalWidth ) + (i * X) ) * 3);
                        ul_Color = pul_Pal[ (j >> PalDecal) * (16 >> PalDecal) + (i >> PalDecal) ];

                        for ( y = 0; y < Y; y++)
                        {
                            for (x = 0; x < X; x++)
                            {
                                p_Bit[ x * 3 ] = (UCHAR) (ul_Color & 0xFF);
                                p_Bit[ x * 3 + 1] = (UCHAR) ((ul_Color & 0xFF00) >> 8);
                                p_Bit[ x * 3 + 2] = (UCHAR) ((ul_Color & 0xFF0000) >> 16);
                            }
                            p_Bit += (_pst_Res->i_PalWidth) * 3;
                        }
                    }
                }
            }
        }

        if (_pst_Res->ph_PaletteAlpha && b_Alpha) 
        {
            (*_pst_Res->ph_PaletteAlpha) = CreateDIBSection(hDC, &st_BitmapInfo, ui_Usage, (void **) &p_RawA, NULL, 0);
            if (*_pst_Res->ph_PaletteAlpha)
            {
                X = _pst_Res->i_PalWidth >> 4;
                Y = _pst_Res->i_PalHeight >> 4;
                for (j = 0; j < 16; j++)
                {
                    for (i = 0; i < 16; i++)
                    {
                        p_Bit = p_RawA + ( ( ((15 - j) * Y * _pst_Res->i_PalWidth ) + (i * X) ) * 3);
                        ul_Color = pul_Pal[ (j >> PalDecal) * (16 >> PalDecal) + (i >> PalDecal) ];

                        for ( y = 0; y < Y; y++)
                        {
                            for (x = 0; x < X; x++)
                            {
                                p_Bit[ x * 3 ] = p_Bit[ x * 3 + 1] = p_Bit[ x * 3 + 2] = (UCHAR) (ul_Color >> 24);
                            }
                            p_Bit += (_pst_Res->i_PalWidth) * 3;

                        }
                    }
                }
            }
        }
    }

	if (D.p_Bitmap) 
    {
        if (_pst_Res->ppc_Raw) 
            *_pst_Res->ppc_Raw = (char*)D.p_Bitmap;
        else
            TEX_M_File_Free(D.p_Bitmap);
    }
	if( pul_Pal && (pst_Pal == &st_Pal) ) TEX_M_File_Free(pul_Pal);

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_4Edit_SaveBitmap( HBITMAP h_RGB, int W, int H, char *sz_Name )
{
	FILE			*hp_Bmp;
	char			*p_Raw;
	unsigned short	walign;
	BITMAPINFO				st_Bmp;
	BITMAPFILEHEADER		st_BmpHeader;
										
	walign = (W * 3) + (((W * 3) & 3) ? (4 - ((W * 3) & 3)) : 0);
	p_Raw = (char *) L_malloc( (walign + 2) * H);
	GetBitmapBits(h_RGB, walign * H, p_Raw );
	
	L_memset( &st_BmpHeader, 0, sizeof( BITMAPFILEHEADER ) );
	st_BmpHeader.bfType = ((WORD) ('M' << 8) | 'B');
	st_BmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof( BITMAPINFOHEADER );
	st_BmpHeader.bfSize = st_BmpHeader.bfOffBits + (walign * H); 
	
	L_memset( &st_Bmp, 0, sizeof( BITMAPINFO ) );
	st_Bmp.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	st_Bmp.bmiHeader.biWidth = W;
	st_Bmp.bmiHeader.biHeight = H;
	st_Bmp.bmiHeader.biPlanes = 1;
	st_Bmp.bmiHeader.biBitCount = 24;
	st_Bmp.bmiHeader.biCompression = BI_RGB;

	hp_Bmp = L_fopen( sz_Name, "wb" );
	if (hp_Bmp )
	{
		L_fwrite( &st_BmpHeader, 1, sizeof( BITMAPFILEHEADER ), hp_Bmp );
		L_fwrite( &st_Bmp.bmiHeader, 1, sizeof( BITMAPINFOHEADER ), hp_Bmp );
		L_fwrite( p_Raw, walign * H, 1, hp_Bmp );
		L_fclose( hp_Bmp );
	}
	L_free( p_Raw );
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_4Edit_ParseTexture(BIG_INDEX _ul_Dir, int _i_Recurse, int (*pfni_Callback) (ULONG, ULONG *), ULONG *pul_Param)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Sub;
	/*~~~~~~~~~~~~~~~*/

	/* Recurse call for each dirs */
	if (_i_Recurse)
	{
		ul_Sub = BIG_SubDir(_ul_Dir);
		while(ul_Sub != BIG_C_InvalidIndex)
		{
			if(TEX_ul_4Edit_ParseTexture(ul_Sub, 1, pfni_Callback, pul_Param) == 0) return 0;
			ul_Sub = BIG_NextDir(ul_Sub);
		}
	}

	/* Call for each files */
	ul_Sub = BIG_FirstFile(_ul_Dir);
	while(ul_Sub != BIG_C_InvalidIndex)
	{
		if(pfni_Callback(ul_Sub, pul_Param) == 0) return 0;
		ul_Sub = BIG_NextFile(ul_Sub);
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_GetTexWithRaw(ULONG _ul_Index, ULONG *_pul_Key)
{
	/*~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc	D;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!BIG_b_IsFileExtensionIn(_ul_Index, ".tex")) return 1;

	L_memset(&D, 0, sizeof(TEX_tdst_File_Desc));
	D.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
	if(!TEX_l_File_GetInfoAndContent(BIG_FileKey(_ul_Index), &D)) return 1;

	if(D.st_Params.uc_Type != TEX_FP_RawPalFile) return 1;

	if((*_pul_Key == (ULONG) D.st_Tex.ast_Slot[0].ul_Raw) && (D.st_Tex.ast_Slot[0].ul_Pal != BIG_C_InvalidKey))
	{
		*_pul_Key = (ULONG) D.st_Tex.ast_Slot[0].ul_Pal;
		return 0;
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_4Edit_FindPaletteForRaw(BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Key, ul_Dir;
	/*~~~~~~~~~~~~~~~~~~~*/

	ul_Key = BIG_FileKey(_ul_Index);

	ul_Dir = BIG_ParentFile(_ul_Index);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_GetTexWithRaw, &ul_Key);

	if(ul_Key != BIG_FileKey(_ul_Index)) return ul_Key;

	ul_Dir = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_GetTexWithRaw, &ul_Key);

	if(ul_Key != BIG_FileKey(_ul_Index)) return ul_Key;

	return BIG_C_InvalidIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_WhoUseTexture(ULONG _ul_Index, ULONG *pul_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG						ul_Size, ul_Key;
	char						*pc_Buffer;
	TEX_tdst_File_Desc			st_TexDesc;
	TEX_tdst_4Edit_IndexList	*pst_Data;
	MAT_tdst_Material			*pst_Mat;
	MAT_tdst_MTLevel			*pst_MTL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (TEX_tdst_4Edit_IndexList *) pul_Data;
	ul_Key = BIG_FileKey(pst_Data->ul_Index);
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		static ULONG	ul_BreakKey = 0xFFFFFFFF;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(BIG_FileKey(_ul_Index) == ul_BreakKey)
		{
			ul_Size = 0;
		}
	}

	if(BIG_b_IsFileExtensionIn(_ul_Index, ".tex"))
	{
		pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Index), &ul_Size);
		if(TEX_l_File_LoadRawPalette(pc_Buffer, &st_TexDesc, ul_Size))
		{
			if
			(
				(st_TexDesc.st_Tex.ast_Slot[0].ul_Pal != ul_Key)
			&&	(st_TexDesc.st_Tex.ast_Slot[0].ul_TC  != ul_Key)
			&&	(st_TexDesc.st_Tex.ast_Slot[0].ul_Raw != ul_Key)
			) return 1;
		}
	}
	else if(BIG_b_IsFileExtensionIn(_ul_Index, ".spr"))
	{
		pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Index), &ul_Size);
		if(TEX_l_File_LoadSpriteGen(BIG_FileKey(_ul_Index), pc_Buffer, &st_TexDesc, ul_Size - 32))
		{
			if(TEX_gst_GlobalList.dst_Texture[((MAT_tdst_SpriteGen *) st_TexDesc.st_Params.ul_Params[0])->s_TextureIndex].ul_Key != ul_Key) return 1;
		}
	}
	else if(BIG_b_IsFileExtensionIn(_ul_Index, ".grm"))
	{
		ERR_gb_Warning = FALSE;
		LOA_MakeFileRef(BIG_FileKey(_ul_Index), (ULONG *) &pst_Mat, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
		LOA_Resolve();
		if (ERR_gb_Warning)
		{
			char sz_Text[256];
			sprintf(sz_Text, "[%08X] Error while loading %s", BIG_FileKey(_ul_Index), BIG_NameFile( _ul_Index ) );
			LINK_PrintStatusMsg( sz_Text );
		}

		if(pst_Mat->st_Id.i->ul_Type == GRO_MaterialSingle)
		{
			if(TEX_gst_GlobalList.dst_Texture[((MAT_tdst_Single *) pst_Mat)->l_TextureId].ul_Key != ul_Key) return 1;
		}
		else if(pst_Mat->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
		{
			pst_MTL = ((MAT_tdst_MultiTexture *) pst_Mat)->pst_FirstLevel;
			while(pst_MTL)
			{
				if(TEX_gst_GlobalList.dst_Texture[pst_MTL->s_TextureId].ul_Key == ul_Key) break;
				pst_MTL = pst_MTL->pst_NextLevel;
			}

			if(!pst_MTL) return 1;
		}
		else
			return 1;
	}
	else
		return 1;

	/* rajoutes l'élément */
	if(pst_Data->ul_Number == pst_Data->ul_Max)
	{
		pst_Data->ul_Max += 32;
		if(pst_Data->ul_Max == 32)
			pst_Data->pul_Indexes = (ULONG *) L_malloc(32 * sizeof(ULONG));
		else
			pst_Data->pul_Indexes = (ULONG *) L_realloc(pst_Data->pul_Indexes, pst_Data->ul_Max * 4);
	}

	pst_Data->pul_Indexes[pst_Data->ul_Number++] = _ul_Index;

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_4Edit_WhoUseTexture(BIG_INDEX _ul_Index, TEX_tdst_4Edit_IndexList *_pst_Data)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Dir;
	/*~~~~~~~~~~~*/

	L_memset(_pst_Data, 0, sizeof(TEX_tdst_4Edit_IndexList));
	_pst_Data->ul_Index = _ul_Index;
	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_GameData);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_WhoUseTexture, (ULONG *) _pst_Data);
	return _pst_Data->ul_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

typedef struct TEX_4Edit_tdst_DupRawOne_
{
	USHORT	W, H;
	ULONG	Add, Xor, ul_Index;
} TEX_4Edit_tdst_DupRawOne;

typedef struct TEX_4Edit_tdst_DupRawAll_
{
	int	Bit4_Nb;
	int	Bit4_Size;
	int Bit4_TableNb[ 256 ];
	int Bit4_TableMax[ 256 ];
	TEX_4Edit_tdst_DupRawOne *Bit4_Data[ 256 ];

	int	Bit8_Nb;
	int	Bit8_Size;
	int Bit8_TableNb[ 256 ];
	int Bit8_TableMax[ 256 ];
	TEX_4Edit_tdst_DupRawOne *Bit8_Data[ 256 ];
} TEX_4Edit_tdst_DupRawAll;

int TEX_i_SearchDuplicateRaw( ULONG _ul_Index, ULONG *pul_Data )
{
	TEX_tdst_File_Desc			st_Desc;
	ULONG						ul_Size, Count, *Raw, Add, Xor;
	TEX_4Edit_tdst_DupRawAll	*pst_Data;
	TEX_4Edit_tdst_DupRawOne	*pst_Raw;
	int							b_4bit;
	
	if(!BIG_b_IsFileExtensionIn(_ul_Index, ".raw")) return 1;

	L_memset( &st_Desc, 0, sizeof( TEX_tdst_File_Desc ) );
		
	ERR_gb_Warning = FALSE;
	TEX_l_File_GetInfoAndContent( BIG_FileKey( _ul_Index ), &st_Desc );
	if (ERR_gb_Warning || !st_Desc.p_Bitmap || (st_Desc.st_Params.uc_Type != TEX_FP_RawFile) )
	{
		char sz_Text[ 256 ];
		sprintf( sz_Text, "[%08X] Error while loading %s", BIG_FileKey( _ul_Index ), BIG_NameFile( _ul_Index ) );
		LINK_PrintStatusMsg( sz_Text );
		return 1;
	}

	pst_Data = (TEX_4Edit_tdst_DupRawAll *) pul_Data;

	ul_Size = st_Desc.uw_Height * st_Desc.uw_Width;
	b_4bit = (st_Desc.uc_BPP == 4);
	if (b_4bit) ul_Size >>= 1;

	Raw = (ULONG *) st_Desc.p_Bitmap;
	Count = ul_Size >> 2;
	Add = 0;
	Xor = 0;
	while (Count--)
	{
		Add += *Raw;
		Xor ^= *Raw;
		Raw++;
	}

	Count = Add & 0xFF;
	if ( b_4bit )
	{
		pst_Data->Bit4_Nb++;
		pst_Data->Bit4_Size += ul_Size;
		if ( pst_Data->Bit4_TableNb[ Count ] >= pst_Data->Bit4_TableMax[ Count ] )
		{
			pst_Data->Bit4_TableMax[ Count ] += 100;
			pst_Data->Bit4_Data[ Count ] = (TEX_4Edit_tdst_DupRawOne *) L_realloc(pst_Data->Bit4_Data[ Count ], pst_Data->Bit4_TableMax[ Count ] * sizeof( TEX_4Edit_tdst_DupRawOne ) );
		}
		pst_Raw = &pst_Data->Bit4_Data[ Count ][ pst_Data->Bit4_TableNb[ Count ]++ ];
	}
	else
	{
		pst_Data->Bit8_Nb++;
		pst_Data->Bit8_Size += ul_Size;
		if ( pst_Data->Bit8_TableNb[ Count ] >= pst_Data->Bit8_TableMax[ Count ] )
		{
			pst_Data->Bit8_TableMax[ Count ] += 100;
			pst_Data->Bit8_Data[ Count ] = (TEX_4Edit_tdst_DupRawOne *) L_realloc(pst_Data->Bit8_Data[ Count ], pst_Data->Bit8_TableMax[ Count ] * sizeof( TEX_4Edit_tdst_DupRawOne ) );
		}
		pst_Raw = &pst_Data->Bit8_Data[ Count ][ pst_Data->Bit8_TableNb[ Count ]++ ];
	}

	pst_Raw->W = st_Desc.uw_Width;
	pst_Raw->H = st_Desc.uw_Height;
	pst_Raw->Add = Add;
	pst_Raw->Xor = Xor;
	pst_Raw->ul_Index = _ul_Index;

	TEX_File_FreeDescription( &st_Desc );

	if ( ((pst_Data->Bit4_Nb + pst_Data->Bit8_Nb) % 50) == 0 )
	{
		char sz_Text[ 50 ];
		sprintf(sz_Text, "%d", pst_Data->Bit4_Nb + pst_Data->Bit8_Nb );
		LINK_PrintStatusMsg( sz_Text );
	}

	return 1;
}

void TEX_4Edit_SearchDuplicateRaw( void )
{
	/*~~~~~~~~~~~*/
	char						sz_Text[ 256 ];
	TEX_4Edit_tdst_DupRawAll	st_Data, *pst_Data;
	int							i, depth, cur, comp;
	TEX_4Edit_tdst_DupRawOne	*pst_Cur, *pst_Comp;
	ULONG						ul_Dir, ul_Size, Count;
	TEX_tdst_File_Desc			st_D1, st_D2;
	ULONG						*R1, *R2;
	ULONG						DupNb, DupSize;
	/*~~~~~~~~~~~*/

	/* init data */
	L_memset( &st_Data, 0, sizeof( st_Data ) );

	/* alloc data for full raw */
	for (i = 0; i < 256; i++ )
	{
		st_Data.Bit4_TableMax[ i ] = 100;
		st_Data.Bit4_Data[ i ] = (TEX_4Edit_tdst_DupRawOne *) L_malloc( 100 * sizeof( TEX_4Edit_tdst_DupRawOne ) );

		st_Data.Bit8_TableMax[ i ] = 100;
		st_Data.Bit8_Data[ i ] = (TEX_4Edit_tdst_DupRawOne *) L_malloc( 100 * sizeof( TEX_4Edit_tdst_DupRawOne ) );
	}

	LINK_PrintStatusMsg( "start first pass : load all raw" );

	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_GameData);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_SearchDuplicateRaw, (ULONG *) &st_Data);

	LINK_PrintStatusMsg( "ending of first pass" );
	sprintf( sz_Text, "...Raw 4bits : %d (taille = %d)", st_Data.Bit4_Nb, st_Data.Bit4_Size );
	LINK_PrintStatusMsg( sz_Text );
	sprintf( sz_Text, "...Raw 8bits : %d (taille = %d)", st_Data.Bit8_Nb, st_Data.Bit8_Size );
	LINK_PrintStatusMsg( sz_Text );

	/*
	for (i = 0; i < 256; i++)
	{
		sprintf( sz_Text, "[%3d] %4d %4d", i, st_Data.Bit4_TableNb[ i ], st_Data.Bit8_TableNb[ i ] );
		LINK_PrintStatusMsg( sz_Text );
	}
	*/

	LINK_PrintStatusMsg( "start second pass : search duplicate" );

	DupNb = 0;
	DupSize = 0;

	for (depth = 0; depth< 2; depth++)
	{
		pst_Data = depth ? (TEX_4Edit_tdst_DupRawAll *) &st_Data.Bit8_Nb : &st_Data;

		for (i = 0; i < 256; i++)
		{
			if (pst_Data->Bit4_TableNb[ i ] < 2 ) continue;

			for (cur = 0; cur < pst_Data->Bit4_TableNb[i]; cur++ )
			{
				pst_Cur = &pst_Data->Bit4_Data[ i ][ cur ];
				if (pst_Cur->ul_Index == 0xFFFFFFFF) continue;
				for (comp = cur + 1; comp < pst_Data->Bit4_TableNb[i]; comp++)
				{
					pst_Comp = &pst_Data->Bit4_Data[ i ][ comp ];
					
					if (pst_Comp->W != pst_Cur->W ) continue;
					if (pst_Comp->H != pst_Cur->H ) continue;
					if (pst_Comp->Add != pst_Cur->Add) continue;
					if (pst_Comp->Xor != pst_Cur->Xor) continue;

					// duplication possible

					L_memset( &st_D1, 0, sizeof( TEX_tdst_File_Desc ) );
					TEX_l_File_GetInfoAndContent( BIG_FileKey( pst_Cur->ul_Index ), &st_D1);
					L_memset( &st_D2, 0, sizeof( TEX_tdst_File_Desc ) );
					TEX_l_File_GetInfoAndContent( BIG_FileKey( pst_Comp->ul_Index ), &st_D2 );

					ul_Size = pst_Cur->W * pst_Cur->H;
					if ( !depth ) ul_Size >>= 1;

					R1 = (ULONG *) st_D1.p_Bitmap;
					R2 = (ULONG *) st_D2.p_Bitmap;
					Count = ul_Size >> 2;

					while (--Count)
					{
						if ( *R1 != *R2 ) break;
						R1++;
						R2++;
					}
					if (*R1 == *R2 )
					{
						LINK_PrintStatusMsg( "Duplicata found :" );
						sprintf( sz_Text, "....[%08X] %s", BIG_FileKey( pst_Cur->ul_Index), BIG_NameFile( pst_Cur->ul_Index ) );
						LINK_PrintStatusMsg( sz_Text );
						sprintf( sz_Text, "....[%08X] %s", BIG_FileKey( pst_Comp->ul_Index), BIG_NameFile( pst_Comp->ul_Index ) );
						LINK_PrintStatusMsg( sz_Text );

						pst_Comp->ul_Index = 0xFFFFFFFF;
						DupNb++;
						DupSize += ul_Size;
					}

					TEX_File_FreeDescription( &st_D1 );
					TEX_File_FreeDescription( &st_D2 );
				}
			}
		}
	}

	LINK_PrintStatusMsg( "End of searching duplicata, result is : ");
	if (DupNb)
	{
		sprintf( sz_Text, "%d duplicata found for a size of %d", DupNb, DupSize );
		LINK_PrintStatusMsg( sz_Text );
	}
	else
		LINK_PrintStatusMsg( "No duplicata found : Cool !!!" );



	/* free struct */
	for (i = 0; i < 256; i++)
	{
		L_free( st_Data.Bit4_Data[ i ] ); 
		L_free( st_Data.Bit8_Data[ i ] ); 
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_CheckMatTexture(ULONG _ul_Index, ULONG *pul_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Index, ul_Key, ul_Size;
	MAT_tdst_Material	*pst_Mat;
	MAT_tdst_MTLevel	*pst_MTL;
	int					i_Layer;
	char				sz_Text[256];
	char				*pc_Buffer;
	TEX_tdst_File_Desc	st_TexDesc;
	short				s_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(BIG_b_IsFileExtensionIn(_ul_Index, ".tex"))
	{
		pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Index), &ul_Size);

		if(TEX_l_File_LoadRawPalette(pc_Buffer, &st_TexDesc, ul_Size))
		{
			ul_Key = st_TexDesc.st_Tex.ast_Slot[0].ul_Pal;
            if (ul_Key != BIG_C_InvalidKey)
            {
			    ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
			    if(ul_Index == BIG_C_InvalidKey)
			    {
				    sprintf
				    (
					    sz_Text,
					    "[%08X] %s use a bad key : %08X",
					    BIG_FileKey(_ul_Index),
					    BIG_NameFile(_ul_Index),
					    ul_Key
				    );
				    LINK_PrintStatusMsg(sz_Text);
			    }
                else if( !BIG_b_IsFileExtension(ul_Index, ".pal") )
                {
                    sprintf
				    (
					    sz_Text,
					    "[%08X] %s use a bad palette : %08X",
					    BIG_FileKey(_ul_Index),
					    BIG_NameFile(_ul_Index),
					    ul_Key
				    );
                }
            }

			ul_Key = st_TexDesc.st_Tex.ast_Slot[0].ul_TC;
            if (ul_Key != BIG_C_InvalidKey)
            {
			    ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
			    if(ul_Index == BIG_C_InvalidKey)
			    {
				    sprintf
				    (
					    sz_Text,
					    "[%08X] %s use a bad key : %08X",
					    BIG_FileKey(_ul_Index),
					    BIG_NameFile(_ul_Index),
					    ul_Key
				    );
				    LINK_PrintStatusMsg(sz_Text);
                }
                else if( !BIG_b_IsFileExtension(ul_Index, ".raw") )
                {
                    sprintf
				    (
					    sz_Text,
					    "[%08X] %s use a bad raw : %08X",
					    BIG_FileKey(_ul_Index),
					    BIG_NameFile(_ul_Index),
					    ul_Key
				    );
                }
			}

			ul_Key = st_TexDesc.st_Tex.ast_Slot[0].ul_Raw;
            if (ul_Key != BIG_C_InvalidKey)
            {
			    ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
			    if(ul_Index == BIG_C_InvalidKey)
			    {
				    sprintf
				    (
					    sz_Text,
					    "[%08X] %s use a bad key : %08X",
					    BIG_FileKey(_ul_Index),
					    BIG_NameFile(_ul_Index),
					    ul_Key
				    );
				    LINK_PrintStatusMsg(sz_Text);
                }
			}
		}

		return 1;
	}

	if(BIG_b_IsFileExtensionIn(_ul_Index, ".spr"))
	{
		pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Index), &ul_Size);
		if(TEX_l_File_LoadSpriteGen(BIG_FileKey(_ul_Index), pc_Buffer, &st_TexDesc, ul_Size - 32))
		{
			s_Index = ((MAT_tdst_SpriteGen *) st_TexDesc.st_Params.ul_Params[0])->s_TextureIndex;
			ul_Key = ((MAT_tdst_SpriteGen *) st_TexDesc.st_Params.ul_Params[0])->TEXTURE_BIGKEY;
			if (s_Index == -1)
			{
				if ( (ul_Key != BIG_C_InvalidKey) && (ul_Key != 0) ) 
				{
					sprintf( sz_Text, "[%08X] (sprite gen) %s use a bad texture key : %08X", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), ul_Key );
					LINK_PrintStatusMsg(sz_Text);
				}
				else
				{
					sprintf( sz_Text, "[%08X] (sprite gen) %s has no texture", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index));
					LINK_PrintStatusMsg(sz_Text);
				}
			}
			else
			{
				ul_Key = TEX_gst_GlobalList.dst_Texture[ s_Index ].ul_Key;
				ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_Index == BIG_C_InvalidKey)
				{
					sprintf
					(
						sz_Text,
						"[%08X] (sprite gen) %s use a bad key : %08X",
						BIG_FileKey(_ul_Index),
						BIG_NameFile(_ul_Index),
						ul_Key
					);
					LINK_PrintStatusMsg(sz_Text);
				}
			}
		}

		return 1;
	}

	if(!BIG_b_IsFileExtensionIn(_ul_Index, ".grm")) return 1;

    /* swap multi material */
    {
        ULONG   *pul_Buffer;
        LONG    l_Length;


        pul_Buffer = (ULONG *) BIG_pc_ReadFileTmp( BIG_PosFile( _ul_Index ), (ULONG*)&l_Length);
        if (*pul_Buffer == GRO_MaterialMulti) 
            return 1;
    }


	ERR_gb_Warning = FALSE;
	LOA_MakeFileRef(BIG_FileKey(_ul_Index), (ULONG *) &pst_Mat, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
	LOA_Resolve();

	if (ERR_gb_Warning)
	{
		char sz_Text[256];
		sprintf(sz_Text, "[%08X] Error while loading %s", BIG_FileKey(_ul_Index), BIG_NameFile( _ul_Index ) );
		LINK_PrintStatusMsg( sz_Text );
	}

	if(pst_Mat->st_Id.i->ul_Type == GRO_MaterialSingle)
	{
		if(((MAT_tdst_Single *) pst_Mat)->l_TextureId == -1) return 1;

		ul_Key = TEX_gst_GlobalList.dst_Texture[((MAT_tdst_Single *) pst_Mat)->l_TextureId].ul_Key;
		ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
		if(ul_Index == BIG_C_InvalidKey)
		{
			sprintf(sz_Text, "[%08X] %s use a bad key : %08X", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), ul_Key);
			LINK_PrintStatusMsg(sz_Text);
		}
        else if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( ul_Index ), -1) )
        {
            sprintf
			(
			    sz_Text,
				"[%08X] %s reference a bad file format (not a texture) : %08X %s",
				BIG_FileKey(_ul_Index),
				BIG_NameFile(_ul_Index),
				ul_Key, 
                BIG_NameFile( ul_Index )
		    );
			LINK_PrintStatusMsg(sz_Text);
        }
	}
	else if(pst_Mat->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
	{
		pst_MTL = ((MAT_tdst_MultiTexture *) pst_Mat)->pst_FirstLevel;

		i_Layer = 0;
		while(pst_MTL)
		{
            if ( pst_MTL->s_TextureId == -1)
            {
                sprintf
				(
					sz_Text,
					"[%08X] %s [Layer %d] USE A BAD TEXTURE",
					BIG_FileKey(_ul_Index),
					BIG_NameFile(_ul_Index),
					i_Layer
				);
                LINK_PrintStatusMsg(sz_Text);
            }
            else
            {
			    ul_Key = TEX_gst_GlobalList.dst_Texture[pst_MTL->s_TextureId].ul_Key;
			    ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
			    if(ul_Index == BIG_C_InvalidKey)
			    {
				    sprintf
				    (
					    sz_Text,
					    "[%08X] %s [Layer %d] use a bad key : %08X",
					    BIG_FileKey(_ul_Index),
					    BIG_NameFile(_ul_Index),
					    i_Layer,
					    ul_Key
				    );
				    LINK_PrintStatusMsg(sz_Text);
			    }
                else if ( !TEX_l_File_IsFormatSupported( BIG_NameFile( ul_Index ), -1) )
                {
                    sprintf
				    (
					    sz_Text,
					    "[%08X] %s [Layer %d] reference a bad file format (not a texture) : %08X %s",
					    BIG_FileKey(_ul_Index),
					    BIG_NameFile(_ul_Index),
					    i_Layer,
					    ul_Key, 
                        BIG_NameFile( ul_Index )
				    );
				    LINK_PrintStatusMsg(sz_Text);

                }
            }

			pst_MTL = pst_MTL->pst_NextLevel;
			i_Layer++;
		}
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_4Edit_CheckMatTexture(void)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Dir;
	/*~~~~~~~~~~~*/

	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_GameData);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_CheckMatTexture, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_CheckMultiMat(ULONG _ul_Index, ULONG *pul_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_Mat;
	int				i;
	char			sz_Text[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!BIG_b_IsFileExtensionIn(_ul_Index, ".grm")) return 1;

	LOA_MakeFileRef(BIG_FileKey(_ul_Index), (ULONG *) &pst_Mat, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
	LOA_Resolve();

	i = pst_Mat->st_Id.i->ul_Type;
	if((i != GRO_MaterialMulti) && (i != GRO_MaterialSingle) && (i != GRO_MaterialMultiTexture))
	{
		sprintf(sz_Text, "[%08X] %s has bad type (%d)", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i);
		LINK_PrintStatusMsg(sz_Text);
		return 1;
	}

	if(pst_Mat->st_Id.i->ul_Type != GRO_MaterialMulti) return 1;

	if(pst_Mat->l_NumberOfSubMaterials == 0)
	{
		sprintf(sz_Text, "[%08X] %s has no sub material", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index));
		LINK_PrintStatusMsg(sz_Text);
		return 1;
	}

	for(i = 0; i < pst_Mat->l_NumberOfSubMaterials; i++)
	{
		if(pst_Mat->dpst_SubMaterial[i] == NULL)
		{
			sprintf(sz_Text, "[%08X] %s has bad sub material", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index));
			LINK_PrintStatusMsg(sz_Text);
			return 1;
		}
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_4Edit_CheckMultiMat(void)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Dir;
	/*~~~~~~~~~~~*/

	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_GameData);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_CheckMultiMat, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_CheckSoundID(ULONG _ul_Index, ULONG *pul_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_Mat;
	int				i;
	char			sz_Text[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!BIG_b_IsFileExtensionIn(_ul_Index, ".grm")) return 1;

	LOA_MakeFileRef(BIG_FileKey(_ul_Index), (ULONG *) &pst_Mat, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
	LOA_Resolve();

	i = pst_Mat->st_Id.i->ul_Type;
	if( i == GRO_MaterialMultiTexture )
	{
		i = ((MAT_tdst_MultiTexture *) pst_Mat)->uc_Sound;
		if ( ( i == 0 ) || (i >= MUTEX_NbSoundID) )
		{
			sprintf(sz_Text, "[%08X] %s has bad sound id (%d)", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index), i);
			LINK_PrintStatusMsg(sz_Text);
		}
	}
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_4Edit_CheckSoundID(void)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Dir;
	/*~~~~~~~~~~~*/

	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_GameData);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_CheckSoundID, 0);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEX_i_WhoUseMaterial(ULONG _ul_Index, ULONG *pul_Mat )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_Mat;
    //OBJ_tdst_GameObject *pst_Gao;
	int				i;
	char			sz_Text[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if( BIG_b_IsFileExtensionIn(_ul_Index, ".grm") ) 
    {
	    LOA_MakeFileRef(BIG_FileKey(_ul_Index), (ULONG *) &pst_Mat, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
	    LOA_Resolve();

        if(pst_Mat->st_Id.i->ul_Type != GRO_MaterialMulti) return 1;
        for(i = 0; i < pst_Mat->l_NumberOfSubMaterials; i++)
	    {
		    if(pst_Mat->dpst_SubMaterial[i] == (MAT_tdst_Material *) pul_Mat)
		    {
			    sprintf(sz_Text, "...[%08X] %s", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index));
			    LINK_PrintStatusMsg(sz_Text);
		    }
	    }
    }
    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_4Edit_WhoUseMaterial( ULONG _ul_Index )
{
	/*~~~~~~~~~~~*/
	ULONG	            ul_SearchIndex, ul_Gao, ul_Dir;
    MAT_tdst_Material   *pst_Mat;
    char                sz_Text[256];
	/*~~~~~~~~~~~*/

    if (_ul_Index == BIG_C_InvalidIndex) return;

    pst_Mat = NULL;
    LOA_MakeFileRef(BIG_FileKey(_ul_Index), (ULONG *) &pst_Mat, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
	LOA_Resolve();

    if (pst_Mat == NULL) return;

    WORCheck_LoadAllGao();

    sprintf(sz_Text, "[%08X] %s is used by ->", BIG_FileKey(_ul_Index), BIG_NameFile(_ul_Index));
    LINK_PrintStatusMsg(sz_Text);

    ul_SearchIndex = 0;
    while(1)
    {
        ul_Gao = WORCheck_ul_SearchMaterial( &ul_SearchIndex, _ul_Index );
        if (!ul_Gao) break;
        sprintf(sz_Text, "...[%08X] %s", BIG_FileKey(ul_Gao), BIG_NameFile(ul_Gao));
        LINK_PrintStatusMsg(sz_Text);
    }

    
    BAS_binit(&WOR_ListAllKeys, 100);
	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_GameData);
	TEX_ul_4Edit_ParseTexture(ul_Dir, 1, TEX_i_WhoUseMaterial, (ULONG *) pst_Mat );
    BAS_bfree(&WOR_ListAllKeys);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char	*ExportTga_sz_BFPath;
char	*ExportTga_sz_Path;
int		ExportTga_PaletteMode;
int		*ExportTga_pi_Slot;
char	**ExportTga_ppsz_SlotName;
/**/
int TEX_i_ExportTga(ULONG _ul_Index, ULONG *pul_Data)
{
	char				sz_Path[ BIG_C_MaxLenPath ];
	char				sz_PathOnDisk[ BIG_C_MaxLenPath ];
	char				*sz_CreateDir;
	ULONG               W, H, ul_Size, *pul_Palette, *pul_OldColor;
	TEX_tdst_File_Desc  st_TDesc, st_Desc, st_TgaDesc;
	TEX_tdst_Palette    st_Pal;
	int                 i, j, k, i_slot;
	UCHAR               *puc_Bmp;
	
	if ( !BIG_b_IsFileExtensionIn(_ul_Index, ".tga") && !BIG_b_IsFileExtensionIn(_ul_Index, ".tex") )
		return 1;
		
	BIG_ComputeFullName( BIG_ParentFile( _ul_Index ), sz_Path );
		
	sprintf( sz_PathOnDisk, "%s%s", ExportTga_sz_Path, sz_Path + strlen( ExportTga_sz_BFPath ) );
	if (sz_PathOnDisk[ strlen(sz_PathOnDisk) - 1 ]!= '/')
		strcat( sz_PathOnDisk, "/" );
	strcat( sz_PathOnDisk, BIG_NameFile( _ul_Index ) );
	
	sz_CreateDir = strchr( sz_PathOnDisk + strlen( ExportTga_sz_Path ) + 1, '/' );
	while (sz_CreateDir )
	{
		*sz_CreateDir = 0;
		CreateDirectory( sz_PathOnDisk, NULL );
		*sz_CreateDir ='/';
		sz_CreateDir = strchr( sz_CreateDir + 1, '/' );
	}
		
		
	if ( BIG_b_IsFileExtensionIn(_ul_Index, ".tga") )
	{
		BIG_ExportFileToDisk( sz_PathOnDisk, sz_Path, BIG_NameFile( _ul_Index ) );
	}
	else if ( BIG_b_IsFileExtensionIn(_ul_Index, ".tex") )
	{
		if ( !TEX_l_File_GetInfoAndContent( BIG_FileKey( _ul_Index ), &st_TDesc ) )
			return 1;
			
		for (i_slot = 0; i_slot < 4; i_slot++)
		{
			if (!ExportTga_pi_Slot[ i_slot ] ) 
				continue;
		
			/* load palette */
			L_memset( &st_Pal, 0, sizeof( TEX_tdst_Palette ) );
			st_Pal.ul_Key = st_TDesc.st_Tex.ast_Slot[ i_slot ].ul_Pal;
			if ( st_Pal.ul_Key == BIG_C_InvalidKey ) continue;
			TEX_File_LoadPalette( &st_Pal, FALSE );
			if (st_Pal.uc_Flags & TEX_uc_InvalidPalette) continue;
			pul_Palette = 0;
				
			/* load raw */
			if ( !TEX_l_File_GetInfoAndContent( st_TDesc.st_Tex.ast_Slot[ i_slot ].ul_Raw, &st_Desc ) )
				continue;
			W = st_Desc.st_Params.uw_Width;
			H = st_Desc.st_Params.uw_Height;
			ul_Size = W * H;

			/* prepare tga descriptor */
			L_memset( &st_TgaDesc, 0, sizeof( TEX_tdst_File_Desc ) );
			TEX_File_InitParams( &st_TgaDesc.st_Params );
			st_TgaDesc.uw_Width = st_Desc.st_Params.uw_Width;
			st_TgaDesc.uw_Height = st_Desc.st_Params.uw_Height;

			st_TgaDesc.p_Bitmap = MEM_p_Alloc( ul_Size * 4);

			if ( ExportTga_PaletteMode )
			{
				if (st_Pal.uc_Flags & TEX_uc_Palette16)
				{
					TEX_Convert_4To8( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, W, H );
					i = W * H;
					puc_Bmp = (UCHAR *) st_TgaDesc.p_Bitmap;
					while ( i-- )
					{
						*puc_Bmp = 255 - *puc_Bmp; 
						puc_Bmp++;
					}
					pul_Palette = (ULONG *) L_malloc( 256 * 4 );
					for (i=0; i < 16; i++)
						pul_Palette[ 255 - i ] = st_Pal.pul_Color[ i ];
					L_memset( pul_Palette, 0, 240 * 4 );
					pul_OldColor = st_Pal.pul_Color;
					st_Pal.pul_Color = pul_Palette;
				}
				else
				{
					L_memcpy( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, ul_Size );
				}
				i = 256;
				j = k = 0;
				while( i-- )
				{
					((char *) st_Pal.pul_Color)[j++] = ((char *) st_Pal.pul_Color)[k++];
					((char *) st_Pal.pul_Color)[j++] = ((char *) st_Pal.pul_Color)[k++];
					((char *) st_Pal.pul_Color)[j++] = ((char *) st_Pal.pul_Color)[k++];
					k++;
				}
				st_TgaDesc.uc_BPP = 8;
				st_TgaDesc.p_Palette = st_Pal.pul_Color;
				st_TgaDesc.uc_PaletteBPC = 24;
				st_TgaDesc.uw_PaletteLength = 256;
			}
			else
			{
				if (st_Pal.uc_Flags & TEX_uc_AlphaPalette)
				{
					st_TgaDesc.uc_BPP = 32;
					if (st_Desc.uc_BPP == 4)
						TEX_Convert_4To32( (ULONG *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, ul_Size );
					else
						TEX_Convert_8To32( (ULONG *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, ul_Size );
				}
				else
				{
					st_TgaDesc.uc_BPP = 24;
					if (st_Desc.uc_BPP == 4)
						TEX_Convert_4To24( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, W, H, 0);
					else
						TEX_Convert_8To24( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, W, H, 0);
				}
			}
	        
			sz_CreateDir = strrchr( sz_PathOnDisk, '.' );
			if (sz_CreateDir) *sz_CreateDir = 0;
			if (i_slot != 0)
			{
				strcat( sz_PathOnDisk, "_" );
				strcat( sz_PathOnDisk, ExportTga_ppsz_SlotName[i_slot] );
			}
			strcat( sz_PathOnDisk, ".tga" );
			TEX_l_File_SaveTga( sz_PathOnDisk, &st_TgaDesc );
			
			// Free
			if ( pul_Palette ) 
			{
				L_free( pul_Palette );
				st_Pal.pul_Color = pul_OldColor;
			}
			MEM_Free( st_TgaDesc.p_Bitmap );
			TEX_M_File_Free( st_Pal.pul_Color ); 
			TEX_File_FreeDescription( &st_Desc );
			TEX_File_FreeDescription( &st_TDesc );
		}
	}
	return 1;
}
/**/
void TEX_4Edit_ExportTga( ULONG _ul_Dir, char *_sz_Path, int _i_Recurse, int _i_Palette, int *_pi_Slot, char **_ppsz_SlotName )
{
	char	sz_BFPath[ BIG_C_MaxLenPath ];
	
	BIG_ComputeFullName( _ul_Dir, sz_BFPath );
	ExportTga_sz_BFPath = sz_BFPath;
	ExportTga_sz_Path = _sz_Path;
	ExportTga_PaletteMode = _i_Palette;
	ExportTga_pi_Slot = _pi_Slot;
	ExportTga_ppsz_SlotName = _ppsz_SlotName;
	TEX_ul_4Edit_ParseTexture( _ul_Dir, _i_Recurse, TEX_i_ExportTga, 0);
}


/* mamagouille */
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
#endif ACTIVE_EDITORS
