#include <eeregs.h>
#include <libgraph.h>
#include <libpad.h>
#include "Gsp.h"

extern u_int volatile VBlankCounter;
extern void StoreZBUFVIF1( u_long128* base_addr, short start_addr, short pixel_mode, int buff_width, short x, short y, short width, short height );
extern u_long GSP_GetPowerOf2( u_int K ) ;
extern u_int ShowNormals;
//u_int ulLastFrameBuffer[32 * 32] __attribute__((aligned(16)));

float GlobalBrightness = 0.0f;
float GlobalContrast = 0.0f;

#define GSP_AE_SetRegister(Register , Value) \
	{((MyRegister *)p_AE_CurrentBufferPointer)->ulRegister = (unsigned long)Register;\
	((MyRegister *)p_AE_CurrentBufferPointer)->ulValueToWrite = (unsigned long)Value;\
	p_AE_CurrentBufferPointer+=16;}

// Special Case for GS_Restriction
#define GSP_AE_SetRegister_TEST_1(Value) \
	{((MyRegister *)p_AE_CurrentBufferPointer)->ulRegister = (unsigned long)GSP_TEST_1;\
	if (!(Value & (1L << 16L))) \
	{\
		((MyRegister *)p_AE_CurrentBufferPointer)->ulValueToWrite = (1L << 16L) | (1L << 17L) | (((unsigned long)Value) & ~(7L << 16L));\
	}\
	else\
	{\
		((MyRegister *)p_AE_CurrentBufferPointer)->ulValueToWrite = (unsigned long)Value;\
	}\
	p_AE_CurrentBufferPointer+=16;}

#define GSP_AE_SetPrim(Primitiv) \
	{*(u_long128 *)p_AE_CurrentBufferPointer = *((u_long128*)&Primitiv);\
	p_AE_CurrentBufferPointer+=16;}
	
#define Gsp_AE_M_SetGifTag(A , B , C , D , E , F , G)\
	{Gsp_M_SetGifTag((GspGifTag *)p_AE_CurrentBufferPointer, A , B , C , D , E , F , G);\
	p_AE_CurrentBufferPointer+=16;}
	
	/*
Gsp_SendToGif
Gsp_SetGSRegisters
*/
static u_int SaveLastCBP;
static u_int p_AE_CurrentBufferPointer;
static u_int IsOn = 0;
static u32	AE_TexturesAreFlushed = 0;
void GSP_AE_DisplayMire(u32 Mode);

void Gsp_AE_BEGIN() 
{
	/* Prepare 2 new free entry's (VIF DIRECT -> GIF TAG)*/
	/* and set the Old DMA TAG End TO Cnt */
	if (!IsOn)
	{
		p_AE_CurrentBufferPointer = SaveLastCBP = GspGlobal_ACCESS(p_CurrentBufferPointer);
		p_AE_CurrentBufferPointer += 16 * 2;
	} 
	IsOn++;
}
void Gsp_AE_SetGSRegisters()
{
	/* Set the VIF DIRECT & Gif Tag */
	u_int Lenght;
	/* gif TAG is ok */
	/* Set the VIF DIRECT */
	Lenght = (p_AE_CurrentBufferPointer - SaveLastCBP) >> 4;
	if (Lenght > 2)
	{
		((u_int *)SaveLastCBP)[0] = 0; /* VIF NOP */
		((u_int *)SaveLastCBP)[1] = SCE_VIF1_SET_MARK(0xAE01,0);;
		((u_int *)SaveLastCBP)[2] = 0x11000000; /* FLUSH*/
		((u_int *)SaveLastCBP)[3] = 0x50000000 | (Lenght - 1); /* DIRECT */
		Gsp_M_SetGifTag((GspGifTag *)&((u_int *)SaveLastCBP)[4], Lenght - 2 , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 ,   0xE);
		SaveLastCBP = p_AE_CurrentBufferPointer;
		p_AE_CurrentBufferPointer += 16 * 2;
	} 
}

void Gsp_AE_END() 
{
	/* correct if Nothing has been drawn */
	IsOn--;
	if (!IsOn)	
	{
		u_int Stoppy;
		GspGlobal_ACCESS(p_CurrentBufferPointer) = p_AE_CurrentBufferPointer - 16 * 2;
		p_AE_CurrentBufferPointer = 0;

		Stoppy = p_AE_CurrentBufferPointer - GspGlobal_ACCESS(p_CurrentBufferPointer);
		if (Stoppy > (RAM_BUF_SIZE - 0x1000))
		{
			Gsp_FIFO_STOP();
		}
	}
}

void Gsp_AE_SendToGif()
{
	u_int Lenght;
	/* gif TAG is ok */
	/* Set the VIF DIRECT */
	Lenght = (p_AE_CurrentBufferPointer - SaveLastCBP) >> 4;
	if (Lenght > 2)
	{
		((u_int *)SaveLastCBP)[0] = 0; /* VIF NOP */
		((u_int *)SaveLastCBP)[1] = 0;
		((u_int *)SaveLastCBP)[2] = 0;
		((u_int *)SaveLastCBP)[3] = 0;
		((u_int *)SaveLastCBP)[4] = 0; /* VIF NOP */
		((u_int *)SaveLastCBP)[5] = SCE_VIF1_SET_MARK(0xAE02,0);
		((u_int *)SaveLastCBP)[6] = 0x11000000; /* FLUSH*/
		((u_int *)SaveLastCBP)[7] = 0x50000000 | (Lenght - 2); /* DIRECT */
		SaveLastCBP = p_AE_CurrentBufferPointer;
		p_AE_CurrentBufferPointer += 16 * 2;
	} 
}
 	
u_int GSP_ulZ_To_ZBuffer(float Z)
{
	return (u_int)(16.0f * GspGlobal_ACCESS(ZFactor) / (Z * GIGSCALE0));
}
u_int GSP_ulOoZ_To_ZBuffer(float OoZ)
{
	return (u_int)(16.0f * GspGlobal_ACCESS(ZFactor) * OoZ / (GIGSCALE0));
}
u_int Get_FBF(u_int Number , u_int AsTexture)
{
	u_int DPtr;
	switch (Number)
	{
		case 0: /* Current FBF */
#ifdef GSP_USE_TRIPLE_BUFFERRING
			DPtr = (unsigned long)(GspGlobal_ACCESS(FBP2));
#else			
			if (!(GSP_IsFrameODD()))
			{ // Select buffer 2 
				DPtr = (unsigned long)(GspGlobal_ACCESS(FBP1));
			} else 
			{ /* Select Buffer 1 */
				DPtr = (unsigned long)(GspGlobal_ACCESS(FBP2));
			}
#endif			
			break;
		case 1: /* Other FBF */
#ifdef GSP_USE_TRIPLE_BUFFERRING
			DPtr = (unsigned long)(GspGlobal_ACCESS(FBP2));
#else			
			if ((GSP_IsFrameODD()))
			{ // Select buffer 2 
				DPtr = (unsigned long)(GspGlobal_ACCESS(FBP1));
			} else 
			{ /* Select Buffer 1 */
				DPtr = (unsigned long)(GspGlobal_ACCESS(FBP2));
			}
#endif			
			break;
		case 2: /* ZBF */
			DPtr = (unsigned long)(GspGlobal_ACCESS(ZBP));
			break;
		case 4: /* TEXTURE */
			if (!AE_TexturesAreFlushed) 
				GSP_FLushAllTextureCache();
			AE_TexturesAreFlushed = 1;
			DPtr = (unsigned long)(GspGlobal_ACCESS(TBP));
			break;
#ifdef GSP_USE_TRIPLE_BUFFERRING
		case 8: /* TripleBuffering destination */
			{
				extern u_int TRIPLE_BIGSWAP_SOFT;
				extern u_int RealFBP2;
				if (TRIPLE_BIGSWAP_SOFT & 1)
					DPtr = RealFBP2;
				else
					DPtr = 0;
			}
			break;
#endif			
	}
	if (AsTexture)
	{
		DPtr *= 2048;
		DPtr >>= 6;
	}
	return DPtr;
}
u32 gae_bBilinearFiltering;
u32 BWSpecial = 0;
u_long LastT;
s32 SPECIAL_GetExpOfUpperPowerOf2(f32 Value)
{
	s32 SACA;
	*(f32 *)&SACA = Value ;
	SACA --;
	SACA >>= 23;
	SACA &= 0xff;
	SACA -= 127;
	return SACA + 1;
}

void Gsp_AE_Set_Hell_Blitter_B(
	u_int SrcBuf , // SrcBuf 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color  , | 0x80000000 -> TextureNum , 
	u_int DstBuf , // DstBuf 0->Current FBF , 1->Other FBF , 2->ZBF , 
	u_int DstMsk ,
	u_int bBilinearFiltering )
{
	u_int TPtr , FPtr , FBW;
	u_int SpecialZBufferFlagSrc , SpecialZBufferFlagDst;
	gae_bBilinearFiltering = bBilinearFiltering;
	if (!(SrcBuf & 0x80000000))
	{
		SpecialZBufferFlagSrc = SrcBuf & 0x30;
		SpecialZBufferFlagDst = DstBuf & 0x30;
		SrcBuf &= ~0x30;
		DstBuf &= ~0x30;
	} else
	{
		SpecialZBufferFlagSrc = 0;
		SpecialZBufferFlagDst = 0;
	}

	/* No Z wirte */
	GspGlobal_ACCESS(CUR_REG_ZBUF) |= (1L << 32L); /* Zwrite off */
	GSP_AE_SetRegister(GSP_ZBUF_1 , GspGlobal_ACCESS(CUR_REG_ZBUF));
	FBW = GspGlobal_ACCESS(Xsize)>> 6;
	FPtr = Get_FBF(DstBuf , 0);
	if (BWSpecial) 
	{
		SpecialZBufferFlagDst = 0;	
	}
	
	GSP_AE_SetRegister(GSP_FRAME_1 , /*FBP*/((u_long)FPtr << 0L) | /*FBW*/(((unsigned long)FBW) << 16L) | /*OPP*/(((unsigned long)((SpecialZBufferFlagDst) | GspGlobal_ACCESS(BppMode))) << 24L) | /*FBMSK*/((u_long)DstMsk << 32L));
	if (SrcBuf & 0x80000000)
	{
	
		GSP_AE_SetRegister(GSP_TEXA 	, (0L<<0L)/*Ta0*/ | (0L<<15L) | (0x7fL<<32L)/*Ta1*/);
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[(SrcBuf & 0x7fffffff) % GspGlobal_ACCESS(ulNumberOfTextures)].TEXCLUT)
		{
			GSP_AE_SetRegister(GSP_TEXCLUT , 	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[(SrcBuf & 0x7fffffff) % GspGlobal_ACCESS(ulNumberOfTextures)].TEXCLUT);
		}
	
		GSP_AE_SetRegister(GSP_TEX0_1 	, GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[(SrcBuf & 0x7fffffff) % GspGlobal_ACCESS(ulNumberOfTextures)].TEX0_REG);
		GSP_AE_SetRegister(GSP_TEX1_1 	, (0L<<0L) | (0L<<2L) | (1L<<5L) | (0L<<6L) | (0L<<9L) | (0L<<19L) | (0x68L<<32L));
		GSP_AE_SetRegister(GSP_CLAMP_1 	, (1L<<0L) | (1L<<2L) | (0L<<4L) | (0L<<14L) | (0L<<24L) | (0L<<34L));
		GSP_AE_SetRegister(GSP_PRMODE 	, (1L<<8L) | (1L<<4L) /* No texture */| (1L<<6L) | (1L<<3L));
	} else
	if (SrcBuf == 3)
	{
		GSP_AE_SetRegister(GSP_PRMODE 	, (1L<<8L) | (0L<<4L) /* No texture */| (1L<<6L) | (1L<<3L));
	}
	else
	{
		u_int TBW64 , TPO2;
		extern u32 BinkVideoWith;
		TPtr = Get_FBF(SrcBuf , 1);
		
		
		if (GspGlobal_ACCESS(Xsize) == 640) 
		{
			TBW64 = GspGlobal_ACCESS(Xsize) >> 6;
			TPO2 = 10;
		}
		else
		{
			TBW64 = GspGlobal_ACCESS(Xsize) >> 6;
		 	TPO2 = 9;

		}
		
		if (BinkVideoWith)
		{
			TBW64 = BinkVideoWith >> 6;
			TPO2 = SPECIAL_GetExpOfUpperPowerOf2((f32)BinkVideoWith);//GSP_GetPowerOf2( BinkVideoWith ) ; 
		}
			
		
		if (SrcBuf == 8) 
		{
			SpecialZBufferFlagSrc = 2;
			TPtr = 0;
		}
		if (BWSpecial) 
		{
			SpecialZBufferFlagSrc = 2;//*/
		}
			
		if (GspGlobal_ACCESS(Ysize) == 255)
		{
			GSP_AE_SetRegister(GSP_TEX0_1 	, ((u_long)TPtr<<0L) | ((u_long)TBW64<<14L) | (((u_long)SpecialZBufferFlagSrc)<<20L) | ((u_long)TPO2<<26L) | (8L<<30L) | (1L<<34L) | (0L<<35L) | (0L<<37L) | (0L<<51L) | (0L<<55L) | (0L<<56L) | (0L<<61L));
		} else
		{
			GSP_AE_SetRegister(GSP_TEX0_1 	, ((u_long)TPtr<<0L) | ((u_long)TBW64<<14L) | (((u_long)SpecialZBufferFlagSrc)<<20L) | ((u_long)TPO2<<26L) | (9L<<30L) | (1L<<34L) | (0L<<35L) | (0L<<37L) | (0L<<51L) | (0L<<55L) | (0L<<56L) | (0L<<61L));
		} 
		if (bBilinearFiltering)
		{
			GSP_AE_SetRegister(GSP_TEX1_1 	, (0L<<0L) | (0L<<2L) | (1L<<5L) | (0L<<6L) | (0L<<9L) | (0L<<19L) | (0x68L<<32L));
		}
		else
		{
			GSP_AE_SetRegister(GSP_TEX1_1 	, (0L<<0L) | (0L<<2L) | (0L<<5L) | (0L<<6L) | (0L<<9L) | (0L<<19L) | (0x68L<<32L));
		}
		GSP_AE_SetRegister(GSP_CLAMP_1 	, (1L<<0L) | (1L<<2L) | (0L<<4L) | (0L<<14L) | (0L<<24L) | (0L<<34L));
		GSP_AE_SetRegister(GSP_PRMODE 	, (1L<<8L) | (1L<<4L) /* No texture */| (1L<<6L) | (1L<<3L));
	}
	/* No Z wirte */
	Gsp_AE_SetGSRegisters();
}
void Gsp_AE_Set_Hell_Blitter(
	u_int SrcBuf , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color  , 4->Texture ram  , | 0x80000000 -> TextureNum , 
	u_int DstBuf , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
	u_int DstMsk )
{
	Gsp_AE_Set_Hell_Blitter_B(SrcBuf , DstBuf , DstMsk , 1 );
}


void GSP_AE_RotateAroundPoint (MyIVectorFormat *v , MyIVectorFormat *vertexC , float SR , float CR )
{
	MyIVectorFormat VT;
	VT.x = v ->x - vertexC->x;
	VT.y = v ->y - vertexC->y;
	v -> x = vertexC->x + (int)((float)VT.x * CR + (float)VT.y * SR);
	v -> y = vertexC->y + (int)((float)VT.y * CR - (float)VT.x * SR);
}
void Gsp_AE_Hell_Blitter_UV1_UV2_ROT_C1_C2(
	int DX , 
	int DY , 
	float U1,
	float V1,
	float U2,
	float V2,
	float Rotation,
	int SmoothU,
	int SmoothV,
	int UReductionFactor , // 0->0 , 1->2 , 2->4 , 3->8 , 4->16...
	int VReductionFactor , // 0->0 , 1->2 , 2->4 , 3->8 , 4->16...
	int Z , 
	int Color ,
	int Color2)
{
	MyIVectorFormat vertex1, vertex2, color;
	MyIVectorFormat vertexUV1,vertexUV2;
	MyIVectorFormat vertex3, vertex4;
	MyIVectorFormat vertexUV3,vertexUV4;
	extern u32 BinkVideoWith;
	extern u32 BinkVideoHeight;
	u_int XSize, YSize;
	XSize = (GspGlobal_ACCESS(Xsize) + 0xf) & 0xfffffff0;
	YSize = (GspGlobal_ACCESS(Ysize) + 0xf) & 0xfffffff0;

	vertex1.x = (DX + GspGlobal_ACCESS(BX0)) << 4L;
	vertex1.y = (DY + GspGlobal_ACCESS(BY0)) << 4L;
	if (UReductionFactor > 0)
		vertex2.x = (DX + (XSize >> UReductionFactor) + GspGlobal_ACCESS(BX0)) << 4L;
	else
		vertex2.x = (DX + (XSize) + GspGlobal_ACCESS(BX0)) << 4L;
	if (VReductionFactor > 0)
		vertex2.y = (DY + (YSize >> VReductionFactor) + GspGlobal_ACCESS(BY0)) << 4L;
	else
		vertex2.y = (DY + (YSize) + GspGlobal_ACCESS(BY0)) << 4L;
	vertex3.w = vertex4.w = vertex1.w = vertex2.w = 0;
	vertex3.z = vertex4.z = vertex1.z = vertex2.z = Z;
	if (SmoothU) SmoothU = 8;
	if (SmoothV) SmoothV = 8;
	SmoothU ^= 8;
	SmoothV ^= 8;
	if (BinkVideoWith)
	{
		extern u32 ps2_ScreenMode;
		u32 LocalWith;
		if (ps2_ScreenMode == 2)
		{
			V1 = 0.12f;
			V2 = 0.88f;
		}
		LocalWith = BinkVideoWith;
		LocalWith &= ~31;
		vertexUV1.x = (int)((float)LocalWith * U1);
		vertexUV1.y = (int)((float)BinkVideoHeight * V1);
		vertexUV2.x = (int)((float)LocalWith * U2);
		vertexUV2.y = (int)((float)BinkVideoHeight * V2) ;
	}
	else
	{
		vertexUV1.x = (int)((float)XSize * U1);
		vertexUV1.y = (int)((float)YSize * V1);
		vertexUV2.x = (int)((float)XSize * U2);
		vertexUV2.y = (int)((float)YSize * V2) ;
	}
	if (UReductionFactor < 0)
	{
		vertexUV1.x >>= -UReductionFactor;
		vertexUV2.x >>= -UReductionFactor;
	}
	if (VReductionFactor < 0)
	{
		vertexUV1.y >>= -UReductionFactor;
		vertexUV2.y >>= -VReductionFactor;
	}
	
	vertexUV1.x <<= 4;
	vertexUV1.y <<= 4;
	vertexUV2.x <<= 4;
	vertexUV2.y <<= 4;
	vertexUV1.x += SmoothU;
	vertexUV1.y += SmoothV;
	vertexUV2.x += SmoothU;
	vertexUV2.y += SmoothV;
	
	color.x = (Color >> 0)  & 0xff;
	color.y = (Color >> 8)  & 0xff;
	color.z = (Color >> 16) & 0xff;
	color.w = Color >> 24;
	if ((Rotation != 0.0f) || (Color2 != Color))
	{
		float SR,CR;
		MyIVectorFormat vertexC;
		MyIVectorFormat color2;
		vertex3.x = vertex1.x;
		vertex3.y = vertex2.y;
		vertexUV3.x = vertexUV1.x;
		vertexUV3.y = vertexUV2.y;
		vertex4.x = vertex2.x;
		vertex4.y = vertex1.y;
		vertexUV4.x = vertexUV2.x;
		vertexUV4.y = vertexUV1.y;
		color2.x = (Color2 >> 0)  & 0xff;
		color2.y = (Color2 >> 8)  & 0xff;
		color2.z = (Color2 >> 16) & 0xff;
		color2.w = Color2 >> 24;
		
		vertexC.x = (vertex1.x + vertex2.x)>>1;
		vertexC.y = (vertex1.y + vertex2.y)>>1;
		SR = sin(Rotation);
		CR = cos(Rotation);
		GSP_AE_RotateAroundPoint (&vertex1 , &vertexC , SR , CR );
		GSP_AE_RotateAroundPoint (&vertex2 , &vertexC , SR , CR );
		GSP_AE_RotateAroundPoint (&vertex3 , &vertexC , SR , CR );
		GSP_AE_RotateAroundPoint (&vertex4 , &vertexC , SR , CR );
		
		Gsp_AE_M_SetGifTag( 1 , 1 , 1 , 4 , 0 , 12 , 0x531531531531);
		GSP_AE_SetPrim(color);
		GSP_AE_SetPrim(vertexUV1);
		GSP_AE_SetPrim(vertex1);
		GSP_AE_SetPrim(color2);
		GSP_AE_SetPrim(vertexUV3);
		GSP_AE_SetPrim(vertex3);
		GSP_AE_SetPrim(color);
		GSP_AE_SetPrim(vertexUV4);
		GSP_AE_SetPrim(vertex4);
		GSP_AE_SetPrim(color2);
		GSP_AE_SetPrim(vertexUV2);
		GSP_AE_SetPrim(vertex2);
	} else
	{
		Gsp_AE_M_SetGifTag( 1 , 1 , 1 , 6 , 0 , 5 , 0x53531);
		GSP_AE_SetPrim(color);
		GSP_AE_SetPrim(vertexUV1);
		GSP_AE_SetPrim(vertex1);
		GSP_AE_SetPrim(vertexUV2);
		GSP_AE_SetPrim(vertex2);
	}
	Gsp_AE_SendToGif();
}

void Gsp_AE_Hell_Blitter(
	int DX , 
	int DY , 
	int SmoothU,
	int SmoothV,
	int UReductionFactor , // 0->0 , 1->2 , 2->4 , 3->8 , 4->16...
	int VReductionFactor , // 0->0 , 1->2 , 2->4 , 3->8 , 4->16...
	int Z , 
	int Color)
{
	 Gsp_AE_Hell_Blitter_UV1_UV2_ROT_C1_C2(DX , DY , 0.0f,0.0f,1.0f,1.0f,0.0f,SmoothU,SmoothV,UReductionFactor ,VReductionFactor , Z , Color , Color);
}
typedef struct Gsp_Rect_
{
	int DX ;
	int DY ;
	int SX ; 
	int SY ;
	
} Gsp_Rect;


