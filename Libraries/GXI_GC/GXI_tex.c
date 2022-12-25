
/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Load texture in memory to be used by GameCube RC.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "GEOmetric/GEOobject.h" 
#include "GXI_init.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXmemory.h"

#include "GXI_tex.h"

#define USE_PALETTE_NAMES

#ifdef USE_MY_TEX_REGION
	#define MY_TEX_REGION_NUM_REGIONS     8 // number of caches
	#define MY_TEX_REGION_TCACHE_SZ       GX_TEXCACHE_32K
	u32 g_nbTextureLoaded=0;
	GXTexRegion  MyTexRegions[MY_TEX_REGION_NUM_REGIONS]; // cache regions
#endif

void *MEM_TEX_p_Alloc(ULONG MemSize ) 
{
	{
#ifdef GC_BENCHGRAPH
		void *ptrReturn;
		ULONG	ulPreviousSize;
		ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
		ptrReturn = MEM_p_Alloc(MemSize);
		GXI_Global_ACCESS(ulTEXMEM) += MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - ulPreviousSize;
		return ptrReturn;
#else	
		return MEM_p_Alloc(MemSize);
#endif
	}
	
};
void *MEM_TEX_p_AllocAlign(ULONG MemSize , ULONG Align ) 	
{
#ifdef GC_BENCHGRAPH
	{
	void *ptrReturn;
	ULONG	ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	ptrReturn = MEM_p_AllocAlign(MemSize,Align);
	GXI_Global_ACCESS(ulTEXMEM) += MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - ulPreviousSize;
	return (void *) ptrReturn;
	}
#else
	return MEM_p_AllocAlign(MemSize,Align);
#endif	
	
};
void MEM_TEX_v_Free(void *ptr ) 			
{
#ifdef GC_BENCHGRAPH
	{
	ULONG	ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	MEM_Free(ptr);
	GXI_Global_ACCESS(ulTEXMEM) -= ulPreviousSize - MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	}
#else
	MEM_Free(ptr);
#endif
};
void MEM_TEX_v_FreeAlign(void *ptr ) 		
{
#ifdef GC_BENCHGRAPH
	{
	ULONG	ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	MEM_FreeAlign(ptr);
	GXI_Global_ACCESS(ulTEXMEM) -= ulPreviousSize - MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	}
#else
	MEM_FreeAlign(ptr);
#endif
};


/*
 =======================================================================================================================
    Aim:    Utility fonction used to find the power of 2 quickly
 =======================================================================================================================
 */
u32 GXI_GetPowerOf2( u32 K ) 
{
	switch (K)
	{
		case 1:return 	0L;
		case 2:return 	1L;
		case 4:return 	2L;
		case 8:return 	3L;
		case 16:return 	4L;
		case 32:return 	5L;
		case 64:return 	6L;
		case 128:return 7L;
		case 256:return 8L;
		case 512:return 9L;
		case 1024:return 10L;
	}
	return 0;
}

/*
 =======================================================================================================================
    Aim:    Create a texture from raw texdata
 =======================================================================================================================
 */
