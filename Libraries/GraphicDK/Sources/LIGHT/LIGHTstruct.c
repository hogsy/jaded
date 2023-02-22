/*$T LIGHTstruct.c GC! 1.078 03/14/00 15:43:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifndef PSX2_TARGET
#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "MAD_mem/Sources/MAD_mem.h"
#endif
#endif
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/INTersection/INTmain.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GEOmetric/GEODebugObject.h"

#ifdef JADEFUSION
#include "GDInterface/GDIrequest.h"
#include <algorithm>
#endif

#include "LIGHT/LIGHTstruct.h"
#include "GRObject/GROstruct.h"
#include "SOFT/SOFTlinear.h"

#ifdef ACTIVE_EDITORS
#include "../Main/WinEditors/Sources/EDIpaths.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeLightShaftManager.h"
#include "XenonGraphics/XeShadowManager.h"
#endif

#ifdef _XBOX
#include "GX8/Gx8Light.h"
#include "GX8/Gx8color.h"
#endif

#include "BASe/BENch/BENch.h"

#ifdef GSP_PS2_BENCH
u_int	n_Parralleles;
u_int	n_Omni;
u_int	n_Spot;
u_int	n_LightCalls;
#endif

#ifdef JADEFUSION
// Current light version
#define LIGHT_VERSION            11

// Current light shaft version
#define LIGHT_LIGHTSHAFT_VERSION 7

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */
inline BOOL LIGHT_b_IsOfType(LIGHT_tdst_Light* _pst_Light, ULONG _ul_Type)
{
    return ((_pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == _ul_Type);
}

#if defined(_XENON_RENDER)

#if defined(ACTIVE_EDITORS)
void LIGHT_RenderLightShaft_Editor(OBJ_tdst_GameObject* _pst_GO);
#endif

void LIGHT_RenderXenon(OBJ_tdst_GameObject* _pst_GO);

#endif
#endif
/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
char					*LIGHT_gasz_TypeName[LIGHT_Cul_LF_NumberOfType] = { "Omni", "Direct", "Spot", "Fog", "Add Material", "Light Shaft" };
LONG					LIGHT_gl_ComputeRLI = 0;
ULONG                   LIGHT_gul_ComputeRLIMask = 0xFFFFFFFF;

#endif /* ACTIVE_EDITORS */

LONG					LIGHT_gl_DebugObject[LIGHT_Cul_LF_NumberOfType] =
{
	GEO_DebugObject_LightOmni,
	GEO_DebugObject_Arrow,
	GEO_DebugObject_LightSpot,
	GEO_DebugObject_LightOmni,
    GEO_DebugObject_LightOmni,
    GEO_DebugObject_LightSpot
};


ULONG			        LIGHT_gaul_ExponantTable[256];
ULONG			        LIGHT_gaul_LinearTable[256];
ULONG			        *LIGHT_gpul_CurrentTable = NULL;
MATH_tdst_Matrix        LIGHT_gst_NIM ONLY_PSX2_ALIGNED(16);
MATH_tdst_Matrix        LIGHT_gst_LP;
ULONG			        LIGHT_gul_Color;
LIGHT_tdst_Light        *LIGHT_gpst_Cur;
GEO_tdst_Object	        *LIGHT_gpst_CurObject;
GEO_Vertex   	        *LIGHT_gpst_CurPointsBuffer;


/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_List_Init(LIGHT_tdst_List *_pst_LightList, ULONG _ul_MaxLight)
{
	if(_pst_LightList == NULL) return;

	_pst_LightList->dpst_Light = (OBJ_tdst_GameObject **) MEM_p_Alloc(_ul_MaxLight * sizeof(OBJ_tdst_GameObject *));
	_pst_LightList->ul_Current = 0;
	_pst_LightList->ul_Max = _ul_MaxLight;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_List_Close(LIGHT_tdst_List *_pst_LightList)
{
	if(_pst_LightList == NULL) return;

	if(_pst_LightList->dpst_Light) MEM_Free(_pst_LightList->dpst_Light);

	L_memset(_pst_LightList, 0, sizeof(LIGHT_tdst_List));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_List_Reset(LIGHT_tdst_List *_pst_LightList)
{
	_pst_LightList->ulContainExclusiveLights = 0;
	_pst_LightList->ul_Current = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_List_AddLight(LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_LightNode)
{
	_pst_LightList->dpst_Light[_pst_LightList->ul_Current++] = _pst_LightNode;
	if (_pst_LightList->ul_Current >= _pst_LightList->ul_Max) 
	{
		LINK_PrintStatusMsg( "Too many light (more than 256)" );
		_pst_LightList->ul_Current = 0;
	}
}
#ifdef JADEFUSION
void LIGHT_List_Sort(LIGHT_tdst_List *_pst_LightList )
{
    std::sort( _pst_LightList->dpst_Light, _pst_LightList->dpst_Light + _pst_LightList->ul_Current );
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
LIGHT_tdst_Light *LIGHT_pst_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light	*pst_Light;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Light = (LIGHT_tdst_Light *) MEM_p_Alloc(sizeof(LIGHT_tdst_Light));
	L_memset(pst_Light, 0, sizeof(LIGHT_tdst_Light));
	pst_Light->ul_Flags = LIGHT_Cul_LF_Active;
	pst_Light->ul_Flags |= LIGHT_Cul_LF_UseColor;
	pst_Light->ul_Flags |= LIGHT_Cul_LF_RealTimeOnDynam;
    pst_Light->ul_Flags |= LIGHT_Cul_LF_RLIOnNonDynam;
    pst_Light->ul_Flags |= LIGHT_Cul_LF_RLICastRay;
	pst_Light->ul_Color = 0xFFFFFF;
	pst_Light->st_Omni.f_Near = 2.0f;
	pst_Light->st_Omni.f_Far = 5.0f;
	pst_Light->st_Spot.f_LittleAlpha = .1f;
	pst_Light->st_Spot.f_BigAlpha = .2f;
#ifdef JADEFUSION
    pst_Light->st_Fog.f_PitchAttenuationMax = 90.0f;
    pst_Light->st_Fog.f_PitchAttenuationIntensity = 0.0f;
    pst_Light->ul_Version = LIGHT_VERSION;
    pst_Light->ul_Type = 0;
    pst_Light->f_RLIBlendingScale  = 0.0f;
    pst_Light->f_RLIBlendingOffset = 0.0f;
    pst_Light->f_DiffuseMultiplier  = 1.0f;
    pst_Light->f_SpecularMultiplier = 1.0f;
    pst_Light->f_ActorDiffusePonderator  = 1.0f;
    pst_Light->f_ActorSpecularPonderator = 1.0f;

	// LRL
	pst_Light->b_LightActor = TRUE;
	pst_Light->b_LightScene = TRUE;

    // Xenon shadow
    pst_Light->ul_ShadowColor = 0;
    pst_Light->b_UseAmbientAsColor = TRUE;
    pst_Light->f_ShadowNear = 1.0f;
    pst_Light->f_ShadowFar = 20.0f;
    pst_Light->ul_NumIterations = 4;
    pst_Light->f_FilterSize = 8.0f;
    pst_Light->f_ZOffset = 0.01f;
    pst_Light->ul_CookieTextureKey = BIG_C_InvalidKey;
    pst_Light->us_CookieTexture = -1;
    pst_Light->f_HiResFOV = 0.4f;
	pst_Light->b_RainEffect = TRUE;

    pst_Light->b_UseHiResFOV = FALSE;
    pst_Light->b_ForceStaticReceiver = TRUE;
#endif
	GRO_Struct_Init(&pst_Light->st_Id, GRO_Light);
	return pst_Light;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_Free(LIGHT_tdst_Light *_pst_Light)
{
	if(_pst_Light == NULL) return;
	if(_pst_Light->st_Id.i->ul_Type != GRO_Light) return;
	if(_pst_Light->st_Id.l_Ref > 0) return;

#if defined(_XENON_RENDER)
    if (LIGHT_b_IsOfType(_pst_Light, LIGHT_Cul_LF_LightShaft))
    {
        g_oXeLightShaftManager.UnregisterLightShaft(_pst_Light);
    }

    g_oXeShadowManager.UnRegisterCookieTexture(_pst_Light);
#endif
	GRO_Struct_Free(&_pst_Light->st_Id);
	LOA_DeleteAddress(_pst_Light);
	MEM_Free(_pst_Light);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_SetInterpolTable( int i )
{
    if (i)
        LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable; 
    else
        LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;
}




/*
 =======================================================================================================================
    Aim:    Load a light
 =======================================================================================================================
 */
void *LIGHT_p_CreateFromBuffer(GRO_tdst_Struct *_pst_Id, char **ppc_Buffer, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light	*pst_Light;
	BIG_KEY				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Light = LIGHT_pst_Create();

	pst_Light->ul_Flags = LOA_ReadULong(ppc_Buffer);
	pst_Light->ul_Color = LOA_ReadULong(ppc_Buffer);

#ifdef JADEFUSION
    if (LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_ExtendedLightType))
    {
        pst_Light->ul_Version = LOA_ReadULong(ppc_Buffer);
        pst_Light->ul_Type    = LOA_ReadULong(ppc_Buffer);
        pst_Light->ul_Flags   &= ~LIGHT_Cul_LF_Type;
        pst_Light->ul_Flags   |= pst_Light->ul_Type & LIGHT_Cul_LF_Type;
        
        if (pst_Light->ul_Version > 0)
        {
            pst_Light->f_RLIBlendingScale = LOA_ReadFloat(ppc_Buffer);
            pst_Light->f_RLIBlendingOffset = LOA_ReadFloat(ppc_Buffer);
        }

        if (pst_Light->ul_Version > 1)
        {
            pst_Light->f_DiffuseMultiplier = LOA_ReadFloat(ppc_Buffer);
            pst_Light->f_SpecularMultiplier = LOA_ReadFloat(ppc_Buffer);
#if defined(_XENON)
			// Clamp light diffuse multiplier above zero (to avoid divison by zero, as shader constants are not checking on Xenon)
            pst_Light->f_DiffuseMultiplier = (pst_Light->f_DiffuseMultiplier > 0.0f) ? pst_Light->f_DiffuseMultiplier : 0.0001f;
#endif
        }

        if (pst_Light->ul_Version > 2)
        {
            // Load Xenon shadow params
            pst_Light->ul_NumIterations = LOA_ReadULong(ppc_Buffer);
            pst_Light->f_FilterSize = LOA_ReadFloat(ppc_Buffer);
        }
        if (pst_Light->ul_Version > 3)
        {
            pst_Light->f_ZOffset = LOA_ReadFloat(ppc_Buffer);
            pst_Light->ul_CookieTextureKey = LOA_ReadULong(ppc_Buffer);

            // Load the cookie texture
            if (pst_Light->ul_CookieTextureKey != BIG_C_InvalidKey)
            {
                pst_Light->us_CookieTexture = TEX_w_List_AddTexture(&TEX_gst_GlobalList, 
                                                                    pst_Light->ul_CookieTextureKey,
                                                                    1);
#if defined(_XENON_RENDER)
                g_oXeShadowManager.RegisterCookieTexture(pst_Light);
#endif
            }
        }

        if (pst_Light->ul_Version > 4)
        {
            pst_Light->ul_ShadowColor = LOA_ReadULong(ppc_Buffer);
            pst_Light->b_UseAmbientAsColor = LOA_ReadULong(ppc_Buffer);
            pst_Light->f_ShadowNear = LOA_ReadFloat(ppc_Buffer);
            pst_Light->f_ShadowFar = LOA_ReadFloat(ppc_Buffer);
        }

        if (pst_Light->ul_Version > 5)
        {
            pst_Light->f_HiResFOV = LOA_ReadFloat(ppc_Buffer);
        }

        if (pst_Light->ul_Version > 6)
        {
            pst_Light->b_UseHiResFOV = LOA_ReadULong(ppc_Buffer);
            pst_Light->b_ForceStaticReceiver = LOA_ReadULong(ppc_Buffer);
        }

		if (pst_Light->ul_Version > 7)
		{
			pst_Light->b_RainEffect = LOA_ReadULong(ppc_Buffer);
		}

        if( pst_Light->ul_Version > 8 )
        {   
            pst_Light->st_Fog.f_PitchAttenuationMax = LOA_ReadFloat(ppc_Buffer);
            pst_Light->st_Fog.f_PitchAttenuationIntensity = LOA_ReadFloat(ppc_Buffer);
        }

        if( pst_Light->ul_Version > 9 )
        {   
            pst_Light->f_ActorDiffusePonderator = LOA_ReadFloat(ppc_Buffer);
            pst_Light->f_ActorSpecularPonderator = LOA_ReadFloat(ppc_Buffer);
        }

		if( pst_Light->ul_Version > 10 )
		{   
			pst_Light->b_LightActor = LOA_ReadUChar(ppc_Buffer);
			pst_Light->b_LightScene = LOA_ReadUChar(ppc_Buffer);
		}
	}
#endif

	pst_Light->st_Spot.f_Near = LOA_ReadFloat(ppc_Buffer);
	pst_Light->st_Spot.f_Far = LOA_ReadFloat(ppc_Buffer);
	pst_Light->st_Spot.f_LittleAlpha = LOA_ReadFloat(ppc_Buffer);
	pst_Light->st_Spot.f_BigAlpha = LOA_ReadFloat(ppc_Buffer);

#ifdef JADEFUSION
//popowarning
    if(pst_Light->ul_Version < 6)
    {
        // Init dynamic caster FOV to current FOV for old lights
        pst_Light->f_HiResFOV = pst_Light->st_Spot.f_BigAlpha;
    }
#endif
	/* Load group of excluded objects */
	ul_Key = LOA_ReadULong(ppc_Buffer);

    /* Load the group if needed */
	if(ul_Key != BIG_C_InvalidKey)
		LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Light->pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	else
		pst_Light->pst_GO = NULL;

#ifdef JADEFUSION
    // Light shaft properties
    if (LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_LightShaft))
    {
        ULONG ulVersion = LOA_ReadULong(ppc_Buffer);

        // Reset everything before we read the properties
        LIGHT_InitLightShaft(&pst_Light->st_LightShaft);

        if (ulVersion >= 1)
        {
            pst_Light->st_LightShaft.f_Start  = LOA_ReadFloat(ppc_Buffer);
            pst_Light->st_LightShaft.f_Length = LOA_ReadFloat(ppc_Buffer);
            pst_Light->st_LightShaft.f_FOVX   = LOA_ReadFloat(ppc_Buffer);
            pst_Light->st_LightShaft.f_FOVY   = LOA_ReadFloat(ppc_Buffer);

            if (ulVersion >= 2)
            {
                pst_Light->st_LightShaft.ul_CookieTextureKey = LOA_ReadULong(ppc_Buffer);
                pst_Light->st_LightShaft.ul_NoiseTextureKey  = LOA_ReadULong(ppc_Buffer);
            }

            if (ulVersion >= 3)
            {
                pst_Light->st_LightShaft.ul_Flags = LOA_ReadULong(ppc_Buffer);
            }

            if (ulVersion >= 4)
            {
                pst_Light->st_LightShaft.f_Noise1ScrollU = LOA_ReadFloat(ppc_Buffer);
                pst_Light->st_LightShaft.f_Noise1ScrollV = LOA_ReadFloat(ppc_Buffer);
                pst_Light->st_LightShaft.f_Noise2ScrollU = LOA_ReadFloat(ppc_Buffer);
                pst_Light->st_LightShaft.f_Noise2ScrollV = LOA_ReadFloat(ppc_Buffer);

                pst_Light->st_LightShaft.f_PlaneDensity = LOA_ReadFloat(ppc_Buffer);
            }

            if (ulVersion >= 5)
            {
                pst_Light->st_LightShaft.f_AttenuationStart  = LOA_ReadFloat(ppc_Buffer);
                pst_Light->st_LightShaft.f_AttenuationFactor = LOA_ReadFloat(ppc_Buffer);
            }

            if (ulVersion >= 6)
            {
                pst_Light->st_LightShaft.f_SpotInnerAngle = LOA_ReadFloat(ppc_Buffer);
                pst_Light->st_LightShaft.f_SpotOuterAngle = LOA_ReadFloat(ppc_Buffer);
            }

            if (ulVersion >= 7)
            {
                pst_Light->st_LightShaft.ul_Color = LOA_ReadULong(ppc_Buffer);
            }
            else
            {
                pst_Light->st_LightShaft.ul_Color = pst_Light->ul_Color;
            }
        }

        // Load the beam texture
        if (pst_Light->st_LightShaft.ul_CookieTextureKey != BIG_C_InvalidKey)
        {
            pst_Light->st_LightShaft.us_CookieTexture = TEX_w_List_AddTexture(&TEX_gst_GlobalList, 
                                                                              pst_Light->st_LightShaft.ul_CookieTextureKey, 1);
        }

        // Load the mask texture
        if (pst_Light->st_LightShaft.ul_NoiseTextureKey != BIG_C_InvalidKey)
        {
            pst_Light->st_LightShaft.us_NoiseTexture = TEX_w_List_AddTexture(&TEX_gst_GlobalList, 
                                                                             pst_Light->st_LightShaft.ul_NoiseTextureKey, 1);
        }

#if defined(_XENON_RENDER)
        g_oXeLightShaftManager.RegisterLightShaft(pst_Light);
#endif
    }
#endif //JADEFUSION

	return pst_Light;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *LIGHT_p_Duplicate(LIGHT_tdst_Light *_pst_Light, char *_asz_Path, char *_sz_Name, ULONG _ul_Flags)
{
#ifdef ACTIVE_EDITORS

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light	*pst_NewLight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_NewLight = LIGHT_pst_Create();

	pst_NewLight->ul_Flags = _pst_Light->ul_Flags;
	pst_NewLight->ul_Color = _pst_Light->ul_Color;
	pst_NewLight->st_Spot.f_Near = _pst_Light->st_Spot.f_Near;
	pst_NewLight->st_Spot.f_Far = _pst_Light->st_Spot.f_Far;
	pst_NewLight->st_Spot.f_LittleAlpha = _pst_Light->st_Spot.f_LittleAlpha;
	pst_NewLight->st_Spot.f_BigAlpha = _pst_Light->st_Spot.f_BigAlpha;
#ifdef JADEFUSION
    pst_NewLight->st_Fog.f_PitchAttenuationMax = _pst_Light->st_Fog.f_PitchAttenuationMax;
    pst_NewLight->st_Fog.f_PitchAttenuationIntensity = _pst_Light->st_Fog.f_PitchAttenuationIntensity;
#endif

	pst_NewLight->pst_GO = _pst_Light->pst_GO;

#ifdef JADEFUSION
    // Duplicate extended properties
    pst_NewLight->ul_Version = _pst_Light->ul_Version;
    pst_NewLight->ul_Type = _pst_Light->ul_Type;
    pst_NewLight->f_RLIBlendingScale = _pst_Light->f_RLIBlendingScale;
    pst_NewLight->f_RLIBlendingOffset = _pst_Light->f_RLIBlendingOffset;
    pst_NewLight->f_DiffuseMultiplier = _pst_Light->f_DiffuseMultiplier;
    pst_NewLight->f_SpecularMultiplier = _pst_Light->f_SpecularMultiplier;
    pst_NewLight->f_ActorDiffusePonderator = _pst_Light->f_ActorDiffusePonderator;
    pst_NewLight->f_ActorSpecularPonderator = _pst_Light->f_ActorSpecularPonderator;

	// Duplicate LRL 
	pst_NewLight->b_LightActor = _pst_Light->b_LightActor;
	pst_NewLight->b_LightScene = _pst_Light->b_LightScene;

    // Duplicate the light shaft properties
    if (LIGHT_b_IsOfType(pst_NewLight, LIGHT_Cul_LF_LightShaft))
    {
        L_memcpy(&pst_NewLight->st_LightShaft, &_pst_Light->st_LightShaft, sizeof(LIGHT_tdst_LightShaft));
    }

    // Duplicate the shadow properties
    pst_NewLight->ul_ShadowColor = _pst_Light->ul_ShadowColor;
    pst_NewLight->b_UseAmbientAsColor = _pst_Light->b_UseAmbientAsColor;
    pst_NewLight->f_ShadowNear = _pst_Light->f_ShadowNear;
    pst_NewLight->f_ShadowFar = _pst_Light->f_ShadowFar;
    pst_NewLight->ul_NumIterations = _pst_Light->ul_NumIterations;
    pst_NewLight->f_FilterSize = _pst_Light->f_FilterSize;
    pst_NewLight->f_ZOffset = _pst_Light->f_ZOffset;
    pst_NewLight->ul_CookieTextureKey = _pst_Light->ul_CookieTextureKey;
    pst_NewLight->us_CookieTexture = _pst_Light->us_CookieTexture;
    pst_NewLight->f_HiResFOV = _pst_Light->f_HiResFOV;
    pst_NewLight->b_UseHiResFOV = _pst_Light->b_UseHiResFOV;
    pst_NewLight->b_ForceStaticReceiver = _pst_Light->b_ForceStaticReceiver;

    // Other properties
	pst_NewLight->b_RainEffect = _pst_Light->b_RainEffect;
#endif

	GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_NewLight, _asz_Path, NULL, NULL);

	return pst_NewLight;
#else
	return NULL;
#endif
}

/*
 =======================================================================================================================
    Aim:    Save a light
 =======================================================================================================================
 */
LONG LIGHT_l_SaveInBuffer(LIGHT_tdst_Light *_pst_Light, void *p_Unused)
{
#ifdef ACTIVE_EDITORS

	/*~~~~~~~~~~~*/
	BIG_KEY ul_Key;
#ifdef JADEFUSION
    ULONG ul_Flags, ul_Type, ul_Version = LIGHT_VERSION;
#endif
	/*~~~~~~~~~~~*/

	GRO_Struct_Save(&_pst_Light->st_Id);
#ifdef JADEFUSION
    ul_Type  = _pst_Light->ul_Flags & LIGHT_Cul_LF_Type;
    ul_Flags = _pst_Light->ul_Flags & ~LIGHT_Cul_LF_Type;
    ul_Flags |= LIGHT_Cul_LF_ExtendedLightType;
#endif

#ifdef JADEFUSION
	SAV_Buffer(&ul_Flags, 4);
#else
	SAV_Buffer(&_pst_Light->ul_Flags, 4);
#endif
	SAV_Buffer(&_pst_Light->ul_Color, 4);
#ifdef JADEFUSION
    SAV_Buffer(&ul_Version,                       sizeof(ULONG));
    SAV_Buffer(&ul_Type,                          sizeof(ULONG));
    SAV_Buffer(&_pst_Light->f_RLIBlendingScale,   sizeof(float));
    SAV_Buffer(&_pst_Light->f_RLIBlendingOffset,  sizeof(float));
    SAV_Buffer(&_pst_Light->f_DiffuseMultiplier,  sizeof(float));
    SAV_Buffer(&_pst_Light->f_SpecularMultiplier, sizeof(float));

    SAV_Buffer(&_pst_Light->ul_NumIterations,     sizeof(ULONG));
    SAV_Buffer(&_pst_Light->f_FilterSize,         sizeof(float));
    SAV_Buffer(&_pst_Light->f_ZOffset,            sizeof(float));
    SAV_Buffer(&_pst_Light->ul_CookieTextureKey,  sizeof(ULONG));
    SAV_Buffer(&_pst_Light->ul_ShadowColor,       sizeof(ULONG));
    SAV_Buffer(&_pst_Light->b_UseAmbientAsColor,  sizeof(ULONG));
    SAV_Buffer(&_pst_Light->f_ShadowNear,         sizeof(float));
    SAV_Buffer(&_pst_Light->f_ShadowFar,          sizeof(float));
    SAV_Buffer(&_pst_Light->f_HiResFOV,           sizeof(float));
    SAV_Buffer(&_pst_Light->b_UseHiResFOV,        sizeof(float));
    SAV_Buffer(&_pst_Light->b_ForceStaticReceiver,sizeof(float));
	SAV_Buffer(&_pst_Light->b_RainEffect,		  sizeof(ULONG));

    SAV_Buffer(&_pst_Light->st_Fog.f_PitchAttenuationMax, 4);
    SAV_Buffer(&_pst_Light->st_Fog.f_PitchAttenuationIntensity, 4);

    SAV_Buffer(&_pst_Light->f_ActorDiffusePonderator,  sizeof(float));
    SAV_Buffer(&_pst_Light->f_ActorSpecularPonderator, sizeof(float));

	SAV_Buffer(&_pst_Light->b_LightActor, sizeof(UCHAR));
	SAV_Buffer(&_pst_Light->b_LightScene, sizeof(UCHAR));
#endif
	SAV_Buffer(&_pst_Light->st_Spot.f_Near, 4);
	SAV_Buffer(&_pst_Light->st_Spot.f_Far, 4);
	SAV_Buffer(&_pst_Light->st_Spot.f_LittleAlpha, 4);
	SAV_Buffer(&_pst_Light->st_Spot.f_BigAlpha, 4);

	/* Save group */
    if ( !(_pst_Light->ul_Flags & LIGHT_Cul_LF_DoNotSaveObjRef) && (_pst_Light->pst_GO) )
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Light->pst_GO);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, sizeof(BIG_KEY));
#ifdef JADEFUSION
    // Light shaft properties
    if (LIGHT_b_IsOfType(_pst_Light, LIGHT_Cul_LF_LightShaft))
    {
        ULONG ulVersion = LIGHT_LIGHTSHAFT_VERSION;

        SAV_Buffer(&ulVersion,                                      sizeof(ULONG));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_Start,              sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_Length,             sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_FOVX,               sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_FOVY,               sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.ul_CookieTextureKey,  sizeof(ULONG));
        SAV_Buffer(&_pst_Light->st_LightShaft.ul_NoiseTextureKey,   sizeof(ULONG));
        SAV_Buffer(&_pst_Light->st_LightShaft.ul_Flags,             sizeof(ULONG));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_Noise1ScrollU,      sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_Noise1ScrollV,      sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_Noise2ScrollU,      sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_Noise2ScrollV,      sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_PlaneDensity,       sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_AttenuationStart,   sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_AttenuationFactor,  sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_SpotInnerAngle,     sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.f_SpotOuterAngle,     sizeof(FLOAT));
        SAV_Buffer(&_pst_Light->st_LightShaft.ul_Color,             sizeof(ULONG));
    }

#endif
#endif
	return 0;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *LIGHT_p_CreateFromMad(void *p_MadLight)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light	*pst_Light;
	MAD_Light			*pst_MadLight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MadLight = (MAD_Light *) p_MadLight;

	pst_Light = LIGHT_pst_Create();
	GRO_Struct_SetName(&pst_Light->st_Id, pst_MadLight->ID.Name);

	switch(pst_MadLight->LightType)
	{
	case MAD_LightOmni:
		pst_Light->ul_Flags &= ~LIGHT_Cul_LF_Type;
		pst_Light->ul_Flags |= LIGHT_Cul_LF_Omni;
		pst_Light->st_Omni.f_Near = pst_MadLight->Omni.Near;
		pst_Light->st_Omni.f_Far = pst_MadLight->Omni.Far;
		break;
	case MAD_LightSpot:
		pst_Light->ul_Flags &= ~LIGHT_Cul_LF_Type;
		pst_Light->ul_Flags |= LIGHT_Cul_LF_Spot;
		pst_Light->st_Spot.f_LittleAlpha = pst_MadLight->Spot.LittleAlpha;
		pst_Light->st_Spot.f_BigAlpha = pst_MadLight->Spot.BigAlpha;
		pst_Light->st_Spot.f_Near = pst_MadLight->Spot.Near;
		pst_Light->st_Spot.f_Far = pst_MadLight->Spot.Far;
		break;
	case MAD_LightDirect:
		pst_Light->ul_Flags &= ~LIGHT_Cul_LF_Type;
		pst_Light->ul_Flags |= LIGHT_Cul_LF_Direct;
		pst_Light->st_Direct.f_Near = pst_MadLight->Direct.Near;
		pst_Light->st_Direct.f_Far = pst_MadLight->Direct.Far;
		break;
	}

	pst_Light->ul_Color = pst_MadLight->LightColor & 0xFFFFFF;
	return pst_Light;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAD_Light *LIGHT_p_ToMad(LIGHT_tdst_Light *_pst_Light, WOR_tdst_World *_pst_World)
{
#ifdef ACTIVE_EDITORS

	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_Light	*pst_MadLight;
	//char		*psz_Ext;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MAD_MALLOC(MAD_Light, pst_MadLight, 1);

	pst_MadLight->ID.IDType = ID_MAD_Light;
	pst_MadLight->ID.SizeOfThisOne = sizeof(MAD_Light);

	//if((psz_Ext = strrchr(_pst_Light->st_Id.sz_Name, '.'))) *psz_Ext = 0;
	strcpy(pst_MadLight->ID.Name, GRO_sz_Struct_GetName( &_pst_Light->st_Id) );
	//if(psz_Ext) *psz_Ext = '.';

	if((_pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni)
	{
		pst_MadLight->LightType = MAD_LightOmni;
		pst_MadLight->Omni.Near = _pst_Light->st_Omni.f_Near;
		pst_MadLight->Omni.Far = _pst_Light->st_Omni.f_Far;
		pst_MadLight->Omni.RadiusOfLight = 0.0f;
	}
	else if((_pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Spot)
	{
		pst_MadLight->LightType = MAD_LightSpot;
		pst_MadLight->Spot.Near = _pst_Light->st_Spot.f_Near;
		pst_MadLight->Spot.Far = _pst_Light->st_Spot.f_Far;
		pst_MadLight->Spot.LittleAlpha = _pst_Light->st_Spot.f_LittleAlpha;
		pst_MadLight->Spot.BigAlpha = _pst_Light->st_Spot.f_BigAlpha;
		pst_MadLight->Spot.RadiusOfLight = 0.0f;
	}
	else if((_pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
	{
		pst_MadLight->LightType = MAD_LightDirect;
		pst_MadLight->Direct.Near = _pst_Light->st_Direct.f_Near;
		pst_MadLight->Direct.Far = _pst_Light->st_Direct.f_Far;
		pst_MadLight->Direct.RadiusOfLight = 0.0f;
	}

	pst_MadLight->LightColor = _pst_Light->ul_Color;
	pst_MadLight->IsOn = (_pst_Light->ul_Flags & LIGHT_Cul_LF_Active) ? 1 : 0;
	pst_MadLight->LightFlags = 0;
	pst_MadLight->Multiplier = 1.0f;
	return pst_MadLight;
#endif
	return NULL;
}

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG LIGHT_l_HasSomethingToRender
(
	GRO_tdst_Visu			*_pst_Visu,
	GEO_tdst_Object			**ppst_PickableObject
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light	*pst_Light;
#ifdef ACTIVE_EDITORS
	LONG				l_Object;
#endif
#ifdef JADEFUSION
    BOOL b_IsLightShaft = FALSE;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Light = (LIGHT_tdst_Light *) _pst_Visu->pst_Object;
#ifdef ACTIVE_EDITORS
	l_Object = LIGHT_gl_DebugObject[pst_Light->ul_Flags & LIGHT_Cul_LF_Type];
	*ppst_PickableObject = GEO_pst_DebugObject_Get(l_Object);
#endif /* ACTIVE_EDITORS */
#ifdef JADEFUSION
    // Light shafts need to be rendered
    if (LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_LightShaft))
        b_IsLightShaft = TRUE;

	return b_IsLightShaft | ((GDI_gpst_CurDD_SPR.ul_DisplayFlags & GDI_Cul_DF_DisplayLight) != 0);
#else
	return GDI_gpst_CurDD_SPR.ul_DisplayFlags & GDI_Cul_DF_DisplayLight;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_Render(OBJ_tdst_GameObject *_pst_Node)
{
#ifdef ACTIVE_EDITORS

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light	*pst_Light;
	ULONG				C, DM;
	LONG				l_Object;
	MATH_tdst_Matrix	*M;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_Node->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)) return;

	pst_Light = (LIGHT_tdst_Light *) _pst_Node->pst_Extended->pst_Light;
	if(!pst_Light) return;

	l_Object = LIGHT_gl_DebugObject[pst_Light->ul_Flags & LIGHT_Cul_LF_Type];

  #ifdef _XENON_RENDER
    if (pst_Light->ul_Flags & LIGHT_Cul_LF_ExtendedLight)
    {
        if (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected)
        {
            C = 0xFF00FFFF;
        }
        else if (pst_Light->ul_Flags & LIGHT_Cul_LF_ExtendedShadowLight)
        {
            C = 0xFFFFFFFF;
        }
        else if (LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_Direct))
        {
            C = 0xFF0000FF;
        }
        else if (LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_Spot))
        {
            C = 0xFF808000;
        }
        else if (LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_LightShaft))
        {
            C = 0xFF4080FF;
        }
        else
        {
            C = 0xFFFF00FF;
        }
        
        GEO_DebugObject_SetLight(C);
    }
    else
  #endif
	{
		C = (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) ? 1 : 0;
		C = GEO_ul_DebugObject_GetColor(l_Object, C);
		GEO_DebugObject_SetLight(C);
	}
#ifdef JADEFUSION
	if((GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayLight))
#endif 
	{

		M = OBJ_pst_GetAbsoluteMatrix(_pst_Node);
		DM = GDI_Cul_DM_All - GDI_Cul_DM_UseAmbient - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor;
		GEO_DebugObject_Draw(&GDI_gpst_CurDD_SPR, l_Object, DM, _pst_Node->ul_EditorFlags, M);
	}
#endif //Editor

#if defined(_XENON_RENDER)

#if defined(ACTIVE_EDITORS)
    if (LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_LightShaft) && 
        (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) &&
        (GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayLight))

    {
        LIGHT_RenderLightShaft_Editor(_pst_Node);
    }

    if (GDI_b_IsXenonGraphics())
