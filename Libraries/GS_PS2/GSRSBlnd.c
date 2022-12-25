
/*$T GSPinit.c GC! 1.081 05/04/00 15:08:05 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>

#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "ENGine/Sources/WORld/WORstruct.h"


#ifdef PSX2_TARGET

#include "Gsp_Bench.h"

#include "ENGvars.h"

/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif
/*static MyRegister *p_Registers; 
static u_int Numregs; */

/*
 =======================================================================================================================
 Aim: Setup blending mode 
 =======================================================================================================================
 */
#define GSP_RS_SetRegister(Register , Value ) \
	{\
		p_Registers->ulRegister = (unsigned long)Register;\
		(p_Registers++)->ulValueToWrite = (unsigned long)Value;\
	}
	
MyRegister *GSP_SetTileBilinearTrilinear_MipMapping(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG Flag);

	
MyRegister *GSP_SetTextureRenderShadow(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG _l_TSDW)
{
	u_long TextureRegister;
	int 	DPtr , TBW , BPPMode;
	
	/* Texture is defined as follow:
		Size X Po2 = (_l_TSDW >> 26) & 0xf;
		Size Y Po2 = (_l_TSDW >> 22) & 0xf;
		Buffer X = (_l_TSDW >> 10) & 0x3ff;
		Buffer Y = (_l_TSDW >> 0) & 0x3ff;  
	//*/
	
	if ((GSP_IsFrameODD2()))
	{ // Select buffer 2 
		DPtr = (unsigned long)(p_Globals->TFBP1);
	} else 
	{ /* Select Buffer 1 */
		DPtr = (unsigned long)(p_Globals->TFBP2);
	}

	BPPMode = 0;
	DPtr *= 2048;
	DPtr >>= 6;
	DPtr += ((_l_TSDW >> 0) & 0x3ff) * (p_Globals -> Xsize >> 6); // Y
#ifdef  GSP_USE_TRIPLE_BUFFERRING
	{
		DPtr = ((_l_TSDW >> 0) & 0x3ff) * (p_Globals -> Xsize >> 7); // Y;
		BPPMode = 2;
	}
#endif
	{	
		ULONG XSize;
		XSize = ((_l_TSDW >> 10) & 0x3ff);
#ifdef  GSP_USE_TRIPLE_BUFFERRING
		XSize *= 32;
#else
		XSize *= 32;
#endif
		DPtr += XSize >> 6; // X
	}

	TBW  = p_Globals -> Xsize >> 6;

	TextureRegister = 
		((u_long)DPtr)|
		(((u_long)TBW)<<14L)| 
		(((u_long)BPPMode)<<20L)|
		((u_long)((_l_TSDW >> 26) & 0xf) << 26L)|
		((u_long)((_l_TSDW >> 22) & 0xf) << 30L)|
		(1L<<34L)|
		(0L<<35L);
	GSP_RS_SetRegister(GSP_TEX0_1 , 	TextureRegister );
	GSP_RS_SetRegister(GSP_TEX2_1 , 			(((u_long)BPPMode)<<20L) );
	TextureRegister = TextureRegister >> 20L;
	TextureRegister &= 0x3f;
	return p_Registers;

}
#if 0
void GSP_SetShadowBufferTarget(ULONG ShadowBufNumplusOne)
{
	u_int Remorf;
	ULONG 					Numregs;
	MyRegister 				*p_Registers , *p_RegistersBase;
	Gsp_BigStruct *p_Globals;
	
	p_Globals = &gs_st_Globals;
	Remorf = GspGlobal_ACCESS(Morfling) & ~31;
	
	p_RegistersBase = p_Registers = Gsp_SetDrawBuffer_SRS( p_Globals );

	if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)
	{
		GSP_RS_SetRegister(GSP_SCISSOR_1 	, ((((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L) - 1L) << 0L) | ((((unsigned long)GspGlobal_ACCESS(Xsize) ) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
		GSP_RS_SetRegister(GSP_SCISSOR_2 	, ((((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L) - 1L) << 0L) | ((((unsigned long)GspGlobal_ACCESS(Xsize) ) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
		
	} else
	{
		GSP_RS_SetRegister(GSP_SCISSOR_1 	, (0L << 0L) | ((((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
		GSP_RS_SetRegister(GSP_SCISSOR_2 	, (0L << 0L) | ((((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
	} 
	Numregs = (ULONG)p_Registers - (ULONG)p_RegistersBase;
	Numregs >>= 4;
	if (Numregs)
	{
		register u_long128 *p_ReturnValue;
		p_ReturnValue = (u_long128 *)p_Globals->p_CurrentBufferPointer;
		((u_int *)p_ReturnValue)[0] = 0; /* VIF NOP */
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0x11000000; /* FLUSHE */
		((u_int *)p_ReturnValue)[3] = 0x50000000 | (Numregs + 1); /* DIRECT */
		p_ReturnValue++;
		Gsp_M_SetGifTag  ((GspGifTag *)p_ReturnValue, Numregs , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 , 0xEL);
		p_ReturnValue++;
		p_ReturnValue+=(Numregs);
		((u_int *)p_ReturnValue)[0] = 0; /* VIF NOP */
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0; 
		((u_int *)p_ReturnValue)[3] = 0x11000000; /* FLUSHE */
		p_ReturnValue++;
		p_Globals->p_CurrentBufferPointer = (u_int)p_ReturnValue;
	}
	
}
#endif
void GSP_SetTextureCache(Gsp_BigStruct *p_Globals , ULONG _l_Texture)
{
#ifdef GSP_USE_TEXTURE_CACHE	
	Gsp_tdst_Texture 	*p_TD;
	if (_l_Texture < p_Globals->ulNumberOfTextures)
	{

		p_TD = p_Globals->p_MyTextureDescriptorsAligned + _l_Texture;
		if (p_TD->ulMotherTextureIndex)
		{
			GSP_LoadCachedTexture(p_TD->ulMotherTextureIndex -1 );
			GSP_LoadCachedPalette(p_TD->ulPaletteIndex - 1 );
			GSP_Set_Texture_Palette( NULL , _l_Texture, p_TD->ulMotherTextureIndex -1, p_TD->ulPaletteIndex - 1);
			p_TD -> InterfaceFlags |= (p_Globals->p_MyTextureDescriptorsAligned + p_TD->ulMotherTextureIndex -1) -> InterfaceFlags & GSP_MustBeFlushed;
		}
		else 
			GSP_LoadCachedTexture(_l_Texture);
	}
#endif
}
MyRegister *GSP_SetTexture12(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG _l_Texture)
{
	Gsp_tdst_Texture 	*p_TD;
	if (_l_Texture >= p_Globals->ulNumberOfTextures)
	{
		/* ShadowTExture detected */
		p_Registers = GSP_SetTextureRenderShadow(p_Globals , p_Registers , _l_Texture );
	}
	else
	{
		p_TD = p_Globals->p_MyTextureDescriptorsAligned + _l_Texture;
		if (p_TD->TEXCLUT) 
		{
			GSP_RS_SetRegister(GSP_TEXCLUT , 	p_TD->TEXCLUT );
		}
//		if (p_TD->InterfaceFlags & GSP_MustBeFlushed)
		{
			GSP_RS_SetRegister(GSP_TEXFLUSH , 	1L );
			//p_TD->InterfaceFlags -= GSP_MustBeFlushed;
		}
		
/*		if (p_TD->TEX1_REG!=0L)
		{
			GSP_RS_SetRegister(GSP_TEX1_1 , 	p_TD->TEX1_REG );
			p_Registers = GSP_SetTileBilinearTrilinear_MipMapping(p_Globals , p_Registers , MAT_Cul_Flag_Bilinear);
		}
		else
		{
			GSP_RS_SetRegister(GSP_TEX1_1 , 	0L);
		}//*/
		GSP_RS_SetRegister(GSP_TEX0_1 , 	p_TD->TEX0_REG );
		//GSP_RS_SetRegister(GSP_TEX1_1 , 	0l);
		p_Globals->ul64LastText0Used = p_TD->TEX0_REG;
		p_TD->InterfaceFlags |= GSP_IsNeeded;
		if (p_TD->InterfaceFlags & GSP_IsChildOfInterfaceTexture)
		{
			((Gsp_tdst_Texture *)p_TD->p_BitmapPtr)->InterfaceFlags |= GSP_IsNeeded;
		}
	}
	return p_Registers;
}
MyRegister *GSP_SetBlending(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG BM , ULONG Flag)
{
	switch(MAT_GET_Blending(BM))
	{
	default:
	case MAT_Cc_Op_Glow:
		GSP_RS_SetRegister(GSP_ALPHA_1 , GST_COPY );
		GSP_RS_SetRegister(GSP_FOGCOL , p_Globals->FogColor);
		break;
	case MAT_Cc_Op_Copy:
		GSP_RS_SetRegister(GSP_ALPHA_1 , GST_COPY );
		GSP_RS_SetRegister(GSP_FOGCOL , p_Globals->FogColor);
		break;
	case MAT_Cc_Op_Alpha:
#ifdef GSP_PS2_BENCH
		if (p_Globals->Status& GSP_Status_Show_Depth)
		{
			GSP_RS_SetRegister(GSP_ALPHA_1 , (0x06L << 32L) | (1L << 6L) | (2L << 4L) | (2L << 2L) | (0L << 0L));
			p_Globals->ulColorOr = 0xffffffff;
		} else
#endif			
		if (Flag & MAT_Cul_Flag_InvertAlpha)
		{
			GSP_RS_SetRegister(GSP_ALPHA_1 , GST_ALPHA );
		}
		else
		{
			GSP_RS_SetRegister(GSP_ALPHA_1 , GST_ALPHA_I );
		}
		GSP_RS_SetRegister(GSP_FOGCOL , p_Globals->FogColor );
		break;
	case MAT_Cc_Op_AlphaPremult:
		GSP_RS_SetRegister(GSP_ALPHA_1 , GST_ALPHAP );
		GSP_RS_SetRegister(GSP_FOGCOL , p_Globals->FogColor );
		break;
	case MAT_Cc_Op_AlphaDest:
		if (Flag & MAT_Cul_Flag_InvertAlpha)
		{
			GSP_RS_SetRegister(GSP_ALPHA_1 , GST_PALPHA );
		}
		else
		{
			GSP_RS_SetRegister(GSP_ALPHA_1 , GST_PALPHA_I );
		}
		GSP_RS_SetRegister(GSP_FOGCOL , p_Globals->FogColor );
		break;
	case MAT_Cc_Op_AlphaDestPremult:
		GSP_RS_SetRegister(GSP_ALPHA_1 , GST_PALPHAP );
		GSP_RS_SetRegister(GSP_FOGCOL , p_Globals->FogColor );
		break;
	case MAT_Cc_Op_Add:
		GSP_RS_SetRegister(GSP_ALPHA_1 , GST_ADD );
		GSP_RS_SetRegister(GSP_FOGCOL , 0 );
		break;
	case MAT_Cc_Op_Sub:
		GSP_RS_SetRegister(GSP_ALPHA_1 , GST_SUBMUL );
		GSP_RS_SetRegister(GSP_FOGCOL , 0 );
		break;
	case MAT_Cc_Op_PSX2ShadowSpecific:
		GSP_RS_SetRegister(GSP_ALPHA_1 , GST_SHADOWMODE );
		GSP_RS_SetRegister(GSP_FOGCOL , 0 );//*/
		break;
	}//*/
	return p_Registers;
}
MyRegister *GSP_SetTileUV(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG Flag)
{
	u_long Clamp_X;
	Clamp_X = (1L<<0L)|(1L<<2L);
	if	(Flag & MAT_Cul_Flag_TileU) Clamp_X ^= (1L<<0L);
	if	(Flag & MAT_Cul_Flag_TileV) Clamp_X ^= (1L<<2L);//*/
	GSP_RS_SetRegister(GSP_CLAMP_1 , Clamp_X );
	return p_Registers;
}
MyRegister *GSP_SetTileBilinearTrilinear(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG Flag)
{
	u_long64 Text1_X;
	Text1_X = (0L<<19L) | (0x000L<<32L); // L & K
	Text1_X |= (0L<<2L); // MAX MIPMAPPING LVLS 
	Text1_X |= (1L<<9L); // AUTOMATIC 
	switch (Flag & (MAT_Cul_Flag_Bilinear|MAT_Cul_Flag_Trilinear)) 
	{
		case 0: 
			Text1_X |= (0L<<5L) | (2L<<6L); // MAX MIPMAPPING LVLS 
			break;
		case MAT_Cul_Flag_Bilinear: 
			Text1_X |= (1L<<5L) | (4L<<6L); // MAX MIPMAPPING LVLS 
			break;
		case MAT_Cul_Flag_Trilinear: 
			Text1_X |= (0L<<5L) | (3L<<6L); // MAX MIPMAPPING LVLS 
			break;
		case MAT_Cul_Flag_Bilinear|MAT_Cul_Flag_Trilinear: 
			Text1_X |= (1L<<5L) | (5L<<6L); // MAX MIPMAPPING LVLS 
			break;
	}//*/
	GSP_RS_SetRegister(GSP_TEX1_1 , Text1_X );
	return p_Registers;
}//*/

MyRegister *GSP_SetTileBilinearTrilinear_MipMapping(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG Flag)
{
	u_long64 Text1_X;
	Text1_X = (0L<<19L) | (0x000L<<32L); // L & K
	Text1_X |= (2L<<2L); // MAX MIPMAPPING LVLS 
	Text1_X |= (1L<<9L); // AUTOMATIC 
	switch (Flag & (MAT_Cul_Flag_Bilinear|MAT_Cul_Flag_Trilinear)) 
	{
		case 0: 
			Text1_X |= (0L<<5L) | (2L<<6L); // MAX MIPMAPPING LVLS 
			break;
		case MAT_Cul_Flag_Bilinear: 
			Text1_X |= (1L<<5L) | (4L<<6L); // MAX MIPMAPPING LVLS 
			break;
		case MAT_Cul_Flag_Trilinear: 
			Text1_X |= (0L<<5L) | (3L<<6L); // MAX MIPMAPPING LVLS 
			break;
		case MAT_Cul_Flag_Bilinear|MAT_Cul_Flag_Trilinear: 
			Text1_X |= (1L<<5L) | (5L<<6L); // MAX MIPMAPPING LVLS 
			break;
	}//*/
	GSP_RS_SetRegister(GSP_TEX1_1 , Text1_X );
	return p_Registers;
}//*/

MyRegister *GSP_SetHideColorAlpha(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG Flag)
{
	p_Globals->CUR_REG_FRAME &= 0x00000000ffffffffL;
	p_Globals->CUR_REG_FRAME |= 0x0100000000000000L;
	if (p_Globals->Status & GSP_Status_AE_WATEREFFECT) p_Globals->CUR_REG_FRAME |= 0x8000000000000000L;
	
	if ((Flag & (MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_HideColor)) == (MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_HideColor))
	{
		/* ShadowRendering is detected */
		u_long CurRegFrame __attribute__((aligned(8)));;
#ifdef GSP_USE_TRIPLE_BUFFERRING			
 		CurRegFrame = /*FBP*/(0L << 0L) | /*FBW*/(((unsigned long)p_Globals->Xsize >> 6L) << 16L) | /*OPP*/(2L << 24L) | /*FBMSK*/(0L << 32L);
		CurRegFrame |= 0x00ffffff00000000L; /* Hide Color */
#else
		if ((GSP_IsFrameODD2()))
		{ // Select buffer 2 
			CurRegFrame = /*FBP*/((unsigned long)p_Globals->FBP1 << 0L) | /*FBW*/(((unsigned long)p_Globals->Xsize>> 6L) << 16L) | /*OPP*/((unsigned long)p_Globals->BppMode << 24L) | /*FBMSK*/(0L << 32L);
		} else 
		{ /* Select Buffer 1 */
			CurRegFrame = /*FBP*/((unsigned long)p_Globals->FBP2 << 0L) | /*FBW*/(((unsigned long)p_Globals->Xsize>> 6L) << 16L) | /*OPP*/((unsigned long)p_Globals->BppMode << 24L) | /*FBMSK*/(0L << 32L);
		}
		CurRegFrame |= 0x00ffffff00000000L; /* Hide Color */
#endif		
		if (p_Globals->Status & GSP_Status_AE_WATEREFFECT) CurRegFrame |= 0x8000000000000000L;
		GSP_RS_SetRegister(GSP_FRAME_1 , CurRegFrame );
	} else
	{
		if (Flag & MAT_Cul_Flag_HideAlpha) p_Globals->CUR_REG_FRAME |= 0xff00000000000000L;
		if (Flag & MAT_Cul_Flag_HideColor) p_Globals->CUR_REG_FRAME |= 0x00ffffff00000000L;
		GSP_RS_SetRegister(GSP_FRAME_1 , p_Globals->CUR_REG_FRAME );
	}
	return p_Registers;
}

MyRegister *GSP_SetAlphaTest(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG BM , ULONG Flag)
{
	u_long 	rTEST;
//	if (p_Globals -> RenderingInterface)
	if ((!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_ZTest)) || (p_Globals -> RenderingInterface))
		rTEST = (1L << 16L) | (1L << 17L); /* ZTest OFF */
	else
		rTEST = (1L << 16L) | (2L << 17L); /* ZTest ON & GREQUAL */
	if(Flag & MAT_Cul_Flag_AlphaTest)
	{
		rTEST |= 1L | (((u_long)MAT_GET_AlphaTresh(BM)) << 3L);
		if(Flag & MAT_Cul_Flag_InvertAlpha)
			rTEST |= 2L << 1L; 	/* LESS */
		else
		{
			if (p_Globals->pCompresseddNormals)	
				rTEST |= 5L << 1L;
			else
				rTEST |= 6L << 1L;	/* GREATER */	
		}
		
	}
	p_Globals->last_GSP_TEST_1 = rTEST;
	GSP_RS_SetRegister(GSP_TEST_1 , rTEST );
	return p_Registers;
}
MyRegister *GSP_SetNZW(Gsp_BigStruct *p_Globals , MyRegister *p_Registers , ULONG Flag)
{
	p_Globals->CUR_REG_ZBUF &= ~(1L << 32L); /* Zwrite on */
	if ((Flag & MAT_Cul_Flag_NoZWrite) || (p_Globals -> RenderingInterface)) p_Globals->CUR_REG_ZBUF |= (1L << 32L); /* Zwrite off */
	GSP_RS_SetRegister(GSP_ZBUF_1 , p_Globals->CUR_REG_ZBUF );
	return p_Registers;
}
extern void Gsp_RenderBuffer(register Gsp_BigStruct	  *p_BIG , register ULONG Num);


void GSP_SetTextureBlending12(Gsp_BigStruct *p_Globals)
{	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u_long 					rPRMODE __attribute__((aligned(16)));
	u_int					Flag;
	u_int					Delta;
	ULONG 					_l_Texture,BM;
	MyRegister 				*p_Registers , *p_RegistersBase;
	ULONG 					Numregs;
	static u32 				ulLasFogEnable = 0;
	u32 				ulFogEnable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Delta = 0;
	
	_l_Texture = p_Globals->ulNewTXT ;
	BM = p_Globals->ulNewBMD ;
	p_Globals->bSRSMustBeUodate = 0;
	
	ulFogEnable = (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (p_Globals->Status & GSP_Status_FogOn);
	
	if ((p_Globals->ulLastBMD == BM) && (p_Globals->ulLastTXT == _l_Texture) && (!p_Globals->bSRS_FromScratch) && (ulFogEnable == ulLasFogEnable)) return;
	
	ulLasFogEnable = ulFogEnable;



	
	Delta = p_Globals->ulLastBMD ^ BM;
//	Delta = -1;

	if ((GDI_gpst_CurDD_SPR.ul_CurrentDrawMask ^ GspGlobal_ACCESS(ulLastDrawMask)) & GDI_Cul_DM_ZTest)
	{
		Delta |= MAT_Cc_AlphaTresh_MASK;
	}


	Flag = MAT_GET_FLAG(BM);

	if (p_Globals->bSRS_FromScratch)
	{
		if (p_Globals->bSRS_FromScratch == 2)
			Gsp_BE_PrepareHidenBuffer();
		p_Globals->bSRS_FromScratch = 0;
		p_Globals->ulLastTXT = _l_Texture | 0x1000;
		Delta = 0xffffffff;
	}
	

	
#ifdef GSP_USE_TEXTURE_CACHE	
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize)) 
		GSP_SetTextureCache(p_Globals , _l_Texture);
#endif	

	p_RegistersBase = p_Registers = Gsp_SetDrawBuffer_SRS( p_Globals );
	
	if (p_Globals->ulLastTXT != _l_Texture)	
		p_Registers = GSP_SetTexture12(p_Globals , p_Registers , _l_Texture );
		
 
 
 
	if (Delta)
	{
		if (Delta & (MAT_Cul_Flag_TileU|MAT_Cul_Flag_TileV)) 
			p_Registers = GSP_SetTileUV(p_Globals , p_Registers , Flag);

		if (Delta & (MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_Trilinear)) 	
			p_Registers = GSP_SetTileBilinearTrilinear(p_Globals , p_Registers , Flag);

		if (Delta & (MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_HideColor)) 
			p_Registers  = GSP_SetHideColorAlpha(p_Globals , p_Registers , Flag);

		if (Delta & (MAT_Cul_Flag_AlphaTest | MAT_Cc_AlphaTresh_MASK | MAT_Cul_Flag_InvertAlpha)) 
			p_Registers = GSP_SetAlphaTest(p_Globals , p_Registers , BM , Flag);

		if (Delta & MAT_Cul_Flag_NoZWrite) 
			p_Registers = GSP_SetNZW(p_Globals , p_Registers , Flag);
		
		if (Delta & (MAT_Cc_Blending_MASK | MAT_Cul_Flag_InvertAlpha)) 
			p_Registers = GSP_SetBlending(p_Globals , p_Registers , BM , Flag);
	}
	
/*	if(Flag & MAT_Cul_Flag_UseLocalAlpha)
		p_Globals->ulColorOr = 0xff000000;	
	else
		p_Globals->ulColorOr = 0;*/
		
	rPRMODE = (0L << 8L) | (1L << 3L) | (0x1L << 6L)/* Use STQ, not UV | Gouraud shading | AlphaBlend */;
	if (_l_Texture != 0xffffffff) 
		rPRMODE |= (1L << 4L); /* Texture mapping On */
		
	//GSP_Fogged();		
	
	if (ulFogEnable)
	{
		//rPRMODE |= 1L<<5L;// FOg on
	}
	//rPRMODE |= 1L<<5L;// FOg on
	if (MAT_GET_Blending(BM) == MAT_Cc_Op_Glow)
	{
		u_long CurRF;
		rPRMODE &= ~(0x1L << 6L); // Disable Alpha
		p_Globals->ulColorOr |= 0xff000000;
		p_Globals->ulOGLSetCol_XOr &= 0x00ffffff;
		p_Globals->ulOGLSetCol_XOr |= 0xFF000000;
		p_Globals->ulOGLSetCol_XOr ^= 0x06000000;
		CurRF = p_Globals->CUR_REG_FRAME & ~0x0100000000000000L;
		GSP_RS_SetRegister(GSP_FRAME_1 , CurRF);
		p_Globals->ul64LastText0Used &= ~(1L << 34L); // TccTo 0;
		GSP_RS_SetRegister(GSP_TEX0_1 , 	p_Globals->ul64LastText0Used);
		p_Globals->bSRS_FromScratch = 1; // Force reset status 
		p_Globals->p_AE_Params->ulG_GlowMode |= 0x80;
	} else
	{
		if (MAT_GET_Blending(BM) == MAT_Cc_Op_Copy)
			rPRMODE &= ~(0x1L << 6L); // Disable Alpha
	}
	
	Numregs = (ULONG)p_Registers - (ULONG)p_RegistersBase;
	Numregs >>= 4;
	if (Numregs || (rPRMODE != p_Globals->Last_rPRMODE))
	{
		register u_long128 *p_ReturnValue;
		Numregs+=2;
		GSP_RS_SetRegister(GSP_PRMODE , rPRMODE);
		GSP_RS_SetRegister(GSP_PRIM , 4L);
		
		p_ReturnValue = (u_long128 *)p_Globals->p_CurrentBufferPointer;
		((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xBE00,0); /* VIF NOP */
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0x11000000; /* FLUSHE */
		((u_int *)p_ReturnValue)[3] = 0x50000000 | (Numregs + 1); /* DIRECT */
		p_ReturnValue++;
		
		Gsp_M_SetGifTag  ((GspGifTag *)p_ReturnValue, Numregs , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 , 0xEL);
		p_ReturnValue++;
		p_ReturnValue+=(Numregs);
		
		((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xBE01,0); /* VIF NOP */; /* VIF NOP */
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0; 
		((u_int *)p_ReturnValue)[3] = 0x11000000; /* FLUSHE */
		p_ReturnValue++;

	
		p_Globals->p_CurrentBufferPointer = (u_int)p_ReturnValue;
	
		p_Globals->NCallsSRS++;
		p_Globals->ulLastTXT = _l_Texture;
		p_Globals->ulLastBMD = BM;
		p_Globals->Last_rPRMODE = rPRMODE;
	}
	GspGlobal_ACCESS(ulLastDrawMask) = GDI_gpst_CurDD_SPR.ul_CurrentDrawMask;
}

void GSP_SetTextureTarget(ULONG _l_Texture,ULONG)
{
	if (_l_Texture != 0xffffffff)
	{
		GspGlobal_ACCESS(ulNextVU1Jump) = 0x14000008; /* Call render shadows */
	}
	else
	{
		GspGlobal_ACCESS(ulNextVU1Jump) = 0x14000000; /* Call 0 */
	}
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif

