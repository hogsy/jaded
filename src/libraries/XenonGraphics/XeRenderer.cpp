#include "precomp.h"

#include "XeRenderer.h"
#include "XeFXManager.h"
#include "XeVertexShaderManager.h"
#include "XePixelShaderManager.h"
#include "XeRenderStateManager.h"
#include "XeTextureManager.h"
#include "XeUtils.h"
#include "XeDynVertexBuffer.h"
#include "XeRenderTargetManager.h"
#include "XeShadowManager.h"
#include "XeAfterEffectManager.h"
#include "XeGDInterface.h"
#include "XeShader.h"
#include "XeLightShaftManager.h"
#include "XeWeatherManager.h"
#include "XeHeatManager.h"
#include "XeWaterManager.h"
#include "XeFurManager.h"
#include "XeTextureManager.h"
#include "XeShaderDatabase.h"
#include "XeContextManager.h"

#include "BASe/BENch/BENch.h"
#include "DevHelpers/XeBench.h"
#include "DevHelpers/XePerf.h"

#include "Xenon/VideoManager/VideoManager.h"
#include "Xenon/VideoManager/VideoNoLoop.h"
#include "Xenon/Interf/MessageBar.h"

#include "Xenon/VersionInfo/VersionInfo.h"

// on render modifiers
#include "ENGine/Sources/MODifier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2_Holder.h"

#if defined(_XENON)
#include "Xenon/MenuManager/MenuManager.h"

// Predicated tiling
//#define _XE_PREDICATED_TILING
#define XE_MIN_PIXEL_SHADING_GPR_COUNT 32
#define XE_MAX_PIXEL_SHADING_GPR_COUNT 96

#endif

#ifdef _XE_PREDICATED_TILING
D3DSurface* g_pTilingRenderTarget           = NULL;
D3DSurface* g_pTilingDepthStencil           = NULL;
D3DTexture* g_pFrontBufferTexture[2]        = { NULL, NULL };
DWORD       g_dwFrontBufferSizeBytes        = 0;
DWORD       g_dwTileTargetSizeBytes         = 0;
DWORD       g_dwTileDepthStencilSizeBytes   = 0;
DWORD       g_dwCurrentBuffer               = 0;
#endif

#if defined(USE_DEFERRED_FREE_AND_RELEASE)
CRITICAL_SECTION g_ThreadSafeLock;
#endif

// Fast Z-Prepass
#define XE_USE_FAST_ZPASS

// unique instance of the XeRenderer
XeRenderer g_oXeRenderer;

// globals used for FPS calculations
static DWORD g_dwBaseTime;
static DWORD g_dwFrameCounter;

// Debug
ULONG g_ulNumCasters = 0;
ULONG g_ulNumReceivers = 0;

// Render with batches of at most 2048 vertices (40960 bytes)
#define MAX_LINES_PER_MESH 1024
#define MAX_QUADS_PER_MESH 512

#ifdef VIDEOCONSOLE_ENABLE
    // General
    TBool bSafeFrame          = FALSE;

    // SPG2
    TBool NoSPG2_Geom        = FALSE;
    TBool NoSPG2_Grid        = FALSE;
    TBool NoSPG2_AI          = FALSE;
    TBool NoSPG2_PixelLighting = FALSE;

    // AfterFx
    TBool NoAfterFX          = FALSE;
    TBool NoGodRayLowRes     = FALSE;
    TBool NoBlackQuad        = FALSE;
    TFloat GodRayRatio       = 0.75f;

    // 3d Engine
    TBool NoRenderList       = FALSE;
    TBool NoDrawOpaque       = FALSE;
    TBool NoDrawTransparent  = FALSE;
    TBool NoTexture          = FALSE;
    TBool NoMoss             = FALSE;
    TBool NoRain             = FALSE;
    TBool NoHeatShimmering   = FALSE;
    TBool NoWater            = FALSE;
    TBool NoLightShafts      = FALSE;
    TBool VisQueries         = TRUE;
    TBool FastZPrePass       = TRUE;
    TInt  ShadowBufferSize   = 512;
    TBool ShadowBlur         = TRUE;
    TBool ShowShadowBuffers  = FALSE;
    TBool ShowStaticSB       = TRUE;
    TBool ShowShadowResult   = FALSE;
    TBool UseOptFastZPass    = TRUE;
    TBool UseFastShadowPass  = TRUE;

    // Guard band
    TFloat GuardBand_X       = 2.0f;
    TFloat GuardBand_Y       = 2.0f;
    TFloat DiscardBand_X     = 1.0f;
    TFloat DiscardBand_Y     = 1.0f;

    // Fur
    TBool NoFur              = FALSE;

    // Physics
    TBool NoODE              = FALSE;
    TBool NoWaterTurbulance  = FALSE;
    TBool NoFurPhysics       = FALSE;

    // Material LOD
    TListID MaterialLODDetailEnableID   = kiListID_Invalid;
    TListID MaterialLODDetailNearID     = kiListID_Invalid;
    TListID MaterialLODDetailFarID      = kiListID_Invalid;
    TListID MaterialLODEnableID         = kiListID_Invalid;
    TListID MaterialLODNearID           = kiListID_Invalid;
    TListID MaterialLODFarID            = kiListID_Invalid;

	TListID MipMapLODBiasID  = kiListID_Invalid;

    TListID SaturationID        = kiListID_Invalid;
    TListID Brightness_R_ID     = kiListID_Invalid;
    TListID Brightness_G_ID     = kiListID_Invalid;
    TListID Brightness_B_ID     = kiListID_Invalid;
    TListID ContrastID          = kiListID_Invalid;
    TBool   NoColorCorrection   = FALSE;

#endif

#if defined(PCWIN_TOOL)
extern BOOL sgb_FullScreen;
extern int  sgi_FullScreenRes;
#endif

BOOL g_bUseFastShadowPath = TRUE;
BOOL g_bShowShadowBuffers  = FALSE;
BOOL g_bShowStaticSB       = TRUE;
BOOL g_bShowShadowResult   = FALSE;
bool g_bHideFPSAndVersionInfo = false;
FLOAT g_fGuardBand_X = 2.0f;
FLOAT g_fGuardBand_Y = 2.0f;
FLOAT g_fDiscardBand_X = 1.0f;
FLOAT g_fDiscardBand_Y = 1.0f;


volatile bool g_bIgnoreShadowClear = false;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

inline void BreakOnGAO(OBJ_tdst_GameObject* _pGAO, char* _pszName)
{
#ifdef _DEBUG
    // Break on object name
    if((ULONG)_pGAO > ROS_LAST_RENDER_OBJECT_SOURCE)
    {
        if(_pGAO->sz_Name)
        {
            if(strstr(_pGAO->sz_Name, _pszName))
            {
                _breakpoint_;
            }
        }
    }
#endif
}

#if defined(_XENON) && !defined(_FINAL_)
void GPUHangCallback(const char *pDescription )
{
    if (pDescription == NULL)
    {
        ERR_OutputDebugString("[GPUHangCallback] now rebooting...\n");
        DebugBreak();
        DmReboot(DMBOOT_COLD);
    }
    else
        ERR_OutputDebugString("[GPUHangCallback] %s\n", pDescription);
}
#endif

// Rim Light Height Attenuation externs
extern BOOL  IsRimLightHeightAttenuationEnabled;
extern float RimLightHeightWorldMin;
extern float RimLightHeightWorldMax;
extern float RimLightHeightAttenuationMin;
extern float RimLightHeightAttenuationMax;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

#if defined(VIDEOCONSOLE_ENABLE)
void XeRenderer_RasterCallback_NumShadows(TInt* _pVariable, TInt _iOldValue, TListID _iOptionID)
{
}
#endif

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

XeRenderer::XeRenderer()
{
#if defined(USE_DEFERRED_FREE_AND_RELEASE)
    InitializeCriticalSection(&g_ThreadSafeLock);
#endif

    m_dwBackgroundColor = 0;
    m_uiNumDrawCalls = 0;
    m_uiNumTriangles = 0;

  #if defined(_XENON)
    m_bUseParallelGDK = TRUE;
  #else
    m_bUseParallelGDK = FALSE;
  #endif

    m_bUseTripleBuffer = FALSE;
    m_bWireframe = FALSE;
    m_bFog = TRUE;
    m_bVisQueries = TRUE;
    m_iShadowBufferSize = 512;
    m_iMaxNumShadows = 3;
    m_iMaxShadowIter = 2;
    m_bFastZPrepass = TRUE;
    m_fHalfViewportPixelWidth = 0.0f;
    m_fHalfViewportPixelHeight = 0.0f;

#if defined(ACTIVE_EDITORS)
    memset(&m_stViewport, 0, sizeof(D3DVIEWPORT9));
    m_stNormalizedViewport.fPosX = 0.0f;
    m_stNormalizedViewport.fPosY = 0.0f;
    m_stNormalizedViewport.fWidth = 0.0f;
    m_stNormalizedViewport.fHeight = 0.0f;

    m_stViewport.MaxZ = 1.0f;

    // Custom materials with one texture
    for (UINT iMat = 0; iMat < XERENDER_MAX_MATERIALS; ++iMat)
    {
        m_apoMaterialPool[iMat] = NULL;
    }
    m_ulNbMaterials = 0;

    m_pst2DRequestMaterial = NULL;

    m_bZBiasEnable = FALSE;
#endif

    m_astDisplayModes = NULL;

	m_fAvgFPS = 0.0f;
	m_fMinFPS = Cf_Infinit;
	m_fMaxFPS = 0.0f;
    m_uiMaxRequestedDynMeshes = 0;

    m_bRenderedGlowOpaque = FALSE;
    m_bRenderedGlowTransparent = FALSE;

	m_bScreenCapture = FALSE;
    m_bHasSPG2WriteAlpha = FALSE;

#ifdef _XENON
	m_pVideo = NULL;
    m_iPixelShadingGPRCount = XE_MAX_PIXEL_SHADING_GPR_COUNT;
#endif

    m_bOceanInitialized = FALSE;
    m_pOceanMaterial = NULL;
}

XeRenderer::~XeRenderer()
{
#if defined(USE_DEFERRED_FREE_AND_RELEASE)
    DeleteCriticalSection(&g_ThreadSafeLock);
#endif
}


void scale_to_range( float& fValue, const float fMin, const float fMax )
{
    fValue = ( fMax - fMin ) / ( 200 - 0 ) * ( fValue * 100 ) + fMin;
}


void XeRenderer::SetGamma(float fGammaFactor, float fContrastFactor, float fBrightnessFactor)
{
    // scale values to pre-established range
    scale_to_range( fGammaFactor,      0.50f, 1.50f );
    scale_to_range( fContrastFactor,   0.50f, 1.50f );
    scale_to_range( fBrightnessFactor, 0.88f, 1.12f );

    // partial stupid bug fix (hack) to prevent screen from showing red and blue when it shouldn't
    // in certain situations.  Doesn't fix all situations but helps a bit.
    if( fBrightnessFactor == 1.0f )
        fBrightnessFactor = 1.01f;

    // initialize ramp
    float ramp[ 256 ];
    for( int i = 0; i < 256; ++i )
        ramp[ i ] = (float)i;

    // gamma
    const float fInvGammaFactor = 1.0f / fGammaFactor;

    for( int i = 0; i < 256; ++i )
    {
        float x = ( 1.0f / 256.0f ) * ramp[ i ];
        float y = pow( x, fInvGammaFactor );

        if( y < 0 ) y = 0;
        if( y > 1 ) y = 1;

        ramp[ i ] = (unsigned short)( 256.0f * y );
    }

    // contrast
    static const float fDefaultContrast = 0.785398163375f;
    const float fContrast = fDefaultContrast * fContrastFactor;

    for( int i = 0; i < 256; ++i )
    {
        if( ramp[ i ] < (int)( 128.0f + 128.0f * tan( fContrast ) ) && ramp[ i ] > (int)( 128.0f - 128.0f * tan( fContrast ) ) )
            ramp[ i ] = ( ramp[ i ] - 128 ) / tan( fContrast ) + 128;
        else if( ramp[ i ] >= (int)( 128.0f + 128.0f * tan( fContrast ) ) )
            ramp[ i ] = 255;
        else
            ramp[ i ] = 0;
    }

    // brightness
    const float fFinalBrightnessFactor = fBrightnessFactor - 1.0f;

    for( int i = 0; i < 256; ++i )
        ramp[ i ] = ramp[ i ] + ( 256.0f * fFinalBrightnessFactor );

    // generate Direct3D gamma ramp
    D3DGAMMARAMP d3dRamp;
    for( int i = 0; i < 256; ++i )
    {
        int nFinalValue = int( ramp[ i ] * 256.0f );

        if( nFinalValue <= 0 )
            nFinalValue = 1;
        if( nFinalValue > 65535 )
            nFinalValue = 65535;

        d3dRamp.red[ i ] = d3dRamp.green[ i ] = d3dRamp.blue[ i ] = nFinalValue;
    }

    // set ramp
    m_pD3DDevice->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &d3dRamp );
}


//----------------------------------------------------------------------@FHB--
// XeRenderer::Init
// 
// Role   : Initializes D3D
//          
//          
// Return : HRESULT
// 
// Author : Eric Le
// Date   : 16 Dec 2004
//----------------------------------------------------------------------@FHE--
HRESULT XeRenderer::Init(HWND _hMainWindow, HWND _hRenderWindow)
{
    HRESULT hr;

#if defined(VIDEOCONSOLE_ENABLE)
	// Create Video Debugging Console
	VideoConsole_Initialize();
#endif

#if defined(_XENON_RENDERER_USETHREAD)
	m_iThreadOwner_ID		= -1;
	m_iThreadOwner_RefCount	= 0;
#endif

    // Create the Direct3D interface
    m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    XeValidateRet(m_pD3D != NULL, E_FAIL, "Unable to create the Direct3D interface");

    // Get the available display modes
    InitializeDisplayModes();

    // Set the present parameters
    InitializePresentParams(_hMainWindow);

#if defined(_XENON_RENDER_PC)
    ULONG ulBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | 
                            D3DCREATE_PUREDEVICE                | 
                            D3DCREATE_FPU_PRESERVE;
#else
  #if (_XDK_VER > 1332)
    D3DGPUVERSION gpuVersion;
    m_pD3D->QueryGpuVersion(&gpuVersion);

    ULONG ulBehaviorFlags = D3DCREATE_BUFFER_2_FRAMES;
    //if (gpuVersion.MajorVersion <= D3DGPUMAJORVERSION_ALPHA2)
    //{
        //ulBehaviorFlags |= D3DCREATE_RELAXEDRAMLIMIT;
    //}
  #else
    ULONG ulBehaviorFlags = D3DCREATE_BUFFER_2_FRAMES/* | D3DCREATE_RELAXEDRAMLIMIT*/;
  #endif
#endif

    // Create the device
    hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,           // Adapter
                              D3DDEVTYPE_HAL,               // DeviceType
                              _hRenderWindow,               // hFocusWindow
                              ulBehaviorFlags,              // BehaviorFlags
                              &m_stParams,                  // pPresentationParameters
                              &m_pD3DDevice);               // ppReturnedDeviceInterface
    XeValidateRet(SUCCEEDED(hr) && (m_pD3DDevice != NULL), E_FAIL, "Unable to create the Direct3D device");

    /*static D3DRING_BUFFER_PARAMETERS stParameters = m_stParams.RingBufferParameters;
    stParameters.Flags = 0;
    stParameters.PrimarySize = 4*128*1024;
    stParameters.SecondarySize = 32*1024*1024;
    hr = m_pD3DDevice->SetRingBufferParameters(&stParameters);
    XeValidateRet(SUCCEEDED(hr) && (m_pD3DDevice != NULL), E_FAIL, "Bad ring buffer parameters");*/

#ifdef _XENON
#if (_XDK_VER > 1332)
    if (gpuVersion.MajorVersion == D3DGPUMAJORVERSION_BETAPREVIEW)
    {
#if _XENON_DEBUG_SAFEMODE
        // Slower Although More Stable
        m_pD3DDevice->SetSafeLevel(0, 5);
#else
        // Faster Setup
        m_pD3DDevice->SetSafeLevel(0, 1);
#endif
    }
#endif // (_XDK_VER > 1332)
#endif // _XENON

    // Get the device caps
    hr = m_pD3DDevice->GetDeviceCaps(&m_stDeviceCaps);

    // check for pixel shader 2_a support
    m_bPS_2_a = m_bPS_3_0 = FALSE;
    if (m_stDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(2,0))
    {
        if (m_stDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(3,0))
        {
            m_bPS_2_a = m_bPS_3_0 = TRUE;
        }
    }
    m_iRequestedMaxLight = m_iRequestedMaxLightPerPass = VS_MAX_LIGHTS;
    m_iRequestedMaxLightPerPass = GetMaxLightsPerPass(FALSE);

/*    for(UINT uiListIdx = XeRT_FIRSTLIST; uiListIdx < XeRT_NUMTYPES; uiListIdx++)
    {
        //m_avRenderObjectLists[uiListIdx].reserve(XERENDER_MINLISTSIZE);
        //m_avRenderObjectLists[uiListIdx].resize(0);
    }
*/
    m_pUIRectRenderable = NULL;
    for(int i=0; i<XERENDER_NB_RECT_MESH; i++)
        m_pUIRectMesh[i]= NULL;
    m_pUIRectMaterial   = NULL;

    g_dwBaseTime = GetTickCount();
    g_dwFrameCounter = 0;

    m_fHalfViewportPixelWidth   = 0.5f / ((float) m_stParams.BackBufferWidth);
    m_fHalfViewportPixelHeight  = 0.5f / ((float) m_stParams.BackBufferHeight);

#ifdef _XENON

    // set gpr allocation in favor of pixel shading
    m_iPixelShadingGPRCount = XE_MAX_PIXEL_SHADING_GPR_COUNT;
    hr = m_pD3DDevice->SetShaderGPRAllocation(0, GPU_GPRS - m_iPixelShadingGPRCount, m_iPixelShadingGPRCount);

    // Set GPU hang callback
  #if !defined(_FINAL_)
    m_pD3DDevice->SetHangCallback(GPUHangCallback);
  #endif

    // set default gamma ramp
    m_fCurrentGammaFactor       = m_fRequestedGammaFactor       = 1.0f;
    m_fCurrentContrastFactor    = m_fRequestedContrastFactor    = 1.0f;
    m_fCurrentBrightnessFactor  = m_fRequestedBrightnessFactor  = 1.0f;

    SetGamma(m_fCurrentGammaFactor, m_fCurrentContrastFactor, m_fCurrentBrightnessFactor);
#endif

#if defined(ACTIVE_EDITORS)
    SetViewport(0, 0, m_stParams.BackBufferWidth, m_stParams.BackBufferHeight);

    // Custom materials with one texture
    for (UINT iMat = 0; iMat < XERENDER_MAX_MATERIALS; ++iMat)
    {
        m_apoMaterialPool[iMat] = XeMaterial_Create();
        m_apoMaterialPool[iMat]->AddTextureStage();
    }
    m_ulNbMaterials = 0;

    // 2D request drawing material
    m_pst2DRequestMaterial = XeMaterial_Create();
    m_pst2DRequestMaterial->SetAlphaBlend(FALSE);
    m_pst2DRequestMaterial->SetAlphaTest(FALSE);
    m_pst2DRequestMaterial->SetColorSource(MAT_Cc_ColorOp_RLI);
    m_pst2DRequestMaterial->SetColorWrite(TRUE);
    m_pst2DRequestMaterial->SetZState(TRUE, FALSE);
#endif

    UINT uiWidth, uiHeight;
    ResetRTResolution();
    GetBackbufferResolution(&uiWidth, &uiHeight);
    InitFullscreenQuad(&m_poQuadMesh, uiWidth, uiHeight);
    InitFullscreenQuad(&m_po640x480QuadMesh, 640, 480);

    // Init all the managers
    g_oFXManager.ReInit();
    g_oRenderStateMgr.ReInit();
    g_oXeContextManager1.ReInit();
#ifdef _XENON
    g_oXeContextManager2.ReInit();
#endif
    g_oVertexShaderMgr.ReInit();
    g_oPixelShaderMgr.ReInit();
    g_XeBufferMgr.Init();
    g_oXeTextureMgr.Init();
    g_oXeRenderTargetMgr.Init();
    g_oAfterEffectManager.ReInit();
    g_oXeShadowManager.Init();
    g_oXeLightShaftManager.Initialize();
    g_oXeWeatherManager.Initialize();
    g_oHeatManager.Init();
    g_oWaterManager.ReInit();
    g_oXeShaderDatabase.Initialize();

    InitUIRects();

    // Add Raster Options To Console
    InitializeRasterOptions();

#ifdef _DEBUG
    m_bSkipNextObject = FALSE;
#endif

#if defined(_XENON_RENDERER_USETHREAD)
	// Initialize Rendering Thread

   	m_pThread = new CEventThread(eXeThreadEvent_Count, 256, eThreadID_RenderingFlip);

    m_pThread->SetEventCallback(eXeThreadEvent_DynamicVB_InitialLock, Xe_ThreadCallback_DynamicVB_InitialLock);
    m_pThread->SetEventCallback(eXeThreadEvent_DynamicVB_FinalUnlock, Xe_ThreadCallback_DynamicVB_FinalUnlock);
   	m_pThread->SetEventCallback(eXeThreadEvent_Flip, Xe_ThreadCallback_Flip);

	// Initial Dynamic Vertex Buffer Lock

    ReleaseThreadOwnership();
    RequestThreadEvent(eXeThreadEvent_DynamicVB_InitialLock);
    WaitForEventComplete(eXeThreadEvent_DynamicVB_InitialLock);
#else
	// Initial Dynamic Vertex Buffer Lock

    ULONG ulOffset;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
    g_XeBufferMgr.LockDynVertexBuffer(0, &pVertexBuffer, &ulOffset, TRUE);
#endif

#ifdef _XENON
    m_iShadowBufferSize = m_oVideoMode.fIsHiDef ? 832 : 512;
#endif

    m_pOceanMesh = new XeMesh;
    m_pOceanMesh->ClearAllStreams();

    m_pOceanMaterial = XeMaterial_Create();
    m_pOceanMaterial->AddTextureStage();
    m_pOceanMaterial->SetAddressMode(0, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP);
    m_pOceanMaterial->SetColorSource(COLORSOURCE_VERTEX_COLOR);
    m_pOceanMaterial->SetZState(TRUE, TRUE);
    m_pOceanMaterial->SetTwoSided(TRUE);
    m_pOceanMaterial->SetCustomVS(0);
    m_pOceanMaterial->SetCustomPS(CUSTOM_PS_OCEAN);
    CreatePredicatedTilingStuff();

    return hr;
}

void XeRenderer::InitializeDisplayModes(void)
{
  #if (defined(_XENON) && (_XDK_VER >= 1838))
    XGetVideoMode(&m_oVideoMode);
  #else
    UINT uiAdapterCount = m_pD3D->GetAdapterCount();
    UINT uiModeCount    = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_A8R8G8B8);

    if (uiModeCount > 0)
    {
        m_astDisplayModes = (D3DDISPLAYMODE*)MEM_p_Alloc(uiModeCount * sizeof(D3DDISPLAYMODE));

        for(UINT uiModeIdx = 0; uiModeIdx < uiModeCount; uiModeIdx++)
        {
            m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_astDisplayModes[uiModeIdx]);
        }
    }
  #endif
}

void XeRenderer::InitializePresentParams(HWND _hDeviceWindow)
{
    ZeroMemory( &m_stParams, sizeof(D3DPRESENT_PARAMETERS) );

#if defined(_XENON)
    // Xenon 
    BOOL bEnable720p  = m_oVideoMode.fIsHiDef;

	if(bEnable720p)
	{
		m_stParams.BackBufferWidth            = 1280;
		m_stParams.BackBufferHeight           = 720;
	}
	else
	{
		m_stParams.BackBufferWidth            = 640;
		m_stParams.BackBufferHeight           = 480;
	}

    m_stParams.BackBufferFormat           = D3DFMT_A8R8G8B8;

#ifdef _XE_PREDICATED_TILING
    m_stParams.BackBufferCount            = 0;
    m_stParams.MultiSampleType            = D3DMULTISAMPLE_2_SAMPLES;
    m_stParams.EnableAutoDepthStencil     = FALSE;
#else
    m_stParams.BackBufferCount            = m_bUseTripleBuffer ? 2 : 1;
    m_stParams.MultiSampleType            = bEnable720p ? D3DMULTISAMPLE_NONE : D3DMULTISAMPLE_4_SAMPLES;
    m_stParams.EnableAutoDepthStencil     = TRUE;
#endif

    m_stParams.MultiSampleQuality         = 0;                      // Ignored on Xenon
    m_stParams.SwapEffect                 = D3DSWAPEFFECT_DISCARD;  // Ignored on Xenon
    m_stParams.AutoDepthStencilFormat     = D3DFMT_D24S8;
    m_stParams.Flags                      = 0;
    m_stParams.FullScreen_RefreshRateInHz = 0;

#ifndef _FINAL
    m_stParams.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
#else
    m_stParams.PresentationInterval       = D3DPRESENT_INTERVAL_TWO;
#endif

    m_stParams.DisableAutoBackBuffer      = FALSE;
    m_stParams.DisableAutoFrontBuffer     = FALSE;
    m_stParams.FrontBufferFormat          = D3DFMT_X8R8G8B8;

#endif // defined(_XENON)

#if defined(_XENON_RENDER_PC)

#if defined(ACTIVE_EDITORS)
    m_hRenderWindow = _hDeviceWindow;

    RECT rcClient;
    GetClientRect(m_hRenderWindow, &rcClient);

    m_stParams.BackBufferWidth            = rcClient.right - rcClient.left;
    m_stParams.BackBufferHeight           = rcClient.bottom - rcClient.top;
    m_stParams.BackBufferFormat           = D3DFMT_A8R8G8B8;
    m_stParams.BackBufferCount            = 1;
    m_stParams.MultiSampleType            = D3DMULTISAMPLE_NONE;
    m_stParams.MultiSampleQuality         = 0;
    m_stParams.SwapEffect                 = D3DSWAPEFFECT_COPY;
    m_stParams.hDeviceWindow              = (HWND)_hDeviceWindow;
    m_stParams.Windowed                   = TRUE;
    m_stParams.EnableAutoDepthStencil     = TRUE;
    m_stParams.AutoDepthStencilFormat     = D3DFMT_D24S8;
    m_stParams.Flags                      = 0;
    m_stParams.FullScreen_RefreshRateInHz = 0;
    m_stParams.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
#else

    m_hRenderWindow = _hDeviceWindow;

    if (sgb_FullScreen)
    {
        switch (sgi_FullScreenRes)
        {
            case 2:
                m_stParams.BackBufferWidth  = 800;
                m_stParams.BackBufferHeight = 600;
                break;

            case 3:
                m_stParams.BackBufferWidth  = 1024;
                m_stParams.BackBufferHeight = 768;
                break;

            default:
                m_stParams.BackBufferWidth  = 640;
                m_stParams.BackBufferHeight = 480;
                break;
        }

        m_stParams.Windowed        = FALSE;
        m_stParams.SwapEffect      = D3DSWAPEFFECT_FLIP;
        m_stParams.BackBufferCount = 2;

        m_stParams.hDeviceWindow = MAI_gh_MainWindow;
    }
    else
    {
        RECT rcClient;
        GetClientRect(m_hRenderWindow, &rcClient);

        m_stParams.Windowed         = TRUE;
        m_stParams.BackBufferWidth  = rcClient.right - rcClient.left;
        m_stParams.BackBufferHeight = rcClient.bottom - rcClient.top;
        m_stParams.SwapEffect       = D3DSWAPEFFECT_COPY;
        m_stParams.BackBufferCount  = 1;

        m_stParams.hDeviceWindow    = (HWND)_hDeviceWindow;
    }
    m_stParams.BackBufferFormat           = D3DFMT_A8R8G8B8;
    m_stParams.MultiSampleType            = D3DMULTISAMPLE_NONE;
    m_stParams.MultiSampleQuality         = 0;
    m_stParams.EnableAutoDepthStencil     = TRUE;
    m_stParams.AutoDepthStencilFormat     = D3DFMT_D24S8;
    m_stParams.Flags                      = 0;
    m_stParams.FullScreen_RefreshRateInHz = 0;
    m_stParams.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

#endif

#endif // defined(_XENON_RENDER_PC)
}

void XeRenderer::InitializeRasterOptions(void)
{
#ifdef VIDEOCONSOLE_ENABLE
  #ifdef _XENON
    VideoConsole_AddRasterOption(L"Gamma", &m_fRequestedGammaFactor, 0.0f, 10.0f, 0.05f);
    VideoConsole_AddRasterOption(L"Brightness", &m_fRequestedBrightnessFactor, 0.0f, 2.0f, 0.02f);
    VideoConsole_AddRasterOption(L"Contrast", &m_fRequestedContrastFactor, 0.0f, 2.0f, 0.01f);
  #endif
    VideoConsole_AddRasterOption(L"Safe Frame", &bSafeFrame);
    VideoConsole_AddRasterOption(L"Run Ultra Fast", (TBool*)&m_bUseParallelGDK);
    VideoConsole_AddRasterOption(L"Wireframe", &m_bWireframe);
    VideoConsole_AddRasterOption(L"Visibility Queries", &VisQueries);
    VideoConsole_AddRasterOption(L"Fast Z prepass", &FastZPrePass);
    VideoConsole_AddRasterOption(L"Super Fast Z prepass", &UseOptFastZPass);
    VideoConsole_AddRasterOption(L"Fog", &m_bFog);
    MipMapLODBiasID = VideoConsole_AddRasterOption(L"MipMap LOD Bias", &GDI_gpst_CurDD->pst_World->f_XeMipMapLODBias, -15.00f, 15.00f, 0.25f);
    VideoConsole_AddRasterOption(L"Guard band X ", &GuardBand_X, 1.0f, (TFloat)GPU_GUARDBAND/m_stParams.BackBufferWidth, 0.1f);
    VideoConsole_AddRasterOption(L"Guard band Y ", &GuardBand_Y, 1.0f, (TFloat)GPU_GUARDBAND/m_stParams.BackBufferHeight, 0.1f);
    VideoConsole_AddRasterOption(L"Discard band X ", &DiscardBand_X, 1.0f, (TFloat)GPU_GUARDBAND/m_stParams.BackBufferWidth, 0.1f);
    VideoConsole_AddRasterOption(L"Discard band Y ", &DiscardBand_Y, 1.0f, (TFloat)GPU_GUARDBAND/m_stParams.BackBufferHeight, 0.1f);
    VideoConsole_AddRasterOption(L"Pixel Shading GPR count", &m_iPixelShadingGPRCount, XE_MIN_PIXEL_SHADING_GPR_COUNT, XE_MAX_PIXEL_SHADING_GPR_COUNT, 8);

    // Lighting
    VideoConsole_AddRasterOption(L"Lighting");
    VideoConsole_AddRasterOption(L"Max Num Lights per element", &m_iRequestedMaxLight, 0, VS_MAX_LIGHTS, 1, XeRenderer_RasterCallback_NumShadows);
    VideoConsole_AddRasterOption(L"Max Num Lights per pass", &m_iRequestedMaxLightPerPass, 1, GetMaxHWLightsPerPass(), 1, XeRenderer_RasterCallback_NumShadows);
    VideoConsole_AddRasterOption(L"No texture"          , &NoTexture);

    // Shadows
    VideoConsole_AddRasterOption(L"Shadows");
    VideoConsole_AddRasterOption(L"Fast Shadow", &UseFastShadowPass);
    VideoConsole_AddRasterOption(L"Max Num Shadows", &m_iMaxNumShadows, 0, 3/*XE_MAXNUMSHADOWS*/, 1, XeRenderer_RasterCallback_NumShadows);
    VideoConsole_AddRasterOption(L"Max shadows iterations", &m_iMaxShadowIter, 2, 8, 1);
    VideoConsole_AddRasterOption(L"ShadowBuffer Size", &m_iShadowBufferSize, 512, XE_SHADOWBUFFERSIZE, 64);
    VideoConsole_AddRasterOption(L"ShadowBuffer Blur", &ShadowBlur);
    VideoConsole_AddRasterOption(L"Show shadow buffers", &ShowShadowBuffers);
    VideoConsole_AddRasterOption(L"Show static SB", &ShowStaticSB);
    VideoConsole_AddRasterOption(L"Show shadow result", &ShowShadowResult);

    // spg2
    VideoConsole_AddRasterOption(L"Sprite Generator");
    VideoConsole_AddRasterOption(L"Disable SPG2 - Geom", &NoSPG2_Geom       );
    VideoConsole_AddRasterOption(L"Disable SPG2 - Grid", &NoSPG2_Grid       );
    VideoConsole_AddRasterOption(L"Disable SPG2 - AI"   ,&NoSPG2_AI         );
    VideoConsole_AddRasterOption(L"Disable SPG2 - Per-Pixel Lighting",&NoSPG2_PixelLighting);

    // fur
    VideoConsole_AddRasterOption(L"Fur");
    VideoConsole_AddRasterOption(L"Disable Fur",         &NoFur       );

    // Physics
    VideoConsole_AddRasterOption(L"Physics");
    VideoConsole_AddRasterOption(L"Disable Fur Physics", &NoFurPhysics       );
    VideoConsole_AddRasterOption(L"Disable ODE", &NoODE);
    VideoConsole_AddRasterOption(L"Disable Water turbulance", &NoWaterTurbulance);

    // after fx
    VideoConsole_AddRasterOption(L"AfterFX");
    VideoConsole_AddRasterOption(L"Disable ALL AfterFX" ,       &NoAfterFX        );
    VideoConsole_AddRasterOption(L"Disable Black Quad" ,        &NoBlackQuad      );
    VideoConsole_AddRasterOption(L"Disable Rain" ,              &NoRain           );
    VideoConsole_AddRasterOption(L"Disable Heat" ,              &NoHeatShimmering );
    VideoConsole_AddRasterOption(L"Disable Light Shafts",       &NoLightShafts    );
    VideoConsole_AddRasterOption(L"Disable God Ray Low Res",    &NoGodRayLowRes   );
    VideoConsole_AddRasterOption(L"God Ray Reduce Res",         &GodRayRatio, 0.0f, 10.0f, 0.05f);

    SaturationID        = VideoConsole_AddRasterOption( L"Saturation",   &GDI_gpst_CurDD->pst_World->f_XeSaturation, 0.0f, 1.0f, 0.05f);
    Brightness_R_ID     = VideoConsole_AddRasterOption( L"Brightness_R", &GDI_gpst_CurDD->pst_World->v_XeBrightness.x, -10.0f, 10.0f, 0.01f);
    Brightness_G_ID     = VideoConsole_AddRasterOption( L"Brightness_G", &GDI_gpst_CurDD->pst_World->v_XeBrightness.y, -10.0f, 10.0f, 0.01f);
    Brightness_B_ID     = VideoConsole_AddRasterOption( L"Brightness_B", &GDI_gpst_CurDD->pst_World->v_XeBrightness.z, -10.0f, 10.0f, 0.01f);
    ContrastID          = VideoConsole_AddRasterOption( L"Contrast",     &GDI_gpst_CurDD->pst_World->f_XeContrast, -10.0f, 10.0f, 0.01f);
    VideoConsole_AddRasterOption(L"Disable Color Correction", &NoColorCorrection );

    // water
    VideoConsole_AddRasterOption(L"3D Water");
    VideoConsole_AddRasterOption(L"Disable Water" ,             &NoWater          );
    VideoConsole_AddRasterOption(L"Water Reflection Blur Factor", &g_fWaterBlurFactor, 0.0f, 10.0f, 0.1f);

    // 3d Engine
    VideoConsole_AddRasterOption(L"3D Engine");
    VideoConsole_AddRasterOption(L"Skip RenderList"     , &NoRenderList     );
    VideoConsole_AddRasterOption(L"Skip Opaque"         , &NoDrawOpaque     );
    VideoConsole_AddRasterOption(L"Skip Transparent"    , &NoDrawTransparent);
    VideoConsole_AddRasterOption(L"Skip Moss"           , &NoMoss);

    // Material LOD
    MaterialLODDetailEnableID  = VideoConsole_AddRasterOption(L"Material LOD Detail Enable", &GDI_gpst_CurDD->pst_World->b_XeMaterialLODDetailEnable);
    MaterialLODDetailNearID    = VideoConsole_AddRasterOption(L"Material LOD Detail Near", &GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailNear, 0.0f, 100.0f, 0.5f);
    MaterialLODDetailFarID     = VideoConsole_AddRasterOption(L"Material LOD Detail Far", &GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailFar,  0.0f, 100.0f, 0.5f);
    MaterialLODEnableID        = VideoConsole_AddRasterOption(L"Material LOD Enable", &GDI_gpst_CurDD->pst_World->b_XeMaterialLODEnable);
    MaterialLODNearID          = VideoConsole_AddRasterOption(L"Material LOD Near", &GDI_gpst_CurDD->pst_World->f_XeMaterialLODNear, 0.0f, 450.0f, 0.5f);
    MaterialLODFarID           = VideoConsole_AddRasterOption(L"Material LOD Far", &GDI_gpst_CurDD->pst_World->f_XeMaterialLODFar,  0.0f, 450.0f, 0.5f);
#endif
}

HRESULT XeRenderer::ReInit(HWND _hRenderWindow)
{
    return Reset(false, _hRenderWindow);
}

void XeRenderer::CreatePredicatedTilingStuff(void)
{
#if defined(_XE_PREDICATED_TILING)
    HRESULT hr;

    // Set up tiling front buffer textures.
    // These are the size of your desired rendering surface (in this case, the whole
    // screen).
    // Predicated Tiling will resolve to these textures after each tile is rendered.
    // We need to create 2 front buffers to avoid visual tearing.
    m_pD3DDevice->CreateTexture(m_stParams.BackBufferWidth, m_stParams.BackBufferHeight, 1, 0,
                                D3DFMT_LE_X8R8G8B8, D3DPOOL_DEFAULT,
                                &g_pFrontBufferTexture[0], NULL );
    m_pD3DDevice->CreateTexture(m_stParams.BackBufferWidth, m_stParams.BackBufferHeight, 1, 0,
                                D3DFMT_LE_X8R8G8B8, D3DPOOL_DEFAULT,
                                &g_pFrontBufferTexture[1], NULL );

    ERR_X_Assert( g_pFrontBufferTexture[0] && g_pFrontBufferTexture[1] );

    // Record the size of one of the frontbuffers.
    D3DSurface* pSurface;
    g_pFrontBufferTexture[0]->GetSurfaceLevel( 0, &pSurface );
    g_dwFrontBufferSizeBytes = pSurface->Size;
    SAFE_RELEASE( pSurface );

    // Create color and depth/stencil rendertargets.
    // These rendertargets are where Predicated Tiling will render each tile.  Therefore,
    // these rendertargets should be set up with all rendering quality settings you desire,
    // such as multisample antialiasing.
    // Note how we use the dimension of the largest tile rectangle to define how big the
    // rendertargets are.
    DWORD dwTileWidth  = 1280;
    DWORD dwTileHeight = 384;

    // Use custom EDRAM allocation to create the rendertargets.
    // The color rendertarget is placed at address 0 in EDRAM.
    D3DSURFACE_PARAMETERS TileSurfaceParams;
    memset(&TileSurfaceParams, 0, sizeof(D3DSURFACE_PARAMETERS));
    TileSurfaceParams.Base = 0;
    hr = m_pD3DDevice->CreateRenderTarget(dwTileWidth, dwTileHeight, D3DFMT_A8R8G8B8,
                                          g_oXeRenderer.GetMultiSampleType(), 0, FALSE,
                                          &g_pTilingRenderTarget, &TileSurfaceParams );
    ERR_X_Assert((hr == D3D_OK) && "Can't create tiling rendertargets!");

    // Record the size of the created rendertarget, and then set up allocation
    // for the next rendertarget right after the end of the first rendertarget.
    // Put the hierarchical Z buffer at the start of hierarchical Z memory.
    g_dwTileTargetSizeBytes = g_pTilingRenderTarget->Size;
    TileSurfaceParams.Base = g_dwTileTargetSizeBytes / GPU_EDRAM_TILE_SIZE;
    TileSurfaceParams.HierarchicalZBase = 0;

    hr = m_pD3DDevice->CreateDepthStencilSurface(dwTileWidth, dwTileHeight, D3DFMT_D24S8,
                                                 g_oXeRenderer.GetMultiSampleType(), 0, FALSE,
                                                 &g_pTilingDepthStencil, &TileSurfaceParams );

    ERR_X_Assert((hr == D3D_OK) && "Can't create tiling depth stencil surface!");

    // Record the size of the depth/stencil rendertarget.
    g_dwTileDepthStencilSizeBytes = g_pTilingDepthStencil->Size;

#endif
}
HRESULT XeRenderer::Reset(bool _bSkipTCL, HWND _hRenderWindow)
{
    HRESULT hr;

#if defined(_XENON_RENDERER_USETHREAD)
	m_iThreadOwner_ID		= -1;
	m_iThreadOwner_RefCount	= 0;
#endif

#if defined(_XE_PREDICATED_TILING)
    SAFE_RELEASE( g_pTilingRenderTarget );
    SAFE_RELEASE( g_pTilingDepthStencil );
    SAFE_RELEASE( g_pFrontBufferTexture[0] );
    SAFE_RELEASE( g_pFrontBufferTexture[1] );
#endif

#if defined(_XENON_RENDER_PC)
    if (!_bSkipTCL)
    {
        hr = m_pD3DDevice->TestCooperativeLevel();
        if (FAILED(hr) && (hr != D3DERR_DEVICENOTRESET))
        {
            ERR_OutputDebugString("[XeDX9] XeRenderer::Reset() Waiting for device to be ready...\n");
            return S_OK;
        }
    }
#endif // defined(_XENON_RENDER_PC)

#if defined(ACTIVE_EDITORS)
    {
        InvalidateRenderLists();

        // Reset the material pool
        m_ulNbMaterials = 0;

        // Reset the 2D line and quad pools
        m_av2DLinePool.resize(0);
        m_av2DQuadPool.resize(0);
        m_av3DQuadPool.resize(0);

        // make sure all streams are unbound before locking any of them
        g_oRenderStateMgr.UnbindAllStreams();
    }
#endif

    // Prepare for a reset
    g_oXeShaderDatabase.OnDeviceLost();
    g_oRenderStateMgr.OnDeviceLost();
    g_oPixelShaderMgr.OnDeviceLost();
    g_oVertexShaderMgr.OnDeviceLost();
    g_oXeTextureMgr.OnDeviceLost();
    g_oFXManager.OnDeviceLost();
    g_oXeRenderTargetMgr.OnDeviceLost();
    g_oAfterEffectManager.OnDeviceLost();
    g_oXeShadowManager.OnDeviceLost();
    g_oXeLightShaftManager.OnDeviceLost();
    g_oXeWeatherManager.OnDeviceLost();
    g_oHeatManager.OnDeviceLost();
    g_oWaterManager.OnDeviceLost();
    g_XeBufferMgr.OnDeviceLost();

    SAFE_DELETE(m_poQuadMesh);
    SAFE_DELETE(m_po640x480QuadMesh);
    SAFE_DELETE(m_pOceanMesh);

    InitializePresentParams(m_stParams.hDeviceWindow);

    m_fHalfViewportPixelWidth   = 0.5f / ((float) m_stParams.BackBufferWidth);
    m_fHalfViewportPixelHeight  = 0.5f / ((float) m_stParams.BackBufferHeight);
    ResetRTResolution();

    hr = m_pD3DDevice->Reset(&m_stParams);
    XeValidateRet(SUCCEEDED(hr), E_FAIL, "Unable to reset the device");

#if defined(_XENON_RENDER_PC)
    m_pD3DDevice->EvictManagedResources();
#endif

#if defined(ACTIVE_EDITORS)
    SetViewport(0, 0, m_stParams.BackBufferWidth, m_stParams.BackBufferHeight);
#endif

    CreatePredicatedTilingStuff();

    // Restore after reset
    UINT uiWidth, uiHeight;
    GetBackbufferResolution(&uiWidth, &uiHeight);
    InitFullscreenQuad(&m_poQuadMesh, uiWidth, uiHeight);
    InitFullscreenQuad(&m_po640x480QuadMesh, 640, 480);

    m_pOceanMesh = new XeMesh;
    m_pOceanMesh->ClearAllStreams();
    m_bOceanInitialized = FALSE;

    g_oRenderStateMgr.OnDeviceReset();
    g_oPixelShaderMgr.OnDeviceReset();
    g_oVertexShaderMgr.OnDeviceReset();
    g_oXeTextureMgr.OnDeviceReset();
    g_XeBufferMgr.OnDeviceReset();
    g_oFXManager.OnDeviceReset();
    g_oXeRenderTargetMgr.OnDeviceReset();
    g_oAfterEffectManager.OnDeviceReset();
    g_oXeShadowManager.OnDeviceReset();
    g_oXeLightShaftManager.OnDeviceReset();
    g_oXeWeatherManager.OnDeviceReset();
    g_oHeatManager.OnDeviceReset();
    g_oWaterManager.OnDeviceReset();
    g_oXeShaderDatabase.OnDeviceReset();

    ERR_OutputDebugString("[XeDX9] XeRenderer::Reset() succeeded\n");
    ERR_OutputDebugString("[XeDX9] Backbuffer resolution: %ux%u\n", m_stParams.BackBufferWidth, m_stParams.BackBufferHeight);

    return S_OK;
}

void XeRenderer::Shutdown(void)
{
#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.RequestThreadEvent(eXeThreadEvent_DynamicVB_FinalUnlock);
	g_oXeRenderer.WaitForEventComplete(eXeThreadEvent_DynamicVB_FinalUnlock);
#endif

#if defined(ACTIVE_EDITORS)
    for (UINT iMat = 0; iMat < XERENDER_MAX_MATERIALS; ++iMat)
    {
        XeMaterial_Destroy(m_apoMaterialPool[iMat]);
        m_apoMaterialPool[iMat] = NULL;
    }
    m_ulNbMaterials = 0;

    if (m_pst2DRequestMaterial)
    {
        XeMaterial_Destroy(m_pst2DRequestMaterial);
        m_pst2DRequestMaterial = NULL;
    }
#endif

    SAFE_DELETE(m_pOceanMesh);
    m_pOceanMesh = NULL;
    XeMaterial_Destroy(m_pOceanMaterial);
    m_pOceanMaterial = NULL;

    MEM_Free(m_astDisplayModes);

    ShutdownUIRects();

    SAFE_DELETE(m_poQuadMesh);
    SAFE_DELETE(m_po640x480QuadMesh);

    // Shutdown all the managers
    g_oXeShaderDatabase.Shutdown();
    g_oWaterManager.Shutdown();
    g_oHeatManager.Shutdown();
    g_oXeLightShaftManager.Shutdown();
    g_oXeWeatherManager.Shutdown();
    g_oXeShadowManager.Shutdown();
    g_oXeRenderTargetMgr.Shutdown();
    g_oRenderStateMgr.Shutdown();
    g_oPixelShaderMgr.Shutdown();
    g_oVertexShaderMgr.Shutdown();

    g_oXeContextManager1.Shutdown();
#ifdef _XENON
    g_oXeContextManager2.Shutdown();
#endif

    g_oXeTextureMgr.Shutdown();
    g_oAfterEffectManager.Shutdown();
    g_XeBufferMgr.Shutdown();
    g_oFXManager.Shutdown();

#if defined(_XENON_RENDERER_USETHREAD)
	// Acquire Ownership One Last Time To Destroy Device
	g_oXeRenderer.AcquireThreadOwnership();
#endif

    // Release the device
    SAFE_RELEASE(m_pD3DDevice);

    // Release the Direct3D interface
    SAFE_RELEASE(m_pD3D);

#if defined(_XENON_RENDERER_USETHREAD)
    // Delete Rendering Thread
    delete m_pThread;
#endif

#if defined(VIDEOCONSOLE_ENABLE)
	// Destroy Video Debugging Console
	VideoConsole_Uninit();
#endif
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::TakeScreenShot
// 
// Role   : Save Backbuffer to file
//          
//          
// Return : void
// 
// Author : Etienne Fournier
// Date   : 15 juillet 2005
//----------------------------------------------------------------------@FHE--
void XeRenderer::TakeScreenShot()
{
#if defined(_XENON)
	char sz_ScreenshotFileName[260];
	char sz_ScreenshotDirName[260];
	
	//test for Screenshot Dir
	static BOOL DirExist = FALSE;
	static BIG_KEY ulCurrentWorldKey = BIG_C_InvalidKey;

	if(!DirExist || (WOR_gpst_CurrentWorld && (ulCurrentWorldKey != WOR_gpst_CurrentWorld->h_WorldKey)))
	{
		if(WOR_gpst_CurrentWorld)
			ulCurrentWorldKey = WOR_gpst_CurrentWorld->h_WorldKey;

		GetScreenShotDirName(sz_ScreenshotDirName);

		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile(sz_ScreenshotDirName, &wfd);
		if(hFind == INVALID_HANDLE_VALUE)
		{
			DirExist = XeCreateDirectory(sz_ScreenshotDirName);
		}
		else
		{
			DirExist = TRUE;

			// Close the find handle.
			FindClose( hFind );
		}
	}

	GetScreenShotFileName(sz_ScreenshotFileName, D3DXIFF_PNG);

	g_oXeTextureMgr.ResolveToScratchBuffer(XESCRATCHBUFFER0_ID);
	D3DXSaveTextureToFile(sz_ScreenshotFileName, D3DXIFF_PNG, g_oXeTextureMgr.GetTextureFromID(XESCRATCHBUFFER0_ID), NULL);
	m_bScreenCapture = FALSE;
#endif
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::ResetFPSStats
// 
// Role   : Reset all FPS statistics.
//          
//          
// Return : void
// 
// Author : Etienne Fournier
// Date   : 09 aout 2005
//----------------------------------------------------------------------@FHE--
void XeRenderer::ResetFPSStats()
{
	m_fAvgFPS = 0.0f;
	m_fMinFPS = Cf_Infinit;
	m_fMaxFPS = 0.0f;
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::ComputeFPS
// 
// Role   : Calculates Frames Per Second whenever 50 frames have elapsed.
//          
//          
// Return : void
// 
// Author : Eric Le
// Date   : 21 Dec 2004
//----------------------------------------------------------------------@FHE--
void XeRenderer::ComputeFPS()
{
	static BOOL b_First = TRUE;

    DWORD dwCurrentTime = GetTickCount();
    DWORD dwDeltaTime = dwCurrentTime - g_dwBaseTime;

    // Update Every Second

    if (dwDeltaTime < 1000)
    {
        g_dwFrameCounter++;
    }
    else
    {
        m_fFPS           = (1000.0f * g_dwFrameCounter) / (float)dwDeltaTime;
		
		//extra FPS info
		if(m_fFPS < m_fMinFPS)
			m_fMinFPS    = m_fFPS;
		if(m_fFPS > m_fMaxFPS)
			m_fMaxFPS    = m_fFPS;
		if(b_First)
		{
			m_fAvgFPS = m_fFPS;
			b_First = FALSE;
		}
		else
		{
			m_fAvgFPS = (m_fAvgFPS + m_fFPS) / 2.0f;
		}

        m_fMeanDeltaTime = g_dwFrameCounter > 0 ? (FLOAT) dwDeltaTime / (FLOAT) g_dwFrameCounter : (FLOAT) dwDeltaTime;
      #if defined(_XENON_RENDER_PC)
        ERR_OutputDebugString("[FPS] %.2f (%i ms)\n", m_fFPS, (INT) m_fMeanDeltaTime);
      #endif
        g_dwBaseTime = dwCurrentTime;
        g_dwFrameCounter = 0;
    }
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::BeforeDisplay
// 
// Role   : Some initialization before the rendering thread starts a new frame
//          Previous rendering thread may not have completed yet so carefull what you change
//          
// Return : void
// 
// Author : Dany Joannette
// Date   : september 13th 2005
//----------------------------------------------------------------------@FHE--
void XeRenderer::BeforeDisplay()
{
#if defined(ACTIVE_EDITORS)
    DiscardHelpers();
#endif
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::AfterDisplay
// 
// Role   : 
//          
// Return : void
// 
// Author : Dany Joannette
// Date   : september 13th 2005
//----------------------------------------------------------------------@FHE--
void XeRenderer::AfterDisplay()
{
    XeContextManager *pSwap = g_pXeContextManagerRender;
    g_pXeContextManagerRender = g_pXeContextManagerEngine;
    g_pXeContextManagerEngine = pSwap;

    if(GDI_gpst_CurDD && !(g_pXeContextManagerRender->ul_DisplayFlags & GDI_Cul_DF_DoNotRender))
    {
        ULONG ul_Color = (GDI_gpst_CurDD->pst_World) ? GDI_gpst_CurDD->pst_World->ul_BackgroundColor : 0;
        if (g_pXeContextManagerRender->ul_DisplayFlags & GDI_Cul_DF_DoNotClear)
        {
            GDI_gpst_CurDD->st_GDI.pfnv_Clear(GDI_Cl_ZBuffer, ul_Color);
            GDI_gpst_CurDD->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotClear;
        }
        else
            GDI_gpst_CurDD->st_GDI.pfnv_Clear(GDI_Cl_ColorBuffer | GDI_Cl_ZBuffer, ul_Color);

        GDI_gpst_CurDD->ul_RenderingCounter++;
    }

#if defined(ACTIVE_EDITORS)
    // Actually convert the line and quad requests to meshes
    QueueRequests();
#endif

    g_oAfterEffectManager.SwitchContexts();
    g_oXeShadowManager.BackupLights();

    g_pXeContextManagerEngine->ResetDynMesh();

    if(GDI_gpst_CurDD && GDI_gpst_CurDD->pst_World)
    {

#if defined(VIDEOCONSOLE_ENABLE)
        VideoConsole_UpdateLinkedVariable(MipMapLODBiasID, &GDI_gpst_CurDD->pst_World->f_XeMipMapLODBias);
        VideoConsole_UpdateLinkedVariable(MaterialLODDetailEnableID, (TBool*)&GDI_gpst_CurDD->pst_World->b_XeMaterialLODDetailEnable);
        VideoConsole_UpdateLinkedVariable(MaterialLODDetailNearID, &GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailNear);
        VideoConsole_UpdateLinkedVariable(MaterialLODDetailFarID, &GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailFar);
        VideoConsole_UpdateLinkedVariable(MaterialLODEnableID, (TBool*)&GDI_gpst_CurDD->pst_World->b_XeMaterialLODEnable);
        VideoConsole_UpdateLinkedVariable(MaterialLODNearID, &GDI_gpst_CurDD->pst_World->f_XeMaterialLODNear);
        VideoConsole_UpdateLinkedVariable(MaterialLODFarID, &GDI_gpst_CurDD->pst_World->f_XeMaterialLODFar);

        VideoConsole_UpdateLinkedVariable(SaturationID,    &GDI_gpst_CurDD->pst_World->f_XeSaturation );
        VideoConsole_UpdateLinkedVariable(Brightness_R_ID, &GDI_gpst_CurDD->pst_World->v_XeBrightness.x );
        VideoConsole_UpdateLinkedVariable(Brightness_G_ID, &GDI_gpst_CurDD->pst_World->v_XeBrightness.y );
        VideoConsole_UpdateLinkedVariable(Brightness_B_ID, &GDI_gpst_CurDD->pst_World->v_XeBrightness.z );
        VideoConsole_UpdateLinkedVariable(ContrastID,      &GDI_gpst_CurDD->pst_World->f_XeContrast );
#endif

        g_pXeContextManagerRender->f_XeMipMapLODBias           = GDI_gpst_CurDD->pst_World->f_XeMipMapLODBias;
        g_pXeContextManagerRender->b_XeMaterialLODDetailEnable = GDI_gpst_CurDD->pst_World->b_XeMaterialLODDetailEnable;
        g_pXeContextManagerRender->f_XeMaterialLODDetailNear   = GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailNear;
        g_pXeContextManagerRender->f_XeMaterialLODDetailFar    = GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailFar;
        g_pXeContextManagerRender->b_XeMaterialLODEnable       = GDI_gpst_CurDD->pst_World->b_XeMaterialLODEnable;
        g_pXeContextManagerRender->f_XeMaterialLODNear         = GDI_gpst_CurDD->pst_World->f_XeMaterialLODNear;
        g_pXeContextManagerRender->f_XeMaterialLODFar          = GDI_gpst_CurDD->pst_World->f_XeMaterialLODFar;

        g_pXeContextManagerRender->b_DrawWideScreenBands       = GDI_gpst_CurDD->b_DrawWideScreenBands;
        g_pXeContextManagerRender->b_EnableColorCorrection     = GDI_gpst_CurDD->b_EnableColorCorrection;

        g_pXeContextManagerRender->ul_DisplayFlags             = GDI_gpst_CurDD->ul_DisplayFlags;

        g_pXeContextManagerRender->f_FieldOfVision             = GDI_gpst_CurDD->st_Camera.f_FieldOfVision;
        g_pXeContextManagerRender->f_FactorX                   = GDI_gpst_CurDD->st_Camera.f_FactorX;
        g_pXeContextManagerRender->f_NearPlane                 = GDI_gpst_CurDD->st_Camera.f_NearPlane;
        g_pXeContextManagerRender->f_FarPlane                  = GDI_gpst_CurDD->st_Camera.f_FarPlane;

        g_pXeContextManagerRender->b_AntiAliasingBlur          = GDI_gpst_CurDD->b_AntiAliasingBlur;

        g_pXeContextManagerRender->ul_XeDiffuseColor           = GDI_gpst_CurDD->pst_World->ul_XeDiffuseColor;
        g_pXeContextManagerRender->ul_XeSpecularColor          = GDI_gpst_CurDD->pst_World->ul_XeSpecularColor;
        g_pXeContextManagerRender->ul_AmbientColor             = GDI_gpst_CurDD->pst_World->ul_AmbientColor;
        g_pXeContextManagerRender->f_XeSpecularShiny           = GDI_gpst_CurDD->pst_World->f_XeSpecularShiny;
        g_pXeContextManagerRender->f_XeSpecularStrength        = GDI_gpst_CurDD->pst_World->f_XeSpecularStrength;

        g_pXeContextManagerRender->f_XeSaturation              = GDI_gpst_CurDD->pst_World->f_XeSaturation;
        g_pXeContextManagerRender->v_XeBrightness              = GDI_gpst_CurDD->pst_World->v_XeBrightness;
        g_pXeContextManagerRender->f_XeContrast                = GDI_gpst_CurDD->pst_World->f_XeContrast;

#ifdef VIDEOCONSOLE_ENABLE
        if( NoColorCorrection )
        {
            g_pXeContextManagerRender->f_XeSaturation              = 1.0f;
            MATH_InitVector( &g_pXeContextManagerRender->v_XeBrightness, 0.0f, 0.0f, 0.0f );
            g_pXeContextManagerRender->f_XeContrast                = 0.0f;
        }
#endif

        MATH_CopyMatrix(&g_pXeContextManagerRender->st_InverseCameraMatrix, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
        MATH_CopyMatrix(&g_pXeContextManagerRender->st_CameraMatrix, &GDI_gpst_CurDD->st_Camera.st_Matrix);
    }

    m_poLastWorldMatrix = NULL;

    g_oWaterManager.BackupState();
    g_oWaterManager.ClearPatchSetStack();

    g_oXeLightShaftManager.BackupState();
    g_oXeLightShaftManager.Update();

    g_oHeatManager.BackupState();
    g_oHeatManager.Update();

    // Reset the weather manager's lists
    g_oXeWeatherManager.Swap();
    g_oXeWeatherManager.Reset();

    // spg2 fire contextualisation
    g_iCurrentFireContext = (g_iCurrentFireContext+1)%2;

    // Make sure all streams are unbound before locking any of them
    g_oRenderStateMgr.UnbindAllStreams();

    // Unlock all dynamic VBs
    BOOL bForceUnlock = FALSE;
#if defined(_XENON_RENDER_PC)
    bForceUnlock = TRUE;
#endif

    // vertex buffer update
    ULONG ulOffset;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
    g_XeBufferMgr.UnLockDynVertexBuffers(bForceUnlock);
    g_XeBufferMgr.Update();
    g_XeBufferMgr.LockDynVertexBuffer(0, &pVertexBuffer, &ulOffset, TRUE);

#if defined(_XENON)
    // Debugging GUI
    g_MenuManager.RenderConsoles();
#endif

#if defined(ACTIVE_EDITORS)
    // Reset the material pool
    m_ulNbMaterials = 0;

    // Reset the 2D line and quad pools
    m_av2DLinePool.resize(0);
    m_av2DQuadPool.resize(0);
    m_av3DQuadPool.resize(0);
#endif

    ComputeFPS();

    m_bHasSPG2WriteAlpha = FALSE;

    g_XeBufferMgr.SwapVBContext();
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::PostRender
// 
// Role   : Maintenance that needs to be done once every frame.
//          Clears the internal lists of objects to render
//          
// Return : void
// 
// Author : Eric Le
// Date   : 21 Dec 2004
//----------------------------------------------------------------------@FHE--
void XeRenderer::PostRender()
{
    InvalidateRenderLists();
}


//----------------------------------------------------------------------@FHB--
// XeTransparentObjectCompare
// 
// Role   : Sort transparent object by order then z
//          
//          
// Return : order
// 
// Author : Dany Joannette
// Date   : 3 Mars 2005
//----------------------------------------------------------------------@FHE--
int XeTransparentObjectCompare(const void* p1, const void* p2)
{
	XeRenderObject* 	pEntry1 = *(XeRenderObject**)p1;
	XeRenderObject* 	pEntry2 = *(XeRenderObject**)p2;

	if(pEntry1->GetOrder() < pEntry2->GetOrder())
		return 1;

	if(pEntry2->GetOrder() < pEntry1->GetOrder())
		return -1;

	if(g_pXeContextManagerRender->GetWorldViewMatrixByIndex(pEntry1->GetWorldViewMatrixIndex())->_43 < g_pXeContextManagerRender->GetWorldViewMatrixByIndex(pEntry2->GetWorldViewMatrixIndex())->_43)
		return 1;

	if(g_pXeContextManagerRender->GetWorldViewMatrixByIndex(pEntry1->GetWorldViewMatrixIndex())->_43 > g_pXeContextManagerRender->GetWorldViewMatrixByIndex(pEntry2->GetWorldViewMatrixIndex())->_43)
		return -1;

	// use order of insertion if all else is the same
	if(pEntry1->GetInsertOrder() < pEntry2->GetInsertOrder())
		return -1;

	return 1;
}

//----------------------------------------------------------------------@FHB--
// XeOpaqueObjectCompare
// 
// Role   : Sort opaque objects by z (roughly front to back)
//          
//          
// Return : order
// 
// Author : Sebastien Comte
// Date   : April 26, 2005
//----------------------------------------------------------------------@FHE--
int XeOpaqueObjectCompare(const void* p1, const void* p2)
{
    XeRenderObject* pEntry1 = *(XeRenderObject**)p1;
    XeRenderObject* pEntry2 = *(XeRenderObject**)p2;

	UCHAR ucLOD1 = pEntry1->GetLOD();
	UCHAR ucLOD2 = pEntry2->GetLOD();

    if (ucLOD1 > ucLOD2)
	{
        return -1;
	}

    if (ucLOD1 < ucLOD2)
	{
        return 1;
	}

    // Use order of insertion if all else is the same

    if (pEntry1->GetInsertOrder() < pEntry2->GetInsertOrder())
	{
        return -1;
	}

    return 1;
}

//----------------------------------------------------------------------@FHB--
// XeSPG2ObjectCompare
// 
// Role   : Sort SPG2 objects
//          
//          
// Return : order
// 
// Author : Dany Joannette
// Date   : 3 Mars 2005
//----------------------------------------------------------------------@FHE--
int XeSPG2ObjectCompare(const void* p1, const void* p2)
{
    XeRenderObject* 	pEntry1 = *(XeRenderObject**)p1;
    XeRenderObject* 	pEntry2 = *(XeRenderObject**)p2;

    if(pEntry1->GetOrder() < pEntry2->GetOrder())
        return 1;

    if(pEntry2->GetOrder() < pEntry1->GetOrder())
        return -1;

    if(
        (pEntry1->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_NORMAL)>pEntry2->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_NORMAL)) && 
        (pEntry1->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_BASE)>pEntry2->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_BASE)) && 
        (pEntry1->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_SPECULAR)>pEntry2->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_SPECULAR))
        )
        return 1;

    if(
        (pEntry1->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_NORMAL)<pEntry2->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_NORMAL)) && 
        (pEntry1->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_BASE)<pEntry2->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_BASE)) && 
        (pEntry1->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_SPECULAR)<pEntry2->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_SPECULAR))
        )
        return -1;

    // use order of insertion if all else is the same
    if(pEntry1->GetInsertOrder() < pEntry2->GetInsertOrder())
        return -1;

    return 1;
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::RenderFrame
// 
// Role   : Render the different lists and UI elements
//          
//          
// Return : void
// 
// Author : Eric Le
// Date   : 21 Dec 2004
//----------------------------------------------------------------------@FHE--
void XeRenderer::RenderFrame()
{
    XEProfileFunction();

    CXBBeginEventObject oEvent("RenderFrame");
    int iWaterObjects = g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_WATER);

    static int iPage = -1;
    g_oXeTextureMgr.DisplayLoadedTextures(iPage);

    // Determine the shadow light set to use for this frame
    g_oXeShadowManager.SortLights();

    // Global mipmap LOD bias