#endif
    {
        LIGHT_RenderXenon(_pst_Node);
    }

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
char *LIGHT_sz_FileExtension(void)
{
	return EDI_Csz_ExtGraphicLight;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Interface	*i;
	/*~~~~~~~~~~~~~~~~~~~*/

	/* Init light object interface */
	i = &GRO_gast_Interface[GRO_Light];
	i->pfnp_CreateDefault = (void *(*) (void)) LIGHT_pst_Create;
#if defined (JADEFUSION)
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct_ *,char ** ,void *))LIGHT_p_CreateFromBuffer;
    i->pfnp_Duplicate = (void *(__cdecl *)(void *,char *, char*, ULONG))LIGHT_p_Duplicate;
	i->pfn_Destroy = (void (__cdecl *)(void *))LIGHT_Free;
	i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))LIGHT_l_HasSomethingToRender;
#else
	i->pfnp_CreateFromBuffer = LIGHT_p_CreateFromBuffer;
    i->pfnp_Duplicate = LIGHT_p_Duplicate;
	i->pfn_Destroy = LIGHT_Free;
	i->pfnl_HasSomethingToRender = LIGHT_l_HasSomethingToRender;
#endif
#ifdef ACTIVE_EDITORS
#if defined (JADEFUSION)
	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))LIGHT_l_SaveInBuffer;
	i->pfnp_CreateFromMad = LIGHT_p_CreateFromMad;
	i->pfnp_ToMad = (void* (__cdecl *)(void *,void *))LIGHT_p_ToMad;
    i->pfnsz_FileExtension = LIGHT_sz_FileExtension;
	i->pfnl_PushSpecialMatrix = (LONG (__cdecl *)(void *))GRO_PushSpecialMatrixForProportionnal;
    i->pfn_Render = (void (__cdecl *)(void *))LIGHT_Render;
