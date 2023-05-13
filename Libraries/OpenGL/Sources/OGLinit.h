/*$T OGLinit.h GC!1.52 11/23/99 15:29:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#include "MATHs/MATH.h"
#include "OGLrenderstate.h"
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

//#	define OGL_DEBUG_CALLS
#	if !defined( NDEBUG ) && defined( OGL_DEBUG_CALLS )
#		include <assert.h>
#		define OGL_CALL( X )                     \
			{                                     \
				glGetError();                     \
				X;                                \
				unsigned int _err = glGetError(); \
				assert( _err == GL_NO_ERROR );    \
			}
#	else
#		define OGL_CALL( X ) X
#	endif

#define OGL_M_SD(_pst_DD)   ((OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData)
#define OGL_M_RS(_pst_DD)   (&((OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData)->st_RS)

#define OGL_C_BumpTexGran   32
#define OGL_C_BumpTexFlag   0x00010000
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
typedef struct OGL_tdst_UpdatablePalette_
{
    short   w_Texture; 
    short   w_Palette; 
    short   w_Width; 
    short   w_Height;
    LONG    l_Size;
    char    *p_Raw;
} OGL_tdst_UpdatablePalette;

typedef struct OGL_tdst_InterfaceTexture_
{
    ULONG       ul_Texture;
    ULONG       ul_Key;
    int         W, H, BPP, format;
    int         Mipmap;
    short       w_Palette;    
    union
    {
        char    *pc_Buf;
        char    **ppc_Bufs;
    };
} OGL_tdst_InterfaceTexture;

typedef struct OGL_tdst_BumpTexture_
{
    ULONG       ul_Bump;
    ULONG       ul_InvBump;
} OGL_tdst_BumpTexture;

typedef struct  OGL_tdst_SpecificData_
{
#ifndef PSX2_TARGET /* those topics don't exist with PS2 OGL */
    HWND                        h_Wnd;
    HDC                         h_DC;
    HGLRC                       h_RC;
#endif
    RECT                        rcViewportRect;

    LONG                        l_NumberOfTextures;
    ULONG			            *dul_Texture;

    /* Following field is an optimisation */
    ULONG					    *dul_TextureDeltaBlend;

    MATH_tdst_Matrix            *pst_ProjMatrix;

    ULONG					    ulColorOr;  /* Used for Force Alpha full (TExture alpha Only) */

    OGL_tdst_RenderState        st_RS;
	ULONG					    ulFogState; /* Bit 0 : On-Off , Bit 1 : ForcedToBlack */
    float					    fFogColor[4];  /* Used keep fog information during add or sub transparency */
    float					    fFogColorOn2[4];  /* Used keep fog information during add or sub transparency */
    float					    fFogBlack[4];  /* Used keep fog information during add or sub transparency */

    LONG                        l_NumberOfUpdatablePalettes;
    OGL_tdst_UpdatablePalette   *dst_UdatePalette;

    LONG                        l_NbInterfaceTex;
    OGL_tdst_InterfaceTexture   *dst_InterfaceTex;

    LONG                        l_NbBumpTex;
    LONG                        l_MaxBumpTex;
    OGL_tdst_BumpTexture        *dst_BumpTex;
} OGL_tdst_SpecificData;

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
OGL_tdst_SpecificData   *OGL_pst_CreateDevice(void);
void                    OGL_DestroyDevice(void *);
LONG                    OGL_l_Close(struct GDI_tdst_DisplayData_ *);
#ifdef PSX2_TARGET
LONG                    OGL_l_Init(struct GDI_tdst_DisplayData_ *);
#else
LONG                    OGL_l_Init(HWND _hWnd, struct GDI_tdst_DisplayData_ *);
#endif
LONG                    OGL_l_ReadaptDisplay(HWND, struct GDI_tdst_DisplayData_ *);

/*
 ===================================================================================================
    Flip / clear / surface status
 ===================================================================================================
 */
void                    OGL_Flip(void);
void                    OGL_Clear(LONG, ULONG);

/*
 ===================================================================================================
    Render function
 ===================================================================================================
 */
void                    OGL_SetViewMatrix(MATH_tdst_Matrix *);
void                    OGL_SetProjectionMatrix(struct CAM_tdst_Camera_ *);
void                    OGL_SetTextureBlending( ULONG, ULONG );

void					OGL_SetTextureTarget(ULONG , ULONG Clear);
void					OGL_SetViewMatrix_SDW(struct MATH_tdst_Matrix_ *_pst_Matrix , float *Limits);

/*
 ===================================================================================================
    Draw primitives
 ===================================================================================================
 */
LONG                    OGL_l_DrawElementIndexedTriangles( struct GEO_tdst_ElementIndexedTriangles_ *, GEO_Vertex *, struct GEO_tdst_UV_ * , ULONG);
LONG                    OGL_l_DrawElementIndexedSprites(struct GEO_tdst_ElementIndexedSprite_		*_pst_Element,GEO_Vertex	*,	ULONG);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
