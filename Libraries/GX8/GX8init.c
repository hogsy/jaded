/*$T Gx8init.c GC! 1.081 01/16/02 15:38:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
//#pragma optimize ("",off)
#include "math.h"
#include "Gx8init.h"
#include <xtl.h>
#include <d3d8.h>    
#include <d3dx8.h>
#include "Gx8.h"
#include "Gx8color.h"
//#include <XGMath.h>       
#include "GX8water.h"
#include <assert.h>
#include <xmmintrin.h>

  #include "Gx8FontC.h"
//#include "xmmintrin.h"
#include "GX8/RASter/Gx8_CheatFlags.h"
#include "ENGine/Sources/text/text.h"
#include "INOut/INO.h"

#include "gx8shadowbuffer.h"
#include "Gx8.h"
#include "bink/player.h"

//#include "ENGine/Sources/DEModisk/DEModisk.h"

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BENch/BENch.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "Gx8renderstate.h"
#include "Gx8tex.h"
#include "GEOmetric/GEO_STRIP.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "RASter/Gx8_CheatFlags.h"
#include "AfterFX/Gx8AfterFX_Def.h"
#include "Gx8GPUMon.h"
#include <xmv.h>
#include "Gx8HandleNoPad.h"
#include "xgraphics.h"

//normalmap
//#include "Gx8tex.h"
//#include "INOut/INOjoystick.h"
//extern void _MakeSpecularExponent32b( ULONG* buffer, int width, int height ,int exp);


//Timer
LARGE_INTEGER	D_qwElapsedTime;
LARGE_INTEGER	D_qwElapsedAppTime;
LARGE_INTEGER	D_qwAppTime;
LARGE_INTEGER	D_qwLastTime;
LARGE_INTEGER	D_qwTimerFreq;
float		D_m_fTimerPeriod;
//void BinkPlayer(IDirect3DDevice8 *D3D,int VideoId);
//void BinkPlayer(int VideoIds);

#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
extern BOOL Antialias_E3;
extern BOOL Antialias_PE3;
bool Render3do=FALSE;
extern int NbrObjectClone;
#ifndef _GX8VERTEXBUFFER_H_
#include "Gx8VertexBuffer.h"
#endif

#ifndef _GX8ADDINFO_H_
#include "Gx8AddInfo.h"
#endif

#ifndef __GX8VERTEXSHADERS_H__
#include "Gx8VertexShaders.h"
#endif

#ifndef __GX8PIXELSHADERS_H__
#include "Gx8PixelShaders.h"
#endif

__inline __m128 ubi_mm_hsum_3d_ss(__m128 a)
{
	return _mm_add_ss(a, _mm_add_ss(_mm_shuffle_ps(a, a, 1), _mm_shuffle_ps(a, a, 2)));
}
//#define USE_TANGENTE
/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define GLI_M_SetTextureColorStage(dev, i, arg1, op, arg2) { \
    IDirect3DDevice8_SetTextureStageState(dev, i, D3DTSS_COLOROP, op); \
    IDirect3DDevice8_SetTextureStageState(dev, i, D3DTSS_COLORARG1, arg1); \
    IDirect3DDevice8_SetTextureStageState(dev, i, D3DTSS_COLORARG2, arg2); \
}

#define GLI_M_SetTextureAlphaStage(dev, i, arg1, op, arg2) {\
    IDirect3DDevice8_SetTextureStageState(dev, i, D3DTSS_ALPHAOP, op); \
    IDirect3DDevice8_SetTextureStageState(dev, i, D3DTSS_ALPHAARG1, arg1); \
    IDirect3DDevice8_SetTextureStageState(dev, i, D3DTSS_ALPHAARG2, arg2); \
}


/*$4
 ***********************************************************************************************************************
    constant
 ***********************************************************************************************************************
 */
extern BOOL Normalmap;
BOOL RenderInNormal;
ULONG TimeBeforeVideo;
BOOL STOPVIDEO;

BOOL                    Gx8_gb_Init = 0;
Gx8_tdst_SpecificData   * p_gGx8SpecificData;
int                     iBeginVertexSpriteVB = -1;      // index of the first vertex in BigSpriteVB to be copied to pSPG_VB
int                     iLastVertexSpriteVB = -1;           // index of the last vertex (+1) in BigSpriteVB before restarting the buffer (circular buffer)
bool    bDisableColors=false;
bool    bSmallViewport=false;
float AttractdwTimeout=60*1000;

extern unsigned int g_uiCurrentLight;
extern BOOL GDI_gb_WaveSprite;
extern u_int32 g_iNewLight;
extern int xbINO_gi_ControlMode;
extern bool gFullscreenVideo;
//TV Mode
extern u_int32 TVMode;
//extern u_int32 bTest;

/*DEVICE SETTINGS**************************************************************************************/
DWORD  DeviceSettingsHANDLE;
/******************************************************************************************************/

//////////////////NEW PARTICLE STUFF///////////////////////////////////////////////////////////////////
//HANDLE TO VERTEX SHADER
DWORD shaderHandler;
float *pFloatBuffer;
extern u_int32 g_NewSprite;
////////////////////////////////////////////////////////////////////////////////////////////////////////

/*$4
 ***********************************************************************************************************************
    Private and external function prototype
 ***********************************************************************************************************************
 */

void                    Gx8_SetDCPixelFormat(HDC);
void                    Gx8_SetupRC(Gx8_tdst_SpecificData *);

extern void             LOA_BeginSpeedMode(BIG_KEY _ul_Key);
extern void             LOA_EndSpeedMode(void);


/*$4
 ***********************************************************************************************************************
    Public Function
 ***********************************************************************************************************************
 */

/*=================================================================================================

VIDEO VARIABLES

=================================================================================================
*/

XMVDecoder          *m_pXMVPlayer;    // WMV Player object
LPDIRECT3DTEXTURE8  m_pTarget;          // Target textures
LPDIRECT3DSURFACE8  m_pTargetSurface;  // Target Surfaces
RECT                m_SrcRect;
RECT                m_DestRect;
XMVVIDEO_DESC       videoInfo;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_Viewport(Gx8_tdst_SpecificData *_pst_SD, LONG x, LONG y, LONG w, LONG h)
{
    if(!(_pst_SD->mp_D3DDevice)) return;

    if(x < 0) x = 0;
    if(y < 0) y = 0;
#ifdef WIN32
    if(w + x > (LONG) _pst_SD->mst_D3DPP.BackBufferWidth) w = _pst_SD->mst_D3DPP.BackBufferWidth - x;
    if(h + y > (LONG) _pst_SD->mst_D3DPP.BackBufferHeight) h = _pst_SD->mst_D3DPP.BackBufferHeight - y;
#endif
    _pst_SD->V8.X = x;
    _pst_SD->V8.Y = y;
    _pst_SD->V8.Width = w;
    _pst_SD->V8.Height = h;
    _pst_SD->V8.MinZ = 0.0f;
    _pst_SD->V8.MaxZ = 1.0f;
    IDirect3DDevice8_SetViewport(_pst_SD->mp_D3DDevice, &_pst_SD->V8);
}

/* Aim: Create a device */
extern void Gx8_l_Texture_CreateShadowTextures(Gx8_tdst_SpecificData *);
extern void Gx8_l_Texture_DestroyShadowTextures(Gx8_tdst_SpecificData *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
Gx8_tdst_SpecificData *Gx8_pst_CreateDevice(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = (Gx8_tdst_SpecificData *) MEM_p_Alloc(sizeof(Gx8_tdst_SpecificData));
    L_memset(pst_SD, 0, sizeof(Gx8_tdst_SpecificData));
    p_gGx8SpecificData = pst_SD;

    pst_SD->mp_D3D = Direct3DCreate8(D3D_SDK_VERSION);
    return pst_SD;
}

/*
 =======================================================================================================================
    Aim:    Destroy a device
 =======================================================================================================================
 */
void Gx8_DestroyDevice(void *_pst_SD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = (Gx8_tdst_SpecificData *) _pst_SD;
    if(!pst_SD) return;

    MEM_Free( pst_SD->pBackBufferTexture );
	
/*	if ( Antialias_E3 )
	MEM_Free( pst_SD->pBackBufferTextureTemp );*/

	//MEMORY<<<<<<<<<<<<
	MEM_Free( pst_SD->pZBufferTexture );
    IDirect3D8_Release (pst_SD->mp_D3D);

    MEM_Free(pst_SD);
    p_gGx8SpecificData = NULL;
}


/*
 =======================================================================================================================
    Aim:    Close OpenGL display
 =======================================================================================================================
 */
HRESULT Gx8_l_Close(GDI_tdst_DisplayData *_pst_DD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Gx8_gb_Init = 0;

    pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

    if ( pst_SD->positionVB.pVB )
    {
        // Release Position Circular VB
        IDirect3DVertexBuffer8_Release( pst_SD->positionVB.pVB );
        pst_SD->positionVB.pVB = NULL;
        pst_SD->positionVB.FVF = 0;
        pst_SD->positionVB.stride = 0;
        pst_SD->positionVB.nextIndex = 0;
        pst_SD->positionVB.currIndex = 0;
        pst_SD->positionVB.pCurrGO = NULL;
        pst_SD->positionVB.pCurrSource = NULL;
    }

    if ( pst_SD->normalVB.pVB )
    {
        // Release normal Circular VB
        IDirect3DVertexBuffer8_Release( pst_SD->normalVB.pVB );
        pst_SD->normalVB.pVB = NULL;
        pst_SD->normalVB.FVF = 0;
        pst_SD->normalVB.stride = 0;
        pst_SD->normalVB.nextIndex = 0;
        pst_SD->normalVB.currIndex = 0;
        pst_SD->normalVB.pCurrGO = NULL;
        pst_SD->normalVB.pCurrSource = NULL;
    }

    if ( pst_SD->colorVB.pVB )
    {
        // Release color Circular VB
        IDirect3DVertexBuffer8_Release( pst_SD->colorVB.pVB );
        pst_SD->colorVB.pVB = NULL;
        pst_SD->colorVB.FVF = 0;
        pst_SD->colorVB.stride = 0;
        pst_SD->colorVB.nextIndex = 0;
        pst_SD->colorVB.currIndex = 0;
        pst_SD->colorVB.pCurrGO = NULL;
        pst_SD->colorVB.pCurrSource = NULL;
    }


    if(pst_SD->m_stBigVBData.m_VertexBuffer != NULL)
    {
        IDirect3DVertexBuffer8_Release (pst_SD->m_stBigVBData.m_VertexBuffer);
        pst_SD->m_stBigVBData.m_VertexBuffer = NULL;
    }

    if(pst_SD->m_stBigVBforSpritesData.m_VertexBuffer != NULL)
    {
        IDirect3DVertexBuffer8_Release (pst_SD->m_stBigVBforSpritesData.m_VertexBuffer);
        pst_SD->m_stBigVBforSpritesData.m_VertexBuffer = NULL;
    }

    if(pst_SD->mp_D3DDevice != NULL)
    {
        Gx8_vReleaseVertexShaders();
		Gx8_vReleasePixelShaders();
        Gx8_l_Texture_DestroyShadowTextures(pst_SD);

        Gx8_VertexBuffer_Destroy();

        IDirect3DDevice8_Release (pst_SD->mp_D3DDevice);
        pst_SD->mp_D3DDevice = NULL;
    }

    return D3D_OK;
}

// Gamma correction

static BYTE
_gammaCorrectComponent( int value )
{
//    float newValue = 1.0811f*(float)value - 20.68f;
    float newValue = 0.925f*(float)value + 19.25f;

    if( newValue >= 255.0f )
    {
        return 255;
    }
    else if( newValue <= 0.0f )
    {
        return 0;
    }
    else
    {
        return (BYTE) newValue;
    }
}

static void
_setGammaCorrection( LPDIRECT3DDEVICE8 device )
{
    D3DGAMMARAMP ramp;
    int i;

    IDirect3DDevice8_GetGammaRamp( device, &ramp );
    for( i=0; i<256; i++ )
    {
        ramp.red[i] = _gammaCorrectComponent( ramp.red[i] );
        ramp.green[i] = _gammaCorrectComponent( ramp.green[i] );
        ramp.blue[i] = _gammaCorrectComponent( ramp.blue[i] );
    }
 //   IDirect3DDevice8_SetGammaRamp( device, D3DSGR_IMMEDIATE, &ramp );
}

// create the circular texture for the shadow buffer
void Gx8_CreateCircleTexture(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture)
{
    IDirect3DSurface8   *ppSurfaceLevel;
    D3DLOCKED_RECT rect;
    int i,j;
    u_int *pData;
    int distance;
    MATH_tdst_Vector v1,v2;

    IDirect3DTexture8_GetSurfaceLevel
        (
            sourceTexture,
            0,
            &ppSurfaceLevel
        );

    IDirect3DSurface8_LockRect(ppSurfaceLevel,&rect,NULL,0);

    pData=(u_int *)rect.pBits;

    for(i=0;i<ShadowBuffer_Height;i++)
        for(j=0;j<ShadowBuffer_Width;j++)
        {
            v1.x=(float)i;
            v1.y=(float)j;
            v1.z=0.0f;

            v2.x=(float)ShadowBuffer_Height/2.0f;
            v2.y=(float)ShadowBuffer_Width/2.0f;
            v2.z=0.0f;

            distance=(int)MATH_f_Distance(&v1,&v2);
            
            if (distance>(ShadowBuffer_Height/2))
                *pData=0x00000000;
            else
                *pData=0xffffffff;
            
            pData++;
        }

    IDirect3DSurface8_UnlockRect(ppSurfaceLevel);

    IDirect3DSurface8_Release(ppSurfaceLevel);

}


/*
 =======================================================================================================================
    Aim:    Init GX8 object
 =======================================================================================================================
 */
/*void Gx8_SetLanguage(void)
{
AI_EvalFunc_IoGameLangSet_C(0);
}*/

LONG Gx8_l_Init(HWND _hWnd, GDI_tdst_DisplayData *_pst_DD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    RECT                    st_Rect;
    Gx8_tdst_SpecificData   *pst_SD;
    HRESULT                 hr;
    DWORD                   dw_Behavior;
    DWORD buf = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = /*(Gx8_tdst_SpecificData *)*/ _pst_DD->pv_SpecificData;

    // this initialization will be necessay if we will use manual palette generation
    //Gx8_InitPaletteMemory();

    pst_SD->iFadeAlfa=0;
    pst_SD->bShadowBuffer=false;
    pst_SD->iNOfSBLights=1;
    pst_SD->bRenderingFromLight=false;
    

    pst_SD->RenderScene = FALSE;

    /* Cleanup any objects that might've been created before */
    if(Gx8_l_Close(_pst_DD) != D3D_OK) return E_FAIL;

    Gx8_gb_Init = 1;

    st_Rect.top = st_Rect.left = 0;
    st_Rect.right = 640;
    st_Rect.bottom = 480;
    pst_SD->pst_ProjMatrix = &_pst_DD->st_Camera.st_ProjectionMatrix;
    pst_SD->rcViewportRect = st_Rect;

    //pst_SD->d3ddm = D3DFMT_X8R8G8B8;
    ZeroMemory(&pst_SD->mst_D3DPP, sizeof(pst_SD->mst_D3DPP));
    pst_SD->mst_D3DPP.Windowed = FALSE;
    pst_SD->mst_D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pst_SD->mst_D3DPP.BackBufferFormat = D3DFMT_LIN_A8R8G8B8;

	//temporaire force
    //GDI_gpst_CurDD->st_Device.l_Width = 720;
	//GDI_gpst_CurDD->st_Device.l_Height = 576;


    pst_SD->mst_D3DPP.BackBufferCount = 2;
    pst_SD->mst_D3DPP.hDeviceWindow = 0;
    pst_SD->mst_D3DPP.BackBufferWidth = 640;
    pst_SD->mst_D3DPP.BackBufferHeight = 480;
    pst_SD->mst_D3DPP.EnableAutoDepthStencil = TRUE;
    pst_SD->mst_D3DPP.AutoDepthStencilFormat = D3DFMT_LIN_D24S8;//D3DFMT_LIN_D24S8;
    pst_SD->mst_D3DPP.FullScreen_RefreshRateInHz = 0;
    //#ifdef XBOX_TUNING
    //pst_SD->mst_D3DPP.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    //#else
    pst_SD->mst_D3DPP.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	//pst_SD->mst_D3DPP.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO;
    //#endif

#if defined(_DEBUG)
    pst_SD->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR;
#else
	pst_SD->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

    //Set to LINEAR, 'cause the BackBufferScale make strrange artifacts
    
#endif
  
if ( Antialias_E3 )
	pst_SD->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR;
else
	pst_SD->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;

	
	//CARLONE
	
	//pst_SD->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;
	//Gx8_fnvSetAntiAlias(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR);
	//pst_SD->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

    //pst_SD->mst_D3DPP.Flags = D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
    
	pst_SD->mst_D3DPP.Flags = D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
	//pst_SD->mst_D3DPP.Flags = D3DPRESENTFLAG_INTERLACED; 
	
	dw_Behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;


	#ifdef _DEBUG
	IDirect3D8_SetPushBufferSize( pst_SD->mp_D3D,2*1024*1024, 32*1024 );
	#elif _FINAL_
	IDirect3D8_SetPushBufferSize( pst_SD->mp_D3D,1.1*1024*1024, 32*1024 );
	#else
	IDirect3D8_SetPushBufferSize( pst_SD->mp_D3D,1.0*1024*1024, 32*1024 );
//	IDirect3D8_SetPushBufferSize( pst_SD->mp_D3D,2*1024*1024, 32*1024 );//demo
	#endif

	//IDirect3D8_SetPushBufferSize( pst_SD->mp_D3D, 128*(4 * (1<<10)), (4 * (1<<10)) );

    hr = IDirect3D8_CreateDevice( pst_SD->mp_D3D,
                                  D3DADAPTER_DEFAULT,
                                  D3DDEVTYPE_HAL,
                                  NULL,
                                  dw_Behavior,
                                  &pst_SD->mst_D3DPP,
                                  &pst_SD->mp_D3DDevice );
    
    ERR_X_Assert( SUCCEEDED( hr ));


	

    _setGammaCorrection(pst_SD->mp_D3DDevice);
    Gx8_VertexBuffer_Create( pst_SD->mp_D3DDevice );
    
    Gx8_l_Texture_CreateShadowTextures(pst_SD);

    pst_SD->pBackBufferTexture = MEM_p_Alloc( sizeof(IDirect3DTexture8) );

/*if ( Antialias_E3 )
{
	pst_SD->pBackBufferTextureTemp= MEM_p_Alloc( sizeof(IDirect3DTexture8) );
}*/

    //<<<<<<MEMORY
	pst_SD->pZBufferTexture = MEM_p_Alloc( sizeof(IDirect3DTexture8) );

    AE_InitAETexturesPool(pst_SD);

    Gx8_CreateAfterFXTextures(pst_SD);



    //Gx8_CreateCircleTexture(pst_SD, pst_SD->pCircleTexture);

// ============
/* NOISE
{
int i;
for (i=0;i<2;i++)
{
	IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, 64, 64,
                                     0, 0, D3DFMT_A8R8G8B8, 0, &pst_SD->Tex_Noise );

	{
		IDirect3DSurface8* Gx8_Surface;
		RECT stSrcRect;
		ULONG* dstImage=NULL;
		int a;
		D3DSURFACE_DESC popo;

		IDirect3DTexture8_GetSurfaceLevel(pst_SD->Tex_Noise, 0, &Gx8_Surface);
		IDirect3DTexture8_GetLevelDesc( pst_SD->Tex_Noise,0, &popo );

		dstImage = MEM_p_Alloc( sizeof(ULONG) * 64 * 64 );

		_MakeNoise32b( dstImage, 64, 64 ,0);//16
		
		//IDirect3DTexture8_GetDesc(&Gx8_Surface);
		stSrcRect.left=stSrcRect.top=0;
		stSrcRect.right = 64;
		stSrcRect.bottom = 64;
		D3DXLoadSurfaceFromMemory(Gx8_Surface,NULL,NULL,dstImage,D3DFMT_A8R8G8B8,64*4,NULL,&stSrcRect,
        D3DX_FILTER_NONE,0);
		
		MEM_Free( dstImage );
		IDirect3DSurface8_Release(Gx8_Surface);
	}
}
}
*/
//==========================


if (Normalmap)
{
	// -> Texture temporaire pour essai Normal Map <-
//	D3DXCreateTextureFromFile( pst_SD->mp_D3DDevice, "d:\\stonenormalmap.bmp", &pst_SD->Tex_Normalmap );
//	D3DXCreateTextureFromFile( pst_SD->mp_D3DDevice, "d:\\normal.jpg", &pst_SD->Tex_Normalmap );
//	D3DXCreateTextureFromFile( pst_SD->mp_D3DDevice, "d:\\test_normal_map.bmp", &pst_SD->Tex_Normalmap );
//	D3DXCreateTextureFromFile( pst_SD->mp_D3DDevice, "d:\\stone.bmp", &pst_SD->Tex_Image );
//	D3DXCreateTextureFromFile( pst_SD->mp_D3DDevice, "d:\\VSPower.bmp", &pst_SD->g_pTextureP );
//	D3DXCreateTextureFromFile( pst_SD->mp_D3DDevice, "d:\\exponent.bmp", &pst_SD->g_pTextureP );
	D3DXCreateTextureFromFile( pst_SD->mp_D3DDevice, "d:\\expodot3.tga", &pst_SD->g_pTextureP );
//  pst_SD->g_pTextureP = NULL;
	IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, 64, 64,
                                     0, 0, D3DFMT_A8R8G8B8, 0, &pst_SD->g_pTextureP );

	{
		IDirect3DSurface8* Gx8_Surface;
		RECT stSrcRect;
		ULONG* dstImage=NULL;
		//int a;
		D3DSURFACE_DESC popo;

		IDirect3DTexture8_GetSurfaceLevel(pst_SD->g_pTextureP, 0, &Gx8_Surface);
		IDirect3DTexture8_GetLevelDesc( pst_SD->g_pTextureP,0, &popo );

		dstImage = MEM_p_Alloc( sizeof(ULONG) * popo.Width * popo.Height );

		_MakeSpecularExponent32b( dstImage, popo.Width, popo.Height ,48);//16
		
		//IDirect3DTexture8_GetDesc(&Gx8_Surface);
		stSrcRect.left=stSrcRect.top=0;
		stSrcRect.right = popo.Width;
		stSrcRect.bottom = popo.Height;
		D3DXLoadSurfaceFromMemory(Gx8_Surface,NULL,NULL,dstImage,D3DFMT_A8R8G8B8,popo.Width*4,NULL,&stSrcRect,
        D3DX_FILTER_NONE,0);
		
		MEM_Free( dstImage );
		IDirect3DSurface8_Release(Gx8_Surface);
	}

}

#ifdef GX8_BENCH
    if (XGetVideoStandard()==XC_VIDEO_STANDARD_PAL_I)
        g_fFramePerSecond = 50.f;
    else
        g_fFramePerSecond = 60.f;
#endif

	Gx8_SwitchDisplay();
	//Start Intro
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//BinkPlayer(pst_SD->mp_D3DDevice,0);//INTRO
	//Gx8_SwitchDisplay();

    /* clear the two buffers */
	IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,  0x00000000, 1.0f, 0);
    IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_DEFAULT);
    IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);

	IDirect3DDevice8_CreateVertexBuffer
        (
            pst_SD->mp_D3DDevice,
            Gx8_C_BigVertexBufferVertexCount * sizeof(Gx8VertexFormat),
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
            D3DFVF_VERTEXF,
            D3DPOOL_DEFAULT,
            &pst_SD->m_stBigVBData.m_VertexBuffer
        );
    pst_SD->m_stBigVBData.m_uiWhereToLockFrom = 0;

    IDirect3DDevice8_CreateVertexBuffer
        (
            pst_SD->mp_D3DDevice,
            Gx8_C_BigVertexBufferForSpritesVertexCount * sizeof(Gx8VertexFormatForSprites) * 4,
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
            0, // custom vertex format
            D3DPOOL_DEFAULT,
            &pst_SD->m_stBigVBforSpritesData.m_VertexBuffer
        );
    pst_SD->m_stBigVBforSpritesData.m_uiWhereToLockFrom = 0;

    // Init UV in Big Sprite VB
    {
        Gx8VertexFormatForSprites *p_vData;
        Gx8VertexFormatForSprites *p_vDataEnd;

        IDirect3DVertexBuffer8_Lock(pst_SD->m_stBigVBforSpritesData.m_VertexBuffer, 0, 0, (BYTE **)&p_vData, D3DLOCK_NOOVERWRITE);

        p_vDataEnd = p_vData + ( Gx8_C_BigVertexBufferForSpritesVertexCount * 4 );
        while ( p_vData < p_vDataEnd )
        {
            p_vData[0].fU = 1.0f;
            p_vData[1].fU = 0.0f;
            p_vData[2].fU = 0.0f;
            p_vData[3].fU = 1.0f;

            p_vData[0].fV = 1.0f;
            p_vData[1].fV = 1.0f;
            p_vData[2].fV = 0.0f;
            p_vData[3].fV = 0.0f;

            p_vData += 4;
        }
    }

    {
        Gx8_tdst_CircularVB     * pCircVB;

        // create Position Circular VB
        pCircVB = &pst_SD->positionVB;
        pCircVB->nextIndex = 0;
        pCircVB->currIndex = 0;
        pCircVB->pCurrGO = NULL;
        pCircVB->pCurrSource = NULL;
        pCircVB->FVF = D3DFVF_XYZ;
        pCircVB->stride = D3DXGetFVFVertexSize( pCircVB->FVF );
        IDirect3DDevice8_CreateVertexBuffer( pst_SD->mp_D3DDevice,
                                            CIRCULAR_VB_SIZE * pCircVB->stride,
                                            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                            pCircVB->FVF,
                                            D3DPOOL_DEFAULT,
                                            &pCircVB->pVB );

        // create Normal Circular VB
        pCircVB = &pst_SD->normalVB;
        pCircVB->nextIndex = 0;
        pCircVB->currIndex = 0;
        pCircVB->pCurrGO = NULL;
        pCircVB->pCurrSource = NULL;
        pCircVB->FVF = D3DFVF_NORMAL;
        pCircVB->stride = D3DXGetFVFVertexSize( pCircVB->FVF );
        IDirect3DDevice8_CreateVertexBuffer( pst_SD->mp_D3DDevice,
                                            CIRCULAR_VB_SIZE * pCircVB->stride,
                                            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                            pCircVB->FVF,
                                            D3DPOOL_DEFAULT,
                                            &pCircVB->pVB );

        // create Color Circular VB
        pCircVB = &pst_SD->colorVB;
        pCircVB->nextIndex = 0;
        pCircVB->currIndex = 0;
        pCircVB->pCurrGO = NULL;
        pCircVB->pCurrSource = NULL;
        pCircVB->FVF = D3DFVF_DIFFUSE;
        pCircVB->stride = D3DXGetFVFVertexSize( pCircVB->FVF );
        IDirect3DDevice8_CreateVertexBuffer( pst_SD->mp_D3DDevice,
                                            CIRCULAR_VB_SIZE_COLOR * pCircVB->stride,
                                            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                            pCircVB->FVF,
                                            D3DPOOL_DEFAULT,
                                            &pCircVB->pVB );
/*      {
            void *pColorData;

            IDirect3DVertexBuffer8_Lock( pCircVB->pVB, 0, CIRCULAR_VB_SIZE * pCircVB->stride, &pColorData, 0 );
            memset( pColorData, 0x7f7f7f7f, CIRCULAR_VB_SIZE * pCircVB->stride );
            IDirect3DVertexBuffer8_Unlock( pCircVB->pVB );
        }
*/
    }

    {
        DWORD decl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(1),
            D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ),
            D3DVSD_STREAM(2),
            D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),
            D3DVSD_END()
        };

        IDirect3DDevice8_CreateVertexShader( pst_SD->mp_D3DDevice, decl, NULL, &pst_SD->pVertexShaderWithUV, 0);
    }

    {
        DWORD decl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(1),
            D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ),
            D3DVSD_END()
        };

        IDirect3DDevice8_CreateVertexShader( pst_SD->mp_D3DDevice, decl, NULL, &pst_SD->pVertexShader, 0);
    }



    


    /////////////////NEW FORMAT FOR NORMALS////////////////////////////////////////////////////////////////
    
    {
        DWORD decl[] =
        {

            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(1),
            D3DVSD_REG( D3DVSDE_NORMAL, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(2),
            D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ),
            D3DVSD_STREAM(3),
            D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),    
            D3DVSD_END()
        };

        IDirect3DDevice8_CreateVertexShader( pst_SD->mp_D3DDevice, decl, NULL, &pst_SD->pVertexShaderNormalWithUV, 0);
    }
if (Normalmap)
{
	// yoann 2uv
	{
        DWORD decl[] =
        {

            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(1),
            D3DVSD_REG( D3DVSDE_NORMAL, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(2),
            D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ),
            D3DVSD_STREAM(3),
            D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),    
            D3DVSD_STREAM(4),
            D3DVSD_REG( D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2 ),    
            D3DVSD_END()
        };

        IDirect3DDevice8_CreateVertexShader( pst_SD->mp_D3DDevice, decl, NULL, &pst_SD->pVertexShaderNormalWith2UV, 0);
    }
}
    {
        DWORD decl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(1),
            D3DVSD_REG( D3DVSDE_NORMAL, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(2),
            D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ),
            D3DVSD_END()
        };

        IDirect3DDevice8_CreateVertexShader( pst_SD->mp_D3DDevice, decl, NULL, &pst_SD->pVertexShaderNormal, 0);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////




    /* create the shaders */
    Gx8_vCreateVertexShaders();
	Gx8_vCreatePixelShaders();

    /* Select the pixel format */
    Gx8_SetupRC(pst_SD);
    Gx8_RS_Init(pst_SD);
    GDI_gpst_CurDD = _pst_DD;
    _pst_DD->LastBlendingMode = 0;
    Gx8_SetTextureBlending( (unsigned long)-1l, (unsigned long)-1l, 0);
    Gx8_SetProjectionMatrix(&_pst_DD->st_Camera);
    Gx8_RS_DepthTest( pst_SD, 1 );

    // OPTIMISATION TEST : Z Occlusion Culling
    Gx8_M_RenderState(pst_SD, D3DRS_OCCLUSIONCULLENABLE, TRUE);

    // init visibility tests
    {
        int i;
        for ( i = 0; i < 255; i++)
        {
            IDirect3DDevice8_BeginVisibilityTest( pst_SD->mp_D3DDevice );
            IDirect3DDevice8_EndVisibilityTest( pst_SD->mp_D3DDevice, i );
        }
    }


    //CARLONE shadow buffer initializations
#if USE_SHADOW_BUFFER
    Gx8_InitShadowBufferSystem();
    p_gGx8SpecificData->needSBMatrix=false;
#endif


    //Init vertex shader  for PARTICLE SYSTEM
    Gx8_CreateParticleVertexShader(p_gGx8SpecificData);
    Gx8_CreateParticleVertexBuffer(p_gGx8SpecificData,MAX_PART_NUM);

//#ifdef _DEBUG
	Gx8CreateFont();
//#endif
    
	//Gx8_SwitchDisplay();
	//TimeBeforeVideo=0;
	//P_StartTimer();
	{
		LARGE_INTEGER qwTime;
		QueryPerformanceFrequency( &D_qwTimerFreq );
		D_m_fTimerPeriod = 1.0f / (FLOAT)( D_qwTimerFreq.QuadPart );
		QueryPerformanceCounter( &qwTime );
		D_qwElapsedTime.QuadPart = 0;
		D_qwLastTime.QuadPart    = qwTime.QuadPart;
		D_qwAppTime.QuadPart     = 0;
	}
	
	
	{
			char Loading[32];
		    INO_tden_LanguageId language = (INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage;//INO_e_English;//INO_getGameLanguage();
			
			switch (language)
			{
				case INO_e_English:
				swprintf (Loading,L"...Loading...");break;//
				case INO_e_French:
				swprintf (Loading,L"...Chargement...");break;//
				case INO_e_German:
				swprintf (Loading,L"...Laden...");break;
				case INO_e_Spanish:
				swprintf (Loading,L"...Cargando...");break;//
				case INO_e_Italian:
				swprintf (Loading,L"...Caricamento...");break;
				//case INO_e_Dutch:
				//swprintf (Loading,L"...Loading...");break;
				case INO_e_Swedish:
				swprintf (Loading,L"...Laddar...");break;//
				case INO_e_Danish:
				swprintf (Loading,L"...Indlæser...");break;//
				case INO_e_Finnish:
				swprintf (Loading,L"...Ladataan...");break;//
				case INO_e_Norwegian:
				swprintf (Loading,L"...Lastes...");break;
			}
			WriteString( Loading,
                         250,
                         240,
                         0xffffffff ); // Black

			IDirect3DDevice8_Present(pst_SD->mp_D3DDevice, NULL,NULL,NULL,NULL);
	}
			return D3D_OK;
}

/* Aim: Adapt OpenGL display to window */
extern void Gx8_Texture_ReleaseAll(GDI_tdst_DisplayData *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_SwitchDisplay()
{
//  HRESULT                 hr;
//    RECT					st_Rect;
	Gx8_tdst_SpecificData   *pst_SD;
    int                     w, h;
	DWORD m_dwVideoFlags = XGetVideoFlags();
	static BOOL Init=1;

	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

	pst_SD->mst_D3DPP.Flags = D3DPRESENTFLAG_10X11PIXELASPECTRATIO;// | D3DPRESENTFLAG_INTERLACED;
	w=640;
	h=480;

	/*if ( TVMode==3 ) 
		return;*/
	if ( (m_dwVideoFlags & XC_VIDEO_FLAGS_WIDESCREEN))// && Init) 
	{
		TVMode=3;Init=0;
	}

	else if ( (m_dwVideoFlags &  XC_VIDEO_FLAGS_LETTERBOX))// && Init) 
	{
		TVMode=1;Init=0;
	}
	else// if (Init)
	{
		TVMode=2;Init=0;
	}
/*	if (TVMode<2 )
	{
		w=640;
		h=480;
	}
	
	if (TVMode==2 )
	{
		w=640;
		h=480;
	}*/

	/*if (TVMode>3 && TVMode<7 && (m_dwVideoFlags & XC_VIDEO_FLAGS_HDTV_720p ))
	{
		w=1280;
		h=720;
		pst_SD->mst_D3DPP.Flags |= D3DPRESENTFLAG_PROGRESSIVE;
	}*/

	if ( TVMode==3 || TVMode==5 )
	{
		if (m_dwVideoFlags & XC_VIDEO_FLAGS_WIDESCREEN)
		{
			pst_SD->mst_D3DPP.Flags |= D3DPRESENTFLAG_WIDESCREEN;
//			pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_16over9;
//			pst_DisplayDatas->st_ScreenFormat.ul_Flags &= ~GDI_Cul_SFF_169BlackBand;
		}
		else {TVMode=-1;return;}
	}

	/*if (TVMode==7 )
	{
		w=720;
		h=480;
		pst_SD->mst_D3DPP.Flags = D3DPRESENTFLAG_INTERLACED; 
	}*/

	//pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	//pst_SD->mst_D3DPP.Flags = D3DPRESENTFLAG_INTERLACED ;//D3DPRESENTFLAG_PROGRESSIVE;D3DPRESENTFLAG_INTERLACED;
    //if ( TVMode == 1 ) pst_SD->mst_D3DPP.Flags |= D3DPRESENTFLAG_WIDESCREEN;
	
	//_pst_DD->st_Device.l_Width = w;
    //_pst_DD->st_Device.l_Height = h;
	
/*	st_Rect.top = st_Rect.left = 0;
    st_Rect.right = w;
    st_Rect.bottom = h;
    pst_SD->pst_ProjMatrix = &GDI_gpst_CurDD->st_Camera.st_ProjectionMatrix;
    pst_SD->rcViewportRect = st_Rect;

	pst_SD->mst_D3DPP.BackBufferWidth = w;
    pst_SD->mst_D3DPP.BackBufferHeight = h;

	GDI_gpst_CurDD->st_Device.l_Width = w;
	GDI_gpst_CurDD->st_Device.l_Height = h;do

	Gx8_Viewport(pst_SD, 0, 0, w, h);*/



   // Gx8_RS_Init(pst_SD);
	//if ( TVMode == 2) pst_SD->mst_D3DPP.Flags |= D3DPRESENTFLAG_WIDESCREEN;
	//pst_SD->mst_D3DPP.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO;

	IDirect3DDevice8_Reset (pst_SD->mp_D3DDevice, &pst_SD->mst_D3DPP);
	//Gx8_CreateAfterFXTextures(pst_SD);

}

HRESULT Gx8_l_ReadaptDisplay(HWND _hWnd, GDI_tdst_DisplayData *_pst_DD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    HRESULT                 hr;
    Gx8_tdst_SpecificData   *pst_SD;
    int                     w, h;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

    if(!Gx8_gb_Init)
    {
        hr = Gx8_l_Init(_hWnd ? _hWnd : GX8_M_SD(_pst_DD)->h_Wnd, _pst_DD);
        return hr;
    }

    w = _pst_DD->st_Device.l_Width;
    h = _pst_DD->st_Device.l_Height;

    if(pst_SD->m_stBigVBData.m_VertexBuffer)
        IDirect3DVertexBuffer8_Release(pst_SD->m_stBigVBData.m_VertexBuffer);
    if(pst_SD->m_stBigVBforSpritesData.m_VertexBuffer)
        IDirect3DVertexBuffer8_Release(pst_SD->m_stBigVBforSpritesData.m_VertexBuffer);
/*
	if(pst_SD->VBforSPG2.Spg2VB)
        IDirect3DVertexBuffer8_Release(pst_SD->VBforSPG2.Spg2VB);
		pst_SD->VBforSPG2.Spg2VB = NULL;
*/
    pst_SD->mst_D3DPP.Windowed = 1/*TRUE*/;
    pst_SD->mst_D3DPP.BackBufferFormat = pst_SD->d3ddm.Format;
    pst_SD->mst_D3DPP.BackBufferCount = 2;
    pst_SD->mst_D3DPP.SwapEffect = D3DSWAPEFFECT_FLIP;
    pst_SD->mst_D3DPP.hDeviceWindow = _hWnd;
    pst_SD->mst_D3DPP.BackBufferWidth = w;
    pst_SD->mst_D3DPP.BackBufferHeight = h;

    Gx8_l_Texture_DestroyShadowTextures(pst_SD);

    IDirect3DDevice8_Reset (pst_SD->mp_D3DDevice, &pst_SD->mst_D3DPP);

    Gx8_l_Texture_CreateShadowTextures(pst_SD);

    Gx8_Viewport(pst_SD, 0, 0, w, h);
    Gx8_RS_Init(pst_SD);

	IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0);
    IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_DEFAULT);
    IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0);

    /* Gx8_Viewport( pst_SD, 0, (h - w) / 2, w, w); */
    IDirect3DDevice8_CreateVertexBuffer
        (
            pst_SD->mp_D3DDevice,
            Gx8_C_BigVertexBufferVertexCount * sizeof(Gx8VertexFormat),
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
            D3DFVF_VERTEXF,
            D3DPOOL_DEFAULT,
            &pst_SD->m_stBigVBData.m_VertexBuffer
        );
    pst_SD->m_stBigVBData.m_uiWhereToLockFrom = 0;
    IDirect3DDevice8_CreateVertexBuffer
        (
            pst_SD->mp_D3DDevice,
            Gx8_C_BigVertexBufferForSpritesVertexCount * sizeof(Gx8VertexFormatForSprites) * 3,
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
            0, // custom vertex format
            D3DPOOL_DEFAULT,
            &pst_SD->m_stBigVBforSpritesData.m_VertexBuffer
        );
    pst_SD->m_stBigVBforSpritesData.m_uiWhereToLockFrom = 0;

    GDI_gpst_CurDD->LastBlendingMode = 0;
    Gx8_SetTextureBlending((unsigned long)-1l, (unsigned long)-1l, 0);

    Gx8_SetProjectionMatrix(&_pst_DD->st_Camera);

    return 1;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */

void P_ResetTimer(void)
{
	D_qwAppTime.QuadPart     = 0;
}
float P_GetTime(void)
{
	LARGE_INTEGER qwTime;
	FLOAT fSeconds;

	QueryPerformanceCounter( &qwTime );
	D_qwElapsedTime.QuadPart = qwTime.QuadPart - D_qwLastTime.QuadPart;
	D_qwLastTime.QuadPart    = qwTime.QuadPart;
	D_qwElapsedAppTime.QuadPart = D_qwElapsedTime.QuadPart;

	//if (!gDontIncrementTimer)
	{
	 D_qwAppTime.QuadPart    += D_qwElapsedAppTime.QuadPart;
	}

	 // Convert to floating pt
    fSeconds = (FLOAT)( D_qwAppTime.QuadPart ) * D_m_fTimerPeriod;

	//convert to milliseconds
	return fSeconds * 1000.0f;

}
void Atr_ResetAttract(void)
{
				LARGE_INTEGER qwTime;
				QueryPerformanceFrequency( &D_qwTimerFreq );
				D_m_fTimerPeriod = 1.0f / (FLOAT)( D_qwTimerFreq.QuadPart );
				QueryPerformanceCounter( &qwTime );
				D_qwElapsedTime.QuadPart = 0;
				D_qwLastTime.QuadPart    = qwTime.QuadPart;
				D_qwAppTime.QuadPart     = 0;
}

void Gx8_Flip(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    static bool Intro=1;//PadOk;
	static int FontLoaded=0;
	static BOOL DEMOON=1;//<<<<<<<<<<<<<<<<<
	extern BOOL ENG_gb_ExitApplication;
	extern int ControlMode;
	extern ULONG h_SaveWorldKey;
	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
	Render3do=TRUE;

	_GSP_EndRaster(GSP_NRaster-1);

    _GSP_BeginRaster(47);
    _GSP_EndRaster(19);// do not count raster display in display
#if !defined( _FINAL_ )
    Gx8_RAS_fnvDrawRasters((void *)(pst_SD->mp_D3DDevice));
#endif

	_GSP_BeginRaster(19);
    _GSP_EndRaster(47);
    _GSP_BeginRaster(GSP_NRaster-1);

    if(pst_SD->RenderScene)
    {
        IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
    }
    
    /////////////////////NO PAD STRING///////////////////////////////////////////
	
    //Gx8_HandleNoPad();
    
    /////////////////////TITLESAFEAREA///////////////////////////////////////////
 
	//AE_TITLESAFEAREA(pst_SD->mp_D3DDevice);

    //Scale the BACK BUFFER BEFORE FLIP
    if((xbINO_gi_ControlMode==4) && (!gFullscreenVideo))
    {
        //Gsp_ScaleBackBuffer();//desac
	}

	if ( Antialias_E3 )
	{
		Gx8_StartGPUMon(); //desac  

		//for antialias:
		IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_FINISH);  

		Gx8_StopGPUMon();//desac
	}
	else
	 //without antialias:
	IDirect3DDevice8_Present(pst_SD->mp_D3DDevice, NULL,NULL,NULL,NULL);

    pst_SD->RenderScene = FALSE;
    
    // reset for next frame
    pst_SD->positionVB.pCurrGO = NULL;
    pst_SD->colorVB.pCurrGO = NULL;
    pst_SD->normalVB.pCurrGO = NULL;

#define ATTRACTMODE
#ifdef ATTRACTMODE

#ifndef _XBOXDEMO
	if ((P_GetTime()>AttractdwTimeout && !ENG_gb_ExitApplication && DEMOON && h_SaveWorldKey==0x3d00c456 ) || (Intro) || (ENG_gb_ExitApplication && DEMOON))// && PadOk)
#else
	if ((P_GetTime()>AttractdwTimeout && !ENG_gb_ExitApplication && DEMOON ) || (Intro) || (ENG_gb_ExitApplication && DEMOON))// && PadOk)
#endif
	{	
		void SND_MuteAll(BOOL b_Mute);
		void SND_TrackPauseAll(BOOL b_Stop);
		void SND_Update(MATH_tdst_Matrix *_pst_Camera);
		
			int VideoId=2000;
			SND_MuteAll(TRUE);
			SND_TrackPauseAll(TRUE);
			SND_Update(NULL);

			/*if ( Intro == 1) VideoId=0;
			else if ( Intro == 2) VideoId=3;
			else if ( Intro == 3) VideoId=4;
			else */
			if (!Intro) VideoId=2001;//else/sinon attract mode
			if (ENG_gb_ExitApplication) VideoId=2002;

			Mem_CreateForBink();
			//D3DTexture_Release(pst_SD->pSaveBufferTexture);
			//pst_SD->pSaveBufferTexture=NULL;
			//MEM_Free( pst_SD->pZBufferTexture );
Intro=false;
			if ( Intro )
			{
				BinkPlayer( 2000 );//Intro
				#ifndef _XBOXDEMO
				BinkPlayer( 2003 );//Universal
				BinkPlayer( 2004 );//Wingnut
				#endif
				Intro = 0;
			}
			else
			BinkPlayer( VideoId );

			Mem_RestoreForBink();
			/*IDirect3DDevice8_CreateTexture
			(
				pst_SD->mp_D3DDevice,
				640,//desc.Width,
				480,
				0,
				D3DUSAGE_RENDERTARGET,
				D3DFMT_LIN_A8R8G8B8,
				D3DPOOL_DEFAULT,
				&pst_SD->pSaveBufferTexture
			);
			
			pst_SD->pZBufferTexture = MEM_p_Alloc( sizeof(IDirect3DTexture8) );*/

			SND_MuteAll(FALSE);
			SND_TrackPauseAll(FALSE);
			SND_Update(NULL);
			/*Intro++;
			if (Intro>3) Intro=0;//no more intro*/

			Atr_ResetAttract();

			//SPG2_Free();
	}
#ifndef _XBOXDEMO	
	else if (DEMOON && h_SaveWorldKey!=0x3d00c456) Atr_ResetAttract();
#endif

	//else IDirect3DDevice8_EnableOverlay( pst_SD->mp_D3DDevice,FALSE );
#endif

}

/*
 =======================================================================================================================
    Aim:    Clear buffer
 =======================================================================================================================
 */
void Gx8_Clear(LONG _l_Buffer, ULONG _ul_Color)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    DWORD                   dw_Buffer;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

	//yo test scale back buffer
	//IDirect3DDevice8_SetBackBufferScale(pst_SD->mp_D3DDevice,0.8f,0.8f);

    if(_l_Buffer == GDI_Cl_ZBuffer)
    {
		//il etait desactivé ??
		IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0);
    }
    else
    {

//TiZ: why the color is ALWAYS black?       _ul_Color = 0xff000000;

        //TiZ: Use the right clear color passed by the engine...
        _ul_Color = Gx8_M_ConvertColor((_ul_Color & 0xfefefefe)>>1);//divide by 2 for mul 2x after fx
//		_ul_Color |= 0xff000000;
        
        dw_Buffer = D3DCLEAR_TARGET | ((_l_Buffer & GDI_Cl_ZBuffer) ? (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL) : 0);
        IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, dw_Buffer, _ul_Color, 1.0f, 0);

        if ( !pst_SD->RenderScene )
        {
            IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);
            pst_SD->RenderScene = TRUE;
        }
    }

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_SetGlobalAmbient(struct Gx8_tdst_SpecificData_ *pst_SD, ULONG _ul_Color)
{
    IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_AMBIENT, Gx8_M_ConvertColor(_ul_Color));
}

void Gx8_SetMateriaAmbient(struct Gx8_tdst_SpecificData_ *pst_SD, ULONG _ul_Color)
{
    D3DMATERIAL8            tempMat;
    DWORD color;
    IDirect3DDevice8_GetMaterial(pst_SD->mp_D3DDevice,&tempMat);
    color=Gx8_M_ConvertColor(_ul_Color);
    

    tempMat.Ambient.b=((float)(color&0x000000ff))/255.0f;
    tempMat.Ambient.g=((float)((color&0x0000ff00)>>8))/255.0f;
    tempMat.Ambient.r=((float)((color&0x00ff0000)>>16))/255.0f;
    tempMat.Ambient.a=1.0f;
    

    IDirect3DDevice8_SetMaterial(pst_SD->mp_D3DDevice,&tempMat);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

// this function prepares uv matrixes for shadow buffer (texture stage state 1 & 2)
void Gx8_PrepareShadowBufferUV(MATH_tdst_Matrix *pObjectToCamera)
{
    D3DXMATRIX Projection;
    D3DXMATRIX matViewport;
    MATH_tdst_Matrix matInvObjectCamera;
    MATH_tdst_Matrix matObjectWorld;
    MATH_tdst_Matrix matCameraToWorld;
    MATH_tdst_Matrix matCameraToLight;
    MATH_tdst_Matrix matCameraToLightProjection;
    MATH_tdst_Matrix matWT;

    D3DXMatrixIdentity( &matViewport );

    matViewport._11 = ShadowBuffer_Width * 0.5f;
    matViewport._22 = -(ShadowBuffer_Height * 0.5f);
    matViewport._33 = D3DZ_MAX_D24S8; //D3DZ_MAX_D24S8;    // Maximum depth value possible for the D3DFMT_D24S8 depth buffer.
    matViewport._41 = ShadowBuffer_Width * 0.5f + 0.5f;
    matViewport._42 = ShadowBuffer_Height * 0.5f + 0.5f;

    IDirect3DDevice8_GetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_PROJECTION,
            &Projection
        );
    
    // build the matrix, the original coordinates are in camera space
    MATH_InvertMatrix(&matInvObjectCamera, pObjectToCamera);
    MATH_MulMatrixMatrix(&matObjectWorld, pObjectToCamera, &GDI_gpst_CurDD->st_Camera.st_Matrix);
    MATH_MulMatrixMatrix(&matCameraToWorld, &matInvObjectCamera, &matObjectWorld);
    MATH_MulMatrixMatrix(&matCameraToLight, &matCameraToWorld, &p_gGx8SpecificData->mSBTextureMatrix);
    MATH_MakeOGLMatrix(&matCameraToLight, &matCameraToLight);
    D3DXMatrixMultiply( (D3DMATRIX*)&matCameraToLightProjection, (D3DMATRIX*)&matCameraToLight, &Projection );
    D3DXMatrixMultiply( (D3DMATRIX*)&matWT, (D3DMATRIX*)&matCameraToLightProjection, &matViewport );

    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_TEXTURE1,
            (D3DMATRIX *) &matWT
        );

    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_TEXTURE2,
            (D3DMATRIX *) &matWT
        );
}

void Gx8_SetViewMatrix(MATH_tdst_Matrix *_pst_Matrix)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix    st_OGLMatrix;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(p_gGx8SpecificData->needSBMatrix)
    {
        Gx8_ShadowBufferPrepareMatrix(_pst_Matrix);
    }

    /* allons plutôt mettre la caméra toujours au même endroit, et déplaçons l'objet, nom de nom */
#pragma message("TODO: optimize camera matrix SetTransform calls (=> Gilles?)")
    // If the camera is specified only once a frame, why specify it for every game object ?
    MATH_MakeOGLMatrix(&st_OGLMatrix, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_VIEW,
            (const struct _D3DMATRIX *) &st_OGLMatrix
        );

//  MATH_MulMatrixMatrix(&st_OGLMatrix,&GDI_gpst_CurDD->st_Camera.st_Matrix,GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
    MATH_MulMatrixMatrix(&st_OGLMatrix,_pst_Matrix,&GDI_gpst_CurDD->st_Camera.st_Matrix);
    MATH_MakeOGLMatrix(&st_OGLMatrix, &st_OGLMatrix);

//  MATH_MakeOGLMatrix(&st_OGLMatrix, GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
    p_gGx8SpecificData->eStateFlags &= ~Gx8_C_InvertLocalMatrixSet;
    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_WORLD,
            (const struct _D3DMATRIX *) &st_OGLMatrix
        );

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_Ortho(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    D3DMATRIX   OrthoMatrix;
    /*~~~~~~~~~~~~~~~~~~~~*/

    memset(&OrthoMatrix, 0, sizeof(OrthoMatrix));

    OrthoMatrix._11 = 2.0f / (Right - Left);
    OrthoMatrix._22 = 2.0f / (Top - Bottom);
    OrthoMatrix._33 = -4.0f / (Far - Near);

    OrthoMatrix._14 = -(Right + Left) / (Right - Left);
    OrthoMatrix._24 = -(Top + Bottom) / (Top - Bottom);
    OrthoMatrix._34 = -(Far + Near) / (Far - Near);

    OrthoMatrix._44 = 1.0f;
    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_PROJECTION,
            &OrthoMatrix
        );
}

/*
 =======================================================================================================================
    Aim:    Send View matrix for render
 =======================================================================================================================
 */
void Gx8_SetProjectionMatrix(CAM_tdst_Camera *_pst_Cam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    float                   f, f_ScreenRatio;
    LONG                    w, h, W, H;
    LONG                    x, y;
    ULONG                   Flags;
    float                   l, r, t, b;
    float                   f_near, f_far;
    
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// TV Mode
	int Gx8_ScreenRatio=2;
	{
        pst_SD = GX8_M_SD( GDI_gpst_CurDD );

		
		//if ( (TVMode == 3 || TVMode == 2) && Render3do) GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_16over9;
		// >>>> Force 4/3 <<<<
		//GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_4over3;
		if ( TVMode == 3 && Render3do ) Gx8_ScreenRatio=3;
		//else 
		//Gx8_ScreenRatio=2;
	}
	

	if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)
    {
        D3DXMATRIX projMatrix;
        float wp,hp,Q;
        float fieldOfView;

        pst_SD = GX8_M_SD( GDI_gpst_CurDD );

        f_near = _pst_Cam->f_NearPlane/20.0f;
        f_far = _pst_Cam->f_FarPlane;



        fieldOfView=_pst_Cam->f_FieldOfVision;
        fieldOfView*=1.2f;

        //Set directX projection matrix
        hp=wp= 1.0f /(float)( tan(fieldOfView/2.0f) );
        Q=f_far/(f_far-f_near);

        D3DXMatrixIdentity(&projMatrix);
        projMatrix._11=wp;
        projMatrix._22=hp;
        projMatrix._33=Q;
        projMatrix._34=1.0f;
        projMatrix._43=(-Q*f_near);

        IDirect3DDevice8_SetTransform(pst_SD->mp_D3DDevice,
                                      D3DTS_PROJECTION,
                                      &projMatrix);
    
        if (bSmallViewport)
            Gx8_Viewport(pst_SD, 0, 70, 640, 340);
        else
			Gx8_Viewport(pst_SD,0,0,GDI_gpst_CurDD->st_Device.l_Width,GDI_gpst_CurDD->st_Device.l_Height);

    }
    else
    {

    pst_SD = GX8_M_SD( GDI_gpst_CurDD );

    // set near and far
    f_near = _pst_Cam->f_NearPlane/20.0f;
    f_far = _pst_Cam->f_FarPlane;

    /* Compute screen ratio */
    
	if (bSmallViewport)
        f_ScreenRatio=0.53125f;
    else
    if
    (
        (GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst <= 0)
    ||  (GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst >= GDI_Cul_SRC_Number)
    ) f_ScreenRatio = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
    else
//        f_ScreenRatio = GDI_gaf_ScreenRation[GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst];
        f_ScreenRatio = GDI_gaf_ScreenRation[Gx8_ScreenRatio];

    f = _pst_Cam->f_YoverX * GDI_gpst_CurDD->st_ScreenFormat.f_PixelYoverX * f_ScreenRatio;
    Flags = GDI_gpst_CurDD->st_ScreenFormat.ul_Flags;

    /* Compute height and width of screen */
    w = GDI_gpst_CurDD->st_Device.l_Width;
	h = GDI_gpst_CurDD->st_Device.l_Height;

    if(Flags & GDI_Cul_SFF_ReferenceIsY)
    {
        H = h;
        W = (LONG) (h / f);

        if(((Flags & GDI_Cul_SFF_CropToWindow) && (W > w)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (W < w)))
        {
            H = (LONG) (w * f);
            W = w;
        }
    }
    else
    {
        H = (LONG) (w * f);
        W = w;

        if(((Flags & GDI_Cul_SFF_CropToWindow) && (H > h)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (H < h)))
        {
            W = (LONG) (h / f);
            H = h;
        }
    }

    if(_pst_Cam->f_ViewportWidth == 0)
    {
        _pst_Cam->f_ViewportWidth = 1.0f;
        _pst_Cam->f_ViewportHeight = 1.0f;
        _pst_Cam->f_ViewportLeft = 0.0f;
        _pst_Cam->f_ViewportTop = 0.0f;
    }

    x = (int) (_pst_Cam->f_ViewportLeft * W + ((w - W) / 2));
    y = (int) (_pst_Cam->f_ViewportTop * H + ((h - H) / 2));
    h = (int) (_pst_Cam->f_ViewportHeight * H);
    w = (int) (_pst_Cam->f_ViewportWidth * W);

    _pst_Cam->l_ViewportRealLeft = x;
    _pst_Cam->l_ViewportRealTop = y;

    if ( _pst_Cam->ul_Flags & CAM_Cul_Flags_Ortho )
    {
        D3DXMATRIX offsetMatrix;
        D3DXMATRIX orthoMatrix;
        float offsetX = 0.5f / w; // to place the position exactly at pixel center 
        float offsetY = (0.5f / w); // to place the position exactly at pixel center  

        D3DXMatrixTranslation( &offsetMatrix, offsetX, offsetY, 0.0f );
        
		if (_pst_Cam->ul_Flags & CAM_Cul_Flags_OrthoYInvert)
		// -- Yoann --
//		D3DXMatrixOrthoOffCenterRH( &orthoMatrix, -0.1f/3, 0.1f/3, 0.1f/3, -0.1f/3, -1, 1 );
		D3DXMatrixOrthoOffCenterRH( &orthoMatrix, 0, 1, 1, 0, -1, 1 );
		else
		D3DXMatrixOrthoOffCenterRH( &orthoMatrix, 0.0f, (float) w, 0.0f, (float) h, 0.0f, 1.0f );

        D3DXMatrixMultiply( (D3DXMATRIX *) pst_SD->pst_ProjMatrix, &orthoMatrix, &offsetMatrix );
        IDirect3DDevice8_SetTransform( pst_SD->mp_D3DDevice, D3DTS_PROJECTION, (const D3DMATRIX *) pst_SD->pst_ProjMatrix );

        return;
    }

    if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        f = 1.0f / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
        MATH_SetIdentityMatrix(pst_SD->pst_ProjMatrix);

        if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
        {
          pst_SD->pst_ProjMatrix->Ix = f * ( f_ScreenRatio * 1.073f );  // ->before 1.1 :PIXEL ASPECT RATIO 11:10
          pst_SD->pst_ProjMatrix->Jy = 4;//-f;
        }
        else
        {
            pst_SD->pst_ProjMatrix->Ix = f;
            pst_SD->pst_ProjMatrix->Jy = -f / ( f_ScreenRatio * 1.073f ); // PIXEL ASPECT RATIO 11:10
        }

        pst_SD->pst_ProjMatrix->Kz = f_far / (f_far - f_near);
        pst_SD->pst_ProjMatrix->T.z = -pst_SD->pst_ProjMatrix->Kz * f_near;
        pst_SD->pst_ProjMatrix->Sz = 1.0f;
        pst_SD->pst_ProjMatrix->w = 0.0f;

        IDirect3DDevice8_SetTransform
            (
                pst_SD->mp_D3DDevice,
                D3DTS_PROJECTION,
                (const struct _D3DMATRIX *) pst_SD->pst_ProjMatrix
            );

    }
    else
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        float   f_IsoFactorZoom, f_Scale;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        MATH_SetIdentityMatrix(pst_SD->pst_ProjMatrix);

        f_IsoFactorZoom = _pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom;
        f_Scale = f_IsoFactorZoom;

        if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
        {
            f = 1.0f / GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;

            if(_pst_Cam->f_IsoFactor == 0)
            {
                Gx8_Ortho((1.0f - f) / 2.f, (1.f + f) / 2.f, 0.f, 1.f, -_pst_Cam->f_NearPlane, -_pst_Cam->f_FarPlane);
            }
            else
            {
                l = f_Scale * (-f);
                r = f_Scale * (f);
                b = f_Scale * -1.f;
                t = f_Scale * 1.f;

                Gx8_Ortho(l, r, b, t, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
            }
        }
        else
        {
            f = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
            if(_pst_Cam->f_IsoFactor == 0.f)
            {
                Gx8_Ortho(0.f, 1.f, (1.f - f) / 2.f, (1.f + f) / 2.f, -_pst_Cam->f_NearPlane, -_pst_Cam->f_FarPlane);
            }
            else
            {
                t = f_Scale * (-f);
                b = f_Scale * (f);
                l = f_Scale * -1.f;
                r = f_Scale * 1.f;

                Gx8_Ortho(l, r, b, t, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
            }
        }
    }

    if (bSmallViewport)
        Gx8_Viewport(pst_SD, 0, 70, 640, 340);
    else
        Gx8_Viewport(pst_SD, x, y, w, h);
    }
}

