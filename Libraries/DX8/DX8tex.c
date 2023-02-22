/*$T DX8tex.c GC! 1.081 01/14/02 11:02:39 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Load texture in memory to be used by OpenGL RC. */
#include "Precomp.h"
#include <D3D8.h>
#include <D3Dx8.h>
#if defined(_XBOX) || defined(_XENON)
#include <xgraphics.h>
#endif
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXmemory.h"

#include "DX8init.h"
#include "DX8tex.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGread.h"
#endif
#ifdef PSX2_TARGET
#include "PS2GL.h"
#include "PS2GL_Redefine.h"

/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif
extern DX8_tdst_SpecificData	*p_gDX8SpecificData;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Init specific data for texture creation
 =======================================================================================================================
 */
LONG DX8_l_Texture_Init(GDI_tdst_DisplayData *_pst_DD, ULONG _ul_NbTextures)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	LONG					l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	if(_ul_NbTextures == 0) _ul_NbTextures = 1;
	pst_SD->l_NumberOfTextures = _ul_NbTextures;

	l_Size = (_ul_NbTextures + 1) * sizeof(ULONG);
	pst_SD->dul_Texture = (IDirect3DTexture8 **) MEM_p_Alloc(l_Size);
	L_memset(pst_SD->dul_Texture, 0, l_Size);

	/*
    pst_SD->dul_TextureDeltaBlend = (ULONG *) MEM_p_Alloc(l_Size);
	L_memset(pst_SD->dul_TextureDeltaBlend , 0, l_Size);
    */
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_l_Texture_CreateShadowTextures(DX8_tdst_SpecificData *_pst_SD)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	TexCounter;
	/*~~~~~~~~~~~~~~~*/

    L_zero( _pst_SD->dul_SDW_Texture, sizeof( IDirect3DTexture8 * ) * MaxShadowTexture );

	TexCounter = MaxShadowTexture;
	while(TexCounter--)
	{
		HRESULT err = IDirect3DDevice8_CreateTexture(
			    p_gDX8SpecificData->mp_D3DDevice,
			    ShadowTextureSize,
			    ShadowTextureSize,
			    1,
			    D3DUSAGE_RENDERTARGET,
			    D3DFMT_A8R8G8B8 /* | D3DFMT_R8G8B8 */,
			    D3DPOOL_DEFAULT /* | D3DPOOL_MANAGED | D3DPOOL_SYSTEMMEM */,
			    &_pst_SD->dul_SDW_Texture[ TexCounter ] );
		if ( err != D3D_OK )
		{
			_pst_SD->dul_SDW_Texture[ TexCounter ] = NULL;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_l_Texture_DestroyShadowTextures(DX8_tdst_SpecificData *_pst_SD)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	TexCounter;
	/*~~~~~~~~~~~~~~~*/

	TexCounter = MaxShadowTexture;
	while(TexCounter--)
	{
		if ( _pst_SD->dul_SDW_Texture[ TexCounter ] == NULL )
		{
			continue;
		}
        
		IDirect3D8_Release( _pst_SD->dul_SDW_Texture[ TexCounter ] );
		_pst_SD->dul_SDW_Texture[ TexCounter ] = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_Palette_Load(GDI_tdst_DisplayData *_pst_DD, TEX_tdst_Palette *p_Pal, ULONG Index)
{
   	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
    TEX_tdst_Palette        *pst_Pal;
    ULONG                   *pul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (Index == -1) return;

	pst_SD = DX8_M_SD( _pst_DD );

    pst_Pal = (TEX_tdst_Palette *) p_Pal;
	pul_Color = pst_Pal->pul_Color;
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetPaletteEntries(/*pst_SD->mp_D3DDevice,*/ Index, (PALETTEENTRY *) pul_Color);
#else
	pst_SD->mp_D3DDevice->lpVtbl->SetPaletteEntries(pst_SD->mp_D3DDevice, Index, (PALETTEENTRY *) pul_Color);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_Set_Texture_Palette(GDI_tdst_DisplayData *_pst_DD, ULONG _ulTexNum, ULONG IdexTex, ULONG IndexPal)
{
}

/*
 =======================================================================================================================
    Stocke la texture en RAM pour chargement différé en VRAM £
    retourne 1 si la texture est stockée et ne doit pas être stocké immédiatement en VRAM retourne 0 sinon
 =======================================================================================================================
 */
LONG DX8_l_Texture_Store
(
	GDI_tdst_DisplayData	*_pst_DD,
	TEX_tdst_Data			*_pst_TexData,
	TEX_tdst_File_Desc		*_pst_Tex,
	ULONG					_ul_Texture
)
{
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_Texture_Load
(
	GDI_tdst_DisplayData	*_pst_DD,
	TEX_tdst_Data			*_pst_TexData,
	TEX_tdst_File_Desc		*_pst_Tex,
	ULONG					_ul_Texture
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						c;
	ULONG					TX, TY, MMC, BPP;
	ULONG					*pul_ConvertBuffer;
	int						i_MipmapLevel;
	ULONG					l_MipmapFlag;
	DX8_tdst_SpecificData	*pst_SD;
	IDirect3DTexture8		*DX8_Texture, *DX8_TextureHard;
    HRESULT                 hr;
    IDirect3DSurface8	    *DX8_Surface;
    D3DLOCKED_RECT		    stLockedRect;
    ULONG		            LineCounter;
	unsigned char		    *BitPtr, *DestPtr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	TX = _pst_TexData->w_Width;
	TY = _pst_TexData->w_Height;
    BPP = _pst_Tex->uc_FinalBPP;
	MMC = 0;
	i_MipmapLevel = 1;

	if(_pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn)
	{
		_pst_Tex->uw_FileFlags |= TEX_uw_Mipmap;
		c = (TX > TY) ? TX : TY;
		i_MipmapLevel = 0;
		while(c)
		{
			i_MipmapLevel++;
			c >>= 1;
		}
	}
	else
	{
		_pst_Tex->uw_FileFlags &= ~TEX_uw_Mipmap;
	}

	_pst_TexData->uw_Flags = _pst_Tex->uw_FileFlags;

	c = _pst_Tex->st_Params.ul_Color;
	l_MipmapFlag = _pst_Tex->st_Params.uw_Flags & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor | TEX_FP_MipmapKeepBorder);

	pul_ConvertBuffer = (ULONG *) _pst_Tex->p_Bitmap;

    hr = IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, TX, TY, i_MipmapLevel, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &DX8_Texture );
	if ( hr != D3D_OK )
	{
		LINK_PrintStatusMsg( "Failed to create D3D8 texture!" );
		return;
	}
	hr = IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, TX, TY, i_MipmapLevel, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &DX8_TextureHard );
	if ( hr != D3D_OK )
	{
		LINK_PrintStatusMsg( "Failed to create D3D8 texture!" );
		return;
	}

    char *p_Buf = NULL;
    if ( BPP == 4 || BPP == 8 )
	{
		size_t size = TX * TY;
		p_Buf = ( char * ) L_malloc( size * 4 );
		L_zero( p_Buf, size * 4 );

		// loop through texture to find a good association
		TEX_tdst_Data *pst_RawPal;
		if ( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_OneTexForRawPal )
		{
			for ( c = 0; c < TEX_gst_GlobalList.l_NumberOfTextures; c++, pst_RawPal++ )
			{
			}
		}
		else
		{
			pst_RawPal = TEX_gst_GlobalList.dst_Texture;
			for ( c = 0; c < TEX_gst_GlobalList.l_NumberOfTextures; c++, pst_RawPal++ )
			{
				if ( ( pst_RawPal->uw_Flags & TEX_uw_RawPal ) && ( pst_RawPal->w_Height == ( SHORT ) _ul_Texture ) )
					break;
			}
		}

		if ( BPP == 4 )
			TEX_Convert_4To32( ( unsigned char * ) p_Buf, pul_ConvertBuffer, TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_TexPal ].pul_Color, size );
		else if ( BPP == 8 )
			TEX_Convert_8To32( ( unsigned char * ) p_Buf, pul_ConvertBuffer, TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_TexPal ].pul_Color, size );

        pul_ConvertBuffer = ( ULONG * ) p_Buf;
		BPP = 32;
	}

	if(BPP == 32)
	{
		TEX_Convert_32SwapRB(pul_ConvertBuffer, TX * TY);

		while((TX > 0) && (TY > 0))
		{
			_pst_DD->st_TexManager.l_MemoryTakenByLoading += TX * TY * 4L;
			{
#ifdef JADEFUSION
				DX8_Texture->/*lpVtbl->*/GetSurfaceLevel(/*DX8_Texture,*/ MMC, &DX8_Surface);
				DX8_Surface->/*lpVtbl->*/LockRect(/*DX8_Surface,*/ &stLockedRect, NULL, 0);
				BitPtr = (unsigned char*)stLockedRect.pBits;
#else
				DX8_Texture->lpVtbl->GetSurfaceLevel(DX8_Texture, MMC, &DX8_Surface);
				DX8_Surface->lpVtbl->LockRect(DX8_Surface, &stLockedRect, NULL, 0);
				BitPtr = stLockedRect.pBits;
#endif
				DestPtr = (unsigned char *) pul_ConvertBuffer;
				LineCounter = TY;
				while(LineCounter--)
				{
					memcpy(BitPtr, DestPtr, TX * 4);
					DestPtr += TX * 4;
					BitPtr += stLockedRect.Pitch;
				}
#ifdef JADEFUSION
				DX8_Surface->/*lpVtbl->*/UnlockRect(/*DX8_Surface*/);
				DX8_Surface->/*lpVtbl->*/Release(/*DX8_Surface*/);
#else
				DX8_Surface->lpVtbl->UnlockRect(DX8_Surface);
				DX8_Surface->lpVtbl->Release(DX8_Surface);
#endif
			}

			if(!(_pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn)) break;

			switch(l_MipmapFlag & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor))
			{
			case TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor:
				if(i_MipmapLevel < 4)
					TEX_Blend_AlphaColor50(pul_ConvertBuffer, pul_ConvertBuffer, TX * TY, c);
				else
					TEX_Blend_AlphaColor25(pul_ConvertBuffer, pul_ConvertBuffer, TX * TY, c);
				break;
			case TEX_FP_MipmapUseAlpha:
				if(i_MipmapLevel < 4)
					TEX_Blend_Alpha50(pul_ConvertBuffer, pul_ConvertBuffer, TX * TY, c);
				else
					TEX_Blend_Alpha25(pul_ConvertBuffer, pul_ConvertBuffer, TX * TY, c);
				break;
			case TEX_FP_MipmapUseColor:
				if(i_MipmapLevel < 4)
					TEX_Blend_Color50(pul_ConvertBuffer, pul_ConvertBuffer, TX * TY, c);
				else
					TEX_Blend_Color25(pul_ConvertBuffer, pul_ConvertBuffer, TX * TY, c);
				break;
			}

			/* Keep good Border Here */
			if((TX > 2) && (TY > 2) && (l_MipmapFlag & TEX_FP_MipmapKeepBorder))
				TEX_Blend_KeepBorder(pul_ConvertBuffer, TX, TY);

			/* Compress Image by Two */
			if((TX == 1) && (TY == 1))
			{
				break;
			}

			if(TX > 1)
			{
				TEX_Compress_Xo2(pul_ConvertBuffer, pul_ConvertBuffer, TX, TY);

				TX >>= 1;
			}

			if(TY > 1)
			{
				if(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_InvertMipmap)
				{
					TEX_Compress_Yo2InvertColor(pul_ConvertBuffer, pul_ConvertBuffer, TX, TY);
				}
				else
				{
					TEX_Compress_Yo2(pul_ConvertBuffer, pul_ConvertBuffer, TX, TY);
				}

				TY >>= 1;
			}

			MMC++;
			i_MipmapLevel--;
		}
	}
	else if(BPP == 24)
	{
        TEX_Convert_24To32SwapRB(pul_ConvertBuffer, TX * TY);

		while((TX > 0) && (TY > 0))
		{
			_pst_DD->st_TexManager.l_MemoryTakenByLoading += TX * TY * 3L;
			{
				DX8_Texture->lpVtbl->GetSurfaceLevel(DX8_Texture, MMC, &DX8_Surface);
				DX8_Surface->lpVtbl->LockRect(DX8_Surface, &stLockedRect, NULL, 0);
				BitPtr = stLockedRect.pBits;

				DestPtr = (unsigned char *) pul_ConvertBuffer;
				LineCounter = TY;
				while(LineCounter--)
				{
					memcpy(BitPtr, DestPtr, TX * 4);
					DestPtr += TX * 4;
					BitPtr += stLockedRect.Pitch;
				}

				DX8_Surface->lpVtbl->UnlockRect(DX8_Surface);
				DX8_Surface->lpVtbl->Release(DX8_Surface);
			}

			if(!(_pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn)) break;

			if(l_MipmapFlag & TEX_FP_MipmapUseColor)
			{
				if(i_MipmapLevel < 4)
					TEX_Blend24_Color50((UCHAR *) pul_ConvertBuffer, (UCHAR *) pul_ConvertBuffer, TX * TY, c);
				else
					TEX_Blend24_Color25((UCHAR *) pul_ConvertBuffer, (UCHAR *) pul_ConvertBuffer, TX * TY, c);
				break;
			}

			/* Keep good Border Here */
			if((TX > 2) && (TY > 2) && (l_MipmapFlag & TEX_FP_MipmapKeepBorder))
				TEX_Blend24_KeepBorder((UCHAR *) pul_ConvertBuffer, TX, TY);

			/* Compress Image by Two */
			if((TX == 1) && (TY == 1))
			{
				break;
			}

			if(TX > 1)
			{
				TEX_Compress24_Xo2((UCHAR *) pul_ConvertBuffer, (UCHAR *) pul_ConvertBuffer, TX, TY);
				TX >>= 1;
			}

			if(TY > 1)
			{
				if(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_InvertMipmap)
					TEX_Compress24_Yo2InvertColor((UCHAR *) pul_ConvertBuffer, (UCHAR *) pul_ConvertBuffer, TX, TY);
				else
					TEX_Compress24_Yo2((UCHAR *) pul_ConvertBuffer, (UCHAR *) pul_ConvertBuffer, TX, TY);
				TY >>= 1;
			}

			MMC++;
			i_MipmapLevel--;
		}
	}

    L_free( p_Buf );

	if(DX8_TextureHard)
	{
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/UpdateTexture( /*pst_SD->mp_D3DDevice,*/ (IDirect3DBaseTexture8 *) DX8_Texture, (IDirect3DBaseTexture8 *) DX8_TextureHard );
		DX8_Texture->/*lpVtbl->*/Release(/*DX8_Texture*/);
#else
		pst_SD->mp_D3DDevice->lpVtbl->UpdateTexture( pst_SD->mp_D3DDevice, (IDirect3DBaseTexture8 *) DX8_Texture, (IDirect3DBaseTexture8 *) DX8_TextureHard );
		DX8_Texture->lpVtbl->Release(DX8_Texture);
#endif
	}
	(IDirect3DTexture8 *) pst_SD->dul_Texture[_ul_Texture] = DX8_TextureHard;
}

