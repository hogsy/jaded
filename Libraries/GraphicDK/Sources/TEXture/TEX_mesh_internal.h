#ifndef TEX_MESH_INTERNAL_H
#define TEX_MESH_INTERNAL_H

#include "TEXture/WATER_FFT.h"

#if defined(_M_IX86) && !defined(ACTIVE_EDITORS)
#define TEX_MESH_USE_SSE
#endif

#ifdef TEX_MESH_USE_SSE
#include <xmmintrin.h>
typedef __m128 GEO_VertexForZmap;
#else
typedef GEO_Vertex GEO_VertexForZmap;
#endif

#ifdef _GAMECUBE
#define WTR_ComputeNORMALS
#define WTR_ComputeOriginalsUV
#endif

#ifdef _M_IX86
#define WTR_ComputeNORMALS
#endif

typedef unsigned	u32;
typedef int			s32;

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

#ifdef ACTIVE_EDITORS
	WATER_Export_Struct *pBackRef;
#endif
}
WTR_Generator_Struct;

#endif // TEX_MESH_INTERNAL_H