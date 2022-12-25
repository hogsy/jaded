#include "Precomp.h"

#ifdef _M_IX86

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "GEOmetric/GEOobject.h"
#include "GDInterface/GDInterface.h"
#include "TEX_mesh.h"
#include "TEX_mesh_internal.h"

float GlobalWaterZ; // yuck! bleah! globalon...

#if defined(_XBOX) || defined(_PC_RETAIL)

#include <xmmintrin.h>

#if defined(_XBOX)

#include "GX8/Gx8init.h"
#include "GX8/Gx8renderstate.h"
#include <d3dx8math.h>

extern Gx8_tdst_SpecificData*			p_gGx8SpecificData;			//GLOBAL...INITIALIZED SOMEWHERE
extern IDirect3DTexture8*				waterReflectionTexture;		//Water texture created by GX8Water
#define gWaterReflectionTexture			waterReflectionTexture

typedef IDirect3DVertexBuffer8			IDirect3DVertexBuffer;
typedef IDirect3DIndexBuffer8			IDirect3DIndexBuffer;

#define DxCreateVertexBuffer(a, b, c)	IDirect3DDevice8_CreateVertexBuffer(p_gGx8SpecificData->mp_D3DDevice, (a), 0, (b), 0, (c))
#define DxVertexBufferLock(a, b)		IDirect3DVertexBuffer8_Lock((a), 0, 0, (BYTE**)(b), 0)
#define DxVertexBufferUnlock(a)			(void)(a)

#define DxCreateIndexBuffer(a, b, c)	IDirect3DDevice8_CreateIndexBuffer(p_gGx8SpecificData->mp_D3DDevice, (a), 0, (b), 0, (c))
#define DxIndexBufferLock(a, b)			IDirect3DIndexBuffer8_Lock((a), 0, 0, (BYTE**)(b), 0)
#define DxIndexBufferUnlock(a)			(void)(a)

#define CONCAT(a, b)	CONCAT1(a, b)
#define CONCAT1(a, b)	a ## b
#define DEVICE_METHOD2(method, a, b)	CONCAT(IDirect3DDevice8_, method)(p_gGx8SpecificData->mp_D3DDevice, (a), (b))
#define DEVICE_METHOD3(method, a, b, c)	CONCAT(IDirect3DDevice8_, method)(p_gGx8SpecificData->mp_D3DDevice, (a), (b), (c))

#define DxGetProjectionMatrix(a)		DEVICE_METHOD2(GetTransform, D3DTS_PROJECTION, (a))
#define DxSetViewMatrix(a)				DEVICE_METHOD2(SetTransform, D3DTS_VIEW, (a))
#define DxSetWorldMatrix(a)				DEVICE_METHOD2(SetTransform, D3DTS_WORLD, (a))
#define DxSetTextureMatrix(a, b)		DEVICE_METHOD2(SetTransform, D3DTS_TEXTURE0 + (a), (b))

#define DxSetColorOp(a, b)				DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_COLOROP, (b))
#define DxSetColorArg1(a, b)			DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_COLORARG1, (b))
#define DxSetColorArg2(a, b)			DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_COLORARG2, (b))
#define DxSetAlphaOp(a, b)				DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_ALPHAOP, (b))
#define DxSetTexCoordIndex(a, b)		DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_TEXCOORDINDEX, (b))
#define DxSetTextureTransformFlags(a, b)DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_TEXTURETRANSFORMFLAGS, (b))
#define DxSetMagFilter(a, b)			DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_MAGFILTER, (b))
#define DxSetMinFilter(a, b)			DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_MINFILTER, (b))
#define DxSetAddressU(a, b)				DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_ADDRESSU, (b))
#define DxSetAddressV(a, b)				DEVICE_METHOD3(SetTextureStageState, (a), D3DTSS_ADDRESSV, (b))
#define DxSetLighting(a)				DEVICE_METHOD2(SetRenderState, D3DRS_LIGHTING, (a))
#define DxSetTextureFactor(a)			DEVICE_METHOD2(SetRenderState, D3DRS_TEXTUREFACTOR, (a))
#define DxSetNormalizeNormals(a)		DEVICE_METHOD2(SetRenderState, D3DRS_NORMALIZENORMALS, (a))
#define DxSetAlphaBlendEnable(a)		DEVICE_METHOD2(SetRenderState, D3DRS_ALPHABLENDENABLE, (a))
#define DxSetZWriteEnable(a)			DEVICE_METHOD2(SetRenderState, D3DRS_ZWRITEENABLE, (a))
#define DxSetZFunc(a)					DEVICE_METHOD2(SetRenderState, D3DRS_ZFUNC, (a))

