
#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_afterfx.h"
#include "GXI_specialfx.h"
#include "GDInterface/GDInterface.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/Math.h"
#include "GXI_dbg.h"

extern void GXI_set_color_generation_parameters(BOOL _bUseClr, BOOL _bUseTex);

GXTexObj g_AfterFXTextures;
GXTexObj g_AfterFXTextures_BW;
GXTexObj g_AfterFXTextures_O2;
GXTexObj g_AfterFXTextures_MB;
GXTexObj g_AfterFXTextures_ZS;
//GXTexObj g_AfterFXTextures_Misc;
GXTexObj g_AfterFXTextures_ZSR_Save;
GXTexObj g_AfterFXTextures_ZSR;
GXTexObj g_AfterFXTextures_Mul2X;
/*#ifdef _DEBUG
GXTexObj g_AfterFXTextures_ZBuffer;
#endif // _DEBUG*/

#define BLUR_TEXTURE_RESOLUTION_X 128
#define BLUR_TEXTURE_RESOLUTION_Y 100


#define MINUSSULUS 0.0015625f // 1/640 (1 pixel in normalized screen coordinates)


GXTexObj g_BlurTexture;
INT 	 g_AfterFXTexturesUpdated[1];
u32      *p_AfterFXBuffer = NULL;
u32      *p_AfterFXBuffer_ZSR = NULL;
u32      *p_AfterFXBuffer_MB = NULL;


static MATH_tdst_Vector		MotionSmoothPoint; 
float 	fGXI_FadeTime = 0.0f;
float 	fGXI_FadeTimeInc = 0.0f;
void GXI_AE_FADE_PUSH(float TiminSecond);
void GXI_AE_BigBlur(float _fFactor);


float Gsp_AE_MASTER_GET_P1(ULONG AENum , ULONG P1 )
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			switch (P1)
			{
				case 0: return (GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart);
				case 1: return (GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd);
				case 2: return (GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_Factor);
			}
		case 1: // Motion blur
			return (GXI_Global_ACCESS(p_AE_Params)->MotionBlurFactor);
		case 2: // screen Smooth
			return (GXI_Global_ACCESS(p_AE_Params)->BlurFactor);
		case 3: // Emboss
			return (GXI_Global_ACCESS(p_AE_Params)->EmbossFactor);
		case 4: // Motion smooth
			return 0.0f;
		case 5: // Zoom smooth and zoom smooth center
			switch (P1)
			{
				case 0: return (GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor);
				case 1: return (GXI_Global_ACCESS(p_AE_Params)->ZSDir.x);
				case 2: return (GXI_Global_ACCESS(p_AE_Params)->ZSDir.y);
				case 3: return (GXI_Global_ACCESS(p_AE_Params)->ZSDir.z);
			}
		case 6: // Rotate smooth
			return (GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor);
		case 7: // Remanance
			return (GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor);
		case 8: // Brightness
			return (GXI_Global_ACCESS(p_AE_Params)->Brighness);
		case 9: // Contrast
			return (GXI_Global_ACCESS(p_AE_Params)->Contrast);
		case 10: // AA
			return (float)(GXI_Global_ACCESS(p_AE_Params)->AA22BlurMode);
		case 11: // BW
			return (GXI_Global_ACCESS(p_AE_Params)->BWFactor);
		case 12: // Color balance
			switch (P1)
			{
				case 0: return (GXI_Global_ACCESS(p_AE_Params)->CB_Intensity);
				case 1: return (GXI_Global_ACCESS(p_AE_Params)->CB_Spectre);
				case 2: return (GXI_Global_ACCESS(p_AE_Params)->CB_PhotoFilterEffect);
			}
			return 0.0f;
		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: return 0;//(GXI_Global_ACCESS(fFogCorrector));
				case 1: return 0;//(GXI_Global_ACCESS(fFogCorrectorFar));
			}
			return 0.0f;
			break;
		case 14: // Depth blur near
			return (GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_Near);
			break;
		case 16: // blur
			return (GXI_Global_ACCESS(p_AE_Params)->BlurFactor);
			break;
		case 18: // ZoomSmoothRemanence (god rays)
			switch (P1)
			{
				case 0: return GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceFactor;
				case 1: return GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceLightDirection.x;
				case 2: return GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceLightDirection.y;
				case 3: return GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceLightDirection.z;
			}
			break;
		case 20: // AE_BorderBrightness (or FoggyBlur) 
			return (GXI_Global_ACCESS(p_AE_Params)->fBorderBrightnessFactor);
			break;
		case 23: // BorderColor 
			return (GXI_Global_ACCESS(p_AE_Params)->BorderColor);
			break;
			
			
	}
	return 0.0f;
}

ULONG Gsp_AE_MASTER_GET_ONOFF(ULONG AENum )
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			return (GXI_Global_ACCESS(Status)  & GC_Status_AE_DB);
		case 1: // Motion blur
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_MB);
		case 2: // screen Smooth
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_SM);
		case 3: // Emboss
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_EMB);
		case 4: // Motion smooth
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_SMSM);
		case 5: // Zoom smooth and zoom smooth center
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_ZS);
		case 6: // Rotate smooth
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_RS);
		case 7: // Remanance
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_REMANANCE);
		case 10: // AA
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_BlurTest44);
		case 11: // BW
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_BW);
		case 12: // Color balance
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_CB_On);
		case 13: // fOG CORRECTION
			break;
		case 14: // Depth Blur nrear
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_DBN);
			break;
		case 18: // ZoomSmoothRemanence (god rays)
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_ZS_Remanence);
			break;
		case 20: // AE_BorderBrightness (or FoggyBlur) 
		case 23: // BorderColor 
			return (GXI_Global_ACCESS(Status) & GC_Status_AE_BorderBrightness);
			break;
	}
	return 0;
}


void Gsp_AE_MASTER(ULONG AENum , ULONG OnOff , ULONG P1 , float Pf1)
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_DB;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_DB;
			switch (P1)
			{
				case 0: GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart = Pf1;	break;
				case 1: GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd = Pf1;		break;
				case 2: GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_Factor = Pf1;	break;
			}
			break;
		case 1: // Motion blur
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_MB;
			if (OnOff && (Pf1 > 0.05f)) GXI_Global_ACCESS(Status) |= GC_Status_AE_MB;
			GXI_Global_ACCESS(p_AE_Params)->MotionBlurFactor = Pf1;
			break;
		case 2: // screen Smooth
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_SM;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_SM;
			GXI_Global_ACCESS(p_AE_Params)->BlurFactor = Pf1;
			break;
		case 3: // Emboss
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_EMB;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_EMB;
			GXI_Global_ACCESS(p_AE_Params)->EmbossFactor = Pf1;
			break;
		case 4: // Motion smooth
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_SMSM;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_SMSM;
			break;
		case 5: // Zoom smooth and zoom smooth center
			switch (P1)
			{
				case 0: 
				{	
					GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_ZS;
					if (Pf1 > 0.01f)
					{
						if (OnOff) 
							GXI_Global_ACCESS(Status) |= GC_Status_AE_ZS;
					}
					GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor = Pf1;
					break;
				}
				case 1: GXI_Global_ACCESS(p_AE_Params)->ZSDir.x = Pf1;GXI_Global_ACCESS(p_AE_Params)->ZSDirIsValidated = 1;	break;
				case 2: GXI_Global_ACCESS(p_AE_Params)->ZSDir.y = Pf1;GXI_Global_ACCESS(p_AE_Params)->ZSDirIsValidated = 1;	break;
				case 3: GXI_Global_ACCESS(p_AE_Params)->ZSDir.z = Pf1;GXI_Global_ACCESS(p_AE_Params)->ZSDirIsValidated = 1;	break;
			}
			break;
		case 6: // Rotate smooth
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_RS;
			if (Pf1 > 0.001f)
			{
				if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_RS;
			}
			GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor = Pf1;
			break;
		case 7: // Remanance
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_REMANANCE;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_REMANANCE;
			GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor = Pf1;
			break;
		case 8: // Brightness
			GXI_Global_ACCESS(p_AE_Params)->Brighness = Pf1;
			break;
		case 9: // Contrast
			GXI_Global_ACCESS(p_AE_Params)->Contrast  = Pf1;
			break;
		case 10: // AA
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_BlurTest44;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_BlurTest44;
			GXI_Global_ACCESS(p_AE_Params)->AA22BlurMode  = (unsigned int)Pf1;
			break;
		case 11: // BW
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_BW;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_BW;
			GXI_Global_ACCESS(p_AE_Params)->BWFactor  = Pf1;
			break;
		case 12: // Color balance
			switch (P1)
			{
				case 0: GXI_Global_ACCESS(p_AE_Params)->CB_Intensity = Pf1;	break;
				case 1: GXI_Global_ACCESS(p_AE_Params)->CB_Spectre = Pf1;		break;
				case 2: GXI_Global_ACCESS(p_AE_Params)->CB_PhotoFilterEffect = Pf1;	break;
			}
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_CB_On;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_CB_On;
			break;
		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: break;//GXI_Global_ACCESS(fFogCorrector)		 = Pf1;	break;
				case 1: break;//GXI_Global_ACCESS(fFogCorrectorFar) 	 = Pf1;	break;
			}
			break;
		case 14: // Depth Blur nrear
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_DBN;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_DBN;
			GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_Near = Pf1;
			break;
		case 15: // Fade
			GXI_AE_FADE_PUSH(Pf1);
			break;
		case 16: // Blur
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_BLUR;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_BLUR;
			GXI_Global_ACCESS(p_AE_Params)->BlurFactor = Pf1;
			break;
		case 18: // ZoomSmoothRemanance or god rays
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_ZS_Remanence;
			if (OnOff) 
			{
				GXI_Global_ACCESS(Status) |= GC_Status_AE_ZS_Remanence;
			
				switch (P1)
				{
					case 0: GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceFactor = Pf1;	break;
					case 1: GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceLightDirection.x = Pf1;		break;
					case 2: GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceLightDirection.y = Pf1;		break;
					case 3: GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceLightDirection.z = Pf1;		break;
				}
			}
			else
				GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothRemanenceFactor = 0.0f;
			
			break;
			
		case 20: // AE_BorderBrightness (or FoggyBlur) 
			GXI_Global_ACCESS(Status) &= ~ GC_Status_AE_BorderBrightness;
			if (OnOff) GXI_Global_ACCESS(Status) |= GC_Status_AE_BorderBrightness;
			GXI_Global_ACCESS(p_AE_Params)->fBorderBrightnessFactor = Pf1;
			break;
			
		case 23: // BorderColor 
			GXI_Global_ACCESS(p_AE_Params)->BorderColor = P1 & 0xffffff;
			break;
	}
}