#if defined(ACTIVE_EDITORS)
    GDI_gpst_CurDD->pst_World->f_XeMipMapLODBias = MATH_f_FloatLimit(GDI_gpst_CurDD->pst_World->f_XeMipMapLODBias, -1.0f, 2.0f);
#endif

	bool bWorldIsReady = true;
#ifdef _XENON
	bWorldIsReady = !g_oXeSimpleRenderer.IsActive( );
#endif

    if( bWorldIsReady )
        g_oFXManager.SetGlobalLODBias(g_pXeContextManagerRender->f_XeMipMapLODBias);
    else
        g_oFXManager.SetGlobalLODBias( 0.0f );

    //--------------------------------------------------------------------------------

    // render reflection now since Z-buffer doesn't contain any useful information
    // we assume that the backbuffer and Z buffer are cleared at this point
#ifdef VIDEOCONSOLE_ENABLE
    if (!NoWater) 
#endif
    {
        CXBBeginEventObject oEvent("Water Reflection");

        if (g_oWaterManager.BeginReflection(iWaterObjects))
        {
            g_oWaterManager.RenderReflection(iWaterObjects, 
                                             g_pXeContextManagerRender->GetRenderObjectList(XeRT_WATER), 
                                             g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_REFLECTED), 
                                             g_pXeContextManagerRender->GetRenderObjectList(XeRT_REFLECTED));
            g_oWaterManager.EndReflection();
        }
    }

#ifdef _XE_PREDICATED_TILING
    // Set our tiled render target.
    m_pD3DDevice->SetRenderTarget( 0, g_pTilingRenderTarget );
    m_pD3DDevice->SetDepthStencilSurface( g_pTilingDepthStencil );

    D3DRECT     TilingRects[2] = {{0, 0, 1280, 384}, {0, 384, 1280, 720}};
    D3DVECTOR4  ClearColor = {0, 0, 0, 0};
    m_pD3DDevice->BeginTiling(0,
                              2,
                              TilingRects,
                              &ClearColor,
                              1.0f,
                              0);
#endif

    StartRenderingGlowMask( );

    // Render OPAQUE list
#ifdef VIDEOCONSOLE_ENABLE
    if(!NoDrawOpaque)
#endif
    {
        XEProfileScope("XeRenderer::RenderFrame::Opaques");

        if (g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_OPAQUE) > 1)
        {
            qsort((void*)(g_pXeContextManagerRender->GetRenderObjectList(XeRT_OPAQUE)), 
                  g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_OPAQUE), 
                  sizeof(XeRenderObject*), XeOpaqueObjectCompare);
        }

#if defined(_XENON_OPAQUES_FRONT_TO_BACK)
        if (g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_OPAQUE_NO_FOG) > 1)
        {
            qsort((void*)(g_pXeContextManagerRender->GetRenderObjectList(XeRT_OPAQUE_NO_FOG)), 
                  g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_OPAQUE_NO_FOG), 
                  sizeof(XeRenderObject*), XeOpaqueObjectCompare);
        }
#endif

        g_ulNumCasters = 0;
        g_ulNumReceivers = 0;

        RenderList(XeRT_OPAQUE);
    }

    // SPG2
    {
        XEProfileScope("XeRenderer::RenderFrame::SPG2");

        if (g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_SPG2) > 1)
        {
            qsort((void*)(g_pXeContextManagerRender->GetRenderObjectList(XeRT_SPG2)), 
                  g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_SPG2), 
                  sizeof(XeRenderObject*), XeSPG2ObjectCompare);
        }
        RenderList(XeRT_SPG2OPAQUE);
    }

    // Fog for opaque objects
    if (g_pXeContextManagerRender->IsFogEnabled() && m_bZPassRenderedSomething
#ifdef VIDEOCONSOLE_ENABLE
        && m_bFog
#endif
       )
    {
        XEProfileScope("XeRenderer::RenderFrame::Fog");
        g_oAfterEffectManager.ApplyFog();
    }

    // Render opaque un-fogged list
    {
        XEProfileScope("XeRenderer::RenderFrame::Opaque not fogged");
        RenderList(XeRT_OPAQUE_NO_FOG);
    }

    // Disable global RLI scale and offset for the rest of the geometry
    g_pXeContextManagerRender->SetGlobalRLIScaleAndOffset(1.0f, 0.0f);

    // Render glowing transparent
    RenderTransparentGlowList( );

    // End rendering glow mask
    EndRenderingGlowMask( );

#ifndef _FINAL
    if(g_bShowShadowResult)
    {
        static INT iBufferIdx;
        g_oXeShadowManager.DrawShadowQuad(iBufferIdx);
    }
#endif

    RenderList(XeRT_ZOVERWRITE);
    RenderList(XeRT_ZOVERWRITESPG2OPAQUE);

    // Render water list
#ifdef VIDEOCONSOLE_ENABLE
    if (!NoWater)
#endif
    {
        if(iWaterObjects)
        {
            XEProfileScope("XeRenderer::RenderFrame::Water");
            g_oWaterManager.RenderWater(iWaterObjects, 
                                        g_pXeContextManagerRender->GetRenderObjectList(XeRT_WATER), 
                                        g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_REFLECTED), 
                                        g_pXeContextManagerRender->GetRenderObjectList(XeRT_REFLECTED));
        }
    }

#ifdef VIDEOCONSOLE_ENABLE
    if(!NoDrawTransparent)
#endif
    {
        XEProfileScope("XeRenderer::RenderFrame::Transparents");

        // Render TRANSPARENT list
        if (g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_TRANSPARENT) > 1)
        {
            qsort((void*)(g_pXeContextManagerRender->GetRenderObjectList(XeRT_TRANSPARENT)), 
                  g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_TRANSPARENT), 
                  sizeof(XeRenderObject*), XeTransparentObjectCompare);
        }

        RenderList(XeRT_TRANSPARENT);
    }

    RenderList(XeRT_SPG2TRANSPARENT);

    RenderList(XeRT_ZOVERWRITESPG2TRANSPARENT);

#ifdef VIDEOCONSOLE_ENABLE
    if (!NoHeatShimmering)
#endif
    {
        XEProfileScope("XeRenderer::RenderFrame::RenderHeatShimmerQuadList");
        RenderHeatShimmerQuadList();
    }

#ifdef _XE_PREDICATED_TILING
    m_pD3DDevice->EndTiling(D3DRESOLVE_RENDERTARGET0 | D3DRESOLVE_ALLFRAGMENTS,
                            NULL, g_pFrontBufferTexture[g_dwCurrentBuffer],
                            &ClearColor, 1.0f, 0L, NULL );
#endif

#ifndef _XE_PREDICATED_TILING
    // SKIP ALL THE EFFECTS WHILE TESTING PREDICATED TILING

    // Render the rain
#ifdef VIDEOCONSOLE_ENABLE
    if (!NoRain)
#endif
    {
        XEProfileScope("XeRenderer::RenderFrame::Rain");
        g_oXeWeatherManager.ApplyRain();
    }

    // apply after effects
#ifdef _XENON
	if( m_pVideo == NULL )
#endif
    {
        XEProfileScope("XeRenderer::RenderFrame::AfterEffects");
        g_oAfterEffectManager.ApplyAfterEffects();
    }

#ifdef VIDEOCONSOLE_ENABLE
    if (!NoHeatShimmering)
#endif
    {
        XEProfileScope("XeRenderer::RenderFrame::HotAir");
        g_oHeatManager.RenderHotAirObjects();
    }

    g_oAfterEffectManager.ApplyXInvert();

    if( g_pXeContextManagerRender->b_DrawWideScreenBands )
    {
        g_oAfterEffectManager.ApplyWideScreen();
    }
#endif // _XE_PREDICATED_TILING
    // Render Interface list
    {
        XEProfileScope("XeRenderer::RenderFrame::Interface");
        RenderList(XeRT_INTERFACE);
    }

#if defined(ACTIVE_EDITORS)
    // Render helper list
    {
        XEProfileScope("XeRenderer::RenderFrame::Helpers");
        RenderList(XeRT_HELPERS);
    }
#endif

	// Y position of text on screen (used for FPS and version info)
	FLOAT fFPSAndVersionInfoPosition = 0.90f;

#if defined(DESKTOP_ENABLE)
#if defined(VIDEOCONSOLE_ENABLE)
    if(!VideoConsole_GetConfiguration(VideoConsole::eConfiguration_Display_HideVersionInfo))
#endif
    {
		if( !g_bHideFPSAndVersionInfo )
		{
			Desktop::DrawString(g_XeVersionInfo.GetBigFileVersionInfo(), 0.075f, fFPSAndVersionInfoPosition, 0.13f, 0.13f, 0xffffffff);
			fFPSAndVersionInfoPosition -= 0.03;
			Desktop::DrawString(g_XeVersionInfo.GetEngineVersionInfo(), 0.075f, fFPSAndVersionInfoPosition, 0.13f, 0.13f, 0xffffffff);
			fFPSAndVersionInfoPosition -= 0.03;
		}
    }
#endif

#if defined(DESKTOP_ENABLE)
#if defined(VIDEOCONSOLE_ENABLE)
	if(!VideoConsole_GetConfiguration(VideoConsole::eConfiguration_Display_HideFrameRate))
#endif
	{
		if( !g_bHideFPSAndVersionInfo )
		{
			Desktop::TWChar strFPS[16];
			swprintf(strFPS, L"%4.1f %.1f\n", m_fFPS, m_fMeanDeltaTime);

            DWORD dwColor;
            if(m_fFPS >= 30.0f)
                dwColor = 0xffffffff;   // >= 30 fps : white
            else if(m_fFPS > 25.0f)
                dwColor = 0xff00ff00;   // 25-30 fps : green
            else if(m_fFPS > 20.0f)
                dwColor = 0xffffff00;   // 20-25 fps : yellow
            else
                dwColor = 0xffff0000;   // <= 20 fps : red

			Desktop::DrawString(strFPS, 0.075f, fFPSAndVersionInfoPosition, 0.15f, 0.15f, dwColor);
		}
	}
#endif


#if defined(VIDEOCONSOLE_ENABLE)
    Desktop::TWChar s[2048];
    Desktop::TWChar temp[64];

    swprintf(s, L"FPS: %.1f\n", m_fFPS);
	swprintf(temp, L"Min FPS#: %.1f\n", m_fMinFPS);
	wcscat(s, temp);
	swprintf(temp, L"Max FPS#: %.1f\n", m_fMaxFPS);
	wcscat(s, temp);
	swprintf(temp, L"Avg FPS#: %.1f\n", m_fAvgFPS);
	wcscat(s, temp);
	swprintf(temp, L"Frame #: %d\n", m_uiFrameCounter);
    wcscat(s, temp);
    swprintf(temp, L"DP calls: %d\n", m_uiNumDrawCalls);
    wcscat(s, temp);
    swprintf(temp, L"Num Tris: %d\n", m_uiNumTriangles);
    wcscat(s, temp);
    swprintf(temp, L"Num SPG2: %d\n", m_uiNumSPG2);
    wcscat(s, temp);
    swprintf(temp, L"Total RAM: %dKB\nAvailable RAM: %dKB\n",
        m_stMemStatus.dwTotalPhys/1024,
        m_stMemStatus.dwAvailPhys/1024);
    wcscat(s, temp);
    swprintf(temp, L"Jade Total: %dKB\nJade Avail.: %dKB\n", 
             MEM_gst_MemoryInfo.ul_DynamicMaxSize / 1024,
             (MEM_gst_MemoryInfo.ul_DynamicMaxSize - MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated) / 1024);
    wcscat(s, temp);
    swprintf(temp, L"Wait For Threaded Flip: %d MS\n", m_uiThreadWaitMS);
    wcscat(s, temp);
    swprintf(temp, L"Max Requested Dynamic Meshes: %d\n", m_uiMaxRequestedDynMeshes);
    wcscat(s, temp);

    VideoConsole_UpdateStatistics(s);
#endif

#ifndef _FINAL
    // Display shadow buffers
    if(g_bShowShadowBuffers)
    {
        DrawRectangleEx(0.8125f, 0.00f, 1.00f, 0.25f, 1.0f, 1.0f, 0.0f, 0.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, g_oXeShadowManager.GetShadowBufferHandle(0, g_bShowStaticSB ? 0 : 1));
        DrawRectangleEx(0.8125f, 0.25f, 1.00f, 0.50f, 1.0f, 1.0f, 0.0f, 0.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, g_oXeShadowManager.GetShadowBufferHandle(1, g_bShowStaticSB ? 0 : 1));
        DrawRectangleEx(0.8125f, 0.50f, 1.00f, 0.75f, 1.0f, 1.0f, 0.0f, 0.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, g_oXeShadowManager.GetShadowBufferHandle(2, g_bShowStaticSB ? 0 : 1));
        DrawRectangleEx(0.8125f, 0.75f, 1.00f, 1.00f, 1.0f, 1.0f, 0.0f, 0.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, g_oXeShadowManager.GetShadowBufferHandle(3, g_bShowStaticSB ? 0 : 1));
    }
#endif

#ifdef _XENON
	// video
	if( m_pVideo )
		m_pVideo->DoFrame( );

	// message bar
	CXeMessageBarManager::Get( ).Display( );
#endif

    // Render 2D stuff here
    {
        XEProfileScope("XeRenderer::RenderFrame::UIRectangles");
        RenderUIRectList();
    }

  #if defined(_XENON) && !defined(_FINAL_) && defined(VIDEOCONSOLE_ENABLE)
    if (bSafeFrame)
    {
        CXBBeginEventObject oAEEvent("SafeFrame");
        g_oAfterEffectManager.ApplySafeFrame();
    }
  #endif

    // clear all the lists
    PostRender();
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::PresentFrame
// 
// Role   : Flip back/front buffers
//          
//          
// Return : void
// 
// Author : Eric Le
// Date   : 16 Dec 2004
//----------------------------------------------------------------------@FHE--
void XeRenderer::PresentFrame()
{
    XEProfileFunction();

    HRESULT hr;

    _GSP_BeginRaster(XE_StartRaster+2);

    // Frame tick for the shader database (convert compiled shaders into D3D shaders)
    g_oXeShaderDatabase.Tick();

#ifdef _XENON

#if defined(VIDEOCONSOLE_ENABLE)
    g_bUseFastShadowPath = UseFastShadowPass;
    m_bFastZPrepass = FastZPrePass;
    m_bVisQueries = VisQueries;
    if(!ShadowBlur) g_oXeShadowManager.SetGaussianStrength(0.0f);
    ShadowBufferSize = g_oXeShadowManager.SetShadowBufferSize(m_iShadowBufferSize);
    g_bShowShadowBuffers  = ShowShadowBuffers;
    g_bShowStaticSB       = ShowStaticSB;
    g_bShowShadowResult   = ShowShadowResult;
    g_fGuardBand_X = GuardBand_X;
    g_fGuardBand_Y = GuardBand_Y;
    g_fDiscardBand_X = DiscardBand_X;
    g_fDiscardBand_Y = DiscardBand_Y;

#else
    g_oXeShadowManager.SetShadowBufferSize(m_iShadowBufferSize);
#endif

    m_pD3DDevice->SetRenderState(D3DRS_GUARDBAND_X, *((DWORD*)&g_fGuardBand_X));
    m_pD3DDevice->SetRenderState(D3DRS_GUARDBAND_Y, *((DWORD*)&g_fGuardBand_Y));
    m_pD3DDevice->SetRenderState(D3DRS_DISCARDBAND_X, *((DWORD*)&g_fDiscardBand_X));
    m_pD3DDevice->SetRenderState(D3DRS_DISCARDBAND_Y, *((DWORD*)&g_fDiscardBand_Y));

    m_iVisQueryIndex = 0;

    if ((m_fCurrentGammaFactor      != m_fRequestedGammaFactor) || 
        (m_fCurrentContrastFactor   != m_fRequestedContrastFactor) ||
        (m_fCurrentBrightnessFactor != m_fRequestedBrightnessFactor))
    {
        m_fCurrentGammaFactor      = m_fRequestedGammaFactor;
        m_fCurrentContrastFactor   = m_fRequestedContrastFactor;
        m_fCurrentBrightnessFactor = m_fRequestedBrightnessFactor;

        SetGamma(m_fCurrentGammaFactor, m_fCurrentContrastFactor, m_fCurrentBrightnessFactor);
    }
#endif

#if defined(_XENON_RENDER_PC)
    hr = m_pD3DDevice->TestCooperativeLevel();
    if (FAILED(hr))
    {
        if (hr == D3DERR_DEVICENOTRESET)
        {
            // Reset the device
            if (FAILED(Reset(true, m_hRenderWindow)))
            {
                m_uiFrameCounter++;
                PostRender();
                _GSP_EndRaster(XE_StartRaster+2);
                return;
            }
            hr = S_OK;
        }
        else if (hr == D3DERR_DEVICELOST)
        {
            // Device is lost and not ready to render, skip the frame
            m_uiFrameCounter++;
            PostRender();
            _GSP_EndRaster(XE_StartRaster+2);
            return;
        }

        // Nothing we can do if the calls still failed
        if (FAILED(hr))
        {
            XeValidate(SUCCEEDED(hr), "Direct3D device is in an unstable state, unable to recover");
            _GSP_EndRaster(XE_StartRaster+2);
            return;
        }
    }
#endif // defined(_XENON_RENDER_PC)

    hr = m_pD3DDevice->BeginScene();

    if(IsThereSomethingToRender())
    {
        RenderFrame();
    }
    else
    {
        PostRender();
    }

    hr = m_pD3DDevice->EndScene();
    XeValidate(SUCCEEDED(hr), "EndScene() failed");

#ifdef _XENON
    // reset default gpr allocation
    m_pD3DDevice->SetShaderGPRAllocation(0, 0, 0);
#endif

#ifdef _XE_PREDICATED_TILING
    // Swap to the current front buffer, so we can see it on screen.
    m_pD3DDevice->SynchronizeToPresentationInterval();
    m_pD3DDevice->Swap( g_pFrontBufferTexture[g_dwCurrentBuffer], NULL );

    // Swap buffer usage
    g_dwCurrentBuffer = g_dwCurrentBuffer ? 0L : 1L;
#else
    if(!(g_pXeContextManagerRender->ul_DisplayFlags & (GDI_Cul_DF_DoNotRender | GDI_Cul_DF_DoNotFlip)) )
        hr = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
#endif

#if defined(_XENON_RENDER_PC)
    if (hr == D3DERR_DEVICELOST)
    {
        m_uiFrameCounter++;
        _GSP_EndRaster(XE_StartRaster+2);
        return;
    }
#endif
    ERR_X_Assert(SUCCEEDED(hr));

#if defined(ACTIVE_EDITORS) && defined(SC_DEV)
    {
        // Clear the entire viewport since the debug runtime of D3D will clear to a random but surely annoying color
        D3DVIEWPORT9 stTempVP = { 0, 0, m_stParams.BackBufferWidth, m_stParams.BackBufferHeight, 0.0f, 1.0f };
        m_pD3DDevice->SetViewport(&stTempVP);
        m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
        m_pD3DDevice->SetViewport(&m_stViewport);
    }
#endif

    m_uiFrameCounter++;

#if defined(_XENON) && !defined(_FINAL_)
	if(m_uiFrameCounter == 10)
		ResetFPSStats();
#endif

    Xe_DebugInfo();
    m_uiNumDrawCalls = 0;
    m_uiNumTriangles = 0;

    GlobalMemoryStatus(&m_stMemStatus);

#if defined(_XENON)
    // set gpr allocation in favor of pixel shading
    m_pD3DDevice->SetShaderGPRAllocation(0, GPU_GPRS - m_iPixelShadingGPRCount, m_iPixelShadingGPRCount);

	// Play Video (If Applicable)

	g_pVideoManager->PerformRequests();
#endif

    _GSP_EndRaster(XE_StartRaster+2);
}

BOOL XeRenderer::IsThereSomethingToRender()
{
    // Do not try to render when no world is loaded
#if defined(ACTIVE_EDITORS)
    if (GDI_gpst_CurDD->pst_World == NULL)
        return FALSE;
#endif

    for(UINT uiListIdx = XeRT_FIRSTLIST; uiListIdx < XeRT_NUMTYPES; uiListIdx++)
    {
        if(g_pXeContextManagerRender->GetNbRenderObjectOfList((eXeRENDERLISTTYPE)uiListIdx) > 0)
            return TRUE;
    }

    if(g_pXeContextManagerRender->GetNbUIRects() != 0)
    {
        return TRUE;
    }

#ifdef _XENON
	if( m_pVideo != NULL )
		return TRUE;
#endif

    return FALSE;
}

void XeRenderer::InvalidateRenderLists(void)
{
    g_pXeContextManagerRender->ResetUIRects();
    g_pXeContextManagerRender->ClearMatrixStacks();
    g_pXeContextManagerRender->ClearLightStacks();
    g_pXeContextManagerRender->ClearWYB();
    g_pXeContextManagerRender->ClearSymmetry();
    g_pXeContextManagerRender->ClearRenderLists();

    g_oXeShadowManager.SetCurrentShadowID(XE_INVALIDSHADOWID, -1);

#ifdef _XENON
    //m_pD3DDevice->InsertFence();
#endif

#if defined(ACTIVE_EDITORS)
    // Reset the material pool
    m_ulNbMaterials = 0;

    m_bZBiasEnable = FALSE;
#endif
}

void XeRenderer::Clear(DWORD _dwColor, float _fDepth, DWORD _dwStencil)
{
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    HRESULT hr = m_pD3DDevice->Clear(0, NULL, dwFlags, XeConvertColor(_dwColor), _fDepth, _dwStencil);
    ERR_X_Assert(SUCCEEDED(hr));
}

void XeRenderer::ClearTarget(DWORD _dwColor)
{
    DWORD dwFlags = D3DCLEAR_TARGET;

    HRESULT hr = m_pD3DDevice->Clear(0, NULL, dwFlags, XeConvertColor(_dwColor), 1.0f, 0 );
    ERR_X_Assert(SUCCEEDED(hr));
}

void XeRenderer::ClearDepthStencil(float _fDepth, DWORD _dwStencil)
{
    DWORD dwFlags = D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    HRESULT hr = m_pD3DDevice->Clear(0, NULL, dwFlags, 0x00000000, _fDepth, _dwStencil);
    ERR_X_Assert(SUCCEEDED(hr));
}

