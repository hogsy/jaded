#include "GDInterface/GDInterface.h"
#include "GX8\Gx8init.h"
#include "GDInterface/GDIrasters.h"

#include "BASe/MEMory/MEM.h"
#ifndef _AFTERFX_DEF_H_
#include "Gx8AfterFX_Def.h"
#endif

#include "Gx8\Gx8renderstate.h"

#include "Gx8\Gx8GPUMon.h"

#ifdef _XBOX
#include <xgraphics.h>
void Gx8_MemorySpy(void *Dev);
#endif

#include "Gx8AfterFX_OPT.h"
extern u32 JADE_HorizontalSymetry;

// Before
//#define HIGHBAND	70
//#define LOWBAND		410
extern BOOL Antialias_E3;
BOOL flag_MemorySpy=0;

#define MINUSSULUS 0.0015f
#define C_ZOOM 0.7f

#define MEM_SPY


static float SKYZ = 0.999999f;

#define HIGHBAND	0
#define LOWBAND		480
#define Gx8_M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)
extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;

//TV Option
extern u_int TVMode;
//extern bool Render3do;

int nearAlphaAdjustment=125;
bool	g_bOptimizedReimanance = true;
static bool SwapOneTime=false;

u_int gAE_Status;
u_int gAE_OldStatus=0;
GSP_AfterEffectParams gAE_Params={0.5f,0.5f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f};
static MATH_tdst_Vector		MotionSmoothPoint; 
static MATH_tdst_Vector		OldMotionSmoothPoint; 

void AE_SwapTextures(IDirect3DTexture8 **sourceTexture,IDirect3DTexture8 **destTexture)
{
	IDirect3DTexture8 *tmpTexture;

	tmpTexture=*sourceTexture;
	*sourceTexture=*destTexture;
	*destTexture=tmpTexture;				
	SwapOneTime = true; 
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

float Gsp_AE_MASTER_GET_P1(ULONG AENum , ULONG P1 )
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			switch (P1)
			{
				case 0: return (gAE_Params.Depth_Blur_ZStart);
				case 1: return (gAE_Params.Depth_Blur_ZEnd);
				case 2: return (gAE_Params.Depth_Blur_Factor);
			}
		case 1: // Motion blur
			return (gAE_Params.MotionBlurFactor);
		case 2: // screen Smooth
			return (gAE_Params.BlurFactor);
/*
		case 3: // Emboss
			return (gAE_Params.EmbossFactor);
*/
		case 4: // Motion smooth
			return 0.0f;
		case 5: // Zoom smooth
			return (gAE_Params.ZoomSmoothFactor);
		case 6: // Rotate smooth
			return (gAE_Params.SpinSmoothFactor);
		case 7: // Remanance
			return (gAE_Params.RemananceFactor);
		case 8: // Brightness
			return (gAE_Params.Brighness);
		case 9: // Contrast
			return (gAE_Params.Contrast);
/*		case 10: // AA
			return (float)(gAE_Params.AA22BlurMode);
*/
		case 11: // BW
			return (gAE_Params.BWFactor);
		case 12: // Color balance
			switch (P1)
			{
				case 0: return (gAE_Params.CB_Intensity);
				case 1: return (gAE_Params.CB_Spectre);
				case 2: return (gAE_Params.CB_PhotoFilterEffect);
			}
			return 0.0f;
/*		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: return 0;//(GXI_Global_ACCESS(fFogCorrector));
				case 1: return 0;//(GXI_Global_ACCESS(fFogCorrectorFar));
			}
			return 0.0f;
			break;
			*/
		case 14: // Depth blur near
			return (gAE_Params.Depth_Blur_Near);
			break;
			/*
		case 16: // blur
			return (gAE_Params.BlurFactor);
			break;
*/
	}
	return 0.0f;
}

ULONG Gsp_AE_MASTER_GET_ONOFF(ULONG AENum )
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			return (gAE_Status  & GSP_Status_AE_DB);
		case 1: // Motion blur
			return (gAE_Status & GSP_Status_AE_MB);
		case 2: // screen Smooth
			return (gAE_Status & GSP_Status_AE_SM);
		case 3: // Emboss
			return (gAE_Status & GSP_Status_AE_EMB);
		case 4: // Motion smooth
			return (gAE_Status & GSP_Status_AE_SMSM);
		case 5: // Zoom smooth
			return (gAE_Status & GSP_Status_AE_ZS);
		case 6: // Rotate smooth
			return (gAE_Status & GSP_Status_AE_RS);
		case 7: // Remanance
			return (gAE_Status & GSP_Status_AE_REMANANCE);
		case 10: // AA
			return (gAE_Status & GSP_Status_AE_BlurTest44);
		case 11: // BW
			return (gAE_Status & GSP_Status_AE_BW);
		case 12: // Color balance
			return (gAE_Status & GSP_Status_AE_CB_On);
		case 13: // fOG CORRECTION
			break;
		case 14: // Depth Blur nrear
			return (gAE_Status & GSP_Status_AE_DBN);
			break;
		case 15: // Fade
			return (gAE_Status & GSP_Status_AE_FADE);
			break;
			
	}
	return 0;
}

void Gsp_AE_MASTER(ULONG AENum , ULONG OnOff , ULONG P1 , float Pf1)
{
	switch (AENum)
	{		
		case 0: // Depth Bluring
			gAE_Status &= ~ GSP_Status_AE_DB;
			if (OnOff) gAE_Status |= GSP_Status_AE_DB;
			switch (P1)
			{
				case 0: gAE_Params.Depth_Blur_ZStart = Pf1;	break;
				case 1: gAE_Params.Depth_Blur_ZEnd = Pf1;		break;
				case 2: gAE_Params.Depth_Blur_Factor = Pf1;	break;
			}
			break;
		case 1: // Motion blur
			gAE_Status &= ~ GSP_Status_AE_MB;
			if (OnOff) gAE_Status |= GSP_Status_AE_MB;
			gAE_Params.MotionBlurFactor = Pf1;
			break;
		case 2: // screen Smooth
			gAE_Status &= ~ GSP_Status_AE_SM;
			if (OnOff) gAE_Status |= GSP_Status_AE_SM;
			gAE_Params.BlurFactor = Pf1;
			break;
/*
		case 3: // Emboss
			gAE_Status &= ~ GSP_Status_AE_EMB;
			if (OnOff) gAE_Status |= GSP_Status_AE_EMB;
			gAE_Params.EmbossFactor = Pf1;
			break;
*/
		case 4: // Motion smooth
			gAE_Status &= ~ GSP_Status_AE_SMSM;
			if (OnOff) gAE_Status |= GSP_Status_AE_SMSM;
			break;
		case 5: // Zoom smooth
			gAE_Status &= ~ GSP_Status_AE_ZS;
			if (OnOff) gAE_Status |= GSP_Status_AE_ZS;
			switch (P1)
			{
				case 0: gAE_Params.ZoomSmoothFactor = Pf1;	break;
				case 1: gAE_Params.ZSDir.x = Pf1;gAE_Params.ZSDirIsValidated = 1;	break;
				case 2: gAE_Params.ZSDir.y = Pf1;gAE_Params.ZSDirIsValidated = 1;	break;
				case 3: gAE_Params.ZSDir.z = Pf1;gAE_Params.ZSDirIsValidated = 1;	break;
			}			
			//gAE_Params.ZoomSmoothFactor = Pf1;
			break;
		case 6: // Rotate smooth
			gAE_Status &= ~ GSP_Status_AE_RS;
			if (OnOff) gAE_Status |= GSP_Status_AE_RS;
			gAE_Params.SpinSmoothFactor = Pf1;
			break;
		case 7: // Remanance
			gAE_Status &= ~ GSP_Status_AE_REMANANCE;
			if (OnOff) 
				gAE_Status |= GSP_Status_AE_REMANANCE;
			gAE_Params.RemananceFactor = Pf1;
			break;
		case 8: // Brightness
			gAE_Params.Brighness = Pf1;
			break;
		case 9: // Contrast
			gAE_Params.Contrast  = Pf1;
			break;
/*		
		case 10: // AA
			gAE_Status &= ~ GSP_Status_AE_BlurTest44;
			if (OnOff) gAE_Status |= GSP_Status_AE_BlurTest44;
			gAE_Params.AA22BlurMode  = (unsigned int)Pf1;
			break;
*/
		case 11: // BW
			gAE_Status &= ~ GSP_Status_AE_BW;
			if (OnOff) 
				gAE_Status |= GSP_Status_AE_BW;
			gAE_Params.BWFactor = Pf1;
			break;
		case 12: // Color balance
			switch (P1)
			{
				case 0: gAE_Params.CB_Intensity = Pf1;	break;
				case 1: gAE_Params.CB_Spectre = Pf1;		break;
				case 2: gAE_Params.CB_PhotoFilterEffect = Pf1;	break;
			}
			gAE_Status &= ~ GSP_Status_AE_CB_On;
			if (OnOff) gAE_Status |= GSP_Status_AE_CB_On;
			break;
/*		
		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: GspGlobal_ACCESS(fFogCorrector)		 = Pf1;	break;
				case 1: GspGlobal_ACCESS(fFogCorrectorFar) 	 = Pf1;	break;
			}
			break;
*/
		case 14: // Depth Blur nrear
			gAE_Status &= ~ GSP_Status_AE_DBN;
			if (OnOff) gAE_Status |= GSP_Status_AE_DBN;
			gAE_Params.Depth_Blur_Near = Pf1;
			break;
		case 15: // Fade
			gAE_Status &= ~ GSP_Status_AE_FADE;
			if (OnOff)
			{
				gAE_Status |= GSP_Status_AE_FADE;
				if (p_gGx8SpecificData->iFadeAlfa==0)
					p_gGx8SpecificData->iFadeAlfa=255;
			}
			gAE_Params.fadeTime=Pf1;
			break;
	
/*
		case 16: // Water Ref
			switch (P1)
			{
				case 0:gAE_Params.fWR_ZHeight= Pf1;	break;;
				case 1:gAE_Params.fWR_ZPlane = Pf1;	break;
				case 2:gAE_Params.fWR_SinusEffect= Pf1;	break;
				case 3:gAE_Params.fWR_SinusEffectBalance= Pf1;	break;
				case 4:gAE_Params.fWR_TransparencyMax= Pf1;	break;
				case 5:gAE_Params.ulWR_ZOrder = (u_int)Pf1; break;
				case 6:gAE_Params.fWR_ZDepth = (float)Pf1; break;
				
			}
			if (!OnOff) gAE_Params.fWR_ZHeight = 0.0f;
			break;
*/
		case 17: // Warp
			gAE_Params.WarpFactor = Pf1;
			if (!OnOff) gAE_Params.WarpFactor = 0.0f;
			break;
		case 18: // ZoomSmoothRemanance 
			gAE_Status &= ~ GSP_Status_AE_ZSR;
			if (!OnOff)	gAE_Params.ZoomSmoothRemananceFactor = 0.0f;
			if (OnOff) gAE_Status |= GSP_Status_AE_ZSR;
			//gAE_Params.ZoomSmoothRemananceFactor = 5;//Pf1;
			switch (P1)
			{
				case 0: gAE_Params.ZoomSmoothRemananceFactor = Pf1;	break;
				case 1: gAE_Params.ZoomSmoothRemananceLightDirection.x = Pf1;		break;
				case 2: gAE_Params.ZoomSmoothRemananceLightDirection.y = Pf1;		break;
				case 3: gAE_Params.ZoomSmoothRemananceLightDirection.z = Pf1;		break;
			}
			break;
		case 19: // Sharpen
			gAE_Params.SharpenFactor = Pf1;
			break;

		case 20: // Border brightness
			gAE_Status &= ~ GSP_Status_AE_Border;
			if (OnOff) gAE_Status |= GSP_Status_AE_Border;
			gAE_Params.BorderBrightNess = Pf1;
			break;

		case 21: // FoggyBlur
			gAE_Params.FoggyBlur = Pf1;
			break;

		case 22: // Glow
			gAE_Params.Glow = Pf1;
			break;
		
		case 23: // BorderColor 
			gAE_Params.BorderColor = P1 & 0xffffff;
			break;

		case 24: // PerfectGlow 
			switch (P1)
			{
				case 0: gAE_Params.PerfectGow_Factor = Pf1;break;
				case 1: gAE_Params.PerfectGow_Thresh = Pf1;break;
				case 2: gAE_Params.PerfectGow_Radius = Pf1;break;
			}
			break;
	}
}

