//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2004 Ubisoft
//
// Author       Eric Le
// Date         14 Dec 2004
//
// $Archive: $
//
// Description
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __XERENDERER_H__INCLUDED
#define __XERENDERER_H__INCLUDED

#include "Precomp.h"
#include <vector>
#include "XeRenderObject.h"
#include "XeFXManager.h"
#include "XeVideoConsole.h"
#include "XeSharedDefines.h"
#include "XeSimpleRenderer.h"

#if defined(_XENON_RENDERER_USETHREAD)
#include "Xenon/EventThread/EventThread.h"
#endif

// for better object rendering tracing and debug
#define ROS_SPRITES			1
#define ROS_SPG2X			2
#define ROS_SPG2SPRITES		3
#define ROS_HELPERS			4
#define ROS_LIGHT_SHAFT     5
#define ROS_LAST_RENDER_OBJECT_SOURCE 5

#define XERENDER_INTERFACE_DEPTH        -64
#define XERENDER_MINDYNMESH             50
#define XERENDER_MAX_MATERIALS          4096
#define XERENDER_MAX_SHADOWLIGHTS       256

// so we are not always using the same rectangle buffers and causing undesired vertex lock lag
#define XERENDER_NB_RECT_MESH           10

// QueueMeshForRender Extra Flags
#define QMFR_CULLEDFROMCAMERA       0x00000002
#define QMFR_ANIMATED               0x00000004
#define QMFR_LIGHTED                0x00000008
#define QMFR_USELIGHTMAP            0x00000010
#define QMFR_ZBIAS                  0x00000020
#define QMFR_SPG2_WRITE_ALPHA       0x00000040
#define QMFR_GAO                    0x00000080
#define QMFR_SPG2GRID               0x00000100
#define QMFR_WYB1                   0x00000200
#define QMFR_WYB2                   0x00000400
#define QMFR_SYMMETRY               0x00000800
#define QMFR_HEATSHIMMER_GRID       0x00001000
#define QMFR_EDITOR_INFO_MASK       0xf0000000	// Editor only
#define QMFR_EDITOR_INFO_SHIFT      28

#if defined(ACTIVE_EDITORS)

//define used in ul_HighlightMode variable
#define QMFR_EDINFO_OK              0
#define QMFR_EDINFO_TOO_MANY_LIGHTS 1
#define QMFR_EDINFO_BR_OR_INTEGRATE	2
#define QMFR_EDINFO_PIXEL_LIGHTED	3
#define QMFR_EDINFO_CAST_REC_SHADOW	4
#define QMFR_EDINFO_END				5

//define passed to QueueMeshForRender
#define QMFR_EDINFO_COLOR_01		1
#define QMFR_EDINFO_COLOR_02		2
#define QMFR_EDINFO_COLOR_03		3
#define QMFR_EDINFO_COLOR_04		4
#define QMFR_EDINFO_COLOR_05		5

inline void XeSetQMFREditorInfo(ULONG& _rulQMFRFlags, ULONG _ulInfo)
{
	_rulQMFRFlags &= ~QMFR_EDITOR_INFO_MASK;
	_rulQMFRFlags |= (_ulInfo << QMFR_EDITOR_INFO_SHIFT);
}

inline ULONG XeGetQMFREditorInfo(ULONG _ulQMFRFlags)
{
	return ((_ulQMFRFlags & QMFR_EDITOR_INFO_MASK) >> QMFR_EDITOR_INFO_SHIFT);
}
#endif

// DrawPrimitive callback types
#define DPCB_SPG2HOLDER             0x00000000
#define DPCB_SPG2GRID               0x00000001
#define DPCB_SPG2GRIDZPREPASS       0x00000002

#define XERENDER_USE_XENON_LIGHTS
#if defined(XERENDER_USE_XENON_LIGHTS)
#pragma message("XeRenderer - XERENDER_USE_XENON_LIGHTS defined")
#endif