void XeRenderer::ClearAlphaOnly(DWORD _dwColor)
{
    // clear alpha channel
    XeRenderObject  oClearObject;
    XeMaterial      oMaterial;

    oClearObject.SetMaterial(&oMaterial);
    oClearObject.SetMesh(m_poQuadMesh);

    oMaterial.SetColorWrite(FALSE);
    oMaterial.SetAlphaWrite(TRUE);
    oMaterial.SetConstantColor(_dwColor);
    oMaterial.SetZState(FALSE, FALSE);
    oMaterial.SetColorSource(MAT_Cc_ColorOp_ConstantColor);

    // render flags
    oClearObject.SetDrawMask(GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    oClearObject.SetPrimType(XeRenderObject::TriangleStrip);

    RenderObject(&oClearObject, XeFXManager::RP_DEFAULT);
}

void XeRenderer::ClearDepthTexture()
{
    // clear alpha channel
    XeRenderObject  oClearObject;
    XeMaterial      oMaterial;

    oClearObject.SetMaterial(&oMaterial);
    oClearObject.SetMesh(m_poQuadMesh);

    oMaterial.SetColorWrite(TRUE);
    oMaterial.SetAlphaWrite(FALSE);
    oMaterial.SetConstantColor(0xFFFFFFFF);
    oMaterial.SetZState(FALSE, FALSE);
    oMaterial.SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    oMaterial.SetAlphaBlend(FALSE);
    oMaterial.SetAlphaTest(FALSE);
    oMaterial.SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    oMaterial.SetCustomPSFeature(5, 2);
   
    // render flags
    oClearObject.SetDrawMask(GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    oClearObject.SetPrimType(XeRenderObject::TriangleStrip);

    RenderObject(&oClearObject, XeFXManager::RP_DEFAULT);
}

void XeRenderer::GetBackbufferResolution(UINT* _puiWidth, UINT* _puiHeight)
{
    *_puiWidth = m_uiRTWidth;
    *_puiHeight = m_uiRTHeight;
}

float XeRenderer::GetBackbufferAspectRatio()
{
    return (float)m_stParams.BackBufferWidth / (float)m_stParams.BackBufferHeight;
}

void XeRenderer::AddHalfViewportPixelOffset( float & u, float & v )
{
    u += m_fHalfViewportPixelWidth;
    v += m_fHalfViewportPixelHeight;
}

#if defined(ACTIVE_EDITORS)

void XeRenderer::SetViewport(LONG _lX, LONG _lY, UINT _uiWidth, UINT _uiHeight)
{
    HRESULT hr;

    if (_lX < 0)
    {
        _uiWidth += _lX;
        _lX       = 0;
    }

    if (_lY < 0)
    {
        _uiHeight += _lY;
        _lY        = 0;
    }

    if (_lX + _uiWidth > m_stParams.BackBufferWidth)
    {
        _uiWidth = m_stParams.BackBufferWidth - _lX;
    }

    if (_lY + _uiHeight > m_stParams.BackBufferHeight)
    {
        _uiHeight = m_stParams.BackBufferHeight - _lY;
    }

    if ((_lX       != m_stViewport.X)     ||
        (_lY       != m_stViewport.Y)     ||
        (_uiWidth  != m_stViewport.Width) ||
        (_uiHeight != m_stViewport.Height))
    {
        m_stViewport.X      = _lX;
        m_stViewport.Y      = _lY;
        m_stViewport.Width  = _uiWidth;
        m_stViewport.Height = _uiHeight;
        hr = m_pD3DDevice->SetViewport(&m_stViewport);
        XeValidate(SUCCEEDED(hr), "Failed to set the viewport");

        // Update normalized viewport
        m_stNormalizedViewport.fPosX = ((float)m_stViewport.X)/((float)m_stParams.BackBufferWidth);
        m_stNormalizedViewport.fPosY = ((float)m_stViewport.Y)/((float)m_stParams.BackBufferHeight);
        m_stNormalizedViewport.fWidth = ((float)m_stViewport.Width)/((float)m_stParams.BackBufferWidth);
        m_stNormalizedViewport.fHeight = ((float)m_stViewport.Height)/((float)m_stParams.BackBufferHeight);
    }
}

void  
XeRenderer::GetViewport(LONG* _plX, LONG* _plY, UINT* _puiWidth, UINT* _puiHeight)
{
     *_plX        = m_stViewport.X;      
     *_plY        = m_stViewport.Y;      
     *_puiWidth   = m_stViewport.Width;  
     *_puiHeight  = m_stViewport.Height;
}

void 
XeRenderer::GetNormalizedViewport(float* _pfX, float* _pfY, float* _pfWidth, float* _pfHeight)
{
    *_pfX       = m_stNormalizedViewport.fPosX;      
    *_pfY       = m_stNormalizedViewport.fPosY;      
    *_pfWidth   = m_stNormalizedViewport.fWidth;  
    *_pfHeight  = m_stNormalizedViewport.fHeight;
}

void XeRenderer::GetViewportResolution(UINT* _puiWidth, UINT* _puiHeight)
{
    *_puiWidth  = m_stViewport.Width;
    *_puiHeight = m_stViewport.Height;
}

FLOAT XeRenderer::GetViewportAspectRatio(void)
{
    return (FLOAT)m_stViewport.Width / (FLOAT)m_stViewport.Height;
}

void  XeRenderer::AdjustUVToViewport( float & u, float & v )
{
    u = m_stNormalizedViewport.fPosX + u * m_stNormalizedViewport.fWidth;
    v = m_stNormalizedViewport.fPosY + v * m_stNormalizedViewport.fHeight;
}

void XeRenderer::DiscardHelpers(void)
{
    g_pXeContextManagerRender->ResetRenderObjectOfList(XeRT_HELPERS);
    m_av2DLinePool.resize(0);
    m_av2DQuadPool.resize(0);
    m_av3DQuadPool.resize(0);
}

void XeRenderer::Add2DLineRequest(FLOAT _fPtAX, FLOAT _fPtAY, FLOAT _fPtAZ, FLOAT _fPtAW,
                                  FLOAT _fPtBX, FLOAT _fPtBY, FLOAT _fPtBZ, FLOAT _fPtBW,
                                  ULONG _ulColor)
{
    TransformedLine line;

    line.stPtA.fX      = _fPtAX;
    line.stPtA.fY      = _fPtAY;
    line.stPtA.fZ      = _fPtAZ;
    line.stPtA.fW      = _fPtAW;
    line.stPtA.ulColor = XeConvertColor(_ulColor);
    line.stPtB.fX      = _fPtBX;
    line.stPtB.fY      = _fPtBY;
    line.stPtB.fZ      = _fPtBZ;
    line.stPtB.fW      = _fPtBW;
    line.stPtB.ulColor = XeConvertColor(_ulColor);

    m_av2DLinePool.push_back(line);
}

void XeRenderer::Add2DQuadRequest(FLOAT _fPt1X, FLOAT _fPt1Y,
                                  FLOAT _fPt2X, FLOAT _fPt2Y,
                                  FLOAT _fZ,    ULONG _ulColor)
{
    TransformedQuad quad;

    quad.fPt1X   = _fPt1X;
    quad.fPt1Y   = _fPt1Y;
    quad.fPt2X   = _fPt2X;
    quad.fPt2Y   = _fPt2Y;
    quad.fZ      = _fZ;
    quad.ulColor = XeConvertColor(_ulColor);

    m_av2DQuadPool.push_back(quad);
}

void XeRenderer::Add3DQuadRequest(D3DXVECTOR4* _pstVertices, ULONG _ulColor)
{
    Transformed3DQuad quad;
    ULONG i;

    for (i = 0; i < 4; ++i)
    {
        quad.p[i] = _pstVertices[i];
    }
    quad.ulColor = XeConvertColor(_ulColor);

    m_av3DQuadPool.push_back(quad);
}

void XeRenderer::QueueRequests(void)
{
    TransformedVertex* pVertices;
    TransformedQuad*   pQuad;
    Transformed3DQuad* p3DQuad;
    XeMesh* pMesh;
    ULONG   ulNbElements;
    ULONG   ulNbRequests;
    ULONG   ulStartElement;
    ULONG   ulBatchSize;
    ULONG   ulDrawMask;
    ULONG   ulVertex;

    // Queue the lines
    ulDrawMask   = 0xffffffff & ~(GDI_Cul_DM_Fogged | GDI_Cul_DM_Lighted | GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);
    ulNbElements = m_av2DLinePool.size();
    if (ulNbElements > 0)
    {
        ulStartElement = 0;
        ulNbRequests   = ulNbElements / MAX_LINES_PER_MESH;
        if ((ulNbElements % MAX_LINES_PER_MESH) > 0)
        {
            ++ulNbRequests;
        }

        while (ulNbRequests > 0)
        {
            if (ulNbElements > MAX_LINES_PER_MESH)
            {
                ulBatchSize = MAX_LINES_PER_MESH;
            }
            else
            {
                ulBatchSize = ulNbElements;
            }

            pMesh = RequestDynamicMesh();
            pMesh->SetStreamComponents(0, XEVC_POSITIONT | XEVC_COLOR0);

            pVertices  = (TransformedVertex*)pMesh->GetStream(0)->pBuffer->Lock(ulBatchSize * 2, sizeof(TransformedVertex));
            L_memcpy(pVertices, &m_av2DLinePool[ulStartElement], ulBatchSize * 2 * sizeof(TransformedVertex));
            pMesh->GetStream(0)->pBuffer->Unlock();

            ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulBatchSize * 2);

            g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
                                            pMesh, m_pst2DRequestMaterial,
                                            ulDrawMask,
                                            -1,
                                            XeRT_HELPERS,
                                            XeRenderObject::LineList,
                                            255, 0, (LPVOID)ROS_HELPERS);

            ulNbElements   -= MAX_LINES_PER_MESH;
            ulStartElement += MAX_LINES_PER_MESH;
            --ulNbRequests;
        }
    }
    m_av2DLinePool.resize(0);

    // Queue the quads
    ulDrawMask   = 0xffffffff & ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Fogged | GDI_Cul_DM_Lighted  | GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);
    ulNbElements = m_av2DQuadPool.size();
    if (ulNbElements > 0)
    {
        ulStartElement = 0;
        ulNbRequests   = ulNbElements / MAX_QUADS_PER_MESH;
        if ((ulNbElements % MAX_QUADS_PER_MESH) > 0)
        {
            ++ulNbRequests;
        }

        while (ulNbRequests > 0)
        {
            if (ulNbElements > MAX_QUADS_PER_MESH)
            {
                ulBatchSize = MAX_QUADS_PER_MESH;
            }
            else
            {
                ulBatchSize = ulNbElements;
            }

            pMesh = RequestDynamicMesh();
            pMesh->SetStreamComponents(0, XEVC_POSITIONT | XEVC_COLOR0);

            pVertices  = (TransformedVertex*)pMesh->GetStream(0)->pBuffer->Lock(ulBatchSize * 4, sizeof(TransformedVertex));

            pQuad    = &m_av2DQuadPool[ulStartElement];
            ulVertex = 0;
            for (ULONG i = 0; i < ulBatchSize; ++i, ++pQuad)
            {
                pVertices[ulVertex].fX      = pQuad->fPt1X;
                pVertices[ulVertex].fY      = pQuad->fPt1Y;
                pVertices[ulVertex].fZ      = pQuad->fZ;
                pVertices[ulVertex].fW      = 1.0f;
                pVertices[ulVertex].ulColor = pQuad->ulColor;
                ++ulVertex;

                pVertices[ulVertex].fX      = pQuad->fPt2X;
                pVertices[ulVertex].fY      = pQuad->fPt1Y;
                pVertices[ulVertex].fZ      = pQuad->fZ;
                pVertices[ulVertex].fW      = 1.0f;
                pVertices[ulVertex].ulColor = pQuad->ulColor;
                ++ulVertex;

                pVertices[ulVertex].fX      = pQuad->fPt2X;
                pVertices[ulVertex].fY      = pQuad->fPt2Y;
                pVertices[ulVertex].fZ      = pQuad->fZ;
                pVertices[ulVertex].fW      = 1.0f;
                pVertices[ulVertex].ulColor = pQuad->ulColor;
                ++ulVertex;

                pVertices[ulVertex].fX      = pQuad->fPt1X;
                pVertices[ulVertex].fY      = pQuad->fPt2Y;
                pVertices[ulVertex].fZ      = pQuad->fZ;
                pVertices[ulVertex].fW      = 1.0f;
                pVertices[ulVertex].ulColor = pQuad->ulColor;
                ++ulVertex;
            }

            pMesh->GetStream(0)->pBuffer->Unlock();

            ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulVertex);

            g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
                                            pMesh, m_pst2DRequestMaterial,
                                            ulDrawMask,
                                            -1,
                                            XeRT_HELPERS,
                                            XeRenderObject::QuadList,
                                            255, 0, (LPVOID)ROS_HELPERS);

            ulNbElements   -= MAX_QUADS_PER_MESH;
            ulStartElement += MAX_QUADS_PER_MESH;
            --ulNbRequests;
        }
    }
    m_av2DQuadPool.resize(0);

    // Queue the 3D quads
    ulDrawMask   = 0xffffffff & ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Fogged | GDI_Cul_DM_Lighted | GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);
    ulNbElements = m_av3DQuadPool.size();
    if (ulNbElements > 0)
    {
        ulStartElement = 0;
        ulNbRequests   = ulNbElements / MAX_QUADS_PER_MESH;
        if ((ulNbElements % MAX_QUADS_PER_MESH) > 0)
        {
            ++ulNbRequests;
        }

        while (ulNbRequests > 0)
        {
            if (ulNbElements > MAX_QUADS_PER_MESH)
            {
                ulBatchSize = MAX_QUADS_PER_MESH;
            }
            else
            {
                ulBatchSize = ulNbElements;
            }

            pMesh = RequestDynamicMesh();
            pMesh->SetStreamComponents(0, XEVC_POSITIONT | XEVC_COLOR0);

            pVertices = (TransformedVertex*)pMesh->GetStream(0)->pBuffer->Lock(ulBatchSize * 4, sizeof(TransformedVertex));

            p3DQuad  = &m_av3DQuadPool[ulStartElement];
            ulVertex = 0;
            for (ULONG i = 0; i < ulBatchSize; ++i, ++p3DQuad)
            {
                for (ULONG j = 0; j < 4; ++j)
                {
                    pVertices[ulVertex].fX      = p3DQuad->p[j].x;
                    pVertices[ulVertex].fY      = p3DQuad->p[j].y;
                    pVertices[ulVertex].fZ      = p3DQuad->p[j].z;
                    pVertices[ulVertex].fW      = p3DQuad->p[j].w;
                    pVertices[ulVertex].ulColor = p3DQuad->ulColor;
                    ++ulVertex;
                }
            }

            pMesh->GetStream(0)->pBuffer->Unlock();

            ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulVertex);

            g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
                                             pMesh, m_pst2DRequestMaterial,
                                             ulDrawMask,
                                             -1,
                                             XeRT_HELPERS,
                                             XeRenderObject::QuadList,
                                             255, 0, (LPVOID)ROS_HELPERS);

            ulNbElements   -= MAX_QUADS_PER_MESH;
            ulStartElement += MAX_QUADS_PER_MESH;
            --ulNbRequests;
        }
    }
    m_av3DQuadPool.resize(0);
}

#endif

//----------------------------------------------------------------------@FHB--
// XeRenderer::QueueMeshForRenderSPG2
// 
//----------------------------------------------------------------------@FHE--
void XeRenderer::QueueMeshForRenderSPG2(MATH_tdst_Matrix *_pstGlobalMatrix, 
									XeMesh* _poMesh,
									XeMaterial* _poMaterial,
									ULONG _ulDrawMask,
									eXeRENDERLISTTYPE _eRenderType,
									XeRenderObject::XEPRIMITIVETYPE _ePrimType,
									UCHAR _ucLOD,
									CHAR _cOrder,
									LPVOID _pUserData,
                                    BOOL _bWriteToAlpha )
{
    ULONG ulExtraFlags = QMFR_LIGHTED;

    SPG2_tdst_Modifier *pst_SPG2 = (SPG2_tdst_Modifier *)_pUserData;
    if(pst_SPG2->ulFlags & SPG2_GridGeneration)
        ulExtraFlags |= QMFR_SPG2GRID;

    if( _bWriteToAlpha )
    {
        m_bHasSPG2WriteAlpha = TRUE;
        ulExtraFlags |= QMFR_SPG2_WRITE_ALPHA;
    }

	QueueMeshForRender(_pstGlobalMatrix, 
		_poMesh,
		_poMaterial,
		_ulDrawMask,
        -1,
        XeRT_SPG2,
		_ePrimType,
		_ucLOD,
		_cOrder,
		_pUserData, 
		XeRenderObject::SPG2,
        0,ulExtraFlags,0);
}

// ------------------------------------------------------------------------------------------------
// XeRenderer::QueueLightShaftForRender
// ------------------------------------------------------------------------------------------------
void XeRenderer::QueueLightShaftForRender(MATH_tdst_Matrix*               _pstGlobalMatrix,
                                          XeMesh*                         _poMesh,
                                          XeMaterial*                     _poMaterial,
                                          ULONG                           _ulDrawMask,
                                          XeRenderObject::XEPRIMITIVETYPE _ePrimType,
                                          UCHAR                           _ucLOD,
                                          CHAR                            _cOrder,
                                          LPVOID                          _pUserData,
                                          ULONG                           _ulIndex,
                                          ULONG                           _ulWorldMatrixIndex,
                                          ULONG                           _ulWorldViewMatrixIndex,
                                          ULONG                           _ulProjectionMatrixIndex)
{
    XeRenderObject* pRenderable;

    QueueMeshForRender(_pstGlobalMatrix,
                       _poMesh,
                       _poMaterial,
                       _ulDrawMask,
                       -1,
                       XeRT_TRANSPARENT,
                       _ePrimType,
                       _ucLOD,
                       _cOrder,
                       _pUserData,
                       XeRenderObject::LightShaft,
                       _ulIndex);

    // special case : we use the g_pXeContextManagerRender because the light shaft queue is done inthe render frame and not by graphicDK
    pRenderable = g_pXeContextManagerRender->GetLastRenderObjectInList(XeRT_TRANSPARENT);

    pRenderable->SetWorldMatrixIndex(_ulWorldMatrixIndex);
    pRenderable->SetWorldViewMatrixIndex(_ulWorldViewMatrixIndex);
    pRenderable->SetProjMatrixIndex(_ulProjectionMatrixIndex);
}

// ------------------------------------------------------------------------------------------------
// XeRenderer::QueueRainForRender
// ------------------------------------------------------------------------------------------------
void XeRenderer::QueueRainForRender(MATH_tdst_Matrix*               _pstGlobalMatrix,
                                    XeMesh*                         _poMesh,
                                    XeMaterial*                     _poMaterial,
                                    ULONG                           _ulDrawMask,
                                    XeRenderObject::XEPRIMITIVETYPE _ePrimType,
                                    UCHAR                           _ucLOD,
                                    CHAR                            _cOrder,
                                    LPVOID                          _pUserData,
                                    ULONG                           _ulWorldMatrixIndex,
                                    ULONG                           _ulWorldViewMatrixIndex,
                                    ULONG                           _ulProjectionMatrixIndex)
{
    XeRenderObject* pRenderable;

    QueueMeshForRender(_pstGlobalMatrix,
                       _poMesh,
                       _poMaterial,
                       _ulDrawMask,
                       -1,
                       XeRT_TRANSPARENT,
                       _ePrimType,
                       _ucLOD,
                       _cOrder,
                       _pUserData,
                       XeRenderObject::Common);

    pRenderable = g_pXeContextManagerEngine->GetLastRenderObjectInList(XeRT_TRANSPARENT);

    pRenderable->SetWorldMatrixIndex(_ulWorldMatrixIndex);
    pRenderable->SetWorldViewMatrixIndex(_ulWorldViewMatrixIndex);
    pRenderable->SetProjMatrixIndex(_ulProjectionMatrixIndex);
}

//----------------------------------------------------------------------@FHB--
// XeRenderer::QueueMeshForRender
// 
// Role   : 
//          
// Params : _poMesh :       ptr to XeMesh
//          _poMaterial :   ptr to XeMaterial
//          _ulDrawMask :   Jade draw mask
//          _eRenderType :  Opaque, Alpha...
//          _ePrimType :    TriangleList or SpriteList
//          
// Return : void
// 
// Author : Eric Le
// Date   : 10 Jan 2005
//----------------------------------------------------------------------@FHE--
void XeRenderer::QueueMeshForRender(MATH_tdst_Matrix *_pstGlobalMatrix, 
                                    XeMesh* _poMesh,
                                    XeMaterial* _poMaterial,
                                    ULONG _ulDrawMask,
                                    ULONG _ulLMTex,
                                    eXeRENDERLISTTYPE _eRenderType,
                                    XeRenderObject::XEPRIMITIVETYPE _ePrimType,
                                    UCHAR _ucLOD,
									CHAR _cOrder,
                                    LPVOID _pUserData,
									XeRenderObject::XEOBJECTTYPE _eObjectType,
                                    ULONG _ulExtraDataIndex,
                                    ULONG _ulExtraFlags,
                                    ULONG _ulLayerIndex)
{
    CXBBeginEventObjectCPU oEvent(0, "Queue");

    BOOL bFurLayer = FALSE;
    XeContextManager *pContext;

    BOOL bIsHeatShimmerGrid = _eRenderType==XeRT_HEAT_SHIMMER_QUADS && (_ulExtraFlags & QMFR_HEATSHIMMER_GRID);

    // all queue is done by graphicDK except for the light shaft
    if(_eObjectType==XeRenderObject::LightShaft || bIsHeatShimmerGrid )
        pContext = g_pXeContextManagerRender;
    else
        pContext = g_pXeContextManagerEngine;

    if(pContext->GetDoublePassZOverwrite())
    {
        if(_eRenderType == XeRT_SPG2)
        {
            SPG2_tdst_Modifier *pSPG2 = (SPG2_tdst_Modifier *)_pUserData;
            if(pSPG2->ulFlags & SPG2_ModeAdd)
                _eRenderType = XeRT_ZOVERWRITESPG2TRANSPARENT;
            else
                _eRenderType = XeRT_ZOVERWRITESPG2OPAQUE;
        }
        else
            _eRenderType = XeRT_ZOVERWRITE;
    }

    ERR_X_Assert((_eRenderType >= XeRT_FIRSTLIST) && (_eRenderType < XeRT_NUMTYPES));
    ERR_X_Assert(_poMesh != NULL);
    ERR_X_Assert(_poMesh->GetStream(0)->pBuffer != NULL);
   
#if defined(ACTIVE_EDITORS)
    if(_eRenderType == XeRT_HELPERS)
    {
        _ulDrawMask &= ~(GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);
    }
#define SELECTION_MASK (GDI_Cul_DM_NotWired | GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontForceColor)
    if ((_poMaterial == NULL) || ((_poMaterial->GetCustomPS() == 0) && (_poMaterial->GetCustomVS() == 0)))
    {
        if ((_poMaterial == NULL) ||
            ((MAT_gst_DefaultSingleMaterial.pst_FirstLevel != NULL) &&
            ((_poMaterial == MAT_gst_DefaultSingleMaterial.pst_FirstLevel->pst_XeMaterial) ||
            ((_ulDrawMask & SELECTION_MASK) == 0))))
        {
            if (m_ulNbMaterials < XERENDER_MAX_MATERIALS)
            {
                _poMaterial = m_apoMaterialPool[m_ulNbMaterials];
                ++m_ulNbMaterials;

                _poMaterial->SetCustomVS(0);
                _poMaterial->SetCustomPS(0);

                _poMaterial->SetTwoSided((_ulDrawMask & GDI_Cul_DM_TestBackFace) == 0);

                if (_ulDrawMask & GDI_Cul_DM_UseTexture )
                {
                    _poMaterial->SetTextureId(XeMaterial::TEXTURE_BASE, 
                        MAT_gst_DefaultSingleMaterial.pst_FirstLevel->s_TextureId);
                }
                else
                {
                    _poMaterial->SetTextureId(XeMaterial::TEXTURE_BASE, MAT_Xe_InvalidTextureId);
                }

                if (_ulDrawMask & GDI_Cul_DM_DontForceColor)
                {
                    _poMaterial->SetColorSource(MAT_Cc_ColorOp_RLI);
                }
                else
                {
                    _poMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
                    _poMaterial->SetConstantColor(GDI_gpst_CurDD->ul_ColorConstant);
                }

                _ulDrawMask &= ~GDI_Cul_DM_Lighted;
                _ulDrawMask &= ~GDI_Cul_DM_Fogged;
                _ulDrawMask |= GDI_Cul_DM_DontForceColor;
            }
            else
            {
                XeValidate(FALSE, "Material pool size is too small");
            }
        }
    }
#endif

    if(_eObjectType!=XeRenderObject::LightShaft && !bIsHeatShimmerGrid) // special path for light shaft and heat shimmer. the queue mesh is called from within the render frame and the global variables are not used nor valid
    {
        if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_RenderingInterface)
            _eObjectType = XeRenderObject::Interface;

        // check for fur rendering
        if((_poMaterial->GetJadeFlags() != 0xffffffff) && (_poMaterial->GetJadeFlags() & MAT_XYZ_Flag_ShiftUsingNormal))
        {
        #ifdef VIDEOCONSOLE_ENABLE
            if (NoFur) return;
        #endif

            bFurLayer    = TRUE;
            _eRenderType = XeRT_TRANSPARENT; // force fur to be in the transparent list
        }

    #if defined(ACTIVE_EDITORS)
        if (GDI_gpst_CurDD->b_DisableBaseMap)
        {
            _ulDrawMask &= ~GDI_Cul_DM_UseTexture;
        }
        if (GDI_gpst_CurDD->b_ShowVertexColor)
        {
            _ulDrawMask |=  GDI_Cul_DM_UseRLI;
            _ulDrawMask &= ~GDI_Cul_DM_Lighted;
        }
    #endif
    }

    // Debug
    //BreakOnGAO((OBJ_tdst_GameObject*)_pUserData, "DEC_ENV_Ciel_Ciel_FP01");

    if(pContext->GetNbRenderObjectOfList(_eRenderType)>=XERENDER_MAXLISTSIZE)
    {
        ERR_X_Assert(FALSE);
        return;
    }

    if(pContext->GetTotalNumberOfRenderObjects()>=XERENDER_RENDEROBJECTPOOLSIZE)
    {
        ERR_X_Assert(FALSE);
        return;
    }

    XeRenderObject* _poObject = pContext->PushRenderObject(_eRenderType);
	_poObject->SetInsertOrder(pContext->GetNbRenderObjectOfList(_eRenderType), _ulLayerIndex);

    if( (_pstGlobalMatrix != NULL) && (_pstGlobalMatrix != m_poLastWorldMatrix))
	{
		MATH_tdst_Matrix stOGLMatrix;
		MATH_MakeOGLMatrix(&stOGLMatrix, _pstGlobalMatrix);

		pContext->PushWorldMatrix((D3DXMATRIX*)&stOGLMatrix);
		m_poLastWorldMatrix = _pstGlobalMatrix;
	}
	
    _poObject->SetWorldMatrixIndex(pContext->GetCurrentWorldMatrixIndex());
    _poObject->SetWorldViewMatrixIndex(pContext->GetCurrentWorldViewMatrixIndex());
    _poObject->SetProjMatrixIndex(pContext->GetCurrentProjMatrixIndex());
    _poObject->SetPlanarGizmoMatrixIndex(pContext->GetCurrentPlanarGizmoMatrixIndex());
	
	if(_poMaterial->GetCustomVS()==CUSTOM_VS_SPG2 || _poMaterial->GetCustomPS()==CUSTOM_PS_SPG2PLANT)
		_poObject->SetSPG2ShadersConsts(pContext->GetSPG2ShadersConsts());

	if(_poMaterial->GetCustomVS()!=0)
	{		
		_poObject->SetCustomVS(_poMaterial->GetCustomVS());
		for(int i=1; i<=XE_VS_CUSTOM_FEATURE_COUNT; i++)
		{
			_poObject->SetCustomVSFeature(i, _poMaterial->GetCustomVSFeature(i));
		}
	}
    else
        _poObject->SetCustomVS(0);

	if(_poMaterial->GetCustomPS()!=0)
	{
		_poObject->SetCustomPS(_poMaterial->GetCustomPS());
		for(int i=1; i<=XE_PS_CUSTOM_FEATURE_COUNT; i++)
		{
			_poObject->SetCustomPSFeature(i, _poMaterial->GetCustomPSFeature(i));
		}
	}
    else
    {
        _poObject->SetCustomPS(0);

        // Material LOD for generic pixel shader
        _poObject->SetMaterialLODDetailState( XeRenderObject::MatLODFull );
        _poObject->SetMaterialLODState( XeRenderObject::MatLODFull );
      
        if(_eObjectType!=XeRenderObject::LightShaft) // special path for light shaft. the queue mesh is called from within the render frame and the global variables are not used nor valid
        {
            if( GDI_gpst_CurDD->pst_World->b_XeMaterialLODDetailEnable || GDI_gpst_CurDD->pst_World->b_XeMaterialLODEnable )
            {
                float fDistance = XeDistanceCameraToBV( (D3DXVECTOR3*)&GDI_gpst_CurDD->st_Camera.st_Matrix.T,
                    _poMesh, 
                    pContext->GetCurrentWorldMatrix() );
          
                if( GDI_gpst_CurDD->pst_World->b_XeMaterialLODDetailEnable )
                {
                    float fRatio = (fDistance - GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailNear) / (GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailFar - GDI_gpst_CurDD->pst_World->f_XeMaterialLODDetailNear );
                    if( fRatio >= 1.0f )
                    {
                        _poObject->SetMaterialLODDetailState( XeRenderObject::MatLODOptimize );
                    }
                    else if( fRatio > 0.0f )
                    {
                        _poObject->SetMaterialLODDetailState( XeRenderObject::MatLODBlend );
                        _poObject->SetMaterialLODDetailBlend( 1.0f - fRatio );
                    }
                }

                if( GDI_gpst_CurDD->pst_World->b_XeMaterialLODEnable )
                {
                    float fRatio = (fDistance - GDI_gpst_CurDD->pst_World->f_XeMaterialLODNear) / (GDI_gpst_CurDD->pst_World->f_XeMaterialLODFar - GDI_gpst_CurDD->pst_World->f_XeMaterialLODNear );
                    if( fRatio >= 1.0f )
                    {
                        _poObject->SetMaterialLODState( XeRenderObject::MatLODOptimize );
                    }
                    else if( fRatio > 0.0f )
                    {
                        _poObject->SetMaterialLODState( XeRenderObject::MatLODBlend );
                        _poObject->SetMaterialLODBlend( 1.0f - fRatio );
                    }
                }
            }
        }

        // fur rendering
        if (bFurLayer)
        {
            _poObject->UpdateFurInfo();
            _eObjectType = XeRenderObject::Fur;
        }
    }

    // Rim Light can only be applied to default shaders
    if(_poMaterial->IsRimLightEnabled() && _poMaterial->GetCustomPS() == 0 && _poMaterial->GetCustomVS() == 0)
    {
        _poObject->EnableRimLight( true );

        // Set rim light params from current global values
        tdstRimLightPSConsts stRimLightPSConsts;
        stRimLightPSConsts.m_fWidthMin                      = _poMaterial->GetRimLightWidthMin();
        stRimLightPSConsts.m_fWidthMax                      = _poMaterial->GetRimLightWidthMax();
        stRimLightPSConsts.m_fLerp                          = _poMaterial->GetRimLightNormalMapRatio();
        stRimLightPSConsts.m_fIntensity                     = _poMaterial->GetRimLightIntensity();
        stRimLightPSConsts.m_bSpecularMapAttenuationEnabled = _poMaterial->IsRimLightSpecularMapAttenuation();

        tdstRimLightVSConsts stRimLightVSConsts;
        stRimLightVSConsts.m_isHeightAttenuationEnabled = IsRimLightHeightAttenuationEnabled;
        if( IsRimLightHeightAttenuationEnabled )
        {
            stRimLightVSConsts.m_fHeightWorldMin        = RimLightHeightWorldMin;
            stRimLightVSConsts.m_fHeightWorldMax        = RimLightHeightWorldMax;
            stRimLightVSConsts.m_fHeightAttenuationMin  = RimLightHeightAttenuationMin;
            stRimLightVSConsts.m_fHeightAttenuationMax  = RimLightHeightAttenuationMax;
        }
      
        _poObject->SetRimLightVSConsts( stRimLightVSConsts );
        _poObject->SetRimLightPSConsts( stRimLightPSConsts );
    }
    else
    {
        _poObject->EnableRimLight( false );
    }

#if defined(ACTIVE_EDITORS)
    if (m_bZBiasEnable)
    {
      _ulExtraFlags |= QMFR_ZBIAS;
    }
