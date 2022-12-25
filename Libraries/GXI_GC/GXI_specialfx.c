
#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_specialfx.h"
#include "GDInterface/GDInterface.h"
#include "TIMer/TIMdefs.h"
#include "BASe/MEMory/MEM.h"

extern void GXI_set_fog(BOOL _enable);

tdstSpecialFXGlobal g_SpecialFXGlobals;

FLOAT g_fEffectiveSpeed;

#if 0
void GXI_SFX_Init()
{
	GXI_SFX_SandStormInit();
}

//-----------------------------------------------------------------------------------
//       S A N D S T O R M
//-----------------------------------------------------------------------------------

void GXI_SFX_SandStormInit()
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	UCHAR *data;
	INT i;
	INT u32TextureSize = GXGetTexBufferSize( SPECIALFX_SAND_TEXTURE_SIZE, SPECIALFX_SAND_TEXTURE_SIZE, GX_TF_I8, FALSE, 0xFF );
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	data = (UCHAR*)MEM_p_AllocAlign(u32TextureSize, 32);		
	
	// init data to random values
	// the values will be used as alpha 
	for(i=0;i<SPECIALFX_SAND_TEXTURE_SIZE*SPECIALFX_SAND_TEXTURE_SIZE;i++)
	{
		data[i] = (UCHAR)(lRand(0, 255));
	}
		
	GXInitTexObj(&g_SpecialFXGlobals.oSandStormTexture, 
			 (void*)data, 
			 SPECIALFX_SAND_TEXTURE_SIZE, 
			 SPECIALFX_SAND_TEXTURE_SIZE, 
			 GX_TF_I8, 
			 GX_REPEAT, 
			 GX_REPEAT, 
			 GX_FALSE); 
			 
	GXInitTexObjLOD(&g_SpecialFXGlobals.oSandStormTexture,
				GX_LINEAR,
				GX_LINEAR,
				0,
				0,
				0,
				0,
				0,
				GX_ANISO_1);
    					
   	GXInitTexObjUserData(&g_SpecialFXGlobals.oSandStormTexture, (void*)0);
   	
	// sandstorm init
	g_SpecialFXGlobals.fSandStormIntensity = 0.80f; // this is what is considered a good sandstorm
	*(ULONG*)&g_SpecialFXGlobals.lSandStormColor = 0xEFCA9B00;
	g_SpecialFXGlobals.bSandStormActive = FALSE;
	g_SpecialFXGlobals.iSandStormCurrentStep = 0;
	g_SpecialFXGlobals.fSandStormIntensityStart = 0.0f;
	g_SpecialFXGlobals.fSandStormSpeed = 1.0f;
	g_SpecialFXGlobals.fSandStormAmplitude = 1.0f;
	g_SpecialFXGlobals.fSandStormGrainSizeFront = 1.0f;
	g_SpecialFXGlobals.fSandStormGrainSizeBack = 2.0f;
	
	g_SpecialFXGlobals.vWindDirection = (Vec){1.0f, 0.0f, 0.0f};
}


// change storm from intensity fstart to intensity fstop in ftime seconds
void GXI_SFX_SandStormSetIntensity(FLOAT _fStart, FLOAT _fEnd, FLOAT _fTime)
{
	if(_fTime>0.0f)
	{
		g_SpecialFXGlobals.fSandStormIntensityStart = _fStart; 
		g_SpecialFXGlobals.fSandStormIntensityStop = _fEnd;
		g_SpecialFXGlobals.fSandStormTimeStart = TIM_f_Clock_TrueRead();
		g_SpecialFXGlobals.fSandStormTimeStop = g_SpecialFXGlobals.fSandStormTimeStart+_fTime;
	}
	
	g_SpecialFXGlobals.fSandStormIntensity = _fStart; 
}


