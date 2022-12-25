/*$T DX8init.h GC!1.52 11/23/99 15:29:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __DX8INIT_H__
#define __DX8INIT_H__

#include "MATHs/MATH.h"
#include <d3d8.h>
#include "DX8renderstate.h"
#include "BASe/BAStypes.h"
#include "SOFT/SOFTstruct.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Macros
 ***************************************************************************************************
 */

#define DX8_M_SD(_pst_DD)   ((DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData)
#define DX8_M_RS(_pst_DD)   (&((DX8_tdst_SpecificData *) _pst_DD->pv_SpecificData)->st_RS)
#define DX8_M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)

#ifdef JADEFUSION
#define DX8_M_RenderState(_pst_SD, a, b)  _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ a, b )
#define DX8_M_StageState(_pst_SD, a, b, c) _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState( /*_pst_SD->mp_D3DDevice,*/ a, b, c)
#else
#define DX8_M_RenderState(_pst_SD, a, b)  _pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, a, b )
#define DX8_M_StageState(_pst_SD, a, b, c) _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState( _pst_SD->mp_D3DDevice, a, b, c)
#endif
extern BOOL DX8_gb_Init;

/*$4
 ***************************************************************************************************
    Structure
 ***************************************************************************************************
 */

/*
 ---------------------------------------------------------------------------------------------------
    Specific data for OpenGL Device
 ---------------------------------------------------------------------------------------------------
 */
#define MaxShadowTexture	8
#define ShadowTextureSize	128
typedef struct  DX8_tdst_SpecificData_
{
    HWND                    h_Wnd;
    RECT                    rcViewportRect;
    LONG                    l_NumberOfTextures;
    IDirect3DTexture8		**dul_Texture;
    IDirect3DTexture8		*dul_SDW_Texture[MaxShadowTexture];

    MATH_tdst_Matrix        *pst_ProjMatrix;
    ULONG					ulColorOr;  /* Used for Force Alpha full (TExture alpha Only) */
	ULONG					ulFogState; /* Bit 0 : On-Off , Bit 1 : ForcedToBlack */
    //float					fFogColor[4];  /* Used keep fog information during add or sub transparency */
    //float					fFogColorOn2[4];  /* Used keep fog information during add or sub transparency */
    //float					fFogBlack[4];  /* Used keep fog information during add or sub transparency */

    ULONG                   ul_FogColor;
    ULONG                   ul_FogColorOn2;
    ULONG                   ul_FogColorBlack;

    
    DX8_tdst_RenderState    st_RS;

    LPDIRECT3D8				mp_D3D;                 // The main D3D object

	D3DPRESENT_PARAMETERS	mst_D3DPP;              // Parameters for CreateDevice/Reset
	LPDIRECT3DDEVICE8		mp_D3DDevice;           // The D3D rendering device

	D3DCAPS8				m_d3dCaps;           // Caps for the device
	D3DSURFACE_DESC			m_d3dsdBackBuffer;   // Surface desc of the backbuffer
	DWORD					m_dwCreateFlags;     // Indicate sw or hw vertex processing
	DWORD					m_dwWindowStyle;     // Saved window style for mode switches
	RECT					m_rcWindowBounds;    // Saved window bounds for mode switches
	RECT					m_rcWindowClient;    // Saved client area size for mode switches
	LPDIRECT3DVERTEXBUFFER8 m_VertexBuffer;
    struct DX8VertexFormat_	*pst_LockedVertex;
    ULONG                   ul_LockedVertex, ul_LVLeft;
	D3DDISPLAYMODE			d3ddm;
	BOOL					RenderScene ;
    LPDIRECT3DSURFACE8		pBackBuffer_SAVE, pZBuffer_SAVE;

	D3DVIEWPORT8			V8;
	D3DMATRIX				SavedProjection;

} DX8_tdst_SpecificData;

#define D3DFVF_VERTEXF (D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2 )
typedef struct  DX8VertexFormat_
{
	float X,Y,Z;
	ULONG Color;
	float fU,fV;
} DX8VertexFormat;


/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Close / init / reinit
 ===================================================================================================
 */
DX8_tdst_SpecificData   *DX8_pst_CreateDevice(void);
DX8_tdst_SpecificData*  DX8_GetSpecificData(void);
void                    DX8_DestroyDevice(void *);
LONG                    DX8_l_Close(struct GDI_tdst_DisplayData_ *);
#ifdef PSX2_TARGET
LONG                    DX8_l_Init(struct GDI_tdst_DisplayData_ *);
#else
LONG                    DX8_l_Init(HWND _hWnd, struct GDI_tdst_DisplayData_ *);
#endif
LONG                    DX8_l_ReadaptDisplay(HWND, struct GDI_tdst_DisplayData_ *);

/*
 ===================================================================================================
    Flip / clear / surface status
 ===================================================================================================
 */
void                    DX8_Flip(void);
void                    DX8_Clear(LONG, ULONG);

/*
 ===================================================================================================
    Render function
 ===================================================================================================
 */
void                    DX8_SetViewMatrix(MATH_tdst_Matrix *);
void                    DX8_SetProjectionMatrix(struct CAM_tdst_Camera_ *);
void                    DX8_SetTextureBlending( ULONG, ULONG );

void					DX8_SetTextureTarget(ULONG,ULONG Clear);
void					DX8_SetViewMatrix_SDW(struct MATH_tdst_Matrix_ *_pst_Matrix , float *Limits);


/*
 ===================================================================================================
    Draw primitives
 ===================================================================================================
 */
LONG                    DX8_l_DrawElementIndexedTriangles( struct GEO_tdst_ElementIndexedTriangles_ *, GEO_Vertex *, struct GEO_tdst_UV_ * , ULONG);
LONG                    DX8_l_DrawElementIndexedSprites(struct GEO_tdst_ElementIndexedSprite_		*_pst_Element,GEO_Vertex	*,	ULONG);

/*
 ===================================================================================================
    Inline functions
 ===================================================================================================
 */

__inline DX8_tdst_SpecificData*
DX8_GetSpecificData( void )
{
    extern DX8_tdst_SpecificData* p_gDX8SpecificData;

    return p_gDX8SpecificData;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __DX8INIT_H__ */