u32 Gsp_AE_ClipRect(Gsp_Rect *pSRC,Gsp_Rect *pDST,Gsp_Rect *pScreenSRC)
{
	float Coef;
	if (pSRC->SX + pSRC->DX < pScreenSRC->DX) return 1; // Clip
	if (pSRC->DX > pScreenSRC->SX + pScreenSRC->DX) return 1; // Clip
	if (pSRC->SY + pSRC->DY < pScreenSRC->DY) return 1; // Clip
	if (pSRC->DY > pScreenSRC->SY + pScreenSRC->DY) return 1; // Clip
	if (pSRC->DX < pScreenSRC->DX)
	{
		Coef = (pSRC->DX + pSRC->SX - pScreenSRC->DX) / (float)(pSRC->SX);
		pDST->DX += pDST->SX * (1.0f - Coef);
		pDST->SX *= Coef;
		pSRC->DX = pScreenSRC->DX;
		pSRC->SX *= Coef;

	}
	if (pSRC->DX + pSRC->SX > pScreenSRC->DX + pScreenSRC->SX)
	{
		Coef = (pScreenSRC->DX + pScreenSRC->SX - pSRC->DX) / (float)(pSRC->SX);
		pDST->SX *= Coef;

		pSRC->SX = pScreenSRC->DX + pScreenSRC->SX - pSRC->DX;
	}
	if (pSRC->DY < pScreenSRC->DY)
	{
		Coef = (pSRC->DY + pSRC->SY - pScreenSRC->DY) / (float)(pSRC->SY);
		pDST->DY += pDST->SY * (1.0f - Coef);
		pDST->SY *= Coef;
		pSRC->DY = pScreenSRC->DY;
		pSRC->SY *= Coef;

	}
	if (pSRC->DY + pSRC->SY > pScreenSRC->DY + pScreenSRC->SY)
	{
		Coef = (pScreenSRC->DY + pScreenSRC->SY - pSRC->DY) / (float)(pSRC->SY);
		pDST->SY *= Coef;

		pSRC->SY = pScreenSRC->DY + pScreenSRC->SY - pSRC->DY;
	}
	return 0;
}


void Gsp_AE_ShiftRect(Gsp_Rect *p)
{
	p->DX <<= 4;
	p->DY <<= 4;
	p->SX <<= 4;
	p->SY <<= 4;
}
void Gsp_AE_DownShiftRect(Gsp_Rect *p)
{
	p->DX >>= 4;
	p->DY >>= 4;
	p->SX >>= 4;
	p->SY >>= 4;
}



void Gsp_AE_BarreBlitter_FRAC(
	Gsp_Rect *p_DST,
	Gsp_Rect *p_SRC,
	int Color1,int Color2,int Color3,int Color4)
{
	MyIVectorFormat vertex1, vertex2, vertex3, vertex4, colov1,colov2, colov3,colov4;
	vertex1.x = (GspGlobal_ACCESS(BX0) - 0) << 4;
	vertex1.y = (GspGlobal_ACCESS(BY0) - 0) << 4;
	vertex1.w = vertex1.z = 0;
	vertex4 = vertex3 = vertex2 = vertex1;
	
	vertex1.x += p_DST -> DX;
	vertex1.y += p_DST -> DY;
	
	vertex2.x += p_DST -> DX;
	vertex2.y += (p_DST -> DY + p_DST -> SY);

	vertex3.x += (p_DST -> DX + p_DST -> SX);
	vertex3.y += p_DST -> DY;
	
	vertex4.x += (p_DST -> DX + p_DST -> SX);
	vertex4.y += (p_DST -> DY + p_DST -> SY);
	
	
	colov1.x = (Color1 >> 0)  & 0xff;
	colov1.y = (Color1 >> 8)  & 0xff;
	colov1.z = (Color1 >> 16) & 0xff;
	colov1.w = Color1 >> 24;
	
	colov2.x = (Color2 >> 0)  & 0xff;
	colov2.y = (Color2 >> 8)  & 0xff;
	colov2.z = (Color2 >> 16) & 0xff;
	colov2.w = Color2 >> 24;
	
	colov3.x = (Color3 >> 0)  & 0xff;
	colov3.y = (Color3 >> 8)  & 0xff;
	colov3.z = (Color3 >> 16) & 0xff;
	colov3.w = Color3 >> 24;
	
	colov4.x = (Color4 >> 0)  & 0xff;
	colov4.y = (Color4 >> 8)  & 0xff;
	colov4.z = (Color4 >> 16) & 0xff;
	colov4.w = Color4 >> 24;
	
	
//	Gsp_AE_M_SetGifTag( 1 , 1 , 1 , 6 , 0 , 6 , 0x531531);
	Gsp_AE_M_SetGifTag( 1 , 1 , 1 , 4 , 0 , 8 , 0x51515151);
	
	GSP_AE_SetPrim(colov1);
	GSP_AE_SetPrim(vertex1);
	GSP_AE_SetPrim(colov2);
	GSP_AE_SetPrim(vertex2);
	GSP_AE_SetPrim(colov3);
	GSP_AE_SetPrim(vertex3);
	GSP_AE_SetPrim(colov4);
	GSP_AE_SetPrim(vertex4);
	Gsp_AE_SendToGif();
}


u32 ZUSedForCase = 0;
void Gsp_AE_CaseBlitter_FRAC(
	Gsp_Rect *p_DST,
	Gsp_Rect *p_SRC,
	int Color)
{
	MyIVectorFormat vertex1, vertex2, color;
	MyIVectorFormat vertexUV1,vertexUV2;
	if (gae_bBilinearFiltering)
	{
		vertex1.x = vertex2.x = (GspGlobal_ACCESS(BX0) - 0) << 4;
		vertex1.y = vertex2.y = (GspGlobal_ACCESS(BY0) - 0) << 4;
	} else
	{
		if (GspGlobal_ACCESS(Xsize) != 512)
			vertex1.x = vertex2.x = (GspGlobal_ACCESS(BX0) - 1) << 4;
		else
			vertex1.x = vertex2.x = (GspGlobal_ACCESS(BX0) - 0) << 4;
		vertex1.y = vertex2.y = (GspGlobal_ACCESS(BY0) - 1) << 4;
	} 
	if (BWSpecial)
	{
		vertex1.x = vertex2.x = (GspGlobal_ACCESS(BX0) - 0) << 4;
		vertex1.y = vertex2.y = (GspGlobal_ACCESS(BY0) - 0) << 4;
	}
	
	vertex1.x += p_DST -> DX;
	vertex1.y += p_DST -> DY;
	vertex2.x += (p_DST -> DX + p_DST -> SX);
	vertex2.y += (p_DST -> DY + p_DST -> SY);
	
	vertex1.w = vertex2.w = 0;
	vertex1.z = vertex2.z = ZUSedForCase;
	

	
	

	vertexUV1.x = p_SRC -> DX;
	vertexUV1.y = p_SRC -> DY;
	vertexUV2.x = (p_SRC -> DX + p_SRC -> SX);
	vertexUV2.y = (p_SRC -> DY + p_SRC -> SY);
	if (!BWSpecial)
	{
		
		if (gae_bBilinearFiltering)
		{
			if (p_DST -> SX > p_SRC -> SX)
			{
				vertexUV1.x += 8;
				vertexUV2.x += 8;
			}

			if (p_DST -> SY > p_SRC -> SY)
			{
				vertexUV1.y += 8;
				vertexUV2.y += 8;
			}//*/
		} else 
		if (GspGlobal_ACCESS(Xsize) == 512)
		{
				vertexUV1.x += 12;
				vertexUV2.x += 12;
		}//*/
	}

	color.x = (Color >> 0)  & 0xff;
	color.y = (Color >> 8)  & 0xff;
	color.z = (Color >> 16) & 0xff;
	color.w = Color >> 24;
	
	
	Gsp_AE_M_SetGifTag( 1 , 1 , 1 , 6 , 0 , 5 , 0x53531);
	GSP_AE_SetPrim(color);
	GSP_AE_SetPrim(vertexUV1);
	GSP_AE_SetPrim(vertex1);
	GSP_AE_SetPrim(vertexUV2);
	GSP_AE_SetPrim(vertex2);
	Gsp_AE_SendToGif();
}

void Gsp_AE_CaseBlitter(
	Gsp_Rect *p_DST,
	Gsp_Rect *p_SRC,
	int Color)
{
	Gsp_AE_ShiftRect(p_DST);
	if (p_DST != p_SRC)
		Gsp_AE_ShiftRect(p_SRC);
	Gsp_AE_CaseBlitter_FRAC(p_DST , p_SRC , Color);
	Gsp_AE_DownShiftRect(p_DST);
	if (p_DST != p_SRC)
		Gsp_AE_DownShiftRect(p_SRC);
}
	
void Gsp_AE_BarreBlitter(
	Gsp_Rect *p_DST,
	Gsp_Rect *p_SRC,
	int Color1,int Color2,int Color3,int Color4)
{
	Gsp_AE_ShiftRect(p_DST);
	if (p_DST != p_SRC)
		Gsp_AE_ShiftRect(p_SRC);
	Gsp_AE_BarreBlitter_FRAC(p_DST , p_SRC , Color1, Color2, Color3, Color4);
	Gsp_AE_DownShiftRect(p_DST);
	if (p_DST != p_SRC)
		Gsp_AE_DownShiftRect(p_SRC);
}
	
void Gsp_AE_EraseAlphaBuffer(u_int Value , u_int Z)
{
	Gsp_AE_BEGIN();
	GSP_AE_SetRegister_TEST_1( (1L << 16L) | (2L << 17L));
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter(3 , 0 , 0xffffff );
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,Z ,Value << 24); 
	Gsp_AE_END();
}



void Gsp_AE_ZSetAlphaBuffer(float ZBegin  , float  Zend , u_int Counter)
{
	u_int color;
	float Ainc , Zinc , CurrentZ , CurrentA;
	Gsp_AE_BEGIN();
	/* normal Ztest */
	Gsp_AE_EraseAlphaBuffer(128 , 0xffffffff);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter(3 , 0 , 0xffffff );
	color = 128;

	CurrentZ = (float)ZBegin;
	Zinc = ((float)Zend - (float)ZBegin) / (float)Counter;
	Ainc = (64.0f) / (float)Counter;
	CurrentA = 64.0f;
	Counter ++;
	while (Counter --)
	{
		Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,GSP_ulZ_To_ZBuffer(CurrentZ) , color << 24);
		CurrentA -= Ainc;
		CurrentZ += Zinc;
		color = (u_int)CurrentA;
	}
	Gsp_AE_END();
}
u_int Gsp_AE_GetZBufferTexturePtr(u_int Divider)
{
	u_int ZPtr , DX;
	ZPtr = GspGlobal_ACCESS(ZBP);
	ZPtr *= 2048L;
	ZPtr >>= 6L;
	if (Divider > 1) DX = GspGlobal_ACCESS(Xsize) >> 1;
	if (Divider > 2) DX += GspGlobal_ACCESS(Xsize) >> 2;
	if (Divider > 3) DX += GspGlobal_ACCESS(Xsize) >> 3;
	if (Divider > 4) DX += GspGlobal_ACCESS(Xsize) >> 4;
	ZPtr += (DX * 32) >> 6;
	return ZPtr;
}

void Gsp_AE_DepthBlurNear(float ZFoc , float fFactor)
{
	u_int Counter;
	u_int Number ;
/*
#define DB_FAR	0x00ffffff
#define DB_NEAR	0x000fffff
*/
	Gsp_AE_BEGIN();

	Number = 3;
#define DBN_COLOR 0x80808080
	Gsp_AE_EraseAlphaBuffer(0, 0xffffffff);
	Gsp_AE_EraseAlphaBuffer(128, GSP_ulZ_To_ZBuffer(ZFoc));
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter(0 , 2 , 0 ); // Hide COLOR
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff,DBN_COLOR); 
	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( (1L << 0L) | (4L << 1L) | ((u_long)128L << 4L) | (1L << 14L) | (0L << 15L) | (0L << 16L) | (2L << 17L));
	Gsp_AE_Set_Hell_Blitter(2 , 0 , 0xff000000 ); // Hide ALPHA
#define DBN_Erosion 5
	Gsp_AE_Hell_Blitter( -DBN_Erosion,0,0,0,0,0,0xffffffff,DBN_COLOR); 
	Gsp_AE_Hell_Blitter( +DBN_Erosion,0,0,0,0,0,0xffffffff,DBN_COLOR); 
	Gsp_AE_Hell_Blitter( 0,-DBN_Erosion,0,0,0,0,0xffffffff,DBN_COLOR); 
	Gsp_AE_Hell_Blitter( 0,+DBN_Erosion,0,0,0,0,0xffffffff,DBN_COLOR); 
	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| (0x40L << 32L));
	GSP_AE_SetRegister_TEST_1( (1L << 0L) | (4L << 1L) | ((u_long)0L << 4L) | (1L << 16L) | (2L << 17L));
	Gsp_AE_Set_Hell_Blitter(0 , 0 , 0xFF000000 ); // Hide Alpha
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff,DBN_COLOR); 

	for (Counter = 0 ; Counter < Number ; Counter++)
	{	
		int BluCounter;
		/* normal Ztest */
		/* Blurring */
		for (BluCounter = 0 ; BluCounter < 2 ; BluCounter++)
		{
			int DX,DY , C;
			DX = DY = 0;
			if (BluCounter == 0) DX = 1;
			else DY = 1;
			C = (Number - Counter);
			if (C & 2)
			{ 	// DX
				DX = -DX;
			} 
			if (C & 1)
			{ 	// DY
				DY = -DY;
			} //*/
			if (C > 6) C = 6;
			DX *= C;
			DY *= C;//*/
			Gsp_AE_Hell_Blitter( DX,DY,1,1,0,0,0xffffffff,0x80808080); 
		}
	}
	GSP_AE_SetRegister_TEST_1( ((1L << 16L) | (2L << 17L)));
	Gsp_AE_SetGSRegisters();
	
	Gsp_AE_END();
	
}

// GIGSCALE0
void Gsp_AE_DepthBlur(float ZStart ,float ZEnd)
{
	float Ainc , Zinc , CurrentZ , CurrentA;
	u_int Counter;
	u_int Number;
/*
#define DB_FAR	0x00ffffff
#define DB_NEAR	0x000fffff
*/
	Gsp_AE_BEGIN();

	Number = 4;
	
	GSP_AE_SetRegister_TEST_1( (1L << 16L) | (2L << 17L));
	Gsp_AE_ZSetAlphaBuffer(ZStart , ZEnd , Number);

	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| (0x40L << 32L));
	Gsp_AE_Set_Hell_Blitter(0 , 0 , 0xff000000 ); // Hide Alpha

	CurrentZ = (float)ZEnd;
	Zinc = ((float)ZEnd - (float)ZStart) / (float)Number;
	Ainc = (64.0f) / (float)Number;
	CurrentA = 0.0f;

	for (Counter = 0 ; Counter < Number ; Counter++)
	{	
		unsigned char ATresh;
		int BluCounter;
		/* normal Ztest */
		CurrentA += Ainc;
		CurrentZ -= Zinc;
		ATresh = (unsigned char)CurrentA;
		GSP_AE_SetRegister_TEST_1( (1L << 0L) | (2L << 1L) | ((u_long)ATresh << 4L) | (1L << 16L) | (2L << 17L));
		Gsp_AE_SetGSRegisters();//*/
		/* Blurring */
		for (BluCounter = 0 ; BluCounter < 2 ; BluCounter++)
		{
			int DX,DY , C;
			DX = DY = 0;
			if (BluCounter == 0) DX = 1;
			else DY = 1;
			C = (Number - Counter);
			if (C & 2)
			{ 	// DX
				DX = -DX;
			} 
			if (C & 1)
			{ 	// DY
				DY = -DY;
			} //*/
			if (C > 6) C = 6;
			DX *= C;
			DY *= C;//*/
			Gsp_AE_Hell_Blitter( DX,DY,1,1,0,0,GSP_ulZ_To_ZBuffer(CurrentZ + Zinc * (float)BluCounter * 0.5f),0x80808080); 
		}
	}
	GSP_AE_SetRegister_TEST_1( ((1L << 16L) | (2L << 17L)));
	Gsp_AE_SetGSRegisters();
	
	Gsp_AE_END();
	
}
float BIG_gf_DispBinProgressSave = 0.0f;
void Gsp_LoadingQuad()
{
	Gsp_Rect stDST,stSRC;
	u32 Color;
	static u32 DrawBarreForeced = 0;
	extern volatile float BIG_gf_DispBinProgress ;
	extern u32 ps2INO_IsJOYTouched(u32 NoMushrooms);
	
	if (ps2INO_IsJOYTouched(2)) DrawBarreForeced = 1;
	
	if (BIG_gf_DispBinProgressSave == 0.0f)
		DrawBarreForeced = 0;
	
	BIG_gf_DispBinProgressSave = fMax(BIG_gf_DispBinProgress,BIG_gf_DispBinProgressSave);
	if ((BIG_gf_DispBinProgressSave>0) && DrawBarreForeced)
	{
		extern u32 BINK_MTH_ReadyToFinish;
		u32 Dlt;
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GSP_AE_SetRegister_TEST_1(	0L);
		Gsp_AE_Set_Hell_Blitter(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
								8 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
								0 );
								
		Dlt = 2;					
		stDST.DX = (GspGlobal_ACCESS(Xsize)>>3) - 45;
		stDST.DY = GspGlobal_ACCESS(Ysize) - 64;
		stDST.SX = 45 * 2;
		stDST.SY = 22;
		stSRC = stDST;
		Gsp_AE_CaseBlitter(&stDST,&stSRC,0);
		stDST.DX += Dlt;
		stDST.DY += Dlt;
		stDST.SX -= Dlt + Dlt;
		stDST.SY -= Dlt + Dlt;
		stSRC = stDST;
		Gsp_AE_CaseBlitter(&stDST,&stSRC,0x808080);
		stDST.DX += Dlt;
		stDST.DY += Dlt;
		stDST.SX -= Dlt + Dlt;
		stDST.SY -= Dlt + Dlt;
		stSRC = stDST;
		Gsp_AE_CaseBlitter(&stDST,&stSRC,0);
		stDST.DX += Dlt;
		stDST.DY += Dlt;
		stDST.SX -= Dlt + Dlt;
		stDST.SY -= Dlt + Dlt;
		stDST.SX *= BIG_gf_DispBinProgressSave;
		stSRC = stDST;
		Color = ((VBlankCounter & 31))<<3;
		if (VBlankCounter & 32)
			Color ^= 0xff;
			
		Color |= Color<<8;
		Color |= Color<<16;
		Color &= 0xfefefe;
		Color >>= 1;
		Color += 0x303030;
		if (BINK_MTH_ReadyToFinish)
		{
			Color &= 0xff00;
		}
		Gsp_AE_CaseBlitter(&stDST,&stSRC,Color);
	}
}

#ifdef GSP_PS2_BENCH
void Gsp_DrawColorCostQuad(OBJ_tdst_GameObject *p_Go2D , u32 Color)
{
	Gsp_Rect stDST,stSRC;
	MATH_tdst_Vector	ScreenC; 
	s32 Cx,Cy;

	ScreenC = p_Go2D->pst_GlobalMatrix->T;
	MATH_TransformVertex(&ScreenC, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &ScreenC);
	if (ScreenC . z > 1.0f)
	{
		ScreenC . x = 0.5f + 0.5f * (GspGlobal_ACCESS(fCurrentFocale) * ScreenC . x) / ScreenC . z;
		ScreenC . y = 0.5f + 0.5f * (GspGlobal_ACCESS(YCorrectionCoef) * GspGlobal_ACCESS(fCurrentFocale) * ScreenC . y) / ScreenC . z;
		Cx = (int)(GspGlobal_ACCESS(Xsize) * ScreenC . x);
		Cy = (int)(GspGlobal_ACCESS(Ysize) * ScreenC . y);
		if (Cx < -10) return;
		if (Cx > GspGlobal_ACCESS(Xsize) + 10) return;
		if (Cy < -10) return;
		if (Cy > GspGlobal_ACCESS(Ysize) + 10) return;
		
		Gsp_AE_BEGIN();
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GspGlobal_ACCESS(CUR_REG_ZBUF) &= ~(1L << 32L); /* Zwrite off */
		GSP_AE_SetRegister_TEST_1(	0L);
		Gsp_AE_Set_Hell_Blitter(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
								0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
								0 );
		stDST.DX = Cx - 10;
		stDST.DY = Cy - 10;
		stDST.SX = 20;
		stDST.SY = 20;
		Gsp_AE_CaseBlitter(&stDST,&stSRC,Color);
		Gsp_AE_END();
		
		{
			char Str_NumberOfTriangles[50];
			sprintf(Str_NumberOfTriangles, "%d", p_Go2D->NumberOfTris);
			GSP_DrawTExt(Cx - ((L_strlen(Str_NumberOfTriangles) * 8)>>1),Cy + 10 ,Str_NumberOfTriangles,0xffffff,0x505050,GST_COPY,GST_SUB);
		}

		
	}
}
#endif
void Gsp_ClearStart()
{
	Gsp_AE_BEGIN();
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1(	0L);
	Gsp_AE_SetGSRegisters();
	{
		u_int Stoppy;
		Stoppy = p_AE_CurrentBufferPointer - GspGlobal_ACCESS(p_CurrentBufferPointer);
		if (Stoppy > (RAM_BUF_SIZE - 0x1000))
		{
			Gsp_AE_END();
			Gsp_FIFO_STOP();
		}
		else
			Gsp_AE_END();
		
	}
	
}
void Gsp_ClearBorderExtend()
{
	u_int Remorf;
	Gsp_Rect stDST,stSRC;
	extern u32 ps2_ScreenMode;
	
	if (ps2_ScreenMode != 0) return;
	
	Remorf = (float)((1.0f - (9.0f/16.0f) / (3.0f/4.0f)) * (float)GspGlobal_ACCESS(Ysize));
	Remorf >>= 1;

	
	Gsp_AE_BEGIN();
	
	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1(	0L);
	GspGlobal_ACCESS(CUR_REG_ZBUF) |= (1L << 32L); /* Zwrite off */
	Gsp_AE_Set_Hell_Blitter(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0 );
	stDST.DX = 0;
	stDST.DY = 0;
	stDST.SX = GspGlobal_ACCESS(Xsize);
	stDST.SY = Remorf;
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0);
	stDST.DX = 0;
	stDST.DY = GspGlobal_ACCESS(Ysize) - Remorf;
	stDST.SX = GspGlobal_ACCESS(Xsize);
	stDST.SY = Remorf;
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0);
	
	Gsp_AE_END();
	
}
void Gsp_SpecialRaster()
{
	u_int Remorf;
	Gsp_Rect stDST,stSRC;
	extern float TheSpecialRasterFuck;
#ifdef 	GSP_USE_TRIPLE_BUFFERRING
	extern u_int volatile GSPDMASEMAPHORE;
#else
#define GSPDMASEMAPHORE 1
#endif


	Remorf = GspGlobal_ACCESS(Morfling);	
	
	if (Remorf < 32) Remorf = 32;
	
	Gsp_AE_BEGIN();
	
	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1(0L);
	Gsp_AE_Set_Hell_Blitter(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0xff000000 );

	stDST.DX = 0;
	stDST.DY = GspGlobal_ACCESS(Ysize) - Remorf ;
	stDST.SX = (u_int)((float)GspGlobal_ACCESS(Xsize) * TheSpecialRasterFuck);
	stDST.SY = Remorf;
	if (GSPDMASEMAPHORE)
		Gsp_AE_CaseBlitter(&stDST,&stSRC,0xff0000);
	else
		Gsp_AE_CaseBlitter(&stDST,&stSRC,0xff);
	stDST.DX = GspGlobal_ACCESS(Xsize) >> 2;
	stDST.DY = GspGlobal_ACCESS(Ysize) - Remorf;
	stDST.SX = 6;
	stDST.SY = Remorf;
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0xffffff);
	stDST.DX += GspGlobal_ACCESS(Xsize) >> 2;
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0xffffff);
	stDST.DX += GspGlobal_ACCESS(Xsize) >> 2;
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0xffffff);
	
	Gsp_AE_END();
	
}




