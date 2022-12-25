// Dx9init.c


#include "Dx9init.h"
#include "Dx9renderstate.h"
#include "Dx9samplerstate.h"
#include "Dx9texturestagestate.h"
#include "Dx9matrix.h"
#include "Dx9blending.h"
#include "Dx9gauges.h"
#include "Dx9init.h"
#include "Dx9water.h"
#include "d3dx9core.h"
#include "BASe/MEMory/MEM.h"
#include <assert.h>
#include "AfterFx\Dx9AfterFX_Def.h"
#include "Dx9Error.h"

#ifdef _PC_MEMORY_CHECK
#include "../Main/MainPC/Sources/MemoryTracker.h"
#endif //_PC_MEMORY_CHECK

/************************************************************************************************************************
    Global variables
 ************************************************************************************************************************/
Dx9_tdst_SpecificData	gDx9SpecificData;

/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

//////////////////////////////////
Dx9_tdst_SpecificData *Dx9_pst_CreateDevice(void)
{
	gDx9SpecificData.pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	return &gDx9SpecificData;
}

//////////////////////////////////
void	Dx9_DestroyDevice( void *pSD )
{
	if(pSD != &gDx9SpecificData) 
		return;

    IDirect3D9_Release( gDx9SpecificData.pD3D );
	memset(&gDx9SpecificData, 0, sizeof(Dx9_tdst_SpecificData));
}

//////////////////////////////////

static void _Close_RenderTarget(void);
static void _Close_RenderSurfaces(void);

LONG	Dx9_l_Close( GDI_tdst_DisplayData *pDD )
{
	(void)pDD;

	_Close_RenderSurfaces();
	_Close_RenderTarget();

	if ( gDx9SpecificData.pVertexDeclaration )
	{
		// Release Vertex Declaration
		IDirect3DVertexDeclaration9_Release( gDx9SpecificData.pVertexDeclaration );
		gDx9SpecificData.pVertexDeclaration = NULL;
	}
	if ( gDx9SpecificData.pVertexDeclarationWithUV )
	{
		// Release Vertex Declaration
		IDirect3DVertexDeclaration9_Release( gDx9SpecificData.pVertexDeclarationWithUV );
		gDx9SpecificData.pVertexDeclarationWithUV = NULL;
	}

	if ( gDx9SpecificData.positionVB.pVB )
	{
		// Release Position Circular VB
		IDirect3DVertexBuffer9_Release( gDx9SpecificData.positionVB.pVB );
		gDx9SpecificData.positionVB.pVB = NULL;
		gDx9SpecificData.positionVB.FVF = 0;
		gDx9SpecificData.positionVB.stride = 0;
		gDx9SpecificData.positionVB.nextIndex = 0;
		gDx9SpecificData.positionVB.currIndex = 0;
		gDx9SpecificData.positionVB.pCurrGO = NULL;
	}

	if ( gDx9SpecificData.normalVB.pVB )
	{
		// Release normal Circular VB
		IDirect3DVertexBuffer9_Release( gDx9SpecificData.normalVB.pVB );
		gDx9SpecificData.normalVB.pVB = NULL;
		gDx9SpecificData.normalVB.FVF = 0;
		gDx9SpecificData.normalVB.stride = 0;
		gDx9SpecificData.normalVB.nextIndex = 0;
		gDx9SpecificData.normalVB.currIndex = 0;
		gDx9SpecificData.normalVB.pCurrGO = NULL;
	}

	if ( gDx9SpecificData.colorVB.pVB )
	{
		// Release color Circular VB
		IDirect3DVertexBuffer9_Release( gDx9SpecificData.colorVB.pVB );
		gDx9SpecificData.colorVB.pVB = NULL;
		gDx9SpecificData.colorVB.FVF = 0;
		gDx9SpecificData.colorVB.stride = 0;
		gDx9SpecificData.colorVB.nextIndex = 0;
		gDx9SpecificData.colorVB.currIndex = 0;
		gDx9SpecificData.colorVB.pCurrGO = NULL;
	}

	if ( gDx9SpecificData.m_VertexBuffer )
	{
		// Release VB
		IDirect3DVertexBuffer9_Release( gDx9SpecificData.m_VertexBuffer );
		gDx9SpecificData.m_VertexBuffer = NULL;
	}

	if ( gDx9SpecificData.pD3DDevice )
	{
		// Release Device
		IDirect3DDevice9_Release( gDx9SpecificData.pD3DDevice );
		gDx9SpecificData.pD3DDevice = NULL;
	}

	// uninit drawing of reflection texture for water
	Dx9_UninitDrawReflection();

	// release Occlusion Test interface
	if ( gDx9SpecificData.pOcclusionQuery )
		IDirect3DQuery9_Release( gDx9SpecificData.pOcclusionQuery );

	return D3D_OK;
}

