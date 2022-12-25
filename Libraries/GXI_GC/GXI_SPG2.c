
#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "MoDiFier/MDFmodifier_SPG2.h"

//#define NO_SPG2

// Constants
#define CosAlpha -0.34202014332566873304409961468226f 
#define SinAlpha 0.9396926207859083840541092773247f 

#define TEX_FRAC 7


static int iThisFrameDisplayedPointNb = 0;
static int iLastFrameDisplayedPointNb = 0;

// Text coords for sprites : 00, 10, 11, 01
static float GXI_SPG2_gSpriteTextureCoordBuffer[8] = {1.f,1.f,  0.f,1.f,  0.f,0.f,  1.f,0.f};

void GXI_SPG2_OneFrameCall()
{
	// Average few last frames
	iLastFrameDisplayedPointNb = iThisFrameDisplayedPointNb;
	iThisFrameDisplayedPointNb = 0;
}

// For SPG2 "is lighted" == "use normals"
#define	GXI_bIsLighted() ((GXI_Global_ACCESS(LightMask) != GX_LIGHT_NULL) && (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted))

extern void GXI_set_color_generation_parameters(BOOL _bUseClr, BOOL _bUseTex);


void GXI_SPG2SetVertexFormat(SPG2_CachedPrimitivs *pCachedLine,BOOL _bSpriteFormat, BOOL _bUseVtxColor)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	int bUseNormals,bDirectColor,bDirectTextCoord,bDirectPos,bCompressedTextCoordFormat;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	
	bUseNormals = GXI_bIsLighted();
	if (_bSpriteFormat)
	{
		bDirectColor = 1;
		bCompressedTextCoordFormat = 0;
		bDirectPos = 1;
		ERR_X_Assert(!(pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED));
	}
	else
	{
		int bCompressedVertices = pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED;
		bDirectColor = !bCompressedVertices;
		bDirectTextCoord = !bCompressedVertices;
		bDirectPos = !bCompressedVertices;
		bCompressedTextCoordFormat = bDirectTextCoord;
	}
	
		/*if (_bSpriteFormat)
	{
		OSReport("GXPosition3f32\n");
		if (bUseNormals)
			OSReport("GXNormal3f32\n");
		
		if (_bUseVtxColor)
		{
			if (bDirectColor)
				OSReport("GXColor1u32\n");
			else
				OSReport("GXColor1x8 (u32 array)\n");
		}
		
		if (_bSpriteFormat)
		{
			OSReport("GXTexCoord1x8 (f32 array)\n");
		}
		else
		{
			if (bCompressedTextCoordFormat)
				OSReport("GXTexCoord2u8\n");
			else
				OSReport("GXTexCoord2f32\n");
		}
	}*/	
	
	
	// Set description of vertex buffer
	
	// POS
	ERR_X_Assert(bDirectPos);
	//if (bDirectPos)
	{
		GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT); 
		GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	}		
	/*else
	{
		GX_GXSetVtxDesc(GX_VA_POS, GX_INDEX16); 
		GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_S16, pCachedLine->uFrac);
		GX_GXSetArray(GX_VA_POS,pCachedLine->a_VtxBuffer,3*sizeof(tCmpVertex));
	}*/

	// CLR0	
	if (_bUseVtxColor)
	{
		if (bDirectColor)
			GX_GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		else
		{
			GX_GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GX_GXSetArray(GX_VA_CLR0, pCachedLine->a_ColorLA2, 4);
		}
		GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	}
	else
		GX_GXSetVtxDesc(GX_VA_CLR0, GX_NONE); // Ambiant color is used
		
	// NRM
	if (bUseNormals)
	{
		GX_GXSetVtxDesc(GX_VA_NRM,  GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	}
	else
		GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	
	// TEX0
	if (_bSpriteFormat)
	{
		GX_GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
		GX_GXSetArray(GX_VA_TEX0,GXI_SPG2_gSpriteTextureCoordBuffer,2*sizeof(float));
		ERR_X_Assert(!bCompressedTextCoordFormat);
	}
	else
	{
		ERR_X_Assert(bDirectTextCoord);
		//if (bDirectTextCoord)
		{
			GX_GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		}	
		/*else
		{
			GX_GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GX_GXSetArray(GX_VA_TEX0,pCachedLine->a_TextureCoordBuffer,2*sizeof(float));
		}*/
	}
	
	if (bCompressedTextCoordFormat)
		GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_U8, TEX_FRAC);
	else
		GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}


/*void GXI_l_DrawSPG2_Compressed(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	ULONG								ulMode,
	SPG2_InstanceInforamtion			*p_stII
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	u16 iIndexVertex;
	Mtx ModelViewMatrix;
	MATH_tdst_Vector DeltaPosGlob,DeltaPosLoc;
	u8  uTexCoord0,uTexCoord1,uTexCoord2,uTexCoord3;
	u8									indexColor;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	
	// Change Modelview Transform matrix to add the cache line translation.
	MATH_CopyMatrixToGCMatrix(&ModelViewMatrix,&GXI_Global_ACCESS(current_modelview_matrix));
	MATH_AddVector(&DeltaPosLoc,&(pCachedLine->stCenter),&(p_stII->GlobalPos));
	MATH_InvTransformVectorNoScale(&DeltaPosGlob,&ModelViewMatrix,&DeltaPosLoc);
		
	ModelViewMatrix[0][3] += DeltaPosGlob.x;
	ModelViewMatrix[1][3] += DeltaPosGlob.y;
	ModelViewMatrix[2][3] += DeltaPosGlob.z;

	GXLoadPosMtxImm(ModelViewMatrix, GX_PNMTX2);
	GX_GXSetCurrentMtx(GX_PNMTX2);
				
	GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulnumberOfPoints);
		
	// Loop on the primitives
	iIndexVertex = ulMode*4;
	uTexCoord0 = ulMode*4;
	uTexCoord1 = uTexCoord0+1;
	uTexCoord2 = uTexCoord0+2;
	uTexCoord3 = uTexCoord0+3;
	indexColor = 0;
	while (ulnumberOfPoints--)
	{
		GXPosition1x16(iIndexVertex);
		GXColor1x8(indexColor);
		GXTexCoord1x8(uTexCoord0);
		iIndexVertex++;
			
		GXPosition1x16(iIndexVertex);
		GXColor1x8(indexColor);
		GXTexCoord1x8(uTexCoord1);
		iIndexVertex++;

		GXPosition1x16(iIndexVertex);
		GXColor1x8(indexColor);
		GXTexCoord1x8(uTexCoord2);
		iIndexVertex++;
			
		GXPosition1x16(iIndexVertex);
		GXColor1x8(indexColor);
		GXTexCoord1x8(uTexCoord3);
			
		iIndexVertex+= 9; // (1+8)
		indexColor++;
	}
	GXEnd();
}*/



void GXI_l_DrawSPG2_InnerLoopLightedVtxColorDirect(SPG2_CachedPrimitivs *pCachedLine, register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter,u8 indexPoint)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
	register u32	uColor = pCachedLine->a_ColorLA2[indexPoint];
	uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();

}


void GXI_l_DrawSPG2_InnerLoopLightedRegColor(register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();

}

void GXI_l_DrawSPG2_InnerLoopVtxColorDirect(SPG2_CachedPrimitivs *pCachedLine, register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter,u8 indexPoint)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
	register u32	uColor = pCachedLine->a_ColorLA2[indexPoint];
	uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);

	
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();

}


void GXI_l_DrawSPG2_InnerLoopRegColor(register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();

}



void GXI_l_DrawSPG2_DirectLightedInnerLoopVtxColor(SPG2_CachedPrimitivs *pCachedLine, register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter,u8 indexPoint)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
	register u32	uColor = pCachedLine->a_ColorLA2[indexPoint];
	uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);

			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
	
}

void GXI_l_DrawSPG2_DirectLightedInnerLoopRegColor(SPG2_CachedPrimitivs *pCachedLine, register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
	
}

void GXI_l_DrawSPG2_DirectInnerLoopVtxColor(SPG2_CachedPrimitivs *pCachedLine, register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter,u8 indexPoint)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
	register u32	uColor = pCachedLine->a_ColorLA2[indexPoint];
	uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);

			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
	
}

void GXI_l_DrawSPG2_DirectInnerLoopRegColor(SPG2_CachedPrimitivs *pCachedLine, register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW
		}

		GXTexCoord2u8(uTexUBase,uV);

		asm {
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
	
}

/*
void GXI_l_DrawSPG2_IndirectInnerLoopVtxColor(register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter,u8 indexColor)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		GXColor1x8(indexColor);
		GXTexCoord2u8(uTexUBase,uV);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW	
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
				
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXColor1x8(indexColor);
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
	
}
*/

void GXI_l_DrawSPG2_IndirectInnerLoopRegColor(register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter
)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		GXTexCoord2u8(uTexUBase,uV);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW	
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
				
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
	
}

/*void GXI_l_DrawSPG2_IndirectLightedInnerLoopVtxColor(register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter,u8 indexColor)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXColor1x8(indexColor);
		GXTexCoord2u8(uTexUBase,uV);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW	
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
				
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXColor1x8(indexColor);
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
	
}*/

