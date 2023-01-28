#include "Precomp.h"

/*DECOMMENT THIS IF YOU DON'T WANT WATER DISPLAY
#ifndef _NO_WATER_
#define _NO_WATER_
#endif
*/

#if !defined( PSX2_TARGET ) && !defined( _GAMECUBE ) && !defined( _XBOX ) && !defined( _PC_RETAIL ) && !defined(_XENON)
#define WTR_OGL_VERSION
#define WTR_ComputeOriginalsUV
#endif

#include <math.h>

#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"

#include "GEOmetric/GEOobject.h"

#include "TEXture/WATER_FFT.h"
#include "TEXture/TEX_Mesh.h"
#include "TEXture/TEX_Mesh_internal.h"

#include "SOFT/SOFTzlist.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDIrequest.h"

#ifdef WTR_OGL_VERSION
#define WTR_ComputeOriginalsUV
#include "..\OpenGL\Sources\OGLinit.h"

// God fucking damn it Ubisoft
// TODO: consolidate GL specific code into GL specific driver!! ~hogsy
#include <GL/GL.h>
#endif

#ifdef _XENON_RENDER
#include "../XenonGraphics/XeMesh.h"
#include "../XenonGraphics/XeDynVertexBuffer.h"
#include "../XenonGraphics/XeRenderer.h"
#endif

#if defined( _M_IX86 ) /* && !defined( ACTIVE_EDITORS ) && ( defined( _PC_RETAIL ) || defined( _XBOX ) || defined( _XENON ) )*/
#define TEX_MESH_USE_SSE
#endif

#ifdef TEX_MESH_USE_SSE
#include <xmmintrin.h>

__inline __m128 ubi_mm_hsum_3d_ss(__m128 a)
{
	return _mm_add_ss(a, _mm_add_ss(_mm_shuffle_ps(a, a, 1), _mm_shuffle_ps(a, a, 2)));
}

__inline __m128 ubi_mm_norm2_3d_ss(__m128 a)
{
	return ubi_mm_hsum_3d_ss(_mm_mul_ps(a, a));
}

__inline __m128 ubi_mm_dotprod_3d_ss(__m128 a, __m128 b)
{
	return ubi_mm_hsum_3d_ss(_mm_mul_ps(a, b));
}

__inline __m128 ubi_mm_crossprod_3d_ps(__m128 a, __m128 b)
{
	return _mm_sub_ps(
				_mm_mul_ps(
					_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
					_mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
				_mm_mul_ps(
					_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)),
					_mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
}

__inline __m128 ubi_mm_ss_to_ps(__m128 a)
{
	return _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 0, 0, 0));
}

__inline void ubi_mm_store_3d_ps(GEO_Vertex* dst, __m128 src)
{
	_mm_store_ss(&dst->x, _mm_shuffle_ps(src, src, _MM_SHUFFLE(0, 0, 0, 0)));
	_mm_store_ss(&dst->y, _mm_shuffle_ps(src, src, _MM_SHUFFLE(1, 1, 1, 1)));
	_mm_store_ss(&dst->z, _mm_shuffle_ps(src, src, _MM_SHUFFLE(2, 2, 2, 2)));
}

__inline __m128 ubi_mm_normalize_3d_ps(__m128 a)
{
	return _mm_mul_ps(a, ubi_mm_ss_to_ps(_mm_rsqrt_ss(ubi_mm_norm2_3d_ss(a))));
}

#endif

///////XBOX SPECIFIC  -  USED FOR WATER RENDERING  -
#ifdef _XBOX
#if !defined(_XENON)
#include <d3d8.h>
#endif
#include "GX8/Gx8init.h"
#include "GX8/Gx8renderstate.h"

//GLOBAL...INITIALIZED SOMEWHERE
extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;

//Water textures create by GX8Water
extern IDirect3DTexture8 * waterReflectionTexture;  //The reflection
extern IDirect3DTexture8 * waterRefractionTexture;	//The refraction
extern IDirect3DTexture8 * waterReflectionCopyTexture; //Copy of reflection (without highlight)
extern IDirect3DTexture8 * sunReflectionAlphaMask;
#define Gx8_M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)
//Draw the "higlight" (like the sun) over the reflection map, before drawing the water
extern void Gx8_DrawSkyLight();

//The GLOBAL water level (always 0)
float GlobalWaterZ;

#endif
///////////////////////////////////////////




#include "GEOmetric/GEO_STRIP.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#include "BASe/BENch/BENch.h"

extern int LINK_gi_SpeedDraw;

#if 1

#define u32 ULONG
#define s32 LONG

#ifdef _XBOX
//#define WTR_ComputeNORMALS   //COMPUTE WATER MESH NORMALS...
#endif

#if defined(_XENON_RENDER)
#define WTR_ComputeNORMALS   //COMPUTE WATER MESH NORMALS...
#endif


#define WTR_SCDPASS_COS_ALP	0.9848077530f * 0.9f // 10°
#define WTR_SCDPASS_SIN_ALP	0.1736481776f * 0.9f // 10°

#define VM_DISCX_DEFAULT 120//50
#define VM_DISCY_DEFAULT 150//120
#define WTR_ComputeOriginalsUV
#define WAveMapShift_Default 6
#define WAveMapDXYShift_Default 6

#define PSX2_ASM_PREFETCH(a) lq $0 , a//pref 0 , a //lq	$0 , a

#ifdef TEX_MESH_USE_SSE
typedef __m128 GEO_VertexForZmap;
#else
typedef GEO_Vertex GEO_VertexForZmap;
#endif

typedef struct WTR_Generator_Struct_
{
	GEO_Vertex		 	*pVertexMap;
#ifdef WTR_ComputeNORMALS
	GEO_Vertex			*pVertexMap_NRM;
	GEO_Vertex			*pVertexMap_NRMSPEC;
#endif
#ifdef WTR_ComputeOriginalsUV
	SOFT_tdst_UV		*p_VertexMapORIGINALS;
#endif
#ifdef _GAMECUBE
	unsigned char 		*ucWATER_DisplayList;
#endif
	ULONG				*pColors;
	FFT_Complex			*pSpaceMap;
	FFT_Complex			*pSpaceMapDX;
	FFT_Complex			*pSpaceMapDY;
	GEO_VertexForZmap	*pConcatenedMap;
	u32					FrustrumMesh_SX;
	u32					FrustrumMesh_SY;
	float				ZFactor;
	float				fChoppyFactor ;
	float				HCoef ;
	float				DisturbFactor ;
	float				SpeedFactor ;
	float				GlobalWaterZ;
	float				WaterFog ;
	tdst_FFT			WTR ONLY_PSX2_ALIGNED(32);
	u32					ulWAveMapShift;
	u32					ulWAveMapShiftCHP;
	float				fftime;
	u32					pColorTable[512];
	u32					ulColorTableFastRecomputeOK;
	u32					pColorTableFastRecompute[512];
	u32					pColorTableSave[128];
	u32					ColorTableMod; // 0 == Ntocomp ; 1 == Up ; 2 == Sub
	u32					SkyCol;	
	u32					BottomCol;	
	u32					WaterCol;
	u32					ulTextureRef;
	u32					CloudShadingEnabled;	
	u32					ulSymetryActive;

	float				SclaeUVFactor;
#ifdef ACTIVE_EDITORS
	WATER_Export_Struct *pBackRef;
#endif
}
WTR_Generator_Struct;

void WTR_ComputeFresnel(WTR_Generator_Struct *pst_Params);
void WTR_Concate(WTR_Generator_Struct *pst_Params);
u32 WTR_Vector_2_Color(MATH_tdst_Vector *stColor);
void WTR_Color_2_Vector(MATH_tdst_Vector *stColorDST , u32 Color);

#ifdef PSX2_TARGET
#define WTR_fTrunc(CX) iRoundf(CX)
#else
#define WTR_fTrunc(CX) lFloatToLong(CX - 0.5f)
#endif

void *WTR_AllocAlign(ULONG Size , ULONG Align)
{
	void *Ptr;
	Ptr = MEM_p_AllocAlign(Size , Align);
	L_memset(Ptr , 0 , Size);
	return Ptr;
}

_inline_ void MATH_CrossProduct2
(
	register MATH_tdst_Vector	*dst,
	register MATH_tdst_Vector	*vec1,
	register MATH_tdst_Vector	*vec2
)
{
	dst->x = fMul(vec1->y, vec2->z) - fMul(vec1->z, vec2->y);
	dst->y = fMul(vec1->z, vec2->x) - fMul(vec1->x, vec2->z);
	dst->z = fMul(vec1->x, vec2->y) - fMul(vec1->y, vec2->x);
}

_inline_ void MATH_SubVector2
(
	register MATH_tdst_Vector	*dst,
	register MATH_tdst_Vector	*vec1,
	register MATH_tdst_Vector	*vec2
)
{
	dst->x = vec1->x - vec2->x;
	dst->y = vec1->y - vec2->y;
	dst->z = vec1->z - vec2->z;
}

void WTR_AllocSubStruct(WTR_Generator_Struct *pst_Params)
{
	pst_Params ->pVertexMap		= (MATH_tdst_Vector*)WTR_AllocAlign(pst_Params->FrustrumMesh_SX * (pst_Params->FrustrumMesh_SY + 1) * sizeof(GEO_Vertex) , 16);
#ifdef WTR_ComputeNORMALS
	pst_Params ->pVertexMap_NRM  = (MATH_tdst_Vector*)WTR_AllocAlign(pst_Params->FrustrumMesh_SX * (pst_Params->FrustrumMesh_SY + 1) * sizeof(GEO_Vertex) , 16);
	pst_Params ->pVertexMap_NRMSPEC  = (MATH_tdst_Vector*)WTR_AllocAlign(pst_Params->FrustrumMesh_SX * (pst_Params->FrustrumMesh_SY + 1) * sizeof(GEO_Vertex) , 16);
#endif
#ifdef WTR_ComputeOriginalsUV
	pst_Params ->p_VertexMapORIGINALS  = (SOFT_tdst_UV*)WTR_AllocAlign(pst_Params->FrustrumMesh_SX * (pst_Params->FrustrumMesh_SY + 1) * sizeof(SOFT_tdst_UV) , 16);
#endif
#ifdef _GAMECUBE
	{
		extern void WATER_ComputeDisplayList(unsigned char *pDL , u32 SX , u32 SY);
		pst_Params ->ucWATER_DisplayList = WTR_AllocAlign(WTR_DISPLAYLISTSIZE(pst_Params->FrustrumMesh_SX , pst_Params->FrustrumMesh_SY) + 32, 32);
		WATER_ComputeDisplayList(pst_Params ->ucWATER_DisplayList , pst_Params->FrustrumMesh_SX , pst_Params->FrustrumMesh_SY);
	}
#endif

	pst_Params ->pColors 		= (ULONG*)WTR_AllocAlign(pst_Params->FrustrumMesh_SX * (pst_Params->FrustrumMesh_SY + 1) * sizeof(ULONG) , 16);

	pst_Params ->pSpaceMap		= (FFT_Complex *)WTR_AllocAlign((1 << (pst_Params->ulWAveMapShift << 1)) * sizeof(FFT_Complex) , 16);
	pst_Params ->pSpaceMapDX	= (FFT_Complex *)WTR_AllocAlign((1 << (pst_Params->ulWAveMapShiftCHP << 1)) * sizeof(FFT_Complex) , 16);
	pst_Params ->pSpaceMapDY	= (FFT_Complex *)WTR_AllocAlign((1 << (pst_Params->ulWAveMapShiftCHP << 1)) * sizeof(FFT_Complex) , 16);
	
	{
		ULONG SizeOfConcatenedMAp;
		SizeOfConcatenedMAp = ((2 << pst_Params->ulWAveMapShift) + (1 << (pst_Params->ulWAveMapShift << 1))) + 1;
		SizeOfConcatenedMAp += SizeOfConcatenedMAp >> 1; // MipMapping
		pst_Params -> pConcatenedMap	= (GEO_VertexForZmap*)WTR_AllocAlign(SizeOfConcatenedMAp * sizeof(GEO_VertexForZmap), 32);
	}
}

void WTR_FreeSubStruct(WTR_Generator_Struct *pst_Params)
{
	if (pst_Params ->pVertexMap)			MEM_FreeAlign(pst_Params ->pVertexMap);
#ifdef WTR_ComputeNORMALS
	if (pst_Params ->pVertexMap_NRM )		MEM_FreeAlign(pst_Params ->pVertexMap_NRM );
	if (pst_Params ->pVertexMap_NRMSPEC )		MEM_FreeAlign(pst_Params ->pVertexMap_NRMSPEC );
	pst_Params ->pVertexMap_NRM			= NULL;
	pst_Params ->pVertexMap_NRMSPEC				= NULL;
#endif

#ifdef WTR_ComputeOriginalsUV
	if (pst_Params ->p_VertexMapORIGINALS )		MEM_FreeAlign(pst_Params ->p_VertexMapORIGINALS );
	pst_Params ->p_VertexMapORIGINALS = NULL;
#endif

#ifdef _GAMECUBE
	if (pst_Params ->ucWATER_DisplayList )		MEM_FreeAlign(pst_Params ->ucWATER_DisplayList );
#endif

	if (pst_Params ->pColors )		MEM_FreeAlign(pst_Params ->pColors);
	
	if (pst_Params ->pSpaceMap )			MEM_FreeAlign(pst_Params ->pSpaceMap );
	if (pst_Params ->pSpaceMapDX )			MEM_FreeAlign(pst_Params ->pSpaceMapDX );
	if (pst_Params ->pSpaceMapDY )			MEM_FreeAlign(pst_Params ->pSpaceMapDY );
	if (pst_Params ->pConcatenedMap)		MEM_FreeAlign(pst_Params ->pConcatenedMap);
	
	if (pst_Params->WTR.pSpectroMap)	MEM_FreeAlign(pst_Params->WTR.pSpectroMap);
	if (pst_Params->WTR.pDXDYMap)		MEM_FreeAlign(pst_Params->WTR.pDXDYMap);

	pst_Params ->pVertexMap				= NULL;
#ifdef _GAMECUBE
	pst_Params ->ucWATER_DisplayList  	= NULL;
#endif

	pst_Params ->pColors 				= NULL;
	pst_Params ->pSpaceMap				= NULL;
	pst_Params ->pSpaceMapDX			= NULL;
	pst_Params ->pSpaceMapDY			= NULL;
	pst_Params ->pConcatenedMap			= NULL;
	pst_Params->WTR.pSpectroMap	= NULL;
	pst_Params->WTR.pDXDYMap		= NULL;
}
void WTR_SetDefault(WTR_Generator_Struct *pst_Params)
{
	L_memset(pst_Params, 0 , sizeof(WTR_Generator_Struct));
	pst_Params->ulWAveMapShift = WAveMapShift_Default;
	pst_Params->ulWAveMapShiftCHP = WAveMapDXYShift_Default;
	pst_Params->WTR . BorderSizePo2 = pst_Params->ulWAveMapShift;
	pst_Params->WTR . DXDYBorderSizePo2 = pst_Params->ulWAveMapShiftCHP;
	pst_Params->WTR . Wind.X = 60.0f;
	pst_Params->WTR . Wind.Y = 0.0f;
	pst_Params->WTR . WindBalance = 1.0f;
	pst_Params->WTR . WorldSizeInMeters = 10.f;
	pst_Params->WTR . P_Factor = 2.0f;
	pst_Params->ZFactor = 0.13f;
	pst_Params->fChoppyFactor = 0.30f;
	pst_Params->HCoef = 0.01f;
	pst_Params->DisturbFactor = 0.3f;
	pst_Params->SpeedFactor = 0.3f;
	pst_Params->GlobalWaterZ = -1.0f;
	pst_Params->WaterFog = 50.0f;
	pst_Params->FrustrumMesh_SX = VM_DISCX_DEFAULT;
	pst_Params->FrustrumMesh_SY = VM_DISCY_DEFAULT;
	pst_Params->CloudShadingEnabled = 0;
	pst_Params->SclaeUVFactor = 0.004f;
	pst_Params->ulSymetryActive = 0;
	{
			MATH_tdst_Vector stBottomCol , stWaterCol , stSkyCol ;
			MATH_InitVector(&stSkyCol , 0.6f , 0.6f , 0.6f);
			MATH_InitVector(&stWaterCol , 0.3f , 0.4f , 0.5f); 
			MATH_ScaleVector(&stBottomCol , &stWaterCol , 0.1f);

/*			MATH_InitVector(&stSkyCol , 0.5f , 0.5f , 0.5f);
			MATH_InitVector(&stBottomCol , 0.1f , 0.3f , 0.3f);
			MATH_InitVector(&stWaterCol , 0.2f , 0.5f , 0.5f);
		//*/	
/*#ifdef _GAMECUBE		
			MATH_InitVector(&stSkyCol , 0.3f , 0.4f , 0.5f); 
			MATH_InitVector(&stWaterCol , 0.3f , 0.4f , 0.5f); 
			MATH_ScaleVector(&stBottomCol , &stWaterCol , 0.4f);
			MATH_ScaleVector(&stSkyCol , &stSkyCol , 1.5f);

#endif	*/		
//			MATH_ScaleVector(&stSkyCol , &stSkyCol , 0.5f);//*/
			
			pst_Params->SkyCol = WTR_Vector_2_Color(&stSkyCol);
			pst_Params->BottomCol = WTR_Vector_2_Color(&stBottomCol);
			pst_Params->WaterCol = WTR_Vector_2_Color(&stWaterCol);
	}

}

WTR_Generator_Struct *WTR_Mesh_Create()
{
	WTR_Generator_Struct *pst_Params;
	pst_Params = (WTR_Generator_Struct*)MEM_p_AllocAlign(sizeof(WTR_Generator_Struct) , 32);
	WTR_SetDefault(pst_Params);
	WTR_AllocSubStruct(pst_Params);
	WATERFFT_Init_Spectre(&pst_Params->WTR);
	WTR_ComputeFresnel(pst_Params);
	return pst_Params;
}


WTR_Generator_Struct *pst_GlobalsWaterParams = NULL;
WTR_Generator_Struct *pst_GlobalsWaterParams_Save = NULL;
WATER_Export_Struct stExportWaterParrams;

void WTR_Mesh_Destroy(WTR_Generator_Struct *pst_WaterParams)
{
	WTR_FreeSubStruct(pst_WaterParams);
	MEM_FreeAlign(pst_WaterParams);
}