void Gsp_AE_MotionBlur(u_int CurZ)
{
	Gsp_AE_BEGIN();

CurZ >>= 1; 
	if (CurZ > 0x80) CurZ = 0x80;
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| ((u_long)CurZ << 32L));
	Gsp_AE_Set_Hell_Blitter(1 , 0 , 0 );
	Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , 0x80808080); //*/
	
	Gsp_AE_END();
	
}

void Gsp_AE_Blur(u_int Number , u_int Type)
{
	int Counter;
	
	Gsp_AE_BEGIN();
	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| (0x40L << 32L));
	Gsp_AE_Set_Hell_Blitter(0 , 0 , 0 );
	
	for (Counter = 0 ; Counter < Number ; Counter++)
	{	
		int DX,DY;
		DX = DY = 0;
		switch (Counter & 3)
		{
			case 0:DX = 1;break;
			case 1:DY = 1;break;
			case 2:DX = -1;break;
			case 3:DY = -1;break;
		}
		DX *= (Counter >> 2) + 1;
		DY *= (Counter >> 2) + 1;
		Gsp_AE_Hell_Blitter(DX ,DY ,0,0,0,0,0xffffffff , 0x80808080);
	}
	
	Gsp_AE_END();
	
}

void Gsp_AE_Blur2(float Radius,u32 SubUse,u32 AsGlow)
{
	int Counter,SubBuf;
	Gsp_Rect stDST,stSRC;
	Gsp_Rect stSRC_CLIP;
	Gsp_Rect stDST_C,stSRC_C;
	
	SubBuf = 2;
	if (AsGlow) SubBuf = 4;
	
	Radius *= 0.02f;
	
	if (!SubUse)
	{
		if (Radius < 0.0010f) return;
		Gsp_AE_BEGIN();
	}
	Counter = 0;

	stSRC.DX = 0;
	stSRC.DY = 0;
	stSRC.SX = (u_int)GspGlobal_ACCESS(Xsize);
	stSRC.SY = (u_int)GspGlobal_ACCESS(Ysize);
	Gsp_AE_ShiftRect(&stSRC);
	stDST = stSRC;
	
	GSP_AE_SetRegister_TEST_1( 0L);
	if (AsGlow)
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY_By_AlphaSRC);
	}
	else
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	}
	
	/* Copy to ZBUF */
	Gsp_AE_Set_Hell_Blitter_B(0 , SubBuf , 0 , 0);
	stDST.SX>>=1;
	stDST.SY>>=1;
	Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC,0x80808080);
	
	Gsp_AE_Set_Hell_Blitter_B(SubBuf , SubBuf , 0 , 1);
 
	while (	Radius > 0.0015f)
	{
		int Color;
		float FactorL;
		Counter++;
		
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_SetGSRegisters();
		Gsp_AE_Set_Hell_Blitter_B(SubBuf , SubBuf , 0 , 0);
		stSRC.DX = 0;
		stSRC.DY = 0;
		stSRC.SX = (u_int)GspGlobal_ACCESS(Xsize)>>1;
		stSRC.SY = (u_int)GspGlobal_ACCESS(Ysize)>>1;
		Gsp_AE_ShiftRect(&stSRC);
		stSRC_CLIP = stSRC;
		stDST = stSRC;
		stDST.DX = ((u_int)GspGlobal_ACCESS(Xsize)>>1)<<4;
		stDST.SX += 16;
		stDST.SY += 16;
		Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC,0x80808080);
		stDST.SX -= 16;
		stDST.SY -= 16;
		Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC,0x80808080);
		stDST = stSRC;
		Gsp_AE_Set_Hell_Blitter_B(SubBuf , SubBuf , 0 , 1);
		
		/* BLUR */
		BWSpecial = 1;
		stDST.DX += (s32)(Radius * 320.0f * 16.0f);
		//stDST.DY += (s32)(Radius * 320.0f * 16.0f);
		FactorL = 1.0f;
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| ((u_long)((1.0f / FactorL) * 127.0f) << 32L));
		Gsp_AE_SetGSRegisters();
		
		stDST_C = stDST;stSRC_C = stSRC;
		Gsp_AE_ClipRect(&stDST_C,&stSRC_C,&stSRC_CLIP);
		stSRC_C.DX += ((u_int)GspGlobal_ACCESS(Xsize)>>1)<<4;
		Gsp_AE_CaseBlitter_FRAC(&stDST_C,&stSRC_C,0x80808080);
		stDST = stSRC;

		stDST.DX -= (s32)(Radius * 320.0f * 16.0f);
		//stDST.DY += (s32)(Radius * 320.0f * 16.0f);
		FactorL += 1.0f;
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| ((u_long)((1.0f / FactorL) * 127.0f) << 32L));
		Gsp_AE_SetGSRegisters();
		stDST_C = stDST;stSRC_C = stSRC;
		Gsp_AE_ClipRect(&stDST_C,&stSRC_C,&stSRC_CLIP);
		stSRC_C.DX += ((u_int)GspGlobal_ACCESS(Xsize)>>1)<<4;
		Gsp_AE_CaseBlitter_FRAC(&stDST_C,&stSRC_C,0x80808080);
		stDST = stSRC;

		//stDST.DX -= (s32)(Radius * 320.0f * 16.0f);
		stDST.DY -= (s32)(Radius * 320.0f * 16.0f);
		FactorL += 1.0f;
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| ((u_long)((1.0f / FactorL) * 127.0f) << 32L));
		Gsp_AE_SetGSRegisters();
		stDST_C = stDST;stSRC_C = stSRC;
		Gsp_AE_ClipRect(&stDST_C,&stSRC_C,&stSRC_CLIP);
		stSRC_C.DX += ((u_int)GspGlobal_ACCESS(Xsize)>>1)<<4;
		Gsp_AE_CaseBlitter_FRAC(&stDST_C,&stSRC_C,0x80808080);
		stDST = stSRC;

		//stDST.DX += (s32)(Radius * 320.0f * 16.0f);
		stDST.DY += (s32)(Radius * 320.0f * 16.0f);
		FactorL += 1.0f;
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| ((u_long)((1.0f / FactorL) * 127.0f) << 32L));
		Gsp_AE_SetGSRegisters();
		stDST_C = stDST;stSRC_C = stSRC;
		Gsp_AE_ClipRect(&stDST_C,&stSRC_C,&stSRC_CLIP);
		stSRC_C.DX += ((u_int)GspGlobal_ACCESS(Xsize)>>1)<<4;
		Gsp_AE_CaseBlitter_FRAC(&stDST_C,&stSRC_C,0x80808080);
		stDST = stSRC;
		BWSpecial = 0;
		
		
		{
			u_int Stoppy;
			Stoppy = p_AE_CurrentBufferPointer - GspGlobal_ACCESS(p_CurrentBufferPointer);
			if (Stoppy > (RAM_BUF_SIZE - 0x1000))
			{
				Gsp_AE_END();
				Gsp_FIFO_STOP();
				Gsp_AE_BEGIN();
			}
		}
		//*/
		Radius*= 0.5f;
	}
	/* Copy from ZBUF */
	if (!SubUse)
	{
		stSRC.DX = 0;
		stSRC.DY = 0;
		stSRC.SX = (u_int)GspGlobal_ACCESS(Xsize);
		stSRC.SY = (u_int)GspGlobal_ACCESS(Ysize);
		Gsp_AE_ShiftRect(&stSRC);
		stDST = stSRC;
		
		stSRC.SX >>= 1;
		stSRC.SY >>= 1;
		GSP_AE_SetRegister_TEST_1( 0L);
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		
		Gsp_AE_Set_Hell_Blitter_B(SubBuf , 0 , 0 , 0);
		Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC,0x80808080);
		
		Gsp_AE_END();
	}
}


void Gsp_AE_Emboss(u_int DX , u_int DY , u_int Strenght)
{
	u_int Color;
	
	Gsp_AE_BEGIN();
	
	Color = (Strenght) | (Strenght << 8) | (Strenght << 16) | 0xff000000;
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter(0 , 2 , 0 );
	Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , 0x80808080); 
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_SUB2x);
	Gsp_AE_Set_Hell_Blitter(2 , 2 , 0 );
	Gsp_AE_Hell_Blitter(-1,-1, 0 , 0 , 0 , 0 , 0xffffffff , 0x80808080); 
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_SUB2x);
	Gsp_AE_Set_Hell_Blitter(2 , 0 , 0 );
	Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , Color); 
	Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , Color); 
	Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , Color); 
	
	Gsp_AE_END();
	
}


void Gsp_AE_MotionSmooth(int GDX , int GDY)
{
	u_int Counter , Number;
	int GDX2 , GDY2 ;
	
	Gsp_AE_BEGIN();
	
	GDX2 = GDX;
	GDY2 = GDY;
	if (GDX < 0) GDX = -GDX;
	if (GDY < 0) GDY = -GDY;
	GDX >>=1;
	GDY >>=1;
	if (GDX > 32) GDX = 32;
	if (GDY > 32) GDY = 32;
	Counter = lMax(GDX,GDY);
	Number = 0;
	while (Counter)
	{
		Counter>>=1;
		Number++;
	}
	GDX2 >>= 1;
	GDY2 >>= 1;
	Number = lMin(Number , 6);
	/* Clear ZBUF */	
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter(3 , 2 , 0 );
	Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , 0); 
	for (Counter = 0 ; Counter < Number ; Counter++)
	{	
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		if (Counter & 1)
			Gsp_AE_Set_Hell_Blitter(2 , 0 , 0 );
		else
			Gsp_AE_Set_Hell_Blitter(0 , 2 , 0 );
		Gsp_AE_Hell_Blitter(-GDX2 , -GDY2 , 0 , 0 , 0 , 0 , 0xffffffff , 0x0040404040); 
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ADD);
		Gsp_AE_SetGSRegisters();
		Gsp_AE_Hell_Blitter(GDX2 , GDY2 , 0 , 0 , 0 , 0 , 0xffffffff , 0x0040404040); 
		GDX2 >>= 1;
		GDY2 >>= 1;
	}
	if (Number & 1)
	{
		/* 3 : Copy Z buffer in FrameBuffer */
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter(2 , 0 , 0 );
		Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , 0x80808080); 
	}

	Gsp_AE_END();

}

void Gsp_AE_ZoomSmooth(float Coef,ULONG Mode)
{
	u32 Counter,Number;
	float K,Kinc;
	if (Coef < 0.005f) 
	{
		return;
	}

	
	Gsp_AE_BEGIN();
	
	if (Mode == 0)
	{
		Coef *= 0.25f;
		Number = 12;
	}
	else
	{ 
		Number = 15;
	}
	Number = 4;
	/* 2 : GodRay */
	if (Mode == 0) /* Zoomsmooth*/
	{
		float ConvoleRevolution;
		ConvoleRevolution = 0.5f;
		Coef *= 4.25f;
		K = 0.0f;
		Counter = 0;
		Kinc = Coef / 2.0f;
		while (Kinc > 0.003f) //(Counter = 0 ; Counter < Number ; Counter++)
		{	
			u32 Color;
			if (Counter == 0) GSP_AE_SetRegister_TEST_1( 0L);
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
			if (Counter & 1)
				Gsp_AE_Set_Hell_Blitter(2 , 0 , 0 );
			else
				Gsp_AE_Set_Hell_Blitter(0 , 2 , 0 );
			Color = 0x404040;
			
			Color = (u32)(ConvoleRevolution * 128.0f);
			Color |= Color<<8;
			Color |= Color<<16;
			
			Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , Color); 
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ADD);
			Gsp_AE_SetGSRegisters();
			
			Color = (u32)((1.0f - ConvoleRevolution) * 128.0f);
			Color |= Color<<8;
			Color |= Color<<16;
			
			Gsp_AE_Hell_Blitter_UV1_UV2_ROT_C1_C2(0 ,0 ,Kinc,Kinc,1.0f - Kinc,1.0f - Kinc,0.0f,0,0,0,0,0xffffffff , Color , Color);
			Kinc *= 0.5f;
			Counter++;
		}	
	}
	else
	{
		K = 0.0f;
		GSP_AE_SetRegister_TEST_1( 0L);
		/* clear ZBuffer */
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter(3 , 2 , 0 );
		Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , 0); 
		Coef *= 3.1415927f / 4.0f;
		Kinc = Coef / 4.0f;
		for (Counter = 0 ; Counter < Number ; Counter++)
		{	
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
			if (Counter & 1)
				Gsp_AE_Set_Hell_Blitter(2 , 0 , 0 );
			else
				Gsp_AE_Set_Hell_Blitter(0 , 2 , 0 );
			Gsp_AE_Hell_Blitter_UV1_UV2_ROT_C1_C2(0 ,0 ,0.0f,0.0f,1.0f,1.0f,-Kinc,0,0,0,0,0xffffffff , 0x0040404040 , 0x0040404040);
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ADD);
			Gsp_AE_SetGSRegisters();
			Gsp_AE_Hell_Blitter_UV1_UV2_ROT_C1_C2(0 ,0 ,0.0f,0.0f,1.0f,1.0f,Kinc,0,0,0,0,0xffffffff , 0x0040404040 , 0x0040404040);
			Kinc *= 0.5f;
		}
	}
	if (Number & 1)
	{
		/* 3 : Copy Z buffer in FrameBuffer */
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter(2 , 0 , 0 );
		Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , 0x80808080); 
	}
	
	Gsp_AE_END();
	
}
#define SWAP_RECT(A,B) {Gsp_Rect *SWP ; SWP = A ; A = B ; B = SWP; }
void Gsp_AE_ZoomSmoothGodRays(float Coef,MATH_tdst_Vector *LigthDir , u32 CenterMode)
{
	u32 Counter,Number,Invert;
	float KoefVG,Ky,Kinc,KincSave,Px,Py;
	float ConvoleRevolution;
	Gsp_Rect p_1,p_2A,p_2B,p_3;
	Gsp_Rect *p_DST,*p_SRC;
	
	MATH_tdst_Vector	ScreenC; 
	s32 Cx,Cy;
	
//	return;
	
	
	if (CenterMode)
		ScreenC = *LigthDir;
	else MATH_NegVector(&ScreenC , LigthDir);
	MATH_TransformVector(&ScreenC, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &ScreenC);
	MATH_NormalizeAnyVector(&ScreenC, &ScreenC);
	if (CenterMode)
	{
		if (ScreenC . z < 0.0f) return;
	}
	
	if (ScreenC . z * ScreenC . z > 0.0001f)
	{
		ScreenC . x = 0.5f + 0.5f * (GspGlobal_ACCESS(fCurrentFocale) * ScreenC . x) / ScreenC . z;
		ScreenC . y = 0.5f + 0.5f * (GspGlobal_ACCESS(YCorrectionCoef) * GspGlobal_ACCESS(fCurrentFocale) * ScreenC . y) / ScreenC . z;
		Cx = (int)((float)GspGlobal_ACCESS(Xsize) * ScreenC . x);
		Cy = (int)((float)GspGlobal_ACCESS(Ysize) * ScreenC . y);
		Invert = 0;
		if (ScreenC . z > 0.0f) Invert = 1.0f;
	} else return;

	

	Gsp_AE_BEGIN();
	
	KoefVG = 0.0f;
	Counter = 0;
	Kinc = 4;
	if (CenterMode)
	{
		Kinc = ScreenC . z * ScreenC . z * Coef * 2.0;
	}
	
	KoefVG = GspGlobal_ACCESS(Xsize)>>5;
	Ky = GspGlobal_ACCESS(Ysize)>>5;
	p_DST = &p_2A;
	p_SRC = &p_2B;

	Cx >>= 1;
	Cy >>= 1;
	
	p_1.DX = 0;
	p_1.DY = 0;
	p_1.SX = GspGlobal_ACCESS(Xsize);
	p_1.SY = GspGlobal_ACCESS(Ysize);
	p_2A.DX = 0;
	p_2A.DY = 0;
	p_2A.SX = GspGlobal_ACCESS(Xsize)>>1;
	p_2A.SY = GspGlobal_ACCESS(Ysize)>>1;
	p_2B.DX = GspGlobal_ACCESS(Xsize)>>1;
	p_2B.DY = 0;
	p_2B.SX = GspGlobal_ACCESS(Xsize)>>1;
	p_2B.SY = GspGlobal_ACCESS(Ysize)>>1;
	Gsp_AE_ShiftRect(&p_1);
	Gsp_AE_ShiftRect(&p_2A);
	Gsp_AE_ShiftRect(&p_2B);

	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister(GSP_TEXFLUSH 	, 0L);
	Gsp_AE_Set_Hell_Blitter_B(3 , 0 , 0xffffff ,0);
	Gsp_AE_CaseBlitter_FRAC(&p_1,&p_1 , 0);
	
/*	Gsp_AE_EraseAlphaBuffer(0x80 , 0);*/
	{
		GSP_AE_SetRegister_TEST_1( (1L << 16L) | (2L << 17L));
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter_B(3 , 0 , 0xffffff ,0);
		Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0 ,0x80 << 24); 
	}
	
#define DESTBUSZSGR 2	
/*	EraseWorkingBuffer */
	p_3.DX = 0;
	p_3.DY = 0;
	p_3.SX = GspGlobal_ACCESS(Xsize) + 10;
	p_3.SY = (GspGlobal_ACCESS(Ysize)>>1) + 10;
	GSP_AE_SetRegister_TEST_1( 0L );
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter_B(3 , DESTBUSZSGR , 0 ,0);
	Gsp_AE_ShiftRect(&p_3);
	Gsp_AE_CaseBlitter_FRAC(&p_3,&p_3 , 0);

	
	

	/* Errase according to alpha */
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	if (CenterMode)
		Gsp_AE_Set_Hell_Blitter_B(0 , DESTBUSZSGR , 1 ,1);
	else
		Gsp_AE_Set_Hell_Blitter_B(0 , DESTBUSZSGR , 0 ,0);
	Gsp_AE_CaseBlitter_FRAC(p_DST,&p_1 , 0x80808080);
	
	if (!CenterMode)
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 , (0x00L << 32L) | (2L << 6L) | (1L << 4L) | (2L << 2L) | (0L << 0L));
		Gsp_AE_Set_Hell_Blitter_B(DESTBUSZSGR , DESTBUSZSGR , 0 ,0);
		Gsp_AE_CaseBlitter_FRAC(p_DST,p_DST , 0x80808080);
	}

	Gsp_AE_Set_Hell_Blitter_B(DESTBUSZSGR , DESTBUSZSGR , 0 ,0);
	KincSave = Kinc;
	if (1)
	{
		Cx <<= 4;
		Cy <<= 4;
		ConvoleRevolution = 0.666666f;	
		if (CenterMode)
					ConvoleRevolution = 0.5f;	
		while (Kinc > 0.003f) //(Counter = 0 ; Counter < Number ; Counter++)
		{	
			u32 Color;
			Gsp_Rect st_ZoomS,st_ZoomD;
			SWAP_RECT(p_DST,p_SRC);
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
			Color = (u32)(ConvoleRevolution * 128.0f);
			Color |= Color<<8;
			Color |= Color<<16;
			
			Gsp_AE_SetGSRegisters();
			Gsp_AE_CaseBlitter_FRAC(p_DST,p_SRC , Color);
			
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ADD);
			Gsp_AE_SetGSRegisters();
		
			Color = (u32)((1.0f - ConvoleRevolution) * 128.0f);
			Color |= Color<<8;
			Color |= Color<<16;
			
			st_ZoomS = *p_SRC;
			st_ZoomD = *p_DST;
			
			Cx += p_SRC->DX;
			Cy += p_SRC->DY;
			
			st_ZoomS.SX += st_ZoomS.DX;
			st_ZoomS.SY += st_ZoomS.DY;
			if (Invert)
				KoefVG = (1.0f / (1.0f + 0.1f * Kinc));
			else
				KoefVG = 1.0f + 0.1f * Kinc;

			st_ZoomS.DX = (s32)(((float)Cx + ((float)st_ZoomS.DX - (float)Cx) * KoefVG) * 1.0f);
			st_ZoomS.SX = (s32)(((float)Cx + ((float)st_ZoomS.SX - (float)Cx) * KoefVG) * 1.0f);

			st_ZoomS.DY = (s32)(((float)Cy + ((float)st_ZoomS.DY - (float)Cy) * KoefVG) * 1.0f);
			st_ZoomS.SY = (s32)(((float)Cy + ((float)st_ZoomS.SY - (float)Cy) * KoefVG) * 1.0f);//*/
//			Gsp_AE_ShiftRect(&st_ZoomS);
			st_ZoomS.SX -= st_ZoomS.DX;
			st_ZoomS.SY -= st_ZoomS.DY;
			
			
			st_ZoomS.DX -= 8;
			st_ZoomS.DY -= 8;

			Cx -= p_SRC->DX;
			Cy -= p_SRC->DY;
			
			if (!Gsp_AE_ClipRect(&st_ZoomS,&st_ZoomD,p_SRC))
				Gsp_AE_CaseBlitter_FRAC(&st_ZoomD,&st_ZoomS , Color);
			else
				Gsp_AE_CaseBlitter_FRAC(p_DST,p_SRC , Color);
			
			Kinc *= 0.5f;
			ConvoleRevolution = (ConvoleRevolution - 0.5f) * 0.5f + 0.5f;
		}
		Kinc = KincSave;		
		KincSave = 0.0f;
	}	
	if (!CenterMode)
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ADD);
	} else
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Coef = 1.0f;
	} 
	Gsp_AE_SetGSRegisters();
	
	Gsp_AE_Set_Hell_Blitter_B(DESTBUSZSGR , 0 , 0 ,0);
	{
		u32 Color;
		float Max;
		Coef *= 162.0f;
		Max = 128.0f;
		if (!CenterMode) Max = 160.0f;
		if (Coef>Max) Coef = Max;
		Color = 	(u32)Coef ;
		Color |= Color << 8;
		Color |= Color << 16;
		Gsp_AE_CaseBlitter_FRAC(&p_1,p_DST , Color);
	}
	
	Gsp_AE_END();
	
}

