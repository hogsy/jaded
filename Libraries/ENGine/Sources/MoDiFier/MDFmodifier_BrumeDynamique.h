#ifndef __MDFMODIFIER_FOGDY_H__
#define __MDFMODIFIER_FOGDY_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
typedef struct	GAO_tdst_ModifierFOGDY_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
	u32					IsInit;
	OBJ_tdst_GameObject *p_GO;
#endif
	float				XSize;
	float				YSize;
	float				ZSize;
	u32					GFXExplodeID;
	u32					GFXExplodeNumber;
	u32					ActiveChannel;
	u32					SubMaterialNum1;
	u32					SubMaterialNum2;
	u32					SubMaterialNum3;
	u32					SubMaterialNum4;

	float				SpeedStart;
	float				MaxSpeed;

/* Mechanic parrameters */
	u32					CollisionMode;
	float				PushPower;
	float				ExtractionSpeed;

/* GFX Explode parrameters */
	u32					GFX_ID;
	u32					Colors1;
	u32					Colors2;
	u32					Colors3;
	float				TimePhase1;
	float				TimePhase2;
	float				SpeedMin;
	float				SpeedMax;
	float				SizeMin;
	float				SizeMax;
	float				GrowingMin;
	float				GrowingMax;
	float				FrictionGrow;
	MATH_tdst_Vector	FrictionSpeed;
	float				Gravitiy;
	float				TimeVariance;
	float				GenerationRate;
	u32					ulNumberOfActiveSprites;
	float				FrictionLenght;

/* GFX Spark parrameters */
	u32					GFX_SPARK_ID;


} GAO_tdst_ModifierFOGDY;


void GAO_ModifierFOGDY_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, u32 mode , float fInterpolatedValue);
void GAO_ModifierFOGDY_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void GAO_ModifierFOGDY_Destroy(MDF_tdst_Modifier *_pst_Mod);
void GAO_ModifierFOGDY_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
void GAO_ModifierFOGDY_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj); 
void GAO_ModifierFOGDY_Init(GAO_tdst_ModifierFOGDY *p_FOGDY);
void GAO_ModifierFOGDY_Reinit(MDF_tdst_Modifier *_pst_Mod);
void FOGDY_Modifier_Save(MDF_tdst_Modifier *_pst_Mod);
ULONG FOGDY_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
int FogDy_Modifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src );

#define ModifierFOGDY_Emiter_BoneNUmber 8
typedef struct	GAO_tdst_ModifierFOGDY_Emiter_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
	ULONG				ulShowInfo;
#endif
	u32					ActiveChannel;
	ULONG				ActiveBoneNumber;
	ULONG				BoneID		[ModifierFOGDY_Emiter_BoneNUmber];
	float				BoneRadius	[ModifierFOGDY_Emiter_BoneNUmber];
	MATH_tdst_Vector	BoneCenters	[ModifierFOGDY_Emiter_BoneNUmber];
	MATH_tdst_Vector	BoneSpeeds	[ModifierFOGDY_Emiter_BoneNUmber];
	MATH_tdst_Vector	BoneDelta[ModifierFOGDY_Emiter_BoneNUmber];
} GAO_tdst_ModifierFOGDY_Emiter;

void GAO_ModifierFOGDY_Emtr_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, u32 mode , float fInterpolatedValue);
void GAO_ModifierFOGDY_Emtr_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void GAO_ModifierFOGDY_Emtr_Destroy(MDF_tdst_Modifier *_pst_Mod);
void GAO_ModifierFOGDY_Emtr_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
void GAO_ModifierFOGDY_Emtr_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj); 
void GAO_ModifierFOGDY_Emtr_Init(GAO_tdst_ModifierFOGDY_Emiter *p_FOGDY);
void GAO_ModifierFOGDY_Emtr_Reinit(MDF_tdst_Modifier *_pst_Mod);
void FOGDY_Emtr_Modifier_Save(MDF_tdst_Modifier *_pst_Mod);
ULONG FOGDY_Emtr_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
int FOGDY_Emtr_Modifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src );




#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif 
