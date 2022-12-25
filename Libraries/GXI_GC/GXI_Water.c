#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_render.h"
#include "GXI_displaylist.h"
#include "GXI_vertexspace.h"
#include "GXI_tex.h"
#include "GXI_font.h"
#include "GXI_shadows.h"
#include "GXI_renderstate.h"
#include "GXI_bench.h"
#include "MATHs/MATH.h"
#include "BASe/MEMory/MEM.h"
#include "MATerial/MATstruct.h"
#include "GXI_dbg.h"
#include "GEOmetric/GEO_STRIP.h"
#include "TEXture/WATER_FFT.h"
#include "GameCube/GC_File.h"
#include "ENGine/Sources/WORld/WORstruct.h"

//#define WTR_DO_THE_FOAM
//#define WTR_DO_THE_DISRUPT
//#define WTR_DO_THE_TRANSPARENCY
//#define	WTR_DO_THE_UV
//#define	WTR_DO_THE_UV
//#define REFLECTION 




u32 ulRealWaterColor = 0;

tdst_FFT WATERFFT;
float ZFactor = 1.0;
float fChoppyFactor = 1.0;
float HCoef = 0.02f;
float DisturbFactor = 1.f;
float SpeedFactor = 10000;
float GlobalWaterZ = -1.0f;
float WaterFog = 50.0f;
u32 bWiredLock = 1;
u32 ZListOrder = 0;
extern void *pst_GlobalsWaterParams;




#ifdef REFLEXION
#define WAveMapShift 6
#define WAveMapDXYShift 6

#define DisturbMapPo2 6

#define VM_DISCX 50
#define VM_DISCY 100 // BIG JUMPS
#define FresnelMapPo2 6
#define DisruptMapPo2 8
#define FoamMapPo2 7

#define DisruptbASE 100
#define FogMapPo2	4
u16	pTextureMap[(1 << (DisturbMapPo2 << 1)) + ((1 << (DisturbMapPo2 << 1)) >> 1)] ATTRIBUTE_ALIGN(32);
u16	pPerturbAlphaMap[(1 << (FoamMapPo2 << 1)) + ((1 << (FoamMapPo2 << 1)) >> 1)] ATTRIBUTE_ALIGN(32);
u16 g_ClipTexturesMap[4 * 4] ATTRIBUTE_ALIGN(32);


// GXI_RenderWATER
GXTexObj g_WaterDisplacementTexture;
GXTexObj g_WaterTextures;
GXTexObj g_WaterTextures_SUN; 

// GXI_ReverseWorldAndCallDisplayListASecondTime
GXTexObj g_ClipTextures;

// GXI_RenderLights
GXTexObj g_WaterPerturbAlphaTexture;

#endif // REFLEXION


inline void MATH_CrossProduct2
(
	register MATH_tdst_Vector	*dst,
	register MATH_tdst_Vector	*vec1,
	register MATH_tdst_Vector	*vec2
)
{
	dst->x = fMul(vec1->y, vec2->z) - fMul(vec1->z, vec2->y);
	dst->y = fMul(vec1->z, vec2->x) - fMul(vec1->x, vec2->z);
	dst->z = fMul(vec1->x, vec2->y) - fMul(vec1->y, vec2->x);
}