void AE_Splash_AddOne(MATH_tdst_Vector *p2DPosPlusSize , u32 Color )

{
//popo
}




void AE_DrawTheRenderedScreenWithColorAttenuation(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture , int attenuation)
{
	IDirect3DSurface8	*ppSurfaceLevel;
	ULONG Color;

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
	
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	Color = 0xff000000|(u_int)(attenuation);
	Color |= Color << 8;
	Color |= Color << 16;

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(480);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = (640-0.5f);
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(480);

		MonStream[3].x = (640-0.5f);
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}


	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}

/*
void AE_CopyRenderTextureToBackBuffer(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 *sourceTexture)
{
	// render the rendered target

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1  );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );
	
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;
		int i = 0;

		UINT uiVStride;
		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

		MonStream[i].x = 0;												// 0.5f offset to fit D3D pixel center
		MonStream[i].y = 0;												// 0.5f offset to fit D3D pixel center
		MonStream[i].z = 0;
		MonStream[i].rhw = 1;
		MonStream[i].Color = 0xffffffff;
		MonStream[i].u = 0.0f;
		MonStream[i].v = 0.0f;
		i++;
		MonStream[i].x = (640-0.5f);				// 0.5f offset to fit D3D pixel center
//		MonStream[i].x = (640-0.5f);				// 0.5f offset to fit D3D pixel center
		MonStream[i].y = 0;												// 0.5f offset to fit D3D pixel center
		MonStream[i].z = 0;
		MonStream[i].rhw = 1;
		MonStream[i].Color = 0xffffffff;
		MonStream[i].u = (float)640;
		MonStream[i].v = 0.0f;
		i++;
//		MonStream[i].x = (640-0.5f);				// 0.5f offset to fit D3D pixel center
//		MonStream[i].y = (pst_SD->AfterFXTexture_Height-0.5f);				// 0.5f offset to fit D3D pixel center
		MonStream[i].x = (640-0.5f);				// 0.5f offset to fit D3D pixel center
		MonStream[i].y = (480-0.5f);				// 0.5f offset to fit D3D pixel center
		MonStream[i].z = 0;
		MonStream[i].rhw = 1;
		MonStream[i].Color = 0xffffffff;
		MonStream[i].u = (float)640;
		MonStream[i].v = (float)pst_SD->AfterFXTexture_Height;
		i++;
		MonStream[i].x = 0;												// 0.5f offset to fit D3D pixel center
//		MonStream[i].y = (pst_SD->AfterFXTexture_Height-0.5f);				// 0.5f offset to fit D3D pixel center
		MonStream[i].y = (480-0.5f);				// 0.5f offset to fit D3D pixel center
		MonStream[i].z = 0;
		MonStream[i].rhw = 1;
		MonStream[i].Color = 0xffffffff;
		MonStream[i].u = 0.0f;
		MonStream[i].v = (float)pst_SD->AfterFXTexture_Height;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,1,(void*)MonStream,uiVStride);
	}
}
*/
void AE_DrawTheRenderedScreen_FlipH(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
	IDirect3DSurface8	*ppSurfaceLevel;
	ULONG Color;

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);



	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 640;//0
		MonStream[0].v = LOWBAND;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffff00ff;
		MonStream[1].u = 640;//0
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = 0;//640
		MonStream[2].v = LOWBAND;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = 0;//640
		MonStream[3].v = HIGHBAND;
		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);

	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}

void AE_DrawTheRenderedScreen(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture )
{
	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			//AfterFXTexture_Width
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = LOWBAND;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffff00ff;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = 640;
		MonStream[2].v = LOWBAND;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = 640;
		MonStream[3].v = HIGHBAND;
		//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 1/*TRUE*/);
		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
		//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0/*TRUE*/);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}
void AE_DrawTheRenderedScreenTemp(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture )
{
	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	//Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		//pst_SD->AfterFXTexture_Width
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = LOWBAND;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = 1280;
		MonStream[2].v = LOWBAND;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = 1280;
		MonStream[3].v = HIGHBAND;
		//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 1/*TRUE*/);
		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
		//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0/*TRUE*/);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}
void AE_DrawScreen(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture ,D3DCOLOR Color)
{
	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,  D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
 
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = LOWBAND;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = 640;//1280
		MonStream[2].v = LOWBAND;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = 640;//1280
		MonStream[3].v = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}

IDirect3DTexture8 * CreateTileTexture(int w,int h,int TileNumber)
{
	IDirect3DTexture8 * retTExture;
	VOID			  * pColorMemory; 
	LPDIRECT3DSURFACE8  pTempSurface;
	D3DSURFACE_DESC desc;
	D3DTILE tile;

	retTExture=0x00000000;

	retTExture =(IDirect3DTexture8 *)malloc(sizeof(D3DTexture));
    XGSetTextureHeader( w, h, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, 
                        retTExture, 0, 0 );
    pColorMemory = D3D_AllocContiguousMemory(4*w*h,D3DTILE_ALIGNMENT );
	IDirect3DTexture8_Register(retTExture,pColorMemory);


	//TILE THIS TEXTURE
	IDirect3DTexture8_GetSurfaceLevel(retTExture,0,&pTempSurface );

	//Get surface descriptor
    IDirect3DSurface8_GetDesc(pTempSurface,&desc );
	

	//Release the surface
	IDirect3DSurface8_Release(pTempSurface);
	pTempSurface=0;


	//TO DO
	ZeroMemory( &tile, sizeof(tile) );
	tile.Flags     = 0;
    tile.Pitch     = desc.Size / desc.Height;
    tile.pMemory   = pColorMemory;
    tile.Size      = desc.Size;
    
	//TILE THE MEMORY
	IDirect3DDevice8_SetTile( p_gGx8SpecificData->mp_D3DDevice,TileNumber, &tile );
	

	return retTExture;
    
}

void AE_InitAETexturesPool(Gx8_tdst_SpecificData *pst_SD)
{
	int i;

	pst_SD->iAEColorET_Count=AEColorET_Max;
	for(i=0;i<AEColorET_Max;i++)
	{
		pst_SD->bAEColorET_Available[i]=true;
		pst_SD->pAEColorET[i]=CreateTileTexture(640,480,i + 2);
	}
}

void AE_DrawTheRenderedScreenWithZoomSmooth(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{
const int ZOOMSMOOTH_NOFRENDER=2;

	IDirect3DSurface8	*ppSurfaceLevel;
	
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

	int i;
    float resizeStep,resizeStep2;

	resizeStep=gAE_Params.ZoomSmoothFactor*6;
	resizeStep2=gAE_Params.ZoomSmoothFactor*1;

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,3,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,3,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,255,255,255));

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );


    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);
	
	Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
	uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );

	MonStream[0].x = 0;
	MonStream[0].y = LOWBAND-0.5f;
	MonStream[0].z = 0;
	MonStream[0].rhw = 1;
	MonStream[0].Color = 0xffffffff;
	MonStream[1].x = 0;
	MonStream[1].y = HIGHBAND;
	MonStream[1].z = 0;
	MonStream[1].rhw = 1;
	MonStream[1].Color = 0xffffffff;
	MonStream[2].x = 640-0.5f;
	MonStream[2].y = LOWBAND;
	MonStream[2].z = 0;
	MonStream[2].rhw = 1;
	MonStream[2].Color = 0xffffffff;
	MonStream[3].x = 640-0.5f;
	MonStream[3].y = HIGHBAND;
	MonStream[3].z = 0;
	MonStream[3].rhw = 1;
	MonStream[3].Color = 0xffffffff;
	
	for (i=0;i<4;i++)
	{
		MonStream[0].u2 = 0+resizeStep;
		MonStream[0].v2 = LOWBAND-resizeStep;
		MonStream[0].u = 0-resizeStep;
		MonStream[0].v = LOWBAND+resizeStep;

		MonStream[1].u2 = 0+resizeStep;
		MonStream[1].v2 = HIGHBAND+resizeStep;
		MonStream[1].u = 0-resizeStep;
		MonStream[1].v = HIGHBAND-resizeStep;

		MonStream[2].u2 = 640-resizeStep;
		MonStream[2].v2 = LOWBAND-resizeStep;
		MonStream[2].u = 640+resizeStep;
		MonStream[2].v = LOWBAND+resizeStep;

		MonStream[3].u2 = 640-resizeStep;
		MonStream[3].v2 = HIGHBAND+resizeStep;
		MonStream[3].u = 640+resizeStep;
		MonStream[3].v = HIGHBAND-resizeStep;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);

		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)destTexture);
		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)destTexture);

		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, TRUE );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );

		resizeStep*=0.65f;

	}
	
	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
	
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,NULL);

}

void AE_DrawTheRenderedScreenWithMotionSmooth(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture,float step,float step2)
{
//const unsigned int MOTIONSMOOTH_STEP=2;

	// render the rendered target
	float motionSmoothStep,motionSmoothStep2;
	IDirect3DSurface8	*ppSurfaceLevel;

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
  
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_MINFILTER, D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_MINFILTER, D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_MINFILTER, D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_MAGFILTER, D3DTEXF_POINT);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1  );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_ADD);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_ADD);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG2, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP,   D3DTOP_ADD);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)sourceTexture );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,3,(IDirect3DBaseTexture8 *)sourceTexture );
		
	motionSmoothStep=(float)step;
	motionSmoothStep2=(float)step2;

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
			float u3,v3;
			float u4,v4;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = -motionSmoothStep*3;
		MonStream[0].v = (float)(LOWBAND)-motionSmoothStep2*3;
		MonStream[0].u2 = motionSmoothStep*2;
		MonStream[0].v2 = (float)(LOWBAND)+motionSmoothStep2*2;
		MonStream[0].u3 = -motionSmoothStep;
		MonStream[0].v3 = (float)(LOWBAND)-motionSmoothStep2;
		MonStream[0].u4 = 0;
		MonStream[0].v4 = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = -motionSmoothStep*3.0f;
		MonStream[1].v = HIGHBAND-motionSmoothStep2*3.0f;
		MonStream[1].u2 = motionSmoothStep*2.0f;
		MonStream[1].v2 = HIGHBAND+motionSmoothStep2*2.0f;
		MonStream[1].u3 = -motionSmoothStep;
		MonStream[1].v3 = HIGHBAND-motionSmoothStep2;
		MonStream[1].u4 = 0;
		MonStream[1].v4 = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = (640)-motionSmoothStep*3;
		MonStream[2].v = (float)(LOWBAND)-motionSmoothStep2*3;
		MonStream[2].u2 = (640)+motionSmoothStep*2;
		MonStream[2].v2 = (float)(LOWBAND)+motionSmoothStep2*2;
		MonStream[2].u3 = (640)-motionSmoothStep;
		MonStream[2].v3 = (float)(LOWBAND)-motionSmoothStep2;
		MonStream[2].u4 = (float)(640);
		MonStream[2].v4 = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = (640)-motionSmoothStep*3;
		MonStream[3].v = HIGHBAND-motionSmoothStep2*3;
		MonStream[3].u2 = (640)+motionSmoothStep*2;
		MonStream[3].v2 = HIGHBAND+motionSmoothStep2*2;
		MonStream[3].u3 = (640)-motionSmoothStep;
		MonStream[3].v3 = HIGHBAND-motionSmoothStep2;
		MonStream[3].u4 = (float)(640);
		MonStream[3].v4 = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);

	}
	
	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 1, NULL);
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 2, NULL);
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 3, NULL);

}


