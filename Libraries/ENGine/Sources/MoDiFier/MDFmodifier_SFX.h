 #ifndef __MDFMODIFIER_SFX_H__
#define __MDFMODIFIER_SFX_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#ifdef _XENON_RENDER
#include "XenonGraphics/XeHeatManager.h"
#endif

//#ifdef __cplusplus
//extern "C"
//{
//#endif

typedef enum 
{
	MDF_SFX_HotAir                    = 0,
    MDF_SFX_RimLight                  = 1
} MDF_SFX_Type ;

//-----------------------------
// Hear Shimmering
//-----------------------------
typedef struct MDF_tdst_HotAir_
{
    MATH_tdst_Vector	stOffset;
#ifdef _XENON_RENDER
    HotAirObject        stHotAirObject;
#endif
} MDF_tdst_HotAir;

//-----------------------------
// Rim Light
//-----------------------------
typedef struct MDF_tdst_RimLight_
{
    float               fHeightAttenuationMin;
    float               fHeightAttenuationMax;
} MDF_tdst_RimLight;

#define MDF_SFX_Flags_WasDuplicated	0x00000001

typedef struct  GAO_tdst_ModifierSfx_
{
	MDF_SFX_Type		type;
	ULONG				flags;
	
	// in editor it's not a union so we can keep our parameters
	// when switching types in the properties
#ifndef ACTIVE_EDITORS
	union 
	{
#endif
		MDF_tdst_HotAir				hotAirParams;
        MDF_tdst_RimLight           rimLightParams;
#ifndef ACTIVE_EDITORS
	};
#endif

	ULONG						ulUserID;
	
} GAO_tdst_ModifierSfx;



extern MDF_tdst_HotAir * MDF_SfxGetHotAirModifier( OBJ_tdst_GameObject *   _pst_GO );

extern void     GAO_ModifierSfx_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void     GAO_ModifierSfx_Destroy(MDF_tdst_Modifier *);
extern void     GAO_ModifierSfx_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierSfx_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierSfx_Reinit(MDF_tdst_Modifier *);
extern ULONG    GAO_ModifierSfx_Load(MDF_tdst_Modifier *, char *);
extern void     GAO_ModifierSfx_Save(MDF_tdst_Modifier *);

extern BOOL     IsRimLightHeightAttenuationEnabled;  
extern float	RimLightHeightWorldMin;               
extern float	RimLightHeightWorldMax;               
extern float	RimLightHeightAttenuationMin;
extern float	RimLightHeightAttenuationMax;         

//#ifdef __cplusplus
//}
//#endif
#endif 
