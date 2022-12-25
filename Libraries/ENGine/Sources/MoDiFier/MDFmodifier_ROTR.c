/*$T MDFmodifier_ROTR.c GC 1.129 10/09/01 16:44:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_ROTR.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierROTR_UpdatePointers(MDF_tdst_Modifier * _pst_Mod)
{
	OBJ_tdst_GameObject		*pst_GO;
	GAO_tdst_ModifierROTR	*p_ROTR;

	p_ROTR = (GAO_tdst_ModifierROTR *) _pst_Mod->p_Data;
	p_ROTR->pst_GO = NULL;
	if(_pst_Mod->pst_GO && _pst_Mod->pst_GO->pst_Base && _pst_Mod->pst_GO->pst_Base->pst_Hierarchy && _pst_Mod->pst_GO->pst_Base->pst_Hierarchy->pst_Father)
	{
		pst_GO = p_ROTR->pst_GO = _pst_Mod->pst_GO->pst_Base->pst_Hierarchy->pst_Father;
		MATH_CopyVector(&p_ROTR->v_Init, MATH_pst_GetZAxis(&_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix));
	}
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierROTR_Create (OBJ_tdst_GameObject * _pst_GO, MDF_tdst_Modifier * _pst_Mod, void *p_Data)
{
	GAO_tdst_ModifierROTR * p_ROTR;

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierROTR));
	p_ROTR = (GAO_tdst_ModifierROTR *) _pst_Mod->p_Data;
	if(p_Data == NULL)
	{
		/* First init */
		L_memset(_pst_Mod->p_Data, 0, sizeof(GAO_tdst_ModifierROTR));
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		GAO_ModifierROTR_UpdatePointers(_pst_Mod);
	}
	else
	{
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierROTR));
	}
	p_ROTR->pst_GO = NULL;
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierROTR_Destroy (MDF_tdst_Modifier * _pst_Mod)
{
	if(_pst_Mod->p_Data) MEM_Free(_pst_Mod->p_Data);
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierROTR_Apply (MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj)
{
	GAO_tdst_ModifierROTR	*p_ROTR;
	float					DT;
	MATH_tdst_Vector		v2;
	extern AI_EvalFunc_MATHVecBlendRotate_C(MATH_tdst_Vector *, MATH_tdst_Vector *, float, MATH_tdst_Vector *);
	MATH_tdst_Matrix prev, cur;
	MATH_tdst_Vector vprev, vcur;
	MATH_tdst_Vector *pst_XAxis, *pst_YAxis, *pst_ZAxis;


	DT = TIM_gf_dt;
	if(DT == 0.0f) DT = 1.0f / 50.0f;
	p_ROTR = (GAO_tdst_ModifierROTR *) _pst_Mod->p_Data;
	if(!p_ROTR->pst_GO) return;

	MATH_CopyMatrix(&prev, _pst_Mod->pst_GO->pst_GlobalMatrix);
	OBJ_ComputeGlobalWithLocal(_pst_Mod->pst_GO, _pst_Mod->pst_GO->pst_GlobalMatrix, 1);
	MATH_CopyMatrix(&cur, _pst_Mod->pst_GO->pst_GlobalMatrix);

	MATH_SubVector(&vprev, &prev.T, &_pst_Mod->pst_GO->pst_GlobalMatrix->T);
	MATH_AddEqualVector(&vprev, MATH_pst_GetZAxis(&prev));
	MATH_SetNormVector(&vprev, &vprev, 1);

	MATH_VectorGlobalToLocal(&vprev, p_ROTR->pst_GO->pst_GlobalMatrix, &vprev);
//	MATH_VectorGlobalToLocal(&vcur, p_ROTR->pst_GO->pst_GlobalMatrix, &p_ROTR->v_Init);
	MATH_CopyVector(&vcur, &p_ROTR->v_Init);

	v2.x = MATH_f_FloatBlend(vprev.x, vcur.x, p_ROTR->f_RetardX * TIM_gf_dt);
	v2.y = MATH_f_FloatBlend(vprev.y, vcur.y, p_ROTR->f_RetardY * TIM_gf_dt);
	v2.z = MATH_f_FloatBlend(vprev.z, vcur.z, p_ROTR->f_RetardZ * TIM_gf_dt);

	MATH_SetNormVector(&v2, &v2, 1);

	MATH_CopyVector(MATH_pst_GetZAxis(&_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix), &v2);

	pst_XAxis = MATH_pst_GetXAxis(&_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	pst_YAxis = MATH_pst_GetYAxis(&_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	pst_ZAxis = MATH_pst_GetZAxis(&_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	MATH_CrossProduct(pst_YAxis, pst_ZAxis, pst_XAxis);
	MATH_NormalizeEqualVector(pst_YAxis);
	MATH_CrossProduct(pst_XAxis, pst_YAxis, pst_ZAxis);
	MATH_NormalizeEqualVector(pst_XAxis);

//	MATH_Orthonormalize(&_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);

#if 0
	MATH_SubVector(&v, OBJ_pst_GetAbsolutePosition(_pst_Mod->pst_GO), OBJ_pst_GetAbsolutePosition(p_ROTR->pst_GO));
	MATH_SetNormVector(&v, &v, MATH_f_NormVector(&p_ROTR->v_Init));
	MATH_VectorGlobalToLocal(&v1, p_ROTR->pst_GO->pst_GlobalMatrix, &v);

	v2.x = MATH_f_FloatBlend(v1.x, p_ROTR->v_Init.x, p_ROTR->f_RetardX * TIM_gf_dt);
	v2.y = MATH_f_FloatBlend(v1.y, p_ROTR->v_Init.y, p_ROTR->f_RetardY * TIM_gf_dt);
	v2.z = p_ROTR->v_Init.z;

//	AI_EvalFunc_MATHVecBlendRotate_C(&v1, &p_ROTR->v_Init, p_ROTR->f_Retard * TIM_gf_dt, &v2);

	MATH_SetNormVector(&v, &v2, MATH_f_NormVector(&p_ROTR->v_Init));
	MATH_CopyVector(&_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &v);
#endif
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierROTR_Unapply (MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj)
{
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierROTR_Reinit (MDF_tdst_Modifier * _pst_Mod)
{
	GAO_ModifierROTR_UpdatePointers(_pst_Mod);
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
ULONG GAO_ModifierROTR_Load (MDF_tdst_Modifier * _pst_Mod, char *_pc_Buffer)
{
	GAO_tdst_ModifierROTR * p_ROTR;
	char *_pc_BufferSave;

	_pc_BufferSave = _pc_Buffer;
	p_ROTR = (GAO_tdst_ModifierROTR *) _pst_Mod->p_Data;

	/* size */
	LOA_ReadLong_Ed(&_pc_Buffer, NULL); // skip size
	p_ROTR->i_1 = LOA_ReadInt(&_pc_Buffer);
	p_ROTR->f_RetardX = LOA_ReadFloat(&_pc_Buffer);
	p_ROTR->f_RetardY = LOA_ReadFloat(&_pc_Buffer);
	p_ROTR->f_RetardZ = LOA_ReadFloat(&_pc_Buffer);
	p_ROTR->f_3 = LOA_ReadFloat(&_pc_Buffer);
	p_ROTR->pst_GO = NULL;

	return _pc_Buffer - _pc_BufferSave;
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef ACTIVE_EDITORS
void GAO_ModifierROTR_Save (MDF_tdst_Modifier * _pst_Mod)
{
	GAO_tdst_ModifierROTR * p_ROTR;
	ULONG ulSize;

	p_ROTR = (GAO_tdst_ModifierROTR *) _pst_Mod->p_Data;

	/* Save Size */
	ulSize = 8;
	SAV_Buffer(&ulSize, 4);

	SAV_Buffer(&p_ROTR->i_1, 4);
	SAV_Buffer(&p_ROTR->f_RetardX, 4);
	SAV_Buffer(&p_ROTR->f_RetardY, 4);
	SAV_Buffer(&p_ROTR->f_RetardZ, 4);
	SAV_Buffer(&p_ROTR->f_3, 4);
}
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
