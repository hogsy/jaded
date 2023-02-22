/*$T DX8init.c GC! 1.081 01/16/02 15:38:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include <D3D8.h>

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GDInterface/GDIrasters.h"
#include "DX8init.h"
#include "DX8tex.h"
#include "GEOmetric/GEO_STRIP.h"

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    constant
 ***********************************************************************************************************************
 */

BOOL					DX8_gb_Init = 0;
DX8_tdst_SpecificData	*p_gDX8SpecificData;

/*$4
 ***********************************************************************************************************************
    Private and external function prototype
 ***********************************************************************************************************************
 */

void					DX8_SetDCPixelFormat(HDC);
void					DX8_SetupRC(DX8_tdst_SpecificData *);

extern void				LOA_BeginSpeedMode(BIG_KEY _ul_Key);
extern void				LOA_EndSpeedMode(void);

/*$4
 ***********************************************************************************************************************
    Public Function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_Viewport(DX8_tdst_SpecificData *_pst_SD, LONG x, LONG y, LONG w, LONG h)
{
	if(!(_pst_SD->mp_D3DDevice)) return;

	if(x < 0) x = 0;
	if(y < 0) y = 0;

	if(w + x > (LONG) _pst_SD->mst_D3DPP.BackBufferWidth) w = _pst_SD->mst_D3DPP.BackBufferWidth - x;
	if(h + y > (LONG) _pst_SD->mst_D3DPP.BackBufferHeight) h = _pst_SD->mst_D3DPP.BackBufferHeight - y;

	_pst_SD->V8.X = x;
	_pst_SD->V8.Y = y;
	_pst_SD->V8.Width = w;
	_pst_SD->V8.Height = h;
	_pst_SD->V8.MinZ = 0.0f;
	_pst_SD->V8.MaxZ = 1.0f;
#ifdef JADEFUSION
	_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetViewport(/*_pst_SD->mp_D3DDevice,*/ &_pst_SD->V8);
#else
	_pst_SD->mp_D3DDevice->lpVtbl->SetViewport(_pst_SD->mp_D3DDevice, &_pst_SD->V8);
#endif
}

/* Aim: Create a device */
extern void DX8_l_Texture_CreateShadowTextures(DX8_tdst_SpecificData *);
extern void DX8_l_Texture_DestroyShadowTextures(DX8_tdst_SpecificData *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DX8_tdst_SpecificData *DX8_pst_CreateDevice(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) MEM_p_Alloc(sizeof(DX8_tdst_SpecificData));
	L_memset(pst_SD, 0, sizeof(DX8_tdst_SpecificData));
	p_gDX8SpecificData = pst_SD;

	pst_SD->mp_D3D = Direct3DCreate8(D3D_SDK_VERSION);
	return pst_SD;
}

/*
 =======================================================================================================================
    Aim:    Destroy a device
 =======================================================================================================================
 */
void DX8_DestroyDevice(void *_pst_SD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) _pst_SD;
	if(!pst_SD) return;
#ifdef JADEFUSION
	pst_SD->mp_D3D->/*lpVtbl->*/Release(/*pst_SD->mp_D3D*/);
#else
	pst_SD->mp_D3D->lpVtbl->Release(pst_SD->mp_D3D);
#endif
	MEM_Free(pst_SD);
	p_gDX8SpecificData = NULL;
}

/*
 =======================================================================================================================
    Aim:    Close OpenGL display
 =======================================================================================================================
 */