void AE_DrawTheRenderedScreenWithRotationSmooth(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{
const int ZOOMSMOOTH_NOFRENDER=1;

	float rotationStep;
	IDirect3DSurface8	*ppSurfaceLevel;
	
	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,3,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,3,D3DTSS_MAGFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,255,255,255) );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,NULL);

	rotationStep=gAE_Params.SpinSmoothFactor/12;//25

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;
		int i,n;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );

		//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, TRUE );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );

		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );

		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;
		MonStream[1].u2 = 0;
		MonStream[1].v2 = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);
		MonStream[2].u2 = (float)(640);
		MonStream[2].v2 = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;
		MonStream[3].u2 = (float)(640);
		MonStream[3].v2 = HIGHBAND;

	for (n=0;n<4;n++)
	{
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(LOWBAND);
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;
		MonStream[1].u2 = 0;
		MonStream[1].v2 = HIGHBAND;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);
		MonStream[2].u2 = (float)(640);
		MonStream[2].v2 = (float)(LOWBAND);
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;
		MonStream[3].u2 = (float)(640);
		MonStream[3].v2 = HIGHBAND;

		for(i=0;i<4;i++)
		{

			
			MonStream[i].u-=(640)/2;
			MonStream[i].v-=(480)/2;
			MonStream[i].u=(float)(MonStream[i].u*fCos(rotationStep)+MonStream[i].v*fSin(rotationStep));
			MonStream[i].v=(float)(MonStream[i].u*-fSin(rotationStep)+MonStream[i].v*fCos(rotationStep));
			MonStream[i].u+=(640)/2;
			MonStream[i].v+=(480)/2;

			MonStream[i].u2-=(640)/2;
			MonStream[i].v2-=(480)/2;
			MonStream[i].u2=(float)(MonStream[i].u2*cos(-(rotationStep))+MonStream[i].v2*sin(-(rotationStep)));
			MonStream[i].v2=(float)(MonStream[i].u2*-sin(-(rotationStep))+MonStream[i].v2*cos(-(rotationStep)));
			MonStream[i].u2+=(640)/2;
			MonStream[i].v2+=(480)/2;

		}

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
		rotationStep*=0.5f;

		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)destTexture);
		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)destTexture);
}
		
	}
	
	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,NULL);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, FALSE );
}

void fnvDrawTheRenderedScreenBlended(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture)
{

	// render the rendered target

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1  );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, 1);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_BLENDOP,D3DBLENDOP_ADD);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;
		int alfaFactor;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		alfaFactor=(int)(255-gAE_Params.MotionBlurFactor*255) ;

		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(alfaFactor,0xff,0xff,0xff);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(alfaFactor,0xff,0xff,0xff);
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(alfaFactor,0xff,0xff,0xff);
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(alfaFactor,0xff,0xff,0xff);
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	}
}

void fnvDrawBlendedInMBBuffer(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 *sourceTexture)
{
	IDirect3DSurface8	*ppSurfaceLevel;

	IDirect3DTexture8_GetSurfaceLevel
		(
			pst_SD->pMotionBlurFrameBufferTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	fnvDrawTheRenderedScreenBlended(pst_SD,sourceTexture);

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

}

void AE_DrawTheRenderedScreenWithMotionBlur(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture)
{
	
	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;

	IDirect3DTexture8_GetSurfaceLevel
		(
			sourceTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)pst_SD->pMotionBlurFrameBufferTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}

void AE_DrawTheRenderedScreenBW(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{
	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	int alphaFactor;

	alphaFactor=(int)(255-(gAE_Params.BWFactor*255));

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_POINT);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_DOTPRODUCT3);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_SPECULAR );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORSIGN, D3DTSIGN_RSIGNED | D3DTSIGN_GSIGNED | D3DTSIGN_BSIGNED  );
	//IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORSIGN, 0);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORSIGN, 0);

	

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,128,128,128));

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			DWORD Specular;
			float u,v;
			float u2,v2;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;
		D3DCOLOR Color,Specular;

		Color = D3DCOLOR_ARGB(0,0xff,0xff,0xff);
		Specular = D3DCOLOR_ARGB(0,0xff,0x80,0x80);


		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].Specular = Specular;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(480);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(480);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].Specular = Specular;
		MonStream[1].u = 0;
		MonStream[1].v = 0;
		MonStream[1].u2 = 0;
		MonStream[1].v2 = 0;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].Specular = Specular;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(480);
		MonStream[2].u2 = (float)(640);
		MonStream[2].v2 = (float)(480);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].Specular = Specular;
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;
		MonStream[3].u2 = (float)(640);
		MonStream[3].v2 = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORSIGN, 0);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,NULL);

}
void AE_DrawTheRenderedScreenBW_2Pass(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{
	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	int alphaFactor;

	alphaFactor=(int)(255-(gAE_Params.BWFactor*255));

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,128,128,128));

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;
		D3DCOLOR Color;

		Color = D3DCOLOR_ARGB(alphaFactor,0xff,0xff,0xff);


		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(480);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(480);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = 0.5;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,NULL);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

}

void AE_DrawTheRenderedScreenInSmoothTexture(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture,float offset,int s, bool remanance)
{
#define HIGHBANDSMOOTH	0
#define LOWBANDSMOOTH	480

	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	int deltau,deltav;
	
	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

//	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,255,255,255));
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,255,255,255));

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	/*if (remanance)
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	else
	*/
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);


	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA    );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,(IDirect3DBaseTexture8 *)sourceTexture);

	//IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,NULL);
	//IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );

	deltau=deltav=lFloatToLong(offset);
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);

		MonStream[0].x = 0;
		MonStream[0].y = LOWBANDSMOOTH-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[0].u = (float)(-deltau);
		MonStream[0].v = (float)LOWBANDSMOOTH;
		MonStream[0].u2 = (float)deltau;
		MonStream[0].v2 = (float)LOWBANDSMOOTH;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBANDSMOOTH;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[1].u = (float)(-deltau);
		MonStream[1].v = HIGHBANDSMOOTH;
		MonStream[1].u2 = (float)deltau;
		MonStream[1].v2 = HIGHBANDSMOOTH;

		MonStream[2].x = 640-0.5f;//(640-0.5f);
		MonStream[2].y = LOWBANDSMOOTH-0.5f;//(LOWBANDSMOOTH-0.5f);
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[2].u = (float)640-deltau;
		MonStream[2].v = (float)LOWBANDSMOOTH;
		MonStream[2].u2 = (float)640+deltau;
		MonStream[2].v2 = (float)LOWBANDSMOOTH;

		MonStream[3].x = 640-0.5f;//(640-0.5f);
		MonStream[3].y = HIGHBANDSMOOTH;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[3].u = (float)640-deltau;
		MonStream[3].v = HIGHBANDSMOOTH;
		MonStream[3].u2 = (float)640+deltau;
		MonStream[3].v2 = HIGHBANDSMOOTH;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, TRUE );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );

		MonStream[0].x = 0;
		MonStream[0].y = LOWBANDSMOOTH-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBANDSMOOTH-deltav);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(LOWBANDSMOOTH+deltav);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBANDSMOOTH;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[1].u = 0;
		MonStream[1].v = (float)(HIGHBANDSMOOTH-deltav);
		MonStream[1].u2 = 0;
		MonStream[1].v2 = (float)(HIGHBANDSMOOTH+deltav);

		MonStream[2].x = 640-0.5f;//(640-0.5f);
		MonStream[2].y = LOWBANDSMOOTH-0.5f;//(LOWBANDSMOOTH-0.5f);
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[2].u = (float)640;
		MonStream[2].v = (float)LOWBANDSMOOTH-deltav;
		MonStream[2].u2 = (float)640;
		MonStream[2].v2 = (float)LOWBANDSMOOTH+deltav;

		MonStream[3].x = 640-0.5f;//(640-0.5f);
		MonStream[3].y = HIGHBANDSMOOTH;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[3].u = (float)640;
		MonStream[3].v = (float)(HIGHBANDSMOOTH-deltav);
		MonStream[3].u2 = (float)640;
		MonStream[3].v2 = (float)(HIGHBANDSMOOTH+deltav);

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
	
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, FALSE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 2, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 3, NULL);
}
void AE_BigBlur(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture,float offset,int s, bool remanance)
{
#define HIGHBANDSMOOTH	0
#define LOWBANDSMOOTH	480
	
UINT uiVStride;

	struct MonStream_
	{
		float x,y,z,rhw;
		DWORD Color;
		float u,v;
		float u2,v2;
	} MonStream[4];

	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	float deltau,deltav;
//	int i;
	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,255,255,255));

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);


	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA    );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );

		MonStream[0].x = 0;
		MonStream[0].y = LOWBANDSMOOTH-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBANDSMOOTH;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBANDSMOOTH-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBANDSMOOTH;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;

	Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
	uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);


while (offset>1)
//for (i=0;i<8;i++)
{
//	deltau=deltav=lFloatToLong(offset);
	deltau=deltav=(offset);

		MonStream[0].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[0].u = (-deltau);
		MonStream[0].v = LOWBANDSMOOTH;
		MonStream[0].u2 = deltau;
		MonStream[0].v2 = LOWBANDSMOOTH;

		MonStream[1].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[1].u = (-deltau);
		MonStream[1].v = HIGHBANDSMOOTH;
		MonStream[1].u2 = deltau;
		MonStream[1].v2 = HIGHBANDSMOOTH;

		MonStream[2].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[2].u = 640-deltau;
		MonStream[2].v = LOWBANDSMOOTH;
		MonStream[2].u2 = 640+deltau;
		MonStream[2].v2 = LOWBANDSMOOTH;

		MonStream[3].Color = D3DCOLOR_ARGB(128,255,255,255);
		MonStream[3].u = 640-deltau;
		MonStream[3].v = HIGHBANDSMOOTH;
		MonStream[3].u2 = 640+deltau;
		MonStream[3].v2 = HIGHBANDSMOOTH;

		IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
		
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, TRUE );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
        IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );

		MonStream[0].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[0].u = 0;
		MonStream[0].v = (LOWBANDSMOOTH-deltav);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (LOWBANDSMOOTH+deltav);

		MonStream[1].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[1].u = 0;
		MonStream[1].v = (HIGHBANDSMOOTH-deltav);
		MonStream[1].u2 = 0;
		MonStream[1].v2 = (HIGHBANDSMOOTH+deltav);

		MonStream[2].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[2].u = 640;
		MonStream[2].v = LOWBANDSMOOTH-deltav;
		MonStream[2].u2 = 640;
		MonStream[2].v2 = LOWBANDSMOOTH+deltav;

		MonStream[3].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[3].u = 640;
		MonStream[3].v = (HIGHBANDSMOOTH-deltav);
		MonStream[3].u2 = 640;
		MonStream[3].v2 = (HIGHBANDSMOOTH+deltav);
	

		IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)destTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)destTexture);

	offset*=0.5f;
	
}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
	
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, FALSE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 2, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 3, NULL);
}
void AE_DrawTheRenderedScreenRemanance(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{

#define XDISP 0//5
#define YDISP -0//-5

	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	int alphaFactor;

	alphaFactor=(int)(gAE_Params.RemananceFactor*255);
 
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,destTexture,8,0,true);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,4,0,true);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,pst_SD->pSmoothBufferTexture,destTexture,2,0,true);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,1,0,true);

	//AE_BigBlur(pst_SD,sourceTexture,pst_SD->pSmoothBufferTexture,16,0,false);
	//AE_SwapTextures(&pst_SD->pSmoothBufferTexture,&destTexture);

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	//CARLONE..TO BE TESTED
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );


	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

/*	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG1, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	//CARLONE..TO BE TESTED
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );*/

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)pst_SD->pSmoothBufferTexture);
	//IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)pst_SD->pSmoothBufferTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
			//float u3,v3;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(alphaFactor,128,128,128);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);
		MonStream[0].u2 = XDISP;
		MonStream[0].v2 = (float)(LOWBAND)+YDISP;
		/*MonStream[0].u3 = XDISP;
		MonStream[0].v3 = (float)(LOWBAND)+YDISP;*/

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(alphaFactor,128,128,128);
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;
		MonStream[1].u2 = XDISP;
		MonStream[1].v2 = HIGHBAND+YDISP;
		/*MonStream[1].u3 = XDISP;
		MonStream[1].v3 = HIGHBAND+YDISP;*/

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(alphaFactor,128,128,128);
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);
		MonStream[2].u2 = (float)(640)+XDISP;
		MonStream[2].v2 = (float)(LOWBAND)+YDISP;
		/*MonStream[2].u3 = (float)(640)+XDISP;
		MonStream[2].v3 = (float)(LOWBAND)+YDISP;*/

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(alphaFactor,128,128,128);
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;
		MonStream[3].u2 = (float)(640)+XDISP;
		MonStream[3].v2 = HIGHBAND+YDISP;
		/*MonStream[3].u3 = (float)(640)+XDISP;
		MonStream[3].v3 = HIGHBAND+YDISP;*/

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 2, NULL);
}




