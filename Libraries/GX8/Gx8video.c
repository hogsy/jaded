/*$T Gx8video.c GC! 1.081 01/16/02 15:38:03 */

#include "GDInterface/GDInterface.h"
#include "Gx8video.h"
//#include <d3dx8.h>
#include <xmv.h>
#include "Gx8renderstate.h"
#include "Gx8FileError.h"
#include "..\engine\sources\demodisk\DEModisk.h"

#define	GX8_XMV_FULLSCREEN_VIDEO_TRAILER	"d:\\attractmode.xmv"
#define	GX8_XMV_FULLSCREEN_VIDEO_MAKINGOF	"d:\\makingoff.xmv"

XMVDecoder          *m_pXMVPlayer;    // WMV Player object
LPDIRECT3DTEXTURE8  m_pTarget = NULL;			// Target textures
LPDIRECT3DSURFACE8  m_pTargetSurface = NULL;  // Target Surfaces
RECT				m_SrcRect;
RECT				m_DestRect;
XMVVIDEO_DESC		videoInfo;
bool				videoOk=true;
bool				gFullscreenVideo = false;
 
void Gx8_SetNoVideo(void)
{
	videoOk=false;
}

bool Gx8_InitVideo(Gx8_tdst_SpecificData *_pst_SD, CHAR* strOriginalFilename)
{
	char strFilename[256];
	memset(strFilename,0,sizeof(strFilename));
	if(DEM_GetLaunchPath()!=NULL)
	{
		strcpy(strFilename,DEM_GetLaunchPath());
		strcat(strFilename,&strOriginalFilename[3]);
	}
	else
	{
		strcpy(strFilename,strOriginalFilename);
	}

	if (!XMVDecoder_CreateDecoderForFile( 0, strFilename, &m_pXMVPlayer )==S_OK)
	{
		ERR_X_Warning(0, "Video not found: ", strFilename);
		videoOk=false;
		return false;
	}

	videoOk=true;

    // Get information about the video format
    XMVDecoder_GetVideoDescriptor( m_pXMVPlayer, &videoInfo );

    m_SrcRect.left    = 0;
    m_SrcRect.top     = 0;
    m_SrcRect.right   = videoInfo.Width;
    m_SrcRect.bottom  = videoInfo.Height;

    m_DestRect.left   = 0;
    m_DestRect.top    = 0;
    m_DestRect.right  = 128;
    m_DestRect.bottom = 128;

    IDirect3DDevice8_CreateTexture( _pst_SD->mp_D3DDevice, videoInfo.Width, videoInfo.Height,
                                     1, 0, D3DFMT_YUY2, 0, &m_pTarget );
    IDirect3DTexture8_GetSurfaceLevel( m_pTarget, 0, &m_pTargetSurface );

	return true;
}

void Gx8_CloseVideo(void)
{
	if (!videoOk)
		return;

	XMVDecoder_CloseDecoder(m_pXMVPlayer);

	IDirect3DTexture8_Release(m_pTarget);
	IDirect3DSurface8_Release(m_pTargetSurface);

	m_pTarget = NULL;
	m_pTargetSurface = NULL;
}

