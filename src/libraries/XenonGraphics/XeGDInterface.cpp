// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------

#include "precomp.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GraphicDK/Sources/SOFT/SOFTstruct.h"

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"

#include "XeUtils.h"
#include "XeGDInterface.h"
#include "XeRenderer.h"
#include "XeSimpleRenderer.h"
#include "XeTextureManager.h"
#include "XeVertexShaderManager.h"
#include "XePixelShaderManager.h"
#include "XeDynVertexBuffer.h"
#include "XeShadowManager.h"
#include "XeShader.h"
#include "XeAfterEffectManager.h"
#include "XeWeatherManager.h"
#include "XeHeatManager.h"
#include "XeShaderDatabase.h"
#include "XeLightShaftManager.h"

#include "../Tools/XenonTexture/Sources/XenonTexture.h"

#if defined(_XENON_RENDER)
#include "STRing/STRstruct.h"
#endif


// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------

struct GDI_tdst_DisplayData_;
struct CAM_tdst_Camera_;
#define GEO_Vertex  MATH_tdst_Vector

extern XeRenderer g_oXeRenderer;

#ifdef TEST_DETECT_LINE_OF_FIRE_SPG2
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
void SPG2_GenerateLeafExplosion(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pos, FLOAT fRadius);
#endif

#if !defined(_FINAL_)
BOOL s_bMustUnloadShaders = FALSE;
#endif

#if defined(ACTIVE_EDITORS)
extern BOOL EDI_gb_ComputeMap;
extern BOOL EDI_gb_XeQuickLoad;
extern BOOL EDI_gb_SlashXPK;
extern BOOL EDI_gb_SlashLXPK;
#endif

// ------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------

#if defined(ACTIVE_EDITORS)

static void Xe_GDIInitTool();
static void Xe_GDIShutdownTool();

static void Xe_GDIRequest(ULONG _ulData, BOOL _bEnable);

static void Xe_GDIDrawSoftArrow(SOFT_tdst_Arrow* _pst_Arrow);
static void Xe_GDIDrawSoftEllipse(SOFT_tdst_Ellipse* _pst_Ellipse);
static void Xe_GDIDrawSoftSquare(SOFT_tdst_Square* _pst_Square);
static void Xe_GDIDrawPointEx(GDI_tdst_Request_DrawPointEx* _pst_Data);
static void Xe_GDIDrawLineEx(GDI_tdst_Request_DrawLineEx* _pst_Data);
static void Xe_GDIDrawLine(MATH_tdst_Vector** _pst_Vectors);
static void Xe_GDIDrawQuad(MATH_tdst_Vector** _pst_Vectors);
static void Xe_GDIDrawQuadEx(GDI_tdst_Request_DrawQuad* _pst_Data);

#endif

extern D3DFORMAT TEX_XeConvertJadeXTFToD3DFORMAT(ULONG _ul_JadeXTF);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

void * Xe_pst_CreateDevice(void)
{
    return NULL;
}

void Xe_DestroyDevice(void *)
{
    int a = 0;
}

LONG Xe_l_Init(HWND _hWnd, struct GDI_tdst_DisplayData_* _pst_DD)
{
    _pst_DD->GlobalMul2X           = TRUE;
    _pst_DD->GlobalMul2XFactor     = 1.0f;
    _pst_DD->b_DrawWideScreenBands = TRUE;
    _pst_DD->b_EnableColorCorrection = TRUE;

#if defined(_XENON_RENDER_PC)
    HRESULT hr = g_oXeRenderer.Init(_hWnd, MAI_gh_MainWindow);

#if defined(ACTIVE_EDITORS)
    if (!FAILED(hr))
    {
        Xe_GDIInitTool();
    }
    _pst_DD->ShowAEInEngine       = TRUE;
    _pst_DD->ShowAEEditor         = FALSE;
    _pst_DD->DrawGraphicDebugInfo = TRUE;
    _pst_DD->TRI_ALarm            = 500000;
    _pst_DD->SPG_ALarm            = 20000;
#endif

    return hr;

#else
    g_oXeRenderer.Init();

    return D3D_OK;
#endif
}

LONG Xe_l_Close(void *)
{
#if defined(_XENON_RENDER_PC)
    GDI_RestoreAllAfterXenonGraphics();
#endif

#if defined(ACTIVE_EDITORS)
    Xe_GDIShutdownTool();
#endif

	STR_XenonShutdown();

    g_oXeRenderer.Shutdown();

    return D3D_OK;
}

LONG Xe_l_ReadaptDisplay(HWND _hWnd, struct GDI_tdst_DisplayData_ *)
{
#if defined(_XENON_RENDER_PC)
    return (g_oXeRenderer.ReInit(_hWnd) == S_OK);
#endif

    return 1;
}

void Xe_Clear(LONG _l_Buffer, ULONG _ul_Color)
{
    if(_l_Buffer == GDI_Cl_ZBuffer)
    {
        g_oXeRenderer.ClearDepthStencil(1.0f, 0);
    }
    else
    {
        CXBBeginEventObject oEvent("Xe_Clear");
        g_oXeRenderer.Clear(_ul_Color, 1.0f, 0);
        g_oXeRenderer.SetBackgroundColor(_ul_Color);
    }
}

static UINT g_uiFrameCounter = 0;

void Xe_RequestFlip(void)
{
#if defined(_XENON_RENDERER_USETHREAD)
    g_oXeRenderer.ReleaseThreadOwnership();
    g_oXeRenderer.RequestThreadEvent(eXeThreadEvent_Flip);
#else
    Xe_Flip();
#endif
}

#if defined(_XENON_RENDERER_USETHREAD)
void Xe_ThreadCallback_DynamicVB_InitialLock(int _iEventID, void* _pParameter)
{
}

void Xe_ThreadCallback_DynamicVB_FinalUnlock(int _iEventID, void* _pParameter)
{
}

void Xe_ThreadCallback_Flip(int _iEventID, void* _pParameter)
{
    Xe_Flip();

	if( g_oXeSimpleRenderer.IsActive( ) )
		g_oXeSimpleRenderer.SignalMainRenderingOff( );
}
#endif

void Xe_ResetAllShaders(void)
{
#if !defined(_FINAL_)
    s_bMustUnloadShaders = TRUE;
#endif
}

void Xe_Flip(void)
{
    // raster must not be on while presenting frame, ... deactivate
    _GSP_EndRaster(19);

#if defined(_XENON_RENDERER_USETHREAD)
    g_oXeRenderer.AcquireThreadOwnership();
#endif

#if !defined(_FINAL_)
    if (s_bMustUnloadShaders)
    {
        s_bMustUnloadShaders = FALSE;

        // Unload all vertex and pixel shaders
        g_oVertexShaderMgr.UnloadAllShaders();
        g_oPixelShaderMgr.UnloadAll();
 
        // Clean up the shader database
        g_oXeShaderDatabase.UnloadShaders();
    }
#endif

    g_oXeRenderer.PresentFrame();

	/* Screen capture */
	if(g_oXeRenderer.GetScreenCapture())
	{
		g_oXeRenderer.TakeScreenShot();
	}


    // raster must not be on while presenting frame, ... reactivate
    _GSP_BeginRaster(19);

/*// DJ_TEMP +++++++++++++++
    if(!(g_pXeContextManagerRender->ul_DisplayFlags & GDI_Cul_DF_DoNotRender))
    {
        ULONG ul_Color = (_pst_DD->pst_World) ? _pst_DD->pst_World->ul_BackgroundColor : 0;
        if (g_pXeContextManagerRender->ul_DisplayFlags & GDI_Cul_DF_DoNotClear)
        {
            _pst_DD->st_GDI.pfnv_Clear(GDI_Cl_ZBuffer, ul_Color);
            _pst_DD->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotClear;
        }
        else
            _pst_DD->st_GDI.pfnv_Clear(GDI_Cl_ColorBuffer | GDI_Cl_ZBuffer, ul_Color);

        _pst_DD->ul_RenderingCounter++;
    }
// DJ_TEMP -----------------*/

#if defined(_XENON_RENDERER_USETHREAD)
    g_oXeRenderer.ReleaseThreadOwnership();
#endif

    g_oXeRenderer.m_uiNumSPG2 = 0;
}

void Xe_SetViewMatrix(MATH_tdst_Matrix* _pstMatrix)
{
    if(MATH_b_TestScaleType(_pstMatrix))
    {
        MATH_tdst_Matrix stOGLMatrix;
        MATH_MakeOGLMatrix(&stOGLMatrix, _pstMatrix);

        g_pXeContextManagerEngine->PushWorldViewMatrix(((D3DXMATRIX*)&stOGLMatrix));
    }
    else
    {
        g_pXeContextManagerEngine->PushWorldViewMatrix((D3DXMATRIX*)_pstMatrix);
    }
}
/*
void Xe_SetProjectionMatrix(struct CAM_tdst_Camera_ *_pstCamera)
{
D3DXMATRIX stProjMatrix;

D3DXMatrixPerspectiveFovLH(&stProjMatrix,
_pstCamera->f_FieldOfVision,
g_oXeRenderer.GetBackbufferAspectRatio(),
_pstCamera->f_NearPlane,
_pstCamera->f_FarPlane);

g_oVertexShaderMgr.SetProjectionMatrix(&stProjMatrix);
}
*/

void Xe_Ortho(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    D3DXMATRIX   OrthoMatrix;
    /*~~~~~~~~~~~~~~~~~~~~*/

    memset(&OrthoMatrix, 0, sizeof(OrthoMatrix));

    OrthoMatrix._11 = 2.0f / (Right - Left);
    OrthoMatrix._22 = 2.0f / (Top - Bottom);
    OrthoMatrix._33 = -4.0f / (Far - Near);

    OrthoMatrix._41 = -(Right + Left) / (Right - Left);
    OrthoMatrix._42 = -(Top + Bottom) / (Top - Bottom);
    OrthoMatrix._43 = -(Far + Near) / (Far - Near);

    OrthoMatrix._44 = 1.0f;
    g_pXeContextManagerEngine->PushProjectionMatrix(&OrthoMatrix);
}

void Xe_SetProjectionMatrix(CAM_tdst_Camera *_pst_Cam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float                   f, f_ScreenRatio, f_NearPlane;
    LONG                    w, h, W, H;
    LONG                    x, y;
    ULONG                   Flags;
    float                   l, r, t, b;
    FLOAT                   f_FieldOfView = _pst_Cam->f_FieldOfVision;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Compute screen ratio
    if (!(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective) )
        f_ScreenRatio = GDI_gaf_ScreenRation[ GDI_Cul_SRC_Square ];
    else
    {
#ifdef _XENON
      #if (_XDK_VER >= 1838)
        if (g_oXeRenderer.GetVideoMode()->fIsWideScreen)
      #else
        if(XGetVideoFlags() & XC_VIDEO_FLAGS_WIDESCREEN)
      #endif
        {
            x = GDI_Cul_SRC_16over9;
        }
        else
#endif
        {
            x = GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst;
        }

        if ( (x <= 0) ||    (x >= GDI_Cul_SRC_Number) ) 
            f_ScreenRatio = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
        else
            f_ScreenRatio = GDI_gaf_ScreenRation[ x ];
    }

    // apply gigascale
    f_NearPlane = _pst_Cam->f_NearPlane * 0.05f;


    f = _pst_Cam->f_YoverX * GDI_gpst_CurDD->st_ScreenFormat.f_PixelYoverX * f_ScreenRatio;

    g_pXeContextManagerEngine->SetCameraParams(f_NearPlane,
        _pst_Cam->f_FarPlane,
        f_FieldOfView,
        f_FieldOfView * f);

    Flags = GDI_gpst_CurDD->st_ScreenFormat.ul_Flags;

    // Compute height and width of screen
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

    if (_pst_Cam->ul_Flags & CAM_Cul_Flags_Ortho )
    {
        if (_pst_Cam->ul_Flags & CAM_Cul_Flags_OrthoYInvert)
            Xe_Ortho( 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);
        else
            Xe_Ortho( 0.0f, (FLOAT) w, 0.0f, (FLOAT) h, -1.0f, 1.0f);
        return;
    }

    static MATH_tdst_Matrix s_ProjMatrix;

    if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        f = 1.0f / fNormalTan(f_FieldOfView / 2);
        MATH_SetIdentityMatrix(&s_ProjMatrix);

        if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
        {
            s_ProjMatrix.Ix = f * f_ScreenRatio;
            s_ProjMatrix.Jy = -f;
        }
        else
        {
            s_ProjMatrix.Ix = f;
            s_ProjMatrix.Jy = -f / f_ScreenRatio;
        }

        s_ProjMatrix.Kz = (_pst_Cam->f_FarPlane + f_NearPlane) / (_pst_Cam->f_FarPlane - f_NearPlane);
        s_ProjMatrix.Sz = 1.0f;
        s_ProjMatrix.T.z = -f_NearPlane;
        s_ProjMatrix.w = 0.0f; // MATRIX W!

        g_pXeContextManagerEngine->PushProjectionMatrix((D3DXMATRIX*)&s_ProjMatrix);
    }
    else
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        float   f_IsoFactorZoom, f_Scale;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        f_IsoFactorZoom = _pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom;
        f_Scale = f_IsoFactorZoom;

        if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
        {
            f = 1 / GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;

            if(_pst_Cam->f_IsoFactor == 0)
            {
                Xe_Ortho((1 - f) / 2, (1 + f) / 2, 0, 1, -f_NearPlane, -_pst_Cam->f_FarPlane);
            }
            else
            {
                l = f_Scale * (-f);
                r = f_Scale * (f);
                b = f_Scale * -1;
                t = f_Scale * 1;

                Xe_Ortho(l, r, b, t, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
            }
        }
        else
        {
            f = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
            if(_pst_Cam->f_IsoFactor == 0)
            {
                Xe_Ortho(0, 1, (1 - f) / 2, (1 + f) / 2, -f_NearPlane, -_pst_Cam->f_FarPlane);
            }
            else
            {
                t = f_Scale * (-f);
                b = f_Scale * (f);
                l = f_Scale * -1;
                r = f_Scale * 1;
                Xe_Ortho(l, r, b, t, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
            }
        }
    }

#if defined(ACTIVE_EDITORS)
    {
        extern ULONG g_ul_BIG_SNAPSHOT_COUNTER;
        w &= ~3;

        GDI_gpst_CurDD->st_Device.Vx = x;
        GDI_gpst_CurDD->st_Device.Vy = y;
        GDI_gpst_CurDD->st_Device.Vw = w;
        GDI_gpst_CurDD->st_Device.Vh = h;
        // SC: glScissor(x, y ? y - 1 : 0, w + 2, h + 2);

        if (g_ul_BIG_SNAPSHOT_COUNTER)
        {
            w <<= 2;
            h <<= 2;
            x -= ((g_ul_BIG_SNAPSHOT_COUNTER>> 2) & 3) * (w>>2);
            y -= (g_ul_BIG_SNAPSHOT_COUNTER & 3) * (h>>2);
        }

        g_oXeRenderer.SetViewport(x , y, (UINT)w, (UINT)h);
    }
#endif // defined(ACTIVE_EDITORS)
}
typedef struct  DX9VertexFormat_
{
	float X,Y,Z;
	ULONG Color;
	float fU,fV;
} DX9VertexFormat;

#define DX8_SetColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
		ulOGLSetCol ^= ulOGLSetCol_XOr; \
		if(pst_Alpha) \
		{ \
			ulOGLSetCol &= 0x00ffffff; \
			ulOGLSetCol |= pst_Alpha[a]; \
		} \
		pVertexBuffer->Color = XeConvertColor(ulOGLSetCol); \
	} \
	else if(pst_Alpha) \
	{ \
		ulOGLSetCol &= 0x00ffffff; \
		ulOGLSetCol |= pst_Alpha[a]; \
		pVertexBuffer->Color = XeConvertColor(ulOGLSetCol); \
	}
#define D3DFVF_VERTEXF (D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2 )