#endif

    _poObject->SetMesh(_poMesh);
    _poObject->SetMaterial(_poMaterial);
    _poObject->CopyMaterial(); // use a copy and not the original
    _poObject->SetPrimType(_ePrimType);
    _poObject->SetUserData(_pUserData);
    _poObject->SetDrawMask(_ulDrawMask);
    _poObject->SetLOD(_ucLOD);
	_poObject->SetOrder(_cOrder);
	_poObject->SetObjectType(_eObjectType);
    _poObject->SetExtraFlags(_ulExtraFlags);
    _poObject->SetExtraDataIndex(_ulExtraDataIndex);

    _poObject->SetLMTexture(_ulLMTex);

	_poObject->bDynamic = _poMesh->IsDynamic();

    // lighting info
    if(_ulExtraFlags & QMFR_LIGHTED)
    {
        if( _poMesh->UseLightSet() )
        {
            _poObject->SetLightSetIndex( _poMesh->GetLightSetIndex() );
        }
        else
        {
            _poObject->SetLightSetIndex( pContext->GetCurrentLightSetIndex() );
        }
    }
    else
    {
        // Light set 0 is always empty
        _poObject->SetLightSetIndex(0);
    }

    if((_ulExtraFlags & QMFR_WYB1))
    {
        INT iCurIdx = pContext->GetCurrentWYBParamsIndex();
        if(_ulExtraFlags & QMFR_WYB2)
        {
            _poObject->SetWYBIndices(iCurIdx - 1, iCurIdx - 1);
            _poObject->SetCustomVS(11);
        }
        else
        {
            _poObject->SetWYBIndices(iCurIdx - 1, -1);
            _poObject->SetCustomVS(11);
        }

        if(bFurLayer)
        {
            XeWYBParams* pWYBParams = pContext->GetWYBParams(iCurIdx - 1);
            pWYBParams->Values[0].w = 1.0f;
            pWYBParams->Values[1].w = 1.0f;
        }
    }
    else
    {
        _poObject->SetWYBIndices(-1, -1);
    }

    // Symmetry modifier
    if(_ulExtraFlags & QMFR_SYMMETRY)
    {
        _poObject->SetSymmetryIndex(pContext->GetCurrentSymmetryParamsIndex() - 1);
        _poObject->SetCustomVS(CUSTOM_VS_SYMMETRY);

        // We'll flip the vertices, so invert the culling mode
        _poObject->SetDrawMask(_poObject->GetDrawMask() ^ GDI_Cul_DM_NotInvertBF);
    }

    // skinning info
    _poObject->SetSkinningSet(pContext->GetLastSkinningSet());

    if(_eObjectType==XeRenderObject::LightShaft)
    {
        _poObject->SetBlendingMode(_poMaterial->GetJadeFlags());
    }
    else
    {
        // make a copy of the blending mode
        if(_poMaterial)
        {
            if(bIsHeatShimmerGrid)
            {
                _poObject->SetBlendingMode(_poMaterial->GetJadeFlags());
            }
            else
            {
                _poObject->SetBlendingMode((GDI_gpst_CurDD->LastBlendingMode == -1) ? _poMaterial->GetJadeFlags() : GDI_gpst_CurDD->LastBlendingMode);

                if (_poMaterial->IsReflected() && (_eObjectType != XeRenderObject::SPG2))
                {
                    // also add the object in the reflected list
                    pContext->AddRenderObjectToList(XeRT_REFLECTED, _poObject);
                }
            }
        }

        if( _poMaterial->GetCustomVS() == CUSTOM_VS_HEATSHIMMERING )
        {
            _poObject->SetHeatShimmerVSConsts( pContext->GetHeatShimmerConsts() );
        }

        if (_eObjectType == XeRenderObject::Water)
        {
            _poObject->SetWaterPatchSetIndex( g_oWaterManager.GetCurrentWaterPatchSetIndex() );
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderList
// Params : _raList     : List to render
//          _eListType  : List type (opaque, transparent, ...)
// RetVal : None
// Descr. : Render a list of objects
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderList(eXeRENDERLISTTYPE _eListType)
{
#ifdef USE_PIX
    char s[256];
    strcpy(s, "XeRenderer::RenderList(");
    strcat(s, GetListName(_eListType));
    strcat(s, ")");
    CXBBeginEventObject oEvent(s);
#endif

#if defined(ACTIVE_EDITORS)
    bool bForceColor = ((GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_DontForceColor) == 0);
#else
    const bool bForceColor = false;
#endif

#ifdef VIDEOCONSOLE_ENABLE
    if(NoRenderList)
        return;
#endif
    m_eCurrentList = _eListType;

    ULONG ulNbElements = g_pXeContextManagerRender->GetNbRenderObjectOfList(_eListType);
    ULONG ulElementIdx;

    XeRenderObject*  pObj;
    XeRenderObject** pROList;

    INT iNumShadowLights = min(g_oXeShadowManager.GetNumLights(), XE_MAXNUMSHADOWS);
    iNumShadowLights = min(iNumShadowLights, m_iMaxNumShadows);

    if (_eListType == XeRT_OPAQUE)
    {
        // Render opaque elements into shadow buffers
        RenderShadowBuffers(iNumShadowLights);

/*
#ifdef _XENON
        // This extra clear fixes garbage in the background on Xenon.
        if( !g_bIgnoreShadowClear )
            ClearTarget(m_dwBackgroundColor);
#endif
*/

        // Render a depth only pass (for opaque elements, and special double zpass objects)
        RenderZPrepass(XeRT_OPAQUE);
    }

    // Render and blur a selected light shaft since the depth texture is now available
    if ((_eListType == XeRT_OPAQUE)
#ifdef VIDEOCONSOLE_ENABLE
        && !NoLightShafts
#endif
        )
    {
        // Light shaft pre-render
        g_oXeLightShaftManager.PreRender();
    }

    // Calculate shadow mask
    if((_eListType == XeRT_OPAQUE) && 
       (iNumShadowLights != 0) && 
       (g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_OPAQUE) > 0))
    {
        g_oXeShadowManager.CalculateShadowResult();

#ifdef _XENON
        // This extra clear fixes garbage in the background on Xenon.
        // TODO: optimize this by clearing in the last resolve of CalculateShadowResult()
		if( !g_bIgnoreShadowClear )
            ClearTarget(m_dwBackgroundColor);
#endif
    }

    if(_eListType == XeRT_ZOVERWRITESPG2OPAQUE)
    {
        RenderSPG2List(eSPG2PassZOverwriteOpaque);
    }
    else if(_eListType == XeRT_ZOVERWRITESPG2TRANSPARENT)
    {
        RenderSPG2List(eSPG2PassZOverwriteTransparent);
    }
    else if(_eListType == XeRT_SPG2OPAQUE)
    {
        RenderSPG2List(eSPG2PassOpaque);
    }
    else if(_eListType == XeRT_SPG2TRANSPARENT)
    {
        RenderSPG2List(eSPG2PassTransparent);
    }
    else

    {
        if (_eListType == XeRT_OPAQUE)
            g_oFXManager.SetRenderingOpaques(TRUE);

        if (_eListType == XeRT_INTERFACE)
            g_oFXManager.SetRenderingInterface(TRUE);

        pROList = g_pXeContextManagerRender->GetRenderObjectList(_eListType);

        // Multi-pass on each object
        for(ulElementIdx = 0; ulElementIdx < ulNbElements; ++ulElementIdx,pROList++)
        {
            pObj = *pROList;//m_apRenderObjectLists[_eListType][ulElementIdx];

            if (pObj->GetExtraFlags() & QMFR_CULLEDFROMCAMERA)
                continue;

            // reset light count
            g_oPixelShaderMgr.SetPPLightCount(0);

            if (
                (pObj->GetDrawMask() & GDI_Cul_DM_Lighted)
#if defined(ACTIVE_EDITORS)
                && !bForceColor
                && (_eListType != XeRT_HELPERS)
                && !GDI_gpst_CurDD->b_ShowVertexColor
#endif
            )
            {
                ULONG ulLightSetIndex = pObj->GetLightSetIndex();
 
                BOOL  bVertexLighting = (pObj->GetMaterial()->GetTextureId(XeMaterial::TEXTURE_NORMAL) == -1);
                ULONG ulNbLights      = g_pXeContextManagerRender->GetLightSetLightCount(ulLightSetIndex);
                if (ulNbLights > 0 )
                {
                    // Additive pass for each light
                    ULONG ulRemainingLights = min(ulNbLights, GetMaxTotalLightCount());
                    ULONG ulLightCount;
                    ULONG uiLightIdx;
                    int iChannel;
                    int iShadowCount = 0;
                    INT aiShadowChannels[VS_MAX_LIGHTS];
                    BOOL abRimLightEnabled[VS_MAX_LIGHTS];
                    BOOL bRimLightEnabled = !bVertexLighting && pObj->IsRimLightEnabled();

                    // parse each light in the light set to find shadow lights and rim lights
                    for(uiLightIdx = 0; uiLightIdx < ulRemainingLights; uiLightIdx++)
                    {
                        ULONG ulType;
                        XeLight* poLight = g_pXeContextManagerRender->GetLight(ulLightSetIndex, uiLightIdx, &ulType);

                        if(g_oXeShadowManager.IsLightUsedForShadows(poLight->JadeLight, iChannel))
                        {
                            ERR_X_Assert( iShadowCount < 4 );
                            iShadowCount++;
                        }
                        aiShadowChannels[uiLightIdx] = iChannel;
                        abRimLightEnabled[uiLightIdx] = bRimLightEnabled ? poLight->IsRimLight : FALSE;
                    }
                    g_oFXManager.SetShadowCount(iShadowCount);

                    if (bVertexLighting && (iShadowCount < 1))
                    {
                        // Vertex lighting without shadows
                        RenderObject(pObj, XeFXManager::RP_APPLY_VERTEXLIGHTING);
                    }
                    else
                    {
                        XeFXManager::RenderPass ePassType = bVertexLighting ? XeFXManager::RP_APPLY_VERTEXLIGHTING : XeFXManager::RP_APPLY_PIXELLIGHTING_FIRSTPASS;
                        for(uiLightIdx = 0; (uiLightIdx < ulNbLights) && (ulRemainingLights > 0); )
				        { 
                            // find light count for this pass
                            ulLightCount = min(ulRemainingLights, GetMaxLightsPerPass( FALSE ));

                            // See if there is at least one rim light for this pass
                            BOOL hasRimLight = FALSE;

                            // setup VS for multi-light rendering
                            g_oPixelShaderMgr.SetPPLightCount(bVertexLighting ? 0 : ulLightCount);
                            g_oVertexShaderMgr.SetBaseLightIndex(uiLightIdx);
                            for (ULONG ulCurLight = 0; ulCurLight < VS_MAX_LIGHTS_PER_PASS; ulCurLight++)
                            {
                                INT ulLightIndex = uiLightIdx + ulCurLight;
                                if (ulCurLight < ulLightCount)
				                {
                                    g_pXeContextManagerRender->SetPerPixelLightIndex(ulCurLight, ulLightIndex);
                                    g_oPixelShaderMgr.SetShadowChannels( ulCurLight, aiShadowChannels[ulLightIndex]);

                                    if (abRimLightEnabled[ulLightIndex])
                                        hasRimLight = TRUE;
                                }
                                else
                                {
                                    g_pXeContextManagerRender->SetPerPixelLightIndex(ulCurLight, -1);
                                    g_oPixelShaderMgr.SetShadowChannels( ulCurLight, -1 );
                                }
                            }

                            g_oVertexShaderMgr.RimLightEnable( hasRimLight );

                            // render this pass using ulLightCount lights
                            RenderObject(pObj, ePassType);

                            // no vertex lighting / multipass for now
                            if (bVertexLighting)
                                break;

                            uiLightIdx += ulLightCount;
                            ulRemainingLights -= ulLightCount;
                            ePassType = XeFXManager::RP_APPLY_PIXELLIGHTING;
				        }
                    }
                }
                else
                {
                    // No lighting
                    RenderObject(pObj, XeFXManager::RP_DEFAULT);
                }
            }
            else
            {
                // Editor gizmos and 2D elements such as rects and interface should use this code path
                RenderObject(pObj, XeFXManager::RP_DEFAULT);
            }
        }

        // reset light count
        g_oPixelShaderMgr.SetPPLightCount(0);

        g_oFXManager.SetRenderingOpaques(FALSE);
        g_oFXManager.SetRenderingInterface(FALSE);
    }

    g_pXeContextManagerRender->ResetRenderObjectOfList(_eListType);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderShadowBuffers
// Params : _pObject : Object to render
//          _eFXPass : Current FX pass
// RetVal : None
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderShadowBuffers(INT _iNumShadowLights)
{
    XeRenderObject* pObj;
    ULONG ulNbElements = g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_OPAQUE);
    ULONG ulElementIdx;
    BOOL bFirstElement = TRUE;

    if (ulNbElements == 0)
    {
        ClearTarget(0x0);
        return;
    }

    // Pre-render all objects in the shadow buffer(s) first
    CXBBeginEventObject oEvent("All Shadow Buffers");
    for(INT iMipIdx = 0; iMipIdx < 2; iMipIdx++)
    {
        for(INT iShadowIdx = 0; iShadowIdx < _iNumShadowLights; iShadowIdx++)
        {
            CXBBeginEventObject oEvent("RenderToShadowBuffer");
            g_oXeShadowManager.BeginShadowBuffer(iShadowIdx, iMipIdx, (iMipIdx+iShadowIdx == 0));

            XeRenderObject** pROList = g_pXeContextManagerRender->GetRenderObjectList(XeRT_OPAQUE);

            // Render all elements into current shadow buffer
            for(ulElementIdx = 0; ulElementIdx < ulNbElements; ++ulElementIdx,pROList++)
            {
                pObj = *pROList;

                DWORD dwExtraFlags = pObj->GetExtraFlags();
                if(iMipIdx == 0 )
                {
                    if(dwExtraFlags & QMFR_ANIMATED)
                        continue;
                }
                else
                {
                    if(!(dwExtraFlags & QMFR_ANIMATED))
                        continue;
                }
#ifdef _ACTIVE_EDITORS
                if((pObj->GetDrawMask() & GDI_Cul_DM_NotWired) != GDI_Cul_DM_NotWired)
                {
                    continue;
                }
#endif
                if((pObj->GetLayerIndex() == 0) &&
                    (pObj->GetDrawMask() & GDI_Cul_DM_EmitShadowBuffer) &&
                    g_oXeShadowManager.IsVisibleFromLight(pObj))
                {
                    // Save material
                    XeMaterial* pBackupMat = pObj->GetMaterial();
                    XeMaterial* pShadowMat = g_oXeShadowManager.GetMaterial(pBackupMat);

                    // Assign special shadow material
                    pObj->SetMaterial(pShadowMat);

                    // Custom render the object
                    if(bFirstElement || !g_bUseFastShadowPath)
                    {
                        //*****************************************************
                        // Generic path
                        //*****************************************************
                        RenderObject(pObj, XeFXManager::RP_COMPUTE_SHADOW);

                      #ifdef VIDEOCONSOLE_ENABLE
                        bFirstElement = !g_bUseFastShadowPath;
                      #else
                        bFirstElement = FALSE;
                      #endif
                    }
                    else
                    {
                        //*****************************************************
                        // Fast path
                        //*****************************************************
/*
                        // Get the right blending mode
                        XeMaterial* pMat    = pObj->GetMaterial();
                        ULONG       ulOldBM = pMat->GetJadeFlags();
                        pMat->SetJadeFlags(pObj->GetBlendingMode());
*/
#if defined(PIX_ENABLE)

                        // Generate PIX Identifier String
                        char acPIXString[256];

                        if (pObj->GetExtraFlags() & QMFR_GAO)
                        {
                            OBJ_tdst_GameObject *_pGAO = (OBJ_tdst_GameObject *)pObj->GetUserData();
                            sprintf(acPIXString, "RO:%s, Lay %d, Mat %0x", _pGAO->sz_Name, pObj->GetLayerIndex(), pObj->GetMaterial());
                        }
                        else
                        {
                            sprintf(acPIXString, "RO:Non-GAO, Lay %d, Mat %0x", pObj->GetLayerIndex(), pObj->GetMaterial());
                        }
                        CXBBeginEventObject oEvent(0xFFFF0000, acPIXString);
#endif

                        // Pre-render setup
//*****************************************************************************
                        int iNbOfPrimitiveDraw = 1;
                        g_pXeContextManagerRender->SetCurrentRenderObject(pObj);
                        XeFurInfo *poFurConsts = NULL;

                        // Use the blending modes as they were in QueueMeshForRender()
                        XeMaterial* pMaterial = pObj->GetMaterial();
                        ULONG       ulOldBM   = pMaterial->GetJadeFlags();
                        pMaterial->SetJadeFlags(pObj->GetBlendingMode());

                        XeMaterial *pCurrentMaterial  = pObj->GetMaterial();
                        XeMesh     *pCurrentMesh      = pObj->GetMesh();
                        ULONG      ulCurrentDrawMask  = pObj->GetDrawMask();
                        XeVertexShaderManager::XeVertexDeclaration *pCurrentDeclaration = g_oVertexShaderMgr.GetVertexDeclaration(pCurrentMesh);

                        // Reset all the features
                        g_oVertexShaderMgr.ResetAllFeatures();

                        // Culling
                        if (!pCurrentMaterial->IsTwoSided())
                        {
                            if (ulCurrentDrawMask & GDI_Cul_DM_NotInvertBF)
                            {
                                g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                            }
                            else
                            {
                                g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
                            }
                        }
                        else
                        {
                            g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
                        }

                        // send vertex declaration
                        g_oRenderStateMgr.SetVertexDeclaration(pCurrentDeclaration->pDeclaration);

                        // Skinning
                        g_pXeContextManagerRender->SetCurrentSkinningSet(pObj->GetSkinningSet());
                        g_oVertexShaderMgr.SetMaxWeight(pCurrentMesh->GetMaxWeights());
                        g_oVertexShaderMgr.SetFeatureBonesConfig(pCurrentMesh->GetMaxBones());
                        g_oVertexShaderMgr.SetFeatureLightsConfig(pCurrentMesh->GetMaxBones());

//*********************************************************************
                        // feature context
                        switch((unsigned int)pObj->GetObjectType())
                        {
                        case XeRenderObject::Common:
                            {
                                // Custom shader features are set in the material
                                g_oPixelShaderMgr.SetFeatureCustomShader(CUSTOM_PS_RENDERSHADOW);
                                g_oPixelShaderMgr.SetFeatureCustom(2, pShadowMat->IsUsingLocalAlpha());
                                g_oPixelShaderMgr.SetFeatureCustom(3, pShadowMat->IsAlphaTestEnabled());

                                g_oVertexShaderMgr.SetFeatureCustomShader(CUSTOM_VS_SHADOW);

                                XeVertexShaderManager::XeVertexDeclaration* pDecl = g_oVertexShaderMgr.GetVertexDeclaration(pObj->GetMesh());

                                if ((pDecl->eAttributes.ulTransformType == TRANSFORM_SKINNING) &&
                                    (g_pXeContextManagerRender->GetCurrentRenderObject()->GetSkinningSet()->ulSkinningMatrixCount == 0))
                                {
                                    g_oVertexShaderMgr.SetFeatureCustom(1, TRANSFORM_NORMAL);
                                }
                                else
                                {
                                    g_oVertexShaderMgr.SetFeatureCustom(1, pDecl->eAttributes.ulTransformType);
                                }
                                g_oVertexShaderMgr.SetFeatureCustom(2, 0);
                                pShadowMat->SetCustomVSFeature(2, 0);

                                D3DXMATRIX WorldView;

                                // Set the WorldView matrix index
                                D3DXMATRIX* pWorld = g_pXeContextManagerRender->GetCurrentRenderObject()->GetWorldMatrix();

                                D3DXMatrixMultiply(&WorldView, pWorld, g_oXeShadowManager.GetViewMatrix(-1));
                                g_pXeContextManagerRender->GetCurrentRenderObject()->SetShadowWorldViewMatrixIndex(g_pXeContextManagerRender->PushWorldViewMatrix(&WorldView));

                                // Set the projection matrix index
                                const D3DXMATRIX*Projection = g_oXeShadowManager.GetProjMatrix(-1);
                                g_pXeContextManagerRender->GetCurrentRenderObject()->SetShadowProjMatrixIndex(g_pXeContextManagerRender->PushProjectionMatrix(Projection));

                                g_pXeContextManagerRender->SetWorldViewMatrix(g_pXeContextManagerRender->GetCurrentRenderObject()->GetShadowWorldViewMatrixIndex());
                                g_pXeContextManagerRender->SetProjectionMatrix(g_pXeContextManagerRender->GetCurrentRenderObject()->GetShadowProjMatrixIndex());

                                g_oRenderStateMgr.SetBlendState(FALSE);
                                g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

                                g_oRenderStateMgr.SetTexture(0, pShadowMat->GetTextureId(0));
                            }
                            break;

                        case XeRenderObject::SPG2:
                        case XeRenderObject::Fur:
                        case XeRenderObject::LightShaft:
                        case XeRenderObject::Rain:
                        case XeRenderObject::Water: 
                        case XeRenderObject::Sprites:
                        case XeRenderObject::Particles:
                        default:
                            // Restore material
                            pMaterial->SetJadeFlags(ulOldBM);
                            pObj->SetMaterial(pBackupMat);
                            continue;
                            break;
                        }

                        // Set the flags back to normal
                        pMaterial->SetJadeFlags(ulOldBM);
//*****************************************************************************

                        // Update pixel shader
                        g_oPixelShaderMgr.Update(pObj);

                        // Send object to VS Manager
                        g_oVertexShaderMgr.Update(pObj);

                        // Update RenderStateMgr
                        g_oRenderStateMgr.Update(pObj);

                        for(int iPrimitive = 0; iPrimitive < iNbOfPrimitiveDraw; iPrimitive++)
                        {
                            HRESULT hr;
                            XeMesh*			 poMesh			  =	pObj->GetMesh(); // TODO : might want to check before and do a special custon draw when the mesh is NULL
                            XeIndexBuffer*   poIndices        = poMesh->GetIndices();
                            UINT             uiNumFaces       = pObj->GetPrimitiveCount();
                            ULONG            ulVertexCount    = 0;
                            D3DPRIMITIVETYPE ePrimitiveType   = pObj->GetPrimType();

#if defined(_XENON_RENDER_PC)
                            // Fake quad list support with indexed triangles and a special index buffer
                            if (ePrimitiveType == D3DPT_QUADLIST)
                            {
                                XeValidate(poIndices == NULL, "A QuadList mesh must not use indices on PC!");
                                poIndices      = g_XeBufferMgr.GetSpriteIB();
                                ePrimitiveType = D3DPT_TRIANGLELIST;
                            }
#endif
                            /*
                            OBJ_tdst_GameObject *pst_GO = NULL;

                            if(pObj->GetExtraFlags() & QMFR_GAO)
                            {
                                pst_GO = (OBJ_tdst_GameObject *)pObj->GetUserData();
                            }

                            if (pst_GO)
                            {
                                MDF_ApplyAllRender(pst_GO);
                            }
                            */
                            if (uiNumFaces)
                            {
                                if (poIndices)
                                {
#if defined(_XENON_RENDER_PC)
                                    ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif
                                    hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                                        0,                          // base index
                                        0,                          // MinIndex - unused
                                        ulVertexCount,              // NumVertices - unused
                                        0,                          // start index
                                        uiNumFaces);
                                }
                                else
                                {
                                    hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                                        0,                         // start vertex
                                        uiNumFaces);
                                }

                                ERR_X_Assert(SUCCEEDED(hr));

                                m_uiNumTriangles += uiNumFaces;
                                m_uiNumDrawCalls++;
                            }
                            /*
                            if(pst_GO)
                            {
                                MDF_UnApplyAllRender(pst_GO);
                            }
                            */
                        }

                        // Restore the material
                        pMaterial->SetJadeFlags(ulOldBM);

                        g_pXeContextManagerRender->SetCurrentRenderObject(NULL);
                    }

                    g_ulNumCasters++;

                    // Restore material
                    pObj->SetMaterial(pBackupMat);
                }
            }

            g_oXeShadowManager.EndShadowBuffer( !((iMipIdx==1) && (iShadowIdx == (_iNumShadowLights-1))) );
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderZPrepass
// Params : _pObject : Object to render
//          _eFXPass : Current FX pass
// RetVal : None
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderZPrepass(eXeRENDERLISTTYPE _eListType)
{
    CXBBeginEventObject oEvent("Z Pre-pass");
    XeFXManager::RenderPass ePass = XeFXManager::RP_ZPASS_Z_ONLY;
    ULONG               ulNbElements[4];
    IDirect3DTexture9   *pDepthTexture = (IDirect3DTexture9 *) g_oXeTextureMgr.GetTextureFromID( g_oXeTextureMgr.GetDepthBufferID() );
    bool bRenderDepthToColor= false;

    // TODO: When a depth surface can be resolved on Xenon, uncomment these guards
    //#if defined _XENON_RENDER_PC
    bRenderDepthToColor = true;
    //#endif

    ULONG ulTotalElement = 0;

    ulNbElements[0] = g_pXeContextManagerRender->GetNbRenderObjectOfList(_eListType);//m_nbObjects[_eListType];
    ulTotalElement += ulNbElements[0];
    ulNbElements[1] = g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_ZOVERWRITE);
    ulTotalElement += ulNbElements[1];
    ulNbElements[2] = g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_SPG2);
    ulTotalElement += ulNbElements[2];
    ulNbElements[3] = g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_ZOVERWRITESPG2OPAQUE);
    ulTotalElement += ulNbElements[3];

    m_bZPassRenderedSomething = ulTotalElement > 0;

    if( bRenderDepthToColor && (ulTotalElement > 0))
    {
        HRESULT hr = S_OK;

        // Enable all channel
        g_oRenderStateMgr.SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

    #if defined(_XENON)
        if(!m_bFastZPrepass)
        {
            // No fast Z prepass so we need to setup a depth texture to render to
            g_oXeRenderTargetMgr.BeginRenderTarget( 0, 
                pDepthTexture, 
                NULL,
                TRUE,
                FALSE,
                D3DCOLOR_RGBA(255, 255, 255, 255),
                FALSE, 
                1.0f,
                TRUE,
                D3DFMT_R32F,
                D3DFMT_D16,
                g_oXeRenderer.GetMultiSampleType());

            // SDALLAIRE clear to 0.9999f instead of 1.0f to fix alpha kit bug
            D3DVECTOR4 oColor;
            oColor.x = 0.99999f;
            oColor.y = oColor.z = oColor.w = 0.0f;
            hr = m_pD3DDevice->ClearF(D3DCLEAR_TARGET, NULL, &oColor, 1.0f, 0);
        }

    #elif defined(_XENON_RENDER_PC)

        g_oXeRenderTargetMgr.BeginRenderTarget( 0, 
            pDepthTexture, 
            NULL,
            TRUE,
            TRUE,
            D3DCOLOR_RGBA(255, 255, 255, 255),
            FALSE, 
            1.0f,
            FALSE,
            D3DFMT_R32F,
            D3DFMT_D16,
            g_oXeRenderer.GetMultiSampleType());
    #endif

#ifdef ACTIVE_EDITORS
        // Force view port
        hr = m_pD3DDevice->SetViewport( &m_stViewport );
        ERR_X_Assert( SUCCEEDED(hr) );
#endif

        g_oFXManager.SetRenderingDepthToColorInZPass( TRUE );
    }

    BOOL  bFullUpdate;
    FLOAT fLODBias = g_oFXManager.GetGlobalLODBias();
    BOOL  bSetColorWriteMask = bRenderDepthToColor && !g_oXeRenderer.IsFastZEnabled();

    XeRenderObject** pROList = g_pXeContextManagerRender->GetRenderObjectList(_eListType);
    for(int i=0; i<2; i++)
    {
        bFullUpdate = TRUE;

        for (ULONG ulElementIdx = 0; ulElementIdx < ulNbElements[i]; ++ulElementIdx,pROList++)
        {
            XeRenderObject *pObj = *pROList;//m_apRenderObjectLists[_eListType][ulElementIdx];

            // Reset visibility query index for this object
            pObj->SetVisQueryIndex(-1);

            // Only render if visible from camera
            if ((pObj->GetExtraFlags() & QMFR_CULLEDFROMCAMERA) || 
                (pObj->GetLayerIndex() != 0) || 
                (!pObj->GetMaterial()->IsZWriteEnabled()))
                continue;

            // Use the proper blending flags
            XeMaterial* pMat    = pObj->GetMaterial();
            ULONG       ulOldBM = pMat->GetJadeFlags();
            pMat->SetJadeFlags(pObj->GetBlendingMode());

            // Switch to alpha test mode when necessary
            if (pObj->GetMaterial()->IsAlphaTestEnabled())
            {
                ePass = XeFXManager::RP_ZPASS_ALPHA_TEST;
            }
            else
            {
                ePass = XeFXManager::RP_ZPASS_Z_ONLY;
            }

        #ifdef XE_USE_FAST_ZPASS
            if (bFullUpdate)
            {
        #endif
                // This is the first rendered element, do a complete update of the pipeline

                XeRenderObject::XEOBJECTTYPE eObjType = pObj->GetObjectType();

                // Only allow normal geometry
                pObj->SetObjectType(XeRenderObject::Common);

                RenderObject(pObj, ePass);

                pObj->SetObjectType(eObjType);

        #ifdef XE_USE_FAST_ZPASS
            #ifdef VIDEOCONSOLE_ENABLE
                bFullUpdate = !UseOptFastZPass;
            #else
                bFullUpdate = FALSE;
            #endif

            }
            else     
            {
                // Not the first rendered element, do a quick update of the pipeline

                XeMaterial *pCurrentMaterial  = pObj->GetMaterial();
                XeMesh     *pCurrentMesh      = pObj->GetMesh();
                ULONG      ulCurrentDrawMask  = pObj->GetDrawMask();
                XeVertexShaderManager::XeVertexDeclaration *pCurrentDeclaration = g_oVertexShaderMgr.GetVertexDeclaration(pCurrentMesh);
                BOOL       bAlphaTest         = pObj->GetMaterial()->IsAlphaTestEnabled();

                // set current object in renderer
                g_pXeContextManagerRender->SetCurrentRenderObject(pObj);

                // Visibility query
                BeginConditionalRendering(pObj, ePass);

                g_oVertexShaderMgr.SetFeatureCustomShader(0);
                g_oPixelShaderMgr.SetFeatureCustomShader(0);

                // Culling
                if (!pCurrentMaterial->IsTwoSided())
                {
                    if (ulCurrentDrawMask & GDI_Cul_DM_NotInvertBF)
                    {
                        g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
                    }
                    else
                    {
                        g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                    }
                }
                else
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
                }

                // Color Write Mask
                ULONG ul_ActiveChannels  = 0;

                if( bSetColorWriteMask )
                {
                    if (pCurrentMaterial->IsColorWriteEnabled())
                    {
                        ul_ActiveChannels |= D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
                    }

                    if (pCurrentMaterial->IsAlphaWriteEnabled() )
                    {
                        ul_ActiveChannels |= D3DCOLORWRITEENABLE_ALPHA;
                    }

                    g_oRenderStateMgr.SetRenderState(D3DRS_COLORWRITEENABLE, ul_ActiveChannels);
                }

                // send vertex declaration
                g_oRenderStateMgr.SetVertexDeclaration(pCurrentDeclaration->pDeclaration);

                // Set the transform feature
                // SC: Do not attempt skinning if the skinning set is empty, happens a lot with meshes 
                //     having weights only for SPG2 generation
                if ((pCurrentDeclaration->eAttributes.ulTransformType == TRANSFORM_SKINNING) &&
                    ((pObj->GetSkinningSet()->ulSkinningMatrixCount == 0) ||
                    (pCurrentMesh->GetMaxWeights() == 0)))
                {
                    g_oVertexShaderMgr.SetFeatureTransform(TRANSFORM_NORMAL);
                }
                else
                {
                    g_oVertexShaderMgr.SetFeatureTransform(pCurrentDeclaration->eAttributes.ulTransformType);
                }

                // Set the world matrix index
                g_pXeContextManagerRender->SetWorldViewMatrix(pObj->GetWorldViewMatrixIndex());

                // Set the projection matrix index
                g_pXeContextManagerRender->SetProjectionMatrix(pObj->GetProjMatrixIndex());

                // Skinning
                g_pXeContextManagerRender->SetCurrentSkinningSet(pObj->GetSkinningSet());
                g_oVertexShaderMgr.SetMaxWeight(pCurrentMesh->GetMaxWeights());
                g_oVertexShaderMgr.SetFeatureBonesConfig(pCurrentMesh->GetMaxBones());
                g_oVertexShaderMgr.SetFeatureLightsConfig(pCurrentMesh->GetMaxBones());

                //  Stencil
                BOOL b_StencilEnabled = pCurrentMaterial->IsStencilEnabled() || g_oXeRenderer.IsRenderingGlowMask();
                if( b_StencilEnabled && g_oXeRenderer.IsRenderingGlowMask() )
                {
                    BYTE stencilRef = pCurrentMaterial->IsGlowEnabled() ? 255 : 0;

                    if( pCurrentMaterial->IsGlowEnabled( ) )
                    {
                        g_oXeRenderer.IncGlowPrimitiveCount( );
                    }

                    g_oRenderStateMgr.SetRenderState( D3DRS_STENCILENABLE,  TRUE );
                    g_oRenderStateMgr.SetRenderState( D3DRS_STENCILFUNC,    D3DCMP_ALWAYS );
                    g_oRenderStateMgr.SetRenderState( D3DRS_STENCILZFAIL,   D3DSTENCILOP_KEEP );
                    g_oRenderStateMgr.SetRenderState( D3DRS_STENCILPASS,    D3DSTENCILOP_REPLACE );
                    g_oRenderStateMgr.SetRenderState( D3DRS_STENCILREF,     stencilRef );
                }
                else
                {
                    g_oRenderStateMgr.SetRenderState( D3DRS_STENCILENABLE, FALSE );
                }

                LONG l_BaseMapTextureId   = pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);
                LONG l_NormalMapTextureId = pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL);

                // Alpha testing
                BOOL bAlphaTestRenderState;
                if( bAlphaTest )
                {
                    // When rendering depth to color, alpha test is done in shader
                    bAlphaTestRenderState = !bRenderDepthToColor || g_oXeRenderer.IsFastZEnabled();

                    ULONG ul_AddressU, ul_AddressV, ul_AddressW;
                    ULONG ul_MagFilter, ul_MinFilter, ul_MipFilter;
                    FLOAT f_MipmapLODBias;

                    // Textures
                    g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_BASE, l_BaseMapTextureId);

                    if (l_BaseMapTextureId != MAT_Xe_InvalidTextureId)
                    {
                        pCurrentMaterial->GetAddressMode(XeMaterial::TEXTURE_BASE, &ul_AddressU, &ul_AddressV, &ul_AddressW);
                        pCurrentMaterial->GetFilterMode(XeMaterial::TEXTURE_BASE, &ul_MagFilter, &ul_MinFilter, &ul_MipFilter);

                        f_MipmapLODBias  = pCurrentMaterial->GetMipmapLODBias(XeMaterial::TEXTURE_BASE);
                        f_MipmapLODBias += fLODBias;

                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_ADDRESSU, ul_AddressU);
                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_ADDRESSV, ul_AddressV);
                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_ADDRESSW, ul_AddressW);
                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_BORDERCOLOR, pCurrentMaterial->GetBorderColor(XeMaterial::TEXTURE_BASE));
                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MAGFILTER, ul_MagFilter);
                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MINFILTER, ul_MinFilter);
                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MIPFILTER, ul_MipFilter);
                        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&f_MipmapLODBias);
                    }
                }
                else
                {
                    bAlphaTestRenderState = FALSE;
                    g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_BASE, -1);
                }

                if (bAlphaTestRenderState)
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAREF, pCurrentMaterial->GetAlphaRef());

                    if (pCurrentMaterial->IsAlphaInverted())
                    {
                        g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
                    }
                    else
                    {
                        g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
                    }
                }
                else
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
                }

                // Update Vertex Shader

                if(pCurrentMaterial->GetCustomVS())
                {
                    // Set the custom vertex shader and features
                    g_oVertexShaderMgr.SetFeatureCustomShader(pCurrentMaterial->GetCustomVS());
                    g_oVertexShaderMgr.SetFeaturesCustom(pCurrentMaterial->GetCustomVSFeatures());

                    // force a full update since all the features have been overwritten
                    bFullUpdate = TRUE;
                }
                else
                {
                    if (bAlphaTest)
                    {
                        // Color source
                        ULONG ulColorSource = pCurrentMaterial->GetColorSource();
                    #if defined(ACTIVE_EDITORS)
                        if ((ulCurrentDrawMask & GDI_Cul_DM_DontForceColor) == 0)
                        {
                            ulColorSource = MAT_Cc_ColorOp_FullLight;
                        }
                    #endif
                        ulColorSource = ConvertColorSource(ulColorSource);
                        g_oVertexShaderMgr.SetFeatureColorSource(ulColorSource);

                        // UV Source
                        ULONG ulUVSource = pCurrentMaterial->GetUVSource();
                        g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ulUVSource));

                        // make sure we don't use RLI if not present in vertex format
                        g_oVertexShaderMgr.SetFeatureAddRLI(pCurrentDeclaration->eAttributes.bHasColor && (ulCurrentDrawMask & GDI_Cul_DM_UseRLI));

                        // Texture coordinate transform
                        ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
                        pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
                        g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);

                        // SC: We need to use those features to prevent z fighting with the normal pass...
                        //  -> Vertex or pixel based lighting will make sure the same compilation options are used 
                        //     when generating the shader
                        if (ulCurrentDrawMask & GDI_Cul_DM_Lighted)
                        {
                            if (l_NormalMapTextureId != MAT_Xe_InvalidTextureId)
                                g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_PIXEL);
                            else
                                g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_VERTEX);
                        }
                        else
                        {
                            g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);
                        }
                    }
                    else
                    {
                        // SC: Using a constant color source to make sure the lighting will not be computed
                        g_oVertexShaderMgr.SetFeatureColorSource(COLORSOURCE_CONSTANT);
                        g_oVertexShaderMgr.SetFeatureUVSource(0);
                        g_oVertexShaderMgr.SetFeatureAddRLI(0);
                        g_oVertexShaderMgr.SetFeatureBaseMapTransform(0);
                        g_oVertexShaderMgr.SetFeatureLighting(0);
                    }

                    g_oVertexShaderMgr.SetFeatureAddAmbient(FALSE);
                }

                // Update Pixel Shader
                if(pCurrentMaterial->GetCustomPS())
                {
                    // Set the custom pixel shader and features
                    g_oPixelShaderMgr.SetFeatureCustomShader(pCurrentMaterial->GetCustomPS());

                    for (ULONG i = 1; i <= XE_PS_CUSTOM_FEATURE_COUNT; ++i)
                    {
                        g_oPixelShaderMgr.SetFeatureCustom(i, pCurrentMaterial->GetCustomPSFeature(i));
                    }

                    // force a full update since all the features have been overwritten
                    bFullUpdate = TRUE;
                }
                else
                {
                    // Local alpha
                    g_oPixelShaderMgr.SetFeatureLocalAlpha(pCurrentMaterial->IsUsingLocalAlpha() ? TRUE : FALSE);

                    if(!g_oXeRenderer.IsFastZEnabled() || bAlphaTest)
                    {
                        if (bRenderDepthToColor && !g_oXeRenderer.IsFastZEnabled())
                        {
                            g_oPixelShaderMgr.SetFeatureDepthToColorEnable( TRUE );
                            g_oPixelShaderMgr.SetFeatureAlphaTestEnable(bAlphaTest);
                        }
                        else
                        {
                            g_oPixelShaderMgr.SetFeatureDepthToColorEnable( FALSE );
                            g_oPixelShaderMgr.SetFeatureAlphaTestEnable( FALSE );
                        }

                        if( bAlphaTest )
                        {
                            g_oPixelShaderMgr.SetFeatureRemapAlphaRange(pCurrentMaterial->IsAlphaRangeRemapped() ? 1 : 0);

                            // Samplers
                            // --------
                            if (((ulCurrentDrawMask & GDI_Cul_DM_UseTexture) != 0) && (l_BaseMapTextureId != MAT_Xe_InvalidTextureId))
                            {
                                g_oPixelShaderMgr.SetFeatureUseBaseMap(TRUE);
                            }
                            else
                            {
                                g_oPixelShaderMgr.SetFeatureUseBaseMap(FALSE);
                            }

                            if ((ulCurrentDrawMask & GDI_Cul_DM_Lighted) && (l_NormalMapTextureId != MAT_Xe_InvalidTextureId))
                            {
                                g_oPixelShaderMgr.SetFeatureDiffusePPL(l_NormalMapTextureId != MAT_Xe_InvalidTextureId ? TRUE : FALSE);
                            }
                            else
                            {
                                g_oPixelShaderMgr.SetFeatureDiffusePPL(FALSE);
                            }
                        }
                        else
                        {
                            g_oPixelShaderMgr.SetFeatureRemapAlphaRange(FALSE);
                            g_oPixelShaderMgr.SetFeatureUseBaseMap(FALSE);
                            g_oPixelShaderMgr.SetFeatureDiffusePPL(FALSE);
                        }
                    }
                    else
                    {
                        g_oPixelShaderMgr.SetFeatureDepthToColorEnable( FALSE );
                        g_oPixelShaderMgr.SetFeatureAlphaTestEnable( FALSE );
                        g_oPixelShaderMgr.SetFeatureRemapAlphaRange( FALSE );
                        g_oPixelShaderMgr.SetFeatureUseBaseMap( FALSE );
                        g_oPixelShaderMgr.SetFeatureDiffusePPL( FALSE );
                    }
                }

    #if defined(ACTIVE_EDITORS)
                if ((pObj->GetExtraFlags() & QMFR_ZBIAS) != 0)
                {
                    FLOAT fSlopeBias = 2.0f;

                    g_oRenderStateMgr.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeBias);
                }
                else
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
                }

                if (pObj->GetDrawMask() & GDI_Cul_DM_NotWired)
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
                }
                else
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
                }
    #endif

                // Update pixel shader
                g_oPixelShaderMgr.Update(pObj);

                // Send object to VS Manager
                g_oVertexShaderMgr.Update(pObj);

                // Update RenderStateMgr
                g_oRenderStateMgr.Update(pObj);

                // primitive number context
                if (pObj->GetObjectType() == XeRenderObject::Fur)
                {
                    g_oFurManager.UpdateFurPrimitive(pObj, 0);
                }

                HRESULT hr;
                XeIndexBuffer*   poIndices        = pCurrentMesh->GetIndices();
                UINT             uiNumFaces       = pObj->GetPrimitiveCount();
                ULONG            ulVertexCount    = 0;
                D3DPRIMITIVETYPE ePrimitiveType   = pObj->GetPrimType();

    #if defined(_XENON_RENDER_PC)
                // Fake quad list support with indexed triangles and a special index buffer
                if (ePrimitiveType == D3DPT_QUADLIST)
                {
                    XeValidate(poIndices == NULL, "A QuadList mesh must not use indices on PC!");
                    poIndices      = g_XeBufferMgr.GetSpriteIB();
                    ePrimitiveType = D3DPT_TRIANGLELIST;
                }
    #endif
                /*
                OBJ_tdst_GameObject *pst_GO = NULL;

                if(pObj->GetExtraFlags() & QMFR_GAO)
                {
                    pst_GO = (OBJ_tdst_GameObject *)pObj->GetUserData();
                }

                if (pst_GO)
                {
                    MDF_ApplyAllRender(pst_GO);
                }
                */
                if (uiNumFaces)
                {
                    if (poIndices)
                    {
    #if defined(_XENON_RENDER_PC)
                        ulVertexCount = pCurrentMesh->GetStream(0)->pBuffer->GetVertexCount();
    #endif
                        hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                            0,                          // base index
                            0,                          // MinIndex - unused
                            ulVertexCount,              // NumVertices - unused
                            0,                          // start index
                            uiNumFaces);
                    }
                    else
                    {
                        hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                            0,                         // start vertex
                            uiNumFaces);
                    }

                    ERR_X_Assert(SUCCEEDED(hr));

                    m_uiNumTriangles += uiNumFaces;
                    m_uiNumDrawCalls++;
                }
                /*
                if(pst_GO)
                {
                    MDF_UnApplyAllRender(pst_GO);
                }
                */
                // Visibility query
                EndConditionalRendering(pObj, ePass);
            }
        #endif

            // Restore the flags
            pMat->SetJadeFlags(ulOldBM);
        }

        if(i==0 && ulNbElements[1])
        {
            pROList = g_pXeContextManagerRender->GetRenderObjectList(XeRT_ZOVERWRITE);
            g_oFXManager.SetRenderingDoublePassZOverwrite(TRUE);
        }
    }

    g_oFXManager.SetRenderingDoublePassZOverwrite(FALSE);
    g_pXeContextManagerRender->SetCurrentRenderObject(NULL);

    // SPG2