/*
 =======================================================================================================================
    Aim:    unload all texture from hardware memory
 =======================================================================================================================
 */
void DX8_Texture_Unload(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					*pul_Texture, *pul_LastTexture;
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	pul_Texture = (ULONG *) pst_SD->dul_Texture;
	pul_LastTexture = pul_Texture + pst_SD->l_NumberOfTextures;

	for(; pul_Texture < pul_LastTexture; pul_Texture++)
	{
		if(*pul_Texture)
		{
#ifdef JADEFUSION
			((IDirect3DTexture8 *) * pul_Texture)->/*lpVtbl->*/Release(/*((IDirect3DTexture8 *) * pul_Texture)*/);
#else
			((IDirect3DTexture8 *) * pul_Texture)->lpVtbl->Release(((IDirect3DTexture8 *) * pul_Texture));
#endif
		}
	}

	if(pst_SD->dul_Texture)
	{
		MEM_Free(pst_SD->dul_Texture);
		pst_SD->dul_Texture = NULL;
	}

	/*
	 * if(pst_SD->dul_TextureDeltaBlend) { MEM_Free(pst_SD->dul_TextureDeltaBlend);
	 * pst_SD->dul_TextureDeltaBlend = NULL; }
	 */
	pst_SD->l_NumberOfTextures = 0;
}

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ::: Texture Compression :::::::::::::::::::::::::::::::::::::::::::::::::::
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#if defined( ACTIVE_EDITORS )