LONG Xe_l_DrawElementIndexedTriangles(GEO_tdst_ElementIndexedTriangles  *_pst_Element,
                                      GEO_Vertex                        *_pst_Point,
                                      GEO_tdst_UV                       *_pst_UV,
                                      ULONG                             ulnumberOfPoints)
{
/* 	DX9VertexFormat				*pVertexBuffer;
	GEO_tdst_IndexedTriangle	*t, *tend;
	ULONG						*pst_Color, *pst_Alpha;
	ULONG						TNum, ulOGLSetCol, ulOGLSetCol_XOr, ulOGLSetCol_Or;
    GDI_tdst_DisplayData	*pst_SD;
	LPDIRECT3DDEVICE9   m_pD3DDevice;
	m_pD3DDevice   = g_oXeRenderer.GetDevice();

    D3DVERTEXELEMENT9 VertexElements[] =
    {
            { 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0 },
            { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,    0 },
            { 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };
    
    // Create a vertex declaration from the element descriptions
    IDirect3DVertexDeclaration9* pVertexDecl;
    m_pD3DDevice->CreateVertexDeclaration( VertexElements, &pVertexDecl );


	pst_SD = GDI_gpst_CurDD;
	
    TNum = _pst_Element->l_NbTriangles;
	if(!TNum) return 0;

	IDirect3DVertexBuffer9* m_VertexBuffer;
    m_pD3DDevice->CreateVertexBuffer( TNum * 3 * sizeof(DX9VertexFormat),
                                      D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT,
                                      &m_VertexBuffer, NULL );


	m_VertexBuffer->Lock(  0, TNum * sizeof(DX9VertexFormat) * 3, (void**) &pVertexBuffer, D3DLOCK_DISCARD );

	ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
//	ulOGLSetCol_Or = (GDI_gpst_CurDD->pv_SpecificData)->ulColorOr;
	if(pst_Color == NULL)
	{
		ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
		ulOGLSetCol ^= ulOGLSetCol_XOr;
		ulOGLSetCol = XeConvertColor(ulOGLSetCol);
	}

	// Fill
	t = _pst_Element->dst_Triangle;
	tend = _pst_Element->dst_Triangle + TNum;
	if(_pst_UV)
	{
		if(pst_Color)
		{
			if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
			{
				while(t < tend)
				{
					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[0]];
					DX8_SetColorRGBA(t->auw_Index[0]);
					pVertexBuffer->fU = _pst_UV[t->auw_Index[0]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_Index[0]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[1]];
					DX8_SetColorRGBA(t->auw_Index[1]);
					pVertexBuffer->fU = _pst_UV[t->auw_Index[1]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_Index[1]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[2]];
					DX8_SetColorRGBA(t->auw_Index[2]);
					pVertexBuffer->fU = _pst_UV[t->auw_Index[2]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_Index[2]].fV;
					pVertexBuffer++;

					t++;
				}
			}
			else
			{
				while(t < tend)
				{
					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[0]];
					DX8_SetColorRGBA(t->auw_Index[0]);
					pVertexBuffer->fU = _pst_UV[t->auw_UV[0]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_UV[0]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[1]];
					DX8_SetColorRGBA(t->auw_Index[1]);
					pVertexBuffer->fU = _pst_UV[t->auw_UV[1]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_UV[1]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[2]];
					DX8_SetColorRGBA(t->auw_Index[2]);
					pVertexBuffer->fU = _pst_UV[t->auw_UV[2]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_UV[2]].fV;
					pVertexBuffer++;
					t++;
				}
			}
		}
		else
		{
			if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
			{
				while(t < tend)
				{
					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[0]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer->fU = _pst_UV[t->auw_Index[0]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_Index[0]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[1]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer->fU = _pst_UV[t->auw_Index[1]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_Index[1]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[2]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer->fU = _pst_UV[t->auw_Index[2]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_Index[2]].fV;
					pVertexBuffer++;

					t++;
				}
			}
			else
			{
				while(t < tend)
				{
					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[0]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer->fU = _pst_UV[t->auw_UV[0]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_UV[0]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[1]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer->fU = _pst_UV[t->auw_UV[1]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_UV[1]].fV;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[2]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer->fU = _pst_UV[t->auw_UV[2]].fU;
					pVertexBuffer->fV = _pst_UV[t->auw_UV[2]].fV;
					pVertexBuffer++;

					t++;
				}
			}
		}
	}
	else
	{
		if(pst_Color)
		{
			while(t < tend)
			{
				*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[0]];
				DX8_SetColorRGBA(t->auw_Index[0]);
				pVertexBuffer++;

				*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[1]];
				DX8_SetColorRGBA(t->auw_Index[1]);
				pVertexBuffer++;

				*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[2]];
				DX8_SetColorRGBA(t->auw_Index[2]);
				pVertexBuffer++;

				t++;
			}
		}
		else
		{
			while(t < tend)
			{
				*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[0]];
				pVertexBuffer->Color = ulOGLSetCol;
				pVertexBuffer++;

				*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[1]];
				pVertexBuffer->Color = ulOGLSetCol;
				pVertexBuffer++;

				*(GEO_Vertex *) pVertexBuffer = _pst_Point[t->auw_Index[2]];
				pVertexBuffer->Color = ulOGLSetCol;
				pVertexBuffer++;

				t++;
			}
		}
	}

	// End fill
	m_VertexBuffer->Unlock;
	

    ULONG ulCustomVSBK = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetCustomVS();
    ULONG ulCustomPSBK = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetCustomPS();

	GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomPS(ulCustomVSBK);
    GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVS(ulCustomPSBK);

	m_pD3DDevice->SetStreamSource( 0, m_VertexBuffer, 0,sizeof(DX9VertexFormat) );
	//m_pD3DDevice->SetVertexShader( D3DFVF_VERTEXF);
    //m_pD3DDevice->SetVertexDeclaration( pVertexDecl );
	m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, TNum );
*/
	return 0;
}

LONG Xe_l_DrawElementIndexedSprites(GEO_tdst_ElementIndexedSprite   *_pst_Element,
                                    GEO_Vertex                      *_pst_Point,
                                    ULONG                           ulnumberOfPoints)
{
    MATH_tdst_Vector            XCam , YCam , *p_point;
    MATH_tdst_Vector            Sprite[5];
    float                       Size;
    GDI_tdst_DisplayData        *pst_DD;
    GEO_tdst_IndexedSprite      *p_Frst,*p_Last;
    ULONG                       ulOGLSetCol;
    ULONG                       *pst_Color;
    ULONG                       *pst_Alpha;
    float                       count;

    extern BOOL GDI_gb_WaveSprite;

    pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
    pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
    ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient;

    pst_DD = GDI_gpst_CurDD;

    XCam . x = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Ix * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
    XCam . y = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Jx * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
    XCam . z = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Kx * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
    YCam . x = pst_DD->st_MatrixStack.pst_CurrentMatrix->Iy * _pst_Element->fGlobalSize;
    YCam . y = pst_DD->st_MatrixStack.pst_CurrentMatrix->Jy * _pst_Element->fGlobalSize;
    YCam . z = pst_DD->st_MatrixStack.pst_CurrentMatrix->Ky * _pst_Element->fGlobalSize;

    pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_BeforeDrawSprite, _pst_Element->l_NbSprites);

    p_Frst = _pst_Element->dst_Sprite;
    p_Last = p_Frst + _pst_Element->l_NbSprites;
    if (p_Frst ->auw_Index == 0xC0DE) // GFX Signal of mega-flux (no indexes) , points ares SOFT_tdst_AVertexes with w = size
    {
        SOFT_tdst_AVertex *pS,*pSE;
        pS = (SOFT_tdst_AVertex *)_pst_Point;
        pS += p_Frst [1].auw_Index;
        pSE = pS + _pst_Element->l_NbSprites;
        if(pst_Color) pst_Color +=  p_Frst [1].auw_Index;
        if(pst_Alpha) pst_Alpha +=  p_Frst [1].auw_Index;
        while (pS < pSE)
        {
            if(pst_Color) 
            { 
                ulOGLSetCol = *pst_Color; 
                if(pst_Alpha) 
                { 
                    ulOGLSetCol &= 0x00ffffff; 
                    ulOGLSetCol |= pst_Alpha[p_Frst->auw_Index]; 
                } 
                pst_Color++;
            } 
            else if(pst_Alpha) 
            { 
                ulOGLSetCol &= 0x00ffffff; 
                ulOGLSetCol |= pst_Alpha[p_Frst->auw_Index]; 
            }

            *(ULONG *)&Sprite[4].x =  ulOGLSetCol;
            Size = pS->w;

            Sprite[0] . x = pS->x  + (- XCam.x - YCam.x) * Size;
            Sprite[0] . y = pS->y  + (- XCam.y - YCam.y) * Size;
            Sprite[0] . z = pS->z  + (- XCam.z - YCam.z) * Size;
            Sprite[1] . x = pS->x  + (+ XCam.x - YCam.x) * Size;
            Sprite[1] . y = pS->y  + (+ XCam.y - YCam.y) * Size;
            Sprite[1] . z = pS->z  + (+ XCam.z - YCam.z) * Size;
            Sprite[2] . x = pS->x  + (+ XCam.x + YCam.x) * Size;
            Sprite[2] . y = pS->y  + (+ XCam.y + YCam.y) * Size;
            Sprite[2] . z = pS->z  + (+ XCam.z + YCam.z) * Size;
            Sprite[3] . x = pS->x  + (- XCam.x + YCam.x) * Size;
            Sprite[3] . y = pS->y  + (- XCam.y + YCam.y) * Size;
            Sprite[3] . z = pS->z  + (- XCam.z + YCam.z) * Size;

            pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSpriteUV, (ULONG)Sprite );
            pS++;
        }
    } else
    {
        count = 0;
        while (p_Frst < p_Last)
        {
            count += 1.0f;
            if(pst_Color) 
            { 
                ulOGLSetCol = pst_Color[p_Frst->auw_Index]; 
                if(pst_Alpha) 
                { 
                    ulOGLSetCol &= 0x00ffffff; 
                    ulOGLSetCol |= pst_Alpha[p_Frst->auw_Index]; 
                } 
            } 
            else if(pst_Alpha) 
            { 
                ulOGLSetCol &= 0x00ffffff; 
                ulOGLSetCol |= pst_Alpha[p_Frst->auw_Index]; 
            }

            *(ULONG *)&Sprite[4].x =  ulOGLSetCol;
            p_point = _pst_Point + p_Frst->auw_Index;
            Size = *(float *)p_Frst;

            Sprite[0] . x = p_point->x  + (- XCam.x - YCam.x) * Size;
            Sprite[0] . y = p_point->y  + (- XCam.y - YCam.y) * Size;
            Sprite[0] . z = p_point->z  + (- XCam.z - YCam.z) * Size;
            Sprite[1] . x = p_point->x  + (+ XCam.x - YCam.x) * Size;
            Sprite[1] . y = p_point->y  + (+ XCam.y - YCam.y) * Size;
            Sprite[1] . z = p_point->z  + (+ XCam.z - YCam.z) * Size;
            Sprite[2] . x = p_point->x  + (+ XCam.x + YCam.x) * Size;
            Sprite[2] . y = p_point->y  + (+ XCam.y + YCam.y) * Size;
            Sprite[2] . z = p_point->z  + (+ XCam.z + YCam.z) * Size;
            Sprite[3] . x = p_point->x  + (- XCam.x + YCam.x) * Size;
            Sprite[3] . y = p_point->y  + (- XCam.y + YCam.y) * Size;
            Sprite[3] . z = p_point->z  + (- XCam.z + YCam.z) * Size;

            if(GDI_gb_WaveSprite)
            {
                extern float TIM_gf_MainClockReal;
                float ff, ff1;
                ff = fOptSin(count);
                ff1 = fOptSin(TIM_gf_MainClockReal * ff * 2.5f) * ff * 0.2f;
                Sprite[0] . x += ff1;
                Sprite[1] . x += ff1;
            }

            pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSpriteUV, (ULONG)Sprite );
            p_Frst++;
        }
    }
    pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_AfterDrawSprite, 0);
    return 0;
}

void Xe_SetFogParams(SOFT_tdst_FogParams* pFogParams)
{
    if( pFogParams->c_Flag & SOFT_C_FogActive &&
        (GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_Fogged) != 0 )
    {
        g_oFXManager.SetFogParams(TRUE,
            XeConvertColor(pFogParams->ul_Color),
            pFogParams->f_Start,
            pFogParams->f_End,
            pFogParams->f_Density,
            pFogParams->f_PitchAttenuationMin,
            pFogParams->f_PitchAttenuationMax,
            pFogParams->f_PitchAttenuationIntensity );
    }
    else
    {
        g_oFXManager.SetFogParams(FALSE);
    }
}

class XeMesh            *g_pSpriteMesh;
XeBuffer                *g_pSpriteBuffer;
XeRenderer::XeVertexDyn *g_pSpriteBufferArray=NULL; 

#ifdef _DEBUG
ULONG g_NbSpritesSent = 0;
#endif