#define USE_DEFERRED_FREE_AND_RELEASE
#if defined(USE_DEFERRED_FREE_AND_RELEASE)
    typedef std::vector<void*>          ThreadSafeFree;
    typedef std::vector<XeMesh*>        ThreadSafeMeshDelete;
    typedef std::vector<XeBuffer*>      ThreadSafeVBRelease;
    typedef std::vector<XeIndexBuffer*> ThreadSafeIBRelease;

    extern CRITICAL_SECTION     g_ThreadSafeLock;
    extern ThreadSafeFree       g_ThreadSafeFree;
    extern ThreadSafeMeshDelete g_ThreadSafeMeshDelete;
    extern ThreadSafeVBRelease  g_ThreadSafeVBRelease;
    extern ThreadSafeIBRelease  g_ThreadSafeIBRelease;

    #ifndef THREAD_SAFE_FREE
    #define THREAD_SAFE_FREE(_x_)                \
    if ((_x_) != NULL)                           \
    {                                            \
        EnterCriticalSection(&g_ThreadSafeLock); \
        g_ThreadSafeFree.push_back(_x_);         \
        (_x_) = NULL;                            \
        LeaveCriticalSection(&g_ThreadSafeLock); \
    }
    #endif // #ifndef THREAD_SAFE_FREE

    #ifndef THREAD_SAFE_MESH_DELETE
    #define THREAD_SAFE_MESH_DELETE(_x_)         \
    if ((_x_) != NULL)                           \
    {                                            \
        EnterCriticalSection(&g_ThreadSafeLock); \
        g_ThreadSafeMeshDelete.push_back(_x_);   \
        (_x_) = NULL;                            \
        LeaveCriticalSection(&g_ThreadSafeLock); \
    }
    #endif // #ifndef THREAD_SAFE_MESH_DELETE

    #ifndef THREAD_SAFE_VB_RELEASE
    #define THREAD_SAFE_VB_RELEASE(_x_)          \
    if ((_x_) != NULL)                           \
    {                                            \
        EnterCriticalSection(&g_ThreadSafeLock); \
        g_ThreadSafeVBRelease.push_back(_x_);    \
        (_x_) = NULL;                            \
        LeaveCriticalSection(&g_ThreadSafeLock); \
    }
    #endif // #ifndef THREAD_SAFE_VB_RELEASE

    #ifndef THREAD_SAFE_IB_RELEASE
    #define THREAD_SAFE_IB_RELEASE(_x_)          \
    if ((_x_) != NULL)                           \
    {                                            \
        EnterCriticalSection(&g_ThreadSafeLock); \
        g_ThreadSafeIBRelease.push_back(_x_);    \
        (_x_) = NULL;                            \
        LeaveCriticalSection(&g_ThreadSafeLock); \
    }
    #endif // #ifndef THREAD_SAFE_VB_RELEASE
#else
    #ifndef THREAD_SAFE_FREE
    #define THREAD_SAFE_FREE(_x_) SAFE_FREE(_x_)
    #endif
    #ifndef THREAD_SAFE_MESH_DELETE
    #define THREAD_SAFE_MESH_DELETE(_x_) SAFE_DELETE(_x_)
    #endif
    #ifndef THREAD_SAFE_VB_RELEASE
    #define THREAD_SAFE_VB_RELEASE(_x_) SAFE_RELEASE(_x_)
    #endif
    #ifndef THREAD_SAFE_IB_RELEASE
    #define THREAD_SAFE_IB_RELEASE(_x_) SAFE_RELEASE(_x_)
    #endif
#endif

#if defined(_XENON_RENDERER_USETHREAD)
typedef enum
{
    eXeThreadEvent_DynamicVB_InitialLock,
    eXeThreadEvent_DynamicVB_FinalUnlock,
	eXeThreadEvent_Flip,

	eXeThreadEvent_Count
}
eXeRenderer_ThreadEvents;
#endif

class CVideoNoLoop;

class XeRenderer
{
    struct UIVertex
    {
        FLOAT fX; FLOAT fY; FLOAT fZ;
        ULONG ulColor;
        FLOAT fU; FLOAT fV;
    };

#if defined(ACTIVE_EDITORS)
    // Transformed vertex [ XEVC_POSITIONT | XEVC_DIFFUSE ]
    struct TransformedVertex
    {
        FLOAT fX, fY, fZ, fW;
        ULONG ulColor;
    };

    // Transformed line
    struct TransformedLine
    {
        TransformedVertex stPtA;
        TransformedVertex stPtB;
    };