void Gx8_fnvSetAntiAlias(int iSet)
{
//	if ( Antialias_E3 ==1 ) Antialias_E3 =0;
//	else Antialias_E3 =0;

	Antialias_PE3*=-1;
	if (Antialias_PE3)
	p_gGx8SpecificData->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR;
	else
	p_gGx8SpecificData->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;

	IDirect3DDevice8_Reset(p_gGx8SpecificData->mp_D3DDevice, &p_gGx8SpecificData->mst_D3DPP );

/* Temp deactive
	int i;
	D3DMULTISAMPLE_TYPE MultiType;

	switch (iSet)
    {
	case 0:
        MultiType = D3DMULTISAMPLE_NONE;
        break;
    case 1:
        MultiType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
        break;
    case 2:
        MultiType = D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR ;
        break;
    case 3:
        MultiType = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN ;
        break;
    case 4:
        MultiType = D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN  ;
        break;
    default:
        assert( iSet < 0 );
        assert( iSet > 4 ); // either one of this is false.
        MultiType = 0;      // never executed, just shut up the compiler.
        break;
    
	}
    
	if (p_gGx8SpecificData->mst_D3DPP.MultiSampleType != MultiType)
    {
        p_gGx8SpecificData->mst_D3DPP.MultiSampleType = MultiType;
        IDirect3DDevice8_Reset(p_gGx8SpecificData->mp_D3DDevice, &p_gGx8SpecificData->mst_D3DPP );

        IDirect3DTexture8_Release(p_gGx8SpecificData->pTmpFrameBufferTexture);
		IDirect3DTexture8_Release(p_gGx8SpecificData->pSaveBufferTexture);//yoann extension

//      IDirect3DTexture8_Release(p_gGx8SpecificData->pShadowBufferColorTexture);
        IDirect3DTexture8_Release(p_gGx8SpecificData->pShadowBufferDepthTexture);
        for(i=0;i<ShadowBuffer_MaxLights;i++)
            IDirect3DTexture8_Release(p_gGx8SpecificData->pShadowBufferSpot[i]);
        IDirect3DTexture8_Release(p_gGx8SpecificData->pCircleTexture);

        Gx8_CreateAfterFXTextures(p_gGx8SpecificData);

        Gx8_CreateCircleTexture(p_gGx8SpecificData, p_gGx8SpecificData->pCircleTexture);

    }
*/
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_SetTextureTarget(ULONG Num, ULONG MustBeCleared)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    IDirect3DSurface8   *ppSurfaceLevel;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(Num != 0xffffffff)
    {
        if(Num>=MaxShadowTexture)
        {
            ERR_X_Assert("Possible crash: requested a number > MaxShadowTexture");
            return;
        }

        /* Select Texture as frame buf frame buffer */
        if((p_gGx8SpecificData->pBackBuffer_SAVE == NULL) && (p_gGx8SpecificData->pZBuffer_SAVE == NULL))
        {
            IDirect3DDevice8_GetRenderTarget
                (
                    p_gGx8SpecificData->mp_D3DDevice,
                    &p_gGx8SpecificData->pBackBuffer_SAVE
                );
            IDirect3DDevice8_GetDepthStencilSurface
                (
                    p_gGx8SpecificData->mp_D3DDevice,
                    &p_gGx8SpecificData->pZBuffer_SAVE
                );
        }

        IDirect3DTexture8_GetSurfaceLevel
            (
                p_gGx8SpecificData->dul_SDW_Texture[Num],
                0,
                &ppSurfaceLevel
            );
        IDirect3DDevice8_SetRenderTarget
            (
                p_gGx8SpecificData->mp_D3DDevice,
                ppSurfaceLevel,
                NULL
            );

        if(ppSurfaceLevel)
            IDirect3DSurface8_Release(ppSurfaceLevel);

        if(MustBeCleared)
        {
            IDirect3DDevice8_Clear
                (
                    p_gGx8SpecificData->mp_D3DDevice,
                    0,
                    NULL,
                    D3DCLEAR_TARGET,
                    0,//0xffffffff Test vimont
                    1.0f,
                    0
                );
        }
        {
            /*~~~~~~~~~~~~~~~~~~*/
            D3DVIEWPORT8    V8SHD;
            /*~~~~~~~~~~~~~~~~~~*/

            V8SHD.X = 1;
            V8SHD.Y = 1;
            V8SHD.Width = ShadowTextureSize - 2;
            V8SHD.Height = ShadowTextureSize - 2;
            V8SHD.MinZ = 0.0f;
            V8SHD.MaxZ = 1.0f;
            IDirect3DDevice8_SetViewport(p_gGx8SpecificData->mp_D3DDevice, &V8SHD);
        }

        Gx8_RS_CullFace(p_gGx8SpecificData, GX8_C_CULLFACE_OFF);
    }
    else if((p_gGx8SpecificData->pBackBuffer_SAVE != NULL) && (p_gGx8SpecificData->pZBuffer_SAVE != NULL))
        {
            /* restore original frame buffer */
            IDirect3DDevice8_SetRenderTarget
                (
                    p_gGx8SpecificData->mp_D3DDevice,
                    p_gGx8SpecificData->pBackBuffer_SAVE,
                    p_gGx8SpecificData->pZBuffer_SAVE
                );
            IDirect3DSurface8_Release(p_gGx8SpecificData->pBackBuffer_SAVE);
            IDirect3DSurface8_Release(p_gGx8SpecificData->pZBuffer_SAVE);
            p_gGx8SpecificData->pBackBuffer_SAVE = p_gGx8SpecificData->pZBuffer_SAVE = NULL;
            IDirect3DDevice8_SetViewport
                (
                    p_gGx8SpecificData->mp_D3DDevice,
                    &p_gGx8SpecificData->V8
                );
            IDirect3DDevice8_SetTransform
                (
                    p_gGx8SpecificData->mp_D3DDevice,
                    D3DTS_PROJECTION,
                    (const struct _D3DMATRIX *) &p_gGx8SpecificData->SavedProjection
                );
        }
    else
    {
        //Unknown state...
        if(p_gGx8SpecificData->pBackBuffer_SAVE)
            IDirect3DSurface8_Release(p_gGx8SpecificData->pBackBuffer_SAVE);
        if(p_gGx8SpecificData->pZBuffer_SAVE)
            IDirect3DSurface8_Release(p_gGx8SpecificData->pZBuffer_SAVE);

        p_gGx8SpecificData->pZBuffer_SAVE = p_gGx8SpecificData->pBackBuffer_SAVE = NULL;

        ERR_X_Warning(0,"Set texture target problem!",0);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void Gx8_SetViewMatrix_SDW(MATH_tdst_Matrix *pst_Matrix)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix    Identity;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p_gGx8SpecificData->eStateFlags &= ~Gx8_C_InvertLocalMatrixSet;
    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_WORLD,
            (const struct _D3DMATRIX *)&MATH_gst_IdentityMatrix
        );

    IDirect3DDevice8_GetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_PROJECTION,
            &p_gGx8SpecificData->SavedProjection
        );
    MATH_SetIdentityMatrix(&Identity);
    //Identity.T.z = 0.5f;
    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_PROJECTION,
            (const struct _D3DMATRIX *) &Identity
        );
    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_VIEW,
            (const struct _D3DMATRIX *) pst_Matrix
        );
}

/*
 =======================================================================================================================
    Aim:    Draw indexed triangles
 =======================================================================================================================
 */
#define Gx8_SetColorRGBA(a) \
    if(pst_Color) \
    { \
        ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
        ulOGLSetCol ^= ulOGLSetCol_XOr; \
        if(pst_Alpha) \
        { \
            ulOGLSetCol &= 0x00ffffff; \
            ulOGLSetCol |= pst_Alpha[a]; \
        } \
        pVertexBuffer->Color = Gx8_M_ConvertColor(ulOGLSetCol); \
    } \
    else if(pst_Alpha) \
    { \
        ulOGLSetCol &= 0x00ffffff; \
        ulOGLSetCol |= pst_Alpha[a]; \
        pVertexBuffer->Color = Gx8_M_ConvertColor(ulOGLSetCol); \
    }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void *pst_GlobalsWaterParams;

LONG Gx8_l_DrawElementIndexedTriangles
(
    GEO_tdst_ElementIndexedTriangles    *_pst_Element,
    GEO_Vertex                          *_pst_Point,
    GEO_tdst_UV                         *_pst_UV,
    ULONG                               ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_IndexedTriangle    *t, *tend;
    ULONG                       *pst_Color, *pst_Alpha;
    ULONG                       TNum, ulOGLSetCol, ulOGLSetCol_XOr, ulOGLSetCol_Or, ulAmbient;
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD( GDI_gpst_CurDD );
    GEO_tdst_Object             * pObject;

    static bool bSingleColor_OPT=false;  //this will enable the trick of the single color for all the triangles
    static bool bMultipleColor_OPT=false;
	//extern bTest;
	//bSingleColor_OPT=bTest;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _GSP_BeginRaster(45);

	TNum = _pst_Element->l_NbTriangles;
    if(!TNum) return 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////
    pObject = GDI_gpst_CurDD->pst_CurrentGeo;
    
    // ANTI-BUG for pre-submission version
    if (pObject == NULL) 
        return 0;
    //ERR_X_Assert( pObject != NULL );
    
	if ( ( pObject->pIB ) && ( _pst_Element->pst_StripData ) )
    {
        ULONG                       * pColorSource;
        ULONG                       * pColorData;
        Gx8VertexComponent_Position * pPointData;
        Gx8VertexComponent_Normal   * pNormalData;
        IDirect3DVertexBuffer8      tmpVBpos;
        IDirect3DVertexBuffer8      tmpVBNorm;
        IDirect3DVertexBuffer8      tmpVBcol;
        UINT    drawStripCounter;
        GEO_tdst_StripData  * pStripData = _pst_Element->pst_StripData;
        


        //Dont draw the sky in Z buffer
//      if(Gx8_IsSky())
        {
//TiZ: this will make problems with renderstate. PLEASE use the right calls!!!      IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice,D3DRS_ZWRITEENABLE,FALSE);
//          Gx8_RS_DepthMask(pst_SD,0);
        }
        
        
        // Prepare Position Stream
        if ( _pst_Point == pObject->dst_Point )
        {
            IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, pObject->pVB_Position, sizeof(Gx8VertexComponent_Position) );
        }
        else
        {
            if ( pst_SD->positionVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject )
            {
                if ( pst_SD->positionVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE )
                {
                    pst_SD->positionVB.currIndex = 0;
                    pst_SD->positionVB.nextIndex = pObject->l_NbPoints;
                }
                else
                {
                    pst_SD->positionVB.currIndex = pst_SD->positionVB.nextIndex;
                    pst_SD->positionVB.nextIndex += pObject->l_NbPoints;
                }
                pst_SD->positionVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
                IDirect3DVertexBuffer8_Lock( pst_SD->positionVB.pVB, pst_SD->positionVB.currIndex * pst_SD->positionVB.stride, pObject->l_NbPoints * pst_SD->positionVB.stride, (BYTE **) &pPointData, D3DLOCK_NOOVERWRITE );   // D3DLOCK_NOOVERWRITE
                memcpy( pPointData, _pst_Point, pObject->l_NbPoints * sizeof(Gx8VertexComponent_Position) );
                IDirect3DVertexBuffer8_Unlock( pst_SD->positionVB.pVB );
            }
            else
            {
                IDirect3DVertexBuffer8_Lock( pst_SD->positionVB.pVB, pst_SD->positionVB.currIndex * pst_SD->positionVB.stride, pObject->l_NbPoints * pst_SD->positionVB.stride, (BYTE **) &pPointData, D3DLOCK_READONLY );  // D3DLOCK_NOOVERWRITE
                IDirect3DVertexBuffer8_Unlock( pst_SD->positionVB.pVB );
            }

            XGSetVertexBufferHeader( 0, 0, 0, 0, &tmpVBpos, 0 );
            IDirect3DVertexBuffer8_Register( &tmpVBpos, pPointData );

            if ( FAILED( IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, &tmpVBpos, pst_SD->positionVB.stride ) ) )
            {
                assert(FALSE); // failed setting position stream
            }
        }



        ///NEW prepare normals Stream ()
        if(pObject->dst_PointNormal)
        {
            //ATTENTION...THE SAME CHECK AS FOR POSITION IS USED FOR NORMALS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if ( _pst_Point == pObject->dst_Point )
            {
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, pObject->pVB_Normal, sizeof(Gx8VertexComponent_Normal) );
            }
            else
            {
                if ( pst_SD->normalVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject )
                {
                    if ( pst_SD->normalVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE )
                    {
                        pst_SD->normalVB.currIndex = 0;
                        pst_SD->normalVB.nextIndex = pObject->l_NbPoints;
                    }
                    else
                    {
                        pst_SD->normalVB.currIndex = pst_SD->normalVB.nextIndex;
                        pst_SD->normalVB.nextIndex += pObject->l_NbPoints;
                    }
                    pst_SD->normalVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
                    IDirect3DVertexBuffer8_Lock( pst_SD->normalVB.pVB, pst_SD->normalVB.currIndex * pst_SD->normalVB.stride, pObject->l_NbPoints * pst_SD->normalVB.stride, (BYTE **) &pNormalData, D3DLOCK_NOOVERWRITE );  // D3DLOCK_NOOVERWRITE
                

                    //To be check

                    memcpy( pNormalData, pObject->dst_PointNormal, pObject->l_NbPoints * sizeof(Gx8VertexComponent_Normal) );
                    IDirect3DVertexBuffer8_Unlock( pst_SD->normalVB.pVB );
                }
                else
                {
                    IDirect3DVertexBuffer8_Lock( pst_SD->normalVB.pVB, pst_SD->normalVB.currIndex * pst_SD->normalVB.stride, pObject->l_NbPoints * pst_SD->normalVB.stride, (BYTE **) &pNormalData, D3DLOCK_READONLY ); // D3DLOCK_NOOVERWRITE
                    IDirect3DVertexBuffer8_Unlock( pst_SD->normalVB.pVB );
                }

                XGSetVertexBufferHeader( 0, 0, 0, 0, &tmpVBNorm, 0 );
                IDirect3DVertexBuffer8_Register( &tmpVBNorm, pNormalData );

                if ( FAILED( IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, &tmpVBNorm, pst_SD->normalVB.stride ) ) )
                {
                    assert(FALSE); // failed setting position stream
                }
            

            }//New object is used

        
        }//If normal
        else
        {
            //No normal....no HW lighting
            LIGHT_TurnOffObjectLighting();
        }


        // Prepare Color Stream
        pColorSource = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
        ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
        ulOGLSetCol_Or = pst_SD->ulColorOr;
        ulAmbient = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient;

        if ( ( pst_SD->colorVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject ) ||
            ( pst_SD->colorVB.pCurrSource != pColorSource ) ||
            ( pst_SD->colorVB.currData1 != ulOGLSetCol_XOr ) ||
            ( pst_SD->colorVB.currData2 != ulOGLSetCol_Or ) ||
            ( pst_SD->colorVB.ambient != ulAmbient ) )
        {

            /*if(pst_SD->colorVB.nextIndex + pObject->l_NbPoints>=(CIRCULAR_VB_SIZE/2))
            {
                static DWORD fence=-1;      
                if(fence==-1) //First time
                {
                    fence=IDirect3DDevice8_InsertFence(pst_SD->mp_D3DDevice);
                }
                else
                {
                    if(IDirect3DDevice8_IsFencePending(pst_SD->mp_D3DDevice,fence))
                    {
                        IDirect3DDevice8_BlockOnFence(pst_SD->mp_D3DDevice,fence);
                    }
                    fence=IDirect3DDevice8_InsertFence(pst_SD->mp_D3DDevice);
                }

            }
            */
            


            if ( pst_SD->colorVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE_COLOR )
            {
                pst_SD->colorVB.currIndex = 0;
                pst_SD->colorVB.nextIndex = pObject->l_NbPoints;
            }
            else
            {
                pst_SD->colorVB.currIndex = pst_SD->colorVB.nextIndex;
                pst_SD->colorVB.nextIndex += pObject->l_NbPoints;
            }
            pst_SD->colorVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
            pst_SD->colorVB.pCurrSource = pColorSource;
            pst_SD->colorVB.currData1 = ulOGLSetCol_XOr;
            pst_SD->colorVB.currData2 = ulOGLSetCol_Or;
            pst_SD->colorVB.ambient = ulAmbient;

            if( !pColorSource )
            { 
                if(bSingleColor_OPT)    //Go for the optimized one
                {   
                    //TiZ note:
                    //since the color is the same for the whole element
                    //just set the diffuse to that color.
                    //This prevent copying all the colors to the array.
                    pColorData=0;


                    ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
                    ulOGLSetCol ^= ulOGLSetCol_XOr;
                    ulOGLSetCol = Gx8_M_ConvertColor(ulOGLSetCol);

                    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_TEXTUREFACTOR,ulOGLSetCol);
                    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
                    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG0,D3DTA_TFACTOR);
                }
                else
                {
                    ULONG   * pDest;
                    ULONG   * pEnd;

                    IDirect3DVertexBuffer8_Lock( pst_SD->colorVB.pVB, pst_SD->colorVB.currIndex * pst_SD->colorVB.stride, pObject->l_NbPoints * pst_SD->colorVB.stride, (BYTE **)&pColorData, D3DLOCK_NOOVERWRITE );    // D3DLOCK_NOOVERWRITE

                    pDest = (ULONG *) pColorData;
                    pEnd = pDest + pObject->l_NbPoints;


                    ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
                    ulOGLSetCol ^= ulOGLSetCol_XOr;
                    ulOGLSetCol = Gx8_M_ConvertColor(ulOGLSetCol);

                    for ( ; pDest < pEnd; pDest++ )
                        * pDest = ulOGLSetCol;

                    IDirect3DVertexBuffer8_Unlock( pst_SD->colorVB.pVB );
                    
                }
            }
            else
            {
                {
                    ULONG   * pDest;
                    ULONG   * pSource = (ULONG *) pColorSource;
                    ULONG   * pEnd;
                    ULONG   col;

                    IDirect3DVertexBuffer8_Lock( pst_SD->colorVB.pVB, pst_SD->colorVB.currIndex * pst_SD->colorVB.stride, pObject->l_NbPoints * pst_SD->colorVB.stride, (BYTE **)&pColorData, D3DLOCK_NOOVERWRITE );    // D3DLOCK_NOOVERWRITE
                    pDest = (ULONG *) pColorData;
                    pEnd = pDest + pObject->l_NbPoints;

                    for ( ; pDest < pEnd; pDest++ )
                    {
                        col = ( ( ( * pSource ) | ulOGLSetCol_Or ) ^ ulOGLSetCol_XOr );
                        * pDest = Gx8_M_ConvertColor( col );
                        pSource++;
                    }
                    IDirect3DVertexBuffer8_Unlock( pst_SD->colorVB.pVB );
                } 
            }

        }
        else 
        {
            IDirect3DVertexBuffer8_Lock( pst_SD->colorVB.pVB, pst_SD->colorVB.currIndex * pst_SD->colorVB.stride, pObject->l_NbPoints * pst_SD->colorVB.stride, (BYTE **)&pColorData, D3DLOCK_READONLY );   // D3DLOCK_NOOVERWRITE
            IDirect3DVertexBuffer8_Unlock( pst_SD->colorVB.pVB );
        }


        if(pColorData)
        {
            XGSetVertexBufferHeader( 0, 0, 0, 0, &tmpVBcol, 0 );
            IDirect3DVertexBuffer8_Register( &tmpVBcol, pColorData );
        }


        if(!pObject->dst_PointNormal)
        {
            if(!pColorData)
            {
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, NULL, 0 );
            }
            else
            {
                if ( FAILED( IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, &tmpVBcol, pst_SD->colorVB.stride ) ) )
                {
                    assert(FALSE); // failed setting color stream
                }
            }
        }
        else
        {
            if(!pColorData)
            {
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
            }
            else
            {
                if ( FAILED( IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, &tmpVBcol, pst_SD->colorVB.stride ) ) )
                {
                    assert(FALSE); // failed setting color stream
                }
            }

        }

if (Normalmap && RenderInNormal)
{
		if(pObject->CotangantesU)
        {
 //           if ( _pst_Point == pObject->dst_Point )
            {
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 5, pObject->pVB_Tangente, sizeof(D3DVECTOR4) );
            }
   /*         else
            {
                if ( pst_SD->normalVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject )
                {
                    if ( pst_SD->normalVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE )
                    {
                        pst_SD->normalVB.currIndex = 0;
                        pst_SD->normalVB.nextIndex = pObject->l_NbPoints;
                    }
                    else
                    {
                        pst_SD->normalVB.currIndex = pst_SD->normalVB.nextIndex;
                        pst_SD->normalVB.nextIndex += pObject->l_NbPoints;
                    }
                    pst_SD->normalVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
                    IDirect3DVertexBuffer8_Lock( pst_SD->normalVB.pVB, pst_SD->normalVB.currIndex * pst_SD->normalVB.stride, pObject->l_NbPoints * pst_SD->normalVB.stride, (BYTE **) &pNormalData, D3DLOCK_NOOVERWRITE );  // D3DLOCK_NOOVERWRITE
                

                    //To be check

                    memcpy( pNormalData, pObject->dst_PointNormal, pObject->l_NbPoints * sizeof(Gx8VertexComponent_Normal) );
                    IDirect3DVertexBuffer8_Unlock( pst_SD->normalVB.pVB );
                }
                else
                {
                    IDirect3DVertexBuffer8_Lock( pst_SD->normalVB.pVB, pst_SD->normalVB.currIndex * pst_SD->normalVB.stride, pObject->l_NbPoints * pst_SD->normalVB.stride, (BYTE **) &pNormalData, D3DLOCK_READONLY ); // D3DLOCK_NOOVERWRITE
                    IDirect3DVertexBuffer8_Unlock( pst_SD->normalVB.pVB );
                }

                XGSetVertexBufferHeader( 0, 0, 0, 0, &tmpVBNorm, 0 );
                IDirect3DVertexBuffer8_Register( &tmpVBNorm, pNormalData );

                if ( FAILED( IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 5, &tmpVBNorm, pst_SD->normalVB.stride ) ) )
                {
                    assert(FALSE); // failed setting position stream
                }
            

            }//New object is used
*/
        
        }//If Tangente