void GXI_AfterFX_Init()
{
	
	GXI_Global_ACCESS(p_AE_Params) = (GXI_AfterEffectParams *)MEM_p_Alloc(sizeof (GXI_AfterEffectParams));
	L_memset((void *)GXI_Global_ACCESS(p_AE_Params) , 0 , sizeof(GXI_AfterEffectParams));
	GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart = 5.0f;
	GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd   = 20.0f;
	GXI_Global_ACCESS(p_AE_Params)->Contrast = 0.5f;
	GXI_Global_ACCESS(p_AE_Params)->Brighness = 0.5f;
	GXI_Global_ACCESS(p_AE_Params)->fBorderBrightnessFactor = 0.0f;
	GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor = 0;
	GXI_Global_ACCESS(p_AE_Params)->ZSDirIsValidated = 0;
	
	ERR_X_Assert(p_AfterFXBuffer && p_AfterFXBuffer_ZSR);
	
	
	GXInitTexObj(&g_AfterFXTextures_ZSR, 
 			      p_AfterFXBuffer_ZSR, 
				  (FRAME_BUFFER_WIDTH ),//>>1, 
				  gul_FRAME_BUFFER_HEIGHT,//>>1, 
				  GX_TF_RGBA8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);	
	
	// create previous frame buffer texture
	GXInitTexObj(&g_AfterFXTextures, 
 			      p_AfterFXBuffer, 
				  FRAME_BUFFER_WIDTH, 
				  (gul_FRAME_BUFFER_HEIGHT), 
				  GX_TF_RGBA8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);
				  
	GXInitTexObj(&g_AfterFXTextures_ZSR_Save, 
 			      p_AfterFXBuffer, 
				  FRAME_BUFFER_WIDTH, 
				  (gul_FRAME_BUFFER_HEIGHT), 
				  GX_TF_RGBA8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);

	ERR_X_Assert(GXGetTexBufferSize( FRAME_BUFFER_WIDTH, (gul_FRAME_BUFFER_HEIGHT), GX_TF_I8, FALSE, 0xFF)
				<= GXGetTexBufferSize( FRAME_BUFFER_WIDTH, (gul_FRAME_BUFFER_HEIGHT), GX_TF_RGBA8, FALSE, 0xFF));
				  
	GXInitTexObj(&g_AfterFXTextures_BW, 
 			      p_AfterFXBuffer, 
				  FRAME_BUFFER_WIDTH, 
				  (gul_FRAME_BUFFER_HEIGHT), 
				  GX_TF_I8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);
				  

	GXInitTexObj(&g_AfterFXTextures_O2, 
 			      p_AfterFXBuffer, 
				  FRAME_BUFFER_WIDTH >> 1, 
				  (gul_FRAME_BUFFER_HEIGHT) >> 1, 
				  GX_TF_RGBA8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);
				  

	{
	u32 u32TextureSize = GXGetTexBufferSize( FRAME_BUFFER_WIDTH, 
									 	 (gul_FRAME_BUFFER_HEIGHT), 
									 	 GX_TF_RGBA8, 
									 	 FALSE, 
									 	 0xFF);
	p_AfterFXBuffer_MB = (u32 *)((((u32)p_AfterFXBuffer) + u32TextureSize - FRAME_BUFFER_WIDTH * gul_FRAME_BUFFER_HEIGHT * 2) & ~63);
	}
	GXInitTexObj(&g_AfterFXTextures_MB, 
 			      p_AfterFXBuffer_MB, 
				  FRAME_BUFFER_WIDTH, 
				  (gul_FRAME_BUFFER_HEIGHT), 
				  GX_TF_RGB565, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);
				  
	GXInitTexObj(&g_AfterFXTextures_ZS, 
 			      p_AfterFXBuffer, 
				  FRAME_BUFFER_WIDTH >> 1, 
				  (gul_FRAME_BUFFER_HEIGHT) >> 1, 
				  GX_TF_RGBA8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);
				  
	GXInitTexObj(&g_BlurTexture, 
 			      p_AfterFXBuffer, 
				  BLUR_TEXTURE_RESOLUTION_X, 
				  BLUR_TEXTURE_RESOLUTION_Y, 
				  GX_TF_RGBA8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);
				  
				  
	GXInitTexObj(&g_AfterFXTextures_Mul2X, 
 			      p_AfterFXBuffer_ZSR, 
				  (FRAME_BUFFER_WIDTH ),
				  gul_FRAME_BUFFER_HEIGHT,
				  GX_TF_RGBA8, 
				  GX_CLAMP,
				  GX_CLAMP,
				  FALSE);
				  
#ifdef USE_MY_TEX_REGION
	GXInitTexObjUserData(&g_AfterFXTextures, (void*)0);
#endif
				  
	g_AfterFXTexturesUpdated[0] = 0;
}

float GXI_AE_fDeltaFromQuad(float _fZoom, float _fCenterX, float _fCenterY, float _fDeltaX, float _fDeltaY)
{
	float fMinDeltaX,fMaxDeltaX,fMinDeltaY,fMaxDeltaY;
	fMinDeltaX = fAbs(_fDeltaX - (_fCenterX + 1.f) * _fZoom);
	fMaxDeltaX = fAbs(_fDeltaX - (_fCenterX - 1.f) * _fZoom);
	fMinDeltaY = fAbs(_fDeltaY - (_fCenterY + 1.f) * _fZoom);
	fMaxDeltaY = fAbs(_fDeltaY - (_fCenterY - 1.f) * _fZoom);
	return fMax(fMax(fMinDeltaY,fMaxDeltaY),fMax(fMinDeltaX,fMaxDeltaX));
}

void GXI_AE_DrawQuad(float _fZoom, float _fCenterX, float _fCenterY, float _fDeltaX, float _fDeltaY, u8 _uAlpha)
{
	float fZoomCoef,fMinX,fMaxX,fMinY,fMaxY;
	GXColor matcolor = GX_WHITE;
	
	matcolor.a = _uAlpha;    
	GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);	
	
	fZoomCoef = 1.f + _fZoom;
	fMinX = (-1.f - _fCenterX) * fZoomCoef + _fCenterX + _fDeltaX;
	fMaxX = ( 1.f - _fCenterX) * fZoomCoef + _fCenterX + _fDeltaX;
	fMinY = (-1.f - _fCenterY) * fZoomCoef + _fCenterY + _fDeltaY;
	fMaxY = ( 1.f - _fCenterY) * fZoomCoef + _fCenterY + _fDeltaY;
		
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	
	GXPosition3f32(fMinX, fMinY, -0.5f);
	GXTexCoord2f32(0, 0);
	
	GXPosition3f32(fMaxX, fMinY, -0.5f);
	GXTexCoord2f32(1.0f, 0);
	
	GXPosition3f32(fMaxX, fMaxY, -0.5f);
	GXTexCoord2f32(1.0f, 1.0f);
	
	GXPosition3f32(fMinX, fMaxY, -0.5f);
	GXTexCoord2f32(0, 1.0f);
	GXEnd();
}

void GXI_AE_PrepareBlending(GXColorSrc _BlendColorSrc, u32 _bUseTexture, u32 _bUseZBuffer, GXTevMode _eTevMode,GXBlendFactor _eBlendSrcFactor,GXBlendFactor _eBlendDstFactor) 
{
	// Blend texture to EFB.
	if (_bUseTexture)
		GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
	else
		GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);		
	
	GX_GXSetTevOp(GX_TEVSTAGE0, _eTevMode);	
			
		
	GXSetProjection(GXI_Global_ACCESS(orthogonal2D_matrix), GX_ORTHOGRAPHIC);					
	GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);
		
	GX_GXSetCullMode(GX_CULL_NONE);
				
	//GXI_set_color_generation_parameters(TRUE, TRUE);
	GX_GXSetNumChans(1);     
	if (_bUseTexture)
		GX_GXSetNumTexGens(1);
	else
		GX_GXSetNumTexGens(0);
	
	GX_GXSetNumTevStages(1);
		
	if (_bUseTexture)
		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY,GX_FALSE, GX_PTIDENTITY);

	// During blend , dst_pix_clr = src_pix_clr * src_factor + dst_pix_clr * dst_factor
	// Here, dst_pix_clr = src_pix_clr * src_alpha + dst_pix_clr * (1 - src_alpha)
	GX_GXSetBlendMode(
		GX_BM_BLEND,		// type (GX_BM_NONE, GX_BM_BLEND, GX_BM_LOGIC or GX_BM_SUBTRACT)
		_eBlendSrcFactor,		// src_factor (GX_BL_SRCALPHA : source alpha)
		_eBlendDstFactor,	// dst_factor (GX_BL_INVSRCALPHA : 1 - source alpha)
		GX_LO_NOOP);		// op (used if type == GX_BM_LOGIC)


	GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	
	if (_BlendColorSrc == GX_SRC_VTX)
	{
		GX_GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	}
	else
		GX_GXSetVtxDesc(GX_VA_CLR0, GX_NONE);
		
	if (_bUseTexture)
	{
		GX_GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	}
	else
		GX_GXSetVtxDesc(GX_VA_TEX0, GX_NONE);
			
	GX_GXSetChanCtrl(  GX_COLOR0A0,		// chan (GX_COLOR0A0 : Color channel 0 + Alpha channel 0)
	    			   GX_DISABLE,		// enable Channel ? (GX_DISABLE -> use mat source for output, and ignore amb source).
	    			   _BlendColorSrc,		// amb source (GX_SRC_REG : from color register set by GX_GXSetChanAmbColor)
	    			   _BlendColorSrc,		// mat source (GX_SRC_REG : from color register set by GX_GXSetChanMatColor)
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function
	    

	if (_bUseZBuffer)
		GX_GXSetZMode(GX_ENABLE, GX_GEQUAL, GX_DISABLE);
	else
		GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
}


void GXI_AE_CopyFrameBufferToTexture2(GXTexObj *_pTex , u32 Mode)
{
    extern GXRenderModeObj *g_pst_mode;
    u8 VFilter[7] = {0x00 , 0x00 , 0x00 , 0x3f , 0x00 , 0x01 , 0x00};
    static u8 VFilterZSR[7] = {0x00 , 0x00 , 0x00 , 0x20 , 0x20 , 0x00 , 0x00}; // Total must be 0x40 (64)
    u8 VFilterBLUR[7] = {0x0A , 0x0A , 0x0C , 0x00 , 0x0C , 0x0A , 0x0A};
	// Copy frame buffer image into texture
	if (Mode)
	{
#ifdef DONT_USE_ALPHA_DEST
		if (Mode == 3) // MotionBlur
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_FALSE, NULL); //GX_TRUE,VFilter);
			GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH), gul_FRAME_BUFFER_HEIGHT);
			GXSetTexCopyDst((FRAME_BUFFER_WIDTH), (gul_FRAME_BUFFER_HEIGHT), GX_TF_RGB565, GX_FALSE);
		} else
		if (Mode == 2)
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE,VFilterBLUR);
			GXSetTexCopySrc(512, FrameBufferHeight - BLUR_TEXTURE_RESOLUTION_Y, BLUR_TEXTURE_RESOLUTION_X, BLUR_TEXTURE_RESOLUTION_Y);
			GXSetTexCopyDst(BLUR_TEXTURE_RESOLUTION_X, BLUR_TEXTURE_RESOLUTION_Y, GX_TF_RGBA8, GX_FALSE);
		} else
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE,VFilterBLUR);
			GXSetTexCopySrc(0, 0, FRAME_BUFFER_WIDTH >> 0, gul_FRAME_BUFFER_HEIGHT >> 0);
			GXSetTexCopyDst(FRAME_BUFFER_WIDTH >> 1, (gul_FRAME_BUFFER_HEIGHT) >> 1, GX_TF_RGBA8, GX_TRUE);
		}			
#else	
		GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE,VFilterBLUR);
		GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH) >> Mode, gul_FRAME_BUFFER_HEIGHT >> Mode);
		GXSetTexCopyDst((FRAME_BUFFER_WIDTH) >> 0, (gul_FRAME_BUFFER_HEIGHT) >> 0, GX_TF_RGBA8, GX_FALSE);