#ifdef XE_USE_FAST_ZPASS
    #ifdef VIDEOCONSOLE_ENABLE
    if (UseOptFastZPass)
        RenderZPrepassSPG2(XeRT_SPG2);
    else
        RenderSPG2List(eSPG2PassShadow);
    #else
    RenderZPrepassSPG2(XeRT_SPG2);
    #endif
#else
    RenderSPG2List(eSPG2PassShadow);
#endif

#ifdef XE_USE_FAST_ZPASS
    #ifdef VIDEOCONSOLE_ENABLE
    if (UseOptFastZPass)
    #endif
    {
        // spg2 draw at end objects
        if(ulNbElements[3])
        {
            g_oFXManager.SetRenderingDoublePassZOverwrite(TRUE);
            RenderZPrepassSPG2(XeRT_ZOVERWRITESPG2OPAQUE);
            g_oFXManager.SetRenderingDoublePassZOverwrite(FALSE);
        }
    }
#endif

    if( bRenderDepthToColor && ulTotalElement )  
    {
        g_oFXManager.SetRenderingDepthToColorInZPass( FALSE );

#if defined(_XENON)
        if(m_bFastZPrepass)
        {
            // Resolve Z buffer to a texture so we can use it for shadows and fog
            m_pD3DDevice->Resolve(D3DRESOLVE_DEPTHSTENCIL | D3DRESOLVE_FRAGMENT0,
                NULL,
                pDepthTexture,
                NULL,
                0,
                0,
                0,
                1.0f,
                0,
                NULL);
        }
        else
        {
            // clear render target since it is shared with the R32F depth buffer
            g_oXeRenderTargetMgr.EndRenderTarget( 0, FALSE, TRUE, 0, FALSE );
        }
#else
        g_oXeRenderTargetMgr.EndRenderTarget( 0, FALSE, FALSE, 0, FALSE );
#endif

#ifdef ACTIVE_EDITORS
        // Force the viewport
        HRESULT hr = m_pD3DDevice->SetViewport( &m_stViewport );
        ERR_X_Assert( SUCCEEDED(hr) );
#endif
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderSPG2List
// Params : _pObject : Object to render
//          _eFXPass : Current FX pass
// RetVal : None
// Descr. : Render a single object
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderSPG2List(SPG2PASS _ePass)
{
    CXBBeginEventObject oEvent("RenderSPG2List");

    eXeRENDERLISTTYPE eList = ((_ePass==eSPG2PassZOverwriteOpaque) ? XeRT_ZOVERWRITESPG2OPAQUE : ((_ePass==eSPG2PassZOverwriteTransparent) ? XeRT_ZOVERWRITESPG2TRANSPARENT : XeRT_SPG2));

    ULONG ulNbElements = g_pXeContextManagerRender->GetNbRenderObjectOfList(eList);

    HRESULT hr;
    XeMesh*			 poMesh;
    UINT             uiNumFaces;
    ULONG            ulVertexCount=0;
    D3DPRIMITIVETYPE ePrimitiveType;
    XeIndexBuffer*   poIndices;

    XeRenderObject** pROList = g_pXeContextManagerRender->GetRenderObjectList(eList);
    XeRenderObject* _pObject;// = m_apRenderObjectLists[XeRT_SPG2][0];

    BOOL bNewShader = TRUE;

    ULONG128 VSFeatures = 0;

    BOOL bKeepSPG2Alpha = (_ePass==eSPG2PassTransparent);
//    BOOL bKeepSPG2Alpha = (_ePass!=eSPG2PassShadow);

    if( ulNbElements && bKeepSPG2Alpha )
    {
        ClearAlphaOnly( 0 );
    }

    // force base index to 0 since we are not doinf multipass
    g_oVertexShaderMgr.SetBaseLightIndex(0);

    ULONG BM;
    for(ULONG ulElementIdx = 0; ulElementIdx < ulNbElements; ++ulElementIdx,pROList++)
    {
        _pObject = *pROList;//m_apRenderObjectLists[XeRT_SPG2][ulElementIdx];
        poMesh			 =	_pObject->GetMesh();

        if(!_pObject->GetMaterial()->IsJadeMaterial())
            continue;

        ULONG ulOldBM;
        BM = _pObject->GetBlendingMode();
        if(BM!=-1)
        {
            if(_ePass!=eSPG2PassZOverwriteOpaque && _ePass!=eSPG2PassZOverwriteTransparent)
            {
                if((BM & MAT_Cul_Flag_AlphaTest) && (_ePass==eSPG2PassTransparent))
                    continue;
                if(!(BM & MAT_Cul_Flag_AlphaTest) && (_ePass!=eSPG2PassTransparent))
                    continue;
            }

            ulOldBM = _pObject->GetMaterial()->GetJadeFlags();
            if(_ePass!=eSPG2PassShadow || (_pObject->GetBlendingMode() & MAT_Cul_Flag_AlphaTest))
            _pObject->GetMaterial()->SetJadeFlags(_pObject->GetBlendingMode());
        }

        if(_ePass==eSPG2PassShadow && !(_pObject->GetMaterial()->GetJadeFlags() & MAT_Cul_Flag_AlphaTest))
            continue;

        int iShadowCount = 0;
        g_pXeContextManagerRender->SetCurrentRenderObject(_pObject);

        if(_ePass!=eSPG2PassShadow)
        {
            ULONG ulLightSetIndex = _pObject->GetLightSetIndex();
            ULONG ulLightCount = g_pXeContextManagerRender->GetLightSetLightCount(ulLightSetIndex);
            ulLightCount = min(ulLightCount, GetMaxTotalLightCount());
            ulLightCount = min(ulLightCount, GetMaxLightsPerPass( FALSE ));
            g_oPixelShaderMgr.SetPPLightCount(ulLightCount);

            // Init index remap table

            ULONG uiLightIdx;
            INT aiShadowChannels[VS_MAX_LIGHTS];

            if(ulLightCount>0)
            {
                int iChannel;
                for(uiLightIdx = 0; uiLightIdx < ulLightCount; uiLightIdx++)
                {
                    ULONG ulType;
                    XeLight* poLight = g_pXeContextManagerRender->GetLight(ulLightSetIndex, uiLightIdx, &ulType);

                    if(g_oXeShadowManager.IsLightUsedForShadows(poLight->JadeLight, iChannel))
                    {
                        ERR_X_Assert( iShadowCount < 4 );
                        iShadowCount++;
                    }
                    aiShadowChannels[uiLightIdx] = iChannel;
                }
            }

            g_oFXManager.SetShadowCount(iShadowCount);

            for (ULONG ulCurLight = 0; ulCurLight < VS_MAX_LIGHTS_PER_PASS; ulCurLight++)
            {
                if (ulCurLight < ulLightCount)
                {
                    g_pXeContextManagerRender->SetPerPixelLightIndex(ulCurLight, ulCurLight);
                    g_oPixelShaderMgr.SetShadowChannels( ulCurLight, aiShadowChannels[ulCurLight] );
                }
                else
                {
                    g_pXeContextManagerRender->SetPerPixelLightIndex(ulCurLight, -1);
                    g_oPixelShaderMgr.SetShadowChannels( ulCurLight, -1 );
                }
            }
        }

        int iMode = _pObject->GetCustomVSFeature(1);
        BOOL bSimpleCombineXYMode = FALSE;
        BOOL bSimpleCombineXYHATMode = FALSE;
        if(iMode==6 || iMode==7) // simple mode for x and y combo
        {
            bSimpleCombineXYMode = TRUE;
            iMode = 1; // first do the y

            if(iMode==7)
                bSimpleCombineXYHATMode = TRUE;
        }

        XeFXManager::RenderPass ePass = XeFXManager::RP_SPG2;
        if(_ePass==eSPG2PassShadow)
        {
            ePass = XeFXManager::RP_ZPASS_ALPHA_TEST;
        }
        else if(_ePass==eSPG2PassZOverwriteTransparent)
        {
            ePass = XeFXManager::RP_SPG2ZOVERWRITE;
        }

        g_oFXManager.BeginUpdate(_pObject, ePass);

            g_oFXManager.UpdateVSFeatures();
            g_oVertexShaderMgr.SetFeatureSPG2Mode(iMode);

            g_oFXManager.UpdatePSFeatures();
            g_pXeContextManagerRender->SetSPG2ShadersConsts(_pObject->GetSPG2ShadersConsts());

        g_oFXManager.EndUpdate();


        if(_ePass == eSPG2PassShadow)
        {
            g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);
            g_oVertexShaderMgr.SetFeatureColorSource(COLORSOURCE_VERTEX_COLOR);
            g_oPixelShaderMgr.SetFeatureDiffusePPL(FALSE);
            g_oPixelShaderMgr.SetFeatureSpecularPPL(FALSE);
        }
        g_oPixelShaderMgr.SetFeatureRemapAlphaRange(FALSE);
        
        g_oPixelShaderMgr.Update(_pObject);
        g_oVertexShaderMgr.Update(_pObject);
        g_oRenderStateMgr.Update(_pObject);

        g_oVertexShaderMgr.UpdateShaderConstantsSPG2(g_oRenderStateMgr.GetCurrentVertexShader(), _pObject, bNewShader);

        uiNumFaces       = _pObject->GetPrimitiveCount();
        ePrimitiveType   = _pObject->GetPrimType();
        poIndices        = poMesh->GetIndices();

#if defined(_XENON_RENDER_PC)
        // Fake quad list support with indexed triangles and a special index buffer
        if (ePrimitiveType == D3DPT_QUADLIST)
        {
            XeValidate(poIndices == NULL, "A QuadList mesh must not use indices on PC!");
            poIndices      = g_XeBufferMgr.GetSpriteIB();
            ePrimitiveType = D3DPT_TRIANGLELIST;
        }
#endif

        if(uiNumFaces == 0)
            continue;

        if (poIndices != NULL)
        {
#if defined(_XENON_RENDER_PC)
            ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

            hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                0,                          // base index
                0,                          // MinIndex - unused
                ulVertexCount,              // NumVertices - unused
                0,                          // start index
                uiNumFaces);
        }
        else
        {

            hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                0,                         // start vertex
                uiNumFaces);
        }
        ERR_X_Assert( SUCCEEDED(hr) );

        m_uiNumTriangles += uiNumFaces;
        m_uiNumDrawCalls++;

        if(bSimpleCombineXYMode)
        {
            iMode = 2; // now do the x
            g_oVertexShaderMgr.SetFeatureSPG2Mode(iMode);
            g_oVertexShaderMgr.Update(_pObject);
            g_oRenderStateMgr.Update(_pObject);

            if (poIndices != NULL)
            {
#if defined(_XENON_RENDER_PC)
                ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

                hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                    0,                          // base index
                    0,                          // MinIndex - unused
                    ulVertexCount,              // NumVertices - unused
                    0,                          // start index
                    uiNumFaces);
            }
            else
            {

                hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                    0,                         // start vertex
                    uiNumFaces);
            }
            ERR_X_Assert( SUCCEEDED(hr) );

            m_uiNumTriangles += uiNumFaces;
            m_uiNumDrawCalls++;
        }

        if(bSimpleCombineXYHATMode)
        {
            iMode = 3; // now do the hat
            g_oVertexShaderMgr.SetFeatureSPG2Mode(iMode);
            g_oVertexShaderMgr.Update(_pObject);
            g_oRenderStateMgr.Update(_pObject);

            if (poIndices != NULL)
            {
#if defined(_XENON_RENDER_PC)
                ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

                hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                    0,                          // base index
                    0,                          // MinIndex - unused
                    ulVertexCount,              // NumVertices - unused
                    0,                          // start index
                    uiNumFaces);
            }
            else
            {

                hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                    0,                         // start vertex
                    uiNumFaces);
            }
            ERR_X_Assert( SUCCEEDED(hr) );

            m_uiNumTriangles += uiNumFaces;
            m_uiNumDrawCalls++;
        }

        if(_pObject->GetExtraFlags() & QMFR_SPG2GRID)
            PostDrawPrimitiveCallback(DPCB_SPG2GRID, _pObject->GetUserData());

        if(BM != -1)
        {
            _pObject->GetMaterial()->SetJadeFlags(ulOldBM);
        }
    }

    if( bKeepSPG2Alpha && m_bHasSPG2WriteAlpha)
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESPG2ALPHASCRATCH_ID );
    }

    g_pXeContextManagerRender->SetCurrentRenderObject(NULL);
}


// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderZPrepassSPG2
// Params : _pObject : Object to render
//          _eFXPass : Current FX pass
// RetVal : None
// Descr. : Render a single object
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderZPrepassSPG2(eXeRENDERLISTTYPE _eListType)
{
    CXBBeginEventObject oEvent("RenderZPrepassSPG2");

    ULONG ulNbElements = g_pXeContextManagerRender->GetNbRenderObjectOfList(_eListType);

    HRESULT hr;
    UINT             uiNumFaces;
    ULONG            ulVertexCount=0;
    D3DPRIMITIVETYPE ePrimitiveType;
    XeIndexBuffer*   poIndices;
    XeRenderObject*  _pObject;

    XeRenderObject** pROList = g_pXeContextManagerRender->GetRenderObjectList(_eListType);

    BOOL bNewShader = TRUE;

    ULONG128 VSFeatures = 0;
    bool bRenderDepthToColor= false;

    // TODO: When a depth surface can be resolved on Xenon, uncomment these guards
    //#if defined _XENON_RENDER_PC
    bRenderDepthToColor = true;
    //#endif

    // force base index to 0 since we are not doinf multipass
    g_oVertexShaderMgr.SetBaseLightIndex(0);

    ULONG BM;
    BOOL bFullUpdate = TRUE;
    FLOAT fLODBias = g_oFXManager.GetGlobalLODBias();
    BOOL  bSetColorWriteMask = bRenderDepthToColor && !g_oXeRenderer.IsFastZEnabled();

    for(ULONG ulElementIdx = 0; ulElementIdx < ulNbElements; ++ulElementIdx,pROList++)
    {
        _pObject = *pROList;//m_apRenderObjectLists[XeRT_SPG2][ulElementIdx];

        XeMaterial *pCurrentMaterial  = _pObject->GetMaterial();
        XeMesh     *pCurrentMesh      = _pObject->GetMesh();
        ULONG      ulCurrentDrawMask  = _pObject->GetDrawMask();
        XeVertexShaderManager::XeVertexDeclaration *pCurrentDeclaration = g_oVertexShaderMgr.GetVertexDeclaration(pCurrentMesh);

        if(!pCurrentMaterial->IsJadeMaterial())
            continue;

        ULONG ulOldBM;
        BM = _pObject->GetBlendingMode();
        if(BM!=-1)
        {
            if (!(BM & MAT_Cul_Flag_AlphaTest))
                continue;

            ulOldBM = pCurrentMaterial->GetJadeFlags();
            if (_pObject->GetBlendingMode() & MAT_Cul_Flag_AlphaTest)
                pCurrentMaterial->SetJadeFlags(_pObject->GetBlendingMode());
        }

        if(!(pCurrentMaterial->GetJadeFlags() & MAT_Cul_Flag_AlphaTest))
            continue;

        int iShadowCount = 0;
        g_pXeContextManagerRender->SetCurrentRenderObject(_pObject);

        int iMode = _pObject->GetCustomVSFeature(1);
        BOOL bSimpleCombineXYMode = FALSE;
        BOOL bSimpleCombineXYHATMode = FALSE;
        if(iMode==6 || iMode==7) // simple mode for x and y combo
        {
            bSimpleCombineXYMode = TRUE;
            iMode = 1; // first do the y

            if(iMode==7)
                bSimpleCombineXYHATMode = TRUE;
        }
      
      #ifdef XE_USE_FAST_ZPASS
        if (bFullUpdate)
        {
      #endif
            g_oFXManager.BeginUpdate(_pObject, XeFXManager::RP_ZPASS_ALPHA_TEST);

            g_oFXManager.UpdateVSFeatures();
            g_oVertexShaderMgr.SetFeatureSPG2Mode(iMode);

            g_oFXManager.UpdatePSFeatures();
            g_pXeContextManagerRender->SetSPG2ShadersConsts(_pObject->GetSPG2ShadersConsts());

            g_oFXManager.EndUpdate();


            g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);
            g_oVertexShaderMgr.SetFeatureColorSource(COLORSOURCE_VERTEX_COLOR);
            g_oPixelShaderMgr.SetFeatureDiffusePPL(FALSE);
            g_oPixelShaderMgr.SetFeatureSpecularPPL(FALSE);
            g_oPixelShaderMgr.SetFeatureRemapAlphaRange(FALSE);

      #ifdef XE_USE_FAST_ZPASS
          #ifdef VIDEOCONSOLE_ENABLE
            bFullUpdate = !UseOptFastZPass;
          #else
            bFullUpdate = FALSE;
          #endif

        }
        else
        {
            // Not the first rendered element, do a quick update of the pipeline

            g_oVertexShaderMgr.SetFeatureCustomShader(0);
            g_oPixelShaderMgr.SetFeatureCustomShader(0);

            // Channels
            if( bSetColorWriteMask )
            {
                ULONG ul_ActiveChannels = 0;

                if (pCurrentMaterial->IsColorWriteEnabled())
                {
                    ul_ActiveChannels |= D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
                }

                if (pCurrentMaterial->IsAlphaWriteEnabled() )
                {
                    ul_ActiveChannels |= D3DCOLORWRITEENABLE_ALPHA;
                }

                g_oRenderStateMgr.SetRenderState(D3DRS_COLORWRITEENABLE, ul_ActiveChannels);
            }

            // send vertex declaration
            g_oRenderStateMgr.SetVertexDeclaration(pCurrentDeclaration->pDeclaration);

            g_oVertexShaderMgr.SetFeatureTransform(pCurrentDeclaration->eAttributes.ulTransformType);

            // Set the world matrix index
            g_pXeContextManagerRender->SetWorldViewMatrix(_pObject->GetWorldViewMatrixIndex());

            // Set the projection matrix index
            g_pXeContextManagerRender->SetProjectionMatrix(_pObject->GetProjMatrixIndex());

            // Skinning
            g_pXeContextManagerRender->SetCurrentSkinningSet(_pObject->GetSkinningSet());

            g_oVertexShaderMgr.SetMaxWeight(pCurrentMesh->GetMaxWeights());
            g_oVertexShaderMgr.SetFeatureBonesConfig(pCurrentMesh->GetMaxBones());
            g_oVertexShaderMgr.SetFeatureLightsConfig(pCurrentMesh->GetMaxBones());

            // When rendering depth to color, alpha test is done in shader
            if (!bRenderDepthToColor || g_oXeRenderer.IsFastZEnabled())
            {
                g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAREF, pCurrentMaterial->GetAlphaRef());

                if (pCurrentMaterial->IsAlphaInverted())
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
                }
                else
                {
                    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
                }
            }
            else
            {
                g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
            }

            //  Stencil
            BOOL b_StencilEnabled = pCurrentMaterial->IsStencilEnabled() || g_oXeRenderer.IsRenderingGlowMask();

            if( b_StencilEnabled && g_oXeRenderer.IsRenderingGlowMask() )
            {
                BYTE stencilRef = pCurrentMaterial->IsGlowEnabled() ? 255 : 0;

                if( pCurrentMaterial->IsGlowEnabled( ) )
                {
                    g_oXeRenderer.IncGlowPrimitiveCount( );
                }

                g_oRenderStateMgr.SetRenderState( D3DRS_STENCILENABLE,  TRUE );
                g_oRenderStateMgr.SetRenderState( D3DRS_STENCILFUNC,    D3DCMP_ALWAYS );
                g_oRenderStateMgr.SetRenderState( D3DRS_STENCILZFAIL,   D3DSTENCILOP_KEEP );
                g_oRenderStateMgr.SetRenderState( D3DRS_STENCILPASS,    D3DSTENCILOP_REPLACE );
                g_oRenderStateMgr.SetRenderState( D3DRS_STENCILREF,     stencilRef );
            }
            else
            {
                g_oRenderStateMgr.SetRenderState( D3DRS_STENCILENABLE, FALSE );
            }

            // Textures

            ULONG ul_AddressU, ul_AddressV, ul_AddressW;
            ULONG ul_MagFilter, ul_MinFilter, ul_MipFilter;
            FLOAT f_MipmapLODBias;

            // Textures
            LONG l_BaseTextureId = pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);

          #ifdef VIDEOCONSOLE_ENABLE
            if(NoTexture)
                g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_BASE, -1);
            else
          #endif
                g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_BASE, l_BaseTextureId);

            if (l_BaseTextureId != MAT_Xe_InvalidTextureId)
            {
                pCurrentMaterial->GetAddressMode(XeMaterial::TEXTURE_BASE, &ul_AddressU, &ul_AddressV, &ul_AddressW);
                pCurrentMaterial->GetFilterMode(XeMaterial::TEXTURE_BASE, &ul_MagFilter, &ul_MinFilter, &ul_MipFilter);

                f_MipmapLODBias  = pCurrentMaterial->GetMipmapLODBias(XeMaterial::TEXTURE_BASE);
                f_MipmapLODBias += fLODBias;

                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_ADDRESSU, ul_AddressU);
                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_ADDRESSV, ul_AddressV);
                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_ADDRESSW, ul_AddressW);
                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_BORDERCOLOR, pCurrentMaterial->GetBorderColor(XeMaterial::TEXTURE_BASE));
                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MAGFILTER, ul_MagFilter);
                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MINFILTER, ul_MinFilter);
                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MIPFILTER, ul_MipFilter);
                g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_BASE, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&f_MipmapLODBias);
            }

            // Vertex Shader
            
            // UV Source
            ULONG ulUVSource = pCurrentMaterial->GetUVSource();
            g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ulUVSource));

            // make sure we don't use RLI if not present in vertex format
            g_oVertexShaderMgr.SetFeatureAddRLI(pCurrentDeclaration->eAttributes.bHasColor && (ulCurrentDrawMask & GDI_Cul_DM_UseRLI));

            // Texture coordinate transform
            ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
            pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
            g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);

            // ambient feature
            g_oVertexShaderMgr.SetFeatureAddAmbient(FALSE);

            g_oVertexShaderMgr.SetFeatureSPG2Mode(iMode);

            // Pixel Shader

            // Local alpha
            g_oPixelShaderMgr.SetFeatureLocalAlpha(pCurrentMaterial->IsUsingLocalAlpha() ? TRUE : FALSE);

            // Samplers
            // --------
            if (((ulCurrentDrawMask & GDI_Cul_DM_UseTexture) != 0) && (l_BaseTextureId != MAT_Xe_InvalidTextureId))
            {
                g_oPixelShaderMgr.SetFeatureUseBaseMap(TRUE);
            }
            else
            {
                g_oPixelShaderMgr.SetFeatureUseBaseMap(FALSE);
            }

            g_pXeContextManagerRender->SetSPG2ShadersConsts(_pObject->GetSPG2ShadersConsts());
        }
      #endif

        g_oPixelShaderMgr.Update(_pObject);
        g_oVertexShaderMgr.Update(_pObject);
        g_oRenderStateMgr.Update(_pObject);

        g_oVertexShaderMgr.UpdateShaderConstantsSPG2(g_oRenderStateMgr.GetCurrentVertexShader(), _pObject, bNewShader);

        uiNumFaces       = _pObject->GetPrimitiveCount();
        ePrimitiveType   = _pObject->GetPrimType();
        poIndices        = pCurrentMesh->GetIndices();

#if defined(_XENON_RENDER_PC)
        // Fake quad list support with indexed triangles and a special index buffer
        if (ePrimitiveType == D3DPT_QUADLIST)
        {
            XeValidate(poIndices == NULL, "A QuadList mesh must not use indices on PC!");
            poIndices      = g_XeBufferMgr.GetSpriteIB();
            ePrimitiveType = D3DPT_TRIANGLELIST;
        }