void Xe_DrawSprite(MATH_tdst_Vector *v)
{
#ifdef _DEBUG
    if(!g_pSpriteMesh)
    {
        OutputDebugString("No spritelist currently assigned\n");
        return;
    }
#endif

    static float            tf_UV[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
    ULONG                   ul_Color;

    if(!g_pSpriteBufferArray) 
        return;

    ul_Color = *(ULONG *) &v[4].x;

    ul_Color = XeConvertColor(ul_Color);

    *(GEO_Vertex *) &g_pSpriteBufferArray->vPos = v[0];
    g_pSpriteBufferArray->ulColor = ul_Color;
    g_pSpriteBufferArray->UV.fU = tf_UV[3];
    g_pSpriteBufferArray->UV.fV = tf_UV[4];
    g_pSpriteBufferArray++;

    *(GEO_Vertex *) &g_pSpriteBufferArray->vPos = v[1];
    g_pSpriteBufferArray->ulColor = ul_Color;
    g_pSpriteBufferArray->UV.fU = tf_UV[2];
    g_pSpriteBufferArray->UV.fV = tf_UV[3];
    g_pSpriteBufferArray++;

    *(GEO_Vertex *) &g_pSpriteBufferArray->vPos = v[2];
    g_pSpriteBufferArray->ulColor = ul_Color;
    g_pSpriteBufferArray->UV.fU = tf_UV[1];
    g_pSpriteBufferArray->UV.fV = tf_UV[2];
    g_pSpriteBufferArray++;

    *(GEO_Vertex *) &g_pSpriteBufferArray->vPos = v[3];
    g_pSpriteBufferArray->ulColor = ul_Color;
    g_pSpriteBufferArray->UV.fU = tf_UV[0];
    g_pSpriteBufferArray->UV.fV = tf_UV[1];
    g_pSpriteBufferArray++;

#ifdef _DEBUG
    g_NbSpritesSent++;
#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void Xe_BeforeDrawSprite(ULONG _ul_NbSprites)
{
#ifdef _DEBUG
    if(g_pSpriteMesh)
    {
        OutputDebugString("Starting spritelist without finishing previous one\n");
    }
#endif

    g_pSpriteMesh = g_oXeRenderer.RequestDynamicMesh();

#ifdef _DEBUG
    if(!g_pSpriteMesh)
    {
        OutputDebugString("Unable to create a spritelist mesh object\n");
        return;
    }
#endif

    int iSizeOfElements = sizeof(XeRenderer::XeVertexDyn);
    g_pSpriteBuffer = (XeBuffer*)g_pSpriteMesh->GetStream(0)->pBuffer;
    g_pSpriteMesh->SetStreamComponents(0, XEVC_POSITION | XEVC_COLOR0 | XEVC_TEXCOORD0);

    g_pSpriteBufferArray = (XeRenderer::XeVertexDyn *)g_pSpriteBuffer->Lock(_ul_NbSprites*4, iSizeOfElements);

#ifdef _DEBUG
    g_NbSpritesSent = 0;
#endif

}

/*
=======================================================================================================================
=======================================================================================================================
*/
void Xe_AfterDrawSprite()
{
#ifdef _DEBUG
    if(!g_pSpriteMesh)
    {
        OutputDebugString("No spritelist currently assigned\n");
        return;
    }

    if(g_NbSpritesSent != (g_pSpriteBuffer->GetVertexCount() / 4))
    {
        OutputDebugString("Incorrect number of sprites sent\n");
    }
#endif

    g_pSpriteBuffer->Unlock();

    if(!GDI_gpst_CurDD->pst_CurrentMLTTXLVL)
    {
        OutputDebugString("sprite with no material\n");
        return;
    }

    eXeRENDERLISTTYPE eRT = XeRT_OPAQUE;
    if (((MAT_GET_Blending(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags) != MAT_Cc_Op_Copy) && (MAT_GET_Blending(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags) != MAT_Cc_Op_Glow)) || (GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags & MAT_Cul_Flag_HideColor))
        eRT = XeRT_TRANSPARENT;

    g_oXeRenderer.QueueMeshForRender(GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix,
        g_pSpriteMesh,
        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial,
        (GDI_gpst_CurDD->ul_CurrentDrawMask & ~GDI_Cul_DM_Lighted), // never lighted
        -1,
        eRT,
        XeRenderObject::QuadList,
        NULL,
        GDI_gpst_CurDD->g_cZListCurrentDisplayOrder,
        (LPVOID)ROS_SPRITES,
        XeRenderObject::Sprites);

    g_pSpriteMesh = NULL;
}

LONG Xe_l_Request(ULONG _ulRequest, ULONG _ulData)
{
    switch (_ulRequest)
    {
    case GDI_Cul_Request_SetFogParams:
        Xe_SetFogParams((SOFT_tdst_FogParams*)_ulData);
        break;

    case GDI_Cul_Request_ReloadPalette:
        //            g_XbTextureMgr.ReloadPalette(_ulData, TEX_List_GetPalette(_ulData));
        break;

    case GDI_Cul_Request_BeforeDrawSprite:
        Xe_BeforeDrawSprite(_ulData);
        break;

    case GDI_Cul_Request_DrawSprite:
    case GDI_Cul_Request_DrawSpriteUV:
        Xe_DrawSprite((MATH_tdst_Vector *) _ulData );
        break;

    case GDI_Cul_Request_AfterDrawSprite:
        Xe_AfterDrawSprite();
        break;

    case GDI_Cul_Request_PushZBuffer:
        break;

#if defined(ACTIVE_EDITORS)

    case GDI_Cul_Request_NumberOfTextures:
        // Return 1 if the texture count changed
        break;

    case GDI_Cul_Request_DepthTest:
        // Enable|Disable depth texting
        break;

    case GDI_Cul_Request_LoadInterfaceTex:
    case GDI_Cul_Request_UnloadInterfaceTex:
        // Load|Unload the interface textures
        break;

    case GDI_Cul_Request_DepthFunc:
        // Change the depth function (1 = LessEqual, 0 = Always)
        break;

    case GDI_Cul_Request_Enable:
        Xe_GDIRequest(_ulData, TRUE);
        break;

    case GDI_Cul_Request_Disable:
        Xe_GDIRequest(_ulData, FALSE);
        break;

    case GDI_Cul_Request_DrawSoftArrow:
        Xe_GDIDrawSoftArrow((SOFT_tdst_Arrow*)_ulData);
        break;

    case GDI_Cul_Request_DrawSoftEllipse:
        Xe_GDIDrawSoftEllipse((SOFT_tdst_Ellipse*)_ulData);
        break;

    case GDI_Cul_Request_DrawSoftSquare:
        Xe_GDIDrawSoftSquare((SOFT_tdst_Square*)_ulData);
        break;

    case GDI_Cul_Request_PolygonOffset:
        // SC: Set the polygon offset - No need right now
        break;

    case GDI_Cul_Request_EnableFog:
        // SC: Enable|Disable fog
        break;

    case GDI_Cul_Request_DrawPointEx:
        Xe_GDIDrawPointEx((GDI_tdst_Request_DrawPointEx*)_ulData);
        break;

    case GDI_Cul_Request_DrawLineEx:
        Xe_GDIDrawLineEx((GDI_tdst_Request_DrawLineEx*)_ulData);
        break;

    case GDI_Cul_Request_DrawLine:
        Xe_GDIDrawLine((MATH_tdst_Vector**)_ulData);
        break;

    case GDI_Cul_Request_DrawQuad:
        Xe_GDIDrawQuad((MATH_tdst_Vector**)_ulData);
        break;

    case GDI_Cul_Request_DrawQuadEx:
        Xe_GDIDrawQuadEx((GDI_tdst_Request_DrawQuad*)_ulData);
        break;

#if defined(SC_DEV)
        // SC: TODO:
    case GDI_Cul_Request_DrawTransformedTriangle:   XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawPoint:                 XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawTriangle:              XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_ReloadTexture:             XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_SizeOfPoints:              XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_SizeOfLine:                XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawPointSize:             XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawPointMin:              XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_Draw2DTriangle:            XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_ReadScreen:                XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_ReadPixel:                 XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_GetInterfaceTexBuffer:     XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_TextureUnloadCompare:      XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_Display169BlackBand:       XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_ReloadTexture2:            XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawBox:                   XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawSphere:                XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawCone:                  XeValidate(FALSE, "Not supported"); break;
    case GDI_Cul_Request_DrawCylinder:              XeValidate(FALSE, "Not supported"); break;
#endif // defined(SC_DEV)

#endif // defined(ACTIVE_EDITORS)

    default:
        //            ERR_OutputDebugString("\nWARNING: Xbox_l_Request(): Unsupported request id (0x%08x)\n", _ulRequest);
        break;
    }

    return 0;
}

void Xe_SetTextureBlending( ULONG _l_Texture, ULONG BM, unsigned short s_AditionalFlags )
{
    int a = 0;
}

LONG Xe_l_Texture_Init(struct GDI_tdst_DisplayData_ *, ULONG _ul_TextureCount)
{
    g_oXeTextureMgr.Init2DTextures(_ul_TextureCount);

    return 1;
}

void Xe_Texture_Resize(ULONG _ul_NewTextureCount)
{
    g_oXeTextureMgr.Resize2DTextures(_ul_NewTextureCount);
}

void Xe_SetAnimatedTexture(LONG _l_TextureId, LONG _l_CurrentAnimTextureId)
{
    g_oXeTextureMgr.SetAnimatedTexture((ULONG)_l_TextureId, (ULONG)_l_CurrentAnimTextureId);
}

void Xe_BeforeDisplay()
{
    g_oXeRenderer.BeforeDisplay();
}

void Xe_AfterDisplay()
{
    g_oXeRenderer.AfterDisplay();
}

//----------------------------------------------------------------------@FHB--
// Xe_Texture_Load
// 
// Role   : Load a texture in video memory
//          
// Params : _pst_DD : 
//          _pst_TexData : 
//          _pst_Tex : 
//          _ul_Texture : 
//          
// Return : void
// 
// Author : Eric Le
// Date   : 3 Jan 2005
//----------------------------------------------------------------------@FHE--
void Xe_Texture_Load
(
 GDI_tdst_DisplayData    *_pst_DD,
 TEX_tdst_Data           *_pst_TexData,
 TEX_tdst_File_Desc      *_pFileDesc,
 ULONG                   _ul_Texture
 )
{
    DWORD     dwWidth         = _pFileDesc->uw_Width;
    DWORD     dwHeight        = _pFileDesc->uw_Height;
    DWORD     dwSize          = 0;
    DWORD     dwNbLevels      = 1;
    D3DFORMAT eSrcFormat      = D3DFMT_UNKNOWN;
    D3DFORMAT eDstFormat      = D3DFMT_A8R8G8B8;
    DWORD     dwBytesPerPixel = 0;
    bool      bIsNormalMap    = false;

    _pst_TexData->uw_Flags = _pFileDesc->uw_FileFlags;

#if defined(ACTIVE_EDITORS)
	if (LOA_IsBinarizing() || EDI_gb_XeQuickLoad || EDI_gb_ComputeMap || EDI_gb_SlashXPK || EDI_gb_SlashLXPK)
		return;
#endif

    // Conversion needed?
    if (_pFileDesc->w_TexFlags & TEX_XenonConvert)
    {
        // Use the output properties specified in the texture file information
        dwWidth      = _pFileDesc->st_Tex.st_XeProperties.ul_OutputWidth;
        dwHeight     = _pFileDesc->st_Tex.st_XeProperties.ul_OutputHeight;
        dwNbLevels   = _pFileDesc->st_Tex.st_XeProperties.ul_NbLevels;
        eDstFormat   = TEX_XeConvertJadeXTFToD3DFORMAT(_pFileDesc->st_Tex.st_XeProperties.ul_OutputFormat);
        dwSize       = _pFileDesc->st_Tex.st_XeProperties.ul_FileSize;
        bIsNormalMap = ((_pFileDesc->st_Tex.st_XeProperties.ul_Flags & TEX_Xe_IsNormalMap) != 0);

#if defined(_XENON)
        // Let's force DXT5 compression for uncompressed textures (memory savings)
        if (_pFileDesc->st_Tex.st_XeProperties.ul_OutputFormat == TEX_XTF_AUTO)
        {
            eDstFormat = D3DFMT_DXT5;
        }
#endif

        ULONG ulHeader = *(ULONG*)_pFileDesc->p_Bitmap;
#if !defined(_XENON)
        SwapDWord(&ulHeader);
#endif
        if (ulHeader == XE_TEXTURE_2D)
        {
            _pFileDesc->w_TexFlags &= ~TEX_XenonConvert;
        }
    }

    if (!(_pFileDesc->w_TexFlags & TEX_XenonConvert))
    {
        switch( _pFileDesc->uc_FinalBPP )
        {
        case 32:
            dwBytesPerPixel = 4;
            eSrcFormat      = D3DFMT_LIN_A8R8G8B8;
            dwSize          = dwWidth * dwHeight * 4;
            break;

        case 24:
            dwBytesPerPixel = 4;
            eSrcFormat      = D3DFMT_LIN_X8R8G8B8;
            break;

        case 8:
            eSrcFormat = D3DFMT_PAL8;
            break;

        case 4:
            eSrcFormat = D3DFMT_PAL4;
            break;

        case 0x80:
            eSrcFormat = D3DFMT_DDS;
            dwSize     = _pFileDesc->st_Tex.st_XeProperties.ul_FileSize;
            break;

        default:
            // unknown texture depth.
            break;
        }
    }

    g_oXeTextureMgr.Create2DTexture(_ul_Texture, dwWidth, dwHeight, dwNbLevels, eSrcFormat, 
                                    _pFileDesc->p_Bitmap, dwSize, eDstFormat, 
                                    (_pFileDesc->st_Params.uw_Flags & TEX_FP_Interface) == 0,
                                    bIsNormalMap);
}

LONG Xe_l_Texture_Store(struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Data_ *, struct TEX_tdst_File_Desc_ *, ULONG)
{
    return 0;
}

void Xe_Palette_Load(struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Palette_ *, ULONG)
{
    int a = 0;
}

void Xe_Set_Texture_Palette(struct GDI_tdst_DisplayData_ *_pst_DD, ULONG _ulTexNum, ULONG IT, ULONG IP)
{
    // Palette 
    if (IP == 0xffffffff)
    { 
        TEX_gst_GlobalList.dst_Texture[IT].uw_Flags |= TEX_uw_Shared;
        g_oXeTextureMgr.ShareTexture(IT, _ulTexNum);
        return;
    }

#if defined(_XENON_RENDER_PC)
    g_oXeTextureMgr.SetPalette(_ulTexNum, IT, TEX_gst_GlobalList.dst_Palette[IP].pul_Color);
#endif
}

void Xe_Texture_Unload(struct GDI_tdst_DisplayData_ *_pst_DD)
{
    g_oXeTextureMgr.Unload2DTexturesAndCubeMaps();
}

void Xe_SetTextureTarget(ULONG NumShadowTex , ULONG CLEAR)
{
    int a = 0;
}

void Xe_SetViewMatrix_SDW(struct MATH_tdst_Matrix_ *_pst_Matrix , float *Limits)
{
    int a = 0;
}

void Xe_InvalidateRenderLists(void)
{
#if defined(_XENON_RENDERER_USETHREAD)
    g_oXeRenderer.WaitForEventComplete(eXeThreadEvent_Flip);
#endif

    g_oXeRenderer.InvalidateRenderLists();

    // Clear requests in both contexts of the light shaft manager
    g_oXeLightShaftManager.ClearRequests();
}

// Disable global optimization for the following function as it introduces a shadow/lighting bug in map 07A_p1
#pragma optimize("g", off)
ULONG Xe_AddLight(LIGHT_tdst_Light* _poLight, BOOL bAddToLightToSet )
{
    BOOL  bIsLightShaft = FALSE;
    ULONG ulLightType;

    if (_poLight == NULL)
    {
        // too many lights
        ERR_X_Assert(FALSE);
        return 0;
    }

    // find light type
    switch(_poLight->ul_Flags & LIGHT_Cul_LF_Type)
    {
    case LIGHT_Cul_LF_Omni:
        ulLightType = LIGHT_TYPE_OMNI;
        break;
    case LIGHT_Cul_LF_Direct:        
        ulLightType = LIGHT_TYPE_DIRECT;
        break;
    case LIGHT_Cul_LF_Spot:
        ulLightType = (_poLight->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical) ? LIGHT_TYPE_CYLINDRICAL_SPOT : LIGHT_TYPE_SPOT;
        break;
    case LIGHT_Cul_LF_LightShaft:
        bIsLightShaft = TRUE;
        ulLightType   = LIGHT_TYPE_SPOT;
        break;
    default:
        ulLightType = LIGHT_TYPE_NONE;
        break;
    }

    XeLight *poNewLight = g_pXeContextManagerEngine->PushLight(ulLightType, bAddToLightToSet);
    if (poNewLight == NULL)
    {
        // too many lights
        ERR_X_Assert(FALSE);
        return 0;
    }

    poNewLight->JadeLight = _poLight;
    D3DXMATRIX* poCurrentWorldViewMatrix = g_pXeContextManagerEngine->GetCurrentWorldViewMatrix();
    ERR_X_Assert(poCurrentWorldViewMatrix != NULL);

    // save general params
    poNewLight->RLIBlendingScale    = _poLight->f_RLIBlendingScale;
    poNewLight->RLIBlendingOffset   = _poLight->f_RLIBlendingOffset;
    poNewLight->DiffuseMultiplier   = _poLight->f_DiffuseMultiplier;
    poNewLight->SpecularMultiplier  = _poLight->f_SpecularMultiplier;
    poNewLight->ActorDiffusePonderator   = _poLight->f_ActorDiffusePonderator;
    poNewLight->ActorSpecularPonderator  = _poLight->f_ActorSpecularPonderator;
    poNewLight->IsRimLight          = (_poLight->ul_Flags & LIGHT_Cul_LF_RimLight) != 0;
    poNewLight->IsLM                = (_poLight->ul_Flags & LIGHT_Cul_LF_UseLightMap) != 0;
    poNewLight->IsInverted          = (_poLight->ul_Flags & LIGHT_Cul_LF_Inverted) != 0;

    if (ulLightType != LIGHT_TYPE_DIRECT)
    {
        FLOAT fScale = 1.0f;
        if(GDI_gpst_CurDD->pst_CurrentGameObject)
        {
            if(MATH_b_TestScaleType(GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix) && GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix->Sx>0.0f)
                fScale = 1.0f/GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix->Sx;
        }

        // set position
        poNewLight->position.x = LIGHT_gst_LP.T.x;
        poNewLight->position.y = LIGHT_gst_LP.T.y;
        poNewLight->position.z = LIGHT_gst_LP.T.z;
        poNewLight->position.w = poNewLight->IsInverted ? -1.0f : 1.0f;

        if (bIsLightShaft)
        {
            FLOAT fMinFOV = 0.5f * _poLight->st_LightShaft.f_SpotInnerAngle;
            FLOAT fMaxFOV = 0.5f * _poLight->st_LightShaft.f_SpotOuterAngle;

            fMinFOV = MATH_f_FloatLimit(fMinFOV, 0.0f, 180.0f);
            fMaxFOV = MATH_f_FloatLimit(fMaxFOV, fMinFOV, 180.0f);

            poNewLight->params.x = _poLight->st_LightShaft.f_Start*fScale + _poLight->st_LightShaft.f_Length*fScale;
            poNewLight->params.y = 1.0f / _poLight->st_LightShaft.f_Length*fScale;
            poNewLight->params.z = fOptCos(Cf_PiBy180 * fMaxFOV);
            if (fMinFOV < fMaxFOV)
            {
                poNewLight->params.w = 1.0f / (fOptCos(Cf_PiBy180 * fMinFOV) - poNewLight->params.z);
            }
            else
            {
                poNewLight->params.w = FLT_MAX;
            }
        }
        else
        {
            // set optional params
            poNewLight->params.x = _poLight->st_Direct.f_Far*fScale;

            if (_poLight->st_Direct.f_Near != _poLight->st_Direct.f_Far)
            {
                poNewLight->params.y = 1.0f / (_poLight->st_Direct.f_Far*fScale - _poLight->st_Direct.f_Near*fScale);
            }
            else
            {
                poNewLight->params.y = FLT_MAX;
            }

            if (ulLightType == LIGHT_TYPE_CYLINDRICAL_SPOT)
            {
                FLOAT fLittleAlpha, fBigAlpha;
                poNewLight->params.z = fLittleAlpha = _poLight->st_Spot.f_Near*fScale * fTan(_poLight->st_Spot.f_LittleAlpha);
                fBigAlpha = _poLight->st_Spot.f_Far*fScale * fTan(_poLight->st_Spot.f_BigAlpha);

                if( fLittleAlpha != fBigAlpha)
                {
                    poNewLight->params.w = 1.0f / (fBigAlpha - fLittleAlpha);
                }
                else
                {
                    poNewLight->params.w = FLT_MAX;
                }
            }
            else if (ulLightType == LIGHT_TYPE_SPOT)
            {
                poNewLight->params.z = fOptCos(_poLight->st_Spot.f_BigAlpha);

                if( _poLight->st_Spot.f_LittleAlpha != _poLight->st_Spot.f_BigAlpha)
                {
                    poNewLight->params.w = 1.0f / (fOptCos(_poLight->st_Spot.f_LittleAlpha) - fOptCos(_poLight->st_Spot.f_BigAlpha));
                }
                else
                {
                    poNewLight->params.w = FLT_MAX;
                }
            }
        }
    }

    // omni lights don't have a direction
    if (ulLightType != LIGHT_TYPE_OMNI)
    {
        // set light direction
        poNewLight->direction.x = MATH_pst_GetYAxis(&LIGHT_gst_LP)->x;
        poNewLight->direction.y = MATH_pst_GetYAxis(&LIGHT_gst_LP)->y;
        poNewLight->direction.z = MATH_pst_GetYAxis(&LIGHT_gst_LP)->z;
        poNewLight->direction.w = 0.0f;
    }

    // set light color
    D3DXCOLOR oColor(XeConvertColor(LIGHT_gul_Color));
    poNewLight->color.x = oColor.r;
    poNewLight->color.y = oColor.g;
    poNewLight->color.z = oColor.b;
    poNewLight->color.w = 1.0f;

    FLOAT fLightningIntensity = g_oXeWeatherManager.GetLightningIntensity();

    // Specular boost (Rain increases specular gradually.. artists can also boost without rain effect)
    if (_poLight->b_RainEffect && fLightningIntensity <= 0.0f) // only when no lightning is in effect
    {
        float specularBoost;
        float diffuseFactor;
        g_oXeWeatherManager.GetRainEffectFactors(GDI_gpst_CurDD->pst_World->f_XeDrySpecularBoost, 
            GDI_gpst_CurDD->pst_World->f_XeWetSpecularBoost, 
            GDI_gpst_CurDD->pst_World->f_XeDryDiffuseFactor, 
            GDI_gpst_CurDD->pst_World->f_XeWetDiffuseFactor, 
            GDI_gpst_CurDD->pst_World->f_XeRainEffectDelay, 
            GDI_gpst_CurDD->pst_World->f_XeRainEffectDryDelay, 
            &specularBoost, 
            &diffuseFactor);
        poNewLight->SpecularMultiplier *= (1.0f + specularBoost);
        poNewLight->DiffuseMultiplier  *= (1.0f + diffuseFactor);
    }

    // Return the index of the new light in the appropriate stack
    switch( ulLightType )
    {
    case LIGHT_TYPE_OMNI:
        return g_pXeContextManagerEngine->GetNextOmniLightIndex()-1;
    case LIGHT_TYPE_DIRECT:
        return g_pXeContextManagerEngine->GetNextDirLightIndex()-1;
    case LIGHT_TYPE_SPOT:
        return g_pXeContextManagerEngine->GetNextSpotLightIndex()-1;
    case LIGHT_TYPE_CYLINDRICAL_SPOT:
        return g_pXeContextManagerEngine->GetNextCylSpotLightIndex()-1;
    }

    // Lightnings
    if (fLightningIntensity > 0.0f)
    {
        poNewLight->DiffuseMultiplier  += fLightningIntensity * 8.0f;
        poNewLight->SpecularMultiplier += fLightningIntensity * 8.0f;
    }

    return 0;
}
#pragma optimize("", on)

void Xe_AddShadowLight(OBJ_tdst_GameObject * _poLightGAO)
{
    LIGHT_tdst_Light* poLight = (LIGHT_tdst_Light*)_poLightGAO->pst_Extended->pst_Light;

    // Init values common to all light types
    XeShadowLight oLight;
    MATH_tdst_Matrix mtx;
    float fDeterminant;

    oLight.JadeLight = (void*)poLight;

    oLight.ShadowNear = poLight->f_ShadowNear;

    // Artists asked to use the light's far instead of having a separate far for shadows
    //oLight.ShadowFar = poLight->f_ShadowFar;

    MATH_MakeOGLMatrix(&mtx, _poLightGAO->pst_GlobalMatrix);

    if ((poLight->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_LightShaft)
    {
        D3DXMATRIX mTrans;
        D3DXMatrixTranslation(&mTrans, 0.0f, 0.0f, poLight->st_LightShaft.f_Start);
        MATH_RotateMatrix_AroundLocalXAxis(&mtx, -Cf_PiBy2);
        D3DXMatrixMultiply((D3DXMATRIX*)&mtx, &mTrans, (D3DXMATRIX*)&mtx);
    }
    else
    {
        MATH_RotateMatrix_AroundLocalXAxis(&mtx, -Cf_PiBy2);
    }
    D3DXMatrixInverse(&oLight.View, &fDeterminant, (D3DXMATRIX*)&mtx);

    D3DXMATRIX* ProjMatrix = g_pXeContextManagerEngine->GetCurrentProjectionMatrix();
    D3DXMatrixInverse(&oLight.InvCameraProj, &fDeterminant, g_pXeContextManagerEngine->GetCurrentProjectionMatrix());

    MATH_tdst_Matrix temp;
    MATH_MulMatrixMatrix(&temp, _poLightGAO->pst_GlobalMatrix, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);

    DWORD dwLightType = poLight->ul_Flags & LIGHT_Cul_LF_Type;
    switch(dwLightType)
    {
    case LIGHT_Cul_LF_Spot:
        {
            oLight.ShadowFar = poLight->st_Spot.f_Far;
            oLight.Umbra = poLight->st_Spot.f_LittleAlpha * 2.0f;
            oLight.Penumbra = poLight->st_Spot.f_BigAlpha * 2.0f;
            oLight.HiResShadowFOV = poLight->f_HiResFOV * 2.0f;

            if(poLight->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
            {
                oLight.LightType = LIGHT_TYPE_CYLINDRICAL_SPOT;
                oLight.CylinderRadius = poLight->st_Spot.f_Far * fTan(poLight->st_Spot.f_BigAlpha);
                FLOAT fSize = 2.0f * oLight.CylinderRadius;

                D3DXMatrixOrthoRH(&oLight.Projection[0], fSize, fSize, oLight.ShadowNear, oLight.ShadowFar);

                // Calculate 2nd proj matrix for character shadows
                if(poLight->b_UseHiResFOV)
                {
                    fSize = 2.0f * poLight->st_Spot.f_Far * fTan(oLight.HiResShadowFOV / 2.0f);
                    D3DXMatrixOrthoRH(&oLight.Projection[1], fSize, fSize, oLight.ShadowNear, oLight.ShadowFar);
                }
                else
                {
                    oLight.Projection[1] = oLight.Projection[0];
                }
            }
            else
            {
                oLight.LightType = LIGHT_TYPE_SPOT;
                D3DXMatrixPerspectiveFovRH(&oLight.Projection[0], oLight.Penumbra, 1.0f, oLight.ShadowNear, oLight.ShadowFar);
                if(poLight->b_UseHiResFOV)
                {
                    D3DXMatrixPerspectiveFovRH(&oLight.Projection[1], oLight.HiResShadowFOV, 1.0f, oLight.ShadowNear, oLight.ShadowFar);
                }
                else
                {
                    oLight.Projection[1] = oLight.Projection[0];
                }
            }
            break;
        }

    case LIGHT_Cul_LF_LightShaft:
        {
            oLight.LightType    = LIGHT_TYPE_NONE;
            oLight.ShadowNear   = poLight->st_LightShaft.f_Start;
            oLight.ShadowFar    = poLight->st_LightShaft.f_Start + poLight->st_LightShaft.f_Length;
            oLight.Umbra        = Cf_PiBy180 * poLight->st_LightShaft.f_SpotInnerAngle;
            oLight.Penumbra     = Cf_PiBy180 * poLight->st_LightShaft.f_SpotOuterAngle;
            D3DXMatrixPerspectiveFovRH(&oLight.Projection[0], oLight.Penumbra, 1.0f, oLight.ShadowNear, oLight.ShadowFar);
        }
        break;

    case LIGHT_Cul_LF_Direct:
        {
            oLight.ShadowFar = poLight->st_Direct.f_Far;
            oLight.LightType = LIGHT_TYPE_DIRECT;
            static float XE_DIRECTIONAL_SIZE = 20.0f;
            D3DXMatrixOrthoRH(&oLight.Projection[0], XE_DIRECTIONAL_SIZE, XE_DIRECTIONAL_SIZE, oLight.ShadowNear, oLight.ShadowFar);

            break;
        }

    case LIGHT_Cul_LF_Omni:
        {
            //ERR_X_Warning(0, "Omni lights that cast shadows are not supported!", NULL);

            return;
        }
    }

    if(poLight->b_UseAmbientAsColor)
    {
        ULONG ulAmbient = GDI_gpst_CurDD->pst_World->ul_AmbientColor;
        oLight.Color.x = ((ulAmbient >>  0) & 0xFF) / 255.0f;
        oLight.Color.y = ((ulAmbient >>  8) & 0xFF) / 255.0f;
        oLight.Color.z = ((ulAmbient >> 16) & 0xFF) / 255.0f;
        oLight.Color.w = ((ulAmbient >> 24) & 0xFF) / 255.0f;
    }
    else
    {
        oLight.Color.x = ((poLight->ul_ShadowColor >>  0) & 0xFF) / 255.0f;
        oLight.Color.y = ((poLight->ul_ShadowColor >>  8) & 0xFF) / 255.0f;
        oLight.Color.z = ((poLight->ul_ShadowColor >> 16) & 0xFF) / 255.0f;
        oLight.Color.w = ((poLight->ul_ShadowColor >> 24) & 0xFF) / 255.0f;
    }

    oLight.NumIterations = min(poLight->ul_NumIterations, (ULONG)g_oXeRenderer.GetMaxShadowIter());
    oLight.NumIterations = max(oLight.NumIterations, 2);

    oLight.FilterSize = poLight->f_FilterSize;
    oLight.ZOffset = poLight->f_ZOffset;
    oLight.CookieTexureHandle = poLight->us_CookieTexture;

    // Calculate the light's priority used for selection of shadow lights
    float fDistance = 0.0f;
    float fAttenuation = 1.0f;

    // Compute distance
    if((poLight->ul_Flags & LIGHT_Cul_LF_Type) != LIGHT_Cul_LF_Direct)
    {
        fDistance = MATH_f_Distance(&_poLightGAO->pst_GlobalMatrix->T, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
    }

    // Compute luminance
    ULONG ul_Color = poLight->ul_Color;
    FLOAT r = ((float)(ul_Color & 0xFF)) / 255.0f;
    FLOAT g = ((float)((ul_Color>>8) & 0xFF)) /255.0f;
    FLOAT b = ((float)((ul_Color>>16) & 0xFF)) / 255.0f;
    FLOAT fLuminance = (r * 0.3f) + (g * 0.59f) + (b * 0.11f);

    // Compute attenuation (base on distance calculated above)
    if((poLight->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Spot)
    {
        float fNear = poLight->st_Spot.f_Near;
        float fFar = poLight->st_Spot.f_Far;
        if( fDistance > fFar )
        {
            fAttenuation = 0.0f;
        }
        else if (fDistance > fNear)
        {
            fAttenuation = (fFar - fDistance) / (fFar - fNear);
        }
    }

    oLight.Priority = poLight->f_DiffuseMultiplier * fLuminance * fAttenuation;

    if(!(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseMapLightSettings))
        oLight.Priority *= poLight->f_ActorDiffusePonderator;

    // Lightnings
    FLOAT fLightningIntensity = g_oXeWeatherManager.GetLightningIntensity();
    if (fLightningIntensity > 0.0f)
    {
        // Make the shadow more opaque when there is a lightning
        oLight.Color.x = MATH_f_FloatBlend(oLight.Color.x, 0.25f * oLight.Color.x, fLightningIntensity);
        oLight.Color.y = MATH_f_FloatBlend(oLight.Color.y, 0.25f * oLight.Color.y, fLightningIntensity);
        oLight.Color.z = MATH_f_FloatBlend(oLight.Color.z, 0.25f * oLight.Color.z, fLightningIntensity);
        oLight.Color.w = MATH_f_FloatBlend(oLight.Color.w, 0.25f * oLight.Color.w, fLightningIntensity);

        // Reduce attenuation
        oLight.ShadowFar += fLightningIntensity * 20.0f;
    }

    oLight.ForceStaticReceiver = poLight->b_ForceStaticReceiver;

    g_oXeShadowManager.AddLight(&oLight);
}

/**********************************************************************************************************************/
/* SPG2 specific functions BEGIN **************************************************************************************/
/**********************************************************************************************************************/
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"

#define CosAlpha -0.34202014332566873304409961468226f 
#define SinAlpha 0.9396926207859083840541092773247f 

#if defined(_XENON_RENDER_PC)
extern void OGL_l_DrawSPG2( SPG2_CachedPrimitivs                *pCachedLine,
                           ULONG                                *ulTextureID,
                           GEO_Vertex                           *XCam,
                           GEO_Vertex                           *YCam,
                           SPG2_tdst_Modifier                   *_pst_SPG2,
                           SOFT_tdst_AVertex                    *p_stWind,
                           SPG2_InstanceInforamtion         *p_II
                           );
#endif

struct XeVertexSPG2
{
    struct _vPosv0 // stored in position
    {
        FLOAT fX; FLOAT fY; FLOAT fZ; FLOAT fW;
    } vPosv0;

    ULONG ulColor;

    struct _Extra   // stored in texcoord0
    {
        FLOAT fSegmentIdx; 
        FLOAT fPrimitiveIdx;
    } Extra;

    struct _vPosv2  // stored in weights
    {
#if 0//defined(_XE_COMPRESS_NORMALS)
        ULONG ulWeight; // DEC3N
#else
        FLOAT fX; FLOAT fY; FLOAT fZ; FLOAT fW;
#endif
    } vPosv2;

    struct _vNrm    // stored in normals
    {
#if 0//defined(_XE_COMPRESS_NORMALS)
        ULONG ulNrm; // DEC3N
#else
        FLOAT fX; FLOAT fY; FLOAT fZ;
#endif
    } vNrm;

    struct _vPosv1 // stored in tangents
    {
#if 0//defined(_XE_COMPRESS_NORMALS)
        ULONG ulTangent; // DEC3N
#else
        FLOAT fX; FLOAT fY; FLOAT fZ; FLOAT fW;
#endif
    } vPosv1;
};

XeVertexSPG2 *g_pSPG2BufferArray=NULL; 

typedef struct _vCam
{
    FLOAT fX; FLOAT fY; FLOAT fZ;
} vCam;

struct XeVertexSPG2Helicoidal
{
    // stored in texcoord 1 UV and texcoord 2 U
    vCam vCamX;

    // stored in texcoord 2 VWX
    vCam vCamY;
};

XeVertexSPG2Helicoidal *g_pSPG2HelicoidalBufferArray=NULL; 

void Xe_l_DrawSPG2_2X(
                      XeMesh                            **pMesh,
                      XeBuffer                          **pVB,
                      SOFT_tdst_AVertex                 *Coordinates,
                      GEO_Vertex                        *XCamera,
                      GEO_Vertex                        *YCamera,
                      ULONG                             *pColors,
                      ULONG                             ulnumberOfPoints,
                      ULONG                             ulNumberOfSegments,
                      float                             fTrapeze,
                      float                             fEOHP,
                      float                             fRatio,
                      ULONG                             TileNumber,
                      ULONG                             ulMode,
                      SOFT_tdst_AVertex                 *pWind,
                      SPG2_InstanceInforamtion          *p_stII,
					  BOOL								bSpecialLookAtX,
                      SPG2_tdst_Modifier                *_pst_SPG2,
                      SPG2_Instance     				*pCachedLine
                      )
{
    ULONG BM = 0;
    float DeltaU;
    float fTrapezeDelta,fOoNumOfSeg;
    ULONG Counter;
    ULONG ulNbPoints = ulnumberOfPoints;

    if(!ulnumberOfPoints || !ulNumberOfSegments)
        return;

    // AI generated spg2 use just one element and spawns it where it wants
    if(_pst_SPG2->ulFlags & SPG2_IAGenerator)
    {
        ulnumberOfPoints = 1;
        Coordinates[0].x = Coordinates[0].y = Coordinates[0].z = 0.0f;
        Coordinates[2].w = 8.0f;
    }

    if(ulNumberOfSegments==1)
        g_oXeRenderer.m_uiNumSPG2 += (ulnumberOfPoints*4);
    else
        g_oXeRenderer.m_uiNumSPG2 += ((ulnumberOfPoints*(ulNumberOfSegments+1)*2)) +( ulNumberOfSegments*2) - 3;


    fOoNumOfSeg = 1.0f / (float)ulNumberOfSegments;
    fTrapezeDelta = -fTrapeze * fOoNumOfSeg;
    DeltaU = (float)TileNumber * fOoNumOfSeg;

    if (ulMode == 2) // DrawH
    {
        fTrapeze = 0.0f;
    }

    VECTOR4FLOAT vUVManip;
    ULONG TextureTilerUV_Base = ((p_stII->BaseAnimUv >> 16) & 0xff);
    ULONG U_SHIFT = ((p_stII->BaseAnimUv >> 24) & 0xf);
    ULONG V_SHIFT = ((p_stII->BaseAnimUv >> 28) & 0xf);

    FLOAT TexUMax, TexVMax;
    *(ULONG *)&TexUMax = (127 - U_SHIFT) << 23;
    *(ULONG *)&TexVMax = (127 - V_SHIFT) << 23;

    DeltaU *= TexVMax;

    vUVManip.x = (FLOAT) TextureTilerUV_Base;
    vUVManip.y = DeltaU;
    vUVManip.z = TexUMax;
    vUVManip.w = TexVMax;

    g_pXeContextManagerEngine->SetSPG2TexUVManip(vUVManip);
    g_pXeContextManagerEngine->SetSPG2Ratio(fRatio);
    g_pXeContextManagerEngine->SetSPG2OoNumOfSeg(fOoNumOfSeg);
    g_pXeContextManagerEngine->SetSPG2GlobalZAdd(*(VECTOR4FLOAT*)&p_stII->GlobalZADD);
    g_pXeContextManagerEngine->SetSPG2GlobalScale(p_stII->GlobalSCale); 
    g_pXeContextManagerEngine->SetSPG2Trapeze(fTrapeze);
    g_pXeContextManagerEngine->SetSPG2GlobalPos(*(VECTOR4FLOAT*)&p_stII->GlobalPos);
    g_pXeContextManagerEngine->SetSPG2Wind((VECTOR4FLOAT*)pWind);
    g_pXeContextManagerEngine->SetSPG2TrapezeDelta(fTrapezeDelta);
    g_pXeContextManagerEngine->SetSPG2EOHP(fEOHP);

    if(bSpecialLookAtX)
    {
        MATH_tdst_Vector    stCameraDir;
        MATH_CrossProduct(&stCameraDir,(MATH_tdst_Vector *)XCamera,(MATH_tdst_Vector *)YCamera);
        g_pXeContextManagerEngine->SetSPG2XCam(*(VECTOR4FLOAT*)&stCameraDir);
    }

    int iSizeOfElements = sizeof(XeVertexSPG2);

    BOOL bCreate = FALSE;

    if(*pVB==NULL)
    {
        if(ulNumberOfSegments==1)
            *pVB = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer((ulnumberOfPoints*4), iSizeOfElements);
        else
            *pVB = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer((ulnumberOfPoints*(ulNumberOfSegments + 1)*2)+(ulnumberOfPoints*2)-1, iSizeOfElements);
        bCreate = TRUE;
    }

    if(*pMesh==NULL)
    {
        *pMesh = new XeMesh();
        (*pMesh)->AddStream(XEVC_POSITIONT | XEVC_COLOR0 | XEVC_TEXCOORD0
#if 0//defined(_XE_COMPRESS_NORMALS)
            | XEVC_BLENDWEIGHT3_DEC3N
#else
            | XEVC_BLENDWEIGHT4
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
            | XEVC_NORMAL_DEC3N
#else
            | XEVC_NORMAL
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
            | XEVC_TANGENT3_DEC3N
#else
            | XEVC_TANGENT
#endif
            , *pVB);
    }

    if(bCreate)
    {
        MATH_tdst_Vector BVMin = {99999.0f,99999.0f,99999.0f};
        MATH_tdst_Vector BVMax = {-99999.0f,-99999.0f,-99999.0f};

        pCachedLine->fMaxSpriteSize = 0.0f;

        if(ulNumberOfSegments==1)
            g_pSPG2BufferArray = (XeVertexSPG2 *)(*pVB)->Lock((ulnumberOfPoints*4), iSizeOfElements);
        else
            g_pSPG2BufferArray = (XeVertexSPG2 *)(*pVB)->Lock((ulnumberOfPoints*(ulNumberOfSegments + 1)*2)+(ulnumberOfPoints*2)-1, iSizeOfElements);

        USHORT idx=0;

        int iPointIdx=0;

        while (ulnumberOfPoints--)
        {
            SOFT_tdst_AVertex   u_4Vert[4];
            SOFT_tdst_AVertex   stNormale;

            u_4Vert[0] = *(Coordinates ++);// Pos
            u_4Vert[1] = *(Coordinates ++);// Xa 
            u_4Vert[2] = *(Coordinates ++);// Ya
            stNormale = *(Coordinates ++);

            pCachedLine->fMaxSpriteSize = max( u_4Vert[2].w, pCachedLine->fMaxSpriteSize );

            Counter = ulNumberOfSegments + 1;

            if(BVMin.x > u_4Vert[0].x) BVMin.x = u_4Vert[0].x;
            if(BVMin.y > u_4Vert[0].y) BVMin.y = u_4Vert[0].y;
            if(BVMin.z > u_4Vert[0].z) BVMin.z = u_4Vert[0].z;
            if(BVMax.x < u_4Vert[0].x) BVMax.x = u_4Vert[0].x;
            if(BVMax.y < u_4Vert[0].y) BVMax.y = u_4Vert[0].y;
            if(BVMax.z < u_4Vert[0].z) BVMax.z = u_4Vert[0].z;

            int iVtxCount = 0;
            XeVertexSPG2 *g_pSPG2BufferArrayPrevious2; 

            ULONG ulColor;

            if (p_stII->GlobalColor) 
            {
                ulColor = p_stII->GlobalColor;
            }
            else
            {
                ulColor = *pColors++;
            }

            ulColor = XeConvertColor(ulColor);

            if(ulNumberOfSegments==1)
            {
                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&u_4Vert[0];
                g_pSPG2BufferArray->vPosv0.fW = u_4Vert[2].w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&u_4Vert[2]);
#else
                *(SOFT_tdst_AVertex *) &g_pSPG2BufferArray->vPosv2 = *(SOFT_tdst_AVertex *)&u_4Vert[2];
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&u_4Vert[1]);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&u_4Vert[1];
#endif                
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&stNormale);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&stNormale;
#endif
                g_pSPG2BufferArray->Extra.fPrimitiveIdx = (FLOAT)(stNormale.c ^ p_stII->BaseWind)*1000+iPointIdx;
                g_pSPG2BufferArray->Extra.fSegmentIdx = (FLOAT)(1.1f); // the .1 is to make sure we get the right integer value (float imprecision issue)
                g_pSPG2BufferArray->ulColor = ulColor;
                g_pSPG2BufferArray++;

                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&u_4Vert[0];
                g_pSPG2BufferArray->vPosv0.fW = u_4Vert[2].w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&u_4Vert[2]);
#else
                *(SOFT_tdst_AVertex *) &g_pSPG2BufferArray->vPosv2 = *(SOFT_tdst_AVertex *)&u_4Vert[2];
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&u_4Vert[1]);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&u_4Vert[1];
#endif                
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&stNormale);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&stNormale;
#endif
                g_pSPG2BufferArray->Extra.fPrimitiveIdx = (FLOAT)(stNormale.c ^ p_stII->BaseWind)*1000+iPointIdx;
                g_pSPG2BufferArray->Extra.fSegmentIdx = (FLOAT)(0.1f); // the .1 is to make sure we get the right integer value (float imprecision issue)
                g_pSPG2BufferArray->ulColor = ulColor;
                g_pSPG2BufferArray++;

                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&u_4Vert[0];
                g_pSPG2BufferArray->vPosv0.fW = u_4Vert[2].w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&u_4Vert[2]);
#else
                *(SOFT_tdst_AVertex *) &g_pSPG2BufferArray->vPosv2 = *(SOFT_tdst_AVertex *)&u_4Vert[2];
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&u_4Vert[1]);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&u_4Vert[1];
#endif                
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&stNormale);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&stNormale;
#endif
                g_pSPG2BufferArray->Extra.fPrimitiveIdx = (FLOAT)(stNormale.c ^ p_stII->BaseWind)*1000+iPointIdx;
                g_pSPG2BufferArray->Extra.fSegmentIdx = (FLOAT)(10.1f); // the .1 is to make sure we get the right integer value (float imprecision issue)
                g_pSPG2BufferArray->ulColor = ulColor;
                g_pSPG2BufferArray++;

                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&u_4Vert[0];
                g_pSPG2BufferArray->vPosv0.fW = u_4Vert[2].w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&u_4Vert[2]);