#endif		
	}
	else
	{
		/*if (_pTex == &g_AfterFXTextures_Misc)
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_FALSE, NULL); //GX_TRUE,VFilter);
			GXSetTexCopySrc(
				0, 							// left
				0, 							// top
				FRAME_BUFFER_WIDTH, 		// width
				gul_FRAME_BUFFER_HEIGHT);	// height
			GXSetTexCopyDst(
				FRAME_BUFFER_WIDTH,		// width
				gul_FRAME_BUFFER_HEIGHT, 	// height
				GX_TF_RGBA8,					// format
				GX_FALSE);						// mipmap
		} else */
/*#ifdef _DEBUG
		else if (_pTex == &g_AfterFXTextures_ZBuffer)
		{
			GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH), gul_FRAME_BUFFER_HEIGHT);
			GXSetTexCopyDst(FRAME_BUFFER_WIDTH, gul_FRAME_BUFFER_HEIGHT, GX_TF_Z24X8, GX_FALSE);
		} 
#endif // _DEBUG*/
		if (_pTex == &g_AfterFXTextures_ZSR)
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE, VFilterZSR); 
			GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH ), gul_FRAME_BUFFER_HEIGHT);
			GXSetTexCopyDst((FRAME_BUFFER_WIDTH )/*>>1*/, gul_FRAME_BUFFER_HEIGHT/*>>1*/, GX_TF_RGBA8,GX_FALSE/* GX_TRUE*/); // LOD divides width and height by 2.
		} else
		if (_pTex == &g_AfterFXTextures_ZS)
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_FALSE, NULL); //GX_TRUE,VFilter);
			GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH), gul_FRAME_BUFFER_HEIGHT);
			GXSetTexCopyDst((FRAME_BUFFER_WIDTH >> 1), (gul_FRAME_BUFFER_HEIGHT >> 1), GX_TF_RGBA8, GX_TRUE);
		} else
		if (_pTex == &g_AfterFXTextures_BW)
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_FALSE, NULL); //GX_TRUE,VFilter);
			GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH), gul_FRAME_BUFFER_HEIGHT);
			GXSetTexCopyDst((FRAME_BUFFER_WIDTH), (gul_FRAME_BUFFER_HEIGHT), GX_TF_I8, GX_FALSE); // Utiliser GX_CTF_R8 ? Rouge -> blanc et Vert & Bleu -> Noir...
		} else if (_pTex == &g_AfterFXTextures_Mul2X)
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE,VFilter);
			GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH), gul_FRAME_BUFFER_HEIGHT);
			GXSetTexCopyDst((FRAME_BUFFER_WIDTH), (gul_FRAME_BUFFER_HEIGHT), GX_TF_RGBA8, GX_FALSE);
		}
		else
		// if (_pTex == &g_AfterFXTextures)
		{
			GXSetCopyFilter( GX_FALSE,NULL,GX_FALSE, NULL); //GX_TRUE,VFilter);
			GXSetTexCopySrc(0, 0, (FRAME_BUFFER_WIDTH), gul_FRAME_BUFFER_HEIGHT);
			GXSetTexCopyDst((FRAME_BUFFER_WIDTH), (gul_FRAME_BUFFER_HEIGHT), GX_TF_RGBA8, GX_FALSE);
		}
	}
    GXCopyTex(	GXGetTexObjData(_pTex), // texture destination
     			GX_FALSE); // clear ?

    // Wait for finishing the copy task in the graphics pipeline
    GXPixModeSync();
	GXSetCopyFilter(g_pst_mode->aa, g_pst_mode->sample_pattern, GX_TRUE, g_pst_mode->vfilter);
    
}


void GXI_AE_ZSetAlphaBuffer(FLOAT ZBegin  , FLOAT  Zend , UINT Counter)
{
}


void GXI_AE_SendTextureToFrameBuffer_NOLOAD( GXColor *_ambcolor, GXColor *_matcolor, INT DX, INT DY, FLOAT _textureOffsetX, FLOAT _textureOffsetY, FLOAT _rorationFactor , FLOAT XFactor , FLOAT BigScale , u32 QuarterMode)
{	
	static f32 ortho = 1.0f;
	Mtx mtx, mtx2;
	GXSetProjection(GXI_Global_ACCESS(orthogonal2D_matrix), GX_ORTHOGRAPHIC);					
	GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);

	if(_rorationFactor != 0.0f) // rotate using center as point of reference
	{
		MTXTrans(mtx, -0.5f, -0.5f, 0.0f);
		MTXRotRad(mtx2, 'Z', _rorationFactor);
		MTXConcat(mtx2, mtx, mtx2);
		MTXTrans(mtx, 0.5f, 0.5f, 0.0f);
		MTXConcat(mtx, mtx2, mtx2);
		GXLoadTexMtxImm(mtx2, GX_TEXMTX0,  GX_MTX2x4);
		
	}
	{	
		float X0,Y0,X1,Y1;
		X0 = -ortho+((float)DX/(float)FRAME_BUFFER_WIDTH);
		Y0 = -ortho+((float)DY/(float)(gul_FRAME_BUFFER_HEIGHT/2));
		X1 = (ortho+((float)DX/(float)FRAME_BUFFER_WIDTH));
		Y1 = ortho+((float)DY/(float)(gul_FRAME_BUFFER_HEIGHT/2));

		if (QuarterMode)		
		{
			switch(QuarterMode)
			{
				case 1:
					BigScale *= 0.5f;
					X1 = (X0 + X1) * 0.5f;
					Y1 = (Y0 + Y1) * 0.5f;
					break;
			}
		}
		
		XFactor *= BigScale;


		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(X0, Y0, -0.5f);
			GXTexCoord2f32(_textureOffsetX * BigScale, _textureOffsetY * BigScale);
			
			GXPosition3f32(X1, Y0, -0.5f);
			GXTexCoord2f32(XFactor * (1.0f-_textureOffsetX), _textureOffsetY * BigScale);
			
			GXPosition3f32(X1, Y1, -0.5f);
			GXTexCoord2f32(XFactor * (1.0f-_textureOffsetX), (1.0f-_textureOffsetY) * BigScale);
			
			GXPosition3f32(X0, Y1, -0.5f);
			GXTexCoord2f32(_textureOffsetX * BigScale, (1.0f-_textureOffsetY) * BigScale);
		GXEnd();
	}
}
void GXI_AE_POST_SendTextureToFrameBuffer(GXTexObj *_pTex, GXBlendFactor _srcBlend, GXBlendFactor _dstBlend, GXColor *_ambcolor, GXColor *_matcolor, INT DX, INT DY, FLOAT _textureOffsetX, FLOAT _textureOffsetY, FLOAT _rorationFactor , FLOAT XFactor , FLOAT BigScale)
{	
	GX_GXLoadTexObj(_pTex, GX_TEXMAP0);
	GX_GXSetCullMode(GX_CULL_NONE);
			
	//GXI_set_color_generation_parameters(TRUE, TRUE);
	
	if(_rorationFactor != 0.0f) // rotate using center as point of reference
	{
		GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);					
	}
	else
	{
		GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	}

	GX_GXSetBlendMode(GX_BM_BLEND, _srcBlend, _dstBlend, GX_LO_NOOP);

	GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_NONE);
	
    GX_GXSetChanMatColor(GX_COLOR0A0, *_matcolor);    			   
    GX_GXSetChanAmbColor(GX_COLOR0A0, *_ambcolor);    			   
    
	GX_GXSetChanCtrl(  GX_COLOR0A0,
	    			   GX_DISABLE,		// nable Channel
	    			   GX_SRC_REG,		// amb source
	    			   GX_SRC_REG,		// mat source
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_NONE,		// diffuse function
	    			   GX_AF_NONE );	// atten   function
    			   
	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(1);
	GX_GXSetNumTevStages(1);
	
}
void GXI_AE_SendTextureToFrameBuffer(GXTexObj *_pTex, GXBlendFactor _srcBlend, GXBlendFactor _dstBlend, GXColor *_ambcolor, GXColor *_matcolor, INT DX, INT DY, FLOAT _textureOffsetX, FLOAT _textureOffsetY, FLOAT _rorationFactor , FLOAT XFactor , FLOAT BigScale)
{	
	GXI_AE_POST_SendTextureToFrameBuffer(_pTex,_srcBlend, _dstBlend, _ambcolor, _matcolor, DX, DY, _textureOffsetX, _textureOffsetY, _rorationFactor , XFactor , BigScale);
	GXI_AE_SendTextureToFrameBuffer_NOLOAD(_ambcolor, _matcolor, DX, DY, _textureOffsetX, _textureOffsetY, _rorationFactor , XFactor , BigScale , 0);
}


void GXI_AE_SendTextureToFrameBuffer_NOTEVORDER(GXTexObj *_pTex, GXBlendFactor _srcBlend, GXBlendFactor _dstBlend, GXColor *_ambcolor, GXColor *_matcolor, INT DX, INT DY )
{	
	static f32 ortho = 1.0f;
	GXSetProjection(GXI_Global_ACCESS(orthogonal2D_matrix), GX_ORTHOGRAPHIC);					
	GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);
	
	GX_GXSetCullMode(GX_CULL_NONE);
			
	//GXI_set_color_generation_parameters(TRUE, TRUE);
	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(1);
	GX_GXSetNumTevStages(1);
	
	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_GXSetBlendMode(GX_BM_BLEND, _srcBlend, _dstBlend, GX_LO_NOOP);

	GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_NONE);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
	    			   GX_DISABLE,		// enable Channel
	    			   GX_SRC_REG,		// amb source
	    			   GX_SRC_REG,		// mat source
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function
    			   
    GX_GXSetChanMatColor(GX_COLOR0A0, *_matcolor);    			   
    GX_GXSetChanAmbColor(GX_COLOR0A0, *_ambcolor);    			   

	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GX_GXLoadTexObj(_pTex, GX_TEXMAP0);
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-ortho+(DX/FRAME_BUFFER_WIDTH), -ortho+(DY/(gul_FRAME_BUFFER_HEIGHT/2)), -0.5f);
		GXTexCoord2f32(0, 0);
		GXPosition3f32(ortho+(DX/FRAME_BUFFER_WIDTH), -ortho+(DY/(gul_FRAME_BUFFER_HEIGHT/2)), -0.5f);
		GXTexCoord2f32(1.0f-0, 0);
		GXPosition3f32(ortho+(DX/FRAME_BUFFER_WIDTH), ortho+(DY/(gul_FRAME_BUFFER_HEIGHT/2)), -0.5f);
		GXTexCoord2f32(1.0f-0, 1.0f-0);
		GXPosition3f32(-ortho+(DX/FRAME_BUFFER_WIDTH), ortho+(DY/(gul_FRAME_BUFFER_HEIGHT/2)), -0.5f);
		GXTexCoord2f32(0, 1.0f-0);
	GXEnd();
}

void GXI_AE_SendColorQuadToFrameBuffer(GXColor *_matcolor, GXBlendMode _modeBlend, GXBlendFactor _srcBlend, GXBlendFactor _dstBlend, GXLogicOp _opBlend)
{
	static f32 ortho = 1.0f;
	GXSetProjection(GXI_Global_ACCESS(orthogonal2D_matrix), GX_ORTHOGRAPHIC);					
	GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);
	
	GX_GXSetCullMode(GX_CULL_NONE);
			
	//GXI_set_color_generation_parameters(FALSE, TRUE);
	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);	
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);	

	
	GX_GXSetBlendMode(_modeBlend, _srcBlend, _dstBlend, _opBlend);

	GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_NONE);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
	    			   GX_DISABLE,		// enable Channel
	    			   GX_SRC_VTX,		// amb source
	    			   GX_SRC_REG,		// mat source
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function
    		
    GX_GXSetChanMatColor(GX_COLOR0A0, *_matcolor);    			   
    	   
	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-ortho, -ortho, -0.5f);
		GXPosition3f32(ortho, -ortho, -0.5f);
		GXPosition3f32(ortho, ortho, -0.5f);
		GXPosition3f32(-ortho, ortho, -0.5f);
	GXEnd();
}