void Gsp_AE_ShowShadows()
{
///*
	Gsp_AE_BEGIN();

	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_SHADOWMODE);
	Gsp_AE_Set_Hell_Blitter(8 , 0 , 0xff000000 );
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff ,0x60808080); 
	
	Gsp_AE_END();
	
//*/
}
void Gsp_AE_CopyShadows()
{
#ifndef GSP_USE_TRIPLE_BUFFERRING	
	Gsp_AE_BEGIN();

	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter(1 , 0 , 0x00ffffff ); // Hide color
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff ,0x80808080); //*/
	
	Gsp_AE_END();
#endif
}
void Gsp_AE_FlushAllSpecial()
{
#ifndef GSP_USE_TRIPLE_BUFFERRING	
	u_int Counter;
	Gsp_Rect stDST;
	/* To avoid Complex track of Finish */
	/* Draw 16 micro Quad (1x1)*/
	Counter = 16;
	Gsp_AE_BEGIN();
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1(0L);
	Gsp_AE_Set_Hell_Blitter(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0xff000000 );
	stDST.DX = 0;
	stDST.DY = 0;
	stDST.SX = 1;
	stDST.SY = 1;
	while (Counter--) 
	{
		Gsp_AE_CaseBlitter(&stDST,&stDST,0);
		{
			u_int Stoppy;
			Stoppy = p_AE_CurrentBufferPointer - GspGlobal_ACCESS(p_CurrentBufferPointer);
			if (Stoppy > (RAM_BUF_SIZE - 0x1000))
			{
				Gsp_AE_END();
				Gsp_FIFO_STOP();
				Gsp_AE_BEGIN();
			}
		}
	}
	
	Gsp_AE_END();
#endif
	
}


void Gsp_AE_ShowAlphaBuffer()
{

	Gsp_AE_BEGIN();

	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 , (0x00L << 32L) | (2L << 6L) | (1L << 4L) | (2L << 2L) | (0L << 0L));
	Gsp_AE_Set_Hell_Blitter(3 , 0 , 0xff000000 );
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff ,0x80808080); 
	
	Gsp_AE_END();
	
}


void Gsp_AE_ShowZbuffer()
{
	Gsp_AE_ShowAlphaBuffer();
}

void GSP_Remanance(float Factor,u32 Asglow)
{
	u_int Color;
	Gsp_Rect stDST,stSRC;
	u_int Remorf , YSize;
	u_int ModeSub;
	ModeSub = 0;
	if (Factor < 0.0f) 
	{
		Factor = -Factor;
		ModeSub = 1;
	}
	
	if (Factor < 0.005f) 
	{
		return;
	}
	
	Color = (u_int)(255.0f * Factor);
	Color = (Color << 24) | (Color << 16) | (Color << 8) | (Color << 0);
	
	Remorf = GspGlobal_ACCESS(Morfling);
	Remorf &= ~31;
	YSize = GspGlobal_ACCESS(Ysize) + 31;
	YSize &= ~31;
	YSize -= (Remorf << 1);

	
	Gsp_AE_BEGIN();	
	
	Gsp_AE_Blur2(1.0f,1,Asglow);
	
	stSRC.DX = 0;
	stSRC.DY = 0;
	stSRC.SX = (u_int)GspGlobal_ACCESS(Xsize);
	stSRC.SY = (u_int)GspGlobal_ACCESS(Ysize);
	Gsp_AE_ShiftRect(&stSRC);
	stDST = stSRC;
	
	stSRC.SX >>= 1;
	stSRC.SY >>= 1;
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ADD);
	
	Gsp_AE_Set_Hell_Blitter_B(Asglow ? 4 : 2, 0 , 0 , 0);
	while (Factor > 1.0f)
	{
		Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC,0x808080);
		Factor -= 1.0f;
	}
	Color = (u32)(Factor * 128.0f);
	Color |= Color << 8;
	Color |= Color << 16;
	Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC,Color );
	Gsp_AE_END();
}

float GetPhase(float fColorSpctr)
{
	if (fColorSpctr > 1.0f) fColorSpctr-= 1.0f;
	if (fColorSpctr > 0.5f) fColorSpctr = 1.0f - fColorSpctr;
	if (fColorSpctr < 0.1666666f) return 1.0f;
	if (fColorSpctr > 0.3333333f) return 0.0f;
	fColorSpctr = fColorSpctr - 0.166666666f;
	fColorSpctr *= 6.0f;
	return 1.0f - fColorSpctr;
}


void GSP_FlipH()
{
	u_int Color;
	Gsp_Rect stDST,stSRC;
	u_int YSize;
	u_int ModeSub;
	

	
	Gsp_AE_BEGIN();	
/* Copy Frame buffer to ZBuffer / 4  */ 
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter_B(0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , 4->Textures , | 0x80000000 -> TextureNum	
						2  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = 0;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = GspGlobal_ACCESS(Ysize); 
	stDST = stSRC;
	Gsp_AE_CaseBlitter(&stDST,&stSRC , 0x80808080);

	Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , 4->Textures , | 0x80000000 -> TextureNum	
						0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stDST.DX = GspGlobal_ACCESS(Xsize);
	stDST.DY = 0;
	stDST.SX = -GspGlobal_ACCESS(Xsize);
	stDST.SY = GspGlobal_ACCESS(Ysize); 
	Gsp_AE_CaseBlitter(&stDST,&stSRC , 0x80808080);
	
	Gsp_AE_END();
}

void Gsp_AE_BrightnessContrast_ColorBalance(float Contrast , float Brighness , float CB_I , float CB_S , float CB_PhotoFilterEffect , float CB_Pastel)
{
	ULONG Color,Color2;
	/* Brightness */
	
	Gsp_AE_BEGIN();

	/* ColorBalance */
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_CB_On)//*/
	{
		float Red,Green,Blue;
		u_int OverTwo;
		CB_I = fMax(CB_I , 0.f);
		CB_I = fMin(CB_I , 1.f);
		
		Red   = CB_I * (1.0f - GetPhase(CB_S)) + (1.0f - CB_I);
		Green = CB_I * (1.0f - GetPhase(CB_S - 0.3333333f + 1.0f )) + (1.0f - CB_I);
		Blue  = CB_I * (1.0f - GetPhase(CB_S - 0.6666666f + 1.0f)) + (1.0f - CB_I);
		
		
		Color = (u_int)(Red * 255.0f);
		Color |= (u_int)(Green * 255.0f) << 8;
		Color |= (u_int)(Blue * 255.0f) << 16;//*/
/*		OverTwo = 0;
		if (CB_PhotoFilterEffect >= 0.9f)
		{
			OverTwo = 1;
			CB_PhotoFilterEffect = 1.0f;
		}
		Color2 = (u_int)((1.0f - CB_PhotoFilterEffect) * Red * 255.0f);
		Color2 |= (u_int)((1.0f - CB_PhotoFilterEffect) * Green * 255.0f) << 8;
		Color2 |= (u_int)((1.0f - CB_PhotoFilterEffect) * Blue * 255.0f) << 16;//*/
		
		Color &= 0xfefefe;
//		Color2 &= 0xfefefe;
		Color >>= 1;
//		Color2 >>= 1;
		
		GSP_AE_SetRegister_TEST_1( 0L);
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter_B(0 , 0 , 0xff000000 ,0); // Hide Alpha
		Gsp_AE_Hell_Blitter_UV1_UV2_ROT_C1_C2(0 , 0 , 0.0f,0.0f,1.0f,1.0f,0.0f,0,0,0 ,OverTwo , 0xffffffff , Color , Color);
	}
	
	
	if (Brighness != 0.5f)
	{
		Brighness = fMax(Brighness , 0.0f);
		Brighness = fMin(Brighness , 1.0f);
		if (Brighness < 0.5f)
		{
			Brighness = (0.5f - Brighness) * 1.0f;
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_SUB2x);
		} else
		{
			Brighness = (Brighness - 0.5f) * 2.0f;
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ADD);
		}
		GSP_AE_SetRegister_TEST_1( 0L);
		Gsp_AE_Set_Hell_Blitter_B(3 , 0 , 0xff000000 ,0); // Hide Alpha
		Color = (u_int)(Brighness * 255.0f);
		Color |= Color << 8;
		Color |= Color << 16;
		Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff,Color); 
	}
	/* Contrast */
//	Contrast = 0.5f;
	if (Contrast > 0.5f)
	{
		Contrast -= 0.5f;
		if (Contrast > 1.0f) Contrast = 1.0f;
//		(A -B) * C + D
		GSP_AE_SetRegister(GSP_ALPHA_1 	, (((u_long64)(Contrast * 255.0f)) << 32L) | (0L << 6L) | (2L << 4L) | (2L << 2L) | (0L << 0L));
		GSP_AE_SetRegister_TEST_1( 0L);
		Gsp_AE_Set_Hell_Blitter_B(0 , 0 , 0xff000000 ,0); // Hide Alpha
		Color = 0x808080;
		Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff,Color); 
	}
	
	Gsp_AE_END();
	
}
int Get_TheFactor(u_int TextureSize,u_int BigSize)
{
	int Ret;
	Ret = 0;
	BigSize = (BigSize + 0xf) & 0xfffffff0;	
	while (TextureSize < BigSize)
	{
		Ret--;
		TextureSize<<=1;
	}
	while (TextureSize > BigSize)
	{
		Ret++;
		TextureSize>>=1;
	}
	return Ret;
}
int GetRealNumberOfTexture(ULONG *RealNum)
{
	int TexCounter,RealTexNumber;
	if (!GspGlobal_ACCESS(ulNumberOfTextures)) return 0;
	TexCounter = GspGlobal_ACCESS(ulNumberOfTextures);
	RealTexNumber = 0;
	while (TexCounter--)
	{
		if ((GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexCounter].ZOB != 32) && 
			((GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexCounter].TEX0_REG & 0xfffffff800000000L) == 0L)) // Is palette
		{
		} else 
		if ((GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexCounter].W == 0) || (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexCounter].H == 0)) // Is Raw
		{
		} else
			RealNum[RealTexNumber ++] = TexCounter;
		
	}
	return RealTexNumber;
}

extern void GSP_SetTextureCache(Gsp_BigStruct *p_Globals , ULONG _l_Texture);
int TextnumToShow = 0;
void Gsp_AE_ShowTexture()
{
	LONG TexNum , ulNumberOfTextures;
	LONG AllTextSize[1024];
	LONG TExRealNum[1024];
	Gsp_Rect RS , RD  ;
	RD . DX = 0;
	RD . DY = GspGlobal_ACCESS(Ysize) - GspGlobal_ACCESS(Morfling);
	
	Gsp_Flush(FLUSH_ALL);
	ulNumberOfTextures = GetRealNumberOfTexture( TExRealNum);
	if (!ulNumberOfTextures) return;
	/* compute all Size; */
	TexNum = ulNumberOfTextures;
	while (TexNum--)
	{
		ULONG Local;
		Local =TExRealNum[TexNum];
		AllTextSize[TexNum] = (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[Local].ZOB*GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[Local].W*GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[Local].H) >> 13;
	}
	/* Sort By Size */
	if (ulNumberOfTextures > 1)
	{
		LONG X,Y;
		for (X = 0 ; X < ulNumberOfTextures - 1 ; X++)
		for (Y = X + 1 ; Y < ulNumberOfTextures  ; Y++)
		{
			if (AllTextSize[X] < AllTextSize[Y])
			{
				ULONG Swap;
				Swap = AllTextSize[X];
				AllTextSize[X] = AllTextSize[Y];
				AllTextSize[Y] = Swap;
				Swap = TExRealNum[X];
				TExRealNum[X] = TExRealNum[Y];
				TExRealNum[Y] = Swap;
			}
		}
	}
	
	
	
	TextnumToShow =lMax( lMin(ulNumberOfTextures -1, TextnumToShow ) , 0 );
	
	/* Show ThumBall */
	RD . DX = 32 + 16;
	RD . DY = (GspGlobal_ACCESS(Ysize) >> 1) - GspGlobal_ACCESS(Morfling) - 32;
	RD . SX = 32;
	RD . SY = 32;
	{
		long TExtloc;
		for (TExtloc = TextnumToShow -8 ; TExtloc < TextnumToShow + 16 ; TExtloc++)
		{
			if ((TExtloc >= 0) && (TExtloc < ulNumberOfTextures))
			{
				
				TexNum = TExRealNum[TExtloc];
				GSP_SetTextureCache(&gs_st_Globals , TexNum);

				Gsp_AE_BEGIN();
				RS . DX = 0;
				RS . DY = 0;
				RS . SX = GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].W;
				RS . SY = GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].H;
				GSP_AE_SetRegister(GSP_TEXFLUSH 	, 0L);
				GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
				GSP_AE_SetRegister_TEST_1( 0L);
				Gsp_AE_Set_Hell_Blitter(TexNum | 0x80000000 , 0 , 0xff000000 ); // Hide Alpha
				Gsp_AE_CaseBlitter(	&RD , &RS ,	0x80808080);
				Gsp_AE_END();
				Gsp_FIFO_STOP();
				Gsp_Flush(FLUSH_ALL);
			}
			RD . DX += 32;
		}
	}
	TexNum = TExRealNum[TextnumToShow];
	GSP_SetTextureCache(&gs_st_Globals , TexNum);
	
	Gsp_AE_BEGIN();
	
	RD . DX = 0;
	RD . DY = GspGlobal_ACCESS(Ysize) - GspGlobal_ACCESS(Morfling);
	RD . SX = lMin(GspGlobal_ACCESS(Xsize) >> 1 , GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].W);
	RD . SY = lMin(GspGlobal_ACCESS(Ysize) >> 1 , GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].H);
	RD . SX = GspGlobal_ACCESS(Xsize) >> 1;
	RD . SY = -(GspGlobal_ACCESS(Ysize) >> 1);
	RS . DX = 0;
	RS . DY = 0;
	RS . SX = GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].W;
	RS . SY = GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].H;
	
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].InterfaceFlags |= GSP_IsNeeded;
	{
		GSP_AE_SetRegister(GSP_TEXFLUSH 	, 0L);
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GSP_AE_SetRegister_TEST_1( 0L);
		Gsp_AE_Set_Hell_Blitter(TexNum | 0x80000000 , 0 , 0x00000000 ); // Hide Alpha
		Gsp_AE_CaseBlitter(	&RD , &RS ,	0x80000000);
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_PALPHA);
		GSP_AE_SetRegister_TEST_1( 0L);
		Gsp_AE_Set_Hell_Blitter(3 , 0 , 0x00000000 ); // Hide Alpha
		Gsp_AE_CaseBlitter(	&RD , &RS ,	0x80FFFFFF);
	}
	RD . DX = GspGlobal_ACCESS(Xsize) >> 1;
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter(TexNum | 0x80000000 , 0 , 0xff000000 ); // Hide Alpha
	Gsp_AE_CaseBlitter(	&RD , &RS ,	0x80808080);
	
	GspGlobal_ACCESS(CUR_REG_FRAME) &= 0x00000000ffffffffL;
	GSP_AE_SetRegister(GSP_FRAME_1 , GspGlobal_ACCESS(CUR_REG_FRAME));
	Gsp_AE_SetGSRegisters();
	
	Gsp_AE_END();
	Gsp_FIFO_STOP();
	Gsp_Flush(FLUSH_ALL);
	{
		unsigned char String[256];
		unsigned char String2[256];
		ULONG BarCounter;
		ULONG COLOR;
		COLOR = 0xff80ff;
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ZOB == 4) COLOR = 0x8888FF;
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ZOB == 8) COLOR = 0x88FF88;
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ZOB == 32) COLOR = 0xFF8888;
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ZOB == 4)
		sprintf(String, "num %d / %d \nBigkey = 0x%X \n%dbpp %d * %d \nsize %dK \nPalette %d ", TextnumToShow , ulNumberOfTextures-1 ,GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ulBigKey , GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ZOB,GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].W,GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].H,
		AllTextSize[TextnumToShow] , 
		((GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].TEX0_REG & (2L << 51L)) != 0L) ? 16 : 32
		);
		else
		sprintf(String, "num %d / %d \nBigkey = 0x%X \n%dbpp %d * %d \nsize %dK", TextnumToShow , ulNumberOfTextures  -1,GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ulBigKey , GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].ZOB,GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].W,GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].H,
		AllTextSize[TextnumToShow]
		);
		

		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TexNum].p_BitmapPtr) 
		{
			if ((GspGlobal_ACCESS(LastVBlankCounter) & 0x8))
			{
				sprintf(String2, "%s\nReal-time loaded", String);
				sprintf(String, "%s", String2);
			}
		}
			
		GSP_DrawTExt_X2Y2(GspGlobal_ACCESS(Xsize)>>1,(GspGlobal_ACCESS(Ysize)>>3) - 58,String,COLOR,0,GST_COPY,GST_COPY,1,1);
		Gsp_Flush(FLUSH_ALL);
	}
}





static MATH_tdst_Vector		MotionSmoothPoint; 
static MATH_tdst_Vector		FadePoint; 
u_int ulMotionBlurValue = 7;
u_int ulRemananceStrong = 0;
u_int ulZoomSmoothStreinght = 0;
u_int ulRotateSmoothStreinght = 0;

float Gsp_AE_MASTER_GET_P1(ULONG AENum , ULONG P1 )
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			switch (P1)
			{
				case 0: return (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart);
				case 1: return (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd);
				case 2: return (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Factor);
			}
		case 1: // Motion blur
			return (GspGlobal_ACCESS(p_AE_Params)->MotionBlurFactor);
		case 2: // screen Smooth
			return (GspGlobal_ACCESS(p_AE_Params)->BlurFactor);
		case 3: // Emboss
			return (GspGlobal_ACCESS(p_AE_Params)->EmbossFactor);
		case 4: // Motion smooth
			return 0.0f;
		case 5: // Zoom smooth
			return (GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor);
		case 6: // Rotate smooth
			return (GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor);
		case 7: // Remanance
			return (GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor);
		case 8: // Brightness
			return (GspGlobal_ACCESS(p_AE_Params)->Brighness);
		case 9: // Contrast
			return (GspGlobal_ACCESS(p_AE_Params)->Contrast);
		case 10: // AA
			return (float)(GspGlobal_ACCESS(p_AE_Params)->AA22BlurMode);
		case 11: // BW
			return (GspGlobal_ACCESS(p_AE_Params)->BWFactor);
		case 12: // Color balance
			switch (P1)
			{
				case 0: return (GspGlobal_ACCESS(p_AE_Params)->CB_Intensity);
				case 1: return (GspGlobal_ACCESS(p_AE_Params)->CB_Spectre);
				case 2: return (GspGlobal_ACCESS(p_AE_Params)->CB_PhotoFilterEffect);
			}
			return 0.0f;
		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: return (GspGlobal_ACCESS(fFogCorrector));
				case 1: return (GspGlobal_ACCESS(fFogCorrectorFar));
			}
			return 0.0f;
			break;
		case 14: // Depth blur near
			return (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near);
			break;
		case 17: // Depth blur near
			return GspGlobal_ACCESS(p_AE_Params)->fWarpFactor;
			break;
		case 22: // Glow
			return GspGlobal_ACCESS(p_AE_Params)->Glow;
			break;
	}
	return 0.0f;
}

ULONG Gsp_AE_MASTER_GET_ONOFF(ULONG AENum )
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			return (GspGlobal_ACCESS(Status)  & GSP_Status_AE_DB);
		case 1: // Motion blur
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_MB);
		case 2: // screen Smooth
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_SM);
		case 3: // Emboss
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_EMB);
		case 4: // Motion smooth
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_SMSM);
		case 5: // Zoom smooth
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_ZS);
		case 6: // Rotate smooth
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_RS);
		case 7: // Remanance
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_REMANANCE);
		case 10: // AA
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_BlurTest44);
		case 11: // BW
			return (GspGlobal_ACCESS(p_AE_Params)->BWFactor != 0.0f);
		case 12: // Color balance
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_CB_On);
		case 13: // fOG CORRECTION
			break;
		case 14: // Depth Blur nrear
			return (GspGlobal_ACCESS(Status) & GSP_Status_AE_DBN);
			break;
		case 17: // warp
			return (GspGlobal_ACCESS(p_AE_Params)->fWarpFactor != 0.0f);
			break;
		case 22: // Glow
			return (GspGlobal_ACCESS(p_AE_Params)->Glow != 0.0f);
			break;
			
	}
	return 0;
}


