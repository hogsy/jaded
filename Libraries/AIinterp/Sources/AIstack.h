/*$T AIstack.h GC! 1.097 11/14/00 13:55:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __AISTACK_H__
#define __AISTACK_H__

#include "AIerrid.h"
#include "AIdebug.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ENUTYPE_INT		1	/* AI_gaw_EnumLink[TYPE_INT] */
#define ENUTYPE_FLOAT	2	/* AI_gaw_EnumLink[TYPE_FLOAT] */
#define ENUTYPE_VECTOR	3	/* AI_gaw_EnumLink[TYPE_VECTOR] */

#define MINSIZE			32

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Stack
 -----------------------------------------------------------------------------------------------------------------------
 */

#define AI_C_LenLocalStack		(1024 * 15)
#define AI_C_LenGlobalStack		(1024 * 15)
#define AI_C_LenGlobalVarStack	(1024 * 15)
#define AI_C_LenUserStack		1024
#define AI_C_LenUserVarStack	1024
#define AI_C_MarginLen          200

extern char				AI_gac_LocalStack[AI_C_LenLocalStack];
extern char				AI_gac_GlobalStack[AI_C_LenGlobalStack];
extern char				AI_gac_UserStack[AI_C_LenUserStack];

extern AI_tdst_PushVar	AI_gast_GlobalVarStack[AI_C_LenGlobalVarStack];
extern AI_tdst_PushVar	AI_gast_UserVarStack[AI_C_LenUserVarStack];

extern ULONG			AI_gul_SizeGlobalStack;
extern ULONG			AI_gul_SizeGlobalVarStack;
extern ULONG			AI_gul_SizeLocalStack;
extern ULONG			AI_gul_SizeUserStack;
extern ULONG			AI_gul_SizeUserVarStack;

#ifdef ACTIVE_EDITORS
#include <float.h>
_inline_ void _MATH_CheckFloat(float *val, char *msg)
{
	AI_Check(!_isnan(*val), msg);
}