void GXI_WTR_MipMapATile(u16 *pDST , u16 *pDSTTile , u16 *pDSTTileNextLine)
{
	pDST[0] = ((pDSTTile[0] & 0xfcfc) >> 2) + ((pDSTTile[1] & 0xfcfc) >> 2) + ((pDSTTile[4] & 0xfcfc) >> 2) + ((pDSTTile[5] & 0xfcfc) >> 2);
	pDST[1] = ((pDSTTile[2] & 0xfcfc) >> 2) + ((pDSTTile[3] & 0xfcfc) >> 2) + ((pDSTTile[6] & 0xfcfc) >> 2) + ((pDSTTile[7] & 0xfcfc) >> 2);
	pDST[4] = ((pDSTTile[8] & 0xfcfc) >> 2) + ((pDSTTile[9] & 0xfcfc) >> 2) + ((pDSTTile[12] & 0xfcfc) >> 2) + ((pDSTTile[13] & 0xfcfc) >> 2);
	pDST[5] = ((pDSTTile[10] & 0xfcfc) >> 2) + ((pDSTTile[11] & 0xfcfc) >> 2) + ((pDSTTile[14] & 0xfcfc) >> 2) + ((pDSTTile[15] & 0xfcfc) >> 2);
	
	pDSTTile += 4*4; // Next X Tile
	
	pDST[2] = ((pDSTTile[0] & 0xfcfc) >> 2) + ((pDSTTile[1] & 0xfcfc) >> 2) + ((pDSTTile[4] & 0xfcfc) >> 2) + ((pDSTTile[5] & 0xfcfc) >> 2);
	pDST[3] = ((pDSTTile[2] & 0xfcfc) >> 2) + ((pDSTTile[3] & 0xfcfc) >> 2) + ((pDSTTile[6] & 0xfcfc) >> 2) + ((pDSTTile[7] & 0xfcfc) >> 2);
	pDST[6] = ((pDSTTile[8] & 0xfcfc) >> 2) + ((pDSTTile[9] & 0xfcfc) >> 2) + ((pDSTTile[12] & 0xfcfc) >> 2) + ((pDSTTile[13] & 0xfcfc) >> 2);
	pDST[7] = ((pDSTTile[10] & 0xfcfc) >> 2) + ((pDSTTile[11] & 0xfcfc) >> 2) + ((pDSTTile[14] & 0xfcfc) >> 2) + ((pDSTTile[15] & 0xfcfc) >> 2);
	
	pDSTTile = pDSTTileNextLine; // Next X Tile
	pDST += 8;
	
	pDST[0] = ((pDSTTile[0] & 0xfcfc) >> 2) + ((pDSTTile[1] & 0xfcfc) >> 2) + ((pDSTTile[4] & 0xfcfc) >> 2) + ((pDSTTile[5] & 0xfcfc) >> 2);
	pDST[1] = ((pDSTTile[2] & 0xfcfc) >> 2) + ((pDSTTile[3] & 0xfcfc) >> 2) + ((pDSTTile[6] & 0xfcfc) >> 2) + ((pDSTTile[7] & 0xfcfc) >> 2);
	pDST[4] = ((pDSTTile[8] & 0xfcfc) >> 2) + ((pDSTTile[9] & 0xfcfc) >> 2) + ((pDSTTile[12] & 0xfcfc) >> 2) + ((pDSTTile[13] & 0xfcfc) >> 2);
	pDST[5] = ((pDSTTile[10] & 0xfcfc) >> 2) + ((pDSTTile[11] & 0xfcfc) >> 2) + ((pDSTTile[14] & 0xfcfc) >> 2) + ((pDSTTile[15] & 0xfcfc) >> 2);
	
	pDSTTile += 4*4; // Next X Tile
	
	pDST[2] = ((pDSTTile[0] & 0xfcfc) >> 2) + ((pDSTTile[1] & 0xfcfc) >> 2) + ((pDSTTile[4] & 0xfcfc) >> 2) + ((pDSTTile[5] & 0xfcfc) >> 2);
	pDST[3] = ((pDSTTile[2] & 0xfcfc) >> 2) + ((pDSTTile[3] & 0xfcfc) >> 2) + ((pDSTTile[6] & 0xfcfc) >> 2) + ((pDSTTile[7] & 0xfcfc) >> 2);
	pDST[6] = ((pDSTTile[8] & 0xfcfc) >> 2) + ((pDSTTile[9] & 0xfcfc) >> 2) + ((pDSTTile[12] & 0xfcfc) >> 2) + ((pDSTTile[13] & 0xfcfc) >> 2);
	pDST[7] = ((pDSTTile[10] & 0xfcfc) >> 2) + ((pDSTTile[11] & 0xfcfc) >> 2) + ((pDSTTile[14] & 0xfcfc) >> 2) + ((pDSTTile[15] & 0xfcfc) >> 2);
	
}
void GXI_WTR_ComputeTextureMipMap(u16 *pDST ,u32 SizePo2 , u32 Smooth)
{
	if ((SizePo2 == 2) && Smooth)
	{

		u32 Counter;
		u16 LastColor;
		Counter = 4*4;
		LastColor = 0;
		if (Smooth) LastColor = 0x8080;
		while(Counter--)
			*(pDST++) = LastColor;
		return;
	}//*/
	{
		u32 XT,YT;
		u16 *pDSTMipMap , *pSRCTile00;
		pDSTMipMap = pDST + (1 << (SizePo2 << 1));
		for (YT = 0 ; YT < 1 << SizePo2 ; YT += 8)
		{
			for (XT = 0 ; XT < 1 << SizePo2 ; XT += 8)
			{
				pSRCTile00 = pDST + (((XT >> 2) + ((YT >> 2) << (SizePo2 - 2))) << 4);
				GXI_WTR_MipMapATile(pDSTMipMap , pSRCTile00 , pSRCTile00 + (4 << SizePo2));
				pDSTMipMap += 4*4;
			}
		}
	}
	pDST += (1 << (SizePo2 << 1));
	SizePo2 -= 1;
	if (Smooth)
	{
		u16 *pDSTLast;
		pDSTLast = pDST + (1 << (SizePo2 << 1));
		while(pDST < pDSTLast)
		{
			*pDST = ((*pDST & 0xfcfc) >> 1) + (0x8080 >> 1);
			*pDST = ((*pDST & 0xfcfc) >> 1) + (0x8080 >> 1);
			pDST++;
		}
		pDST -= (1 << (SizePo2 << 1));
	}
	if (SizePo2 == 2) return;
	GXI_WTR_ComputeTextureMipMap(pDST,SizePo2,Smooth );
}
u32 GXI_WTR_GetFresnelColor(float NZ , float CosThetai)
{
	return 0;
}
void GXI_InitFresnelMap()
{
}

