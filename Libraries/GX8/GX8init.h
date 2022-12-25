/*$T Gx8init.h GC!1.52 11/23/99 15:29:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GX8INIT_H__
#define __GX8INIT_H__

#include <xtl.h>
#include <d3d8.h>

#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"
#include "SOFT/SOFTstruct.h"
#include "MATerial/MATstruct.h"
#include "GEOmetric/GEOobject.h"
#ifdef __cplusplus
extern "C"
{
#endif


/*$4
 ***************************************************************************************************
    Macros
 ***************************************************************************************************
 */

#define GX8_M_SD(_pst_DD)   ((Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData)
#define GX8_M_RS(_pst_DD)   (&((Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData)->st_RS)

#ifdef _XBOX
#define Gx8_M_RenderState(_pst_SD, a, b)  IDirect3DDevice8_SetRenderState ( _pst_SD->mp_D3DDevice, a, b )
#else // _XBOX
#error you're not compiling for XBox
#define Gx8_M_RenderState(_pst_SD, a, b)  _pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, a, b )
#endif //_XBOX

//#define USE_TANGENTE
/*$4
 ***************************************************************************************************
    Structure
 ***************************************************************************************************
 */
typedef struct SPRITEVERTEX_
{
    FLOAT x,y,z; // The transformed position for the vertex
    //D3DXVECTOR3 p;
	DWORD color;        // The vertex color
	//D3DXVECTOR2 tu;
	FLOAT u,v;

}SPRITEVERTEX;

/*---------------------------------------------------------------------------------------------------
    Auxiliary Circular Vertex Buffers
 ---------------------------------------------------------------------------------------------------*/

#define CIRCULAR_VB_SIZE	0x0000ffff		// size (number of vertices) of a Circular VB
#define	CIRCULAR_VB_SIZE_COLOR 0x0002fffd

typedef struct  Gx8_tdst_CircularVB_
{
	IDirect3DVertexBuffer8			* pVB;
	UINT							stride;
	DWORD							FVF;
	UINT							nextIndex;
	UINT							currIndex;
	struct OBJ_tdst_GameObject_		* pCurrGO;		// used to store Current GameObject
	void							* pCurrSource;	// Color Circular VB use it to store Current pColorSource
	UINT							currData1;		// Color Circular VB use it to store Current ulOGLSetCol_XOr
	UINT							currData2;		// Color Circular VB use it to store Current ulOGLSetCol_Or
	UINT							ambient;
} Gx8_tdst_CircularVB;


/*---------------------------------------------------------------------------------------------------
    Gx8 Vertex Format
 ---------------------------------------------------------------------------------------------------*/

////////////////////
typedef struct  Gx8VertexComponent_Position_
{
	float X,Y,Z;
} Gx8VertexComponent_Position;

typedef struct  Gx8VertexComponent_Normal_
{
	float X,Y,Z;
} Gx8VertexComponent_Normal;

typedef struct  Gx8VertexComponent_Color_
{
	ULONG Color;
} Gx8VertexComponent_Color;

typedef struct  Gx8VertexComponent_UV_
{
	float U,V;
} Gx8VertexComponent_UV;
//--> for Test <--
typedef struct LINEVERTEX
{
    FLOAT x, y, z;
    DWORD color;
} LINEVERTEX;
/*
 ---------------------------------------------------------------------------------------------------
    Specific data for OpenGL Device
 ---------------------------------------------------------------------------------------------------
 */

typedef enum {
    GX8_C_CULLFACE_NORMAL   = D3DCULL_CCW,
    GX8_C_CULLFACE_INVERTED = D3DCULL_CW,
    GX8_C_CULLFACE_OFF      = D3DCULL_NONE
} Gx8_tdeCullFaceType;

typedef struct  Gx8_tdst_RenderState_
{
    LONG                l_LastTexture;
    LONG                l_DepthFunc;
    float               f_LineWidth;
    float               f_PointSize;
    char                c_Wired;
    char                c_DepthMask;
    char                c_Lighting;
    char                c_Fogged;
    Gx8_tdeCullFaceType e_CullFace;

    DWORD               dwCurrentVertexShader;
	DWORD               dwCurrentPixelShader;
} Gx8_tdst_RenderState;

typedef enum
{
    Gx8_C_InvertLocalMatrixSet = 0x00000001,

    FORCE_LONG_FOR_SOMESTATEFLAGS = 0xffffffff
} Gx8_tdeSomeStateFlags;

#define MaxShadowTexture    8//8//10
#define ShadowTextureSize   128

