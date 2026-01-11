// Dx9tex.c


#include "Dx9tex.h"
#include "BASe/MEMory/MEM.h"
#include "TEXture/TEXconvert.h"

#ifdef _PC_MEMORY_CHECK
#include "../Main/MainPC/Sources/MemoryTracker.h"
#endif //_PC_MEMORY_CHECK

/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

/////////////
void	Dx9_Texture_Load( GDI_tdst_DisplayData *pDD, TEX_tdst_Data *pTexData, TEX_tdst_File_Desc *pTexFileDesc, ULONG textureId )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						c;
	ULONG					TX, TY, MMC, BPP;
	ULONG					*pul_ConvertBuffer;
	int						i_MipmapLevel;
	ULONG					l_MipmapFlag;
	IDirect3DTexture9		*Dx9_Texture, *Dx9_TextureHard;
    HRESULT                 hr;
    IDirect3DSurface9	    *Dx9_Surface;
    D3DLOCKED_RECT		    stLockedRect;
    ULONG		            LineCounter;
	unsigned char		    *BitPtr, *DestPtr;
    char                    *p_Buf = NULL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	(void)pDD;
	TX = pTexData->w_Width;
	TY = pTexData->w_Height;
    BPP = pTexFileDesc->uc_FinalBPP;
	MMC = 0;
	i_MipmapLevel = 1;

	if(pTexFileDesc->st_Params.uw_Flags & TEX_FP_MipmapOn)
	{
		pTexFileDesc->uw_FileFlags |= TEX_uw_Mipmap;
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
		pTexFileDesc->uw_FileFlags &= ~TEX_uw_Mipmap;
	}

	pTexData->uw_Flags = pTexFileDesc->uw_FileFlags;

	c = pTexFileDesc->st_Params.ul_Color;
	l_MipmapFlag = pTexFileDesc->st_Params.uw_Flags & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor | TEX_FP_MipmapKeepBorder);

	pul_ConvertBuffer = (ULONG *) pTexFileDesc->p_Bitmap;

	Dx9_Texture = Dx9_TextureHard = NULL;

	if(BPP == 32)
	{
		hr = IDirect3DDevice9_CreateTexture( gDx9SpecificData.pD3DDevice, TX, TY, i_MipmapLevel, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM , &Dx9_Texture, NULL );
		hr = IDirect3DDevice9_CreateTexture( gDx9SpecificData.pD3DDevice, TX, TY, i_MipmapLevel, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &Dx9_TextureHard, NULL );

		TEX_Convert_32SwapRB(pul_ConvertBuffer, TX * TY);

		while((TX > 0) && (TY > 0))
		{
			pDD->st_TexManager.l_MemoryTakenByLoading += TX * TY * 4L;
			{
				IDirect3DTexture9_GetSurfaceLevel(Dx9_Texture, MMC, &Dx9_Surface);
				IDirect3DSurface9_LockRect(Dx9_Surface, &stLockedRect, NULL, 0);
				BitPtr = stLockedRect.pBits;
				DestPtr = (unsigned char *) pul_ConvertBuffer;
				LineCounter = TY;
				while(LineCounter--)
				{
					memcpy(BitPtr, DestPtr, TX * 4);
					DestPtr += TX * 4;
					BitPtr += stLockedRect.Pitch;
				}

				IDirect3DSurface9_UnlockRect(Dx9_Surface);
				IDirect3DSurface9_Release(Dx9_Surface);
			}

			if(!(pTexFileDesc->st_Params.uw_Flags & TEX_FP_MipmapOn)) break;

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
				if(pDD->st_TexManager.ul_Flags & TEX_Manager_InvertMipmap)
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
		IDirect3DDevice9_CreateTexture( gDx9SpecificData.pD3DDevice, TX, TY, i_MipmapLevel, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &Dx9_Texture, NULL );
		IDirect3DDevice9_CreateTexture( gDx9SpecificData.pD3DDevice, TX, TY, i_MipmapLevel, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &Dx9_TextureHard, NULL );

        TEX_Convert_24To32SwapRB(pul_ConvertBuffer, TX * TY);

		while((TX > 0) && (TY > 0))
		{
			pDD->st_TexManager.l_MemoryTakenByLoading += TX * TY * 3L;
			{
				IDirect3DTexture9_GetSurfaceLevel(Dx9_Texture, MMC, &Dx9_Surface);
				IDirect3DSurface9_LockRect(Dx9_Surface, &stLockedRect, NULL, 0);
				BitPtr = stLockedRect.pBits;
				DestPtr = (unsigned char *) pul_ConvertBuffer;
				LineCounter = TY;
				while(LineCounter--)
				{
					memcpy(BitPtr, DestPtr, TX * 4);
					DestPtr += TX * 4;
					BitPtr += stLockedRect.Pitch;
				}

				IDirect3DSurface9_UnlockRect(Dx9_Surface);
				IDirect3DSurface9_Release(Dx9_Surface);
			}

			if(!(pTexFileDesc->st_Params.uw_Flags & TEX_FP_MipmapOn)) break;

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
				if(pDD->st_TexManager.ul_Flags & TEX_Manager_InvertMipmap)
					TEX_Compress24_Yo2InvertColor((UCHAR *) pul_ConvertBuffer, (UCHAR *) pul_ConvertBuffer, TX, TY);
				else
					TEX_Compress24_Yo2((UCHAR *) pul_ConvertBuffer, (UCHAR *) pul_ConvertBuffer, TX, TY);
				TY >>= 1;
			}

			MMC++;
			i_MipmapLevel--;
		}
	}
	else if ( (BPP == 8) || (BPP == 4) )
	{
        if (BPP == 4)
        {
            p_Buf = (char *) malloc( TX * TY );
            TEX_Convert_4To8( (unsigned char *) p_Buf, (unsigned char *) pul_ConvertBuffer, TX, TY );
            pul_ConvertBuffer = (ULONG *) p_Buf;
        }

        /*

        // loop through texture to find a good association
        if (TEX_DD->st_TexManager.ul_Flags & TEX_Manager_OneTexForRawPal)
        {
            pst_RawPal = TEX_gst_GlobalList.dst_Texture;
            for (c = 0; c < TEX_gst_GlobalList.l_NumberOfTextures; c++, pst_RawPal++)
            {
                if ( (pst_RawPal->uw_Flags & TEX_uw_RawPal) && (pst_RawPal->w_Height == (SHORT) textureId) )
                {
                    // watch if palette is updatable, in such case we have to keep the buffer and association of buffer and palette
                    if (TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_Width ].uc_Flags & TEX_uc_UpdatablePal )
                        OGL_Texture_AddDataForUpdatablePalette( TEX_SD, (short) c, pst_RawPal->w_Width, (short) TX, (short) TY, TX * TY, (char *) pul_ConvertBuffer );

                    LoadCB( c, pst_RawPal->ul_Key, &ul_Texture, 0, 0, BPP, TX, TY, GL_COLOR_INDEX, pul_ConvertBuffer, pst_RawPal->w_Width );

                    TEX_SD->dul_Texture[ c ] = ul_Texture;
                    ul_Texture = -1;
                }
            }
        }
        else
        {
            pst_RawPal = TEX_gst_GlobalList.dst_Texture;
            for (c = 0; c < TEX_gst_GlobalList.l_NumberOfTextures; c++, pst_RawPal++)
            {
                if ( (pst_RawPal->uw_Flags & TEX_uw_RawPal) && (pst_RawPal->w_Height == (SHORT) textureId) )
                    break;
            }

            // watch if palette is updatable, in such case we have to keep the buffer and association of buffer and palette
            if (c != TEX_gst_GlobalList.l_NumberOfTextures)
            {
                if (TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_Width ].uc_Flags & TEX_uc_UpdatablePal )
                    OGL_Texture_AddDataForUpdatablePalette( TEX_SD, pTexData->w_Index, pst_RawPal->w_Width, (short) TX, (short) TY, TX * TY, (char *) pul_ConvertBuffer );
                w_Pal = pst_RawPal->w_Width;
            }
            else
                w_Pal = -1;

            LoadCB( textureId, pTexData->ul_Key, &ul_Texture, 0, 0, BPP, TX, TY, GL_COLOR_INDEX, pul_ConvertBuffer, w_Pal );
        }
        */

        

		//Dx9_Set_Texture_Palette(pDD, 0, 0, 0);

		pDD->st_TexManager.l_MemoryTakenByLoading += TX * TY;

		hr = IDirect3DDevice9_CreateTexture( gDx9SpecificData.pD3DDevice, TX, TY, 1, 0, D3DFMT_P8, D3DPOOL_SYSTEMMEM, &Dx9_Texture, NULL );
		hr = IDirect3DDevice9_CreateTexture( gDx9SpecificData.pD3DDevice, TX, TY, 1, 0, D3DFMT_P8, D3DPOOL_DEFAULT, &Dx9_TextureHard, NULL );
        //IDirect3DDevice9_SetCurrentTexturePalette( gDx9SpecificData.pD3DDevice, 0 );

		if ( Dx9_Texture )
		{
			IDirect3DTexture9_GetSurfaceLevel(Dx9_Texture, MMC, &Dx9_Surface);
			IDirect3DSurface9_LockRect(Dx9_Surface, &stLockedRect, NULL, 0);
			BitPtr = stLockedRect.pBits;
			DestPtr = (unsigned char *) pul_ConvertBuffer;
			LineCounter = TY;
			while(LineCounter--)
			{
				memcpy(BitPtr, DestPtr, TX);
				DestPtr += TX;
				BitPtr += stLockedRect.Pitch;
			}
    		IDirect3DSurface9_UnlockRect(Dx9_Surface);
    		IDirect3DSurface9_Release(Dx9_Surface);

			if (BPP == 4) 
				free( p_Buf );
		}       
	}

	if ( Dx9_TextureHard )
	{
		IDirect3DDevice9_UpdateTexture( gDx9SpecificData.pD3DDevice, (IDirect3DBaseTexture9 *) Dx9_Texture, (IDirect3DBaseTexture9 *) Dx9_TextureHard );
		IDirect3DTexture9_Release(Dx9_Texture);
	}
	(IDirect3DTexture9 *) gDx9SpecificData.TextureList[textureId] = Dx9_TextureHard;
}