HRESULT DX8_l_Close(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	DX8_gb_Init = 0;

	pst_SD = (DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	if(pst_SD->m_VertexBuffer != NULL)
	{
#ifdef JADEFUSION
		pst_SD->m_VertexBuffer->/*lpVtbl->*/Release(/*pst_SD->m_VertexBuffer*/);
#else
		pst_SD->m_VertexBuffer->lpVtbl->Release(pst_SD->m_VertexBuffer);
#endif
		pst_SD->m_VertexBuffer = NULL;
	}

	if(pst_SD->mp_D3DDevice != NULL)
	{
		DX8_l_Texture_DestroyShadowTextures(pst_SD);
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/Release(/*pst_SD->mp_D3DDevice*/);
#else
		pst_SD->mp_D3DDevice->lpVtbl->Release(pst_SD->mp_D3DDevice);
#endif
		pst_SD->mp_D3DDevice = NULL;
	}

	return D3D_OK;
}

/*
 =======================================================================================================================
    Aim:    Init OpenGL object
 =======================================================================================================================
 */
LONG DX8_l_Init(HWND _hWnd, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RECT					st_Rect;
	DX8_tdst_SpecificData	*pst_SD;
	HRESULT					hr;
	DWORD					dw_Behavior;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	pst_SD->RenderScene = FALSE;

	/* Cleanup any objects that might've been created before */
	if(DX8_l_Close(_pst_DD) != D3D_OK) return E_FAIL;

	DX8_gb_Init = 1;

	/* Check window size */
	GetClientRect(_hWnd, &st_Rect);
	if((st_Rect.top >= st_Rect.bottom) || (st_Rect.left >= st_Rect.right)) return D3D_OK;

	pst_SD->h_Wnd = _hWnd;
	pst_SD->pst_ProjMatrix = &_pst_DD->st_Camera.st_ProjectionMatrix;
	pst_SD->rcViewportRect = st_Rect;

	/* GetClientRect(_hWnd, &pst_SD->rcViewportRect); */
#ifdef JADEFUSION
	pst_SD->mp_D3D->/*lpVtbl->*/GetAdapterDisplayMode(/*pst_SD->mp_D3D,*/ D3DADAPTER_DEFAULT, &pst_SD->d3ddm);
#else
	pst_SD->mp_D3D->lpVtbl->GetAdapterDisplayMode(pst_SD->mp_D3D, D3DADAPTER_DEFAULT, &pst_SD->d3ddm);
#endif
	ZeroMemory(&pst_SD->mst_D3DPP, sizeof(pst_SD->mst_D3DPP));
	pst_SD->mst_D3DPP.Windowed = TRUE;
	pst_SD->mst_D3DPP.SwapEffect = D3DSWAPEFFECT_FLIP;
	pst_SD->mst_D3DPP.BackBufferFormat = pst_SD->d3ddm.Format;
	pst_SD->mst_D3DPP.BackBufferCount = 0;
	pst_SD->mst_D3DPP.hDeviceWindow = _hWnd;
	pst_SD->mst_D3DPP.BackBufferWidth = st_Rect.right - st_Rect.left;
	pst_SD->mst_D3DPP.BackBufferHeight = st_Rect.bottom - st_Rect.top;
	pst_SD->mst_D3DPP.EnableAutoDepthStencil = TRUE;
	pst_SD->mst_D3DPP.AutoDepthStencilFormat = D3DFMT_D16;
	pst_SD->mst_D3DPP.FullScreen_RefreshRateInHz = 0;
	pst_SD->mst_D3DPP.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	pst_SD->mst_D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;

	/* try to create HAL device with hardware vertex processing */
	dw_Behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
#ifdef JADEFUSION
	hr = pst_SD->mp_D3D->/*lpVtbl->*/CreateDevice
#else
		hr = pst_SD->mp_D3D->lpVtbl->CreateDevice
#endif
			(
#ifndef JADEFUSION
			pst_SD->mp_D3D,
#endif
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			_hWnd,
			dw_Behavior,
			&pst_SD->mst_D3DPP,
			&pst_SD->mp_D3DDevice
		);
	if(hr != D3D_OK)
	{
		/* try to create HAL device with software vertex processing */
		dw_Behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
#ifdef JADEFUSION
		hr = pst_SD->mp_D3D->/*lpVtbl->*/CreateDevice
#else	
		hr = pst_SD->mp_D3D->lpVtbl->CreateDevice
#endif
		(
#ifndef JADEFUSION
				pst_SD->mp_D3D,
#endif				
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				_hWnd,
				dw_Behavior,
				&pst_SD->mst_D3DPP,
				&pst_SD->mp_D3DDevice
			);
	}

	DX8_l_Texture_CreateShadowTextures(pst_SD);

	/* clear the two buffers */
#ifdef JADEFUSION	
	pst_SD->mp_D3DDevice->/*lpVtbl->*/Clear(/*pst_SD->mp_D3DDevice,*/ 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/Present(/*pst_SD->mp_D3DDevice,*/ NULL, NULL, NULL, NULL);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/Clear(/*pst_SD->mp_D3DDevice,*/ 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#else
	pst_SD->mp_D3DDevice->lpVtbl->Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	pst_SD->mp_D3DDevice->lpVtbl->Present(pst_SD->mp_D3DDevice, NULL, NULL, NULL, NULL);
	pst_SD->mp_D3DDevice->lpVtbl->Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#endif
	/* create vertex buffer */
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/CreateVertexBuffer
#else
		pst_SD->mp_D3DDevice->lpVtbl->CreateVertexBuffer
#endif
	(
#ifndef JADEFUSION
	pst_SD->mp_D3DDevice,
#endif
			SOFT_Cul_ComputingBufferSize * sizeof(DX8VertexFormat) * 3,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			D3DFVF_VERTEXF,
			D3DPOOL_DEFAULT,
			&pst_SD->m_VertexBuffer
		);

	/* Select the pixel format */
	DX8_SetupRC(pst_SD);
	DX8_RS_Init(&pst_SD->st_RS);
	GDI_gpst_CurDD = _pst_DD;
	_pst_DD->LastBlendingMode = 0;
	DX8_SetTextureBlending(-1, -1);
	DX8_SetProjectionMatrix(&_pst_DD->st_Camera);
    DX8_RS_DepthTest( pst_SD, 1 );
    DX8_M_RenderState(pst_SD, D3DRS_LIGHTING, FALSE);
	return D3D_OK;
}

/* Aim: Adapt OpenGL display to window */
extern void DX8_Texture_ReleaseAll(GDI_tdst_DisplayData *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HRESULT DX8_l_ReadaptDisplay(HWND _hWnd, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HRESULT					hr;
	DX8_tdst_SpecificData	*pst_SD;
	int						w, h;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	if(!DX8_gb_Init)
	{
		hr = DX8_l_Init(_hWnd ? _hWnd : DX8_M_SD(_pst_DD)->h_Wnd, _pst_DD);
		return hr;
	}

	w = _pst_DD->st_Device.l_Width;
	h = _pst_DD->st_Device.l_Height;

#ifdef JADEFUSION
	if(pst_SD->m_VertexBuffer) pst_SD->m_VertexBuffer->/*lpVtbl->*/Release(/*pst_SD->m_VertexBuffer*/);
#else
	if(pst_SD->m_VertexBuffer) pst_SD->m_VertexBuffer->lpVtbl->Release(pst_SD->m_VertexBuffer);
#endif

	GetClientRect(_hWnd, &pst_SD->rcViewportRect);
	pst_SD->mst_D3DPP.Windowed = TRUE;
	pst_SD->mst_D3DPP.BackBufferFormat = pst_SD->d3ddm.Format;
	pst_SD->mst_D3DPP.BackBufferCount = 1;
	pst_SD->mst_D3DPP.SwapEffect = D3DSWAPEFFECT_FLIP;
	pst_SD->mst_D3DPP.hDeviceWindow = _hWnd;
	pst_SD->mst_D3DPP.BackBufferWidth = w;
	pst_SD->mst_D3DPP.BackBufferHeight = h;

	DX8_l_Texture_DestroyShadowTextures(pst_SD);

#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/Reset(/*pst_SD->mp_D3DDevice,*/ &pst_SD->mst_D3DPP);
#else
	pst_SD->mp_D3DDevice->lpVtbl->Reset(pst_SD->mp_D3DDevice, &pst_SD->mst_D3DPP);
#endif

	DX8_l_Texture_CreateShadowTextures(pst_SD);

	DX8_Viewport(pst_SD, 0, 0, w, h);
	DX8_M_RenderState(pst_SD, D3DRS_LIGHTING, FALSE);
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/Clear(/*pst_SD->mp_D3DDevice,*/ 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/Present(/*pst_SD->mp_D3DDevice,*/ NULL, NULL, NULL, NULL);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/Clear(/*pst_SD->mp_D3DDevice,*/ 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#else
	pst_SD->mp_D3DDevice->lpVtbl->Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	pst_SD->mp_D3DDevice->lpVtbl->Present(pst_SD->mp_D3DDevice, NULL, NULL, NULL, NULL);
	pst_SD->mp_D3DDevice->lpVtbl->Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#endif
	/* DX8_Viewport( pst_SD, 0, (h - w) / 2, w, w); */
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/CreateVertexBuffer
#else
		pst_SD->mp_D3DDevice->lpVtbl->CreateVertexBuffer
#endif
			(
#ifndef JADEFUSION
		pst_SD->mp_D3DDevice,
#endif
		SOFT_Cul_ComputingBufferSize * sizeof(DX8VertexFormat) * 3,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			D3DFVF_VERTEXF,
			D3DPOOL_DEFAULT,
			&pst_SD->m_VertexBuffer
		);

	GDI_gpst_CurDD->LastBlendingMode = 0;
	DX8_SetTextureBlending(-1, -1);

	DX8_SetProjectionMatrix(&_pst_DD->st_Camera);

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_Flip(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

#ifdef JADEFUSION
	if(pst_SD->RenderScene) pst_SD->mp_D3DDevice->/*lpVtbl->*/EndScene(/*pst_SD->mp_D3DDevice*/);

	pst_SD->mp_D3DDevice->/*lpVtbl->*/Present(/*pst_SD->mp_D3DDevice,*/ NULL, NULL, NULL, NULL);
#else
	if(pst_SD->RenderScene) pst_SD->mp_D3DDevice->lpVtbl->EndScene(pst_SD->mp_D3DDevice);
	pst_SD->mp_D3DDevice->lpVtbl->Present(pst_SD->mp_D3DDevice, NULL, NULL, NULL, NULL);
#endif
	pst_SD->RenderScene = FALSE;
}

/*
 =======================================================================================================================
    Aim:    Clear buffer
 =======================================================================================================================
 */
void DX8_Clear(LONG _l_Buffer, ULONG _ul_Color)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	DWORD					dw_Buffer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (DX8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

	if(_l_Buffer == GDI_Cl_ZBuffer)
	{
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/Clear(/*pst_SD->mp_D3DDevice,*/ 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#else
		pst_SD->mp_D3DDevice->lpVtbl->Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#endif
	}
	else
	{
		_ul_Color = DX8_M_ConvertColor(_ul_Color);
		dw_Buffer = D3DCLEAR_TARGET | ((_l_Buffer & GDI_Cl_ZBuffer) ? D3DCLEAR_ZBUFFER : 0);
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/Clear(/*pst_SD->mp_D3DDevice,*/ 0, NULL, dw_Buffer, _ul_Color, 1.0f, 0);
#else
		pst_SD->mp_D3DDevice->lpVtbl->Clear(pst_SD->mp_D3DDevice, 0, NULL, dw_Buffer, _ul_Color, 1.0f, 0);
#endif
        if ( !pst_SD->RenderScene )
        {
#ifdef JADEFUSION
		    pst_SD->mp_D3DDevice->/*lpVtbl->*/BeginScene(/*pst_SD->mp_D3DDevice*/);
#else
			pst_SD->mp_D3DDevice->lpVtbl->BeginScene(pst_SD->mp_D3DDevice);
#endif
			pst_SD->RenderScene = TRUE;
        }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_SetViewMatrix(MATH_tdst_Matrix *_pst_Matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_OGLMatrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!p_gDX8SpecificData->RenderScene)
	{
		p_gDX8SpecificData->RenderScene = TRUE;
#ifdef JADEFUSION
		p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/BeginScene(/*p_gDX8SpecificData->mp_D3DDevice*/);
#else 
		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->BeginScene(p_gDX8SpecificData->mp_D3DDevice);
#endif
	}

	/*
	 * YLT,if there is scale in the matrix, we need to put the scale (last column)
	 * into the transformation matrix before sending to open gl
	 */
	MATH_MakeOGLMatrix(&st_OGLMatrix, _pst_Matrix);
#ifdef JADEFUSION
	p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/SetTransform
#else
		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetTransform
#endif
			(
#ifndef JADEFUSION
		p_gDX8SpecificData->mp_D3DDevice,
#endif
			D3DTS_VIEW,
			(const struct _D3DMATRIX *) &st_OGLMatrix
		);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_Ortho(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	D3DMATRIX	OrthoMatrix;
	/*~~~~~~~~~~~~~~~~~~~~*/

	memset(&OrthoMatrix, 0, sizeof(OrthoMatrix));

	OrthoMatrix._11 = 2.0f / (Right - Left);
	OrthoMatrix._22 = 2.0f / (Top - Bottom);
	OrthoMatrix._33 = -4.0f / (Far - Near);

	OrthoMatrix._14 = -(Right + Left) / (Right - Left);
	OrthoMatrix._24 = -(Top + Bottom) / (Top - Bottom);
	OrthoMatrix._34 = -(Far + Near) / (Far - Near);

	OrthoMatrix._44 = 1.0f;
	{
		/*~~~~~~~~~~~~~~~~~*/
		D3DCLIPSTATUS8	stCS;
		/*~~~~~~~~~~~~~~~~~*/

		stCS.ClipUnion = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM;
		stCS.ClipIntersection = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM;
#ifdef JADEFUSION
		p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/SetClipStatus(/*p_gDX8SpecificData->mp_D3DDevice,*/ &stCS);
#else
		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetClipStatus(p_gDX8SpecificData->mp_D3DDevice, &stCS);
#endif
	}

#ifdef JADEFUSION
	p_gDX8SpecificData->mp_D3DDevice->SetTransform
#else
		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetTransform
#endif
	(
#ifndef JADEFUSION
			p_gDX8SpecificData->mp_D3DDevice,
#endif
			D3DTS_PROJECTION,
			&OrthoMatrix
		);
}

/*
 =======================================================================================================================
    Aim:    Send View matrix for render
 =======================================================================================================================
 */
void DX8_SetProjectionMatrix(CAM_tdst_Camera *_pst_Cam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	float					f, f_ScreenRatio;
	LONG					w, h, W, H;
	LONG					x, y;
	ULONG					Flags;
	float					l, r, t, b;
	D3DCLIPSTATUS8			stCS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD( GDI_gpst_CurDD );

	/* Compute screen ratio */
	if
	(
		(GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst <= 0)
	||	(GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst >= GDI_Cul_SRC_Number)
	) f_ScreenRatio = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
	else
		f_ScreenRatio = GDI_gaf_ScreenRation[GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst];

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

	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		f = 1.0f / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
		MATH_SetIdentityMatrix(pst_SD->pst_ProjMatrix);

		if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
		{
			pst_SD->pst_ProjMatrix->Ix = f * f_ScreenRatio;
			pst_SD->pst_ProjMatrix->Jy = -f;
		}
		else
		{
			pst_SD->pst_ProjMatrix->Ix = f;
			pst_SD->pst_ProjMatrix->Jy = -f / f_ScreenRatio;
		}

		pst_SD->pst_ProjMatrix->Kz = (_pst_Cam->f_FarPlane + _pst_Cam->f_NearPlane) / (_pst_Cam->f_FarPlane - _pst_Cam->f_NearPlane);
		pst_SD->pst_ProjMatrix->Sz = 1.0f;
		pst_SD->pst_ProjMatrix->T.z = -0.05f;
		pst_SD->pst_ProjMatrix->w = 0.0f;

#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTransform
#else
			pst_SD->mp_D3DDevice->lpVtbl->SetTransform
#endif
		(
#ifndef JADEFUSION
		pst_SD->mp_D3DDevice,
#endif
				D3DTS_PROJECTION,
				(const struct _D3DMATRIX *) pst_SD->pst_ProjMatrix
			);

		stCS.ClipUnion = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM | D3DCS_FRONT | D3DCS_BACK;
		stCS.ClipIntersection = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM | D3DCS_FRONT | D3DCS_BACK;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetClipStatus(/*pst_SD->mp_D3DDevice,*/ &stCS);
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetClipStatus(pst_SD->mp_D3DDevice, &stCS);
#endif
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float	f_IsoFactorZoom, f_Scale;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_SetIdentityMatrix(pst_SD->pst_ProjMatrix);

		f_IsoFactorZoom = _pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom;
		f_Scale = f_IsoFactorZoom;

		if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
		{
			f = 1.0f / GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;

			if(_pst_Cam->f_IsoFactor == 0)
			{
				DX8_Ortho((1.0f - f) / 2.f, (1.f + f) / 2.f, 0.f, 1.f, -_pst_Cam->f_NearPlane, -_pst_Cam->f_FarPlane);
			}
			else
			{
				l = f_Scale * (-f);
				r = f_Scale * (f);
				b = f_Scale * -1.f;
				t = f_Scale * 1.f;

				DX8_Ortho(l, r, b, t, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
			}
		}
		else
		{
			f = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
			if(_pst_Cam->f_IsoFactor == 0.f)
			{
				DX8_Ortho(0.f, 1.f, (1.f - f) / 2.f, (1.f + f) / 2.f, -_pst_Cam->f_NearPlane, -_pst_Cam->f_FarPlane);
			}
			else
			{
				t = f_Scale * (-f);
				b = f_Scale * (f);
				l = f_Scale * -1.f;
				r = f_Scale * 1.f;

				DX8_Ortho(l, r, b, t, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
			}
		}
	}

	DX8_Viewport(pst_SD, x, y, w, h);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_SetTextureTarget( ULONG Num, ULONG MustBeCleared )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	IDirect3DSurface8 *ppSurfaceLevel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( Num != 0xffffffff )
	{
		if ( p_gDX8SpecificData->dul_SDW_Texture[ Num ] == NULL )
			return;

		/* Select Texture as frame buf frame buffer */
		if ( ( p_gDX8SpecificData->pBackBuffer_SAVE == NULL ) && ( p_gDX8SpecificData->pZBuffer_SAVE == NULL ) )
		{
#ifdef JADEFUSION
			p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/ GetRenderTarget
#else
			p_gDX8SpecificData->mp_D3DDevice->lpVtbl->GetRenderTarget
#endif
			        (
#ifndef JADEFUSION
			                p_gDX8SpecificData->mp_D3DDevice,
#endif
			                &p_gDX8SpecificData->pBackBuffer_SAVE );
#ifdef JADEFUSION
			p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/ GetDepthStencilSurface
#else
			p_gDX8SpecificData->mp_D3DDevice->lpVtbl->GetDepthStencilSurface
#endif
			        (
#ifndef JADEFUSION
			                p_gDX8SpecificData->mp_D3DDevice,
#endif
			                &p_gDX8SpecificData->pZBuffer_SAVE );
		}

		p_gDX8SpecificData->dul_SDW_Texture[ Num ]->lpVtbl->GetSurfaceLevel(
		        p_gDX8SpecificData->dul_SDW_Texture[ Num ],
		        0,
		        &ppSurfaceLevel );

		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetRenderTarget(
		        p_gDX8SpecificData->mp_D3DDevice,
		        ppSurfaceLevel,
		        NULL );

		ppSurfaceLevel->lpVtbl->Release( ppSurfaceLevel );
		if ( MustBeCleared )
		{
			p_gDX8SpecificData->mp_D3DDevice->lpVtbl->Clear(
			        p_gDX8SpecificData->mp_D3DDevice,
			        0,
			        NULL,
			        D3DCLEAR_TARGET,
			        0,
			        1.0f,
			        0 );
		}
		{
			/*~~~~~~~~~~~~~~~~~~*/
			D3DVIEWPORT8 V8SHD;
			/*~~~~~~~~~~~~~~~~~~*/

			V8SHD.X = 1;
			V8SHD.Y = 1;
			V8SHD.Width = ShadowTextureSize - 2;
			V8SHD.Height = ShadowTextureSize - 2;
			V8SHD.MinZ = 0.0f;
			V8SHD.MaxZ = 1.0f;
#ifdef JADEFUSION
			p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/ SetViewport( /*p_gDX8SpecificData->mp_D3DDevice,*/ &V8SHD );
#else
			p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetViewport( p_gDX8SpecificData->mp_D3DDevice, &V8SHD );
#endif
		}

		DX8_M_RenderState( p_gDX8SpecificData, D3DRS_CULLMODE, D3DCULL_NONE );
	}
	else
	{
		/* restore original frame buffer */
#ifdef JADEFUSION
		p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/ SetRenderTarget
#else
		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetRenderTarget
#endif
		        (
#ifndef JADEFUSION
		                p_gDX8SpecificData->mp_D3DDevice,
#endif
		                p_gDX8SpecificData->pBackBuffer_SAVE,
		                p_gDX8SpecificData->pZBuffer_SAVE );

		p_gDX8SpecificData->pBackBuffer_SAVE->lpVtbl->Release( p_gDX8SpecificData->pBackBuffer_SAVE );
		p_gDX8SpecificData->pZBuffer_SAVE->lpVtbl->Release( p_gDX8SpecificData->pZBuffer_SAVE );
		p_gDX8SpecificData->pBackBuffer_SAVE = p_gDX8SpecificData->pZBuffer_SAVE = NULL;
		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetViewport(
#ifndef JADEFUSION
		        p_gDX8SpecificData->mp_D3DDevice,
#endif
		        &p_gDX8SpecificData->V8 );
#ifdef JADEFUSION
		p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/ SetTransform
#else
		p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetTransform
#endif
		        (
#ifndef JADEFUSION
		                p_gDX8SpecificData->mp_D3DDevice,
#endif
		                D3DTS_PROJECTION,
		                ( const struct _D3DMATRIX * ) &p_gDX8SpecificData->SavedProjection );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_SetViewMatrix_SDW(MATH_tdst_Matrix *pst_Matrix, float *Limits)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	Identity;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/GetTransform
		(
			/*p_gDX8SpecificData->mp_D3DDevice,*/
			D3DTS_PROJECTION,
			&p_gDX8SpecificData->SavedProjection
		);
	MATH_SetIdentityMatrix(&Identity);
	Identity.Kz = 0.0f;
	Identity.T.z = 0.5f;
	p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/SetTransform
		(
			/*p_gDX8SpecificData->mp_D3DDevice,*/
			D3DTS_PROJECTION,
			(const struct _D3DMATRIX *) &Identity
		);
	p_gDX8SpecificData->mp_D3DDevice->/*lpVtbl->*/SetTransform
		(
			/*p_gDX8SpecificData->mp_D3DDevice,*/
			D3DTS_VIEW,
			(const struct _D3DMATRIX *) pst_Matrix
		);
#else
	p_gDX8SpecificData->mp_D3DDevice->lpVtbl->GetTransform
		(
			p_gDX8SpecificData->mp_D3DDevice,
			D3DTS_PROJECTION,
			&p_gDX8SpecificData->SavedProjection
		);

	MATH_SetIdentityMatrix(&Identity);
	Identity.Kz = 0.0f;
	Identity.T.z = 0.5f;
	p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetTransform
		(
			p_gDX8SpecificData->mp_D3DDevice,
			D3DTS_PROJECTION,
			(const struct _D3DMATRIX *) &Identity
		);
	p_gDX8SpecificData->mp_D3DDevice->lpVtbl->SetTransform
		(
			p_gDX8SpecificData->mp_D3DDevice,
			D3DTS_VIEW,
			(const struct _D3DMATRIX *) pst_Matrix
		);
#endif
}

/*
 =======================================================================================================================
    Aim:    Draw indexed triangles
 =======================================================================================================================
 */
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
		pVertexBuffer->Color = DX8_M_ConvertColor(ulOGLSetCol); \
	} \
	else if(pst_Alpha) \
	{ \
		ulOGLSetCol &= 0x00ffffff; \
		ulOGLSetCol |= pst_Alpha[a]; \
		pVertexBuffer->Color = DX8_M_ConvertColor(ulOGLSetCol); \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG DX8_l_DrawElementIndexedTriangles
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex							*_pst_Point,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8VertexFormat				*pVertexBuffer;
	GEO_tdst_IndexedTriangle	*t, *tend;
	ULONG						*pst_Color, *pst_Alpha;
	ULONG						TNum, ulOGLSetCol, ulOGLSetCol_XOr, ulOGLSetCol_Or;
    DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD( GDI_gpst_CurDD );
    
    TNum = _pst_Element->l_NbTriangles;
	if(!TNum) return 0;

	if(!pst_SD->RenderScene)
	{
		pst_SD->RenderScene = TRUE;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/BeginScene(/*p_gDX8SpecificData->mp_D3DDevice*/);
#else
		pst_SD->mp_D3DDevice->lpVtbl->BeginScene(p_gDX8SpecificData->mp_D3DDevice);
#endif
	}

#ifdef JADEFUSION
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Lock( /*pst_SD->m_VertexBuffer,*/ 0, TNum * sizeof(DX8VertexFormat) * 3, (unsigned char **) &pVertexBuffer, D3DLOCK_DISCARD );
#else
	pst_SD->m_VertexBuffer->lpVtbl->Lock( pst_SD->m_VertexBuffer, 0, TNum * sizeof(DX8VertexFormat) * 3, (unsigned char **) &pVertexBuffer, D3DLOCK_DISCARD );
#endif
	ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
	ulOGLSetCol_Or = ((DX8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData)->ulColorOr;
	if(pst_Color == NULL)
	{
		ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
		ulOGLSetCol ^= ulOGLSetCol_XOr;
		ulOGLSetCol = DX8_M_ConvertColor(ulOGLSetCol);
	}

	/* Fill */
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

	/* End fill */
#ifdef JADEFUSION	
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Unlock(/*pst_SD->m_VertexBuffer*/);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetStreamSource( /*pst_SD->mp_D3DDevice,*/ 0, pst_SD->m_VertexBuffer, sizeof(DX8VertexFormat) );
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetVertexShader(/*pst_SD->mp_D3DDevice,*/ D3DFVF_VERTEXF);
    pst_SD->mp_D3DDevice->/*lpVtbl->*/DrawPrimitive( /*pst_SD->mp_D3DDevice,*/ D3DPT_TRIANGLELIST, 0, TNum );
#else
	pst_SD->m_VertexBuffer->lpVtbl->Unlock(pst_SD->m_VertexBuffer);
	pst_SD->mp_D3DDevice->lpVtbl->SetStreamSource( pst_SD->mp_D3DDevice, 0, pst_SD->m_VertexBuffer, sizeof(DX8VertexFormat) );
	pst_SD->mp_D3DDevice->lpVtbl->SetVertexShader(pst_SD->mp_D3DDevice, D3DFVF_VERTEXF);
    pst_SD->mp_D3DDevice->lpVtbl->DrawPrimitive( pst_SD->mp_D3DDevice, D3DPT_TRIANGLELIST, 0, TNum );
#endif
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG DX8_l_DrawElementIndexedSprites
(
	GEO_tdst_ElementIndexedSprite	*_pst_Element,
	GEO_Vertex						*_pst_Point,
	ULONG							ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		XCam, YCam, *p_point;
	MATH_tdst_Vector		Sprite[5];
	float					Size;
	GDI_tdst_DisplayData	*pst_DD;
	GEO_tdst_IndexedSprite	*p_Frst, *p_Last;
    ULONG					ul_ColorOr, ul_ColorXor, ul_ColorSet;
	ULONG					*pst_Color;
	ULONG					*pst_Alpha;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_DD = GDI_gpst_CurDD;

	pst_Color = pst_DD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = pst_DD->pst_ComputingBuffers->CurrentAlphaField;
	ul_ColorOr = DX8_M_SD( pst_DD )->ulColorOr;
    ul_ColorXor = pst_DD->pst_ComputingBuffers->ulColorXor;
	ul_ColorSet = pst_DD->pst_ComputingBuffers->ul_Ambient | ul_ColorOr;
	ul_ColorSet ^= ul_ColorXor;

	XCam.x = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Ix * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
	XCam.y = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Jx * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
	XCam.z = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Kx * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
	YCam.x = pst_DD->st_MatrixStack.pst_CurrentMatrix->Iy * _pst_Element->fGlobalSize;
	YCam.y = pst_DD->st_MatrixStack.pst_CurrentMatrix->Jy * _pst_Element->fGlobalSize;
	YCam.z = pst_DD->st_MatrixStack.pst_CurrentMatrix->Ky * _pst_Element->fGlobalSize;
	pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_BeforeDrawSprite, _pst_Element->l_NbSprites);

	p_Frst = _pst_Element->dst_Sprite;
	p_Last = p_Frst + _pst_Element->l_NbSprites;
	while(p_Frst < p_Last)
	{
        /* calcul de la couleur du sprite */
        if( pst_Color ) 
		{ 
			ul_ColorSet = (pst_Color[p_Frst->auw_Index] | ul_ColorOr) ^ ul_ColorXor; 
			if(pst_Alpha) 
				ul_ColorSet = (ul_ColorSet & 0x00ffffff) | pst_Alpha[p_Frst->auw_Index]; 
		} 
		else if(pst_Alpha) 
			ul_ColorSet = (ul_ColorSet & 0x00ffffff) |pst_Alpha[p_Frst->auw_Index]; 

        *(ULONG *) &Sprite[4].x =  ul_ColorSet;
		
		p_point = _pst_Point + p_Frst->auw_Index;
		Size = *(float *) p_Frst;

		Sprite[0].x = p_point->x + (-XCam.x - YCam.x) * Size;
		Sprite[0].y = p_point->y + (-XCam.y - YCam.y) * Size;
		Sprite[0].z = p_point->z + (-XCam.z - YCam.z) * Size;
		Sprite[1].x = p_point->x + (+XCam.x - YCam.x) * Size;
		Sprite[1].y = p_point->y + (+XCam.y - YCam.y) * Size;
		Sprite[1].z = p_point->z + (+XCam.z - YCam.z) * Size;
		Sprite[2].x = p_point->x + (+XCam.x + YCam.x) * Size;
		Sprite[2].y = p_point->y + (+XCam.y + YCam.y) * Size;
		Sprite[2].z = p_point->z + (+XCam.z + YCam.z) * Size;
		Sprite[3].x = p_point->x + (-XCam.x + YCam.x) * Size;
		Sprite[3].y = p_point->y + (-XCam.y + YCam.y) * Size;
		Sprite[3].z = p_point->z + (-XCam.z + YCam.z) * Size;

		pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSpriteUV, (ULONG) Sprite);
		p_Frst++;
	}

	pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_AfterDrawSprite, 0);
	return 0;
}

/*
 =======================================================================================================================
    Aim:    Setup blending mode
 =======================================================================================================================
 */
void DX8_SetTextureBlending(ULONG _l_Texture, ULONG BM)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					Flag;
	ULONG					Delta;
	int						i_Value;
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(GDI_gpst_CurDD);

	if(!pst_SD->RenderScene)
	{
		pst_SD->RenderScene = TRUE;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/BeginScene(/*pst_SD->mp_D3DDevice*/);
#else
		pst_SD->mp_D3DDevice->lpVtbl->BeginScene(pst_SD->mp_D3DDevice);
#endif
	}

    DX8_RS_DrawWired(pst_SD, !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));
    DX8_RS_DepthTest(pst_SD, GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest );
	DX8_RS_Fogged( pst_SD, ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (pst_SD->ulFogState)));
    
	if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace)
	{
		if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF)
			DX8_M_RenderState(pst_SD, D3DRS_CULLMODE, D3DCULL_CW);
		else
			DX8_M_RenderState(pst_SD, D3DRS_CULLMODE, D3DCULL_CCW);
	}
	else
	{
		DX8_M_RenderState(pst_SD, D3DRS_CULLMODE, D3DCULL_NONE);
	}

	Delta = GDI_gpst_CurDD->LastBlendingMode ^ BM;

	/* Delta = -1; */
	Flag = MAT_GET_FLAG(BM);

    DX8_RS_UseTexture(pst_SD, _l_Texture);

    
    DX8_RS_DepthMask(pst_SD, (Flag & MAT_Cul_Flag_NoZWrite) ? 0 : 1);
	DX8_RS_DepthFunc(pst_SD, (Flag & MAT_Cul_Flag_ZEqual) ? D3DCMP_EQUAL : D3DCMP_LESSEQUAL);

	if(Delta & MAT_Cul_Flag_TileU)
	{
		i_Value = (Flag & MAT_Cul_Flag_TileU) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*pst_SD->mp_D3DDevice,*/ 0, D3DTSS_ADDRESSU, i_Value);
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU, i_Value);
#endif
	}

	if(Delta & MAT_Cul_Flag_TileV)
	{
		i_Value = (Flag & MAT_Cul_Flag_TileV) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*pst_SD->mp_D3DDevice,*/ 0, D3DTSS_ADDRESSV, i_Value);
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV, i_Value);
#endif
	}

	if(Delta & MAT_Cul_Flag_Bilinear)
	{
		i_Value = (Flag & MAT_Cul_Flag_Bilinear) ? D3DTEXF_LINEAR : D3DTEXF_POINT;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*pst_SD->mp_D3DDevice,*/ 0, D3DTSS_MAGFILTER, i_Value);
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*pst_SD->mp_D3DDevice,*/ 0, D3DTSS_MINFILTER, i_Value);
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MAGFILTER, i_Value);
		pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MINFILTER, i_Value);