#define ShadowBuffer_MaxLights	2	// maximum number of lights casting (at the same time) shadows
#define ShadowBuffer_Width 640	// SB width resolution
#define ShadowBuffer_Height 480	// SB height resolution
#define ShadowBuffer_ShadowIntensity 128	// shadow intensity

#define AEColorET_Max	3
//#define AEDepthET_Max	2


typedef struct  Gx8_tdst_SpecificData_
{
    HWND                    h_Wnd;
    RECT                    rcViewportRect;
    LONG                    l_NumberOfTextures;
    IDirect3DTexture8       **dul_Texture;
    IDirect3DTexture8       *dul_SDW_Texture[MaxShadowTexture];

//#ifdef	USE_TANGENTE
	// texture test normalmap
	
	IDirect3DTexture8       *Tex_Normalmap;
	IDirect3DTexture8       *Tex_Image;
	IDirect3DTexture8		*g_pTextureP;
	
	// NOISE
	//IDirect3DTexture8		*Tex_Noise;
	
	//#endif
    MATH_tdst_Matrix        *pst_ProjMatrix;
    ULONG                   ulColorOr;  /* Used for Force Alpha full (TExture alpha Only) */
    ULONG                   ulFogState; /* Bit 0 : On-Off , Bit 1 : ForcedToBlack */
    //float                 fFogColor[4];  /* Used keep fog information during add or sub transparency */
    //float                 fFogColorOn2[4];  /* Used keep fog information during add or sub transparency */
    //float                 fFogBlack[4];  /* Used keep fog information during add or sub transparency */

    ULONG                   ul_FogColor;
    ULONG                   ul_FogColorOn2;
    ULONG                   ul_FogColorBlack;

    
    Gx8_tdst_RenderState    st_RS;

    LPDIRECT3D8             mp_D3D;                 // The main D3D object

    D3DPRESENT_PARAMETERS   mst_D3DPP;              // Parameters for CreateDevice/Reset
    LPDIRECT3DDEVICE8       mp_D3DDevice;           // The D3D rendering device

    D3DCAPS8                m_d3dCaps;           // Caps for the device
    D3DSURFACE_DESC         m_d3dsdBackBuffer;   // Surface desc of the backbuffer
    DWORD                   m_dwCreateFlags;     // Indicate sw or hw vertex processing
    DWORD                   m_dwWindowStyle;     // Saved window style for mode switches
    RECT                    m_rcWindowBounds;    // Saved window bounds for mode switches
    RECT                    m_rcWindowClient;    // Saved client area size for mode switches
    struct
    {
        LPDIRECT3DVERTEXBUFFER8 m_VertexBuffer; // (size is Gx8C_BigVertexBufferVertexCount)
        UINT                    m_uiWhereToLockFrom;
        UINT                    m_uiLockedVerticesCount;
    }                       m_stBigVBData;
    struct
    {
        LPDIRECT3DVERTEXBUFFER8 m_VertexBuffer; // (size is Gx8C_BigVertexBufferVertexCount)
        UINT                    m_uiWhereToLockFrom;
        UINT                    m_uiLockedVerticesCount;
    }                       m_stBigLitVBData;   // a vertex buffer used for hardware-lit objects
    struct
    {
        LPDIRECT3DVERTEXBUFFER8 m_VertexBuffer; // Special vb dedicated to 3D sprites rendering (size is Gx8C_BigVertexBufferForSpritesVertexCount)
        UINT                    m_uiWhereToLockFrom;
        BYTE                    *m_pData;
    }                       m_stBigVBforSpritesData;
/*
	struct
	{
		LPDIRECT3DVERTEXBUFFER8 Spg2VB;
		//LPDIRECT3DVERTEXBUFFER8 swapVB[3];
		//DWORD                   m_dwCurrentBuffer;
	}VBforSPG2;


LPDIRECT3DVERTEXBUFFER8 zouege;
*/
	struct Gx8VertexFormat_ *pst_LockedVertex;
    ULONG                   ul_LockedVertex, ul_LVLeft;
    D3DDISPLAYMODE          d3ddm;
    BOOL                    RenderScene;
    LPDIRECT3DSURFACE8      pBackBuffer_SAVE, pZBuffer_SAVE;

    D3DVIEWPORT8            V8;
    D3DMATRIX               SavedProjection;

    Gx8_tdeSomeStateFlags   eStateFlags;
    D3DMATRIX               stInvertWorldMatrix; // matrix to go from global system to object-local system (only if flag Gx8C_InvertLocalMatrixSet is set)

    IDirect3DTexture8       *pBackBufferTexture;
	//IDirect3DTexture8       *pBackBufferTextureTemp;//antialias AfterFX zoomsmooth temp

	//IDirect3DTexture8       *pBackBufferTextureInv;
    IDirect3DTexture8       *pZBufferTexture;

    //DWORD                   AfterFXTexture_Width;
    //DWORD                   AfterFXTexture_Height;
    IDirect3DTexture8       *pMotionBlurFrameBufferTexture;
    IDirect3DTexture8       *pTmpFrameBufferTexture;
	IDirect3DTexture8       *pSaveBufferTexture;//yoann extension
    IDirect3DTexture8       *pFadeBufferTexture;
    IDirect3DTexture8       *pBWBufferTexture;
//    IDirect3DTexture8       tBWBufferTexture;
    IDirect3DTexture8       *pSmoothBufferTexture;
    int                     iFadeAlfa;	// used by fade
	bool					bShadowBuffer; // true if shadow buffer is required
	bool					bRenderingFromLight; // true when rendering from light POV
//    IDirect3DTexture8       *pShadowBufferColorTexture; 

	IDirect3DTexture8       *pShadowBufferDepthTexture; // shadow buffer texture
    IDirect3DTexture8       *pShadowBufferSpot[ShadowBuffer_MaxLights]; // one rendering for each spot
    IDirect3DTexture8       *pCircleTexture; // circular texture
	MATH_tdst_Matrix		mLightMatrix[ShadowBuffer_MaxLights]; // spot lights matrixes
	MATH_tdst_Matrix		mSBTextureMatrix; // used for UV computing

	IDirect3DSurface8		*pSaveRenderTarget; // original render target
	IDirect3DSurface8		*pSaveZBuffer; // original z buffer
	int						iNOfSBLights; // number of spot lights casting shadows
	IDirect3DTexture8       tZBufferForDepthBlurTexture; // depth texture for depth blur effect
    IDirect3DTexture8       tZBufferForNearBlurTexture; // depth texture for near blur effect
    IDirect3DTexture8       *pAlphaForDepthBlurTexture; // alpha texture for depth blur effect
    IDirect3DTexture8       *pAlphaForNearBlurTexture; // alpha texture for near blur effect

	IDirect3DTexture8       *pAEColorET[AEColorET_Max]; // AEs extra textures buffer
    bool					bAEColorET_Available[AEColorET_Max]; // extra textures flags (available or not)
	int						iAEColorET_Count; // current number of extra textures used

	// auxiliary rotative VB
	Gx8_tdst_CircularVB		positionVB;					// auxiliary VB for positions
	Gx8_tdst_CircularVB		normalVB;					// auxiliary VB for normals
	Gx8_tdst_CircularVB		colorVB;					// auxiliary VB for colors
	//yo temp
	//Gx8_tdst_CircularVB		Spg2VB;

	DWORD					pVertexShader;
	DWORD					pVertexShaderWithUV;

	DWORD					pVertexShaderNormal;
	DWORD					pVertexShaderNormalWithUV;
	DWORD					pVertexShaderNormalWith2UV; //temp yoann

	bool					needSBMatrix; //Tell if the pipeline need to calculare Shadow buffer's matrix  
	bool                    diffuseInTexture;
	bool					IsDiffuseActive;


} Gx8_tdst_SpecificData;