#else
                *(SOFT_tdst_AVertex *) &g_pSPG2BufferArray->vPosv2 = *(SOFT_tdst_AVertex *)&u_4Vert[2];
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&u_4Vert[1]);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&u_4Vert[1];
#endif                
#if 0//defined(_XE_COMPRESS_NORMALS)
                g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&stNormale);
#else
                *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&stNormale;
#endif
                g_pSPG2BufferArray->Extra.fPrimitiveIdx = (FLOAT)(stNormale.c ^ p_stII->BaseWind)*1000+iPointIdx;
                g_pSPG2BufferArray->Extra.fSegmentIdx = (FLOAT)(11.1f); // the .1 is to make sure we get the right integer value (float imprecision issue)
                g_pSPG2BufferArray->ulColor = ulColor;
                g_pSPG2BufferArray++;
            }
            else
            {
                while ( Counter -- )
                {
                    g_pSPG2BufferArrayPrevious2 = g_pSPG2BufferArray;

                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&u_4Vert[0];
                    g_pSPG2BufferArray->vPosv0.fW = u_4Vert[2].w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&u_4Vert[2]);
#else
                    *(SOFT_tdst_AVertex *) &g_pSPG2BufferArray->vPosv2 = *(SOFT_tdst_AVertex *)&u_4Vert[2];
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&u_4Vert[1]);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&u_4Vert[1];
#endif                
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&stNormale);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&stNormale;
#endif
                    g_pSPG2BufferArray->Extra.fPrimitiveIdx = (FLOAT)(stNormale.c ^ p_stII->BaseWind)*1000+iPointIdx;
                    g_pSPG2BufferArray->Extra.fSegmentIdx = (FLOAT)((ulNumberOfSegments-Counter)*10+1)+0.1f; // the .1 is to make sure we get the right integer value (float imprecision issue)
                    g_pSPG2BufferArray->ulColor = ulColor;
                    g_pSPG2BufferArray++;

                    // degenerate   
                    if(iVtxCount==0 && idx++)
                        *g_pSPG2BufferArray++ = *g_pSPG2BufferArrayPrevious2;

                    iVtxCount++;

                    g_pSPG2BufferArrayPrevious2 = g_pSPG2BufferArray;

                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&u_4Vert[0];
                    g_pSPG2BufferArray->vPosv0.fW = u_4Vert[2].w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&u_4Vert[2]);
