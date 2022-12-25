/*$T GSP_Jungle.c GC! 1.081 05/04/00 15:08:05 */

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

#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"

#include "MATerial/MATstruct.h"


#include "Gsp_Bench.h"
#include "BASe/BENch/Bench.h"

extern void Gsp_RenderBuffer(register Gsp_BigStruct	  *p_BIG , register ULONG Num);

#define S_32		0x0
#define S_16		0x1
#define S_8			0x2
#define V2_32		0x4
#define V2_16		0x5
#define V2_8		0x6
#define V3_32		0x8
#define V3_16		0x9
#define V3_8		0xA
#define V4_32		0xC
#define V4_16		0xD
#define V4_8		0xE
#define V4_5		0xF

static 	u_int	VIF_BASE = 0;

extern void GSP_ArtisanalFlushAll();
#ifdef PSX2_TARGET
u_long128 gspg2[10] ONLY_PSX2_ALIGNED(16);
void GSP_SPG2_PrepareCamra(SPG2_tdst_Modifier *_pst_SPG2 , u_long128 *p_stWind , float f_LOD)
{
	MATH_tdst_Vector *p_Local;
	float Coef , fExtractionOfHorizontalPlane,OoFogMax,OoFogMin,FogFactor;
	register u_long128 *pBuffer128,*pLocal;
	register Gsp_BigStruct	  *p_BIG;
	u32 NumberOfSegments;
	p_BIG = &gs_st_Globals;
/*	Gsp_RenderBuffer(&gs_st_Globals,p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	GSP_ArtisanalFlushAll();*/


	GSP_Vu1LoadCode(( u_int )&My_Jungle_code);
	if ((p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase) > RAM_BUF_SIZE - 1024) 
		Gsp_RenderBuffer(p_BIG,p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	
	Coef = (float)GIGSCALE0 / (float)GspGlobal_ACCESS(ZFactor) ;
	 fExtractionOfHorizontalPlane = _pst_SPG2->fExtractionOfHorizontalPlane + 0.5f;
	 
	
	p_Local = (MATH_tdst_Vector *)&gspg2[0];
	*p_Local = *MATH_pst_GetXAxis(GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
	p_Local->x *= Coef * (float)((float)((u_int)GspGlobal_ACCESS(Xsize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale));
	p_Local->y *= Coef * ((u_int)GspGlobal_ACCESS(Ysize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale) * GspGlobal_ACCESS(YCorrectionCoef);
	p_Local->z *= Coef ;

	p_Local = (MATH_tdst_Vector *)&gspg2[1];
	*p_Local = *MATH_pst_GetYAxis(GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
	p_Local->x *= Coef * (float)((float)((u_int)GspGlobal_ACCESS(Xsize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale));
	p_Local->y *= Coef * ((u_int)GspGlobal_ACCESS(Ysize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale) * GspGlobal_ACCESS(YCorrectionCoef);
	p_Local->z *= Coef ;

	p_Local = (MATH_tdst_Vector *)&gspg2[2];
	*p_Local = *MATH_pst_GetZAxis(GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
	p_Local->x *= Coef * (float)((float)((u_int)GspGlobal_ACCESS(Xsize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale));
	p_Local->y *= Coef * ((u_int)GspGlobal_ACCESS(Ysize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale) * GspGlobal_ACCESS(YCorrectionCoef);
	p_Local->z *= Coef ;
	p_Local++;
	p_Local->x = Coef * Coef;

	p_Local = (MATH_tdst_Vector *)&gspg2[3];
	*p_Local = *MATH_pst_GetTranslation(GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
	p_Local->x *= Coef * (float)((float)((u_int)GspGlobal_ACCESS(Xsize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale));
	p_Local->y *= Coef * ((u_int)GspGlobal_ACCESS(Ysize) >> 1L) * GspGlobal_ACCESS(fCurrentFocale) * GspGlobal_ACCESS(YCorrectionCoef);
	p_Local->z *= Coef ;
	
    if (MATH_b_TestScaleType( GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix ))
    {
    	MATH_ScaleEqualVector((MATH_tdst_Vector *)&gspg2[0] , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix -> Sx);
    	MATH_ScaleEqualVector((MATH_tdst_Vector *)&gspg2[1] , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix -> Sy);
    	MATH_ScaleEqualVector((MATH_tdst_Vector *)&gspg2[2] , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix -> Sz);
    }
	

	NumberOfSegments = lMin(_pst_SPG2->NumberOfSegments , 8);
	
	/* Set add screen */
	p_Local = (MATH_tdst_Vector *)&gspg2[5];
	p_Local->x = (float)(GspGlobal_ACCESS(BX0) + ((u_int)GspGlobal_ACCESS(Xsize) >> 1L));
	p_Local->y = (float)(GspGlobal_ACCESS(BY0) + ((u_int)GspGlobal_ACCESS(Ysize) >> 1L));
	p_Local->z = 0.0f;
	// Fog Add (after multiplication)
	OoFogMax = 1.0f / (Coef * GspGlobal_ACCESS(FogZFar));
	OoFogMin = 1.0f / (Coef * GspGlobal_ACCESS(FogZNear));
	FogFactor = 255.0f / ((OoFogMin - OoFogMax)); //W = FOG MULTIPLICATION
	(p_Local+1)->x = -FogFactor * OoFogMax;
	/* Set max screen */
	p_Local =  (MATH_tdst_Vector *)&gspg2[6];
	p_Local->x = (GspGlobal_ACCESS(BX0) - 1024);
	p_Local->y = (GspGlobal_ACCESS(BY0) - 1024);
	p_Local->z = 0.0f;
	// Fog Max
	(p_Local+1)->x = 0.0f;
//	p_Local->w = ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP / 2.0f; // == P0
	/* Set min screen */
	p_Local = (MATH_tdst_Vector *)&gspg2[7];
	p_Local->x = (GspGlobal_ACCESS(BX0) + GspGlobal_ACCESS(Xsize) + 1024);
	p_Local->y = (GspGlobal_ACCESS(BY0) + GspGlobal_ACCESS(Ysize) + 1024);
	p_Local->z = 0.0f;
	// Fog Min 
	(p_Local+1)->x = 255.0f;
	/* Set Oonseg_fRatio_fEOHP_VOID */
	p_Local = (MATH_tdst_Vector *)&gspg2[8]; // Oonseg_fRatio_fEOHP_VOID
	p_Local->x = 1.0f / (float)NumberOfSegments;
	p_Local->y = _pst_SPG2->f_GlobalRatio * 0.5f;
	p_Local->z = fExtractionOfHorizontalPlane;//(float)(1<<20);

	VIF_BASE = 0;

	pBuffer128 = (u_int128*)p_BIG->p_CurrentBufferPointer;

	/* Setup Vif header decoder 	*/
	
	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_MSCALF(0,0);
	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_FLUSH(0);
	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_STCYCL(1,1,0);
	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_UNPACK(VIF_BASE, 20 , V4_32, 0);
	
	/* Begin unpack */
// 4
	*(pBuffer128++) = *(p_stWind++); // WIND
	*(pBuffer128++) = *(p_stWind++); // WIND
	*(pBuffer128++) = *(p_stWind++); // WIND
	*(pBuffer128++) = *(p_stWind++); // WIND
// 8
	*(pBuffer128++) = *(p_stWind++); // WIND
	*(pBuffer128++) = *(p_stWind++); // WIND
	*(pBuffer128++) = *(p_stWind++); // WIND
	*(pBuffer128++) = *(p_stWind++); // WIND
// 12
	/* Setup header 				*/
	pLocal = &gspg2;
	*(pBuffer128++) = pLocal[0]; // CAMERA_I 		
	*(pBuffer128++) = pLocal[1]; // CAMERA_J 		
	*(pBuffer128++) = pLocal[2]; // CAMERA_K 		
	*(pBuffer128++) = pLocal[3]; // CAMERA_S 		
// 13
// SprteGeneratorRadius_OoSpriteNumber_SpriteSize_SqrEoHpO2
	*(((float *)pBuffer128)++) = _pst_SPG2->f_SpriteGeneratorRadius;
	*(((float *)pBuffer128)++) = 1.0f / (float)_pst_SPG2->NumberOfSprites;
	*(((float *)pBuffer128)++) = 0.5f * (float)_pst_SPG2->f_SpriteSize * f_LOD * Coef * GspGlobal_ACCESS(fCurrentFocale) * (float)((u_int)GspGlobal_ACCESS(Xsize) >> 1L); // == P1
	*(((float *)pBuffer128)++) = fExtractionOfHorizontalPlane * fExtractionOfHorizontalPlane / 2.0f; // == P0;
// 16
	*(pBuffer128++) = pLocal[5]; // ADD_SCREEN		
	*(pBuffer128++) = pLocal[6]; // MAX_SCREEN		
	*(pBuffer128++) = pLocal[7]; // MIN_SCREEN		
// 17
	/* Set gif tag for FOG */
	Gsp_M_SetGifTag((GspGifTag *)pBuffer128, 1 , 1 , 1, 4 , SCE_GIF_PACKED , 1 ,   0xa);
	pBuffer128++;
	
	
// 18
	*(pBuffer128++) = pLocal[8]; // Oonseg_fRatio_fEOHP_VOID
	/* Set gif tag */
	Gsp_M_SetGifTag((GspGifTag *)pBuffer128, 0 , 1 , 1, 4 , SCE_GIF_PACKED , 3 ,   0x512);
// 19
	pBuffer128++;

	
// 20
	/* COSAS_SINAS_One_FOGMul */
	if (_pst_SPG2 ->ulFlags & SPG2_SpriteRotation)
	{
		*(((float *)pBuffer128)++) = -0.34202014332566873304409961468226f ; //Cos Alpha fro sprites
		*(((float *)pBuffer128)++) = 0.9396926207859083840541092773247f; //Sin Alpha fro sprites
	} else
	{
		*(((float *)pBuffer128)++) = -1.0f ; //Cos Alpha fro sprites
		*(((float *)pBuffer128)++) = 0.0f; //Sin Alpha fro sprites
	}
	
	*(((float *)pBuffer128)++) = 1.0f; //Z
	*(((float *)pBuffer128)++) = FogFactor;

	/* end unpack */

	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_MARK(0xcd03,0);
	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_NOP(0);
	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_FLUSH(0);
	*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_MSCNT(0);
	
	p_BIG->p_CurrentBufferPointer = (u_int)pBuffer128;


/*	GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , (((u_int)p_BIG->p_CurrentBufferPointer - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1, 0 , p_BIG->BX_LastCntPtr);
	*(u_int *)&p_BIG->BX_LastCntPtr = ((u_int)p_BIG->p_CurrentBufferPointer);
	*/
}

#define VIF_PACK_SIZE_MAX (64)
u_int Statistic_NumberOfBranches = 0;
 void GSP_DrawSPG2_Local(	SOFT_tdst_AVertex			        *Coordinates,
							ULONG								*pColors,
							ULONG								ulMode,
							ULONG								ulnumberOfPoints,
							GEO_Vertex					        *XCam,
							GEO_Vertex					        *YCam,
							SPG2_tdst_Modifier					*_pst_SPG2,
							SOFT_tdst_AVertex					*p_stWind,
							SPG2_InstanceInforamtion			*p_II							
							)
{
	register u_long128 *pBuffer128,*pBuffer128Last;
	register Gsp_BigStruct	  *p_BIG;
	register ULONG	ulNumberOfSegments; 
	p_BIG = &gs_st_Globals;
	if (p_BIG->bSRSMustBeUodate) 
	{
		GSP_SetTextureBlending12(p_BIG);
	}//*/	
	
	ulNumberOfSegments = lMin(_pst_SPG2->NumberOfSegments , 8);
	if (ulNumberOfSegments>>1)
	{
		ulNumberOfSegments = (ULONG)((float)ulNumberOfSegments * p_II->Culling);
		if (ulNumberOfSegments <= 2) ulNumberOfSegments = 2;
	}


	/* While number of points 			*/
	while (ulnumberOfPoints)
	{
		u_int ulSize,ulnumberOfPointsLocal;
		ulnumberOfPointsLocal = lMin(VIF_PACK_SIZE_MAX , ulnumberOfPoints);
		ulnumberOfPoints -= ulnumberOfPointsLocal;

		/* Compute & Allocate size 		*/
		ulSize = 16 * 4/*Points*/+ 32 /* params */ + ulnumberOfPointsLocal * 4 + 16/*Colors + align*/ + 16 * 5/* VIF codes*/ + 64;
		if ((p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase) + ulSize > RAM_BUF_SIZE - 1024) 
			Gsp_RenderBuffer(p_BIG,p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
		pBuffer128 = (u_int128*)p_BIG->p_CurrentBufferPointer;
		/* Copy vertices in 2 parts to avoid the >= 256 unpacks */
		/* Setup Vif Vertices decoder part 1 */
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_NOP(0); // Keep Alignment
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_NOP(0); // Keep Alignment
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_STCYCL(1,1,0); // Keep Alignment
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_UNPACK(VIF_BASE, ulnumberOfPointsLocal * 2 + 4, V4_32, 0);

		/* First Point is Parameters */
		*(((u_int *)pBuffer128)++) = ulnumberOfPointsLocal ; // NumberOfPoints	
		*(((u_int *)pBuffer128)++) = _pst_SPG2->TileNumber ; // TileNumber		
		*(((u_int *)pBuffer128)++) = ulNumberOfSegments ; // NumberOfSegment	
		*(((u_int *)pBuffer128)++) = _pst_SPG2->fTrapeze ; // trapeze	

		/* second Point is Parameters */
		*(((u_int *)pBuffer128)++) = ulMode ; // NumberOfPoints	
		*(((u_int *)pBuffer128)++) = _pst_SPG2->NumberOfSprites ; // 
		/* Anim U & V */
		*(((u_int *)pBuffer128)++) = (127 - ((p_II->BaseAnimUv >> 24) & 0xf)) << 23;
		*(((u_int *)pBuffer128)++) = (127 - ((p_II->BaseAnimUv >> 28) & 0xf)) << 23;
		
		/* 3 Point is GlobalPos + GlobalScale */
		*(((float *)pBuffer128)++) = p_II->GlobalPos.x ; // 
		*(((float *)pBuffer128)++) = p_II->GlobalPos.y ; // 
		*(((float *)pBuffer128)++) = p_II->GlobalPos.z ; // 
		*(((float *)pBuffer128)++) = p_II->GlobalSCale ; // 
		
		/* 3 Point is GlobalPos + AnimPos */
		*(((float *)pBuffer128)++) = p_II->GlobalZADD.x ; // 
		*(((float *)pBuffer128)++) = p_II->GlobalZADD.y ; // 
		*(((float *)pBuffer128)++) = p_II->GlobalZADD.z ; // 
		
		if (p_II->BaseAnimUv)
		{
			*(((float *)pBuffer128)++) = (float)((p_II->BaseAnimUv >> 16) & 0xff) ; // 
		} else
			*(((float *)pBuffer128)++) = 0.0f ; // 
		
		p_BIG->p_CurrentBufferPointer = (u_int)pBuffer128;
		Gsp_Send_To_VU1_Ref((u_int)Coordinates , ulnumberOfPointsLocal * 2);
		(u_long128 *)Coordinates += ulnumberOfPointsLocal * 2;
		pBuffer128 = (u_int128*)p_BIG->p_CurrentBufferPointer;
		/* Setup Vif Vertices decoder part 2 */
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_NOP(0); // Keep Alignment
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_NOP(0); // Keep Alignment
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_MARK(0xcd01,0); // Keep Alignment
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_UNPACK(VIF_BASE + ulnumberOfPointsLocal * 2 + 4, ulnumberOfPointsLocal * 2, V4_32, 0);

		p_BIG->p_CurrentBufferPointer = (u_int)pBuffer128;
		Gsp_Send_To_VU1_Ref((u_int)Coordinates , ulnumberOfPointsLocal * 2);
		(u_long128 *)Coordinates += ulnumberOfPointsLocal * 2;
		pBuffer128 = (u_int128*)p_BIG->p_CurrentBufferPointer;
		
		VIF_BASE += 4 * VIF_PACK_SIZE_MAX;

		/* Setup Vif colors decoder 	*/
 		ulSize = ulnumberOfPointsLocal;
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_UNPACK(VIF_BASE + 4, ulSize, V4_8, 0);
		/* Copy Color 					*/
		if (!p_II->GlobalColor)
			while (ulSize--) *(((u_int *)pBuffer128)++) = *(pColors++);
		else
			while (ulSize--) *(((u_int *)pBuffer128)++) = p_II->GlobalColor;
		


		/* vif continue 				*/
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_FLUSH(0);
		*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_MSCNT(0);
		while ((u_int)pBuffer128 & 0x0f) 
			*(((u_int *)pBuffer128)++) = SCE_VIF1_SET_MARK(0xcd02,0);


		p_BIG->p_CurrentBufferPointer = (u_int)pBuffer128;
		VIF_BASE &= 512;
		VIF_BASE ^= 512;
	}
	/* While number of points finished ?*/
//	 GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
}

#define CosAlpha -0.34202014332566873304409961468226f	
#define SinAlpha 0.9396926207859083840541092773247f		

void GSP_DrawSPG2(	SPG2_CachedPrimitivs				*pCachedLine,
							ULONG								*ulTextureID,
							GEO_Vertex					        *XCam,
							GEO_Vertex					        *YCam,
							SPG2_tdst_Modifier					*_pst_SPG2,
							SOFT_tdst_AVertex					*p_stWind,
							SPG2_InstanceInforamtion			*p_II
							)
{
	register Gsp_BigStruct	  *p_BIG;
	u_int BM,NumberOfPoints;
	p_BIG = &gs_st_Globals;
	_GSP_BeginRaster(42);
	BM = 0;

	NumberOfPoints = pCachedLine->a_PtrLA2>>2;

	Statistic_NumberOfBranches += NumberOfPoints;
	

	if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
	{
		MAT_SET_FLAG(BM, MAT_Cul_Flag_InvertAlpha | MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV | MAT_Cul_Flag_NoZWrite);
		MAT_SET_Blending(BM , MAT_Cc_Op_Add);
    } else
	if (_pst_SPG2 ->ulFlags & SPG2_DrawinAlpha)  
	{
		MAT_SET_FLAG(BM, MAT_Cul_Flag_NoZWrite | MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_AlphaTest | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV);
		MAT_SET_Blending(BM , MAT_Cc_Op_Alpha);
		MAT_SET_AlphaTresh(BM , _pst_SPG2->AlphaThreshold);
    } else
    {
		MAT_SET_FLAG(BM, MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_AlphaTest | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV);
		MAT_SET_Blending(BM , MAT_Cc_Op_Copy);
		MAT_SET_AlphaTresh(BM , _pst_SPG2->AlphaThreshold);
    }
	if ((_pst_SPG2->ulFlags & SPG2_XAxisIsInlookat) && (pCachedLine->ulFlags & 2))
	{
		ULONG Counter;
		SOFT_tdst_AVertex	*Coordinates;
		MATH_tdst_Vector	stCameraDir;
		_pst_SPG2->ulFlags &= ~(SPG2_DrawY|SPG2_RotationNoise);
		_pst_SPG2->ulFlags |= SPG2_DrawX;
		pCachedLine->ulFlags &= ~2;
		Coordinates = pCachedLine->a_PointLA2;
		Counter = NumberOfPoints;
		MATH_CrossProduct(&stCameraDir,(MATH_tdst_Vector *)YCam,(MATH_tdst_Vector *)XCam);
		while (Counter --)
		{
			MATH_tdst_Vector	LocalX ;
			MATH_CrossProduct(&LocalX , &stCameraDir,(MATH_tdst_Vector *)(Coordinates + 3));
			MATH_NormalizeEqualVector(&LocalX);
			*(MATH_tdst_Vector *)(Coordinates + 2) = LocalX ;
			Coordinates+=4;
		}
	} 
    else
	/* "Random" rotation ***********************/
	if ((_pst_SPG2->ulFlags & SPG2_RotationNoise) && (pCachedLine->ulFlags & 1))
	{
		ULONG Counter;
		float					CosV,SinV,Swap;
		SOFT_tdst_AVertex	*Coordinates;
		CosV  = 1.0f;
		SinV  = 0.0f;
		Coordinates = pCachedLine->a_PointLA2;
		Counter = NumberOfPoints;
		while (Counter --)
		{
			MATH_tdst_Vector	LocalX ,LocalY ;
			Swap = CosAlpha * CosV + SinAlpha * SinV;
			SinV = SinAlpha * CosV - CosAlpha * SinV;
			CosV = Swap ;
			MATH_ScaleVector(&LocalX , (MATH_tdst_Vector *)(Coordinates + 1) , CosV );
			MATH_AddScaleVector(&LocalX , &LocalX , (MATH_tdst_Vector *)(Coordinates + 2) , SinV );
			MATH_ScaleVector(&LocalY , (MATH_tdst_Vector *)(Coordinates + 2) , CosV );
			MATH_AddScaleVector(&LocalY , &LocalY , (MATH_tdst_Vector *)(Coordinates + 1) , -SinV );
			*(MATH_tdst_Vector *)(Coordinates + 1) = LocalX ;
			*(MATH_tdst_Vector *)(Coordinates + 2) = LocalY ;
			Coordinates+=4;
		}
	}
 

	if (_pst_SPG2 ->ulFlags & SPG2_DrawHat)
	{
		GSP_SetTextureBlending(ulTextureID[0], BM , 0);
		GSP_DrawSPG2_Local(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,0,NumberOfPoints,XCam,YCam,_pst_SPG2,p_stWind,p_II);
	}//*/

	if (_pst_SPG2 ->ulFlags & SPG2_DrawX)
	{
		GSP_SetTextureBlending(ulTextureID[2], BM , 0);
		GSP_DrawSPG2_Local(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,1,NumberOfPoints,XCam,YCam,_pst_SPG2,p_stWind,p_II);
	}//*/

	if (_pst_SPG2 -> ulFlags & SPG2_DrawY)
	{
		GSP_SetTextureBlending(ulTextureID[1], BM , 0);
		GSP_DrawSPG2_Local(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,2,NumberOfPoints,XCam,YCam,_pst_SPG2,p_stWind,p_II);
	}//*/

	if (_pst_SPG2 ->ulFlags & SPG2_DrawSprites)
	{
		if (_pst_SPG2->NumberOfSprites)
		{
			GSP_SetTextureBlending(ulTextureID[3], BM , 0);
			GSP_DrawSPG2_Local(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,3,NumberOfPoints,XCam,YCam,_pst_SPG2,p_stWind,p_II);
		}
	}//*/
	
	_GSP_EndRaster(42);

}

void GSP_SPG2_CloseCamra(SPG2_tdst_Modifier					*_pst_SPG2)
{

	/* Re-Load Triangle Code */
//	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
/*	Gsp_RenderBuffer(&gs_st_Globals,gs_st_Globals.p_CurrentBufferPointer - gs_st_Globals.p_BufferPointerBase);
	GSP_ArtisanalFlushAll();*/
}

#endif