#define D3DFVF_VERTEXF (D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2 )
typedef struct  Gx8VertexFormat_
{
    float X,Y,Z;
    ULONG Color;
    float fU,fV;
} Gx8VertexFormat;
//#define Gx8_C_BigVertexBufferVertexCount 0xFFFF /* was SOFT_Cul_ComputingBufferSize * 3 */
#define Gx8_C_BigVertexBufferVertexCount 0x000F /* was SOFT_Cul_ComputingBufferSize * 3 */

#define D3DFVF_VERTEXF2D (D3DFVF_DIFFUSE | D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2 )
typedef struct  Gx8VertexFormatFor2D_
{
    float X,Y,Z,rhw;
    ULONG Color;
    float fU,fV;
} Gx8VertexFormatFor2D;
#define Gx8_C_BigVertexBuffer2DVertexCount 0x2000 /* was SOFT_Cul_ComputingBufferSize * 3 */

#define D3DFVF_LITVERTEXF (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2 )
typedef struct  Gx8LitVertexFormat_
{
    float X,Y,Z;
    float fU,fV;
} Gx8LitVertexFormat;

typedef struct
{
    D3DVECTOR stCenterPostion; // 3D position of the sprite center
    FLOAT     f2DOffsetX;      // [ 2 offsets to get 2D corner from 2D center position
    FLOAT     f2DOffsetY;      // ]
    DWORD     dwSpriteColor;   // corner color
    FLOAT     fU;              // [ corner UVs
    FLOAT     fV;              // ]
} Gx8VertexFormatForSprites;
//#define Gx8_C_BigVertexBufferForSpritesVertexCount 0xFFFF
#define Gx8_C_BigVertexBufferForSpritesVertexCount 0x000F

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
Gx8_tdst_SpecificData  *Gx8_pst_CreateDevice(void);
void                    Gx8_DestroyDevice(void *);
LONG                    Gx8_l_Close(struct GDI_tdst_DisplayData_ *);
#ifdef PSX2_TARGET
#error You're compiling for the wrong platform
LONG                    Gx8_l_Init(struct GDI_tdst_DisplayData_ *);
#else
LONG                    Gx8_l_Init(HWND _hWnd, struct GDI_tdst_DisplayData_ *);
#endif
LONG                    Gx8_l_ReadaptDisplay(HWND, struct GDI_tdst_DisplayData_ *);