#define DxBeginScene()					IDirect3DDevice8_BeginScene(p_gGx8SpecificData->mp_D3DDevice)
#define DxEndScene()					IDirect3DDevice8_EndScene(p_gGx8SpecificData->mp_D3DDevice)

#define DxSaveDeviceState()				Gx8_SaveDeviceSettings(p_gGx8SpecificData)
#define DxRestoreDeviceState()			Gx8_RestoreDeviceSettings(p_gGx8SpecificData)

#define DxSetStreamSource(a, b)			IDirect3DDevice8_SetStreamSource(p_gGx8SpecificData->mp_D3DDevice, 0, (a), (b))
#define DxSetIndices(a)					IDirect3DDevice8_SetIndices(p_gGx8SpecificData->mp_D3DDevice, (a), 0)
#define DxSetVertexFormat(a)			IDirect3DDevice8_SetVertexShader(p_gGx8SpecificData->mp_D3DDevice, (a))

#define DxSetTexture(a, b)				IDirect3DDevice8_SetTexture(p_gGx8SpecificData->mp_D3DDevice, (a), (IDirect3DBaseTexture8*)(b))
#define DxDrawIndexedPrimitive(a, b, c)	IDirect3DDevice8_DrawIndexedPrimitive(p_gGx8SpecificData->mp_D3DDevice, (a), 0, (b), 0, (c));

#elif defined(_PC_RETAIL)

#include "Dx9/Dx9draw.h"
#include "Dx9/Dx9matrix.h"
#include "Dx9/Dx9renderstate.h"
#include "Dx9/Dx9samplerstate.h"
#include "Dx9/Dx9texturestagestate.h"
#include <d3dx9math.h>

typedef IDirect3DVertexBuffer9			IDirect3DVertexBuffer;
typedef IDirect3DIndexBuffer9			IDirect3DIndexBuffer;

extern IDirect3DTexture9*				gWaterReflectionTexture;

#define DxCreateVertexBuffer(a, b, c)	IDirect3DDevice9_CreateVertexBuffer(gDx9SpecificData.pD3DDevice, (a), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, (b), 0, (c), 0)
#define DxVertexBufferLock(a, b)		IDirect3DVertexBuffer9_Lock((a), 0, 0, (BYTE**)(b), D3DLOCK_DISCARD)
#define DxVertexBufferUnlock(a)			IDirect3DVertexBuffer9_Unlock(a)

#define DxCreateIndexBuffer(a, b, c)	IDirect3DDevice9_CreateIndexBuffer(gDx9SpecificData.pD3DDevice, (a), D3DUSAGE_WRITEONLY, (b), 0, (c), 0)
#define DxIndexBufferLock(a, b)			IDirect3DIndexBuffer9_Lock((a), 0, 0, (BYTE**)(b), 0)
#define DxIndexBufferUnlock(a)			IDirect3DIndexBuffer9_Unlock(a)

#define DxGetProjectionMatrix			Dx9_GetProjectionMatrixX
#define DxSetViewMatrix					Dx9_SetViewMatrixX
#define DxSetWorldMatrix				Dx9_SetWorldMatrixX
#define DxSetTextureMatrix				Dx9_SetTextureMatrixX