#endif
	}

	if(Delta & MAT_Cul_Flag_Trilinear)
	{
		i_Value = (Flag & MAT_Cul_Flag_Trilinear) ? D3DTEXF_LINEAR : D3DTEXF_POINT;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*pst_SD->mp_D3DDevice,*/ 0, D3DTSS_MIPFILTER, i_Value);
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_MIPFILTER, i_Value);
#endif
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
			break;
		}

		DX8_M_RenderState(pst_SD, D3DRS_COLORWRITEENABLE, i_Value);
	}

	if(Delta & (MAT_Cul_Flag_AlphaTest | MAT_Cc_AlphaTresh_MASK))
	{
		if(Flag & MAT_Cul_Flag_AlphaTest)
		{
			DX8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, TRUE);
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				DX8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_LESS);
			else
				DX8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			DX8_M_RenderState(pst_SD, D3DRS_ALPHAREF, MAT_GET_AlphaTresh(BM));
		}
		else
		{
			DX8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, FALSE);
		}
	}

	if(Flag & MAT_Cul_Flag_UseLocalAlpha)
		pst_SD->ulColorOr = 0xff000000;
	else
		pst_SD->ulColorOr = 0;

    

	if((MAT_GET_Blending(Delta)) || (Delta & MAT_Cul_Flag_InvertAlpha))
	{
		switch(MAT_GET_Blending(BM))
		{
		case MAT_Cc_Op_Copy:
		case MAT_Cc_Op_Glow:

			DX8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, FALSE);
			DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
			DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ZERO);
			break;
		case MAT_Cc_Op_Alpha:
			DX8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, TRUE);
            if(Flag & MAT_Cul_Flag_InvertAlpha)
            {
                DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
			    DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
            }
            else
            {
			    DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			    DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            }
			break;
		case MAT_Cc_Op_AlphaPremult:
			DX8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, TRUE);
            DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
            if(Flag & MAT_Cul_Flag_InvertAlpha)
                DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
            else
			    DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			break;
		case MAT_Cc_Op_AlphaDest:
			DX8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, TRUE);
			if(Flag & MAT_Cul_Flag_InvertAlpha)
			{
				DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
				DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA);
			}
			else
			{
				DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA);
				DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
			}
			break;
		case MAT_Cc_Op_AlphaDestPremult:
			DX8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, TRUE);
			if(Flag & MAT_Cul_Flag_InvertAlpha)
			{
				DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
				DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
			}
			else
			{
				DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA);
				DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
			}
			break;
		case MAT_Cc_Op_Add:
			DX8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, TRUE);
			DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ONE);
			DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_ONE);
			break;
		case MAT_Cc_Op_Sub:
		case MAT_Cc_Op_PSX2ShadowSpecific:
			DX8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, TRUE);
			DX8_M_RenderState(pst_SD, D3DRS_SRCBLEND, D3DBLEND_ZERO);
			DX8_M_RenderState(pst_SD, D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
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

				//CARLONE FOG COLOR
				DX8_M_RenderState(pst_SD, D3DRS_FOGCOLOR,pst_SD->ul_FogColor);
			}
			break;
		case MAT_Cc_Op_AlphaPremult:
		case MAT_Cc_Op_AlphaDestPremult:
			if(pst_SD->ulFogState != 5)
			{
				pst_SD->ulFogState = 5;
				pst_SD->ul_FogColorOn2 &= 0xFFFFFF;
				DX8_M_RenderState(pst_SD, D3DRS_FOGCOLOR,pst_SD->ul_FogColor);
			}
			break;
		case MAT_Cc_Op_Add:
		case MAT_Cc_Op_Sub:
			if(pst_SD->ulFogState != 3)
			{
				pst_SD->ulFogState = 3;
				DX8_M_RenderState(pst_SD, D3DRS_FOGCOLOR, pst_SD->ul_FogColorBlack);
			}
			break;
		}
	}

	GDI_gpst_CurDD->LastTextureUsed = _l_Texture;
	GDI_gpst_CurDD->LastBlendingMode = BM;
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
void DX8_SetDCPixelFormat(HDC _hDC)
{
}

/*
 =======================================================================================================================
    Aim:    Setup rendering context
 =======================================================================================================================
 */
void DX8_SetupRC(DX8_tdst_SpecificData *_pst_SD)
{
	/*~~~~~~~~~*/
	LONG	w, h;
	/*~~~~~~~~~*/

	_pst_SD->RenderScene = FALSE;
	w = _pst_SD->rcViewportRect.right - _pst_SD->rcViewportRect.left;
	h = _pst_SD->rcViewportRect.bottom - _pst_SD->rcViewportRect.top;
	DX8_Viewport(_pst_SD, 0, (h - w) / 2, w, w);

	/* Reset coordinate system */

	/* Establish clipping volume (left, right, bottom, top, near, far) */
	_pst_SD->pst_ProjMatrix->Jy = -1.0f;
	_pst_SD->pst_ProjMatrix->Sz = 1.0f;
	_pst_SD->pst_ProjMatrix->T.z = -0.1f;
	_pst_SD->pst_ProjMatrix->w = 0.0f;	/* MATRIX W! */
}