/*
 ===================================================================================================
    Flip / clear / surface status
 ===================================================================================================
 */
void                    Gx8_Flip(void);
void                    Gx8_Clear(LONG, ULONG);

/*
 ===================================================================================================
    Render function
 ===================================================================================================
 */

void                    Gx8_SetGlobalAmbient(struct Gx8_tdst_SpecificData_ *pst_SD, ULONG _ul_Color);
void                    Gx8_SetMateriaAmbient(struct Gx8_tdst_SpecificData_ *pst_SD, ULONG _ul_Color);

void                    Gx8_SetViewMatrix(MATH_tdst_Matrix *);
void                    Gx8_SetProjectionMatrix(struct CAM_tdst_Camera_ *);
void                    Gx8_SetTextureBlending( ULONG _l_Texture, ULONG BM, unsigned short s_AditionalFlags );

void                    Gx8_SetTextureTarget(ULONG,ULONG Clear);
void                    Gx8_SetViewMatrix_SDW(struct MATH_tdst_Matrix_ *_pst_Matrix);

void                    Gx8_vBeginRenderUsingTheBigVertexBuffer(UINT uiVertexCount, BYTE **p_vData);
void                    Gx8_vEndRenderUsingTheBigVertexBuffer(UINT uiPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);

void                    Gx8_vBeginRenderUsingTheBigVertexBuffer2D(UINT uiVertexCount, BYTE **p_vData);
void                    Gx8_vEndRenderUsingTheBigVertexBuffer2D(UINT uiPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);

void                    Gx8_vSetNormalMaterial(void);
void                    Gx8_vChangeMaterialForShadow(float fAlpha);

// -------------------------------------------------------------------------------------------------

void					Gx8_SwitchDisplay();

/*
 ===================================================================================================
    Draw primitives
 ===================================================================================================
 */
LONG                    Gx8_l_DrawElementIndexedTriangles( GEO_tdst_ElementIndexedTriangles *, GEO_Vertex *, struct GEO_tdst_UV_ * , ULONG);
LONG                    Gx8_l_DrawElementIndexedSprites(GEO_tdst_ElementIndexedSprite *_pst_Element,GEO_Vertex    *,  ULONG);

/** sprites */
// GVW 21/05
void Gx8_fnvBeforeSprite(int iNumberMaxOfSprite);
void Gx8_fnvEndSprite(int iNumberMaxOfSprite);
void Gx8_fnvAddSprite(GEO_Vertex *p_stPoint,float fSize,ULONG ulColor);

// Sprite VB
void Gx8_BeginSpriteVBCreation( void );
void Gx8_EndSpriteVBCreation( GEO_tdst_Object *_pst_Obj, GEO_tdst_ElementIndexedSprite *pSpriteElement );
void Gx8_DrawSpriteVB( GEO_tdst_Object *_pst_Obj, MAT_tdst_Material *_pst_Material );
BOOL Gx8_CheckSpriteVBCreation(void);

// shadow buffer
void
Gx8_ShadowBuffer_LightRender_Init(Gx8_tdst_SpecificData *pst_SD);
void
Gx8_ShadowBuffer_CameraRender_Init(Gx8_tdst_SpecificData *pst_SD, IDirect3DTexture8 *pRenderTexture);
void
Gx8_ShadowBuffer_ResetRenderStates(Gx8_tdst_SpecificData *pst_SD);