/*		if(pObject->Binormales)
        {
 //           if ( _pst_Point == pObject->dst_Point )
            {
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 6, pObject->pVB_Binormales, sizeof(Gx8VertexComponent_Normal) );
            }
         
        }//If binorm*/

}


        IDirect3DDevice8_SetIndices( pst_SD->mp_D3DDevice, pObject->pIB, 0 );

        for ( drawStripCounter = 0; drawStripCounter < pStripData->drawStripNumber; drawStripCounter++)
        {
            GEO_tdst_DrawStrip  * pDrawStrip = &pStripData->pDrawStripList[ drawStripCounter ];
            float costant[4]={0.0f,0.0f,0.0f,0.0f};


            //Alpha lit (to be check)
            if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader)
            {


                // Set UV source stream and Vertex Declaration
                if ( pObject->pVBlist_UV )
                {
                    if(pObject->dst_PointNormal)
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
                    
                    }
                    else
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
                    
                    }
                }
                else
                {   
                    if(pObject->dst_PointNormal)
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
                    }
                    else
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
                    }
                }


                 Gx8_vUseLitAlphaVertexShader();


                //Carlone
                 IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice, 44, costant,1);

                ERR_X_Assert(pDrawStrip->vertexNumber>2);

                 IDirect3DDevice8_DrawIndexedPrimitive( pst_SD->mp_D3DDevice, 
                                                        D3DPT_TRIANGLESTRIP,
                                                        0,
                                                        pObject->l_NbPoints,
                                                        pDrawStrip->startingIndex,
                                                        pDrawStrip->vertexNumber - 2 );
            }
            else
            {


                // Set UV source stream and Vertex Declaration
                if ( pObject->pVBlist_UV )
                {
                
                    if(pObject->dst_PointNormal)
                    {
                       IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
 
					    //if (!Normalmap)
					    Gx8_vSetVertexShader( pst_SD->pVertexShaderNormalWithUV );
						/*else
						{
						Gx8_vSetVertexShader( pst_SD->pVertexShaderNormalWith2UV );
						IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 4, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
						}*/
                    }
                    else
                    {
                        Gx8_vSetVertexShader( pst_SD->pVertexShaderWithUV );
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
                    
                    }
                }
                else
                {
                    
                    if(pObject->dst_PointNormal)
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
                        Gx8_vSetVertexShader( pst_SD->pVertexShaderNormal );
                    }
                    else
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
                        Gx8_vSetVertexShader( pst_SD->pVertexShader );
                    }
                }


                ERR_X_Assert(pDrawStrip->vertexNumber>2);


				if (Normalmap && RenderInNormal)
				{
		//Test si light
							
					MATH_tdst_Vector pos={0.5f,0.5f,0.5f};
					D3DXVECTOR3 Pos={0.5f,0.5f,0.5f};
					//GDI_tdst_DisplayData    *pst_DD;
					OBJ_tdst_GameObject			**ppst_LightNode;
					LIGHT_tdst_List* List;

					List = &GDI_gpst_CurDD->st_LightList;
					ppst_LightNode = List->dpst_Light;
					ppst_LightNode++;//next
					pos = (*ppst_LightNode)->pst_GlobalMatrix->T;

					Pos.x=pos.x;
					Pos.y=pos.y;
					Pos.z=pos.z;
/*
			{
					LINEVERTEX Lines[1]; =
					{
					    { 0.0f, 100.0f, 0.0f, 0xffffffff },
					};
					Lines[0].color = 0xffffffff;
					Lines[0].x = pos.x;
					Lines[0].y = pos.y;
					Lines[0].z = pos.z;

					IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
					IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
					IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
					
					IDirect3DDevice8_SetVertexShader( pst_SD->mp_D3DDevice,D3DFVF_XYZ|D3DFVF_DIFFUSE );
					IDirect3DDevice8_DrawPrimitiveUP( pst_SD->mp_D3DDevice,D3DPT_LINELIST, 1, Lines, 
                                        sizeof(Lines[0]) );
 
				}*/
					//LIGHT_TurnOffObjectLighting();
					//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SPECULARENABLE, TRUE );
					Gx8_vUseNormalMapVertexShader((D3DXMATRIX*)&GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix,Pos);
					Gx8_vUseNormalMapPixelShader();
//					IDirect3DDevice8_SetPixelShader( pst_SD->mp_D3DDevice,Gx8_gs_a_dwPShaderHandles[Gx8_eNormalMapSpecPShader]);

				}
			//TEST CLONAGE
				{
					int i;
					int popo=NbrObjectClone;
			if (NbrObjectClone>1) popo=200;
			for (i = 0; i < popo ; i++)
			{
						if (NbrObjectClone>1)
						{
						MATH_tdst_Matrix popo;
						D3DXMatrixTranslation(&popo,(float)i*2,0,0);
						D3DXMatrixMultiply(&popo,GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix,&popo);
						IDirect3DDevice8_SetTransform( pst_SD->mp_D3DDevice,D3DTS_WORLD, &popo );
						//MATH_SetIdentityMatrix(&popo);
						//popo.T.x = (float)i*100;//i*0.2f;
						//popo.T.y = 0;
						//popo.T.z = 0;
						//glLoadMatrixf(GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
						//glMultMatrixf(&popo);
						}
				
				IDirect3DDevice8_DrawIndexedPrimitive( pst_SD->mp_D3DDevice, 
                                                        D3DPT_TRIANGLESTRIP,
                                                        0,
                                                        pObject->l_NbPoints,
                                                        pDrawStrip->startingIndex,
                                                        pDrawStrip->vertexNumber - 2 );
            
			}
				}
				if (Normalmap && RenderInNormal)
				{
					Gx8_vSetPixelShader(NULL);
					//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_SPECULARENABLE, FALSE );
				}
				//IDirect3DDevice8_SetPixelShader( pst_SD->mp_D3DDevice,NULL );
			}
            

        }
      if(pst_GlobalsWaterParams)
        {
            Gx8_PrepareDrawElementReflectedInTexture();
            for ( drawStripCounter = 0; drawStripCounter < pStripData->drawStripNumber; drawStripCounter++)
            {
                GEO_tdst_DrawStrip  * pDrawStrip = &pStripData->pDrawStripList[ drawStripCounter ];

                // Set UV source stream and Vertex Declaration


                if ( pObject->pVBlist_UV )
                {
                
                    if(pObject->dst_PointNormal)
                    {
                        Gx8_vSetVertexShader( pst_SD->pVertexShaderNormalWithUV );
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
                    
                    }
                    else
                    {
                        Gx8_vSetVertexShader( pst_SD->pVertexShaderWithUV );
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
                    
                    }
                }
                else
                {   
                    if(pObject->dst_PointNormal)
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
                    }
                    else
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
                    }
                }
                Gx8_DrawElementReflectedInTexture(pObject->l_NbPoints,
                                                  pDrawStrip->startingIndex,
                                                  pDrawStrip->vertexNumber - 2);
            }
            Gx8_UnprepareDrawElementReflectedInTexture();
        }

    

        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, NULL, 0 );
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, NULL, 0 );
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 4, NULL, 0 );//UV 2
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 5, NULL, 0 );// Tangente
        //IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 6, NULL, 0 );// Binormale

        //If it was sky...enable zwriting
//      if(Gx8_IsSky())
        {
//TiZ: PLEASE!!!            IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice,D3DRS_ZWRITEENABLE,TRUE);
//          Gx8_RS_DepthMask(pst_SD,1);
        }
        

        _GSP_EndRaster(45);

		//attention a virer
		//IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,1, NULL );

        return 0;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    



    //Dont draw the sky in Z buffer

//  if(Gx8_IsSky())
    { 
//TiZ: PLEASE!!!        IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice,D3DRS_ZWRITEENABLE,FALSE);
//      Gx8_RS_DepthMask(pst_SD,0);
    }

    {
        _GSP_BeginRaster(43);

		//TiZ NOT needed and cause a bug!       GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );
        ulOGLSetCol = 0;
        ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
        pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
        pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
        ulOGLSetCol_Or = ((Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData)->ulColorOr;
        if(pst_Color == NULL)
        {
            ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
            ulOGLSetCol ^= ulOGLSetCol_XOr;
            ulOGLSetCol = Gx8_M_ConvertColor(ulOGLSetCol);
        }

        /* Fill */
        t = _pst_Element->dst_Triangle;
        tend = _pst_Element->dst_Triangle + TNum;

        {   // draw GFX using Begin/End
            Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

			if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader)
            {
                 Gx8_vUseLitAlphaVertexShader();
            }
            else
            {
            
                //CARLONE ....VERTEX FORMAT HAS NORMALS.
				Gx8_vSetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) );
							
			}       
            if (_pst_UV)
            {
                if (pst_Color)
                {
					IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLELIST );
					
					if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
                    {

                        
                        while (t < tend)
                        {
                            ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[0]] | ulOGLSetCol_Or );
                            if(pst_Alpha)
                            {
                                ulOGLSetCol &= 0x00ffffff;
                                ulOGLSetCol |= pst_Alpha[t->auw_Index[0]];
                            }
                            IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_Index[0]].fU, _pst_UV[t->auw_Index[0]].fV );
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[0]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[0]].x, _pst_Point[t->auw_Index[0]].y, _pst_Point[t->auw_Index[0]].z, 1.0f );



                            ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[1]] | ulOGLSetCol_Or );
                            if(pst_Alpha)
                            {
                                ulOGLSetCol &= 0x00ffffff;
                                ulOGLSetCol |= pst_Alpha[t->auw_Index[1]];
                            }
                            IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_Index[1]].fU, _pst_UV[t->auw_Index[1]].fV );
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[1]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[1]].x, _pst_Point[t->auw_Index[1]].y, _pst_Point[t->auw_Index[1]].z, 1.0f );


                            

                            ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[2]] | ulOGLSetCol_Or );
                            if(pst_Alpha)
                            {
                                ulOGLSetCol &= 0x00ffffff;
                                ulOGLSetCol |= pst_Alpha[t->auw_Index[2]];
                            }
                            IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_Index[2]].fU, _pst_UV[t->auw_Index[2]].fV );
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[2]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }

                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[2]].x, _pst_Point[t->auw_Index[2]].y, _pst_Point[t->auw_Index[2]].z, 1.0f );
    
                            t++;
                        }
                    }
                    else
                    {
						///CARLONE...JUST HERE !
                        
						while (t < tend)
                        {
                            ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[0]] | ulOGLSetCol_Or );
                            if(pst_Alpha)
                            {
                                ulOGLSetCol &= 0x00ffffff;
                                ulOGLSetCol |= pst_Alpha[t->auw_Index[0]];
                            }
                           
                            IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_UV[0]].fU - 0.001f, _pst_UV[t->auw_UV[0]].fV - 0.001f );

							if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[0]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                                                      
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[0]].x, _pst_Point[t->auw_Index[0]].y, _pst_Point[t->auw_Index[0]].z, 1.0f );




                            ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[1]] | ulOGLSetCol_Or );
                            if(pst_Alpha)
                            {
                                ulOGLSetCol &= 0x00ffffff;
                                ulOGLSetCol |= pst_Alpha[t->auw_Index[1]];
                            }

							IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                           IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_UV[1]].fU - 0.001f , _pst_UV[t->auw_UV[1]].fV - 0.001f);
  
							if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[1]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                        
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[1]].x, _pst_Point[t->auw_Index[1]].y, _pst_Point[t->auw_Index[1]].z, 1.0f );

                        
                            

                            ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[2]] | ulOGLSetCol_Or );
                            if(pst_Alpha)
                            {
                                ulOGLSetCol &= 0x00ffffff;
                                ulOGLSetCol |= pst_Alpha[t->auw_Index[2]];
                            }

                            IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                           IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_UV[2]].fU - 0.001f, _pst_UV[t->auw_UV[2]].fV - 0.001f);
   
							if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[2]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
         
							IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[2]].x, _pst_Point[t->auw_Index[2]].y, _pst_Point[t->auw_Index[2]].z, 1.0f );

                            t++;
                        }
					}
 
				}
                else
                {

                   
                    //No lighting required here
                    LIGHT_TurnOffObjectLighting();
                    IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLELIST );



                    // if (!pst_Color)
                    if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
                    {
                        IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                        while (t < tend)
                        {
                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_Index[0]].fU, _pst_UV[t->auw_Index[0]].fV );
                            
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[0]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[0]].x, _pst_Point[t->auw_Index[0]].y, _pst_Point[t->auw_Index[0]].z, 1.0f );

                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_Index[1]].fU, _pst_UV[t->auw_Index[1]].fV );
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[1]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[1]].x, _pst_Point[t->auw_Index[1]].y, _pst_Point[t->auw_Index[1]].z, 1.0f );

                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_Index[2]].fU, _pst_UV[t->auw_Index[2]].fV );
                            

                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[2]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[2]].x, _pst_Point[t->auw_Index[2]].y, _pst_Point[t->auw_Index[2]].z, 1.0f );

                            t++;
                        }
                    }
                    else
                    {
                        IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                        while (t < tend)
                        {
                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_UV[0]].fU, _pst_UV[t->auw_UV[0]].fV );
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[0]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            
                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[0]].x, _pst_Point[t->auw_Index[0]].y, _pst_Point[t->auw_Index[0]].z, 1.0f );

                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_UV[1]].fU, _pst_UV[t->auw_UV[1]].fV );
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[1]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[1]].x, _pst_Point[t->auw_Index[1]].y, _pst_Point[t->auw_Index[1]].z, 1.0f );

                            IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, _pst_UV[t->auw_UV[2]].fU, _pst_UV[t->auw_UV[2]].fV );
                            
                            if(pObject->dst_PointNormal)
                            {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[2]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].z
                                                                                                        ,1.0f);
                                                                                                        
                            }
                            
                            
                            IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[2]].x, _pst_Point[t->auw_Index[2]].y, _pst_Point[t->auw_Index[2]].z, 1.0f );

                            t++;
                        }
                    }
                }
            }
            else
            {   
                // if (!_pst_UV)
                if (pst_Color)
				{

                    IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLELIST );

                    while (t < tend)
                    {
                        ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[0]] | ulOGLSetCol_Or );
                        if(pst_Alpha)
                        {
                            ulOGLSetCol &= 0x00ffffff;
                            ulOGLSetCol |= pst_Alpha[t->auw_Index[0]];
                        }
                        IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                        
                        
                        if(pObject->dst_PointNormal)
                        {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[0]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].z
                                                                                                        ,1.0f);
                                                                                                        
                        }
                        
                        
                        
                        IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[0]].x, _pst_Point[t->auw_Index[0]].y, _pst_Point[t->auw_Index[0]].z, 1.0f );

                        ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[1]] | ulOGLSetCol_Or );
                        if(pst_Alpha)
                        {
                            ulOGLSetCol &= 0x00ffffff;
                            ulOGLSetCol |= pst_Alpha[t->auw_Index[1]];
                        }
                        IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                        
                        if(pObject->dst_PointNormal)
                        {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[1]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].z
                                                                                                        ,1.0f);
                                                                                                        
                        }
                        
                        
                        IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[1]].x, _pst_Point[t->auw_Index[1]].y, _pst_Point[t->auw_Index[1]].z, 1.0f );

                        ulOGLSetCol = ulOGLSetCol_XOr ^ ( pst_Color[t->auw_Index[2]] | ulOGLSetCol_Or );
                        if(pst_Alpha)
                        {
                            ulOGLSetCol &= 0x00ffffff;
                            ulOGLSetCol |= pst_Alpha[t->auw_Index[2]];
                        }
                        IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                        
                        if(pObject->dst_PointNormal)
                        {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[2]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].z
                                                                                                        ,1.0f);
                                                                                                        
                        }
                        
                        
                        IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[2]].x, _pst_Point[t->auw_Index[2]].y, _pst_Point[t->auw_Index[2]].z, 1.0f );

                        t++;
                    }
                }
                else
                {   
                    
                    
                    LIGHT_TurnOffObjectLighting();
                    IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLELIST );

                    
                    // if (!pst_Color)
                    IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol) );
                    while (t < tend)
                    {
                        if(pObject->dst_PointNormal)
                        {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[0]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[0]].z
                                                                                                        ,1.0f);
                                                                                                        
                        }
                        
                        IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[0]].x, _pst_Point[t->auw_Index[0]].y, _pst_Point[t->auw_Index[0]].z, 1.0f );

                        if(pObject->dst_PointNormal)
                        {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[1]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[1]].z
                                                                                                        ,1.0f);
                                                                                                        
                        }
                        IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[1]].x, _pst_Point[t->auw_Index[1]].y, _pst_Point[t->auw_Index[1]].z, 1.0f );

                        if(pObject->dst_PointNormal)
                        {
                                //NORMALS
                                IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL  ,pObject->dst_PointNormal[t->auw_Index[2]].x
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].y
                                                                                                        ,pObject->dst_PointNormal[t->auw_Index[2]].z
                                                                                                        ,1.0f);
                                                                                                        
                        }
                        IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[t->auw_Index[2]].x, _pst_Point[t->auw_Index[2]].y, _pst_Point[t->auw_Index[2]].z, 1.0f );

                        t++;
                    }
                }
            }
            IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
 		}
    }   

    //If it was sky...enable zwriting
//  if(Gx8_IsSky())
    {
//TiZ: PLEASE!!!        IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice,D3DRS_ZWRITEENABLE,TRUE);
//      Gx8_RS_DepthMask(pst_SD,1);
    }
    
    _GSP_EndRaster(45);

    return 0;
}

/*
 =======================================================================================================================
    fonctions pour l'affichage de sprite GEN (génération à partir de faces
    GVW 21/05
 =======================================================================================================================
 */
void Gx8_fnvBeforeSprite(int iNumberMaxOfSprite)
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

    Gx8_vUseSpriteVertexShader( 1.0f, 0.75f );

    IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_QUADLIST );
}

void Gx8_fnvEndSprite(int iNumberMaxOfSprite)
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

    IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
}

void Gx8_fnvAddSprite(GEO_Vertex *p_stPoint,float fSize,ULONG ulColor)
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

    _GSP_BeginRaster(46);

    IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, ulColor );

    // Offset are in camera space so X points to the left and Y points _upward_

    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL, fSize/2.f, fSize/2.f );
    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, 1, 1 );
    IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, p_stPoint->x, p_stPoint->y, p_stPoint->z, 1.0f );

    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL, -fSize/2.f, fSize/2.f );
    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, 0, 1 );
    IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, p_stPoint->x, p_stPoint->y, p_stPoint->z, 1.0f );

    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL, -fSize/2.f, -fSize/2.f );
    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, 0, 0 );
    IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, p_stPoint->x, p_stPoint->y, p_stPoint->z, 1.0f );

    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_NORMAL, fSize/2.f, -fSize/2.f );
    IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, 1, 0 );
    IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, p_stPoint->x, p_stPoint->y, p_stPoint->z, 1.0f );

    _GSP_EndRaster(46);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_BeginSpriteVBCreation(void)
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

    iBeginVertexSpriteVB = pst_SD->m_stBigVBforSpritesData.m_uiWhereToLockFrom;
    iLastVertexSpriteVB = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL Gx8_CheckSpriteVBCreation(void)
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

    if ( (iBeginVertexSpriteVB != (int) pst_SD->m_stBigVBforSpritesData.m_uiWhereToLockFrom) &&
         (iBeginVertexSpriteVB != -1 ) )
    {
        return TRUE;
    }

    iBeginVertexSpriteVB = -1;
    return FALSE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_EndSpriteVBCreation( GEO_tdst_Object *_pst_Obj, GEO_tdst_ElementIndexedSprite *pSpriteElement )
{
    Gx8_tdst_SpecificData   * pst_SD = GX8_M_SD(GDI_gpst_CurDD);
    int                     iNumberOfSpriteVertex;
    int                     iNumberOfSpriteVertexPerLevel;
    int                     iEndVertexSpriteVB = pst_SD->m_stBigVBforSpritesData.m_uiWhereToLockFrom;
    int                     index;
    int                     size;
    int                     counter;
    int                     level ;
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer;
    BYTE                    * pSourceData;
    BYTE                    * pDestData;

        // check for End called without Begin
    assert( iBeginVertexSpriteVB != -1 );

        // compute Number of Sprites' Vertex
    if ( iLastVertexSpriteVB == -1 )
    {       // circular buffer did not restart from begin
        iNumberOfSpriteVertex = iEndVertexSpriteVB - iBeginVertexSpriteVB;
    }
    else
    {       // circular buffer restarted from begin
        iNumberOfSpriteVertex = iLastVertexSpriteVB - iBeginVertexSpriteVB + iEndVertexSpriteVB;
    }

    iNumberOfSpriteVertexPerLevel = iNumberOfSpriteVertex / GX8_SPG_LEVEL_NUMBER;

    if ( iNumberOfSpriteVertex > 0 )
    {
            // chach that is a multiple of GX8_SPG_LEVEL_NUMBER
        assert( iNumberOfSpriteVertex == iNumberOfSpriteVertexPerLevel * GX8_SPG_LEVEL_NUMBER );
        
            // create SpriteVBData for each level
        for( level = 0; level < GX8_SPG_LEVEL_NUMBER; level++)
        {
                // init index
            if ( iNumberOfSpriteVertexPerLevel > GX8_SPG_MAX_SPRITE_PER_LEVEL )
            {
                index = iBeginVertexSpriteVB + ( level * GX8_SPG_MAX_SPRITE_PER_LEVEL );
            }
            else
            {
                index = iBeginVertexSpriteVB + ( level * iNumberOfSpriteVertexPerLevel );
            }

                // Create the Sprite VB
            if ( D3D_OK != IDirect3DDevice8_CreateVertexBuffer( pst_SD->mp_D3DDevice,
                                                                iNumberOfSpriteVertexPerLevel * sizeof(Gx8VertexFormatForSprites),
                                                                0,
                                                                0,
                                                                0,
                                                                &pVertexBuffer) )
            {   // Out of contiguous memory
                assert( false );
            }

                // set Sprite VB data
            _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].pSPG_VB = pVertexBuffer;
            _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].iSpriteVertexNumber = iNumberOfSpriteVertexPerLevel;
            _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].iMaterialID = pSpriteElement[level].l_MaterialId; 
            _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].fGlobalSize = pSpriteElement[level].fGlobalSize;
            _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].fGlobalRatio = pSpriteElement[level].fGlobalRatio;

                // Lock SPG_VB
            IDirect3DVertexBuffer8_Lock(    pVertexBuffer, 
                                            0, 
                                            0, 
                                            &pDestData, 
                                            0 );

            // init counter
            counter = 0;
            
            // copy data block by block
            while ( counter < iNumberOfSpriteVertexPerLevel )
            {
                    // compute size
                if ( iNumberOfSpriteVertexPerLevel - counter > GX8_SPG_MAX_SPRITE_PER_LEVEL )
                {
                    size = GX8_SPG_MAX_SPRITE_PER_LEVEL;
                }
                else
                {
                    size = iNumberOfSpriteVertexPerLevel - counter;
                }
                    
                    // Lock Big Sprite VB
                IDirect3DVertexBuffer8_Lock(    pst_SD->m_stBigVBforSpritesData.m_VertexBuffer, 
                                                index * sizeof(Gx8VertexFormatForSprites), 
                                                size * sizeof(Gx8VertexFormatForSprites), 
                                                &pSourceData, 
                                                D3DLOCK_READONLY );

                    // copy data
                memcpy( pDestData, pSourceData, size * sizeof(Gx8VertexFormatForSprites) );
                pDestData += ( size * sizeof(Gx8VertexFormatForSprites) );

                    // Unlock VBs
                IDirect3DVertexBuffer8_Unlock( pst_SD->m_stBigVBforSpritesData.m_VertexBuffer ); 

                    // increase counter 
                counter += size;

                    // update index
                index += GX8_SPG_MAX_SPRITE;
                if ( ( iLastVertexSpriteVB != -1) && ( index > iLastVertexSpriteVB ) )
                {
                    index = index - iLastVertexSpriteVB;
                }

            }

            IDirect3DVertexBuffer8_Unlock( pVertexBuffer ); 
        }

    }

    iBeginVertexSpriteVB = -1;
    iLastVertexSpriteVB = -1;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_DrawSpriteVB( GEO_tdst_Object *_pst_Obj, MAT_tdst_Material *pMaterial )
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);
    int level;
    MAT_tdst_Material *_pst_Material = NULL;
    MAT_tdst_MultiTexture   *pst_MLTTX;
    MAT_tdst_MTLevel        *pst_MLTTXLVL;

    for( level = 0; level < GX8_SPG_LEVEL_NUMBER; level ++)
    {
            // get sub material
        if( pMaterial && ( pMaterial->st_Id.i->ul_Type == GRO_MaterialMulti))
        {
            _pst_Material = ((MAT_tdst_Multi *)pMaterial)->dpst_SubMaterial[ lMin( _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].iMaterialID , 
                                                                                   ((MAT_tdst_Multi *)pMaterial)->l_NumberOfSubMaterials - 1) ];
        }
            // use default material if needed
        if(_pst_Material == NULL) 
            _pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

            
        if(_pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
        {
                // get multi texture level
            pst_MLTTX = (MAT_tdst_MultiTexture *) _pst_Material;
            pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;
            if ( pst_MLTTXLVL )
            {
                    // set texture 
                Gx8_SetTextureBlending( pst_MLTTXLVL->s_TextureId, pst_MLTTXLVL->ul_Flags, pst_MLTTXLVL->s_AditionalFlags);

                    // draw 
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].pSPG_VB, sizeof(Gx8VertexFormatForSprites) );
                Gx8_vUseSpriteVertexShader( _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].fGlobalSize, _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].fGlobalRatio );
                IDirect3DDevice8_DrawPrimitive( pst_SD->mp_D3DDevice, D3DPT_QUADLIST, 0, _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData[level].iSpriteVertexNumber / 4 );
            }
        }
    }

    iBeginVertexSpriteVB = -1;
    iLastVertexSpriteVB = -1;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG Gx8_l_DrawElementIndexedSprites
(
    GEO_tdst_ElementIndexedSprite   *_pst_Element,
    GEO_Vertex                      *_pst_Point,
    ULONG                           ulnumberOfPoints
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GDI_tdst_DisplayData    *pst_DD;
    GEO_tdst_IndexedSprite  *p_Frst, *p_Last;
    ULONG                   ul_ColorOr, ul_ColorXor, ul_ColorSet;
    ULONG                   *pst_Color;
    ULONG                   *pst_Alpha;

    int NumberOfParticle;
    //int counter;


    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _GSP_BeginRaster(46);

    pst_DD = GDI_gpst_CurDD;

    pst_Color = pst_DD->pst_ComputingBuffers->CurrentColorField;
    pst_Alpha = pst_DD->pst_ComputingBuffers->CurrentAlphaField;
    ul_ColorOr = pst_SD->ulColorOr;
    ul_ColorXor = pst_DD->pst_ComputingBuffers->ulColorXor;
    ul_ColorSet = pst_DD->pst_ComputingBuffers->ul_Ambient | ul_ColorOr;
    ul_ColorSet ^= ul_ColorXor;

        {   
            //NEW PARTICLE'S VERTEX SHADER
            Gx8_vUseSpriteVertexShaderNew(_pst_Element->fGlobalSize, _pst_Element->fGlobalRatio,shaderHandler);
            
            //CAMERA AND SIZE CONSTANTS
            Gx8_PrepareIndexParticleVSSizeConstant(pst_SD,pst_DD->st_MatrixStack.pst_CurrentMatrix,_pst_Element->fGlobalSize, _pst_Element->fGlobalRatio);



            p_Frst = _pst_Element->dst_Sprite;
            p_Last = p_Frst + _pst_Element->l_NbSprites;

            if (p_Frst->auw_Index == 0xC0DE) // GFX Signal of mega-flux (no indexes) , points ares SOFT_tdst_AVertexes with w = size
            {


                //Prepare the color buffer
                SOFT_tdst_AVertex *pS; //*pSE;
                //int NumParticle;
                float alpha;
                bool alphaOut;  
            
        
                //Calculate number of particle to be rendered
                NumberOfParticle=_pst_Element->l_NbSprites;
                

                //Prepare the device for rendering
                Gx8_PrepareDeviceForParticle(pst_SD);

                pS = (SOFT_tdst_AVertex *)_pst_Point;
                pS += p_Frst [1].auw_Index;
                            
                if (pst_Color) 
                    pst_Color +=  p_Frst [1].auw_Index;

                
                if (pst_Alpha) 
                    pst_Alpha +=  p_Frst [1].auw_Index;


                if(pst_Alpha) 
                { 
                    alpha = (float )pst_Alpha[p_Frst->auw_Index]; 
                    alphaOut=true;
                }
                else
                {
                    alphaOut=false;
                    alpha=0.0f;
                }


                do
                {
                    //NO COLOR NOW

                    //SET THE VS CONSTANT (THE PARTICLE POSIION AND SIZE)
                    if(NumberOfParticle<MAX_PART_NUM)
                    {
                        Gx8_PrepareParticleVSConstants(pst_SD,pS,NumberOfParticle);
                        //TO DO Gx8_PrepareParticleColorsFAST(pst_SD,NumberOfParticle,pst_Color,alpha,alphaOut);
                        Gx8_PrepareParticleColors(pst_SD,NumberOfParticle,pst_Color,(ULONG)alpha,alphaOut,ul_ColorSet);
                        //Gx8_PrepareParticleColorsVB(pst_SD,NumberOfParticle,pst_Color,alpha,alphaOut);
                        IDirect3DDevice8_DrawPrimitive(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,0,NumberOfParticle);
                    }
                    
                    else
                    {
                        Gx8_PrepareParticleVSConstants(pst_SD,pS,MAX_PART_NUM);
                        //TO DO Gx8_PrepareParticleColorsFAST(pst_SD,MAX_PART_NUM,pst_Color,alpha,alphaOut);
                        Gx8_PrepareParticleColors(pst_SD,MAX_PART_NUM,pst_Color,(ULONG)alpha,alphaOut,ul_ColorSet);
                        //Gx8_PrepareParticleColorsVB(pst_SD,NumberOfParticle,pst_Color,alpha,alphaOut);
                        IDirect3DDevice8_DrawPrimitive(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,0,MAX_PART_NUM);
                    }



                    pS+=MAX_PART_NUM;
                    pst_Color+=MAX_PART_NUM;


                    
                }while((NumberOfParticle-=MAX_PART_NUM)>0);

            } 
            else
            {
                int partToDraw;

                //Calculate number of particle to be rendered
                NumberOfParticle=_pst_Element->l_NbSprites;
                
                //Prepare the device for rendering
                Gx8_PrepareDeviceForParticle(pst_SD);


                do
                {
                    

                    if(NumberOfParticle<MAX_PART_NUM)
                        partToDraw=NumberOfParticle;
                    else
                        partToDraw=MAX_PART_NUM;

                    if(pst_Alpha)
                    {
                        //Gx8_PrepareParticleColorsVBIndex(pst_SD,NumberOfParticle,p_Frst,pst_Color,pst_Alpha,true);
                        Gx8_PrepareParticleColorIndex(pst_SD,partToDraw,p_Frst,pst_Color,pst_Alpha,true,ul_ColorSet);
                        Gx8_PrepareIndexParticleVSConstants(pst_SD,p_Frst,_pst_Point,partToDraw);
                        IDirect3DDevice8_DrawPrimitive(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,0,partToDraw);
                    }
                    else
                    {
                        //Gx8_PrepareParticleColorsVBIndex(pst_SD,NumberOfParticle,p_Frst,pst_Color,0,false);
                        Gx8_PrepareParticleColorIndex(pst_SD,partToDraw,p_Frst,pst_Color,0,false,ul_ColorSet);
                        Gx8_PrepareIndexParticleVSConstants(pst_SD,p_Frst,_pst_Point,partToDraw);
                        IDirect3DDevice8_DrawPrimitive(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,0,partToDraw);
                    }


                    //Move vertex ahead
                    p_Frst+=MAX_PART_NUM;


                }while((NumberOfParticle-=MAX_PART_NUM)>0);

            }

        }


    _GSP_EndRaster(46);
    return 0;
}