    // Transformed quad
    struct TransformedQuad
    {
        FLOAT fPt1X, fPt1Y;
        FLOAT fPt2X, fPt2Y;
        FLOAT fZ;
        ULONG ulColor;
    };

    struct Transformed3DQuad
    {
        D3DXVECTOR4 p[4];
        ULONG       ulColor;
    };

    struct NormalizedViewport
    {
        float   fPosX;
        float   fPosY;
        float   fWidth;
        float   fHeight;
    };

    typedef std::vector<TransformedLine>   TransformedLineArray;
    typedef std::vector<TransformedQuad>   TransformedQuadArray;
    typedef std::vector<Transformed3DQuad> Transformed3DQuadArray;
#endif

public:

    XeRenderer();
    ~XeRenderer();


    char* GetListName(eXeRENDERLISTTYPE _eListType);

	struct XeVertexDyn
	{
		struct _vPos
		{
			FLOAT fX; FLOAT fY; FLOAT fZ;
		} vPos;

		ULONG ulColor;

		struct _UV
		{
			FLOAT fU; 
			FLOAT fV;
		} UV;		
	};

    enum  SPG2PASS
    {
        eSPG2PassShadow,
        eSPG2PassOpaque,
        eSPG2PassTransparent,
        eSPG2PassZOverwriteOpaque,
        eSPG2PassZOverwriteTransparent,

        eSPG2PassCount
    };

    void    SetGamma(float fGammaFactor, float fContrastFactor, float fBrightnessFactor);
    HRESULT Init(HWND _hMainWindow = NULL, HWND _hRenderWindow = NULL);
    HRESULT ReInit(HWND _hRenderWindow = NULL);
    void    Shutdown(void);
    FLOAT   GetFPS() { return m_fFPS; }
    FLOAT   GetAvgFPS() { return m_fAvgFPS; }
    FLOAT   GetMaxFPS() { return m_fMaxFPS; }
    FLOAT   GetMinFPS() { return m_fMinFPS; }
    UINT    GetNumTriangles() { return m_uiNumTriangles; }
    UINT    GetNumSPG2() { return m_uiNumSPG2; }
    UINT    GetFrameCounter() { return m_uiFrameCounter; }
	BOOL	GetScreenCapture() { return m_bScreenCapture; }
	void	SetScreenCapture() { m_bScreenCapture = TRUE; }
	void	TakeScreenShot();
    void    UseTripleBuffer(BOOL bEnable) { m_bUseTripleBuffer = bEnable; }
    void    UseParallelGDK(BOOL bEnable) { m_bUseParallelGDK = bEnable; }
    BOOL    IsGDKParallelized() { return m_bUseParallelGDK; }

	void	ResetFPSStats();
    inline void    AddToDrawCallCount(UINT _uiDrawCallsToAdd) { m_uiNumDrawCalls += _uiDrawCallsToAdd; }
    inline void    AddToTriangleCount(UINT _uiTrianglesToAdd) { m_uiNumTriangles += _uiTrianglesToAdd; }

    void    BeforeDisplay();
    void    AfterDisplay();

    inline void    DrawSPG2HolderInstances(void *_pData);
    inline void    DrawSPG2GridInstances(void *_pData, BOOL _bZPrepass = FALSE );

    inline void    PreDrawPrimitiveCallback(ULONG _ulType, void *_pData);
    inline void    PostDrawPrimitiveCallback(ULONG _ulType, void *_pData);

    inline  IDirect3DDevice9* GetDevice() { return m_pD3DDevice; }
    inline  IDirect3D9* GetD3D() { return m_pD3D; }
    void    GetBackbufferResolution(UINT* _puiWidth, UINT* _puiHeight);
    float   GetBackbufferAspectRatio();
    void    AddHalfViewportPixelOffset( float & u, float & v );

#if defined(ACTIVE_EDITORS)
    void  SetViewport(LONG _lX, LONG _lY, UINT _uiWidth, UINT _uiHeight);
    void  GetViewport(LONG* _plX, LONG* _plY, UINT* _puiWidth, UINT* _puiHeight);
    void  GetNormalizedViewport(float* _pfX, float* _pfY, float* _pfWidth, float* _pfHeight);
    void  GetViewportResolution(UINT* _puiWidth, UINT* _puiHeight);
    FLOAT GetViewportAspectRatio(void);
    void  AdjustUVToViewport( float & u, float & v );