static void _Close_RenderTarget(void)
{
	if(gDx9SpecificData.pImageRenderTarget) //one for all
	{
		IDirect3DTexture9_Release(gDx9SpecificData.pImageRenderTarget);
		gDx9SpecificData.pImageRenderTarget = NULL;
		IDirect3DSurface9_Release(gDx9SpecificData.pDepthStencilSurface);
		gDx9SpecificData.pDepthStencilSurface = NULL;
	}
}

static void _Close_RenderSurfaces(void)
{
	if(gDx9SpecificData.pImageFrameSurface[0]) //one for all
	{
		int i;
		for(i=0; i<2; i++)
		{
			IDirect3DSurface9_Release(gDx9SpecificData.pImageFrameSurface[i]);
			gDx9SpecificData.pImageFrameSurface[i] = NULL;
			IDirect3DSurface9_Release(gDx9SpecificData.pDepthSurfaceFrameSurface[i]);
			gDx9SpecificData.pDepthSurfaceFrameSurface[i] = NULL;
		}
	}
}

//////////////////////////////////

static void _Init_RenderTarget(void);
static void _Init_RenderSurfaces(void);

__inline bool GetDwordFromRegistry(HKEY hkey, const char* name, void* data)
{
	DWORD type, size = 4;
	return RegQueryValueEx(hkey, name, 0, &type, (BYTE*)data, &size) == ERROR_SUCCESS
		&& type == REG_DWORD && size == 4;
}