#else
	i->pfnl_SaveInBuffer = LIGHT_l_SaveInBuffer;
	i->pfnp_CreateFromMad = LIGHT_p_CreateFromMad;
	i->pfnp_ToMad = LIGHT_p_ToMad;
    i->pfnsz_FileExtension = LIGHT_sz_FileExtension;
	i->pfnl_PushSpecialMatrix = GRO_PushSpecialMatrixForProportionnal;
    i->pfn_Render = LIGHT_Render;
#endif
#endif

#if defined(_XENON) || (defined(_XENON_RENDER) && !defined(ACTIVE_EDITORS))
//    i->pfn_Render = LIGHT_RenderXenon;
    i->pfn_Render = (void (__cdecl*)(void*))LIGHT_RenderXenon;
#endif
	if(LIGHT_gpul_CurrentTable == NULL)
	{
		/*~~~~~~~~~~~~~~~~~~~*/
		ULONG	Counter, Local;
		/*~~~~~~~~~~~~~~~~~~~*/

		for(Counter = 0; Counter < 256; Counter++)
		{
			LIGHT_gaul_LinearTable[Counter] = Counter;
			Local = Counter;
			Local = (Local * Local) >> 8;
			Local = (Local * Local) >> 8;
			Local = (Local * Local) >> 8;
			LIGHT_gaul_ExponantTable[Counter] = Local;
		}
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_GOSet(LIGHT_tdst_Light *_pst_Light, OBJ_tdst_GameObject *_pst_GO)
{
	_pst_Light->pst_GO = _pst_GO;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

#ifdef _XBOX

//CARLONE 

void LIGHT_ConvertLightColor(LIGHT_tdst_Light *pLight)
{

	if(pLight->colorAlreadyFiltered!=333)
	{
		pLight->colorAlreadyFiltered=333;
		pLight->ul_Color=Gx8_ConvertChrominancePixel(Gx8_M_ConvertColor(pLight->ul_Color)); 
		pLight->ul_Color=Gx8_M_ConvertColor(pLight->ul_Color);
	}

}

#endif

#ifdef JADEFUSION //JADE360
static MATH_tdst_Vector s_stPos   = {  0.0f,  0.0f,  0.0f };
static MATH_tdst_Vector s_stXAxis = { -1.0f,  0.0f,  0.0f };
static MATH_tdst_Vector s_stYAxis = {  0.0f,  0.0f, -1.0f };
static MATH_tdst_Vector s_stZAxis = {  0.0f, -1.0f,  0.0f };

void LIGHT_GetLightShaftPlaneSize(LIGHT_tdst_LightShaft* _pst_LightShaft, FLOAT _f_Dist, FLOAT* _pf_SizeX, FLOAT* _pf_SizeY)
{
    *_pf_SizeX = _f_Dist * fTan(Cf_PiBy180 * _pst_LightShaft->f_FOVX * 0.5f);
    *_pf_SizeY = _f_Dist * fTan(Cf_PiBy180 * _pst_LightShaft->f_FOVY * 0.5f);
}

void LIGHT_GetLightShaftFrustum(LIGHT_tdst_LightShaft* _pst_LightShaft, MATH_tdst_Vector* _ast_Frustum)
{
    MATH_tdst_Vector vZ;
    MATH_tdst_Vector vTemp;
    FLOAT fX, fY;

    // Top
    LIGHT_GetLightShaftPlaneSize(_pst_LightShaft, _pst_LightShaft->f_Start, &fX, &fY);
    MATH_CopyVector(&vZ, &s_stPos);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis, -fX);
    MATH_AddScaleVector(&_ast_Frustum[0], &vTemp, &s_stYAxis,  fY);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis,  fX);
    MATH_AddScaleVector(&_ast_Frustum[1], &vTemp, &s_stYAxis,  fY);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis,  fX);
    MATH_AddScaleVector(&_ast_Frustum[2], &vTemp, &s_stYAxis, -fY);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis, -fX);
    MATH_AddScaleVector(&_ast_Frustum[3], &vTemp, &s_stYAxis, -fY);

    // Bottom
    LIGHT_GetLightShaftPlaneSize(_pst_LightShaft, _pst_LightShaft->f_Start + _pst_LightShaft->f_Length, &fX, &fY);
    MATH_AddScaleVector(&vZ, &s_stPos, &s_stZAxis, _pst_LightShaft->f_Length);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis, -fX);
    MATH_AddScaleVector(&_ast_Frustum[4], &vTemp, &s_stYAxis,  fY);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis,  fX);
    MATH_AddScaleVector(&_ast_Frustum[5], &vTemp, &s_stYAxis,  fY);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis,  fX);
    MATH_AddScaleVector(&_ast_Frustum[6], &vTemp, &s_stYAxis, -fY);
    MATH_AddScaleVector(&vTemp, &vZ, &s_stXAxis, -fX);
    MATH_AddScaleVector(&_ast_Frustum[7], &vTemp, &s_stYAxis, -fY);
}