#else
                    *(SOFT_tdst_AVertex *) &g_pSPG2BufferArray->vPosv2 = *(SOFT_tdst_AVertex *)&u_4Vert[2];
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&u_4Vert[1]);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&u_4Vert[1];
#endif                
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&stNormale);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&stNormale;
#endif
                    g_pSPG2BufferArray->Extra.fPrimitiveIdx = (FLOAT)((SHORT)(stNormale.c ^ p_stII->BaseWind)*1000+iPointIdx);
                    g_pSPG2BufferArray->Extra.fSegmentIdx = (FLOAT)((ulNumberOfSegments-Counter)*10)+0.1f; // the .1 is to make sure we get the right integer value (float imprecision issue)
                    g_pSPG2BufferArray->ulColor = ulColor;
                    g_pSPG2BufferArray++;

                    iVtxCount++;
                }

                // degenerate       
                *g_pSPG2BufferArray++ = *g_pSPG2BufferArrayPrevious2;       

                iPointIdx++;
            }
        }

        (*pVB)->Unlock();

        (*pMesh)->SetAxisAlignedBoundingVolume( BVMin, BVMax );
    }

    BOOL bWriteToAlpha = FALSE;

    if(GDI_gpst_CurDD->pst_CurrentMLTTXLVL)
    {
        BOOL bQueue = TRUE;
        if((*pMesh)->HasBoundingVolume() && !(_pst_SPG2->ulFlags & SPG2_GridGeneration) && !(_pst_SPG2->ulFlags & SPG2_IAGenerator))
        {            
            MATH_tdst_Vector  vBox[2];
            MATH_tdst_Vector* ppBox[2] = { &vBox[0], &vBox[1] };

            BOOL OBJ_CullingOBBox
                (
                MATH_tdst_Vector    *_pst_LMin,
                MATH_tdst_Vector    *_pst_LMax,
                MATH_tdst_Matrix    *_pst_GlobalObject,
                CAM_tdst_Camera     *_pst_Cam
                );

            (*pMesh)->GetAxisAlignedBoundingVolume(vBox[0], vBox[1]);

            FLOAT iSize = fRatio * ulNumberOfSegments * p_stII->GlobalSCale;
            vBox[0].x += -iSize + p_stII->GlobalPos.x;
            vBox[0].y += -iSize + p_stII->GlobalPos.y;
            vBox[0].z += -iSize + p_stII->GlobalPos.z;
            vBox[1].x +=  iSize + p_stII->GlobalPos.x;
            vBox[1].y +=  iSize + p_stII->GlobalPos.y;
            vBox[1].z +=  iSize + p_stII->GlobalPos.z;

            bQueue = !(OBJ_CullingOBBox(&vBox[0], &vBox[1], GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix, &GDI_gpst_CurDD->st_Camera));
    	}

        if(!bQueue) 
            return;

        eXeRENDERLISTTYPE eRT = XeRT_OPAQUE;
        if (((MAT_GET_Blending(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags) != MAT_Cc_Op_Copy) && (MAT_GET_Blending(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags) != MAT_Cc_Op_Glow)) || (GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags & MAT_Cul_Flag_HideColor))
            eRT = XeRT_TRANSPARENT;

        ULONG ulCustomVSBK = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetCustomVS();
        ULONG ulCustomPSBK = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetCustomPS();

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomPS(CUSTOM_PS_SPG2PLANT);

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVS(CUSTOM_VS_SPG2);
        if(bSpecialLookAtX)
            GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(1, 5); // special lookat X
        else
            GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(1, ulMode+1); // +1 because 0 is no spg2 in shader

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(2, (p_stII->BaseAnimUv!=0));
        //GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(3, bSpecialLookAtX);      
        //GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(4, ((GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL)!=-1) ? 1 : 0));


		g_oXeRenderer.QueueMeshForRenderSPG2(GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix,
			*pMesh,
			GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial,
			GDI_gpst_CurDD->ul_CurrentDrawMask,
			eRT,
			(ulNumberOfSegments==1) ? XeRenderObject::QuadList : XeRenderObject::TriangleStrip,
			0,
			eRT != XeRT_TRANSPARENT ? GDI_gpst_CurDD->g_cZListCurrentDisplayOrder : (GDI_gpst_CurDD->g_cZListCurrentDisplayOrder-1),
			(LPVOID)_pst_SPG2,
            bWriteToAlpha );

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVS(ulCustomVSBK);
        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomPS(ulCustomPSBK);
    }
    else
        ERR_X_Assert(0);
}

void Xe_l_DrawSPG2_SPRITES_2X(
                              XeMesh                            **pMesh,
                              XeBuffer                          **pVB,
                              XeBuffer                          **pHelicoidalVB,
                              SOFT_tdst_AVertex                 *Coordinates,
                              GEO_Vertex                            *XCamera,
                              GEO_Vertex                            *YCamera,
                              ULONG                             *pColors,
                              ULONG                             ulnumberOfPoints,
                              ULONG                             ulNumberOfSprites,
                              float                             CosAlpha2,
                              float                             SinAlpha2,
                              float                             SpriteGenRadius,
                              float                             fEOHP,
                              float                             fRatio,
                              SOFT_tdst_AVertex                 *pWind,
                              SPG2_InstanceInforamtion          *p_stII,
                              SPG2_tdst_Modifier                *_pst_SPG2
                              )
{
    ULONG BM;
    ULONG Counter;
    float OoNSPR;
    float ComplexRadius_RE;
    float ComplexRadius_IM;
    float ComplexRadius_LOCAL;
    vCam  XCam,YCam;
    BM = 0;

    *(vCam *)&XCam = *(vCam*)XCamera;
    *(vCam *)&YCam = *(vCam*)YCamera;

    g_oXeRenderer.m_uiNumSPG2 += ulnumberOfPoints*ulNumberOfSprites*2;

    ComplexRadius_IM = 1.0f;
    ComplexRadius_RE = 0.0f;

    ulNumberOfSprites &= 63;
    if (!ulnumberOfPoints || !ulNumberOfSprites) 
        return;

    OoNSPR = 1.0f / (float)ulNumberOfSprites;

    BOOL bCreate = FALSE;
    int iSizeOfElements = sizeof(XeVertexSPG2);
    g_pSPG2HelicoidalBufferArray = NULL;

    if(*pVB==NULL)
    {
        *pVB = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer((ulnumberOfPoints*ulNumberOfSprites*4), iSizeOfElements);
        bCreate = TRUE;
    }

    if(*pHelicoidalVB==NULL)
    {
#ifndef ACTIVE_EDITORS
        if (CosAlpha2 != -1.0f || SinAlpha2 != 0.0f)
#endif
        {
            *pHelicoidalVB = (XeBuffer*)g_XeBufferMgr.CreateDynVertexBuffer(FALSE);
        }
    }

    if(*pMesh==NULL)
    {
        *pMesh = new XeMesh();
        (*pMesh)->AddStream(XEVC_POSITIONT | XEVC_COLOR0 | XEVC_TEXCOORD0
#if 0//defined(_XE_COMPRESS_NORMALS)
            | XEVC_BLENDWEIGHT3_DEC3N 
#else
            | XEVC_BLENDWEIGHT4 
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
            | XEVC_NORMAL_DEC3N 
#else
            | XEVC_NORMAL 
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
            | XEVC_TANGENT3_DEC3N 
#else
            | XEVC_TANGENT 
#endif
            , *pVB);

#ifndef ACTIVE_EDITORS
        if(*pHelicoidalVB)
#endif
        {
            (*pMesh)->AddStream(XEVC_TEXCOORD1 | XEVC_TEXCOORD2_FLOAT4, *pHelicoidalVB);
        }
    }

    g_pXeContextManagerEngine->SetSPG2Ratio(fRatio);
    g_pXeContextManagerEngine->SetSPG2OoNumOfSeg(OoNSPR);
    g_pXeContextManagerEngine->SetSPG2GlobalZAdd(*(VECTOR4FLOAT*)&p_stII->GlobalZADD);
    g_pXeContextManagerEngine->SetSPG2GlobalScale(p_stII->GlobalSCale); 
    g_pXeContextManagerEngine->SetSPG2GlobalPos(*(VECTOR4FLOAT*)&p_stII->GlobalPos);
    g_pXeContextManagerEngine->SetSPG2EOHP(fEOHP);
    g_pXeContextManagerEngine->SetSPG2SpriteRadius(SpriteGenRadius);
    g_pXeContextManagerEngine->SetSPG2XCam(*(VECTOR4FLOAT*)XCamera);
    g_pXeContextManagerEngine->SetSPG2YCam(*(VECTOR4FLOAT*)YCamera);

    g_pXeContextManagerEngine->SetSPG2Trapeze(*pHelicoidalVB ? 0.0f : 1.0f);

    if(bCreate || *pHelicoidalVB)
    {
        if(bCreate)
            g_pSPG2BufferArray = (XeVertexSPG2 *)(*pVB)->Lock((ulnumberOfPoints*ulNumberOfSprites*4), iSizeOfElements);

        if(*pHelicoidalVB)
        {
            g_pSPG2HelicoidalBufferArray = (XeVertexSPG2Helicoidal *)(*pHelicoidalVB)->Lock((ulnumberOfPoints*ulNumberOfSprites*4), sizeof(XeVertexSPG2Helicoidal), FALSE, TRUE);
        }

        while (ulnumberOfPoints--)
        {
            SOFT_tdst_AVertex   VC,Xa,Ya,Za,LocalX,LocalY;
            float   fdEPT;
            ULONG ulColor;

            if(bCreate)
            {
                VC = *(Coordinates ++);
                Xa = *(Coordinates ++);
                Ya = *(Coordinates ++);
                Za = *(Coordinates ++);

                fdEPT = 0.0f;

                if (p_stII->GlobalColor) 
                {
                    ulColor = p_stII->GlobalColor;
                }
                else
                {
                    ulColor = *pColors++;
                }

                ulColor = XeConvertColor(ulColor);
            }

            Counter = ulNumberOfSprites;

            while ( Counter -- )
            {
                if(bCreate)
                {
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&VC;
                    g_pSPG2BufferArray->vPosv0.fW = Ya.w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                    //g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&Ya);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv2 = *(GEO_Vertex *)&Ya;
                    g_pSPG2BufferArray->vPosv2.fW = 0.0f;//,0
#endif                  
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&Xa);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&Xa;
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&Za);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&Za;
#endif
                    g_pSPG2BufferArray->Extra.fSegmentIdx = (ulNumberOfSprites-(Counter+1));
                    g_pSPG2BufferArray->Extra.fPrimitiveIdx = ComplexRadius_IM;
                    g_pSPG2BufferArray->ulColor = ulColor;
                    g_pSPG2BufferArray++;

                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&VC;
                    g_pSPG2BufferArray->vPosv0.fW = Ya.w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                    //g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&Ya);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv2 = *(GEO_Vertex *)&Ya;
                    g_pSPG2BufferArray->vPosv2.fW = 1.0f;//,0
#endif                  
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&Xa);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&Xa;
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&Za);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&Za;
#endif
                    g_pSPG2BufferArray->Extra.fSegmentIdx = (ulNumberOfSprites-(Counter+1));
                    g_pSPG2BufferArray->Extra.fPrimitiveIdx = ComplexRadius_IM;
                    g_pSPG2BufferArray->ulColor = ulColor;
                    g_pSPG2BufferArray++;

                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&VC;
                    g_pSPG2BufferArray->vPosv0.fW = Ya.w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                    //g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&Ya);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv2 = *(GEO_Vertex *)&Ya;
                    g_pSPG2BufferArray->vPosv2.fW = 1.0f;//,1
#endif                  
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&Xa);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&Xa;
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&Za);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&Za;
#endif
                    g_pSPG2BufferArray->Extra.fSegmentIdx = (ulNumberOfSprites-(Counter+1))+0.995f;
                    g_pSPG2BufferArray->Extra.fPrimitiveIdx = ComplexRadius_IM;
                    g_pSPG2BufferArray->ulColor = ulColor;
                    g_pSPG2BufferArray++;

                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv0 = *(GEO_Vertex *)&VC;
                    g_pSPG2BufferArray->vPosv0.fW = Ya.w;