void WTR_SetDefaulExp(WATER_Export_Struct *pExp)
{
	WTR_Generator_Struct WaterParams;
	WTR_SetDefault(&WaterParams);
	pExp -> ABadCafe		= 0xABadCafe;
	pExp -> Activate		= 0 ;
	pExp -> FrustrumMesh_SX	= WaterParams . FrustrumMesh_SX;
	pExp -> FrustrumMesh_SY	= WaterParams . FrustrumMesh_SY;
	pExp -> ulWAveMapShift	= WaterParams . ulWAveMapShift;
	pExp -> ulWAveMapShiftCHP= WaterParams . ulWAveMapShiftCHP;
	pExp -> BottomCol		= WaterParams . BottomCol;
	pExp -> WaterCol		= WaterParams . WaterCol;
	pExp -> SkyCol			= WaterParams . SkyCol;
	pExp -> ZFactor			= WaterParams . ZFactor;
	pExp -> fChoppyFactor	= WaterParams . fChoppyFactor;
	pExp -> HCoef			= WaterParams . HCoef;
	pExp -> SpeedFactor		= WaterParams . SpeedFactor;
	pExp -> GlobalWaterZ	= WaterParams . GlobalWaterZ;
	pExp -> WindBalance 	= WaterParams . WTR . WindBalance ;
	pExp -> P_Factor		= WaterParams . WTR . P_Factor;
	pExp -> CloudShadingEnabled= WaterParams . CloudShadingEnabled;
	pExp -> ulTextureRef = WaterParams . ulTextureRef ;
	pExp -> ulSymetryActive = WaterParams . ulSymetryActive ;
	pExp -> TextorigFactor = WaterParams .SclaeUVFactor ;
}
float WTR_GetWaterZ()
{
	if  (pst_GlobalsWaterParams) return pst_GlobalsWaterParams->GlobalWaterZ;
	else  return 0.0f;
}
void WTR_SetWaterZ(float Z)
{
	if  (pst_GlobalsWaterParams) 
	{
		pst_GlobalsWaterParams->GlobalWaterZ = Z;
		stExportWaterParrams.GlobalWaterZ = Z;
	}
}
void WTR_SetExportParams(WATER_Export_Struct *pExp)
{
	ULONG ulMustRealoc,ulMustReinit,ulMustComputeFresnel;
	ulMustRealoc = 0;
	ulMustReinit = 0;
	ulMustComputeFresnel = 0;
	
#if defined( PSX2_TARGET) || defined (_XBOX) || defined (_GAMECUBE)
	pExp -> FrustrumMesh_SX = VM_DISCX_DEFAULT;
	pExp -> FrustrumMesh_SY = VM_DISCY_DEFAULT;
	pExp -> ulWAveMapShift = WAveMapShift_Default;
	pExp -> ulWAveMapShiftCHP = WAveMapDXYShift_Default;//*/
	
//	pExp->Activate = 0;
#endif

	if (!pst_GlobalsWaterParams)
	{
		if (!pExp->Activate) return;
		if (pst_GlobalsWaterParams_Save)
		{
			pst_GlobalsWaterParams = pst_GlobalsWaterParams_Save;
			pst_GlobalsWaterParams_Save = NULL;
		} else
		{
			pst_GlobalsWaterParams = (WTR_Generator_Struct*)MEM_p_AllocAlign(sizeof(WTR_Generator_Struct) , 32);
			WTR_SetDefault(pst_GlobalsWaterParams);
			ulMustComputeFresnel = 1;
			ulMustRealoc = 1;
			ulMustReinit = 1;
		}
	} else
	{
		if (!pExp->Activate) 
		{
			pst_GlobalsWaterParams_Save = pst_GlobalsWaterParams;
			pst_GlobalsWaterParams = NULL;
			return;
		}
	}

	if (!pExp -> FrustrumMesh_SX) pExp -> FrustrumMesh_SX = pst_GlobalsWaterParams -> FrustrumMesh_SX;
	if (!pExp -> FrustrumMesh_SY) pExp -> FrustrumMesh_SY = pst_GlobalsWaterParams -> FrustrumMesh_SY;
	if (!pExp -> ulWAveMapShift) pExp -> ulWAveMapShift = pst_GlobalsWaterParams -> ulWAveMapShift;


	if (pExp -> ulWAveMapShift > 9) pExp -> ulWAveMapShift = 9;
	if (pExp -> ulWAveMapShift < 4) pExp -> ulWAveMapShift = 4;

	if (pExp -> ulWAveMapShiftCHP > 9) pExp -> ulWAveMapShiftCHP = 9;
	if (pExp -> ulWAveMapShiftCHP < 4) pExp -> ulWAveMapShiftCHP = 4;
	if (pExp -> ulWAveMapShiftCHP > pExp -> ulWAveMapShift) pExp -> ulWAveMapShiftCHP = pExp -> ulWAveMapShift;

	if ((pExp->SkyCol	!= pst_GlobalsWaterParams ->SkyCol) || 
		(pExp->BottomCol!= pst_GlobalsWaterParams ->BottomCol) ||
		(pExp->WaterCol	!= pst_GlobalsWaterParams ->WaterCol	)) 
			ulMustComputeFresnel = 1;


	if ((pExp->FrustrumMesh_SX	> pst_GlobalsWaterParams ->FrustrumMesh_SX) || 
		(pExp->FrustrumMesh_SY	> pst_GlobalsWaterParams ->FrustrumMesh_SY) ||
		(pExp->ulWAveMapShift	> pst_GlobalsWaterParams ->ulWAveMapShift) || 
		(pExp->ulWAveMapShiftCHP > pst_GlobalsWaterParams ->ulWAveMapShiftCHP) 		)
			ulMustRealoc = 1;

	if ((pExp->FrustrumMesh_SX	!= pst_GlobalsWaterParams ->FrustrumMesh_SX) || 
		(pExp->FrustrumMesh_SY	!= pst_GlobalsWaterParams ->FrustrumMesh_SY) ||
		(pExp->ulWAveMapShift	!= pst_GlobalsWaterParams ->ulWAveMapShift) || 
		(pExp->ulWAveMapShiftCHP != pst_GlobalsWaterParams ->ulWAveMapShiftCHP) 		)
		ulMustReinit = 1;
		
#if 0//ACTIVE_EDITORS
	{
		static BOOL b_Speed = FALSE;
		static ULONG sFrustrumMesh_SX;
		static ULONG sFrustrumMesh_SY;
		static ULONG sulWAveMapShift;
		static ULONG sulWAveMapShiftCHP;

		if(LINK_gi_SpeedDraw)
		{
			if(!b_Speed)
			{
				sFrustrumMesh_SX = pExp->FrustrumMesh_SX;
				sFrustrumMesh_SY = pExp->FrustrumMesh_SY;
				sulWAveMapShift = pExp->ulWAveMapShift;
				sulWAveMapShiftCHP = pExp->ulWAveMapShiftCHP;
				b_Speed = TRUE;
			}

			pExp->FrustrumMesh_SX = 24;
			pExp->FrustrumMesh_SY = 64;
			pExp->ulWAveMapShift = 5;
			pExp->ulWAveMapShiftCHP = 5;
		}
		else
		{
			if(b_Speed)
			{
				pExp->FrustrumMesh_SX = sFrustrumMesh_SX;
				pExp->FrustrumMesh_SY = sFrustrumMesh_SY;
				pExp->ulWAveMapShift = sulWAveMapShift;
				pExp->ulWAveMapShiftCHP = sulWAveMapShiftCHP;
				b_Speed = FALSE;
			}
		}
	}
#endif

	pst_GlobalsWaterParams -> FrustrumMesh_SX	= pExp -> FrustrumMesh_SX ;
	pst_GlobalsWaterParams -> FrustrumMesh_SY	= pExp -> FrustrumMesh_SY ;
	pst_GlobalsWaterParams -> ulWAveMapShift	= pExp -> ulWAveMapShift;
	pst_GlobalsWaterParams -> ulWAveMapShiftCHP	= pExp -> ulWAveMapShiftCHP;
	pst_GlobalsWaterParams -> ulSymetryActive	= 0&pExp -> ulSymetryActive;
	
	pst_GlobalsWaterParams -> BottomCol			= pExp -> BottomCol;
	pst_GlobalsWaterParams -> WaterCol			= pExp -> WaterCol;
	pst_GlobalsWaterParams -> SkyCol			= pExp -> SkyCol;
#define WTR_GeneralFactor 1.0f
	
	pst_GlobalsWaterParams -> ZFactor			= pExp -> ZFactor 		* WTR_GeneralFactor;
	pst_GlobalsWaterParams -> fChoppyFactor		= pExp -> fChoppyFactor * WTR_GeneralFactor;
	pst_GlobalsWaterParams -> HCoef				= pExp -> HCoef;
	pst_GlobalsWaterParams -> SpeedFactor		= pExp -> SpeedFactor 	* WTR_GeneralFactor;
	pst_GlobalsWaterParams -> GlobalWaterZ		= pExp -> GlobalWaterZ ;

	pst_GlobalsWaterParams -> CloudShadingEnabled= pExp -> CloudShadingEnabled;



	pst_GlobalsWaterParams -> WTR . BorderSizePo2 = pst_GlobalsWaterParams->ulWAveMapShift;
	pst_GlobalsWaterParams -> WTR . DXDYBorderSizePo2 = pst_GlobalsWaterParams->ulWAveMapShiftCHP;

	if ((pst_GlobalsWaterParams -> WTR . P_Factor != pExp->P_Factor) ||
		(pst_GlobalsWaterParams -> WTR .WindBalance!= pExp->WindBalance))
			ulMustReinit  = 1;

	pst_GlobalsWaterParams -> WTR . P_Factor    = pExp->P_Factor;
	pst_GlobalsWaterParams -> WTR . WindBalance = pExp->WindBalance;

	pst_GlobalsWaterParams -> ulTextureRef  = pExp -> ulTextureRef ;

	pst_GlobalsWaterParams -> SclaeUVFactor = pExp -> TextorigFactor ;
	
/*#ifdef _GAMECUBE		
	{
		MATH_tdst_Vector stBottomCol , stWaterCol , stSkyCol;

		MATH_InitVector(&stSkyCol , 0.3f , 0.4f , 0.5f); 
		MATH_InitVector(&stWaterCol , 0.3f , 0.4f , 0.5f); 
		MATH_ScaleVector(&stBottomCol , &stWaterCol , 0.4f);
		MATH_ScaleVector(&stSkyCol , &stSkyCol , 1.5f);
		
		pst_GlobalsWaterParams->SkyCol = WTR_Vector_2_Color(&stSkyCol);
		pst_GlobalsWaterParams->BottomCol = WTR_Vector_2_Color(&stBottomCol);
		pst_GlobalsWaterParams->WaterCol = WTR_Vector_2_Color(&stWaterCol);
	}
#endif*/

	if (ulMustRealoc)
	{
		WTR_FreeSubStruct(pst_GlobalsWaterParams);
		WTR_AllocSubStruct(pst_GlobalsWaterParams);
		ulMustReinit = 1;
	}
	if (ulMustReinit)
	{
		WATERFFT_Init_Spectre(&pst_GlobalsWaterParams->WTR);
		WATERFFT_Turn_Spectre(&pst_GlobalsWaterParams->WTR , pst_GlobalsWaterParams->pSpaceMap , pst_GlobalsWaterParams->pSpaceMapDX , pst_GlobalsWaterParams->pSpaceMapDY , pst_GlobalsWaterParams->fftime);
		WTR_Concate(pst_GlobalsWaterParams);
	}
	if (ulMustComputeFresnel)
		WTR_ComputeFresnel(pst_GlobalsWaterParams);//*/

#ifdef ACTIVE_EDITORS
	pst_GlobalsWaterParams->pBackRef = pExp;
#endif

//CARLONE

#ifdef _XBOX
	{
		GlobalWaterZ = pst_GlobalsWaterParams -> GlobalWaterZ;
	}
#endif


}