void GXI_Texture_Load
(
    GDI_tdst_DisplayData    *_pst_DD,
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG 		    ulTextNum 
)
{
	u32 	u32TextureSize, W, H;
	int		c;
	int		i_MipmapLevel;
	ULONG	TX, TY;
	ULONG	l_MipmapFlag;
	ULONG   *pul_ConvertBuffer;
	
	GXI_tdst_TextureDesc *l_pstTexDesc = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum];
	
	// set attributes
	
	l_pstTexDesc->wrap_s = GX_CLAMP;
	l_pstTexDesc->wrap_t = GX_CLAMP;

	l_pstTexDesc->paletteNum = -1;
	
	TX = W = l_pstTexDesc->W = _pst_Tex->uw_Width;
	TY = H = l_pstTexDesc->H = _pst_Tex->uw_Height;
	
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
		
		l_pstTexDesc->bUseMipMap = 1;
	}
	else
	{
		_pst_Tex->uw_FileFlags &= ~TEX_uw_Mipmap;
		l_pstTexDesc->bUseMipMap = 0;
	}
	
	c = _pst_Tex->st_Params.ul_Color;
	l_MipmapFlag = _pst_Tex->st_Params.uw_Flags & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor | TEX_FP_MipmapKeepBorder);

	pul_ConvertBuffer = (ULONG *) _pst_Tex->p_Bitmap;

	if(_pst_Tex->uc_FinalBPP == 32) // 32bits texture
	{
		l_pstTexDesc->formatTEX=GX_TF_RGBA8;

		u32TextureSize = GXGetTexBufferSize( l_pstTexDesc->W, 
										 	 l_pstTexDesc->H, 
										 	 l_pstTexDesc->formatTEX, 
										 	 l_pstTexDesc->bUseMipMap ? GX_TRUE : GX_FALSE, 
										 	 0xFF);

		// copy raw data
		l_pstTexDesc->bitmapData = MEM_TEX_p_AllocAlign(u32TextureSize, 32);
		
		// now create the tiles to convert bitmap in gamecube format
		// 32 bits is trickier.  Fill two tiles at once.
		{
		u32 tileSize = 32;			
		u8* currARTilePtr = (u8*)l_pstTexDesc->bitmapData;
		u8* currGBTilePtr = (u8*)l_pstTexDesc->bitmapData + tileSize;
		GXColor *color;
		int ix, iy; // image x and y;
		int tileY, tileX;
		int nbXTiles, nbYTiles, nbTexelsPerTileX, nbTexelsPerTileY;
		u32 *srcImage = (u32*)_pst_Tex->p_Bitmap;
			
		nbTexelsPerTileX = 4;
		nbTexelsPerTileY = 4;

		while((TX > 0) && (TY > 0)) // for all mipmaps...
		{
			nbXTiles = TX/nbTexelsPerTileX;
			nbYTiles = TY/nbTexelsPerTileY;

			for(tileY = 0; tileY < nbYTiles; tileY++)
			{
				for(tileX = 0; tileX < nbXTiles; tileX++)
				{
					// fill the tiles texel by texel
					int tx, ty; // tile x and y
					for(ty = 0; ty < nbTexelsPerTileY; ty++)
					{
						for(tx = 0; tx < nbTexelsPerTileX; tx++)
						{
							// get the color in the image
							ix = tileX*nbTexelsPerTileX+tx;
							iy = tileY*nbTexelsPerTileY+ty;
							if(ix<TX&&iy<TY)
							{
								color = (GXColor*)(srcImage + (iy * TX + ix));
								
								// set the color in both tiles si RGBA
								if(_pst_Tex->uc_BPP != _pst_Tex->uc_FinalBPP ) // 24 bits conversion
								{
									*currARTilePtr = color->r;
									*(currARTilePtr+1) = color->g;
									*currGBTilePtr = color->b;
									*(currGBTilePtr+1) = color->a;
								}
								else
								{
									*currARTilePtr = color->a;
									*(currARTilePtr+1) = color->r;
									*currGBTilePtr = color->g;
									*(currGBTilePtr+1) = color->b;
								}
							}
							currARTilePtr += 2;
							currGBTilePtr += 2;
						}
					}
					currARTilePtr+=tileSize;
					currGBTilePtr+=tileSize;
				}
			}
			
			// scale to next mipmap
			if(!l_pstTexDesc->bUseMipMap) break;

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

			i_MipmapLevel--;
		}
		}
		DCFlushRange(l_pstTexDesc->bitmapData, u32TextureSize);
	}	

