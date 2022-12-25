// ------------------------------------------------------------------------------------------------
// File   : MDFmodifier_Weather.c
// Date   : 2005-04-15
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MDFmodifier_Weather.h"
#include "GEOmetric/GEOobject.h"
#include "BASe/BENch/BENch.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "OBJects/OBJaccess.h"

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeWeatherManager.h"
#endif

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------
const ULONG MDF_WEATHER_VERSION = 1;

// ------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------
static MAT_tdst_MultiTexture* ExtractMultiTexture(MDF_tdst_Modifier* _pst_Mod);

static void ResetWeatherParameters(MDF_tdst_Weather* _pst_Weather);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

void Weather_Modifier_Create(struct OBJ_tdst_GameObject_* _pst_GO, MDF_tdst_Modifier* _pst_Mod, void* _pst_Data)
{
    MDF_tdst_Weather* pWeather;

    pWeather         = (MDF_tdst_Weather*)MEM_p_Alloc(sizeof(MDF_tdst_Weather));
    _pst_Mod->p_Data = pWeather;

    if (_pst_Data != NULL)
    {
        // Duplicate
        L_memcpy(pWeather, _pst_Data, sizeof(MDF_tdst_Weather));
    }
    else
    {
        // Create from scratch
        L_memset(pWeather, 0, sizeof(MDF_tdst_Weather));

        // Assign default parameters to the modifier
        ResetWeatherParameters(pWeather);
    }
}

void Weather_Modifier_Destroy(MDF_tdst_Modifier* _pst_Mod)
{
    MDF_tdst_Weather* pWeather;

    pWeather = (MDF_tdst_Weather*)_pst_Mod->p_Data;
    if (pWeather)
    {
        MEM_Free(pWeather);
        _pst_Mod->p_Data = NULL;
    }
}

void Weather_Modifier_Apply(MDF_tdst_Modifier* _pst_Mod, struct GEO_tdst_Object_* _pst_Geo)
{
    MDF_tdst_Weather*      pWeather = (MDF_tdst_Weather*)_pst_Mod->p_Data;
    MAT_tdst_MultiTexture* pMultiTex;

#if defined(_XENON_RENDER)

#if defined(ACTIVE_EDITORS)
    if (!GDI_b_IsXenonGraphics())
        return;
#endif

    switch (pWeather->e_Type)
    {
        case MDF_Weather_RainFX:
            {
                // Deactivate the material so that the object will not be queued
                pMultiTex = ExtractMultiTexture(_pst_Mod);
                if (pMultiTex && pMultiTex->pst_FirstLevel)
                {
                    pMultiTex->pst_FirstLevel->ul_Flags |= MAT_Cul_Flag_InActive;
                }

#if !defined(XML_CONV_TOOL)
                g_oXeWeatherManager.AddRainFX(_pst_Mod->pst_GO, pWeather);
#endif
            }
            break;
    }

#endif
}

void Weather_Modifier_Unapply(MDF_tdst_Modifier* _pst_Mod, struct GEO_tdst_Object_* _pst_Geo)
{
    MDF_tdst_Weather*      pWeather = (MDF_tdst_Weather*)_pst_Mod->p_Data;
    MAT_tdst_MultiTexture* pMultiTex;

#if defined(_XENON_RENDER)

#if defined(ACTIVE_EDITORS)
    if (!GDI_b_IsXenonGraphics())
        return;
#endif

    switch (pWeather->e_Type)
    {
        case MDF_Weather_RainFX:
            {
                // Reactivate the material
                pMultiTex = ExtractMultiTexture(_pst_Mod);
                if (pMultiTex && pMultiTex->pst_FirstLevel)
                {
                    pMultiTex->pst_FirstLevel->ul_Flags &= ~MAT_Cul_Flag_InActive;
                }
            }
            break;
    }

#endif
}

