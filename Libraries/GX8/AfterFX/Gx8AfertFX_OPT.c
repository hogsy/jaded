#include "GDInterface/GDInterface.h"
#include "GX8\Gx8init.h"
#include "BASe/MEMory/MEM.h"

#ifndef _AFTERFX_DEF_H_
#include "Gx8AfterFX_Def.h"
#endif

#include "Gx8\Gx8renderstate.h"

#include "Gx8\Gx8GPUMon.h"

#ifdef _XBOX
#include <xgraphics.h>
#endif

#include "Gx8AfterFX_OPT.h"

#define HIGHBAND	60
#define LOWBAND		420


extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;
extern GSP_AfterEffectParams gAE_Params;

void AE_DrawTheRenderedScreenInSmoothTexture_OPT(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture, int offset, float *ScaleFactor)
{
	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	int deltau,deltav;
	float	HIGHBANDSMOOTH = 60.0f;
	float	LOWBANDSMOOTH = 420.0f * (*ScaleFactor);
	float	TargetScaleFactor = (*ScaleFactor)/2.0f;
	
	float	P0x =  0.0f;
	float	P0y = 420.0f * TargetScaleFactor;
	float	P1x =  0.0f;
	float	P1y = 60.0f;
	float	P2x = 640.0f * TargetScaleFactor;
	float	P2y = 420.0f * TargetScaleFactor;
	float	P3x = 640.0f * TargetScaleFactor;
	float	P3y = 60.0f;


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

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,3,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,3,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLORARG2, D3DTA_CURRENT );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,3,(IDirect3DBaseTexture8 *)sourceTexture);

	deltau=deltav=offset;

	{
		struct MonStream_ {
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
			
		MonStream[0].x = P0x;
		MonStream[0].y = P0y;
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[0].u = (float)(-deltau);
		MonStream[0].v = (float)LOWBANDSMOOTH;
		MonStream[0].u2 = (float)deltau;
		MonStream[0].v2 = (float)LOWBANDSMOOTH;
		MonStream[0].u3 = 0;
		MonStream[0].v3 = (float)(LOWBANDSMOOTH-deltav);
		MonStream[0].u4 = 0;
		MonStream[0].v4 = (float)(LOWBANDSMOOTH+deltav);

		MonStream[1].x = P1x;
		MonStream[1].y = P1y;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[1].u = (float)(-deltau);
		MonStream[1].v = HIGHBANDSMOOTH;
		MonStream[1].u2 = (float)deltau;
		MonStream[1].v2 = HIGHBANDSMOOTH;
		MonStream[1].u3 = 0;
		MonStream[1].v3 = (float)(HIGHBANDSMOOTH-deltav);
		MonStream[1].u4 = 0;
		MonStream[1].v4 = (float)(HIGHBANDSMOOTH+deltav);

		MonStream[2].x = P2x;
		MonStream[2].y = P2y;
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[2].u = (float)640*(*ScaleFactor)-deltau;
		MonStream[2].v = (float)LOWBANDSMOOTH;
		MonStream[2].u2 = (float)640*(*ScaleFactor)+deltau;
		MonStream[2].v2 = (float)LOWBANDSMOOTH;
		MonStream[2].u3 = (float)640*(*ScaleFactor);
		MonStream[2].v3 = (float)LOWBANDSMOOTH-deltav;
		MonStream[2].u4 = (float)640*(*ScaleFactor);
		MonStream[2].v4 = (float)LOWBANDSMOOTH+deltav;

		MonStream[3].x = P3x;
		MonStream[3].y = P3y;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(255,255,255,255);
		MonStream[3].u = (float)640*(*ScaleFactor)-deltau;
		MonStream[3].v = HIGHBANDSMOOTH;
		MonStream[3].u2 = (float)640*(*ScaleFactor)+deltau;
		MonStream[3].v2 = HIGHBANDSMOOTH;
		MonStream[3].u3 = (float)640*(*ScaleFactor);
		MonStream[3].v3 = (float)(HIGHBANDSMOOTH-deltav);
		MonStream[3].u4 = (float)640*(*ScaleFactor);
		MonStream[3].v4 = (float)(HIGHBANDSMOOTH+deltav);

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 2, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 3, NULL);

	(*ScaleFactor)=(*ScaleFactor) / 2.0f;
}



