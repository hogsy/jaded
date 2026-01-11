// DX9struct.h

#ifndef __DX9STRUCT_H__
#define __DX9STRUCT_H__

#include <d3d9.h>
#include "MATHs/MATH.h"

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************************************
    Constants
 ****************************************************************************************************/

#define MAX_TEXTURE_STAGE	8		// Dx9 supports up to 8 texture stages

#define CIRCULAR_VB_SIZE	0x0000ffff		// size (number of vertices) of a Circular VB

//AfterFX (VM)
#define AEColorET_Max	3
#define ShadowBuffer_MaxLights	2	// maximum number of lights casting (at the same time) shadows
#define ShadowBuffer_ShadowIntensity 128	// shadow intensity

	
/****************************************************************************************************
    Macros
 ****************************************************************************************************/

#define Dx9_M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)

/****************************************************************************************************
    Structure
 ****************************************************************************************************/

/*---------------------------------------------------------------------------------------------------
    UV Generation Type
 ---------------------------------------------------------------------------------------------------*/

typedef enum
{
    Dx9_eUVOff, // directly taken from the vertex UV (no transformation)
    Dx9_eUVNormal, // computed from the camera-space positions
    Dx9_eUVChrome, // computed from the camera-space normals
    Dx9_eUVRotation // computed from the vertex UV
} Dx9_tdeUVGenerationType;



/*---------------------------------------------------------------------------------------------------
    Render State
 ---------------------------------------------------------------------------------------------------*/

typedef struct  Dx9_tdst_RenderState_
{
	D3DZBUFFERTYPE			ZEnable;
    D3DCMPFUNC 				ZFunc;
    BOOL					ZWriteEnable;

    BOOL					FogEnable;
    DWORD					FogColor;

	BOOL					AlphaTestEnable;
	D3DCMPFUNC 				AlphaFunc;
	DWORD					AlphaRef;

	BOOL					AlphaBlendEnable;
	D3DBLEND				SrcBlend;
	D3DBLEND				DestBlend;

	D3DFILLMODE				FillMode;
    BOOL					Lighting;
    D3DCULL					CullMode;
	DWORD					ColorWriteEnable;

	BOOL					NormalizeNormals;
	DWORD					TextureFactor;

	IDirect3DVertexShader9*	CurrentVertexShader;
	union
	{
		IDirect3DVertexDeclaration9*	CurrentVertexDeclaration;
		DWORD							CurrentVertexFormat;
	};
} Dx9_tdst_RenderState;


/*---------------------------------------------------------------------------------------------------
    Sampler State
 ---------------------------------------------------------------------------------------------------*/

typedef struct  Dx9_tdst_SamplerState_
{
	D3DSAMPLERSTATETYPE		AddressU;
	D3DSAMPLERSTATETYPE		AddressV;
	D3DTEXTUREFILTERTYPE	MinFilter;
	D3DTEXTUREFILTERTYPE	MagFilter;
	D3DTEXTUREFILTERTYPE	MipFilter;

} Dx9_tdst_SamplerState;


/*---------------------------------------------------------------------------------------------------
    Texture Stage State
 ---------------------------------------------------------------------------------------------------*/

typedef struct  Dx9_tdst_TextureStageState_
{
    LONG				LastTexture;
	D3DTEXTUREOP		ColorOp;
	DWORD				ColorArg1;		
	DWORD				ColorArg2;
	D3DTEXTUREOP		AlphaOp;
	DWORD				AlphaArg1;
	DWORD				AlphaArg2;
	float				BumpEnvMat00;
	float				BumpEnvMat01;
	float				BumpEnvMat10;
	float				BumpEnvMat11;
	DWORD				TexCoordIndex;
	float				BumpEnvLScale;
	float				BumpEnvLOffset;
	D3DTEXTURETRANSFORMFLAGS TextureTransformFlags;
	DWORD				ColorArg0;
	DWORD				AlphaArg0;
	DWORD				ResultArg;
	DWORD				Constant;

} Dx9_tdst_TextureStageState;


/*---------------------------------------------------------------------------------------------------
    Auxiliary Circular Vertex Buffers
 ---------------------------------------------------------------------------------------------------*/

struct OBJ_tdst_GameObject_;

typedef struct  Dx9_tdst_CircularVB_
{
	IDirect3DVertexBuffer9			* pVB;
	UINT							stride;
	DWORD							FVF;
	UINT							nextIndex;
	UINT							currIndex;
	struct OBJ_tdst_GameObject_		* pCurrGO;		// used to store Current GameObject
	void							* pCurrSource;	// Color Circular VB use it to store Current pColorSource
	UINT							currData1;		// Color Circular VB use it to store Current ulOGLSetCol_XOr
	UINT							currData2;		// Color Circular VB use it to store Current ulOGLSetCol_Or
} Dx9_tdst_CircularVB;


/*---------------------------------------------------------------------------------------------------
    Dx9 Vertex Format
 ---------------------------------------------------------------------------------------------------*/

#define D3DFVF_VERTEXF (D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2 )
typedef struct  Dx9VertexFormat_
{
	float X,Y,Z;
	ULONG Color;
	float fU,fV;
} Dx9VertexFormat;

////////////////////
typedef struct  Dx9VertexComponent_Position_
{
	float X,Y,Z;
} Dx9VertexComponent_Position;