#define m_numMips   1       // maybe one day we would like to precompute MipMap levels.
                            // but today is not yet that day.

#define m_bXbox     1       // try to use XBox XGraphics library to do texture
                            // conversion.


/**
 * XDXTEXLoadSurfaceFromSurface
 *
 * @param pDestSurface
 * @param pDestPalette
 * @param pDestRect
 * @param pSrcSurface
 * @param pSrcPalette
 * @param pSrcRect
 * @param Filter
 * @param ColorKey
 * @param pd3ddev
 * @param UseXGR
 * @return
 */

static HRESULT WINAPI
XDXTEXLoadSurfaceFromSurface( LPDIRECT3DSURFACE8 pDestSurface,
                              const PALETTEENTRY* pDestPalette,
                              const RECT* pDestRect,
                              LPDIRECT3DSURFACE8 pSrcSurface,
                              const PALETTEENTRY* pSrcPalette,
                              const RECT* pSrcRect,
                              DWORD Filter,
                              D3DCOLOR ColorKey,
                              LPDIRECT3DDEVICE8 pd3ddev,
                              bool UseXGR)
{
    // Check the Dest Surface to see if a DXTn format is requested
    D3DSURFACE_DESC sd;
#if defined(_XBOX) || defined(_XENON)
    D3DSURFACE_DESC ss;
#endif

    IDirect3DSurface8_GetDesc( pDestSurface, &sd );
#if defined(_XBOX) || defined(_XENON)
    if(UseXGR &&
       ((sd.Format == D3DFMT_DXT1) ||
        (sd.Format == D3DFMT_DXT2) ||
        (sd.Format == D3DFMT_DXT3) ||
        (sd.Format == D3DFMT_DXT4) ||
        (sd.Format == D3DFMT_DXT5)))
    {
        BOOL bNoTmpTexture;
        HRESULT hr;
        LPDIRECT3DTEXTURE8 pTmpTex;
        LPDIRECT3DSURFACE8 pTmpSurf;
        D3DLOCKED_RECT sr, dr;
        BOOL bPreMultiply;
        DWORD xFormat;

        OutputDebugString("************XGraphics did compression\n");
        IDirect3DSurface8_GetDesc( pSrcSurface, &ss );
        if((ss.Width == sd.Width) &&
           (ss.Height == sd.Height) &&
           (ss.Format == D3DFMT_A8R8G8B8))
        {
            bNoTmpTexture = TRUE;
        }
        else
        {
            bNoTmpTexture = FALSE;
        }

        // Create an intermediate texture if necessary
        if(!bNoTmpTexture)
        {
            hr = IDirect3DDevice8_CreateTexture( pd3ddev,
                                                 sd.Width, sd.Height,
                                                 1,
                                                 0,
                                                 D3DFMT_A8R8G8B8,
                                                 D3DPOOL_MANAGED,
                                                 &pTmpTex );
            if (FAILED(hr))
            {
                //AfxMessageBox(ID_ERROR_CANTCREATETEXTURE);
                return E_FAIL;
            }
            hr = IDirect3DTexture8_GetSurfaceLevel( pTmpTex, 0, &pTmpSurf );
            // put converted bits into temporary surface
            D3DXLoadSurfaceFromSurface(pTmpSurf,
                                       NULL,
                                       pDestRect,
                                       pSrcSurface,
                                       pSrcPalette,
                                       pSrcRect,
                                       Filter,
                                       ColorKey);
        }
        // now use XGraphics to convert to dest format
        if(bNoTmpTexture)
        {
            IDirect3DSurface8_LockRect(pSrcSurface, &sr, NULL, 0 );
        }
        else
        {
            IDirect3DSurface8_LockRect( pTmpSurf, &sr, NULL, 0);
        }

        IDirect3DSurface8_LockRect( pDestSurface, &dr, NULL, 0);
        switch(sd.Format)
        {
        case D3DFMT_DXT1:
            xFormat = 0x0000000C;
            bPreMultiply = FALSE;
            break;
        case D3DFMT_DXT2:
            xFormat = 0x0000000E;
            bPreMultiply = TRUE;
            break;
        case D3DFMT_DXT3:
            xFormat = 0x0000000E;
            bPreMultiply = FALSE;
            break;
        case D3DFMT_DXT4:
            xFormat = 0x0000000F;
            bPreMultiply = TRUE;
            break;
        case D3DFMT_DXT5:
            xFormat = 0x0000000F;
            bPreMultiply = FALSE;
            break;
        }

        // Finally!  Do the compression.
        XGCompressRect( dr.pBits,
                        (D3DFORMAT)xFormat,
                        0,
                        sd.Width,
                        sd.Height,
                        sr.pBits,
                        (D3DFORMAT)0x00000012, // xbox D3DFMT_LIN_A8R8G8B8
                        sr.Pitch,
                        0.5f,
                        bPreMultiply ? XGCOMPRESS_PREMULTIPLY : 0);
        
        if(bNoTmpTexture)
        {
            IDirect3DSurface8_UnlockRect( pSrcSurface );
            OutputDebugString("************No Temp Surface\n");
        }
        else
        {
            IDirect3DSurface8_UnlockRect( pTmpSurf );
            IDirect3DSurface8_Release( pTmpSurf );
            IDirect3DSurface8_Release( pTmpTex );
            OutputDebugString("************CREATED Temp Surface\n");
        }

        IDirect3DSurface8_UnlockRect( pDestSurface );
        return S_OK;
    }
    else
#endif
    {
        OutputDebugString("D3DX did conversion\n");
        return D3DXLoadSurfaceFromSurface(pDestSurface,
                                          pDestPalette,
                                          pDestRect,
                                          pSrcSurface,
                                          pSrcPalette,
                                          pSrcRect,
                                          Filter,
                                          ColorKey);
    }
}