//	else if(S3TC) ...

	else // palette
	{
		u32 ix, iy, tileX, tileY, tx, ty;
		GXColor black = { 0,0,0,255 };
		
		l_pstTexDesc->bUseMipMap = FALSE; // no mipmap for palettes
		
		if(_pst_Tex->uc_FinalBPP == 4)
			l_pstTexDesc->formatCI=GX_TF_C4;
		else 
			l_pstTexDesc->formatCI=GX_TF_C8;

		u32TextureSize = GXGetTexBufferSize( l_pstTexDesc->W, 
									 	 	 l_pstTexDesc->H, 
									 	 	 l_pstTexDesc->formatCI, 
									 	 	 l_pstTexDesc->bUseMipMap, 
									 	 	 0xFF);

		// copy raw data
		l_pstTexDesc->bitmapData = MEM_TEX_p_AllocAlign(u32TextureSize, 32);		
		
		if(_pst_Tex->uc_FinalBPP == 4)
		{
			int nbXTiles, nbYTiles, nbTexelsPerTileX, nbTexelsPerTileY;
			u8 *currTilePtr = (u8*)l_pstTexDesc->bitmapData;
			u8 *destImage   = (u8*)_pst_Tex->p_Bitmap;
			
			nbTexelsPerTileX = 8;
			nbTexelsPerTileY = 8;
			
			nbXTiles = W/nbTexelsPerTileX;
			nbYTiles = H/nbTexelsPerTileY;

			// Fill the tiles sequentially.
			for(tileY = 0; tileY < nbYTiles; tileY++)
			{ 
				for(tileX = 0; tileX < nbXTiles; tileX++)
				{
					// fill the tile, 2 texels (1 byte) at a time 
					for(ty = 0; ty < nbTexelsPerTileY; ty++)
					{
						for(tx = 0; tx < nbTexelsPerTileX; tx+=2)
						{
							// get the color in the image
							ix = tileX*nbTexelsPerTileX+tx;
							iy = tileY*nbTexelsPerTileY+ty;
							if(ix<W && iy<H)
							{
								*currTilePtr = *(destImage + (W/2) * iy + (ix/2));
							}

							// set the color in tile
							currTilePtr++;
						}
					}
				}
			}
		}
		else if(_pst_Tex->uc_FinalBPP == 8)	
		{
			int nbXTiles, nbYTiles, nbTexelsPerTileX, nbTexelsPerTileY;
			u8 *currTilePtr = (u8*)l_pstTexDesc->bitmapData;
			u8 *destImage   = (u8*)_pst_Tex->p_Bitmap;
			
			nbTexelsPerTileX = 8;
			nbTexelsPerTileY = 4;
			
			nbXTiles = W/nbTexelsPerTileX;
			nbYTiles = H/nbTexelsPerTileY;
		
			// Fill the tiles sequentially.
			for(tileY = 0; tileY < nbYTiles; tileY++)
			{
				for(tileX = 0; tileX < nbXTiles; tileX++)
				{
					// fill the tile texel by texel
					for(ty = 0; ty < nbTexelsPerTileY; ty++)
					{
						for(tx = 0; tx < nbTexelsPerTileX; tx++)
						{
							// get the color in the image
							ix = tileX*nbTexelsPerTileX+tx;
							iy = tileY*nbTexelsPerTileY+ty;
							if(ix<W&&iy<H)
							{
								*currTilePtr = *(destImage + iy * W + ix);
							}

							// set the color in tile
							currTilePtr++;
						}
					}
				}
			}		
		}
		else
			OSHalt("Unrecognize texture type\n");
			
		DCFlushRange(l_pstTexDesc->bitmapData, u32TextureSize);
			
		return; // settexturepalette will create the texture
	}
	
	// create gamecube texture
	GXInitTexObj(&l_pstTexDesc->obj, 
 			      l_pstTexDesc->bitmapData, 
				  l_pstTexDesc->W, 
				  l_pstTexDesc->H, 
				  l_pstTexDesc->formatTEX, 
				  l_pstTexDesc->wrap_s,
				  l_pstTexDesc->wrap_t,
				  l_pstTexDesc->bUseMipMap ? GX_TRUE : GX_FALSE);

	if(l_pstTexDesc->bUseMipMap)
	{
		f32 maxLevel = GXI_GetPowerOf2(max(W, H));
		GXInitTexObjLOD(&l_pstTexDesc->obj,
						 GX_LIN_MIP_LIN,										// min_filter
						 GX_LINEAR,												// max_filter
						 0,														// min_lod
						 maxLevel,												// max_lod
						 0,														// lod_bias
						 GX_TRUE,												// bias_clamp
						 GX_FALSE,												// do_edge_lod
						 GX_ANISO_1);											// max_aniso
	}
	else
	{
		GXInitTexObjLOD(&l_pstTexDesc->obj,
						 GX_LINEAR,												// min_filter
						 GX_LINEAR,												// max_filter
						 0,														// min_lod
						 0,														// max_lod
						 0,														// lod_bias
						 GX_FALSE,												// bias_clamp
						 GX_FALSE,												// do_edge_lod
						 GX_ANISO_1);											// max_aniso
	}
	