typedef struct  Dx9VertexComponent_Normal_
{
	float X,Y,Z;
} Dx9VertexComponent_Normal;

typedef struct  Dx9VertexComponent_Color_
{
	ULONG Color;
} Dx9VertexComponent_Color;

typedef struct  Dx9VertexComponent_UV_
{
	float U,V;
} Dx9VertexComponent_UV;

/*---------------------------------------------------------------------------------------------------
    Specific data for Dx9 Device
 ---------------------------------------------------------------------------------------------------*/

typedef struct  Dx9_tdst_SpecificData_
{
	// information read from the registry, set up by the external settings application
    UINT					Adapter;
    //D3DDEVTYPE			DeviceType;					// always D3DDEVTYPE_HAL
	D3DFORMAT				AdapterFormat;
	D3DFORMAT				DepthStencilFormat;

	// d3d and device
    HWND                    hWnd;			
	IDirect3D9				* pD3D;
    D3DPRESENT_PARAMETERS   d3dPresentParameters;		// Parameters for CreateDevice/Reset
    IDirect3DDevice9		* pD3DDevice;				// The D3D rendering device
	D3DCAPS9				d3dCaps;					// Caps for the device

	// projection and vieport
	MATH_tdst_Matrix        * pProjectionMatrix;
    RECT                    viewportRect;
    D3DVIEWPORT9            Viewport;					

	// render buffers
	IDirect3DTexture9		* pImageRenderTarget;		
	IDirect3DSurface9		* pDepthStencilSurface;
	IDirect3DSurface9*		pImageFrameSurface[2];			// assume 2-ple buffering
	IDirect3DSurface9*		pDepthSurfaceFrameSurface[2];	// assume 2-ple buffering

	// textures
	IDirect3DTexture9		** TextureList;				// list of pointers to textures
	ULONG					NumberOfTextures;

	// status
	BOOL					RenderScene;				// BeginScene/EndScene status
	Dx9_tdst_RenderState	RenderState;				// current render state
	Dx9_tdst_SamplerState		SamplerState[MAX_TEXTURE_STAGE];		// current sampler state (for each stage)
	Dx9_tdst_TextureStageState	TextureStageState[MAX_TEXTURE_STAGE];	// current texture state (for each stage)
	Dx9_tdeUVGenerationType		UVType;

	// Query Interfaces
	IDirect3DQuery9			* pOcclusionQuery;			// interface for Occlusion Test (flare)

	// fog
    ULONG                   fogState;					// Bit 0 : On-Off, Bit 1 : ForcedToBlack (use fogColorBlack instead of fogColor)
    ULONG                   fogColor;					
    ULONG                   fogColorBlack;

	// VB	------------------------------- TO REMOVE
	IDirect3DVertexBuffer9	* m_VertexBuffer;
    Dx9VertexFormat			* pLockedVertex;
    ULONG					numberOfSpritePrimitive;
    ULONG					numberOfSpritePrimitiveToFill;
	// ------------------------------- TO REMOVE

	// auxiliary rotative VB
	Dx9_tdst_CircularVB		positionVB;					// auxiliary VB for positions
	Dx9_tdst_CircularVB		normalVB;					// auxiliary VB for normals
	Dx9_tdst_CircularVB		colorVB;					// auxiliary VB for colors

	LPDIRECT3DVERTEXDECLARATION9	pVertexDeclarationWithUV;	// 
	LPDIRECT3DVERTEXDECLARATION9	pVertexDeclaration;			// 

	//
    ULONG					ulColorOr;					// Used for Force Alpha full (TExture alpha Only)

	//AfterFx (VM)
	//Todo: remove hungarian notation
	int                     iFadeAlfa;	// used by fade
	IDirect3DTexture9       *pAEColorET[AEColorET_Max]; // AEs extra textures buffer
    bool					bAEColorET_Available[AEColorET_Max]; // extra textures flags (available or not)
	int						iAEColorET_Count; // current number of extra textures used
	int						iNOfSBLights; // number of spot lights casting shadows
    IDirect3DTexture9       *pAlphaForDepthBlurTexture; // alpha texture for depth blur effect
    IDirect3DTexture9       *pAlphaForNearBlurTexture; // alpha texture for near blur effect
	bool					bShadowBuffer; // true if shadow buffer is required
    IDirect3DTexture9       *pBackBufferTexture;
    IDirect3DTexture9       *pFadeBufferTexture;
    IDirect3DTexture9       *pMotionBlurFrameBufferTexture;
    IDirect3DTexture9       *pSmoothBufferTexture;
    IDirect3DTexture9       *pTmpFrameBufferTexture;
    IDirect3DTexture9       tZBufferForDepthBlurTexture; // depth texture for depth blur effect
    IDirect3DTexture9       tZBufferForNearBlurTexture; // depth texture for near blur effect
	IDirect3DTexture9       *pShadowBufferSpot[ShadowBuffer_MaxLights]; // one rendering for each spot
 
} Dx9_tdst_SpecificData;

extern Dx9_tdst_SpecificData gDx9SpecificData;

void Dx9_SaveState(D3DSTATEBLOCKTYPE stateBlock);
void Dx9_RestoreState(void);

#ifdef __cplusplus
}
#endif

#endif /* __DX9STRUCT_H__ */
