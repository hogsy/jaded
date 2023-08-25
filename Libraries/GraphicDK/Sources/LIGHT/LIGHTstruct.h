/*$T LIGHTstruct.h GC!1.52 11/08/99 10:35:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __LIGHTSTRUCT_H__
#define __LIGHTSTRUCT_H__


#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "BASe/BAStypes.h"
#include "SOFT/SOFTstruct.h"

#if (defined(_XBOX) || defined(_M_X86)) && !defined(_XENON)

#include "xmmintrin.h"

//#define OPT_MULCOLOR	//optimized MMX/SSE LIGHT_ul_MulColor

//#define OPT_ADDCOLOR
//#define OPT_SUBCOLOR

#endif



#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Constants
 ***************************************************************************************************
 */

#define LIGHT_Cul_LF_Omni                               0x00000000
#define LIGHT_Cul_LF_Direct                             0x00000001
#define LIGHT_Cul_LF_Spot                               0x00000002
#define LIGHT_Cul_LF_Fog                                0x00000003
#define LIGHT_Cul_LF_AddMaterial                        0x00000004
//POPoWARNING marasme JADE/JADE360
#define LIGHT_Cul_LF_LightShaft                         0x00000005
#define LIGHT_Cul_LF_NumberOfType                       0x00000006
#define LIGHT_Cul_LF_Type                               0x00000007
#ifdef JADEFUSION
#define LIGHT_Cul_LF_ExtendedLightType                  0x00000007
#endif

#define LIGHT_Cul_LF_Active                             0x00000008
#define LIGHT_Cul_LF_Paint                              0x00000010
#define LIGHT_Cul_LF_UseColor                           0x00000020
#define LIGHT_Cul_LF_UseAlpha                           0x00000040
#define LIGHT_Cul_LF_Absorb                             0x00000080
#define LIGHT_Cul_LF_Inverted                           LIGHT_Cul_LF_Absorb
#define LIGHT_Cul_LF_RealTimeOnDynam                    0x00000100
#define LIGHT_Cul_LF_RealTimeOnNonDynam                 0x00000200
#define LIGHT_Cul_LF_RLIOnDynam                         0x00000400
#define LIGHT_Cul_LF_RLIOnNonDynam                      0x00000800
#define LIGHT_Cul_LF_Specular                           0x00001000
#define LIGHT_Cul_LF_RLICastRay                         0x00002000
#define LIGHT_Cul_LF_EmitRTShadows		                0x00004000
#define LIGHT_Cul_LF_CastShadows		                0x00008000

#define LIGHT_Cul_LF_FogLinear                          0x00010000
#define LIGHT_Cul_LF_FogExp                             0x00020000
#define LIGHT_Cul_LF_FogExp2                            0x00040000

#define LIGHT_Cul_LF_EnablePenombria	                0x00080000

#define LIGHT_Cul_LF_OmniConst                          0x00100000
#define LIGHT_Cul_LF_DontAttenuate						0x00200000
#define LIGHT_Cul_LF_ExclusiveLight						0x00400000

#ifdef JADEFUSION
#define LIGHT_Cul_LF_ExtendedLight                      0x00400000
#define LIGHT_Cul_LF_ExtendedShadowLight                0x00800000
#endif

#define LIGHT_Cul_LF_AddMaterialDontUseNormalMaterial   0x01000000
#define LIGHT_Cul_LF_AddMaterialBVMustBeInside          0x02000000
#define LIGHT_Cul_LF_DoNotSaveObjRef                    0x04000000

#define LIGHT_Cul_LF_SpotIsCylindrical                  0x08000000

#ifdef JADEFUSION
#define LIGHT_Cul_LF_RimLight                           0x10000000
#else
#define LIGHT_Cul_LF_Bump                               0x10000000
#endif

#define LIGHT_Cul_LF_OceanSpecular                      0x20000000

#ifdef JADEFUSION
#define LIGHT_Cul_LF_UseLightMap                        0x40000000
#else
#define LIGHT_Cul_LF_UseShadowBuffer                    0x40000000
#endif

#define LIGHT_Cul_LF_NotReplaceShadowBuffer             0x80000000

#define LIGHT_C_fSpecularExp	        2.f
#define LIGHT_C_OO_fSpecularExp	        0.5f

// Light shaft flags
#define LIGHT_Cul_LightShaft_XXXXXXXXXXXXXXXXXX         0x00000001
#define LIGHT_Cul_LightShaft_EnableNoise                0x00000002
#define LIGHT_Cul_LightShaft_Enable2ndNoiseChannel      0x00000004
#define LIGHT_Cul_LightShaft_DisableFog                 0x00000008

/*$4
 ***************************************************************************************************
    Globals
 ***************************************************************************************************
 */
#ifdef ACTIVE_EDITORS
extern char     *LIGHT_gasz_TypeName[LIGHT_Cul_LF_NumberOfType];
extern LONG    LIGHT_gl_ComputeRLI;
extern ULONG   LIGHT_gul_ComputeRLIMask;
#endif /* ACTIVE_EDITORS */