#define WTR_FOG_ScaleZ 	   WaterFog
#define WTR_FOG_ScaleDepth (WaterFog * 8.0f)

u32 GXI_WTR_GetFogColor(float Y , float X )
{
	return 0;
}


_inline_ float GXI_fOptInvSqrt(register float a)
{
	register float fReturnValue;
	asm {
		frsqrte fReturnValue,a
	}
	return fReturnValue;

}

void GXI_WTR_PreComputeNormaleMap(MATH_tdst_Vector *pSrc , f32 *p_UV , MATH_tdst_Vector *pDest , unsigned int SizeAndPitch )
{
}


void GXI_WTR_ComputeTextureMap(u16 *pDST ,u32 SizePo2 )
{
	s32 XT,YT,Offset;
	FFT_Complex *pSrcTile;
	u16 *pDSTSAVE;

#if 1	
	pDSTSAVE = pDST;
	for (YT = 0 ; YT < 1 << SizePo2 ; YT += 4)
	{
		for (XT = 0 ; XT < 1 << SizePo2 ; XT += 4)
		{
			Offset = 4;
			while (Offset--)
			{
				u8 *PDSTS8;
				s32 YDIPS;
				PDSTS8 = (u8 *)pDST;
				YDIPS = (YT << (8 - SizePo2));
				if (YDIPS < 128) YDIPS = 128 + (128 - YDIPS);
#define RANDOMTEST(a) (a)//((a > 128) ? (128 + (rand()%(a - 128))) : (128 - (rand()%(128 - a))))
		        PDSTS8[1] = (u8)RANDOMTEST(YDIPS) - 0;
		        PDSTS8[3] = (u8)RANDOMTEST(YDIPS) - 0;
		        PDSTS8[5] = (u8)RANDOMTEST(YDIPS) - 0;
		        PDSTS8[7] = (u8)RANDOMTEST(YDIPS) - 0;

		        PDSTS8[0] = (u8)(RANDOMTEST(XT + 0) << (8 - SizePo2)) - 0;
		        PDSTS8[2] = (u8)(RANDOMTEST(XT + 1) << (8 - SizePo2)) - 0;
		        PDSTS8[4] = (u8)(RANDOMTEST(XT + 2) << (8 - SizePo2)) - 0;
		        PDSTS8[6] = (u8)(RANDOMTEST(XT + 3) << (8 - SizePo2)) - 0;
		        
//*/		        
		        YT += 1;
		        pDST+= 4;
	        }
	        YT -= 4;
		}
	}//*/
	GXI_WTR_ComputeTextureMipMap(pDSTSAVE ,SizePo2,1 );
#endif	
}
u8 WTR_GetRand()
{
	u32 FUCK;
	FUCK = rand() & 255;
	FUCK *= FUCK;
	FUCK *= FUCK;
	FUCK >>= 24;
	FUCK *= FUCK;
	FUCK *= FUCK;
	FUCK >>= 24;
	return FUCK ;
}

u8 WTR_COMPUTELENS_FLARE(u32 x,u32 y  , float Half) 
{
	float Dista2Ctr , DX,DY;
	DX = (float)x - Half;
	DY = (float)y - Half;
	DX /= Half;
	DY /= Half;
	Dista2Ctr = 1.0f - fSqrt(fSqrt(fSqrt(DX * DX + DY * DY)));
	if (Dista2Ctr < 0.0f) return 0;
	
	return (u8)(255.0f * Dista2Ctr);
}

void GXI_WTR_ComputeFoamMap(u16 *pDST ,u32 SizePo2 )
{
	u32 XT,YT,Offset;
	FFT_Complex *pSrcTile;
	u16 *pDSTSAVE;
	tdstGC_File *MyFILE;
	float Half;

/*	MyFILE = GC_fOpen("MOUSSE.raw", "rb");
	GC_fRead(MyFILE, FoamMap, 128*128*4);
	GC_fClose(MyFILE);*/

	Half = (float)(1 << (SizePo2 - 1));
	pDSTSAVE = pDST;
	for (YT = 0 ; YT < 1 << SizePo2 ; YT += 4)
	{
		for (XT = 0 ; XT < 1 << SizePo2 ; XT += 4)
		{
			Offset = 4;
			while (Offset--)
			{
				u8 *PDSTS8;
				PDSTS8 = (u8 *)pDST;
#define COMPUTELENS_FLARE(x,y) WTR_COMPUTELENS_FLARE(x,y,Half)
		        PDSTS8[0] = (u8)COMPUTELENS_FLARE(YT,XT + 0);
		        PDSTS8[2] = (u8)COMPUTELENS_FLARE(YT,XT + 1);
		        PDSTS8[4] = (u8)COMPUTELENS_FLARE(YT,XT + 2);
		        PDSTS8[6] = (u8)COMPUTELENS_FLARE(YT,XT + 3);

		        PDSTS8[1] = (u8)COMPUTELENS_FLARE(YT,XT + 0);
		        PDSTS8[3] = (u8)COMPUTELENS_FLARE(YT,XT + 1);
		        PDSTS8[5] = (u8)COMPUTELENS_FLARE(YT,XT + 2);
		        PDSTS8[7] = (u8)COMPUTELENS_FLARE(YT,XT + 3);
//*/		        
		        YT += 1;
		        pDST+= 4;
	        }
	        YT -= 4;
		}
	}//*/
	GXI_WTR_ComputeTextureMipMap(pDSTSAVE ,SizePo2,0 );
}