/**
 * BltAllLevels
 *
 * @param ptexSrc
 * @param ptexDest
 * @return
 */


static HRESULT
BltAllLevels( LPDIRECT3DTEXTURE8 ptexSrc,
              LPDIRECT3DTEXTURE8 ptexDest)
{
    HRESULT hr;
    LPDIRECT3DTEXTURE8 pmiptexSrc;
    LPDIRECT3DTEXTURE8 pmiptexDest;
    LPDIRECT3DSURFACE8 psurfSrc;
    LPDIRECT3DSURFACE8 psurfDest;
    DWORD iLevel;
    LPDIRECT3DDEVICE8 pd3ddev;
    
    // je voulè an devis de l'ecran

    pd3ddev = DX8_GetSpecificData()->mp_D3DDevice;

    pmiptexSrc = ptexSrc;
    pmiptexDest = ptexDest;

    for (iLevel = 0; iLevel < m_numMips; iLevel++)
    {
        hr = IDirect3DTexture8_GetSurfaceLevel(pmiptexSrc, iLevel, &psurfSrc);
        if( FAILED( hr ))
        {
            return hr;
        }
        hr = IDirect3DTexture8_GetSurfaceLevel( pmiptexDest, iLevel, &psurfDest);
        if( FAILED( hr ))
        {
            return hr;
        }
        hr = XDXTEXLoadSurfaceFromSurface( psurfDest,
                                           NULL,
                                           NULL,
                                           psurfSrc,
                                           NULL,
                                           NULL,
                                           D3DX_FILTER_TRIANGLE,
                                           0,
                                           pd3ddev,
                                           m_bXbox);
        if( FAILED( hr ))
        {
            return hr;
        }
    }

    return S_OK;
}