#ifdef USE_MY_TEX_REGION
	GXInitTexObjUserData(&l_pstTexDesc->obj, (void*)++g_nbTextureLoaded);
#endif
	
	l_pstTexDesc->paletteNum = -1;
		
}

/*
 =======================================================================================================================
    Aim:    Free all textures and palettes descriptors
 =======================================================================================================================
 */
void GXI_Texture_Unload( GDI_tdst_DisplayData *_pst_SD )
{
	// Free bitmaps. Deal with shared data.
	u32 i = GXI_Global_ACCESS(ulNumberOfTextures);
	while(i--)
	{
		u32 j = i;
		void *pAddressToFree = GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[i].bitmapData;
		if (pAddressToFree)
		{
			MEM_TEX_v_FreeAlign(pAddressToFree);
			GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[i].bitmapData = NULL;
			while (j--)
			{
				if (pAddressToFree == GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[j].bitmapData) 
				{
					GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[j].bitmapData = NULL;
				}
			}
		}
	}
//	L_memset(GXI_Global_ACCESS(p_MyTextureDescriptorsAligned),0,sizeof(GXI_tdst_TextureDesc) * GXI_Global_ACCESS(ulNumberOfTextures));
	
	// Free palettes. Deal with shared data.
	i = GXI_Global_ACCESS(ulNumberOfPalettes);
	while(i--)
	{
		u32 j = i;
		void *pAddressToFree = GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[i].paletteData;
		if (pAddressToFree)
		{
			MEM_TEX_v_FreeAlign(pAddressToFree);
			GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[i].paletteData = NULL;
			while (j--)
			{
				if (pAddressToFree == GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[j].paletteData) 
				{
					GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[j].paletteData = NULL;
				}
			}
		}
	}
			
	//L_memset(GXI_Global_ACCESS(p_MyPalletteDescriptorAligned),0,sizeof(GXI_tdst_PaletteDesc) * GXI_Global_ACCESS(ulNumberOfPalettes));
	
	if (GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)) 
		MEM_TEX_v_FreeAlign(GXI_Global_ACCESS(p_MyTextureDescriptorsAligned));

	GXI_Global_ACCESS(p_MyTextureDescriptorsAligned) = NULL;
	GXI_Global_ACCESS(ulNumberOfTextures) = 0;
	
	//ERR_X_Assert( (GXI_Global_ACCESS(ulNumberOfPalettes) == 0) == (GXI_Global_ACCESS(p_MyPalletteDescriptorAligned) == NULL));
	
	if (GXI_Global_ACCESS(ulNumberOfPalettes))
	{
		MEM_TEX_v_FreeAlign(GXI_Global_ACCESS(p_MyPalletteDescriptorAligned));
		GXI_Global_ACCESS(ulNumberOfPalettes) = 0;
	}
	
	if(GXI_Global_ACCESS(dul_TextureDeltaBlend))
	{
		MEM_TEX_v_Free(GXI_Global_ACCESS(dul_TextureDeltaBlend));
		GXI_Global_ACCESS(dul_TextureDeltaBlend) = NULL;
	}
}

/*
 =======================================================================================================================
    Aim:    Determine if alpha is in use for this palette
 =======================================================================================================================
 */