void AE_DepthNearBlur_InitZBuffer(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{
	IDirect3DSurface8 *pBuffer;
    D3DSURFACE_DESC desc;
	DWORD textureSize;

	return;
	IDirect3DTexture8_GetSurfaceLevel
	(
		sourceTexture,
		0,
		&pBuffer
	);

    IDirect3DSurface8_GetDesc( pBuffer, &desc );
    textureSize=XGSetTextureHeader( desc.Width, desc.Height, 1, 0, D3DFMT_LIN_D24S8, 0, destTexture, 
                        pBuffer->Data, desc.Width * XGBytesPerPixelFromFormat(D3DFMT_LIN_D24S8) );
	IDirect3DSurface8_Release(pBuffer);
}

/*
void AE_DepthNearBlur_CopyZBufferIntoAlpha(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{
	IDirect3DSurface8 *pZBuffer;
    D3DSURFACE_DESC desc;
	DWORD textureSize;

	IDirect3DTexture8_GetSurfaceLevel
	(
		sourceTexture,
		0,
		&pZBuffer
	);

    IDirect3DSurface8_GetDesc( pZBuffer, &desc );
    textureSize=XGSetTextureHeader( desc.Width, desc.Height, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, destTexture, 
                        pZBuffer->Data, desc.Width * XGBytesPerPixelFromFormat(D3DFMT_LIN_A8R8G8B8) );
	IDirect3DSurface8_Release(pZBuffer);
}
*/
void AE_LinearizeAlphaTextureForDepthNearBlur(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 *sourceTexture)
{
	IDirect3DSurface8	*ppSurfaceLevel;

return;
	IDirect3DTexture8_GetSurfaceLevel
		(
			sourceTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE  );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAARG2, D3DTA_CURRENT  );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG1, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ALPHAARG2, D3DTA_CURRENT  );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLORARG1, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ALPHAARG2, D3DTA_CURRENT  );


	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE );
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );

	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture );

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
			float u3,v3;
			float u4,v4;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(480);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(480);
		MonStream[0].u3 = 0;
		MonStream[0].v3 = (float)(480);
		MonStream[0].u4 = 0;
		MonStream[0].v4 = (float)(480);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = 0;
		MonStream[1].v = 0;
		MonStream[1].u2 = 0;
		MonStream[1].v2= 0;
		MonStream[1].u3 = 0;
		MonStream[1].v3 = 0;
		MonStream[1].u4 = 0;
		MonStream[1].v4 = 0;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(480);
		MonStream[2].u2 = (float)(640);
		MonStream[2].v2 = (float)(480);
		MonStream[2].u3 = (float)(640);
		MonStream[2].v3 = (float)(480);
		MonStream[2].u4 = (float)(640);
		MonStream[2].v4 = (float)(480);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;
		MonStream[3].u2 = (float)(640);
		MonStream[3].v2 = 0;
		MonStream[3].u3 = (float)(640);
		MonStream[3].v3 = 0;
		MonStream[3].u4 = (float)(640);
		MonStream[3].v4 = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

}

void AE_DrawTheRenderedScreenWithDepthBlur(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
#define YOFFS	-3

	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
return;
	AE_LinearizeAlphaTextureForDepthNearBlur(pst_SD,pst_SD->pAlphaForDepthBlurTexture);

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG1, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE );
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(nearAlphaAdjustment,0,0,0) );

	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)pst_SD->pSmoothBufferTexture );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)pst_SD->pAlphaForDepthBlurTexture );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)sourceTexture );

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
			float u3,v3;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND+YOFFS);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(LOWBAND);
		MonStream[0].u3 = 0;
		MonStream[0].v3 = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND+YOFFS;
		MonStream[1].u2 = 0;
		MonStream[1].v2= HIGHBAND;
		MonStream[1].u3 = 0;
		MonStream[1].v3 = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND+YOFFS);
		MonStream[2].u2 = (float)(640);
		MonStream[2].v2 = (float)(LOWBAND);
		MonStream[2].u3 = (float)(640);
		MonStream[2].v3 = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND+YOFFS;
		MonStream[3].u2 = (float)(640);
		MonStream[3].v2 = HIGHBAND;
		MonStream[3].u3 = (float)(640);
		MonStream[3].v3 = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 2, NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 3, NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
}

void AE_DrawTheRenderedScreenWithPerfectGlow(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture,float Factor,float Thresh,float radius)
{
	int invert;
	float FactorSave;//,W,H ;
	int Color;
	FactorSave = Factor;
	invert = 0;
	if (Factor == 0.0f) return;
	//pipi OGL_AE_SetViewport(&W , &H);
	//pipi OGL_AE_ValidateTexture(&FBTexture[6], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	/* 1 Save */
	//pipi OGL_AE_CopyTexture(&FBTexture[6] );
	//pipi glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	/* 2 BW */


	//OGL_BlackWhite(1.0f);
	AE_DrawTheRenderedScreenBW(pst_SD,sourceTexture,destTexture);
	AE_DrawTheRenderedScreenBW_2Pass(pst_SD,sourceTexture,destTexture);
	AE_SwapTextures(&sourceTexture,&destTexture);


	/* 3 Sub */
    /* 3a Invert dest */
	//glDisable(GL_TEXTURE_2D);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR , GL_ZERO );
	Color = 0xffffff;
	//OGL_DrawQUAD(W,H,Color);
    /* 3b Add dest */
	//glBlendFunc(GL_ONE , GL_ONE );
	Color = (u32)(Thresh * 255.0f);
	Color |= (Color << 8) | (Color << 16);
	Color ^= 0xffffff ;
	//OGL_DrawQUAD(W,H,Color);
    /* 3c Invert dest */
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR , GL_ZERO );
	Color = 0xffffff;
	//OGL_DrawQUAD(W,H,Color);

		

	/* 4 Blur */
	//OGL_BigBlur(radius * 0.02f ,0);
	AE_BigBlur(pst_SD,sourceTexture,destTexture,radius * 0.02f,0,false);
	AE_SwapTextures(&sourceTexture,&destTexture);


	/* 5 Add saved 2x */

	//OGL_AE_ValidateTexture(&FBTexture[7], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	//OGL_AE_CopyTexture(&FBTexture[7] );

	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, FBTexture[6]);
	//glBlendFunc(GL_ONE,GL_SRC_ALPHA );

	Color = (u32)(Factor * 255.f) << 24;
	Color |= 0xffffff;
	//OGL_DrawQUAD(W,H,Color);

	//glBindTexture(GL_TEXTURE_2D, FBTexture[7]);
	//glBlendFunc(GL_SRC_ALPHA , GL_ONE);
	//OGL_DrawQUAD(W,H,Color);
	//OGL_DrawQUAD(W,H,Color);
	//OGL_DrawQUAD(W,H,Color);

}

void AE_DrawTheRenderedScreenWithNearBlur(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	return;
	AE_LinearizeAlphaTextureForDepthNearBlur(pst_SD,pst_SD->pAlphaForNearBlurTexture);


	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_POINT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG1, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE );
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(nearAlphaAdjustment,0,0,0) );

	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)pst_SD->pAlphaForNearBlurTexture );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)pst_SD->pSmoothBufferTexture );
//	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)pst_SD->pSmoothBufferTexture );

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
			float u3,v3;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(LOWBAND);
		MonStream[0].u3 = 0;
		MonStream[0].v3 = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;
		MonStream[1].u2 = 0;
		MonStream[1].v2= HIGHBAND;
		MonStream[1].u3 = 0;
		MonStream[1].v3 = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);
		MonStream[2].u2 = (float)(640);
		MonStream[2].v2 = (float)(LOWBAND);
		MonStream[2].u3 = (float)(640);
		MonStream[2].v3 = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;
		MonStream[3].u2 = (float)(640);
		MonStream[3].v2 = HIGHBAND;
		MonStream[3].u3 = (float)(640);
		MonStream[3].v3 = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 2, NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 3, NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
}
void AE_QUADZ(Gx8_tdst_SpecificData *pst_SD,DWORD color,float z)
{
	static float DEBUGZ = 0.9999999f;
	z = DEBUGZ;
//DEBUGZ+=0.001f;

	//Gx8_RS_CullFace(pst_SD, D3DCULL_NONE);
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_GREATEREQUAL);//D3DCMP_GREATEREQUAL

	//IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	//IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    //IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_ONE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;

		} MonStream[4];
		HRESULT hr;
		float f=1000;
		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW| D3DFVF_DIFFUSE );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND;
		MonStream[0].z = z;
		MonStream[0].rhw = 0;
		MonStream[0].Color = color;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = z;
		MonStream[1].rhw = 0;
		MonStream[1].Color = color;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND;
		MonStream[2].z = z;
		MonStream[2].rhw = 0;
		MonStream[2].Color = color;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = z;
		MonStream[3].rhw = 0;
		MonStream[3].Color = color;
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 1/*TRUE*/);
		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0/*TRUE*/);

		Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);
	}
}
void AE_QUAD169(Gx8_tdst_SpecificData *pst_SD)
{

	//pipiGx8_RS_CullFace(pst_SD, D3DCULL_NONE);
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	//IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	//IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    //IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_ZERO);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
/*	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZENABLE, FALSE );*/
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_ALWAYS );

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;

		} MonStream[4];
		HRESULT hr;
		float f=1000;
		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW| D3DFVF_DIFFUSE );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

		MonStream[0].x = 0;
		MonStream[0].y = 70;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xff000000;

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xff000000;

		MonStream[2].x = 640;
		MonStream[2].y = 70;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xff000000;

		MonStream[3].x = 640;
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw =1;
		MonStream[3].Color = 0xff000000;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);

		MonStream[0].x = 0;
		MonStream[0].y = 480;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xff000000;

		MonStream[1].x = 0;
		MonStream[1].y = 410;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xff000000;

		MonStream[2].x = 640;
		MonStream[2].y = 480;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xff000000;

		MonStream[3].x = 640;
		MonStream[3].y = 410;
		MonStream[3].z = 0;
		MonStream[3].rhw =1;
		MonStream[3].Color = 0xff000000;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);

	}
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 1/*TRUE*/);
}



void AE_TITLESAFEAREA(Gx8_tdst_SpecificData *pst_SD)
{
struct TITLESAFE_BOX_VERTEX_
{
    D3DXVECTOR3 v;
    float       fRHW;
    D3DCOLOR    cDiffuse;
}box[4]; 
const FLOAT fTitleSafePercentage = 0.85f;//0.85f;
int i;

	float fScreenWidth = (float) 640;
    float fScreenHeight = (float) 480;

    float fBoxWidth = ( fScreenWidth * fTitleSafePercentage );
    float fBoxHeight = ( fScreenHeight * fTitleSafePercentage );

    float x1 = ( fScreenWidth - fBoxWidth ) / 2.0f;
    float x2 = fScreenWidth - x1;
    float y1 = ( fScreenHeight - fBoxHeight ) / 2.0f;
    float y2 = fScreenHeight - y1;

    //TITLESAFE_BOX_VERTEX box[4];

    for (i = 0; i < 4; i++)
    {
        box[i].v.z = 0.000001f;
        box[i].fRHW = 1.0f/box[i].v.z;
        box[i].cDiffuse = 0xffffffff;
    }

    box[0].v.x = x1; box[0].v.y = y2;
    box[1].v.x = x1; box[1].v.y = y1;
    box[2].v.x = x2; box[2].v.y = y1;
    box[3].v.x = x2; box[3].v.y = y2;
 
    Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	//m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZENABLE, FALSE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_STENCILENABLE, FALSE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHATESTENABLE, FALSE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE );

    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0, NULL );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    IDirect3DDevice8_DrawVerticesUP( pst_SD->mp_D3DDevice,D3DPT_LINELOOP, 4, box, sizeof( struct TITLESAFE_BOX_VERTEX_ ) );
}

void AE_GLOBALMUL2Xb(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
		} MonStream[4];

		UINT uiVStride;
		ULONG Color = 0xffffffff;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE );

		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ONE );


		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0/*TRUE*/);

		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);

		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;

		IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}
	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	
}

void AE_BorderBright(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture,float factor)
{
#define div 16

	IDirect3DSurface8	*ppSurfaceLevel;
	ULONG Color;
	int v;
	float Hb,Wb,Wi,Hi;
	float X,Y;
	int CounterX,CounterY;

	IDirect3DTexture8_GetSurfaceLevel
	(
		destTexture,
		0,
		&ppSurfaceLevel
	);

	IDirect3DDevice8_SetRenderTarget
	(
		pst_SD->mp_D3DDevice,
		ppSurfaceLevel,
		NULL
	);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	//gAE_Params.BorderColor=0xff0000ff;
	//gAE_Params.BorderBrightNess=4.f;
	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);// <- missing before