void GXI_WTR_ComputePerturbAlphaMap(u16 *pDST ,MATH_tdst_Vector *pTex , u32 SizePo2 )
{
	u32 XT,YT,Offset;
	FFT_Complex *pSrcTile;
	u16 *pDSTSAVE;
	tdstGC_File *MyFILE;

	pDSTSAVE = pDST;
	for (YT = 0 ; YT < 1 << SizePo2 ; YT += 4)
	{
		for (XT = 0 ; XT < 1 << SizePo2 ; XT += 4)
		{
			Offset = 4;
			while (Offset--)
			{
				u8 *PDSTS8;
				PDSTS8 = (u8 *)pDST;
#define PerturbRandom(a) (pTex[(YT << SizePo2) + XT + a].z * 400.0f + 128.0f)

		        PDSTS8[1] = PDSTS8[0] = (u8)(PerturbRandom(0));
		        PDSTS8[3] = PDSTS8[2] = (u8)(PerturbRandom(1));
		        PDSTS8[5] = PDSTS8[4] = (u8)(PerturbRandom(2));
		        PDSTS8[7] = PDSTS8[6] = (u8)(PerturbRandom(3));

		        YT += 1;
		        pDST+= 4;
	        }
	        YT -= 4;
		}
	}//*/
	GXI_WTR_ComputeTextureMipMap(pDSTSAVE ,SizePo2,0 );
}

void GXI_InitFogMap() {};


void GXI_ReverseWorldAndCallDisplayListASecondTime(GEO_tdst_ElementIndexedTriangles	*_pst_Element)
{
#ifdef REFLECTION
	static int bFirst = 1;
	GXColor color;
	if (!pst_GlobalsWaterParams) 
	{
		GXCallDisplayList(_pst_Element->dl, _pst_Element->dl_size & 0x0fffffff); // Normal	mode
		return;
	}
	if (bFirst)
	{
		GXInitTexObj(&g_ClipTextures, g_ClipTexturesMap, 4, 4, GX_TF_IA8, GX_CLAMP , GX_CLAMP,GX_FALSE);
	    GXInitTexObjLOD( &g_ClipTextures,GX_NEAR_MIP_NEAR, GX_NEAR,0.0F, 0.0f,0.0F,GX_FALSE,GX_FALSE,GX_ANISO_1 );
	    bFirst = 8;
	    while (bFirst--)
	    {
		    g_ClipTexturesMap[bFirst] = 0xffff;
		}
	    bFirst = 8;
	    while (bFirst--)
	    {
		    g_ClipTexturesMap[bFirst+8] = 0x0000;
		}
		bFirst = 0;
	}

#ifndef WTR_DO_THE_TRANSPARENCY
	GXCallDisplayList(_pst_Element->dl, _pst_Element->dl_size & 0x0fffffff); // Normal	
#endif	
	GX_GXSetNumTexGens(2);
	GX_GXSetNumTevStages(2);
	
	{
		MATH_tdst_Vector VD,VDif;
		MATH_tdst_Matrix Matrix;
		Mtx mtx;
		VD = *MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix);
		VD.z = 0.0f;
		MATH_NormalizeEqualVector(&VD);
		MATH_MakeOGLMatrix(&Matrix, GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
		mtx[0][0] = MATH_f_DotProduct(MATH_pst_GetXAxis(&Matrix) , &VD) / WTR_FOG_ScaleDepth;
		mtx[0][1] = MATH_f_DotProduct(MATH_pst_GetYAxis(&Matrix) , &VD) / WTR_FOG_ScaleDepth;
		mtx[0][2] = MATH_f_DotProduct(MATH_pst_GetZAxis(&Matrix) , &VD) / WTR_FOG_ScaleDepth;
		mtx[1][0] = Matrix.Iz / WTR_FOG_ScaleZ;
		mtx[1][1] = Matrix.Jz / WTR_FOG_ScaleZ;
		mtx[1][2] = Matrix.Kz / WTR_FOG_ScaleZ;

		MATH_SubVector(&VDif , &GDI_gpst_CurDD->st_Camera.st_Matrix.T , &GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix->T);
		VDif .z = 0.0f;
		mtx[0][3] = - MATH_f_DotProduct(&VDif , &VD) / WTR_FOG_ScaleDepth;
		mtx[1][3] = 0.5f + (GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix->T.z - GlobalWaterZ)  / WTR_FOG_ScaleZ;
		
		GXLoadTexMtxImm(&mtx, GX_TEXMTX1,  GX_MTX2x4);
	}
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);	
	
	
	
	GX_GXLoadTexObj(&g_ClipTextures, GX_TEXMAP1);
	GX_GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