    void DiscardHelpers(void);

    void Add2DLineRequest(FLOAT _fPtAX, FLOAT _fPtAY, FLOAT _fPtAZ, FLOAT _fPtAW,
                          FLOAT _fPtBX, FLOAT _fPtBY, FLOAT _fPtBZ, FLOAT _fPtBW,
                          ULONG _ulColor);

    void Add2DQuadRequest(FLOAT _fPt1X, FLOAT _fPt1Y,
                          FLOAT _fPt2X, FLOAT _fPt2Y,
                          FLOAT _fZ,    ULONG _ulColor);

    void Add3DQuadRequest(D3DXVECTOR4* _pstVertices, ULONG _ulColor);

    void QueueRequests(void);

    inline void SetZBias(BOOL _bEnable) { m_bZBiasEnable = _bEnable; }

#endif

  #ifdef _XENON
    // Xenon specific
  #if (_XDK_VER >= 1838)
    PXVIDEO_MODE GetVideoMode() { return &m_oVideoMode; }
  #endif
  #endif

    void SetBackgroundColor(DWORD dwColor) {m_dwBackgroundColor = dwColor; }
    DWORD GetBackgroundColor() { return m_dwBackgroundColor; };
    void Clear(DWORD _dwColor, float _fDepth, DWORD _dwStencil);
    void ClearTarget(DWORD _dwColor);
    void ClearDepthStencil(float _fDepth, DWORD _dwStencil);
    void ClearAlphaOnly(DWORD _dwColor);
    void ClearDepthTexture();

    void PostRender();
    void RenderFrame();
    void PresentFrame();

    void InvalidateRenderLists(void);

    XeMesh* RequestDynamicMesh();
    void QueueMeshForRender(MATH_tdst_Matrix *_pstGlobalMatrix, 
							XeMesh* _poMesh,
                            XeMaterial* _poMaterial,
                            ULONG _ulDrawMask,
                            ULONG _ulLMTex,
                            eXeRENDERLISTTYPE _eRenderType,
                            XeRenderObject::XEPRIMITIVETYPE _ePrimType,
                            UCHAR _ucLOD,
							CHAR _cOrder,
                            LPVOID _pUserData = NULL,
							XeRenderObject::XEOBJECTTYPE _eObjectType=XeRenderObject::Common,
                            ULONG _ulExtraDataIndex = 0,
                            ULONG _ulExtraFlags = 0,
                            ULONG _ulLayerIndex = 0);

	void QueueMeshForRenderSPG2(MATH_tdst_Matrix *_pstGlobalMatrix, 
		XeMesh* _poMesh,
		XeMaterial* _poMaterial,
		ULONG _ulDrawMask,
		eXeRENDERLISTTYPE _eRenderType,
		XeRenderObject::XEPRIMITIVETYPE _ePrimType,
		UCHAR _ucLOD,
		CHAR _cOrder,
		LPVOID _pUserData = NULL,
        BOOL _bWriteToAlpha = FALSE );

    void QueueLightShaftForRender(MATH_tdst_Matrix*               _pstGlobalMatrix,
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
                                  ULONG                           _ulProjectionMatrixIndex);

    void QueueRainForRender(MATH_tdst_Matrix*               _pstGlobalMatrix,
                            XeMesh*                         _poMesh,
                            XeMaterial*                     _poMaterial,
                            ULONG                           _ulDrawMask,
                            XeRenderObject::XEPRIMITIVETYPE _ePrimType,
                            UCHAR                           _ucLOD,
                            CHAR                            _cOrder,
                            LPVOID                          _pUserData,
                            ULONG                           _ulWorldMatrixIndex,
                            ULONG                           _ulWorldViewMatrixIndex,
                            ULONG                           _ulProjectionMatrixIndex);

    void AddUIRectangleRequest(float _fX1, float _fY1, float _fX2, float _fY2,
                               float _fU1, float _fV1, float _fU2, float _fV2,
                               DWORD _dwColorTL, DWORD _dwColorTR, DWORD _dwColorBL, DWORD _dwColorBR,
                               float _fAngle, DWORD _dwBlendingFlags, DWORD _dwTextureID);