#endif

#if 0
PDIRECT3DTEXTURE8*
DX8_Compress( TEX_tdst_File_Desc* textureDescription, D3DFORMAT fmtTo )
{
    HRESULT hr;
    LPDIRECT3DDEVICE8 pd3ddev = DX8_GetSpecificData()->mp_D3DDevice;
    LPDIRECT3DTEXTURE8 pmiptex;
    D3DFORMAT fmtFrom;
    LPDIRECT3DTEXTURE8 pmiptexNew;
    LPDIRECT3DTEXTURE8 m_ptexNew;
    D3DSURFACE_DESC sd;

    pmiptex = (LPDIRECT3DTEXTURE8)textureDescription->p_Bitmap;       // questo non è vero!!!
    IDirect3DTexture8_GetLevelDesc(pmiptex, 0, &sd);
    fmtFrom = sd.Format;

    if (fmtFrom == D3DFMT_DXT2 || fmtFrom == D3DFMT_DXT4)
    {
        if (fmtTo == D3DFMT_DXT1)
        {
            return NULL;
        }
        else if (fmtTo != D3DFMT_DXT2 && fmtTo != D3DFMT_DXT4)
        {
//            AfxMessageBox(ID_ERROR_PREMULTALPHA);
            return NULL;
        }
    }

    hr = IDirect3DDevice8_CreateTexture(pd3ddev,
                                        textureDescription->uw_Width,
                                        textureDescription->uw_Height,
                                        m_numMips,
                                        0,
                                        fmtTo,
                                        D3DPOOL_MANAGED,
                                        &pmiptexNew);
    if( FAILED(hr) )
    {
        return NULL;
    }
    ReleasePpo(&m_ptexNew);
    m_ptexNew = pmiptexNew;
    hr = BltAllLevels( m_ptexOrig, m_ptexNew );
    if( FAILED(hr) )
    {
        return NULL;
    }

    return m_ptexNew;
}