void GXI_SFX_SandStormUpdate()
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FLOAT newtime;	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	newtime = TIM_f_Clock_TrueRead();
	
	if(g_SpecialFXGlobals.fSandStormIntensityStart>0.0f)
	{
		if(g_SpecialFXGlobals.fSandStormTimeStop>newtime) 
		{
			g_SpecialFXGlobals.fSandStormIntensity = g_SpecialFXGlobals.fSandStormIntensityStop;
			g_SpecialFXGlobals.fSandStormIntensityStart = 0.0f;
		}
		else
		{
			g_SpecialFXGlobals.fSandStormIntensity = g_SpecialFXGlobals.fSandStormIntensityStart + (g_SpecialFXGlobals.fSandStormIntensityStop-g_SpecialFXGlobals.fSandStormIntensityStart)*((newtime-g_SpecialFXGlobals.fSandStormTimeStart)/(g_SpecialFXGlobals.fSandStormTimeStop-g_SpecialFXGlobals.fSandStormTimeStart));
		}
	}
	
	g_SpecialFXGlobals.iSandStormCurrentStep += (UINT)((newtime-g_SpecialFXGlobals.fSandStormTimeCurrent)*60.0f);
	g_SpecialFXGlobals.iSandStormCurrentStep %= SPECIALFX_SAND_TEXTURE_SIZE;
	g_SpecialFXGlobals.fSandStormTimeCurrent = newtime;
	
	{
		Vec camorient;
		f32 speedadjust;
		
		MTXMultVec(*(Mtx*)&GDI_gpst_CurDD->st_Camera.st_Matrix, &g_SpecialFXGlobals.vWindDirection, &camorient);
		VECNormalize(&camorient, &camorient);
		
		speedadjust = VECDotProduct(&camorient, &g_SpecialFXGlobals.vWindDirection);
		
		if(speedadjust<0.0f)
			g_fEffectiveSpeed = g_SpecialFXGlobals.fSandStormSpeed*(speedadjust);
		else
			g_fEffectiveSpeed = g_SpecialFXGlobals.fSandStormSpeed*(speedadjust);
	}
}


void GXI_SFX_DrawSandIntensityFilter()
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	GXColor sandfogcolor = g_SpecialFXGlobals.lSandStormColor;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	if(g_SpecialFXGlobals.fSandStormIntensity<0.1) return;

	// change alpha/intensity of fog. add a variation/modulation with the sin using a 0-pi/4 range and the current step
	sandfogcolor.a = (g_SpecialFXGlobals.fSandStormIntensity)*60 + \
					 sin( ((f32)g_SpecialFXGlobals.iSandStormCurrentStep/SPECIALFX_SAND_TEXTURE_SIZE) * 3.141592654f ) *30;
	
	GX_GXSetVtxDesc(GX_VA_POS,   GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,   GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0,  GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
	GX_GXSetVtxDesc(GX_VA_CLR1,  GX_NONE);
#endif
	GX_GXSetVtxDesc(GX_VA_TEX0,  GX_NONE);
	
	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);	

	GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);//alpha
	
	GX_GXSetChanCtrl(  GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE );
	GX_GXSetChanAmbColor( GX_COLOR0A0, GX_BLACK );
	GX_GXSetChanMatColor( GX_COLOR0A0, sandfogcolor );	
	
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
	GX_GXSetZCompLoc(GX_FALSE);

	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);	

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-1.0f, -1.0f, 0);
		GXPosition3f32(1.0f, -1.0f, 0);
		GXPosition3f32(1.0f, 1.0f, 0);
		GXPosition3f32(-1.0f, 1.0f, 0);
	GXEnd();
}