LONG	Dx9_l_Init(HWND hWnd, GDI_tdst_DisplayData *pDD)
{
	HKEY			hkey;
	DWORD			mode;
	D3DDISPLAYMODE	displayMode;
	HANDLE			kernel32;
    BOOL			(__stdcall *IsDebuggerPresent)(void);

    // Cleanup any objects that might've been created before
    if ( Dx9_l_Close( pDD ) != D3D_OK ) 
		return E_FAIL;

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Ubi Soft\\BGE PC\\SettingsApplication.INI\\Basic video", 0, KEY_READ, &hkey) != ERROR_SUCCESS
		|| !GetDwordFromRegistry(hkey, "Video Adapter",			&gDx9SpecificData.Adapter)
		|| !GetDwordFromRegistry(hkey, "Format",				&gDx9SpecificData.AdapterFormat)
		|| !GetDwordFromRegistry(hkey, "Z Buffer depth",		&gDx9SpecificData.DepthStencilFormat)
		|| !GetDwordFromRegistry(hkey, "Mode",					&mode)
		|| !GetDwordFromRegistry(hkey, "Multisample type",		&gDx9SpecificData.d3dPresentParameters.MultiSampleType)
		|| !GetDwordFromRegistry(hkey, "Antialiasing quality",	&gDx9SpecificData.d3dPresentParameters.MultiSampleQuality))
	{
		return E_FAIL;
	}

	RegCloseKey(hkey);

	kernel32 = LoadLibrary("kernel32");
	IsDebuggerPresent = GetProcAddress(kernel32, "IsDebuggerPresent");
	if(IsDebuggerPresent && IsDebuggerPresent())
	{
		CHK_D3D(IDirect3D9_GetAdapterDisplayMode(gDx9SpecificData.pD3D, gDx9SpecificData.Adapter, &displayMode));
		gDx9SpecificData.AdapterFormat = displayMode.Format;
		gDx9SpecificData.d3dPresentParameters.Windowed = TRUE;
	    GetClientRect( hWnd, &gDx9SpecificData.viewportRect );
	}
	else
	{
		CHK_D3D(IDirect3D9_EnumAdapterModes(gDx9SpecificData.pD3D, gDx9SpecificData.Adapter, gDx9SpecificData.AdapterFormat, mode, &displayMode));
		gDx9SpecificData.d3dPresentParameters.Windowed = FALSE;
		gDx9SpecificData.d3dPresentParameters.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
		gDx9SpecificData.viewportRect.right = displayMode.Width;
		gDx9SpecificData.viewportRect.bottom = displayMode.Height;
	}

    gDx9SpecificData.hWnd = hWnd;
    gDx9SpecificData.pProjectionMatrix = &pDD->st_Camera.st_ProjectionMatrix;

	// Set Present Parameters
    gDx9SpecificData.d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    gDx9SpecificData.d3dPresentParameters.BackBufferFormat = gDx9SpecificData.AdapterFormat;
    gDx9SpecificData.d3dPresentParameters.BackBufferCount = 0;
    gDx9SpecificData.d3dPresentParameters.hDeviceWindow = hWnd;
    gDx9SpecificData.d3dPresentParameters.BackBufferWidth = gDx9SpecificData.viewportRect.right;
    gDx9SpecificData.d3dPresentParameters.BackBufferHeight = gDx9SpecificData.viewportRect.bottom;
    gDx9SpecificData.d3dPresentParameters.EnableAutoDepthStencil = TRUE;
    gDx9SpecificData.d3dPresentParameters.AutoDepthStencilFormat = gDx9SpecificData.DepthStencilFormat;
	gDx9SpecificData.d3dPresentParameters.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	gDx9SpecificData.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	// multisample flags read from registry above

	// try to create HAL device with hardware vertex processing
	if(gDx9SpecificData.pD3DDevice==NULL)
	{
		if FAILED( IDirect3D9_CreateDevice( gDx9SpecificData.pD3D,
									gDx9SpecificData.Adapter,
									D3DDEVTYPE_HAL,
									hWnd,
									D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
									&gDx9SpecificData.d3dPresentParameters,
									&gDx9SpecificData.pD3DDevice ) )
		{
			// try to create HAL device with software vertex processing
			if FAILED( IDirect3D9_CreateDevice( gDx9SpecificData.pD3D,
													gDx9SpecificData.Adapter,
													D3DDEVTYPE_HAL,
													hWnd,
													D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
													&gDx9SpecificData.d3dPresentParameters,
													&gDx9SpecificData.pD3DDevice ) )
			{
				// Unable to create Device
				assert( FALSE );
			}
		}	
	}

	if (FAILED(IDirect3DDevice9_SetDialogBoxMode(gDx9SpecificData.pD3DDevice, TRUE)))
	{
		OutputDebugString("SetDialogBoxMode failed\n");
	}

	if FAILED( IDirect3D9_GetDeviceCaps( gDx9SpecificData.pD3D, gDx9SpecificData.Adapter, D3DDEVTYPE_HAL, &( gDx9SpecificData.d3dCaps ) ) )
	{
		// Unable to get Device Caps
		assert( FALSE );
	}

    // clear the two buffers
    IDirect3DDevice9_Clear( gDx9SpecificData.pD3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    IDirect3DDevice9_Present( gDx9SpecificData.pD3DDevice, NULL, NULL, NULL, NULL );
    IDirect3DDevice9_Clear( gDx9SpecificData.pD3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// create vertex buffer
	IDirect3DDevice9_CreateVertexBuffer( gDx9SpecificData.pD3DDevice,
										SOFT_Cul_ComputingBufferSize * sizeof(Dx9VertexFormat) * 3,
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										D3DFVF_VERTEXF,
										D3DPOOL_DEFAULT,
										&gDx9SpecificData.m_VertexBuffer,
										NULL );


	{
		Dx9_tdst_CircularVB		* pCircVB;

		// create Position Circular VB
		pCircVB = &gDx9SpecificData.positionVB;
		pCircVB->nextIndex = 0;
		pCircVB->currIndex = 0;
		pCircVB->pCurrGO = NULL;
		pCircVB->FVF = D3DFVF_XYZ;
		pCircVB->stride = D3DXGetFVFVertexSize( pCircVB->FVF );
		IDirect3DDevice9_CreateVertexBuffer( gDx9SpecificData.pD3DDevice,
											CIRCULAR_VB_SIZE * pCircVB->stride,
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
											pCircVB->FVF,
											D3DPOOL_DEFAULT,
											&pCircVB->pVB,
											NULL );

		// create Normal Circular VB
		pCircVB = &gDx9SpecificData.normalVB;
		pCircVB->nextIndex = 0;
		pCircVB->currIndex = 0;
		pCircVB->pCurrGO = NULL;
		pCircVB->FVF = D3DFVF_NORMAL;
		pCircVB->stride = D3DXGetFVFVertexSize( pCircVB->FVF );
		IDirect3DDevice9_CreateVertexBuffer( gDx9SpecificData.pD3DDevice,
											CIRCULAR_VB_SIZE * pCircVB->stride,
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
											pCircVB->FVF,
											D3DPOOL_DEFAULT,
											&pCircVB->pVB,
											NULL );

		// create Color Circular VB
		pCircVB = &gDx9SpecificData.colorVB;
		pCircVB->nextIndex = 0;
		pCircVB->currIndex = 0;
		pCircVB->pCurrGO = NULL;
		pCircVB->FVF = D3DFVF_DIFFUSE;
		pCircVB->stride = D3DXGetFVFVertexSize( pCircVB->FVF );
		IDirect3DDevice9_CreateVertexBuffer( gDx9SpecificData.pD3DDevice,
											CIRCULAR_VB_SIZE * pCircVB->stride,
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
											pCircVB->FVF,
											D3DPOOL_DEFAULT,
											&pCircVB->pVB,
											NULL );
		{
			void *pColorData;

			IDirect3DVertexBuffer9_Lock( pCircVB->pVB, 0, CIRCULAR_VB_SIZE * pCircVB->stride, &pColorData, 0 );
			memset( pColorData, 0x7f7f7f7f, CIRCULAR_VB_SIZE * pCircVB->stride );
			IDirect3DVertexBuffer9_Unlock( pCircVB->pVB );
		}

	}

	{
		D3DVERTEXELEMENT9 vertexElements[] =
		{
			{ 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0}, 
			{ 1,  0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0}, 
			D3DDECL_END()
		};
		
		IDirect3DDevice9_CreateVertexDeclaration( gDx9SpecificData.pD3DDevice, vertexElements, &gDx9SpecificData.pVertexDeclaration );
	}

	{
		D3DVERTEXELEMENT9 vertexElements[] =
		{
			{ 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0}, 
			{ 1,  0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0}, 
			{ 2,  0, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0}, 
//			{ 0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0}, 
			D3DDECL_END()
		};
		
		IDirect3DDevice9_CreateVertexDeclaration( gDx9SpecificData.pD3DDevice, vertexElements, &gDx9SpecificData.pVertexDeclarationWithUV );
	}

	// Init Display Data
    GDI_gpst_CurDD = pDD;
    pDD->LastBlendingMode = 0;

	{	// Set Viewport
		LONG w = gDx9SpecificData.viewportRect.right - gDx9SpecificData.viewportRect.left;
		LONG h = gDx9SpecificData.viewportRect.bottom - gDx9SpecificData.viewportRect.top;
		Dx9_SetViewport( 0, (h - w) / 2, w, w );		// center vertically the viewport
	}

	// Establish clipping volume (left, right, bottom, top, near, far)
	gDx9SpecificData.pProjectionMatrix->Jy	= -1.0f;
	gDx9SpecificData.pProjectionMatrix->Sz	=  1.0f;
	gDx9SpecificData.pProjectionMatrix->T.z = -0.1f;
	gDx9SpecificData.pProjectionMatrix->w	=  0.0f;	// MATRIX W!

	// Init render states
    Dx9_InitRenderStates( );

	// Init sampler states
    Dx9_InitSamplerStates( );

	// Init texture stage states
    Dx9_InitTextureStageStates( );

	// Init Occlusion test
	gDx9SpecificData.pOcclusionQuery = NULL;
	IDirect3DDevice9_CreateQuery( gDx9SpecificData.pD3DDevice, D3DQUERYTYPE_OCCLUSION, &gDx9SpecificData.pOcclusionQuery );

	// init texture blending
    Dx9_SetTextureBlending( 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 );

	// init Projection Matrix 
	Dx9_SetProjectionMatrix( &pDD->st_Camera );

	// init drawing of reflection texture for water
	Dx9_InitDrawReflection();

	// force Z-test
    Dx9_RS_ZEnable( TRUE );

	// init UV Type 
	gDx9SpecificData.UVType = Dx9_eUVOff; 
	
	_Init_RenderTarget();
	_Init_RenderSurfaces();

	// AfterFx initialization
	AE_Init(&gDx9SpecificData);

	// Disable D3DSpy on release builds, if present
#ifndef _DEBUG
	{
        HMODULE hModule = LoadLibrary("d3d9.dll");
		if(hModule)
		{
			void (*D3DSpyDisable)(void);
			D3DSpyDisable = (void(*)(void))GetProcAddress(hModule, "DisableD3DSpy");
			if(D3DSpyDisable)
				D3DSpyDisable();
		}
    }
#endif

    return D3D_OK;
}

static void _Init_RenderTarget(void)
{
	D3DSURFACE_DESC		imageBackBufferDesc;
	D3DSURFACE_DESC		depthStencilBackBufferDesc;

	{
		IDirect3DSurface9*	pImageBackBuffer;
		IDirect3DDevice9_GetBackBuffer(
			gDx9SpecificData.pD3DDevice,
			0, //swap chain
			0, //buffer 0
			D3DBACKBUFFER_TYPE_MONO,
			&pImageBackBuffer
		);
		IDirect3DSurface9_GetDesc(
			pImageBackBuffer,
			&imageBackBufferDesc
		);
		IDirect3DSurface9_Release(
			pImageBackBuffer
		);
	}

	{
		IDirect3DSurface9*	pDepthStencilBackBuffer;
		IDirect3DDevice9_GetDepthStencilSurface(
			gDx9SpecificData.pD3DDevice,
			&pDepthStencilBackBuffer
		);
		IDirect3DSurface9_GetDesc(
			pDepthStencilBackBuffer,
			&depthStencilBackBufferDesc
		);		
		IDirect3DSurface9_Release(
			pDepthStencilBackBuffer
		);
	}

	{
		int					width;
		int					height;

		width = imageBackBufferDesc.Width;
		height = imageBackBufferDesc.Height;
		/*
		//select a pow2 texture size
		{
			int i;
			for(i = 0; i<=14; i++) //2>>14==4096
			{
				if(width < (2<<i))
				{
					width = 2<<i;
					break;
				}
			}
			for(i = 0; i<=14; i++) //2>>14==4096
			{
				if(height < (2<<i))
				{
					height = 2<<i;
					break;
				}
			}
		}
		*/

		//create image texture
		CHK_D3D(IDirect3DDevice9_CreateTexture(
			gDx9SpecificData.pD3DDevice,
			width, 
			height,  
			1, //levels
			D3DUSAGE_RENDERTARGET,
			imageBackBufferDesc.Format,  
			D3DPOOL_DEFAULT,  
			&gDx9SpecificData.pImageRenderTarget,
			NULL //handle
		));

		//create depthstencil surface
		// ATTENTION for Valentino: DepthStencil surface MUST be created with
		// CreateDepthStencilSurface. CreateTexture fails on certain hardware (ex. GeForce2)
		CHK_D3D(IDirect3DDevice9_CreateDepthStencilSurface(
			gDx9SpecificData.pD3DDevice,
			width,
			height,
			depthStencilBackBufferDesc.Format,
			depthStencilBackBufferDesc.MultiSampleType,
			depthStencilBackBufferDesc.MultiSampleQuality,
			TRUE,
			&gDx9SpecificData.pDepthStencilSurface,
			NULL //handle
		));
	}
}

static void _Init_RenderSurfaces(void)
{
	IDirect3DDevice9_GetBackBuffer(
		gDx9SpecificData.pD3DDevice,
		0, //swap chain
		0, //buffer 0
		D3DBACKBUFFER_TYPE_MONO,
		&gDx9SpecificData.pImageFrameSurface[0]
	);
	IDirect3DDevice9_GetDepthStencilSurface(
		gDx9SpecificData.pD3DDevice,
		&gDx9SpecificData.pDepthSurfaceFrameSurface[0]
	);
	IDirect3DDevice9_Present( 
		gDx9SpecificData.pD3DDevice, 
		NULL, NULL, NULL, NULL 
	);
	IDirect3DDevice9_GetBackBuffer(
		gDx9SpecificData.pD3DDevice,
		0, //swap chain
		0, //buffer 0
		D3DBACKBUFFER_TYPE_MONO,
		&gDx9SpecificData.pImageFrameSurface[1]
	);
	IDirect3DDevice9_GetDepthStencilSurface(
		gDx9SpecificData.pD3DDevice,
		&gDx9SpecificData.pDepthSurfaceFrameSurface[1]
	);
}

////////////////
LONG	Dx9_l_ReadaptDisplay( HWND hWnd, GDI_tdst_DisplayData *pDD )
{
	// TODO: what to do here?
	return 0;// Dx9_l_Init( hWnd ? hWnd : gDx9SpecificData.hWnd, pDD );
}

////////////////
void	Dx9_Flip(void)
{
	Dx9_DrawPerformanceGauges();

	Dx9_EndScene();

	IDirect3DDevice9_Present( gDx9SpecificData.pD3DDevice, NULL, NULL, NULL, NULL );
}

////////////////
void	Dx9_Clear(LONG buffer, ULONG color)
{
	DWORD	dw_Buffer;

	if( buffer== GDI_Cl_ZBuffer )
	{
		IDirect3DDevice9_Clear(gDx9SpecificData.pD3DDevice, 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	}
	else
	{
		color = Dx9_M_ConvertColor(color);
		dw_Buffer = D3DCLEAR_TARGET | ( ( buffer & GDI_Cl_ZBuffer ) ? D3DCLEAR_ZBUFFER : 0 );
		IDirect3DDevice9_Clear( gDx9SpecificData.pD3DDevice, 0, NULL, dw_Buffer, color, 1.0f, 0 );
	}
}

////////////////
static IDirect3DStateBlock9*		gsSavedStateBlock;
static D3DSTATEBLOCKTYPE			gsSavedStateBlockType;
static Dx9_tdst_RenderState			gsSavedRenderState;
static Dx9_tdst_SamplerState		gsSavedSamplerState[MAX_TEXTURE_STAGE];
static Dx9_tdst_TextureStageState	gsSavedTextureStageState[MAX_TEXTURE_STAGE];

void Dx9_SaveState(D3DSTATEBLOCKTYPE stateBlock)
{
	assert(gsSavedStateBlock == 0);
	IDirect3DDevice9_CreateStateBlock(gDx9SpecificData.pD3DDevice, stateBlock, &gsSavedStateBlock);
	IDirect3DStateBlock9_Capture(gsSavedStateBlock);

	if(stateBlock != D3DSBT_VERTEXSTATE)
	{
		memcpy(&gsSavedSamplerState, &gDx9SpecificData.SamplerState, sizeof(gsSavedSamplerState));
		memcpy(&gsSavedTextureStageState, &gDx9SpecificData.TextureStageState, sizeof(gsSavedTextureStageState));
	}

	if(stateBlock != D3DSBT_PIXELSTATE)
	{
		memcpy(&gsSavedRenderState, &gDx9SpecificData.RenderState, sizeof(gsSavedRenderState));
	}

	gsSavedStateBlockType = stateBlock;
}

void Dx9_RestoreState(void)
{
	assert(gsSavedStateBlock);
	IDirect3DStateBlock9_Apply(gsSavedStateBlock);
	IDirect3DStateBlock9_Release(gsSavedStateBlock);
	gsSavedStateBlock = 0;

	if(gsSavedStateBlockType != D3DSBT_VERTEXSTATE)
	{
		memcpy(&gDx9SpecificData.SamplerState, &gsSavedSamplerState, sizeof(gsSavedSamplerState));
		memcpy(&gDx9SpecificData.TextureStageState, &gsSavedTextureStageState, sizeof(gsSavedTextureStageState));
	}

	if(gsSavedStateBlockType != D3DSBT_PIXELSTATE)
	{
		memcpy(&gDx9SpecificData.RenderState, &gsSavedRenderState, sizeof(gsSavedRenderState));
	}
}

bool Dx9_CheckDeviceFormat(DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	return SUCCEEDED(IDirect3D9_CheckDeviceFormat(
		gDx9SpecificData.pD3D,
		gDx9SpecificData.Adapter,
		D3DDEVTYPE_HAL,
		gDx9SpecificData.AdapterFormat,
		Usage,
		RType,
		CheckFormat));
}

bool Dx9_CheckDepthStencilMatch(D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	return SUCCEEDED(IDirect3D9_CheckDepthStencilMatch(
		gDx9SpecificData.pD3D,
		gDx9SpecificData.Adapter,
		D3DDEVTYPE_HAL,
		gDx9SpecificData.AdapterFormat,
		RenderTargetFormat,
		DepthStencilFormat));
}