static void WTR_MipMap_Texel(GEO_VertexForZmap	*pDST , GEO_VertexForZmap	*pA , GEO_VertexForZmap	*pB)
{
#ifdef WTR_PSX2_ASSEMBLY
	asm __volatile__ ("
		lqc2 	$vf10 , 0x00(pA)
		lqc2 	$vf11 , 0x00(pB)
		lqc2 	$vf12 , 0x10(pA)
		lqc2 	$vf13 , 0x10(pB)
		lqc2 	$vf14 , 0x20(pA)
		lqc2 	$vf15 , 0x20(pB)
		lqc2 	$vf16 , 0x30(pA)
		lqc2 	$vf17 , 0x30(pB)
		PSX2_ASM_PREFETCH(0x40(pB))
		vadd	$vf10 , $vf10 , $vf11
		vadd	$vf14 , $vf14 , $vf15
		vadd	$vf12 , $vf12 , $vf13
		vadd	$vf16 , $vf16 , $vf17
		vadd	$vf10 , $vf10 , $vf12
		vadd	$vf14 , $vf14 , $vf16
		vmul	$vf10 , $vf10 , $vf20
		vmul	$vf14 , $vf14 , $vf20
		sqc2	$vf10 , 0x00(pDST)
		sqc2	$vf14 , 0x10(pDST)
	");
#elif defined(TEX_MESH_USE_SSE)
	GEO_VertexForZmap OneQuarter = _mm_setr_ps(0.25f, 0.25f, 0.25f, 0);
	pDST[0] = _mm_mul_ps(_mm_add_ps(pA[0], _mm_add_ps(pB[0], _mm_add_ps(pA[1], pB[1]))), OneQuarter);
	pDST[1] = _mm_mul_ps(_mm_add_ps(pA[2], _mm_add_ps(pB[2], _mm_add_ps(pA[3], pB[3]))), OneQuarter);
#else
	register GEO_VertexForZmap	*pC ,*pD;
	ULONG Counter;
	Counter = 2;
	pC = pA + 1;
	pD = pB + 1;
	while(Counter--)
	{
		pDST->x = (pA->x + pB->x + pC->x + pD->x) * 0.25f ;
		pDST->y = (pA->y + pB->y + pC->y + pD->y) * 0.25f ;
		pDST->z = (pA->z + pB->z + pC->z + pD->z) * 0.25f ;
		pDST++;
		pA+=2;
		pB+=2;
		pC+=2;
		pD+=2;
	}
#endif
}

void WTR_Compute_MipMap(GEO_VertexForZmap *pCB , GEO_VertexForZmap *pCM , u32 MapSize)
{
	GEO_VertexForZmap	*pDST ,*pSRC , *pDSTLastLine , *pDSTLast ;
#ifdef WTR_PSX2_ASSEMBLY
	float fCoef;
	fCoef = 0.25f;
	WTR_Preload_MipMap_Texel_Param(&fCoef);
#endif

	pDST = pCM;
	pSRC = pCB;
	pDSTLast = pDST + (MapSize * MapSize) + (MapSize);
	while (pDST < pDSTLast)
	{
		pDSTLastLine = pDST + MapSize;
		while (pDST < pDSTLastLine)
		{
			WTR_MipMap_Texel(pDST , pSRC , (pSRC + 1 + (MapSize << 1)) );
			pDST+=2;
			pSRC += 4;
		}
		*pDST = *(pDST - MapSize);
		pDST++;//*/
		pSRC += 2 + (MapSize << 1);
	}
	pDSTLast += MapSize + 1;
	pDSTLastLine = pCM;
	while (pDST < pDSTLast)	*(pDST++) = *(pDSTLastLine++);
}
_inline_ void WTR_Extend_Texel(register GEO_VertexForZmap *pDST , register GEO_VertexForZmap *pDSTPL , register GEO_VertexForZmap *pSRC, register GEO_VertexForZmap *pSRCPL)
{
#ifdef WTR_PSX2_ASSEMBLY
	asm __volatile__ ("
		lqc2 	$vf10 , 0(pSRC)
		lqc2 	$vf12 , 0x10(pSRC)
		lqc2 	$vf11 , 0(pSRCPL)
		lqc2 	$vf13 , 0x10(pSRCPL)
		vadd	$vf15 , $vf10 , $vf12 	// pSRC + (pSRC+1)
		vadd	$vf16 , $vf10 , $vf11 	// pSRC + (pSRCPL)
		vadd	$vf17 , $vf13 , $vf11 	// (pSRCPL) + (pSRCPL + 1)
		sqc2	$vf10 , 0(pDST)
		vmul	$vf15 , $vf15 , $vf20
		vmul	$vf16 , $vf16 , $vf20
		vmul	$vf17 , $vf17 , $vf20
		sqc2	$vf15 , 0x10(pDST)		// pSRC + (pSRC+1)
		vadd	$vf17 , $vf17 , $vf15
		sqc2	$vf16 , 0x0(pDSTPL)		// pSRC + (pSRCPL)
		vmul	$vf17 , $vf17 , $vf20
		sqc2	$vf17 , 0x10(pDSTPL)	// (pSRC + 1) + (pSRCPL + 1)
	");
#elif defined(TEX_MESH_USE_SSE)
	__m128 Half = _mm_setr_ps(0.5f, 0.5f, 0.5f, 0), temp1, temp2;
	*pDST = *pSRC;
	pDST[1]   = temp1 = _mm_mul_ps(_mm_add_ps(pSRC[0], pSRC[1]), Half);
	pDSTPL[0] = _mm_mul_ps(_mm_add_ps(pSRC[0], pSRCPL[0]), Half);
	temp2 = _mm_mul_ps(_mm_add_ps(pSRCPL[0], pSRCPL[1]), Half);
	pDSTPL[1] = _mm_mul_ps(_mm_add_ps(temp1, temp2), Half);
#else
	*pDST = *pSRC;
	MATH_BlendVector(pDST + 1 , pSRC , pSRC + 1 , 0.5f);
	MATH_BlendVector(pDSTPL , pSRC , pSRCPL , 0.5f);
	MATH_BlendVector(pDSTPL + 1, pSRCPL , pSRCPL + 1 , 0.5f);
	MATH_BlendVector(pDSTPL + 1, pDSTPL + 1 , pDST + 1 , 0.5f);
#endif	
}
void WTR_BackExtendDXDY(GEO_VertexForZmap *pDSTMap , GEO_VertexForZmap *pSRCMap , ULONG SrcSize)
{
	GEO_VertexForZmap *pSRC , *pDST , *pSRCLast ,*pSRCLastLine;
	register ULONG PitchDST,PitchSRC;
#ifdef WTR_PSX2_ASSEMBLY
	float fCoef;
	fCoef = 0.5f;
	WTR_Preload_MipMap_Texel_Param(&fCoef);
#endif
	PitchDST = 1 + (SrcSize << 1);
	PitchSRC = 1 + SrcSize;
	pSRC = pSRCMap;
	pDST = pDSTMap;
	pSRCLast = pSRC + (SrcSize * SrcSize) + (SrcSize);
	/* Copy lines */
	while (pSRC < pSRCLast)
	{
		pSRCLastLine = pSRC + SrcSize;
		while (pSRC < pSRCLastLine)
		{
			WTR_Extend_Texel(pDST , pDST + PitchDST , pSRC, pSRC + PitchSRC);
			pDST+=2;
			pSRC++;
		}
		pSRC ++;
		*(pDST) = *(pDST - (SrcSize << 1));
		pDST ++;
		pDST += SrcSize << 1;
		*(pDST) = *(pDST - (SrcSize << 1));
		pDST ++;
	}//*/
	pSRC = pDSTMap;
	pSRCLast = pSRC + (SrcSize << 1) + 1;
	while (pSRC < pSRCLast)	*(pDST++) = *(pSRC++);
}
void WTR_Concate(WTR_Generator_Struct *pst_Params)
{
	FFT_Complex *pDX , *pDY , *pDZ ;
	GEO_VertexForZmap *pDST , *pDSTLastLine , *pDSTLast ;
	ULONG SizeOfMap;
	pDST = pst_Params->pConcatenedMap;
	pDSTLast = pDST + (1 << (pst_Params->ulWAveMapShift << 1)) + (1 << pst_Params->ulWAveMapShift);
	pDX = pst_Params->pSpaceMapDX;
	pDY = pst_Params->pSpaceMapDY;
	pDZ = pst_Params->pSpaceMap;
	if (pst_Params->ulWAveMapShift == pst_Params->ulWAveMapShiftCHP)
	{
		while (pDST < pDSTLast)
		{
			pDSTLastLine = pDST + (1 << pst_Params->ulWAveMapShift);
			while (pDST < pDSTLastLine)
			{
#ifdef TEX_MESH_USE_SSE
				*pDST = _mm_setr_ps(pDX++->re, pDY++->re, pDZ++->re, 0);
				//TODO: try this: _mm_stream_ps((float*)pDST, _mm_setr_ps(pDX++->re, pDY++->re, pDZ++->re, 0));
#else
				*((u32*)&pDST->x) = *((u32*)&(pDX++)->re) ;
				*((u32*)&pDST->y) = *((u32*)&(pDY++)->re) ;
				*((u32*)&pDST->z) = *((u32*)&(pDZ++)->re) ;
#endif
				pDST++;
			}
			*pDST = *(pDST - (1 << pst_Params->ulWAveMapShift));
			pDST++;//*/
		}
	} else
	{
		while (pDST < pDSTLast)
		{
#ifdef WTR_PSX2_ASSEMBLY
			register ULONG TMP0 , TMP1 , TMP2 , TMP3;
			register ULONG TMP0u , TMP1u , TMP2u , TMP3u;
#endif			
			pDSTLastLine = pDST + (1 << pst_Params->ulWAveMapShift);
			while (pDST < pDSTLastLine)
			{
#ifdef WTR_PSX2_ASSEMBLY
				{	
					asm __volatile__ ("
						lq 	TMP0 , 0x00(pDZ)
						lq 	TMP1 , 0x10(pDZ)
						lq 	TMP2 , 0x20(pDZ)
						lq 	TMP3 , 0x30(pDZ)
						PSX2_ASM_PREFETCH(0x40(pDZ))
						pcpyud	TMP0u, TMP0, $zero 
						pcpyld	TMP0 , TMP0, $zero 
						pcpyud	TMP1u, TMP1, $zero 
						pcpyld	TMP1 , TMP1, $zero 
						pcpyud	TMP2u, TMP2, $zero 
						pcpyld	TMP2 , TMP2, $zero 
						pcpyud	TMP3u, TMP3, $zero 
						pcpyld	TMP3 , TMP3, $zero 
						sq		TMP0 , 0x00(pDST)
						sq		TMP0u, 0x10(pDST)
						sq		TMP1 , 0x20(pDST)
						sq		TMP1u, 0x30(pDST)
						sq		TMP2 , 0x40(pDST)
						sq		TMP2u, 0x50(pDST)
						sq		TMP3 , 0x60(pDST)
						sq		TMP3u, 0x70(pDST)
					");
				}
				pDST+=8;
				pDZ+=8;
#elif defined(TEX_MESH_USE_SSE)
				pDST[0].m128_f32[2] = pDZ[0].re;
				pDST[1].m128_f32[2] = pDZ[1].re;
				pDST[2].m128_f32[2] = pDZ[2].re;
				pDST[3].m128_f32[2] = pDZ[3].re;
				pDST+=4;
				pDZ+=4;
#else
				*((u32 *)&(pDST + 0)->z) = *((u32*)&(pDZ+0)->re) ;
				*((u32 *)&(pDST + 1)->z) = *((u32*)&(pDZ+1)->re) ;
				*((u32 *)&(pDST + 2)->z) = *((u32*)&(pDZ+2)->re) ;
				*((u32 *)&(pDST + 3)->z) = *((u32*)&(pDZ+3)->re) ;
				pDST+=4;
				pDZ+=4;
#endif				
			}
			*pDST = *(pDST - (1 << pst_Params->ulWAveMapShift));
			pDST++;//*/
		}
	} 
	
	pDSTLast += (1 << pst_Params->ulWAveMapShift) + 1;
	pDSTLastLine = pst_Params->pConcatenedMap;
	while (pDST < pDSTLast)	*(pDST++) = *(pDSTLastLine++);
	
	SizeOfMap = (1 << pst_Params->ulWAveMapShift);
	pDSTLast = pst_Params->pConcatenedMap;
	pDST = pDSTLast + (SizeOfMap * SizeOfMap) + 2 * SizeOfMap + 1;
	SizeOfMap >>= 1;
	while (SizeOfMap)
	{
		WTR_Compute_MipMap(pDSTLast , pDST , SizeOfMap);
		if ((SizeOfMap == (1 << pst_Params->ulWAveMapShiftCHP)) && pst_Params->ulWAveMapShiftCHP)
		{
			/* Choppy Map size difference is solved here */
			register GEO_VertexForZmap	*pDSTLocal , *pDSTLastLocal , *pDSTLastLineLocal;
			u32 LocalSizeOfMap;
			pDSTLocal = pDST;
			pDSTLastLocal = pDST + (1 << (pst_Params->ulWAveMapShiftCHP << 1)) + (1 << pst_Params->ulWAveMapShiftCHP);
			while (pDSTLocal < pDSTLastLocal)
			{
				pDSTLastLineLocal = pDSTLocal + (1 << pst_Params->ulWAveMapShiftCHP);
#ifdef WTR_PSX2_ASSEMBLY
				while (pDSTLocal < pDSTLastLineLocal)
				{
					register ULONG TMP0 , TMP1 , TMP2 , TMP3;
					asm __volatile__ ("
						lq	 TMP0 , 0x00(pDX) 		// ?? | X2 | ?? | X1
						lq	 TMP2 , 0x10(pDX) 		// ?? | X2 | ?? | X1
						lq	 TMP1 , 0x00(pDY) 		// ?? | Y2 | ?? | Y1
						lq	 TMP3 , 0x10(pDY) 		// ?? | Y2 | ?? | Y1
						PSX2_ASM_PREFETCH(0x40(pDX))
						ppacw TMP0 , TMP1 , TMP0 	// Y2 | Y1 | X2 | X1
						ppacw TMP2 , TMP3 , TMP2 	// Y2 | Y1 | X2 | X1
						pexcw TMP0 , TMP0       	// Y2 | X2 | Y1 | X1
						pexcw TMP2 , TMP2       	// Y2 | X2 | Y1 | X1
						sd TMP0 , 0x00(pDSTLocal)
						sd TMP2 , 0x20(pDSTLocal)
						pcpyud	TMP0 , TMP0 , TMP0
						pcpyud	TMP2 , TMP2 , TMP2
						sd TMP0 , 0x10(pDSTLocal)
						sd TMP2 , 0x30(pDSTLocal)
					");
					pDX += 4;
					pDY += 4;
					pDSTLocal+=4;
				}
#elif defined(TEX_MESH_USE_SSE)
				while (pDSTLocal < pDSTLastLineLocal)
				{
					pDSTLocal->m128_f32[0] = pDX++->re;
					pDSTLocal->m128_f32[1] = pDY++->re;
					pDSTLocal++;
				}
#else
				while (pDSTLocal < pDSTLastLineLocal)
				{
					*((u32*)&pDSTLocal->x) = *((u32*)&(pDX++)->re) ;
					*((u32*)&pDSTLocal->y) = *((u32*)&(pDY++)->re) ;
					pDSTLocal++;
				}
#endif				
				*pDSTLocal = *(pDSTLocal - (1 << pst_Params->ulWAveMapShiftCHP));
				pDSTLocal++;//*/
			}
			pDSTLastLocal += (1 << pst_Params->ulWAveMapShiftCHP) + 1;
			pDSTLastLineLocal = pDST;
			while (pDSTLocal < pDSTLastLocal)	*(pDSTLocal++) = *(pDSTLastLineLocal++);
/* --------------------------------------------------------------------------------------------------- */
/* ----------------------------------------- BACK EXTEND DX DY --------------------------------------- */
/* --------------------------------------------------------------------------------------------------- */
			pDSTLastLocal = pDSTLast;
			pDSTLocal = pDST;
			LocalSizeOfMap = SizeOfMap;
			while (pDSTLastLocal >= pst_Params->pConcatenedMap)
			{
				WTR_BackExtendDXDY(pDSTLastLocal ,  pDSTLocal , LocalSizeOfMap);
				pDSTLocal = pDSTLastLocal;
				LocalSizeOfMap <<= 1;
				pDSTLastLocal -= 4 * (LocalSizeOfMap * LocalSizeOfMap) + 4 * LocalSizeOfMap + 1;
			}
		}
		pDSTLast = pDST;
		pDST = pDSTLast + (SizeOfMap * SizeOfMap) + 2 * SizeOfMap + 1;
		SizeOfMap >>= 1;
	}
}
extern ULONG LIGHT_ul_Interpol2Colors(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef);
void WTR_DirectComputeColors_CloudShading(register GEO_Vertex *pSrc ,register ULONG *pColorDest, register unsigned int SizeAndPitch , ULONG UpCol , ULONG DownCol , 	float fMulL,float GZ)
{
	GEO_Vertex *pSrcLast;
	pSrcLast = pSrc + (SizeAndPitch - 1);
	while (pSrc < pSrcLast)
	{
		*pColorDest = LIGHT_ul_Interpol2Colors(UpCol , DownCol, ((pSrc->z - GZ) * fMulL) + 0.5f);
		pSrc++;
		pColorDest++;
	}
	*(pColorDest) = *(pColorDest - 1);
}

void WTR_DirectComputeColors_And_UV(register GEO_Vertex *pSrc,
									register  GEO_Vertex *pCameraPos,
									register GEO_Vertex *pNRM,
									register GEO_Vertex *pNRMSPEC,
									register ULONG *pColorDest,
									register unsigned int SizeAndPitch,
									register  ULONG *pColorTable,
									register float Factorizer )
{
#if defined(TEX_MESH_USE_SSE)

	GEO_Vertex *pSrcLast;
	__m128 stpCPL, stpVSym, stpCP0, stpV, stpN, kFactorizer, CameraPos;
	__m128 SqrN, SqrV, VNAlf, OoSqrNV, SqrAlf, kMinus2, kMinus255, kZero;
	int Index;

	kFactorizer = _mm_setr_ps(Factorizer, Factorizer, 1.0f, 0.0f);
	kMinus2 = _mm_set_ss(-2.0f);
	kMinus255 = _mm_set_ss(-255.0f);
	kZero = _mm_setzero_ps();
	CameraPos = _mm_loadu_ps(&pCameraPos->x);
	pSrcLast = pSrc + (SizeAndPitch - 1);

	while (pSrc < pSrcLast)
	{
		__m128 src = _mm_loadu_ps(&pSrc->x);
		stpV   = _mm_sub_ps(src, CameraPos);
		stpCPL = _mm_sub_ps(_mm_loadu_ps(&pSrc[1].x), src);
		stpCP0 = _mm_sub_ps(_mm_loadu_ps(&pSrc[SizeAndPitch].x), src);
		stpCP0 = _mm_mul_ps(stpCP0, kFactorizer);
		stpN   = ubi_mm_crossprod_3d_ps(stpCP0, stpCPL);

		// Foam should be detected here, with negative Z
		SqrN = ubi_mm_norm2_3d_ss(stpN);
		SqrV = ubi_mm_norm2_3d_ss(stpV);
		VNAlf = ubi_mm_dotprod_3d_ss(stpN , stpV);

		/////////////////////////////////////////////////////////////////////////
		//CARLONE...THIS IS ONE ANTIBUG...CHECK WITH PHILIPPE
		if(_mm_comige_ss(VNAlf, kZero))
			VNAlf = kZero;
		/////////////////////////////////////////////////////////////////////////

		OoSqrNV = _mm_rsqrt_ss(_mm_mul_ss(_mm_mul_ss(SqrN, SqrN), SqrV)); // 1 / sqrt (N^4 * V^2) == 1 / (N^2 * V)
		SqrAlf = _mm_mul_ss(VNAlf, OoSqrNV);
		SqrAlf = _mm_mul_ss(_mm_mul_ss(SqrAlf, SqrAlf), SqrN);

		stpVSym = _mm_add_ps(
			_mm_mul_ps(stpV, ubi_mm_ss_to_ps(_mm_mul_ss(SqrN, OoSqrNV))),
			_mm_mul_ps(stpN, ubi_mm_ss_to_ps(_mm_mul_ss(kMinus2, _mm_mul_ss(VNAlf, OoSqrNV)))));

		Index = (256 - _mm_cvtss_si32(_mm_mul_ss(kMinus255, SqrAlf))) & 511;
		*pColorDest = pColorTable[Index];
		ubi_mm_store_3d_ps(pNRMSPEC, stpVSym);
		pNRMSPEC ++;

		ubi_mm_store_3d_ps(pNRM, ubi_mm_normalize_3d_ps(stpN));
		pNRM ++;

		pSrc++;
		pColorDest++;
	}
	*(pColorDest) = *(pColorDest - 1);

#else

	GEO_Vertex stpCPL , stpVSym , stpCP0, stpV , stpN , *pSrcLast;
	register float SqrN  , SqrV,VNAlf,OoSqrNV,SqrAlf;
	u32 Index;
	pSrcLast = pSrc + (SizeAndPitch - 1);
	while (pSrc < pSrcLast)
	{
		MATH_SubVector2(&stpV , pSrc , pCameraPos);
		MATH_SubVector2(&stpCPL , (pSrc + 1 )			 , pSrc);
		MATH_SubVector2(&stpCP0 , (pSrc + SizeAndPitch) , pSrc);
		stpCP0.x *= Factorizer;
		stpCP0.y *= Factorizer;
		MATH_CrossProduct2(&stpN , &stpCP0 , &stpCPL );
		// Foam should be detected here, with negative Z
		SqrN = stpN.x * stpN.x + stpN.y * stpN.y + stpN.z * stpN.z;
		SqrV = stpV.x * stpV.x + stpV.y * stpV.y + stpV.z * stpV.z;
		VNAlf = MATH_f_DotProduct(&stpN , &stpV);

		/////////////////////////////////////////////////////////////////////////
		//CARLONE...THIS IS ONE ANTIBUG...CHECK WITH PHILIPPE
		if(VNAlf>0.0f)
			VNAlf=0.0f;
		/////////////////////////////////////////////////////////////////////////



		OoSqrNV = fOptInvSqrt(SqrN * SqrN * SqrV); // 1 / sqrt (N^4 * V^2) == 1 / (N^2 * V)
		SqrAlf = VNAlf * OoSqrNV;
		SqrAlf = SqrAlf * SqrAlf * SqrN;
		MATH_ScaleVector(&stpVSym , &stpV , SqrN * OoSqrNV);
		MATH_AddScaleVector(&stpVSym , &stpVSym , &stpN , -2.0f * VNAlf * OoSqrNV );
		Index = (u32)(256.0f - 255.0f * SqrAlf * fSign(VNAlf));
		Index &= 511;
		*pColorDest = pColorTable[Index];
		*pNRMSPEC = stpVSym;
		pNRMSPEC ++;

		MATH_NormalizeVector(pNRM , &stpN);
		pNRM ++;
		
		pSrc++;
		pColorDest++;
	}
	*(pColorDest) = *(pColorDest - 1);
#endif
}


void WTR_DirectComputeColors(register GEO_Vertex *pSrc ,register  GEO_Vertex *pCameraPos , register ULONG *pDST, register unsigned int SizeAndPitch ,register  ULONG *pColorTable )
{
#ifdef WTR_PSX2_ASSEMBLY
	GEO_Vertex  *pSrcLast;
	register GEO_Vertex *pSrcPL;
	GEO_Vertex stCampos ONLY_PSX2_ALIGNED(16);
	register ULONG *pColorDest;

	register u32 Index;
	pColorDest = pDST;
	stCampos = *pCameraPos;
	pCameraPos = &stCampos;
	pSrcLast = pSrc + (SizeAndPitch - 1);
	pSrcPL = pSrc +  SizeAndPitch;
	asm __volatile__ ("
		vaddw.x $vf01 , $vf00 , $vf00w 
		lqc2	$vf09 , 0(pCameraPos)
		lqc2	$vf10 , 0x00(pSrc)
		lqc2	$vf11 , 0x10(pSrc)
		lqc2	$vf12 , 0x00(pSrcPL)
		vsub	$vf13 , $vf10 , $vf09
		vsub	$vf11 , $vf11 , $vf10
		vsub	$vf12 , $vf12 , $vf10
		vopmula.xyz	$acc  ,	$vf11 , $vf12
		vopmsub.xyz	$vf14 ,	$vf12 , $vf11
		vmul	$vf15 , $vf13 , $vf14	// dOT PROD
		vmul	$vf14 , $vf14 , $vf14	// 
		vmul	$vf13 , $vf13 , $vf13
		vaddy.x	$vf15 , $vf15 , $vf15y
		vaddy.x	$vf14 , $vf14 , $vf14y
		vaddy.x	$vf13 , $vf13 , $vf13y
		vaddz.x	$vf15 , $vf15 , $vf15z
		vaddz.x	$vf14 , $vf14 , $vf14z
		vaddz.x	$vf13 , $vf13 , $vf13z
		vmulx.x	$vf15 , $vf15 , $vf20
		vmul.x	$vf13 , $vf13 , $vf14
		vrsqrt	Q , 	$vf15x, $vf13x
		");
	
	while (pSrc < pSrcLast)
	{
		register ULONG TMP0;
		pSrc++;
		pSrcPL++;
		asm __volatile__ ("
			lqc2		$vf10 , 0x00(pSrc)
			lqc2		$vf11 , 0x10(pSrc)
			lqc2		$vf12 , 0x00(pSrcPL)
			vsub		$vf13 , $vf10 , $vf09
			vsub		$vf11 , $vf11 , $vf10
			vsub		$vf12 , $vf12 , $vf10
			vwaitq
			vaddq.x		$vf16 , $vf01 , Q
			vopmula.xyz	$acc  ,	$vf11 , $vf12
			vftoi12		$vf16 , $vf16
			vopmsub.xyz	$vf14 ,	$vf12 , $vf11
			qmfc2		TMP0 , $vf16
			vmul		$vf15 , $vf13 , $vf14	// dOT PROD
			srl			TMP0 , TMP0 , 4
			vmul		$vf14 , $vf14 , $vf14	// 
			andi    	TMP0 , TMP0 , 0x1ff			
			vmul		$vf13 , $vf13 , $vf13
			sll			TMP0 , TMP0 , 2
			vaddy.x		$vf15 , $vf15 , $vf15y
			add			TMP0 , TMP0 , pColorTable
			vaddy.x		$vf14 , $vf14 , $vf14y
			vaddy.x		$vf13 , $vf13 , $vf13y
			lw			TMP0 , 0(TMP0)
			vaddz.x		$vf15 , $vf15 , $vf15z
			vaddz.x		$vf14 , $vf14 , $vf14z
			vaddz.x		$vf13 , $vf13 , $vf13z
			sw			TMP0 , 0(pColorDest)
			vmul.x		$vf13 , $vf13 , $vf14
			vrsqrt		Q , 	$vf15x, $vf13x
			
		");
		pColorDest++;
	}
	*(pColorDest) = *(pColorDest - 1);
	*(pDST) = *(pDST + 1);

#elif defined(TEX_MESH_USE_SSE)

	GEO_Vertex* pSrcLast;
	__m128 stpCPL, stpCP0, stpCPC, stpCPR;
	__m128 fMulL = _mm_set_ss(-255.0f);
	int Index;
	pSrcLast = pSrc + (SizeAndPitch - 1);
	while (pSrc < pSrcLast)
	{
		__m128 src = _mm_loadu_ps(&pSrc->x);
		__m128 Factor;
		stpCPC = _mm_sub_ps(src, _mm_loadu_ps(&pCameraPos->x));
		stpCPL = _mm_sub_ps(_mm_loadu_ps(&pSrc[1].x), src);
		stpCP0 = _mm_sub_ps(_mm_loadu_ps(&pSrc[SizeAndPitch].x), src);
		stpCPR = ubi_mm_crossprod_3d_ps(stpCP0, stpCPL);

		// Foam should be detected here, with negative Z
		Factor = _mm_mul_ss(
					_mm_rsqrt_ss(
						_mm_mul_ss(ubi_mm_norm2_3d_ss(stpCPR), ubi_mm_norm2_3d_ss(stpCPC))),
					ubi_mm_dotprod_3d_ss(stpCPR, stpCPC)
				);

		Index = (256 + _mm_cvtss_si32(_mm_mul_ss(fMulL, Factor))) & 511;

#ifdef _XBOX
		//Yo Convert color
		*pDST = Gx8_M_ConvertColor(pColorTable[Index]);
#else
		*pDST = pColorTable[Index];
#endif
		pSrc++;
		pDST++;
	}
	*(pDST) = *(pDST - 1);

#else

	GEO_Vertex stpCPL , stpCP0, stpCPC , stpCPR , *pSrcLast;
	float Factor , fMulL,fAddL;
	u32 Index;
	fMulL = 255.0f;
	fAddL = 256.0f;
	pSrcLast = pSrc + (SizeAndPitch - 1);
	while (pSrc < pSrcLast)
	{
		MATH_SubVector(&stpCPC , pSrc , pCameraPos);
		MATH_SubVector(&stpCPL , (pSrc + 1 )			 , pSrc);
		MATH_SubVector(&stpCP0 , (pSrc + SizeAndPitch) , pSrc);
		MATH_CrossProduct2(&stpCPR , &stpCP0 , &stpCPL );
		// Foam should be detected here, with negative Z
		Factor = stpCPR.x * stpCPR.x + stpCPR.y * stpCPR.y + stpCPR.z * stpCPR.z;
/*		if (fOptInvSqrt(Factor) * stpCPR.z < 0.90f)
		{
			*pDST = 0xffffff;
		} else*/
		{
			Factor *= stpCPC.x * stpCPC.x + stpCPC.y * stpCPC.y + stpCPC.z * stpCPC.z;
			Factor = fOptInvSqrt(Factor);
			Factor = MATH_f_DotProduct(&stpCPR , &stpCPC) * Factor;
			Index = (u32)(fAddL - fMulL * Factor);
			Index &= 511;
			*pDST = pColorTable[Index];
		}
		pSrc++;
		pDST++;
	}
	*(pDST) = *(pDST - 1);
#endif
}

void WTR_PreComputeNormaleMap(GEO_Vertex *pSrc , GEO_Vertex *pDest , unsigned int SizeAndPitch )
{
	GEO_Vertex stpCPL , stpCP0 , stpCPR , *pSrcLast;
	pSrcLast = pSrc + (SizeAndPitch - 1);
	while (pSrc < pSrcLast)
	{
		MATH_SubVector(&stpCPL , (pSrc + 1 )			 , pSrc);
		MATH_SubVector(&stpCP0 , (pSrc + SizeAndPitch) , pSrc);
		MATH_CrossProduct(&stpCPR , &stpCP0 , &stpCPL );
		MATH_ScaleVector(pDest , &stpCPR , fOptInvSqrt(stpCPR.x * stpCPR.x + stpCPR.y * stpCPR.y + stpCPR.z * stpCPR.z) );
		pSrc++;
		pDest++;
	}
	*(pDest) = *(pDest - 1);
}

/*
	0 - 1
	3 - 2
*/

float WTR_DistToPlane(MATH_tdst_Vector *pPoint , MATH_tdst_Vector *pPlaneNormale , float fPlaneDist)
{
	return fPlaneDist - MATH_f_DotProduct(pPoint , pPlaneNormale);
}

u32 WTR_Clip(MATH_tdst_Vector *pSrc , MATH_tdst_Vector *pDst , MATH_tdst_Vector *pPlaneNormale , float fPlaneDist , u32 lNbPoints)
{
	MATH_tdst_Vector *pSrcLast,*pSrcM1;
	u32 ulState , RetValue;
	float fLastDistoPlane,fDistoPlane;
	pSrcLast = pSrc + lNbPoints;
	pSrcM1 = pSrcLast - 1;
	RetValue = 0;
	fLastDistoPlane = WTR_DistToPlane(pSrcM1 , pPlaneNormale , fPlaneDist);
	if (fLastDistoPlane < 0.0f)
		ulState = 1;
	else 
		ulState = 0;

	while (pSrc < pSrcLast)
	{
		ulState <<= 1;
		fDistoPlane = WTR_DistToPlane(pSrc , pPlaneNormale , fPlaneDist);
		if (fDistoPlane < 0.0f)
			ulState |= 1;
		ulState &= 3;
			
		switch (ulState)
		{
			case 2: // Come in
				MATH_BlendVector(pDst++ , pSrcM1 , pSrc , -fLastDistoPlane / (fDistoPlane - fLastDistoPlane));
				RetValue++;
			case 0: // Stay in
				*(pDst++) = *pSrc;
				RetValue++;
				break;
			case 1: // Go out
				MATH_BlendVector(pDst++ , pSrcM1 , pSrc , -fLastDistoPlane / (fDistoPlane - fLastDistoPlane));
				RetValue++;
				break;
			case 3: // Stay Out
				break;
		}
		fLastDistoPlane = fDistoPlane;
		pSrcM1 = pSrc++;
	}
	return RetValue;
}

/*
	0 - 1
	3 - 2
*/
u32 WTR_Compute4Points(GEO_Vertex *p4Dir , MATH_tdst_Vector *pTCamYVEC , MATH_tdst_Vector *pCamPos , MATH_tdst_Vector *pCamDir , float WaterZ , float finfinite , float Focale , float ScreenRatio , float Zlimit , float Hlimit)
{
	MATH_tdst_Vector st44Points_A[8];
	MATH_tdst_Vector st44Points_B[8];
	MATH_tdst_Vector st4PlaneNorm[5];
	float			 st4Distances[5];
	u32				 lNbPoints;
	MATH_tdst_Vector stSpecialDir,LocalZ,LocalY,LocalPos;
	MATH_tdst_Vector VX,VY,VZ;
	
	MATH_InitVector(&LocalZ , 0.0f , 0.0f , 1.0f);
	MATH_CrossProduct(&VX , &LocalZ , pCamDir );
	MATH_NormalizeEqualVector(&VX);
	MATH_NormalizeEqualVector(pCamDir);
	MATH_CrossProduct(&VY , pCamDir , &VX);
	
	MATH_ScaleVector(&VZ , pCamDir , Focale);

	*pTCamYVEC = VY;
	MATH_ScaleVector(&VX , &VX , 1.05f);
	MATH_ScaleVector(&VY , &VY , 1.0f * ScreenRatio);
	
	MATH_SubVector(&p4Dir[1] , &VZ , &VX);
	MATH_SubVector(&p4Dir[2] , &VZ , &VX);
	MATH_ScaleVector(&VX , &VX , 1.04f);
	MATH_AddVector(&p4Dir[0] , &VZ , &VX);
	MATH_AddVector(&p4Dir[3] , &VZ , &VX);
	MATH_AddVector(&p4Dir[0] , &p4Dir[0] , &VY);
	MATH_AddVector(&p4Dir[1] , &p4Dir[1] , &VY);
	MATH_SubVector(&p4Dir[2] , &p4Dir[2] , &VY);
	MATH_SubVector(&p4Dir[3] , &p4Dir[3] , &VY);
	
	MATH_CrossProduct(&st4PlaneNorm[0] , &p4Dir[1] , &p4Dir[0]);
	MATH_CrossProduct(&st4PlaneNorm[1] , &p4Dir[2] , &p4Dir[1]);
	MATH_CrossProduct(&st4PlaneNorm[2] , &p4Dir[3] , &p4Dir[2]);
	MATH_CrossProduct(&st4PlaneNorm[3] , &p4Dir[0] , &p4Dir[3]);
	MATH_ScaleVector(&st4PlaneNorm[4] , pCamDir , -1.0f);

	MATH_NormalizeEqualVector(&st4PlaneNorm[0]);
	MATH_NormalizeEqualVector(&st4PlaneNorm[1]);
	MATH_NormalizeEqualVector(&st4PlaneNorm[2]);
	MATH_NormalizeEqualVector(&st4PlaneNorm[3]);
	MATH_NormalizeEqualVector(&st4PlaneNorm[4]);

	Zlimit *= 0.25f;
	Hlimit *= 0.25f;
	st4Distances[1] = MATH_f_DotProduct(&st4PlaneNorm[1] , pCamPos) + fAbs(MATH_f_DotProduct(&st4PlaneNorm[1] , &LocalZ) * Zlimit) + fSqrt(st4PlaneNorm[1].x * st4PlaneNorm[1].x + st4PlaneNorm[1].y * st4PlaneNorm[1].y) * Hlimit;
	st4Distances[3] = MATH_f_DotProduct(&st4PlaneNorm[3] , pCamPos) + fAbs(MATH_f_DotProduct(&st4PlaneNorm[3] , &LocalZ) * Zlimit) + fSqrt(st4PlaneNorm[3].x * st4PlaneNorm[3].x + st4PlaneNorm[3].y * st4PlaneNorm[3].y) * Hlimit;
	
	st4Distances[0] = MATH_f_DotProduct(&st4PlaneNorm[0] , pCamPos) + fAbs(MATH_f_DotProduct(&st4PlaneNorm[0] , &LocalZ) * Zlimit) + fSqrt(st4PlaneNorm[0].x * st4PlaneNorm[0].x + st4PlaneNorm[0].y * st4PlaneNorm[0].y) * Hlimit;
	st4Distances[2] = MATH_f_DotProduct(&st4PlaneNorm[2] , pCamPos) + fAbs(MATH_f_DotProduct(&st4PlaneNorm[2] , &LocalZ) * Zlimit) + fSqrt(st4PlaneNorm[2].x * st4PlaneNorm[2].x + st4PlaneNorm[2].y * st4PlaneNorm[2].y) * Hlimit;

	st4Distances[4] = MATH_f_DotProduct(&st4PlaneNorm[4] , pCamPos) + 2.0f;
	
	stSpecialDir = *pCamDir;
	stSpecialDir.z = 0.0f;
	MATH_NormalizeEqualVector(&stSpecialDir);
	
	MATH_CrossProduct(&LocalY , &stSpecialDir , &LocalZ);
	
	MATH_InitVector(&LocalPos , pCamPos->x, pCamPos->y , WaterZ);
	
	MATH_AddScaleVector(&LocalPos , &LocalPos , &stSpecialDir , -finfinite); // Clipping Near
	
	MATH_AddScaleVector(&st44Points_A[2] , &LocalPos , &LocalY , 8.0f  * finfinite);
	MATH_AddScaleVector(&st44Points_A[3] , &LocalPos , &LocalY , -8.0f  * finfinite);

	MATH_AddScaleVector(&LocalPos , &LocalPos , &stSpecialDir , finfinite * 2.0f);
	
	MATH_AddScaleVector(&st44Points_A[0] , &LocalPos , &LocalY , -8.0f  * finfinite);
	MATH_AddScaleVector(&st44Points_A[1] , &LocalPos , &LocalY , 8.0f  * finfinite);
	
	lNbPoints = WTR_Clip(st44Points_A , st44Points_B , &st4PlaneNorm[0] , st4Distances[0] , 4);
	lNbPoints = WTR_Clip(st44Points_B , st44Points_A , &st4PlaneNorm[1] , st4Distances[1] , lNbPoints);
	lNbPoints = WTR_Clip(st44Points_A , st44Points_B , &st4PlaneNorm[2] , st4Distances[2] , lNbPoints);
	lNbPoints = WTR_Clip(st44Points_B , st44Points_A , &st4PlaneNorm[3] , st4Distances[3] , lNbPoints);
	lNbPoints = WTR_Clip(st44Points_A , st44Points_B , &st4PlaneNorm[4] , st4Distances[4] , lNbPoints);
	//*/
	{
		u32 			 Turn;
		float 			 fAverage;
		Turn = 0;
		// Retreive Point 0
		st4Distances[0] = MATH_f_DotProduct(&st44Points_B[0] , &stSpecialDir);
		st4Distances[1] = MATH_f_DotProduct(&st44Points_B[1] , &stSpecialDir);
		st4Distances[2] = MATH_f_DotProduct(&st44Points_B[2] , &stSpecialDir);
		st4Distances[3] = MATH_f_DotProduct(&st44Points_B[3] , &stSpecialDir);
		fAverage = 0.25f * (st4Distances[0] + st4Distances[1] + st4Distances[2] + st4Distances[3]);
		if ((st4Distances[0] < fAverage) && (st4Distances[1] > fAverage)) Turn = 1;
		if ((st4Distances[1] < fAverage) && (st4Distances[2] > fAverage)) Turn = 2;
		if ((st4Distances[2] < fAverage) && (st4Distances[3] > fAverage)) Turn = 3;
		if ((st4Distances[3] < fAverage) && (st4Distances[0] > fAverage)) Turn = 0;
	
	
		lNbPoints &= 4;
		*(MATH_tdst_Vector *)&(p4Dir[0]) = st44Points_B[(Turn + 0) & 3];
		*(MATH_tdst_Vector *)&(p4Dir[1]) = st44Points_B[(Turn + 1) & 3];
		*(MATH_tdst_Vector *)&(p4Dir[2]) = st44Points_B[(Turn + 2) & 3];
		*(MATH_tdst_Vector *)&(p4Dir[3]) = st44Points_B[(Turn + 3) & 3];
	}

	return lNbPoints;
}
#ifdef WTR_ComputeOriginalsUV
void WTR_Compute_Mesh0_Rasterize_Line_UV_ORIG(register SOFT_tdst_UV *pVertexMapLine , register MATH_tdst_Vector *p1 ,register  MATH_tdst_Vector *p2 , register u32 Lenght,float SclFactor)
{
	GEO_Vertex *pVertexMapLineLast ;
	MATH_tdst_Vector inc ;
	inc = *p1;
	pVertexMapLineLast = (GEO_Vertex*)pVertexMapLine + Lenght;
	while (pVertexMapLine < (SOFT_tdst_UV*)pVertexMapLineLast)
	{
		MATH_AddEqualVector(&inc , p2);
		pVertexMapLine->u = inc.x * SclFactor;
		pVertexMapLine->v = inc.y * SclFactor;
		pVertexMapLine++;
	}
}
#endif
void WTR_Compute_Mesh0_Rasterize_Line(register GEO_Vertex *pVertexMapLine ,  register GEO_VertexForZmap *Zmap , register u32 SizeofZMapPo2 ,register  float ZMapFactor , register MATH_tdst_Vector *p1 ,register  MATH_tdst_Vector *p2 , register u32 Lenght , register float fChpyFactor,register  float ZHeightFactor , register float SeconPass)
{
#ifdef WTR_PSX2_ASSEMBLY
	register ULONG SizeofZMap , MASK , TMP  , TMP1;	
	register GEO_Vertex *pVertexMapLineLast ;
	register s32 	ADDRESS,ADDRESSPL;
	pVertexMapLineLast = pVertexMapLine + Lenght;
	SizeofZMap = 1 << SizeofZMapPo2;
	ZMapFactor *= (float)SizeofZMap;
	
#define VF_DLTA $vf01

#define VF_INCA $vf02
#define VF_P00A $vf03
#define VF_P01A $vf04
#define VF_P10A $vf05
#define VF_P11A $vf06
#define VF_FXYA $vf07
#define VF_1XYA $vf08
#define VF_TM0A $vf09
#define VF_TM1A $vf10
#define VF_TM2A $vf11

#define VF_INCB $vf12
#define VF_P00B $vf13
#define VF_P01B $vf14
#define VF_P10B $vf15
#define VF_P11B $vf16
#define VF_FXYB $vf17
#define VF_1XYB $vf18
#define VF_TM0B $vf19
#define VF_TM1B $vf20
#define VF_TM2B $vf21

#define VF_INCR $vf22
#define VF_PREV $vf23
#define VF_PVML $vf24


	MASK = SizeofZMap - 1;
	asm __volatile__ ("
	
		lqc2 	VF_INCA , 0(p1)
		lqc2 	VF_DLTA , 0(p2)
		ppacw 	MASK,MASK,MASK
		ppacw 	MASK,MASK,MASK
		mfc1	TMP , ZMapFactor
		ppacw 	TMP,TMP,TMP
		ppacw 	TMP,TMP,TMP
		qmtc2	TMP,$vf30
		mfc1	TMP ,  fChpyFactor
		ppacw 	TMP,TMP,TMP
		ppacw 	TMP,TMP,TMP
		mfc1	TMP1 , ZHeightFactor
		ppacw 	TMP1,TMP1,TMP1
		ppacw 	TMP1,TMP1,TMP1
		ppacw 	TMP,TMP1,TMP
		qmtc2	TMP,$vf31
		xor		TMP1 , TMP1
		add		TMP1 , TMP1 , 16384
		add		TMP1 , TMP1 , TMP1
		ppacw 	TMP1,TMP1,TMP1
		ppacw 	TMP1,TMP1,TMP1
		qmtc2	TMP1,$vf29
		vitof0	$vf29 , $vf29
		
		vmulx.w	$vf31 , $vf31 , $vf00x
		vmulx.w	VF_INCA , VF_INCA , $vf00x
		vmulx.w	VF_DLTA , VF_DLTA , $vf00x
		
		//SeconPass
		mfc1	TMP1 , SeconPass
		qmtc2	TMP1 , VF_TM0A
		vadd	VF_PVML , $vf00 , $vf00
		vaddw.y	VF_PVML , $vf00 , $vf00w
		vsubx.y	VF_PVML , VF_PVML , VF_TM0A
		vaddx.x	VF_PVML , VF_PVML , VF_TM0A

		
		
		vmul	VF_TM0A , VF_INCA , $vf30 // Hcoef | Hcoef
		vadd	VF_TM0A , VF_TM0A , $vf29
		vftoi0	VF_TM2A , VF_TM0A
		vitof0	VF_TM1A , VF_TM2A
		qmfc2	ADDRESS 	, VF_TM2A
		pand	ADDRESS 	, ADDRESS 	, MASK
		dsrl32	ADDRESSPL 	, ADDRESS 	, 0
		add		ADDRESS 	, ADDRESS 	, ADDRESSPL
		sllv	ADDRESSPL 	, ADDRESSPL , SizeofZMapPo2
		add		ADDRESS 	, ADDRESS 	, ADDRESSPL
		add		ADDRESSPL 	, ADDRESS 	, SizeofZMap
		sll		ADDRESS 	, ADDRESS 	, 4
		sll		ADDRESSPL 	, ADDRESSPL , 4
		add		ADDRESS 	, ADDRESS 	, Zmap
		add		ADDRESSPL 	, ADDRESSPL	, Zmap
		
	");
	SizeofZMap++;

	while (pVertexMapLine < pVertexMapLineLast)
	{
		asm __volatile__ ("
			PSX2_ASM_PREFETCH(0x00(ADDRESS))
			lqc2	VF_PREV , 0(pVertexMapLine)
			vadd	VF_INCA , VF_INCA , VF_DLTA
			vsub	VF_1XYA , VF_TM0A , VF_TM1A	// VF21 = FX 		| 	FY
			vmul	VF_TM0A , VF_INCA , $vf30 // Hcoef | Hcoef
			vmuly.x	VF_FXYA , VF_1XYA , VF_1XYA	// VF20 = FX * FY	|	--------
			lqc2	VF_P11A , 0x10(ADDRESSPL)	// [ X + 1 , Y + 1 ]
			lqc2	VF_P10A , 0x00(ADDRESSPL)	// [ X , Y + 1 ]
			lqc2	VF_P01A , 0x10(ADDRESS)	// [ X + 1, Y ]
			lqc2	VF_P00A , 0x00(ADDRESS)	// [ X , Y ]
			vmulax	$acc , VF_P11A , VF_FXYA 	// FX * FY
			vadd	VF_TM0A , VF_TM0A , $vf29
			vmadday	$acc , VF_P10A , VF_1XYA 	// (1.0f - FX) * FY 			== FY - FX * FY
			vftoi0	VF_TM2A , VF_TM0A
			vmsubax	$acc , VF_P10A , VF_FXYA 	// (1.0f - FX) * FY 			== FY - FX * FY
			vitof0	VF_TM1A , VF_TM2A
			vmaddax	$acc , VF_P01A , VF_1XYA	// FX * (1.0f - FY)				== FX - FX * FY
			qmfc2	ADDRESS 	, VF_TM2A
			vmsubax	$acc , VF_P01A , VF_FXYA	// FX * (1.0f - FY)				== FX - FX * FY
			pand	ADDRESS 	, ADDRESS 	, MASK
			vmaddaw	$acc , VF_P00A , $vf00w	// (1.0f - FX) * (1.0f - FY)	== (1.0f) - FX - FY + FX * FY
			dsrl32	ADDRESSPL 	, ADDRESS 	, 0
			vmsubax	$acc , VF_P00A , VF_1XYA	// 								== 1.0f - (FX) - FY + FX * FY
			add		ADDRESS 	, ADDRESS 	, ADDRESSPL
			vmsubay	$acc , VF_P00A , VF_1XYA	// 								== 1.0f - FX - (FY) + FX * FY
			sllv	ADDRESSPL 	, ADDRESSPL , SizeofZMapPo2
			vmaddx	VF_P00A , VF_P00A , VF_FXYA	// 								== 1.0f - FX - FY + (FX * FY)
			add		ADDRESS 	, ADDRESS 	, ADDRESSPL
			vmuly	VF_INCR , VF_INCA , VF_PVML
			add		ADDRESSPL 	, ADDRESS 	, SizeofZMap
			vmul	VF_P00A , VF_P00A , $vf31	// ChoppyX | ChoppyY | ZFactor | 0
			sll		ADDRESS 	, ADDRESS 	, 4
			vmulx	VF_PREV , VF_PREV , VF_PVML
			sll		ADDRESSPL 	, ADDRESSPL , 4
			vadd	VF_P00A , VF_P00A , VF_INCR
			add		ADDRESS 	, ADDRESS 	, Zmap
			vadd	VF_PREV , VF_PREV , VF_P00A
			add		ADDRESSPL 	, ADDRESSPL	, Zmap
			PSX2_ASM_PREFETCH(0x00(ADDRESSPL))
			sqc2	VF_PREV , 0(pVertexMapLine)
			add		pVertexMapLine , pVertexMapLine , sizeof(GEO_Vertex) * 1
		");
	
	}
#elif defined(TEX_MESH_USE_SSE)
	GEO_Vertex *pVertexMapLineLast = pVertexMapLine + Lenght;
	ULONG SizeofZMap = 1 << SizeofZMapPo2;
	__m128 stAccumulator;
	__m128 inc = _mm_loadu_ps(&p1->x);
	__m128 increment = _mm_loadu_ps(&p2->x);
	__m128 MulVect = _mm_setr_ps(fChpyFactor , fChpyFactor , ZHeightFactor, 0);
	__m128 AdditiveC = _mm_setr_ps(40000.0f, 40000.0f, 0, 0);
	__m128 MultiplicativeC = _mm_setr_ps(ZMapFactor * SizeofZMap, ZMapFactor * SizeofZMap, 0, 0);
	__m128 One = _mm_setr_ps(1.0f, 1.0f, 1.0f, 1.0f);
	__m128 VSeconPass0 = _mm_setr_ps(SeconPass, SeconPass, SeconPass, 0);
	__m128 VSeconPass1 = _mm_setr_ps(1.0f - SeconPass, 1.0f - SeconPass, 1.0f - SeconPass, 0);
	__m64 MaskZ = _mm_set_pi32(SizeofZMap - 1, SizeofZMap - 1);
	__m64 ZM;
	s32 ADDRESS;
	__m128 C;
	__m128 F;

	_mm_prefetch((char*)pVertexMapLine, _MM_HINT_NTA);
	_mm_prefetch((char*)(pVertexMapLine + 2), _MM_HINT_NTA);
	while (pVertexMapLine < pVertexMapLineLast)
	{
		_mm_prefetch((char*)(pVertexMapLine + 4), _MM_HINT_NTA);

		C = _mm_mul_ps(_mm_add_ps(inc, AdditiveC), MultiplicativeC);
		ZM = _mm_cvttps_pi32(C);

		F = _mm_sub_ps(C, _mm_cvtpi32_ps(C, ZM));
		F = _mm_shuffle_ps(F, _mm_sub_ps(One, F), _MM_SHUFFLE(1, 0, 1, 0));
		F = _mm_mul_ps(F, _mm_shuffle_ps(F, F, _MM_SHUFFLE(0, 3, 2, 1)));
		ZM = _mm_and_si64(ZM, MaskZ);
		ADDRESS = ZM.m64_u32[0] + ZM.m64_u32[1] + (ZM.m64_u32[1] << SizeofZMapPo2);

		stAccumulator =
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(Zmap[ADDRESS], _mm_shuffle_ps(F, F, _MM_SHUFFLE(2, 2, 2, 2))),
					_mm_mul_ps(Zmap[ADDRESS + 1], _mm_shuffle_ps(F, F, _MM_SHUFFLE(3, 3, 3, 3)))),
				_mm_add_ps(
					_mm_mul_ps(Zmap[ADDRESS + SizeofZMap + 1], _mm_shuffle_ps(F, F, _MM_SHUFFLE(1, 1, 1, 1))),
					_mm_mul_ps(Zmap[ADDRESS + SizeofZMap + 2], _mm_shuffle_ps(F, F, _MM_SHUFFLE(0, 0, 0, 0)))));

		stAccumulator =
			_mm_add_ps(
				_mm_mul_ps(stAccumulator, MulVect),
				_mm_add_ps(
					_mm_mul_ps(inc, VSeconPass1),
					_mm_mul_ps(_mm_loadu_ps(&pVertexMapLine->x), VSeconPass0)));

		_mm_store_ss(&pVertexMapLine->x, stAccumulator);
		_mm_store_ss(&pVertexMapLine->y, _mm_shuffle_ps(stAccumulator, stAccumulator, _MM_SHUFFLE(1, 1, 1, 1)));
		_mm_store_ss(&pVertexMapLine->z, _mm_shuffle_ps(stAccumulator, stAccumulator, _MM_SHUFFLE(2, 2, 2, 2)));
		inc = _mm_add_ps(inc, increment);
		pVertexMapLine++;
	}
	_mm_empty();
#else
	ULONG SizeofZMap;	
	GEO_Vertex *pVertexMapLineLast ;
	GEO_Vertex stAccumulator;
	MATH_tdst_Vector inc,MulVect ;
	inc = *p1;
	pVertexMapLineLast = pVertexMapLine + Lenght;
	SizeofZMap = 1 << SizeofZMapPo2;
	ZMapFactor *= (float)SizeofZMap;
	MATH_InitVector(&MulVect , fChpyFactor , fChpyFactor , ZHeightFactor);
	while (pVertexMapLine < pVertexMapLineLast)
	{
		s32 ZMX,ZMY;
		s32 ADDRESS;
		float CX,CY;
		float FX,FY;
		CX = (inc.x + 40000.0f) * ZMapFactor;
		CY = (inc.y + 40000.0f) * ZMapFactor;
		ZMX = WTR_fTrunc(CX);
		ZMY = WTR_fTrunc(CY);
		FX = CX - ZMX;
		FY = CY - ZMY;
		ZMX &= SizeofZMap - 1;
		ZMY &= SizeofZMap - 1;
		ADDRESS = ZMX + ZMY + (ZMY << SizeofZMapPo2);
		
		MATH_ScaleVector	(&stAccumulator , &Zmap[ADDRESS] , (1.0f - FX) * (1.0f - FY));
		MATH_AddScaleVector	(&stAccumulator , &stAccumulator , &Zmap[ADDRESS + 1] , FX * (1.0f - FY));
		MATH_AddScaleVector	(&stAccumulator , &stAccumulator , &Zmap[ADDRESS + SizeofZMap + 1] , (1.0f - FX) * FY);
		MATH_AddScaleVector	(&stAccumulator , &stAccumulator , &Zmap[ADDRESS + SizeofZMap + 2] , FX * FY);
		

		stAccumulator . x *= MulVect.x;
		stAccumulator . y *= MulVect.y;
		stAccumulator . z *= MulVect.z;
/*
		stAccumulator . x = 
		stAccumulator . y = 
		stAccumulator . z = 0.0f;
*/
		MATH_AddScaleVector( &stAccumulator , &stAccumulator , &inc , 1.0f - SeconPass);
		
		MATH_AddScaleVector( pVertexMapLine , &stAccumulator , pVertexMapLine , SeconPass);

		MATH_AddEqualVector(&inc , p2);
		pVertexMapLine++;
	}
#endif	
}


u32 WTR_Compute_Mesh0(WTR_Generator_Struct *pst_WaterParams , MATH_tdst_Vector *pCamPos , MATH_tdst_Vector *pCamDir , float AlphaX , float AlphaY )
{
	GEO_Vertex T4Dirs[4];
	MATH_tdst_Vector TCamYVEC,UVRECENTER;
	float ZHeight,ChoppyF;
	ULONG MipMapChoosen[20] , LiNum;
	u32 XSign,YSign;
	
	ZHeight = pst_WaterParams->ZFactor / pst_WaterParams->HCoef;
	ChoppyF = pst_WaterParams->fChoppyFactor/ pst_WaterParams->HCoef;

	if (!WTR_Compute4Points(T4Dirs , &TCamYVEC ,  pCamPos , pCamDir , pst_WaterParams->GlobalWaterZ , 18000.0f , AlphaX , AlphaY , ZHeight * 2.0f, ChoppyF * 2.0f)) return 0;
	
	L_memset(MipMapChoosen , 0 , sizeof(MipMapChoosen));
	
	
	MATH_BlendVector(&UVRECENTER , &T4Dirs[2] , &T4Dirs[3] , 0.5f);
	XSign = *((u32 *)&UVRECENTER.x) & 0x80000000;
	YSign = *((u32 *)&UVRECENTER.y) & 0x80000000;
	*((u32 *)&UVRECENTER.x) &= ~0x80000000;
	*((u32 *)&UVRECENTER.y) &= ~0x80000000;
	MATH_ScaleEqualVector(&UVRECENTER , 1024.0f * pst_WaterParams->SclaeUVFactor);
	
	UVRECENTER.x = (float)((s32)UVRECENTER.x >> 10);
	UVRECENTER.y = (float)((s32)UVRECENTER.y >> 10);
	MATH_ScaleEqualVector(&UVRECENTER ,1.0f/ pst_WaterParams->SclaeUVFactor);
	

	*((u32 *)&UVRECENTER.x) |= XSign;
	*((u32 *)&UVRECENTER.y) |= YSign;
	

/*
	0 - 1
	3 - 2
*/
	{
		float Z0 , Z2;
		float OoZ0 , OoZ2 , OoZI;
		float OoFB , OoFI;
		float OoLenght;
		float Minima;
		float Factor;
		float Factor_Linear_Base;
		float Factor_Linear_Inc;

		GEO_Vertex *pBase , *pBaseEnd ;
		ULONG 	*pBaseCol;
		MATH_tdst_Vector stp02 ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Vector stp13 ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Vector stp02b ONLY_PSX2_ALIGNED(16);
#ifdef WTR_ComputeNORMALS					
		GEO_Vertex  	 *p_NRM ONLY_PSX2_ALIGNED(16);
		GEO_Vertex  	 *p_NRMSPEC ONLY_PSX2_ALIGNED(16);
#endif
#ifdef 	WTR_ComputeOriginalsUV
		SOFT_tdst_UV	*pBaseUVOR ONLY_PSX2_ALIGNED(16);
#endif
		
		MATH_SubVector(&stp02 , &T4Dirs[0] , pCamPos);
		Z0 = MATH_f_DotProduct(pCamDir , &stp02);
		MATH_SubVector(&stp02 , &T4Dirs[3] , pCamPos);
		Z2 = MATH_f_DotProduct(pCamDir , &stp02);
		/*
		{
			u_int Counter;
			Counter = 512;
			while(Counter--)
			{
				pst_WaterParams->pColorTable[Counter] &= 0xff000000;
				pst_WaterParams->pColorTable[Counter] |= GDI_gpst_CurDD->pst_World->ul_AmbientColor & 0xffffff;
			}

		}*/
	
		OoLenght = 1.0f / (float)(pst_WaterParams->FrustrumMesh_SY - 1);
		OoZ0 = 1.0f / Z0;
		
		Minima = 1.0f / (pst_WaterParams->HCoef * (float)(1 << pst_WaterParams->ulWAveMapShift));
		Minima = Minima * (Z0 * OoLenght); 
		if (Z2 < Minima) Z2 = Minima;//*/
		
		
		OoZ2 = 1.0f / Z2;
		OoZI = (OoZ2 - OoZ0) * OoLenght;
		OoFB = (0.0f * OoZ0) * OoLenght;
		OoFI = (1.0f * OoZ2) * OoLenght;

		MATH_BlendVector(&stp02 , &T4Dirs[0] , &T4Dirs[1] , 0.5f);
		MATH_BlendVector(&stp13 , &T4Dirs[3] , &T4Dirs[2] , 0.5f);
		MATH_SubVector(&stp02 , &stp13 , &stp02);
		Factor_Linear_Inc = 1.0f / (pst_WaterParams->HCoef);
		Factor_Linear_Inc /= (float)(1 << pst_WaterParams->ulWAveMapShift);
		Factor_Linear_Inc = Factor_Linear_Inc / MATH_f_NormVector(&stp02);
		Factor_Linear_Base = 1.0f - Factor_Linear_Inc * pst_WaterParams->FrustrumMesh_SY;

		
		
		pBase = pst_WaterParams->pVertexMap;
#ifdef WTR_ComputeNORMALS
		p_NRM = pst_WaterParams->pVertexMap_NRM;
		p_NRMSPEC  = pst_WaterParams->pVertexMap_NRMSPEC;
#endif
#ifdef 	WTR_ComputeOriginalsUV
		pBaseUVOR = pst_WaterParams->p_VertexMapORIGINALS;
#endif
		pBaseCol = pst_WaterParams->pColors;
		pBaseEnd = pBase + pst_WaterParams->FrustrumMesh_SX * pst_WaterParams->FrustrumMesh_SY;
		OoLenght = 1.0f / (float)(pst_WaterParams->FrustrumMesh_SX - 1 );
		
		
		{
			LiNum = 0;
			while (pBase < pBaseEnd)
			{
				MATH_tdst_Vector Delta  ONLY_PSX2_ALIGNED(16);
				MATH_tdst_Vector Deltab ONLY_PSX2_ALIGNED(16);
				
				float AverageTexJump;
				ULONG SizeofZMapPo2;
				GEO_VertexForZmap *Zmap;
				
				
				Factor = fMin( (float)Factor_Linear_Base, OoFB / OoZ0);	
				MATH_BlendVector(&stp02 , &T4Dirs[0] , &T4Dirs[3] , Factor);
				MATH_BlendVector(&stp13 , &T4Dirs[1] , &T4Dirs[2] , Factor);
				
				OoFB += OoFI;
				OoZ0 += OoZI;
				Factor_Linear_Base += Factor_Linear_Inc;
				
				

				
				
				Zmap = pst_WaterParams->pConcatenedMap;
				SizeofZMapPo2 = pst_WaterParams->ulWAveMapShift;
				MATH_SubVector(&Delta , &stp13 ,&stp02);
				MATH_ScaleEqualVector(&Delta , OoLenght);	
				AverageTexJump = pst_WaterParams->HCoef * (float)(1 << SizeofZMapPo2) * fOptSqrt(Delta.x * Delta.x + Delta.y * Delta.y);
				while ((AverageTexJump > 2.0f) && (SizeofZMapPo2 >= 2))
				{
					Zmap += (1 << (SizeofZMapPo2 << 1)) + (2 << SizeofZMapPo2) + 1;
					SizeofZMapPo2--;//*/
					AverageTexJump *= 0.5f;
				}
				MipMapChoosen[SizeofZMapPo2] = LiNum;
				stp02b . x = stp02 . x * WTR_SCDPASS_COS_ALP + stp02 . y * WTR_SCDPASS_SIN_ALP;
				stp02b . y = stp02 . y * WTR_SCDPASS_COS_ALP - stp02 . x * WTR_SCDPASS_SIN_ALP;
				stp02b . z = 0.0f;
				Deltab . x = Delta . x * WTR_SCDPASS_COS_ALP + Delta . y * WTR_SCDPASS_SIN_ALP;
				Deltab . y = Delta . y * WTR_SCDPASS_COS_ALP - Delta . x * WTR_SCDPASS_SIN_ALP;
				Deltab . z = 0.0f;
				if (LiNum) 
				{
					WTR_Compute_Mesh0_Rasterize_Line(pBase, Zmap ,  SizeofZMapPo2, pst_WaterParams->HCoef, &stp02 , &Delta , pst_WaterParams->FrustrumMesh_SX, ChoppyF, ZHeight,0.0f);
#ifdef WTR_USE_2_PASSES					
					WTR_Compute_Mesh0_Rasterize_Line(pBase, Zmap ,  SizeofZMapPo2, pst_WaterParams->HCoef, &stp02b , &Deltab , pst_WaterParams->FrustrumMesh_SX, ChoppyF, ZHeight,1.0f);
#endif
#ifdef 	WTR_ComputeOriginalsUV
					{
						MATH_tdst_Vector stp02_Recentered;
						stp02_Recentered = stp02 ;
						stp02_Recentered.x -= UVRECENTER.x;
						stp02_Recentered.y -= UVRECENTER.y;
						stp02_Recentered.z = stp02.z;
						WTR_Compute_Mesh0_Rasterize_Line_UV_ORIG(pBaseUVOR, &stp02_Recentered , &Delta , pst_WaterParams->FrustrumMesh_SX,pst_WaterParams->SclaeUVFactor);
					}
#endif
					if (pst_WaterParams->CloudShadingEnabled)
						WTR_DirectComputeColors_CloudShading(pBase - pst_WaterParams->FrustrumMesh_SX,pBaseCol - pst_WaterParams->FrustrumMesh_SX, pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->SkyCol, pst_WaterParams->BottomCol , 1.0f / (ZHeight * 1.0f), pst_WaterParams->GlobalWaterZ);
					else
#ifdef WTR_ComputeNORMALS					
						WTR_DirectComputeColors_And_UV(pBase - pst_WaterParams->FrustrumMesh_SX , pCamPos , p_NRM  - pst_WaterParams->FrustrumMesh_SX , p_NRMSPEC  - pst_WaterParams->FrustrumMesh_SX,  pBaseCol - pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->pColorTable , 1.0f );
#else
						WTR_DirectComputeColors(pBase - pst_WaterParams->FrustrumMesh_SX , pCamPos , pBaseCol - pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->pColorTable );
#endif						
				} else
				{
					WTR_Compute_Mesh0_Rasterize_Line(pBase, Zmap ,  SizeofZMapPo2, pst_WaterParams->HCoef, &stp02 , &Delta , pst_WaterParams->FrustrumMesh_SX, 0.0f, 0.0f,0.0f);
				}
				LiNum++;
				pBase += pst_WaterParams->FrustrumMesh_SX;
				pBaseCol += pst_WaterParams->FrustrumMesh_SX;
#ifdef WTR_ComputeNORMALS					
				p_NRM += pst_WaterParams->FrustrumMesh_SX;
				p_NRMSPEC += pst_WaterParams->FrustrumMesh_SX;
#endif
#ifdef 	WTR_ComputeOriginalsUV
				pBaseUVOR += pst_WaterParams->FrustrumMesh_SX;
#endif
			}
		}
		pBase -= pst_WaterParams->FrustrumMesh_SX;
		while (pBase < pBaseEnd) 
		{
			(pBase++) -> z -= ZHeight;
		}
		if (pst_WaterParams->CloudShadingEnabled)
			WTR_DirectComputeColors_CloudShading(pst_WaterParams->pVertexMap ,pst_WaterParams->pColors , pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->SkyCol, pst_WaterParams->BottomCol , 1.0f / (ZHeight * 1.0f), pst_WaterParams->GlobalWaterZ);
		else
#ifdef WTR_ComputeNORMALS					
			WTR_DirectComputeColors_And_UV(pst_WaterParams->pVertexMap , pCamPos ,pst_WaterParams->pVertexMap_NRM , pst_WaterParams->pVertexMap_NRMSPEC , pst_WaterParams->pColors , pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->pColorTable , 1.0f );
#else
			WTR_DirectComputeColors(pst_WaterParams->pVertexMap , pCamPos , pst_WaterParams->pColors , pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->pColorTable );
#endif						
		
	}
#if 1
	/* Compute normales */
	{
		GEO_Vertex *pBase , *pBaseEnd ;
		ULONG 	*NextLine,*pBaseCol;
		pBase = pst_WaterParams->pVertexMap;
		pBaseCol = pst_WaterParams->pColors;
		pBaseEnd = pBase + pst_WaterParams->FrustrumMesh_SX * (pst_WaterParams->FrustrumMesh_SY - 1);
		NextLine = &MipMapChoosen[1];
		LiNum = 0;
		while (pBase < pBaseEnd)
		{
			if ((*NextLine == LiNum) )
			{
				if (LiNum)
				{
					L_memcpy(pBaseCol , pBaseCol - pst_WaterParams->FrustrumMesh_SX , pst_WaterParams->FrustrumMesh_SX * sizeof(ULONG));
				}
				NextLine++;
			} 
			LiNum ++;
			pBase += pst_WaterParams->FrustrumMesh_SX;
			pBaseCol += pst_WaterParams->FrustrumMesh_SX;
		}
		pBaseEnd += pst_WaterParams->FrustrumMesh_SX;
		while (pBase < pBaseEnd)
		{
			*(pBaseCol++) = *(pBaseCol - pst_WaterParams->FrustrumMesh_SX);
			pBase++;
		}
	}
#endif	
	return 1;
}
u32 WTR_Vector_2_Color(MATH_tdst_Vector *stColor)
{
	u32 R,G,B,A;
	R = (u32)(stColor->x * 255.0f);
	G = (u32)(stColor->y * 255.0f);
	B = (u32)(stColor->z * 255.0f);
	A = 0;
	if (R > 255) R = 255;
	if (G > 255) G = 255;
	if (B > 255) B = 255;
	if (A > 255) A = 255;
	return (A << 24) | (R << 0) | (G << 8) | (B << 16); // ARGB
}

void WTR_Color_2_Vector(MATH_tdst_Vector *stColorDST , u32 Color)
{
	u32 R,G,B,A;
	R = (Color >> 0) & 255;
	G = (Color >> 8) & 255;
	B = (Color >> 16) & 255;
	A = 0;
	stColorDST->x = (float)R / 255.0f;
	stColorDST->y = (float)G / 255.0f;
	stColorDST->z = (float)B / 255.0f;
}

u32 WTR_GetFresnelColor(WTR_Generator_Struct *pst_Params , float CosThetai)
{
	float SinTheta,Theta,CosTheta,Thetai,fs,ts ,  DarkHole,Alpha,ColorAlpha;
	MATH_tdst_Vector stBottomCol , stWaterCol , stSkyCol /*, stSpotColor */, stResultColor ;
	u32 A,R,G,B;

/*	A = R = G = B = (u32)(fAbs(CosThetai) * 255.0f);
	if (R > 255) R = 255;
	if (G > 255) G = 255;
	if (B > 255) B = 255;
	if (A > 255) A = 255;*/
	
	if (CosThetai > 0.0f)
	{
		Thetai = fAcos(CosThetai);
		SinTheta = fSin(Thetai) * 1.34f;
		if (SinTheta >= 1.0f)
		{
			// Totale refraction , no sky 
			Alpha = 0.0f;
			CosThetai = 1.0f - CosThetai;
		}
		else 
		{
			Alpha = 1.0f - SinTheta;
			CosThetai = 1.0f - CosThetai;
		}
		DarkHole = (1.0f - CosThetai);
		DarkHole *= DarkHole;
		ColorAlpha= 1.0f-Alpha;
	}
	else 
	{
		CosThetai = -CosThetai;
#ifdef WTR_ComputeNORMALS
		CosThetai = fSqrt(CosThetai);
#endif		
		Thetai = fAcos(CosThetai);
		SinTheta = fSin(Thetai) / 1.34f;
		Theta = fAsin(SinTheta);
		CosTheta = fCos(Theta);
		fs = fSin(Theta - Thetai)	/ fSin(Theta + Thetai);
		ts = fTan(Theta - Thetai)	/ fTan(Theta + Thetai);
		Alpha = (0.5f * ( fs*fs + ts*ts ));
		DarkHole = (1.0f - CosThetai);
		DarkHole *= DarkHole * DarkHole;
		ColorAlpha = Alpha;//fSqrt(fSqrt(Alpha));
	}
	
	WTR_Color_2_Vector(&stSkyCol   , pst_Params->SkyCol);
	WTR_Color_2_Vector(&stBottomCol, pst_Params->BottomCol);
	WTR_Color_2_Vector(&stWaterCol , pst_Params->WaterCol);
	

	MATH_BlendVector(&stBottomCol , &stBottomCol , &stWaterCol , DarkHole);


#ifdef _XBOX
	MATH_BlendVector(&stResultColor , &stBottomCol , &stSkyCol , Alpha);
#else
	MATH_BlendVector(&stResultColor , &stBottomCol , &stSkyCol , Alpha);
#endif

	ColorAlpha = 1.0f - ColorAlpha;
//	ColorAlpha *= ColorAlpha * ColorAlpha ;
	ColorAlpha = 1.0f - ColorAlpha;
#ifdef PSX2_TARGET
	ColorAlpha = 1.0f - Alpha;
//	ColorAlpha = 1.0f - ColorAlpha;
	ColorAlpha = ((ColorAlpha * ColorAlpha)) * 0.75f;
//	ColorAlpha = 1.0f - ColorAlpha;			
	if (ColorAlpha < 0.0f) ColorAlpha = 0.0f;
#elif defined(_GAMECUBE)
	ColorAlpha = (ColorAlpha * ColorAlpha) + ColorAlpha  ;
//	ColorAlpha *= 0.5f;

#elif defined(_XBOX)

	//XBOX
	ColorAlpha = 1.f - ColorAlpha ;
	ColorAlpha *= ColorAlpha ;
	ColorAlpha *= ColorAlpha ;
	ColorAlpha = 1.f - ColorAlpha ;

	/*if(ColorAlpha<0.95)
		ColorAlpha-=0.4;
	*/
//	ColorAlpha -= 0.1f;
	ColorAlpha *= ColorAlpha;
	ColorAlpha *= ColorAlpha;

	//RANGE CHECK 
	if(ColorAlpha<0.0f)
		ColorAlpha=0.0f;
	if(ColorAlpha>1.0f)
		ColorAlpha=1.0f;


#else
	ColorAlpha = 1.f - ColorAlpha ;
	ColorAlpha *= ColorAlpha ;
	ColorAlpha *= ColorAlpha ;
	ColorAlpha = 1.f - ColorAlpha ;
#endif

	R = (u32)(stResultColor.x * 255.0f);
	G = (u32)(stResultColor.y * 255.0f);
	B = (u32)(stResultColor.z * 255.0f);
	A = (u32)((ColorAlpha ) * 255.0f * 1.0f);

	
	if (R > 255) R = 255;
	if (G > 255) G = 255;
	if (B > 255) B = 255;
#ifdef PSX2_TARGET
	if (A > 127) A = 127;
#else
	if (A > 255) A = 255;
#endif	
	return (A << 24) | (R << 0) | (G << 8) | (B << 16); // ARGB
}
#ifdef _GAMECUBE
#define WTR_REVERSE_COLOR(a) ( ((a & 0xff) << 24) | ((a & 0xff00) << 8) | ((a & 0xff0000) >> 8) | ((a & 0xff000000) >> 24) )
#else
#define WTR_REVERSE_COLOR(a) a
#endif
void WTR_UpdateColorMode(WTR_Generator_Struct *pst_Params , float ZCam)
{
	ULONG Counter;
	if (ZCam > pst_Params -> GlobalWaterZ - (pst_Params -> ZFactor / pst_Params -> HCoef))
	{
		if (pst_Params ->ColorTableMod != 2)
		{
			// Restore colors down
			for (Counter = 0; Counter < 64; Counter++)
				pst_Params->pColorTable[256 - 32+ Counter] = pst_Params->pColorTableSave[Counter];

			for (Counter = 0; Counter < 32; Counter++)
				pst_Params->pColorTable[256 - 32 + Counter] = WTR_REVERSE_COLOR(pst_Params ->SkyCol);
			pst_Params -> ColorTableMod = 2;
		}
	} else
	{
		if (pst_Params -> ColorTableMod != 1)
		{
			// Restore colors up
			for (Counter = 0; Counter < 64 ; Counter++)
				pst_Params->pColorTable[256 - 32 + Counter] = pst_Params->pColorTableSave[Counter];

			for (Counter = 0; Counter < 32 ; Counter++)
				pst_Params->pColorTable[256 + Counter] = WTR_REVERSE_COLOR(pst_Params ->BottomCol);
			pst_Params -> ColorTableMod = 1;
		}
	}
}


void WTR_ComputeFresnel(WTR_Generator_Struct *pst_Params)
{
	s32 Counter ;
	pst_Params->BottomCol &= 0x00ffffff;
	pst_Params->WaterCol &= 0x00ffffff;
	pst_Params->SkyCol &= 0x00ffffff;
#ifdef PSX2_TARGET
/*	pst_Params->BottomCol = 0;
	pst_Params->WaterCol |= 0x7f000000;
	pst_Params->SkyCol |= 0x7f000000;*/
/*{
	MATH_tdst_Vector stWaterCol;	
	MATH_InitVector(&stWaterCol , 0.3f , 0.4f , 0.5f); 
	pst_Params->SkyCol = WTR_Vector_2_Color(&stWaterCol);
	pst_Params->WaterCol = pst_Params->SkyCol;
	}*/
#else
	pst_Params->BottomCol |= 0xff000000;
	pst_Params->WaterCol |= 0xff000000;
	pst_Params->SkyCol |= 0xff000000;
#endif
	if (!pst_Params->ulColorTableFastRecomputeOK)
	{
		ULONG ulSB,ulSS,ulSW; 
		ulSB = pst_Params->BottomCol;
		ulSS = pst_Params->WaterCol;
		ulSW = pst_Params->SkyCol;
		pst_Params->BottomCol 	&= ~0xffffff;
		pst_Params->WaterCol 	&= ~0xffffff;
		pst_Params->SkyCol 		&= ~0xffffff;
		pst_Params->BottomCol 	|= 0xff0000;
		pst_Params->WaterCol 	|= 0xff00;
		pst_Params->SkyCol 		|= 0xff;
		for (Counter = 0; Counter < 512 ; Counter++)
			pst_Params->pColorTableFastRecompute[Counter] = WTR_GetFresnelColor(pst_Params , (float)(256 - Counter ) / 256.0f);
		pst_Params->BottomCol	= ulSB;
		pst_Params->WaterCol	= ulSS;
		pst_Params->SkyCol		= ulSW;
		pst_Params->ulColorTableFastRecomputeOK = 1;
	} 
	{
		MATH_tdst_Vector stBottomCol , stWaterCol , stSkyCol , stResultColor ;
		WTR_Color_2_Vector(&stSkyCol   , pst_Params->SkyCol);
		WTR_Color_2_Vector(&stBottomCol, pst_Params->BottomCol);
		WTR_Color_2_Vector(&stWaterCol , pst_Params->WaterCol);
		for (Counter = 0; Counter < 512 ; Counter++)
		{
			ULONG CFast;
			CFast = pst_Params->pColorTableFastRecompute[Counter];
			MATH_ScaleVector(&stResultColor , &stSkyCol , (float)(CFast & 0xff) / 256.0f);
			CFast >>= 8;
			MATH_AddScaleVector(&stResultColor , &stResultColor , &stWaterCol , (float)(CFast & 0xff) / 256.0f);
			CFast >>= 8;
			MATH_AddScaleVector(&stResultColor , &stResultColor , &stBottomCol , (float)(CFast & 0xff) / 256.0f);
			CFast >>= 8;
			pst_Params->pColorTable[Counter] = (CFast << 24) | WTR_Vector_2_Color(&stResultColor);
#ifdef _GAMECUBE
			SwapDWord(&pst_Params->pColorTable[Counter]);
#endif

		}
	}
	for (Counter = 0; Counter < 64 ; Counter++)
		pst_Params->pColorTableSave[Counter] = pst_Params->pColorTable[256 - 32 + Counter];
	pst_Params->ColorTableMod = 0;
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////XBOX SPECIFIC FOR WATER/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef _XBOX

/////////////////////////////////HELPER FUNCTIONS CREATED FOR WATER DRAW ///////////////////////////////////


//Return the alpha values from DWORD packed color
BYTE GetAlpha(DWORD inColor);
BYTE GetAlpha(DWORD inColor)
{
	return (BYTE)(inColor>>24);
}

//Set alpha value in a DWORD packed color
void SetAlpha(DWORD *inColor,BYTE alpha);
void SetAlpha(DWORD *inColor,BYTE alpha)
{
	DWORD tempColor=(*inColor)&0x00FFFFFF;
	*inColor=(tempColor|(alpha<<24));
}

//"Blur" water's mesh 
void BlurWaterNormal( MATH_tdst_Vector *p_VertexMapNRM , int SX , int SY);
void BlurWaterNormal( MATH_tdst_Vector *p_VertexMapNRM , int SX , int SY)
{
#ifdef TEX_MESH_USE_SSE
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
#else
	int BC;
	MATH_tdst_Vector *p_VNRM , *p_VNRMLst , *p_VNRMLstLst;
	BC = 2;
	while (BC--)
	{
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
#endif
}


//Before drawing water, draw the "sky light" (like the sun) over reflection texture
void DrawSkyForReflection();
void DrawSkyForReflection()
{
/*
	D3DMATRIX objMAtrix;

	//NO OBJECT MATRIX
	D3DXMatrixIdentity(&objMAtrix);
#if !defined(_XENON)
	IDirect3DDevice8_SetTransform
	(
		p_gGx8SpecificData->mp_D3DDevice,
		D3DTS_WORLD,
		(const struct _D3DMATRIX *) &objMAtrix
	);
#endif
	Gx8_DrawSkyLight();
*/
	
}

//Set the texture matrix (choosing the stage) to project texture over the scene (used for water's textures)
void SetTextureRefMatrix(int stage);
void SetTextureRefMatrix(int stage)
{
#if !defined(_XENON)
	D3DMATRIX projMatrix;
	D3DMATRIX mulMAtrix;
	D3DMATRIX finMAtrix;


	//Get projection matrix from pipeline
	IDirect3DDevice8_GetTransform(p_gGx8SpecificData->mp_D3DDevice,
								  D3DTS_PROJECTION,
								  &projMatrix
								  );

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
	IDirect3DDevice8_SetTransform(p_gGx8SpecificData->mp_D3DDevice,2+stage,&finMAtrix);
#endif	


}



#endif
#if defined (__cplusplus)
unsigned int WTR_IsSymetryActive()
#else
u_int WTR_IsSymetryActive()
#endif
{
	if (pst_GlobalsWaterParams && pst_GlobalsWaterParams->ulSymetryActive) 
		return 1;
	else 
		return 0;
}
////////////////////////////////////////////////////////////////////////////////

///DRAW WATER MESH
#if defined (__cplusplus)
unsigned int WTR_Mesh_Do(WTR_Generator_Struct *pst_Params)
#else
u_int WTR_Mesh_Do(WTR_Generator_Struct *pst_Params)
#endif
{
#ifndef PSX2_TARGET
	extern float TIM_gf_dt;
#endif	
	MATH_tdst_Vector *pZAxis;
	MATH_tdst_Vector *pPos;
	float AlphaX , AlphaY;
	GEO_Vertex T4DirsSubPlane[4];
	MATH_tdst_Vector TCamYVEC;

#ifdef ACTIVE_EDITORS
	if (pst_Params->pBackRef)
	{
		if ((pst_Params->pBackRef->WaterCol != pst_Params->WaterCol) || 
			(pst_Params->pBackRef->BottomCol != pst_Params->BottomCol) || 
			(pst_Params->pBackRef->SkyCol != pst_Params->SkyCol))
		{
			pst_Params->WaterCol	= pst_Params->pBackRef->WaterCol;
			pst_Params->BottomCol	= pst_Params->pBackRef->BottomCol;
			pst_Params->SkyCol		= pst_Params->pBackRef->SkyCol;
			WTR_ComputeFresnel(pst_Params);
		}
	}
#endif
	// twist Correction
	{
			MATH_tdst_Vector Twist,L4;
			float fTwist;
			MATH_InitVector(&L4 , 0.0f, 0.0f, 1.0f);
			MATH_CrossProduct(&Twist , &L4 , MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
			if (MATH_b_NulVector(&Twist))
				AlphaX = 1.0f;
			else
			{
				MATH_NormalizeVector(&Twist,&Twist);
				fTwist = MATH_f_DotProduct(&Twist , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
				MATH_MulVector(&L4 , &Twist , fTwist);
				fTwist = 1.0f + MATH_f_Distance(&L4 , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
				AlphaX = 1.0f / (fTwist * fTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2.0f));
			}
	}

	
	AlphaY = 1.0f;


	WTR_UpdateColorMode(pst_Params , GDI_gpst_CurDD->st_Camera.st_Matrix.T.z);
	pZAxis = MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix);
	pPos = &GDI_gpst_CurDD->st_Camera.st_Matrix.T;
//	if (bWiredLock)
	{
		if (!WTR_Compute_Mesh0(pst_Params , pPos , pZAxis , AlphaX , AlphaY )) return 0;
		WTR_Compute4Points(T4DirsSubPlane , &TCamYVEC ,  pPos , pZAxis , pst_Params ->GlobalWaterZ - 100.0f, 18000.0f , AlphaX, AlphaY , 0.0f, 0.0f);
	}
	T4DirsSubPlane[0] = pst_Params->pVertexMap[0];
	T4DirsSubPlane[1] = pst_Params->pVertexMap[pst_Params->FrustrumMesh_SX - 1];
	
	GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_Fogged;
	GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_EnableFog, 1);
//////////////////////////////////////////////////////////////////////////////////////////////////////
//WATER FOR X, RULEZ!!!!!!!///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_XBOX) && !defined(_NO_WATER_)
#define popo
#ifdef popo
	{
		// simplify the code using this macros
		LPDIRECT3DDEVICE8 d3dDevice = p_gGx8SpecificData->mp_D3DDevice;
		#define SetTextureStageState(a, b, c) \
			IDirect3DDevice8_SetTextureStageState(d3dDevice, (a), (b), (c))
		#define SetRenderState(a, b) \
			IDirect3DDevice8_SetRenderState(d3dDevice, (a), (b))

		//Define the vertex format for water
		//#define WATER_DX_FORMAT  D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_NORMAL | 0x020 // 0x020 is D3DFVF_PSIZE
//		#define WATER_DX_FORMAT  D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_TEX2 // 0x020 is D3DFVF_PSIZE
		#define WATER_DX_FORMAT   D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 // 0x020 is D3DFVF_PSIZE

				ULONG TextureIndex;
		TEX_tdst_Data *TexFound;


		//water color
		int WATER_COLOR = ((255<<24)|(0<<16)|(0<<8)|(0));

/*		MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
		MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
		MAT_SET_FLAG(ulFlags, MAT_Cul_Flag_NoZWrite|MAT_Cul_Flag_Bilinear);*/


		//Water vertex format (position, normal and color)
		typedef struct __declspec(align(32)) WaterVertex
		{
			float x;//4
			float y;
			float z;//12

			int WaterColor;//32
			/*float nx;
			float ny;
			float nz;*/
			float u;//16
			float v;
			float u2;//24
			float v2;
			

			//int WaterColor;//32

			//float dummy; // mapped as psize
			//float dummy2;

		} DXWaterVertex;
	
		//Temporary vertex, used to port vertex from jade to DX format
		DXWaterVertex tempVertex;

		static LPDIRECT3DVERTEXBUFFER8 p_xVertexBuffer;
		static LPDIRECT3DINDEXBUFFER8 p_xIndexBuffer;
		static int numPrimitives;
		MATH_tdst_Matrix st_FinalMatrix;
		MATH_tdst_Matrix st_TempMatrix;
		MATH_tdst_Matrix st_InvMatrix;
		int ulFlags = 0;   
		float camX = GDI_gpst_CurDD->st_Camera.st_Matrix.T.x;
		float camY = GDI_gpst_CurDD->st_Camera.st_Matrix.T.y;
		float camZ = GDI_gpst_CurDD->st_Camera.st_Matrix.T.z;

		// Create the vertex buffer (big enough to contail all the strip) and the index
		// buffer, the index buffer doesn't change during the game so fill it now
		if(!p_xVertexBuffer)
		{
			unsigned iX, iY;
			short *idxBase, *idx, curPos;
			bool duplicateFirst;

			IDirect3DDevice8_CreateVertexBuffer(
				d3dDevice,
				pst_Params->FrustrumMesh_SX * pst_Params->FrustrumMesh_SY * sizeof(DXWaterVertex),
				0, 0, 0, &p_xVertexBuffer);

			numPrimitives = (2 * (pst_Params->FrustrumMesh_SX + 1) + 4) * (pst_Params->FrustrumMesh_SY - 1) - 3;

			IDirect3DDevice8_CreateIndexBuffer(
				d3dDevice,
				(numPrimitives + 2) * sizeof(short),
				0, 0, 0, &p_xIndexBuffer);

			IDirect3DIndexBuffer8_Lock(p_xIndexBuffer, 0, 0, (BYTE**)&idxBase, 0);

			// each row duplicates the first and last vertex in order to glue them
			// the first vertex of the first row is skipped by specifying a offset of 1 in
			// function SetIndices
			curPos = 0;
			idx = idxBase;
			duplicateFirst = false;
			for(iY = 0; iY < pst_Params->FrustrumMesh_SY - 1; ++iY)//pst_Params->FrustrumMesh_SY
			{
//				ERR_X_Assert(curPos == iY * pst_Params->FrustrumMesh_SX);

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
			ERR_X_Assert((idx - idxBase) == numPrimitives + 2);

			// no unlock required on X-Box
			// IDirect3DIndexBuffer8_Unlock(p_xIndexBuffer);
		}
		else
		{
			// sanity check
			ERR_X_Assert(numPrimitives == (2 * (pst_Params->FrustrumMesh_SX + 1) + 4) * (pst_Params->FrustrumMesh_SY - 1) - 3);
		}
		
		// generate vertex in DX format for the water
		{
			float normalCoeff;
			MATH_tdst_Vector *Pvertex, *PvertexLast, *PvertNormal;
			SOFT_tdst_UV *PCoordUV;
			ULONG *pVertexCol;
			u32 YCounter;
			BYTE *pData;

			//Lock the vertex bufer to write in water mesh
			IDirect3DVertexBuffer8_Lock(p_xVertexBuffer, 0, 0, &pData, 0);
			
			//Water mesh's source for vertex and color
			Pvertex = pst_Params->pVertexMap;
			pVertexCol = pst_Params->pColors;

			//Water mesh's source for normals
			//>>>>>>>>>>>>PvertNormal = pst_Params->pVertexMap_NRM;
			PCoordUV = pst_Params->p_VertexMapORIGINALS;

			//Blur normals (to be checked...i don't know if this is useful)
			//>>>>>>>>>>>>>>>>BlurWaterNormal(PvertNormal, pst_Params->FrustrumMesh_SX, pst_Params->FrustrumMesh_SY);

			// pump up the cache
			_mm_prefetch((char*)Pvertex, _MM_HINT_NTA);
			_mm_prefetch((char*)(Pvertex + 2), _MM_HINT_NTA);
			/*_mm_prefetch((char*)PvertNormal, _MM_HINT_NTA);
			_mm_prefetch((char*)(PvertNormal + 2), _MM_HINT_NTA);*/
			//_mm_prefetch((char*)PCoordUV, _MM_HINT_NTA);
			//_mm_prefetch((char*)(PCoordUV+2), _MM_HINT_NTA);
			_mm_prefetch((char*)(pVertexCol), _MM_HINT_NTA);

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
					//_mm_prefetch((char*)(PvertNormal + 4), _MM_HINT_NTA);
					//_mm_prefetch((char*)(PCoordUV + 4), _MM_HINT_NTA);
					//_mm_prefetch((char*)(pVertexCol + 4), _MM_HINT_NTA);

					tempVertex.x = Pvertex->x;
					tempVertex.y = Pvertex->y;
					tempVertex.z = Pvertex->z;
					/*tempVertex.nx = tempVertex.x - camX;
					tempVertex.ny = tempVertex.y - camY;
					tempVertex.nz = tempVertex.z - camZ - (float)fabs((PvertNormal->x + PvertNormal->y) * normalCoeff);*/
					tempVertex.u= PCoordUV->u;
					tempVertex.v =PCoordUV->v;
					tempVertex.WaterColor = *pVertexCol;//Gx8_M_ConvertColor(*pVertexCol);
				
					// copy tempVertex in the vertex buffer using SSE to minimize cache
					// pollution (the copy happens in reverse order intentionally)
					//_mm_stream_ps((float*)(pData + 16), _mm_load_ps(&tempVertex.ny));
					_mm_stream_ps((float*)(pData + 16), _mm_load_ps(&tempVertex.u));
					_mm_stream_ps((float*)pData, _mm_load_ps(&tempVertex.x));
					//pData += 16;//32
					//_mm_stream_ps((float*)pData, _mm_load_ps(&tempVertex.u));
					pData += 32;//32

					++Pvertex;
					++pVertexCol;
					++PCoordUV;
					/*++PvertNormal;*/
				} // end while (X coordinate)

			} // end for (Y coordinate)

			// no unlock required on X-Box
			// IDirect3DVertexBuffer8_Unlock(p_xVertexBuffer);

		} // end of code block (vertex generation)

		// Begin rendering
		IDirect3DDevice8_BeginScene(d3dDevice);

		// save device settings
		Gx8_SaveDeviceSettings(p_gGx8SpecificData);

		// view matrix manipulation
		MATH_CopyMatrix(&st_TempMatrix, &GDI_gpst_CurDD->st_Camera.st_Matrix);

		MATH_InvertMatrix(&st_InvMatrix, &st_TempMatrix);
		st_InvMatrix.Sx = st_InvMatrix.Sy = st_InvMatrix.Sz=0.0f;
		st_InvMatrix.w = 1.0f;

		MATH_MakeOGLMatrix(&st_FinalMatrix, &st_InvMatrix);
		st_FinalMatrix.w = 1.0f;
		IDirect3DDevice8_SetTransform(d3dDevice, D3DTS_VIEW, (const D3DMATRIX*)&st_FinalMatrix);

		//Identity
		MATH_SetIdentityMatrix(&st_TempMatrix);
		MATH_MakeOGLMatrix(&st_FinalMatrix, &st_TempMatrix);

		IDirect3DDevice8_SetTransform(d3dDevice, D3DTS_WORLD, (const D3DMATRIX*)&st_FinalMatrix);

		// Texture coordinate generation settings
		/*SetTextureRefMatrix(0);

		// Settings for reflection
		SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED );

		// no normalized normal vector in camera space
		SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);*/

		// Set the stream source and indices
		IDirect3DDevice8_SetStreamSource(d3dDevice, 0, p_xVertexBuffer, sizeof(DXWaterVertex));
		IDirect3DDevice8_SetIndices(d3dDevice, p_xIndexBuffer, 0);

		// Set vertex format
		IDirect3DDevice8_SetVertexShader(d3dDevice, WATER_DX_FORMAT);

		// no alpha blending
		SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		//Textures filters settings
		SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

		//Texture clamping (clamp everything)
//		SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
//		SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

		// no hw light for water..this was cause for a very strange alpha blending bug!!!!
		SetRenderState(D3DRS_LIGHTING ,FALSE);
		//SetRenderState(D3DRS_TEXTUREFACTOR, WATER_COLOR);
		/*SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
		SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);*/

		// Water reflection texture (stage 0)
		//IDirect3DDevice8_SetTexture(d3dDevice, 0, (IDirect3DBaseTexture8*)waterReflectionTexture);
		TexFound = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Params->ulTextureRef);
		
		ulFlags = 0;
		/*MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);*/
		MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
		MAT_SET_FLAG(ulFlags, MAT_Cul_Flag_NoZWrite|MAT_Cul_Flag_Bilinear);
		

		if (TexFound)
			Gx8_SetTextureBlending(TexFound - TEX_gst_GlobalList.dst_Texture, ulFlags , 0);
		else
			Gx8_SetTextureBlending(0xffffffff, ulFlags , 0);

/*		IDirect3DDevice8_SetTexture(d3dDevice, 0, TexFound);
		IDirect3DDevice8_SetTexture(d3dDevice, 1, NULL);
*/
		SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);



		// normal zwrite and ztest (I don't need the water to be transparent)
		SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

		// draw everything as a single strip
		IDirect3DDevice8_DrawIndexedPrimitive(d3dDevice, D3DPT_TRIANGLESTRIP, 0, 0, 0, numPrimitives);

		// restore device settings
		Gx8_RestoreDeviceSettings(p_gGx8SpecificData);

		IDirect3DDevice8_EndScene(d3dDevice);
		}
#endif popo
#endif // defined(_XBOX) && !defined(_NO_WATER_)

	/* *******************************************************************************************/
	/* Render OpenGL ***************************************** ************************************/
	/* *******************************************************************************************/

#if defined(_XENON_RENDER)

#if defined(ACTIVE_EDITORS)
    if (GDI_b_IsXenonGraphics())
#endif
    {
        // Get texture index from its key
        TEX_tdst_Data* pTex = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Params->ulTextureRef);

        typedef struct
        {
            float x, y, z;
            DWORD color;
            float u, v;
            float nx, ny, nz;
        } WaterVtx;

        // Copy the vertices into a dynamic VB
        XeDynVertexBuffer* pVB;
        INT iNumVertices = pst_Params->FrustrumMesh_SX * pst_Params->FrustrumMesh_SY;

        XeMesh* pOceanMesh = g_oXeRenderer.GetOceanMesh();
        if(!g_oXeRenderer.IsOceanInitialized())
        {
            pVB = g_XeBufferMgr.CreateDynVertexBuffer();
            pOceanMesh->AddStream(XEVC_POSITION | XEVC_COLOR0 | XEVC_NORMAL | XEVC_TEXCOORD0, TRUE, pVB, iNumVertices);

            // Calculate indices
            INT numPrimitives = (2 * (pst_Params->FrustrumMesh_SX + 1) + 4) * (pst_Params->FrustrumMesh_SY - 1) - 3;
            XeIndexBuffer* pIB = g_XeBufferMgr.CreateIndexBuffer(numPrimitives + 2);
            pIB->SetFaceCount(numPrimitives);
            pOceanMesh->SetIndices(pIB);

            SHORT* pIBData = (SHORT*)pIB->Lock(numPrimitives);

            // each row duplicates the first and last vertex in order to glue them
            // the first vertex of the first row is skipped by specifying an offset of 1 in
            // function SetIndices
            int curPos = 0;
            bool duplicateFirst = false;
            unsigned int iX;
            for(unsigned int iY = 0; iY < pst_Params->FrustrumMesh_SY - 1; ++iY)
            {
                ERR_X_Assert(curPos == iY * pst_Params->FrustrumMesh_SX);

                if(duplicateFirst)
                    *pIBData++ = curPos;
                else
                    duplicateFirst = true;

                for(iX = 0; iX < pst_Params->FrustrumMesh_SX / 2; ++iX)
                {
                    *pIBData++ = curPos;
                    *pIBData++ = (short)(curPos + pst_Params->FrustrumMesh_SX);
                    ++curPos;
                }
                --iX;
                --curPos;
                *pIBData++ = (short)(curPos + pst_Params->FrustrumMesh_SX);
                for(; iX < pst_Params->FrustrumMesh_SX; ++iX)
                {
                    *pIBData++ = (short)(curPos + pst_Params->FrustrumMesh_SX);
                    *pIBData++ = curPos;
                    ++curPos;
                }
                *pIBData++ = curPos - 1;
                *pIBData++ = curPos - 1;
            }

            g_oXeRenderer.SetOceanInitialized();
        }
        else 
        {
            pVB = (XeDynVertexBuffer *)pOceanMesh->GetStream(0)->pBuffer;
        }


        WaterVtx* pVBData = (WaterVtx*)pVB->Lock(iNumVertices, sizeof(WaterVtx), FALSE, TRUE);

        MATH_tdst_Vector* pVertex = pst_Params->pVertexMap;
        MATH_tdst_Vector* pVertNormal = pst_Params->pVertexMap_NRM;
        ULONG*            pVertexCol = pst_Params->pColors;
        SOFT_tdst_UV*     pCoordUV = pst_Params->p_VertexMapORIGINALS;

        float camX = GDI_gpst_CurDD->st_Camera.st_Matrix.T.x;
        float camY = GDI_gpst_CurDD->st_Camera.st_Matrix.T.y;
        float camZ = GDI_gpst_CurDD->st_Camera.st_Matrix.T.z;
        float normalCoeff;

        for(unsigned int Y = 0; Y < pst_Params->FrustrumMesh_SY; ++Y)
        {
            //Calculate coefficient (used for normal perturbation) based on distance from point of view
            float depthCoeff = (((float)Y * 1.2f) / (float)pst_Params->FrustrumMesh_SY);

            if(depthCoeff > 1.0f)
                depthCoeff = 1.0f;

            //Calculate the mesh "normals" perturbation factor (normals are more perturbed as far triangle are from point of view)
            normalCoeff = 48.0f - depthCoeff * 45.0f;

            for(unsigned int X = 0; X < pst_Params->FrustrumMesh_SX; ++X)
            {
                pVBData->x = pVertex->x;
                pVBData->y = pVertex->y;
                pVBData->z = pVertex->z;
                pVBData->color = *pVertexCol;
                pVBData->u = pCoordUV->u;
                pVBData->v = pCoordUV->v;
                pVBData->nx = pVBData->x - camX;
                pVBData->ny = pVBData->y - camY;
                pVBData->nz = pVBData->z - camZ - (float)fabs((pVertNormal->x + pVertNormal->y) * normalCoeff);

                pVBData++;
                pVertex++;
                pVertexCol++;
                pCoordUV++;
            }
        }

        pVB->Unlock();

        MATH_tdst_Matrix st_FinalMatrix;
        MATH_SetIdentityMatrix(&st_FinalMatrix);
        g_oXeRenderer.SetOceanTextureID(pTex ? pTex->w_Index : -1);

        ULONG ulDrawMask = GDI_Cul_DM_NotWired | GDI_Cul_DM_UseRLI | GDI_Cul_DM_DontForceColor;
        if (pTex != NULL)
        {
            ulDrawMask |= GDI_Cul_DM_UseTexture;
        }

        g_oXeRenderer.QueueMeshForRender(&st_FinalMatrix,                   // global mtx
                                        pOceanMesh,
                                        g_oXeRenderer.GetOceanMaterial(),  // material
                                        ulDrawMask,                        // draw mask
                                        -1,                                // LM texture
                                        XeRT_OPAQUE,
                                        XeRenderObject::TriangleStrip,
                                        0,                                 // lod
                                        0,                                 // order
                                        NULL,                              // user data
                                        XeRenderObject::Ocean
                                        );
    }

#elif defined(WTR_OGL_VERSION)
	/* 1 Preinits */
	{
		ULONG ulFlags , TextureIndex;
		TEX_tdst_Data *TexFound;

		/*MATH_tdst_Matrix Matrix;
			MATH_SetIdentityMatrix(&Matrix);*/
		OGL_SetViewMatrix(&GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
		/*	glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
#ifdef WTR_ComputeNORMALS
		glEnable(GL_TEXTURE_2D);
#else
		glDisable(GL_TEXTURE_2D);
#endif
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);*/
		ulFlags = 0;
		MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
		MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
		MAT_SET_FLAG(ulFlags, MAT_Cul_Flag_NoZWrite|MAT_Cul_Flag_Bilinear);
		glDisable(GL_CULL_FACE);
		GDI_SetTextureBlending((*GDI_gpst_CurDD),0xffffffff, ulFlags , 0);
		glBegin(GL_TRIANGLE_STRIP);
		glColor4ubv((unsigned char *)(&pst_Params->BottomCol));
		glVertex3fv((float *)&T4DirsSubPlane[0] );//*/
		glVertex3fv((float *)&T4DirsSubPlane[1] );//*/
		glVertex3fv((float *)&T4DirsSubPlane[3] );//*/
		glVertex3fv((float *)&T4DirsSubPlane[2] );//*/
		glEnd();

		MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
		MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
		MAT_SET_FLAG(ulFlags, MAT_Cul_Flag_NoZWrite|MAT_Cul_Flag_Bilinear|MAT_Cul_Flag_TileU|MAT_Cul_Flag_TileV);

		glDisable(GL_CULL_FACE);
		

#ifdef WTR_ComputeNORMALS
		GDI_SetTextureBlending((*GDI_gpst_CurDD),pst_Params->ulTextureRef, ulFlags , 0);
#else
		GDI_SetTextureBlending((*GDI_gpst_CurDD),0xffffffff, ulFlags , 0);
#endif
		TexFound = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Params->ulTextureRef);
		if (TexFound)
			GDI_SetTextureBlending((*GDI_gpst_CurDD),TexFound - TEX_gst_GlobalList.dst_Texture, ulFlags , 0);
		else
			GDI_SetTextureBlending((*GDI_gpst_CurDD),0xffffffff, ulFlags , 0);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE , GL_DECAL );
 
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE /* GL_FILL */);
		/* 2 Render */
		{
			MATH_tdst_Vector *Pvertex,*PvertexLast;
			SOFT_tdst_UV		*p_UV;
			SOFT_tdst_UV		*p_UVOriginals;
			ULONG *pVertexCol;
			u32 YCounter;
			Pvertex = pst_Params->pVertexMap;
			pVertexCol = pst_Params->pColors;
#ifdef WTR_ComputeNORMALS
			p_UV = pst_Params->pVertexMap_NRM ; 
#endif
#ifdef WTR_ComputeOriginalsUV
			p_UVOriginals = pst_Params->p_VertexMapORIGINALS; 
#endif
			
			YCounter = pst_Params->FrustrumMesh_SY - 1;
			for (YCounter = 0 ; YCounter < pst_Params->FrustrumMesh_SY - 1 ; YCounter++ )
			{
				glBegin(GL_TRIANGLE_STRIP);
				PvertexLast = Pvertex + (pst_Params->FrustrumMesh_SX >> 1);
				while (Pvertex < PvertexLast)
				{
					glColor4ubv((unsigned char *)pVertexCol);
#ifdef WTR_ComputeNORMALS
					glTexCoord2fv((float *)p_UV);
#endif
#ifdef WTR_ComputeOriginalsUV
					glTexCoord2fv((float *)p_UVOriginals);
#endif

					glVertex3fv((float *)Pvertex);
					glColor4ubv((unsigned char *)(pVertexCol + pst_Params->FrustrumMesh_SX));
#ifdef WTR_ComputeNORMALS
					glTexCoord2fv((float *)(p_UV + pst_Params->FrustrumMesh_SX));
#endif
#ifdef WTR_ComputeOriginalsUV
					glTexCoord2fv((float *)(p_UVOriginals + pst_Params->FrustrumMesh_SX));
#endif
					glVertex3fv((float *)(Pvertex + pst_Params->FrustrumMesh_SX) );//*/
					Pvertex++;
					p_UVOriginals++;
					pVertexCol++;
					p_UV++;
				}
				glEnd();
				glBegin(GL_TRIANGLE_STRIP);
				PvertexLast = Pvertex + (pst_Params->FrustrumMesh_SX >> 1);
				Pvertex--;
				pVertexCol--;
				p_UV--;
				p_UVOriginals--;
				while (Pvertex < PvertexLast)
				{

					glColor4ubv((unsigned char *)(pVertexCol + pst_Params->FrustrumMesh_SX));
#ifdef WTR_ComputeNORMALS
					glTexCoord2fv((float *)(p_UV + pst_Params->FrustrumMesh_SX));
#endif
#ifdef WTR_ComputeOriginalsUV
					glTexCoord2fv((float *)(p_UVOriginals + pst_Params->FrustrumMesh_SX));
#endif
					glVertex3fv((float *)(Pvertex + pst_Params->FrustrumMesh_SX) );//*/
					glColor4ubv((unsigned char *)pVertexCol);
#ifdef WTR_ComputeNORMALS
					glTexCoord2fv((float *)(p_UV));
#endif
#ifdef WTR_ComputeOriginalsUV
					glTexCoord2fv((float *)p_UVOriginals);
#endif
					glVertex3fv((float *)Pvertex);
					Pvertex++;
					p_UVOriginals++;
					pVertexCol++;
					p_UV++;
				}
				glEnd();
			}
		}

		glDepthMask(GL_TRUE);
	}

#endif //WTR_OGL_VERSION
	/* *******************************************************************************************/
	/* Render PSX2 *******************************************************************************/
	/* *******************************************************************************************/
#ifdef PSX2_TARGET
	{
		{
			extern void GSP_l_DrawVertexTable(u_long128 *pBase , u_long64 *pBaseUV , ULONG *pColors , ULONG SX ,ULONG SY);
			extern void GSP_l_DrawQuad(u_long128 *pBase , ULONG *pColors);
			extern void GSP_Fogged(LONG _l_FogOn);
			ULONG ulFlags;
			TEX_tdst_Data *TexFound;
			ULONG pColors[4];
			GSP_Fogged(0);
			GDI_SetViewMatrix((*GDI_gpst_CurDD), &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
			ulFlags = 0;
			MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
			if (pst_Params->ulSymetryActive)
				MAT_SET_Blending(ulFlags, MAT_Cc_Op_Alpha);
			else
				MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
			MAT_SET_FLAG(ulFlags, MAT_Cul_Flag_NoZWrite|MAT_Cul_Flag_Bilinear);
			GDI_SetTextureBlending(&GDI_gpst_CurDD->st_GDI,0xffffffff, ulFlags,0);
			pColors[0] = pColors[1] = pColors[2] = pColors[3] = pst_Params->BottomCol;
			GSP_l_DrawQuad(T4DirsSubPlane , pColors);
			GspGlobal_ACCESS(bSRS_FromScratch) = 1;
			GSP_Fogged(1);
			
			if (pst_Params->ulSymetryActive)
				MAT_SET_Blending(ulFlags, MAT_Cc_Op_Alpha);
			else
				MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
			MAT_SET_FLAG(ulFlags, /*MAT_Cul_Flag_NoZWrite|*/MAT_Cul_Flag_Bilinear|MAT_Cul_Flag_TileU|MAT_Cul_Flag_TileV);
//			GDI_SetTextureBlending(&GDI_gpst_CurDD->st_GDI,0xffffffff, ulFlags,0);
			TexFound = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Params->ulTextureRef);
			if (TexFound)
				GDI_SetTextureBlending((*GDI_gpst_CurDD),TexFound - TEX_gst_GlobalList.dst_Texture, ulFlags , 0);
			else
				GDI_SetTextureBlending((*GDI_gpst_CurDD),0xffffffff, ulFlags , 0);
			
#ifdef WTR_ComputeNORMALS			
			GSP_l_DrawVertexTable(pst_Params->pVertexMap ,pst_Params->pVertexMap_NRM , pst_Params->pColors,pst_Params->FrustrumMesh_SX,pst_Params->FrustrumMesh_SY) ;
#else
			GSP_l_DrawVertexTable(pst_Params->pVertexMap ,pst_Params->p_VertexMapORIGINALS, pst_Params->pColors,pst_Params->FrustrumMesh_SX,pst_Params->FrustrumMesh_SY) ;
#endif			
		}
	}
#endif // PSX2_TARGET
	/* *******************************************************************************************/
	/* Render GMC ********************************************************************************/
	/* *******************************************************************************************/
#ifdef _GAMECUBE
	{
		ULONG ulTextureIndex;
		TEX_tdst_Data *TexFound;
		extern void GXI_RenderWATER(unsigned char *DL , u32 DLSize , u32 SX ,u32 SY , MATH_tdst_Vector *p_VertexMap , SOFT_tdst_UV *p_VertexMapORIG , u32 *pColors , MATH_tdst_Vector *p_Texture , u32 TexPo2, u32 _ulTextureIndex);
		
		TexFound = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Params->ulTextureRef);
		if (TexFound)
			ulTextureIndex = TexFound - TEX_gst_GlobalList.dst_Texture;
		else
			ulTextureIndex = 0xffffffff;
		
		/* Compute Differencial UV's */
		GXI_RenderWATER(pst_Params->ucWATER_DisplayList , WTR_DISPLAYLISTSIZE(pst_Params->FrustrumMesh_SX , pst_Params->FrustrumMesh_SY) , pst_Params->FrustrumMesh_SX ,pst_Params->FrustrumMesh_SY , pst_Params->pVertexMap , pst_Params->p_VertexMapORIGINALS , pst_Params->pColors , pst_Params->pConcatenedMap , pst_Params->ulWAveMapShift,ulTextureIndex);
	}//*/
#endif // PSX2_TARGET


	
#ifdef ACTIVE_EDITORS
	if (ENG_gb_EngineRunning)
#endif
//	if (bWiredLock)
	{
		pst_Params->fftime += TIM_gf_dt * pst_Params->SpeedFactor;
		WATERFFT_Turn_Spectre(&pst_Params->WTR , pst_Params->pSpaceMap , pst_Params->pSpaceMapDX , pst_Params->pSpaceMapDY , pst_Params->fftime);
		WTR_Concate(pst_Params);
	}

	return 1;
}


void WTR_Get_A_DifVector(MATH_tdst_Vector *pSrc , MATH_tdst_Vector *pDst , u32 Converg)
{
	if ((!pst_GlobalsWaterParams) || (pst_GlobalsWaterParams && pst_GlobalsWaterParams->CloudShadingEnabled))
	{
		MATH_InitVector( pDst , 0.0f , 0.0f , -1000000000.0f );
		return;
	}

	{
		s32 ZMX,ZMY;
		u32 SizeofZMap , SizeofZMapPo2,ADDRESS;
		//MATH_tdst_Vector inc,inc2,Loc,stAccumulator,stAccumulator2;
		MATH_tdst_Vector inc,Loc,stAccumulator;
		float CX,CY;
		float FX,FY,ZMapFactor;
		u32 Counter;
		inc = *pSrc;
		// make converge the function Counter time
		Counter = 1;
		if (Converg)
		Counter = 5;
		while (Counter--)
		{
			ZMapFactor = pst_GlobalsWaterParams->HCoef;
			ZMapFactor *= (float)(1 << pst_GlobalsWaterParams->WTR.BorderSizePo2);
			SizeofZMap = 1 << pst_GlobalsWaterParams->WTR.BorderSizePo2;
			SizeofZMapPo2 = pst_GlobalsWaterParams->WTR.BorderSizePo2;
			CX = (inc.x + 40000.0f) * ZMapFactor;
			CY = (inc.y + 40000.0f) * ZMapFactor;
			ZMX = WTR_fTrunc(CX);
			ZMY = WTR_fTrunc(CY);
			FX = CX - ZMX;
			FY = CY - ZMY;
			ZMX &= SizeofZMap - 1;
			ZMY &= SizeofZMap - 1;
			ADDRESS = ZMX + ZMY + (ZMY << SizeofZMapPo2);
			MATH_ScaleVector	(&stAccumulator , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS] , (1.0f - FX) * (1.0f - FY));
			MATH_AddScaleVector	(&stAccumulator , &stAccumulator , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS + 1] , (FX) * (1.0f - FY));
			MATH_AddScaleVector	(&stAccumulator , &stAccumulator , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS + SizeofZMap + 1] , (1.0f - FX) * (FY));
			MATH_AddScaleVector	(&stAccumulator , &stAccumulator , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS + SizeofZMap + 2] , (FX) * (FY));