////////////////////////////////NEW CARLONE FOR SHADOW BUFFER//////////////////////////////////////////////////////
void Gx8_SetTextureBlendingSBDiffuse(ULONG _l_Texture, ULONG BM, unsigned short s_AditionalFlags,bool NeedToBlend);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////







// depth/near blur

void Gx8_DepthBlurZBufferRender_Init(Gx8_tdst_SpecificData *pst_SD);
void Gx8_DepthBlurZBufferRender_ResetStates(Gx8_tdst_SpecificData *pst_SD);
void Gx8_NearBlurZBufferRender_Init(Gx8_tdst_SpecificData *pst_SD);
void Gx8_NearBlurZBufferRender_ResetStates(Gx8_tdst_SpecificData *pst_SD);

// video

void Gx8_PlayVideoInTexture(Gx8_tdst_SpecificData *pst_SD, LPDIRECT3DTEXTURE8 destTexture);
bool Gx8_InitVideo(Gx8_tdst_SpecificData *_pst_SD, CHAR* strFilename);
void Gx8_CloseVideo(void);

// for menus
void Gx8_fnvSetAntiAlias(int iSet);
void Gx8_DrawRectangle(void *Dev,float x, float y, float w, float h,u_int Color);
void Gx8_DrawRectangle2(void *Dev,float x, float y, float w, float h,u_int Color,u_int Color2);

void Gx8_Before2D( void );

//HELPER FUNCION TO SARVE AND RESTORE DEVICE SETTINGS
void Gx8_SaveDeviceSettings( Gx8_tdst_SpecificData *pst_DD);
void Gx8_RestoreDeviceSettings( Gx8_tdst_SpecificData *pst_DD );

#if defined(_XBOX)
void Gx8_ScaleBackBuffer(float XScale, float YScale);
void Gx8_Draw2DQuad(float x, float y, float dimx, float dimy, unsigned int Color);
#endif


#ifdef __cplusplus
}
#endif


//New sprite VS rendering.."GIGAFAST"//////////////////////////////////////////////////////////////////////////////////
//#define _NEW_SPRITE_VS_

#define MAX_PART_NUM 40

typedef struct _particleDim
{
	float dimx,dimy,dimz,dimw;
	float u,v;

}particleDim;

typedef struct _particleColors
{
	DWORD color;

}particleColors;


void Gx8_CreateParticleVertexBuffer(Gx8_tdst_SpecificData *pst_SD,int NumParticle);
void Gx8_PrepareParticleVSConstants(Gx8_tdst_SpecificData *pst_SD,SOFT_tdst_AVertex * ,int NumParticle);
void Gx8_PrepareColors(ULONG *pst_Color,int numParticle);
void Gx8_PrepareDeviceForParticle(Gx8_tdst_SpecificData *pst_SD);
void Gx8_CreateParticleVertexShader(Gx8_tdst_SpecificData *pst_SD);
void Gx8_PrepareParticleColors(Gx8_tdst_SpecificData *pst_SD,int NumColors,ULONG *pst_Color,ULONG Alpha,bool alphaOut,DWORD colorAmbient);
void Gx8_PrepareParticleColorsVB(Gx8_tdst_SpecificData *pst_SD,int NumColors,ULONG *pst_Color,ULONG Alpha,bool AlphaOut);
void Gx8_PrepareParticleColorsVBIndex(Gx8_tdst_SpecificData *pst_SD,int NumColors,GEO_tdst_IndexedSprite *pSprite,ULONG *pst_Color,ULONG *pst_Alpha,bool AlphaOut);
void Gx8_PrepareIndexParticleVSConstants(Gx8_tdst_SpecificData *pst_SD,GEO_tdst_IndexedSprite * pVert ,GEO_Vertex *pVertex,int NumParticle);
void Gx8_PrepareParticleColorIndex(Gx8_tdst_SpecificData *pst_SD,int NumColors,GEO_tdst_IndexedSprite *pSprite,ULONG *pst_Color,ULONG *pst_Alpha,bool AlphaOut,DWORD colorAmbient);
void Gx8_PrepareBufferForShader(DWORD *source,float *dest,int numElement);
void Gx8_PrepareParticleColorsFAST(Gx8_tdst_SpecificData *pst_SD,int NumColors,ULONG *pst_Color,ULONG Alpha,bool AlphaOut);
void Gx8_PrepareIndexParticleVSSizeConstant(Gx8_tdst_SpecificData *pst_SD,MATH_tdst_Matrix *pMat ,float,float);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




#endif /* __DX8INIT_H__ */