static LPDIRECT3DTEXTURE8
createDx8TextureCompatible( const TEX_tdst_File_Desc* textureDescription )
{
    LPDIRECT3DDEVICE8 pd3ddev = DX8_GetSpecificData()->mp_D3DDevice;
    PDIRECT3DTEXTURE8 texture = NULL;
    D3DFORMAT format;
    void* sourceBuffer;
    UINT pitch;

    switch( textureDescription->uc_FinalBPP )
    {
        case 24:
            format = D3DFMT_X8R8G8B8;
            sourceBuffer = textureDescription->p_Bitmap;
            pitch = 3*textureDescription->uw_Width;
            break;
        case 32:
            format = D3DFMT_A8R8G8B8;
            sourceBuffer = textureDescription->p_Bitmap;
            pitch = 4*textureDescription->uw_Width;
            break;
        case 4:
            format = D3DFMT_P8;
            sourceBuffer = L_malloc( textureDescription->uw_Width * textureDescription->uw_Height );
            TEX_Convert_4To8( sourceBuffer,
                              textureDescription->p_Bitmap,
                              textureDescription->uw_Width,
                              textureDescription->uw_Height );
            pitch = textureDescription->uw_Width;
            break;
        case 8:
            format = D3DFMT_P8;
            sourceBuffer = textureDescription->p_Bitmap;
            pitch = textureDescription->uw_Width;
            break;
        default:
            format = D3DFMT_UNKNOWN;
            sourceBuffer = textureDescription->p_Bitmap;
            break;
    }
    if( format != D3DFMT_UNKNOWN )
    {
        HRESULT hr;

        hr = IDirect3DDevice8_CreateTexture(pd3ddev,
                                            textureDescription->uw_Width,
                                            textureDescription->uw_Height,
                                            1,      // mip map number.
                                            0,
                                            format,
                                            D3DPOOL_MANAGED,
                                            &texture);
        if( !FAILED( hr ))
        {
            LPDIRECT3DSURFACE8 surface;

            hr = IDirect3DTexture8_GetSurfaceLevel( texture, 0, &surface );
            if( !FAILED( hr ))
            {
                RECT srcRect;

                srcRect.left = 0;
                srcRect.top = 0;
                srcRect.right = textureDescription->uw_Width;
                srcRect.bottom = textureDescription->uw_Height;

                D3DXLoadSurfaceFromMemory( surface,             // pDestSurface
                                           NULL,                // pDestPalette
                                           NULL,                // pDestRect
                                           sourceBuffer,        // pSrcMemory
                                           format,              // SrcFormat
                                           pitch,               // SrcPitch
                                           textureDescription->p_Palette,                    // pSrcPalette
                                           &srcRect,            // pSrcRect
                                           D3DX_FILTER_NONE,    // Filter
                                           0);                  // ColorKey
/*


                D3DLOCKED_RECT lock;

                hr = IDirect3DSurface8_LockRect( surface, &lock, NULL, 0 );
                if( !FAILED( hr ))
                {
                    D3DSURFACE_DESC surfaceDesc;

                    hr = IDirect3DSurface8_GetDesc( surface, &surfaceDesc );
                    if( !FAILED( hr ))
                    {
                        memcpy( lock.pBits, sourceBuffer, surfaceDesc.Size );
                    }
                    IDirect3DSurface8_UnlockRect( surface );
                }
*/
                IDirect3DSurface8_Release( surface );
            }
        }
    }
    if( textureDescription->uc_FinalBPP == 4 )
    {
        L_free( sourceBuffer );
    }
    return texture;
}