BOOL GXI_DoesPaletteUseAlpha(u32 *p_Palette, u32 _nbColors)
{
	u32 Alpha;
	u32 ColorCounter = _nbColors;

	while (ColorCounter --)
	{
		Alpha = *(p_Palette++) >> 24;
		if ((Alpha > 16) && (Alpha < 256-16)) return TRUE;
	}
	
	return FALSE;
}


/*
 =======================================================================================================================
    Aim:    Create a palette instance from the palette data
 =======================================================================================================================
 */
void GXI_Palette_Load( GDI_tdst_DisplayData *, TEX_tdst_Palette *p_PAL, ULONG PaletteNum)
{
	GXTlutFmt palFormat;
	u32		  u32PalSize;

#ifdef USE_PALETTE_NAMES
	static u32 paletteName256=0;
	static u32 paletteName16=0;
#endif	
	
	// check palette type
	
	if(p_PAL->uc_Flags & TEX_uc_Palette16 == 0)
	{
#ifdef _DEBUG	
		OSReport("!!! Only 16bits palettes are supported for now.\n");
#endif		
		return;
	}
		
	// init the palette descriptors if not already done
	
	if (!GXI_Global_ACCESS(ulNumberOfPalettes))
	{
		// KER MODIF ... 256 was not enough :(
//		GXI_Global_ACCESS(ulNumberOfPalettes) = 256;
		GXI_Global_ACCESS(ulNumberOfPalettes) = 512;
		
		GXI_Global_ACCESS(p_MyPalletteDescriptorAligned) = (GXI_tdst_PaletteDesc *) MEM_TEX_p_AllocAlign(sizeof(GXI_tdst_PaletteDesc) * GXI_Global_ACCESS(ulNumberOfPalettes) , 32);
		L_memset(GXI_Global_ACCESS(p_MyPalletteDescriptorAligned),0,sizeof(GXI_tdst_PaletteDesc) * GXI_Global_ACCESS(ulNumberOfPalettes));
	}
	
	// determine palette format
	
	//if (GXI_DoesPaletteUseAlpha(p_PAL->pul_Color, (p_PAL->uc_Flags & TEX_uc_Palette16) ? 16 : 256)) // si alpha utilise
	if(p_PAL->uc_Flags & TEX_uc_AlphaPalette)
		palFormat = GX_TL_RGB5A3;
	else
		palFormat = GX_TL_RGB565;

	if(p_PAL->uc_Flags & TEX_uc_Palette16)
		u32PalSize=16;
	else 
		u32PalSize=256;
		
	// copy palette data
	
	GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteData = MEM_TEX_p_AllocAlign(u32PalSize*2, 32);
	
	// now convert the 32 bits palette into the appropriate 16 bits format
	{
		u32 *pal = (u32*)p_PAL->pul_Color;
		u16 *newpal = (u16*)GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteData;
		u32 i;
		GXColor *oldcol;
		for(i=0; i<u32PalSize; i++)
		{
			oldcol = (GXColor*)pal;

			switch(palFormat)
			{
				case GX_TL_RGB5A3:
				{
					tdst_Palette_RGB5A3 col;
					col.flag = 0x0;
					col.a = (u8)(oldcol->a >> 5) & 0x07;
					col.r = (u8)(oldcol->b >> 4) & 0x0F;
					col.g = (u8)(oldcol->g >> 4) & 0x0F;
					col.b = (u8)(oldcol->r >> 4) & 0x0F;
					*newpal = *(u16*)&col;
				}
				break;
				
				case GX_TL_RGB565:
				{
					tdst_Palette_RGB565 col;
					col.r = (u8)(oldcol->g >> 3) & 0x1F;
					col.g = (u8)(oldcol->b >> 2) & 0x3F;
					col.b = (u8)(oldcol->a >> 3) & 0x1F;
					*newpal = *(u16*)&col;
				}
			}
			
			newpal++; 
			pal++;
		}
	}
	
	DCFlushRange(GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteData, u32PalSize*2);
			
	// create the gamecube palette
	
	GXInitTlutObj(&GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].obj, 
				   GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteData, 
				   palFormat, 
				   u32PalSize);
	
