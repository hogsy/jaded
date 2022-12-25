// ------------------------------------------------------------------------------------------------
// File   : MDFmodifier_Weather.h
// Date   : 2005-04-15
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_MDFMODIFIER_WEATHER_H
#define GUARD_MDFMODIFIER_WEATHER_H

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------
#define MDF_Weather_RainFX_Flag_Dynamic     0x00000001
#define MDF_Weather_RainFX_Flag_EnableWind  0x00000002

// ------------------------------------------------------------------------------------------------
// TYPES
// ------------------------------------------------------------------------------------------------
typedef enum
{
    MDF_Weather_RainFX          = 0,

    MDF_Weather_Count,

    MDF_Weather_Force_Dword     = 0xffffffff
} MDF_Weather_Type;

// ------------------------------------------------------------------------------------------------
// STRUCTURES
// ------------------------------------------------------------------------------------------------
typedef struct Weather_tdst_RainFX_
{
    ULONG ul_Flags;
    FLOAT f_RainScaleU;
    FLOAT f_RainScaleV;
    FLOAT f_AlphaBoost;
    FLOAT f_AlphaIntensity;
} Weather_tdst_RainFX;

typedef struct MDF_tdst_Weather_
{
    MDF_Weather_Type e_Type;

#if !defined(ACTIVE_EDITORS)
    union
    {
#endif
        Weather_tdst_RainFX st_RainFX;
#if !defined(ACTIVE_EDITORS)
    };
#endif

} MDF_tdst_Weather;

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

extern void  Weather_Modifier_Create(struct OBJ_tdst_GameObject_*, MDF_tdst_Modifier*, void*);
extern void  Weather_Modifier_Destroy(MDF_tdst_Modifier*);
extern void  Weather_Modifier_Apply(MDF_tdst_Modifier*, struct GEO_tdst_Object_*);
extern void  Weather_Modifier_Unapply(MDF_tdst_Modifier*, struct GEO_tdst_Object_*);
extern ULONG Weather_Modifier_Load(MDF_tdst_Modifier*, char*);
extern void  Weather_Modifier_Save(MDF_tdst_Modifier*);
extern void  Weather_Modifier_Reinit(MDF_tdst_Modifier*);

#endif // #ifdef GUARD_MDFMODIFIER_WEATHER_H