#endif

        if(uiNumFaces == 0) continue;

        if (poIndices != NULL)
        {
#if defined(_XENON_RENDER_PC)
            ulVertexCount = pCurrentMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

            hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                0,                          // base index
                0,                          // MinIndex - unused
                ulVertexCount,              // NumVertices - unused
                0,                          // start index
                uiNumFaces);
        }
        else
        {

            hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                0,                         // start vertex
                uiNumFaces);
        }
        ERR_X_Assert( SUCCEEDED(hr) );

        m_uiNumTriangles += uiNumFaces;
        m_uiNumDrawCalls++;

        if(bSimpleCombineXYMode)
        {
            iMode = 2; // now do the x
            g_oVertexShaderMgr.SetFeatureSPG2Mode(iMode);
            g_oVertexShaderMgr.Update(_pObject);
            g_oRenderStateMgr.Update(_pObject);

            if (poIndices != NULL)
            {
#if defined(_XENON_RENDER_PC)
                ulVertexCount = pCurrentMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

                hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                    0,                          // base index
                    0,                          // MinIndex - unused
                    ulVertexCount,              // NumVertices - unused
                    0,                          // start index
                    uiNumFaces);
            }
            else
            {

                hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                    0,                         // start vertex
                    uiNumFaces);
            }
            ERR_X_Assert( SUCCEEDED(hr) );

            m_uiNumTriangles += uiNumFaces;
            m_uiNumDrawCalls++;
        }

        if(bSimpleCombineXYHATMode)
        {
            iMode = 3; // now do the hat
            g_oVertexShaderMgr.SetFeatureSPG2Mode(iMode);
            g_oVertexShaderMgr.Update(_pObject);
            g_oRenderStateMgr.Update(_pObject);

            if (poIndices != NULL)
            {
#if defined(_XENON_RENDER_PC)
                ulVertexCount = pCurrentMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

                hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                    0,                          // base index
                    0,                          // MinIndex - unused
                    ulVertexCount,              // NumVertices - unused
                    0,                          // start index
                    uiNumFaces);
            }
            else
            {

                hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                    0,                         // start vertex
                    uiNumFaces);
            }
            ERR_X_Assert( SUCCEEDED(hr) );

            m_uiNumTriangles += uiNumFaces;
            m_uiNumDrawCalls++;
        }

        if(_pObject->GetExtraFlags() & QMFR_SPG2GRID)
            PostDrawPrimitiveCallback(DPCB_SPG2GRIDZPREPASS, _pObject->GetUserData());

        if(_pObject->GetBlendingMode() != -1)
        {
            _pObject->GetMaterial()->SetJadeFlags(ulOldBM);
        }
    }

    g_pXeContextManagerRender->SetCurrentRenderObject(NULL);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::PreRenderObject
// Params : _pObject : Object to render
//          _eFXPass : Current FX pass
// RetVal : None
// Descr. : Render a single object
// ------------------------------------------------------------------------------------------------
int XeRenderer::PreRenderObject(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass)
{
    XEProfileFunction();
    CXBBeginEventObjectCPU oEvent(0, "XeRenderer::PreRenderObject");

	int iNbOfPrimitiveDraw = 1;
    g_pXeContextManagerRender->SetCurrentRenderObject(_pObject);
    XeFurInfo *poFurConsts = NULL;

    // Use the blending modes as they were in QueueMeshForRender()
    XeMaterial* pMaterial = _pObject->GetMaterial();
    ULONG       ulOldBM   = pMaterial->GetJadeFlags();
    pMaterial->SetJadeFlags(_pObject->GetBlendingMode());

    g_oFXManager.BeginUpdate(_pObject, _eFXPass);

	// feature context
	switch((unsigned int)_pObject->GetObjectType())
	{
		case XeRenderObject::SPG2: 
			/*ERR_X_Assert(_pObject->GetCustomVS());

            m_pCurrentObject->SetLightSetIndex(m_ulSPG2LightSet);

			g_oFXManager.UpdateVSFeatures();
			g_oFXManager.UpdatePSFeatures();

            // specific spg2 generic features
            //g_oVertexShaderMgr.SetFeatureSPG2Mode(1);

			// restore shader constants
			g_oVertexShaderMgr.SetSPG2ShadersConsts(_pObject->GetSPG2ShadersConsts());
            g_oVertexShaderMgr.SetFeatureCustomShader(CUSTOM_VS_SPG2);
*/
			break;

		case XeRenderObject::Fur: 
            g_oFXManager.UpdateVSFeatures();
            
            poFurConsts = _pObject->GetFURShaderConsts();


            if (poFurConsts->m_ulType == FUR_TECHNIQUE_XENON1)
            {
                // set transform type for basemap
                ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
                g_pXeContextManagerRender->GetCurrentRenderObject()->GetMaterial()->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
                g_oVertexShaderMgr.SetFeatureBaseMapTransform((ul_BaseTC == TEXTRANSFORM_NORMAL) ? TEXTRANSFORM_NORMAL_FUR_OFFSETS : TEXTRANSFORM_NONE_FUR_OFFSETS);
                g_oVertexShaderMgr.SetFeatureDetailNMapTransform(TEXTRANSFORM_NORMAL_FUR_OFFSETS);
                g_oVertexShaderMgr.SetFeatureVertexSpecular(TRUE);

                g_oFXManager.UpdatePSForFurPass(_eFXPass != XeFXManager::RP_APPLY_PIXELLIGHTING);
            }
            else
            {
            g_oFXManager.UpdatePSFeatures();
            }

            g_oVertexShaderMgr.SetFeatureCustomShader(CUSTOM_VS_FUR);
            
            iNbOfPrimitiveDraw = (int)poFurConsts->m_vFURParams.x;
			DrawFirstPass = poFurConsts->FirstPass;
			break;

		case XeRenderObject::LightShaft:
            g_oXeLightShaftManager.BeginRendering(_pObject->GetExtraDataIndex());
			g_oFXManager.UpdateCustomVSFeatures(_pObject->GetMaterial()->GetCustomVS(), _pObject->GetMaterial()->GetCustomVSFeatures());
			g_oFXManager.UpdateCustomPSFeatures();
			break;

        case XeRenderObject::Rain:
            if (_pObject->GetExtraDataIndex() != 0xffffffff)
            {
                // Rain is renderered as an alpha mask
                g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAREF, 1);
                g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
            }
            g_oFXManager.UpdateCustomVSFeatures(_pObject->GetMaterial()->GetCustomVS(), _pObject->GetMaterial()->GetCustomVSFeatures());
            g_oFXManager.UpdateCustomPSFeatures();
            break;

        case XeRenderObject::Water: 
            g_pXeContextManagerRender->SetCurrentLightSet(_pObject->GetLightSetIndex());

    		g_oFXManager.UpdateCustomVSFeatures(_pObject->GetMaterial()->GetCustomVS(), _pObject->GetMaterial()->GetCustomVSFeatures());
		    g_oFXManager.UpdateCustomPSFeatures();

            break;

        case XeRenderObject::Sprites:
        case XeRenderObject::Particles:
            g_oFXManager.UpdateShaderFeaturesForSprites();
            break;

		case XeRenderObject::Common: 
		default:
			if(_pObject->GetMaterial()->GetCustomVS())
			{
				g_oFXManager.UpdateCustomVSFeatures(_pObject->GetMaterial()->GetCustomVS(), _pObject->GetMaterial()->GetCustomVSFeatures());
			}
			else
			{
				g_oFXManager.UpdateVSFeatures();
			}

			if(_pObject->GetMaterial()->GetCustomPS())
			{
				g_oFXManager.UpdateCustomPSFeatures();
			}
			else
			{
				g_oFXManager.UpdatePSFeatures();
			}

			break;
	}	

	g_oFXManager.EndUpdate();

    // Set the flags back to normal
    pMaterial->SetJadeFlags(ulOldBM);

	return iNbOfPrimitiveDraw;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::PostRenderObject
// Params : _pObject : Object to render
//          _eFXPass : Current FX pass
// RetVal : None
// Descr. : Post render operations
// ------------------------------------------------------------------------------------------------
void XeRenderer::PostRenderObject(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass)
{
    switch((unsigned int)_pObject->GetObjectType())
    {
        case XeRenderObject::LightShaft:
            g_oXeLightShaftManager.EndRendering();
            break;

        default:
            break;
    }
}


// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::PreDrawPrimitiveCallback
// Params : type of event to determine what to do
// RetVal : None
// Descr. : custom process prior to draw primitive
// ------------------------------------------------------------------------------------------------
void XeRenderer::PreDrawPrimitiveCallback(ULONG _ulType, void *_pData)
{
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::DrawSPG2GridInstances
// Params : specific data needed to draw instances
// RetVal : None
// Descr. : custom process after the draw primitive
// ------------------------------------------------------------------------------------------------

float				Xe_SPG2_SphereRadius[MaxSpheres];
MATH_tdst_Vector	Xe_SPG2_SphereCenters[MaxSpheres];
ULONG Xe_SPG2_ulNumberOfSpheres = 0;

void XeRenderer::DrawSPG2GridInstances(void *_pData, BOOL _bZPrepass )
{
    CXBBeginEventObject oEvent("DrawSPG2GridInstances");    
#ifdef VIDEOCONSOLE_ENABLE
    if(NoSPG2_Grid)
        return;
#endif

    SPG2_tdst_Modifier *pSPG2 = (SPG2_tdst_Modifier *)_pData;

    if(pSPG2->ulFlags1 & SPG2_DontDisplay)
        return;

    if(pSPG2->ulFlags & SPG2_GridGeneration)
    {
        XeVertexShader *pShader = g_oRenderStateMgr.GetCurrentVertexShader();
        D3DXMATRIX stBackupWorldViewProj, stBackupWorldView;
        D3DXMATRIX stNewWorldViewProjMatrix, stNewWorldViewMatrix;
        D3DXMATRIX stRotation, stTranslation, stLocalTransform;
        stBackupWorldViewProj = *g_oVertexShaderMgr.GetWorldViewProjMatrix();
        stBackupWorldView = *g_pXeContextManagerRender->GetWorldViewMatrixByIndex(g_pXeContextManagerRender->GetCurrentRenderObject()->GetWorldViewMatrixIndex());
    
        // set the same index to invalidate dirty flags for these matrices
        g_pXeContextManagerRender->SetWorldViewMatrix(g_pXeContextManagerRender->GetCurrentRenderObject()->GetWorldViewMatrixIndex());

        // instance loop

#ifdef _XENON_RENDER
        extern SPG2_GridMazpper	st_CurrentGridMapper;
        extern float   Time;
        extern FakeStr Fake[MAX_RND + 1];


        void  SPG2_ComputeSpecialGrid(SPG2_tdst_Modifier *pst_SPG2, WOR_tdst_World *p_stWorld, OBJ_tdst_GameObject *pst_GO);
        BOOL  SPG2_InBV(float fx, float fy, MATH_tdst_Vector *min, MATH_tdst_Vector *max);
        float MATH_ComputeCulling(MATH_tdst_Vector *pst_CamPos , MATH_tdst_Vector *pst_CamDir , float CamCos , float CamOoSin , MATH_tdst_Vector *pst_SphereCenter , float SphereRadius);

        //void SPG2_GenerateGridSPG2(MDF_tdst_Modifier *_pst_Mod, SPG2_tdst_Modifier *_pst_SPG2)
        {
            ULONG               RandomCounter=0,BaseAnimLocal;
            FakeStr				*pst_CurrentPond;
            
            MATH_tdst_Vector	st_GMin, st_GMax;
            
            MATH_tdst_Vector    stLocal;
            MATH_tdst_Vector    st_CameraPos;
            //MATH_tdst_Vector    st_CameraX;
            //MATH_tdst_Vector    st_CameraY;
            MATH_tdst_Vector    st_CameraDir;
            MATH_tdst_Matrix	stCurrentInverted;

            float     			CosV,SinV,Swap;
            
            CosV  = 1.0f;
            SinV  = 0.0f;

            SPG2_InstanceInforamtion			stII;

            stII.GlobalPos.x = stII.GlobalPos.y = stII.GlobalPos.z = 0.0f;
            stII.BaseWind = 0;
            stII.GlobalSCale = 1.0f;
            stII.Culling = 1.0f;
            stII.GlobalColor = 0;

            // make sure the compression doest not break the animation continuation
            int iTile = (int)(Time * pSPG2->AnimTextureSpeed);
            int nbTiles = ((2<<pSPG2->AnimTextureTileUPo2) * (2<<pSPG2->AnimTextureTileVPo2));
            iTile %= nbTiles;

            //stII.BaseAnimUv = (u32)(Time * pSPG2->AnimTextureSpeed * 65536.0f) & 0xffffff;
            stII.BaseAnimUv = (iTile & 0xffff)<<16;

            stII.BaseAnimUv |= pSPG2->AnimTextureTileUPo2 << 24;
            stII.BaseAnimUv |= pSPG2->AnimTextureTileVPo2 << 28;
            stII.HeatShimmerIntensity = 0.0f;

            RandomCounter = 0;
            BaseAnimLocal = stII.BaseAnimUv ;

            MATH_tdst_Matrix stWorldView;
            MATH_CopyMatrix(&stWorldView, (MATH_tdst_Matrix*)&stBackupWorldView);
            MATH_SetFullIdentityType(&stWorldView);
            MATH_InvertMatrix(&stCurrentInverted, &stWorldView);
            MATH_InitVector(&stLocal , 0.0f,0.0f,0.0f);
            MATH_TransformVertex(&st_CameraPos,&stCurrentInverted,&stLocal );
            MATH_InitVector(&stLocal , 0.0f,0.0f,1.0f);
            MATH_TransformVector(&st_CameraDir,&stCurrentInverted,&stLocal );
            
            // Per point gen + Parse the Grid 
            if (st_CurrentGridMapper.ulNumbeOfNode)
            {
                float SphereRadius , CosAlpha,OoSinAlpha,f_GlobalRatio;
                MATH_tdst_Vector    st_SphereCenter;

                SPG2_GridMazpperNode *p_stFirstGMNode,*p_stLastGMNode;
                
                st_CurrentGridMapper.GRID_NOISES[pSPG2->GridFilter & 0xf] = pSPG2->GridNoise;


                MATH_BlendVector(&st_SphereCenter , &pSPG2->MinBox , &pSPG2->MaxBox , 0.5f);
                SphereRadius = pSPG2->f_GlobalSize  + MATH_f_Distance(&st_SphereCenter , &pSPG2->MinBox );
                CosAlpha   = (float)  fCos(g_pXeContextManagerRender->f_FieldOfVision * 0.5f);
                OoSinAlpha = 1.0f / (float) fSin(g_pXeContextManagerRender->f_FieldOfVision * 0.5f);

                if (pSPG2->SphereCollideRadius == 0.0f ) 
                    pSPG2->SphereCollideRadius = 0.5f;

                p_stFirstGMNode = st_CurrentGridMapper.p_AllNodes;
                p_stLastGMNode = p_stFirstGMNode + st_CurrentGridMapper.ulNumbeOfNode;

                if(pSPG2->ulFlags1 & SPG2_GenInBV)
                {
                    MATH_CopyVector(&st_GMin, OBJ_pst_BV_GetGMin(pSPG2->pParentModifier->pst_GO->pst_BV));
                    MATH_CopyVector(&st_GMax, OBJ_pst_BV_GetGMax(pSPG2->pParentModifier->pst_GO->pst_BV));
                    MATH_AddEqualVector(&st_GMin, &pSPG2->pParentModifier->pst_GO->pst_GlobalMatrix->T);
                    MATH_AddEqualVector(&st_GMax, &pSPG2->pParentModifier->pst_GO->pst_GlobalMatrix->T);
                }

                XeMesh*			 poMesh			  =	g_pXeContextManagerRender->GetCurrentRenderObject()->GetMesh();
                XeIndexBuffer*   poIndices        = poMesh->GetIndices();

                MATH_InvertMatrix(&stCurrentInverted, pSPG2->pParentModifier->pst_GO->pst_GlobalMatrix);

                {
                    while (p_stFirstGMNode < p_stLastGMNode)
                    {
                        #define COSALPHA -0.34202014332566873304409961468226f	
                        #define SINALPHA 0.9396926207859083840541092773247f		

                        if(pSPG2->ulFlags & SPG2_RotationNoise)
                        {
                            Swap = COSALPHA * CosV + SINALPHA * SinV;
                            SinV = SINALPHA * CosV - COSALPHA * SinV;
                            CosV = Swap ;
                        }

                        if(pSPG2->ulFlags1 & SPG2_GenInBV)
                        {
                            if(!SPG2_InBV(p_stFirstGMNode->stPos.x, p_stFirstGMNode->stPos.y, &st_GMin, &st_GMax)) 
                            {
                                p_stFirstGMNode++;
                                continue;
                            }
                        }

                        RandomCounter++;

                        UCHAR cGridFire = p_stFirstGMNode -> GridFireContext[(g_iCurrentFireContext+1)%2];

                        if (((p_stFirstGMNode->GridValue & ~0x30) == pSPG2->GridFilter) || (cGridFire && (pSPG2->GridFilter & 0x10 /* Burning */)))
                        {
                            MATH_TransformVertex(&stII.GlobalPos ,&stCurrentInverted , &p_stFirstGMNode->stPos);
                                   
                            D3DXVECTOR4 vCenterBV4;
                            D3DXVec3Transform( &vCenterBV4, (D3DXVECTOR3*)&stII.GlobalPos, &stBackupWorldView );

                            if(vCenterBV4.z<-0.4f)
                            {
                                p_stFirstGMNode++;
                                continue;
                            }

                            float f_SqrObjDist = D3DXVec3LengthSq((D3DXVECTOR3*)&vCenterBV4);

                            float f_SqrProjRadius = (fSqr(poMesh->GetBoundingVolumeRadius()) * fSqr(g_pXeContextManagerRender->f_FactorX)) / f_SqrObjDist;

                            float f_ProjRadius = fOptSqrt(f_SqrProjRadius);

                            static float fCull = 5.0f;

                            // radius culling except for fire (too apparent)
                            if(f_ProjRadius<fCull && !(pSPG2->ulFlags & SPG2_ModeAdd))
                            {
                                p_stFirstGMNode++;
                                continue;
                            }

                            {
                                stII.HeatShimmerIntensity = 0.0f;

                                MATH_tdst_Vector    st_SphereCenterLocal; 
                                float LODSizeCoef;
                                MATH_AddVector(&st_SphereCenterLocal , &st_SphereCenter , &stII.GlobalPos );
                                stII.Culling = MATH_ComputeCulling(&st_CameraPos, &st_CameraDir, CosAlpha,OoSinAlpha,&st_SphereCenterLocal, SphereRadius);

                                pst_CurrentPond = &Fake[(RandomCounter) & (MAX_RND - 1) ] ;
                                stII.BaseWind = RandomCounter & (SPG2_WindNumbers - 1);
                                stII.GlobalZADD.x = stII.GlobalZADD.y = stII.GlobalZADD.z = 0.0f;
                                LODSizeCoef = stII.Culling * 8.0f - pst_CurrentPond->FakeXYZ.x;

                                if (pSPG2->ulFlags & SPG2_ModeAdd)
                                {
                                    stII.BaseAnimUv = (u32)(BaseAnimLocal + (17*(RandomCounter & 0xff) << 16)) & 0xffffff;
                                    stII.BaseAnimUv |= pSPG2->AnimTextureTileUPo2 << 24;
                                    stII.BaseAnimUv |= pSPG2->AnimTextureTileVPo2 << 28;

                                    VECTOR4FLOAT vUVManip;
                                    ULONG TextureTilerUV_Base = ((stII.BaseAnimUv >> 16) & 0xff);
                                    ULONG U_SHIFT = ((stII.BaseAnimUv >> 24) & 0xf);
                                    ULONG V_SHIFT = ((stII.BaseAnimUv >> 28) & 0xf);

                                    // Fast One Over 2^N
                                    FLOAT TexUMax, TexVMax;
                                    *(ULONG *)&TexUMax = (127 - U_SHIFT) << 23;
                                    *(ULONG *)&TexVMax = (127 - V_SHIFT) << 23;

                                    vUVManip.x = (FLOAT)TextureTilerUV_Base;
                                    vUVManip.y = TexVMax;
                                    vUVManip.z = TexUMax;
                                    vUVManip.w = TexVMax;

                                    if (pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_UVMANIP))
                                    {
                                        pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_UVMANIP, &vUVManip);
                                    }
                                }

                                // if too small, skip this instance (except for fire because it is too apparent)
                                if (LODSizeCoef > 0.05f || (pSPG2->ulFlags & SPG2_ModeAdd))
                                {
                                    float Distance;
                                    if (LODSizeCoef > 1.0f) 
                                        LODSizeCoef = 1.0f;
                                    
                                    stII.GlobalSCale = LODSizeCoef * (pst_CurrentPond ->FakeXYZ.z * 0.5f + 0.5f);

                                    f_GlobalRatio = pSPG2->f_GlobalRatio;

                                    if (pSPG2->ulFlags & SPG2_ModeAdd)
                                    {
                                        stII.GlobalSCale *= (float)cGridFire / 255.0f;
                                        f_GlobalRatio *= (float)cGridFire / 255.0f;

                                        if (pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_RATIO))
                                        {
                                            pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_RATIO, f_GlobalRatio);
                                        }

                                    }

                                    // Other sphere intrusions
                                    //VECTOR4FLOAT vGlobalZadd;
                                    //vGlobalZadd.x = vGlobalZadd.y = vGlobalZadd.z = vGlobalZadd.w = 0.0f;

                                    if (g_pXeContextManagerRender->Xe_SPG2_ulNumberOfSpheres)
                                    {
                                        ULONG SphereCounter;
                                        MATH_tdst_Vector	LocalPos;
                                        SphereCounter = g_pXeContextManagerRender->Xe_SPG2_ulNumberOfSpheres;
                                        while (SphereCounter --)
                                        {
                                            MATH_SubVector(&LocalPos, &g_pXeContextManagerRender->Xe_SPG2_SphereCenters[SphereCounter] , &p_stFirstGMNode->stPos);
                                            LocalPos.z += pSPG2 -> SphereCollideRadius;
                                            Distance = MATH_f_NormVector(&LocalPos);
                                            if (Distance < g_pXeContextManagerRender->Xe_SPG2_SphereRadius[SphereCounter] * pSPG2 -> SphereCollideRadius)
                                            {
                                                Distance = g_pXeContextManagerRender->Xe_SPG2_SphereRadius[SphereCounter] * pSPG2 -> SphereCollideRadius / Distance;
                                                MATH_AddScaleVector(&stII.GlobalZADD , &stII.GlobalZADD , &LocalPos ,  - Distance + 1.0f);
                                            }
                                        }
                                    }

                                    if (pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_GLOBALZADD))
                                    {
                                        pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_GLOBALZADD, (VECTOR4FLOAT*)&stII.GlobalZADD);
                                    }

                                    if (pSPG2->ulFlags & SPG2_ModeAdd)
                                    {
                                        stII.GlobalColor = 0;
                                    }
                                    else if (p_stFirstGMNode->GridValue & 0x30)
                                    {
                                        int Coef;
                                        if (p_stFirstGMNode->GridValue & 0x10) 
                                        {
                                            Coef = 255 - (cGridFire>>1);
                                            stII.GlobalColor = ((p_stFirstGMNode->Color & 0xFCFCFC)>>1) | 0xff000000;;
                                        }
                                        else
                                        {
                                            Coef = (cGridFire>>1);
                                            //stII.GlobalColor = ((p_stFirstGMNode->Color & 0xFCFCFC)>>2) | 0xff000000;;
                                            stII.GlobalColor = 0xff000000;
                                        }
                                        stII.GlobalSCale *= 0.25f + (float)Coef  / 340.0f;
                                    } 
                                    else
                                    {
                                        stII.GlobalColor = p_stFirstGMNode->Color | 0xff000000;
                                    }

                                    if (pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_GLOBALSCALE))
                                    {
                                        pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_GLOBALSCALE, stII.GlobalSCale);
                                    }

                                    //if (!(pSPG2->ulFlags & SPG2_ModeAdd))
                                    {
                                        if (pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_GLOBALCOLORWIND))
                                        {
                                            VECTOR4FLOAT vColorWind;

                                            if(stII.GlobalColor)
                                            {
                                                vColorWind.x = ((stII.GlobalColor & 0x00ff0000)>>16)/255.0f;
                                                vColorWind.y = ((stII.GlobalColor & 0x0000ff00)>>8 )/255.0f;
                                                vColorWind.z = ((stII.GlobalColor & 0x000000ff)    )/255.0f;
                                                //vColorWind.w = stII.BaseWind+0.1f; // 0.1 for round possible imprecision

                                                // grid global wind and color detection for the shader
                                                //if(vColorWind.x==0.0f)
                                                //    vColorWind.x = 0.0001f;

                                                vColorWind.w = max(1.0, stII.BaseWind); // use the global setting
                                            }
                                            else
                                                vColorWind.w = 0.0f;

                                            pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_GLOBALCOLORWIND, &vColorWind);
                                        }
                                    }

                                    BOOL b_WriteToAlpha = FALSE;
                                    if( !_bZPrepass && (pSPG2->ulFlags1 & SPG2_HeatShimmerEnable) != 0 )
                                    {
                                        // Compute heat shimmer intensity
                                        stII.HeatShimmerIntensity = ((float)cGridFire) / 255.0f;
                                        stII.HeatShimmerIntensity = 4.0f * (stII.HeatShimmerIntensity - 0.75f);
                                        stII.HeatShimmerIntensity = min( stII.HeatShimmerIntensity, 1.0f );
                                        
                                        // Heat shimmering
                                        if( poMesh->HasBoundingVolume() && stII.HeatShimmerIntensity > 0.01f )
                                        {
                                            HotAirObject st_HotAir;

                                            MATH_tdst_Vector  vBoxMin, vBoxMax;
                                            poMesh->GetAxisAlignedBoundingVolume( vBoxMin, vBoxMax );

                                            float fGlobalScale = stII.GlobalSCale;
                                         
                                            if( p_stFirstGMNode -> GridFire <= 255 && (p_stFirstGMNode->GridValue & 0x10) == 0 )
                                            {
                                                // The fire is dying down
                                                fGlobalScale = p_stFirstGMNode->MaxGlobalScale;
                                            }
                                            else
                                            {
                                                p_stFirstGMNode->MaxGlobalScale = fGlobalScale;
                                            }

                                            float fBVHeight = pSPG2->stInstance.fMaxSpriteSize * fGlobalScale;
                                          
                                            float fBVWidth = max( vBoxMax.x - vBoxMin.x, vBoxMax.y - vBoxMin.y );
                                            fBVWidth += 2.0f * fBVHeight * f_GlobalRatio;

                                            // Center of bounding volume
                                            MATH_tdst_Vector vCenterBV;
                                            MATH_AddVector( &vCenterBV, &vBoxMin, &vBoxMax );
                                            MATH_ScaleEqualVector( &vCenterBV, 0.5f );

                                            // Center of shimmer plane is top of sprites
                                            vCenterBV.z = vBoxMax.z + (2.0f*fBVHeight/3.0f); 
                                           
                                            MATH_AddEqualVector( &vCenterBV, &stII.GlobalPos );

                                            float fOpacity = min( pSPG2->f_HeatShimmerOpacity, 1.0f );
                                            fOpacity       = max( fOpacity, 0.0f );

                                            // Transform center to camera space

                                            D3DXVECTOR4 vCenterBV4;
                                            D3DXVec3Transform( &vCenterBV4, (D3DXVECTOR3*)&vCenterBV, &stBackupWorldView );

                                            // Find best position / size
                                            st_HotAir.Position         = *((MATH_tdst_Vector*)&vCenterBV4);
                                            st_HotAir.Width            = fBVWidth;
                                            st_HotAir.Height           = pSPG2->f_HeatShimmerHeight;
                                            st_HotAir.NoisePixelSize   = pSPG2->f_HeatShimmerNoisePixelSize;
                                            st_HotAir.Range            = pSPG2->f_HeatShimmerRange;
                                            st_HotAir.ScrollSpeed      = 0.0f;
                                            st_HotAir.Scroll           = pSPG2->f_HeatShimmerScroll;
                                            st_HotAir.Intensity        = stII.HeatShimmerIntensity * fOpacity;
                                            st_HotAir.BottomScale      = 0.35f;
                                            g_oHeatManager.AddHotAirPointToCache( &st_HotAir, g_pXeContextManagerRender->GetCurrentRenderObject()->GetProjMatrixIndex() );

                                            // Only when there is heat shimerring do we write to alpha to create mask of where 
                                            // not to distort the image
                                            b_WriteToAlpha = TRUE;
                                            m_bHasSPG2WriteAlpha = TRUE;
                                        }
                                    }
                                    {
                                        //DrawSPG2(&_pst_SPG2->stInstance,TexturesID,&st_CameraX,&st_CameraY,_pst_SPG2,Wind,&stII);
                                    }

                                    ULONG ulColorWriteMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
                                    if( b_WriteToAlpha )
                                    {
                                        ulColorWriteMask |= D3DCOLORWRITEENABLE_ALPHA;
                                    }
                                    g_oRenderStateMgr.SetRenderState(D3DRS_COLORWRITEENABLE, ulColorWriteMask );

                                    stLocalTransform._11 = CosV;
                                    stLocalTransform._12 = SinV;
                                    stLocalTransform._13 = 0.0f;
                                    stLocalTransform._14 = 0.0f;

                                    stLocalTransform._21 = -SinV;
                                    stLocalTransform._22 = CosV;
                                    stLocalTransform._23 = 0.0f;
                                    stLocalTransform._24 = 0.0f;

                                    stLocalTransform._31 = 0.0f;
                                    stLocalTransform._32 = 0.0f;
                                    stLocalTransform._33 = 1.0f;
                                    stLocalTransform._34 = 0.0f;

                                    stLocalTransform._41 = stII.GlobalPos.x;
                                    stLocalTransform._42 = stII.GlobalPos.y;
                                    stLocalTransform._43 = stII.GlobalPos.z;
                                    stLocalTransform._44 = 1.0f;

                                    if(pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WORLDVIEWPROJ))
                                    {
                                        D3DXMatrixMultiply(&stNewWorldViewProjMatrix, &stLocalTransform, &stBackupWorldViewProj);
                                        pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_WORLDVIEWPROJ, &stNewWorldViewProjMatrix);
                                    }

                                    if(pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WORLDVIEW))
                                    {
                                        D3DXMatrixMultiply(&stNewWorldViewMatrix, &stLocalTransform, &stBackupWorldView);
                                        pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_WORLDVIEW, &stNewWorldViewMatrix);

                                        if (pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_INVTRANSWORLDVIEW))
                                        {
                                            D3DXMATRIX oInvTransWorldView;
                                            D3DXMatrixInverse(&oInvTransWorldView, NULL, &stNewWorldViewMatrix);

                                            pShader->SetGlobalRawData(VS_GLOBAL_CONSTANT_INVTRANSWORLDVIEW, (VECTOR4FLOAT*)&oInvTransWorldView, 3);
                                        }
                                    }

                                    D3DPRIMITIVETYPE ePrimitiveType   = g_pXeContextManagerRender->GetCurrentRenderObject()->GetPrimType();

#if defined(_XENON_RENDER_PC)
                                    // Fake quad list support with indexed triangles and a special index buffer
                                    if (ePrimitiveType == D3DPT_QUADLIST)
                                    {
                                        poIndices      = g_XeBufferMgr.GetSpriteIB();
                                        ePrimitiveType = D3DPT_TRIANGLELIST;
                                    }