#ifdef WTR_USE_2_PASSES			
			inc2 . x = inc . x * WTR_SCDPASS_COS_ALP + inc . y * WTR_SCDPASS_SIN_ALP;
			inc2 . y = inc . y * WTR_SCDPASS_COS_ALP - inc . x * WTR_SCDPASS_SIN_ALP;
			inc2 . z = 0.0f;

			CX = (inc2.x + 40000.0f) * ZMapFactor;
			CY = (inc2.y + 40000.0f) * ZMapFactor;
			ZMX = WTR_fTrunc(CX);
			ZMY = WTR_fTrunc(CY);
			FX = CX - ZMX;
			FY = CY - ZMY;
			ZMX &= SizeofZMap - 1;
			ZMY &= SizeofZMap - 1;
			ADDRESS = ZMX + ZMY + (ZMY << SizeofZMapPo2);
			MATH_ScaleVector	(&stAccumulator2 , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS] , (1.0f - FX) * (1.0f - FY));
			MATH_AddScaleVector	(&stAccumulator2 , &stAccumulator2 , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS + 1] , (FX) * (1.0f - FY));
			MATH_AddScaleVector	(&stAccumulator2 , &stAccumulator2 , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS + SizeofZMap + 1] , (1.0f - FX) * (FY));
			MATH_AddScaleVector	(&stAccumulator2 , &stAccumulator2 , (MATH_tdst_Vector*)&pst_GlobalsWaterParams->pConcatenedMap[ADDRESS + SizeofZMap + 2] , (FX) * (FY));

			MATH_AddVector(&stAccumulator , &stAccumulator , &stAccumulator2);