/*
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
*/
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

		IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
		IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
		
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ZERO);

		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);

	//IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_ 
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4*div*div];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		
		v=0;

		Wi = 640 / div ;
		Hi = 480 / div ;

		Wb = 0.0f;
		CounterX = div;
		factor = 1;
		while(CounterX--)
		{
			Hb = 0.0f;
			CounterY = div;
			while(CounterY--)
			{
				X=Wb;Y=Hb;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = gAE_Params.BorderColor | (int)(X * 255.f);
				Color |= (Color << 8);
				Color |= (Color << 16);

				MonStream[v].x = Wb;
				MonStream[v].y = Hb;
				MonStream[v].z = 0;
				MonStream[v].rhw = 1;
				MonStream[v].Color = Color;
				MonStream[v].u = Wb;
				MonStream[v].v = Hb;

				X=Wb+Wi;Y=Hb;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = gAE_Params.BorderColor | (int)(X * 255.f);
				Color |= Color << 8;
				Color |= Color << 16;
  
				MonStream[v+1].x = Wb+Wi;
				MonStream[v+1].y = Hb;
				MonStream[v+1].z = 0;
				MonStream[v+1].rhw = 1;
				MonStream[v+1].Color = Color;
				MonStream[v+1].u = Wb+Wi;
				MonStream[v+1].v = Hb;

				X=Wb+Wi;Y=Hb+Hi;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = gAE_Params.BorderColor | (int)(X * 255.f);
				Color |= Color << 8;
				Color |= Color << 16;
 
				MonStream[v+2].x = Wb+Wi;
				MonStream[v+2].y = Hb+Hi;
				MonStream[v+2].z = 0;
				MonStream[v+2].rhw = 1;
				MonStream[v+2].Color = Color;
				MonStream[v+2].u = Wb+Wi;
				MonStream[v+2].v = Hb+Hi;

				X=Wb;Y=Hb+Hi;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = gAE_Params.BorderColor | (int)(X * 255.f);
				Color |= Color << 8;
				Color |= Color << 16;

				MonStream[v+3].x = Wb;
				MonStream[v+3].y = Hb+Hi;
				MonStream[v+3].z = 0;
				MonStream[v+3].rhw = 1;
				MonStream[v+3].Color = Color;
				MonStream[v+3].u = Wb;
				MonStream[v+3].v = Hb+Hi;

				v+=4;
				Hb += Hi ;
			}
			Wb += Wi ;
}

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,1*div*div,(void*)MonStream,uiVStride);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE );
	}


	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

}

void AE_BorderBright2(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture,float factor)
{
#define div 16

//	IDirect3DSurface8	*ppSurfaceLevel;
	ULONG Color;
	int v;
	float Hb,Wb,Wi,Hi;
	float X,Y;
	int CounterX,CounterY;
/*
	IDirect3DTexture8_GetSurfaceLevel
	(
		destTexture,
		0,
		&ppSurfaceLevel
	);

	IDirect3DDevice8_SetRenderTarget
	(
		pst_SD->mp_D3DDevice,
		ppSurfaceLevel,
		NULL
	);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);// <- missing before
/*
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
*/
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

		IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
		IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
		
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ZERO);

		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);

	//IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_ 
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4*div*div];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		
		v=0;

		Wi = 640 / div ;
		Hi = 480 / div ;

		Wb = 0.0f;
		CounterX = div;
		//factor = 1;
		while(CounterX--)
		{
			Hb = 0.0f;
			CounterY = div;
			while(CounterY--)
			{
				X=Wb;Y=Hb;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = (int)(X * 255.f);
				Color |= (Color << 8);
				Color |= (Color << 16);

				MonStream[v].x = Wb;
				MonStream[v].y = Hb;
				MonStream[v].z = 0;
				MonStream[v].rhw = 1;
				MonStream[v].Color = Color;
				MonStream[v].u = Wb;
				MonStream[v].v = Hb;

				X=Wb+Wi;Y=Hb;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = (int)(X * 255.f);
				Color |= Color << 8;
				Color |= Color << 16;
  
				MonStream[v+1].x = Wb+Wi;
				MonStream[v+1].y = Hb;
				MonStream[v+1].z = 0;
				MonStream[v+1].rhw = 1;
				MonStream[v+1].Color = Color;
				MonStream[v+1].u = Wb+Wi;
				MonStream[v+1].v = Hb;

				X=Wb+Wi;Y=Hb+Hi;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = (int)(X * 255.f);
				Color |= Color << 8;
				Color |= Color << 16;
 
				MonStream[v+2].x = Wb+Wi;
				MonStream[v+2].y = Hb+Hi;
				MonStream[v+2].z = 0;
				MonStream[v+2].rhw = 1;
				MonStream[v+2].Color = Color;
				MonStream[v+2].u = Wb+Wi;
				MonStream[v+2].v = Hb+Hi;

				X=Wb;Y=Hb+Hi;
				X -= 640 / 2.0f;
				Y -= 480 / 2.0f;
				X *= 2.0f/ 640;
				Y *= 2.0f/ 480;
				Y *= 640/ 480;
				X = 1.0f - (X * X + Y * Y) * factor;
				if (X < 0) X = 0;
				if (X > 1) X = 1;
				Color = (int)(X * 255.f);
				Color |= Color << 8;
				Color |= Color << 16;

				MonStream[v+3].x = Wb;
				MonStream[v+3].y = Hb+Hi;
				MonStream[v+3].z = 0;
				MonStream[v+3].rhw = 1;
				MonStream[v+3].Color = Color;
				MonStream[v+3].u = Wb;
				MonStream[v+3].v = Hb+Hi;

				v+=4;
				Hb += Hi ;
			}
			Wb += Wi ;
}

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,1*div*div,(void*)MonStream,uiVStride);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );

	}


//	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

}

void AE_ZoomSmoothRemanance(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture)
{

	float X,Y,fCurrentFocale ;
	float val,ConvoleRevolution;
	MATH_tdst_Vector		MotionSmoothPoint; 
	MATH_tdst_Vector		LigthDir;
	IDirect3DSurface8	*ppSurfaceLevel;

	HRESULT hr;
	UINT uiVStride;

	// render the rendered target
	int i;
	float resizeStep;

	ConvoleRevolution = 0.666666f;

	LigthDir = gAE_Params.ZoomSmoothRemananceLightDirection;
	fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
	MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &LigthDir);
	MotionSmoothPoint . x = ((fCurrentFocale) * MotionSmoothPoint . x) / MotionSmoothPoint . z;
	MotionSmoothPoint . y = ((fCurrentFocale) * MotionSmoothPoint . y) / MotionSmoothPoint . z;
	X = (MotionSmoothPoint . x);
	Y = (MotionSmoothPoint . y);//-

	X += 1.f;
	Y += 1.f;
	X *= 640 * .5f;
	Y *= 480 * .5f;

	if (MotionSmoothPoint . z < 0.f)
		val=-3;
	else
		val=3;

	resizeStep=val;

	IDirect3DTexture8_GetSurfaceLevel
		(
		destTexture,
		0,
		&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
		pst_SD->mp_D3DDevice,
		ppSurfaceLevel,
		NULL
		);

		IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_POINT);

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,255,255,255));

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_ONE);
	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ONE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);

	Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
	uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
	{
		struct MonStream_ 
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
		} MonStream[4];

		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;


		for (i=0;i<8;i++)
		//while ( fAbs(resizeStep) > MINUSSULUS)//MINUSSULUS)
		{
		IDirect3DSurface8	*ppSurfaceLevel;

			float facto = (resizeStep*0.1f)+1.f;

			MonStream[0].u = 0;
			MonStream[0].v = LOWBAND;
			MonStream[0].u2 = (0 - X) * facto + X;
			MonStream[0].v2 = (LOWBAND - Y) * facto + Y;

			MonStream[1].u = 0;
			MonStream[1].v = HIGHBAND;
			MonStream[1].u2 = (0 - X) * facto + X;
			MonStream[1].v2 = (HIGHBAND - Y) * facto + Y;

			MonStream[2].u = 640;
			MonStream[2].v = LOWBAND;
			MonStream[2].u2 = (640 - X) * facto + X;
			MonStream[2].v2 = (LOWBAND - Y) * facto + Y;

			MonStream[3].u = 640;
			MonStream[3].v = HIGHBAND;
			MonStream[3].u2 = (640 - X) * facto + X;
			MonStream[3].v2 = (HIGHBAND - Y) * facto + Y;


			hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
			//ConvoleRevolution = (ConvoleRevolution - 0.5f) * 0.5f + 0.5f;
			AE_SwapTextures(&sourceTexture,&destTexture);
	
			
		IDirect3DTexture8_GetSurfaceLevel
		(
		destTexture,
		0,
		&ppSurfaceLevel
		);

		IDirect3DDevice8_SetRenderTarget
		(
		pst_SD->mp_D3DDevice,
		ppSurfaceLevel,
		NULL
		);

	
			IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
			IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);

			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, FALSE );

			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );

			resizeStep*=0.5f;

			IDirect3DSurface8_Release(ppSurfaceLevel);
		}
	
	}
	

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,NULL);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,NULL);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,  D3DTOP_SELECTARG1);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);


	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, TRUE );
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SRCBLEND,  D3DBLEND_ONE );
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
	{
		u32 Alpha;
		Alpha = (u32)(gAE_Params.ZoomSmoothRemananceFactor * 255.0f);
		AE_DrawScreen(pst_SD,pst_SD->pSaveBufferTexture,(Alpha << 24) | 0xffffff);
	}

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, FALSE );

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

}

void AE_ZoomSmoothWithCenter(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture,float factor)
{

    float X,Y,fCurrentFocale ;
	float ConvoleRevolution;
	MATH_tdst_Vector		MotionSmoothPoint,ZSDir; 
	IDirect3DSurface8	*ppSurfaceLevel;
	HRESULT	hr;
	UINT uiVStride;

	// render the rendered target
	int	i;
	float resizeStep;
	ConvoleRevolution =	0.666666f;

	ZSDir =	gAE_Params.ZSDir;
	factor = gAE_Params.ZoomSmoothFactor;

	MATH_NormalizeVector(&ZSDir,&ZSDir);
	fCurrentFocale = 1.0f /	fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
	MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &ZSDir);
	MotionSmoothPoint .	x =	((fCurrentFocale) *	MotionSmoothPoint .	x) / MotionSmoothPoint . z;
	MotionSmoothPoint .	y =	((fCurrentFocale) *	MotionSmoothPoint .	y) / MotionSmoothPoint . z;
	X =	(MotionSmoothPoint . x);
	Y =	(MotionSmoothPoint	. y);
	factor *= MotionSmoothPoint	. z	* MotionSmoothPoint	. z;

	X += 1.f;
	Y += 1.f;
	X *= 640 * .5f;
	Y *= 480 * .5f;

	resizeStep=factor;

	IDirect3DTexture8_GetSurfaceLevel
		(
		destTexture,
		0,
		&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
		pst_SD->mp_D3DDevice,
		ppSurfaceLevel,
		NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_TEXTUREFACTOR , D3DCOLOR_ARGB(128,255,255,255));

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,	 D3DTOP_MODULATE  );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,	 D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,	 D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,	 D3DTOP_BLENDFACTORALPHA);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,	 D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,	 D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,	 D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAOP,	 D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);

	Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE	| D3DFVF_TEX2 );
	uiVStride =	D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE	| D3DFVF_TEX2 );
	{
		struct MonStream_ 
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
		} MonStream[4];

		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;

		if ( MotionSmoothPoint . z > 0.	)
		{
			for	(i=0;i<5;i++)
			//while	( fAbs(resizeStep) > MINUSSULUS)
			{
				float facto	= (resizeStep*0.020f)+1.f;
				facto = 1.0f / facto;
				MonStream[0].u = 0;
				MonStream[0].v = LOWBAND;
				MonStream[0].u2	= (0 - X) *	facto +	X;
				MonStream[0].v2	= (LOWBAND - Y)	* facto	+ Y;

				MonStream[1].u = 0;
				MonStream[1].v = HIGHBAND;
				MonStream[1].u2	= (0 - X) *	facto +	X;
				MonStream[1].v2	= (HIGHBAND - Y) *	facto +	Y;

				MonStream[2].u = 640;
				MonStream[2].v = LOWBAND;
				MonStream[2].u2	= (640 - X)	* facto	+ X;
				MonStream[2].v2	= (LOWBAND - Y)	* facto	+ Y;

				MonStream[3].u = 640;
				MonStream[3].v = HIGHBAND;
				MonStream[3].u2	= (640 - X)	* facto +	X;
				MonStream[3].v2	= (HIGHBAND - Y) *	facto	+ Y;

				hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
				ConvoleRevolution =	(ConvoleRevolution - 0.5f) * 0.5f +	0.5f;

				IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)destTexture);
				IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)destTexture);

				IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,	 D3DTOP_MODULATE );
				IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHABLENDENABLE, TRUE );

				IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
				IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	 );

				resizeStep*=0.5f;//0.5f

			}
		}

		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );
		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,NULL);
		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,NULL);
		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,NULL);

		IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
	}		
}
/*
void AE_DrawTheRenderedScreenSmooth(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
	float step=(gAE_Params.BlurFactor*10);// /25
	
//while (step >0.5f)
//{
	float Minuy = 1;
	//if (2.0f*step < Minuy) return;
/*	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,destTexture,(8.0f*step),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,(4.0f*step),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,pst_SD->pSmoothBufferTexture,destTexture,(2.0f*step),0,false);
	//if (0.5f*step < Minuy) return;
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,(1.0f*step),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,pst_SD->pSmoothBufferTexture,destTexture,(0.5f*step),0,false);
	//if (0.125f*step < Minuy) return;
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,(int)(0.25*step),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,pst_SD->pSmoothBufferTexture,destTexture,(int)(0.125*step),0,false);
*/
/*	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,sourceTexture,(8.0f*step),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,sourceTexture,(4.0f*step),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,sourceTexture,(2.0f*step),0,false);
	//if (0.5f*step < Minuy) return;
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,sourceTexture,(1.0f*step),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,sourceTexture,(0.5f*step),0,false);
//	if (0.125f*step < Minuy) return;
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,sourceTexture,(float)((int)(0.25*step)),0,false);
	AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,destTexture,(float)((int)(0.125*step)),0,false);

//	step*= 0.5f;
//}
//
}
*/