#if defined(_XENON_RENDER)

#if defined(ACTIVE_EDITORS)

void LIGHT_RenderLightShaft_Editor(OBJ_tdst_GameObject* _pst_GO)
{
    LIGHT_tdst_Light*      pst_Light = (LIGHT_tdst_Light*)_pst_GO->pst_Extended->pst_Light;
    LIGHT_tdst_LightShaft* pst_Shaft = &pst_Light->st_LightShaft;

    MATH_tdst_Vector            astFrustum[8];
    GDI_tdst_Request_DrawLineEx stLineEx;

    // Build the light shaft frustum
    LIGHT_GetLightShaftFrustum(pst_Shaft, astFrustum);

    for (ULONG i = 0; i < 4; ++i)
    {
        // Top
        stLineEx.A        = &astFrustum[i];
        stLineEx.B        = &astFrustum[(i + 1) % 4];
        stLineEx.f_Width  = 1;
        stLineEx.ul_Flags = 0;
        stLineEx.ul_Color = 0xff204080;
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&stLineEx);

        // Link
        stLineEx.A        = &astFrustum[i];
        stLineEx.B        = &astFrustum[4 + i];
        stLineEx.f_Width  = 1;
        stLineEx.ul_Flags = 0;
        stLineEx.ul_Color = 0xff204080;
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&stLineEx);

        // Bottom
        stLineEx.A        = &astFrustum[4 + i];
        stLineEx.B        = &astFrustum[4 + ((i + 1) % 4)];
        stLineEx.f_Width  = 1;
        stLineEx.ul_Flags = 0;
        stLineEx.ul_Color = 0xff204080;
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&stLineEx);
    }
}
#endif