void GXI_AE_MotionBlur(GXTexObj *g_Texture , UINT _factor)
{
	GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA);

	GX_GXLoadTexObj(&g_AfterFXTextures, GX_TEXMAP0);
	
	GXI_AE_DrawQuad(0,0,0,0,0,_factor); 
}

f32 GetPhase(f32 fColorSpctr)
{
	if (fColorSpctr > 1.0f) fColorSpctr-= 1.0f;
	if (fColorSpctr > 0.5f) fColorSpctr = 1.0f - fColorSpctr;
	if (fColorSpctr < 0.1666666f) return 1.0f;
	if (fColorSpctr > 0.3333333f) return 0.0f;
	fColorSpctr = fColorSpctr - 0.166666666f;
	fColorSpctr *= 6.0f;
	return 1.0f - fColorSpctr;
}

void GXI_AE_BrightnessContrast_ColorBalance(FLOAT Contrast , FLOAT Brighness , FLOAT CB_I , FLOAT CB_S , FLOAT CB_PhotoFilterEffect , FLOAT CB_Pastel)
{
	GXColor matcolor=GX_WHITE;
	float SavedBr = Brighness ;
	
		// ColorBalance 
	if ((GXI_Global_ACCESS(Status) & GC_Status_AE_CB_On) && 
		(CB_I != 0.0f))
	{
		f32 Red,Green,Blue;
		
		Red   = CB_I * (1.0f - GetPhase(CB_S)) + (1.0f - CB_I);
		Green = CB_I * (1.0f - GetPhase(CB_S - 0.3333333f + 1.0f )) + (1.0f - CB_I);
		Blue  = CB_I * (1.0f - GetPhase(CB_S - 0.6666666f + 1.0f)) + (1.0f - CB_I);
		
		matcolor.r = Red*255.0f;
		matcolor.g = Green*255.0f;
		matcolor.b = Blue*255.0f;
		matcolor.a = 128.0f;

		GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_ZERO, GX_BL_SRCCLR , GX_LO_NOOP);
		//GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_DSTCLR, GX_BL_ZERO, GX_LO_NOOP);
	}

	
	// Brightness
	if (Brighness != 0.5f)
	{
		//float W,H ;
		//ULONG Color
		ULONG Invert;
		Brighness -= 0.5f;
		Brighness *= 2.0f;
		Invert = 0;
		if (Brighness < 0.0f) 
		{
			Brighness = -Brighness ;
			Invert = 1;
		}
		if (Brighness > 1.0f) Brighness = 1.0f;
		
		if (Invert)
		{
			matcolor.r = matcolor.g = matcolor.b = 0xFF;
			GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_INVDSTCLR, GX_BL_ZERO, GX_LO_NOOP);
		}
		
		matcolor.r = matcolor.g = matcolor.b = (u8)(Brighness * 255.0f);
		GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
			
		if (Invert)
		{
			matcolor.r = matcolor.g = matcolor.b = 0xFF;
			GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_INVDSTCLR, GX_BL_ZERO, GX_LO_NOOP);
		}
	}
	
	
	/*if (Brighness != 0.5f)
	{
	
		Brighness = fMax(Brighness , 0.0f);
		Brighness = fMin(Brighness , 1.0f);

		if (Brighness < 0.5f)
		{
			matcolor.r = matcolor.g = matcolor.b = 0;
			matcolor.a = ((0.5f-Brighness)*2*255);
			GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
		} 
		else
		{
			matcolor.r = matcolor.g = matcolor.b = matcolor.a = (Brighness-0.5f)*2*255;
			GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
		}
	}*/
	
	// Contrast 
	if (Contrast != 0.5f)
	{
		//float W,H ;
		ULONG Color;

		Contrast -= 0.5f;
		Contrast *= 2.0f;
		if (Contrast > 0.0f)
		{
			while (Contrast > 0.0f)
			{
				Color = (ULONG)(Contrast * 255.0f);
				if (Color > 255) Color = 255;
				matcolor.r = matcolor.g = matcolor.b = Color;
				matcolor.a = 0;
				GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_DSTCLR, GX_BL_ONE, GX_LO_NOOP);
				
				Contrast -= 1.0f;
				if (Contrast > 2.0f) Contrast = 2.0f;
			}
		} else
		{
			Contrast = -Contrast ;
			if (Contrast > 1.0f) Contrast = 1.0f;
			
			SavedBr *= 255.0f;
			if (SavedBr < 0.0f) SavedBr = 0.0f;
			if (SavedBr > 255.0f) SavedBr = 255.0f;
			Color = (int)SavedBr;
			
			matcolor.r = matcolor.g = matcolor.b = Color;
			matcolor.a = (u8)(Contrast * 255.0f);
			GXI_AE_SendColorQuadToFrameBuffer(&matcolor, GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
		}
	}

}

void GXI_AE_FADE_PUSH(float TiminSecond)
{
	if (TiminSecond)
	{
		TiminSecond *= 1.5f;
		fGXI_FadeTime 		= 1.0f;
		fGXI_FadeTimeInc 	= 1.0f / TiminSecond ;
		GXSetDither(GX_TRUE);
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_MB , 3);
		GXSetDither(GX_FALSE);
	}
}

void GXI_AE_DepthBlur(FLOAT ZStart ,FLOAT ZEnd)
{
}


void GXI_AE_MotionSmooth(INT GDX , INT GDY)
{
	GXColor matcolor = {255,255,255,255};
	GXColor ambcolor = {0x80,0x80,0x80,0xff};
	
	UINT Counter , Number;
	INT GDX2 , GDY2 ;
	
	GDX2 = GDX;
	GDY2 = GDY;
	if (GDX < 0) GDX = -GDX;
	if (GDY < 0) GDY = -GDY;
	GDX >>=1;
	GDY >>=1;
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
	
	matcolor.r = matcolor.g = matcolor.b = matcolor.a = 0x80;
	ambcolor.r = ambcolor.g = ambcolor.b = ambcolor.a = 0x80;
	
	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	for (Counter = 0 ; Counter < Number ; Counter++)
	{	
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_ZS , 0);
		GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
		GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);	
		GXI_AE_SendTextureToFrameBuffer(&g_AfterFXTextures_ZS, GX_BL_ONE, GX_BL_ZERO, &ambcolor, &matcolor, -GDX2, -GDY2, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f);
		GXI_AE_SendTextureToFrameBuffer(&g_AfterFXTextures_ZS, GX_BL_ONE, GX_BL_ONE, &ambcolor, &matcolor, GDX2, GDY2, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f);
		GDX2 >>= 1;
		GDY2 >>= 1;
	}
}

	
void GXI_AE_ZoomSmooth(FLOAT Coef)
{
	INT   Counter,Number;
	FLOAT Kinc;
	GXColor matcolor;
	GXColor ambcolor = GX_WHITE;
	
	if ((Coef <= 0.015f) && (Coef >= -0.015f)) return;
	
	matcolor.r = matcolor.g = matcolor.b = 0x80;
	if (Coef < 0.2f)
		matcolor.a = (u8)(Coef * 5.0f * (float)0xFF);
	else
		matcolor.a = 0xFF;
	
	Coef *= 0.25f;

	Number = 4;

	Kinc = Coef / 2.0f;
	
	for (Counter = 0 ; Counter < Number ; Counter++)
	{	
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_ZS , 0);	
		GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
		GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
		GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);	
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
		GXI_AE_SendTextureToFrameBuffer(&g_AfterFXTextures_ZS, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, &ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f);
		GXI_AE_SendTextureToFrameBuffer(&g_AfterFXTextures_ZS, GX_BL_SRCALPHA, GX_BL_ONE, &ambcolor, &matcolor, 0,0, Kinc, Kinc, 0.0f , 1.0f , 1.0f);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
		Kinc *= 0.5f;
	}
}

/*void GXI_DrawQUAD_Rotate(float W,float H,ULONG Color,float Factor)
{
	float XK,YK;
	XK = fCos(Factor);
	YK = fSin(Factor);
	W *= 0.5f;
	H *= 0.5f;
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	glTexCoord2f(0,0);
	glVertex3f (W + (-XK * W - YK * H),H + (YK * W - XK * H),0.0f);
	glTexCoord2f(W * 2.0f,0);
	glVertex3f (W + (XK * W - YK * H),H + (-YK * W - XK * H),0.0f);
	glTexCoord2f(W * 2.0f,H * 2.0f);
	glVertex3f (W + (XK * W + YK * H),H + (-YK * W + XK * H),0.0f);
	glTexCoord2f(0,H * 2.0f);
	glVertex3f (W + (-XK * W + YK * H),H + (YK * W + XK * H),0.0f);
	glEnd( );
}

void GXI_DrawVertexZoom(float X,float Y,float CX,float CY,float Factor)
{
	X = (X - CX) * (Factor+1.0f) + CX;
	Y = (Y - CY) * (Factor+1.0f) + CY;
	glVertex3f (X,Y,0);
}

void GXI_DrawQUAD_Zoom(float W,float H,float XC,float YC,ULONG Color,float Factor)
{
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	glTexCoord2f(0,0);
	OGL_DrawVertexZoom(0,0,XC,YC,Factor);
	glTexCoord2f(W,0);
	OGL_DrawVertexZoom(W,0,XC,YC,Factor);
	glTexCoord2f(W,H);
	OGL_DrawVertexZoom(W,H,XC,YC,Factor);
	glTexCoord2f(0,H);
	OGL_DrawVertexZoom(0,H,XC,YC,Factor);
	glEnd( );
}*/

float GXI_AE_fZSDeltaPos(float _fZoom, float _fCenterX, float _fCenterY)
{
	float fMinDeltaX,fMaxDeltaX,fMinDeltaY,fMaxDeltaY;
	_fZoom -= 1.f;
	fMinDeltaX = fAbs((_fCenterX + 1.f) * _fZoom);
	fMaxDeltaX = fAbs((_fCenterX - 1.f) * _fZoom);
	fMinDeltaY = fAbs((_fCenterY + 1.f) * _fZoom);
	fMaxDeltaY = fAbs((_fCenterY - 1.f) * _fZoom);
	return fMax(fMax(fMinDeltaY,fMaxDeltaY),fMax(fMinDeltaX,fMaxDeltaX));
}

static int GXI_ulMaxZSGCounter = 13;