/*
 =======================================================================================================================
    Aim:    Setup blending mode
 =======================================================================================================================
 */
void Gx8_SetTextureBlending(ULONG _l_Texture, ULONG BM, unsigned short s_AditionalFlags)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG                   Flag;
    ULONG                   Delta;
    int                     i_Value;
    Gx8_tdst_SpecificData   *pst_SD;
    extern ULONG OpenglCorrectBugMul2X;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = GX8_M_SD(GDI_gpst_CurDD);

#pragma message("TODO: clear the confusion between draw mask (element level) and texture blending (texture level).")
 
	Gx8_RS_DrawWired(pst_SD, !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));
    Gx8_RS_DepthTest(pst_SD, GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest );
    Gx8_RS_Fogged( pst_SD, ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (pst_SD->ulFogState)));
//IDirect3DDevice8_SetRenderState(pst_SD, D3DRS_FOGENABLE, 1/*TRUE*/ );

	if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace)
    {
        if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF)
            Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_INVERTED);
        else
            Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_NORMAL);
    }
    else
    {
        Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_OFF);
    }
	//Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_OFF);
	Delta = GDI_gpst_CurDD->LastBlendingMode ^ BM;


	// Delta = -1;
	Flag = MAT_GET_FLAG(BM);

	Gx8_RS_UseTexture(pst_SD, _l_Texture);

	Gx8_RS_DepthMask(pst_SD, (Flag & MAT_Cul_Flag_NoZWrite) ? 0 : 1);
    Gx8_RS_DepthFunc(pst_SD, (Flag & MAT_Cul_Flag_ZEqual) ? D3DCMP_EQUAL : D3DCMP_LESSEQUAL);

    if(Delta & MAT_Cul_Flag_TileU)
    {
        i_Value = (Flag & MAT_Cul_Flag_TileU) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
        IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU, i_Value);
       /* IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_ADDRESSU, i_Value);*/
    }

    if(Delta & MAT_Cul_Flag_TileV)
    {
        i_Value = (Flag & MAT_Cul_Flag_TileV) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
        IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV, i_Value);
         /*IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_ADDRESSV, i_Value);*/
    }

    if(Delta & MAT_Cul_Flag_Bilinear)
    {
        i_Value = (Flag & MAT_Cul_Flag_Bilinear) ? D3DTEXF_LINEAR : D3DTEXF_POINT;
        IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, i_Value);
        IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, i_Value);
/*        IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_MAGFILTER, i_Value);
        IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_MINFILTER, i_Value);*/
    }

    if(Delta & MAT_Cul_Flag_Trilinear)
    {
        i_Value = (Flag & MAT_Cul_Flag_Trilinear) ? D3DTEXF_LINEAR : D3DTEXF_NONE;
        IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MIPFILTER, i_Value);
        /*IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_MIPFILTER, i_Value);*/
    }

	if (Normalmap && RenderInNormal)
	{

		if(MAT_Cul_Flag_TileU)
		{
			i_Value = (Flag & MAT_Cul_Flag_TileU) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
			IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_ADDRESSU, i_Value);
		}

		if(MAT_Cul_Flag_TileV)
		{
			i_Value = (Flag & MAT_Cul_Flag_TileV) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
			IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_ADDRESSV, i_Value);
		}

		if(MAT_Cul_Flag_Bilinear)
		{
			i_Value = (Flag & MAT_Cul_Flag_Bilinear) ? D3DTEXF_LINEAR : D3DTEXF_POINT;
			IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_MAGFILTER, i_Value);
			IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 1, D3DTSS_MINFILTER, i_Value);
		}
	}

    if(Delta & (MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_HideColor))
    {
        switch(Flag & (MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_HideColor))
        {
        case 0:
            i_Value = D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
            break;
        case MAT_Cul_Flag_HideAlpha:
            i_Value = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
            break;
        case MAT_Cul_Flag_HideColor:
            i_Value = D3DCOLORWRITEENABLE_ALPHA;
            break;
        case MAT_Cul_Flag_HideColor | MAT_Cul_Flag_HideAlpha:
            i_Value = 0;
            // test pour l'ombre ... normalement c'est le 0 --> GVW
            i_Value = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
            break;
        default:
            // never executed
            i_Value = 0;
        }

        if (!bDisableColors)
            Gx8_M_RenderState(pst_SD, D3DRS_COLORWRITEENABLE, i_Value);
   }

#pragma message("TODO : better alphatest delta-ing ?")

	if(Delta & (MAT_Cul_Flag_AlphaTest | MAT_Cc_AlphaTresh_MASK))

	//if(Delta & (MAT_Cul_Flag_AlphaTest | MAT_Cc_AlphaTresh_MASK | MAT_Cul_Flag_InvertAlpha))
    {
		if(Flag & MAT_Cul_Flag_AlphaTest)
        {
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, TRUE );
            
			if(Flag & MAT_Cul_Flag_InvertAlpha)
                Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_LESS);
            else
                Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_GREATER);

            Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, MAT_GET_AlphaTresh(BM));
            
        }
        else
        {
			Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, FALSE);
		}
    }

    if(Flag & MAT_Cul_Flag_UseLocalAlpha)
        pst_SD->ulColorOr = 0xff000000;
    else
    pst_SD->ulColorOr = 0;

	if (Flag & MAT_Cul_Flag_UseLocalAlpha)
    {
       GLI_M_SetTextureAlphaStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_TFACTOR);
        if (MAT_GET_ColorOp(BM) == MAT_Cc_ColorOp_Disable)
        {
            Gx8_M_RenderState(pst_SD, D3DRS_TEXTUREFACTOR,
                                (Gx8_M_ConvertColor(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient) & 0x00ffffff) | // color...
                                ((((ULONG) s_AditionalFlags) << 16) & 0xff000000)); // ... and alpha
        }
        else
        {
            Gx8_M_RenderState(pst_SD, D3DRS_TEXTUREFACTOR,
                                0x00ffffff | // color...
                                (((ULONG) s_AditionalFlags) << 16)); // ... and alpha
        }
    }
    else
	{
        GLI_M_SetTextureAlphaStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE);//D3DTA_TEXTURE
        //GLI_M_SetTextureAlphaStage(pst_SD->mp_D3DDevice, 0, D3DTOP_SELECTARG1, D3DTOP_MODULATE, D3DTA_DIFFUSE);//D3DTA_TEXTURE
        //GLI_M_SetTextureAlphaStage(pst_SD->mp_D3DDevice, 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE)

		if (MAT_GET_ColorOp(BM) == MAT_Cc_ColorOp_Disable)
        {
            Gx8_M_RenderState(pst_SD, D3DRS_TEXTUREFACTOR, Gx8_M_ConvertColor(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient) | 0xff000000);
            // same as Gx8_M_RenderState(pst_SD, D3DRS_TEXTUREFACTOR, Gx8_M_ConvertColor(pst_MLTTX->ul_Specular));
        }
    }

/////CHECK IF THE DIFFUSE COMPONTEN IS IN TEXTURE (FOR SHADOW BUFFER)

if(!pst_SD->diffuseInTexture)
{
    unsigned int colorOp; 

#if defined( USE_SHADOW_BUFFER )
    Gx8_ResetDiffuseComponent();
#endif

    colorOp = MAT_GET_ColorOp(BM);

    switch( colorOp )
    {
    case MAT_Cc_ColorOp_Disable: // Constant
        {
           GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_TFACTOR );
 //       	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP, D3DTOP_DISABLE );

		} break;
   case MAT_Cc_ColorOp_Specular:
        {
            //TiZ NOTE: this is actually a good choice.
            //          BUT we never set the specular color, so we'll never see the specular.
            //          So, switch to diffuse
            //GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_SPECULAR );
           GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );
        } break;
   case MAT_Cc_ColorOp_InvertDiffuse:
        {
			GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE | D3DTA_COMPLEMENT );
        } break;
   case MAT_Cc_ColorOp_RLI:
        {
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );
        } break;
   case MAT_Cc_ColorOp_FullLight:
        {
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_TEXTURE);
        } break;
	// To be check later for disable
	case MAT_Cc_ColorOp_Diffuse2X:
/*		{
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE2X, D3DTA_DIFFUSE );
        } break;*/

    //case MAT_Cc_ColorOp_Diffuse2X:
    case MAT_Cc_ColorOp_Diffuse:
        {
			GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );
        } break;
	case MAT_Cc_ColorOp_SHIFT:
        //ERR_X_Warning( false, "Ignoring MAT_Cc_ColorOp_SHIFT color op", 0 );
        break;
    case 0x0F:
        //ERR_X_Warning( false, "Ignoring unknown (0xF) color op", 0 );
        break;
	default:
        //ERR_X_Error( false, "Unhandled material color op", 0 );
        break;

    };
}

else
{
  unsigned int colorOp = MAT_GET_ColorOp(BM);
    switch( colorOp )
    {
    case MAT_Cc_ColorOp_Disable: // Constant
        {
            Gx8_ResetDiffuseComponent();
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_TFACTOR );
        } break;
    case MAT_Cc_ColorOp_Specular:
        {       
            Gx8_ResetDiffuseComponent();
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_SPECULAR );
        } break;
    case MAT_Cc_ColorOp_InvertDiffuse:
        {
           //CARLONE...TO DO  GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE | D3DTA_COMPLEMENT );
            Gx8_SetDiffuseComponent(D3DTOP_MODULATE);
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_TEXTURE |D3DTA_COMPLEMENT);
        
        } break;
    case MAT_Cc_ColorOp_RLI:
        {
            Gx8_SetDiffuseComponent(D3DTOP_MODULATE);
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_TEXTURE );
        } break;
    case MAT_Cc_ColorOp_FullLight:
        {
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_TEXTURE);
        } break;
    case MAT_Cc_ColorOp_Diffuse2X:
    /*    {
            Gx8_SetDiffuseComponent(D3DTOP_MODULATE2X);
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_TEXTURE );
        } break;
*/

    case MAT_Cc_ColorOp_Diffuse:
        {
            Gx8_SetDiffuseComponent(D3DTOP_MODULATE);
            GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_TEXTURE );
        } break;
    case 0x0F:
        //ERR_X_Warning( false, "Ignoring unknown (0xF) color op", 0 );
        break;
    case MAT_Cc_ColorOp_SHIFT:
        //ERR_X_Warning( false, "Ignoring MAT_Cc_ColorOp_SHIFT color op", 0 );
        break;
    default:
        //ERR_X_Error( false, "Unhandled material color op", 0 );
        break;
    };

}




    if((MAT_GET_Blending(Delta)) || (Delta & MAT_Cul_Flag_InvertAlpha))
    {
        unsigned int blending = MAT_GET_Blending(BM);
        switch( blending )
        {
		case MAT_Cc_Op_Glow:
		case MAT_Cc_Op_Copy:
			Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, FALSE);
            //Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE); // no need for that : they're always re-specified when alphablend is enabled.
            //Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ZERO);
            break;
       case MAT_Cc_Op_Alpha:
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
            if(Flag & MAT_Cul_Flag_InvertAlpha)
            {
                Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
            }
            else
            {
                Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            }
            break;
        case MAT_Cc_Op_AlphaPremult:
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
            Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
            if(Flag & MAT_Cul_Flag_InvertAlpha)
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
            else
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            break;
        case MAT_Cc_Op_AlphaDest:
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
            if (Flag & MAT_Cul_Flag_InvertAlpha)
            {
                Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA);
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
            }
            else
            {
                Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA);
            }
            break;
        case MAT_Cc_Op_AlphaDestPremult:
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
            if (Flag & MAT_Cul_Flag_InvertAlpha)
            {
                Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA);
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
            }
            else
            {
                Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
                Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
            }
            break;
        case MAT_Cc_Op_Add:
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
 			//Ligne temporairement rajouté: 
			//GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );

			Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
			Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
			
			if(MAT_GET_Blending(GDI_gpst_CurDD->LastBlendingMode) == MAT_Cc_Op_Copy) Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
			if(MAT_GET_Blending(GDI_gpst_CurDD->LastBlendingMode) == MAT_Cc_Op_Glow) Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
	/*		if (OpenglCorrectBugMul2X)
			{
				if(Flag & MAT_Cul_Flag_InvertAlpha)
				{
					Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
				}
				else
				{
					Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
					Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
				}
			}
			else
			{
				Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
				Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
			}
		*/	

		break;
        case MAT_Cc_Op_Sub:
   			//GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );

			Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
            Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ZERO);
            Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
            break;
        case MAT_Cc_Op_PSX2ShadowSpecific:
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
            Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ZERO);
            Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            break;
        case MAT_Cc_Op_SpecialContrast:
            Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, 1);
            Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
            Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
            break;
        case 0x0F:
            //ERR_X_Warning( false, "Ignoring unknown blending (0x0F)", 0 );
            break;
        default:
            //ERR_X_Error( false, "unhandled MAT blending", 0 );
            break;
        }
    }

	if(pst_SD->ulFogState & 1)
    {
        switch(MAT_GET_Blending(BM))
        {
        case MAT_Cc_Op_Copy:
        case MAT_Cc_Op_Glow:
        case MAT_Cc_Op_Alpha:
        case MAT_Cc_Op_AlphaDest:
            if(pst_SD->ulFogState != 1)
            {
                pst_SD->ulFogState = 1;
                Gx8_M_RenderState(pst_SD, D3DRS_FOGCOLOR, pst_SD->ul_FogColor);
            }
            break;
        case MAT_Cc_Op_AlphaPremult:
        case MAT_Cc_Op_AlphaDestPremult:
            if(pst_SD->ulFogState != 5)
            {
                pst_SD->ulFogState = 5;
                pst_SD->ul_FogColorOn2 &= 0xFFFFFF;
                Gx8_M_RenderState(pst_SD, D3DRS_FOGCOLOR, pst_SD->ul_FogColor);
            }
            break;
        case MAT_Cc_Op_Add:
        case MAT_Cc_Op_Sub:
        case MAT_Cc_Op_PSX2ShadowSpecific:
            if(pst_SD->ulFogState != 3)
            {
                pst_SD->ulFogState = 3;
                Gx8_M_RenderState(pst_SD, D3DRS_FOGCOLOR, pst_SD->ul_FogColorBlack);
            }
            break;
        default:
            ERR_X_Error( false, "unhandled MAT blending", 0 );
            break;
        }
    }
//IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0, (D3DBaseTexture*)pst_SD->Tex_Normalmap );

if (Normalmap && RenderInNormal)
{
// -------------testtemporaire normal map --
{
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,2, NULL );
	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,3, NULL );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,2, D3DTSS_COLOROP, D3DTOP_DISABLE ); 
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_COLOROP, D3DTOP_DISABLE ); 

	IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,3, (D3DBaseTexture*)pst_SD->g_pTextureP );

	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );


	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

}
// ----------------------------------------
}


	GDI_gpst_CurDD->LastTextureUsed = _l_Texture;
    GDI_gpst_CurDD->LastBlendingMode = BM;

}

/**********************************************************************************************************************/
/* SPG2 specific functions BEGIN **************************************************************************************/
/**********************************************************************************************************************/
#define UsePush  //mode _2X
#define UsePusha //mode _2Xa
#define UsePushc //mode _2XOneSeg

#define CosAlpha -0.34202014332566873304409961468226f 
#define SinAlpha 0.9396926207859083840541092773247f 
//u32 FAKEPUSH[20000];

void Gx8_l_DrawSPG2_2X_OneSegment(
	SOFT_tdst_AVertex				    *Coordinates,
	ULONG								*pColors,
	ULONG								ulnumberOfPoints,
	float								fTrapeze,
	float								fEOHP,
	float								fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind,
	SPG2_InstanceInforamtion			*p_stII
)
{
#ifndef UsePushc
	float	*pData;//[2000];


	//float *pVertices;
	//Gx8VertexComponent_Position *pData;

	//float *pDodo;
	struct MonStream_
	{
		float x,y,z;
		DWORD Color;
		float u,v;
	} MonStream[8];

	UINT uiVStride;
	ULONG sulnumberOfPoints;
#endif

	Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

	SOFT_tdst_UV	tf_UV;
	__m128 GlobalPos,GlobalZADD,GlobalScale ,One_One_One_One;
	float DeltaU;

	ULONG DiffuseCol;

	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	float TexUMax,TexVMax;
	float TexUBase,TexVBase;

#ifdef UsePushc
	DWORD	dwVertexSize = sizeof(D3DXVECTOR3) + sizeof(D3DCOLOR)+sizeof(D3DXVECTOR2);
	DWORD   dwNumVertices;
    DWORD   dwPushSizeInBytes;
    DWORD   dwPushSizeInDWORDs;
	DWORD* pPush;
#endif

//	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
//	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

	//_GSP_BeginRaster(50);

	//WriteStringEx(L"Exception Handler installed", 10.0f, 10.0f, 0xffffffff);
	//_GSP_BeginRaster(49);

	TexUBase = TexVBase = 0.0f;

	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;
	//if (TileNumber>0)	ulnumberOfPoints *= 0.7f;//<<<<<<<<<<<<<<<<

	/* Fast One Over 2^N */
	*(u32 *)&TexUMax = (127 - U_SHIFT) << 23;
	*(u32 *)&TexVMax = (127 - V_SHIFT) << 23;

	fRatio *= 0.5f;

	DeltaU = TexVMax*(float)TileNumber;// * fOoNumOfSeg;

	GlobalPos = _mm_loadu_ps((float*)&p_stII->GlobalPos);
	GlobalZADD = _mm_loadu_ps((float*)&p_stII->GlobalZADD);
	GlobalScale = _mm_set_ps1(p_stII->GlobalSCale );
	One_One_One_One = _mm_set_ps1(1.0f);
#ifndef UsePushc
		
	uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	//IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLESTRIP );

	// For performance reasons, vertex buffers are multi-buffered. Each time
    // the vertex buffer contents are updated, use a new vertex buffer.

/*	if( IDirect3DVertexBuffer8_IsBusy( pst_SD->VBforSPG2.Spg2VB ) )
    {
        if( ++pst_SD->VBforSPG2.m_dwCurrentBuffer >= 1 )
            pst_SD->VBforSPG2.m_dwCurrentBuffer = 0;
		pst_SD->VBforSPG2.Spg2VB = pst_SD->VBforSPG2.swapVB[pst_SD->VBforSPG2.m_dwCurrentBuffer];
   }
*/
	//ulnumberOfPoints = 100;
	sulnumberOfPoints = ulnumberOfPoints;

//	IDirect3DVertexBuffer8_Lock( pst_SD->zouege, 0, 0, &pData, 0  );//D3DLOCK_NOOVERWRITE//D3DLOCK_READONLY

#else
	//ulnumberOfPoints = 100;
	dwNumVertices      = 4;
	dwPushSizeInBytes  = dwVertexSize * dwNumVertices;
	dwPushSizeInDWORDs = dwPushSizeInBytes / sizeof(DWORD);
	
		/*if (bTest)
			IDirect3DDevice8_BeginPush( pst_SD->mp_D3DDevice,(11*(ulnumberOfPoints))+(ulnumberOfPoints*dwVertexSize*2), &pPush );
			else*/
				IDirect3DDevice8_BeginPush( pst_SD->mp_D3DDevice,(5*(ulnumberOfPoints))+(ulnumberOfPoints*dwVertexSize), &pPush );

#endif
/*	IDirect3DVertexBuffer8_Lock(pst_SD->VBforSPG2.Spg2VB,
                                0,
                                0,
                                p_vData,
                                D3DLOCK_NOOVERWRITE);
 */
	//ulnumberOfPoints=1;


	while (ulnumberOfPoints--)
	{
	//_GSP_BeginRaster(50);
		
		__m128 Position,Position2;//,Position3,Position4;
		__m128 Xa;
		__m128 Ya;
		__m128 stNormale;
		__m128 Local_01;
		__m128 Local_04;
		
//		float store[4];

		if (p_stII->BaseAnimUv)
		{
			TexUBase = (float)(TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * TexUMax;
			TexVBase = (float)((TextureTilerUV_Base >> U_SHIFT)^0xf) * TexVMax;
			TextureTilerUV_Base += 3;
		}
	
		tf_UV.u = TexUBase;
		tf_UV.v = TexVBase;


 		Position =	_mm_load_ps((float*)(Coordinates ++));// Pos
		Xa =		_mm_load_ps((float*)(Coordinates ++));// Pos
		Ya =		_mm_load_ps((float*)(Coordinates ));// Pos
		Local_01 = _mm_set_ps1((Coordinates++)->w );
		stNormale = _mm_load_ps((float*)(Coordinates ));// Pos
		Coordinates ++;

		Position =  _mm_add_ps(Position , GlobalPos);
		stNormale = _mm_add_ps(stNormale , GlobalZADD);
		Local_04 = Local_01 ;
		stNormale = _mm_mul_ps(stNormale , GlobalScale);
		stNormale = _mm_mul_ps(stNormale , Local_04 );

		Local_04 = _mm_set_ps1(fRatio);
		Local_01 = _mm_mul_ps(Local_01 , Local_04 );

		//if (!bTest)
		{
			if ( ulMode == 1 ) // DrawY
			{
				Xa = Ya;
			}
		}

		Xa = _mm_mul_ps(Xa , Local_01 );

		/*if (bTest)
		{
			Ya = _mm_mul_ps(Ya , Local_01 );//p
			Position3 = Position;//p
		}*/
		
		Position2 = _mm_add_ps(Position , Xa);
		Position = _mm_sub_ps(Position , Xa);

		/*if (bTest)
		{
			Position4 = _mm_add_ps(Position3 , Ya);//p
			Position3 = _mm_sub_ps(Position3 , Ya);//p
		}*/

#ifdef UsePushc
	
		/*if (bTest)
		{
			*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
			*pPush++ = D3DPT_QUADLIST;
			*pPush++ = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG|D3DPUSH_INLINE_ARRAY, dwPushSizeInDWORDs );
		}
		else*/
		{
			*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
			*pPush++ = D3DPT_TRIANGLESTRIP;
			*pPush++ = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG|D3DPUSH_INLINE_ARRAY, dwPushSizeInDWORDs );
		}

#endif

		if (p_stII->GlobalColor) 
		{
			DiffuseCol = Gx8_M_ConvertColor(p_stII->GlobalColor) ;
		}
		else
		{
			ULONG DiffuseTemp=*pColors++;
			DiffuseCol = Gx8_M_ConvertColor(DiffuseTemp);
		}

		tf_UV.u = TexUBase;//0.0f;

#ifdef UsePushc
		_mm_storeu_ps((float*)pPush, Position2);
		pPush += 3;
        *((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;
		tf_UV.u = TexUBase+TexUMax;//1.0f;
		
		_mm_storeu_ps((float*)pPush, Position);
		pPush += 3;
		*((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;
		tf_UV.v += DeltaU;
			
		Position2 = _mm_add_ps(stNormale , Position2 );
		Position = _mm_add_ps(stNormale , Position );

		tf_UV.u = TexUBase;//0.0f;
		
		_mm_storeu_ps((float*)pPush, Position2);
		pPush += 3;
        *((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;
		tf_UV.u = TexUBase+TexUMax;//1.0f;
		
		_mm_storeu_ps((float*)pPush, Position);
		pPush += 3;
		*((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;

// --------------------------------------
		/*if (bTest)
		{
		
		*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
		*pPush++ = 0;

		*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
		*pPush++ = D3DPT_QUADLIST;
        *pPush++ = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG|D3DPUSH_INLINE_ARRAY, dwPushSizeInDWORDs );


		tf_UV.u = 0;//TexUBase;
		tf_UV.v = 0;//TexVBase;

		_mm_storeu_ps((float*)pPush, Position4);
		pPush += 3;
        *((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;
		tf_UV.u = 0;//TexUBase+TexUMax;//1.0f;
		
		_mm_storeu_ps((float*)pPush, Position3);
		pPush += 3;
		*((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;
		tf_UV.v += 0;//DeltaU;
			
		Position2 = _mm_add_ps(stNormale , Position4 );
		Position = _mm_add_ps(stNormale , Position3 );

		tf_UV.u = 0;//TexUBase;//0.0f;
		
		_mm_storeu_ps((float*)pPush, Position3);
		pPush += 3;
        *((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;
		tf_UV.u = 0;//TexUBase+TexUMax;//1.0f;
		
		_mm_storeu_ps((float*)pPush, Position4);//a reinverser
		pPush += 3;
		*((D3DCOLOR*)pPush) = DiffuseCol;
        pPush += 1;
		*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
		pPush += 2;
		}*/
#else
			//_mm_storeu_ps(store, Position2);
/*			_mm_storeu_ps(pData, Position2);
			pData+=3;
/*			*pData =  0;//store[0];
			pData++;
			*pData =  0;//store[1];
			pData++;
			*pData =  0;//store[2];
			pData++;*/

/*			*((D3DCOLOR*)pData) = DiffuseCol;
			pData += 1;
			*((D3DXVECTOR2*)pData)=(*(D3DXVECTOR2 *)&tf_UV);
			pData += 2;
*/
			_mm_storeu_ps(MonStream, Position2);
			//MonStream[0].x =  store[0];
			//MonStream[0].y =  store[1];
			//MonStream[0].z =  store[2];
			MonStream[0].Color = MonStream[1].Color = MonStream[2].Color = MonStream[3].Color = DiffuseCol;
			MonStream[0].u = tf_UV.u;
			MonStream[0].v = tf_UV.v;

			/*if (bTest)
			{
			_mm_storeu_ps(MonStream+4, Position4);
			MonStream[4].Color = MonStream[1].Color = MonStream[2].Color = MonStream[3].Color = DiffuseCol;
			MonStream[4].u = tf_UV.u;
			MonStream[4].v = tf_UV.v;
			}*/

			tf_UV.u = TexUBase+TexUMax;

			//_mm_storeu_ps(store, Position);
/*			_mm_storeu_ps(pData, Position);
			pData+=3;
/*
			*pData =  0;//store[0];
			pData++;
			*pData =  100;//store[1];
			pData++;
			*pData =  0;//store[2];
			pData++;*/

/*			*((D3DCOLOR*)pData) = DiffuseCol;
			pData += 1;
			*((D3DXVECTOR2*)pData)=(*(D3DXVECTOR2 *)&tf_UV);
			pData += 2;
*/
			_mm_storeu_ps(MonStream+1, Position);
			MonStream[1].Color = DiffuseCol;
			MonStream[1].u = tf_UV.u;
			MonStream[1].v = tf_UV.v;

			/*if (bTest)
			{
			_mm_storeu_ps(MonStream+5, Position3);
			MonStream[5].Color = DiffuseCol;
			MonStream[5].u = tf_UV.u;
			MonStream[5].v = tf_UV.v;
			}*/

			tf_UV.v += DeltaU;

		Position2 = _mm_add_ps(stNormale , Position2 );
		Position = _mm_add_ps(stNormale , Position );
		/*if (bTest)
		{
			Position3 = _mm_add_ps(stNormale , Position3 );//p
			Position4 = _mm_add_ps(stNormale , Position4 );//p
		}*/
		tf_UV.u = TexUBase;//0.0f;

			//_mm_storeu_ps(store, Position2);
/*			_mm_storeu_ps(pData, Position2);
			pData+=3;
/*
			*pData =  100;//store[0];
			pData++;
			*pData =  0;//store[1];
			pData++;
			*pData =  0;//store[2];
			pData++;*/

/*     *((D3DCOLOR*)pData) = DiffuseCol;
        pData += 1;
		*((D3DXVECTOR2*)pData)=(*(D3DXVECTOR2 *)&tf_UV);
		pData += 2;
*/
			_mm_storeu_ps(MonStream+2, Position2);
			MonStream[2].Color = DiffuseCol;
			MonStream[2].u = tf_UV.u;
			MonStream[2].v = tf_UV.v;

			/*if (bTest)
			{
			_mm_storeu_ps(MonStream+6, Position3);
			MonStream[6].Color = DiffuseCol;
			MonStream[6].u = tf_UV.u;
			MonStream[6].v = tf_UV.v;
			}*/

			tf_UV.u = TexUBase+TexUMax;

//			_mm_storeu_ps((float*)pData, Position);
			/*pData[0]=10.f+(ulnumberOfPoints*10);
			pData[1]=10.f;
			pData[2]=0.f;*/
//			pData+=3;
/*			*((D3DCOLOR*)pData) = DiffuseCol;
			pData += 1;
			*((D3DXVECTOR2*)pData)=(*(D3DXVECTOR2 *)&tf_UV);
			pData += 2;
*/
			_mm_storeu_ps(MonStream+3, Position);//inv
			MonStream[3].Color = DiffuseCol;
			MonStream[3].u = tf_UV.u;
			MonStream[3].v = tf_UV.v;

			/*if (bTest)
			{
			_mm_storeu_ps(MonStream+7, Position4);//inv
			MonStream[7].Color = DiffuseCol;
			MonStream[7].u = tf_UV.u;
			MonStream[7].v = tf_UV.v;
			}*/

			//_mm_storeu_ps(store, Position);
/*			_mm_storeu_ps(pData, Position);
			pData+=3;
			/**pData =  100;//store[0];
			pData++;
			*pData =  100;//store[1];
			pData++;
			*pData =  0;//store[2];
			pData++;*/

#endif
			tf_UV.v += DeltaU;

#ifdef UsePushc

				*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
				*pPush++ = 0;
			
#else
			/*if (bTest)
			IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_QUADLIST,2,(void*)MonStream,uiVStride);
			else*/
			IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);

#endif
 	}

#ifdef UsePushc

	IDirect3DDevice8_EndPush( pst_SD->mp_D3DDevice,pPush );
#else
	//IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
	
//	IDirect3DVertexBuffer8_Lock( pst_SD->VBforSPG2.Spg2VB, 0, 0, (void *) &pData, 0 );

    //IDirect3DVertexBuffer8_Lock( pst_SD->VBforSPG2.Spg2VB, 0, 0, (BYTE **) &pDodo, D3DLOCK_NOOVERWRITE );   // D3DLOCK_NOOVERWRITE
    //memcpy( pDodo, pData, 100 );
//	IDirect3DVertexBuffer8_Lock( pst_SD->zouege, 0, 0, &pData, 0  );//D3DLOCK_NOOVERWRITE//D3DLOCK_READONLY

	//IDirect3DVertexBuffer8_Unlock( pst_SD->zouege );

//	IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, pst_SD->zouege, sizeof(Gx8VertexComponent_Position) );

	//	IDirect3DDevice8_DrawPrimitive( pst_SD->mp_D3DDevice, D3DPT_QUADLIST, pst_SD->VBforSPG2.Spg2VB, 4 );
//	IDirect3DDevice8_DrawPrimitive( pst_SD->mp_D3DDevice, D3DPT_QUADLIST, 0, sulnumberOfPoints );

/*
	IDirect3DDevice8_DrawIndexedPrimitive( pst_SD->mp_D3DDevice, 
																D3DPT_TRIANGLESTRIP,
																0,
																0,
																0,
																40 );
      */     

#endif
   
//	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
//	IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

}

void Gx8_l_DrawSPG2_2X(
	SOFT_tdst_AVertex				        *Coordinates,
	ULONG								*pColors,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	float								fTrapeze,
	float								fEOHP,
	float								fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind,
	SPG2_InstanceInforamtion			*p_stII
)
{
	
	Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

#ifndef UsePush

	struct MonStream_
	{
		float x,y,z;
		DWORD Color;
		float u,v;
	} MonStream[14];//7 segments * 2 points for the moment...
	UINT uiVStride;
#endif

	SOFT_tdst_UV	tf_UV;
	ULONG BM;
	__m128 GlobalPos,GlobalZADD,GlobalScale ,InterpolatorIntensity,One_One_One_One;
	float DeltaU;
	float fTrapezeDelta,fTrapezeInc,fOoNumOfSeg;

	ULONG Counter;
	
	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	float TexUMax,TexVMax;
	float TexUBase,TexVBase;

	ULONG DiffuseCol;

#ifdef UsePush

	DWORD	dwVertexSize = sizeof(D3DXVECTOR3) + sizeof(D3DCOLOR)+sizeof(D3DXVECTOR2);
	DWORD   dwNumVertices;
    DWORD   dwPushSizeInBytes;
    DWORD   dwPushSizeInDWORDs;
	DWORD* pPush;
#endif
	//_GSP_BeginRaster(50);

	//WriteStringEx(L"Exception Handler installed", 10.0f, 10.0f, 0xffffffff);
	//_GSP_BeginRaster(49);

	TexUBase = TexVBase = 0.0f;

	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	/* Fast One Over 2^N */
	*(u32 *)&TexUMax = (127 - U_SHIFT) << 23;
	*(u32 *)&TexVMax = (127 - V_SHIFT) << 23;

	//ulnumberOfPoints *= 0.8f;
	//if ( ulNumberOfSegments >2 ) ulNumberOfSegments = 2;

	BM = 0;
	fOoNumOfSeg = 1.0f / (float)ulNumberOfSegments;
	fRatio *= 0.5f;

	//DeltaU = (float)TileNumber * fOoNumOfSeg;
	
	DeltaU = TexVMax*(float)TileNumber * fOoNumOfSeg;

	fTrapezeDelta = -fTrapeze * fOoNumOfSeg;
	//if ( ulnumberOfPoints <<2 )
	//ulnumberOfPoints *= (ULONG)0.8f;
#ifdef UsePush
	if ( ulMode == 2 ) dwNumVertices      = 2*2;//*(ulnumberOfPoints);
	else	
	dwNumVertices      = 2*(ulNumberOfSegments+1);//*ulnumberOfPoints;
	
	dwPushSizeInBytes  = dwVertexSize * dwNumVertices;
	dwPushSizeInDWORDs = dwPushSizeInBytes / sizeof(DWORD);
#endif
 	if (ulMode == 2) return ; // DrawH
	GlobalPos = _mm_loadu_ps((float*)&p_stII->GlobalPos);
	GlobalZADD = _mm_loadu_ps((float*)&p_stII->GlobalZADD);
	GlobalScale = _mm_set_ps1(p_stII->GlobalSCale );
	One_One_One_One = _mm_set_ps1(1.0f);
#ifdef UsePush	
//	IDirect3DDevice8_BeginPush( pst_SD->mp_D3DDevice,dwPushSizeInDWORDs + 6*ulnumberOfPoints*4*100, &pPush );//perturbe le frame
	IDirect3DDevice8_BeginPush( pst_SD->mp_D3DDevice,(6*ulnumberOfPoints)+(ulnumberOfPoints*(ulNumberOfSegments+1)*dwVertexSize), &pPush );

#endif	
	while (ulnumberOfPoints--)
	{
	//_GSP_BeginRaster(50);
		
		__m128 Position,Position2;
		__m128 Xa;
		__m128 Ya;
		__m128 stNormale;
		__m128 stGravity;
		__m128 Local_01;
		__m128 Local_02;
		__m128 Local_03;
		__m128 Local_04;
		__m128 Interpolator;
		__m128 Derivative;

//		float store[4];
		SHORT count = 0;

		if (p_stII->BaseAnimUv)
		{
			TexUBase = (float)(TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * TexUMax;
			TexVBase = (float)((TextureTilerUV_Base >> U_SHIFT)^0xf) * TexVMax;
			TextureTilerUV_Base += 3;
		}
	
		tf_UV.u = TexUBase;
		tf_UV.v = TexVBase;
		
 		Position =	_mm_load_ps((float*)(Coordinates ++));// Pos
		Xa =		_mm_load_ps((float*)(Coordinates ++));// Pos
		Ya =		_mm_load_ps((float*)(Coordinates ));// Pos
		Local_01 = _mm_set_ps1((Coordinates++)->w );
		stNormale = _mm_load_ps((float*)(Coordinates ));// Pos
		stGravity = _mm_load_ps((float*)&pWind[Coordinates->c ^ p_stII->BaseWind ]);// Pos
		Local_03 = _mm_set_ps1(pWind[Coordinates->c ^ p_stII->BaseWind ].w );
		Coordinates ++;

		Position =  _mm_add_ps(Position , GlobalPos);
		stNormale = _mm_add_ps(stNormale , GlobalZADD);
		Local_02 = _mm_set_ps1(fOoNumOfSeg );
		Local_04 = _mm_mul_ps(Local_01 , Local_02 );
		InterpolatorIntensity = _mm_mul_ps(Local_02, Local_03);
		stNormale = _mm_mul_ps(stNormale , GlobalScale);

		Interpolator = _mm_sub_ps(One_One_One_One, One_One_One_One);

		Counter = ulNumberOfSegments + 1;
		fTrapezeInc = 	fTrapeze ; 

		
		stNormale = _mm_mul_ps(stNormale , Local_04 );
		stGravity  = _mm_mul_ps(stGravity , Local_04 );

		Local_04 = _mm_set_ps1(fRatio);
		Local_01 = _mm_mul_ps(Local_01 , Local_04 );

		if (ulMode == 1) // DrawY
		{
			Xa = Ya;
		}
		Xa = _mm_mul_ps(Xa , Local_01 );
		Position2 = _mm_add_ps(Position , Xa);
		Position = _mm_sub_ps(Position , Xa);
#ifdef UsePush
		*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
		*pPush++ = D3DPT_TRIANGLESTRIP;
        *pPush++ = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG|D3DPUSH_INLINE_ARRAY, dwPushSizeInDWORDs );
#endif		
		if (p_stII->GlobalColor) 
		{
			DiffuseCol = Gx8_M_ConvertColor(p_stII->GlobalColor) ;
 
		}
		else
		{
			ULONG DiffuseTemp=*pColors++;
			DiffuseCol = Gx8_M_ConvertColor(DiffuseTemp);
		}
#ifndef UsePush		
		IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLESTRIP );
#endif
		while (	Counter -- )
		{
			tf_UV.u = TexUBase;
#ifdef UsePush
			_mm_storeu_ps((float*)pPush, Position2);
			pPush += 3;
            *((D3DCOLOR*)pPush) = DiffuseCol;
            pPush += 1;
			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
			pPush += 2;
			tf_UV.u = TexUBase+TexUMax;//1.0f;
			
			_mm_storeu_ps((float*)pPush, Position);
			pPush += 3;
			*((D3DCOLOR*)pPush) = DiffuseCol;
            pPush += 1;
			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
			pPush += 2;
#endif			
#ifndef UsePush
			_mm_storeu_ps(store, Position2);
			IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, store[0], store[1], store[2], 1.0f );
			IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, DiffuseCol );
			IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, tf_UV.u , tf_UV.v);
/*			MonStream[0+count].x =  store[0];
			MonStream[0+count].y =  store[1];
			MonStream[0+count].z =  store[2];
			MonStream[0+count].Color = DiffuseCol;
			MonStream[0+count].u = tf_UV.u;
			MonStream[0+count].v = tf_UV.v;
*/
			tf_UV.u = TexUBase+TexUMax;

			_mm_storeu_ps(store, Position);
			IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, store[0], store[1], store[2], 1.0f );
			IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, DiffuseCol );
			IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, tf_UV.u , tf_UV.v);
/*			MonStream[1+count].x =  store[0];
			MonStream[1+count].y =  store[1];
			MonStream[1+count].z =  store[2];
			MonStream[1+count].Color = DiffuseCol;
			MonStream[1+count].u = tf_UV.u;
			MonStream[1+count].v = tf_UV.v;
			count+=2;
*/
#endif
			tf_UV.v += DeltaU;

			Derivative = _mm_mul_ps(Interpolator , stGravity );
			Local_01  = _mm_sub_ps(One_One_One_One , Interpolator);
			Local_01  = _mm_mul_ps(stNormale , Local_01 );
			Derivative = _mm_add_ps(Local_01 , Derivative);

			Position2 = _mm_add_ps(Derivative , Position2 );
			Position = _mm_add_ps(Derivative , Position );
			Interpolator = _mm_add_ps(Interpolator,InterpolatorIntensity);
			Interpolator = _mm_min_ps(Interpolator,One_One_One_One);

		}
#ifdef UsePush
			*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
			*pPush++ = 0;
#else
			IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
#endif		
			//IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2*ulNumberOfSegments,(void*)MonStream,uiVStride);

	}
#ifdef UsePush
			
			IDirect3DDevice8_EndPush( pst_SD->mp_D3DDevice,pPush );
#endif
}