void GXI_l_DrawSPG2_IndirectLightedInnerLoopRegColor(register float *pNormale,register float *pGravity,register float *pVert0,register float *pVert1,register float InterpolatorIntensity,register float fTrapezeInc, float fTrapezeDelta,u8 uV,u8 uDeltaU,u8 uUMax,u8 uTexUBase, int Counter)
{
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp;
	register float Interpolator;
			
	Interpolator = 0.0f;
	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
	asm {
		psq_l       fNormaleXY, 0x00(pNormale), 0, 0     // load stNormale[0], stNormale[1]
		psq_l       fNormaleZW, 0x08(pNormale), 1, 0     // load stNormale[2], 1
		psq_l       fGravityXY, 0x00(pGravity), 0, 0     // load stGravity[0], stGravity[1]
		psq_l       fGravityZW, 0x08(pGravity), 1, 0     // load stGravity[2], 1
		psq_l       fVert0XY, 0x00(pVert0), 0, 0     // load u_4Vert0[0], u_4Vert0[1]
		psq_l       fVert0ZW, 0x08(pVert0), 1, 0     // load u_4Vert0[2], 1
		psq_l       fVert1XY, 0x00(pVert1), 0, 0     // load u_4Vert1[0], u_4Vert1[1]
		psq_l       fVert1ZW, 0x08(pVert1), 1, 0     // load u_4Vert1[2], 1
	}

	// Loop on the segments of the primitives.
	while (	Counter -- )
	{
		asm {
			//MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
			ps_sub		fTempXY, fVert0XY, fVert1XY	
			ps_sub		fTempZW, fVert0ZW, fVert1ZW
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert0XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert0ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}

		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXTexCoord2u8(uTexUBase,uV);
						
		asm {
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			ps_sub		fTempXY, fVert1XY, fVert0XY	
			ps_sub		fTempZW, fVert1ZW, fVert0ZW	
			ps_madds0	fTempXY, fTempXY, fTrapezeInc, fVert1XY
			ps_madds0	fTempZW, fTempZW, fTrapezeInc, fVert1ZW
			ps_merge11	fTemp,fTempXY,fTempXY // Put "Y" of fTempXY in fTemp. 
		}
		
		fTrapezeInc += fTrapezeDelta;
			
		GXPosition3f32(fTempXY,fTemp,fTempZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
				
			
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
		
		GXTexCoord2u8(uUMax,uV);
		
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
		
		uV += uDeltaU;
		
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
		
		// Interpolator = fMin(Interpolator , 1.0f);
		fTemp = 1.f;
		asm
		{
			fsubs 	fTempXY, Interpolator, fTemp
			ps_sel 	Interpolator, fTempXY, fTemp, Interpolator
		}
	}
	GXEnd();
}

void GXI_l_DrawSPG2_Default(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	float								fTrapeze,
	float								fEOHP,
	float								fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII,
	BOOL 								bUseVertexColor,
	BOOL 								bLighted
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	ULONG Counter;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	SOFT_tdst_AVertex				        *Coordinates;
	u8									indexColor;
	
	SOFT_tdst_AVertex	u_4Vert0,u_4Vert1,u_4Vert2;
	SOFT_tdst_AVertex	stTrpeze;
	SOFT_tdst_AVertex	stNormale;
	SOFT_tdst_AVertex	stGravity;
	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	
	float InterpolatorIntensity;	
	float fTrapezeDelta,fTrapezeInc;
	
	float fInvNumberOfSegments;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
				
	// Texture coordinate computation.
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
		
	fInvNumberOfSegments = 1.0f / (float)ulNumberOfSegments;
	uDeltaU = (uTexVMax * TileNumber) / ulNumberOfSegments;
	fTrapezeDelta = -fTrapeze * fInvNumberOfSegments;

	// Build vertex buffer
	Coordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	
	// Loop on the primitives
	while (ulnumberOfPoints--)
	{
			
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		u_4Vert0 = *(Coordinates ++);// Pos
		MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		u_4Vert1 = *(Coordinates ++);// Xa 
		u_4Vert2 = *(Coordinates ++);// Ya

		stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		InterpolatorIntensity = stGravity.w * fInvNumberOfSegments;
		stNormale = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);

		Counter = ulNumberOfSegments + 1;
		fTrapezeInc = 	fTrapeze ; 

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w * fInvNumberOfSegments);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w * fInvNumberOfSegments);
		if (ulMode == 0) // DrawX
		{
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
		}
		else if (ulMode == 1) // DrawY
		{
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
		}
		else if (ulMode == 2) // DrawH
		{
			/*	
				F'(X) = 2AX + B
				F(X) = AX² + BX + C

				X E [0,1]
				C = Position(x) de dep
				B = x de la normale
				A = ( x de G - x de N ) / 2
			*/			
			MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP / 2.0f);
			MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			uDeltaU = uTexVMax;
			Counter = 2;
			fTrapeze = 0.0f;
			MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			stGravity = stNormale;
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
		}
				
		if (bLighted)
		{
			if (bUseVertexColor)		
				GXI_l_DrawSPG2_InnerLoopLightedVtxColorDirect(pCachedLine,&stNormale,&stGravity,&u_4Vert0,&u_4Vert1,InterpolatorIntensity,fTrapeze,fTrapezeDelta,uV,uDeltaU, uTexUBase + uTexUMax,uTexUBase,Counter,indexColor);
			else
				GXI_l_DrawSPG2_InnerLoopLightedRegColor(&stNormale,&stGravity,&u_4Vert0,&u_4Vert1,InterpolatorIntensity,fTrapeze,fTrapezeDelta,uV,uDeltaU, uTexUBase + uTexUMax,uTexUBase,Counter);
		}
		else
		{
			if (bUseVertexColor)		
				GXI_l_DrawSPG2_InnerLoopVtxColorDirect(pCachedLine,&stNormale,&stGravity,&u_4Vert0,&u_4Vert1,InterpolatorIntensity,fTrapeze,fTrapezeDelta,uV,uDeltaU, uTexUBase + uTexUMax,uTexUBase,Counter,indexColor);
			else
				GXI_l_DrawSPG2_InnerLoopRegColor(&stNormale,&stGravity,&u_4Vert0,&u_4Vert1,InterpolatorIntensity,fTrapeze,fTrapezeDelta,uV,uDeltaU, uTexUBase + uTexUMax,uTexUBase,Counter);
		}
		indexColor++;
	}
}

#ifndef DEBUG



void GXI_l_DrawSPG2_NoTrapezeLightedDirectVtxColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	ULONG Counter;
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	register float fInvNumberOfSegments;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
		
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
		
	fInvNumberOfSegments = 1.0f / (float)ulNumberOfSegments;
	uDeltaU = (uTexVMax * TileNumber) / ulNumberOfSegments;

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	while (ulnumberOfPoints--)
	{
			
		register u32 uColor = pCachedLine->a_ColorLA2[indexColor];
 		uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);

		
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w * fInvNumberOfSegments;
		asm {
			ps_muls1	InterpolatorIntensity, fInvNumberOfSegments, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		Counter = ulNumberOfSegments + 1;

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls1	fTemp, fInvNumberOfSegments, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			fTemp = (float)ulNumberOfSegments;
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_madds0	fVert0XY, fTempXY, fTemp, fVert0XY
				ps_madds0	fVert0ZW, fTempZW, fTemp, fVert0ZW
			}
			uDeltaU = uTexVMax;
			Counter = 2;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
		// Loop on the segments of the primitives.
		while (	Counter -- )
		{
			asm {
				ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
			}

			GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
			asm {
				ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
			}
			GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
			GXColor1u32(uColor);
			GXTexCoord2u8(uTexUBase,uV);
							
			asm {
				ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
			}
						
			GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
			asm {
				ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
			}
			GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
					
			asm {
				//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
				ps_sub		fTempXY, fGravityXY, fNormaleXY 
				ps_sub		fTempZW, fGravityZW, fNormaleZW
			}
			
			GXColor1u32(uColor);
			GXTexCoord2u8(uUMax,uV);
			
			asm {
				ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
				ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
			}
			
			uV += uDeltaU;
			
			asm {
				//Interpolator += InterpolatorIntensity;
				fadds		Interpolator, Interpolator, InterpolatorIntensity	
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert0XY, fVert0XY, fTempXY
				ps_add		fVert0ZW, fVert0ZW, fTempZW
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert1XY, fVert1XY, fTempXY
				ps_add		fVert1ZW, fVert1ZW, fTempZW
			}
			
			// Interpolator = fMin(Interpolator , 1.0f);
			fTempXY = 1.f;
			asm
			{
				fsubs 	fTempZW, Interpolator, fTempXY
				ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			}
		}


		GXEnd();
		indexColor++;
	}
}



void GXI_l_DrawSPG2_NoTrapezeLightedRegColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	ULONG Counter;
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	register float fInvNumberOfSegments;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
		
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
		
	fInvNumberOfSegments = 1.0f / (float)ulNumberOfSegments;
	uDeltaU = (uTexVMax * TileNumber) / ulNumberOfSegments;

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	while (ulnumberOfPoints--)
	{
			
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w * fInvNumberOfSegments;
		asm {
			ps_muls1	InterpolatorIntensity, fInvNumberOfSegments, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		Counter = ulNumberOfSegments + 1;

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls1	fTemp, fInvNumberOfSegments, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			fTemp = (float)ulNumberOfSegments;
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_madds0	fVert0XY, fTempXY, fTemp, fVert0XY
				ps_madds0	fVert0ZW, fTempZW, fTemp, fVert0ZW
			}
			uDeltaU = uTexVMax;
			Counter = 2;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
		// Loop on the segments of the primitives.
		while (	Counter -- )
		{
			asm {
				ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
			}

			GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
			asm {
				ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
			}
			GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
			GXTexCoord2u8(uTexUBase,uV);
							
			asm {
				ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
			}
						
			GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
			asm {
				ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
			}
			GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
					
			asm {
				//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
				ps_sub		fTempXY, fGravityXY, fNormaleXY 
				ps_sub		fTempZW, fGravityZW, fNormaleZW
			}
			
			GXTexCoord2u8(uUMax,uV);
			
			asm {
				ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
				ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
			}
			
			uV += uDeltaU;
			
			asm {
				//Interpolator += InterpolatorIntensity;
				fadds		Interpolator, Interpolator, InterpolatorIntensity	
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert0XY, fVert0XY, fTempXY
				ps_add		fVert0ZW, fVert0ZW, fTempZW
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert1XY, fVert1XY, fTempXY
				ps_add		fVert1ZW, fVert1ZW, fTempZW
			}
			
			// Interpolator = fMin(Interpolator , 1.0f);
			fTempXY = 1.f;
			asm
			{
				fsubs 	fTempZW, Interpolator, fTempXY
				ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			}
		}

		GXEnd();
		indexColor++;
	}
}