void LIGHT_RenderXenon(OBJ_tdst_GameObject* _pst_GO)
{
    LIGHT_tdst_Light* pst_Light;

    if (!_pst_GO || !OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Lights))
        return;

    pst_Light = (LIGHT_tdst_Light*)_pst_GO->pst_Extended->pst_Light;
    if (!pst_Light)
        return;

    if (!LIGHT_b_IsOfType(pst_Light, LIGHT_Cul_LF_LightShaft))
        return;

    g_oXeLightShaftManager.QueueLightShaftForRender(_pst_GO);
}

#endif

void LIGHT_InitLightShaft(LIGHT_tdst_LightShaft* _pst_LightShaft)
{
    _pst_LightShaft->f_Start                = 1.0f;
    _pst_LightShaft->f_Length               = 4.0f;
    _pst_LightShaft->f_FOVX                 = 45.0f;
    _pst_LightShaft->f_FOVY                 = 45.0f;
    _pst_LightShaft->ul_CookieTextureKey    = BIG_C_InvalidKey;
    _pst_LightShaft->ul_NoiseTextureKey     = BIG_C_InvalidKey;
    _pst_LightShaft->us_CookieTexture       = -1;
    _pst_LightShaft->us_NoiseTexture        = -1;
    _pst_LightShaft->ul_Flags               =  0;
    _pst_LightShaft->f_Noise1ScrollU        = 0.0f;
    _pst_LightShaft->f_Noise1ScrollV        = 0.0f;
    _pst_LightShaft->f_Noise2ScrollU        = 0.0f;
    _pst_LightShaft->f_Noise2ScrollV        = 0.0f;
    _pst_LightShaft->f_PlaneDensity         = 8.0f;
    _pst_LightShaft->f_AttenuationStart     = 0.0f;
    _pst_LightShaft->f_AttenuationFactor    = 1.0f;
    _pst_LightShaft->f_SpotInnerAngle       = 30.0f;
    _pst_LightShaft->f_SpotOuterAngle       = 60.0f;
    _pst_LightShaft->ul_Color               = 0xffffffff;
}
#endif