void AE_DrawTheRenderedScreenRemanance_OPT(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture, IDirect3DTexture8 *destTexture )
{


	// render the rendered target
	IDirect3DSurface8	*ppSurfaceLevel;
	int alphaFactor;
	float ScaleFactor = 1.0f;

	float XDISP = 0.0f;
	float YDISP = -0.0f;
	
//	ERR_X_Assert(gAE_Params.RemananceFactor>0.f);

	if(gAE_Params.RemananceFactor<=0.f)
		return;

	alphaFactor=(int)(gAE_Params.RemananceFactor*255);
 
	AE_DrawTheRenderedScreenInSmoothTexture_OPT(pst_SD,sourceTexture,destTexture,1,&ScaleFactor);
	AE_DrawTheRenderedScreenInSmoothTexture_OPT(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,3,&ScaleFactor);
//	AE_DrawTheRenderedScreenInSmoothTexture_OPT(pst_SD,pst_SD->pSmoothBufferTexture,destTexture,5,&ScaleFactor);
//	AE_DrawTheRenderedScreenInSmoothTexture_OPT(pst_SD,destTexture,pst_SD->pSmoothBufferTexture,7,&ScaleFactor);

//	ScaleFactor = 0.25f;
//	XDISP*=ScaleFactor;
//	YDISP*=ScaleFactor;

	ERR_X_Assert(ScaleFactor == 0.25);

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

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG1, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	//CARLONE..TO BE TESTED
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,1,(IDirect3DBaseTexture8 *)pst_SD->pSmoothBufferTexture);
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,2,(IDirect3DBaseTexture8 *)pst_SD->pSmoothBufferTexture);

	{
		struct MonStream_ {
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
		MonStream[0].y = (LOWBAND-0.5f);
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = D3DCOLOR_ARGB(alphaFactor,255,255,255);
		MonStream[0].u = 0;
		MonStream[0].v = (float)(LOWBAND);
		MonStream[0].u2 = XDISP;
		MonStream[0].v2 = (float)(LOWBAND+YDISP)*ScaleFactor;
		MonStream[0].u3 = XDISP;
		MonStream[0].v3 = (float)(LOWBAND+YDISP)*ScaleFactor;

		MonStream[1].x = 0;
		MonStream[1].y = HIGHBAND;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = D3DCOLOR_ARGB(alphaFactor,255,255,255);
		MonStream[1].u = 0;
		MonStream[1].v = HIGHBAND;
		MonStream[1].u2 = XDISP;
		MonStream[1].v2 = HIGHBAND+YDISP;
		MonStream[1].u3 = XDISP;
		MonStream[1].v3 = HIGHBAND+YDISP;

		MonStream[2].x = (640-0.5f);
		MonStream[2].y = (LOWBAND-0.5f);
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = D3DCOLOR_ARGB(alphaFactor,255,255,255);
		MonStream[2].u = (float)(640);
		MonStream[2].v = (float)(LOWBAND);
		MonStream[2].u2 = (float)(640+XDISP)*ScaleFactor;
		MonStream[2].v2 = (float)(LOWBAND+YDISP)*ScaleFactor;
		MonStream[2].u3 = (float)(640+XDISP)*ScaleFactor;
		MonStream[2].v3 = (float)(LOWBAND+YDISP)*ScaleFactor;

		MonStream[3].x = (640-0.5f);
		MonStream[3].y = HIGHBAND;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = D3DCOLOR_ARGB(alphaFactor,255,255,255);
		MonStream[3].u = (float)(640);
		MonStream[3].v = HIGHBAND;
		MonStream[3].u2 = (float)(640+XDISP)*ScaleFactor;
		MonStream[3].v2 = (HIGHBAND+YDISP);
		MonStream[3].u3 = (float)(640+XDISP)*ScaleFactor;
		MonStream[3].v3 = (HIGHBAND+YDISP);

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 1, NULL);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice, 2, NULL);
}