void GXI_AE_ZoomSmoothGeneric(float _fZoom,float _fAttenuation, float XCenter , float YCenter , int RotMode,GXTexObj *_pTex)
{
	if (RotMode == 2)
	{
		float fMinX,fMaxX,fMinY,fMaxY;
		//float fCurrentZoom;
		u8 uAlpha;
		int iPassCounter = GXI_ulMaxZSGCounter;
		GXColor matcolor = GX_WHITE;
		
		if ((_fZoom <= -1.f) || (_fAttenuation <= 0))
			return;
		
		_fZoom = sqrtf(_fZoom + 1.f);
		if (_fAttenuation > 1.f)
			_fAttenuation = 1.f;
		else
			_fAttenuation = 1.f/ sqrtf(_fAttenuation);
		
		//while (iPassCounter--)
		while (iPassCounter-- && GXI_AE_fZSDeltaPos(_fZoom,XCenter,YCenter) > MINUSSULUS)
		{
			
			GXI_AE_CopyFrameBufferToTexture2(_pTex,0);
			GX_GXLoadTexObj(_pTex, GX_TEXMAP0);
			GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA);
		
			uAlpha = (u8) (255.f * (1.f / (1.f + _fAttenuation)));
			
			// Draw quad
			matcolor.a = uAlpha;    
			GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);	

			fMinX = (-1.f - XCenter) * _fZoom + XCenter;
			fMaxX = ( 1.f - XCenter) * _fZoom + XCenter;
			fMinY = (-1.f - YCenter) * _fZoom + YCenter;
			fMaxY = ( 1.f - YCenter) * _fZoom + YCenter;
	
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(fMinX, fMinY, -0.5f);
			GXTexCoord2f32(0, 0);

			GXPosition3f32(fMaxX, fMinY, -0.5f);
			GXTexCoord2f32(1.0f, 0);

			GXPosition3f32(fMaxX, fMaxY, -0.5f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(fMinX, fMaxY, -0.5f);
			GXTexCoord2f32(0, 1.0f);
			GXEnd();
			
			_fZoom = sqrtf(_fZoom);
			_fAttenuation = sqrtf(_fAttenuation);
		}
	}
}

/*
static BOOL GXI_bInvertBlendMode = FALSE;
static BOOL GXI_bInvertAlphaMode = FALSE;
void GXI_AE_ZoomSmoothGeneric(float _fZoom,float _fAttenuation, float XCenter , float YCenter , int RotMode,GXTexObj *_pTex)
{
	// -1 < _fZoom < infinite
	// 0 < _fAttenuation
	
	if (RotMode == 2)
	{
		float fMinX,fMaxX,fMinY,fMaxY;
		u8 uAlpha;
		GXColor matcolor = GX_WHITE;
		int ulCounter = 0;
		
		_fZoom += 1.f; 
		
		if ((_fZoom <= 0.f) || (_fAttenuation <= 0))
			return;
			
		_fZoom = sqrtf(_fZoom);
		if (_fAttenuation > 1.0f)
			_fAttenuation = 1.0f;
		else
			_fAttenuation = sqrtf(_fAttenuation);
		
		while ((ulCounter < GXI_ulMaxZSGCounter) && (GXI_AE_fZSDeltaPos(_fZoom,XCenter,YCenter) > MINUSSULUS))
		{
			ulCounter++;
			
			GXI_AE_CopyFrameBufferToTexture2(_pTex,0);
			GX_GXLoadTexObj(_pTex, GX_TEXMAP0);
			if (GXI_bInvertBlendMode)
				GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_INVSRCALPHA,GX_BL_SRCALPHA);
			else
				GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA);
			
			if (GXI_bInvertAlphaMode)
				uAlpha = (u8) (255.f *_fAttenuation / (1.f + _fAttenuation));
			else
				uAlpha = (u8) (255.f / (1.f + _fAttenuation));
				
			// Draw quad
			matcolor.a = uAlpha;    
			GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);	

			fMinX = (-1.f - XCenter) * _fZoom + XCenter;
			fMaxX = ( 1.f - XCenter) * _fZoom + XCenter;
			fMinY = (-1.f - YCenter) * _fZoom + YCenter;
			fMaxY = ( 1.f - YCenter) * _fZoom + YCenter;
			
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(fMinX, fMinY, -0.5f);
			GXTexCoord2f32(0, 0);

			GXPosition3f32(fMaxX, fMinY, -0.5f);
			GXTexCoord2f32(1.0f, 0);

			GXPosition3f32(fMaxX, fMaxY, -0.5f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(fMinX, fMaxY, -0.5f);
			GXTexCoord2f32(0, 1.0f);
			GXEnd();
			
			_fZoom = sqrtf(_fZoom);
			_fAttenuation = sqrtf(_fAttenuation);
		}
	}
}

static int ZS_PASS_NB = 1;
//#define ZS_PASS_NB 4
//#define ZS_PASS_FACTOR 1.f/(float)(ZS_PASS_NB+1.f)
void GXI_AE_ZoomSmoothGeneric(float _fZoom,float _fAttenuation, float XCenter , float YCenter , int RotMode,GXTexObj *_pTex)
{
	float ZS_PASS_FACTOR = 1.f/(float)(ZS_PASS_NB+1.f);

	// -1 < _fZoom < infinite
	// 0 < _fAttenuation
	
	if (RotMode == 2)
	{
		float fMinX,fMaxX,fMinY,fMaxY;
		float fCurrentZoom,fAlphaCumul,fAttenuationCumul;
		u8 uAlpha;
		int iPassCounter = GXI_ulMaxZSGCounter;
		GXColor matcolor = GX_WHITE;
		
		if ((_fZoom <= -1.f) || (_fAttenuation <= 0))
			return;
		
		_fZoom = powf(_fZoom + 1.f,1.f/(1.f+ZS_PASS_NB));
		if (_fAttenuation > 1.f)
			_fAttenuation = 1.f;
		else
			_fAttenuation = powf(_fAttenuation, -1.f/(1.f+ZS_PASS_NB));
		
		while (iPassCounter--)
		//while (iPassCounter-- && GXI_AE_fZSDeltaPos(_fZoom,XCenter,YCenter) > MINUSSULUS)
		{
			int iCounter = ZS_PASS_NB;
			
			GXI_AE_CopyFrameBufferToTexture2(_pTex,0);
			GX_GXLoadTexObj(_pTex, GX_TEXMAP0);
			if (GXI_bInvertBlendMode)
				GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_INVSRCALPHA,GX_BL_SRCALPHA);
			else
				GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA);
		
			fCurrentZoom = 1.f;
			fAlphaCumul = 1.f;
			fAttenuationCumul = _fAttenuation;
			
			while (iCounter--)
			{
				float fAlpha;
				
				if (GXI_bInvertAlphaMode)
				{
					fAlpha = fAttenuationCumul / (1.f + fAttenuationCumul);
					fAttenuationCumul *= _fAttenuation * (1.f - fAlpha);
				}
				else
				{
					fAlpha = 1.f / (1.f + fAttenuationCumul * fAlphaCumul);
					fAttenuationCumul *= _fAttenuation;
					fAlphaCumul *= fAlpha;
				}
					
				uAlpha = (u8) (255.f * fAlpha);
				
				fCurrentZoom *= _fZoom;
				
				// Draw quad
				matcolor.a = uAlpha;    
				GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);	
	
				fMinX = (-1.f - XCenter) * fCurrentZoom + XCenter;
				fMaxX = ( 1.f - XCenter) * fCurrentZoom + XCenter;
				fMinY = (-1.f - YCenter) * fCurrentZoom + YCenter;
				fMaxY = ( 1.f - YCenter) * fCurrentZoom + YCenter;
		
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	
				GXPosition3f32(fMinX, fMinY, -0.5f);
				GXTexCoord2f32(0, 0);
	
				GXPosition3f32(fMaxX, fMinY, -0.5f);
				GXTexCoord2f32(1.0f, 0);
	
				GXPosition3f32(fMaxX, fMaxY, -0.5f);
				GXTexCoord2f32(1.0f, 1.0f);
	
				GXPosition3f32(fMinX, fMaxY, -0.5f);
				GXTexCoord2f32(0, 1.0f);
				GXEnd();
			}
			_fZoom = powf(_fZoom,ZS_PASS_FACTOR);
			_fAttenuation = powf(_fAttenuation,ZS_PASS_FACTOR);
		}
	}
}*/



#define ZS_ALPHA_ATTENUATION 0.05f
#define ZS_MUL_FACTOR 1.f

void GXI_AE_ZoomSmoothCenter(float Factor,MATH_tdst_Vector *ZSDir,GXTexObj *_pTex)
{
	float X,Y,fCurrentFocale ;
	MATH_tdst_Vector		ZoomSmoothPoint; 

	MATH_NormalizeVector(ZSDir,ZSDir);
	fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
	MATH_TransformVector(&ZoomSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , ZSDir);
	ZoomSmoothPoint . x = ((fCurrentFocale) * ZoomSmoothPoint . x) / ZoomSmoothPoint . z;
	ZoomSmoothPoint . y = ((fCurrentFocale) * ZoomSmoothPoint . y) / ZoomSmoothPoint . z;
	X = (ZoomSmoothPoint . x);
	Y = -(ZoomSmoothPoint . y);

	Factor *= ZoomSmoothPoint . z * ZoomSmoothPoint . z * ZS_MUL_FACTOR;

	if (ZoomSmoothPoint . z > 0.f)
		GXI_AE_ZoomSmoothGeneric(Factor,ZS_ALPHA_ATTENUATION,X,Y,2,_pTex);
}


static float GXI_fRemanenceAlphaCoef = 2.f;
static float GXI_fRemanenceBlurCoef = 3.f;
void GXI_AE_Remanance(FLOAT Coef)
{
	float fAlpha;
	u8 uAlpha;
//    GXSetScissor(0, 0+WIDE_SCREEN_ADJUST, (u32)FRAME_BUFFER_WIDTH, (u32)FRAME_BUFFER_HEIGTH-2*WIDE_SCREEN_ADJUST);

	// Save frame buffer in high res texture
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_ZSR,0);

	// Create remanence texture
	GX_GXLoadTexObj(&g_AfterFXTextures_ZSR, GX_TEXMAP0);
	GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_DSTCLR,GX_BL_ZERO);
	
	GXI_AE_DrawQuad(0, 0, 0 , 0, 0,255);
	
	// Blur remanence texture
	GXI_AE_BigBlur(Coef*GXI_fRemanenceBlurCoef);

	// Blend blured texture with saved frame buffer
	GX_GXLoadTexObj(&g_AfterFXTextures_ZSR, GX_TEXMAP0);
	GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_ONE,GX_BL_SRCALPHA);
		    
	fAlpha = Coef * 255.f * GXI_fRemanenceAlphaCoef;
	ERR_X_Assert(fAlpha>=0);
	if (fAlpha > 255.f)
		uAlpha = 255;
	else
		uAlpha =  (u8)(fAlpha);
	
	GXI_AE_DrawQuad(0, 0, 0 , 0, 0,uAlpha);
}