    BOOL IsThereSomethingToRender();
    BOOL IsPS_2_a_Supported() { return m_bPS_2_a; };
    BOOL IsPS_3_0_Supported() { return m_bPS_3_0; };
    inline ULONG GetMaxLightsPerPass( BOOL _bUseRimLight );
    inline INT GetMaxHWLightsPerPass();
    inline ULONG GetMaxTotalLightCount();
    inline ULONG GetRequestedMaxLight();
    BOOL IsHiDef();
    D3DMULTISAMPLE_TYPE GetMultiSampleType() { return m_stParams.MultiSampleType; }

    void StartRenderingGlowMask ( ) { m_bIsRenderingGlowMask = true; m_bRenderedGlowOpaque = m_bRenderedGlowTransparent = FALSE; }
    BOOL IsRenderingGlowMask( )  { return m_bIsRenderingGlowMask; }
    void IncGlowPrimitiveCount  ( ) { m_bRenderedGlowOpaque = TRUE; }
    void EndRenderingGlowMask   ( );
    BOOL HasRenderedGlowingOpaque() { return m_bRenderedGlowOpaque; } 
    BOOL HasRenderedGlowingTransparent() { return m_bRenderedGlowTransparent; } 

    BOOL HasSPG2WriteAlpha ( ) { return m_bHasSPG2WriteAlpha; }

  #ifdef _DEBUG
    inline void SkipObject(BOOL bSkip) { m_bSkipNextObject = bSkip; }
  #endif

    BOOL GetWireframe(void) { return m_bWireframe; }
    BOOL GetFog(void) { return m_bFog; }

#if defined(_XENON_RENDERER_USETHREAD)
	inline void RequestThreadEvent(eXeRenderer_ThreadEvents _eEvent);
	inline int  WaitForEventComplete(eXeRenderer_ThreadEvents _eEvent);
    inline void AcquireThreadOwnership();
    inline void ReleaseThreadOwnership();
	inline void PrepareThreadForNewFrame();
#endif

    void RenderObject(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass);
    
    XeMesh* GetQuadMesh() { return m_poQuadMesh; }
    XeMesh* Get640x480QuadMesh() { return m_po640x480QuadMesh; }

    // Shadows
    INT GetMaxShadowIter(void ) { return m_iMaxShadowIter; }
    INT GetMaxShadows(void) { return m_iMaxNumShadows; }

    void SetRTResolution(UINT _uiWidth, UINT _uiHeight) { m_uiRTWidth = _uiWidth; m_uiRTHeight = _uiHeight; }
    void ResetRTResolution(void) { m_uiRTWidth = m_stParams.BackBufferWidth; m_uiRTHeight = m_stParams.BackBufferHeight; }

    // Ocean
    BOOL IsOceanInitialized(void) { return m_bOceanInitialized; }
    void SetOceanInitialized(void) { m_bOceanInitialized = TRUE; }
    void SetOceanTextureID(DWORD _dwID) { m_pOceanMaterial->SetTextureId(0, _dwID); }
    XeMaterial* GetOceanMaterial(void) { return m_pOceanMaterial; }
    XeMesh*     GetOceanMesh(void) { return m_pOceanMesh; }

    // Z prepass
    inline BOOL IsFastZEnabled();

#ifdef _XENON
	void SetVideo( CVideoNoLoop* pVideo )
	{
		m_pVideo = pVideo;
	}

#endif

private:
	int  PreRenderObject(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass);
    void PostRenderObject(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass);
    void InitializeDisplayModes(void);
    void InitializePresentParams(HWND _hDeviceWindow);
	void InitializeRasterOptions(void);

    void RenderList(eXeRENDERLISTTYPE _eListType);
    void RenderShadowBuffers(INT _iNumShadowLights);
    void RenderZPrepass(eXeRENDERLISTTYPE _eListType);
    void RenderZPrepassSPG2(eXeRENDERLISTTYPE _eListType);// fast path for spg2
    void RenderHeatShimmerQuadList();   
    void RenderTransparentGlowList();

    void RenderSPG2List(SPG2PASS _ePass);// fast path for spg2

    // Fullscreen quad
    void InitFullscreenQuad(XeMesh** _ppoQuadMesh, UINT _uiWidth, UINT _uiHeight);