#endif WTR_USE_2_PASSES			
			

			Loc.x = (pst_GlobalsWaterParams->fChoppyFactor / pst_GlobalsWaterParams->HCoef) * stAccumulator.x;
			Loc.y = (pst_GlobalsWaterParams->fChoppyFactor / pst_GlobalsWaterParams->HCoef) * stAccumulator.y;
			Loc.z = pst_GlobalsWaterParams->GlobalWaterZ + (pst_GlobalsWaterParams->ZFactor / pst_GlobalsWaterParams->HCoef) * stAccumulator.z;
			inc.x += (pSrc->x - (inc.x + Loc.x)) * 0.5f;
			inc.y += (pSrc->y - (inc.y + Loc.y)) * 0.5f;
		}
		inc.x += Loc.x;
		inc.y += Loc.y;
		if (Converg)
		{
			*pDst = inc;
			pDst->z = Loc.z;
		}
		else
			*pDst = Loc;
	}
}

void WTR_Get_A_MrmVector(MATH_tdst_Vector *pSrc , MATH_tdst_Vector *pDst)
{
	MATH_tdst_Vector iP0,iP1,iPL;
	MATH_InitVector( pDst , 0.0f , 0.0f , 1.0f );
	if ((!pst_GlobalsWaterParams) || (pst_GlobalsWaterParams && pst_GlobalsWaterParams->CloudShadingEnabled)) return;
	WTR_Get_A_DifVector(pSrc , &iP0 , 1);
	iP1 = iPL = *pSrc;
	iP1.y += 1.5f * (1.0f / pst_GlobalsWaterParams ->HCoef) * (1.0f / (float)(1 << pst_GlobalsWaterParams->WTR.BorderSizePo2));
	iPL.x += 1.5f * (1.0f / pst_GlobalsWaterParams ->HCoef) * (1.0f / (float)(1 << pst_GlobalsWaterParams->WTR.BorderSizePo2));
	WTR_Get_A_DifVector(&iP1 , &iP1 , 1);
	WTR_Get_A_DifVector(&iPL , &iPL , 1);

	MATH_SubVector(&iP1 , &iP0 , &iP1 );
	MATH_SubVector(&iPL , &iP0 , &iPL );

	MATH_CrossProduct(&iP0 , &iPL , &iP1);
	MATH_NormalizeVector(pDst , &iP0 );
}
void WTR_BeginLoadWorldHook()
{
	if (pst_GlobalsWaterParams) WTR_Mesh_Destroy(pst_GlobalsWaterParams);
	pst_GlobalsWaterParams = NULL;
	WTR_SetDefaulExp(&stExportWaterParrams);
	
	if (pst_GlobalsWaterParams_Save) WTR_Mesh_Destroy(pst_GlobalsWaterParams_Save);
	pst_GlobalsWaterParams_Save = NULL;
	
}

#ifdef PSX2_TARGET	
extern void GSP_AE_OldZListHook(int ZlistNum , float Z , float fHeight );
#endif
void GSP_AE_ZListHook(int ZlistNum )
{
	if (ZlistNum == 0)
	{
		_GSP_BeginRaster(43);
        if (pst_GlobalsWaterParams) 
        {
#if defined (__cplusplus)
			static unsigned int Workkkk; // E3 slaughter
#else
			static u_int Workkkk; // E3 slaughter
#endif
#ifdef PSX2_TARGET	
            extern u_int No3DWTR;
/*            if (Workkkk)
				GSP_AE_OldZListHook( ZlistNum , pst_GlobalsWaterParams -> GlobalWaterZ - 1.0f, 1.0f);
			else	
				GSP_AE_OldZListHook( ZlistNum , 0.0f, 0.0f);*/
			if (!No3DWTR)
#endif		
			Workkkk = WTR_Mesh_Do(pst_GlobalsWaterParams);
		}
/*#ifdef PSX2_TARGET	
		else
			GSP_AE_OldZListHook( ZlistNum ,0.0f , 0.0f);
#endif		*/
		_GSP_EndRaster(43);
	}
	
}//*/
#endif