void ClearTexture(Gx8_tdst_SpecificData *pst_SD)
{
	{
		struct MonStream_ {
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;
		DWORD	Color = 0;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = 0;
		MonStream[0].y = (m_DestRect.bottom-0.5f);
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = (float)videoInfo.Height;

		MonStream[1].x = 0;
		MonStream[1].y = 0;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = (m_DestRect.right-0.5f);
		MonStream[2].y = (m_DestRect.bottom-0.5f);
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = (float)videoInfo.Width;
		MonStream[2].v = (float)videoInfo.Height;

		MonStream[3].x = (m_DestRect.right-0.5f);
		MonStream[3].y = 0;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = (float)videoInfo.Width;
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	
	}

}

static void DrawTexture(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture )
{
//#define VIDEO_SIZE	128

	DWORD currentCullMode;
	DWORD	StateAddressU;
	DWORD	StateAddressV;
	DWORD	Color = 0xffffffff;

	if(!sourceTexture)
		Color = 0xFF000000;

	//Save the staes before changin it!
	IDirect3DDevice8_GetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  &StateAddressU );
	IDirect3DDevice8_GetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  &StateAddressV );

	IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

	//Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	/*
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	*/

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	if(sourceTexture)
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	else
		IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
 
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
	if(sourceTexture)
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_YUVENABLE, 1);

	IDirect3DDevice8_GetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, &currentCullMode);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE);


	IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)sourceTexture);

	{
		struct MonStream_ {
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			
		MonStream[0].x = m_DestRect.left-0.5f;
		MonStream[0].y = (m_DestRect.bottom-0.5f);
		MonStream[0].z = 0;
		MonStream[0].rhw = 1;
		MonStream[0].Color = Color;
		MonStream[0].u = 0;
		MonStream[0].v = (float)videoInfo.Height;

		MonStream[1].x = m_DestRect.left-0.5f;
		MonStream[1].y = m_DestRect.top-0.5f;
		MonStream[1].z = 0;
		MonStream[1].rhw = 1;
		MonStream[1].Color = Color;
		MonStream[1].u = 0;
		MonStream[1].v = 0;

		MonStream[2].x = (m_DestRect.right-0.5f);
		MonStream[2].y = (m_DestRect.bottom-0.5f);
		MonStream[2].z = 0;
		MonStream[2].rhw = 1;
		MonStream[2].Color = Color;
		MonStream[2].u = (float)videoInfo.Width;
		MonStream[2].v = (float)videoInfo.Height;

		MonStream[3].x = (m_DestRect.right-0.5f);
		MonStream[3].y = m_DestRect.top-0.5f;
		MonStream[3].z = 0;
		MonStream[3].rhw = 1;
		MonStream[3].Color = Color;
		MonStream[3].u = (float)videoInfo.Width;
		MonStream[3].v = 0;

		hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	
	}
	
	if(sourceTexture)
		IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_YUVENABLE, FALSE);
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, currentCullMode);
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU,  StateAddressU );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV,  StateAddressV );

	IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}

void Gx8_PlayVideoInTexture(Gx8_tdst_SpecificData *pst_SD, LPDIRECT3DTEXTURE8 destTexture)
{

	LPDIRECT3DSURFACE8  pRenderTarget=NULL,pZBuffer=NULL;  // Target Surfaces
	LPDIRECT3DSURFACE8  pTargetSurface=NULL;  // Target Surfaces
	XMVRESULT xr = XMV_NOFRAME;
    HRESULT hr;
/*	extern int g_bNoVideo;

	if(g_bNoVideo) return;*/
	if (!videoOk)
		return;

	hr = IDirect3DTexture8_GetSurfaceLevel( destTexture, 0, &pTargetSurface );
    ERR_X_Assert( SUCCEEDED( hr ));

	IDirect3DDevice8_GetRenderTarget( pst_SD->mp_D3DDevice, &pRenderTarget );
	IDirect3DDevice8_GetDepthStencilSurface( pst_SD->mp_D3DDevice, &pZBuffer );

	IDirect3DDevice8_SetRenderTarget
	(
		pst_SD->mp_D3DDevice,
		pTargetSurface,
		NULL
	);

	XMVDecoder_GetNextFrame( m_pXMVPlayer, m_pTargetSurface, &xr, NULL );

	if (xr == XMV_ENDOFFILE || xr == XMV_FAIL)
	{
		XMVDecoder_Reset( m_pXMVPlayer );
		if(xr == XMV_FAIL)
		{
			Gx8_FileError();
		}
	}
	else
	{
		if(m_pTarget && pTargetSurface)
			DrawTexture(pst_SD, m_pTarget);
	}

	IDirect3DDevice8_SetRenderTarget
	(
		pst_SD->mp_D3DDevice,
		pRenderTarget,
		pZBuffer
	);

	if(pTargetSurface)
		IDirect3DSurface8_Release(pTargetSurface);
	if(pRenderTarget)
		IDirect3DSurface8_Release(pRenderTarget);
	if(pZBuffer)
		IDirect3DSurface8_Release(pZBuffer);
}