#endif

                                    UINT             uiNumFaces       = g_pXeContextManagerRender->GetCurrentRenderObject()->GetPrimitiveCount();
                                    ULONG            ulVertexCount    = 0;

#if defined(_XENON_RENDER_PC)
                                    XeMesh*			 poMesh			  =	g_pXeContextManagerRender->GetCurrentRenderObject()->GetMesh();
                                    ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

                                    HRESULT          hr;

                                    if (poIndices != NULL)
                                    {
#if defined(_XENON_RENDER_PC)
                                        ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

                                        hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                                            0,                          // base index
                                            0,                          // MinIndex - unused
                                            ulVertexCount,              // NumVertices - unused
                                            0,                          // start index
                                            uiNumFaces);
                                    }
                                    else
                                    {

                                        hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                                            0,                         // start vertex
                                            uiNumFaces);
                                    }
                                    ERR_X_Assert( SUCCEEDED(hr) );

                                    m_uiNumTriangles += uiNumFaces;
                                    m_uiNumDrawCalls++;
                                }
                            }
                        }
                        p_stFirstGMNode++;
                    }
                }
            }
        }
#endif // #ifdef _XENON_RENDER

    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::DrawSPG2HolderInstances
// Params : specific data needed to draw instances
// RetVal : None
// Descr. : custom process after the draw primitive
// ------------------------------------------------------------------------------------------------
void XeRenderer::DrawSPG2HolderInstances(void *_pData)
{
    CXBBeginEventObject oEvent("DrawSPG2HolderInstances");    
    
    SPG2Holder_tdst_Modifier *pSPG2Holder = (SPG2Holder_tdst_Modifier *)_pData;

    XeVertexShader *pShader = g_oRenderStateMgr.GetCurrentVertexShader();
    D3DXMATRIX stBackupWorldViewProj, stBackupWorldView;
    D3DXMATRIX stNewWorldViewProjMatrix, stNewWorldViewMatrix;
    D3DXMATRIX stRotation, stTranslation, stLocalTransform;
    stBackupWorldViewProj = *g_oVertexShaderMgr.GetWorldViewProjMatrix();
    stBackupWorldView = *g_pXeContextManagerRender->GetWorldViewMatrixByIndex(g_pXeContextManagerRender->GetCurrentRenderObject()->GetWorldViewMatrixIndex());

    XeMesh*			 poMesh			  =	g_pXeContextManagerRender->GetCurrentRenderObject()->GetMesh();
    XeIndexBuffer*   poIndices        = poMesh->GetIndices();

    // set the same index to invalidate dirty flags for these matrices
    g_pXeContextManagerRender->SetWorldViewMatrix(g_pXeContextManagerRender->GetCurrentRenderObject()->GetWorldViewMatrixIndex());

    XeSPG2Instances *pInstances = pSPG2Holder->pSPG2Instances;
    for(int i=0; i<pSPG2Holder->NumberOfSPG2Entries; i++,pInstances++)
    {
        stLocalTransform._11 = pInstances->vX.x;
        stLocalTransform._12 = pInstances->vX.y;
        stLocalTransform._13 = pInstances->vX.z;
        stLocalTransform._14 = 0.0f;
        stLocalTransform._21 = pInstances->vY.x;
        stLocalTransform._22 = pInstances->vY.y;
        stLocalTransform._23 = pInstances->vY.z;
        stLocalTransform._24 = 0.0f;
        stLocalTransform._31 = pInstances->vZ.x;
        stLocalTransform._32 = pInstances->vZ.y;
        stLocalTransform._33 = pInstances->vZ.z;
        stLocalTransform._34 = 0.0f;
        stLocalTransform._41 = pInstances->vPos.x;
        stLocalTransform._42 = pInstances->vPos.y;
        stLocalTransform._43 = pInstances->vPos.z;
        stLocalTransform._44 = 1.0f;

        if(pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WORLDVIEWPROJ))
        {
            D3DXMatrixMultiply(&stNewWorldViewProjMatrix, &stLocalTransform, &stBackupWorldViewProj);
            pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_WORLDVIEWPROJ, &stNewWorldViewProjMatrix);
        }

        if(pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WORLDVIEW))
        {
            D3DXMatrixMultiply(&stNewWorldViewMatrix, &stLocalTransform, &stBackupWorldView);
            pShader->SetGlobalMatrixAsFloat4x3(VS_GLOBAL_CONSTANT_WORLDVIEW, &stNewWorldViewMatrix);

            if (pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_INVTRANSWORLDVIEW))
            {
                D3DXMATRIX oInvTransWorldView;
                D3DXMatrixInverse(&oInvTransWorldView, NULL, &stNewWorldViewMatrix);

                pShader->SetGlobalRawData(VS_GLOBAL_CONSTANT_INVTRANSWORLDVIEW, (VECTOR4FLOAT*)&oInvTransWorldView, 3);
            }
        }

        D3DPRIMITIVETYPE ePrimitiveType   = g_pXeContextManagerRender->GetCurrentRenderObject()->GetPrimType();

#if defined(_XENON_RENDER_PC)
        // Fake quad list support with indexed triangles and a special index buffer
        if (ePrimitiveType == D3DPT_QUADLIST)
        {
            poIndices      = g_XeBufferMgr.GetSpriteIB();
            ePrimitiveType = D3DPT_TRIANGLELIST;
        }
#endif

        UINT             uiNumFaces       = g_pXeContextManagerRender->GetCurrentRenderObject()->GetPrimitiveCount();
        ULONG            ulVertexCount    = 0;
        HRESULT          hr;

        if (poIndices != NULL)
        {
#if defined(_XENON_RENDER_PC)
        ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

            hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
            0,                          // base index
            0,                          // MinIndex - unused
            ulVertexCount,              // NumVertices - unused
            0,                          // start index
            uiNumFaces);
        }
        else
        {

            hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                0,                         // start vertex
                uiNumFaces);
        }
        ERR_X_Assert( SUCCEEDED(hr) );

        m_uiNumTriangles += uiNumFaces;
        m_uiNumDrawCalls++;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::PostDrawPrimitiveCallback
// Params : type of event to determine what to do
// RetVal : None
// Descr. : custom process after the draw primitive
// ------------------------------------------------------------------------------------------------
inline void XeRenderer::PostDrawPrimitiveCallback(ULONG _ulType, void *_pData)
{
    switch(_ulType)
    {
    case DPCB_SPG2HOLDER:
        DrawSPG2HolderInstances(_pData);
        break;

    case DPCB_SPG2GRID:
        DrawSPG2GridInstances(_pData);
        break;

    case DPCB_SPG2GRIDZPREPASS:
        DrawSPG2GridInstances(_pData, TRUE );
        break;
    }
}


// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderObject
// Params : _pObject : Object to render
//          _eFXPass : Current FX pass
// RetVal : None
// Descr. : Render a single object
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderObject(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass)
{
	// Profiling
    XEProfileFunction();

	// Error Checking
	ERR_X_Assert(_pObject);

    // Get the right blending mode
    XeMaterial* pMat    = _pObject->GetMaterial();
    ULONG       ulOldBM = pMat->GetJadeFlags();
    pMat->SetJadeFlags(_pObject->GetBlendingMode());

#if defined(PIX_ENABLE)
    // Generate PIX Identifier String
    char acPIXString[256];

    if (_pObject->GetExtraFlags() & QMFR_GAO)
    {
	    OBJ_tdst_GameObject *_pGAO = (OBJ_tdst_GameObject *)_pObject->GetUserData();

		sprintf(acPIXString, "RO:%s, Lay %d, Mat %0x", _pGAO->sz_Name, _pObject->GetLayerIndex(), _pObject->GetMaterial());
    }
    else
    {
		sprintf(acPIXString, "RO:Non-GAO, Lay %d, Mat %0x", _pObject->GetLayerIndex(), _pObject->GetMaterial());
    }

    CXBBeginEventObject oEvent(0xFFFF0000, acPIXString);
#endif

    // Visibility query
    BeginConditionalRendering(_pObject, _eFXPass);

    // Pre-render setup
	int iNbOfPrimitiveDraw = PreRenderObject(_pObject, _eFXPass);

    // Update pixel shader
    g_oPixelShaderMgr.Update(_pObject);

    // Send object to VS Manager
    g_oVertexShaderMgr.Update(_pObject);

    // Update RenderStateMgr
    g_oRenderStateMgr.Update(_pObject);

    HRESULT hr;
    XeMesh*			 poMesh			  =	_pObject->GetMesh(); // TODO : might want to check before and do a special custon draw when the mesh is NULL
    XeIndexBuffer*   poIndices        = poMesh->GetIndices();
    UINT             uiNumFaces       = _pObject->GetPrimitiveCount();
    ULONG            ulVertexCount    = 0;
    D3DPRIMITIVETYPE ePrimitiveType   = _pObject->GetPrimType();

  #if defined(_XENON_RENDER_PC)
    // Fake quad list support with indexed triangles and a special index buffer
    if (ePrimitiveType == D3DPT_QUADLIST)
    {
        XeValidate(poIndices == NULL, "A QuadList mesh must not use indices on PC!");
        poIndices      = g_XeBufferMgr.GetSpriteIB();
        ePrimitiveType = D3DPT_TRIANGLELIST;
    }
  #endif
/*
    OBJ_tdst_GameObject *pst_GO = (_pObject->GetExtraFlags() & QMFR_GAO) ? (OBJ_tdst_GameObject *)_pObject->GetUserData() : NULL;

    if (pst_GO)
    {
        MDF_ApplyAllRender(pst_GO);
    }
*/
    if (uiNumFaces)
    {
        for(int iPrimitive = 0; iPrimitive < iNbOfPrimitiveDraw; iPrimitive++)
	    {
		    // primitive number context
		    if (_pObject->GetObjectType() == XeRenderObject::Fur)
		    {
				g_oFurManager.UpdateFurPrimitive(_pObject, iPrimitive);
				if ( DrawFirstPass )
				{
					DrawFirstPass =0;
					continue;
				}
			}

    #ifdef _DEBUG
            if (!m_bSkipNextObject)
    #endif
            {
                if (poIndices)
                {
                #if defined(_XENON_RENDER_PC)
                    ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
                #endif
                    hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType,             // prim type
                                                            0,                          // base index
                                                            0,                          // MinIndex - unused
                                                            ulVertexCount,              // NumVertices - unused
                                                            0,                          // start index
                                                            uiNumFaces);
                }
                else
                {
                    hr = m_pD3DDevice->DrawPrimitive(ePrimitiveType,            // prim type
                                                        0,                         // start vertex
                                                        uiNumFaces);
                }

                ERR_X_Assert(SUCCEEDED(hr));

                m_uiNumTriangles += uiNumFaces;
                m_uiNumDrawCalls++;
            }
    #ifdef _DEBUG 
            else
            {
                // reset 
                m_bSkipNextObject = FALSE;
            }
    #endif
	    }
    }
/*
    if(pst_GO)
    {
        MDF_UnApplyAllRender(pst_GO);
    }
*/
    PostRenderObject(_pObject, _eFXPass);

    // Restore the material
    pMat->SetJadeFlags(ulOldBM);
    
    g_pXeContextManagerRender->SetCurrentRenderObject(NULL);

    // Visibility query
    EndConditionalRendering(_pObject, _eFXPass);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::AddUIRectangleRequest
// Params : ..
// RetVal : None
// Descr. : Add a UI rectangle request to the UI list
// ------------------------------------------------------------------------------------------------
void XeRenderer::AddUIRectangleRequest(float _fX1, float _fY1, float _fX2, float _fY2,
                                       float _fU1, float _fV1, float _fU2, float _fV2,
                                       DWORD _dwColorTL, DWORD _dwColorTR, DWORD _dwColorBL, DWORD _dwColorBR,
                                       float _fAngle, DWORD _dwBlendingFlags, DWORD _dwTextureID)
{
    XeUIRectangle oRect;

    oRect.fX1               = _fX1;
    oRect.fY1               = _fY1;
    oRect.fX2               = _fX2;
    oRect.fY2               = _fY2;
    oRect.fU1               = _fU1;
    oRect.fV1               = _fV1;
    oRect.fU2               = _fU2;
    oRect.fV2               = _fV2;
    oRect.ulColorTL         = _dwColorTL;
    oRect.ulColorTR         = _dwColorTR;
    oRect.ulColorBL         = _dwColorBL;
    oRect.ulColorBR         = _dwColorBR;
    oRect.fAngle            = _fAngle;
    oRect.ulBlendingFlags   = _dwBlendingFlags;
    oRect.ulTextureID       = _dwTextureID;

    g_pXeContextManagerRender->PushUIRects(oRect);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::InitUIRects
// Params : None
// RetVal : None
// Descr. : Initialize the UI rectangle objects
// ------------------------------------------------------------------------------------------------
void XeRenderer::InitUIRects(void)
{
    USHORT* pawIndices = NULL;

    m_pUIRectRenderable = new XeRenderObject();
    for(int i=0; i<XERENDER_NB_RECT_MESH; i++)
        m_pUIRectMesh[i]= new XeMesh();
    m_pUIRectMaterial   = XeMaterial_Create();

    // Indices
    pawIndices = (USHORT*)MEM_p_Alloc((XERENDER_MAX_UI_RECTS * 6) * sizeof(USHORT));
    for (ULONG i = 0; i < XERENDER_MAX_UI_RECTS; ++i)
    {
        pawIndices[(i * 6) + 0] = (USHORT)((i * 4) + 0);
        pawIndices[(i * 6) + 1] = (USHORT)((i * 4) + 1);
        pawIndices[(i * 6) + 2] = (USHORT)((i * 4) + 2);
        pawIndices[(i * 6) + 3] = (USHORT)((i * 4) + 0);
        pawIndices[(i * 6) + 4] = (USHORT)((i * 4) + 2);
        pawIndices[(i * 6) + 5] = (USHORT)((i * 4) + 3);
    }

    XeBuffer *pVB;
    for(int i=0; i<XERENDER_NB_RECT_MESH; i++)
    {
        m_pUIRectMesh[i]->ClearAllStreams();
        m_pUIRectMesh[i]->SetIndices(pawIndices, XERENDER_MAX_UI_RECTS * 6);
	    pVB = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(XERENDER_MAX_UI_RECTS*4, sizeof(UIVertex));
	    m_pUIRectMesh[i]->AddStream(XEVC_POSITION | XEVC_COLOR0 | XEVC_TEXCOORD0, pVB);
    }

    m_pUIRectMaterial->AddTextureStage();

    m_pUIRectRenderable->SetMaterial(m_pUIRectMaterial);
    m_pUIRectRenderable->SetMesh(m_pUIRectMesh[0]);
    m_pUIRectRenderable->SetDrawMask(GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotInvertBF);
    m_pUIRectRenderable->SetWorldMatrixIndex(0); // Identity
    m_pUIRectRenderable->SetWorldViewMatrixIndex(0); // Identity
    m_pUIRectRenderable->SetProjMatrixIndex(0);  // Identity

    MEM_Free(pawIndices);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::ShutdownUIRects
// Params : None
// RetVal : None
// Descr. : Shutdown the UI rectangles
// ------------------------------------------------------------------------------------------------
void XeRenderer::ShutdownUIRects(void)
{
    SAFE_DELETE(m_pUIRectRenderable);
    for(int i=0; i<XERENDER_NB_RECT_MESH; i++)
        SAFE_DELETE(m_pUIRectMesh[i]);
    SAFE_DELETE(m_pUIRectMaterial);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderUIRectList
// Params : None
// RetVal : None
// Descr. : Render all the UI rectangles
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderUIRectList(void)
{
    ULONG ulNbRectangles = g_pXeContextManagerRender->GetNbUIRects();
    XeUIRectangle* pFirst;
    XeUIRectangle* pCurrent;
    ULONG ulRequestSize;

    if (ulNbRectangles == 0)
        return;

    CXBBeginEventObject oEvent("XeRenderer::RenderUIRectList");

    g_pXeContextManagerRender->SetWorldViewMatrix(0);
    g_pXeContextManagerRender->SetProjectionMatrix(0);

    m_fUIHalfWidth     = (FLOAT)m_stParams.BackBufferWidth * 0.5f;
    m_fUINegHalfHeight = (FLOAT)m_stParams.BackBufferHeight * -0.5f;
#if defined(ACTIVE_EDITORS)
    m_fUIAspect        = GetViewportAspectRatio();
#else
    m_fUIAspect        = GetBackbufferAspectRatio();
#endif

    pFirst        = NULL;
    ulRequestSize = 0;
    for (ULONG i = 0; i < ulNbRectangles; ++i)
    {
        pCurrent = g_pXeContextManagerRender->GetUIRect(i);

        if (pFirst == NULL)
        {
            pFirst        = pCurrent;
            ulRequestSize = 1;

            // Can't batch when there is a rotation angle
            if (pFirst->fAngle != 0.0f)
            {
                RenderUIRectangles(pFirst, ulRequestSize);
                pFirst        = NULL;
                ulRequestSize = 0;
            }
        }
        else
        {
            if ((pCurrent->ulBlendingFlags == pFirst->ulBlendingFlags) &&
                (pCurrent->ulTextureID     == pFirst->ulTextureID)     &&
                (pCurrent->fAngle          == 0.0f))
            {
                ++ulRequestSize;
            }
            else
            {
                RenderUIRectangles(pFirst, ulRequestSize);

                pFirst        = pCurrent;
                ulRequestSize = 1;
            }
        }

        if (ulRequestSize == XERENDER_MAX_UI_RECTS)
        {
            RenderUIRectangles(pFirst, ulRequestSize);
            pFirst        = NULL;
            ulRequestSize = 1;
        }
    }

    if ((pFirst != NULL) && (ulRequestSize > 0))
        RenderUIRectangles(pFirst, ulRequestSize);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderUIRectangles
// Params : _pFirstRect : First rectangle to render
//          _ulNbRects  : Number of rectangles in the request
// RetVal : None
// Descr. : Render UI rectangles
// ------------------------------------------------------------------------------------------------
void XeRenderer::RenderUIRectangles(XeUIRectangle* _pFirstRect, ULONG _ulNbRects)
{
    ERR_X_Assert(_ulNbRects <= XERENDER_MAX_UI_RECTS);

    D3DXMATRIX mResult;
    FLOAT fX1;
    FLOAT fY1;
    FLOAT fX2;
    FLOAT fY2;

    // Rotation
    if ((_ulNbRects == 1) && (_pFirstRect->fAngle != 0.0f))
    {
        D3DXMATRIX mTrans, mInvTrans;
        D3DXMATRIX mScale, mInvScale;
        D3DXMATRIX mRot;
        FLOAT fCenterX;
        FLOAT fCenterY;

        fX1 = (_pFirstRect->fX1 / m_fUIHalfWidth)     - 1.0f;
        fY1 = (_pFirstRect->fY1 / m_fUINegHalfHeight) + 1.0f;
        fX2 = (_pFirstRect->fX2 / m_fUIHalfWidth)     - 1.0f;
        fY2 = (_pFirstRect->fY2 / m_fUINegHalfHeight) + 1.0f;

        // Assume pivot is quad's center
        fCenterX = (fX1 + fX2) * 0.5f;
        fCenterY = (fY1 + fY2) * 0.5f;

        D3DXMatrixTranslation(&mTrans, -fCenterX, -fCenterY, 0.0f);
        D3DXMatrixTranslation(&mInvTrans, fCenterX, fCenterY, 0.0f);
        D3DXMatrixScaling(&mScale, m_fUIAspect, 1.0f, 1.0f);
        D3DXMatrixScaling(&mInvScale, 1.0f / m_fUIAspect, 1.0f, 1.0f);
        D3DXMatrixRotationZ(&mRot, _pFirstRect->fAngle);

        D3DXMatrixMultiply(&mResult, &mTrans, &mScale);
        D3DXMatrixMultiply(&mResult, &mResult, &mRot);
        D3DXMatrixMultiply(&mResult, &mResult, &mInvScale);
        D3DXMatrixMultiply(&mResult, &mResult, &mInvTrans);
    }
    else
    {
        D3DXMatrixIdentity(&mResult);
    }

    m_pUIRectRenderable->SetWorldMatrixIndex(0); // Identity

    m_pUIRectRenderable->SetWorldViewMatrixIndex(g_pXeContextManagerRender->PushWorldViewMatrix(&mResult));

    // make sure all streams are unbound before locking any of them
    g_oRenderStateMgr.UnbindAllStreams();

    static int iContext = 0;
    XeBuffer* pVB = m_pUIRectMesh[iContext]->GetStream(0)->pBuffer;
    UIVertex* pstVBData = (UIVertex*)pVB->Lock(_ulNbRects * 4, sizeof(UIVertex), TRUE);

    m_pUIRectRenderable->SetMesh(m_pUIRectMesh[iContext]);

    for (ULONG i = 0; i < _ulNbRects; ++i)
    {
        fX1 = _pFirstRect[i].fX1 * 2.0f - 1.0f;
        fY1 = 1.0f - _pFirstRect[i].fY1 * 2.0f;
        fX2 = _pFirstRect[i].fX2 * 2.0f - 1.0f;
        fY2 = 1.0f - _pFirstRect[i].fY2 * 2.0f;

        pstVBData[(i * 4) + 0].fX      = fX1;
        pstVBData[(i * 4) + 0].fY      = fY1;
        pstVBData[(i * 4) + 0].fZ      = 0.0f;
        pstVBData[(i * 4) + 0].ulColor = _pFirstRect[i].ulColorTL;
        pstVBData[(i * 4) + 0].fU      = _pFirstRect[i].fU1;
        pstVBData[(i * 4) + 0].fV      = _pFirstRect[i].fV1;

        pstVBData[(i * 4) + 1].fX      = fX1;
        pstVBData[(i * 4) + 1].fY      = fY2;
        pstVBData[(i * 4) + 1].fZ      = 0.0f;
        pstVBData[(i * 4) + 1].ulColor = _pFirstRect[i].ulColorBL;
        pstVBData[(i * 4) + 1].fU      = _pFirstRect[i].fU1;
        pstVBData[(i * 4) + 1].fV      = _pFirstRect[i].fV2;

        pstVBData[(i * 4) + 2].fX      = fX2;
        pstVBData[(i * 4) + 2].fY      = fY2;
        pstVBData[(i * 4) + 2].fZ      = 0.0f;
        pstVBData[(i * 4) + 2].ulColor = _pFirstRect[i].ulColorBR;
        pstVBData[(i * 4) + 2].fU      = _pFirstRect[i].fU2;
        pstVBData[(i * 4) + 2].fV      = _pFirstRect[i].fV2;

        pstVBData[(i * 4) + 3].fX      = fX2;
        pstVBData[(i * 4) + 3].fY      = fY1;
        pstVBData[(i * 4) + 3].fZ      = 0.0f;
        pstVBData[(i * 4) + 3].ulColor = _pFirstRect[i].ulColorTR;
        pstVBData[(i * 4) + 3].fU      = _pFirstRect[i].fU2;
        pstVBData[(i * 4) + 3].fV      = _pFirstRect[i].fV1;
    }

    pVB->Unlock(TRUE);

    // Set the number of faces we are currently rendering
    m_pUIRectMesh[iContext]->GetIndices()->SetFaceCount(_ulNbRects * 2);

    // Update the texture
    m_pUIRectMaterial->SetTextureId(0, _pFirstRect->ulTextureID);

    ULONG ulDrawMask = m_pUIRectRenderable->GetDrawMask();
    // Enable|Disable the use texture flag since the FX manager is using it now
    if (_pFirstRect->ulTextureID != MAT_Xe_InvalidTextureId)
    {
        ulDrawMask |= GDI_Cul_DM_UseTexture;
    }
    else
    {
        ulDrawMask &= ~GDI_Cul_DM_UseTexture;
    }

    // Force solid fill on rectangles!
    ulDrawMask |= GDI_Cul_DM_NotWired;
    m_pUIRectRenderable->SetDrawMask(ulDrawMask);
    m_pUIRectRenderable->SetObjectType(XeRenderObject::Interface);

    // Blending mode
    switch (_pFirstRect->ulBlendingFlags)
    {
        case MAT_Cc_Op_Copy:
            m_pUIRectMaterial->SetAlphaBlend(FALSE);
            break;
        case MAT_Cc_Op_Add:
            m_pUIRectMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
            break;
        case MAT_Cc_Op_XeAlphaAdd:
            m_pUIRectMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_ONE);
            break;
        case MAT_Cc_Op_XeModulateColor:
            m_pUIRectMaterial->SetAlphaBlend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ZERO);
            break;
        default:
            m_pUIRectMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
            break;
    }

    // always use texture for UI rects
#ifdef VIDEOCONSOLE_ENABLE
    BOOL bNoTexture = NoTexture;
    NoTexture = false;
#endif

    g_oXeRenderer.RenderObject(m_pUIRectRenderable, XeFXManager::RP_DEFAULT);

#ifdef VIDEOCONSOLE_ENABLE
    NoTexture = bNoTexture;
#endif

    iContext = (iContext+1) % XERENDER_NB_RECT_MESH;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RequestDynamicMesh
// Params : 
// RetVal : XeMesh -> pointer to a mesh frm the pool
// Descr. : Request a dynamic mesh from a pool of allocated mesh
// ------------------------------------------------------------------------------------------------
XeMesh* XeRenderer::RequestDynamicMesh()
{
  #ifndef _FINAL_
    UINT uiDynMeshCount = g_pXeContextManagerEngine->GetNbDynMesh();
    ERR_X_Assert(uiDynMeshCount < MAX_DYNAMIC_MESH);
    if (uiDynMeshCount + 1 > m_uiMaxRequestedDynMeshes)
        m_uiMaxRequestedDynMeshes = uiDynMeshCount + 1;
  #endif
    return g_pXeContextManagerEngine->PushDynMesh();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::InitFullscreenQuad
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRenderer::InitFullscreenQuad(XeMesh** _ppoQuadMesh, UINT _uiWidth, UINT _uiHeight)
{
    // Init fullscreen quad
    // allocate containers
    XeMesh* poQuadMesh = *_ppoQuadMesh;
    poQuadMesh = new XeMesh();

    // build mesh
    poQuadMesh->ClearAllStreams();
    poQuadMesh->AddStream(XEVC_POSITIONT | XEVC_COLOR0 | XEVC_TEXCOORD0, 0, NULL, 4);

    XeBuffer        *pBufferObject = poQuadMesh->GetStream(0)->pBuffer;
    SimpleVertex    *pVertexBuffer = (SimpleVertex*) pBufferObject->Lock(4, sizeof(SimpleVertex));

    // pixel center offset
    FLOAT fOffsetX, fOffsetY;

    fOffsetX = 1.0f / _uiWidth;  // 2.0f / uiWidth * 0.5f
    fOffsetY = 1.0f / _uiHeight;  // 2.0f / uiHeight * 0.5f

    // fullscreen quad
    pVertexBuffer->x = fLeft - fOffsetX;
    pVertexBuffer->y = fBottom + fOffsetY;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->Color = 0xFFFFFFFF;
    pVertexBuffer->u1 = 0.0f;
    pVertexBuffer->v1 = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fLeft - fOffsetX;
    pVertexBuffer->y = fTop + fOffsetY;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->Color = 0xFFFFFFFF;
    pVertexBuffer->u1 = 0.0f;
    pVertexBuffer->v1 = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fRight - fOffsetX;
    pVertexBuffer->y = fBottom + fOffsetY;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->Color = 0xFFFFFFFF;
    pVertexBuffer->u1 = 1.0f;
    pVertexBuffer->v1 = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fRight - fOffsetX;
    pVertexBuffer->y = fTop + fOffsetY;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->Color = 0xFFFFFFFF;
    pVertexBuffer->u1 = 1.0f;
    pVertexBuffer->v1 = 1.0f;
    pVertexBuffer++;

    pBufferObject->Unlock(TRUE);

    // mesh 
    poQuadMesh->SetIndices(NULL);

    *_ppoQuadMesh = poQuadMesh;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::RenderHeatShimmerQuadList
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void 
XeRenderer::RenderHeatShimmerQuadList()
{
    if( g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_HEAT_SHIMMER_QUADS) <= 0 )
        return;

    CXBBeginEventObject oEvent("RenderHeatShimmerQuadList");

    ClearAlphaOnly( 0 );

    XeRenderObject** pROList = g_pXeContextManagerRender->GetRenderObjectList(XeRT_HEAT_SHIMMER_QUADS);
    XeRenderObject*  pObj;

    for(int i=0; i<g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_HEAT_SHIMMER_QUADS); i++,pROList++)
    {
        pObj = *pROList;

        g_pXeContextManagerRender->SetHeatShimmerConsts( pObj->GetHeatShimmerVSConsts() );
        RenderObject( pObj, XeFXManager::RP_DEFAULT );
    }

    g_pXeContextManagerRender->ResetRenderObjectOfList(XeRT_HEAT_SHIMMER_QUADS);

    // Resolve back buffer in a scratch buffer
    g_oXeTextureMgr.ResolveToScratchBuffer( XEHEATSHIMMERSCRATCH_ID );
}

void 
XeRenderer::RenderTransparentGlowList( )
{
    if( g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_TRANSPARENT_GLOW) <= 0 )
        return;

    CXBBeginEventObject oEvent("XeRenderer::RenderTransparentGlowList");
    if (g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_TRANSPARENT_GLOW) > 1)
    {
        qsort((void*)(g_pXeContextManagerRender->GetRenderObjectList(XeRT_TRANSPARENT_GLOW)), 
            g_pXeContextManagerRender->GetNbRenderObjectOfList(XeRT_TRANSPARENT_GLOW), 
            sizeof(XeRenderObject*), XeTransparentObjectCompare);
    }

    m_bRenderedGlowTransparent = TRUE;
    ClearAlphaOnly( 0 );
    RenderList(XeRT_TRANSPARENT_GLOW);
}

void XeRenderer::EndRenderingGlowMask( )
{
    m_bIsRenderingGlowMask = false;    
    if( m_bRenderedGlowTransparent || m_bRenderedGlowOpaque )
    {
        g_oAfterEffectManager.ApplyGlow( ); 
    }
}

char* XeRenderer::GetListName(eXeRENDERLISTTYPE _eListType)
{
    switch(_eListType)
    {
        case XeRT_OPAQUE_NO_FOG:            return "OPAQUE_NO_FOG";
        case XeRT_OPAQUE:                   return "OPAQUE";
        case XeRT_TRANSPARENT:              return "TRANSPARENT";
        case XeRT_WATER:                    return "WATER";
        case XeRT_INTERFACE:                return "INTERFACE";
        case XeRT_REFLECTED:                return "REFLECTED";
        case XeRT_HEAT_SHIMMER_QUADS:       return "HEAT_SHIMMER_QUADS";
        case XeRT_SPG2:                     return "SPG2";
        case XeRT_SPG2OPAQUE:               return "SGP2OPAQUE";
        case XeRT_SPG2TRANSPARENT:          return "SPG2TRANSPARENT";
        case XeRT_FUR:                      return "FUR";
        case XeRT_ZOVERWRITE:               return "ZOVERWRITE";
        case XeRT_ZOVERWRITESPG2OPAQUE:     return "ZOVERWRITESPG2OPAQUE";
        case XeRT_ZOVERWRITESPG2TRANSPARENT:return "ZOVERWRITESPG2TRANSPARENT";
        case XeRT_TRANSPARENT_GLOW:         return "TRANSPARENT_GLOW";


#if defined(ACTIVE_EDITORS)
        case XeRT_HELPERS: return "HELPERS";
#endif

        default: return "Unnamed/Invalid";
    }
}
