/*$T AIcateg.c GC! 1.100 03/20/01 15:26:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "MATHs/MATH.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "BASe/MEMory/MEMpro.h"
#include "TIMer/PROfiler/PROPS2.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_EventParam(AI_tdst_Node *_pst_Node)
{
	ERR_X_Assert(EVE_gpst_CurrentParam);
	AI_PushVal(&EVE_gpst_CurrentParam->i_Param1, _pst_Node->w_Param);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Vars
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_EvalOneVar(AI_tdst_Node *_pst_Node, void *_pv_Addr)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar st_Var;
	/*~~~~~~~~~~~~~~~~~~~*/

	st_Var.w_Type = _pst_Node->w_Param;
	st_Var.pv_Addr = _pv_Addr;
	AI_PushVar(&st_Var);
}

/*
 =======================================================================================================================
    Aim:    Call to initialise some values on stack depending on a array. The stack will contain the number of
            dimensions of the array, and the size of each dimension.
 =======================================================================================================================
 */
_inline_ void AI_EvalInitVarArray(AI_tdst_Node *_pst_Node, void *_pv_Addr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					i, iDim;
	int					*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get number of dimensions */
	AI_PopVar(&Val, &st_Var);
	iDim = Val.i;

	/* Get address of array in local stack */
	p = (int *) _pv_Addr;

	/* Save in local stack the size of each dimension */
	for(i = 0; i < iDim; i++)
	{
		AI_PopVar(&Val, &st_Var);	/* Get size of dimensions */
		p[i] = Val.i;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *AI_EvalVarArray_C(int size, void *_pv_Addr, int i1)
{
	return (char *) _pv_Addr + sizeof(int) + (i1 * AI_gast_Types[size].w_Size);
}
/**/
void *AI_EvalVarArray_C2(void *_pv_Addr, int i1)
{
	return (char *) _pv_Addr + sizeof(int) + (i1 * AI_gast_Types[2].w_Size);
}
/**/
void *AI_EvalVarArray_C3(void *_pv_Addr, int i1)
{
	return (char *) _pv_Addr + sizeof(int) + (i1 * AI_gast_Types[3].w_Size);
}
/**/
void *AI_EvalVarArray_C7(void *_pv_Addr, int i1)
{
	return (char *) _pv_Addr + sizeof(int) + (i1 * AI_gast_Types[7].w_Size);
}
/**/
_inline_ void AI_EvalVarArray(AI_tdst_Node *_pst_Node, void *_pv_Addr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);	/* Get index in array */
	AI_Check((UINT) Val.i < ((UINT *) _pv_Addr)[0], "Indice too big for array");

	st_Var.w_Type = _pst_Node->w_Param;
	st_Var.pv_Addr = AI_EvalVarArray_C(st_Var.w_Type, _pv_Addr, Val.i);
	AI_PushVar(&st_Var);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *AI_EvalVarArray2_C(int size, void *_pv_Addr, int i1, int i2)
{
	/*~~~~~~*/
	int iMax1;
	/*~~~~~~*/

	/* Get size of second dimension */
	iMax1 = ((int *) _pv_Addr)[1];
	_pv_Addr = ((char *) _pv_Addr) + 2 * sizeof(int);

	/* Get address of element */
	size = AI_gast_Types[size].w_Size;
	return ((char *) _pv_Addr) +(size * ((i2 * iMax1) + i1));
}
/**/
_inline_ void AI_EvalVarArray2(AI_tdst_Node *_pst_Node, void *_pv_Addr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1); /* Get index in array */
	AI_PopVar(&Val2, &st_Var2); /* Get index in array */
	AI_Check((UINT) Val1.i < ((UINT *) _pv_Addr)[1], "Indice too big for array");
	AI_Check((UINT) Val2.i < ((UINT *) _pv_Addr)[0], "Indice too big for array");

	/* Get address of array in local stack */
	st_Var1.w_Type = _pst_Node->w_Param;
	st_Var1.pv_Addr = AI_EvalVarArray2_C(st_Var1.w_Type, _pv_Addr, Val1.i, Val2.i);
	AI_PushVar(&st_Var1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *AI_EvalVarArray3_C(int size, void *_pv_Addr, int i1, int i2, int i3)
{
	/*~~~~~~~~~~~~~*/
	int iMax1, iMax2;
	/*~~~~~~~~~~~~~*/

	/* Get address of element */
	iMax1 = ((int *) _pv_Addr)[1];
	iMax2 = ((int *) _pv_Addr)[2];
	_pv_Addr = ((char *) _pv_Addr) + 3 * sizeof(int);

	size = AI_gast_Types[size].w_Size;
	return ((char *) _pv_Addr) + size * ((i3 * iMax1 * iMax2) + (i2 * iMax2) + i1);
}
/**/
_inline_ void AI_EvalVarArray3(AI_tdst_Node *_pst_Node, void *_pv_Addr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2, Val3;
	AI_tdst_PushVar		st_Var1, st_Var2, st_Var3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1); /* Get index in array */
	AI_PopVar(&Val2, &st_Var2); /* Get index in array */
	AI_PopVar(&Val3, &st_Var3); /* Get index in array */
	AI_Check((UINT) Val1.i < ((UINT *) _pv_Addr)[2], "Indice too big for array");
	AI_Check((UINT) Val2.i < ((UINT *) _pv_Addr)[1], "Indice too big for array");
	AI_Check((UINT) Val3.i < ((UINT *) _pv_Addr)[0], "Indice too big for array");

	/* Get address of array in local stack */
	st_Var1.w_Type = _pst_Node->w_Param;
	st_Var1.pv_Addr = AI_EvalVarArray3_C(st_Var1.w_Type, _pv_Addr, Val1.i, Val2.i, Val3.i);
	AI_PushVar(&st_Var1);
}

/*$4
 ***********************************************************************************************************************
    Local vars
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_PopProc(AI_tdst_Node *_pst_Node)
{
	void				*p;
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	p = st_Var.pv_Addr;
	L_memcpy(AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param, p, _pst_Node->w_Param);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalCateg_PopProcRef(AI_tdst_Node *_pst_Node)
{
	void				*p;
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	p = st_Var.pv_Addr;
	L_memcpy(AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param, &p, 4);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_LocalVar(AI_tdst_Node *_pst_Node)
{
	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
	AI_EvalOneVar(_pst_Node, AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalCateg_LocalVarRef(AI_tdst_Node *_pst_Node)
{
	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
	AI_EvalOneVar(_pst_Node, *(void **)(AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_InitLocalVarArray(AI_tdst_Node *_pst_Node)
{
	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
	AI_EvalInitVarArray(_pst_Node, AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_LocalVarArray(AI_tdst_Node *_pst_Node)
{
	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
	AI_EvalVarArray(_pst_Node, AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalCateg_LocalVarArrayRef(AI_tdst_Node *_pst_Node)
{
	char *pc;
	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
#ifdef JADEFUSION
	pc = *(char **) (AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
#else
	pc = *(void **) (AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
#endif
	pc -= 4;
	AI_EvalVarArray(_pst_Node, pc);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_LocalVarArray2(AI_tdst_Node *_pst_Node)
{
	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
	AI_EvalVarArray2(_pst_Node, AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalCateg_LocalVarArray2Ref(AI_tdst_Node *_pst_Node)
{
	char *pc;

	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
#ifdef JADEFUSION
	pc = *(char **) (AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
#else
	pc = *(void **) (AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
#endif
	pc -= 8;
	AI_EvalVarArray2(_pst_Node, pc);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_LocalVarArray3(AI_tdst_Node *_pst_Node)
{
	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
	AI_EvalVarArray3(_pst_Node, AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalCateg_LocalVarArray3Ref(AI_tdst_Node *_pst_Node)
{
	char *pc;

	AI_Check((AI_gul_SizeLocalStack + _pst_Node->l_Param + AI_C_MarginLen < AI_C_LenLocalStack), "Local stack is full");
#ifdef JADEFUSION
	pc = *(char **) (AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
#else
	pc = *(void **) (AI_gac_LocalStack + AI_gul_SizeLocalStack + _pst_Node->l_Param);
#endif
	pc -= 12;
	AI_EvalVarArray3(_pst_Node, pc);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Global vars
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_GlobalVar(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentInstance(pst_Instance);
	AI_Check
	(
		(ULONG) _pst_Node->l_Param < pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit,
		"Reference to Object is invalid. Recompile all Models"
	);
	AI_EvalOneVar(_pst_Node, pst_Instance->pc_VarsBuffer + _pst_Node->l_Param);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_GlobalVarArray(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentInstance(pst_Instance);
	AI_Check
	(
		(ULONG) _pst_Node->l_Param < pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit,
		"Reference to Object is invalid. Recompile all Models"
	);
	AI_EvalVarArray(_pst_Node, pst_Instance->pc_VarsBuffer + _pst_Node->l_Param);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_GlobalVarArray2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentInstance(pst_Instance);
	AI_Check
	(
		(ULONG) _pst_Node->l_Param < pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit,
		"Reference to Object is invalid. Recompile all Models"
	);
	AI_EvalVarArray2(_pst_Node, pst_Instance->pc_VarsBuffer + _pst_Node->l_Param);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_GlobalVarArray3(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentInstance(pst_Instance);
	AI_Check
	(
		(ULONG) _pst_Node->l_Param < pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit,
		"Reference to Object is invalid. Recompile all Models"
	);
	AI_EvalVarArray3(_pst_Node, pst_Instance->pc_VarsBuffer + _pst_Node->l_Param);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_Type(AI_tdst_Node *_pst_Node)
{
	AI_PushVal(&_pst_Node->l_Param, _pst_Node->w_Param);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_Keyword(AI_tdst_Node *_pst_Node)
{
	ERR_X_Assert(AI_gast_Keywords[_pst_Node->w_Param].pfn_Func != NULL);
	return AI_gast_Keywords[_pst_Node->w_Param].pfn_Func(_pst_Node);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_Function(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	AI_tdst_Node	*p;
	/*~~~~~~~~~~~~~~~*/

	ERR_X_Assert(AI_gast_Functions[_pst_Node->w_Param].pfn_Func != NULL);

#ifdef AI_FULL_RASTERS
	if(AI_gb_FullRasters) 
	if(!AI_gast_Functions[_pst_Node->w_Param].aaa)
	{
		extern char *_AI_gast_Functions_Names_[];
		PRO_FirstInitTrameRaster
		(
			&AI_gast_Functions[_pst_Node->w_Param].st_Raster,
			"Trame Loop",
			"AI System",
			L_strdup(_AI_gast_Functions_Names_[_pst_Node->w_Param]),
			PRO_E_Time,
			0
		);
		AI_gast_Functions[_pst_Node->w_Param].aaa = 1;
	}

	if(_pst_Node->w_Param != AI_gaw_EnumLink[FUNCTION_AI_EXECUTE])
		if(AI_gb_FullRasters) 
		PRO_StartTrameRaster(&AI_gast_Functions[_pst_Node->w_Param].st_Raster);
#endif

//#ifdef ACTIVE_EDITORS
//__try
//#endif
{
	p = AI_gast_Functions[_pst_Node->w_Param].pfn_Func(_pst_Node);
}
//#ifdef ACTIVE_EDITORS
//__except(1)
//{
//	AI_Check(0, "Engine function exception");
//}
//#endif

#ifdef AI_FULL_RASTERS
	if(_pst_Node->w_Param != AI_gaw_EnumLink[FUNCTION_AI_EXECUTE])
		if(AI_gb_FullRasters) 
		PRO_StopTrameRaster(&AI_gast_Functions[_pst_Node->w_Param].st_Raster);
#endif
	return p;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalCateg_Field(AI_tdst_Node *_pst_Node)
{
	ERR_X_Assert(AI_gast_Fields[_pst_Node->w_Param].pfn_Func != NULL);
	return AI_gast_Fields[_pst_Node->w_Param].pfn_Func(_pst_Node);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
