/*$T AIfunctions_light.c GC!1.71 01/28/00 15:05:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/ENGinit.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetColor_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Color)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return;

    pst_Light->ul_Color = _ul_Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetColor(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_EvalFunc_LightSetColor_C(pst_GO, AI_PopInt());
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_LightGetColor_C(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return 0;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return 0;

    return pst_Light->ul_Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetColor(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_PushInt(AI_EvalFunc_LightGetColor_C(pst_GO));
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetNearFar_C(OBJ_tdst_GameObject *_pst_GO, float _f_Near, float _f_Far)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return;

    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return;

    switch(pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
    {
    case LIGHT_Cul_LF_Omni:
        if(_f_Near >= 0) pst_Light->st_Omni.f_Near = _f_Near;
        if(_f_Far >= 0) pst_Light->st_Omni.f_Far = _f_Far;
        break;
    case LIGHT_Cul_LF_Spot:
        if(_f_Near >= 0) pst_Light->st_Spot.f_Near = _f_Near;
        if(_f_Far >= 0) pst_Light->st_Spot.f_Far = _f_Far;
        break;
    case LIGHT_Cul_LF_Fog:
        pst_Light->st_Fog.f_Start = _f_Near;
        pst_Light->st_Fog.f_End = _f_Far;
        break;
    }
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetNearFar(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    float               f_Far;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    f_Far = AI_PopFloat();
    AI_EvalFunc_LightSetNearFar_C(pst_GO, AI_PopFloat(), f_Far);
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_LightGetNear_C(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return 0;

    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return 0;

    switch(pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
    {
    case LIGHT_Cul_LF_Omni:
        return pst_Light->st_Omni.f_Near;

    case LIGHT_Cul_LF_Spot:
        return pst_Light->st_Spot.f_Near;

    case LIGHT_Cul_LF_Fog:
        return pst_Light->st_Fog.f_Start;
    }

    return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetNear(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_PushFloat(AI_EvalFunc_LightGetNear_C(pst_GO));
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_LightGetFar_C(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return 0;

    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return 0;

    switch(pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
    {
    case LIGHT_Cul_LF_Omni:
        return pst_Light->st_Omni.f_Far;

    case LIGHT_Cul_LF_Spot:
        return pst_Light->st_Spot.f_Far;

    case LIGHT_Cul_LF_Fog:
        return pst_Light->st_Fog.f_End;
    }

    return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetFar(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_PushFloat(AI_EvalFunc_LightGetFar_C(pst_GO));
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetGO_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_GO2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return;

    pst_Light->pst_GO = _pst_GO2;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetGO(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_EvalFunc_LightSetGO_C(pst_GO, AI_PopGameObject());
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetFlag_C(OBJ_tdst_GameObject *_pst_GO, int _i_Flag, int _i_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return;

    if (_i_Value)
        pst_Light->ul_Flags |= 1 << _i_Flag;
    else
        pst_Light->ul_Flags &= ~(1 << _i_Flag);
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetFlag(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    int                 i_Value;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    i_Value = AI_PopInt();
    AI_EvalFunc_LightSetFlag_C(pst_GO, AI_PopInt(), i_Value);
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_LightGetFlag_C(OBJ_tdst_GameObject *_pst_GO, int _i_Flag )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return 0;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return 0;

    return (pst_Light->ul_Flags & (1 << _i_Flag)) ? 1 : 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetFlag(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_PushInt( AI_EvalFunc_LightGetFlag_C(pst_GO, AI_PopInt()));
    return ++_pst_Node;
}

/*$4 RLI */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetRLIFactor_C( float _f_Value )
{
    if (GDI_gpst_CurDD)
        GDI_gpst_CurDD->f_RLIScale = _f_Value;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetRLIFactor(AI_tdst_Node *_pst_Node)
{
    AI_EvalFunc_LightSetRLIFactor_C( AI_PopFloat() );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_LightGetRLIFactor_C( void )
{
    return (GDI_gpst_CurDD ? GDI_gpst_CurDD->f_RLIScale : 1.0f);
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetRLIFactor(AI_tdst_Node *_pst_Node)
{
    AI_PushFloat( AI_EvalFunc_LightGetRLIFactor_C() );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetRLIDestColor_C( ULONG _ul_Color )
{
    if (GDI_gpst_CurDD)
        GDI_gpst_CurDD->ul_RLIColorDest = _ul_Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetRLIDestColor(AI_tdst_Node *_pst_Node)
{
    AI_EvalFunc_LightSetRLIDestColor_C( AI_PopInt() );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_LightGetRLIDestColor_C( void )
{
    return (GDI_gpst_CurDD ? GDI_gpst_CurDD->ul_RLIColorDest : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetRLIDestColor(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_LightGetRLIDestColor_C() );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_LightRLIFactorUsed_C( int used )
{
    ULONG   ret;
    
    if (!GDI_gpst_CurDD) return 0;

    ret = (GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_DontScaleRLI) ? 0 : 1;

    if ( used == 0)
    {
        GDI_gpst_CurDD->ul_DrawMask |= GDI_Cul_DM_DontScaleRLI;
        GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_DontScaleRLI;
    }
    else if (used == 1)
    {
        GDI_gpst_CurDD->ul_DrawMask &= ~GDI_Cul_DM_DontScaleRLI;
        GDI_gpst_CurDD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontScaleRLI;
    }
    return ret;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightRLIFactorUsed(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_LightRLIFactorUsed_C( AI_PopInt() ) );
    return ++_pst_Node;
}

/*$4 Attenuation factor */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetAttenuationFactor_C( OBJ_tdst_GameObject *_pst_GO, float _f_Value )
{
	if ( _pst_GO )
	{
		if (!_pst_GO->pst_Base) return;
		if (!_pst_GO->pst_Base->pst_Visu || !(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) ) return;
		if ( (_f_Value <= 0) || (_f_Value > 1.0f) )
		{
			_pst_GO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_DontAttenuateLight;
			_pst_GO->pst_Base->pst_Visu->c_LightAttenuation = 0;
		}
		else
		{
			_pst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~GDI_Cul_DM_DontAttenuateLight;
			_pst_GO->pst_Base->pst_Visu->c_LightAttenuation = (unsigned char) (_f_Value * 127.0f);
		}
		return;
	}
    
	if (GDI_gpst_CurDD)
	{
		if ( (_f_Value <= 0) || (_f_Value > 1.0f) )
		{
			GDI_gpst_CurDD->ul_DrawMask |= GDI_Cul_DM_DontAttenuateLight;
			GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_DontAttenuateLight;
			GDI_gpst_CurDD->f_LightAttenuation = 0;
		}
		else
		{
			GDI_gpst_CurDD->ul_DrawMask &= ~GDI_Cul_DM_DontAttenuateLight;
			GDI_gpst_CurDD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontAttenuateLight;
			GDI_gpst_CurDD->f_LightAttenuation = _f_Value;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetAttenuationFactor(AI_tdst_Node *_pst_Node)
{
	float f_Value;
	f_Value = AI_PopFloat();
    AI_EvalFunc_LightSetAttenuationFactor_C( AI_PopGameObject(), f_Value );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_LightGetAttenuationFactor_C( OBJ_tdst_GameObject *_pst_GO )
{
	if ( _pst_GO )
	{
		if (!_pst_GO->pst_Base) return 0.0f;
		if (!_pst_GO->pst_Base->pst_Visu || !(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)))return 0.0f;
		
		if ( _pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_DontAttenuateLight )
			return 0.0f;
		return ((float) _pst_GO->pst_Base->pst_Visu->c_LightAttenuation) / 127.0f;
	}
    
	if (GDI_gpst_CurDD)
	{
		if ( !( GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_DontAttenuateLight ) )
			return GDI_gpst_CurDD->f_LightAttenuation;
	}

	return 0.0f;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetAttenuationFactor(AI_tdst_Node *_pst_Node)
{
    AI_PushFloat( AI_EvalFunc_LightGetAttenuationFactor_C( AI_PopGameObject() ) );
    return ++_pst_Node;
}


/*$4 fog */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_LightFogState_C( int used )
{
    ULONG   ret;
    
    if (!GDI_gpst_CurDD) return 0;

    ret = (GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_Fogged) ? 1 : 0;

    if ( used == 1)
        GDI_gpst_CurDD->ul_DrawMask |= GDI_Cul_DM_Fogged;
    else if (used == 0)
        GDI_gpst_CurDD->ul_DrawMask &= ~GDI_Cul_DM_Fogged;

    return ret;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightFogState(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_LightFogState_C( AI_PopInt() ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_LightGetFog_C( OBJ_tdst_GameObject *_pst_GO )
{
	WOR_tdst_World *pst_World = WOR_World_GetWorldOfObject( _pst_GO );
	assert( pst_World != NULL );

	WOR_World_LightsVector *world_lights = ( WOR_World_LightsVector * ) ( pst_World->st_Lights );

	for ( auto it = world_lights->begin(); it != world_lights->end(); ++it )
	{
		OBJ_tdst_GameObject *pst_LightGO = *it;

		LIGHT_tdst_Light *pst_Light = ( LIGHT_tdst_Light * ) pst_LightGO->pst_Extended->pst_Light;
		if ( pst_Light && ( pst_Light->ul_Flags & LIGHT_Cul_LF_Active ) )
		{
			if ( ( pst_Light->ul_Flags & LIGHT_Cul_LF_Type ) == LIGHT_Cul_LF_Fog )
				return pst_LightGO;
		}
	}

	return NULL;
}

/**/
AI_tdst_Node *AI_EvalFunc_LightGetFog(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushGameObject(AI_EvalFunc_LightGetFog_C(pst_GO));
	return ++_pst_Node;
}
#ifdef JADEFUSION
/*
 ADDED SURESH
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetDiffuse_C(OBJ_tdst_GameObject *_pst_GO, float _f_Diffuse)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return;
	
	pst_Light->f_DiffuseMultiplier = _f_Diffuse;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetDiffuse(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_EvalFunc_LightSetDiffuse_C(pst_GO, AI_PopFloat()); 
    return ++_pst_Node;
}

/*
 ADDED SURESH
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetSpecular_C(OBJ_tdst_GameObject *_pst_GO, float _f_Specular)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return;
	
	pst_Light->f_SpecularMultiplier = _f_Specular;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetSpecular(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_EvalFunc_LightSetSpecular_C(pst_GO, AI_PopFloat()); 
    return ++_pst_Node;
}

/*
 ADDED SURESH
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_LightGetDiffuse_C(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return 0;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return 0;

	return pst_Light->f_DiffuseMultiplier;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetDiffuse(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_PushFloat(AI_EvalFunc_LightGetDiffuse_C(pst_GO));
    return ++_pst_Node;
}

/*
 ADDED SURESH
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_LightGetSpecular_C(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return 0;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return 0;

	return pst_Light->f_SpecularMultiplier;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetSpecular(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_PushFloat(AI_EvalFunc_LightGetSpecular_C(pst_GO));
    return ++_pst_Node;
}

/*
 ADDED SURESH - get light density 
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_LightGetFogDensity_C(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return 0;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return 0;

	if((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Fog)
		return pst_Light->st_Fog.f_Density;	
	else
		return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightGetFogDensity(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_PushFloat(AI_EvalFunc_LightGetFogDensity_C(pst_GO));
    return ++_pst_Node;
}

/*
 ADDED SURESH - set light density
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_LightSetFogDensity_C(OBJ_tdst_GameObject *_pst_GO, float _f_Density)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LIGHT_tdst_Light    *pst_Light;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO || (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)))
        return;
    pst_Light = (LIGHT_tdst_Light *) _pst_GO->pst_Extended->pst_Light;
    if(!pst_Light) return;
	
	if((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Fog)
		pst_Light->st_Fog.f_Density	= _f_Density;
}
/**/
AI_tdst_Node *AI_EvalFunc_LightSetFogDensity(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    AI_EvalFunc_LightSetFogDensity_C(pst_GO, AI_PopFloat()); 
    return ++_pst_Node;
}
#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