void Gsp_AE_MASTER(ULONG AENum , ULONG OnOff , ULONG P1 , float Pf1)
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_DB;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_DB;
			switch (P1)
			{
				case 0: GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart = Pf1;	break;
				case 1: GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd = Pf1;		break;
				case 2: GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Factor = Pf1;	break;
			}
			break;
		case 1: // Motion blur
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_MB;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_MB;
			GspGlobal_ACCESS(p_AE_Params)->MotionBlurFactor = Pf1;
			break;
		case 2: // screen Smooth
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_SM;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_SM;
			GspGlobal_ACCESS(p_AE_Params)->BlurFactor = Pf1;
			break;
		case 3: // Emboss
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_EMB;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_EMB;
			GspGlobal_ACCESS(p_AE_Params)->EmbossFactor = Pf1;
			break;
		case 4: // Motion smooth
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_SMSM;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_SMSM;
			break;
		case 5: // Zoom smooth
			GspGlobal_ACCESS(Status) |= GSP_Status_AE_ZS;
			switch (P1)
			{
				case 0: GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor = Pf1;	break;
				case 1: GspGlobal_ACCESS(p_AE_Params)->ZSDir.x = Pf1;GspGlobal_ACCESS(p_AE_Params)->ZSDirIsValidated = 1;	break;
				case 2: GspGlobal_ACCESS(p_AE_Params)->ZSDir.y = Pf1;GspGlobal_ACCESS(p_AE_Params)->ZSDirIsValidated = 1;	break;
				case 3: GspGlobal_ACCESS(p_AE_Params)->ZSDir.z = Pf1;GspGlobal_ACCESS(p_AE_Params)->ZSDirIsValidated = 1;	break;
			}

			break;
			
			break;
		case 6: // Rotate smooth
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_RS;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_RS;
			GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor = Pf1;
			break;
		case 7: // Remanance
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_REMANANCE;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_REMANANCE;
			GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor = Pf1;
			break;
		case 8: // Brightness
			GspGlobal_ACCESS(p_AE_Params)->Brighness = Pf1;
			break;
		case 9: // Contrast
			GspGlobal_ACCESS(p_AE_Params)->Contrast  = Pf1;
			break;
		case 10: // AA
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_BlurTest44;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_BlurTest44;
			GspGlobal_ACCESS(p_AE_Params)->AA22BlurMode  = (unsigned int)Pf1;
			break;
		case 11: // BW
			GspGlobal_ACCESS(p_AE_Params)->BWFactor = Pf1;
			if (!OnOff) GspGlobal_ACCESS(p_AE_Params)->BWFactor = 0.0f;
			break;
		case 12: // Color balance
			switch (P1)
			{
				case 0: GspGlobal_ACCESS(p_AE_Params)->CB_Intensity = Pf1;	break;
				case 1: GspGlobal_ACCESS(p_AE_Params)->CB_Spectre = Pf1;		break;
				case 2: GspGlobal_ACCESS(p_AE_Params)->CB_PhotoFilterEffect = Pf1;	break;
			}
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_CB_On;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_CB_On;
			break;
		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: GspGlobal_ACCESS(fFogCorrector)		 = Pf1;	break;
				case 1: GspGlobal_ACCESS(fFogCorrectorFar) 	 = Pf1;	break;
			}
			break;
		case 14: // Depth Blur nrear
			GspGlobal_ACCESS(Status) &= ~ GSP_Status_AE_DBN;
			if (OnOff) GspGlobal_ACCESS(Status) |= GSP_Status_AE_DBN;
			GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near = Pf1;
			break;
		case 15: // Fade
			Gsp_AE_FADE_PUSH(Pf1);
			break;
		case 16: // Water Ref
			switch (P1)
			{
				case 0:GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight= 0.0f * Pf1;	break;;
				case 1:GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane = Pf1;	break;
				case 2:GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffect= Pf1;	break;
				case 3:GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffectBalance= Pf1;	break;
				case 4:GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax= Pf1;	break;
				case 5:GspGlobal_ACCESS(p_AE_Params)->ulWR_ZOrder = (u_int)Pf1; break;
				case 6:GspGlobal_ACCESS(p_AE_Params)->fWR_ZDepth = (float)Pf1; break;
				
			}
			if (!OnOff) GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight = 0.0f;
			break;
			
		case 17: // Warp
			GspGlobal_ACCESS(p_AE_Params)->fWarpFactor = Pf1;
			if (!OnOff) GspGlobal_ACCESS(p_AE_Params)->fWarpFactor = 0.0f;
			break;
		case 18: // ZoomSmoothRemanance 
			if (!OnOff) GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceFactor = 0.0f;
			switch (P1)
			{
				case 0: GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceFactor = Pf1;	break;
				case 1: GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceLightDirection.x = Pf1;		break;
				case 2: GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceLightDirection.y = Pf1;		break;
				case 3: GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceLightDirection.z = Pf1;		break;
			}
			break;
		case 20: // BorderBrightness
			GspGlobal_ACCESS(p_AE_Params)->BorderBrithness = Pf1;
			break;
		case 22: // Glow
			GspGlobal_ACCESS(p_AE_Params)->Glow = Pf1;
			break;

			
	}
}
void Gsp_AE_Full_AA_Blur(ULONG BlurMode)
{

	Gsp_AE_BEGIN();

	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter(0 , 2 , 0 ); 
	Gsp_AE_Hell_Blitter( 0,0,BlurMode & 1,BlurMode >> 1,0,0,0xffffffff ,0x80808080); 
	Gsp_AE_Set_Hell_Blitter(2 , 0 , 0 ); 
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff ,0x80808080); 
	
	Gsp_AE_END();

}



void GSP_BufferBW( float Coef )
{
	u_int	SaveMAGH,CurZ,SaveZBP;
	Gsp_Rect stDST,stSRC;
	static u32 KJ = 0;
	Gsp_AE_BEGIN();
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter_B(0 , 2 , 0 , 0);
	Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff ,0x80808080); 

	// 2 Displace 2 bytes
	SaveZBP = GspGlobal_ACCESS(ZBP);
	SaveMAGH = GspGlobal_ACCESS(BppMode);
	BWSpecial = 1;
	GspGlobal_ACCESS(BppMode) = 2; // == 16b
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GspGlobal_ACCESS(ZBP) += GspGlobal_ACCESS(ZBP) - GspGlobal_ACCESS(FBP2);
	while (GspGlobal_ACCESS(ZBP)>SaveZBP)
	{
		GspGlobal_ACCESS(ZBP) --;
		Gsp_AE_Set_Hell_Blitter_B(2 , 2 , 0,0);  // Hide Alpha
		stDST.DX = 0;
		stDST.DY = 0;
		stDST.SX = 64;
		stDST.SY = 64;
		stSRC = stDST;
		Gsp_AE_ShiftRect(&stDST);
		Gsp_AE_ShiftRect(&stSRC);
		//stSRC.DX-= 8 << 4;
		stDST.DX+= 8 << 4;
		KJ ++;
		KJ &= 127;
		Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC,0x80808080);
	}
	
	
	Gsp_AE_SetGSRegisters();//*/
	GspGlobal_ACCESS(ZBP) = SaveZBP ;
	GspGlobal_ACCESS(BppMode) = SaveMAGH ;
	BWSpecial = 0;
	
	
	// TRansform in BW
	stDST.DX = 0;
	stDST.DY = 0;
	stDST.SX = GspGlobal_ACCESS(Xsize);
	stDST.SY = GspGlobal_ACCESS(Ysize);
	stSRC = stDST;
	GSP_AE_SetRegister(GSP_ALPHA_1 , (0x00L << 32L) | (2L << 6L) | (1L << 4L) | (2L << 2L) | (0L << 0L));
	Gsp_AE_Set_Hell_Blitter_B(3 , 2 , 0xff000000 ,0);
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0x80808080);//*/

	CurZ = Coef * 127.0f;
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| ((u_long)CurZ << 32L));
	Gsp_AE_Set_Hell_Blitter_B(2 , 0 , 0 , 0);
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0x80808080);
//*/	

	Gsp_AE_END();
}// end StoreTextureVIF1


extern float fASinPs2Dl(float Alpha);

void Gsp_RotateAround05(SOFT_tdst_AVertex *V,float Alpha)
{
	MATH_tdst_Vector VT;
	float CR,SR;
	SR = sin(Alpha);
	CR = cos(Alpha);
	VT.x = V->x - 0.5f;
	VT.y = V->y - 0.5f;
	VT.y *= 2.f;
	V->x = 0.5f + VT.x * CR + VT.y * SR;
	V->y = 0.5f + 0.5f * (VT.y * CR - VT.x * SR);
}
void Gsp_RotateAroundSC(SOFT_tdst_AVertex *V,float Alpha)
{
	MATH_tdst_Vector VT;
	float CR,SR;
	SR = sin(Alpha);
	CR = cos(Alpha);
	VT.x = V->x - (float)GspGlobal_ACCESS(Xsize) * 0.5f;
	VT.y = V->y - (float)GspGlobal_ACCESS(Ysize) * 0.5f;
	V->x = (float)GspGlobal_ACCESS(Xsize) * 0.5f + VT.x * CR + VT.y * SR;
	V->y = (float)GspGlobal_ACCESS(Ysize) * 0.5f + VT.y * CR - VT.x * SR;
}