_inline_ void _MATH_CheckVector(MATH_tdst_Vector *val, char *msg)
{
	AI_Check(!_isnan(val->x), msg);
	AI_Check(!_isnan(val->y), msg);
	AI_Check(!_isnan(val->z), msg);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_GetStoreVar(void *_pv_Src, short _w_Type, void *_pv_Dest)
{
	AI_gast_Types[_w_Type].pfn_FuncGet(_pv_Src, _pv_Dest);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_ResetUnion(AI_tdst_UnionVar *_pst_U)
{
	MATH_InitVectorToZero(&_pst_U->v);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushVal(void *_pv_Src, short _w_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*p;
	register AI_tdst_PushVar	*pa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Store value in global stack */
	p = AI_gac_GlobalStack + AI_gul_SizeGlobalStack;
	AI_GetStoreVar(_pv_Src, _w_Type, p);

	AI_Check(AI_gul_SizeGlobalStack + MINSIZE <= AI_C_LenGlobalStack, AI_ERR_Csz_GlobalStackFull);
	AI_gul_SizeGlobalStack += MINSIZE;

	/* Store var */
	pa = AI_gast_GlobalVarStack + AI_gul_SizeGlobalVarStack;
	pa->w_Type = _w_Type;
	pa->pv_Addr = p;
	AI_gul_SizeGlobalVarStack++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushBool(BOOL _b_Val)
{
	AI_PushVal(&_b_Val, AI_gaw_EnumLink[TYPE_BOOL]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushInt(int _i_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*p;
	register AI_tdst_PushVar	*pa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Store value in global stack */
	p = AI_gac_GlobalStack + AI_gul_SizeGlobalStack;
	* (int *) p = _i_Val;
	AI_Check(AI_gul_SizeGlobalStack + MINSIZE <= AI_C_LenGlobalStack, AI_ERR_Csz_GlobalStackFull);
	AI_gul_SizeGlobalStack += MINSIZE;
	pa = AI_gast_GlobalVarStack + AI_gul_SizeGlobalVarStack;
	pa->w_Type = ENUTYPE_INT;
	pa->pv_Addr = p;
	AI_gul_SizeGlobalVarStack++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushFloat(float _f_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*p;
	register AI_tdst_PushVar	*pa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Store value in global stack */
	p = AI_gac_GlobalStack + AI_gul_SizeGlobalStack;
	* (float *) p = _f_Val;
	AI_Check(AI_gul_SizeGlobalStack + MINSIZE <= AI_C_LenGlobalStack, AI_ERR_Csz_GlobalStackFull);
	AI_gul_SizeGlobalStack += MINSIZE;
	pa = AI_gast_GlobalVarStack + AI_gul_SizeGlobalVarStack;
	pa->w_Type = ENUTYPE_FLOAT;
	pa->pv_Addr = p;
	AI_gul_SizeGlobalVarStack++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushVector(MATH_tdst_Vector *_pst_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*p;
	register AI_tdst_PushVar	*pa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Store value in global stack */
	p = AI_gac_GlobalStack + AI_gul_SizeGlobalStack;
	MATH_CopyVector((MATH_tdst_Vector *) p, _pst_Val);
	AI_Check(AI_gul_SizeGlobalStack + MINSIZE <= AI_C_LenGlobalStack, AI_ERR_Csz_GlobalStackFull);
	AI_gul_SizeGlobalStack += MINSIZE;
	pa = AI_gast_GlobalVarStack + AI_gul_SizeGlobalVarStack;
	pa->w_Type = ENUTYPE_VECTOR;
	pa->pv_Addr = p;
	AI_gul_SizeGlobalVarStack++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushGameObject(OBJ_tdst_GameObject *_pst_Val)
{
	AI_PushVal(&_pst_Val, AI_gaw_EnumLink[TYPE_GAMEOBJECT]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushNetWork(WAY_tdst_Network *_pst_Val)
{
	AI_PushVal(&_pst_Val, AI_gaw_EnumLink[TYPE_NETWORK]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushFunction(AI_tdst_Function *_pst_Val)
{
	AI_PushVal(&_pst_Val, AI_gaw_EnumLink[TYPE_FUNCTIONREF]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushString(char *_psz_Val)
{
	AI_PushVal(&_psz_Val, AI_gaw_EnumLink[TYPE_STRING]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushVar(AI_tdst_PushVar *_pst_Var)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*p;
	register AI_tdst_PushVar	*pa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get value of var */
	p = AI_gac_GlobalStack + AI_gul_SizeGlobalStack;
	AI_GetStoreVar(_pst_Var->pv_Addr, _pst_Var->w_Type, p);
	AI_Check(AI_gul_SizeGlobalStack + MINSIZE <= AI_C_LenGlobalStack, AI_ERR_Csz_GlobalStackFull);
	AI_gul_SizeGlobalStack += MINSIZE;

	/* Store var */
	pa = AI_gast_GlobalVarStack + AI_gul_SizeGlobalVarStack;
	pa->w_Type = _pst_Var->w_Type;
	pa->pv_Addr = _pst_Var->pv_Addr;
	AI_gul_SizeGlobalVarStack++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PopVar(AI_tdst_UnionVar *_pst_Dest, AI_tdst_PushVar *_pst_Var)
{
	/* AI_ResetUnion(_pst_Dest); */
	AI_Check(AI_gul_SizeGlobalVarStack != 0, AI_ERR_Csz_GlobalStackEmpty);
	_pst_Var->w_Type = AI_gast_GlobalVarStack[--AI_gul_SizeGlobalVarStack].w_Type;
	_pst_Var->pv_Addr = AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack].pv_Addr;

	AI_GetStoreVar(_pst_Var->pv_Addr, _pst_Var->w_Type, _pst_Dest);
	AI_Check(AI_gul_SizeGlobalStack >= MINSIZE, AI_ERR_Csz_GlobalStackEmpty);
	AI_gul_SizeGlobalStack -= MINSIZE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PopIntOrFloat(AI_tdst_UnionVar *_pst_Dest, AI_tdst_PushVar *_pst_Var)
{
	AI_Check(AI_gul_SizeGlobalVarStack != 0, AI_ERR_Csz_GlobalStackEmpty);
	_pst_Var->w_Type = AI_gast_GlobalVarStack[--AI_gul_SizeGlobalVarStack].w_Type;
	_pst_Var->pv_Addr = AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack].pv_Addr;

	*(int *) _pst_Dest = *(int *) _pst_Var->pv_Addr;
	AI_Check(AI_gul_SizeGlobalStack >= MINSIZE, AI_ERR_Csz_GlobalStackEmpty);
	AI_gul_SizeGlobalStack -= MINSIZE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int AI_PopBool(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
	return *(BOOL *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int AI_PopInt(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	if(AI_gast_GlobalVarStack[--AI_gul_SizeGlobalVarStack].w_Type == ENUTYPE_FLOAT)
		return lFloatToLong(*(float *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack));
	return *(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int AI_PopValInt(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
	return *(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int AI_PopChar(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
	return *(char *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float AI_PopFloat(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	if(AI_gast_GlobalVarStack[--AI_gul_SizeGlobalVarStack].w_Type == ENUTYPE_INT)
		return fLongToFloat(*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack));
#ifdef ACTIVE_EDITORS
	_MATH_CheckFloat((float *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack), "Nan in AI Stack[Float]");
#endif
	return *(float *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ AI_tdst_Model *AI_PopModel(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
	return *(AI_tdst_Model **) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PopVector(MATH_tdst_Vector *_pst_Dest)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
#ifdef ACTIVE_EDITORS
	_MATH_CheckVector((MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack), "Nan in AI Stack[Vector]");
#endif

	MATH_CopyVector(_pst_Dest, (MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *AI_PopVectorPtr(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
#ifdef ACTIVE_EDITORS
	_MATH_CheckVector((MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack), "Nan in AI Stack[VectorPtr]");
#endif

	return (MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ TEXT_tdst_Eval *AI_PopTextPtr(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
	return (TEXT_tdst_Eval *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

extern char *AI_EvalFunc_TEXTToString_C(TEXT_tdst_Eval *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ char *AI_PopStringPtr(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	if(st_Var.w_Type == AI_gaw_EnumLink[TYPE_STRING]) return (char *) Val.i;
	return AI_EvalFunc_TEXTToString_C((TEXT_tdst_Eval *) &Val.t);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ OBJ_tdst_GameObject *AI_PopGameObject(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
	AI_Check((int) * (OBJ_tdst_GameObject **) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack) != -1, "Unknown reference");
	return *(OBJ_tdst_GameObject **) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ WAY_tdst_Network *AI_PopNetwork(void)
{
	AI_gul_SizeGlobalStack -= MINSIZE;
	--AI_gul_SizeGlobalVarStack;
	AI_Check((int) * (WAY_tdst_Network **) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack) != -1, "Unknown reference");
	return *(WAY_tdst_Network **) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushUserVal(void *_pv_Src, short _w_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*p;
	register AI_tdst_PushVar	*pa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Store value in global stack */
	p = AI_gac_UserStack + AI_gul_SizeUserStack;
	AI_GetStoreVar(_pv_Src, _w_Type, p);

	AI_Check
	(
		AI_gul_SizeUserStack + (ULONG) AI_gast_Types[_w_Type].w_Size <= AI_C_LenUserStack,
		AI_ERR_Csz_UserStackFull
	);
	AI_gul_SizeUserStack += AI_gast_Types[_w_Type].w_Size;

	/* Store var */
	pa = AI_gast_UserVarStack + AI_gul_SizeUserVarStack;
	pa->w_Type = _w_Type;
	pa->pv_Addr = p;
	AI_gul_SizeUserVarStack++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PushUserVar(AI_tdst_PushVar *_pst_Var)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*p;
	register AI_tdst_PushVar	*pa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get value of var */
	p = AI_gac_UserStack + AI_gul_SizeUserStack;
	AI_GetStoreVar(_pst_Var->pv_Addr, _pst_Var->w_Type, p);

	AI_Check
	(
		AI_gul_SizeUserStack + (ULONG) AI_gast_Types[_pst_Var->w_Type].w_Size <= AI_C_LenUserStack,
		AI_ERR_Csz_UserStackFull
	);
	AI_gul_SizeUserStack += AI_gast_Types[_pst_Var->w_Type].w_Size;

	/* Store var */
	pa = AI_gast_UserVarStack + AI_gul_SizeUserVarStack;
	pa->w_Type = _pst_Var->w_Type;
	pa->pv_Addr = _pst_Var->pv_Addr;
	AI_gul_SizeUserVarStack++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void AI_PopUserVar(AI_tdst_UnionVar *_pst_Dest, AI_tdst_PushVar *_pst_Var)
{
	AI_ResetUnion(_pst_Dest);

	AI_Check(AI_gul_SizeUserVarStack != 0, AI_ERR_Csz_UserStackEmpty);
	_pst_Var->w_Type = AI_gast_UserVarStack[--AI_gul_SizeUserVarStack].w_Type;
	_pst_Var->pv_Addr = AI_gast_UserVarStack[AI_gul_SizeUserVarStack].pv_Addr;

	AI_GetStoreVar(_pst_Var->pv_Addr, _pst_Var->w_Type, _pst_Dest);

	AI_Check(AI_gul_SizeUserStack >= (ULONG) AI_gast_Types[_pst_Var->w_Type].w_Size, AI_ERR_Csz_UserStackEmpty);
	AI_gul_SizeUserStack -= AI_gast_Types[_pst_Var->w_Type].w_Size;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __AISTACK_H__ */