_inline_ ULONG Gx8_ScaleColor(float Factor, ULONG C0 )
{
	ULONG X;
	ULONG CX ;
	ULONG CXp ;
	X = (ULONG)(Factor * 255.0f);
	CX = C0 & 0xff00ff;
	CXp = (C0>>8) & 0xff00ff;
	CX *= X;
	CXp *= X;

	CX &= 0xff00ff00;
	CXp &= 0x0000ff00;
	CXp |= CX >> 8;
	CXp |= C0 & 0xff000000;

	return CXp;
	
}

void Gx8_l_DrawSPG2_SPRITES_2X(
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

	Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

		DWORD	dwVertexSize       = sizeof(D3DXVECTOR3) + sizeof(D3DCOLOR)+sizeof(D3DXVECTOR2);
        DWORD    dwNumVertices;
        DWORD   dwPushSizeInBytes;
        DWORD   dwPushSizeInDWORDs;
		DWORD* pPush;
		/*v buf DWORD number = 0;
		DWORD totalnumber;*/

	ULONG BM;
	ULONG Counter;
	float OoNSPR;
	float ComplexRadius_RE;
	float ComplexRadius_IM;
	float ComplexRadius_LOCAL;
	SOFT_tdst_AVertex	XCam,YCam;
	BM = 0;

	*(MATH_tdst_Vector *)&XCam = *XCamera;
	*(MATH_tdst_Vector *)&YCam = *YCamera;

	_GSP_BeginRaster(50);


	ComplexRadius_IM = 1.0f;
	ComplexRadius_RE = 0.0f;

	ulNumberOfSprites &= 63;
	if (!ulNumberOfSprites) return;
	OoNSPR = 1.0f / (float)ulNumberOfSprites;

	//Gx8_vSetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	//IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_QUADLIST );

			dwNumVertices      = 4*(ulNumberOfSprites);//*(ulnumberOfPoints);	
			dwPushSizeInBytes  = dwVertexSize * dwNumVertices;
			dwPushSizeInDWORDs = dwPushSizeInBytes / sizeof(DWORD);


		//IDirect3DDevice8_BeginPush( pst_SD->mp_D3DDevice,dwPushSizeInDWORDs + 12000, &pPush );
		IDirect3DDevice8_BeginPush( pst_SD->mp_D3DDevice,(6*ulnumberOfPoints)+(ulnumberOfPoints*(ulNumberOfSprites+1)*dwVertexSize), &pPush );

	while (ulnumberOfPoints--)
	{
		SOFT_tdst_AVertex	VC,Xa,Ya,Za,LocalX,LocalY,stGravity;
		float	Interpolator,InterpolatorIntensity ,fdEPT;
		ULONG DiffuseColor;
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


		if (p_stII->GlobalColor) 
			DiffuseColor = p_stII->GlobalColor;
		else
			DiffuseColor = *pColors++;

		Counter = ulNumberOfSprites;


		Interpolator = fEOHP * (float)ulNumberOfSprites * InterpolatorIntensity;
		fdEPT = 0.0f;
		
			*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
			*pPush++ = D3DPT_QUADLIST;
            *pPush++ = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG|D3DPUSH_INLINE_ARRAY, dwPushSizeInDWORDs );

		while (	Counter -- )
		{
		
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			static float	tf_UV[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
			MATH_tdst_Vector Cx,C0,C1,C2,C3,stDerivative;
			ULONG			LocalColor;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			LocalColor = Gx8_ScaleColor(0.5f + fdEPT * 0.5f , DiffuseColor);
			LocalColor = Gx8_M_ConvertColor(LocalColor);
			fdEPT += OoNSPR;

			MATH_AddScaleVector(&Cx , (MATH_tdst_Vector *)&VC , (MATH_tdst_Vector *)&Xa , ComplexRadius_IM);
			MATH_SubScaleVector(&C0,&Cx,(MATH_tdst_Vector *)&XCam,0.5f);
			MATH_SubScaleVector(&C0,&C0,(MATH_tdst_Vector *)&YCam,0.5f);

			MATH_AddVector(&C1,&C0,(MATH_tdst_Vector *)&XCam);
			MATH_AddVector(&C2,&C1,(MATH_tdst_Vector *)&YCam);
			MATH_SubVector(&C3,&C2,(MATH_tdst_Vector *)&XCam);

			
			*((D3DXVECTOR3*)pPush)=(*(D3DXVECTOR3 *)&C0);
			pPush += 3;

            *((D3DCOLOR*)pPush) = LocalColor;
            pPush += 1;

			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV[3]);
			pPush += 2;

			*((D3DXVECTOR3*)pPush)=(*(D3DXVECTOR3 *)&C1);
			pPush += 3;

            *((D3DCOLOR*)pPush) = LocalColor;
            pPush += 1;

			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV[2]);
			pPush += 2;

			*((D3DXVECTOR3*)pPush)=(*(D3DXVECTOR3 *)&C2);
			pPush += 3;

            *((D3DCOLOR*)pPush) = LocalColor;
            pPush += 1;

			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV[1]);
			pPush += 2;

			*((D3DXVECTOR3*)pPush)=(*(D3DXVECTOR3 *)&C3);
			pPush += 3;

            *((D3DCOLOR*)pPush) = LocalColor;
            pPush += 1;

			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV[0]);
			pPush += 2;
			
		

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

	//IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
	*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
	*pPush++ = 0;

	}
	IDirect3DDevice8_EndPush( pst_SD->mp_D3DDevice,pPush );
	
	_GSP_EndRaster(50);

}
void Gx8_l_DrawSPG2_2Xa(
	SOFT_tdst_AVertex				        *Coordinates,
	ULONG								*pColors,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	float								fTrapeze,
	float								fEOHP,
	float								fRatio,
	ULONG								TileNumber,
	ULONG								ulMode,
	SOFT_tdst_AVertex			        *pWind,
	SPG2_InstanceInforamtion			*p_stII
)
{
	
	Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);
	
	SOFT_tdst_UV	tf_UV;
	ULONG BM;
	float DeltaU;
	float fTrapezeDelta,fTrapezeInc,fOoNumOfSeg;

	ULONG Counter;
	
	u32 TextureTilerUV_Base,U_SHIFT,V_SHIFT;
	float TexUMax,TexVMax;
	float TexUBase,TexVBase;

	ULONG DiffuseCol;

	SOFT_tdst_AVertex	u_4Vert[4];
	SOFT_tdst_AVertex	u_4Vert_Trpz[4];
	SOFT_tdst_AVertex	stTrpeze;
	SOFT_tdst_AVertex	stNormale;
	SOFT_tdst_AVertex	stGravity;
	SOFT_tdst_AVertex	stDerivative;
	float				Interpolator;

#ifdef UsePusha
	DWORD	dwVertexSize = sizeof(D3DXVECTOR3) + sizeof(D3DCOLOR)+sizeof(D3DXVECTOR2);
	DWORD   dwNumVertices;
    DWORD   dwPushSizeInBytes;
    DWORD   dwPushSizeInDWORDs;
	DWORD* pPush;
#endif
	//_GSP_BeginRaster(50);

	//WriteStringEx(L"Exception Handler installed", 10.0f, 10.0f, 0xffffffff);
	//_GSP_BeginRaster(49);

	TexUBase = TexVBase = 0.0f;
	//ulnumberOfPoints *= 0.5f;

	TextureTilerUV_Base = (p_stII->BaseAnimUv >> 16) & 0xff;
	U_SHIFT = (p_stII->BaseAnimUv >> 24) & 0xf;
	V_SHIFT = (p_stII->BaseAnimUv >> 28) & 0xf;

	/* Fast One Over 2^N */
	*(u32 *)&TexUMax = (127 - U_SHIFT) << 23;
	*(u32 *)&TexVMax = (127 - V_SHIFT) << 23;

	BM = 0;
	fOoNumOfSeg = 1.0f / (float)ulNumberOfSegments;

	DeltaU = TexVMax*(float)TileNumber * fOoNumOfSeg;
	fTrapezeDelta = -fTrapeze * fOoNumOfSeg;

#ifdef UsePusha
	if ( ulMode == 2 ) dwNumVertices      = 2*2;//*(ulnumberOfPoints);
	else	
	dwNumVertices      = 2*(ulNumberOfSegments+1);//*ulnumberOfPoints;
	
	dwPushSizeInBytes  = dwVertexSize * dwNumVertices;
	dwPushSizeInDWORDs = dwPushSizeInBytes / sizeof(DWORD);
	
	IDirect3DDevice8_BeginPush( pst_SD->mp_D3DDevice,(6*ulnumberOfPoints)+(ulnumberOfPoints*(ulNumberOfSegments+1)*dwVertexSize), &pPush );

#endif

	while (ulnumberOfPoints--)
	{
		if (p_stII->BaseAnimUv)
		{
			TexUBase = (float)(TextureTilerUV_Base & ((1<<U_SHIFT)-1)) * TexUMax;
			TexVBase = (float)((TextureTilerUV_Base >> U_SHIFT)^0xf) * TexVMax;
			TextureTilerUV_Base += 3;
		}
	
		tf_UV.u = TexUBase;
		tf_UV.v = TexVBase;

		u_4Vert[0] = *(Coordinates ++);// Pos

		MATH_AddVector((MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[0],&p_stII->GlobalPos);
		
		u_4Vert[1] = *(Coordinates ++);// Xa 
		u_4Vert[2] = *(Coordinates ++);// Ya

		stNormale = *(Coordinates ++);

		MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		
		//_GSP_EndRaster(50);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);

		Counter = ulNumberOfSegments + 1;
		Interpolator = 0.0f;
		fTrapezeInc = 	fTrapeze ; 

		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , u_4Vert[2].w);// * fOoNumOfSeg);//remet
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stGravity , u_4Vert[2].w);// * fOoNumOfSeg);//remet

		if (ulMode == 0) // DrawX
		{
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[1],-0.5f * u_4Vert[2].w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert[1],(MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[1],u_4Vert[2].w * fRatio);
		}

		else if (ulMode == 1) // DrawY
		{
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[2],-0.5f * u_4Vert[2].w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert[1],(MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[2],u_4Vert[2].w * fRatio);
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
			
//			MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * InterpolatorIntensity  * fEOHP * fEOHP / 2.0f);
			MATH_ScaleEqualVector( (MATH_tdst_Vector *)&stTrpeze  , (float)ulNumberOfSegments * fEOHP * fEOHP / 2.0f);
			MATH_AddScaleVector( (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stTrpeze  , (MATH_tdst_Vector *)&stNormale , fEOHP);
			MATH_AddScaleVector( (MATH_tdst_Vector *)&u_4Vert[0] , (MATH_tdst_Vector *)&u_4Vert[0], (MATH_tdst_Vector *)&stTrpeze , 1);
			DeltaU = TexVMax;//1.0f;
			Counter = 2;
			fTrapeze = 0.0f;
			MATH_ScaleVector((MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)& u_4Vert[2], u_4Vert[2].w * fRatio);
			
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[1],-0.5f * u_4Vert[2].w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[2],-0.5f * u_4Vert[2].w * fRatio);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert[1],(MATH_tdst_Vector *)&u_4Vert[0],(MATH_tdst_Vector *)&u_4Vert[1],u_4Vert[2].w * fRatio);
		}

#ifdef UsePusha
		*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
		*pPush++ = D3DPT_TRIANGLESTRIP;
        *pPush++ = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG|D3DPUSH_INLINE_ARRAY, dwPushSizeInDWORDs );
#endif

		if (p_stII->GlobalColor) 
		{
			DiffuseCol = Gx8_M_ConvertColor(p_stII->GlobalColor) ;
 
		}
		else
		{
			ULONG DiffuseTemp=*pColors++;
			DiffuseCol = Gx8_M_ConvertColor(DiffuseTemp);
		}

		//Direct3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLESTRIP );
		
		while (	Counter -- )
		{
			MATH_SubVector((MATH_tdst_Vector *)&stTrpeze , (MATH_tdst_Vector *)&u_4Vert[0] , (MATH_tdst_Vector *)&u_4Vert[1]);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz[0] , (MATH_tdst_Vector *)&u_4Vert[0] , (MATH_tdst_Vector *)&stTrpeze , fTrapezeInc);
			MATH_AddScaleVector((MATH_tdst_Vector *)&u_4Vert_Trpz[1] , (MATH_tdst_Vector *)&u_4Vert[1] , (MATH_tdst_Vector *)&stTrpeze , -fTrapezeInc);
			fTrapezeInc += fTrapezeDelta;
			tf_UV.u = TexUBase;//0.0f;
#ifdef UsePusha
			*((D3DXVECTOR3*)pPush)=(*(D3DXVECTOR3 *)&u_4Vert_Trpz[0]);
			pPush += 3;

            *((D3DCOLOR*)pPush) = DiffuseCol;
            pPush += 1;

			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
			pPush += 2;

			tf_UV.u = TexUBase+TexUMax;//1.0f;

			*((D3DXVECTOR3*)pPush)=(*(D3DXVECTOR3 *)&u_4Vert_Trpz[1]);
			pPush += 3;

			*((D3DCOLOR*)pPush) = DiffuseCol;
            pPush += 1;

			*((D3DXVECTOR2*)pPush)=(*(D3DXVECTOR2 *)&tf_UV);
			pPush += 2;

#else
			IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, u_4Vert_Trpz[0].x, u_4Vert_Trpz[0].y, u_4Vert_Trpz[0].z, 1.0f );
			IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, DiffuseCol );
			IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, tf_UV.u , tf_UV.v);
			
			tf_UV.u = TexUBase+TexUMax;
			IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, u_4Vert_Trpz[1].x, u_4Vert_Trpz[1].y, u_4Vert_Trpz[1].z, 1.0f );
			IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, DiffuseCol );
			IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, tf_UV.u , tf_UV.v);
#endif
			tf_UV.v += DeltaU;
			MATH_BlendVector((MATH_tdst_Vector *)&stDerivative , (MATH_tdst_Vector *)&stNormale , (MATH_tdst_Vector *)&stGravity , Interpolator);
			MATH_AddVector((MATH_tdst_Vector *)&u_4Vert[0] , (MATH_tdst_Vector *)&u_4Vert[0] , (MATH_tdst_Vector *)&stDerivative);
			MATH_AddVector((MATH_tdst_Vector *)&u_4Vert[1] , (MATH_tdst_Vector *)&u_4Vert[1] , (MATH_tdst_Vector *)&stDerivative);
		}
#ifdef UsePusha

			*pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
            *pPush++ = 0;
#endif
		//IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
	}
#ifdef UsePusha

			IDirect3DDevice8_EndPush( pst_SD->mp_D3DDevice,pPush );
#endif
}

// ===================================================
#define CosAlpha -0.34202014332566873304409961468226f	
#define SinAlpha 0.9396926207859083840541092773247f		

#define MAX_SEGMENT_NB 6
#define MIN_LOD_SEGMENT_NB 10000
#define MAX_LOD_SEGMENT_NB 20000
#define MAX_LOD_SEGMENT_COEF 0.2f
#define LOD_COEF ((1.f - MAX_LOD_SEGMENT_COEF)/((float)(MIN_LOD_SEGMENT_NB - MAX_LOD_SEGMENT_NB)))

static int 	UsedCachedLinesNb = 0;