inline u_long128 GSP_AE_UNPACKCOLOR(u_int Color)
{
	register u_long128 RetValue;
	asm __volatile__("
	pextlb	RetValue , $0 , Color
	pextlb	RetValue , $0 , RetValue	");
	return RetValue;
}

inline void GSP_AE_LoadConvertVecor(SOFT_tdst_AVertex *VS , SOFT_tdst_AVertex *VMin , SOFT_tdst_AVertex *VMax)
{
	asm __volatile__("
	lqc2	$vf21 , 0(VS)
	lqc2	$vf22 , 0(VMin)
	lqc2	$vf23 , 0(VMax)
	");
}

inline u_long128 GSP_AE_ConvertVector(SOFT_tdst_AVertex *VS)
{
	register u_long128 RetValue;
	asm __volatile__("
	lqc2	$vf20 , 0(VS)
	vmax.xy $vf20 , $vf20 , $vf22
	vmini.xy $vf20 , $vf20 , $vf23
	vadd.xyz $vf20 , $vf20 , $vf21
	VFTOI0.xyz $vf20 , $vf20
	qmfc2	RetValue , $vf20	");
	return RetValue;
}

void Gsp_AE_Hell_Blitter_V_UV_C(
	SOFT_tdst_AVertex *VSCREEN,
	SOFT_tdst_AVertex *VSCREEN_UV,
	SOFT_tdst_AVertex *VSCREEN_UV_Corrected,
	ULONG			  *pColors,
	ULONG 			 	ulNumberOfQuads)
{
	u_int Counter;
	MyIVectorFormat color , vertex __attribute__((aligned(16)));
	SOFT_tdst_AVertex stConvertVector __attribute__((aligned(16)));
	SOFT_tdst_AVertex stMinVector __attribute__((aligned(16)));
	SOFT_tdst_AVertex stMaxVector __attribute__((aligned(16)));
	u_long128 *p_BufferPointer128;
//	ULONG COLORS[4];
/*	COLORS[0] = COLORS[2] = C1;
	COLORS[1] = COLORS[3] = C2;*/
	
	stConvertVector . x = (float)(GspGlobal_ACCESS(BX0) << 4L);
	stConvertVector . y = (float)(GspGlobal_ACCESS(BY0) << 4L);
	stConvertVector . z = 0.0f;
	*(u_int *)&stConvertVector . w = 0;
	stMaxVector.x = 1024.0f * 16.0f;
	stMaxVector.y = 1024.0f * 16.0f;
	stMinVector.x = -512.0f * 16.0f;
	stMinVector.y = -512.0f * 16.0f;
	GSP_AE_LoadConvertVecor(&stConvertVector , &stMinVector , &stMaxVector);
	
	if (VSCREEN_UV)
	{
		float CX,CY;
		if (GspGlobal_ACCESS(Xsize) == 640)
		CX = (float)GspGlobal_ACCESS(Xsize) / 1024.0f;
		else
		CX = (float)GspGlobal_ACCESS(Xsize) / 512.0f;
		CY = (float)GspGlobal_ACCESS(Ysize) / 512.0f;

		Counter = 2 + (ulNumberOfQuads << 1);
		while(Counter--)
		{
			VSCREEN_UV_Corrected[Counter].z = 1.0f;
			VSCREEN_UV_Corrected[Counter].x = VSCREEN_UV[Counter].x * CX;
			VSCREEN_UV_Corrected[Counter].y = VSCREEN_UV[Counter].y * CY;
		}
		Gsp_AE_M_SetGifTag( (u_long)(2 + (ulNumberOfQuads << 1)) , 1 , 1 , 4 , 0 , 2 , 0x52);
	 	p_BufferPointer128 = (u_long128*)p_AE_CurrentBufferPointer;
		for (Counter = 0 ; Counter < 2 + (ulNumberOfQuads << 1) ; Counter++)
		{
			p_BufferPointer128[0] = *(u_long128*)VSCREEN_UV_Corrected;
			p_BufferPointer128[1] = GSP_AE_ConvertVector(VSCREEN);
			VSCREEN++;
			VSCREEN_UV_Corrected++;
			p_BufferPointer128 += 2;
	    }	
	} else
	{
		Gsp_AE_M_SetGifTag( (u_long)(2 + (ulNumberOfQuads << 1)) , 1 , 1 , 4 , 0 , 2 , 0x51);
	 	p_BufferPointer128 = (u_long128*)p_AE_CurrentBufferPointer;
		for (Counter = 0 ; Counter < 2 + (ulNumberOfQuads << 1) ; Counter++)
		{
			p_BufferPointer128[0] = GSP_AE_UNPACKCOLOR(*pColors);
			p_BufferPointer128[1] = GSP_AE_ConvertVector(VSCREEN);
			VSCREEN++;
			pColors++;
			p_BufferPointer128 += 2;
	    }	
	}
	
	p_AE_CurrentBufferPointer = (u_int)p_BufferPointer128;
	Gsp_AE_SendToGif();
	{
		u_int Stoppy;
		Stoppy = p_AE_CurrentBufferPointer - GspGlobal_ACCESS(p_CurrentBufferPointer);
		if (Stoppy > (RAM_BUF_SIZE - 0x1000))
		{
			Gsp_AE_END();
			Gsp_FIFO_STOP();
			Gsp_AE_BEGIN();
		}
	}

	
}
void Gsp_GetZFromPlane(SOFT_tdst_AVertex *V2D , SOFT_tdst_AVertex *PF , SOFT_tdst_AVertex *PN)
{
	SOFT_tdst_AVertex Dir2D;
	Dir2D.x = V2D->x - PF->x;
	Dir2D.y = V2D->y - PF->y;
	Dir2D.z = Dir2D.x * PN->x + Dir2D.y * PN->y;
	if (Dir2D.z <= 1.0f) V2D->z = 100000000.0f; /* Real Infinite */
	else
	V2D -> z = 1.0f / (Dir2D.z * PN->z);
}
extern void SOFT_SerialTranformVertices(SOFT_tdst_AVertex  *Src , SOFT_tdst_AVertex  *Dst , ULONG Number , MATH_tdst_Matrix *p_Matrix);
#define NumberOfSegs 40
#define NumberOfXSegs 5
void Gsp_AE_Display_WaterReflection(float ZPlane,float ZHeight,float ZDepth ,float TransparencyMax,float SinusEffect, float SinusEffectBalance, float DarkFactor , ULONG ulPrepare , SOFT_tdst_AVertex *pSaveXStarts)
{
//	static float Angle = 0.0f;
/*
	0 2
	1 3
*/
	static float Angle = 0.0f;
	SOFT_tdst_AVertex UV[NumberOfSegs << 1] __attribute__((aligned(16)));
	SOFT_tdst_AVertex Screen[NumberOfSegs << 1] __attribute__((aligned(16)));
	SOFT_tdst_AVertex *pLocalScreen;
	SOFT_tdst_AVertex *pLocalUV;
	
	ULONG			  Colors[NumberOfSegs << 1] __attribute__((aligned(16)));
	SOFT_tdst_AVertex VSCREEN_UV_Corrected[NumberOfSegs << 1] __attribute__((aligned(16)));
	SOFT_tdst_AVertex *pUV;
	float 			  AllW[NumberOfSegs << 1] __attribute__((aligned(16)));
	static MATH_tdst_Vector stLastPos;
	
	MATH_tdst_Vector PN,PF __attribute__((aligned(16)));
	MATH_tdst_Vector  VX,VY __attribute__((aligned(16)));
	ULONG Counter , CounterX;
	float CX,CY,CYFinal;
	float CyInc;
	float ZNear , ZZZZ;
	float OoZ,OoZInc;
	float CYoZ,CYoZInc;
	float Factor;
	float LAngle , speed;
	float XAngle;
	float FogADD,FogMul;
	float SinusTAbl[12];
	u_int SinusBase;
	float AX,AY,MX,MY ;
	float AU,AV,MU,MV ;
	float FA;
	
	MATH_SubVector(&PN  , &GDI_gpst_CurDD->st_Camera.st_Matrix.T , &stLastPos);
	speed = MATH_f_DotProduct(&PN , MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
	Angle += 0.05f - speed * 0.125f;
	stLastPos = GDI_gpst_CurDD->st_Camera.st_Matrix.T;
	
	if (GDI_gpst_CurDD->st_Camera.st_Matrix.T.z <= ZPlane) return;
	if (ZHeight == 0.0f) return;
	if (ZDepth == 0.0f) ZDepth = 10238.0f;
	
	ZHeight *= 1.15f;
	
	SinusEffect *= 2.0f;
	
	SinusBase = 12;
	LAngle = Angle;
	
	for (SinusBase = 0 ; SinusBase < 12 ;SinusBase++)
	{
		SinusTAbl[SinusBase] = fCos(LAngle) + fCos(LAngle * 0.33333f) * 0.5f;
		LAngle += 1.57f;//*/
	}
	
	DarkFactor = 0.5f;
	
	
	
	PN = *MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix);
	PN.z = 0.0f;
	MATH_NormalizeVector(&PN , &PN);
	PF = GDI_gpst_CurDD->st_Camera.st_Matrix.T;
	PF.z = ZPlane;
	
	Factor = GDI_gpst_CurDD->st_Camera.st_Matrix.T.z - ZPlane;
	Factor -= 20.0f;
	Factor /= 30.0f;
	Factor = fMax(0.0f , fMin(Factor , 1.0f));
	SinusEffect *= 1.0f - Factor;
	SinusEffect *= 2.0f;
	
	VY = (MATH_tdst_Vector){0.0f , 0.0f , 1.0f};
	MATH_CrossProduct(&VX , &VY , &PN);
	MATH_CrossProduct(&VY , &VX  , MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
	MATH_ScaleVector(&PN , &PN , ZDepth);

	CX = ZDepth * 1.4f * fTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision * 0.5f);
	CYFinal = CY = ZHeight * ZDepth * fTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision * 0.5f);

	Factor = (GDI_gpst_CurDD->st_Camera.st_Matrix.T.z - ZPlane) / (GDI_gpst_CurDD->st_Camera.st_Matrix.T.z - CY);
	ZNear = 1.0f;
	ZZZZ = 4.0f;//;fMax(1.0f,ZNear);
//	ZDepth = ZNear; // Linear
	if (ulPrepare)
	{
		GSP_AE_SetRegister_TEST_1((1L << 14L)| (1L << 16L) | (2L << 17L));
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter(3 , 0 ,0xffff00); // Z -> Current
		pUV = NULL;
		GSP_AE_SetRegister(GSP_PRMODE 	, (0L<<8L) | (0L<<5L)  | (0L<<4L) /* No texture */| (1L<<6L) | (1L<<3L));
		Gsp_AE_SetGSRegisters();
	} 
	else
	{
		u_int DarkF , FOne;
		DarkF = (u_int)(DarkFactor * 255.0f);
		DarkF |= (DarkF << 8) | (DarkF << 16);
		*(float *)&FOne = 1.0f;
		GSP_AE_SetRegister(GSP_FOGCOL , GspGlobal_ACCESS(FogColor));
		GSP_AE_SetRegister_TEST_1( (1L << 14L));
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_PALPHA_I);
		GSP_AE_SetRegister(GSP_RGBAQ 	, (u_long)DarkF | ((u_long)FOne << 32));
		Gsp_AE_Set_Hell_Blitter(2 , 0 , 0xff000000); // Z -> Current
		pUV = UV;
		GSP_AE_SetRegister(GSP_PRMODE 	, (0L<<8L) | (0L<<5L)  | (1L<<4L) /* No texture */| (1L<<6L) | (1L<<3L));
		Gsp_AE_SetGSRegisters();
	}
	
	AX = GspGlobal_ACCESS(Xsize) * 0.5f;
	AY = GspGlobal_ACCESS(Ysize) * 0.5f;
	MX = 0.5f * GspGlobal_ACCESS(Xsize) * GspGlobal_ACCESS(fCurrentFocale);
	MY = 0.5f * GspGlobal_ACCESS(YCorrectionCoef) * GspGlobal_ACCESS(Ysize) * GspGlobal_ACCESS(fCurrentFocale);
	AU = GspGlobal_ACCESS(stHSP).XPos;
	MU = (float)(GspGlobal_ACCESS(stHSP).XSize >> 1) / (float)GspGlobal_ACCESS(Xsize);
	AV = GspGlobal_ACCESS(stHSP).YPos;
	MV = (float)(GspGlobal_ACCESS(stHSP).YSize >> 1) / (float)GspGlobal_ACCESS(Ysize);
	FA = -(GDI_gpst_CurDD->st_Camera.st_Matrix.T.z - ZPlane);
	FA = 1.0f / FA;
	FA *= 16.0f * GspGlobal_ACCESS(ZFactor);
	FA /= (float)GIGSCALE0;
	{
		float CXInc,CXBase;
		CXBase = -CX;
		CXInc = 2.0f * CX / NumberOfXSegs;
		CounterX = NumberOfXSegs;
		while (CounterX--)
		{
			MATH_tdst_Vector stCX __attribute__((aligned(16)));
			MATH_tdst_Vector stCXP1 __attribute__((aligned(16)));
			OoZInc = ((1.0f / ZZZZ) - (1.0f / ZNear)) / (NumberOfSegs - 1);
			OoZ = (1.0f / ZNear);
			CYoZInc = ((CYFinal / ZZZZ) - (0.0f / ZNear)) / (NumberOfSegs - 1);
			CyInc = CYFinal / ((float) NumberOfSegs - 1.0f);
			CYoZ = 0.0f;
			CY = 0.0f;
			Counter = NumberOfSegs;
			Factor = (1.0f / CYFinal);
			MATH_AddScaleVector(&stCX , &PN , &VX , CXBase);
			MATH_AddScaleVector(&stCXP1 , &PN , &VX , CXBase + CXInc);
			MATH_SetNormVector(&stCX , &stCX , ZDepth);
			MATH_SetNormVector(&stCXP1 , &stCXP1 , ZDepth);
			MATH_AddVector_ALIGNED(&stCX   , &stCX   , &PF );
			MATH_AddVector_ALIGNED(&stCXP1 , &stCXP1 , &PF );
			pLocalScreen = Screen + (Counter << 1) - 1;
			while (Counter--)
			{
				CYoZ = CY / OoZ;
				MATH_AddScaleVector_ALIGNED((MATH_tdst_Vector *)pLocalScreen , &stCX , &VY , CYoZ);
				AllW[(Counter << 1) + 0] = pLocalScreen -> z;
				pLocalScreen--;
				MATH_AddScaleVector_ALIGNED((MATH_tdst_Vector *)pLocalScreen , &stCXP1 , &VY , CYoZ);
				AllW[(Counter << 1) + 1] = pLocalScreen -> z;
				pLocalScreen--;
				CY += CYoZInc ;
				OoZ += OoZInc ;
			}
			
			
			FogADD = -GspGlobal_ACCESS(FogZNear);
		//	FogADD *= 0.25;
			FogMul = 255.0f / (GspGlobal_ACCESS(FogZFar) - GspGlobal_ACCESS(FogZNear));
			if (1)
			{
				SOFT_SerialTranformVertices(Screen , Screen , NumberOfSegs << 1 , &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
				Counter = NumberOfSegs << 1;
				pLocalScreen = Screen + Counter - 1;
				pLocalUV = UV + Counter - 1;
				while (Counter--)
				{
					OoZ = 1.0f /pLocalScreen-> z;
					pLocalScreen-> x = AX + MX * OoZ * pLocalScreen-> x;
					pLocalScreen-> y = AY + MY * OoZ * pLocalScreen-> y;
					/* Then compute projected Z's */
					pLocalScreen-> w = 255.0f;//255.0f - fMin(fMax((Screen[Counter] . z + FogADD) * FogMul, 0.0f), 255.0f) ;// FOG 
					pLocalScreen-> z = OoZ * (AllW[Counter] - GDI_gpst_CurDD->st_Camera.st_Matrix.T.z) * FA;
					pLocalUV->x = AU + MU * pLocalScreen-> x;
					pLocalUV->y = AV + MV * pLocalScreen-> y;
					pLocalUV->z = pLocalUV->w = 1.f;
					pLocalUV--;
					pLocalScreen--;
				}
			}
			/* compute sinus */
			if ((SinusEffect != 0.0f) && (!ulPrepare))
			{
				float Sinus,SinusInc , Local;
				float AlphaBase,AlphaInc;
				Sinus = SinusEffectBalance;
				SinusInc = ((1.0f - SinusEffectBalance) - Sinus) / (float)(NumberOfSegs - 1);
				Sinus *= SinusEffect;
				SinusInc *= SinusEffect;
				Counter = NumberOfSegs;
				OoZ = 1.0f / (NumberOfSegs << 1);
				AlphaBase = 180.0f;
				AlphaInc = -AlphaBase / (float)(NumberOfSegs - 1);
				SinusBase = 0;
				pLocalUV = UV + (Counter << 1) - 1;
				while (Counter--)
				{
					Local = SinusTAbl[SinusBase++];
					if (SinusBase == 12) SinusBase = 0;
					pLocalUV->y += Sinus * Local;
					pLocalUV--;
					pLocalUV->y += Sinus * Local;
					pLocalUV--;
					AlphaBase += AlphaInc;
					Sinus += SinusInc;
				}
			} else
			{
				float AlphaBase,AlphaInc;
				Counter = NumberOfSegs;
				AlphaBase = 255.0f;
				AlphaInc = 0.0;
				while (Counter--)
				{
//					Colors[(Counter << 1) + 1] = Colors[Counter << 1] = ((lFloatToLong(fMax(fMin(127.0f , TransparencyMax * AlphaBase),0.0f))) << 24) | 0x808080;
					Colors[(Counter << 1) + 1] = Colors[Counter << 1] = 0x7f808080;
					AlphaBase += AlphaInc;
				}
			} 
			Counter = NumberOfSegs << 1;
			OoZ = 1.0f /  (float)GspGlobal_ACCESS(Xsize);
			OoZInc = 1.0f /  (float)GspGlobal_ACCESS(Ysize);
			pLocalScreen = Screen + Counter - 1;
			pLocalUV = UV + Counter - 1;
			while (Counter--)
			{
				pLocalUV->y += 3;
				pLocalUV->x *= OoZ;
				pLocalUV->y *= OoZInc;
				pLocalScreen-> y -= 2;
				pLocalScreen-> x *= 16;
				pLocalScreen-> y *= 16;
				pLocalScreen--;
				pLocalUV--;
			}
			
			if (pSaveXStarts)
			{
				*(pSaveXStarts++) = Screen[((NumberOfSegs - 1) << 1)];
				*(pSaveXStarts++) = Screen[((NumberOfSegs - 1) << 1) + 1];
			} 
			
			Gsp_AE_Hell_Blitter_V_UV_C(Screen , pUV , VSCREEN_UV_Corrected , Colors , NumberOfSegs - 1);
			
			CXBase += CXInc;
			
		}
	}
	
}

float fFadePushCMD = 0.0f;

void Gsp_AE_FADE_PUSH_CMD(float TimeInSeconds)
{
	if (TimeInSeconds <= 0.0f) return;
	GspGlobal_ACCESS(ulFade_Time) = VBlankCounter;
	GspGlobal_ACCESS(ulFade_Alpha) = 1.0f;
	GspGlobal_ACCESS(ulFade_Inc) = 1.0f / (TimeInSeconds * 50.0f);
	
	if (GspGlobal_ACCESS(ulFade_Counter)) GspGlobal_ACCESS(ulFade_Counter) = 0xC0000001;
	else	GspGlobal_ACCESS(ulFade_Counter) = 0x80000001;
}
void Gsp_AE_FADE_PUSH(float TimeInSeconds)
{
	fFadePushCMD = TimeInSeconds;
}

void Gsp_ClearZBuffer()
{
}
extern u_int GSP_VRamLoadText_FROM_RAM(u_int VramPtr , u_int DBW , u_int *p_FirstPixel , u_int W , u_int H ,u_int DX,u_int DY,  u_int BPP , u_int TagAddress);

void GSP_PatternSet(ULONG *p_Pattern , u_int SX ,  u_int SY)
{
	u_int pCBPS,TagAddress ;
	u_int VPTR ;
	u_int Remorf;
	u_long TEX0_REG;
	Gsp_Rect stRECT;


	VPTR = GspGlobal_ACCESS(TBP);
	VPTR <<= (11 - 6);
	
	TagAddress = ((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer));
	TagAddress = GSP_VRamLoadText_FROM_RAM(VPTR , GspGlobal_ACCESS(Xsize)>>6 , (u_int *)((u_int)p_Pattern & 0x0fffffff) , SX , SY ,0,0, 32 , TagAddress);
	pCBPS = (((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer) - (u_int)GspGlobal_ACCESS(BX_LastCntPtr)) >> 4) - 1;
	GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , pCBPS , 0 , GspGlobal_ACCESS(BX_LastCntPtr));
	GspGlobal_ACCESS(BX_LastCntPtr) = (GSP_DMA_Source_Chain_TAG *)TagAddress;
	GspGlobal_ACCESS(p_CurrentBufferPointer) = TagAddress + 16;
	Gsp_AE_BEGIN();	
	TEX0_REG = 
		((u_long)VPTR)| 
		(((u_long)1L)<<14L)|
		(((u_long)0L)<<20L)|
		(GSP_GetPowerOf2( SX ) << 26L)|
		(GSP_GetPowerOf2( SY )<< 30L)|
		(1L<<34L)|
		(1L<<35L)| // DECAL
		0L;
	GSP_AE_SetRegister(GSP_ZBUF_1 , GspGlobal_ACCESS(CUR_REG_ZBUF) | (1L << 32L));
	GSP_AE_SetRegister(GSP_FRAME_1 , /*FBP*/((u_long)Get_FBF(0 , 0) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/((u_long)0 << 32L));
	GSP_AE_SetRegister(GSP_TEXA 	, (0L<<0L)/*Ta0*/ | (0L<<15L) | (0x7fL<<32L)/*Ta1*/);
	GSP_AE_SetRegister(GSP_TEX0_1 	, TEX0_REG);
	GSP_AE_SetRegister(GSP_TEX1_1 	, (0L<<0L) | (0L<<2L) | (0L<<5L) | (0L<<6L) | (0L<<9L) | (0L<<19L) | (0x68L<<32L));
	GSP_AE_SetRegister(GSP_CLAMP_1 	, (0L<<0L) | (0L<<2L) | (0L<<4L) | (0L<<14L) | (0L<<24L) | (0L<<34L)); // Tile
	GSP_AE_SetRegister(GSP_PRMODE 	, (1L<<8L) | (1L<<4L) /* No texture */| (1L<<6L) | (1L<<3L));
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_SetGSRegisters();
	{
		MyIVectorFormat vertex1, vertex2;
		MyIVectorFormat vertexUV1,vertexUV2;
		vertex1 . x = vertex1 . y = vertex1 . z = vertex1 . w = 0;
		vertex2 = vertexUV1 = vertexUV2 = vertex1;
		vertex1.x = vertex2.x = GspGlobal_ACCESS(BX0) << 4;
		vertex1.y = vertex2.y = GspGlobal_ACCESS(BY0) << 4;
		vertex2 . x += GspGlobal_ACCESS(Xsize) << 4;
		vertex2 . y += GspGlobal_ACCESS(Ysize) << 4;
		vertexUV2 . x = GspGlobal_ACCESS(Xsize) << 4;
		vertexUV2 . y = GspGlobal_ACCESS(Ysize) << 4;
		vertexUV1 . x += 16;
		vertexUV1 . y += 16;
		vertexUV2 . x += 16;
		vertexUV2 . y += 16;

		Gsp_AE_M_SetGifTag( 1 , 1 , 1 , 6 , 0 , 4 , 0x5353);
		GSP_AE_SetPrim(vertexUV1);
		GSP_AE_SetPrim(vertex1);
		GSP_AE_SetPrim(vertexUV2);
		GSP_AE_SetPrim(vertex2);
		Gsp_AE_SendToGif();
	}
	
	Gsp_AE_END();
//	Gsp_FIFO_STOP();
}

ULONG ulPAternMatrix[16*16] ONLY_PSX2_ALIGNED(64);
void Gsp_BE_PrepareHidenBuffer()
{
	static ULONG bFirstCounter = 100;
	ULONG XC,YC;
	
	if (!(GspGlobal_ACCESS(Status) & GSP_Status_AE_WATEREFFECT)) return;

	if (bFirstCounter == 100)
	{
		GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffect = 2.0f;
		GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffectBalance = 0.0f;
	}
	


	if (bFirstCounter)
	{
		bFirstCounter=0;
	}

	{
		ULONG *PATERNPTR , ulBackColor;
		 ulBackColor = GspGlobal_ACCESS(ulBackColor) & 0xffffff;
		PATERNPTR = ulPAternMatrix;
		YC = 16;
		while (YC--)
		{
			XC = 16;
			while (XC--)
			{
				if (XC & YC & 1)
					*PATERNPTR = 0x80000000 | ulBackColor; // 04 67 59 30 00
				else
					*PATERNPTR = ulBackColor;
				PATERNPTR++;
			}
		}
		GSP_PatternSet(ulPAternMatrix ,16,  16);
	}
}
void GSP_EA_SaveZBuffer()
{
	Gsp_Rect stDST,stSRC;
	u_int Remorf , YSize;
	Remorf = GspGlobal_ACCESS(Morfling);
	Remorf &= ~31;
	YSize = GspGlobal_ACCESS(Ysize) + 31;
	YSize &= ~31;
	YSize -= (Remorf << 1);

	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter_B(2 | 0x30, // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						2 | 0x30 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize;
	stDST = stSRC;
	stDST.SY >>= 1;
	stSRC.DX ++;
	
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
					
	stSRC = stDST ;
	stDST . DY = Remorf+(YSize>>1);
	stSRC.DY ++;
	stSRC.DX ++;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);

}
void GSP_EA_RestoreZBuffer()
{
	Gsp_Rect stDST,stSRC;
	u_int Remorf , YSize;
	Remorf = GspGlobal_ACCESS(Morfling);
	Remorf &= ~31;
	YSize = GspGlobal_ACCESS(Ysize) + 31;
	YSize &= ~31;
	YSize -= (Remorf << 1);
	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter_B(2 | 0x30 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						2 | 0x30, // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf+(YSize>>1);
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize >> 1;
	stDST.DX = 0;
	stDST.DY = Remorf;
	stDST.SX = GspGlobal_ACCESS(Xsize);
	stDST.SY = YSize;
	stSRC.DY ++;
	stSRC.DX ++;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);

}


#define WATERBLUR	
void GSP_EA_HiddenBufferUse()
{
	Gsp_Rect stDST,stSRC;
	SOFT_tdst_AVertex SaveXStarts[NumberOfXSegs * 2] __attribute__((aligned(16)));
	
	if (!(GspGlobal_ACCESS(Status) & GSP_Status_AE_WATEREFFECT)) return;

	Gsp_AE_BEGIN();

/* Erase Alpha Buffer 0 ========================================================================================================================*/
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( (1L << 14L));
	stDST.DX = 0;
	stDST.DY = 0;
	stDST.SX = GspGlobal_ACCESS(Xsize);
	stDST.SY = GspGlobal_ACCESS(Ysize);
	Gsp_AE_Set_Hell_Blitter_B(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0xffffff ,
						0 );
	Gsp_AE_CaseBlitter(&stDST,&stDST ,	0x00000000);
/* Prepare Alpha Buffer ========================================================================================================================*/
	Gsp_AE_Display_WaterReflection(GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane,
									GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight,
									GspGlobal_ACCESS(p_AE_Params)->fWR_ZDepth,
									1.0f - GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax,
									0.0f,
									GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffectBalance,
									1.0f - GspGlobal_ACCESS(p_AE_Params)->fWR_DarkFactor,
									1,SaveXStarts);
									
									
/* De-interlace ========================================================================================================================*/
	GSP_EA_SaveZBuffer();
/*	Gsp_AE_END();
	return;//*/
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter_B(0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0 ,
							0 );
	stDST.DX = GspGlobal_ACCESS(stHSP).XPos;
	stDST.DY = GspGlobal_ACCESS(stHSP).YPos;
	stDST.SX = GspGlobal_ACCESS(stHSP).XSize;
	stDST.SY = GspGlobal_ACCESS(stHSP).YSize;
	stSRC = stDST;
	stDST.SX >>= 1;
	stDST.SY >>= 1;
	stSRC . DX ++;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x808080);

	/* Set Texture Alpha = OPAQUE */
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	Gsp_AE_Set_Hell_Blitter_B(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0xffffff ,
							0 );

	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
/*	Gsp_AE_END();
	return;//*/

/* Display water REflection */
/* BLUR BEGIN ************************************************************************************************************** */
#ifdef WATERBLUR
	if ((MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix)->z < 0.075f) &&
	    (MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix)->z > -0.9f) &&
	    (MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix)->z < 0.0f))
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GSP_AE_SetRegister_TEST_1( 0L);
		Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0xff000000 ,
							0 );
		GSP_AE_SetRegister(GSP_PRMODE 	, (0L<<8L) | (0L<<5L)  | (1L<<4L) /* No texture */| (1L<<6L) | (1L<<3L));
		Gsp_AE_SetGSRegisters();
		{
			u_int Color[4] __attribute__((aligned(16)));
			u_int Counter , CounterJL;
			SOFT_tdst_AVertex stSCREEN[4] __attribute__((aligned(16)));
			SOFT_tdst_AVertex stUV[4] __attribute__((aligned(16)));
			SOFT_tdst_AVertex stUV_C[4] __attribute__((aligned(16)));
			Counter = NumberOfXSegs;
			Color[0] = Color[1] = Color[2] = Color[3] = 0x808080;
			for (Counter = 0 ; Counter < NumberOfXSegs ; Counter ++)
			{
				MATH_ScaleEqualVector(&SaveXStarts[Counter << 1] , 0.5f);
				MATH_ScaleEqualVector(&SaveXStarts[(Counter << 1) + 1] , 0.5f);
				SaveXStarts[Counter << 1].z = SaveXStarts[Counter << 1].w = 0.0f;
				SaveXStarts[(Counter << 1) + 1].z = SaveXStarts[(Counter << 1) + 1].w = 0.0f;
				stSCREEN[0] = SaveXStarts[Counter << 1];
				stSCREEN[1] = SaveXStarts[(Counter << 1) + 1];
				stSCREEN[2] = SaveXStarts[Counter << 1];
				stSCREEN[3] = SaveXStarts[(Counter << 1) + 1];
				stSCREEN[0] . y += 16.0f;
				stSCREEN[1] . y += 16.0f;
				
				CounterJL = 4;
				while (CounterJL--)
				{
					stUV[CounterJL] . x = (stSCREEN[CounterJL] . x) / (16.0f * (float)GspGlobal_ACCESS(Xsize));
					stUV[CounterJL] . y = (stSCREEN[CounterJL] . y + 48.0f) / (16.0f * (float)GspGlobal_ACCESS(Ysize));
					stUV[CounterJL] . z = stUV[CounterJL] . w = 1.0f;
				}
				stSCREEN[2] . y = 0.0f;
				stSCREEN[3] . y = 0.0f;
				Gsp_AE_Hell_Blitter_V_UV_C(stSCREEN , stUV , stUV_C , Color , 1 );
			}
		}
		
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GSP_AE_SetRegister_TEST_1( 0L);
		Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0xff000000 ,
							1 );
		
		{
			u_int BlurCounter;	
			
			static u_int FuckX = 9;
			static u_int FuckY = 1;
			stDST.DX = 0;
			stDST.DY = 0;
			stSRC.SX = stDST.SX = (GspGlobal_ACCESS(Xsize) >> 1) << 4;
			stSRC.SY = stDST.SY = (GspGlobal_ACCESS(Ysize) >> 1) << 4;
			for (BlurCounter = 0 ; BlurCounter < 4 ; BlurCounter ++)
			{
				stSRC.DX = stDST.DX + FuckX;
				stSRC.DY = stDST.DY + FuckY;
				if (BlurCounter & 1)
					stDST.DX = 0;
				else
					stDST.DX = (GspGlobal_ACCESS(Xsize) >> 1) << 4;
				Gsp_AE_CaseBlitter_FRAC(&stDST,&stSRC ,	0x00808080);
			}
		}//*/
	} 
#endif	
	//*/

/* BLUR END **************************************************************************************************************** */

/*	Gsp_AE_END();
	return;//*/

	Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0 ,
							1 );
	
	
	Gsp_AE_Display_WaterReflection(	GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane,
									GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight,
									GspGlobal_ACCESS(p_AE_Params)->fWR_ZDepth,
									1.0f - GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax,
									GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffect,
									GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffectBalance,
									1.0f - GspGlobal_ACCESS(p_AE_Params)->fWR_DarkFactor,
									0,NULL);
	

/* Restore frame buffer========================================================================================================================*/
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( (1L << 14L) | (1L << 15L));
	
	Gsp_AE_Set_Hell_Blitter_B(0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0xFF000000 ,
							0 );
	stDST.DX = GspGlobal_ACCESS(stHSP).XPos;
	stDST.DY = GspGlobal_ACCESS(stHSP).YPos;
	stDST.SX = GspGlobal_ACCESS(stHSP).XSize;
	stDST.SY = GspGlobal_ACCESS(stHSP).YSize;
	stSRC = stDST;
	stDST.DY --;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
	stDST.DX -= 2;
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| (0x40L << 32L));
	Gsp_AE_SetGSRegisters();
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);//*/
	
	
/* DBUG //////////*/
/*	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister(GSP_TEST_1	, 0L);
	Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0xFF000000 ,
							0 );
	stDST.DX = 0;
	stDST.DY = 0;
	stDST.SX = GspGlobal_ACCESS(Xsize) >> 1;
	stDST.SY = GspGlobal_ACCESS(Ysize) >> 1;
	stSRC = stDST;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);

/* DBUG //////////*/
	

	GSP_EA_RestoreZBuffer();
	GspGlobal_ACCESS(CUR_REG_FRAME) &= 0x00000000ffffffffL;
	GSP_AE_SetRegister(GSP_FRAME_1 , GspGlobal_ACCESS(CUR_REG_FRAME));
	Gsp_AE_SetGSRegisters();
	Gsp_AE_END();
}