void GXI_l_DrawSPG2_NoTrapezeDirectVtxColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	ULONG Counter;
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	register float fInvNumberOfSegments;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
		
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
		
	fInvNumberOfSegments = 1.0f / (float)ulNumberOfSegments;
	uDeltaU = (uTexVMax * TileNumber) / ulNumberOfSegments;

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	while (ulnumberOfPoints--)
	{
		register u32 uColor = pCachedLine->a_ColorLA2[indexColor];
		uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);

		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w * fInvNumberOfSegments;
		asm {
			ps_muls1	InterpolatorIntensity, fInvNumberOfSegments, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		Counter = ulNumberOfSegments + 1;

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls1	fTemp, fInvNumberOfSegments, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			fTemp = (float)ulNumberOfSegments;
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_madds0	fVert0XY, fTempXY, fTemp, fVert0XY
				ps_madds0	fVert0ZW, fTempZW, fTemp, fVert0ZW
			}
			uDeltaU = uTexVMax;
			Counter = 2;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
		// Loop on the segments of the primitives.
		while (	Counter -- )
		{
			asm {
				ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
			}

			GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
			GXColor1u32(uColor);
			GXTexCoord2u8(uTexUBase,uV);
							
			asm {
				ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
			}
						
			GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
					
			asm {
				//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
				ps_sub		fTempXY, fGravityXY, fNormaleXY 
				ps_sub		fTempZW, fGravityZW, fNormaleZW
			}
			
			GXColor1u32(uColor);
			GXTexCoord2u8(uUMax,uV);
			
			asm {
				ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
				ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
			}
			
			uV += uDeltaU;
			
			asm {
				//Interpolator += InterpolatorIntensity;
				fadds		Interpolator, Interpolator, InterpolatorIntensity	
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert0XY, fVert0XY, fTempXY
				ps_add		fVert0ZW, fVert0ZW, fTempZW
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert1XY, fVert1XY, fTempXY
				ps_add		fVert1ZW, fVert1ZW, fTempZW
			}
			
			// Interpolator = fMin(Interpolator , 1.0f);
			fTempXY = 1.f;
			asm
			{
				fsubs 	fTempZW, Interpolator, fTempXY
				ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			}
		}

		GXEnd();
		indexColor++;
	}
}

void GXI_l_DrawSPG2_NoTrapezeRegColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	ULONG Counter;
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	register float fInvNumberOfSegments;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
		
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
		
	fInvNumberOfSegments = 1.0f / (float)ulNumberOfSegments;
	uDeltaU = (uTexVMax * TileNumber) / ulNumberOfSegments;

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	while (ulnumberOfPoints--)
	{
			
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w * fInvNumberOfSegments;
		asm {
			ps_muls1	InterpolatorIntensity, fInvNumberOfSegments, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		Counter = ulNumberOfSegments + 1;

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls1	fTemp, fInvNumberOfSegments, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w * fInvNumberOfSegments);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			fTemp = (float)ulNumberOfSegments;
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_madds0	fVert0XY, fTempXY, fTemp, fVert0XY
				ps_madds0	fVert0ZW, fTempZW, fTemp, fVert0ZW
			}
			uDeltaU = uTexVMax;
			Counter = 2;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, Counter*2); 
		
		// Loop on the segments of the primitives.
		while (	Counter -- )
		{
			asm {
				ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
			}

			GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
			GXTexCoord2u8(uTexUBase,uV);
							
			asm {
				ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
			}
						
			GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
					
			asm {
				//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
				ps_sub		fTempXY, fGravityXY, fNormaleXY 
				ps_sub		fTempZW, fGravityZW, fNormaleZW
			}
			
			GXTexCoord2u8(uUMax,uV);
			
			asm {
				ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
				ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
			}
			
			uV += uDeltaU;
			
			asm {
				//Interpolator += InterpolatorIntensity;
				fadds		Interpolator, Interpolator, InterpolatorIntensity	
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert0XY, fVert0XY, fTempXY
				ps_add		fVert0ZW, fVert0ZW, fTempZW
				//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
				ps_add		fVert1XY, fVert1XY, fTempXY
				ps_add		fVert1ZW, fVert1ZW, fTempZW
			}
			
			// Interpolator = fMin(Interpolator , 1.0f);
			fTempXY = 1.f;
			asm
			{
				fsubs 	fTempZW, Interpolator, fTempXY
				ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			}
		}

		GXEnd();
		indexColor++;
	}
}

void GXI_l_DrawSPG2_NoTrapeze(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII,
	BOOL 								bUseVertexColor,
	BOOL 								bLighted
)
{
	if (bLighted)
	{
		if (bUseVertexColor)
			GXI_l_DrawSPG2_NoTrapezeLightedDirectVtxColor(pCachedLine,ulnumberOfPoints,ulNumberOfSegments,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
		else
			GXI_l_DrawSPG2_NoTrapezeLightedRegColor(pCachedLine,ulnumberOfPoints,ulNumberOfSegments,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
	}
	else
	{
		if (bUseVertexColor)
			GXI_l_DrawSPG2_NoTrapezeDirectVtxColor(pCachedLine,ulnumberOfPoints,ulNumberOfSegments,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
		else
			GXI_l_DrawSPG2_NoTrapezeRegColor(pCachedLine,ulnumberOfPoints,ulNumberOfSegments,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
	}
}


void GXI_l_DrawSPG2_NoTrapezeOneSegmentDirectVtxColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII	
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//	
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
	
	uDeltaU = (uTexVMax * TileNumber);

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulnumberOfPoints);
	while (ulnumberOfPoints--)
	{
		register u32 uColor = pCachedLine->a_ColorLA2[indexColor];
		uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);
		
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w;
		asm {
			ps_merge11	InterpolatorIntensity, fGravityZW, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w);
			ps_merge11	fTemp, fVert2ZW, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_add	fVert0XY, fTempXY, fVert0XY
				ps_add	fVert0ZW, fTempZW, fVert0ZW
			}
			uDeltaU = uTexVMax;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		
		// BEGIN Draw Quad
		
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);
							
		asm {
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
					
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
			
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
			
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
			
		uV += uDeltaU;
			
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
			
		// Interpolator = fMin(Interpolator , 1.0f);
		fTempXY = 1.f;
		asm
		{
			fsubs 	fTempZW, Interpolator, fTempXY
			ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);

		// END Draw Quad
		indexColor++;
	}
	GXEnd();
}

void GXI_l_DrawSPG2_NoTrapezeOneSegmentRegColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII	
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//	
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
	
	uDeltaU = (uTexVMax * TileNumber);

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulnumberOfPoints);
	while (ulnumberOfPoints--)
	{
			
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w;
		asm {
			ps_merge11	InterpolatorIntensity, fGravityZW, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w);
			ps_merge11	fTemp, fVert2ZW, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_add	fVert0XY, fTempXY, fVert0XY
				ps_add	fVert0ZW, fTempZW, fVert0ZW
			}
			uDeltaU = uTexVMax;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		
		// BEGIN Draw Quad
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		GXTexCoord2u8(uTexUBase,uV);
							
		asm {
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
					
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
			
		GXTexCoord2u8(uUMax,uV);
			
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
			
		uV += uDeltaU;
			
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
			
		// Interpolator = fMin(Interpolator , 1.0f);
		fTempXY = 1.f;
		asm
		{
			fsubs 	fTempZW, Interpolator, fTempXY
			ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
		GXTexCoord2u8(uUMax,uV);
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		GXTexCoord2u8(uTexUBase,uV);
		// END Draw Quad
		indexColor++;
	}
	GXEnd();
}


void GXI_l_DrawSPG2_NoTrapezeOneSegmentLightedDirectVtxColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII	
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//	
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
	
	uDeltaU = (uTexVMax * TileNumber);

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulnumberOfPoints);
	while (ulnumberOfPoints--)
	{
		register u32 uColor = pCachedLine->a_ColorLA2[indexColor];
		uColor = ((uColor&0x000000ff)<<24) | ((uColor&0x0000ff00)<<8) | ((uColor&0x00ff0000)>>8) | ((uColor&0xff000000)>>24);
		
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w;
		asm {
			ps_merge11	InterpolatorIntensity, fGravityZW, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w);
			ps_merge11	fTemp, fVert2ZW, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_add	fVert0XY, fTempXY, fVert0XY
				ps_add	fVert0ZW, fTempZW, fVert0ZW
			}
			uDeltaU = uTexVMax;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		
		// BEGIN Draw Quad
		
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);
							
		asm {
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
					
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
			
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
			
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
			
		uV += uDeltaU;
			
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
			
		// Interpolator = fMin(Interpolator , 1.0f);
		fTempXY = 1.f;
		asm
		{
			fsubs 	fTempZW, Interpolator, fTempXY
			ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXColor1u32(uColor);
		GXTexCoord2u8(uUMax,uV);
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXColor1u32(uColor);
		GXTexCoord2u8(uTexUBase,uV);

		// END Draw Quad
		indexColor++;
	}
	GXEnd();
}