#if 0//defined(_XE_COMPRESS_NORMALS)
                    //g_pSPG2BufferArray->vPosv2.ulWeight = XeMakeDEC3N((FLOAT*)&Ya);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv2 = *(GEO_Vertex *)&Ya;
                    g_pSPG2BufferArray->vPosv2.fW = 0.0f;//,1
#endif                  
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vPosv1.ulTangent = XeMakeDEC3N((FLOAT*)&Xa);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vPosv1 = *(GEO_Vertex *)&Xa;
#endif
#if 0//defined(_XE_COMPRESS_NORMALS)
                    g_pSPG2BufferArray->vNrm.ulNrm = XeMakeDEC3N((FLOAT*)&Za);
#else
                    *(GEO_Vertex *) &g_pSPG2BufferArray->vNrm = *(GEO_Vertex *)&Za;
#endif
                    g_pSPG2BufferArray->Extra.fSegmentIdx = (ulNumberOfSprites-(Counter+1))+0.995f;
                    g_pSPG2BufferArray->Extra.fPrimitiveIdx = ComplexRadius_IM;
                    g_pSPG2BufferArray->ulColor = ulColor;
                    g_pSPG2BufferArray++;

                    // Helicoïdal effect 
                   /* MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Xa , CosAlpha);
                    MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&Ya , SinAlpha);
                    MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Ya , CosAlpha);
                    MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&Xa , -SinAlpha);
                    Xa = LocalX ;
                    Ya = LocalY ;
*/
                    ComplexRadius_LOCAL = ComplexRadius_IM * CosAlpha + ComplexRadius_RE * SinAlpha;
                    ComplexRadius_RE = ComplexRadius_IM * SinAlpha - ComplexRadius_RE * CosAlpha;
                    ComplexRadius_IM = ComplexRadius_LOCAL;
                }

                if(g_pSPG2HelicoidalBufferArray)
                {
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamX = *(vCam *)&XCam;
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamY = *(vCam *)&YCam;
                    g_pSPG2HelicoidalBufferArray++;
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamX = *(vCam *)&XCam;
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamY = *(vCam *)&YCam;
                    g_pSPG2HelicoidalBufferArray++;
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamX = *(vCam *)&XCam;
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamY = *(vCam *)&YCam;
                    g_pSPG2HelicoidalBufferArray++;
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamX = *(vCam *)&XCam;
                    *(vCam *) &g_pSPG2HelicoidalBufferArray->vCamY = *(vCam *)&YCam;
                    g_pSPG2HelicoidalBufferArray++;

                    // Helicoïdal effect 
                    MATH_ScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&XCam , CosAlpha2);
                    MATH_AddScaleVector((MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&LocalX , (MATH_tdst_Vector *)&YCam , SinAlpha2);
                    MATH_ScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&YCam , CosAlpha2);
                    MATH_AddScaleVector((MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&LocalY , (MATH_tdst_Vector *)&XCam , -SinAlpha2);
                    XCam = *(vCam*)&LocalX ;
                    YCam = *(vCam*)&LocalY ;
                }
            }
        }

        if(bCreate)
            (*pVB)->Unlock();

        if(*pHelicoidalVB)
            (*pHelicoidalVB)->Unlock();
    }

    if(GDI_gpst_CurDD->pst_CurrentMLTTXLVL)
    {
        eXeRENDERLISTTYPE eRT = XeRT_OPAQUE;
        if (((MAT_GET_Blending(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags) != MAT_Cc_Op_Copy) && (MAT_GET_Blending(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags) != MAT_Cc_Op_Glow)) || (GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags & MAT_Cul_Flag_HideColor))
            eRT = XeRT_TRANSPARENT;

        ULONG ulCustomVSBK = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetCustomVS();
        ULONG ulCustomPSBK = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetCustomPS();

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomPS(CUSTOM_PS_SPG2PLANT);

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVS(CUSTOM_VS_SPG2);
        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(1, 4);
        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(2, 0);
        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVSFeature(4, ((GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL)!=-1) ? 1 : 0));

        g_oXeRenderer.QueueMeshForRenderSPG2(GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix,
            *pMesh,
            GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial,
            GDI_gpst_CurDD->ul_CurrentDrawMask,
            eRT,
            XeRenderObject::QuadList,
            0,
            GDI_gpst_CurDD->g_cZListCurrentDisplayOrder,
            (LPVOID)_pst_SPG2);

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomVS(ulCustomVSBK);
        GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_XeMaterial->SetCustomPS(ulCustomPSBK);
    }
    else
        ERR_X_Assert(0);
}