void Gx8_l_DrawSPG2(	SPG2_CachedPrimitivs				*pCachedLine,
							ULONG								*ulTextureID,
							GEO_Vertex					        *XCam,
							GEO_Vertex					        *YCam,
							SPG2_tdst_Modifier					*_pst_SPG2 ,
							SOFT_tdst_AVertex			        *pWind,
							SPG2_InstanceInforamtion			*p_stII)
{
    static int iThisFrameDisplayedPointNb = 0;
	static int iLastFrameDisplayedPointNb = 0;
	
	Gx8_tdst_SpecificData   *pst_SD;

	float fExtractionOfHorizontalPlane;
	ULONG BM,Transparency,Transparency2;
	ULONG				ulnumberOfPoints;
	ULONG				NumberOfSegments;

	_GSP_BeginRaster(49);

	ulnumberOfPoints = pCachedLine->a_PtrLA2 >> 2;
	NumberOfSegments = _pst_SPG2->NumberOfSegments;

/*
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
*/

	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ROPZCMPALWAYSREAD, 1/*TRUE*/);
	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_OCCLUSIONCULLENABLE, 0/*TRUE*/);
	
	
	if (NumberOfSegments>>1)
	{
		NumberOfSegments = (ULONG)((float)NumberOfSegments * p_stII->Culling);
		if (NumberOfSegments <= 2) NumberOfSegments = 2;
		else if (NumberOfSegments > MAX_SEGMENT_NB)
		NumberOfSegments = MAX_SEGMENT_NB;
	}
	
	Gx8_vSetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	/* Xaxis lookat ***********************/
	if ((_pst_SPG2->ulFlags & SPG2_XAxisIsInlookat) && (pCachedLine->ulFlags & 2))
	{
		ULONG Counter;
		SOFT_tdst_AVertex	*Coordinates;
		MATH_tdst_Vector	stCameraDir;
		_pst_SPG2->ulFlags &= ~(SPG2_DrawY|SPG2_RotationNoise);
		_pst_SPG2->ulFlags |= SPG2_DrawX;
		pCachedLine->ulFlags &= ~2;
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
	if ((_pst_SPG2->ulFlags & SPG2_RotationNoise) && (pCachedLine->ulFlags & 1))
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

	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

	Gx8_RS_CullFace(pst_SD, 0);
	Gx8_RS_DepthTest(pst_SD, GDI_Cul_DM_ZTest);

	Gx8_RS_Fogged(pst_SD, ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (pst_SD->ulFogState)));
	if (!g_iNewLight) LIGHT_TurnOffObjectLighting();

	MAT_SET_FLAG(BM, MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_AlphaTest | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV);
	MAT_SET_Blending(BM , MAT_Cc_Op_Copy);
	MAT_SET_AlphaTresh(BM , _pst_SPG2->AlphaThreshold);

	fExtractionOfHorizontalPlane = _pst_SPG2->fExtractionOfHorizontalPlane + 0.5f;

	Transparency = MAT_Cc_Op_Copy;
	Transparency2 = MAT_Cc_Op_Add;
	if (_pst_SPG2 ->ulFlags & SPG2_DrawinAlpha)  
	{
		Transparency = MAT_Cc_Op_Alpha;
		Transparency2 = MAT_Cc_Op_Add;
	}

	if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
	{
		MAT_SET_FLAG(BM, MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV | MAT_Cul_Flag_NoZWrite);
		Transparency = MAT_Cc_Op_Add;

	}

	MAT_SET_Blending(BM , Transparency);

	GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );

	if (_pst_SPG2 ->ulFlags & SPG2_DrawHat)
	{
		Gx8_SetTextureBlending(ulTextureID[0], BM,0);
		if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
			Gx8_RS_Fogged( pst_SD, 0);
		Gx8_RS_CullFace(pst_SD, 0); 
		GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );
		Gx8_l_DrawSPG2_2Xa(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,2,pWind,p_stII);
		//Gx8_l_DrawSPG2_2X_OneSegment(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,ulnumberOfPoints,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII);

	}

	if (_pst_SPG2 -> ulFlags & SPG2_DrawY)
	{
		Gx8_SetTextureBlending(ulTextureID[1], BM,0);
		GLI_M_SetTextureAlphaStage(pst_SD->mp_D3DDevice, 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE)
		if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
			Gx8_RS_Fogged( pst_SD, 0);
		Gx8_RS_CullFace(pst_SD, 0);
		GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );

		if (NumberOfSegments == 1)
		{		
			Gx8_l_DrawSPG2_2X_OneSegment(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,ulnumberOfPoints,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII);
		}else
			Gx8_l_DrawSPG2_2X(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII);
	
	}

	if (_pst_SPG2 ->ulFlags & SPG2_DrawX)
	{
		Gx8_SetTextureBlending(ulTextureID[2], BM,0);
		if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
			Gx8_RS_Fogged( pst_SD, 0);
		Gx8_RS_CullFace(pst_SD, 0);
		GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );

		if (NumberOfSegments == 1)
		{
			//if (!bTest)
			Gx8_l_DrawSPG2_2X_OneSegment(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,ulnumberOfPoints,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,1,pWind,p_stII);
		}else
			Gx8_l_DrawSPG2_2X(pCachedLine->a_PointLA2,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,1,pWind,p_stII);
	}

	if (_pst_SPG2 ->ulFlags & SPG2_DrawSprites)
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
		Gx8_SetTextureBlending(ulTextureID[3], BM,0);
		if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
		Gx8_RS_Fogged( pst_SD, 0);
		Gx8_RS_CullFace(pst_SD, 0);
		GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE );
		Gx8_l_DrawSPG2_SPRITES_2X(pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,_pst_SPG2->NumberOfSprites,CA,SA,_pst_SPG2->f_SpriteGeneratorRadius,fExtractionOfHorizontalPlane, _pst_SPG2->f_GlobalRatio ,pWind,p_stII);
	}

	_GSP_EndRaster(49);
}

void Gx8_l_DrawSPG2_Alpha(
	SOFT_tdst_AVertex			        *Coordinates,
	ULONG								*pColors,
	ULONG								ulTextureID,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSegments,
	ULONG								AlphaT,
	float								fTrapeze,
	ULONG								TileNumber,
	ULONG								ulMode
)
{
}

void Gx8_l_DrawSPG2_SPRITES(
	SOFT_tdst_AVertex			        *Coordinates,
	GEO_Vertex					        *XCam,
	GEO_Vertex					        *YCam,
	ULONG								*pColors,
	ULONG								ulTextureID,
	ULONG								ulnumberOfPoints,
	ULONG								ulNumberOfSprites,
	ULONG								AlphaT)
{
/*	OGL_l_DrawSPG2_SPRITES_2X(Coordinates,XCam,YCam,pColors,ulTextureID,ulnumberOfPoints,ulNumberOfSprites,AlphaT,MAT_Cc_Op_Copy);
	OGL_l_DrawSPG2_SPRITES_2X(Coordinates,XCam,YCam,pColors,ulTextureID,ulnumberOfPoints,ulNumberOfSprites,AlphaT,MAT_Cc_Op_Add);*/
}

/**********************************************************************************************************************/
/* SPG2 specific functions END ****************************************************************************************/
/**********************************************************************************************************************/

void Gx8_vChangeMaterialForShadow(float fAlpha)
{
        D3DMATERIAL8 stD3DMaterial;
    
        stD3DMaterial.Diffuse.r  = 0.0f;
        stD3DMaterial.Diffuse.g  = 0.0f;
        stD3DMaterial.Diffuse.b  = 0.0f;
        stD3DMaterial.Diffuse.a  = fAlpha*((float)(0xF0))/256.f; // only write alpha
        stD3DMaterial.Ambient.r  = 0.0f;
        stD3DMaterial.Ambient.g  = 0.0f;
        stD3DMaterial.Ambient.b  = 0.0f;
        stD3DMaterial.Ambient.a  = 0.0f;
        stD3DMaterial.Emissive.r = 0.0f; 
        stD3DMaterial.Emissive.g = 0.0f;
        stD3DMaterial.Emissive.b = 0.0f;
        stD3DMaterial.Emissive.a = 0.0f;
        stD3DMaterial.Specular.r = 0.0f;
        stD3DMaterial.Specular.g = 0.0f;
        stD3DMaterial.Specular.b = 0.0f;
        stD3DMaterial.Specular.a = 0.0f;
        stD3DMaterial.Power      = 0.0f;
        IDirect3DDevice8_SetMaterial(p_gGx8SpecificData->mp_D3DDevice, &stD3DMaterial);
}

void Gx8_vSetNormalMaterial(void)
{
        D3DMATERIAL8 stD3DMaterial;
    
        stD3DMaterial.Diffuse.r  = 1.0f;
        stD3DMaterial.Diffuse.g  = 1.0f;
        stD3DMaterial.Diffuse.b  = 1.0f;
        stD3DMaterial.Diffuse.a  = 0.0f; // otherwise you'll add alpha unwittingly to you RLI alphas
        stD3DMaterial.Ambient.r  = 1.0f;
        stD3DMaterial.Ambient.g  = 1.0f;
        stD3DMaterial.Ambient.b  = 1.0f;
        stD3DMaterial.Ambient.a  = 1.0f;
        stD3DMaterial.Emissive.r = 0.0f; // emissive must be zero, otherwise all would be lighted when no per-vertex color is found in the VB.
        stD3DMaterial.Emissive.g = 0.0f;
        stD3DMaterial.Emissive.b = 0.0f;
        stD3DMaterial.Emissive.a = 0.0f;
        stD3DMaterial.Specular.r = 0.0f;
        stD3DMaterial.Specular.g = 0.0f;
        stD3DMaterial.Specular.b = 0.0f;
        stD3DMaterial.Specular.a = 0.0f;
        stD3DMaterial.Power      = 0.0f;
        IDirect3DDevice8_SetMaterial(p_gGx8SpecificData->mp_D3DDevice, &stD3DMaterial);
}

void Gx8_vBeginRenderUsingTheBigVertexBuffer(UINT uiVertexCount, BYTE **p_vData)
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

    if (pst_SD->m_stBigVBData.m_uiWhereToLockFrom + uiVertexCount > Gx8_C_BigVertexBufferVertexCount)
    {
        pst_SD->m_stBigVBData.m_uiWhereToLockFrom = 0;
    }
    IDirect3DVertexBuffer8_Lock(pst_SD->m_stBigVBData.m_VertexBuffer,
                                pst_SD->m_stBigVBData.m_uiWhereToLockFrom * sizeof(Gx8VertexFormat),
                                uiVertexCount * sizeof(Gx8VertexFormat),
                                p_vData,
//                                pst_SD->m_stBigVBData.m_uiWhereToLockFrom == 0 ? 0 : D3DLOCK_NOOVERWRITE);
                                D3DLOCK_NOOVERWRITE);
    pst_SD->m_stBigVBData.m_uiLockedVerticesCount = uiVertexCount;
}

void Gx8_vEndRenderUsingTheBigVertexBuffer(UINT uiPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD(GDI_gpst_CurDD);

    IDirect3DVertexBuffer8_Unlock(pst_SD->m_stBigVBData.m_VertexBuffer);

    IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, pst_SD->m_stBigVBData.m_VertexBuffer, sizeof(Gx8VertexFormat) );
    Gx8_vSetVertexShader(D3DFVF_VERTEXF);
    IDirect3DDevice8_DrawPrimitive( pst_SD->mp_D3DDevice, ePrimitiveType, pst_SD->m_stBigVBData.m_uiWhereToLockFrom, uiPrimitiveCount );
    
    /*
    {
        UINT uiVertexCount;
        switch (ePrimitiveType)
        {
        case D3DPT_POINTLIST     :
            uiVertexCount = uiPrimitiveCount; break;
        case D3DPT_LINELIST      :
            uiVertexCount = uiPrimitiveCount + 1; break;
        case D3DPT_TRIANGLELIST  :
            uiVertexCount = uiPrimitiveCount * 3; break;
        case D3DPT_QUADLIST      :
            uiVertexCount = uiPrimitiveCount * 4; break;
        case D3DPT_LINELOOP      :
        case D3DPT_LINESTRIP     :
        case D3DPT_TRIANGLESTRIP :
        case D3DPT_TRIANGLEFAN   :
        case D3DPT_QUADSTRIP     :
        case D3DPT_POLYGON       :
        default:
            uiVertexCount = 0; break;
        };

        if (uiVertexCount != pst_SD->m_stBigVBData.m_uiLockedVerticesCount)
        {
            _asm int 3;
        }
    }
    */

    pst_SD->m_stBigVBData.m_uiWhereToLockFrom += pst_SD->m_stBigVBData.m_uiLockedVerticesCount;
}


/*$4
 ***********************************************************************************************************************
    Private function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Set Device Context pixel format
 =======================================================================================================================
 */
void Gx8_SetDCPixelFormat(HDC _hDC)
{
}

/*
 =======================================================================================================================
    Aim:    Setup rendering context
 =======================================================================================================================
 */
void Gx8_SetupRC(Gx8_tdst_SpecificData *_pst_SD)
{
    /*~~~~~~~~~*/
    LONG    w, h;
    /*~~~~~~~~~*/

    _pst_SD->RenderScene = FALSE;
    w = _pst_SD->rcViewportRect.right - _pst_SD->rcViewportRect.left;
    h = _pst_SD->rcViewportRect.bottom - _pst_SD->rcViewportRect.top;
    Gx8_Viewport(_pst_SD, 0, (h - w) / 2, w, h);

    /* Reset coordinate system */

    /* Establish clipping volume (left, right, bottom, top, near, far) */
    _pst_SD->pst_ProjMatrix->Jy = -1.0f;
    _pst_SD->pst_ProjMatrix->Sz = 1.0f;
    _pst_SD->pst_ProjMatrix->T.z = -0.1f;
    _pst_SD->pst_ProjMatrix->w = 0.0f;  /* MATRIX W! */
}

/*
 =======================================================================================================================
    Aim:    draw easily a rectangle, without VB... maybe useful to optimize ?
    depend on what will use this function. 
    For the moment : rasters, menus and AfterFX.
 =======================================================================================================================
*/
void Gx8_DrawRectangle(void *Dev,float x, float y, float w, float h,u_int Color)
{
struct MonStream_ {
    float x,y,z,rhw;
    DWORD Color;
} MonStream[4];
HRESULT hr;

UINT uiVStride;
LPDIRECT3DDEVICE8 Device = (LPDIRECT3DDEVICE8)Dev;

    Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

    if (w<=0.f)
        return;

    MonStream[0].x = x;
    MonStream[0].y = y+h;
    MonStream[0].z = 0.0f;
    MonStream[0].rhw = 0.99999f;
    MonStream[0].Color = (DWORD)Color;

    MonStream[1].x = x;
    MonStream[1].y = y;
    MonStream[1].z = 0.0f;
    MonStream[1].rhw = 0.99999f;
    MonStream[1].Color = (DWORD)Color;

    MonStream[2].x = x+w;
    MonStream[2].y = y+h;
    MonStream[2].z = 0.0f;
    MonStream[2].rhw = 0.99999f;
    MonStream[2].Color = (DWORD)Color;

    MonStream[3].x = x+w;
    MonStream[3].y = y;
    MonStream[3].z = 0.0f;
    MonStream[3].rhw = 0.99999f;
    MonStream[3].Color = (DWORD)Color;

    hr = IDirect3DDevice8_DrawPrimitiveUP(Device,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
}
void Gx8_DrawRectangle2(void *Dev,float x, float y, float w, float h,u_int Color,u_int Color2)
{
struct MonStream_ {
    float x,y,z,rhw;
    DWORD Color;
} MonStream[4];
HRESULT hr;

UINT uiVStride;
LPDIRECT3DDEVICE8 Device = (LPDIRECT3DDEVICE8)Dev;

    Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

    if (w<=0.f)
        return;

    MonStream[0].x = x;
    MonStream[0].y = y+h;
    MonStream[0].z = 0.0f;
    MonStream[0].rhw = 0.99999f;
    MonStream[0].Color = (DWORD)Color;

    MonStream[1].x = x;
    MonStream[1].y = y;
    MonStream[1].z = 0.0f;
    MonStream[1].rhw = 0.99999f;
    MonStream[1].Color = (DWORD)Color;

    MonStream[2].x = x+w;
    MonStream[2].y = y+h;
    MonStream[2].z = 0.0f;
    MonStream[2].rhw = 0.99999f;
    MonStream[2].Color = (DWORD)Color2;

    MonStream[3].x = x+w;
    MonStream[3].y = y;
    MonStream[3].z = 0.0f;
    MonStream[3].rhw = 0.99999f;
    MonStream[3].Color = (DWORD)Color2;

    hr = IDirect3DDevice8_DrawPrimitiveUP(Device,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
}
void
Gx8_Before2D( void )
{
    Render3do=false;
	Gx8Global_set( f2DFFX, Gx8Global_get(f2DFFX_A2D));
    Gx8Global_set( f2DFFY, Gx8Global_get(f2DFFY_A2D));
}

// directx initialization before light POV rendering 
void 
Gx8_ShadowBuffer_LightRender_Init(Gx8_tdst_SpecificData *pst_SD)
{
    float fZOffset=4.0f;
    float fSlopeScale=2.0f;
    IDirect3DSurface8 *pShadowDepthSurface;
    IDirect3DSurface8 *pFakeRenderTarget;

    IDirect3DDevice8_GetRenderTarget( pst_SD->mp_D3DDevice, &pst_SD->pSaveRenderTarget );
    IDirect3DDevice8_GetDepthStencilSurface( pst_SD->mp_D3DDevice, &pst_SD->pSaveZBuffer );

    IDirect3DTexture8_GetSurfaceLevel
    (
        pst_SD->pShadowBufferDepthTexture,
        0,
        &pShadowDepthSurface
    );


    IDirect3DTexture8_GetSurfaceLevel
    (
        pst_SD->pTmpFrameBufferTexture,
        0,
        &pFakeRenderTarget
    );


    IDirect3DDevice8_SetRenderTarget
    (
        pst_SD->mp_D3DDevice,
        pFakeRenderTarget,
        pShadowDepthSurface
    );

    //IDirect3DDevice8_SetViewport( pst_SD->mp_D3DDevice,&viewport );

    //IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0xff000000, 1.0f, 0 );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SOLIDOFFSETENABLE, TRUE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_POLYGONOFFSETZOFFSET, (*(DWORD*)&fZOffset) );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_POLYGONOFFSETZSLOPESCALE, (*(DWORD*)&fSlopeScale) );
    //IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_CW );

    // Don't write to the color buffer.
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_COLORWRITEENABLE, 0 );
    bDisableColors=true;

    IDirect3DSurface8_Release(pFakeRenderTarget);
    IDirect3DSurface8_Release(pShadowDepthSurface);


}

// directx initialization before camera POV rendering (with shadows)
// texture stage state 1: shadow buffer
// texture stage state 2: circle
void 
Gx8_ShadowBuffer_CameraRender_Init(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *pRenderTexture)
{
    IDirect3DSurface8 *pRenderTarget;

    IDirect3DTexture8_GetSurfaceLevel
    (
        pRenderTexture,
        0,
        &pRenderTarget
    );

    IDirect3DDevice8_SetRenderTarget
    (
        pst_SD->mp_D3DDevice,
        pRenderTarget,
        pst_SD->pSaveZBuffer
    );

    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0xff000000, 1.0f, 0 );
    //IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    bDisableColors=false;
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_COLORWRITEENABLE,  D3DCOLORWRITEENABLE_ALL );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SOLIDOFFSETENABLE, FALSE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE);

    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SHADOWFUNC, D3DCMP_GREATEREQUAL );

    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 1, (D3DBaseTexture*)pst_SD->pShadowBufferDepthTexture );
    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 2, (D3DBaseTexture*)pst_SD->pCircleTexture );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_BORDERCOLOR, 0xffffffff );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_BORDERCOLOR, 0x00000000 );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC  );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC  );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC  );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC  );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_COLOROP, D3DTOP_MODULATE );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLOROP, D3DTOP_MODULATE );
//  IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLOROP, D3DTOP_DISABLE );

    IDirect3DSurface8_Release(pRenderTarget);
}

// reset directx state after the final rendering
void 
Gx8_ShadowBuffer_ResetRenderStates(Gx8_tdst_SpecificData *pst_SD)
{
    IDirect3DDevice8_SetRenderTarget
    (
        pst_SD->mp_D3DDevice,
        pst_SD->pSaveRenderTarget,
        pst_SD->pSaveZBuffer
    );

    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0x00000000, 1.0f, 0 );

    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 1, NULL );
    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 2, NULL );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SHADOWFUNC, D3DCMP_NEVER );
}

void Gx8_DepthBlurZBufferRender_Init(Gx8_tdst_SpecificData *pst_SD)
{
    IDirect3DSurface8 *pDepthSurface;
    IDirect3DSurface8 *pColorSurface;

    IDirect3DDevice8_GetRenderTarget( pst_SD->mp_D3DDevice, &pst_SD->pSaveRenderTarget );
    IDirect3DDevice8_GetDepthStencilSurface( pst_SD->mp_D3DDevice, &pst_SD->pSaveZBuffer );

    IDirect3DTexture8_GetSurfaceLevel
    (
        pst_SD->pTmpFrameBufferTexture,
        0,
        &pColorSurface
    );

    IDirect3DTexture8_GetSurfaceLevel
    (
        &pst_SD->tZBufferForDepthBlurTexture,
        0,
        &pDepthSurface
    );

    IDirect3DDevice8_SetRenderTarget
    (
        pst_SD->mp_D3DDevice,
        pColorSurface,
        pDepthSurface
    );

    // Don't write to the color buffer.
    bDisableColors=true;
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_COLORWRITEENABLE, 0 );
	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_GREATEREQUAL );
    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0x00000000, 1.0f, 0 );

    IDirect3DSurface8_Release(pColorSurface);
    IDirect3DSurface8_Release(pDepthSurface);

}

void Gx8_DepthBlurZBufferRender_ResetStates(Gx8_tdst_SpecificData *pst_SD)
{
    IDirect3DDevice8_SetRenderTarget
    (
        pst_SD->mp_D3DDevice,
        pst_SD->pSaveRenderTarget,
        pst_SD->pSaveZBuffer
    );

    IDirect3DSurface8_Release(pst_SD->pSaveRenderTarget);
    IDirect3DSurface8_Release(pst_SD->pSaveZBuffer);

//  IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0x00000000, 1.0f, 0 );
    
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_COLORWRITEENABLE,  D3DCOLORWRITEENABLE_ALL );
    bDisableColors=false;

}

void Gx8_NearBlurZBufferRender_Init(Gx8_tdst_SpecificData *pst_SD)
{
    IDirect3DSurface8 *pDepthSurface;
    IDirect3DSurface8 *pColorSurface;

return;
	IDirect3DDevice8_GetRenderTarget( pst_SD->mp_D3DDevice, &pst_SD->pSaveRenderTarget );
    IDirect3DDevice8_GetDepthStencilSurface( pst_SD->mp_D3DDevice, &pst_SD->pSaveZBuffer );

    IDirect3DTexture8_GetSurfaceLevel
    (
        pst_SD->pTmpFrameBufferTexture,
        0,
        &pColorSurface
    );

    IDirect3DTexture8_GetSurfaceLevel
    (
        &pst_SD->tZBufferForNearBlurTexture,
        0,
        &pDepthSurface
    );

    IDirect3DDevice8_SetRenderTarget
    (
        pst_SD->mp_D3DDevice,
        pColorSurface,
        pDepthSurface
    );

    // Don't write to the color buffer.
    bDisableColors=true;
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_COLORWRITEENABLE, 0 );
//  IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_GREATEREQUAL );
    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0x00000000, 1.0f, 0 );

    IDirect3DSurface8_Release(pDepthSurface);
    IDirect3DSurface8_Release(pColorSurface);

}

void Gx8_NearBlurZBufferRender_ResetStates(Gx8_tdst_SpecificData *pst_SD)
{
return; 
	IDirect3DDevice8_SetRenderTarget
    (
        pst_SD->mp_D3DDevice,
        pst_SD->pSaveRenderTarget,
        pst_SD->pSaveZBuffer
    );

    IDirect3DSurface8_Release(pst_SD->pSaveRenderTarget);
    IDirect3DSurface8_Release(pst_SD->pSaveZBuffer);

//  IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0x00000000, 1.0f, 0 );
    
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_COLORWRITEENABLE,  D3DCOLORWRITEENABLE_ALL );
    bDisableColors=false;

}


////////////////////////////////NEW CARLONE FOR SHADOW BUFFER//////////////////////////////////////////////////////

void Gx8_SetTextureBlendingSBDiffuse(ULONG _l_Texture, ULONG BM, unsigned short s_AditionalFlags,bool NeedToBlend)
{

     /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG                   Flag;
    ULONG                   Delta;
    Gx8_tdst_SpecificData   *pst_SD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = GX8_M_SD(GDI_gpst_CurDD);

#pragma message("TODO: clear the confusion between draw mask (element level) and texture blending (texture level).")

    
    //I DON'T KNOW IF IT IS NEEDED
  //  Gx8_RS_DrawWired(pst_SD, !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));
  //  Gx8_RS_DepthTest(pst_SD, GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest );
  //  Gx8_RS_Fogged( pst_SD, ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (pst_SD->ulFogState)));
    
    
    //CULLING IS OK
    if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace)
    {
        if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF)
            Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_INVERTED);
        else
            Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_NORMAL);
    }
    else
    {
        Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_OFF);
    }

    //I DON'T KNOW IF IT IS NEEDED
    Delta = GDI_gpst_CurDD->LastBlendingMode ^ BM;
    Flag = MAT_GET_FLAG(BM);


    //NO TEXTURES NEEDED IN THIS CASE
    if(!NeedToBlend) //If needs to blend...means that texture 0 is the shadow buffer..so don't touch it.
        IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,0, NULL );

    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,1, NULL );
    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice,2, NULL );

    //It should be useful    
    Gx8_RS_DepthMask(pst_SD, (Flag & MAT_Cul_Flag_NoZWrite) ? 0 : 1);
    Gx8_RS_DepthFunc(pst_SD, (Flag & MAT_Cul_Flag_ZEqual) ? D3DCMP_EQUAL : D3DCMP_LESSEQUAL);


    //No alphatest
    Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, FALSE);


    //Alpha op disable and color op JUST THE DIFFUSE COLOR
    GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_DIFFUSE, D3DTOP_MODULATE, D3DTA_TEXTURE );
    //GLI_M_SetTextureColorStage(pst_SD->mp_D3DDevice, 0, D3DTA_DIFFUSE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE );

    //Just ADD blending factor
    if(NeedToBlend)
    {   
        Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, TRUE);
        Gx8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
        Gx8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);

        //I tell the pipeline i'm using alpha blending
        MAT_SET_Blending(BM,MAT_Cc_Op_Add);


    }
    else
    {
        Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, FALSE);
    }

    GDI_gpst_CurDD->LastTextureUsed = _l_Texture;
    GDI_gpst_CurDD->LastBlendingMode = BM;
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DrawTexture(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *sourceTexture )
{
#define VIDEO_SIZE  128

    DWORD currentCullMode;

    IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

    //Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

    /*
    IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    */

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
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
            
        MonStream[0].x = 0;
        MonStream[0].y = (VIDEO_SIZE-0.5f);
        MonStream[0].z = 0;
        MonStream[0].rhw = 1;
        MonStream[0].Color = 0xffffffff;
        MonStream[0].u = 0;
        MonStream[0].v = (float)videoInfo.Height;

        MonStream[1].x = 0;
        MonStream[1].y = 0;
        MonStream[1].z = 0;
        MonStream[1].rhw = 1;
        MonStream[1].Color = 0xffffffff;
        MonStream[1].u = 0;
        MonStream[1].v = 0;

        MonStream[2].x = (VIDEO_SIZE-0.5f);
        MonStream[2].y = (VIDEO_SIZE-0.5f);
        MonStream[2].z = 0;
        MonStream[2].rhw = 1;
        MonStream[2].Color = 0xffffffff;
        MonStream[2].u = (float)videoInfo.Width;
        MonStream[2].v = (float)videoInfo.Height;

        MonStream[3].x = (VIDEO_SIZE-0.5f);
        MonStream[3].y = 0;
        MonStream[3].z = 0;
        MonStream[3].rhw = 1;
        MonStream[3].Color = 0xffffffff;
        MonStream[3].u = (float)videoInfo.Width;
        MonStream[3].v = 0;

        hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
    
    }
    
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_YUVENABLE, FALSE);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, currentCullMode);

    IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);
}


/*
void CopyVideoTexture(void *pTexture,int size)
{
    D3DLOCKED_RECT lr;
    IDirect3DTexture8_LockRect( m_pTarget, 0, &lr, NULL, 0 );
    memcpy( pTexture, lr.pBits, size );
    IDirect3DTexture8_UnlockRect( m_pTarget, 0 );
}
*/





void Gx8_SaveDeviceSettings(Gx8_tdst_SpecificData *pst_SD )
{
    
    IDirect3DDevice8_CreateStateBlock(pst_SD->mp_D3DDevice,D3DSBT_ALL,&DeviceSettingsHANDLE);
    IDirect3DDevice8_CaptureStateBlock(pst_SD->mp_D3DDevice,DeviceSettingsHANDLE);

}

void Gx8_RestoreDeviceSettings(Gx8_tdst_SpecificData *pst_SD )
{

     IDirect3DDevice8_ApplyStateBlock(pst_SD->mp_D3DDevice,DeviceSettingsHANDLE);
     IDirect3DDevice8_DeleteStateBlock(pst_SD->mp_D3DDevice,DeviceSettingsHANDLE);

}










/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////NEW VERTEX SHADER PARTICLE ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




DWORD dwSpriteVSDecl[] =
{
    D3DVSD_STREAM(0), 
    D3DVSD_REG( 0,  D3DVSDT_FLOAT4  ), // Size and index
    D3DVSD_REG( 1,  D3DVSDT_FLOAT2  ), // Texture coords
    D3DVSD_STREAM(1), 
    D3DVSD_REG( 2,  D3DVSDT_D3DCOLOR ), // COLOR
    D3DVSD_END()

};


IDirect3DVertexBuffer8 *pVertexDim;
IDirect3DVertexBuffer8 *pVertColor;


//HANDLE TO VERTEX SHADER
//MOVED IN TOP OF FILE DWORD shaderHandler;