/*static float GXI_RemananceCoef = 10.f;
void GXI_AE_Remanance(FLOAT Coef)
{
	GXColor matcolor;
	GXColor ambcolor = GX_WHITE;
	Coef *= GXI_RemananceCoef;
	if ((Coef <= 0.01f) && (Coef >= -0.01f)) return;
	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	matcolor.r = matcolor.g = matcolor.b = matcolor.a = 0xff;
#ifdef DONT_USE_ALPHA_DEST
	{
		f32 vp[GX_VIEWPORT_SZ];
		u32 oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight;

		GXGetScissor(&oldScissorXOrig, &oldScissorYOrig, &oldScissorWidth, &oldScissorHeight);		
		GXGetViewportv(vp);
		
		GXI_AE_POST_SendTextureToFrameBuffer(&g_AfterFXTextures_O2,GX_BL_ONE, GX_BL_ONE, &ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f);
		GXInitTexObjLOD(&g_AfterFXTextures_O2,GX_LINEAR,GX_LINEAR,0.0f,0.0f,0.0d,GX_FALSE,GX_FALSE,GX_ANISO_1 );	
		GX_GXSetColorUpdate(GX_TRUE);	
		GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
	    GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);
	    
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_O2 , 1);
		GX_GXLoadTexObj(&g_AfterFXTextures_O2, GX_TEXMAP0);
		
		// Render in small viewport 
		GXSetViewport(512.0f, (float)(FrameBufferHeight - BLUR_TEXTURE_RESOLUTION_Y), BLUR_TEXTURE_RESOLUTION_X, BLUR_TEXTURE_RESOLUTION_Y, 0.0f, GX_MAX_Z24);
		GXSetScissor(512.0f, (float)(FrameBufferHeight - BLUR_TEXTURE_RESOLUTION_Y), BLUR_TEXTURE_RESOLUTION_X, BLUR_TEXTURE_RESOLUTION_Y);
		GXI_AE_SendTextureToFrameBuffer_NOLOAD(&ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f,0);
		GXI_AE_CopyFrameBufferToTexture2(&g_BlurTexture , 2);
		
		GXSetScissor(oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight);		
		GXSetViewportv(vp);	
		
		Coef *= 255.0f;
		if (Coef < 0.0f)
		{
			Coef = -Coef;
			GX_GXSetBlendMode(GX_BM_SUBTRACT, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
		} else
		{
			GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
		}
		matcolor.r = matcolor.g = matcolor.b = matcolor.a = (u8)Coef;
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, 1, GX_TEVPREV);	
	    GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);
		GX_GXLoadTexObj(&g_BlurTexture, GX_TEXMAP0);
		GXI_AE_SendTextureToFrameBuffer_NOLOAD(&ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f,0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);	
		GX_GXSetTevOpDirty();
	}
#else	
	GXI_AE_POST_SendTextureToFrameBuffer(&g_AfterFXTextures,GX_BL_ONE, GX_BL_ZERO, &ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f);
	GXInitTexObjLOD(&g_AfterFXTextures,GX_LINEAR,GX_LINEAR,0.0f,0.0f,0.0d,GX_FALSE,GX_FALSE,GX_MAX_ANISOTROPY );	
	GXSetTevSwapModeTable(GX_TEV_SWAP2,GX_CH_RED,GX_CH_RED,GX_CH_RED,GX_CH_RED);
	GX_GXSetTevSwapMode(GX_TEVSTAGE0,GX_TEV_SWAP2,GX_TEV_SWAP2);
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures , 0);
	GX_GXLoadTexObj(&g_AfterFXTextures, GX_TEXMAP0);
	GX_GXSetColorUpdate(GX_FALSE);	
	GXI_AE_SendTextureToFrameBuffer_NOLOAD(&ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 2.0f,1);
	GX_GXSetTevSwapMode(GX_TEVSTAGE0,GX_TEV_SWAP0,GX_TEV_SWAP0);
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures , 1);
	GXI_AE_SendTextureToFrameBuffer_NOLOAD(&ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 2.0f,1);
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures , 2);
	GXI_AE_SendTextureToFrameBuffer_NOLOAD(&ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 2.0f,1);
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures , 3);
	GX_GXSetColorUpdate(GX_TRUE);	
	GXSetTevSwapModeTable(GX_TEV_SWAP2,GX_CH_ALPHA,GX_CH_ALPHA,GX_CH_ALPHA,GX_CH_ALPHA);
	GX_GXSetTevSwapMode(GX_TEVSTAGE0,GX_TEV_SWAP2,GX_TEV_SWAP2);
	Coef *= 255.0f;
	if (Coef < 0.0f)
	{
		Coef = -Coef;
		GX_GXSetBlendMode(GX_BM_SUBTRACT, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
	} else
	{
		GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
	}
	matcolor.r = matcolor.g = matcolor.b = matcolor.a = (u8)Coef;
    GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);    			   
	GXI_AE_SendTextureToFrameBuffer_NOLOAD(&ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 0.125f,0);
	GX_GXSetTevSwapMode(GX_TEVSTAGE0,GX_TEV_SWAP0,GX_TEV_SWAP0);
	GXInitTexObjLOD(&g_AfterFXTextures,GX_NEAR,GX_NEAR,0.0f,0.0f,0.0d,GX_FALSE,GX_FALSE,GX_MAX_ANISOTROPY );	
#endif	

}*/

void GXI_AE_RotateSmooth(FLOAT Coef)
{
	INT   Counter,Number;
	FLOAT Kinc;
	GXColor matcolor;
	GXColor ambcolor = GX_WHITE;
	
	if ((Coef <= 0.015f) && (Coef >= -0.015f)) return;

	matcolor.r = matcolor.g = matcolor.b = 0x80;
	if (Coef < 0.2f)
		matcolor.a = (u8)(Coef * 5.0f * (float)0xFF);
	else
		matcolor.a = 0xFF;
	
	
	Number = 4;
	
	Coef *= 3.1415927f / 4.0f;
	Kinc = Coef / 4.0f;
	
	
	for (Counter = 0 ; Counter < Number ; Counter++)
	{	
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_ZS , 0);	
		GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
		GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
		GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);	
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
		GXI_AE_SendTextureToFrameBuffer(&g_AfterFXTextures_ZS, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, &ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, Kinc , 1.0f , 1.0f);
		GXI_AE_SendTextureToFrameBuffer(&g_AfterFXTextures_ZS, GX_BL_SRCALPHA, GX_BL_ONE, &ambcolor, &matcolor, 0, 0, 0.0f, 0.0f, -Kinc , 1.0f , 1.0f);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
		Kinc *= 0.5f;
	}
	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
}
	
#define GXI_fBWCoef 0.5f
void GXI_AE_FrameBufferBW(FLOAT _fBWFactor)
{
	GXColor matcolor = {0xff,0,0,0xff};
	GXColor ambcolor = {0xff,0xff,0xff,0xff};
	
		
	if (_fBWFactor > 1.0f) _fBWFactor = 1.0f;
	if (_fBWFactor < 0.0f) _fBWFactor = 0.0f;
	matcolor.a = (u8) (_fBWFactor * GXI_fBWCoef * 255.0f);
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_BW , 0);
	
	
// out_reg = (d (op) ((1.0 - c)*a + c*b) + bias(-0.5)) * scale(1);	
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
	GXSetTevColorOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO, GX_CS_SCALE_2,GX_ENABLE, GX_TEVPREV); 
	GXSetTevColorIn(GX_TEVSTAGE0,GX_CC_ZERO,GX_CC_ZERO,GX_CC_ZERO   ,GX_CC_TEXC);
	GXSetTevAlphaOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO, GX_CS_SCALE_2,GX_ENABLE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0,GX_CA_ZERO,GX_CA_ZERO,GX_CA_ZERO   ,GX_CA_RASA);//*/
	GX_GXSetTevOpDirty();

	GXI_AE_SendTextureToFrameBuffer_NOTEVORDER(&g_AfterFXTextures_BW, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, &ambcolor, &matcolor, 0, 0);
}

void COMPUTE_COLOR_BB(float XXX,float YYY, float _fFactor)
{
	float X,Y,U,V,fDistToBorder;
	GXColor Color;
	u32 ulColor = GXI_Global_ACCESS(p_AE_Params)->BorderColor;

	Color.r = ulColor & 0xFF;
	Color.g = (ulColor & 0x00FF)>>8;
	Color.b = (ulColor & 0x0000FF)>>16;
	
	U = XXX/(float)FRAME_BUFFER_WIDTH;
	V = YYY/(float)FRAME_BUFFER_HEIGTH;
	X = 2.f * U - 1.f ;
	Y = 2.f * V - 1.f ;
	Y *= FRAME_BUFFER_WIDTH/ FRAME_BUFFER_HEIGTH; // So that the effect is round.
	fDistToBorder = 1.0f - (X * X + Y * Y) * _fFactor;
	if (fDistToBorder < 0) 
		fDistToBorder = 0;
	Color.a = 255 - (u8)(fDistToBorder * 255.f);
	
	GXPosition3f32( 2.f*U-1.f,  2.f*V-1.f, -0.5f);
	GXColor4u8(Color.r,Color.g,Color.b,Color.a);
	//GXTexCoord2f32(U, V);
}

void GXI_AE_BorderBrightness(float _fFactor )
{
	float Wi, Hi;
	float Wb, Hb;
	int CounterX,CounterY;
    	
	GXI_AE_PrepareBlending(GX_SRC_VTX,0,0,GX_PASSCLR,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA);

#define BB_Divider 16
	Wi = FRAME_BUFFER_WIDTH / (float)BB_Divider ;
	Hi = FRAME_BUFFER_HEIGTH / (float)BB_Divider ;
	Wb = 0.0f;
	CounterX = BB_Divider;
	GXBegin(GX_QUADS,GX_VTXFMT0,4*BB_Divider*BB_Divider); 
	while(CounterX--)
	{
		CounterY = BB_Divider ;
		Hb = 0.0f;
		while(CounterY--)
		{
			COMPUTE_COLOR_BB(Wb,Hb,_fFactor);
			COMPUTE_COLOR_BB(Wb+Wi,Hb,_fFactor);
			COMPUTE_COLOR_BB(Wb+Wi,Hb+Hi,_fFactor);
			COMPUTE_COLOR_BB(Wb,Hb+Hi,_fFactor);

			Hb += Hi ;
		}
		Wb += Wi ;
	}
	GXEnd();
	
}


#include "GXI_renderstate.h"
void GXI_AE_ShowTexture()
{
	extern u32 TextnumToShow;
	Mtx44 m2;
	extern GXTexObj g_ShadowTextures[];	
	
#ifdef USE_SELF_SHADOWS	
	extern GXTexObj g_SelfShadowTextures[8];
	
	if(TextnumToShow<=-9 && TextnumToShow>-18 )
		GX_GXLoadTexObj(&g_SelfShadowTextures[(-1*TextnumToShow)-9], GX_TEXMAP0);
	else
#endif

	if(TextnumToShow<=-1 && TextnumToShow>-9 )
		GX_GXLoadTexObj(&g_ShadowTextures[(-1*TextnumToShow)-1], GX_TEXMAP0);
	else
	{
	if(TextnumToShow > GXI_Global_ACCESS(ulNumberOfTextures))
		TextnumToShow = 0;
		
	if(TextnumToShow<0)
		TextnumToShow = GXI_Global_ACCESS(ulNumberOfTextures);
	
	TextnumToShow = GXI_RS_UseTexture(TextnumToShow);
	if(TextnumToShow>0)
		GX_GXLoadTexObj(&GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[TextnumToShow].obj, GX_TEXMAP0);
	}
	
    MTXOrtho(m2,0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    GXSetProjection(m2, GX_ORTHOGRAPHIC);					
	    
    GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
	GX_GXSetZCompLoc(GX_TRUE);
			
	GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetAlphaUpdate(GX_TRUE);
	
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
    GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	    
    GX_GXSetNumChans(0);     
	GX_GXSetNumTexGens(1);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);	
	
		
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		
	GX_GXSetCullMode(GX_CULL_NONE);
		
	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_NONE);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);


	// set lighting (used by the TEV color channel description)
	GX_GXSetChanCtrl(  GX_COLOR0A0,
	    		   		GX_DISABLE,		// enable Channel     : no light
	    		   GX_SRC_VTX,		// amb source	      : use material color
	    		   GX_SRC_VTX,		// mat source		  : use material color
	    		   GX_LIGHT_NULL,	// light mask		  : no light
	    		   GX_DF_NONE,		// diffuse function   : no diffuse
	    		   GX_AF_NONE );	// atten   function   : no light attenuation factor

	GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);	    
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(0.5f, 0.2f, -0.5f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(0.5f, 0.7f, -0.5f);
		GXTexCoord2f32(0.0f, 1.0f);
		GXPosition3f32(1.0f, 0.7f, -0.5f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(1.0f, 0.2f, -0.5f);
		GXTexCoord2f32(1.0f, 0.0f);
	GXEnd();
}



void GXI_AE_BigBlur(float _fFactor)
{
    float fDelta;
    
   	GXColor ambcolor = {0x80,0x80,0x80,0xff}; // Unused anyways...

	fDelta = 10.f * _fFactor;
		 
	GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA);
	
	while (fDelta > 1.f)
	{
		float fDeltaX,fDeltaY;
		
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures,0);

		GX_GXLoadTexObj(&g_AfterFXTextures, GX_TEXMAP0);
	
		fDeltaX = fDelta/(float)(FRAME_BUFFER_WIDTH);
		fDeltaY = fDelta/(float)gul_FRAME_BUFFER_HEIGHT;
		
		GXI_AE_DrawQuad(0,0,0,fDeltaX,fDeltaY,0xff); 		// take 100% from texture, 0% from EFB
		GXI_AE_DrawQuad(0,0,0,fDeltaX,-fDeltaY,0x80);		// take 50% from texture, 50% from EFB
		GXI_AE_DrawQuad(0,0,0,-fDeltaX,fDeltaY,0x55);		// take 33.3% from texture, 66.6% from EFB
		GXI_AE_DrawQuad(0,0,0,-fDeltaX,-fDeltaY,0x40);	// take 25% from texture, 75% from EFB
		
		fDelta *= .5f;
	}
}