void Xe_l_DrawSPG2(SPG2_Instance                        *pCachedLine,
                   ULONG                                *ulTextureID,
                   GEO_Vertex                           *XCam,
                   GEO_Vertex                           *YCam,
                   SPG2_tdst_Modifier                   *_pst_SPG2 ,
                   SOFT_tdst_AVertex                    *pWind,
                   SPG2_InstanceInforamtion             *p_stII)
{
    if (GDI_gpst_CurDD->pst_CurrentMLTTXLVL == NULL) 
        return;

#if defined(_XENON_RENDER_PC)
    if (!GDI_b_IsXenonGraphics())
    {
        // native format is not compatible anymore...
        //OGL_l_DrawSPG2(pCachedLine, ulTextureID, XCam, YCam, _pst_SPG2, pWind, p_stII);
        return;
    }
#endif

    BOOL  bSpecialLookAtX = FALSE;
    float fExtractionOfHorizontalPlane;
    ULONG BM=0;
    ULONG ulnumberOfPoints;
    ULONG NumberOfSegments;

    ULONG oldDM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~GDI_Cul_DM_TestBackFace;

    MAT_tdst_MTLevel *pCurMatLevelBackup = GDI_gpst_CurDD->pst_CurrentMLTTXLVL;

    ulnumberOfPoints = pCachedLine->a_PtrLA2 >> 2;
    NumberOfSegments = _pst_SPG2->NumberOfSegments;
    if (NumberOfSegments>>1)
    {
        // TODO : not compatible with full caching... see later for lod optimisations
        //NumberOfSegments = (ULONG)((float)NumberOfSegments * p_stII->Culling);
        if (NumberOfSegments <= 2) NumberOfSegments = 2;
    }

    /* Xaxis lookat ***********************/
    if ((_pst_SPG2->ulFlags & SPG2_XAxisIsInlookat))// && (pCachedLine->ulFlags & 2))
    {
        bSpecialLookAtX = TRUE;
        _pst_SPG2->ulFlags &= ~(SPG2_DrawY|SPG2_RotationNoise);
        _pst_SPG2->ulFlags |= SPG2_DrawX;
        //pCachedLine->ulFlags &= ~2;
        // the rest in done in the shader
        //...
    }   
    else // "Random" rotation **********************
        if ((_pst_SPG2->ulFlags & SPG2_RotationNoise))// && (pCachedLine->ulFlags & 1))
        {
            // only compute the first time we create the vertex data
            if( ((_pst_SPG2 ->ulFlags & SPG2_DrawSprites) && !pCachedLine->pVBSprites) || (!(_pst_SPG2 ->ulFlags & SPG2_DrawSprites) && !pCachedLine->pVB2X))
            {
                ULONG Counter;
                float                   CosV,SinV,Swap;
                SOFT_tdst_AVertex   *Coordinates;
                CosV  = 1.0f;
                SinV  = 0.0f;
                Coordinates = pCachedLine->a_PointLA2;
                Counter = ulnumberOfPoints;
                while (Counter --)
                {
                    MATH_tdst_Vector    LocalX ,LocalY ;
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
        }

        if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
        {
            MAT_SET_FLAG(BM, MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV);
        }
        else
        {
            MAT_SET_FLAG(BM, MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_AlphaTest | MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV);    
            MAT_SET_AlphaTresh(BM , _pst_SPG2->AlphaThreshold);
        }

        //MAT_SET_ColorOp(BM, MAT_GET_ColorOp(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ul_Flags));        

        if (_pst_SPG2 ->ulFlags & SPG2_ModeAdd)  
        {
            MAT_SET_ColorOp(BM, MAT_Cc_ColorOp_RLI);
            MAT_SET_Blending(BM , MAT_Cc_Op_AlphaPremult);
            MAT_SET_FLAG(BM, MAT_GET_FLAG(BM)| MAT_Cul_Flag_NoZWrite);
        }
        else if (_pst_SPG2->ulFlags & SPG2_DrawinAlpha)  
        {
            MAT_SET_ColorOp(BM, MAT_Cc_ColorOp_Diffuse2X);
            MAT_SET_Blending(BM , MAT_Cc_Op_Alpha);
            MAT_SET_FLAG(BM, MAT_GET_FLAG(BM)| MAT_Cul_Flag_NoZWrite);
        }
        else
        {
            MAT_SET_ColorOp(BM, MAT_Cc_ColorOp_Diffuse2X);
            MAT_SET_Blending(BM , MAT_Cc_Op_Copy);
        }

        GDI_gpst_CurDD->LastBlendingMode = BM;

        fExtractionOfHorizontalPlane = _pst_SPG2->fExtractionOfHorizontalPlane + 0.5f;

        if((_pst_SPG2 -> ulFlags & SPG2_DrawHat) && (_pst_SPG2 -> ulFlags & SPG2_DrawY) && (_pst_SPG2 -> ulFlags & SPG2_DrawX) && !GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel)
        {
            Xe_l_DrawSPG2_2X(&pCachedLine->pMesh2X, &pCachedLine->pVB2X, pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,6,pWind,p_stII, bSpecialLookAtX, _pst_SPG2, pCachedLine);
        }
        else
        {
            if (_pst_SPG2 ->ulFlags & SPG2_DrawHat)
            {
		        Xe_l_DrawSPG2_2X(&pCachedLine->pMesh2X, &pCachedLine->pVB2X, pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,2,pWind,p_stII, bSpecialLookAtX, _pst_SPG2, pCachedLine);
            }

            // material layers were not setuped properly... in software, we use the first layer property with subsequent layer texture id
            // only update material first time it is used
            if(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel)
            {   
                GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->s_AditionalFlags = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->s_AditionalFlags;
                GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->ScaleSPeedPosU = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ScaleSPeedPosU;
                GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->ScaleSPeedPosV = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ScaleSPeedPosV;
                GDI_gpst_CurDD->pst_CurrentMLTTXLVL = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel;       
                GDI_gpst_CurDD->LastBlendingMode = BM;
            }

            if((_pst_SPG2 -> ulFlags & SPG2_DrawY) && (_pst_SPG2 -> ulFlags & SPG2_DrawX) && !GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel)
            {
                Xe_l_DrawSPG2_2X(&pCachedLine->pMesh2X, &pCachedLine->pVB2X, pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,5,pWind,p_stII, bSpecialLookAtX, _pst_SPG2, pCachedLine);
            }
            else
            {
                if (_pst_SPG2 -> ulFlags & SPG2_DrawY)
                {
		        Xe_l_DrawSPG2_2X(&pCachedLine->pMesh2X, &pCachedLine->pVB2X, pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,0,pWind,p_stII, bSpecialLookAtX, _pst_SPG2, pCachedLine);
                }

                // material layers were not setuped properly... in software, we use the first layer property with subsequent layer texture id
                // only update material first time it is used
                if(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel)
                {   
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->s_AditionalFlags = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->s_AditionalFlags;
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->ScaleSPeedPosU = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ScaleSPeedPosU;
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->ScaleSPeedPosV = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ScaleSPeedPosV;
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel;       
                    GDI_gpst_CurDD->LastBlendingMode = BM;
                }

                if (_pst_SPG2 ->ulFlags & SPG2_DrawX)
                {
		        Xe_l_DrawSPG2_2X(&pCachedLine->pMesh2X, &pCachedLine->pVB2X, pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,NumberOfSegments,_pst_SPG2->fTrapeze , fExtractionOfHorizontalPlane , _pst_SPG2->f_GlobalRatio , _pst_SPG2->TileNumber,1,pWind,p_stII, bSpecialLookAtX, _pst_SPG2, pCachedLine);
                }

                // material layers were not setuped properly... in software, we use the first layer property with subsequent layer texture id
                // only update material first time it is used
                if(GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel)
                {   
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->s_AditionalFlags = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->s_AditionalFlags;
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->ScaleSPeedPosU = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ScaleSPeedPosU;
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel->ScaleSPeedPosV = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->ScaleSPeedPosV;
                    GDI_gpst_CurDD->pst_CurrentMLTTXLVL = GDI_gpst_CurDD->pst_CurrentMLTTXLVL->pst_NextLevel;       
                    GDI_gpst_CurDD->LastBlendingMode = BM;
                }
            }
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

            Xe_l_DrawSPG2_SPRITES_2X(&pCachedLine->pMeshSprites, &pCachedLine->pVBSprites, &pCachedLine->pVBHelicoidal, pCachedLine->a_PointLA2,XCam,YCam,pCachedLine->a_ColorLA2,ulnumberOfPoints,_pst_SPG2->NumberOfSprites,CA,SA,_pst_SPG2->f_SpriteGeneratorRadius,fExtractionOfHorizontalPlane, _pst_SPG2->f_GlobalRatio ,pWind,p_stII, _pst_SPG2);
        }

        GDI_gpst_CurDD->pst_CurrentMLTTXLVL = pCurMatLevelBackup;
        GDI_gpst_CurDD->ul_CurrentDrawMask = oldDM;
        GDI_gpst_CurDD->LastBlendingMode = -1;
}

//#endif // XENON console implementation 

/**********************************************************************************************************************/
/* SPG2 specific functions END ****************************************************************************************/
/**********************************************************************************************************************/

#if defined(ACTIVE_EDITORS)

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - STRUCTURES
// ------------------------------------------------------------------------------------------------
struct XeSoftVertex
{
    FLOAT fX, fY, fZ;
    ULONG ulColor;
    FLOAT fU, fV;

    inline void Set(FLOAT _fX, FLOAT _fY, FLOAT _fZ, ULONG _ulColor)
    {
        fX      = _fX;
        fY      = _fY;
        fZ      = _fZ;
        ulColor = _ulColor;
    }

    inline void Set(const MATH_tdst_Vector& _v, ULONG _ulColor)
    {
        fX      = _v.x;
        fY      = _v.y;
        fZ      = _v.z;
        ulColor = _ulColor;
    }
};

struct XeSoftVertexT
{
    FLOAT fX, fY, fZ, fW;
    ULONG ulColor;
    FLOAT fU, fV;

    inline void Set(FLOAT _fX, FLOAT _fY, FLOAT _fZ, FLOAT _fW, ULONG _ulColor)
    {
        fX      = _fX;
        fY      = _fY;
        fZ      = _fZ;
        fW      = _fW;
        ulColor = _ulColor;
    }

    inline void Set(const MATH_tdst_Vector& _v, FLOAT _fW, ULONG _ulColor)
    {
        fX      = _v.x;
        fY      = _v.y;
        fZ      = _v.z;
        fW      = _fW;
        ulColor = _ulColor;
    }
};

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - MACROS
// ------------------------------------------------------------------------------------------------

#define Xe_M_AddVertex( _pt,_c )\
{\
    pVertices[ulNbVertex++].Set(_pt, _c);\
}

#define Xe_M_AddVertexT( _pt,_c )\
{\
    pVertices[ulNbVertex++].Set(_pt, 1.0f, _c);\
}

#define Xe_M_AddTriangle( pt1,pt2,pt3 )\
{\
    pVertices[ulNbVertex++].Set(pt1, c);\
    pVertices[ulNbVertex++].Set(pt2, c);\
    pVertices[ulNbVertex++].Set(pt3, c);\
}

#define Xe_M_AddLitTriangle( pt1,pt2,pt3 )\
{\
    MATH_SubVector( &a, &(pt2), &(pt1));\
    MATH_SubVector( &b, &(pt3), &(pt1));\
    MATH_CrossProduct( &n, &a, &b );\
    MATH_NormalizeVector( &n, &n );\
    if ( (t = MATH_f_DotProduct( &light, &n ) ) <= 0)\
{\
    c = XeConvertColor(LIGHT_ul_Interpol2Colors(0, ul_Color, -t));\
    pVertices[ulNbVertex++].Set(pt1, c);\
    pVertices[ulNbVertex++].Set(pt2, c);\
    pVertices[ulNbVertex++].Set(pt3, c);\
}\
}

#define Xe_M_GetEllipsePoint(alpha) \
{ \
    MATH_ScaleVector(&A, &_pst_Ellipse->st_A, fOptCos(alpha)); \
    MATH_ScaleVector(&B, &_pst_Ellipse->st_B, fOptSin(alpha)); \
    MATH_AddVector(&N, &A, &B); \
    MATH_AddEqualVector(&N, &C); \
}
// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - PRIVATE VARIABLES
// ------------------------------------------------------------------------------------------------
static XeMaterial* s_pstHelperMatCopy  = NULL;
static XeMaterial* s_pstHelperMatAlpha = NULL;

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------

static void Xe_GDIInitTool(void)
{
    // Helper materials - No texture, using vertex color
    // -------------------------------------------------
    // Copy
    s_pstHelperMatCopy = XeMaterial_Create();
    s_pstHelperMatCopy->SetAlphaBlend(FALSE);
    s_pstHelperMatCopy->SetAlphaTest(FALSE);
    s_pstHelperMatCopy->SetColorSource(MAT_Cc_ColorOp_RLI);
    s_pstHelperMatCopy->SetColorWrite(TRUE);
    s_pstHelperMatCopy->SetZState(FALSE, FALSE);
    s_pstHelperMatCopy->SetTwoSided(FALSE);

    // Alpha blended
    s_pstHelperMatAlpha = XeMaterial_Create();
    s_pstHelperMatAlpha->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    s_pstHelperMatAlpha->SetAlphaTest(FALSE);
    s_pstHelperMatAlpha->SetColorSource(MAT_Cc_ColorOp_RLI);
    s_pstHelperMatAlpha->SetColorWrite(TRUE);
    s_pstHelperMatAlpha->SetZState(FALSE, FALSE);
    s_pstHelperMatAlpha->SetTwoSided(FALSE);
}

static void Xe_GDIShutdownTool(void)
{
    // Helper materials
    // ----------------
    // Copy
    XeMaterial_Destroy(s_pstHelperMatCopy);
    s_pstHelperMatCopy = NULL;

    // Alpha
    XeMaterial_Destroy(s_pstHelperMatAlpha);
    s_pstHelperMatAlpha = NULL;
}

static void Xe_GDIRequest(ULONG _ulData, BOOL _bEnable)
{
    switch (_ulData)
    {
    case 0x2a02:                                // GL_POLYGON_OFFSET_LINE
#if defined(ACTIVE_EDITORS)
        g_oXeRenderer.SetZBias(_bEnable);
#endif
        break;

    case 0x8037:                                // GL_POLYGON_OFFSET_FILL
        break;

    default:
        XeValidate(FALSE, "Unknown GDI request");
        break;
    }
}

XeMesh* Xe_GDIRequestSoftMesh(BOOL _bTransformedCoords = FALSE)
{
    XeMesh* pMesh = g_oXeRenderer.RequestDynamicMesh();

    // Set the vertex format
    // SC: NOTE: The vertex shaders are always expecting texture coordinates so I
    //           have to include that component even if it is not used...
    pMesh->SetStreamComponents(0, (_bTransformedCoords ? XEVC_POSITIONT : XEVC_POSITION) | XEVC_COLOR0 | XEVC_TEXCOORD0);

    return pMesh;
}

static void Xe_GDIDrawSoftArrow(SOFT_tdst_Arrow* _pst_Arrow)
{
    ERR_X_Assert(_pst_Arrow != NULL);

    XeMesh*       pMesh     = NULL;
    XeSoftVertex* pVertices = NULL;

    MATH_tdst_Vector    a, b, a1, b1, i, j, k, light, n;
    MATH_tdst_Vector    pt[8];
    float f_Scale, t;
    ULONG ul_Color, c;
    ULONG ulDrawMask = 0xffffffff & ~(GDI_Cul_DM_ZTest);
    ULONG ulNbVertex;

    ul_Color = _pst_Arrow->st_Start.color;
    ul_Color = XeAttenuateColor(ul_Color);

    MATH_CopyVector(&a, (MATH_tdst_Vector *) &_pst_Arrow->st_Start);
    MATH_InitVector(&light, 0, 0, 1 );

    if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_UseFactor)
        MATH_ScaleVector(&b, &_pst_Arrow->st_Axis, _pst_Arrow->f_Move);
    else
        MATH_CopyVector(&b, &_pst_Arrow->st_Axis);

    MATH_AddEqualVector(&b, &a);

    if ( !(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_DrawNotLine) )
    {
        pMesh = Xe_GDIRequestSoftMesh();

        if ( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForSpot )
        {
            pVertices  = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(4, sizeof(XeSoftVertex));
            ulNbVertex = 0;

            if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForShadow)
                c = XeConvertColor(ul_Color);
            else
                c = XeConvertColor(0xFFFF0000);

            c = XeAttenuateColor(c);
            MATH_AddVector( &b1, &b, &_pst_Arrow->st_Up );
            Xe_M_AddVertex(a, c);
            Xe_M_AddVertex(b1, c);
            MATH_SubVector( &b1, &b, &_pst_Arrow->st_Up );
            Xe_M_AddVertex(a, c);
            Xe_M_AddVertex(b1, c);

            pMesh->GetStream(0)->pBuffer->Unlock();

            g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
                pMesh, s_pstHelperMatCopy,
                ulDrawMask,
                -1,
                XeRT_HELPERS,
                XeRenderObject::LineList,
                255, 0, (LPVOID)ROS_HELPERS,
                XeRenderObject::Common,
                0,
                QMFR_LIGHTED);
        }
        else if ( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_DrawParallel )
        {
            pVertices  = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(4, sizeof(XeSoftVertex));
            ulNbVertex = 0;

            if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForShadow)
                c = XeConvertColor(ul_Color);
            else
                c = XeConvertColor(0xFFFF0000);

            c = XeAttenuateColor(c);
            MATH_AddVector( &a1, &a, &_pst_Arrow->st_Up );
            MATH_AddVector( &b1, &b, &_pst_Arrow->st_Up );
            Xe_M_AddVertex(a1, c);
            Xe_M_AddVertex(b1, c);
            MATH_SubVector( &a1, &a, &_pst_Arrow->st_Up );
            MATH_SubVector( &b1, &b, &_pst_Arrow->st_Up );
            Xe_M_AddVertex(a1, c);
            Xe_M_AddVertex(b1, c);

            pMesh->GetStream(0)->pBuffer->Unlock();

            g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
                pMesh, s_pstHelperMatCopy,
                ulDrawMask,
                -1,
                XeRT_HELPERS,
                XeRenderObject::LineList,
                255, 0, (LPVOID)ROS_HELPERS,
                XeRenderObject::Common,
                0,
                QMFR_LIGHTED);
        }
        else
        {
            pVertices  = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(2, sizeof(XeSoftVertex));
            ulNbVertex = 0;

            Xe_M_AddVertex(a, XeConvertColor(ul_Color));
            Xe_M_AddVertex(b, XeConvertColor(ul_Color));

            pMesh->GetStream(0)->pBuffer->Unlock();

            g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
                pMesh, s_pstHelperMatCopy,
                ulDrawMask,
                -1,
                XeRT_HELPERS,
                XeRenderObject::LineList,
                255, 0, (LPVOID)ROS_HELPERS,
                XeRenderObject::Common,
                0,
                QMFR_LIGHTED);
        }
    }

    if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_NoEndPoint)
        return;

    MATH_CopyVector(&i, &_pst_Arrow->st_Axis);
    if ( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForSpot)
        f_Scale = 0.1f;
    else
        f_Scale = MATH_f_NormVector( &i ) / 10.0f;
    MATH_NormalizeVector(&i, &i);
    MATH_CopyVector(&k, &_pst_Arrow->st_Up);
    MATH_NormalizeVector(&k, &k);
    MATH_CrossProduct(&j, &k, &i);

    MATH_ScaleEqualVector(&i, f_Scale);
    MATH_ScaleEqualVector(&j, f_Scale);
    MATH_ScaleEqualVector(&k, f_Scale);

    MATH_SubVector(&pt[0], &b, &i);
    MATH_AddEqualVector(&pt[0], &j);
    MATH_AddEqualVector(&pt[0], &k);
    MATH_SubVector(&pt[1], &pt[0], &j);
    MATH_SubEqualVector(&pt[1], &j);
    MATH_SubVector(&pt[2], &pt[1], &k);
    MATH_SubEqualVector(&pt[2], &k);
    MATH_AddVector(&pt[3], &pt[2], &j);
    MATH_AddEqualVector(&pt[3], &j);

    if( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_Cube )
    {
        MATH_ScaleEqualVector(&i, 2);
        MATH_AddVector(&pt[4], &pt[0], &i);
        MATH_AddVector(&pt[5], &pt[1], &i);
        MATH_AddVector(&pt[6], &pt[2], &i);
        MATH_AddVector(&pt[7], &pt[3], &i);

        pMesh     = Xe_GDIRequestSoftMesh();
        pVertices = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(12 * 3, sizeof(XeSoftVertex));

        ulNbVertex = 0;

        Xe_M_AddLitTriangle( pt[0], pt[2], pt[1]);
        Xe_M_AddTriangle( pt[0], pt[3], pt[2]);
        Xe_M_AddLitTriangle( pt[0], pt[1], pt[4]);
        Xe_M_AddTriangle( pt[1], pt[5], pt[4]);
        Xe_M_AddLitTriangle( pt[1], pt[2], pt[5]);
        Xe_M_AddTriangle( pt[2], pt[6], pt[5]);
        Xe_M_AddLitTriangle( pt[2], pt[3], pt[6]);
        Xe_M_AddTriangle( pt[3], pt[7], pt[6]);
        Xe_M_AddLitTriangle( pt[3], pt[0], pt[7]);
        Xe_M_AddTriangle( pt[0], pt[4], pt[7]);
        Xe_M_AddLitTriangle( pt[4], pt[5], pt[6]);
        Xe_M_AddTriangle( pt[4], pt[6], pt[7]);

        pMesh->GetStream(0)->pBuffer->Unlock();

        ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulNbVertex);

        g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
            pMesh, s_pstHelperMatCopy,
            ulDrawMask,
            -1,
            XeRT_HELPERS,
            XeRenderObject::TriangleList,
            255, 0, (LPVOID)ROS_HELPERS,
            XeRenderObject::Common,
            0,
            QMFR_LIGHTED);
    }
    else
    {
        MATH_AddVector(&pt[4], &b, &i);

        pMesh     = Xe_GDIRequestSoftMesh();
        pVertices = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(6 * 3, sizeof(XeSoftVertex));

        ulNbVertex = 0;

        Xe_M_AddLitTriangle( pt[0], pt[1], pt[4] );
        Xe_M_AddLitTriangle( pt[1], pt[2], pt[4] );
        Xe_M_AddLitTriangle( pt[2], pt[3], pt[4] );
        Xe_M_AddLitTriangle( pt[3], pt[0], pt[4] );
        Xe_M_AddLitTriangle( pt[0], pt[2], pt[1] );
        Xe_M_AddTriangle( pt[0], pt[3], pt[2]);

        pMesh->GetStream(0)->pBuffer->Unlock();

        ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulNbVertex);

        g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
            pMesh, s_pstHelperMatCopy,
            ulDrawMask,
            -1,
            XeRT_HELPERS,
            XeRenderObject::TriangleList,
            255, 0, (LPVOID)ROS_HELPERS,
            XeRenderObject::Common,
            0,
            QMFR_LIGHTED);
    }
}

static void Xe_GDIDrawSoftEllipse(SOFT_tdst_Ellipse* _pst_Ellipse)
{
    ERR_X_Assert(_pst_Ellipse != NULL);

    XeMesh*       pMesh     = NULL;
    XeSoftVertex* pVertices = NULL;
    ULONG ulDrawMask = 0xffffffff & ~(GDI_Cul_DM_ZTest);
    ULONG ulNbVertex;
    float f_Alpha;
    MATH_tdst_Vector A, B, C, M, N;
    ULONG c, c0, c1;

    if (!(_pst_Ellipse->l_Flag & 8))
    {
        // SC: No depth testing, nothing to do
    }

    MATH_CopyVector(&C, (MATH_tdst_Vector *) &_pst_Ellipse->st_Center);
    c1 = _pst_Ellipse->st_Center.color;
    c0 = XeAttenuateColor(c1);

    // Draw ellipse
    {
        ULONG ulMaxPoints = (ULONG)(Cf_2Pi / 0.1f) + 1;
        ULONG ulTemp;

        pMesh      = Xe_GDIRequestSoftMesh();
        pVertices  = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(2 * ulMaxPoints, sizeof(XeSoftVertex));
        ulNbVertex = 0;

        Xe_M_GetEllipsePoint(Cf_2Pi - .1f);
        MATH_CopyVector(&M, &N);

        f_Alpha = 0.0f;
        for (ulTemp = 0; ulTemp < ulMaxPoints; ++ulTemp)
        {
            f_Alpha += 0.1f;
            Xe_M_GetEllipsePoint(f_Alpha);

            if((_pst_Ellipse->l_Flag & 1) || ((M.z <= C.z) && (N.z <= C.z)))
            {
                c = XeConvertColor(LIGHT_ul_Interpol2Colors(c0, c1, ((C.z - M.z) + .1f) * 5));
                c = XeAttenuateColor(c);
                Xe_M_AddVertex(M, c);
                c = XeConvertColor(LIGHT_ul_Interpol2Colors(c0, c1, ((C.z - N.z) + .1f) * 5));
                c = XeAttenuateColor(c);
                Xe_M_AddVertex(N, c);
            }

            MATH_CopyVector(&M, &N);
        }

        pMesh->GetStream(0)->pBuffer->Unlock();

        ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulNbVertex);

        g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
            pMesh, s_pstHelperMatCopy,
            ulDrawMask,
            -1,
            XeRT_HELPERS,
            XeRenderObject::LineList,
            255, 0, (LPVOID)ROS_HELPERS,
            XeRenderObject::Common,
            0,
            QMFR_LIGHTED);
    }

    if (!(_pst_Ellipse->l_Flag & 4))
    {
        SOFT_tdst_Square stSquare;
        stSquare.l_Flag = 0;

        if (_pst_Ellipse->l_Flag & 1)
        {
            Xe_M_GetEllipsePoint(_pst_Ellipse->f_AlphaStart);
            MATH_CopyVector((MATH_tdst_Vector*)&stSquare.st_Pos, &N);
            MATH_InitVector(&stSquare.st_A, 0.05f, 0.0f, 0.0f);
            stSquare.st_Pos.color = 0xff404040;
            Xe_GDIDrawSoftSquare(&stSquare);
        }

        if (_pst_Ellipse->l_Flag & 2)
        {
            Xe_M_GetEllipsePoint(_pst_Ellipse->f_AlphaEnd);
            MATH_CopyVector((MATH_tdst_Vector*)&stSquare.st_Pos, &N);
            MATH_InitVector(&stSquare.st_A, 0.05f, 0.0f, 0.0f);
            stSquare.st_Pos.color = 0xffffffff;
            Xe_GDIDrawSoftSquare(&stSquare);
        }
    }
}