ULONG Weather_Modifier_Load(MDF_tdst_Modifier* _pst_Mod, char* _pc_Buffer)
{
    MDF_tdst_Weather* pWeather;
    ULONG ulVersion;
    char* pc_BufferSave = _pc_Buffer;

    pWeather = (MDF_tdst_Weather*)_pst_Mod->p_Data;

    // Set default values so we don't have to update parameters that are not read because of the version
    ResetWeatherParameters(pWeather);

    ulVersion = LOA_ReadULong(&_pc_Buffer);

    if (ulVersion >= 1)
    {
        pWeather->e_Type = (MDF_Weather_Type)LOA_ReadULong(&_pc_Buffer);

        switch (pWeather->e_Type)
        {
            case MDF_Weather_RainFX:
                {
                    pWeather->st_RainFX.ul_Flags         = LOA_ReadULong(&_pc_Buffer);
                    pWeather->st_RainFX.f_RainScaleU     = LOA_ReadFloat(&_pc_Buffer);
                    pWeather->st_RainFX.f_RainScaleV     = LOA_ReadFloat(&_pc_Buffer);
                    pWeather->st_RainFX.f_AlphaBoost     = LOA_ReadFloat(&_pc_Buffer);
                    pWeather->st_RainFX.f_AlphaIntensity = LOA_ReadFloat(&_pc_Buffer);
                }
                break;
        }
    }

    return _pc_Buffer - pc_BufferSave;
}

void Weather_Modifier_Reinit(MDF_tdst_Modifier* _pst_Mod)
{
    MDF_tdst_Weather* pWeather = (MDF_tdst_Weather*)_pst_Mod->p_Data;
}

#if defined(ACTIVE_EDITORS)

void Weather_Modifier_Save(MDF_tdst_Modifier* _pst_Mod)
{
    MDF_tdst_Weather* pWeather = (MDF_tdst_Weather*)_pst_Mod->p_Data;
    ULONG ulVersion = MDF_WEATHER_VERSION;
    ULONG ulType    = (ULONG)pWeather->e_Type;

    SAV_Buffer(&ulVersion, sizeof(ULONG));
    SAV_Buffer(&ulType,    sizeof(ULONG));

    switch (pWeather->e_Type)
    {
        case MDF_Weather_RainFX:
            {
                SAV_Buffer(&pWeather->st_RainFX.ul_Flags,           sizeof(ULONG));
                SAV_Buffer(&pWeather->st_RainFX.f_RainScaleU,       sizeof(FLOAT));
                SAV_Buffer(&pWeather->st_RainFX.f_RainScaleV,       sizeof(FLOAT));
                SAV_Buffer(&pWeather->st_RainFX.f_AlphaBoost,       sizeof(FLOAT));
                SAV_Buffer(&pWeather->st_RainFX.f_AlphaIntensity,   sizeof(FLOAT));
            }
            break;
    }
}

#endif

static MAT_tdst_MultiTexture* ExtractMultiTexture(MDF_tdst_Modifier* _pst_Mod)
{
    if (OBJ_b_TestIdentityFlag(_pst_Mod->pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        MAT_tdst_Material* pMaterial = (MAT_tdst_Material*)_pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Material;
        if (pMaterial && (pMaterial->st_Id.i->ul_Type == GRO_MaterialMultiTexture))
        {
            return (MAT_tdst_MultiTexture*)pMaterial;
        }
    }

    return NULL;
}

static void ResetWeatherParameters(MDF_tdst_Weather* _pst_Weather)
{
    if (_pst_Weather->e_Type >= MDF_Weather_Count)
        _pst_Weather->e_Type = MDF_Weather_RainFX;

    switch (_pst_Weather->e_Type)
    {
        case MDF_Weather_RainFX:
            _pst_Weather->st_RainFX.ul_Flags         = 0;
            _pst_Weather->st_RainFX.f_RainScaleU     = 20.0f;
            _pst_Weather->st_RainFX.f_RainScaleV     = 3.0f;
            _pst_Weather->st_RainFX.f_AlphaBoost     = 16.0f;
            _pst_Weather->st_RainFX.f_AlphaIntensity = 0.25f;
            break;
    }
}