// out_reg = (d (op) ((1.0 - c)*a + c*b) + bias(-0.5)) * scale(1);	
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE,GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);//GX_CC_CPREV );
	color.r = color.g = color.b = color.a = 0x03;
	GXSetTevKColor( GX_KCOLOR0,  color );
	GXSetTevKAlphaSel(GX_TEVSTAGE1 , GX_TEV_KCSEL_K0);

	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE,GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_KONST , GX_CA_TEXA);
	GX_GXSetTevOpDirty();
	
	
	GX_GXSetCurrentMtx( GX_PNMTX1);
	
	GX_GXSetCullMode(GX_CULL_NONE);
	GXI_SetDestReflextion();
	
	GX_GXSetAlphaCompare(g_GXI_HW_States.acCompare1, g_GXI_HW_States.acVal1, GX_AOP_AND , GX_NEQUAL , 0xff);
	
	
//	GX_GXSetAlphaCompare(GX_NEQUAL , 0xff , GX_AOP_AND , GX_NEQUAL , 0xff);
	GX_GXSetZCompLoc(GX_FALSE);
	
	GXCallDisplayList(_pst_Element->dl, _pst_Element->dl_size & 0x0fffffff);

	GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
	GX_GXSetNumTexGens(1);
	GX_GXSetNumTevStages(1);
	GX_GXSetCurrentMtx( GX_PNMTX0);
	GXI_SetDestNormal();
#else // REFLECTION
	GXCallDisplayList(_pst_Element->dl, _pst_Element->dl_size & 0x0fffffff); // Normal	mode
#endif // REFLECTION
	
}

#ifdef REFLECTION

void GXI_SetDestReflextion()
{
	u32 Height,RealHeight;
	Height = WatertexHeight;
	RealHeight = FrameBufferHeight - Height;
	GXSetViewport(0.0f, (f32)RealHeight, (f32)WatertexWidth, (f32)(Height), 0.0f, 1.0f);
    GXSetScissor(0, (f32)RealHeight, (u32)WatertexWidth, (u32)(Height));
}
void GXI_SetDestNormal()
{
	u32 Height,RealHeight;
	Height = WatertexHeight;
	RealHeight = FRAME_BUFFER_HEIGTH;//FrameBufferHeight - Height + WIDE_SCREEN_ADJUST;
	if (GXI_Global_ACCESS(bRendering2D))
	{
	    GXSetViewport(0.0f, 0.0f, (f32)FRAME_BUFFER_WIDTH, (f32)RealHeight, 0.0f, 1.0f);
	    GXSetScissor(0, 0, (u32)FRAME_BUFFER_WIDTH, (u32)RealHeight);//*/
    } else
	{
	    GXSetViewport(0.0f, 0.0f, (f32)FRAME_BUFFER_WIDTH, (f32)RealHeight, 0.0f, 1.0f);
	    GXSetScissor(0, 0, (u32)FRAME_BUFFER_WIDTH, (u32)RealHeight - (u32)(WIDE_SCREEN_ADJUST * 0.5f));//*/
    } 
}
void GXI_GetBackReflectionBuffer(GXBool gxClear)
{
	if (gxClear)
	{
		GXSetCopyClear(GXI_Global_ACCESS(FogParams).FogColor, GX_MAX_Z24);
	    GXSetTexCopySrc(0, FrameBufferHeight - WatertexHeight, WatertexWidth, WatertexHeight);
		GXSetTexCopyDst(WatertexWidth, WatertexHeight, GX_TF_RGBA8, GX_FALSE );
	    GXCopyTex(GXGetTexObjData(&g_WaterTextures), GX_FALSE);
	    // Wait for finishing the copy task in the graphics pipeline
	    GXPixModeSync();
    } else
    {
	    extern GXRenderModeObj *g_pst_mode;
	    u8 VFilterBLUR[7] = {0x0A , 0x0A , 0x0C , 0x00 , 0x0C , 0x0A , 0x0A};
		GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE,VFilterBLUR);
		GXSetCopyClear(GXI_Global_ACCESS(FogParams).FogColor, GX_MAX_Z24);
	    GXSetTexCopySrc(0, FrameBufferHeight - WatertexHeight, WatertexWidth, WatertexHeight);
		GXSetTexCopyDst(WatertexWidth, WatertexHeight, GX_CTF_R8 , GX_FALSE );
	    GXCopyTex(GXGetTexObjData(&g_WaterTextures_SUN), GX_TRUE);
	    // Wait for finishing the copy task in the graphics pipeline
	    GXPixModeSync();
		GXSetCopyFilter(g_pst_mode->aa, g_pst_mode->sample_pattern, GX_TRUE, g_pst_mode->vfilter);
    }
}