#ifdef USE_PALETTE_NAMES
	if(u32PalSize==256)
	{
		paletteName256 = GX_TLUT0+((paletteName256+1)%16);
		GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteName = paletteName256;
	}
	else
	{
		paletteName16 = GX_BIGTLUT0+((paletteName16+1)%4);
		GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteName = paletteName16;
	}
#else				   
	GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteName = 0;	
#endif	

	//GXLoadTlut(&GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].obj, GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].paletteName);
}

/*
 =======================================================================================================================
    Aim:    Assign a palette to a texture data, creating a new reference for the texture
 =======================================================================================================================
 */
void GXI_Set_Texture_Palette( GDI_tdst_DisplayData *, ULONG ulTextNum, ULONG IdexTex, ULONG IndexPal )
{	
	GXI_tdst_TextureDesc *l_pstTexDesc = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum];
	GXI_tdst_TextureDesc *l_pstTexRaw  = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex];
	
	if(IndexPal==-1) return; // DJ_TEMP !!?? truecolor...?
	
	l_pstTexDesc->paletteNum = IndexPal;
    l_pstTexDesc->bitmapData = l_pstTexRaw->bitmapData;
    l_pstTexDesc->W			 = l_pstTexRaw->W;
    l_pstTexDesc->H			 = l_pstTexRaw->H;
    l_pstTexDesc->formatCI	 = l_pstTexRaw->formatCI;
    l_pstTexDesc->wrap_s	 = l_pstTexRaw->wrap_s;
    l_pstTexDesc->wrap_t	 = l_pstTexRaw->wrap_t;
//    l_pstTexDesc->bUseMipMap = l_pstTexRaw->bUseMipMap;
	l_pstTexDesc->bUseMipMap = FALSE;
	
	// create gamecube texture
	GXInitTexObjCI(&l_pstTexDesc->obj, 
 			        l_pstTexDesc->bitmapData, 
				    l_pstTexDesc->W, 
				    l_pstTexDesc->H, 
				    l_pstTexDesc->formatCI, 
				    l_pstTexDesc->wrap_s,
				    l_pstTexDesc->wrap_t,
				    l_pstTexDesc->bUseMipMap,
				    GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[IndexPal].paletteName);

	if(l_pstTexDesc->bUseMipMap)
	{
		f32 maxLevel=GXI_GetPowerOf2(max(l_pstTexDesc->W, l_pstTexDesc->H))+1;
		GXInitTexObjLOD(&l_pstTexDesc->obj,
						 GX_LIN_MIP_NEAR,										// min_filter
						 GX_LINEAR,												// max_filter
						 0,														// min_lod
						 maxLevel,												// max_lod
						 0,														// lod_bias
						 GX_FALSE,												// bias_clamp
						 GX_FALSE,												// do_edge_lod
						 GX_ANISO_1);											// max_aniso
	}
	else
	{
		GXInitTexObjLOD(&l_pstTexDesc->obj,
						 GX_LINEAR,												// min_filter
						 GX_LINEAR,												// max_filter
						 0,														// min_lod
						 0,														// max_lod
						 0,														// lod_bias
						 GX_FALSE,												// bias_clamp
						 GX_FALSE,												// do_edge_lod
						 GX_ANISO_1);											// max_aniso
	}
	
#ifdef USE_MY_TEX_REGION
	GXInitTexObjUserData(&l_pstTexDesc->obj, (void*)g_nbTextureLoaded);
	g_nbTextureLoaded++;
#endif
	
}

/*
 =======================================================================================================================
    Aim:    Change the tex data to use the appropriate one at this stage for the animated texture
 =======================================================================================================================
 */
void GXI_SetAnimatedTexture(u32 ulTextureAntIndex , u32 ulTextureToSet)
{
	GXI_tdst_TextureDesc *l_pstTexDesc = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureAntIndex];
	GXI_tdst_TextureDesc *l_pstTexRaw  = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureToSet];

	// Anti-bug ...	
	L_memcpy(l_pstTexDesc,l_pstTexRaw,sizeof(GXI_tdst_TextureDesc));
	
	GXInitTexObjData(&l_pstTexDesc->obj, l_pstTexRaw->bitmapData);
}