void AE_DrawTheRenderedScreenInFadeTexture(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture)
{
	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	
	IDirect3DTexture8_GetSurfaceLevel
		(
			pst_SD->pFadeBufferTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(480);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(480);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

}

void AE_DrawTheRenderedScreenWithFade(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	float fadeSpeed;

	if (gAE_Params.fadeTime*60<255)
		fadeSpeed=255/(60*gAE_Params.fadeTime);
	else
		fadeSpeed=1;

	
	if (pst_SD->iFadeAlfa==255)
	{
		AE_DrawTheRenderedScreenInFadeTexture(pst_SD,sourceTexture);
	}

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
    
	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)pst_SD->pFadeBufferTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
			float u2,v2;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(pst_SD->iFadeAlfa,255,255,255);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(480);
		MonStream[0].u2 = 0;
		MonStream[0].v2 = (float)(480);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(pst_SD->iFadeAlfa,255,255,255);
		MonStream[1].u = 0;
		MonStream[1].v = 0;
		MonStream[1].u2 = 0;
		MonStream[1].v2 = 0;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(pst_SD->iFadeAlfa,255,255,255);
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(480);
		MonStream[2].u2 = (float)(640);
		MonStream[2].v2 = (float)(480);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(pst_SD->iFadeAlfa,255,255,255);
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;
		MonStream[3].u2 = (float)(640);
		MonStream[3].v2 = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);

	if (pst_SD->iFadeAlfa>0)
	{
		pst_SD->iFadeAlfa-=(int)fadeSpeed;
		pst_SD->iFadeAlfa=(pst_SD->iFadeAlfa<0)?0:pst_SD->iFadeAlfa;
	}
}


// this function combine the scene output with shadows renderings
void AE_DrawShadowBufferSpots(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
	IDirect3DSurface8	*ppSurfaceLevel;
	int i,iShadowIntensityInv;
	
	iShadowIntensityInv=256-ShadowBuffer_ShadowIntensity;

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);
	
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(255,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(480);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(255,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity);
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(255,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity);
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(480);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(255,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity,ShadowBuffer_ShadowIntensity);
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_ONE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ONE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, 1);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_BLENDOP,D3DBLENDOP_ADD);

	for(i=0;i<pst_SD->iNOfSBLights;i++)
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;
	
		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)pst_SD->pShadowBufferSpot[i]);

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 480-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(255,iShadowIntensityInv,iShadowIntensityInv,iShadowIntensityInv);
		MonStream[0].u = 0;
		MonStream[0].v = (480-1.0f);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(255,iShadowIntensityInv,iShadowIntensityInv,iShadowIntensityInv);
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = 480-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(255,iShadowIntensityInv,iShadowIntensityInv,iShadowIntensityInv);
		MonStream[2].u = (640-1.0f);
		MonStream[2].v = (480-1.0f);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(255,iShadowIntensityInv,iShadowIntensityInv,iShadowIntensityInv);
		MonStream[3].u = (640-1.0f);
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}

void AE_DrawTheRenderedScreenWithColorBalance(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
	//IDirect3DSurface8	*ppSurfaceLevel;
	float Red,Green,Blue;
	ULONG Color;
	unsigned char Colorb[4];

/*	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);*/

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );//before substract
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	Red   = gAE_Params.CB_Intensity * (1.0f - GetPhase(gAE_Params.CB_Spectre)) + (1.0f - gAE_Params.CB_Intensity);
	Green = gAE_Params.CB_Intensity * (1.0f - GetPhase(gAE_Params.CB_Spectre - 0.3333333f + 1.0f )) + (1.0f - gAE_Params.CB_Intensity);
	Blue  = gAE_Params.CB_Intensity * (1.0f - GetPhase(gAE_Params.CB_Spectre - 0.6666666f + 1.0f)) + (1.0f - gAE_Params.CB_Intensity);
	Colorb[0] = (unsigned char)(Red*255.0f);
	Colorb[1] = (unsigned char)(Green*255.0f);
	Colorb[2] = (unsigned char)(Blue*255.0f);
	Color = Colorb[0] | (Colorb[1]<<8) | (Colorb[2]<<16) | (0x80<<24);

	Color = Gx8_M_ConvertColor(Color);
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}
/*
void AE_popona(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture ,IDirect3DSurface8 *pZBufferSurface)
{
	IDirect3DSurface8	*ppSurfaceLevel;
	float Red,Green,Blue;
	ULONG Color;
	unsigned char Colorb[4];

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			pZBufferSurface
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_GREATER);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );//before substract
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);


	Color = 0xff00ff00;
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0.9f;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0.99f;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0.999f;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0.9999f;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0);
		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}
	//AE_QUADZ(pst_SD,0xff000000,1);
	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}
*/
void AE_DrawTheRenderedScreenWithBrightness(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture )
{
	ULONG Color;
	float Brightness;

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

	if ((Brightness=gAE_Params.Brighness)<0.5)
	{
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SUBTRACT );
		Brightness = (0.5f - Brightness) * 1.0f;
	}
	else
	{
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_ADD );
		Brightness = (Brightness - 0.5f ) * 1.0f;
	}

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	Color = 0xff000000|(u_int)(Brightness * 255.0f);
	Color |= Color << 8;
	Color |= Color << 16;
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = LOWBAND;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = 640;
		MonStream[2].v = LOWBAND;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = 640;
		MonStream[3].v = HIGHBAND;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}


	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}
void AE_DrawQUADContrast(Gx8_tdst_SpecificData *pst_SD, ULONG Color )
{
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
		} MonStream[4];

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

		IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
		IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
		
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ONE);

		IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);

		MonStream[0].x = 0;
		MonStream[0].y = LOWBAND-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;

		MonStream[2].x = 640-0.5f;
		MonStream[2].y = LOWBAND-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;

		MonStream[3].x = 640-0.5f;
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;

		IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

}

void AE_DrawTheRenderedScreenWithContrast(Gx8_tdst_SpecificData *pst_SD )
{
	float Contrast;

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	Contrast=gAE_Params.Contrast;

	if (Contrast != 0.5f)
	{
		ULONG Color;
		Contrast -= 0.5f;
		Contrast *= 2.0f;
		if (Contrast > 0.0f)
		{
			IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

			while (Contrast > 0.0f)
			{
				Color = (ULONG)(Contrast * 255.0f);
				if (Color > 255) Color = 255;
				Color |= Color << 8;
				Color |= Color << 16;
				AE_DrawQUADContrast(pst_SD,Color);

				Contrast -= 1.0f;
				if (Contrast > 2.0f) Contrast = 2.0f;

			}
			
			IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
		} 
	}
}

/*
void AE_DrawTheRenderedScreenReduced(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{
	IDirect3DSurface8	*ppSurfaceLevel;

	IDirect3DTexture8_GetSurfaceLevel
		(
			destTexture,
			0,
			&ppSurfaceLevel
		);

	IDirect3DDevice8_SetRenderTarget
		(
			pst_SD->mp_D3DDevice,
			ppSurfaceLevel,
			NULL
		);

	IDirect3DSurface8_Release(ppSurfaceLevel);

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER, D3DTEXF_QUINCUNX );
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER, D3DTEXF_QUINCUNX );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = (480-0.5f);
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(pst_SD->AfterFXTexture_Height);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = (640-0.5f);
		MonStream[2].y = (480-0.5f);
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(pst_SD->AfterFXTexture_Height);

		MonStream[3].x = (640-0.5f);
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = (float)(640);
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}
*/



/*
void AE_ResetAETexturesPool(Gx8_tdst_SpecificData *pst_SD)
{
	int i;
	int debugSpy;
	int newRefCount;


	debugSpy=pst_SD->iAEColorET_Count;

	gAE_Status=gAE_OldStatus=0;

	for(i=0;i<pst_SD->iAEColorET_Count;i++)
		do {
			newRefCount=IDirect3DTexture8_Release(pst_SD->pAEColorET[i]);
		} while (newRefCount>0);

	pst_SD->iAEColorET_Count=0;
	for(i=0;i<AEColorET_Max;i++)
		pst_SD->bAEColorET_Available[i]=true;

}
*/

bool AE_AllocateColorTexture(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 **texture)
{
	int i;
	//HRESULT                 hr;
	int debugSpy;

	debugSpy=pst_SD->iAEColorET_Count;

	for(i=0;i<pst_SD->iAEColorET_Count;i++)
	{
		if (pst_SD->bAEColorET_Available[i])
		{
			*texture=pst_SD->pAEColorET[i];
			pst_SD->bAEColorET_Available[i]=false;
			return true;
		}
	}

/*
	if (pst_SD->iAEColorET_Count<AEColorET_Max)
	{
			pst_SD->pAEColorET[pst_SD->iAEColorET_Count]=CreateTileTexture(640,480,pst_SD->iAEColorET_Count + 2);
			*texture=pst_SD->pAEColorET[pst_SD->iAEColorET_Count];
			pst_SD->bAEColorET_Available[pst_SD->iAEColorET_Count]=false;
			pst_SD->iAEColorET_Count++;
			return true;
	}
*/
	gAE_Status=gAE_OldStatus;

	return false;
}

void AE_FreeColorTexture(Gx8_tdst_SpecificData *pst_SD,IDirect3DTexture8 **texture)
{
	int i;
	int debugSpy;

	debugSpy=pst_SD->iAEColorET_Count;

	for(i=0;i<pst_SD->iAEColorET_Count;i++)
	{
		if (pst_SD->pAEColorET[i]==*texture)
		{
			*texture=NULL;
			pst_SD->bAEColorET_Available[i]=true;
			return;
		}
	}
}