#define DxSetColorOp					Dx9_TSS_ColorOp
#define DxSetColorArg1					Dx9_TSS_ColorArg1
#define DxSetColorArg2					Dx9_TSS_ColorArg2
#define DxSetAlphaOp					Dx9_TSS_AlphaOp
#define DxSetTexCoordIndex				Dx9_TSS_TexCoordIndex
#define DxSetTextureTransformFlags		Dx9_TSS_TextureTransformFlags
#define DxSetMagFilter					Dx9_SS_MagFilter
#define DxSetMinFilter					Dx9_SS_MinFilter
#define DxSetAddressU					Dx9_SS_AddressU
#define DxSetAddressV					Dx9_SS_AddressV
#define DxSetLighting					Dx9_RS_Lighting
#define DxSetTextureFactor				Dx9_RS_TextureFactor
#define DxSetNormalizeNormals			Dx9_RS_NormalizeNormals
#define DxSetAlphaBlendEnable			Dx9_RS_AlphaBlendEnable
#define DxSetZWriteEnable				Dx9_RS_ZWriteEnable
#define DxSetZFunc						Dx9_RS_ZFunc

#define DxBeginScene()					IDirect3DDevice9_BeginScene(gDx9SpecificData.pD3DDevice)
#define DxEndScene()					IDirect3DDevice9_EndScene(gDx9SpecificData.pD3DDevice)

#define DxSaveDeviceState()				Dx9_SaveState(D3DSBT_ALL)
#define DxRestoreDeviceState()			Dx9_RestoreState()

#define DxSetStreamSource(a, b)			IDirect3DDevice9_SetStreamSource(gDx9SpecificData.pD3DDevice, 0, (a), 0, (b))
#define DxSetIndices(a)					IDirect3DDevice9_SetIndices(gDx9SpecificData.pD3DDevice, (a))
#define DxSetVertexFormat(a)			(IDirect3DDevice9_SetVertexShader(gDx9SpecificData.pD3DDevice, 0), \
										 IDirect3DDevice9_SetFVF(gDx9SpecificData.pD3DDevice, (a)))

#define DxSetTexture(a, b)				IDirect3DDevice9_SetTexture(gDx9SpecificData.pD3DDevice, (a), (IDirect3DBaseTexture9*)(b))

#define DxDrawIndexedPrimitive(a, b, c)	IDirect3DDevice9_DrawIndexedPrimitive(gDx9SpecificData.pD3DDevice, (a), 0, 0, (b), 0, (c))

#endif // _XBOX _PC_RETAIL

///////////////////////////////////////////

//Define the vertex format for water
#define WATER_DX_FORMAT  D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_NORMAL | 0x020 // 0x020 is D3DFVF_PSIZE

//Water vertex format (position, normal and color)
typedef struct __declspec(align(32)) WaterVertex
{
	float x;
	float y;
	float z;

	float nx;
	float ny;
	float nz;

	// we use psize as a padding, just to have structure exactly 32 bytes long
	// however, on PC the vertex color is last while on X-Box it comes just after the normals

#ifdef		_XBOX
	int		WaterColor;
	float	padding;
#else
	float	padding;
	int		WaterColor;
#endif

} DXWaterVertex;

static IDirect3DVertexBuffer*	s_pVertexBuffer;
static IDirect3DIndexBuffer*	s_pIndexBuffer;
static int						s_iNumPrimitives;
int	TEX_MESH_g_BlurWaterNormal = 1;