/*
 =======================================================================================================================
    Aim:    Check how much texture memory is left (appliable on GameCube???)
 =======================================================================================================================
 */
void GXI_ShowAvailableTextureMemory()
{
}

#ifdef USE_MY_TEX_REGION
/*---------------------------------------------------------------------------*
    Name:           MyTexRegionCallback
    
    Description:    Tex cache allocator using simple round algorithm

    Arguments:      texObj : a pointer to texture object to be loaded
                    mapID  : destination texmap ID (just same as GXLoadTexObj)
    
    Returns:        appropriate tex cache region for loading texture.
 *---------------------------------------------------------------------------*/
static GXTexRegion* MyTexRegionCallback(GXTexObj* texObj, GXTexMapID mapID)
{
#pragma unused(mapID)

    u32  regionNum, texID;
   
    texID     = (u32)GXGetTexObjUserData(texObj);
    regionNum = texID ? ((texID % (MY_TEX_REGION_NUM_REGIONS-1))+1) : 0;

    return &MyTexRegions[regionNum];
}

#endif

/*
 =======================================================================================================================
    Aim:    Init the descriptors for the texture management
 =======================================================================================================================
 */
LONG GXI_l_Texture_Init( GDI_tdst_DisplayData *_pst_DD, ULONG _ul_NbTextures )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    //GXI_tdst_SpecificData	*pst_SD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    if (_ul_NbTextures == 0)
        _ul_NbTextures = 1;

	if (GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)) 
		MEM_TEX_v_FreeAlign(GXI_Global_ACCESS(p_MyTextureDescriptorsAligned));
	GXI_Global_ACCESS(p_MyTextureDescriptorsAligned) = NULL;
	GXI_Global_ACCESS(ulNumberOfTextures) = _ul_NbTextures;
	GXI_Global_ACCESS(p_MyTextureDescriptorsAligned) = (GXI_tdst_TextureDesc *)MEM_TEX_p_AllocAlign(sizeof(GXI_tdst_TextureDesc) * GXI_Global_ACCESS(ulNumberOfTextures) , 32);
	
    L_memset(GXI_Global_ACCESS(p_MyTextureDescriptorsAligned),0,sizeof(GXI_tdst_TextureDesc) * GXI_Global_ACCESS(ulNumberOfTextures));

	if (GXI_Global_ACCESS(ulNumberOfPalettes)) MEM_TEX_v_FreeAlign(GXI_Global_ACCESS(p_MyPalletteDescriptorAligned));
	GXI_Global_ACCESS(ulNumberOfPalettes)			 = 0;
	GXI_Global_ACCESS(p_MyPalletteDescriptorAligned) = NULL;
	
	GXI_Global_ACCESS(dul_TextureDeltaBlend) = (u32 *) MEM_TEX_p_Alloc((_ul_NbTextures+1)*sizeof(u32));
    L_memset(GXI_Global_ACCESS(dul_TextureDeltaBlend), 0, (_ul_NbTextures+1)*sizeof(u32));
    
#ifdef USE_MY_TEX_REGION    
{
	u32 i;
    // init texture regions
    for ( i = 0 ; i < MY_TEX_REGION_NUM_REGIONS ; i++ )
    {
        // The region is used as a 32K cache.
        GXInitTexCacheRegion(
            &MyTexRegions[i],
            GX_FALSE,               	// 32b mipmap
            0x00000 + i * 0x08000,  	// tmem_even
            MY_TEX_REGION_TCACHE_SZ,	// size_even
            0x80000 + i * 0x08000,  	// tmem_odd
            MY_TEX_REGION_TCACHE_SZ ); 	// size_odd
    }

    GXSetTexRegionCallback(MyTexRegionCallback);
}    
#endif

    return 1;
}