static LPDIRECT3DTEXTURE8
createDx8TextureCompressed( const TEX_tdst_File_Desc* textureDescription )
{
    LPDIRECT3DDEVICE8 pd3ddev = DX8_GetSpecificData()->mp_D3DDevice;
    LPDIRECT3DTEXTURE8 texture = NULL;
    D3DFORMAT format;

    switch( textureDescription->uc_FinalBPP )
    {
        case 24:
            format = D3DFMT_DXT1;
            break;
        case 32:
            format = D3DFMT_DXT1;
            break;
        case 4:
            format = D3DFMT_DXT1;
            break;
        case 8:
            format = D3DFMT_DXT1;
            break;
        default:
            format = D3DFMT_UNKNOWN;
            break;
    }
    if( format != D3DFMT_UNKNOWN )
    {
        HRESULT hr;

        hr = IDirect3DDevice8_CreateTexture(pd3ddev,
                                            textureDescription->uw_Width,
                                            textureDescription->uw_Height,
                                            1,      // mip map number.
                                            0,
                                            format,
                                            D3DPOOL_MANAGED,
                                            &texture);
        if( FAILED( hr ))
        {
            texture = NULL;
        }
    }
    return texture;
}

static void*
extractTextureBits( LPDIRECT3DTEXTURE8 texture, size_t* size )
{
    D3DLOCKED_RECT lock;
    HRESULT hr;
    void* buffer = NULL;
    LPDIRECT3DSURFACE8 surface;

    hr = IDirect3DTexture8_GetSurfaceLevel( texture, 0, &surface );
    if( !FAILED( hr ))
    {
        hr = IDirect3DSurface8_LockRect( surface, &lock, NULL, 0 );
        if( !FAILED( hr ))
        {
            D3DSURFACE_DESC surfaceDesc;

            hr = IDirect3DSurface8_GetDesc( surface, &surfaceDesc );
            if( !FAILED( hr ))
            {
                buffer = malloc( surfaceDesc.Size );
                if( buffer != NULL )
                {
                    *size = surfaceDesc.Size;
                    memcpy( buffer, lock.pBits, surfaceDesc.Size );
                }
            }
            IDirect3DSurface8_UnlockRect( surface );
        }
        IDirect3DSurface8_Release( surface );
    }

    return buffer;
}