void Gx8_CreateParticleVertexBuffer(Gx8_tdst_SpecificData *pst_SD,int NumParticle)
{
    int counter;
    particleDim *pDim;

    NumParticle+=5;

    IDirect3DDevice8_CreateVertexBuffer(pst_SD->mp_D3DDevice,sizeof(particleDim)*NumParticle*4,NULL,NULL,NULL,&pVertexDim);
    IDirect3DDevice8_CreateVertexBuffer(pst_SD->mp_D3DDevice,sizeof(particleColors)*NumParticle*4,NULL,NULL,NULL,&pVertColor);
    

    //Fill the first vertex buffer
    IDirect3DVertexBuffer8_Lock(pVertexDim,0,0,(BYTE **)&pDim,0);


    for(counter=0;counter<NumParticle;++counter)
    {   

        pDim[0].dimx=-1.0f;
        pDim[0].dimy=-1.0f;
        pDim[0].dimz= 0.0f;
        pDim[0].dimw=(float)counter+4;
        pDim[0].u=1.0f;
        pDim[0].v=1.0f;


        pDim[1].dimx= +1.0f;
        pDim[1].dimy= -1.0f;
        pDim[1].dimz= 0.0f;
        pDim[1].dimw=(float)counter+4;
        pDim[1].u=0.0f;
        pDim[1].v=1.0f;



        pDim[2].dimx= +1.0f;
        pDim[2].dimy= +1.0f;
        pDim[2].dimz= 0.0f;
        pDim[2].dimw=(float)counter+4;
        pDim[2].u=0.0f;
        pDim[2].v=0.0f;
        



        pDim[3].dimx= -1.0f;
        pDim[3].dimy= +1.0f;
        pDim[3].dimz=0.0f;
        pDim[3].dimw=(float)counter+4;
        pDim[3].u=1.0f;
        pDim[3].v=0.0f;


        //Go ahead in vertex buffer 
        pDim+=4;
    
    }

    IDirect3DVertexBuffer8_Unlock(pVertexDim);
    //pFloatBuffer=malloc(sizeof(float)*MAX_PART_NUM*4);

    

}


void Gx8_PrepareParticleVSConstants(Gx8_tdst_SpecificData *pst_SD,SOFT_tdst_AVertex * pVert ,int NumParticle)
{
    //int counter;
    //int position;
    //float constantValues[MAX_PART_NUM*4];


    //To be removed 
    float const94[4];
    float const95[4];


    
    //Someting went wrong
    if(NumParticle>MAX_PART_NUM)
        return;

    //Maybe can be substituted by a memcopy..to do

    /*for(counter=0,position=0;counter<NumParticle;++counter)
    {

        

        constantValues[position]=pVert[counter].x;
        constantValues[position+1]=pVert[counter].y;
        constantValues[position+2]=pVert[counter].z;
        constantValues[position+3]=pVert[counter].w;

        position+=4;

    }
    IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,5,constantValues,NumParticle);
    */


    IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,5,(float *)pVert,NumParticle);




    //TO BE REMOVED
     const94[0]=0.003921568627f;
     const94[1]=0.003921568627f;
     const94[2]=0.003921568627f;
     const94[3]=0.003921568627f;


     const95[0]=0.0f;
     const95[1]=1.0f;
     const95[2]=2.0f;
     const95[3]=3.0f;


     IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,94,const94,1);
     IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,95,const95,1);




    



}



void Gx8_PrepareColors(ULONG *pst_Color,int numParticle)
{
    particleColors *pColors;

    //Lock the buffer
    IDirect3DVertexBuffer8_Lock(pVertColor,0,0,(BYTE **)&pColors,0);
    memcpy(pColors,pst_Color,sizeof(DWORD)*numParticle*4);
    IDirect3DVertexBuffer8_Unlock(pVertColor);

}


void Gx8_PrepareDeviceForParticle(Gx8_tdst_SpecificData *pst_SD)
{
    //Set particle vertex shader
    IDirect3DDevice8_SetVertexShader(pst_SD->mp_D3DDevice,shaderHandler);

    //Prepare the streams
    IDirect3DDevice8_SetStreamSource(pst_SD->mp_D3DDevice,0,pVertexDim,sizeof(float)*6);
    IDirect3DDevice8_SetStreamSource(pst_SD->mp_D3DDevice,1,pVertColor,sizeof(DWORD));

}


void Gx8_CreateParticleVertexShader(Gx8_tdst_SpecificData *pst_SD)
{

    LPXGBUFFER pUcode1;
    const char* strPrefix =


    "#define MAX_PARTICLE 40\n"
    "xvs.1.1\n"

    //Get the particle position from constants
    "mov a0.x,v0.w\n"
    "mov r5,c[a0.x]\n"

    //Get the color
    "mov r10,c[a0.x+40]\n"
    //Transform color
    "mul r10,r10,c94\n"


    //Save particle's size 
    "mov r7.xyz,r5.www\n"
    "mov r7.w,c95.y\n"


    //w=1 for transformation
    "mov r5.w,c95.y\n"


    //Get camera X axe
    "mov r6,c91\n"
    //Get camera Y axe 
    "mov r4,c92\n"


    //Scale the axes (global size and ratio already in the axes)
    "mul r6,r6,r7\n"
    "mul r4,r4,r7\n"


    //Get the offsets signs
    "mov r1.xyz,v0.xxx\n"
    "mov r2.xyz,v0.yyy\n"

    "mov r1.w,c95.x\n"
    "mov r2.w,c95.x\n"


    

    "mul r6,r6,r1\n"
    "mul r4,r4,r2\n"




    //Add the axe to center poistion
    "add r5,r5,r6\n"
    "add r5,r5,r4\n"



    // Transform vertex position 
    "dp4 r0.x, r5, c0\n"
    "dp4 r0.y, r5, c1\n"
    "dp4 r0.z, r5, c2\n"
    "dp4 r0.w, r5, c3\n"



    // Write the diffuse color to the vertex
    "mov oD0, r10\n"
    
    // Write the UVs
    "mov oT0, v1\n"

    // Compute fog
    // we use depth based fog (as does the rest of the engine) as this is faster to compute
    "mov oFog.x, r0.w\n"

    //END
    "mov oPos, r0\n";
    
    
    //Assembly the shader
    AssembleShader( "", strPrefix, strlen(strPrefix), SASM_DONOTOPTIMIZE, NULL,&pUcode1, NULL, NULL, NULL, NULL, NULL );

    //Create the vertex shader
    IDirect3DDevice8_CreateVertexShader(pst_SD->mp_D3DDevice, dwSpriteVSDecl,(DWORD *)XGBuffer_GetBufferPointer(pUcode1),&shaderHandler, 0 );


}










//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




 
void Gx8_PrepareParticleColors(Gx8_tdst_SpecificData *pst_SD,int NumColors,ULONG *pst_Color,ULONG Alpha,bool AlphaOut,DWORD colorAmbient)
{

    int counter;
    //ULONG ul_ColorSet;
    float out[4];
    BYTE temp;
    BYTE tempAlpha;
    DWORD tempColor;


    tempColor=colorAmbient;


    if(AlphaOut)
    {
        tempAlpha=(BYTE)Alpha;
        for(counter=0;counter<NumColors;++counter)
        {
        
            if(pst_Color)
                tempColor=pst_Color[counter];


            temp=(BYTE)(tempColor&0x000000ff)>>1;
            out[0]=temp;

            temp=(BYTE)((tempColor&0x0000ff00)>>9);
            out[1]=temp;

            temp=(BYTE)((tempColor&0x00ff0000)>>17);
            out[2]=temp;
        
            out[3]=tempAlpha;

        
            IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,MAX_PART_NUM+counter+5,out,1);
        
        }
    }
    else
    {

        for(counter=0;counter<NumColors;++counter)
        {

            if(pst_Color)
                tempColor=pst_Color[counter];


            temp=(BYTE)(tempColor&0x000000ff)>>1;
            out[0]=(float) temp;

            temp=(BYTE)((tempColor&0x0000ff00)>>9);
            out[1]=(float) temp;

            temp=(BYTE)((tempColor&0x00ff0000)>>17);
            out[2]= (float )temp;
        
            tempAlpha=(BYTE)((tempColor&0xff000000)>>24);
            out[3]= (float )tempAlpha;
            
            IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,MAX_PART_NUM+counter+5,out,1);
        
        }

    }

}


void Gx8_PrepareParticleColorIndex(Gx8_tdst_SpecificData *pst_SD,int NumColors,GEO_tdst_IndexedSprite *pSprite,ULONG *pst_Color,ULONG *pst_Alpha,bool AlphaOut,DWORD colorAmbient)
{

    int counter;
    //ULONG ul_ColorSet;
    // ULONG Alpha;
    float out[4];
    BYTE temp;
    BYTE tempAlpha;
    //particleColors *pColor;
    DWORD tempColor;

    tempColor=colorAmbient;


    /*if(!pst_Color)
        tempColor=colorAmbient;*/

    if(AlphaOut)
    {
        tempAlpha=(BYTE)pst_Alpha[pSprite->auw_Index];
        for(counter=0;counter<NumColors;++counter)
        {
            if(pst_Color)
                tempColor=pst_Color[pSprite->auw_Index];
            
            temp=(BYTE)(tempColor&0x000000ff)>>1;
            out[0]=temp;

            temp=(BYTE)((tempColor&0x0000ff00)>>9);
            out[1]=temp;

            temp=(BYTE)((tempColor&0x00ff0000)>>17);
            out[2]=temp;

            out[3]=tempAlpha;

            IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,MAX_PART_NUM+counter+5,out,1);

            ++pSprite;
        }
    }
    else
    {
        for(counter=0;counter<NumColors;++counter)
        {

            if(pst_Color)
                tempColor=pst_Color[pSprite->auw_Index];

            temp=(BYTE)(tempColor&0x000000ff)>>1;
            out[0]=temp;

            temp=(BYTE)((tempColor&0x0000ff00)>>9);
            out[1]=temp;

            temp=(BYTE)((tempColor&0x00ff0000)>>17);
            out[2]=temp;
        
            tempAlpha=(BYTE)((tempColor&0xff000000)>>24);
            out[3]= (float )tempAlpha;

        
            IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,MAX_PART_NUM+counter+5,out,1);

            ++pSprite;
    
        }
    }


}



void Gx8_PrepareParticleColorsFAST(Gx8_tdst_SpecificData *pst_SD,int NumColors,ULONG *pst_Color,ULONG Alpha,bool AlphaOut)
{

    int counter;
    //ULONG ul_ColorSet;
    float out[4];
    BYTE temp;
    BYTE tempAlpha;


    if(AlphaOut)
    {
        tempAlpha=(BYTE)Alpha;
        for(counter=0;counter<NumColors;++counter)
        {
        
            temp=(BYTE)(pst_Color[counter]&0x000000ff);
            out[0]=temp;

            temp=(BYTE)((pst_Color[counter]&0x0000ff00)>>8);
            out[1]=temp;

            temp=(BYTE)((pst_Color[counter]&0x00ff0000)>>16);
            out[2]=temp;
        
            out[3]=tempAlpha;

        
            IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,MAX_PART_NUM+counter+5,out,1);
        
        }
    }
    else
    {

        //TRASNFORM COLORS FORMAT FAST
        //Gx8_PrepareBufferForShader(pst_Color,pFloatBuffer,NumColors);
        //IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,MAX_PART_NUM+5,pFloatBuffer,NumColors);
        
    }
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void Gx8_PrepareParticleColorsVB(Gx8_tdst_SpecificData *pst_SD,int NumColors,ULONG *pst_Color,ULONG Alpha,bool AlphaOut)
{
    int counter;
    //ULONG ul_ColorSet;
    //float out[4];
    //BYTE temp;
    //BYTE tempAlpha;
    particleColors *pColor;
    DWORD tempColor;

    IDirect3DVertexBuffer8_Lock(pVertColor,0,0,(BYTE **)&pColor,0);

    if(AlphaOut)
    {
        
        for(counter=0;counter<NumColors;++counter)
        {
            tempColor=pst_Color[counter];
            tempColor &= 0x00ffffff; 
            tempColor |= (Alpha); 
            //tempColor = Gx8_M_ConvertColor(tempColor);

            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
        }
    }
    else
    {
        for(counter=0;counter<NumColors;++counter)
        {
            //tempColor=Gx8_M_ConvertColor(pst_Color[counter]);
            tempColor=pst_Color[counter];
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
        }
    }


    IDirect3DVertexBuffer8_Unlock(pVertColor);


}


void Gx8_PrepareParticleColorsVBIndex(Gx8_tdst_SpecificData *pst_SD,int NumColors,GEO_tdst_IndexedSprite *pSprite,ULONG *pst_Color,ULONG *pst_Alpha,bool AlphaOut)
{

    int counter;
    //ULONG ul_ColorSet;
    ULONG Alpha;
    //float out[4];
    //BYTE temp;
    //BYTE tempAlpha;
    particleColors *pColor;
    DWORD tempColor;


    //Lock the vertex buffer
    IDirect3DVertexBuffer8_Lock(pVertColor,0,0,(BYTE **)&pColor,0);

    if(AlphaOut)
    {
        for(counter=0;counter<NumColors;++counter)
        {

            tempColor=pst_Color[pSprite->auw_Index];
            Alpha=pst_Alpha[pSprite->auw_Index];
            
            tempColor &= 0x00ffffff; 
            tempColor |= (Alpha); 
            //tempColor = Gx8_M_ConvertColor(tempColor);

            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;

            //Go ahead with sprite
            ++pSprite;
            
        }
    }
    else
    {
        for(counter=0;counter<NumColors;++counter)
        {

            tempColor=pst_Color[pSprite->auw_Index];
            //tempColor=Gx8_M_ConvertColor(tempColor);
            
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            pColor->color=tempColor;
            ++pColor;
            
            //Go ahead with sprite
            ++pSprite;

        }
    }

    //Unlock it
    IDirect3DVertexBuffer8_Unlock(pVertColor);


}


void Gx8_PrepareIndexParticleVSConstants(Gx8_tdst_SpecificData *pst_SD,GEO_tdst_IndexedSprite * pVert ,GEO_Vertex *pVertex ,int NumParticle)
{

    int counter;
    int position;
    float constantValues[MAX_PART_NUM*4];

    //To be removed 
    float const94[4];
    float const95[4];


    int index;



    //Someting went wrong
    if(NumParticle>MAX_PART_NUM)
        return;


    //Maybe can be substituted by a memcopy..to do
    for(counter=0,position=0;counter<NumParticle;++counter)
    {

        index=pVert->auw_Index;
        constantValues[position]  =  pVertex[index].x;
        constantValues[position+1]=  pVertex[index].y;
        constantValues[position+2]=  pVertex[index].z;
        constantValues[position+3]= *((float *)pVert);

        ++pVert;
        position+=4;
    }


    IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,5,constantValues,NumParticle);

    //TO BE REMOVED

    const94[0]=0.003921568627f;
    const94[1]=0.003921568627f;
    const94[2]=0.003921568627f;
    const94[3]=0.003921568627f;


    const95[0]=0.0f;
    const95[1]=1.0f;
    const95[2]=2.0f;
    const95[3]=3.0f;

    IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,94,const94,1);
    IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,95,const95,1);

}


void Gx8_PrepareBufferForShader(DWORD *from,float *to,int numElement)
{

/*TO DO     DWORD *from_end=from+numElement;
    
    for(; from != from_end; from += 4, to += 4)    
    {        
        __m64 color = _mm_cvtsi32_si64(*(int*)from);
        _mm_stream_ps(to, _mm_cvtpu8_ps(color)); 
        // migliore, richiede "to" allineato e ottimizza la cache        
        //_mm_store_ps(to, _mm_cvtpu8_ps(color));
        // normale: richiede "to" allineato, no cache        
        //_mm_storeu_ps(to, _mm_cvtpu8_ps(color)); 
        // peggiore: no allineamente, no cache    
    }    
    _mm_empty();
*/
}



void Gx8_PrepareIndexParticleVSSizeConstant(Gx8_tdst_SpecificData *pst_SD,MATH_tdst_Matrix * pMat,float theSize,float theRateo)
{

    ///NEW CARLONE//////////////////////////////////////////////////////////////////////////
    
    float vect1[4]; //X
    float vect2[4]; //Y
    ////////////////////////////////////////////////////////////////////////////////////////

    ///NEW CARLONE//////////////////////////////////////////////////////////////////////////
    vect1[0] = -pMat->Ix * theSize * theRateo;
    vect1[1] = -pMat->Jx * theSize * theRateo;
    vect1[2] = -pMat->Kx * theSize * theRateo;
    vect1[3] = 1.0f;

    vect2[0] = pMat->Iy * theSize;
    vect2[1] = pMat->Jy * theSize;
    vect2[2] = pMat->Ky * theSize;
    vect2[3] = 1.0f;


    IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,91,vect1,1);
    IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice,92,vect2,1);

    ////////////////////////////////////////////////////////////////////////////////////////

}





    

/* Scale the backbuffer to XScale YScale */
void Gx8_ScaleBackBuffer(float XScale, float YScale)
{
    Gx8_tdst_SpecificData   *pst_SD; 
    pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

    //Cal D3D function
    IDirect3DDevice8_SetBackBufferScale(pst_SD->mp_D3DDevice,XScale,YScale);
}


void Gx8_ForceSwap(void)
{
    Gx8_tdst_SpecificData   *pst_SD; 
    pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
    IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_COPY);

    IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_FINISH);
}

void Gx8_ForceCopy(void)
{
    Gx8_tdst_SpecificData   *pst_SD; 
    pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
    IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_COPY);
}


void Gx8_ForceFinish(void)
{
    Gx8_tdst_SpecificData   *pst_SD; 
    pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

    IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_FINISH);
}

void Gx8_ForceClear(void)
{
    Gx8_tdst_SpecificData   *pst_SD; 
    pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

    IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
            0xFF000000, 1.0f, 0 );
}

LPDIRECT3DDEVICE8 Gx8_GetDevice(void)
{
    return ((Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData)->mp_D3DDevice;
}

void Gx8_SmoothTexture(D3DTexture *pSourceTexture, D3DTexture *pDestTexture)
{
    struct S_STRAM {
                    float x,y,z,rhw;
                    DWORD Color;
                    float u,v;
                    } Stream[5];

    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD( GDI_gpst_CurDD );
    IDirect3DSurface8   *pDestSurface = NULL;
    IDirect3DSurface8   *pDestSurface2 = NULL;
    unsigned int        Color = 0xFFFFFFFF;
    float               UVOffset = 1.0f / (ShadowTextureSize)*3;
    LPDIRECT3DSURFACE8  pBackBuffer_SAVE;
    LPDIRECT3DSURFACE8  pZBuffer_SAVE;
    D3DVIEWPORT8        OriginalViewport;
    DWORD   SrcBlend,DestBlend,AlphaBlend,CullMode;
    IDirect3DBaseTexture8* pTexture;

    int RefNum;

/*
        IDirect3DDevice8_GetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_PROJECTION,
            &MProj
        );
*/      

    IDirect3DTexture8_GetSurfaceLevel
            (
                pDestTexture,
                0,
                &pDestSurface
            );

    IDirect3DTexture8_GetSurfaceLevel
            (
                pSourceTexture,
                0,
                &pDestSurface2
            );

    if((!pDestSurface2 || (!pDestSurface)))
    {
        if(pDestSurface2)
            IDirect3DSurface8_Release(pDestSurface2);
        if(pDestSurface)
            IDirect3DSurface8_Release(pDestSurface);
        return;
    }
    IDirect3DDevice8_GetViewport(p_gGx8SpecificData->mp_D3DDevice,&OriginalViewport);


    IDirect3DDevice8_GetRenderTarget
        (
            p_gGx8SpecificData->mp_D3DDevice,
            &pBackBuffer_SAVE
        );
    IDirect3DDevice8_GetDepthStencilSurface
        (
            p_gGx8SpecificData->mp_D3DDevice,
            &pZBuffer_SAVE
        );


    IDirect3DDevice8_SetRenderTarget
            (
                pst_SD->mp_D3DDevice,
                pDestSurface,
                NULL
            );
    
    RefNum = IDirect3DSurface8_Release(pDestSurface);
    ERR_X_Assert(RefNum == 0);

    IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice,0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
    
    IDirect3DDevice8_GetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, &AlphaBlend);
    IDirect3DDevice8_GetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, &SrcBlend);
    IDirect3DDevice8_GetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, &DestBlend);
    IDirect3DDevice8_GetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, &CullMode );
    IDirect3DDevice8_GetTexture( pst_SD->mp_D3DDevice, 0,  &pTexture);
    
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_ONE);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ZERO);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );

    IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)pSourceTexture);


    IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

    {
        HRESULT hr;

        UINT uiVStride;

        Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
        uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
            
        Stream[0].x = 0;
        Stream[0].y = ShadowTextureSize-0.5f;
        Stream[0].z = 0;
        Stream[0].rhw = 1;
        Stream[0].Color = Color;
        Stream[0].u = 5;//0+UVOffset;
        Stream[0].v = ShadowTextureSize -5;//(float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[0].u = 0+UVOffset;
        Stream[0].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        Stream[1].x = 0;
        Stream[1].y = 0;
        Stream[1].z = 0;
        Stream[1].rhw = 1;
        Stream[1].Color = Color;
        Stream[1].u = 5;//0+UVOffset;
        Stream[1].v = 5;//0+UVOffset;
        Stream[1].u = 0+UVOffset;
        Stream[1].v = 0+UVOffset;

        Stream[2].x = (ShadowTextureSize-0.5f);
        Stream[2].y = ShadowTextureSize-0.5f;
        Stream[2].z = 0;
        Stream[2].rhw = 1;
        Stream[2].Color = Color;
        Stream[2].u = ShadowTextureSize -5;//(float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[2].v = ShadowTextureSize -5;//(float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[2].u = (float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[2].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        Stream[3].x = (ShadowTextureSize-0.5f);
        Stream[3].y = 0;
        Stream[3].z = 0;
        Stream[3].rhw = 1;
        Stream[3].Color = Color;
        Stream[3].u = ShadowTextureSize -5;//(float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[3].v = 5;//0+UVOffset;
        Stream[3].u = (float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[3].v = 0+UVOffset;

        hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)Stream,uiVStride);
    }

    IDirect3DDevice8_SetRenderTarget
            (
                pst_SD->mp_D3DDevice,
                pDestSurface2,
                NULL
            );


    IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)pDestTexture);

    UVOffset *= -1.0f;
    {
        HRESULT hr;

        UINT uiVStride;

        Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
        uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
            
        Stream[0].x = 0;
        Stream[0].y = ShadowTextureSize-0.5f;
        Stream[0].z = 0;
        Stream[0].rhw = 1;
        Stream[0].Color = Color;
        Stream[0].u = 0+UVOffset;
        Stream[0].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        Stream[1].x = 0;
        Stream[1].y = 0;
        Stream[1].z = 0;
        Stream[1].rhw = 1;
        Stream[1].Color = Color;
        Stream[1].u = 0+UVOffset;
        Stream[1].v = 0+UVOffset;

        Stream[2].x = (ShadowTextureSize-0.5f);
        Stream[2].y = ShadowTextureSize-0.5f;
        Stream[2].z = 0;
        Stream[2].rhw = 1;
        Stream[2].Color = Color;
        Stream[2].u = (float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[2].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        Stream[3].x = (ShadowTextureSize-0.5f);
        Stream[3].y = 0;
        Stream[3].z = 0;
        Stream[3].rhw = 1;
        Stream[3].Color = Color;
        Stream[3].u = (float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[3].v = 0+UVOffset;

        hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)Stream,uiVStride);
    }

    //Black border lines to avoid texture splattering over the receiver polys
    IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,NULL);
    Color = 0x0;

    {
        HRESULT hr;

        UINT uiVStride;
        Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
        uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

        Stream[0].x = 0;
        Stream[0].y = ShadowTextureSize-0.5f;
        Stream[0].z = 0;
        Stream[0].rhw = 1;
        Stream[0].Color = Color;
        Stream[0].u = 0+UVOffset;
        Stream[0].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        Stream[1].x = 0;
        Stream[1].y = 0;
        Stream[1].z = 0;
        Stream[1].rhw = 1;
        Stream[1].Color = Color;
        Stream[1].u = 0+UVOffset;
        Stream[1].v = 0+UVOffset;

        Stream[2].x = (ShadowTextureSize-1.5f);
        Stream[2].y = 0;
        Stream[2].z = 0;
        Stream[2].rhw = 1;
        Stream[2].Color = Color;
        Stream[2].u = (float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[2].v = 0+UVOffset;

        Stream[3].x = (ShadowTextureSize-1.5f);
        Stream[3].y = ShadowTextureSize-0.5f;
        Stream[3].z = 0;
        Stream[3].rhw = 1;
        Stream[3].Color = Color;
        Stream[3].u = (float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[3].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        Stream[4].x = 0;
        Stream[4].y = ShadowTextureSize-0.5f;
        Stream[4].z = 0;
        Stream[4].rhw = 1;
        Stream[4].Color = Color;
        Stream[4].u = 0+UVOffset;
        Stream[4].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_LINESTRIP,4,(void*)Stream,uiVStride);
    }

    IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

    RefNum = IDirect3DSurface8_Release(pDestSurface2);
    ERR_X_Assert(RefNum == 0);

    IDirect3DDevice8_SetRenderTarget
        (
            p_gGx8SpecificData->mp_D3DDevice,
            pBackBuffer_SAVE,
            pZBuffer_SAVE
        );


/*
        IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_PROJECTION,
            &MProj
        );
*/
    if(pBackBuffer_SAVE)
        IDirect3DSurface8_Release(pBackBuffer_SAVE);
    if(pZBuffer_SAVE)
        IDirect3DSurface8_Release(pZBuffer_SAVE);

    pZBuffer_SAVE = pBackBuffer_SAVE = NULL;

    IDirect3DDevice8_SetViewport(p_gGx8SpecificData->mp_D3DDevice,&OriginalViewport);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, AlphaBlend);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, SrcBlend);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, DestBlend);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, CullMode );
    IDirect3DDevice8_SetTexture( pst_SD->mp_D3DDevice, 0,  pTexture);
    IDirect3DBaseTexture8_Release(pTexture);
    
}


void Gx8_Draw2DQuad(float x, float y, float dimx, float dimy, unsigned int Color)
{
    struct S_STRAM {
                    float x,y,z,rhw;
                    DWORD Color;
                    float u,v;
                    } Stream[5];
    float UVOffset=0.0f;
    D3DVIEWPORT8        OriginalViewport;
    MATH_tdst_Matrix    Identity;
    DWORD VertexShaderHandle;

    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD( GDI_gpst_CurDD );

    IDirect3DDevice8_GetVertexShader(p_gGx8SpecificData->mp_D3DDevice,&VertexShaderHandle);

    IDirect3DDevice8_GetViewport(p_gGx8SpecificData->mp_D3DDevice,&OriginalViewport);

    OriginalViewport.X = 0;
    OriginalViewport.Y = 0;
    OriginalViewport.Width = 640;
    OriginalViewport.Height = 480;

    IDirect3DDevice8_SetViewport(p_gGx8SpecificData->mp_D3DDevice,&OriginalViewport);

    MATH_SetIdentityMatrix(&Identity);

    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_PROJECTION,
            (const struct _D3DMATRIX *) &Identity
        );
    IDirect3DDevice8_SetTransform
        (
            p_gGx8SpecificData->mp_D3DDevice,
            D3DTS_VIEW,
            (const struct _D3DMATRIX *) &Identity
        );

    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, TRUE);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ZWRITEENABLE,FALSE);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ZENABLE,FALSE);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ZFUNC,D3DCMP_ALWAYS);
    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHATESTENABLE,FALSE);

    IDirect3DDevice8_SetTexture(pst_SD->mp_D3DDevice,0,(IDirect3DBaseTexture8 *)NULL);

    {
        HRESULT hr;

        UINT uiVStride;

        IDirect3DDevice8_SetVertexShader(pst_SD->mp_D3DDevice,D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
        uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
            
        Stream[0].x = x;
        Stream[0].y = y+dimy;
        Stream[0].z = 0;
        Stream[0].rhw = 1;
        Stream[0].Color = Color;
        Stream[0].u = 0+UVOffset;
        Stream[0].v = (float)1.0f-UVOffset;//(ShadowTextureSize);

        Stream[1].x = x;
        Stream[1].y = y;
        Stream[1].z = 0;
        Stream[1].rhw = 1;
        Stream[1].Color = Color;
        Stream[1].u = 0+UVOffset;
        Stream[1].v = 0+UVOffset;

        Stream[2].x = (x+dimx);
        Stream[2].y = (y+dimy);
        Stream[2].z = 0;
        Stream[2].rhw = 1;
        Stream[2].Color = Color;
        Stream[2].u = (float)1.0f-UVOffset;
        Stream[2].v = (float)1.0f-UVOffset;

        Stream[3].x = (x+dimx);
        Stream[3].y = y;
        Stream[3].z = 0;
        Stream[3].rhw = 1;
        Stream[3].Color = Color;
        Stream[3].u = (float)1.0f-UVOffset;//(ShadowTextureSize);
        Stream[3].v = 0+UVOffset;
 
        hr=0;
        hr = IDirect3DDevice8_DrawPrimitiveUP(pst_SD->mp_D3DDevice,D3DPT_TRIANGLESTRIP,2,(void*)Stream,uiVStride);
    }

    IDirect3DDevice8_SetVertexShader(pst_SD->mp_D3DDevice,VertexShaderHandle);
}
