/*$T SOFTstruct.h GC!1.71 01/26/00 16:27:26 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __SOFTSTRUCT_H__
#define __SOFTSTRUCT_H__

#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* aligned vertex */
#ifdef PSX2_TARGET
#define ALIGNED_VERTEX
#endif

#ifdef ALIGNED_VERTEX
#define VCast(a) (MATH_tdst_Vector *) (a)
#define AVCast(a) (SOFT_tdst_AVertex *) (a)
#define GEO_Vertex  SOFT_tdst_AVertex
#else
#define VCast(a) (a)
#define AVCast(a) (a)
#define GEO_Vertex  MATH_tdst_Vector
#endif



/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define SOFT_Cul_ComputingBufferSize    50000
#elif  defined(_XENON_RENDER)
#define SOFT_Cul_ComputingBufferSize    15000
#else 
#define SOFT_Cul_ComputingBufferSize    5000
#endif

#define SOFT_Cul_MaxCloneNb             100

#define SOFT_Cul_CB_SpecularField       0x00000001
#define SOFT_Cul_CB_InvertMatrix        0x00000002
#define SOFT_Cul_CB_SpecularColorField  0x00000004

/* flags for arrow */
#define SOFT_Cul_Arrow_UseFactor        0x00000001
#define SOFT_Cul_Arrow_Cube             0x00000002
#define SOFT_Cul_Arrow_ForSpot          0x00000004
#define SOFT_Cul_Arrow_DrawNotLine      0x00000008
#define	SOFT_Cul_Arrow_DrawParallel		0x00000010
#define SOFT_Cul_Arrow_NoEndPoint		0x00000020
#define SOFT_Cul_Arrow_ForShadow		0x00000040

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct  SOFT_tdst_ThreeIndex_
{
    LONG    l_Index[3];
} SOFT_tdst_ThreeIndex;

typedef struct  SOFT_tdst_TriangleIndex_
{
    unsigned short auw_Point[3];
    unsigned short auw_UV[3];
} SOFT_tdst_TriangleIndex;

typedef struct  SOFT_tdst_Vertex_
{
    float   x, y;
    float   ooz;
    ULONG   color;
    float   u, v;
} SOFT_tdst_Vertex;

typedef struct SOFT_tdst_AVertex_
{
    float x,y,z;
    union 
    {
        float w;
        ULONG c;
    };
} SOFT_tdst_AVertex;

typedef struct  SOFT_tdst_Triangle_
{
    SOFT_tdst_Vertex    *v[3];
} SOFT_tdst_Triangle;

typedef struct  SOFT_tdst_Ellipse_
{
    LONG                l_Flag;
    SOFT_tdst_Vertex    st_Center;
    MATH_tdst_Vector    st_A;
    MATH_tdst_Vector    st_B;
    float               f_AlphaStart;
    float               f_AlphaEnd;
    float               f_AlphaDelta;
} SOFT_tdst_Ellipse;

typedef struct  SOFT_tdst_Arrow_
{
    LONG                l_Flag;
    SOFT_tdst_Vertex    st_Start;
    MATH_tdst_Vector    st_Axis;
    MATH_tdst_Vector    st_Up;
    float               f_Move;
} SOFT_tdst_Arrow;

typedef struct  SOFT_tdst_Square_
{
    LONG                l_Flag;
    SOFT_tdst_Vertex    st_Pos;
    MATH_tdst_Vector    st_A;
    MATH_tdst_Vector    st_B;
    MATH_tdst_Vector    st_Move;
    MATH_tdst_Vector    st_PosDecal;
} SOFT_tdst_Square;

typedef struct  SOFT_tdst_UV_
{
    float   u, v;
} SOFT_tdst_UV;

typedef struct  SOFT_tdst_ComputingBuffers_
{
    ULONG               ul_Flags;
    ULONG               ul_Ambient;
//Test Clones Bourrinass
#ifdef WIN32
	ULONG               aul_ComputeClone[SOFT_Cul_MaxCloneNb][SOFT_Cul_ComputingBufferSize]		ONLY_PSX2_ALIGNED(16);
#endif
	ULONG               aul_Diffuse[SOFT_Cul_ComputingBufferSize]		ONLY_PSX2_ALIGNED(16);
    ULONG               aul_Specular[SOFT_Cul_ComputingBufferSize]		ONLY_PSX2_ALIGNED(16);
#ifdef _XENON_RENDER
    ULONG                   staticComputedColors[SOFT_Cul_ComputingBufferSize]	ONLY_PSX2_ALIGNED(16);
	ULONG                   *ComputedColors;
#else
	ULONG               ComputedColors[SOFT_Cul_ComputingBufferSize]	ONLY_PSX2_ALIGNED(16);
#endif
	ULONG               ComputedAlpha[SOFT_Cul_ComputingBufferSize]		ONLY_PSX2_ALIGNED(16);
    ULONG               *CurrentColorField;
    ULONG               *CurrentAlphaField;
    GEO_Vertex          ast_SpecialVB[SOFT_Cul_ComputingBufferSize] 	ONLY_PSX2_ALIGNED(16);/* Used for SKIn & modifiers */
    GEO_Vertex          ast_SpecialVB2[SOFT_Cul_ComputingBufferSize] 	ONLY_PSX2_ALIGNED(16);/* Used for SKIn & modifiers */
#ifdef JADEFUSION
	MATH_tdst_Quaternion    ast_SpecialVB3[SOFT_Cul_ComputingBufferSize] 	ONLY_PSX2_ALIGNED(16);/* Used for SKIn & modifiers */
#endif
	GEO_Vertex          ast_3D[SOFT_Cul_ComputingBufferSize]			ONLY_PSX2_ALIGNED(16);
    SOFT_tdst_Vertex    ast_2D[SOFT_Cul_ComputingBufferSize];
//#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)    
    SOFT_tdst_UV        ast_UV[SOFT_Cul_ComputingBufferSize]		ONLY_PSX2_ALIGNED(16);
//#endif    
#ifdef	_GAMECUBE
    unsigned short      aus_RedirectBuffer[SOFT_Cul_ComputingBufferSize] ONLY_PSX2_ALIGNED(16);
#endif    
#if defined(_XENON_RENDER)
    #define SOFT_Cul_TangentSpaceBufferSize 16384
#endif

    MATH_tdst_Vector    ast_SpecularField[SOFT_Cul_ComputingBufferSize]	ONLY_PSX2_ALIGNED(16);
    SOFT_tdst_UV        *Current;
    ULONG               ulColorXor;
    MATH_tdst_Matrix    st_CurGaoInvertMatrix;
} SOFT_tdst_ComputingBuffers;

/*$4
 ***********************************************************************************************************************
    Fog
 ***********************************************************************************************************************
 */

/* Fog mode constants */
#define SOFT_C_FogLinear    0
#define SOFT_C_FogExp       1
#define SOFT_C_FogExp2      2

/* Fog flag constants */
#define SOFT_C_FogActive    0x01
#define SOFT_C_FogChange    0x02
#define SOFT_C_ForceNoFog   0x04

/*
 -----------------------------------------------------------------------------------------------------------------------
    Fog parameters
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  SOFT_tdst_FogParams_
{
    char    c_Mode;
    char    c_Flag;
    float   f_Start;
    float   f_End;
    ULONG   ul_Color;
    float   f_Density;
#ifdef JADEFUSION
    float   f_PitchAttenuationMin;
    float   f_PitchAttenuationMax;
    float   f_PitchAttenuationIntensity;
#endif
} SOFT_tdst_FogParams;


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTSTRUCT_H__ */ 
 