void*
DX8_DtcCompress( const TEX_tdst_File_Desc* textureDescription, size_t* newSize )
{
LPDIRECT3DTEXTURE8 sourceTexture;
LPDIRECT3DTEXTURE8 destTexture;
void* buffer = NULL;

    // first if the texture is not power of two, then pad it.
    // create a DX8 texture from the textureDescription data.
    // fill its bits with the textureDescription->bitMap
    sourceTexture = createDx8TextureCompatible( textureDescription );

    // create a DX8 DXT texture with the same dimension.
    destTexture = createDx8TextureCompressed( textureDescription );

    // blit from the first to the latter.
    if( sourceTexture != NULL && destTexture != NULL )
    {
        HRESULT hr; 

        hr = BltAllLevels( sourceTexture, destTexture );
        if( FAILED( hr ))
        {
            buffer = NULL;
        }
        else
        {
            buffer = extractTextureBits( destTexture, newSize );
        }
    }
    if( sourceTexture != NULL )
    {
        IDirect3DTexture8_Release( sourceTexture );
    }
    if( destTexture != NULL )
    {
        IDirect3DTexture8_Release( destTexture );
    }
    return buffer;
}
#endif  // defined( ACTIVE_EDITORS )

#if defined PSX2_TARGET && defined __cplusplus
}
#endif