void GXI_l_DrawSPG2_NoTrapezeOneSegmentLightedRegColor(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII	
)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//	
	ULONG uWindIndex;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	u8									indexColor;	
	u8 uTexUMax,uTexVMax;
	u8 uTexUBase,uV,uDeltaU;
	u8 uUMax;
	
	register float InterpolatorIntensity;	
	register float *pTemp,*pCoordinates;
	register float fVert0XY,fVert0ZW;
	register float fVert1XY,fVert1ZW;
	register float fVert2XY,fVert2ZW;
	register float fNormaleXY,fNormaleZW;
	register float fGravityXY,fGravityZW;
	register float fTempXY,fTempZW,fTemp; // Used for many purposes
	register float Interpolator;
	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			
	// Texture coordinate computation.	
	uTexUBase = 0.0f;
	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	// uTexUMax and uTexVMax are the size of a patch in the texture.
	uTexUMax = 1<<(TEX_FRAC-U_SHIFT);
	uTexVMax = 1<<(TEX_FRAC-V_SHIFT);
	
	uDeltaU = (uTexVMax * TileNumber);

	// Build vertex buffer
	pCoordinates = pCachedLine->a_PointLA2;
	indexColor = 0;
	
	// Loop on the primitives
	GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulnumberOfPoints);
	while (ulnumberOfPoints--)
	{
			
		if (p_stII->BaseAnimUv)
		{
			uTexUBase = (TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * uTexUMax;
			uV = (((TextureTilerUV_Base >> U_SHIFT)^0xf) * uTexVMax) & 0x7f;
			TextureTilerUV_Base += 3;
		}
		else
			uV = 0;
			
		//u_4Vert0 = *(Coordinates);// Pos
		asm {
			psq_l       fVert0XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert0ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,&p_stII->GlobalPos);
		pTemp = &p_stII->GlobalPos;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0     
			psq_l       fTempZW, 0x08(pTemp), 1, 0   
			ps_add		fVert0XY,fVert0XY,fTempXY
			ps_add		fVert0ZW,fVert0ZW,fTempZW
		}
		
		
		//u_4Vert1 = *(Coordinates);// Xa 
		asm {
			psq_l       fVert1XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert1ZW, 0x08(pCoordinates), 1, 0     
		}
		pCoordinates +=4;
		
		//u_4Vert2 = *(Coordinates);// Ya
		asm {
			psq_l       fVert2XY, 0x00(pCoordinates), 0, 0     
			psq_l       fVert2ZW, 0x08(pCoordinates), 0, 0    
		}
		pCoordinates +=4;
				
		//stNormale = *(Coordinates);
		asm {
			psq_l       fNormaleXY, 0x00(pCoordinates), 0, 0     
			psq_l       fNormaleZW, 0x08(pCoordinates), 1, 0     
		}
		// 2nd part of fNormaleZW is uWindIndex.
		uWindIndex = *((ULONG *)(pCoordinates+3));
		pCoordinates +=4;

		//stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		pTemp = (float *)(pWind + (uWindIndex ^ p_stII->BaseWind));
		asm {
			psq_l       fGravityXY, 0x00(pTemp), 0, 0     
			psq_l       fGravityZW, 0x08(pTemp), 0, 0     
		}
		
		//InterpolatorIntensity = stGravity.w;
		asm {
			ps_merge11	InterpolatorIntensity, fGravityZW, fGravityZW
		}		
				
		//MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		pTemp = &p_stII->GlobalZADD;
		asm {
			psq_l       fTempXY, 0x00(pTemp), 0, 0
			psq_l       fTempZW, 0x08(pTemp), 1, 0
			ps_add		fNormaleXY,fNormaleXY,fTempXY
			ps_add		fNormaleZW,fNormaleZW,fTempZW
		}
		
		//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
		fTemp = p_stII->GlobalSCale;
		asm {
			ps_muls0		fNormaleXY,fNormaleXY,fTemp
			ps_muls0		fNormaleZW,fNormaleZW,fTemp
		}

		asm {
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w);
			ps_merge11	fTemp, fVert2ZW, fVert2ZW
			ps_muls0	fNormaleXY,fNormaleXY,fTemp
			ps_muls0	fNormaleZW,fNormaleZW,fTemp
			//MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w);
			ps_muls0	fGravityXY,fGravityXY,fTemp
			ps_muls0	fGravityZW,fGravityZW,fTemp
		}		
		
		if (ulMode == 0) // DrawX
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}
		else if (ulMode == 1) // DrawY
		{
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
			}
			fTempXY = -.5f*fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
				ps_madds0	fVert1XY, fVert2XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert2ZW, fTempZW, fVert0ZW
			}			
			
		}
		else if (ulMode == 2) // DrawH
		{
			//MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
			asm {
				ps_sub	fTempXY, fGravityXY, fNormaleXY
				ps_sub	fTempZW, fGravityZW, fNormaleZW
			}
			//MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP * .5f);
			fTemp = InterpolatorIntensity  * fEOHP * fEOHP * .5f;
			asm {
				ps_muls0	fTempXY,fTempXY,fTemp
				ps_muls0	fTempZW,fTempZW,fTemp
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			asm {
				ps_madds0	fTempXY, fNormaleXY , fEOHP ,fTempXY
				ps_madds0	fTempZW, fNormaleZW , fEOHP ,fTempZW
			}
			
			//MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze , (float)ulNumberOfSegments);
			asm {
				ps_add	fVert0XY, fTempXY, fVert0XY
				ps_add	fVert0ZW, fTempZW, fVert0ZW
			}
			uDeltaU = uTexVMax;
			//MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
			asm {
				ps_muls1	fTempZW, fRatio, fVert2ZW
				ps_muls0	fNormaleXY, fVert2XY, fTempZW
				ps_muls0	fNormaleZW, fVert2ZW, fTempZW
			}
			
			// stGravity = stNormale;
			asm {
				ps_mr		fGravityXY, fNormaleXY
				ps_mr		fGravityZW, fNormaleZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
			fTempXY = -.5f * fTempZW;
			asm {
				ps_madds0	fVert0XY, fVert1XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert1ZW, fTempXY, fVert0ZW
			}
			
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert0XY, fVert2XY, fTempXY, fVert0XY
				ps_madds0	fVert0ZW, fVert2ZW, fTempXY, fVert0ZW
			}
			//MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
			asm {
				ps_madds0	fVert1XY, fVert1XY, fTempZW, fVert0XY
				ps_madds0	fVert1ZW, fVert1ZW, fTempZW, fVert0ZW
			}
		}

		uUMax = uTexUBase + uTexUMax;

		Interpolator = 0.0f;
		
		// BEGIN Draw Quad
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXTexCoord2u8(uTexUBase,uV);
							
		asm {
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
					
		asm {
			//MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			ps_sub		fTempXY, fGravityXY, fNormaleXY 
			ps_sub		fTempZW, fGravityZW, fNormaleZW
		}
			
		GXTexCoord2u8(uUMax,uV);
			
		asm {
			ps_madds0	fTempXY, fTempXY, Interpolator, fNormaleXY
			ps_madds0	fTempZW, fTempZW, Interpolator, fNormaleZW
		}
			
		uV += uDeltaU;
			
		asm {
			//Interpolator += InterpolatorIntensity;
			fadds		Interpolator, Interpolator, InterpolatorIntensity	
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert0XY, fVert0XY, fTempXY
			ps_add		fVert0ZW, fVert0ZW, fTempZW
			//MATH_AddVector((MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stDerivative);
			ps_add		fVert1XY, fVert1XY, fTempXY
			ps_add		fVert1ZW, fVert1ZW, fTempZW
		}
			
		// Interpolator = fMin(Interpolator , 1.0f);
		fTempXY = 1.f;
		asm
		{
			fsubs 	fTempZW, Interpolator, fTempXY
			ps_sel 	Interpolator, fTempZW, fTempXY, Interpolator
			ps_merge11	fTempZW,fVert1XY,fVert1XY // Put "Y" of fVert1XY in fTempZW. 
		}
						
		GXPosition3f32(fVert1XY,fTempZW,fVert1ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXTexCoord2u8(uUMax,uV);
		asm {
			ps_merge11	fTempXY,fVert0XY,fVert0XY // Put "Y" of fVert0XY in fTemp. 
		}

		GXPosition3f32(fVert0XY,fTempXY,fVert0ZW);
		asm {
			ps_merge11	fTemp,fNormaleXY,fNormaleXY // Put "Y" of fNormaleXY in fTemp. 
		}
		GXNormal3f32(fNormaleXY,fTemp,fNormaleZW);
		GXTexCoord2u8(uTexUBase,uV);

		// END Draw Quad
		indexColor++;
	}
	GXEnd();
}

void GXI_l_DrawSPG2_NoTrapezeOneSegment(
	SPG2_CachedPrimitivs				*pCachedLine,
	ULONG								ulnumberOfPoints,
	register float						fEOHP,
	register float						fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind, 
	SPG2_InstanceInforamtion			*p_stII,
	BOOL 								bUseVertexColor,
	BOOL 								bLighted
)
{
	if (bLighted)
	{
		if (bUseVertexColor)
			GXI_l_DrawSPG2_NoTrapezeOneSegmentLightedDirectVtxColor(pCachedLine,ulnumberOfPoints,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
		else
			GXI_l_DrawSPG2_NoTrapezeOneSegmentLightedRegColor(pCachedLine,ulnumberOfPoints,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
	}
	else
	{
		if (bUseVertexColor)
			GXI_l_DrawSPG2_NoTrapezeOneSegmentDirectVtxColor(pCachedLine,ulnumberOfPoints,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
		else
			GXI_l_DrawSPG2_NoTrapezeOneSegmentRegColor(pCachedLine,ulnumberOfPoints,fEOHP,fRatio,TileNumber,ulMode,pWind,p_stII);
	}
}

#endif // DEBUG




void GXI_l_DrawSPG2_SPRITES_Direct(
	SOFT_tdst_AVertex			        *Coordinates,
	GEO_Vertex					        *XCamera,
	GEO_Vertex					        *YCamera,
	ULONG								*pColors,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSprites,
	float								CosAlpha2,
	float								SinAlpha2,
	float								SpriteGenRadius,
	float								fEOHP,
	float								fRatio,
	SOFT_tdst_AVertex			        *pWind,
	SPG2_InstanceInforamtion			*p_stII
)
{
	ULONG indexColor;
	ULONG BM;
	float OoNSPR;
	float ComplexRadius_RE;
	float ComplexRadius_IM;
	float ComplexRadius_LOCAL;
	SOFT_tdst_AVertex	XCam,YCam;

	BM = 0;
	

	*(MATH_tdst_Vector *)&XCam = *XCamera;
	*(MATH_tdst_Vector *)&YCam = *YCamera;

	ComplexRadius_IM = 1.0f;
	ComplexRadius_RE = 0.0f;

	ulNumberOfSprites &= 63;
	if (!ulNumberOfSprites) return;
	OoNSPR = 1.0f / (float)ulNumberOfSprites;

	for (indexColor = 0; indexColor < ulnumberOfPoints; indexColor++)
	{
		SOFT_tdst_AVertex	VC,Xa,Ya,Za,LocalX,LocalY,stGravity;
		float	Interpolator,InterpolatorIntensity ;
		ULONG Counter;
		ULONG uColors = pColors[indexColor];
		SwapDWord(&uColors);
		
		VC = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&VC,(MATH_tdst_Vector *)&VC,&p_stII->GlobalPos);
		Xa = *(Coordinates ++);
		Ya = *(Coordinates ++);
		stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		InterpolatorIntensity = stGravity.w * OoNSPR;

		Za = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&Za ,(MATH_tdst_Vector *)&Za ,&p_stII->GlobalZADD);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , p_stII->GlobalSCale);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Xa , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Ya , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , OoNSPR);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , OoNSPR);


		MATH_SubVector( (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&Za );
		MATH_ScaleEqualVector( (MATH_tdst_Vector *)&LocalX  , (float)ulNumberOfSprites * InterpolatorIntensity  * fEOHP * fEOHP / 2.0f);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&Za , fEOHP);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC, (MATH_tdst_Vector *)&LocalX , (float)ulNumberOfSprites);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , 1.0f - fEOHP);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , 1.0f - fEOHP);

		GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulNumberOfSprites);
		
		Interpolator = fEOHP * (float)ulNumberOfSprites * InterpolatorIntensity;
		
		Counter = ulNumberOfSprites;
		while (	Counter -- )
		{
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			MATH_tdst_Vector Cx,C0,C1,C2,C3,stDerivative;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			MATH_AddScaleVector(&Cx , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&Xa , ComplexRadius_IM);
			MATH_SubScaleVector(&C0,&Cx,(MATH_tdst_Vector *)&XCam,0.5f);
			MATH_SubScaleVector(&C0,&C0,(MATH_tdst_Vector *)&YCam,0.5f);
			MATH_AddVector(&C1,&C0,(MATH_tdst_Vector *)&XCam);
			MATH_AddVector(&C2,&C1,(MATH_tdst_Vector *)&YCam);
			MATH_SubVector(&C3,&C2,(MATH_tdst_Vector *)&XCam);


			// 00
			GXPosition3f32(C0.x,C0.y,C0.z); 
			GXColor1u32(uColors);
			GXTexCoord1x8(0);

			// 10
			GXPosition3f32(C1.x,C1.y,C1.z); 
			GXColor1u32(uColors);
			GXTexCoord1x8(1);

			// 11
			GXPosition3f32(C2.x,C2.y,C2.z); 
			GXColor1u32(uColors);
			GXTexCoord1x8(2);

			// 01
			GXPosition3f32(C3.x,C3.y,C3.z); 
			GXColor1u32(uColors);
			GXTexCoord1x8(3);

			MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&Za , (MATH_tdst_Vector *)&stGravity , Interpolator);
			MATH_AddVector((MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&stDerivative);
			Interpolator += InterpolatorIntensity * (1.0f - fEOHP);
			Interpolator = fMin(Interpolator , 1.0f);

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Xa , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Ya , SinAlpha);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Ya , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Xa , -SinAlpha);
			Xa = LocalX ;
			Ya = LocalY ;

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&XCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&YCam , SinAlpha2);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&YCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&XCam , -SinAlpha2);
			XCam = LocalX ;
			YCam = LocalY ;


			ComplexRadius_LOCAL = ComplexRadius_IM * CosAlpha + ComplexRadius_RE * SinAlpha;
			ComplexRadius_RE = ComplexRadius_IM * SinAlpha - ComplexRadius_RE * CosAlpha;
			ComplexRadius_IM = ComplexRadius_LOCAL;

		}

		GXEnd();
	}
}