    void InitUIRects(void);
    void ShutdownUIRects(void);
    void RenderUIRectList(void);
    void RenderUIRectangles(XeUIRectangle* _pFirstRect, ULONG _ulNbRects);

    void CreatePredicatedTilingStuff(void);
    HRESULT Reset(bool _bSkipTCL, HWND _hRenderWindow = NULL);

    // Visibility query stuff
    inline void BeginConditionalRendering(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass);
    inline void EndConditionalRendering(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass);

    void ClearLists();

    IDirect3D9*         m_pD3D;
    IDirect3DDevice9*   m_pD3DDevice;
    D3DPRESENT_PARAMETERS m_stParams;                           // Info that was used for init
    UINT                m_uiRTWidth;                            // Render target width
    UINT                m_uiRTHeight;                           // Render target height
    D3DDISPLAYMODE*     m_astDisplayModes;                      // Enumerated display modes
    DWORD               m_dwBackgroundColor;
    BOOL                m_bPS_2_a;
    BOOL                m_bPS_3_0;
    INT                 m_iRequestedMaxLightPerPass;
    INT                 m_iRequestedMaxLight;
    BOOL                m_bUseTripleBuffer;
    BOOL                m_bUseParallelGDK;

    eXeRENDERLISTTYPE      m_eCurrentList;

    // Statistics / Debug infos
    void                ComputeFPS(void);
    FLOAT               m_fFPS;
	FLOAT				m_fMinFPS;
	FLOAT				m_fMaxFPS;
	FLOAT				m_fAvgFPS;
    FLOAT               m_fMeanDeltaTime;
    UINT                m_uiFrameCounter;                       // How many loops since beginning
    UINT                m_uiNumDrawCalls;                       // # DrawPrim calls
    UINT                m_uiNumTriangles;                       // Submitted triangles
	UINT				m_uiThreadWaitMS;
    MEMORYSTATUS        m_stMemStatus;
    TBool               m_bWireframe;
    TBool               m_bFog;
    TBool               m_bVisQueries;
    TBool               m_bFastZPrepass;
    TInt                m_iMaxNumShadows;
    TInt                m_iMaxShadowIter;
    TInt                m_iShadowBufferSize;
    UINT                m_uiMaxRequestedDynMeshes;

	BOOL				m_bScreenCapture;

#if defined(_XENON_RENDERER_USETHREAD)
    TBool               m_bThreadedRendering;
	int volatile		m_iThreadOwner_ID;
	int volatile		m_iThreadOwner_RefCount;
	CEventThread*       m_pThread;
#endif

    //FLOAT               m_afCameraParams[4];                    // Near, Far, FOV X, FOV Y
    //D3DXMATRIX          m_oCameraMatrix;
    
#if defined(_XENON_RENDER_PC)
    HWND                m_hRenderWindow;
#endif

    // Shared fullscreen quad
    XeMesh              *m_poQuadMesh;
    XeMesh              *m_po640x480QuadMesh;

    // UI rectangles
    XeRenderObject*     m_pUIRectRenderable;
    XeMesh*             m_pUIRectMesh[XERENDER_NB_RECT_MESH];
    XeMaterial*         m_pUIRectMaterial;
    FLOAT               m_fUIHalfWidth;
    FLOAT               m_fUINegHalfHeight;
    FLOAT               m_fUIAspect;

    // spg2 lightset
    //ULONG               m_ulSPG2LightSet;

  #ifdef _DEBUG
    // Utility
    BOOL                m_bSkipNextObject;
  #endif

    void*               m_poLastWorldMatrix;

    float               m_fHalfViewportPixelWidth;
    float               m_fHalfViewportPixelHeight;

    BOOL                m_bZPassRenderedSomething;
    BOOL                m_bIsRenderingGlowMask;
    BOOL                m_bRenderedGlowOpaque;
    BOOL                m_bRenderedGlowTransparent;
    BOOL                m_bHasSPG2WriteAlpha;
	BOOL				DrawFirstPass;

#if defined(ACTIVE_EDITORS)
    D3DVIEWPORT9         m_stViewport;
    NormalizedViewport   m_stNormalizedViewport;

    XeMaterial*          m_apoMaterialPool[XERENDER_MAX_MATERIALS];
    ULONG                m_ulNbMaterials;