void WATER_CreateViewMapping(Mtx *mDest , float Scale)
{
	u32 adflg;
	Mtx mtx,m;
	float fScreenRatio;
	adflg = 0;
	Scale *= 0.5f;
	fScreenRatio = GXI_Global_ACCESS(fScreenRatio);
	MTXScaleApply( 
		*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), 
		&mtx, 
		Scale * GXI_Global_ACCESS(fFocale), 
		Scale * GXI_Global_ACCESS(fFocale) / fScreenRatio, 
		1.0f);
	L_memset(&m , 0 , sizeof(m));
    m[0][0] =  1;
    m[0][2] =  - 0.5;
    m[1][1] =  -1;
    m[1][2] =  - 0.5;
    m[2][2] = -1.0f;
    MTXConcat(m, mtx, m);	
	L_memcpy(mDest , &m , sizeof(m));
}
void WATER_CreateWorldXY(Mtx *mDest , float Scale)
{
	MTXIdentity ( mDest ); // No scale in water
	MTXScaleApply( mDest , mDest, Scale, Scale, Scale);
	(*mDest)[0][3] = 0.0;
	(*mDest)[1][3] = 0.0;
	(*mDest)[2][3] = 0.0;
}
void GXI_Get4LightPoints(MATH_tdst_Vector *p4Points,MATH_tdst_Vector *pLPos, MATH_tdst_Vector *pCPos)
{
	MATH_tdst_Vector Verticale,Horizontale,H,V;
	float Distance;
	MATH_InitVector(&Verticale , 0.0,0.0,1.0);
	MATH_SubVector(&Horizontale , pCPos , pLPos);
	Distance = MATH_f_NormVector(&Horizontale);
	MATH_CrossProduct2(&H , &Horizontale , &Verticale);
	MATH_CrossProduct2(&V , &H , &Horizontale);
	MATH_NormalizeVector(&H , &H);
	MATH_NormalizeVector(&V , &V);
	MATH_ScaleVector(&H , &H , Distance / 2.0f);
	MATH_ScaleVector(&V , &V , Distance / 1.0f);
	MATH_SubVector(p4Points+0, pLPos , &H );
	MATH_AddVector(p4Points+1, pLPos , &H );
	MATH_AddVector(p4Points+2, pLPos , &H );
	MATH_SubVector(p4Points+3, pLPos , &H );

	MATH_AddVector(p4Points+0, p4Points+0 , &V );
	MATH_AddVector(p4Points+1, p4Points+1 , &V );
	MATH_SubVector(p4Points+2, p4Points+2 , &V );
	MATH_SubVector(p4Points+3, p4Points+3 , &V );
}




void WTR_BlurVertexMapNRM( MATH_tdst_Vector *p_VertexMapNRM , u32 SX , u32 SY)
{
	u32 YC,XC,BC;
	MATH_tdst_Vector *p_VNRM , *p_VNRMLst , *p_VNRMLstLst;
	BC = 2;
	while (BC--)
	{
		p_VNRM = p_VertexMapNRM;
		p_VNRMLstLst = p_VertexMapNRM + SX * (SY - 1);
		while (p_VNRM < p_VNRMLstLst)
		{
			p_VNRMLst = p_VNRM + SX - 1;
			while (p_VNRM < p_VNRMLst)
			{
				p_VNRM->x = (p_VNRM->x + (p_VNRM + 1) ->x + (p_VNRM + SX)->x) * 0.333333f;
				p_VNRM->y = (p_VNRM->y + (p_VNRM + 1) ->y + (p_VNRM + SX)->y) * 0.333333f;
				p_VNRM->z = (p_VNRM->z + (p_VNRM + 1) ->z + (p_VNRM + SX)->z) * 0.333333f;
				p_VNRM++;
			}
			p_VNRM++;
		}
	}

}
void GXI_RenderLights()
{
	GXColor White,Col0;
	MATH_tdst_Vector VZ,VP;
	MATH_tdst_Matrix mtx;
	LIGHT_tdst_List *_pst_LightList;
	OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;
	LIGHT_tdst_Light *LIGHT_gpst_Cur;
	SOFT_tdst_UV p4PointsUV[4];
	p4PointsUV[0].u = p4PointsUV[3].u = p4PointsUV[0].v = p4PointsUV[1].v = 0.0f;
	p4PointsUV[1].u = p4PointsUV[2].u = p4PointsUV[2].v = p4PointsUV[3].v = 1.0f;
	_pst_LightList = &GDI_gpst_CurDD->st_LightList;
    ppst_LightNode = _pst_LightList->dpst_Light;
	ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
	
	
	GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	White.a = 0xff;
	White.r = 0xff;
	White.g = 0xff;
	White.b = 0xff;
	GX_GXSetChanMatColor( GX_COLOR0A0, White );
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_ENABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_REG,		// mat source
    			   0,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function

	GX_GXSetNumChans(1);
	GX_GXSetNumTevStages(1);
	GX_GXSetNumTexGens(1);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE );
	

	GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP); // Add