void GXI_l_DrawSPG2_SPRITES_Ambiant(
	SOFT_tdst_AVertex			        *Coordinates,
	GEO_Vertex					        *XCamera,
	GEO_Vertex					        *YCamera,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSprites,
	float								CosAlpha2,
	float								SinAlpha2,
	float								SpriteGenRadius,
	float								fEOHP,
	float								fRatio,
	SOFT_tdst_AVertex			        *pWind,
	SPG2_InstanceInforamtion			*p_stII
)
{
	ULONG indexColor;
	ULONG BM;
	float OoNSPR;
	float ComplexRadius_RE;
	float ComplexRadius_IM;
	float ComplexRadius_LOCAL;
	SOFT_tdst_AVertex	XCam,YCam;


	BM = 0;
	

	*(MATH_tdst_Vector *)&XCam = *XCamera;
	*(MATH_tdst_Vector *)&YCam = *YCamera;

	ComplexRadius_IM = 1.0f;
	ComplexRadius_RE = 0.0f;

	ulNumberOfSprites &= 63;
	if (!ulNumberOfSprites) return;
	OoNSPR = 1.0f / (float)ulNumberOfSprites;

	for (indexColor = 0; indexColor < ulnumberOfPoints; indexColor++)
	{
		SOFT_tdst_AVertex	VC,Xa,Ya,Za,LocalX,LocalY,stGravity;
		float	Interpolator,InterpolatorIntensity ;
		ULONG Counter;
		
		VC = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&VC,(MATH_tdst_Vector *)&VC,&p_stII->GlobalPos);
		Xa = *(Coordinates ++);
		Ya = *(Coordinates ++);
		stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		InterpolatorIntensity = stGravity.w * OoNSPR;

		Za = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&Za ,(MATH_tdst_Vector *)&Za ,&p_stII->GlobalZADD);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , p_stII->GlobalSCale);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Xa , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Ya , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , OoNSPR);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , OoNSPR);


		MATH_SubVector( (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&Za );
		MATH_ScaleEqualVector( (MATH_tdst_Vector *)&LocalX  , (float)ulNumberOfSprites * InterpolatorIntensity  * fEOHP * fEOHP / 2.0f);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&Za , fEOHP);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC, (MATH_tdst_Vector *)&LocalX , (float)ulNumberOfSprites);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , 1.0f - fEOHP);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , 1.0f - fEOHP);

		GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulNumberOfSprites);
		
		Interpolator = fEOHP * (float)ulNumberOfSprites * InterpolatorIntensity;
		
		Counter = ulNumberOfSprites;
		while (	Counter -- )
		{
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			MATH_tdst_Vector Cx,C0,C1,C2,C3,stDerivative;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			MATH_AddScaleVector(&Cx , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&Xa , ComplexRadius_IM);
			MATH_SubScaleVector(&C0,&Cx,(MATH_tdst_Vector *)&XCam,0.5f);
			MATH_SubScaleVector(&C0,&C0,(MATH_tdst_Vector *)&YCam,0.5f);
			MATH_AddVector(&C1,&C0,(MATH_tdst_Vector *)&XCam);
			MATH_AddVector(&C2,&C1,(MATH_tdst_Vector *)&YCam);
			MATH_SubVector(&C3,&C2,(MATH_tdst_Vector *)&XCam);


			// 00
			GXPosition3f32(C0.x,C0.y,C0.z); 
			GXTexCoord1x8(0);

			// 10
			GXPosition3f32(C1.x,C1.y,C1.z); 
			GXTexCoord1x8(1);

			// 11
			GXPosition3f32(C2.x,C2.y,C2.z); 
			GXTexCoord1x8(2);

			// 01
			GXPosition3f32(C3.x,C3.y,C3.z); 
			GXTexCoord1x8(3);

			MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&Za , (MATH_tdst_Vector *)&stGravity , Interpolator);
			MATH_AddVector((MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&stDerivative);
			Interpolator += InterpolatorIntensity * (1.0f - fEOHP);
			Interpolator = fMin(Interpolator , 1.0f);

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Xa , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Ya , SinAlpha);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Ya , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Xa , -SinAlpha);
			Xa = LocalX ;
			Ya = LocalY ;

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&XCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&YCam , SinAlpha2);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&YCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&XCam , -SinAlpha2);
			XCam = LocalX ;
			YCam = LocalY ;


			ComplexRadius_LOCAL = ComplexRadius_IM * CosAlpha + ComplexRadius_RE * SinAlpha;
			ComplexRadius_RE = ComplexRadius_IM * SinAlpha - ComplexRadius_RE * CosAlpha;
			ComplexRadius_IM = ComplexRadius_LOCAL;

		}

		GXEnd();
	}
}



void GXI_l_DrawSPG2_SPRITES_DirectLighted(
	SOFT_tdst_AVertex			        *Coordinates,
	GEO_Vertex					        *XCamera,
	GEO_Vertex					        *YCamera,
	ULONG								*pColors,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSprites,
	float								CosAlpha2,
	float								SinAlpha2,
	float								SpriteGenRadius,
	float								fEOHP,
	float								fRatio,
	SOFT_tdst_AVertex			        *pWind,
	SPG2_InstanceInforamtion			*p_stII
)
{
	ULONG indexColor;
	ULONG BM;
	float OoNSPR;
	float ComplexRadius_RE;
	float ComplexRadius_IM;
	SOFT_tdst_AVertex	XCam,YCam;
	
	
	BM = 0;
	

	*(MATH_tdst_Vector *)&XCam = *XCamera;
	*(MATH_tdst_Vector *)&YCam = *YCamera;

	ComplexRadius_IM = 1.0f;
	ComplexRadius_RE = 0.0f;

	ulNumberOfSprites &= 63;
	if (!ulNumberOfSprites) return;
	OoNSPR = 1.0f / (float)ulNumberOfSprites;

	for (indexColor = 0; indexColor < ulnumberOfPoints; indexColor++)
	{
		SOFT_tdst_AVertex	VC,Xa,Ya,Za,LocalX,LocalY,stGravity;
		float	Interpolator,InterpolatorIntensity ;
		ULONG Counter;
		ULONG uColors = pColors[indexColor];
		SwapDWord(&uColors);
		
		VC = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&VC,(MATH_tdst_Vector *)&VC,&p_stII->GlobalPos);
		Xa = *(Coordinates ++);
		Ya = *(Coordinates ++);
		stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		InterpolatorIntensity = stGravity.w * OoNSPR;

		Za = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&Za ,(MATH_tdst_Vector *)&Za ,&p_stII->GlobalZADD);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , p_stII->GlobalSCale);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Xa , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Ya , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , OoNSPR);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , OoNSPR);


		MATH_SubVector( (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&Za );
		MATH_ScaleEqualVector( (MATH_tdst_Vector *)&LocalX  , (float)ulNumberOfSprites * InterpolatorIntensity  * fEOHP * fEOHP / 2.0f);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&Za , fEOHP);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC, (MATH_tdst_Vector *)&LocalX , (float)ulNumberOfSprites);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , 1.0f - fEOHP);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , 1.0f - fEOHP);
		

		GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulNumberOfSprites);
		
		Interpolator = fEOHP * (float)ulNumberOfSprites * InterpolatorIntensity;
		
		Counter = ulNumberOfSprites;
		while (	Counter -- )
		{
			MATH_tdst_Vector Temp;
			
			// 00
			MATH_AddScaleVector(&Temp , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&Xa , ComplexRadius_IM);
			MATH_SubScaleVector(&Temp,&Temp,(MATH_tdst_Vector *)&XCam,0.5f);
			MATH_SubScaleVector(&Temp,&Temp,(MATH_tdst_Vector *)&YCam,0.5f);
			GXPosition3f32(Temp.x,Temp.y,Temp.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXColor1u32(uColors);
			GXTexCoord1x8(0);
			
			// 10
			MATH_AddVector(&Temp,&Temp,(MATH_tdst_Vector *)&XCam);
			GXPosition3f32(Temp.x,Temp.y,Temp.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXColor1u32(uColors);
			GXTexCoord1x8(1);
			
			// 11
			MATH_AddVector(&Temp,&Temp,(MATH_tdst_Vector *)&YCam);
			GXPosition3f32(Temp.x,Temp.y,Temp.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXColor1u32(uColors);
			GXTexCoord1x8(2);
			
			// 01
			MATH_SubVector(&Temp,&Temp,(MATH_tdst_Vector *)&XCam);
			GXPosition3f32(Temp.x,Temp.y,Temp.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXColor1u32(uColors);
			GXTexCoord1x8(3);


			MATH_BlendVector((MATH_tdst_Vector *)&Temp , (MATH_tdst_Vector *)&Za , (MATH_tdst_Vector *)&stGravity , Interpolator);
			MATH_AddVector((MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&Temp);
			Interpolator += InterpolatorIntensity * (1.0f - fEOHP);
			Interpolator = fMin(Interpolator , 1.0f);

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Xa , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Ya , SinAlpha);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Ya , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Xa , -SinAlpha);
			Xa = LocalX ;
			Ya = LocalY ;

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&XCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&YCam , SinAlpha2);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&YCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&XCam , -SinAlpha2);
			XCam = LocalX ;
			YCam = LocalY ;

			Temp.x = ComplexRadius_IM * CosAlpha + ComplexRadius_RE * SinAlpha;
			ComplexRadius_RE = ComplexRadius_IM * SinAlpha - ComplexRadius_RE * CosAlpha;
			ComplexRadius_IM = Temp.x;
		}

		GXEnd();
	}
}