// this function should be declared "static" but I had to remove it because
// it caused a nasty compiler optimization bug in PopulateBuffer on X-Box
// that sucks!!!
void BlurWaterNormal( MATH_tdst_Vector *p_VertexMapNRM , int SX , int SY)
{
	MATH_tdst_Vector *p_VNRM , *p_VNRMLst , *p_VNRMLstLst;

	// the external loop is unrolled and prefetch is used only in the first pass
	p_VNRM = p_VertexMapNRM;
	p_VNRMLstLst = p_VertexMapNRM + SX * (SY - 1);
	while (p_VNRM < p_VNRMLstLst)
	{
		_mm_prefetch((char*)(p_VNRM), _MM_HINT_T0);
		_mm_prefetch((char*)(p_VNRM + SX), _MM_HINT_T0);
		_mm_prefetch((char*)(p_VNRM + 2), _MM_HINT_T0);
		_mm_prefetch((char*)(p_VNRM + SX + 2), _MM_HINT_T0);
		p_VNRMLst = p_VNRM + SX - 1;
		while (p_VNRM < p_VNRMLst)
		{
			_mm_prefetch((char*)(p_VNRM + 4), _MM_HINT_T0);
			_mm_prefetch((char*)(p_VNRM + SX + 4), _MM_HINT_T0);
			p_VNRM->x = (p_VNRM->x + (p_VNRM + 1) ->x + (p_VNRM + SX)->x) * 0.333333f;
			p_VNRM->y = (p_VNRM->y + (p_VNRM + 1) ->y + (p_VNRM + SX)->y) * 0.333333f;
			p_VNRM->z = (p_VNRM->z + (p_VNRM + 1) ->z + (p_VNRM + SX)->z) * 0.333333f;
			p_VNRM++;
		}
		p_VNRM++;
	}

	p_VNRM = p_VertexMapNRM;
	p_VNRMLstLst = p_VertexMapNRM + SX * (SY - 1);
	while (p_VNRM < p_VNRMLstLst)
	{
		p_VNRMLst = p_VNRM + SX - 1;
		while (p_VNRM < p_VNRMLst)
		{
			p_VNRM->x = (p_VNRM->x + (p_VNRM + 1) ->x + (p_VNRM + SX)->x) * 0.333333f;
			p_VNRM->y = (p_VNRM->y + (p_VNRM + 1) ->y + (p_VNRM + SX)->y) * 0.333333f;
			p_VNRM->z = (p_VNRM->z + (p_VNRM + 1) ->z + (p_VNRM + SX)->z) * 0.333333f;
			p_VNRM++;
		}
		p_VNRM++;
	}
}

static void SetTextureRefMatrix(int stage)
{
	D3DMATRIX projMatrix;
	D3DMATRIX mulMAtrix;
	D3DMATRIX finMAtrix;

	//Get projection matrix from pipeline
	DxGetProjectionMatrix(&projMatrix);

	projMatrix._22= -projMatrix._22;

	//Scale and translation matrix
	memset(&mulMAtrix,0,sizeof(mulMAtrix));

	mulMAtrix._11=0.5f;
	mulMAtrix._22=0.5f;
	mulMAtrix._33=1.0f;
	mulMAtrix._44=1.0f;

	//Translate
	mulMAtrix._41= 0.5f;
	mulMAtrix._42= 0.5f;
	mulMAtrix._43= 0.0f;

    D3DXMatrixMultiply(&finMAtrix,&projMatrix,&mulMAtrix);

	//Set this texture matrix for the requested texture stage
	DxSetTextureMatrix(stage, &finMAtrix);
}

static void CreateBuffers(WTR_Generator_Struct* pst_Params)
{
	unsigned iX, iY;
	short *idxBase, *idx, curPos;
	bool duplicateFirst;

	ERR_X_Assert(s_pVertexBuffer == 0);
	ERR_X_Assert(s_pIndexBuffer == 0);

	s_iNumPrimitives = (2 * (pst_Params->FrustrumMesh_SX + 1) + 4) * (pst_Params->FrustrumMesh_SY - 1) - 3;

	DxCreateVertexBuffer(
		pst_Params->FrustrumMesh_SX * pst_Params->FrustrumMesh_SY * sizeof(DXWaterVertex),
		WATER_DX_FORMAT, &s_pVertexBuffer);

	DxCreateIndexBuffer(
		(s_iNumPrimitives + 2) * sizeof(short),
		D3DFMT_INDEX16, &s_pIndexBuffer);

	DxIndexBufferLock(s_pIndexBuffer, &idxBase);

	// each row duplicates the first and last vertex in order to glue them
	// the first vertex of the first row is skipped by specifying a offset of 1 in
	// function SetIndices
	curPos = 0;
	idx = idxBase;
	duplicateFirst = false;
	for(iY = 0; iY < pst_Params->FrustrumMesh_SY - 1; ++iY)
	{
		ERR_X_Assert(curPos == iY * pst_Params->FrustrumMesh_SX);

		if(duplicateFirst)
			*idx++ = curPos;
		else
			duplicateFirst = true;

		for(iX = 0; iX < pst_Params->FrustrumMesh_SX / 2; ++iX)
		{
			*idx++ = curPos;
			*idx++ = (short)(curPos + pst_Params->FrustrumMesh_SX);
			++curPos;
		}
		--iX;
		--curPos;
		*idx++ = (short)(curPos + pst_Params->FrustrumMesh_SX);
		for(; iX < pst_Params->FrustrumMesh_SX; ++iX)
		{
			*idx++ = (short)(curPos + pst_Params->FrustrumMesh_SX);
			*idx++ = curPos;
			++curPos;
		}
		*idx++ = curPos - 1;
		*idx++ = curPos - 1;
	}

	// sanity check
	ERR_X_Assert((idx - idxBase) == s_iNumPrimitives + 2);

	DxIndexBufferUnlock(s_pIndexBuffer);
}