void GSP_AE_OldZListHook(int ZlistNum , float Z , float fHeight )
{
	GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight = fHeight;
	GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane = Z;
	if (!(GspGlobal_ACCESS(Status) & GSP_Status_AE_WATEREFFECT)) return;
	Gsp_FIFO_STOP();
	GSP_EA_HiddenBufferUse();
	Gsp_FIFO_STOP();
	GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_WATEREFFECT;
	GspGlobal_ACCESS(bSRS_FromScratch) = 1;
}//*/
extern u_int *RamZBuffer;
void GSP_AE_GetZBuffer1_SynchroDma1()
{
	Gsp_Rect stDST,stSRC;
	u_int Remorf , YSize;

	GspGlobal_ACCESS(Status) &= ~GSP_Status_ZBufferBackValid;
	if (!(GspGlobal_ACCESS(Status) & GSP_Status_GetZBufferBack)) return;
	GspGlobal_ACCESS(Status) |= GSP_Status_ZBufferBackValid;
	GspGlobal_ACCESS(Status) &= ~GSP_Status_GetZBufferBack;
	
	
	Remorf = GspGlobal_ACCESS(Morfling);
	Remorf &= ~31;
	YSize = GspGlobal_ACCESS(Ysize) + 31;
	YSize &= ~31;
	YSize -= (Remorf << 1);

	Gsp_AE_BEGIN();	
	
	GSP_EA_SaveZBuffer();

	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter_B(2 | 0x30, // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						2  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize >> 1;
	stDST = stSRC;
	stDST.SX >>= 1;
	stSRC.DY = Remorf + (YSize >> 1);
	stSRC.DX ++;
	stSRC.DY ++;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
	
	GSP_AE_SetRegister(GSP_BITBLTBUF 	, 
			(/*SBP*/((u_long)Get_FBF(2 , 1)) << 0L)|
			(/*SBW*/((u_long)(GspGlobal_ACCESS(Xsize) >> 6L)) << 16L)|
			(/*SPSM*/(0L) << 24L));

	GSP_AE_SetRegister(GSP_TRXPOS 	, 0L);
	GSP_AE_SetRegister(GSP_TRXREG 	, ((u_long)GspGlobal_ACCESS(Xsize) >> 1L) | (((u_long)YSize >> 1L) << 32L));
	GSP_AE_SetRegister(GSP_FINISH 	, 123L);
	GSP_AE_SetRegister(GSP_TRXDIR 	, 1L);
	// set the FINISH event
	DPUT_GS_CSR( GS_CSR_FINISH_M );
	
	GspGlobal_ACCESS(GSP_BX_VAR).pCurrentCpuBuffer->BX_BackSize = (GspGlobal_ACCESS(Xsize) * YSize) >> 4;
	GspGlobal_ACCESS(GSP_BX_VAR).pCurrentCpuBuffer->BX_BackAddress = (u_int)RamZBuffer;
	
	Gsp_AE_SetGSRegisters();
	Gsp_AE_END();
	Gsp_FIFO_STOP();
	
	Gsp_AE_BEGIN();	
	GSP_EA_RestoreZBuffer();
	Gsp_AE_END();
}

void GSP_EA_ShowTheOne(u_int Additionalmode , float fintensity , float fMotionBlurFactor)
{
	Gsp_Rect stDST,stSRC;
	u_int Remorf , YSize , Color;
	Remorf = GspGlobal_ACCESS(Morfling);
	Remorf &= ~31;
	YSize = GspGlobal_ACCESS(Ysize) + 31;
	YSize &= ~31;
	YSize -= (Remorf << 1);
	
	if (fintensity == 0.0f) return;
	Color = (u_int) (fintensity * 255.0f);
	if (Additionalmode)
	{
		Color |= (Color << 8) | (Color << 16);
		Color |= 0x80000000;
	} else
	{
		Color <<= 24;
		Color |= 0x808080;
	}

	
	Gsp_AE_BEGIN();	
/* Erase high bits of alpha buffer  */ 
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter_B(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0x7fFFFFFF ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize;
	stDST = stSRC;
	stSRC.DY ++;
	stSRC.DX ++;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0);

/*	Gsp_AE_END();	
	return;*/

/* Transform 0x01-- to 0x81  */ 
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( (1L << 0L) | (4L << 1L) | (0x3L << 4L));
	GSP_AE_SetRegister(GSP_FBA_1 	, 1L);
	Gsp_AE_Set_Hell_Blitter_B(0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0x00FFFFFF ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize;
	stDST = stSRC;
	stSRC.DY ++;
	stSRC.DX ++;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80000000);
	
/* Erase low bits of alpha buffer  */ 
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_FBA_1 	, 0L);
	Gsp_AE_Set_Hell_Blitter_B(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0x80ffffff ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize;
	stDST = stSRC;
	stSRC.DY ++;
	stSRC.DX ++;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0);


/* Dilate Alpha = 0x81 */	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( (1L << 14L) | (0L << 15L));
	GSP_AE_SetRegister(GSP_FBA_1 	, 0L);
	Gsp_AE_Set_Hell_Blitter_B(0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize; 
	stDST = stSRC;
	stSRC.DX++;
	stSRC.DY++;
	stDST.DX--;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
	stDST.DX--;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
	stDST.DX+=2;
	stDST.DY--;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
	stDST.DY--;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
//*/	
	
	
/* Copy Frame buffer to ZBuffer / 4  */ 
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	GSP_AE_SetRegister(GSP_FBA_1 	, 0L);
	Gsp_AE_Set_Hell_Blitter_B(0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						2  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize; 
	stDST = stSRC;
	stSRC.DX++;
	stSRC.DY++;
	stDST.SX >>= 2;
	stDST.SY >>= 2;
	Gsp_AE_CaseBlitter(&stDST,&stSRC , 0x80808080);
	
	
/* Dilate Alpha = 0x81 */	
	{
		u_int Dilatation;
		Dilatation = 1;
		while (Dilatation--)
		{
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
			GSP_AE_SetRegister_TEST_1( (1L << 14L) | (0L << 15L));
			GSP_AE_SetRegister(GSP_FBA_1 	, 0L);
			Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
								2  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
								0 ,
								0 );
			stSRC.DX = 0;
			stSRC.DY = Remorf;
			stSRC.SX = GspGlobal_ACCESS(Xsize) >> 2;
			stSRC.SY = YSize >> 2;  
			stDST = stSRC;
			stSRC.DX++;
			stSRC.DY++;
			stDST.DX--;
			Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
			stDST.DX--;
			Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
			stDST.DX+=2;
			stDST.DY--;
			Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
			stDST.DY--;
			Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
		//*/	
		}
	}

//*/	

/* Blur result */	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						2  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize;
	stDST = stSRC;
	stDST.SX >>= 2;
	stDST.SY >>= 2;
	stSRC.SX >>= 2;
	stSRC.SY >>= 2;
	stDST.DX += stDST.SX;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
	Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						2  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						1 );
	{
		u_int SSS;
		Gsp_Rect stSRCSave;
		SSS = 5;
		while (SSS--)
		{
			stSRC.DX++;
			stSRC.DY++;
			Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80808080);
			stSRC.DX--;
			stSRC.DY--;
			Gsp_AE_CaseBlitter(&stSRC,&stDST ,	0x80808080);
		}
	}
//*/
/* Transform >= 0x80 -> 0x7f */
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_PALPHA);
	GSP_AE_SetRegister_TEST_1( (1L << 14L) | (1L << 15L));
	Gsp_AE_Set_Hell_Blitter_B(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							2  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0x00ffffff ,
							0 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize;
	stDST = stSRC;
	stDST.SX >>= 2;
	stDST.SY >>= 2;
	stSRC.SX >>= 2;
	stSRC.SY >>= 2;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x7f000000);
//*/	
/* Apply to Frame buffer */	
	if (Additionalmode)
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, (0x00L << 32L) | (1L << 6L) | (0L << 4L) | (2L << 2L) | (0L << 0L));
		GSP_AE_SetRegister_TEST_1( 0L);
	}
	else
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHA_I);
		GSP_AE_SetRegister_TEST_1( 0L);
	}
	Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
						0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
						0 ,
						1 );
	stSRC.DX = 0;
	stSRC.DY = Remorf;
	stSRC.SX = GspGlobal_ACCESS(Xsize);
	stSRC.SY = YSize;
	stDST = stSRC;
	stSRC.SX >>= 2;
	stSRC.SY >>= 2;
	stSRC.DX++;
	stSRC.DY++;
	stDST.DX+= 8;
	stDST.DY+= 8;
	Gsp_AE_CaseBlitter(&stDST,&stSRC ,	Color);
//*/
	
/* Motion blur factor */	
	if (fMotionBlurFactor != 0.0f)
	{
		Color = (u_int)(fMotionBlurFactor * 255.0f);
		Color <<= 24;
		Color |= 0x808080;
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GSP_AE_SetRegister_TEST_1( 0L);
		Gsp_AE_Set_Hell_Blitter_B(2 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0x00ffffff ,
							1 );
		Gsp_AE_CaseBlitter(&stDST,&stSRC ,	Color);
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_PALPHA_I);
		GSP_AE_SetRegister_TEST_1( 0L);
		
		Gsp_AE_Set_Hell_Blitter_B(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
							0  , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
							0 ,
							0 );
		stSRC.DX = 0;
		stSRC.DY = Remorf;
		stSRC.SX = GspGlobal_ACCESS(Xsize);
		stSRC.SY = YSize;
		stDST = stSRC;
		stSRC.DX++;
		stSRC.DY++;
		Gsp_AE_CaseBlitter(&stDST,&stSRC ,	0x80ffffff);
	}
	Gsp_AE_END();
//Gsp_AE_ShowAlphaBuffer();
}
void GSP_Warp(u_int Mode , float Factor)
{
	if (GspGlobal_ACCESS(p_AE_Params)->fWarpFactor * GspGlobal_ACCESS(p_AE_Params)->fWarpFactor < 0.00009f) 
	{
		GspGlobal_ACCESS(p_AE_Params)->fWarpFactor = 0.0f;
		return;
	}

	Gsp_AE_BEGIN();	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter(0 , 2 , 0 );
	Gsp_AE_Hell_Blitter(0 , 0 , 0 , 0 , 0 , 0 , 0xffffffff , 0x80808080); 
	Gsp_AE_Set_Hell_Blitter_B(2 , 0 , 0 , 1 );
	GSP_AE_SetRegister(GSP_PRMODE 	, (0L<<8L) | (0L<<5L)  | (1L<<4L)| (0L<<6L) | (1L<<3L));
	Gsp_AE_SetGSRegisters();
	{
		u_int CounterX , CounterY;
		float XJump,YJump;
#define WARP_DISC_X 30
#define WARP_DISC_Y 20
		XJump = GspGlobal_ACCESS(Xsize) * 16.0f * 1.0f / (float)(WARP_DISC_X - 1);
		YJump = GspGlobal_ACCESS(Ysize) * 16.0f * 1.0f / (float)(WARP_DISC_Y - 1);
		CounterX = WARP_DISC_X;
		while (CounterX--)
		{
			SOFT_tdst_AVertex stSCREEN[WARP_DISC_Y * 2] __attribute__((aligned(16)));
			SOFT_tdst_AVertex stUV[WARP_DISC_Y  * 2] 	__attribute__((aligned(16)));
			SOFT_tdst_AVertex stUV_C[WARP_DISC_Y * 2] 	__attribute__((aligned(16)));
			float X0,X1,Y0;
			Y0 = 0.0f;
			X0 = (float)CounterX * XJump;
			X1 = X0 + XJump;
			for (CounterY = 0 ; CounterY < WARP_DISC_Y * 2 ; CounterY += 2)
			{
				stSCREEN[CounterY		].x = X0;
				stSCREEN[CounterY + 1	].x = X1;
				stSCREEN[CounterY].y = stSCREEN[CounterY + 1].y = Y0;
				stSCREEN[CounterY].z = stSCREEN[CounterY + 1].z = 1.0f;
				stSCREEN[CounterY].w = stSCREEN[CounterY + 1].w = 0.0f;
				Y0 += YJump;
			}
			CounterY = WARP_DISC_Y * 2;
			while (CounterY--)
			{
				
				stUV[CounterY] . x = (stSCREEN[CounterY] . x) / (16.0f * (float)GspGlobal_ACCESS(Xsize));
				stUV[CounterY] . y = (stSCREEN[CounterY] . y + 48.0f) / (16.0f * (float)GspGlobal_ACCESS(Ysize));
				stUV[CounterY] . z = stUV[CounterY] . w = 1.0f;
				
				stUV[CounterY] . x -= 0.5f;
				stUV[CounterY] . y -= 0.5f;
				stUV[CounterY] . x *= 2.0f;
				stUV[CounterY] . y *= 3.0f;
				{
					float Radius;
					float NewX,NewY;
					Radius = fOptSqrt(stUV[CounterY] . x * stUV[CounterY] . x + stUV[CounterY] . y * stUV[CounterY] . y);
					if (Radius != 0.0f)
					{
						if (Radius < 1.0f)
						{
/*							Radius = stUV[CounterY] . x;
							if (Radius < 0.0f) Radius = -Radius;//*/
							stUV[CounterY] . x += (stUV[CounterY] . x * Radius - stUV[CounterY] . x) * Factor;
/*							Radius = stUV[CounterY] . y;
							if (Radius < 0.0f) Radius = -Radius;//*/
							stUV[CounterY] . y += (stUV[CounterY] . y * Radius - stUV[CounterY] . y) * Factor;
						}
					}
				}
				stUV[CounterY] . x /= 2.0f;
				stUV[CounterY] . y /= 3.0f;
				stUV[CounterY] . x += 0.5f;
				stUV[CounterY] . y += 0.5f;
			}
			Gsp_AE_Hell_Blitter_V_UV_C(stSCREEN , stUV , stUV_C , NULL , WARP_DISC_Y - 1 );
		}
	}
	
	Gsp_AE_END();
}
#ifdef GSP_PS2_BENCH
#define MaxNumberOfColorCost 300
u32 ulNumberOfColorCosts = 0;

OBJ_tdst_GameObject *p_CostlyGO[MaxNumberOfColorCost];
void ColorCost_AE_AddAnObject(OBJ_tdst_GameObject *p_GO)
{
	if (ulNumberOfColorCosts == MaxNumberOfColorCost) return;
	p_CostlyGO[ulNumberOfColorCosts++] = p_GO;
}

void ColorCost_AE_Render()
{
	if (ShowNormals) 
	{
		u32 Color;
		while (ulNumberOfColorCosts--)
		{
			switch (ShowNormals & 3)
			{
			case 1:// Engine
				Color = (u32)p_CostlyGO[ulNumberOfColorCosts]->LastEngineTick;
				break;
			case 2:// Draw
				Color = (u32)p_CostlyGO[ulNumberOfColorCosts]->LastDrawTick << 8;
				break;
			case 3:// Global
				Color = (u32)(lMin(p_CostlyGO[ulNumberOfColorCosts]->LastDrawTick + p_CostlyGO[ulNumberOfColorCosts]->LastEngineTick , 255)) << 16;
				break;
			}
			Gsp_DrawColorCostQuad(p_CostlyGO[ulNumberOfColorCosts] , Color);
		}
	}
	ulNumberOfColorCosts = 0;
}

#endif
void GSP_BorderBrightness(float Factor)
{
	if (Factor < 0.05f) return;
//	if (Factor < 0.4f) Factor = 0.4f;
	Factor *= 2.0f;
	if (Factor > 1.0f) Factor = 1.0f;
	Gsp_AE_BEGIN();	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHA_I);
	GSP_AE_SetRegister_TEST_1( 0L);
	Gsp_AE_Set_Hell_Blitter(3 , 0 , 0 );
	Gsp_AE_SetGSRegisters();
	{
		u_int CounterX , CounterY;
		float XJump,YJump;
		extern u32 ps2_ScreenMode;
#define BB_DISC_X 16
#define BB_DISC_Y 16
		XJump = GspGlobal_ACCESS(Xsize) * 16.0f * 1.0f / (float)(BB_DISC_X - 1);
		YJump = GspGlobal_ACCESS(Ysize) * 16.0f * 1.0f / (float)(BB_DISC_Y - 1);
		CounterX = BB_DISC_X;
		while (CounterX--)
		{
			SOFT_tdst_AVertex stSCREEN[BB_DISC_Y * 2 ] __attribute__((aligned(16)));
			u32 				stColors[BB_DISC_Y  * 2 ] 	__attribute__((aligned(16)));
			float X0,X1,Y0,_16_9_COR;
			Y0 = 0.0f;
			X0 = (float)CounterX * XJump;
			X1 = X0 + XJump;
			_16_9_COR = 1.0f;
			if (ps2_ScreenMode == 2)
				_16_9_COR = 0.8f;
			for (CounterY = 0 ; CounterY < BB_DISC_Y * 2 ; CounterY += 2)
			{
				float CCoefX,CCoefY;
				stSCREEN[CounterY		].x = X0;
				stSCREEN[CounterY + 1	].x = X1;
				stSCREEN[CounterY].y = stSCREEN[CounterY + 1].y = Y0;
				stSCREEN[CounterY].z = stSCREEN[CounterY + 1].z = 1.0f;
				stSCREEN[CounterY].w = stSCREEN[CounterY + 1].w = 0.0f;
				Y0 += YJump;
#define CARRRRAMBA_COEF 2.1f
				CCoefX = CARRRRAMBA_COEF * (float)((float)CounterX + 0.5f - (float)(BB_DISC_X>>1)) / (float)BB_DISC_X;
				CCoefY = CARRRRAMBA_COEF * _16_9_COR * (float)((float)(CounterY>>1) + 0.5f  - (float)(BB_DISC_Y>>1)) / (float)BB_DISC_Y;
				CCoefX = ((CCoefX * CCoefX + CCoefY * CCoefY)) * 127.0f;
				if (CCoefX > 127.0f) CCoefX = 127.0f;
				if (CCoefX < 0.0f) CCoefX = 0.0f;
				CCoefX *= Factor;
				stColors[CounterY] = ((u32)CCoefX)<<24;
//				stColors[CounterY] ^= 0x7f000000;
/*				stColors[CounterY]  |= stColors[CounterY] << 8;
				stColors[CounterY]  |= stColors[CounterY] << 16;*/

				CCoefX = CARRRRAMBA_COEF * (float)((float)(CounterX+1) + 0.5f  - (float)(BB_DISC_X>>1)) / (float)BB_DISC_X;
				CCoefY = CARRRRAMBA_COEF * _16_9_COR * (float)((float)(CounterY>>1) + 0.5f  - (float)(BB_DISC_Y>>1)) / (float)BB_DISC_Y;
				CCoefX = ((CCoefX * CCoefX + CCoefY * CCoefY)) * 127.0f;
				if (CCoefX > 127.0f) CCoefX = 127.0f;
				if (CCoefX < 0.0f) CCoefX = 0.0f;
				CCoefX *= Factor;
				stColors[CounterY + 1] = ((u32)CCoefX)<<24;
//				stColors[CounterY + 1] ^= 0x7f000000;
/*				stColors[CounterY + 1]  |= stColors[CounterY + 1] << 8;
				stColors[CounterY + 1]  |= stColors[CounterY + 1] << 16;*/

			}
			Gsp_AE_Hell_Blitter_V_UV_C(stSCREEN , NULL , NULL , stColors , BB_DISC_Y - 1 );
			{
				u_int Stoppy;
				Stoppy = p_AE_CurrentBufferPointer - GspGlobal_ACCESS(p_CurrentBufferPointer);
				if (Stoppy > (RAM_BUF_SIZE - 0x1000))
				{
					Gsp_AE_END();
					Gsp_FIFO_STOP();
					Gsp_AE_BEGIN();
				}
			}
			
		}
	}
	
	Gsp_AE_END();
}
void GSP_PushZBuffer(float Z)
{
	u_long64 SaveCUR_REG_ZBUF;

	Gsp_Rect stDST , stSRC;
	
	ZUSedForCase = GSP_ulZ_To_ZBuffer(Z);
	
	stDST.DX = 0;
	stDST.DY = 0;
	stDST.SX = GspGlobal_ACCESS(Xsize);
	stDST.SY = GspGlobal_ACCESS(Ysize);
	
	/* Push Z buffer -> FPS must not interact with the rest  */
	SaveCUR_REG_ZBUF = GspGlobal_ACCESS(CUR_REG_ZBUF);
	Gsp_AE_BEGIN();

	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1(	0L);
	Gsp_AE_SetGSRegisters();
	
	/* 1 -> Clear Alpha buffer */
	Gsp_AE_Set_Hell_Blitter(3 , 0 , 0xffffff ); // Hide color	
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0);
	
	
	
	/* 2 -> Set Alpha < Z to 255 */
	GSP_AE_SetRegister_TEST_1( (1L << 16L) | (2L << 17L));
	Gsp_AE_Set_Hell_Blitter(3 , 0 , 0xffffff ); // Hide color	
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0xffffffff);
	
	/* 3 -> Draw with destination alpha TEST */
	GSP_AE_SetRegister_TEST_1( (1L << 14L) | (0L << 15L) | (1L << 16L) | (1L << 17L));
	/* Z Wrrite on */
	Gsp_AE_Set_Hell_Blitter(3 , 0 , 0xffffffff); // Hide color + alpha
	GspGlobal_ACCESS(CUR_REG_ZBUF) &= ~(1L << 32L); /* Zwrite on */
	GSP_AE_SetRegister(GSP_ZBUF_1 , GspGlobal_ACCESS(CUR_REG_ZBUF));
	Gsp_AE_SetGSRegisters();
	
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0xffffffff);
	
	
	Gsp_AE_END();
	GspGlobal_ACCESS(CUR_REG_ZBUF) = SaveCUR_REG_ZBUF ;
	GspGlobal_ACCESS(bSRS_FromScratch) = 1;

	ZUSedForCase = 0;	
}