bool Gx8_InitVideoFullScreen(Gx8_tdst_SpecificData *_pst_SD, CHAR* strOriginalFilename)
{
	bool bAdjustAspectRatio = true;
	char strFilename[256];
	memset(strFilename,0,sizeof(strFilename));
	if(DEM_GetLaunchPath()!=NULL)
	{
		strcpy(strFilename,DEM_GetLaunchPath());
		strcat(strFilename,&strOriginalFilename[3]);
	}
	else
	{
		strcpy(strFilename,strOriginalFilename);
	}

	if (!XMVDecoder_CreateDecoderForFile( 0, strFilename, &m_pXMVPlayer )==S_OK)
	{
		ERR_X_Warning(0, "Video not found: ", strFilename);
		return gFullscreenVideo = false;
	}

	XMVDecoder_EnableAudioStream(m_pXMVPlayer, 0, 0, NULL, NULL );


    // Get information about the video format
    XMVDecoder_GetVideoDescriptor( m_pXMVPlayer, &videoInfo );

    m_SrcRect.left    = 0;
    m_SrcRect.top     = 0;
    m_SrcRect.right   = videoInfo.Width;
    m_SrcRect.bottom  = videoInfo.Height;

    m_DestRect.left   = 0;
    m_DestRect.top    = 0;
    m_DestRect.right  = 640;
    m_DestRect.bottom = 480;

	if(bAdjustAspectRatio)
	{
		int YOffset=0;
		int XOffset=0;
		int DestDimX = (m_DestRect.right - m_DestRect.left);
		int DestDimY = (m_DestRect.bottom - m_DestRect.top);
		int SrcDimX = (m_SrcRect.right - m_SrcRect.left);
		int SrcDimY = (m_SrcRect.bottom - m_SrcRect.top);

		if ( (SrcDimX>DestDimX) ||
			 (SrcDimY>DestDimY)
			)
		{
			YOffset = -(((m_DestRect.bottom - m_DestRect.top) - (m_SrcRect.bottom - m_SrcRect.top)) / 2);
			XOffset = -(((m_DestRect.right - m_DestRect.left) - (m_SrcRect.right - m_SrcRect.left)) / 2);
			YOffset=0;
			XOffset=0;
		}
		else
		{
			YOffset = (((m_DestRect.bottom - m_DestRect.top) - (m_SrcRect.bottom - m_SrcRect.top)) / 2)-20;
			XOffset = (((m_DestRect.right - m_DestRect.left) - (m_SrcRect.right - m_SrcRect.left)) / 2);
		}

		//calc starting position
		m_DestRect.top = m_DestRect.top + YOffset;
		m_DestRect.bottom = m_DestRect.bottom - YOffset;

		m_DestRect.left = m_DestRect.left + XOffset;
		m_DestRect.right = m_DestRect.right + XOffset;

	}

    IDirect3DDevice8_CreateTexture( _pst_SD->mp_D3DDevice, videoInfo.Width, videoInfo.Height,
                                     1, 0, D3DFMT_YUY2, 0, &m_pTarget );
    IDirect3DTexture8_GetSurfaceLevel( m_pTarget, 0, &m_pTargetSurface );

	return gFullscreenVideo = true;
}

void Gx8_PlayVideoFullscreen(Gx8_tdst_SpecificData *pst_SD)
{
	XMVRESULT xr = XMV_NOFRAME;  
    HRESULT hr;

	if (!gFullscreenVideo)
		return;


	hr = XMVDecoder_GetNextFrame( m_pXMVPlayer, m_pTargetSurface, &xr, NULL );

	//Skip the non-displayable frames
	while(xr==XMV_NOFRAME)
		XMVDecoder_GetNextFrame( m_pXMVPlayer, m_pTargetSurface, &xr, NULL );

	if (xr == XMV_ENDOFFILE || xr == XMV_FAIL || hr!=S_OK)
	{
		Gx8_CloseVideoFullScreen();
	}
	else
	{
		DrawTexture(pst_SD, m_pTarget);
	}

}



void Gx8_PlayVideoFullScreen(ULONG ulBK)
{
	Gx8_tdst_SpecificData *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

	//select the right XMV video
	switch (ulBK)
	{
		case 1627465662:
			//BGE trailer
			Gx8_InitVideoFullScreen(pst_SD, GX8_XMV_FULLSCREEN_VIDEO_TRAILER);
		break;
		case 1224905920:
			//Making of
			Gx8_InitVideoFullScreen(pst_SD, GX8_XMV_FULLSCREEN_VIDEO_MAKINGOF);
		break;
		default:
			ERR_X_Warning(0, "Fullscreen video not found: ", ulBK);
	}
}

void Gx8_CloseVideoFullScreen(void)
{
	if (!gFullscreenVideo)
		return;

	XMVDecoder_TerminateImmediately(m_pXMVPlayer);

	gFullscreenVideo = false;

	XMVDecoder_CloseDecoder(m_pXMVPlayer);

	IDirect3DTexture8_Release(m_pTarget);
	IDirect3DSurface8_Release(m_pTargetSurface);

}