static void Xe_GDIDrawSoftSquare(SOFT_tdst_Square* _pst_Square)
{
    ERR_X_Assert(_pst_Square != NULL);

    XeMesh*       pMesh     = NULL;
    XeSoftVertex* pVertices = NULL;
    ULONG ulDrawMask = 0xffffffff & ~(GDI_Cul_DM_ZTest);
    ULONG ulNbVertex;
    MATH_tdst_Vector p, ast_t[2], ast_Sqr[4], a;
    ULONG ul_Color, c;
    float f_Scale;

    MATH_CopyVector(&p, (MATH_tdst_Vector *) &_pst_Square->st_Pos);
    f_Scale = MATH_f_NormVector( &_pst_Square->st_A ) * .15f;
    MATH_InitVector(&ast_Sqr[0], p.x - f_Scale, p.y - f_Scale, p.z);
    MATH_InitVector(&ast_Sqr[1], p.x - f_Scale, p.y + f_Scale, p.z);
    MATH_InitVector(&ast_Sqr[2], p.x + f_Scale, p.y + f_Scale, p.z);
    MATH_InitVector(&ast_Sqr[3], p.x + f_Scale, p.y - f_Scale, p.z);

    {
        // Alpha blended square
        pMesh      = Xe_GDIRequestSoftMesh();
        pVertices  = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(4, sizeof(XeSoftVertex));
        ulNbVertex = 0;

        _pst_Square->st_Pos.color &= 0x80FFFFFF;
        Xe_M_AddVertex(ast_Sqr[0], XeConvertColor(_pst_Square->st_Pos.color));
        Xe_M_AddVertex(ast_Sqr[1], XeConvertColor(_pst_Square->st_Pos.color));
        Xe_M_AddVertex(ast_Sqr[3], XeConvertColor(_pst_Square->st_Pos.color));
        Xe_M_AddVertex(ast_Sqr[2], XeConvertColor(_pst_Square->st_Pos.color));

        pMesh->GetStream(0)->pBuffer->Unlock();

        ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulNbVertex);

        g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
            pMesh, s_pstHelperMatAlpha,
            ulDrawMask,
            -1,
            XeRT_HELPERS,
            XeRenderObject::TriangleStrip,
            255, 0, (LPVOID)ROS_HELPERS,
            XeRenderObject::Common,
            0,
            QMFR_LIGHTED);
    }

    {
        // Wireframe highlight
        pMesh      = Xe_GDIRequestSoftMesh();
        pVertices  = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(5, sizeof(XeSoftVertex));
        ulNbVertex = 0;
        ul_Color   = XeConvertColor(0xFF000000);

        Xe_M_AddVertex(ast_Sqr[0], XeConvertColor(ul_Color));
        Xe_M_AddVertex(ast_Sqr[1], XeConvertColor(ul_Color));
        Xe_M_AddVertex(ast_Sqr[2], XeConvertColor(ul_Color));
        Xe_M_AddVertex(ast_Sqr[3], XeConvertColor(ul_Color));
        Xe_M_AddVertex(ast_Sqr[0], XeConvertColor(ul_Color));

        pMesh->GetStream(0)->pBuffer->Unlock();

        ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulNbVertex);

        g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
            pMesh, s_pstHelperMatCopy,
            ulDrawMask,
            -1,
            XeRT_HELPERS,
            XeRenderObject::LineStrip,
            255, 0, (LPVOID)ROS_HELPERS,
            XeRenderObject::Common,
            0,
            QMFR_LIGHTED);
    }

    if(_pst_Square->l_Flag & 2)
    {
        MATH_ScaleVector(&a, &_pst_Square->st_A, 0.8f);
        MATH_AddVector(&ast_t[0], &p, &a);
        MATH_ScaleVector(&a, &_pst_Square->st_B, 0.8f);
        MATH_AddVector(&ast_t[1], &p, &a);

        {
            pMesh      = Xe_GDIRequestSoftMesh();
            pVertices  = (XeSoftVertex*)pMesh->GetStream(0)->pBuffer->Lock(6, sizeof(XeSoftVertex));
            ulNbVertex = 0;
            c          = 0x40ffff00;

            Xe_M_AddTriangle(p, ast_t[0], ast_t[1]);
            Xe_M_AddTriangle(p, ast_t[1], ast_t[0]);

            pMesh->GetStream(0)->pBuffer->Unlock();

            ((XeDynVertexBuffer*)pMesh->GetStream(0)->pBuffer)->SetVertexCount(ulNbVertex);

            g_oXeRenderer.QueueMeshForRender(&MATH_gst_IdentityMatrix,
                pMesh, s_pstHelperMatAlpha,
                ulDrawMask,
                -1,
                XeRT_HELPERS,
                XeRenderObject::TriangleList,
                255, 0, (LPVOID)ROS_HELPERS,
                XeRenderObject::Common,
                0,
                QMFR_LIGHTED);
        }
    }
}

const FLOAT C_f_Xe_ZOffset = 0.00005f;

static void Xe_GDIDrawPointEx(GDI_tdst_Request_DrawPointEx* _pst_Data)
{
    D3DXMATRIX* pstCurWorldView = g_pXeContextManagerEngine->GetCurrentWorldViewMatrix();;
    D3DXMATRIX* pstCurProj      = g_pXeContextManagerEngine->GetCurrentProjectionMatrix();
    FLOAT       fScaleX;
    FLOAT       fScaleY;
    UINT        uiWidth;
    UINT        uiHeight;

    // The point scale is in screen space, convert to world space
    g_oXeRenderer.GetViewportResolution(&uiWidth, &uiHeight);
    fScaleX = _pst_Data->f_Size / (FLOAT)uiWidth;
    fScaleY = _pst_Data->f_Size / (FLOAT)uiHeight;

    D3DXVECTOR3 stOrigin;
    D3DXVECTOR3 stOriginScreen(_pst_Data->A->x, _pst_Data->A->y, _pst_Data->A->z);
    D3DXMATRIX  stWVP;
    D3DXMatrixMultiply(&stWVP, pstCurWorldView, pstCurProj);
    D3DXVec3TransformCoord(&stOrigin, &stOriginScreen, &stWVP);

    if (stOrigin.z >= 0.0f)
    {
        if (stOrigin.z > C_f_Xe_ZOffset) stOrigin.z -= C_f_Xe_ZOffset;

        g_oXeRenderer.Add2DQuadRequest(stOrigin.x - fScaleX, stOrigin.y - fScaleY,
            stOrigin.x + fScaleX, stOrigin.y + fScaleY,
            stOrigin.z, _pst_Data->ul_Color);
    }
}

static void Xe_GDIDrawLineEx(GDI_tdst_Request_DrawLineEx* _pst_Data)
{
    D3DXMATRIX* pstCurWorldView = g_pXeContextManagerEngine->GetCurrentWorldViewMatrix();
    D3DXMATRIX* pstCurProj      = g_pXeContextManagerEngine->GetCurrentProjectionMatrix();

    D3DXVECTOR4 stPtAT, stPtBT;
    D3DXVECTOR4 stPtA(_pst_Data->A->x, _pst_Data->A->y, _pst_Data->A->z, 1.0f);
    D3DXVECTOR4 stPtB(_pst_Data->B->x, _pst_Data->B->y, _pst_Data->B->z, 1.0f);
    D3DXMATRIX  stWVP;
    D3DXMatrixMultiply(&stWVP, pstCurWorldView, pstCurProj);
    D3DXVec4Transform(&stPtAT, &stPtA, &stWVP);
    D3DXVec4Transform(&stPtBT, &stPtB, &stWVP);

    if (stPtAT.z >= C_f_Xe_ZOffset) stPtAT.z -= C_f_Xe_ZOffset;
    if (stPtBT.z >= C_f_Xe_ZOffset) stPtBT.z -= C_f_Xe_ZOffset;

    g_oXeRenderer.Add2DLineRequest(stPtAT.x, stPtAT.y, stPtAT.z, stPtAT.w, 
        stPtBT.x, stPtBT.y, stPtBT.z, stPtBT.w, 
        _pst_Data->ul_Color);

#if defined(SC_DEV)
    if(_pst_Data->ul_Flags & GDI_Request_DrawLine_Arrow)
    {
        XeValidate(FALSE, "GDI_Request_DrawLine_Arrow - Not yet implemented");
    }
#endif
}

static void Xe_GDIDrawLine(MATH_tdst_Vector** _pst_Vectors)
{
    GDI_tdst_Request_DrawLineEx stLineEx;

    stLineEx.A        = _pst_Vectors[0];
    stLineEx.B        = _pst_Vectors[1];
    stLineEx.ul_Flags = 0;
    stLineEx.ul_Color = GDI_gpst_CurDD->ul_ColorConstant;

    Xe_GDIDrawLineEx(&stLineEx);
}

static void Xe_GDIDrawQuad(MATH_tdst_Vector** _pst_Vectors)
{
    D3DXMATRIX* pstCurWorldView = g_pXeContextManagerEngine->GetCurrentWorldViewMatrix();
    D3DXMATRIX* pstCurProj      = g_pXeContextManagerEngine->GetCurrentProjectionMatrix();
    D3DXMATRIX  stWVP;
    D3DXVECTOR4 astVertices[4];

    D3DXMatrixMultiply(&stWVP, pstCurWorldView, pstCurProj);
    for (ULONG i = 0; i < 4; ++i)
    {
        D3DXVECTOR4 stPos(_pst_Vectors[i]->x, _pst_Vectors[i]->y, _pst_Vectors[i]->z, 1.0f);
        D3DXVec4Transform(&astVertices[i], &stPos, &stWVP);
    }

    g_oXeRenderer.Add3DQuadRequest(astVertices, GDI_gpst_CurDD->ul_ColorConstant);
}

void Xe_GDIDrawQuadEx(GDI_tdst_Request_DrawQuad* _pst_Data)
{
    GDI_tdst_Request_DrawLineEx st_Line;
    MATH_tdst_Vector vCenter;
    MATH_tdst_Vector vSide;
    MATH_tdst_Vector vU;
    MATH_tdst_Vector vV;
    MATH_tdst_Vector vNormal;
    FLOAT fLength;
    ULONG i;
    ULONG ulColorBK = GDI_gpst_CurDD->ul_ColorConstant;

    if (_pst_Data->ul_Flags & GDI_Request_DrawQuad_Normal)
    {
        MATH_InitVectorToZero(&vCenter);
        for (i = 0; i < 4; ++i)
        {
            MATH_AddEqualVector(&vCenter, _pst_Data->V[i]);
        }
        MATH_ScaleEqualVector(&vCenter, 0.25f);

        MATH_SubVector(&vU, _pst_Data->V[1], _pst_Data->V[0]);
        MATH_SubVector(&vV, _pst_Data->V[3], _pst_Data->V[0]);
        MATH_CrossProduct(&vNormal, &vU, &vV);
        fLength = MATH_f_NormVector(&vNormal);

        if (fLength != 0.0f)
        {
            MATH_ScaleEqualVector(&vNormal, 1.0f / fLength);
            MATH_AddVector(&vSide, &vCenter, &vNormal);
            st_Line.A        = &vCenter;
            st_Line.B        = &vSide;
            st_Line.f_Width  = _pst_Data->f_LineWidth;
            st_Line.ul_Color = ~_pst_Data->ul_EdgeColor;
            st_Line.ul_Flags = GDI_Request_DrawVector_Arrow;
            Xe_GDIDrawLineEx(&st_Line);
        }
    }

    if(!(_pst_Data->ul_Flags & GDI_Request_DrawQuad_OnlyLine))
    {
        MATH_tdst_Vector* pavVec[4];

        GDI_gpst_CurDD->ul_ColorConstant = _pst_Data->ul_SolidColor;
        pavVec[0] = _pst_Data->V[0];
        pavVec[1] = _pst_Data->V[1];
        pavVec[2] = _pst_Data->V[2];
        pavVec[3] = _pst_Data->V[3];
        Xe_GDIDrawQuad(pavVec);
    }

    if(_pst_Data->ul_Flags & (GDI_Request_DrawQuad_OnlyLine | GDI_Request_DrawQuad_Line))
    {
        GDI_gpst_CurDD->ul_ColorConstant = _pst_Data->ul_EdgeColor;

        for (i = 0; i < 4; ++i)
        {
            MATH_tdst_Vector* pavVec[2];

            pavVec[0] = _pst_Data->V[i];
            pavVec[1] = _pst_Data->V[(i + 1) % 4];
            Xe_GDIDrawLine(pavVec);
        }
    }

    GDI_gpst_CurDD->ul_ColorConstant = ulColorBK;
}

#endif // defined(ACTIVE_EDITORS)

void Xe_DebugInfo()
{
#if defined(ACTIVE_EDITORS)
    HDC hDC;
    char string[1024];

    if (GDI_gpst_CurDD->DrawGraphicDebugInfo && !(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DoNotRender) )
    {
        COLORREF OldTextColor;        
        int OldBkgMode;
        u32 TRI_ALarm = GDI_gpst_CurDD->TRI_ALarm;
        u32 SPG_ALarm = GDI_gpst_CurDD->SPG_ALarm;
        hDC = GetDC(GDI_gpst_CurDD->h_Wnd);

        OldBkgMode = GetBkMode(hDC);
        OldTextColor = GetTextColor(hDC);

        SetBkMode(hDC,TRANSPARENT);    
        SetTextColor(hDC,RGB(255,255,255));

        if (g_oXeRenderer.GetNumTriangles() > TRI_ALarm)
        {             
            SetTextColor(hDC,RGB(255,0,0));
        }
        sprintf(string,"Num of tris : %i", g_oXeRenderer.GetNumTriangles() );
        TextOut(hDC,0,0,string,strlen(string));

        SetTextColor(hDC,RGB(255,255,255));

        if (g_oXeRenderer.GetNumSPG2() > SPG_ALarm)
        {
            SetTextColor(hDC,RGB(255,0,0));
        }
        sprintf(string,"Num of SPG2 : %i ", g_oXeRenderer.GetNumSPG2() );
        TextOut(hDC,0,16,string,strlen(string));    

		SetTextColor(hDC,RGB(255,255,255));

		//Add Info about the highlight mode enabled
		switch(GDI_gpst_CurDD->ul_HighlightMode)
		{
		case QMFR_EDINFO_TOO_MANY_LIGHTS:
			strcpy(string, "Highlight Mode - Too Many Lights");
			TextOut(hDC, 0, 32, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[0] & 0x00FFFFFF));
			strcpy(string, "> 4 Lights");
			TextOut(hDC, 0, 48, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[1] & 0x00FFFFFF));
			strcpy(string, "= 4 Lights");
			TextOut(hDC, 0, 64, string, strlen(string));
			break;

		case QMFR_EDINFO_BR_OR_INTEGRATE:
			strcpy(string, "Highlight Mode - Merged Data");
			TextOut(hDC, 0, 32, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[0] & 0x00FFFFFF));
			strcpy(string, "New");
			TextOut(hDC, 0, 48, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[1] & 0x00FFFFFF));
			strcpy(string, "Modified");
			TextOut(hDC, 0, 64, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[2] & 0x00FFFFFF));
			strcpy(string, "PS2 Mesh !CRC");
			TextOut(hDC, 0, 80, string, strlen(string));
			break;

		case QMFR_EDINFO_PIXEL_LIGHTED:
			strcpy(string, "Highlight Mode - Pixel lighted");
			TextOut(hDC, 0, 32, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[0] & 0x00FFFFFF));
			strcpy(string, "Force RTL enable");
			TextOut(hDC, 0, 48, string, strlen(string));
			break;

		case QMFR_EDINFO_CAST_REC_SHADOW:
			strcpy(string, "Highlight Mode - Cast / Receive Shadows");
			TextOut(hDC, 0, 32, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[0] & 0x00FFFFFF));
			strcpy(string, "Cast and Receive");
			TextOut(hDC, 0, 48, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[1] & 0x00FFFFFF));
			strcpy(string, "Cast");
			TextOut(hDC, 0, 64, string, strlen(string));

			SetTextColor(hDC, (COLORREF) (GDI_gpst_CurDD->st_HighlightColors.ul_Colors[2] & 0x00FFFFFF));
			strcpy(string, "Receive");
			TextOut(hDC, 0, 80, string, strlen(string));
			break;

		default:
			break;
		}

        SetTextColor(hDC,OldTextColor);
        SetBkMode(hDC,OldBkgMode);
        ReleaseDC(GDI_gpst_CurDD->h_Wnd,hDC);
    }
#endif
}