#define SKYZ -0.999999f
#define  ZSR_DISTANCE  2.f
#define  ZSR_FACTOR_MULTIPLICATOR  0.05f
#define  ZSR_ALPHA_MULTIPLICATOR  1.f
//static float ZSR_DISTANCE = 5.f;
//static float ZSR_FACTOR_MULTIPLICATOR = 0.0001f;
//static float ZSR_ALPHA_MULTIPLICATOR  = 1.f;

void GXI_AE_ZoomSmoothRemanence(float Factor,MATH_tdst_Vector *_pLigthDir)
{
	float fCurrentFocale,fZoom ;
	float XCenter,YCenter;
	GXColor matColor = {0,0,0,0xff};
	MATH_tdst_Vector	ZoomSmoothPoint; 
	float fAlpha;
	u8 uAlpha;
	
	//MATH_tdst_Vector Camera,LookAt,Right,Down,Point;

	// Compute zoom smooth parameters
	fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);
	MATH_TransformVector(&ZoomSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , _pLigthDir);
	XCenter = ((fCurrentFocale) * ZoomSmoothPoint.x) / ZoomSmoothPoint.z;
	YCenter = 1.333333f * ((fCurrentFocale) * ZoomSmoothPoint.y) / ZoomSmoothPoint.z;
	ZoomSmoothPoint.z = -ZoomSmoothPoint.z;	
	
    GXSetScissor(0, 0, (u32)FRAME_BUFFER_WIDTH, (u32)FRAME_BUFFER_HEIGTH);

	// Render the black bands before in order to avoid white remanence.
	if (WIDE_SCREEN_ADJUST)
	{
		float fBand = 1.f - 2.f * ((float)WIDE_SCREEN_ADJUST)/((float)gul_FRAME_BUFFER_HEIGHT);
		// Draw black bands.
		GXI_AE_PrepareBlending(GX_SRC_REG,0,0,GX_PASSCLR,GX_BL_ONE,GX_BL_ZERO);
		GX_GXSetChanMatColor(GX_COLOR0A0, matColor);	
		GXBegin(GX_QUADS,GX_VTXFMT0,8);
		GXPosition3f32(-1.f, -1.f, 0);
		GXPosition3f32(1.f, -1.f, 0);
		GXPosition3f32(1.f, -fBand, 0);
		GXPosition3f32(-1.f, -fBand, 0);
		
		GXPosition3f32(-1.f, fBand, 0);
		GXPosition3f32(1.f, fBand, 0);
		GXPosition3f32(1.f, 1.f, 0);
		GXPosition3f32(-1.f, 1.f, 0);
		GXEnd();
	}

    GXSetScissor(0, 0+WIDE_SCREEN_ADJUST, (u32)FRAME_BUFFER_WIDTH, (u32)FRAME_BUFFER_HEIGTH-2*WIDE_SCREEN_ADJUST);

	// Save frame buffer in high res texture
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_ZSR_Save,0);
							
	// Blend texture to EFB, and create sky mask.
	GXI_AE_PrepareBlending(GX_SRC_REG,0,1,GX_PASSCLR,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA);
	GX_GXSetChanMatColor(GX_COLOR0A0, matColor);	
	GXBegin(GX_QUADS,GX_VTXFMT0,4);
	GXPosition3f32(-1.f, -1.f, SKYZ);
	GXPosition3f32(1.f, -1.f, SKYZ);
	GXPosition3f32(1.f, 1.f, SKYZ);
	GXPosition3f32(-1.f, 1.f, SKYZ);
	GXEnd();
						
	// Do zoom smooth on mask with low res texture.

	// fZoom (in screen space) must be 0 if ZoomSmoothPoint.z == 0, 
	// close to -1 if ZoomSmoothPoint.z == -1, and big enough for ZoomSmoothPoint.z == 1
	fZoom = ZSR_DISTANCE * (ZoomSmoothPoint.z + 2.f) * ZoomSmoothPoint.z; 
	
	// Do the zoom smooth on the sky mask (with small viewport and texture).
	GXI_AE_ZoomSmoothGeneric(fZoom,ZSR_FACTOR_MULTIPLICATOR ,XCenter,YCenter,2,&g_AfterFXTextures_ZSR);
	
	
	// Blend with saved high res texture	
	GX_GXLoadTexObj(&g_AfterFXTextures_ZSR_Save, GX_TEXMAP0);
	GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_ONE,GX_BL_SRCALPHA);
		    
	fAlpha = Factor * 255.f * ZSR_ALPHA_MULTIPLICATOR;// * ZoomSmoothPoint.z
	ERR_X_Assert(fAlpha>=0);
	if (fAlpha > 255.f)
		uAlpha = 255;
	else
		uAlpha =  (u8)(fAlpha);
	
	GXI_AE_DrawQuad(0, 0, 0 , 0, 0,uAlpha);
}

void GXI_FlipH()
{
	GXColor matColor = {0xff,0xff,0xff,0xff};

	// Save frame buffer in high res texture
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_ZSR_Save,0);

	// Copy texture to frame buffer, inverted.	
	GX_GXLoadTexObj(&g_AfterFXTextures_ZSR_Save, GX_TEXMAP0);
	GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_REPLACE,GX_BL_ONE,GX_BL_ZERO);
	
	GX_GXSetChanMatColor(GX_COLOR0A0, matColor);	
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	
	GXPosition3f32(1.f, -1.f, -0.5f);
	GXTexCoord2f32(0, 0);
	
	GXPosition3f32(-1.f, -1.f, -0.5f);
	GXTexCoord2f32(1.0f, 0);
	
	GXPosition3f32(-1.f, 1.f, -0.5f);
	GXTexCoord2f32(1.0f, 1.0f);
	
	GXPosition3f32(1.f, 1.f, -0.5f);
	GXTexCoord2f32(0, 1.0f);

	GXEnd();
}

void GXI_DoMul2X()
{
	GXColor matColor = {0xff,0xff,0xff,0xff};

	// Save frame buffer in high res texture
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_Mul2X,0);

	// Blend texture to frame buffer.	
	GX_GXLoadTexObj(&g_AfterFXTextures_Mul2X, GX_TEXMAP0);
	GXI_AE_PrepareBlending(GX_SRC_REG,1,0,GX_MODULATE,GX_BL_ONE,GX_BL_ONE);
	
	GX_GXSetChanMatColor(GX_COLOR0A0, matColor);	
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	
	GXPosition3f32(-1.f, -1.f, -0.5f);
	GXTexCoord2f32(0, 0);
	
	GXPosition3f32(1.f, -1.f, -0.5f);
	GXTexCoord2f32(1.0f, 0);
	
	GXPosition3f32(1.f, 1.f, -0.5f);
	GXTexCoord2f32(1.0f, 1.0f);
	
	GXPosition3f32(-1.f, 1.f, -0.5f);
	GXTexCoord2f32(0, 1.0f);

	GXEnd();
}

/*#ifdef _DEBUG
void GXI_StoreZBuffer()
{
	GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_ZBuffer , 0);
}

void GXI_DisplayZBuffer()
{
	GXColor matcolor = {0xff,0,0,0xff};
	GXColor ambcolor = {0xff,0xff,0xff,0xff};
	u32 uColor = 0x80000080 ;
	
	GXI_AE_PrepareBlending(GX_SRC_VTX,1,0);
	GX_GXLoadTexObj(&g_AfterFXTextures_ZBuffer, GX_TEXMAP0);
	
	
	GX_GXSetChanMatColor(GX_COLOR0A0, matcolor);	

	GXBegin(GX_QUADS,GX_VTXFMT0,4);
		
	GXPosition3f32(-1.f, -1.f, SKYZ);
	GXColor1u32(uColor);
	GXTexCoord2f32(0, 0);
	
	GXPosition3f32(1.f, -1.f, SKYZ);
	GXColor1u32(uColor);
	GXTexCoord2f32(1, 0);
	
	GXPosition3f32(1.f, 1.f, SKYZ);
	GXColor1u32(uColor);
	GXTexCoord2f32(1, 1);
	
	GXPosition3f32(-1.f, 1.f, SKYZ);
	GXColor1u32(uColor);
	GXTexCoord2f32(0, 1);
		
	GXEnd();
}

int compareU32 (const void *pA, const void *pB)
{
	return (*((u32 *)pA)) - (*((u32 *)pB));
}

void GXI_GetZBufferStats()
{
	u16 x,y;
	u32 z,zMin,zMax;
	f32 zMean;
	u32 *aZValues;
	
	zMin = 0xFFFFFFFF;
	zMax = 0;
	zMean = 0;
	
	aZValues = (u32 *)MEM_p_Alloc(FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGTH*sizeof(u32));
	
	for (x=0; x<FRAME_BUFFER_WIDTH; x++)
	{
		for (y=0; y<FRAME_BUFFER_HEIGTH; y++)
		{
			GXPeekZ(x,y,&z);
			zMean += (f32)z;
			if (z<zMin)
				zMin = z;
			if (z>zMax)
				zMax = z;
			aZValues[x+y*FRAME_BUFFER_WIDTH] = z;
		}
	}
	zMean /= (float) (FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGTH);
	
	qsort(aZValues,FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGTH,sizeof(u32),&compareU32);
		
	GXPeekZ(FRAME_BUFFER_WIDTH/2,FRAME_BUFFER_HEIGTH/2,&z);
	OSReport("%f\t%d\t%d\t%f\t%d\t%d\n",SKYZ,zMin,zMax,zMean,aZValues[FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGTH/2],z);
	
	MEM_Free(aZValues);
}


#endif // _DEBUG*/