void GXI_SFX_SandStormRender()
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	f32   pm[GX_PROJECTION_SZ];
	Mtx   mtx;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if(!g_SpecialFXGlobals.bSandStormActive)
		return;
	
	GXI_SFX_SandStormUpdate();
	
	// set fog params
	GXI_Global_ACCESS(MegaFogParams).FogType = GX_FOG_LIN;
	GXI_Global_ACCESS(MegaFogParams).FogStart = 10.0f;
	GXI_Global_ACCESS(MegaFogParams).FogEnd = 5000.0f * (1.0f-g_SpecialFXGlobals.fSandStormIntensity);
	GXI_Global_ACCESS(MegaFogParams).FogDensity = 1.0f;
	GXI_Global_ACCESS(MegaFogParams).FogColor = g_SpecialFXGlobals.lSandStormColor; 
	
	// save the context
	GXGetProjectionv(pm);

	GXSetProjection(GXI_Global_ACCESS(orthogonal2D_matrix), GX_ORTHOGRAPHIC);					
		
	GX_GXSetCullMode(GX_CULL_NONE);
	
	GXI_set_fog(FALSE);
	
	GX_GXLoadTexObj(&g_SpecialFXGlobals.oSandStormTexture, GX_TEXMAP0);

	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);

	GXI_SFX_DrawSandIntensityFilter();	
	
	GX_GXSetVtxDesc(GX_VA_POS,   GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,   GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0,  GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
	GX_GXSetVtxDesc(GX_VA_CLR1,  GX_NONE);
#endif
	GX_GXSetVtxDesc(GX_VA_TEX0,  GX_DIRECT);
			
	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(1);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	

	//GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);//copy
	GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);//alpha

	// use color from specified sandcolor and alpha from the texture	
	GX_GXSetChanCtrl(  GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE );	
	GX_GXSetChanCtrl(  GX_ALPHA0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE );	

	GX_GXSetChanAmbColor( GX_COLOR0A0, GX_BLACK );
	
	// sand color is always at full intensity (if the alphatest is passed)
	g_GXI_HW_States.opMode = GX_DECAL; // force state change
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ONE, GX_CA_ZERO);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
	GX_GXSetTevOpDirty();

	GX_GXSetAlphaCompare(GX_GREATER, (1.0f-g_SpecialFXGlobals.fSandStormIntensity)*255, GX_AOP_OR, GX_NEVER, 0);
	GX_GXSetZCompLoc(GX_FALSE);	
	
	GX_GXSetChanMatColor( GX_COLOR0A0, g_SpecialFXGlobals.lSandStormColor );	
		
	// "front" sand : goes for upleft to downright rapidly with sinus variation
	MTXIdentity(mtx);
	mtx[0][3] = g_fEffectiveSpeed*(-8.0f)*((g_SpecialFXGlobals.iSandStormCurrentStep)%SPECIALFX_SAND_TEXTURE_SIZE)/(f32)SPECIALFX_SAND_TEXTURE_SIZE;
	mtx[1][3] = g_SpecialFXGlobals.fSandStormAmplitude*(-0.25f)*sin(((f32)g_SpecialFXGlobals.iSandStormCurrentStep/SPECIALFX_SAND_TEXTURE_SIZE)*2*3.1415965);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0,  GX_MTX2x4);
		
	// "back" sand : goes for downleft to upright rapidly using smaller sand with sinus variation
	mtx[0][3] = g_fEffectiveSpeed*(-4.0f)*((g_SpecialFXGlobals.iSandStormCurrentStep+10)%SPECIALFX_SAND_TEXTURE_SIZE)/(f32)SPECIALFX_SAND_TEXTURE_SIZE;
	mtx[1][3] = g_SpecialFXGlobals.fSandStormAmplitude*(0.15f)*sin(((f32)g_SpecialFXGlobals.iSandStormCurrentStep/SPECIALFX_SAND_TEXTURE_SIZE)*2*3.1415965);
	GXLoadTexMtxImm(mtx, GX_TEXMTX1,  GX_MTX2x4);
	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);					
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-1.0f, -1.0f, 0);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(1.0f, -1.0f, 0);
		GXTexCoord2f32((f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeFront, 0.0f);
		GXPosition3f32(1.0f, 1.0f, 0);
		GXTexCoord2f32((f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeFront, (f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeFront);
		GXPosition3f32(-1.0f, 1.0f, 0);
		GXTexCoord2f32(0.0f, (f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeFront);
	GXEnd();
	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);					
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-1.0f, -1.0f, 0);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(1.0f, -1.0f, 0);
		GXTexCoord2f32((f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeBack, 0.0f);
		GXPosition3f32(1.0f, 1.0f, 0);
		GXTexCoord2f32((f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeBack, (f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeBack);
		GXPosition3f32(-1.0f, 1.0f, 0);
		GXTexCoord2f32(0.0f, (f32)FRAME_BUFFER_WIDTH/SPECIALFX_SAND_TEXTURE_SIZE*g_SpecialFXGlobals.fSandStormGrainSizeBack);
	GXEnd();
	
	
	// revert to the saved context
	GXSetProjectionv(pm);
}
#endif // #if 0