/////////////
LONG	Dx9_l_Texture_Store( GDI_tdst_DisplayData *pDD, TEX_tdst_Data *pTexData, TEX_tdst_File_Desc *pTexFileDesc, ULONG index )
{
	(void) pDD;
	(void) pTexData;
	(void) pTexFileDesc;
	(void) index;

	return 0;
}

/////////////
void	Dx9_Palette_Load( GDI_tdst_DisplayData *pDD, TEX_tdst_Palette *pPalette, ULONG index)
{
    TEX_tdst_Palette        *pst_Pal;
    ULONG                   *pul_Color;

	(void)pDD;
    if (index == -1) 
		return;

    pst_Pal = (TEX_tdst_Palette *) pPalette;
	pul_Color = pst_Pal->pul_Color;

	IDirect3DDevice9_SetPaletteEntries( gDx9SpecificData.pD3DDevice, index, (PALETTEENTRY *) pul_Color);
}

/////////////
void	Dx9_Set_Texture_Palette( GDI_tdst_DisplayData *pDD, ULONG textureNumber, ULONG indexTex, ULONG indexPal )
{
	(void) pDD;
	(void) textureNumber;
	(void) indexTex;
	(void) indexPal;
}

/////////////
void	Dx9_Texture_Unload( GDI_tdst_DisplayData *pDD )
{
	ULONG					*pul_Texture, *pul_LastTexture;

	(void)pDD;
	pul_Texture = (ULONG *) gDx9SpecificData.TextureList;
	pul_LastTexture = pul_Texture + gDx9SpecificData.NumberOfTextures;

	for (; pul_Texture < pul_LastTexture; pul_Texture++ )
	{
		if ( *pul_Texture )
		{
			IDirect3DTexture9_Release( ( (IDirect3DTexture9 *) * pul_Texture ) );
		}
	}

	if( gDx9SpecificData.TextureList )
	{
		MEM_Free( gDx9SpecificData.TextureList );
		gDx9SpecificData.TextureList = NULL;
	}

	gDx9SpecificData.NumberOfTextures = 0;
}

/////////////
LONG	Dx9_l_Texture_Init( GDI_tdst_DisplayData *pDD, ULONG textureNumber )
{
	LONG					l_Size;

	(void)pDD;
	if ( textureNumber == 0) 
		textureNumber = 1;

	gDx9SpecificData.NumberOfTextures = textureNumber;

	l_Size = (textureNumber + 1) * sizeof(ULONG);
	gDx9SpecificData.TextureList = (IDirect3DTexture9 **) MEM_p_Alloc( l_Size );
	memset( gDx9SpecificData.TextureList, 0, l_Size );

	return TRUE;
}