extern MATH_tdst_Matrix             LIGHT_gst_NIM ONLY_PSX2_ALIGNED(16);
extern MATH_tdst_Matrix             LIGHT_gst_LP;
extern ULONG                        *LIGHT_gpul_CurrentTable;
extern ULONG			            LIGHT_gaul_ExponantTable[256];
extern ULONG			            LIGHT_gaul_LinearTable[256];
extern struct GEO_tdst_Object_      *LIGHT_gpst_CurObject;
extern GEO_Vertex   	            *LIGHT_gpst_CurPointsBuffer;
extern struct LIGHT_tdst_Light_     *LIGHT_gpst_Cur;
extern ULONG			            LIGHT_gul_Color;

#if defined(_XENON_RENDER) && defined(ACTIVE_EDITORS)
extern BOOL LIGHT_gb_DisableXenonSend;
#endif
/*$4
 ***************************************************************************************************
    Structures
 ***************************************************************************************************
 */

typedef struct LIGHT_tdst_SpotFrustum_
{
    MATH_tdst_Vector    m_normal[4];
    float               m_distance[4];
} LIGHT_tdst_SpotFrustum;

typedef struct  LIGHT_tdst_Spot_
{
    float   f_Near;
    float   f_Far;
    float   f_LittleAlpha;
    float   f_BigAlpha;

#ifdef JADEFUSION
    MATH_tdst_Vector        st_LocalBVMin;
    MATH_tdst_Vector        st_LocalBVMax;
    MATH_tdst_Vector        st_GlobalBVCenter;
    MATH_tdst_Matrix        st_Inverse;
    LIGHT_tdst_SpotFrustum  st_FrustumInWorld;
    float                   f_FarRadius;
    BOOL                    b_IsBVValid;
#endif
} LIGHT_tdst_Spot;

typedef struct LIGHT_tdst_LightShaft_
{
    FLOAT   f_Start;
    FLOAT   f_Length;
    FLOAT   f_FOVX;
    FLOAT   f_FOVY;
    BIG_KEY ul_CookieTextureKey;
    BIG_KEY ul_NoiseTextureKey;
    SHORT   us_CookieTexture;
    SHORT   us_NoiseTexture;
    ULONG   ul_Flags;
    FLOAT   f_Noise1ScrollU;
    FLOAT   f_Noise1ScrollV;
    FLOAT   f_Noise2ScrollU;
    FLOAT   f_Noise2ScrollV;
    FLOAT   f_PlaneDensity;
    FLOAT   f_AttenuationStart;
    FLOAT   f_AttenuationFactor;
    FLOAT   f_SpotOuterAngle;
    FLOAT   f_SpotInnerAngle;
    ULONG   ul_Color;
} LIGHT_tdst_LightShaft;

typedef struct  LIGHT_tdst_Direct_
{
    float   f_Near;
    float   f_Far;
} LIGHT_tdst_Direct;

typedef struct  LIGHT_tdst_Omni_
{
    float   f_Near;
    float   f_Far;

} LIGHT_tdst_Omni;

typedef struct  LIGHT_tdst_Fog_
{
    float   f_Start;
    float   f_End;
    float   f_Density;
#ifdef JADEFUSION
	float   f_PitchAttenuationMin;
    float   f_PitchAttenuationMax;
    float   f_PitchAttenuationIntensity;
#endif
	BOOL	b_FogNumber1;
} LIGHT_tdst_Fog;

typedef struct LIGHT_tdst_AddMaterial_
{
    short   w_Id;
    char    c_Dummy2;
    char    c_Dummy;
} LIGHT_tdst_AddMaterial;

typedef struct  LIGHT_tdst_Light_
{
    GRO_tdst_Struct st_Id;
    ULONG   ul_Flags;
    ULONG   ul_Color;
#ifdef JADEFUSION
    ULONG   ul_Version;
    ULONG   ul_Type;
    FLOAT   f_RLIBlendingScale;
    FLOAT   f_RLIBlendingOffset;
    FLOAT   f_DiffuseMultiplier;
    FLOAT   f_SpecularMultiplier;
    FLOAT   f_ActorDiffusePonderator;
    FLOAT   f_ActorSpecularPonderator;

	BOOL	b_LightActor;				// Xenon only (TRUE: light actor/FALSE: don't light actor)
	BOOL	b_LightScene;				// Xenon only (TRUE: light scene/FALSE: don't light scene)

    ULONG   ul_ShadowColor;             // Xenon only (shadow)
    BOOL    b_UseAmbientAsColor;        // Xenon only (shadow)
    FLOAT   f_ShadowNear;               // Xenon only (shadow)
    FLOAT   f_ShadowFar;                // Xenon only (shadow)
    ULONG   ul_NumIterations;           // Xenon only (shadow)
    FLOAT   f_FilterSize;               // Xenon only (shadow)
    FLOAT   f_ZOffset;                  // Xenon only (shadow)
    BIG_KEY ul_CookieTextureKey;        // Xenon only (shadow)
    SHORT   us_CookieTexture;           // Xenon only (shadow)
    FLOAT   f_HiResFOV;                 // Xenon only (shadow)
	BOOL    b_RainEffect;				// Xenon only (dynamic specular)
    BOOL    b_UseHiResFOV;              // Xenon only (shadow)
    BOOL    b_ForceStaticReceiver;      // Xenon only (shadow)
#endif
	union
    {
        LIGHT_tdst_Spot         st_Spot;
        LIGHT_tdst_Omni         st_Omni;
        LIGHT_tdst_Direct       st_Direct;
        LIGHT_tdst_Fog          st_Fog;
        LIGHT_tdst_AddMaterial  st_AddMaterial;
#ifdef JADEFUSION
        LIGHT_tdst_LightShaft   st_LightShaft;
#endif
	};

    /* object lighted by light */
    OBJ_tdst_GameObject *pst_GO;


#ifdef _XBOX

	//CARLONE
	//Tell if color are already filtered for PS2->XBOX
	int colorAlreadyFiltered;

#endif




} LIGHT_tdst_Light;