void GXI_l_DrawSPG2_SPRITES_AmbiantLighted(
	SOFT_tdst_AVertex			        *Coordinates,
	GEO_Vertex					        *XCamera,
	GEO_Vertex					        *YCamera,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSprites,
	float								CosAlpha2,
	float								SinAlpha2,
	float								SpriteGenRadius,
	float								fEOHP,
	float								fRatio,
	SOFT_tdst_AVertex			        *pWind,
	SPG2_InstanceInforamtion			*p_stII
)
{
	ULONG indexColor;
	ULONG BM;
	float OoNSPR;
	float ComplexRadius_RE;
	float ComplexRadius_IM;
	float ComplexRadius_LOCAL;
	SOFT_tdst_AVertex	XCam,YCam;

	BM = 0;
	

	*(MATH_tdst_Vector *)&XCam = *XCamera;
	*(MATH_tdst_Vector *)&YCam = *YCamera;

	ComplexRadius_IM = 1.0f;
	ComplexRadius_RE = 0.0f;

	ulNumberOfSprites &= 63;
	if (!ulNumberOfSprites) return;
	OoNSPR = 1.0f / (float)ulNumberOfSprites;

	for (indexColor = 0; indexColor < ulnumberOfPoints; indexColor++)
	{
		SOFT_tdst_AVertex	VC,Xa,Ya,Za,LocalX,LocalY,stGravity;
		float	Interpolator,InterpolatorIntensity ;
		ULONG Counter;
		
		
		VC = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&VC,(MATH_tdst_Vector *)&VC,&p_stII->GlobalPos);
		Xa = *(Coordinates ++);
		Ya = *(Coordinates ++);
		stGravity = pWind[Coordinates->c ^ p_stII->BaseWind ];
		InterpolatorIntensity = stGravity.w * OoNSPR;

		Za = *(Coordinates ++);
		MATH_AddVector((MATH_tdst_Vector *)&Za ,(MATH_tdst_Vector *)&Za ,&p_stII->GlobalZADD);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , p_stII->GlobalSCale);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , Ya.w);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Xa , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Ya , Ya.w * SpriteGenRadius * fRatio);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , OoNSPR);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , OoNSPR);


		MATH_SubVector( (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&Za );
		MATH_ScaleEqualVector( (MATH_tdst_Vector *)&LocalX  , (float)ulNumberOfSprites * InterpolatorIntensity  * fEOHP * fEOHP / 2.0f);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&LocalX  , (MATH_tdst_Vector *)&Za , fEOHP);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC, (MATH_tdst_Vector *)&LocalX , (float)ulNumberOfSprites);

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&Za , 1.0f - fEOHP);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , 1.0f - fEOHP);

		GXBegin(GX_QUADS, GX_VTXFMT1, 4*ulNumberOfSprites);
		
		Interpolator = fEOHP * (float)ulNumberOfSprites * InterpolatorIntensity;
		
		Counter = ulNumberOfSprites;
		while (	Counter -- )
		{
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			MATH_tdst_Vector Cx,C0,C1,C2,C3,stDerivative;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			MATH_AddScaleVector(&Cx , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&Xa , ComplexRadius_IM);
			MATH_SubScaleVector(&C0,&Cx,(MATH_tdst_Vector *)&XCam,0.5f);
			MATH_SubScaleVector(&C0,&C0,(MATH_tdst_Vector *)&YCam,0.5f);
			MATH_AddVector(&C1,&C0,(MATH_tdst_Vector *)&XCam);
			MATH_AddVector(&C2,&C1,(MATH_tdst_Vector *)&YCam);
			MATH_SubVector(&C3,&C2,(MATH_tdst_Vector *)&XCam);


			// 00
			GXPosition3f32(C0.x,C0.y,C0.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXTexCoord1x8(0);

			// 10
			GXPosition3f32(C1.x,C1.y,C1.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXTexCoord1x8(1);

			// 11
			GXPosition3f32(C2.x,C2.y,C2.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXTexCoord1x8(2);

			// 01
			GXPosition3f32(C3.x,C3.y,C3.z); 
			GXNormal3f32(Za.x,Za.y,Za.z);
			GXTexCoord1x8(3);


			MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&Za , (MATH_tdst_Vector *)&stGravity , Interpolator);
			MATH_AddVector((MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&stDerivative);
			Interpolator += InterpolatorIntensity * (1.0f - fEOHP);
			Interpolator = fMin(Interpolator , 1.0f);

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Xa , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Ya , SinAlpha);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Ya , CosAlpha);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Xa , -SinAlpha);
			Xa = LocalX ;
			Ya = LocalY ;

			// Helicoïdal effect 
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&XCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&YCam , SinAlpha2);
			MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&YCam , CosAlpha2);
			MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&XCam , -SinAlpha2);
			XCam = LocalX ;
			YCam = LocalY ;


			ComplexRadius_LOCAL = ComplexRadius_IM * CosAlpha + ComplexRadius_RE * SinAlpha;
			ComplexRadius_RE = ComplexRadius_IM * SinAlpha - ComplexRadius_RE * CosAlpha;
			ComplexRadius_IM = ComplexRadius_LOCAL;

		}

		GXEnd();
	}
}

void GXI_v_CompressVector(s16 *_pDstVtx,MATH_tdst_Vector *_pSrcVtx,float _f2PowFrac)
{
	MATH_ScaleEqualVector(_pSrcVtx,_f2PowFrac);
	_pDstVtx[0] = (s16) (_pSrcVtx->x);
	_pDstVtx[1] = (s16) (_pSrcVtx->y);
	_pDstVtx[2] = (s16) (_pSrcVtx->z);
}

void GXI_v_ComputeTextCoord(
		int 						ulMode,
		SPG2_InstanceInforamtion 	*p_stII,
		ULONG						TileNumber,
		float						*pTextCoordBuffer)
{
	float DeltaU;
	u32 U_SHIFT,V_SHIFT;
	float TexUMax,TexVMax;
	float TexUBase,TexVBase;
	float TexUBase2,TexVBase2;
	
	// Texture coordinates.
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;
	
	// Fast One Over 2^N 
	*(u32 *)&TexUMax = (127 - U_SHIFT) << 23;
	*(u32 *)&TexVMax = (127 - V_SHIFT) << 23;
		
	DeltaU = (ulMode == 2 ? TexVMax : TexVMax*(float)TileNumber) ;
	if (p_stII->BaseAnimUv)
	{
		u32 TextureTilerUV_Base;
		
		TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
		TexUBase = (float)(TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * TexUMax;
		TexVBase = (float)((TextureTilerUV_Base >> U_SHIFT)^0xf) * TexVMax;
	}
	else
		TexUBase = TexVBase = 0.0f;
	
	TexUBase2 = TexUBase+TexUMax;
	TexVBase2 = TexVBase+DeltaU;
	
	pTextCoordBuffer[0] = TexUBase;
	pTextCoordBuffer[1] = TexVBase;
	pTextCoordBuffer[2] = TexUBase2;
	pTextCoordBuffer[3] = TexVBase;
	pTextCoordBuffer[4] = TexUBase;
	pTextCoordBuffer[5] = TexVBase2;
	pTextCoordBuffer[6] = TexUBase2;
	pTextCoordBuffer[7] = TexVBase2;
}

void GXI_v_CompressCrossAndHat(
		int 						ulMode,
		SPG2_InstanceInforamtion 	*p_stII,
		SOFT_tdst_AVertex			*pWind, 
		tCmpVertex		 			*pVtxBuffer, 
		SOFT_tdst_AVertex 			*pPos,
		SOFT_tdst_AVertex 			*pXA,
		SOFT_tdst_AVertex 			*pYA,
		SOFT_tdst_AVertex 			*pNormal,
		float						fTrapeze,
		float						fEOHP,
		float 						fRatio,
		float 						fMaxDistance,
		MATH_tdst_Vector			*pCenter,
		int 						_fFrac)
{
	SOFT_tdst_AVertex	u_4Vert0,u_4Vert1,u_4Vert2;
	SOFT_tdst_AVertex	stTrpeze;
	SOFT_tdst_AVertex	stNormale;
	SOFT_tdst_AVertex	stGravity;
	MATH_tdst_Vector	stResult;
	float f2PowFrac;
		
	// Vertex buffer
	MATH_SubVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)pPos,pCenter);
	u_4Vert1 = *pXA;// Xa 
	u_4Vert2 = *pYA;// Ya			
	stNormale = *pNormal;
	
	f2PowFrac = (float)(1<<_fFrac);
	
	MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
	MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);

	MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert2.w );
	if (ulMode == 0) // DrawX
	{
		MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
		MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
	}
	else if (ulMode == 1) // DrawY
	{
		MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
		MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,u_4Vert2.w * fRatio);
	}
	else if (ulMode == 2) // DrawH
	{
/*	
		F'(X) = 2AX + B
		F(X) = AX² + BX + C
		
		X E [0,1]
		C = Position(x) de dep
		B = x de la normale
		A = ( x de G - x de N ) / 2
*/			
		stGravity = pWind[pNormal->c ^ p_stII->BaseWind ];
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert2.w );
		
		MATH_SubVector( (MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&stGravity , (MATH_tdst_Vector *)&stNormale );
		MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , stGravity.w  * fEOHP * fEOHP * 0.5f);
		MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
		MATH_AddVector( (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert0, (MATH_tdst_Vector *)&stTrpeze);
		
		MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert2, u_4Vert2.w * fRatio);
		MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&stTrpeze,(MATH_tdst_Vector *)&u_4Vert1,-0.5f * u_4Vert2.w * fRatio);
		MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert2,-0.5f * u_4Vert2.w * fRatio);
		MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert1,(MATH_tdst_Vector *)&u_4Vert0,(MATH_tdst_Vector *)&u_4Vert1,u_4Vert2.w * fRatio);
	}
		
	// First 2 vertices
	MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&u_4Vert1);
	MATH_AddScaleVector(&stResult , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stTrpeze , fTrapeze);
	GXI_v_CompressVector(pVtxBuffer,&stResult,f2PowFrac);
	MATH_AddScaleVector(&stResult , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stTrpeze , -fTrapeze);
	GXI_v_CompressVector(pVtxBuffer+3,&stResult,f2PowFrac);

	// Second 2 vertices
	MATH_AddVector(&stResult , (MATH_tdst_Vector *)&u_4Vert0 , (MATH_tdst_Vector *)&stNormale);
	GXI_v_CompressVector(pVtxBuffer+6,&stResult,f2PowFrac);
	MATH_AddVector(&stResult , (MATH_tdst_Vector *)&u_4Vert1 , (MATH_tdst_Vector *)&stNormale);
	GXI_v_CompressVector(pVtxBuffer+9,&stResult,f2PowFrac);
}