void AE_ManageAETexturesAllocation(Gx8_tdst_SpecificData *_pst_SD)
{

//    HRESULT                 hr;

	// ===================================================================================

	//	FREE 

	// ===================================================================================

	if (
	(
	(gAE_OldStatus & GSP_Status_AE_REMANANCE)
	&&
	!(gAE_Status & GSP_Status_AE_REMANANCE)
	)
	||
	(
	(gAE_OldStatus & GSP_Status_AE_DB)
	&&
	!(gAE_Status & GSP_Status_AE_DB)
	)
	||
	(
	(gAE_OldStatus & GSP_Status_AE_DBN)
	&&
	!(gAE_Status & GSP_Status_AE_DBN)
	)
	||
	(
	(gAE_OldStatus & GSP_Status_AE_SM)
	&&
	!(gAE_Status & GSP_Status_AE_SM)
	)
	)
	{
		if (
		!(gAE_Status & GSP_Status_AE_REMANANCE)
		&&
		!(gAE_Status & GSP_Status_AE_DB)
		&&
		!(gAE_Status & GSP_Status_AE_DBN)
		&&
		!(gAE_Status & GSP_Status_AE_SM)
		)
		{
			if ( _pst_SD->pSmoothBufferTexture )
			AE_FreeColorTexture(_pst_SD,&_pst_SD->pSmoothBufferTexture);
		}
	}

	if 
	(
	(gAE_OldStatus & GSP_Status_AE_DB)
	&&
	!(gAE_Status & GSP_Status_AE_DB)
	)
	{
		if ( _pst_SD->pAlphaForDepthBlurTexture )
		AE_FreeColorTexture(_pst_SD,&_pst_SD->pAlphaForDepthBlurTexture);
	}

	if 
	(
	(gAE_OldStatus & GSP_Status_AE_DBN)
	&&
	!(gAE_Status & GSP_Status_AE_DBN)
	)
	{
		if ( _pst_SD->pAlphaForNearBlurTexture )
		AE_FreeColorTexture(_pst_SD,&_pst_SD->pAlphaForNearBlurTexture);
	}

	if 
	(
	(gAE_OldStatus & GSP_Status_AE_MB)
	&&
	!(gAE_Status & GSP_Status_AE_MB)
	)
	{
 		if ( _pst_SD->pMotionBlurFrameBufferTexture )
		AE_FreeColorTexture(_pst_SD,&_pst_SD->pMotionBlurFrameBufferTexture);
 	}

	if 
	(
	(gAE_OldStatus & GSP_Status_AE_FADE)
	&&
	!(gAE_Status & GSP_Status_AE_FADE)
	)
	{
 		if ( _pst_SD->pFadeBufferTexture )
		AE_FreeColorTexture(_pst_SD,&_pst_SD->pFadeBufferTexture);
 	}

	// ===================================================================================

	//	ALLOC 

	// ===================================================================================

	if (
	(
	(gAE_Status & GSP_Status_AE_REMANANCE)
	&&
	!(gAE_OldStatus & GSP_Status_AE_REMANANCE)
	)
	||
	(
	(gAE_Status & GSP_Status_AE_DB)
	&&
	!(gAE_OldStatus & GSP_Status_AE_DB)
	)
	||
	(
	(gAE_Status & GSP_Status_AE_DBN)
	&&
	!(gAE_OldStatus & GSP_Status_AE_DBN)
	)
	||
	(
	(gAE_Status & GSP_Status_AE_SM)
	&&
	!(gAE_OldStatus & GSP_Status_AE_SM)
	)
	)
	{
		if (
		!(gAE_OldStatus & GSP_Status_AE_REMANANCE)
		&&
		!(gAE_OldStatus & GSP_Status_AE_DB)
		&&
		!(gAE_OldStatus & GSP_Status_AE_DBN)
		&&
		!(gAE_OldStatus & GSP_Status_AE_SM)
		)
		{
			if (!_pst_SD->pSmoothBufferTexture)
			AE_AllocateColorTexture(_pst_SD,&_pst_SD->pSmoothBufferTexture);
		}
	}

	if 
	(
	(gAE_Status & GSP_Status_AE_DB)
	&&
	!(gAE_OldStatus & GSP_Status_AE_DB)
	)
	{
		if (AE_AllocateColorTexture(_pst_SD,&_pst_SD->pAlphaForDepthBlurTexture))
			AE_DepthNearBlur_InitZBuffer(_pst_SD,_pst_SD->pAlphaForDepthBlurTexture,&_pst_SD->tZBufferForDepthBlurTexture);
	}

	if 
	(
	(gAE_Status & GSP_Status_AE_DBN)
	&&
	!(gAE_OldStatus & GSP_Status_AE_DBN)
	)
	{
		if (AE_AllocateColorTexture(_pst_SD,&_pst_SD->pAlphaForNearBlurTexture))
			AE_DepthNearBlur_InitZBuffer(_pst_SD,_pst_SD->pAlphaForNearBlurTexture,&_pst_SD->tZBufferForNearBlurTexture);
	}

	if 
	(
	(gAE_Status & GSP_Status_AE_MB)
	&&
	!(gAE_OldStatus & GSP_Status_AE_MB)
	)
	{
		if (!_pst_SD->pMotionBlurFrameBufferTexture)
		AE_AllocateColorTexture(_pst_SD,&_pst_SD->pMotionBlurFrameBufferTexture);
	}

	if 
	(
	(gAE_Status & GSP_Status_AE_FADE)
	&&
	!(gAE_OldStatus & GSP_Status_AE_FADE)
	)
	{
		if (!_pst_SD->pFadeBufferTexture)
		AE_AllocateColorTexture(_pst_SD,&_pst_SD->pFadeBufferTexture);
	}

}

void Gsp_AE( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//static bool bNoAE = false;
	extern bNoAE;//GX8_Menu
	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
	
	if(bNoAE)
	{
		if ( Antialias_E3 )
		IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_COPY);
	
		return;
	}


	// end for frame buffer texture
	if(pst_SD->RenderScene)
    {		
		IDirect3DTexture8 *sourceTexture,*destTexture,*destTextureCopy;
		IDirect3DSurface8 *pBackBufferSurface;
		IDirect3DSurface8 *pBackBufferSurface2;
		IDirect3DSurface8 *pZBufferSurface;
		IDirect3DSurface8 *pZBufferSurface2;
	    D3DSURFACE_DESC desc;//,desc1;
		ULONG ulBlendingMode = 0;

		IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
			
		//if (( (gAE_Status != 0  || (pst_SD->bShadowBuffer) ||  (gAE_Params.Brighness!=0.5) || (gAE_Params.Contrast!=0.5)) && !(gAE_Status & GSP_Status_AE_NOAE)))
		//if (!(gAE_Status & GSP_Status_AE_NOAE))
		{
			
	
			
			
			// creation of the back buffer texture
			// PC_TODO: creation of the back buffer texture
/*
			IDirect3DDevice8_GetBackBuffer( pst_SD->mp_D3DDevice, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferSurface );
			IDirect3DDevice8_GetDepthStencilSurface (pst_SD->mp_D3DDevice, &pZBufferSurface);
			IDirect3DSurface8_GetDesc( pBackBufferSurface, &desc );
			ZeroMemory( pst_SD->pBackBufferTexture, sizeof(IDirect3DTexture8) );
			XGSetTextureHeader( desc.Width, desc.Height, 1, 0, desc.Format, 0, pst_SD->pBackBufferTexture, 
								pBackBufferSurface->Data, desc.Width * XGBytesPerPixelFromFormat(desc.Format) );
*/
			SwapOneTime = false;
			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ALPHAKILL, FALSE);
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_FOGENABLE ,  FALSE );
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );
	        //Gx8_M_RenderState(pst_SD, D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
			IDirect3DDevice8_SetRenderState(pst_SD,D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

			//Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, FALSE );
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHATESTENABLE,FALSE);
			//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0/*TRUE*/);
			//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ZENABLE,FALSE);


			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX,  D3DTSS_TCI_PASSTHRU | 0 );
			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_TEXCOORDINDEX,  D3DTSS_TCI_PASSTHRU | 1 );
			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_TEXCOORDINDEX,  D3DTSS_TCI_PASSTHRU | 2 );
			IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 3, D3DTSS_TEXCOORDINDEX,  D3DTSS_TCI_PASSTHRU | 3 );
			
			Gx8_RS_DepthFunc(pst_SD, D3DCMP_ALWAYS);
			//pipi Gx8_RS_CullFace(pst_SD, D3DCULL_CCW);

			//GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_TestBackFace;
			//GDI_gpst_CurDD->ul_CurrentDrawMask ^= GDI_Cul_DM_NotInvertBF;
			
			//Gx8_ScaleBackBuffer(0.5f,1);
			IDirect3DDevice8_GetDepthStencilSurface( pst_SD->mp_D3DDevice, &pZBufferSurface );
			//Gx8_ScaleBackBuffer(1,1);
			

			//IDirect3DDevice8_GetDepthStencilSurface( pst_SD->mp_D3DDevice, &pZBufferSurface );

			// For antialias: -> else deactive
			//Gx8_StartGPUMon();
			
			//AE_QUADZ(pst_SD,0x00000000,0.9999999f);
			//Gx8_ScaleBackBuffer(0.5f,1.f);
			//IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice,  D3DSWAP_BYPASSCOPY );//to be set ON with antialias
			
			
			//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_MULTISAMPLEANTIALIAS, FALSE );
			
			
			AE_GLOBALMUL2Xb( pst_SD ,sourceTexture,destTexture);	
			
			//Gx8_StopGPUMon();
			if ( Antialias_E3 )
			{
				IDirect3DDevice8_GetRenderTarget( pst_SD->mp_D3DDevice, &pBackBufferSurface2 );
				IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_COPY);
				IDirect3DDevice8_GetDepthStencilSurface( pst_SD->mp_D3DDevice, &pZBufferSurface2 );
			}

			IDirect3DDevice8_GetRenderTarget( pst_SD->mp_D3DDevice, &pBackBufferSurface );
 			IDirect3DSurface8_GetDesc( pBackBufferSurface, &desc );

			ZeroMemory( pst_SD->pBackBufferTexture, sizeof(IDirect3DTexture8) );
			XGSetTextureHeader( desc.Width, desc.Height, 0, 0, desc.Format, 0, pst_SD->pBackBufferTexture, 
									pBackBufferSurface->Data, desc.Width * XGBytesPerPixelFromFormat(desc.Format) );
			//XGSetTextureHeader( 720, 576, 0, 0, desc.Format, 0, pst_SD->pBackBufferTexture, 
			//						pBackBufferSurface->Data, 720 * XGBytesPerPixelFromFormat(desc.Format) );


			sourceTexture=pst_SD->pBackBufferTexture;
			destTexture=pst_SD->pTmpFrameBufferTexture;
	
			//AE_GLOBALMUL2X( pst_SD ,sourceTexture,destTexture);
			//AE_GLOBALMUL2Xb( pst_SD ,sourceTexture,destTexture);

			//AE_SwapTextures(&sourceTexture,&destTexture);

			if (gAE_Status & GSP_Status_AE_ZSR)
			{			
				if (gAE_Params.ZoomSmoothRemananceFactor!=0)
				{
					// -- Copy backbuffer --
					IDirect3DSurface8	*ppSurfaceLevel;
					
					destTextureCopy=pst_SD->pSaveBufferTexture;

					IDirect3DTexture8_GetSurfaceLevel
					(
						destTextureCopy,//destTextureCopy,
						0,
						&ppSurfaceLevel
					);

					IDirect3DDevice8_SetRenderTarget
					(
						pst_SD->mp_D3DDevice,
						ppSurfaceLevel,
						NULL
					);
					
					//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0);
					//AE_popona(pst_SD,sourceTexture,destTexture,pZBufferSurface);
	/*				
					IDirect3DTexture8_GetSurfaceLevel
					(
						destTexture,//destTextureCopy,
						0,
						&ppSurfaceLevel
					);
					
					IDirect3DDevice8_SetRenderTarget
					(
						pst_SD->mp_D3DDevice,
						ppSurfaceLevel,
						pZBufferSurface
					);
	*/				
					//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 0/*TRUE*/);
					//AE_QUADZ(pst_SD,0x00000000,0.9999999f);
					//AE_SwapTextures(&sourceTexture,&destTexture);

					

					AE_DrawTheRenderedScreen(pst_SD,sourceTexture);
					IDirect3DSurface8_Release(ppSurfaceLevel);
					
					
					
					// Go back to backbuffer

					//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_MULTISAMPLEANTIALIAS, 0 );
					//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_MULTISAMPLERENDERTARGETMODE, D3DMULTISAMPLEMODE_2X );
					
/*if ( Antialias_E3 )
{
					IDirect3DDevice8_SetRenderTarget
					(
						pst_SD->mp_D3DDevice,
						pBackBufferSurface2,
						pZBufferSurface
					);
					
					AE_QUADZ(pst_SD,0x00000000,0.9999999f);
					
					IDirect3DSurface8_GetDesc( pBackBufferSurface2, &desc );
					ZeroMemory( pst_SD->pBackBufferTextureTemp, sizeof(IDirect3DTexture8) );
					XGSetTextureHeader( desc.Width, desc.Height, 0, 0, desc.Format, 0, pst_SD->pBackBufferTextureTemp, 
					pBackBufferSurface2->Data, desc.Width * XGBytesPerPixelFromFormat(desc.Format) );
					
					sourceTexture=pst_SD->pBackBufferTextureTemp;
					destTexture=pst_SD->pBackBufferTexture;

					{	
						IDirect3DSurface8	*ppSurfaceLevelo;

						IDirect3DTexture8_GetSurfaceLevel
						(
							destTexture,
							0,
							&ppSurfaceLevelo
						);

						IDirect3DDevice8_SetRenderTarget
						(
							pst_SD->mp_D3DDevice,
							ppSurfaceLevelo,
							NULL
						);

					AE_DrawTheRenderedScreenTemp(pst_SD,sourceTexture);
					IDirect3DSurface8_Release(ppSurfaceLevelo);
					}
					
}*/
										
					destTexture=pst_SD->pTmpFrameBufferTexture;
					sourceTexture=pst_SD->pBackBufferTexture;

					IDirect3DDevice8_SetRenderTarget
					(
						pst_SD->mp_D3DDevice,
						pBackBufferSurface,
						pZBufferSurface
					);

					if (!Antialias_E3) AE_QUADZ(pst_SD,0x00000000,0.9999999f);
			}
	
	}	

			if (gAE_Status & GSP_Status_AE_ZSR)
			{			
				if (gAE_Params.ZoomSmoothRemananceFactor!=0)
				{
					//AE_BorderBright(pst_SD,sourceTexture,destTexture,1.0f);
					AE_BorderBright(pst_SD,destTexture,sourceTexture,1.0f);
					//AE_SwapTextures(&sourceTexture,&destTexture);

					AE_ZoomSmoothRemanance(pst_SD,sourceTexture,destTexture);            				
					AE_SwapTextures(&sourceTexture,&destTexture);
					
				}
			}

			// if shadow buffer is required combine output with SB renderings
			if (pst_SD->bShadowBuffer)
			{
				AE_DrawShadowBufferSpots(pst_SD,sourceTexture,destTexture);

				AE_SwapTextures(&sourceTexture,&destTexture);
			}