//	GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP); // copy
	
	
	GX_GXLoadTexObj(&g_WaterPerturbAlphaTexture, GX_TEXMAP0);
	
	MATH_SetIdentityMatrix(&mtx);
	GXLoadTexMtxImm(&mtx, GX_TEXMTX0,  GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXI_set_fog(FALSE);
	GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
	GX_GXSetCullMode(GX_CULL_NONE);
    GX_GXSetZMode(GX_TRUE, GX_LEQUAL, GX_DISABLE);
	
	GXI_SetDestReflextion();
	GX_GXSetCurrentMtx( GX_PNMTX1);
	
	while(ppst_LightNode < ppst_Last)
	{
	    LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;
        {
            // calcul de la lumière selon son type 
		    if ((LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_OceanSpecular) || (
		    	((((LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni) || 
		    	((LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)) &&
		    	 ((LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_RealTimeOnDynam|LIGHT_Cul_LF_Active)) == (LIGHT_Cul_LF_RealTimeOnDynam|LIGHT_Cul_LF_Active)) )))
		    {
				MATH_tdst_Vector p4Points[4];
				u32 Overrender;
#ifdef DONT_USE_ALPHA_DEST
				Col0.r = Col0.g = Col0.b = Col0.a = 0xff;
#else
				Col0.r = Col0.g = Col0.b = 0;
				Col0.a = 0xff;
#endif				
				// LIGHT 0
				GXI_Get4LightPoints(p4Points,&(*ppst_LightNode)->pst_GlobalMatrix->T, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
				
#define GXPos(a) GXPosition3f32(a.x, a.y, a.z)
#define GXUV(a) GXTexCoord2f32(a.u,a.v)
				Overrender = 1;
			   	GXBegin(GX_QUADS, GX_VTXFMT0, 4 * Overrender);
			   		while (Overrender--)
			   		{
					   	GXPos(p4Points[0]);
				    	GXColor1u32(*(u32 *)&Col0);
					   	GXUV(p4PointsUV[0]);

					   	GXPos(p4Points[1]);
				    	GXColor1u32(*(u32 *)&Col0);
					   	GXUV(p4PointsUV[1]);

					   	GXPos(p4Points[2]);
				    	GXColor1u32(*(u32 *)&Col0);
					   	GXUV(p4PointsUV[2]);

					   	GXPos(p4Points[3]);
				    	GXColor1u32(*(u32 *)&Col0);
					   	GXUV(p4PointsUV[3]);
				   	}

			    GXEnd();
            }
        }
		ppst_LightNode++;
	}
	GXI_SetDestNormal();
	GX_GXSetCurrentMtx( GX_PNMTX0);
}
#endif // REFLECTION

/*
	0 - 1
	3 - 2
*/
#define DE_ENTERLACE 1
void GXI_WTR_ComputeDisruptLine(s16 *pLineT0 , s16 *pLineOTHER , s16 *pLineTM1_2 , s16 *pLineTM1_3 , u16 Lenght)
{
}
void GXI_WTR_ComputeDisruptMap()
{
}
void WATER_Plaque(MATH_tdst_Vector *pCamPos , MATH_tdst_Vector *pCamDir , float AlphaX , float AlphaY , float WaterZ)
{
}

void WATER_RenderVertexMap()
{
}

#define GXI_WTR_MOD_DISCX(a) (a)//((a) % SX)

u32 WTR_GetDL_Size(u32 DX , u32 DY)
{
	return (((DY - 1) * (DX * 2 + 2))/* nbindexes */ * 2/* SHORT */ * 5 /* Colors + POS + NRM + uv + uv2 */ + (DY - 1) * 3 * 2 /* NB_Strips */);
}

void GXI_CreateWaterWaves()
{
}




void WATER_ComputeDisplayList(unsigned char *pDL , u32 SX , u32 SY)
{
	unsigned long ulNbStrips , StripCnt , StripCntLast;
	unsigned char *Stream;
	Stream = pDL;
	StripCnt = 0;
	for (ulNbStrips = 0 ; ulNbStrips < (SY - 1) ; ulNbStrips++)
	{
		*(Stream++) = (GX_TRIANGLESTRIP | GX_VTXFMT0); 	// Type of primitiv
		*(u16 *)Stream = SX; 					// Number of indexes 
		Stream += 2;
		StripCntLast = StripCnt + (SX >> 1);				// Indexes
		while (StripCnt < StripCntLast)
		{
			*(u16 *)Stream = StripCnt; 
			Stream += 2;
			*(u16 *)Stream = GXI_WTR_MOD_DISCX(StripCnt); // CLR
			Stream += 2;
			*(u16 *)Stream = StripCnt; // TEX1 
			Stream += 2;
			*(u16 *)Stream = StripCnt + SX; // POS
			Stream += 2;
			*(u16 *)Stream = GXI_WTR_MOD_DISCX(StripCnt + SX); // CLR
			Stream += 2;
			*(u16 *)Stream = StripCnt + SX; // TEX1
			Stream += 2;
			StripCnt++;
		}
		*(Stream++) = (GX_TRIANGLESTRIP | GX_VTXFMT0); 	// Type of primitiv
		*(u16 *)Stream = SX + 2; 					// Number of indexes 
		Stream += 2;
		StripCntLast = StripCnt + (SX >> 1);				// Indexes
		StripCnt --;
		while (StripCnt < StripCntLast)
		{
			*(u16 *)Stream = StripCnt + SX; // POS
			Stream += 2;
			*(u16 *)Stream = GXI_WTR_MOD_DISCX(StripCnt + SX); // CLR
			Stream += 2;
			*(u16 *)Stream = StripCnt + SX; // TEX1
			Stream += 2;
			*(u16 *)Stream = StripCnt; 
			Stream += 2;
			*(u16 *)Stream = GXI_WTR_MOD_DISCX(StripCnt); // CLR
			Stream += 2;
			*(u16 *)Stream = StripCnt; // TEX1
			Stream += 2;
			StripCnt++;
		}
	}
	while ((u32)Stream & 31) *Stream++ = 0; 			// GX NOP
	DCFlushRange(pDL, (u32)(Stream - pDL));

}


void GXI_RenderWATER(unsigned char *DL , u32 DLSize , u32 SX ,u32 SY , MATH_tdst_Vector *p_VertexMap , SOFT_tdst_UV *p_VertexMap_UV , u32 *pColors , MATH_tdst_Vector *p_Texture , u32 TexPo2, u32 _ulTextureIndex)
{

	MATH_tdst_Matrix mtx , mtxScreen;
	GXColor black ,White;
	u32 LightNumber;
	ULONG ulFlags; 

	/* Set view matrix */
	GXI_SetViewMatrix(&GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
	
	GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetAlphaUpdate(GX_TRUE);
	
	
	ulFlags = 0;
	MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
	MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
	MAT_SET_FLAG(ulFlags, MAT_Cul_Flag_NoZWrite|MAT_Cul_Flag_Bilinear|MAT_Cul_Flag_TileU|MAT_Cul_Flag_TileV);	
	
	// On met la texture d'écume dans GX_TEXMAP0.
	GXI_SetTextureBlending(_ulTextureIndex, ulFlags , 0);
	
	//GXI_set_color_generation_parameters(TRUE,TRUE);
	GX_GXSetNumTexGens(1);
	GX_GXSetNumChans(1);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0,GX_TEXMAP0, GX_COLOR0A0);	
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	/* Setup pxl frmt */
	GX_GXSetArray(GX_VA_POS  , p_VertexMap		, sizeof(f32) * 3);
	GX_GXSetArray(GX_VA_CLR0 , pColors			, sizeof(u32)	);
	GX_GXSetArray(GX_VA_TEX0 , p_VertexMap_UV , sizeof(f32) * 2);
	
	DCFlushRange(p_VertexMap, SX * SY * sizeof(MATH_tdst_Vector));//*/
	DCFlushRange(p_VertexMap_UV, SX * SY * sizeof(MATH_tdst_Vector));//*/
	DCFlushRange(pColors, SX * SY * sizeof(u32));//*/
	
	GX_GXSetVtxDesc(GX_VA_POS,  GX_INDEX16);
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
	GX_GXSetVtxDesc(GX_VA_TEX1, GX_NONE);
	
	{	
	Mtx mtxFinal;
	
	mtxFinal[0][0] = 1.0f;
	mtxFinal[0][1] = 0.0f;
	mtxFinal[0][2] = 1.0f;
	mtxFinal[1][0] = 0.0f;
	mtxFinal[1][1] = 1.0f;
	mtxFinal[1][2] = 1.0f;
	
	GX_GXLoadTexMtxImm(mtxFinal, GX_TEXMTX0,  GX_MTX2x4);
	GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
	}	
	
	GX_GXSetCullMode(GX_CULL_NONE);
	GXI_set_fog(TRUE);
	
	GXCallDisplayList(DL, (DLSize + 32) & ~31);
}