s32 GetExpOfUpperPowerOf2(f32 Value)
{
	s32 SACA;
	*(f32 *)&SACA = Value ;
	SACA --;
	SACA >>= 23;
	SACA &= 0xff;
	SACA -= 127;
	return SACA + 1;
}

#define MAX_SEGMENT_NB 8
#define MIN_LOD_SEGMENT_NB 10000
#define MAX_LOD_SEGMENT_NB 20000
#define MAX_LOD_SEGMENT_COEF 0.2f
#define LOD_COEF ((1.f - MAX_LOD_SEGMENT_COEF)/((float)(MIN_LOD_SEGMENT_NB - MAX_LOD_SEGMENT_NB)))

static int 	UsedCachedLinesNb = 0;
extern BOOL GXI_bDivColorStage1;

#ifndef NO_SPG2
#if (defined(_GAMECUBE) && !defined(_FINAL_))
extern BOOL NoSPG2;
#endif
void GXI_l_DrawSPG2(	SPG2_CachedPrimitivs				*pCachedLine,
							ULONG								*ulTextureID,
							GEO_Vertex					        *XCam,
							GEO_Vertex					        *YCam,
							SPG2_tdst_Modifier					*_pst_SPG2 ,
							SOFT_tdst_AVertex			        *pWind,
							SPG2_InstanceInforamtion			*p_stII)
{
	float fExtractionOfHorizontalPlane;
	ULONG BM,Transparency;
	ULONG ulnumberOfPoints;
	ULONG NumberOfSegments;
	BOOL bNeedCacheTransfer = FALSE;
	
	#if (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoSPG2)
		return;
	#endif

	BM = 0;	
	
	ulnumberOfPoints = pCachedLine->a_PtrLA2 >> 2;
	NumberOfSegments = _pst_SPG2->NumberOfSegments;
	if (NumberOfSegments>>1)
	{
		// If we display too many segments, we decrease fSegmentNbLODCoef so that 
		// the number of segments decreases.
		float fSegmentNbLODCoef;
		
		if (iLastFrameDisplayedPointNb < MIN_LOD_SEGMENT_NB)
			fSegmentNbLODCoef = 1.f;
		else if (iLastFrameDisplayedPointNb > MAX_LOD_SEGMENT_NB)
			fSegmentNbLODCoef = MAX_LOD_SEGMENT_COEF;
		else 
			fSegmentNbLODCoef = 1.f + ((float)(iLastFrameDisplayedPointNb) - MIN_LOD_SEGMENT_NB) * LOD_COEF;
		
			
		NumberOfSegments = (ULONG)((float)NumberOfSegments * p_stII->Culling * fSegmentNbLODCoef);
		if (NumberOfSegments <= 2) 
			NumberOfSegments = 2;
		else if (NumberOfSegments > MAX_SEGMENT_NB)
			NumberOfSegments = MAX_SEGMENT_NB;
			
		iThisFrameDisplayedPointNb += ulnumberOfPoints;
	}

	/* Xaxis lookat ***********************/
	if ((_pst_SPG2->ulFlags & SPG2_XAxisIsInlookat) && (pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagLOOK_AT))
	{
		ULONG Counter;
		SOFT_tdst_AVertex	*Coordinates;
		MATH_tdst_Vector	stCameraDir;
		_pst_SPG2->ulFlags &= ~(SPG2_DrawY|SPG2_RotationNoise);
		_pst_SPG2->ulFlags |= SPG2_DrawX;
		pCachedLine->ulFlags &= ~SPG2_CachedPrimitivsFlagLOOK_AT;
		Coordinates = pCachedLine->a_PointLA2;
		Counter = ulnumberOfPoints;
		MATH_CrossProduct(&stCameraDir,(MATH_tdst_Vector *)YCam,(MATH_tdst_Vector *)XCam);
		while (Counter --)
		{
			MATH_tdst_Vector	LocalX ;
			MATH_CrossProduct(&LocalX , &stCameraDir,(MATH_tdst_Vector *)(Coordinates + 3));
			MATH_NormalizeEqualVector(&LocalX);
			*(MATH_tdst_Vector *)(Coordinates + 2) = LocalX ;
			Coordinates+=4;
		}
	} else
	/* "Random" rotation ***********************/
	if ((_pst_SPG2->ulFlags & SPG2_RotationNoise) && (pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagRAND_ROT))
	{
		ULONG Counter;
		float					CosV,SinV,Swap;
		SOFT_tdst_AVertex	*Coordinates;
		CosV  = 1.0f;
		SinV  = 0.0f;
		Coordinates = pCachedLine->a_PointLA2;
		Counter = ulnumberOfPoints;
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
	
	if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
		MAT_SET_FLAG(BM, MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV);
	else
		MAT_SET_FLAG(BM, MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_AlphaTest | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV);
		
	MAT_SET_Blending(BM , MAT_Cc_Op_Copy);
	MAT_SET_AlphaTresh(BM , _pst_SPG2->AlphaThreshold);

	fExtractionOfHorizontalPlane = _pst_SPG2->fExtractionOfHorizontalPlane + 0.5f;

	Transparency = MAT_Cc_Op_Copy;
	if (_pst_SPG2 ->ulFlags & SPG2_DrawinAlpha)  
	{
		Transparency = MAT_Cc_Op_Alpha;
		MAT_SET_FLAG(BM, MAT_GET_FLAG(BM)| MAT_Cul_Flag_NoZWrite);
	}

	if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
	{
		Transparency = MAT_Cc_Op_AlphaPremult;
		MAT_SET_FLAG(BM, MAT_GET_FLAG(BM)| MAT_Cul_Flag_NoZWrite);
	} 

	// The direct cache line are used for animated characters, which are lighted and thus uses normals 
	
	// In case of 1 segment primitives, 
	// using cache but not normals (not lighted),
	// and without sprites,
	// we compute the geometry with the 12 vertices on shorts,
	// so that it fits in pCachedLine->a_PointLA2.
	/*if ((NumberOfSegments == 1) && 
		!(pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED) && 
		!(_pst_SPG2->ulFlags & SPG2_DontUseCache) &&
		!(_pst_SPG2->ulFlags & SPG2_XAxisIsInlookat) && 
		!GXI_bIsLighted() && 
		!(_pst_SPG2->ulFlags & SPG2_DrawSprites) &&
		!(p_stII->GlobalColor))
		 //&& 	!(pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagGRID))
	{
		ULONG Counter;
		tCmpVertex *pCompressedVtxBuffer;
		SOFT_tdst_AVertex *pPos;
		MATH_tdst_Vector Center;
		s32 Frac; // Fractionnal part of the vertices (nombre de bits après la virgule).
		float fMaxDistance,fInvNbPoints;
		ULONG *colours = pCachedLine->a_ColorLA2;
		
		pCachedLine->ulFlags |= SPG2_CachedPrimitivsFlagCOMPRESSED;
		bNeedCacheTransfer = TRUE;

		//ERR_X_Assert(!(p_stII->GlobalColor));
		
		// Invert colors
		Counter = ulnumberOfPoints;
		while (Counter --)
			SwapDWord((colours+Counter));
			
		// We compute the center of the cache line.		
		MATH_SetNulVector(&Center);
		Counter = ulnumberOfPoints;
		fInvNbPoints = 1.f/(float)ulnumberOfPoints;
		while (Counter--)
		{
			pPos = &(pCachedLine->a_PointLA2[Counter<<2]);
			MATH_AddScaleVector(&Center,&Center,(MATH_tdst_Vector *)pPos,fInvNbPoints);
		}
				
		MATH_CopyVector(&pCachedLine->stCenter,&Center);

		// We compute the max distance between the center and each vertex.
		fMaxDistance = 0;
		Counter = ulnumberOfPoints;
		while (Counter--)
		{
			pPos = &(pCachedLine->a_PointLA2[Counter<<2]);
			fMaxDistance = fMax(fAbs(pPos->x-Center.x),fMaxDistance);
			fMaxDistance = fMax(fAbs(pPos->y-Center.y),fMaxDistance);
			fMaxDistance = fMax(fAbs(pPos->z-Center.z),fMaxDistance);
		} 
		
		// The primitives can have a radius. We assume it is not over 5 meters.
		fMaxDistance += 5.f;
		
		// We compute the fractional part of the vertices. 15 = 16 bits - 1 sign bit.
		Frac = 15 - GetExpOfUpperPowerOf2(fMaxDistance);
		
#ifdef DEBUG
		if ((Frac > 15) || (Frac<0))
			OSReport("Error with max distance in cache line : %f\n",fMaxDistance);
#endif // DEBUG
		pCachedLine->uFrac = (u8)Frac;
		
		// We compute the compressed geometry expressed in s16 from Center.
		// Counter must decrement because we erase pCachedLine->a_PointLA2 as we read it, from the end.
		Counter = ulnumberOfPoints;
		while (Counter--)
		{
			SOFT_tdst_AVertex Pos,XA,YA,Normal;

			// Copy the vertex info, because it will be overwritten.			
			pPos = &(pCachedLine->a_PointLA2[Counter<<2]);
			Pos = *pPos;
			XA = pPos[1];
			YA = pPos[2];
			Normal = pPos[3];
			
			// 36 = 12 vertices per primitive * 3 vertex.
			pCompressedVtxBuffer = &((pCachedLine->a_VtxBuffer)[Counter*36]);
						
			if (_pst_SPG2 ->ulFlags & SPG2_DrawY)
			{
				GXI_v_CompressCrossAndHat(0,p_stII,pWind,pCompressedVtxBuffer,&Pos,&XA,&YA,&Normal,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane,_pst_SPG2->f_GlobalRatio,fMaxDistance,&Center,Frac);
				GXI_v_ComputeTextCoord(0,p_stII,_pst_SPG2->TileNumber,pCachedLine->a_TextureCoordBuffer);
			}
			if (_pst_SPG2 ->ulFlags & SPG2_DrawX)
			{
				GXI_v_CompressCrossAndHat(1,p_stII,pWind,pCompressedVtxBuffer+12,&Pos,&XA,&YA,&Normal,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane,_pst_SPG2->f_GlobalRatio,fMaxDistance,&Center,Frac);				
				GXI_v_ComputeTextCoord(1,p_stII,_pst_SPG2->TileNumber,pCachedLine->a_TextureCoordBuffer+8);
			}
			if (_pst_SPG2 ->ulFlags & SPG2_DrawHat)
			{
				GXI_v_CompressCrossAndHat(2,p_stII,pWind,pCompressedVtxBuffer+24,&Pos,&XA,&YA,&Normal,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane,_pst_SPG2->f_GlobalRatio,fMaxDistance,&Center,Frac);
				GXI_v_ComputeTextCoord(2,p_stII,_pst_SPG2->TileNumber,pCachedLine->a_TextureCoordBuffer+16);
			}
		}
	}*/
	
	
	// For indirect data, get data from CPU cache to DRAM, and from DRAM to GP cache.
	// Not used when SPG2_DontUseCache is active.
	
	ERR_X_Assert(!bNeedCacheTransfer);
	/*if (bNeedCacheTransfer)
	{
		void *pBeginStoreRange;
		u32 iSizeStoreRange; 
		
		// The data to transfer is the color buffer, 
		// and the vertex buffer and texture coord buffer.
		pBeginStoreRange = pCachedLine->a_VtxBuffer;
		iSizeStoreRange = 
			SPG2_PACKET_SIZE*36 * sizeof(tCmpVertex) + 
			SPG2_PACKET_SIZE * sizeof(ULONG) +
			24 * sizeof(float);
		
		// Flush the data from CPU cache into DRAM. 
		// Costly function (stalls CPU), but we do it only at cache line init.
		DCStoreRange(pBeginStoreRange, iSizeStoreRange);
		
		// Unvalidate GP vertex cache so that it will reads data from DRAM.
		GXInvalidateVtxCache();
	}*/
	
	// Set render registers
	GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);		
	
	/*
	For SPG2, we display front and back.
	if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace)
	{
		if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF)
			GX_GXSetCullMode(GX_CULL_FRONT);
		else
			GX_GXSetCullMode(GX_CULL_BACK);
	} 
	else*/
	{
		GX_GXSetCullMode(GX_CULL_NONE);
	}	
		
	GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, (GXCompare)-1, -1);
	
	
	{
		GXColorSrc MatColorSrc;
		if (p_stII->GlobalColor && !(pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED))
		{
			GXColor Color;
//				ERR_X_Assert(!(pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED));
			Color.r = p_stII->GlobalColor & 0x000000FF;
			Color.g = (p_stII->GlobalColor & 0x0000FF00)>>8;
			Color.b = (p_stII->GlobalColor & 0x00FF0000)>>16;
			Color.a = (p_stII->GlobalColor & 0xFF000000)>>24;
			MatColorSrc = GX_SRC_REG;
			GX_GXSetChanMatColor(GX_COLOR0A0, Color);
		}
		else
			MatColorSrc = GX_SRC_VTX;

		GXI_prepare_to_draw_material(MatColorSrc);
	}
	
	//GXI_bDivColorStage1 = TRUE;
	GXI_set_color_generation_parameters(TRUE,TRUE);	
	
	MAT_SET_Blending(BM , Transparency);
	
	if (_pst_SPG2 ->ulFlags & (SPG2_DrawHat|SPG2_DrawX|SPG2_DrawY))
		GXI_SPG2SetVertexFormat(pCachedLine,0,!p_stII->GlobalColor);
				
		
	// Draw the SPG2.
	if (_pst_SPG2 ->ulFlags & SPG2_DrawHat)
	{
		GXI_SetTextureBlending(ulTextureID[0], BM , 0);
		GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
		
//		if (pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED)
//			GXI_l_DrawSPG2_Compressed(pCachedLine,ulnumberOfPoints,2,p_stII);
//		else
		{
#ifndef DEBUG
			if (_pst_SPG2->fTrapeze)
				GXI_l_DrawSPG2_Default(pCachedLine,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,2,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
			else if (NumberOfSegments == 1)
				GXI_l_DrawSPG2_NoTrapezeOneSegment(pCachedLine,ulnumberOfPoints, fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,2,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
			else
				GXI_l_DrawSPG2_NoTrapeze(pCachedLine,ulnumberOfPoints,NumberOfSegments, fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,2,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
#else //DEBUG
			GXI_l_DrawSPG2_Default(pCachedLine,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,2,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
#endif //DEBUG
		}
	}	
	
	if (_pst_SPG2 -> ulFlags & SPG2_DrawY)
	{
		GXI_SetTextureBlending(ulTextureID[1], BM , 0);
		GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
		
//		if (pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED)
//			GXI_l_DrawSPG2_Compressed(pCachedLine,ulnumberOfPoints,0,p_stII);
//		else
		{
#ifndef DEBUG
			if (_pst_SPG2->fTrapeze)
				GXI_l_DrawSPG2_Default(pCachedLine,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
			else if (NumberOfSegments == 1)
				GXI_l_DrawSPG2_NoTrapezeOneSegment(pCachedLine,ulnumberOfPoints,fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
			else
				GXI_l_DrawSPG2_NoTrapeze(pCachedLine,ulnumberOfPoints,NumberOfSegments,fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
#else //DEBUG
			GXI_l_DrawSPG2_Default(pCachedLine,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
#endif //DEBUG
		}
	}

	if (_pst_SPG2 ->ulFlags & SPG2_DrawX)
	{
		GXI_SetTextureBlending(ulTextureID[2], BM , 0);
		GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);

//		if (pCachedLine->ulFlags & SPG2_CachedPrimitivsFlagCOMPRESSED)
//			GXI_l_DrawSPG2_Compressed(pCachedLine,ulnumberOfPoints,1,p_stII);
//		else
		{
#ifndef DEBUG
			if (_pst_SPG2->fTrapeze)
				GXI_l_DrawSPG2_Default(pCachedLine,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,1,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
			else if (NumberOfSegments == 1)
				GXI_l_DrawSPG2_NoTrapezeOneSegment(pCachedLine,ulnumberOfPoints, fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,1,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
			else
				GXI_l_DrawSPG2_NoTrapeze(pCachedLine,ulnumberOfPoints,NumberOfSegments, fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,1,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
#else //DEBUG
			GXI_l_DrawSPG2_Default(pCachedLine,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,1,pWind,p_stII,!p_stII->GlobalColor,GXI_bIsLighted());
#endif //DEBUG
		}
	}
		
	if (_pst_SPG2->ulFlags & SPG2_DrawSprites)
	{
		float CA,SA;
		if (_pst_SPG2 ->ulFlags & SPG2_SpriteRotation)
		{ 
			CA = CosAlpha;
			SA = SinAlpha;
		} else
		{
			CA = -1.0f;
			SA = 0.0f;
		}
		
		GXI_SPG2SetVertexFormat(pCachedLine,1,!p_stII->GlobalColor);
		GXI_SetTextureBlending(ulTextureID[3], BM,0);
		GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
		
		if (GXI_bIsLighted())
		{
			if (!p_stII->GlobalColor)		
				GXI_l_DrawSPG2_SPRITES_DirectLighted(pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,_pst_SPG2->NumberOfSprites,CA,SA,_pst_SPG2->f_SpriteGeneratorRadius,fExtractionOfHorizontalPlane, _pst_SPG2->f_GlobalRatio ,pWind,p_stII);
			else
				GXI_l_DrawSPG2_SPRITES_AmbiantLighted(pCachedLine->a_PointLA2,XCam,YCam,ulnumberOfPoints,_pst_SPG2->NumberOfSprites,CA,SA,_pst_SPG2->f_SpriteGeneratorRadius,fExtractionOfHorizontalPlane, _pst_SPG2->f_GlobalRatio ,pWind,p_stII);
		}
		else
		{
			if (!p_stII->GlobalColor)		
				GXI_l_DrawSPG2_SPRITES_Direct(pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,_pst_SPG2->NumberOfSprites,CA,SA,_pst_SPG2->f_SpriteGeneratorRadius,fExtractionOfHorizontalPlane, _pst_SPG2->f_GlobalRatio ,pWind,p_stII);
			else
				GXI_l_DrawSPG2_SPRITES_Ambiant(pCachedLine->a_PointLA2,XCam,YCam,ulnumberOfPoints,_pst_SPG2->NumberOfSprites,CA,SA,_pst_SPG2->f_SpriteGeneratorRadius,fExtractionOfHorizontalPlane, _pst_SPG2->f_GlobalRatio ,pWind,p_stII);
		}
	}
	
	GX_GXSetCurrentMtx(GX_PNMTX0);
	
	//GXI_bDivColorStage1 = FALSE;
}
#else // NO_SPG2
void GXI_l_DrawSPG2(	SPG2_CachedPrimitivs				*pCachedLine,
							ULONG								*ulTextureID,
							GEO_Vertex					        *XCam,
							GEO_Vertex					        *YCam,
							SPG2_tdst_Modifier					*_pst_SPG2 ,
							SOFT_tdst_AVertex			        *pWind,
							SPG2_InstanceInforamtion			*p_stII)
{
return;
}
#endif // NO_SPG2