/*
			if (gAE_Status & GSP_Status_AE_CB_On)
			{
	
				AE_DrawTheRenderedScreenWithColorBalance(pst_SD,sourceTexture,sourceTexture);
				//AE_SwapTextures(&sourceTexture,&destTexture);
			}
*/
	/*		if (gAE_Status & GSP_Status_AE_CB_On)
			{
	
				AE_DrawTheRenderedScreenWithColorBalance(pst_SD,sourceTexture,destTexture);
				//AE_DrawTheRenderedScreenWithColorBalance(pst_SD,sourceTexture,sourceTexture);
				//if (sourceTexture == pst_SD->pBackBufferTexture) 
				AE_SwapTextures(&sourceTexture,&destTexture);
			}*/

		if (gAE_Status & GSP_Status_AE_REMANANCE)
			{ 
//				if(g_bOptimizedReimanance)
					//AE_DrawTheRenderedScreenRemanance_OPT(pst_SD,sourceTexture,destTexture);
			//	else
				if (pst_SD->pSmoothBufferTexture)
				{
					AE_DrawTheRenderedScreenRemanance(pst_SD,sourceTexture,destTexture);
					AE_SwapTextures(&sourceTexture,&destTexture);
				}
			}

			if (gAE_Status & GSP_Status_AE_FADE)
			{
				if ( pst_SD->pFadeBufferTexture )
				{
					AE_DrawTheRenderedScreenWithFade(pst_SD,sourceTexture,destTexture);
					AE_SwapTextures(&sourceTexture,&destTexture);		
				}
			}
			
			{
				static u32 LastWasOn = 0;
				// Blendage Bug
				if ( pst_SD->pMotionBlurFrameBufferTexture )
				{
					if (gAE_Params.MotionBlurFactor!=0)
					{
						fnvDrawBlendedInMBBuffer(pst_SD,sourceTexture);
						if (LastWasOn > 30) 
						{
							AE_DrawTheRenderedScreenWithMotionBlur(pst_SD,sourceTexture);
							//LastWasOn++;
						}
						else	LastWasOn++;
					} //else
				}
					//LastWasOn = 0;
			}
			
			if (gAE_Status & GSP_Status_AE_SMSM)
			{
				float fStep;
				float diff1,diff2;

				MotionSmoothPoint.x=1;
				MotionSmoothPoint.y=1;
				MotionSmoothPoint.z=1;
				
				MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &MotionSmoothPoint);
				
				fStep=MATH_f_Distance(&MotionSmoothPoint,&OldMotionSmoothPoint);

				//if ( (fabs(MotionSmoothPoint.x-OldMotionSmoothPoint.x)>0.005f) || (fabs(MotionSmoothPoint.y-OldMotionSmoothPoint.y)>0.005f) )
				if (fStep>0.005f)
				{
					diff1=(MotionSmoothPoint.x-OldMotionSmoothPoint.x)*50;
					diff2=(MotionSmoothPoint.y-OldMotionSmoothPoint.y)*50;

					AE_DrawTheRenderedScreenWithColorAttenuation(pst_SD,sourceTexture,destTexture,64);
					AE_SwapTextures(&sourceTexture,&destTexture);
					
					AE_DrawTheRenderedScreenWithMotionSmooth(pst_SD,sourceTexture,destTexture,diff1,diff2);            
					AE_SwapTextures(&sourceTexture,&destTexture);
				}

				OldMotionSmoothPoint.x=MotionSmoothPoint.x;
				OldMotionSmoothPoint.y=MotionSmoothPoint.y;
				OldMotionSmoothPoint.z=MotionSmoothPoint.z;
			}
			
			if (gAE_Status & GSP_Status_AE_ZS)
			{			
				if (gAE_Params.ZoomSmoothFactor!=0)
				{
					if ( gAE_Params.ZSDirIsValidated )
					{
						AE_ZoomSmoothWithCenter(pst_SD,sourceTexture,sourceTexture,5);
						//AE_ZoomSmoothWithCenter(pst_SD,sourceTexture,destTexture,5);
						//AE_SwapTextures(&sourceTexture,&destTexture);
						gAE_Params.ZSDirIsValidated = 0; //not here in other sources ??
					}
					else
					{
						AE_DrawTheRenderedScreenWithZoomSmooth(pst_SD,sourceTexture,sourceTexture);
						//AE_DrawTheRenderedScreenWithZoomSmooth(pst_SD,sourceTexture,destTexture);            				
						//AE_SwapTextures(&sourceTexture,&destTexture);
						gAE_Params.ZSDirIsValidated = 0;
					}
				}
		}

			if (gAE_Status & GSP_Status_AE_Border)
			{			
				if (gAE_Params.BorderBrightNess!=0)
				{
					AE_BorderBright2(pst_SD,sourceTexture,destTexture,gAE_Params.BorderBrightNess);            				

//					if (gAE_Params.BorderBrightNess<=1.f && gAE_Params.BorderBrightNess>0) AE_BorderBright(pst_SD,destTexture,sourceTexture,gAE_Params.BorderBrightNess);            				
					//AE_SwapTextures(&sourceTexture,&destTexture);
				}
			}

			//AE_BorderBright(pst_SD,sourceTexture,destTexture,10.0f);

			if (gAE_Status & GSP_Status_AE_SM)
			{
				//AE_DrawTheRenderedScreenSmooth(pst_SD,sourceTexture,destTexture);
				//Swap made in fonction " AE_DrawTheRenderedScreenSmooth " if >=1
				float step=(gAE_Params.BlurFactor*10);

				if ( step>1 ) 
				{
					AE_BigBlur(pst_SD,sourceTexture,destTexture,step,0,false);
					AE_SwapTextures(&sourceTexture,&destTexture);
				}
			}

			if (gAE_Status & GSP_Status_AE_RS)
			{
				if (gAE_Params.SpinSmoothFactor!=0)
				{
					AE_DrawTheRenderedScreenWithRotationSmooth(pst_SD,sourceTexture,destTexture);				
					AE_SwapTextures(&sourceTexture,&destTexture);
				}
			}

			
			if (gAE_Status & GSP_Status_AE_BW)
			{				
				AE_DrawTheRenderedScreenBW(pst_SD,sourceTexture,destTexture);
				AE_DrawTheRenderedScreenBW_2Pass(pst_SD,sourceTexture,destTexture);
				AE_SwapTextures(&sourceTexture,&destTexture);
			}
			


			if ((gAE_Status & GSP_Status_AE_DB) || (gAE_Status & GSP_Status_AE_DBN))
			{
				if (pst_SD->pSmoothBufferTexture)
				{
					AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,sourceTexture,pst_SD->pSmoothBufferTexture,1,0,false);
					AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,pst_SD->pSmoothBufferTexture,destTexture,2,0,false);
					AE_DrawTheRenderedScreenInSmoothTexture(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,3,0,false);
				}
			}

			if (gAE_Status & GSP_Status_AE_DB)
			{
			/*	if (pst_SD->pSmoothBufferTexture)
				{
					AE_DrawTheRenderedScreenWithDepthBlur(pst_SD,sourceTexture,destTexture);
					AE_SwapTextures(&sourceTexture,&destTexture);
				}*/
			}

			if (gAE_Status & GSP_Status_AE_DBN)
			{
			/*	if (pst_SD->pSmoothBufferTexture)
				{
					AE_DrawTheRenderedScreenWithNearBlur(pst_SD,sourceTexture,destTexture);
					AE_SwapTextures(&sourceTexture,&destTexture);
				}*/
			}

			if (gAE_Status & GSP_Status_AE_CB_On)
			{
	
				//AE_DrawTheRenderedScreenWithColorBalance(pst_SD,sourceTexture,destTexture);
				AE_DrawTheRenderedScreenWithColorBalance(pst_SD,sourceTexture,sourceTexture);
				//if (sourceTexture == pst_SD->pBackBufferTexture) 
				//AE_SwapTextures(&sourceTexture,&destTexture);
			}

			if (gAE_Params.Brighness!=0.5)
			{
				AE_DrawTheRenderedScreenWithBrightness(pst_SD,sourceTexture);
			}

			if (gAE_Params.Contrast!=0.5)
			{
				AE_DrawTheRenderedScreenWithContrast(pst_SD);				
			}


				JADE_HorizontalSymetry = GDI_gpst_CurDD->GlobalXInvert;
				if (JADE_HorizontalSymetry)	
				{
					AE_DrawTheRenderedScreen_FlipH(pst_SD,sourceTexture,destTexture);
					AE_SwapTextures(&sourceTexture,&destTexture);
				}

				IDirect3DDevice8_SetRenderTarget
				(
					pst_SD->mp_D3DDevice,
					pBackBufferSurface,
					pZBufferSurface
				);

			//AE_DrawTheRenderedScreen(pst_SD,pst_SD->pSaveBufferTexture);
			//AE_DrawTheRenderedScreen(pst_SD,pst_SD->pBackBufferTexture);
			
			//if ( SwapOneTime && sourceTexture == pst_SD->pBackBufferTexture)
			//AE_SwapTextures(&sourceTexture,&destTexture);
			
			if ( sourceTexture != pst_SD->pBackBufferTexture )
			{
				AE_DrawTheRenderedScreen(pst_SD,sourceTexture);

			}
			
			//AE_GLOBALMUL2Xb( pst_SD ,sourceTexture,destTexture);
			if ( TVMode == 1 ) AE_QUAD169( pst_SD );

			//GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_4over3;

			IDirect3DSurface8_Release(pBackBufferSurface);
			IDirect3DSurface8_Release(pZBufferSurface);
//			IDirect3DSurface8_Release(pZBufferSurface1);

			gAE_OldStatus=gAE_Status;
		}		

/* TEST SHOW TEXTURE
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_POINT);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0, pst_SD->Tex_Noise );
	{
		struct MonStream_
		{
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = 64-0.5f;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = 0xffffffff;
		MonStream[0].u = 0;
		MonStream[0].v = (float)(64);

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = 0xffffffff;
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = (64-0.5f);
		MonStream[2].y = 64-0.5f;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = 0xffffffff;
		MonStream[2].u = (float)(64);
		MonStream[2].v = (float)(64);

		MonStream[3].x = (64-0.5f);
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = 0xffffffff;
		MonStream[3].u = (float)(64);
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}
*/
		if (flag_MemorySpy)Gx8_MemorySpy((void *)(pst_SD->mp_D3DDevice));
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 1/*TRUE*/);

		//AE_TITLESAFEAREA( pst_SD->mp_D3DDevice);
		
		//Gx8_ScaleBackBuffer(1,1);

		if (pst_SD->iFadeAlfa==0)
			gAE_Status &= ~GSP_Status_AE_FADE;

		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
		Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode,0);



}

}