typedef struct  LIGHT_tdst_List_
{
    ULONG       ul_Max;
    ULONG       ul_Current;
    ULONG		ulContainExclusiveLights;
    OBJ_tdst_GameObject **dpst_Light;
} LIGHT_tdst_List;

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

void                LIGHT_List_Init(LIGHT_tdst_List *_pst_LightList, ULONG _ul_MaxLight);
void                LIGHT_List_Close(LIGHT_tdst_List *_pst_LightList);
void                LIGHT_List_Reset(LIGHT_tdst_List *_pst_LightList);
void                LIGHT_List_AddLight(LIGHT_tdst_List *, OBJ_tdst_GameObject *);
void                LIGHT_List_Sort(LIGHT_tdst_List * _pst_LightList);
LIGHT_tdst_Light    *LIGHT_pst_Create(void);
void                LIGHT_Free(LIGHT_tdst_Light *);

void                LIGHT_SendObjectToLight( LIGHT_tdst_List *, OBJ_tdst_GameObject * );
void                LIGHT_GetLightShaftFrustum(LIGHT_tdst_LightShaft* _pst_LightShaft, MATH_tdst_Vector* _ast_Frustum);
#ifdef _XBOX
int                 LIGHT_SendObjectToLight_HW( LIGHT_tdst_List *, OBJ_tdst_GameObject *, ULONG _ulValidityMask ); 
void                LIGHT_TurnOffObjectLighting( void ); // for DX8 
void				LIGHT_TurnOffAllLights(void); 
void                LIGHT_ConvertLightColor(LIGHT_tdst_Light *pLight);
void				LIGHT_ComputeVertexColorXB(LONG,bool HWLIGHT);
void                 LIGHT_SendObjectToLightXB( LIGHT_tdst_List *, OBJ_tdst_GameObject * );
#endif // _XBOX

#ifdef _XENON_RENDER
void                LIGHT_SendObjectToLightXenon( LIGHT_tdst_List *, OBJ_tdst_GameObject *, ULONG);
#endif


void                LIGHT_ComputeVertexColor(LONG);
void                LIGHT_Init(void);
void                LIGHT_GOSet( LIGHT_tdst_Light    *, OBJ_tdst_GameObject * );

void                LIGHT_SetInterpolTable( int );

ULONG               LIGHT_ul_Interpol2Colors(ULONG, ULONG, float);
ULONG               LIGHT_ul_MulColor(ULONG, float);
#if defined(OPT_ADDCOLOR) 
__forceinline void LIGHT_AddColor(ULONG *pul_Diffuse, ULONG ul_Diffuse)
{
	*pul_Diffuse = _mm_cvtsi64_si32(_mm_adds_pu8 (_mm_cvtsi32_si64(*pul_Diffuse) , _mm_cvtsi32_si64(ul_Diffuse)));
	_mm_empty();
}
#else
void                LIGHT_AddColor(ULONG *, ULONG );
#endif

#if defined(OPT_SUBCOLOR) 
__forceinline void LIGHT_SubColor(ULONG *pul_Diffuse, ULONG ul_Diffuse)
{
	*pul_Diffuse = _mm_cvtsi64_si32(_mm_subs_pu8 (_mm_cvtsi32_si64(*pul_Diffuse) , _mm_cvtsi32_si64(ul_Diffuse)));
	_mm_empty();
}
#else
void                LIGHT_SubColor(ULONG *, ULONG );
#endif
void                LIGHT_ColorMulAdd(ULONG ,ULONG ,ULONG ,ULONG *,ULONG ,ULONG *);
void                LIGHT_ColorAdd(ULONG ,ULONG *,ULONG ,ULONG *);

void                LIGHT_InitLightShaft(LIGHT_tdst_LightShaft* _pst_LightShaft);
void                LIGHT_ResetAllSpotCullingBV( LIGHT_tdst_List *_pst_LightList );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __LIGHTSTRUCT_H__ */