static void PopulateBuffer(WTR_Generator_Struct * pst_Params)
{
	#define M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)
	float normalCoeff;
	MATH_tdst_Vector *Pvertex, *PvertexLast, *PvertNormal;
	ULONG *pVertexCol;
	u32 YCounter;
	BYTE *pData;
	DXWaterVertex tempVertex;
	float camX = GDI_gpst_CurDD->st_Camera.st_Matrix.T.x;
	float camY = GDI_gpst_CurDD->st_Camera.st_Matrix.T.y;
	float camZ = GDI_gpst_CurDD->st_Camera.st_Matrix.T.z;

	//Lock the vertex bufer to write in water mesh
	DxVertexBufferLock(s_pVertexBuffer, &pData);

	//Water mesh's source for vertex and color
	Pvertex = pst_Params->pVertexMap;
	pVertexCol = pst_Params->pColors;

	//Water mesh's source for normals
	PvertNormal = pst_Params->pVertexMap_NRM;

	//Blur normals (to be checked...i don't know if this is useful)
	if(TEX_MESH_g_BlurWaterNormal)
		BlurWaterNormal(PvertNormal, pst_Params->FrustrumMesh_SX, pst_Params->FrustrumMesh_SY);

	// pump up the cache
	_mm_prefetch((char*)Pvertex, _MM_HINT_NTA);
	_mm_prefetch((char*)(Pvertex + 2), _MM_HINT_NTA);
	_mm_prefetch((char*)PvertNormal, _MM_HINT_NTA);
	_mm_prefetch((char*)(PvertNormal + 2), _MM_HINT_NTA);
	_mm_prefetch((char*)pVertexCol, _MM_HINT_NTA);

	//Convert vertex information in DX format and copy these information in vertex buffer
	for (YCounter = 0; YCounter < pst_Params->FrustrumMesh_SY; ++YCounter)
	{
		//Calculate coefficient (used for normal perturbation) based on distance from point of view
		float depthCoeff= (((float)YCounter * 1.2f) / (float)pst_Params->FrustrumMesh_SY);

		if(depthCoeff > 1.0f)
			depthCoeff = 1.0f;

		//Calculate the mesh "normals" perturbation factor (normals are more perturbed as far triangle are from point of view)
		normalCoeff= 48.0f - depthCoeff * 45.0f;

		PvertexLast = Pvertex + pst_Params->FrustrumMesh_SX;
		while (Pvertex < PvertexLast)
		{
			_mm_prefetch((char*)(Pvertex + 4), _MM_HINT_NTA);
			_mm_prefetch((char*)(PvertNormal + 4), _MM_HINT_NTA);
			_mm_prefetch((char*)(pVertexCol + 4), _MM_HINT_NTA);

			tempVertex.x = Pvertex->x;
			tempVertex.y = Pvertex->y;
			tempVertex.z = Pvertex->z;
			tempVertex.nx = tempVertex.x - camX;
			tempVertex.ny = tempVertex.y - camY;
			tempVertex.nz = tempVertex.z - camZ - (float)fabs((PvertNormal->x + PvertNormal->y) * normalCoeff);
			tempVertex.WaterColor = M_ConvertColor(*pVertexCol);


			// copy tempVertex in the vertex buffer using SSE to minimize cache
			// pollution (the copy happens in reverse order intentionally)
			_mm_stream_ps((float*)(pData + 16), _mm_load_ps(&tempVertex.ny));
			_mm_stream_ps((float*)pData, _mm_load_ps(&tempVertex.x));

			pData += 32;
			++Pvertex;
			++pVertexCol;
			++PvertNormal;
		} // end while (X coordinate)

	} // end for (Y coordinate)

	DxVertexBufferUnlock(s_pVertexBuffer);
}