    TransformedLineArray   m_av2DLinePool;
    TransformedQuadArray   m_av2DQuadPool;
    Transformed3DQuadArray m_av3DQuadPool;
    XeMaterial*            m_pst2DRequestMaterial;

    BOOL                m_bZBiasEnable;
#endif

    D3DCAPS9            m_stDeviceCaps;

    // Ocean
    BOOL                m_bOceanInitialized;
    XeMesh*             m_pOceanMesh;
    XeMaterial*         m_pOceanMaterial;

public:
    UINT                m_uiNumSPG2;

#ifdef _XENON
    // Xenon specific
  #if (_XDK_VER >= 1838)
    XVIDEO_MODE         m_oVideoMode;
  #endif
    
    // Gamma ramp
    FLOAT m_fCurrentGammaFactor,        m_fRequestedGammaFactor;
    FLOAT m_fCurrentContrastFactor,     m_fRequestedContrastFactor;
    FLOAT m_fCurrentBrightnessFactor,   m_fRequestedBrightnessFactor;

    // Visibility Query
    INT m_iVisQueryIndex;

    // GPR allocations
    INT m_iPixelShadingGPRCount;

	// Video
	CVideoNoLoop*		m_pVideo;
#endif
};

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::GetMaxLightsPerPass
// Params :
//          
// RetVal : 
// Descr. :
// ------------------------------------------------------------------------------------------------
ULONG XeRenderer::GetRequestedMaxLight()
{
#ifdef _XENON
    return min(VS_MAX_LIGHTS_IN_ENGINE, m_iRequestedMaxLight);
#else
    ULONG ulReqMaxLights = (GDI_gpst_CurDD && (GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ForceMaxDynLights)) ? min(VS_MAX_LIGHTS_IN_ENGINE, m_iRequestedMaxLight) : m_iRequestedMaxLight;
    return ulReqMaxLights;
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::GetMaxLightsPerPass
// Params :
//          
// RetVal : 
// Descr. :
// ------------------------------------------------------------------------------------------------
ULONG XeRenderer::GetMaxLightsPerPass( BOOL _bUseRimLight )
{
    ULONG ulLightCount;

    // 4 lights per pass
    ulLightCount = min((ULONG)GetMaxHWLightsPerPass(), GetMaxTotalLightCount());
    ulLightCount = min(ulLightCount, (ULONG)m_iRequestedMaxLightPerPass);
   
    return ulLightCount;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::GetMaxLightsPerPass
// Params :
//          
// RetVal : 
// Descr. :
// ------------------------------------------------------------------------------------------------
INT XeRenderer::GetMaxHWLightsPerPass()
{
    return VS_MAX_LIGHTS_PER_PASS;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderer::GetMaxLightsPerPass
// Params :
//          
// RetVal : 
// Descr. :
// ------------------------------------------------------------------------------------------------
ULONG XeRenderer::GetMaxTotalLightCount()
{
    return min(VS_MAX_LIGHTS, GetRequestedMaxLight());
}

inline BOOL XeRenderer::IsHiDef()
{
    return (m_stParams.BackBufferWidth == 1280);
}

inline void XeRenderer::BeginConditionalRendering(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass)
{
#ifdef _XENON
    if(!m_bVisQueries)
        return;

    if((_eFXPass == XeFXManager::RP_ZPASS_ALPHA_TEST) || (_eFXPass == XeFXManager::RP_ZPASS_Z_ONLY))
    {
        // Z pre-pass
        if(m_iVisQueryIndex < 64)
        {
            _pObject->SetVisQueryIndex(m_iVisQueryIndex);
            m_iVisQueryIndex++;
        }

        if(_pObject->GetVisQueryIndex() != -1)
        {
            m_pD3DDevice->BeginConditionalSurvey(_pObject->GetVisQueryIndex(), 0);
        }
    }
    else if((m_eCurrentList == XeRT_OPAQUE) && (_eFXPass != XeFXManager::RP_COMPUTE_SHADOW))
    {
        // Normal opaque rendering
        // Decide if we should render this object depending on result of the query
        if(_pObject->GetVisQueryIndex() != -1)
            m_pD3DDevice->BeginConditionalRendering(_pObject->GetVisQueryIndex());
    }

#endif
}

inline void XeRenderer::EndConditionalRendering(XeRenderObject* _pObject, XeFXManager::RenderPass _eFXPass)
{
#ifdef _XENON
    if(!m_bVisQueries)
        return;

    if((_eFXPass == XeFXManager::RP_ZPASS_ALPHA_TEST) || (_eFXPass == XeFXManager::RP_ZPASS_Z_ONLY))
    {
        // Z pre-pass
        if(_pObject->GetVisQueryIndex() != -1)
            m_pD3DDevice->EndConditionalSurvey(0);
    }
    else if((m_eCurrentList == XeRT_OPAQUE) && (_eFXPass != XeFXManager::RP_COMPUTE_SHADOW))
    {
        // Normal opaque rendering
        if(_pObject->GetVisQueryIndex() != -1)
        {
            if(m_bVisQueries)
            {
                m_pD3DDevice->EndConditionalRendering();
            }

            _pObject->SetVisQueryIndex(-1);
        }
    }
#endif
}

inline BOOL XeRenderer::IsFastZEnabled()
{
#ifdef _XENON
    return m_bFastZPrepass;
#else
    return FALSE;
#endif
}

#if defined(_XENON_RENDERER_USETHREAD)
inline void XeRenderer::RequestThreadEvent(eXeRenderer_ThreadEvents _eEvent)
{
    m_pThread->RequestEvent(_eEvent);
}

inline int XeRenderer::WaitForEventComplete(eXeRenderer_ThreadEvents _eEvent)
{
    return m_pThread->WaitForEventComplete(_eEvent);
}

void XeRenderer::AcquireThreadOwnership()
{
	// Warning : This function is not completely thread-safe and should not be called simultaneously

	// Error Checking

	if (m_pD3DDevice == NULL)
	{
		return;
	}

	// Fetch Current Thread ID

	int iThreadID = GetCurrentThreadId();

	if (m_iThreadOwner_ID != iThreadID)
	{
		// Unowned / Owned By Different Thread

		while (m_iThreadOwner_ID != -1)
		{
			// Wait
#if !defined(_FINAL)
            ERR_OutputDebugString("[XeRenderer] AcquireThreadOwnerShip() - Waiting For Release\n");
#endif
		}

		// No Thread Owns It : Acquire

		m_iThreadOwner_ID = iThreadID;
	}

	m_iThreadOwner_RefCount++;

    m_pD3DDevice->AcquireThreadOwnership();
}

void XeRenderer::ReleaseThreadOwnership()
{
	// Warning : This function is not completely thread-safe and should not be called simultaneously

	// Error Checking

	if (m_pD3DDevice == NULL)
	{
		return;
	}

	// Fetch Current Thread ID

	int iThreadID = GetCurrentThreadId();

	if (m_iThreadOwner_ID == iThreadID)
	{
		// Owned By Current Thread

		m_iThreadOwner_RefCount--;

		if (m_iThreadOwner_RefCount == 0)
		{
		    m_pD3DDevice->ReleaseThreadOwnership();

			m_iThreadOwner_ID = -1;
		}
	}
	else
	{
		// Harmless Release

	    m_pD3DDevice->ReleaseThreadOwnership();
	}
}

inline void XeRenderer::PrepareThreadForNewFrame()
{
    m_uiThreadWaitMS = WaitForEventComplete(eXeThreadEvent_Flip);

	if( g_oXeSimpleRenderer.IsActive( ) )
		g_oXeSimpleRenderer.SignalMainRenderingOff( );
	else
		AcquireThreadOwnership();
}
#endif

extern __declspec(align(32)) XeRenderer g_oXeRenderer;

#ifdef VIDEOCONSOLE_ENABLE
    // SPG2
    extern TBool NoSPG2_Geom;
    extern TBool NoSPG2_Grid;
    extern TBool NoSPG2_AI;

    // AfterFx
    extern TBool NoAfterFX;
    extern TBool NoBlackQuad;
    extern TBool NoGodRayLowRes;
    extern TFloat GodRayRatio; 

    // 3d engine
    extern TBool NoRenderList;
    extern TBool NoDrawOpaque;
    extern TBool NoDrawTransparent;
    extern TBool NoTexture;
    extern TBool NoMoss;
#endif

#endif // !defined(__XERENDERER_H__INCLUDED)
