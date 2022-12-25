
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "GDInterface/GDInterface.h"

#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_UVTexWave.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

/*int					NbrFurLayers = 10;
float				OffsetNormal = 0.035f;
float				OffsetU = 0.0f;
float				OffsetV = 0.0f;
int					FurInvertAlpha = 0;*/

//int					NbrObjectClone = 10;
BOOL UVWave = 0;
int LevelUVWave =0;

void UVTexWave_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	UVTexWave_tdst_Modifier * p_UVTexWave;

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(UVTexWave_tdst_Modifier));
	p_UVTexWave = (UVTexWave_tdst_Modifier *) _pst_Mod->p_Data;
	
//	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

	if(p_Data == NULL)
	{
		int i;
		//for (i=0;i<5;i++)
		{
			p_UVTexWave->MatLevel = 0;
			p_UVTexWave->f_SpeedCoefU = 1;
			p_UVTexWave->f_SpeedCoefV = 1;
			p_UVTexWave->f_AngleU = 1;
			p_UVTexWave->f_AngleV = 1;
		}
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(UVTexWave_tdst_Modifier));
	}
}

void UVTexWave_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if(_pst_Mod->p_Data)
		MEM_Free(_pst_Mod->p_Data);
}

void UVTexWave_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj){

	UVTexWave_tdst_Modifier * p_UVTexWave;
	p_UVTexWave = (UVTexWave_tdst_Modifier *) _pst_Mod->p_Data;
	if (p_UVTexWave)
	{
/*		OffsetNormal = p_FUR->f_NormalOffset/100.0f;
		OffsetU = p_FUR->f_UOffset/10000.0f;
		OffsetV = p_FUR->f_VOffset/10000.0f;
		NbrFurLayers = p_FUR->NBR_Layers;
		FurInvertAlpha = 0;*/
		//NbrObjectClone = p_UVTexWave->NBR_Instances;
		//_pst_Obj->NbDuplicate = p_UVTexWave->NBR_Instances;
		//LevelUVWave = p_UVTexWave->MatLevel;
		UVWave = TRUE;
	}

}

void UVTexWave_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	//LevelUVWave = 0;
	UVWave = FALSE;

}

#ifdef ACTIVE_EDITORS
BOOL UVTexWave_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg){
	// todo test if the obj contain a material
	return TRUE;
}
void UVTexWave_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UVTexWave_tdst_Modifier	*pst_Data;
	ULONG				ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	int i;
	pst_Data = (UVTexWave_tdst_Modifier *) _pst_Mod->p_Data;

	ul_Size = 4;
	SAV_Buffer(&ul_Size, 4);
	
	//for (i=0;i<5;i++)
	{
		SAV_Buffer(&pst_Data->MatLevel, sizeof(int));
		SAV_Buffer(&pst_Data->f_SpeedCoefU, sizeof(float));
		SAV_Buffer(&pst_Data->f_SpeedCoefV, sizeof(float));
		SAV_Buffer(&pst_Data->f_AngleU, sizeof(float));
		SAV_Buffer(&pst_Data->f_AngleV, sizeof(float));
	}
}
#endif

ULONG UVTexWave_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char                        *pc_Cur;
	UVTexWave_tdst_Modifier   *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (UVTexWave_tdst_Modifier *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;
//	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

	// Size
	LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size

	pst_Data->MatLevel = LOA_ReadInt(&pc_Cur);
	pst_Data->f_SpeedCoefU = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_SpeedCoefV = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_AngleU = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_AngleV = LOA_ReadFloat(&pc_Cur);

	return (pc_Cur - _pc_Buffer);
}