void TEX_x86_RenderWater(WTR_Generator_Struct *pst_Params)
{
	//water color
	int WATER_COLOR = ((255<<24)|(0<<16)|(0<<8)|(0));

	MATH_tdst_Matrix st_FinalMatrix;
	MATH_tdst_Matrix st_TempMatrix;
	MATH_tdst_Matrix st_InvMatrix;
	int ulFlags = 0;

	// Create the vertex buffer (big enough to contail all the strip) and the index
	// buffer, the index buffer doesn't change during the game so fill it now
	if(!s_pVertexBuffer)
		CreateBuffers(pst_Params);

	// sanity check
	ERR_X_Assert(s_iNumPrimitives == (2 * (pst_Params->FrustrumMesh_SX + 1) + 4) * (pst_Params->FrustrumMesh_SY - 1) - 3);

	// populate buffer by converting the vertices from the engine format to the DX format
	PopulateBuffer(pst_Params);

	// save device settings
	DxSaveDeviceState();

	// view matrix manipulation
	MATH_CopyMatrix(&st_TempMatrix, &GDI_gpst_CurDD->st_Camera.st_Matrix);

	MATH_InvertMatrix(&st_InvMatrix, &st_TempMatrix);
	st_InvMatrix.Sx = st_InvMatrix.Sy = st_InvMatrix.Sz=0.0f;
	st_InvMatrix.w = 1.0f;

	MATH_MakeOGLMatrix(&st_FinalMatrix, &st_InvMatrix);
	st_FinalMatrix.w = 1.0f;
	DxSetViewMatrix((D3DMATRIX*)&st_FinalMatrix);

	MATH_SetIdentityMatrix(&st_TempMatrix);
	MATH_MakeOGLMatrix(&st_FinalMatrix, &st_TempMatrix);
	DxSetWorldMatrix((D3DMATRIX*)&st_FinalMatrix);

	// Texture coordinate generation settings
	SetTextureRefMatrix(0);

	// Settings for reflection
	DxSetTexCoordIndex(0, D3DTSS_TCI_CAMERASPACENORMAL);
	DxSetTextureTransformFlags(0, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED );

	// no normalized normal vector in camera space
	DxSetNormalizeNormals(FALSE);

	// Set the stream source, indices and vertex format
	DxSetStreamSource(s_pVertexBuffer, sizeof(DXWaterVertex));
	DxSetIndices(s_pIndexBuffer);
	DxSetVertexFormat(WATER_DX_FORMAT);

	// no alpha blending
	DxSetAlphaBlendEnable(FALSE);

	//Textures filters and clamping
	DxSetLighting(FALSE);
	DxSetTextureFactor(WATER_COLOR);
	DxSetColorOp  (0, D3DTOP_BLENDDIFFUSEALPHA);
	DxSetColorArg1(0, D3DTA_TEXTURE);
	DxSetColorArg2(0, D3DTA_DIFFUSE);
	DxSetAlphaOp  (0, D3DTOP_DISABLE);
	DxSetMagFilter(0, D3DTEXF_LINEAR);
	DxSetMinFilter(0, D3DTEXF_LINEAR);
	DxSetAddressU (0, D3DTADDRESS_CLAMP);
	DxSetAddressV (0, D3DTADDRESS_CLAMP);

	// Water reflection texture (stage 0)
	DxSetTexture(0, gWaterReflectionTexture);

	// normal zwrite and ztest (I don't need the water to be transparent)
	DxSetZWriteEnable(FALSE);
	DxSetZFunc(D3DCMP_LESSEQUAL);

	// draw everything as a single strip
	DxDrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		pst_Params->FrustrumMesh_SX * pst_Params->FrustrumMesh_SY, s_iNumPrimitives);

	// restore device settings
	DxRestoreDeviceState();
}

#else // defined(_XBOX) || defined(_PC_RETAIL)

void TEX_x86_RenderWater(WTR_Generator_Struct *pst_Params)
{
	(void)pst_Params;
}

#endif // defined(_XBOX) || defined(_PC_RETAIL)

#endif // _M_IX86