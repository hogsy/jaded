#ifndef __MDFMODIFIER_UVTexWave_H__
#define __MDFMODIFIER_UVTexWave_H__


typedef struct	UVTexWave_tdst_Modifier_
{
	int	MatLevel;
	float f_SpeedCoefU;//[5];
	float f_SpeedCoefV;//[5];
	float f_AngleU;//[5];
	float f_AngleV;//[5];

} UVTexWave_tdst_Modifier;


extern void UVTexWave_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void UVTexWave_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod);
extern void UVTexWave_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern void UVTexWave_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern ULONG UVTexWave_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);

#ifdef ACTIVE_EDITORS
extern BOOL UVTexWave_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg);
extern void UVTexWave_Modifier_Save(MDF_tdst_Modifier *_pst_Mod);
#endif

extern BOOL					UVWave;
extern int					LevelUVWave;
/*extern int					NbrFurLayers ;
extern float				OffsetNormal ;
extern float				OffsetU ;
extern float				OffsetV ;*/

#endif