void AE_Splash_AddOne(MATH_tdst_Vector *p2DPosPlusSize , u32 Color )

{
//popo
}


void GXI_ClearZBuffer()
{
	GX_GXSetColorUpdate(GX_FALSE);
	GXI_AE_PrepareBlending(GX_SRC_REG,0,1,GX_PASSCLR,GX_BL_ZERO,GX_BL_ONE);
	
	// Fill Z buffer.
	GX_GXSetZMode(GX_TRUE,GX_ALWAYS,GX_TRUE);
	
	GXBegin(GX_QUADS,GX_VTXFMT0,4);
	GXPosition3f32(-1.f, -1.f, SKYZ);
	GXPosition3f32(1.f, -1.f, SKYZ);
	GXPosition3f32(1.f, 1.f, SKYZ);
	GXPosition3f32(-1.f, 1.f, SKYZ);
	GXEnd();	
	
	GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetZMode(GX_ENABLE,GX_LEQUAL,GX_TRUE);
}


 
//#define GXI_VERIFY
#ifdef GXI_VERIFY
void GXI_GXVerifyCallback (ULONG level,u32 id,char* msg)
{
	static u32 StopID = -1;
	if (StopID == id)
		StopID += (level<<1)  - (level * 2);
	
	OSReport("Level %d, Warning  %d: %s\n",level,id,msg);
}
#endif // GXI_VERIFY



void GXI_AfterFX()
{
	u32 Status;
	GXI_AfterEffectParams   *p_AE_Params;
#ifdef GXI_VERIFY
	BOOL bInitVerify = FALSE;
	if (!bInitVerify)
	{
		bInitVerify = TRUE;
		GXSetVerifyLevel( GX_WARN_SEVERE );
		GXSetVerifyCallback(&GXI_GXVerifyCallback );
	}
#endif // GXI_VERIFY
		
	Status = GXI_Global_ACCESS(Status);
	p_AE_Params = GXI_Global_ACCESS(p_AE_Params);
		
	GXSetDither(GX_TRUE);
	GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetAlphaUpdate(GX_TRUE);
	GXI_set_fog(FALSE);
	// sandstorm is in effect an afterfx
	//GXI_SFX_SandStormRender();
		
	if(Status & GC_Status_AE_ShowTexture)
		GXI_AE_ShowTexture();
		
	GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
	GX_GXSetZCompLoc(GX_FALSE);
	
	//FadePoint = GDI_gpst_CurDD->st_Camera.st_Matrix.T;		
	//Gsp_ClearBorderExtend();
	
	//-------------------------------------------------
	// SPECIAL FX
	//-------------------------------------------------
	
	GXInitTexObjLOD(&g_AfterFXTextures,
	    GX_NEAR,
	    GX_NEAR,
	    0.0f,
	    0.0f,
	    0.0d,
	    GX_FALSE,
	    GX_FALSE,
	    GX_MAX_ANISOTROPY );	
	    
	// motion blur 1
/*	if (fGXI_FadeTime)	
	{
		GXI_AE_MotionBlur(&g_AfterFXTextures_MB,(UINT)(fGXI_FadeTime * (FLOAT)0x80));
	}
	else
	{
		if ((Status & GC_Status_AE_MB))
		{
			if (p_AE_Params->MotionBlurFactor <= 0.1f)
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_MB;
			else
				GXI_AE_MotionBlur(&g_AfterFXTextures_MB,(UINT)(p_AE_Params->MotionBlurFactor * (FLOAT)0xff));
		}
	}*/
	

	/* A remettre selon les besoins 
	// depth blur
	if (GXI_Global_ACCESS(Status) & GC_Status_AE_DB)	
		GXI_AE_DepthBlur(GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart, GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd);
	//if (GXI_Global_ACCESS(Status) & GC_Status_AE_DBN)	
	//	GXI_AE_DepthBlurNear(GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_Near, 1.0f);
	*/

	// Mul 2X is done here by copying the frame buffer onto itself
	if (g_bUseHWMul2x)
		GXI_DoMul2X();
		
	if (Status & GC_Status_AE_BW) 
		GXI_AE_FrameBufferBW( p_AE_Params->BWFactor );
			
	if (Status & GC_Status_AE_BorderBrightness) 
		GXI_AE_BorderBrightness( p_AE_Params->fBorderBrightnessFactor );


	{
	extern int JADE_HorizontalSymetry;
	JADE_HorizontalSymetry = GDI_gpst_CurDD->GlobalXInvert;
	if (JADE_HorizontalSymetry)
	{
		GXI_FlipH();
	}
	}

#ifdef _DEBUG
	//GXI_StoreZBuffer();
#endif // _DEBUG

	if (Status & GC_Status_AE_ZS_Remanence) 
		GXI_AE_ZoomSmoothRemanence( p_AE_Params->ZoomSmoothRemanenceFactor,&(p_AE_Params->ZoomSmoothRemanenceLightDirection));
		
	// brightness, contrast and color balance	
	GXI_AE_BrightnessContrast_ColorBalance(GXI_Global_ACCESS(p_AE_Params)->Contrast , GXI_Global_ACCESS(p_AE_Params)->Brighness , GXI_Global_ACCESS(p_AE_Params)->CB_Intensity , GXI_Global_ACCESS(p_AE_Params)->CB_Spectre , GXI_Global_ACCESS(p_AE_Params)->CB_PhotoFilterEffect , GXI_Global_ACCESS(p_AE_Params)->CB_Pastel);
		
	if (Status & GC_Status_AE_SM) //GC_Status_AE_BLUR) 
		GXI_AE_BigBlur(p_AE_Params->BlurFactor );
	
	/*	 A remettre selon les besoins 
	// blur 
	if (GXI_Global_ACCESS(Status) & GC_Status_AE_BLUR)	
		GXI_AE_MotionSmooth((int)GXI_Global_ACCESS(p_AE_Params)->BlurFactor , (int)GXI_Global_ACCESS(p_AE_Params)->BlurFactor);

	// motion smooth
	if (GXI_Global_ACCESS(Status) & GC_Status_AE_SMSM)	
	{
		int XX,YY;
		float fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
		MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &MotionSmoothPoint);
		MotionSmoothPoint . x = ((fCurrentFocale) * MotionSmoothPoint . x) / MotionSmoothPoint . z;
		MotionSmoothPoint . y = ((fCurrentFocale) * MotionSmoothPoint . y) / MotionSmoothPoint . z;
		XX = (int)(0.75f * GXI_Global_ACCESS(Xsize) * MotionSmoothPoint . x);
		YY = (int)(0.375f * GXI_Global_ACCESS(Ysize) * MotionSmoothPoint . y);
		GXI_AE_MotionSmooth((int)XX , (int)YY);	
		MotionSmoothPoint.x = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Iz;
		MotionSmoothPoint.y = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Jz;
		MotionSmoothPoint.z = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Kz;
	}*/
		
		
	if (p_AE_Params->ZoomSmoothFactor)			
	{
		if (p_AE_Params->ZSDirIsValidated)
			GXI_AE_ZoomSmoothCenter(p_AE_Params->ZoomSmoothFactor * 0.2f ,&p_AE_Params->ZSDir,&g_AfterFXTextures_ZSR_Save);
		else
			GXI_AE_ZoomSmooth(p_AE_Params->ZoomSmoothFactor);
		p_AE_Params->ZSDirIsValidated = 0;
	}
		

	

	// zoom smooth
	if (GXI_Global_ACCESS(Status) & GC_Status_AE_ZS)
		GXI_AE_ZoomSmooth(GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor);
	
	// rotate smooth
	if (GXI_Global_ACCESS(Status) & GC_Status_AE_RS)
		GXI_AE_RotateSmooth(GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor);
		
	if (GXI_Global_ACCESS(Status) & GC_Status_AE_REMANANCE)   	
		GXI_AE_Remanance(GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor);


	// Warp
	
	//-------------------------------------------------
	
    GXSetViewport(0.0f, 0.0f, (f32)FRAME_BUFFER_WIDTH, (f32)FRAME_BUFFER_HEIGTH, 0.0f, 1.0f);
    GXSetScissor(0, 0, (u32)FRAME_BUFFER_WIDTH, (u32)FRAME_BUFFER_HEIGTH - (u32)(WIDE_SCREEN_ADJUST * 0.5f));//*/
	GXInitTexObjLOD(&g_AfterFXTextures,
	    GX_LINEAR,
	    GX_LINEAR,
	    0.0f,
	    0.0f,
	    0.0d,
	    GX_FALSE,
	    GX_FALSE,
	    GX_MAX_ANISOTROPY );	

#ifdef FALSE_640	
/*	{
		GXColor ambcolor = {0x80,0x80,0x80,0xff};
		GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures , 0);
		GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
		GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);	
		GXI_AE_SendTextureToFrameBuffer(&g_AfterFXTextures, GX_BL_ONE, GX_BL_ZERO, &ambcolor, &GX_WHITE, 0, 0, 0.0f, 0.0f, 0.0f , 1.0f , 1.0f);
	}//*/
#endif	

	
	// Clear Z buffer
	GXI_ClearZBuffer();

#ifndef DONT_USE_ALPHA_DEST
	GXSetDither(GX_TRUE);
#endif	

	/*{
		extern void GC_VideoFullScreenUpdate();
		extern GXTexObj 	*p_VideoTexture;
		extern u32      	*p_VideoTextureBuffer;
		GC_VideoFullScreenUpdate();
		if (p_VideoTextureBuffer)
		{
			GXI_AE_MotionBlur(p_VideoTexture,0x80);
		}
	}
	// motion blur 2
	if (fGXI_FadeTime) 
	{
		extern float TIM_gf_dt;
		fGXI_FadeTime -= fGXI_FadeTimeInc * TIM_gf_dt;
		if (fGXI_FadeTime <= 0.0f)
		{
			fGXI_FadeTime = 0.0f;
		}
	} */
	/*if (Status & GC_Status_AE_MB)
	{
		GXSetDither(GX_TRUE);
		//GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures_MB , 3);
		
		// Copy frame buffer image into texture
		GXI_AE_CopyFrameBufferToTexture2(&g_AfterFXTextures,0);
		
		GXSetDither(GX_FALSE);
	}*/


#ifdef _DEBUG
/*	{
		static int bStartZBufferTest = 0;
		static int bDoZBufferTest = 0;
		static f32 fSkyIncrement = 0.1f;
		static f32 fSkyStart = -10.f;
		static f32 fSkyEnd = 10.f;
		if (bStartZBufferTest)
		{
			bDoZBufferTest = 1;
			bStartZBufferTest = 0;
			SKYZ = fSkyStart;
			OSReport("SKYZ\tMin\tMax\tMean\tMedian\tCenter\n");
		}
		
		if (bDoZBufferTest)
		{
			// Display quad and read zbuffer.
			GXI_AE_ZoomSmoothRemanence( p_AE_Params->ZoomSmoothRemanenceFactor,&(p_AE_Params->ZoomSmoothRemanenceLightDirection));
			
			GXI_GetZBufferStats();
			
			SKYZ += fSkyIncrement;
			if (SKYZ>=fSkyEnd)
				bDoZBufferTest = 0;
		}
	}			*/
	//GXI_DisplayZBuffer();
#endif // _DEBUG	
	
	// keep note of updated textures
	g_AfterFXTexturesUpdated[0] = max(0, g_AfterFXTexturesUpdated[0]-1);
}