void GSP_AE_ZListHookGlow()
{
	//GspGlobal_ACCESS(p_AE_Params)->Glow = 2.0f;
	if (GspGlobal_ACCESS(p_AE_Params)->Glow)
	{
		Gsp_ClearStart();
		GSP_Remanance(GspGlobal_ACCESS(p_AE_Params)->Glow,1);//*/
		GSP_FLushAllTextureCache();
		AE_TexturesAreFlushed = 0;
		GspGlobal_ACCESS(bSRS_FromScratch) = 1;//*/
	}
	
}
void Gsp_AE()
{
//	return;
//	GSP_EA_HiddenBufferUse();
	if (!(GspGlobal_ACCESS(Status) & GSP_Status_AE_ModeCompare))
	{
		Gsp_ClearStart();

		if (GspGlobal_ACCESS(p_AE_Params)->ulG_GlowMode & 0x80)
		{
			GspGlobal_ACCESS(p_AE_Params)->ulG_GlowMode &= ~0x80;
			GSP_EA_ShowTheOne(GspGlobal_ACCESS(p_AE_Params)->ulG_GlowMode , GspGlobal_ACCESS(p_AE_Params)->fG_Glowintensity , GspGlobal_ACCESS(p_AE_Params)->fG_GlowMotionBlur);
	//		GSP_EA_ShowTheOne(GspGlobal_ACCESS(p_AE_Params)->ulG_GlowMode ,1.0f);
		}
		GSP_AE_GetZBuffer1_SynchroDma1();
		if (fFadePushCMD) Gsp_AE_FADE_PUSH_CMD(fFadePushCMD);
		fFadePushCMD = 0.0f;
		FadePoint = GDI_gpst_CurDD->st_Camera.st_Matrix.T;

	//	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_MB)			Gsp_AE_MotionBlur((u_int)(GspGlobal_ACCESS(p_AE_Params)->MotionBlurFactor * (float)0x80));

		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SHOWZBUF)		Gsp_AE_ShowZbuffer();
		
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_DB)	
			Gsp_AE_DepthBlur(GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart, GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd);
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_DBN)	
			Gsp_AE_DepthBlurNear(GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near, 1.0f);
			
			
		
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SM)			
			//Gsp_AE_Blur((u_int)(GspGlobal_ACCESS(p_AE_Params)->BlurFactor * 64.0f) , 0);
			Gsp_AE_Blur2(GspGlobal_ACCESS(p_AE_Params)->BlurFactor,0,0);
		
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SMSM)	
		{
			int XX,YY;
			MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &MotionSmoothPoint);
			MotionSmoothPoint . x = (GspGlobal_ACCESS(fCurrentFocale) * MotionSmoothPoint . x) / MotionSmoothPoint . z;
			MotionSmoothPoint . y = (GspGlobal_ACCESS(fCurrentFocale) * MotionSmoothPoint . y) / MotionSmoothPoint . z;
			XX = (int)(0.2f * GspGlobal_ACCESS(Xsize) * MotionSmoothPoint . x);
			YY = (int)(0.2f * GspGlobal_ACCESS(YCorrectionCoef) * GspGlobal_ACCESS(Ysize) * MotionSmoothPoint . y);
			Gsp_AE_MotionSmooth((int)XX , (int)YY);	
			MotionSmoothPoint.x = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Iz;
			MotionSmoothPoint.y = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Jz;
			MotionSmoothPoint.z = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Kz;
		}
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SHOWSDW)		Gsp_AE_ShowShadows();
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_EMB)			Gsp_AE_Emboss(1 , 1 ,(u_int)(GspGlobal_ACCESS(p_AE_Params)->EmbossFactor * 255.0f));
	/*#ifdef GSP_PS2_BENCH
		{
			extern u_int ShowNormals;
			if (ShowNormals)			Gsp_AE_Emboss(1 , 1 ,(u_int)255);
		}
	#endif	*/
	{
	/*MATH_tdst_Vector LightD;
	LightD.x = -1.0f;
	LightD.y = 0.0f;
	LightD.z = 0.0f;
	Gsp_AE_ZoomSmoothGodRays(1.0f , &LightD);//*/
	}


		if (GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceFactor)  
				Gsp_AE_ZoomSmoothGodRays(GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceFactor , &GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothRemananceLightDirection , 0);
		
		if (GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor)			
		{
			if (GspGlobal_ACCESS(p_AE_Params)->ZSDirIsValidated)
				Gsp_AE_ZoomSmoothGodRays(GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor * 0.2f ,&GspGlobal_ACCESS(p_AE_Params)->ZSDir,1);
			else
				Gsp_AE_ZoomSmooth(GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor * 0.2f ,0);
			if (!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I))
				GspGlobal_ACCESS(p_AE_Params)->ZSDirIsValidated = 0;
		}
	//*/	
		if (GspGlobal_ACCESS(p_AE_Params)->BWFactor != 0.0f) 	GSP_BufferBW( GspGlobal_ACCESS(p_AE_Params)->BWFactor );
		
		Gsp_AE_BrightnessContrast_ColorBalance(GspGlobal_ACCESS(p_AE_Params)->Contrast + GlobalContrast, GspGlobal_ACCESS(p_AE_Params)->Brighness + GlobalBrightness, GspGlobal_ACCESS(p_AE_Params)->CB_Intensity , GspGlobal_ACCESS(p_AE_Params)->CB_Spectre , GspGlobal_ACCESS(p_AE_Params)->CB_PhotoFilterEffect , GspGlobal_ACCESS(p_AE_Params)->CB_Pastel);
		
		GSP_BorderBrightness(GspGlobal_ACCESS(p_AE_Params)->BorderBrithness);

		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_RS)			Gsp_AE_ZoomSmooth(GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor,1);
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_REMANANCE)   	GSP_Remanance(GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor,0);//*/
		if (GspGlobal_ACCESS(p_AE_Params)->fWarpFactor) GSP_Warp(0,GspGlobal_ACCESS(p_AE_Params)->fWarpFactor);//*/
		
		if (GspGlobal_ACCESS(Status) & GSP_Status_AE_BlurTest44)  	Gsp_AE_Full_AA_Blur(GspGlobal_ACCESS(p_AE_Params)->AA22BlurMode);
		
		{
			extern u32 JADE_HorizontalSymetry ;
			JADE_HorizontalSymetry = GDI_gpst_CurDD->GlobalXInvert;
			if (JADE_HorizontalSymetry)		
			{
				GSP_FlipH();
			}
		}
		
		/* Clear border */
		Gsp_ClearBorderExtend();

		if (GspGlobal_ACCESS(Status) & GSP_Status_SVS) Gsp_SpecialRaster();
		
	#ifdef GSP_PS2_BENCH
		ColorCost_AE_Render();
	#endif	
		

		/* No Z reads */
		Gsp_AE_BEGIN();

		GspGlobal_ACCESS(CUR_REG_FRAME) &= 0x00000000ffffffffL;
		GSP_AE_SetRegister(GSP_FRAME_1 , GspGlobal_ACCESS(CUR_REG_FRAME));
		Gsp_AE_SetGSRegisters();
		
		Gsp_AE_END();
		Gsp_FIFO_STOP();	
	}
	GSP_FLushAllTextureCache();
	AE_TexturesAreFlushed = 0;
	GspGlobal_ACCESS(bSRS_FromScratch) = 1;
	GspGlobal_ACCESS(Rendering2D) = 1;
	GspGlobal_ACCESS(YCorrectionCoef) = 4.0f/3.0f;
	
	
}

void Gsp_AE_Vertical_AA()
{

	Gsp_AE_BEGIN();

	if (GspGlobal_ACCESS(VideoMode) == GSP_VRES_x2_TripleAA)
	{
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter(0 , 0 , 0 ); // Hide color
		Gsp_AE_Hell_Blitter( 0,0,0,1,0,1,0xffffffff ,0x80808080); 
	}

	Gsp_AE_END();
	
}
void AE_Splash_AddOne()
{
}
void Gsp_AE_FADE_SAVE()
{
	Gsp_Rect stDST , stSRC;
	
	Gsp_AE_BEGIN();
	
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister_TEST_1( 0L);
	// SrcBuf 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color  , | 0x80000000 -> TextureNum , 
	// DstBuf 0->Current FBF , 1->Other FBF , 2->ZBF , 
	if (GspGlobal_ACCESS(ulFade_Counter) & 0x40000000)
		Gsp_AE_Set_Hell_Blitter(0 , 0 , 0xff000000 ); // Hide alpha
	else
		Gsp_AE_Set_Hell_Blitter(1 , 0 , 0xff000000 ); // Hide alpha
	Gsp_AE_Hell_Blitter( -1,0,1,0,1,0,0xffffffff ,0x80808080); 
	Gsp_AE_Set_Hell_Blitter_B(0 , 0 , 0xff000000 , 0 ); // Hide alpha
	stDST.DX = GspGlobal_ACCESS(Xsize) >> 1;
	stDST.DY = 0;
	stDST.SX = GspGlobal_ACCESS(Xsize) >> 1;
	stDST.SY = GspGlobal_ACCESS(Ysize);
	stSRC = stDST;
	stSRC.DX = 0;
	stDST.DX-=1;
	stDST.DY--;
	Gsp_AE_CaseBlitter(&stDST,&stSRC,0x80808080);
	
	Gsp_AE_END();
	
}
void GSP_AE_DisplayMire(u32 Mode)
{
#ifndef _FINAL_
	extern u32 DisplayMire;
	if (DisplayMire)
	{
		Gsp_Rect stDST , stSRC;
		if (Mode) Gsp_AE_BEGIN();
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GSP_AE_SetRegister_TEST_1(	0L);
		if (Mode)
		Gsp_AE_Set_Hell_Blitter(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
								0 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
								0 );
		else					
		Gsp_AE_Set_Hell_Blitter(3 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 3->Color , | 0x80000000 -> TextureNum	
								8 , // 0->Current FBF , 1->Other FBF , 2->ZBF , 
								0 );
							
		stDST.DX = (GspGlobal_ACCESS(Xsize)>>3);
		stDST.DY = GspGlobal_ACCESS(Ysize) - 54;
		
		if (Mode) stDST.DY -= 30;
		
		stDST.SX = GspGlobal_ACCESS(Xsize) - (GspGlobal_ACCESS(Xsize)>>2);
		stDST.SY = 30;
		stSRC = stDST;
		Gsp_AE_CaseBlitter(&stDST,&stSRC,0x808080);
		
		stDST.DX += 2;
		stDST.DY += 2;
		stDST.SX -= 4;
		stDST.SY -= 4;
		stSRC = stDST;
		Gsp_AE_BarreBlitter(&stDST,&stSRC,0,0,0xffffff,0xffffff);
		if (Mode) Gsp_AE_END();
		
	}

#endif
}
void Gsp_AE_FADE()
{
	if (GspGlobal_ACCESS(ulFade_Counter))
	{
		Gsp_Rect stDST , stSRC ;

		GSP_FlushAllFloatingFlip();

		Gsp_AE_BEGIN();
		
		if (GspGlobal_ACCESS(ulFade_Counter) & 0xC0000000)
		{
			Gsp_AE_FADE_SAVE();
			GspGlobal_ACCESS(ulFade_Counter) &= ~0xC0000000;
		} else
		{
			int ZoomStrongniness;
			u_int FadeDelta,Color;
			float FadeSub;
			FadeDelta = VBlankCounter - GspGlobal_ACCESS(ulFade_Time);
			FadeSub = (float)FadeDelta * GspGlobal_ACCESS(ulFade_Inc);
			GspGlobal_ACCESS(ulFade_Alpha) -= FadeSub;
			if (GspGlobal_ACCESS(ulFade_Alpha) <= 0.0f)
			{
				GspGlobal_ACCESS(ulFade_Counter) = 0;
				Gsp_AE_END();
				return;
			}
			FadeSub = 1.0f - GspGlobal_ACCESS(ulFade_Alpha);
			FadeSub *= FadeSub;
			FadeSub = 1.0f - FadeSub;
			FadeSub = fMax(FadeSub , 0.0f);
			FadeDelta = (u_int)(FadeSub*127.0f);
			GspGlobal_ACCESS(ulFade_Time) = VBlankCounter;

			
			/* XCompress current FBF */
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
			GSP_AE_SetRegister_TEST_1( 0L);
			Gsp_AE_Set_Hell_Blitter(0 , 0 , 0xff000000 ); // Hide alpha
			Gsp_AE_Hell_Blitter( -1,0,1,0,1,0,0xffffffff ,0x80808080); 
			/* Save last FBF */
			Gsp_AE_Set_Hell_Blitter_B(1 , 0 , 0xff000000,0 ); // Hide alpha , no bilinear filtering
			stDST.DX = GspGlobal_ACCESS(Xsize) >> 1;
			stDST.DY = 0;
			stDST.SX = GspGlobal_ACCESS(Xsize) >> 1;
			stDST.SY = GspGlobal_ACCESS(Ysize);
			stSRC = stDST;
			stDST.DX--;
			stDST.DY--;
			Gsp_AE_CaseBlitter(&stDST,&stSRC,0x80808080);
			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| (((u_long)(FadeDelta)) << 32L));
			Color = 0x80808080;
//			GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_ALPHAFIX 	| (((u_long)(FadeDelta)) << 32L));
			Gsp_AE_Set_Hell_Blitter_B(0 , 0 , 0xff000000 , 0 ); // Hide alpha
			stDST.DX = GspGlobal_ACCESS(Xsize) >> 1;
			stDST.DY = 0;
			stDST.SX = GspGlobal_ACCESS(Xsize) >> 1;
			stDST.SY = GspGlobal_ACCESS(Ysize);
			stSRC = stDST;
			stDST.DX--;
			stDST.DY--;
			stDST.DX = -1;
			Gsp_AE_CaseBlitter(&stDST,&stSRC,Color);

			
		}
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		/* Clear bug */
		Gsp_AE_Set_Hell_Blitter(3 , 0 , 0xff000000 ); // Hide alpha
		stDST.DX = (GspGlobal_ACCESS(Xsize) >> 1) - 2;
		stDST.DY = 0;
		stDST.SX = 2;
		stDST.SY = GspGlobal_ACCESS(Ysize);
		Gsp_AE_CaseBlitter(&stDST,&stDST,0);//*/
		Gsp_AE_END();
	}
}
#ifdef GSP_ASYNC_FLIP_ENABLE
extern u_int ulVideoScreentValid;
u32 GSP_BIGFADETOBLACK = 0;
void Gsp_FlipFB_ASYNC()
{
	Gsp_FIFO_STOP();	
	Gsp_AE_BEGIN();
#ifdef GSP_USE_TRIPLE_BUFFERRING	
/* Copy to current FBF	*/
	{
		u_int	SaveMAGH;
		extern u_int TRIPLE_BIGSWAP_SOFT;
		extern u_int volatile GSPDMASEMAPHORE;
		extern u32 BinkVideoWith ;
		extern volatile u32 BINK_MTH_Run;
		SaveMAGH = GspGlobal_ACCESS(BppMode);
		GspGlobal_ACCESS(BppMode) = 2;
		GSP_AE_SetRegister_TEST_1( 0L);
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		GSP_AE_SetRegister(GSP_DTHE 	, 1L);
		GSP_AE_SetRegister(GSP_DIMX 	, 0x0617435217065243L);
		if (BinkVideoWith && (BinkVideoWith != GspGlobal_ACCESS(Xsize)))
			Gsp_AE_Set_Hell_Blitter_B(0 , 8 , 0xFF000000 , 1);  // Hide Alpha
		else
			Gsp_AE_Set_Hell_Blitter_B(0 , 8 , 0xFF000000 , 0);  // Hide Alpha
		Gsp_AE_SetGSRegisters();//*/
		Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0xffffffff ,GSP_BIGFADETOBLACK|(GSP_BIGFADETOBLACK<<8)|(GSP_BIGFADETOBLACK<<16)); 
		if (BinkVideoWith && (BINK_MTH_Run))
		{
			Gsp_LoadingQuad();
		}
		else
		{
			if (!BinkVideoWith)
				GSP_AE_DisplayMire(0);
		}
		
		GSP_BIGFADETOBLACK+=0x2;
		if (GSP_BIGFADETOBLACK > 0x80) GSP_BIGFADETOBLACK = 0x80;
		GspGlobal_ACCESS(BppMode) = SaveMAGH;
		GSP_AE_SetRegister(GSP_DTHE 	, 0L);

		GspGlobal_ACCESS(GSP_BX_VAR).pCurrentCpuBuffer->BX_Option = (TRIPLE_BIGSWAP_SOFT << 1) | 1;
		GSPDMASEMAPHORE ++;
		TRIPLE_BIGSWAP_SOFT ^= 1;
	}	
#else
	GspGlobal_ACCESS(GSP_BX_VAR).pCurrentCpuBuffer->BX_Option = 0xC0D0 | (GSP_IsFrameODD());
#endif
	
	if ((GspGlobal_ACCESS(ZBppMode) == 2) && (GspGlobal_ACCESS(BppMode) != 2))
		GspGlobal_ACCESS(CUR_REG_ZBUF) = /*ZBP*/((unsigned long)GspGlobal_ACCESS(ZBP) << 0L) | /*OPP*/((unsigned long)10 << 24L) | /*ZMSK*/(0L << 32L);
	else
		GspGlobal_ACCESS(CUR_REG_ZBUF) = /*ZBP*/((unsigned long)GspGlobal_ACCESS(ZBP) << 0L) | /*OPP*/((unsigned long)GspGlobal_ACCESS(ZBppMode) << 24L) | /*ZMSK*/(0L << 32L);
	GSP_AE_SetRegister(GSP_ZBUF_1 	, GspGlobal_ACCESS(CUR_REG_ZBUF));
	GSP_AE_SetRegister(GSP_ZBUF_2 	, GspGlobal_ACCESS(CUR_REG_ZBUF));
	GSP_AE_SetRegister(GSP_XYOFFSET_1 	, /*X*/((2048L - ((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L)) << (0L + 4L)) | /*Y*/((2048L - ((unsigned long)GspGlobal_ACCESS(Ysize) >> 1L)) << (32L + 4L)));
	GSP_AE_SetRegister(GSP_XYOFFSET_2 	, /*X*/((2048L - ((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L)) << (0L + 4L)) | /*Y*/((2048L - ((unsigned long)GspGlobal_ACCESS(Ysize) >> 1L)) << (32L + 4L)));
	GSP_AE_SetRegister(GSP_PRMODECONT 	, 0L);
	GSP_AE_SetRegister(GSP_PRMODE 	, (1L<<6L) | (1L<<3L));
	GSP_AE_SetRegister(GSP_TEST_1 	, ((1L << 16L) | (2L << 17L)));
	GSP_AE_SetRegister(GSP_TEST_2 	, ((1L << 16L) | (2L << 17L)));
	GSP_AE_SetRegister(GSP_PABE 	, 0L);
	GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_AE_SetRegister(GSP_ALPHA_2 	, GST_COPY);
	{
		u_int Remorf;
		Remorf = GspGlobal_ACCESS(Morfling) & ~31;
		GSP_AE_SetRegister(GSP_SCISSOR_1 	, (0L << 0L) | (((unsigned long)GspGlobal_ACCESS(Xsize) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
		GSP_AE_SetRegister(GSP_SCISSOR_2 	, (0L << 0L) | (((unsigned long)GspGlobal_ACCESS(Xsize) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
	}
	GSP_AE_SetRegister(GSP_SCANMSK 	, 0L);
	GSP_AE_SetRegister(GSP_TEXA 	, (0L<<0L)/*Ta0*/ | (0L<<15L) | (0x80L<<32L)/*Ta1*/);
	Gsp_AE_SetGSRegisters();
	
	if (GspGlobal_ACCESS(BppMode) == 2)
	{
		GSP_AE_SetRegister(GSP_DTHE 	, 1L);
		GSP_AE_SetRegister(GSP_DIMX 	, 0x0617435217065243L);
	}
	else
	{
		GSP_AE_SetRegister(GSP_DTHE 	, 0L);
	}
#ifdef GSP_USE_TRIPLE_BUFFERRING
	GspGlobal_ACCESS(CUR_REG_FRAME) = /*FBP*/((unsigned long)GspGlobal_ACCESS(FBP2) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/(0L << 32L);
	GSP_AE_SetRegister(GSP_FRAME_1 	, GspGlobal_ACCESS(CUR_REG_FRAME));
	GSP_AE_SetRegister(GSP_FRAME_2 	, GspGlobal_ACCESS(CUR_REG_FRAME));
#else
	if ((GSP_IsFrameODD()))
	{ // Select buffer 2 
		GspGlobal_ACCESS(CUR_REG_FRAME) = /*FBP*/((unsigned long)GspGlobal_ACCESS(FBP2) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/(0L << 32L);
		GSP_AE_SetRegister(GSP_FRAME_1 	, GspGlobal_ACCESS(CUR_REG_FRAME));
		GSP_AE_SetRegister(GSP_FRAME_2 	, GspGlobal_ACCESS(CUR_REG_FRAME));
	} else 
	{ /* Select Buffer 1 */
		GspGlobal_ACCESS(CUR_REG_FRAME) = /*FBP*/((unsigned long)GspGlobal_ACCESS(FBP1) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/(0L << 32L);
		GSP_AE_SetRegister(GSP_FRAME_1 	, GspGlobal_ACCESS(CUR_REG_FRAME));
		GSP_AE_SetRegister(GSP_FRAME_2 	, GspGlobal_ACCESS(CUR_REG_FRAME));
	}
#endif	
	GSP_AE_SetRegister(GSP_PRMODE 	, (0L<<4L) | (1L<<6L) | (1L<<3L));
	GSP_AE_SetRegister(GSP_COLCLAMP 	, 1L);
	GSP_AE_SetRegister(GSP_SCANMSK 	, 0L);
/* clear ZBuffer */
	{
		GSP_AE_SetRegister_TEST_1( (1L << 16L) | (1L << 17L));
		GSP_AE_SetRegister(GSP_ALPHA_1 	, GST_COPY);
		Gsp_AE_Set_Hell_Blitter(3 , 0 , 0 );

#ifdef GSP_PS2_BENCH
		if ((GspGlobal_ACCESS(Status) & GSP_Status_Show_Depth)||ShowNormals)
			Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0 ,0); 
		else
#endif			
		Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0 ,(GspGlobal_ACCESS(ulBackColor) & 0xffffff/*| 0x80000000*/)); 
		Gsp_AE_Set_Hell_Blitter(3 , 0 , 0 );
		GspGlobal_ACCESS(CUR_REG_ZBUF) &= ~(1L << 32L); /* Zwrite off */
		GSP_AE_SetRegister(GSP_ZBUF_1 , GspGlobal_ACCESS(CUR_REG_ZBUF));
		Gsp_AE_SetGSRegisters();//*/
		Gsp_AE_Hell_Blitter( 0,0,0,0,0,0,0 ,GspGlobal_ACCESS(ulBackColor) & 0xffffff); 
	}
	GSP_AE_SetRegister(GSP_PRMODE 	, (0L<<4L) | (1L<<6L) | (1L<<3L));
	GSP_AE_SetRegister(GSP_COLCLAMP 	, 1L);
	GSP_AE_SetRegister(GSP_SCANMSK 	, 0L);
	GSP_AE_SetRegister_TEST_1( (1L << 16L) | (2L << 17L));
	if ((GspGlobal_ACCESS(ZBppMode) == 2) && (GspGlobal_ACCESS(BppMode) != 2))
		GspGlobal_ACCESS(CUR_REG_ZBUF) = /*ZBP*/((unsigned long)GspGlobal_ACCESS(ZBP) << 0L) | /*OPP*/((unsigned long)10 << 24L) | /*ZMSK*/(0L << 32L);
	else
		GspGlobal_ACCESS(CUR_REG_ZBUF) = /*ZBP*/((unsigned long)GspGlobal_ACCESS(ZBP) << 0L) | /*OPP*/((unsigned long)GspGlobal_ACCESS(ZBppMode) << 24L) | /*ZMSK*/(0L << 32L);
	GSP_AE_SetRegister(GSP_ZBUF_1 	, GspGlobal_ACCESS(CUR_REG_ZBUF));
	GSP_AE_SetRegister(GSP_ZBUF_2 	, GspGlobal_ACCESS(CUR_REG_ZBUF));
	
	
	Gsp_AE_SetGSRegisters();
	Gsp_AE_END();
}

#endif // GSP_ASYNC_FLIP_ENABLE