/*$T AIkeywords.c GC 1.139 03/01/04 10:30:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "MATHs/MATH.h"
#include "AIinterp/Sources/AIstack.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/TIMdefs.h"
#include "AIinterp/Sources/AIdebug.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
USHORT	gu16_ProcLink = 1;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Ultra(AI_tdst_Node *_pst_Node)
{
	AI_gpst_CurrentUltra = AI_PopGameObject();
#ifdef ACTIVE_EDITORS
__try
#endif
{

	/* ul_Key = *(LONG*) &(*((*AI_gpst_CurrentFunction).pst_OtherRootNode)).w_Param; */
	AI_Check(AI_gpst_CurrentUltra, "Ultra operator is invalid");
	if((AI_tdst_Instance *) AI_gpst_CurrentUltra == WOR_gst_Universe.pst_AI)
	{
		AI_gpst_CurrentInstanceUltra = WOR_gst_Universe.pst_AI;
		AI_gpst_CurrentUltra = NULL;
	}
	else
	{
		if(AI_gpst_CurrentUltra->pst_Extended)
			AI_gpst_CurrentInstanceUltra = AI_gpst_CurrentUltra->pst_Extended->pst_Ai;
		else
			AI_gpst_CurrentInstanceUltra = NULL;
	}

}
#ifdef ACTIVE_EDITORS
__except(1)
{
	AI_Check(0, "Bad ultra operator");
}
#endif

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_ObjEqual(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_GO1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	pst_GO1 = AI_PopGameObject();
	AI_EvalType_GetGAO(&pst_GO, &pst_GO);
	AI_EvalType_GetGAO(&pst_GO1, &pst_GO1);
	if(pst_GO == pst_GO1)
		AI_PushInt(1);
	else
		AI_PushInt(0);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_ObjDifferent(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_GO1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	pst_GO1 = AI_PopGameObject();
	AI_EvalType_GetGAO(&pst_GO, &pst_GO);
	AI_EvalType_GetGAO(&pst_GO1, &pst_GO1);
	if(pst_GO != pst_GO1)
		AI_PushInt(1);
	else
		AI_PushInt(0);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_ObjNot(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	AI_EvalType_GetGAO(&pst_GO, &pst_GO);
	if(pst_GO)
		AI_PushInt(0);
	else
		AI_PushInt(1);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Return(AI_tdst_Node *_pst_Node)
{
	*AI_gppst_CurrentJumpNode = NULL;	/* Return stop current meta action */
	return (AI_tdst_Node *) AI_CR_StopReturn;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_ReturnTrack(AI_tdst_Node *_pst_Node)
{
	*AI_gppst_CurrentJumpNode = NULL;	/* Return stop current meta action */
	return (AI_tdst_Node *) AI_CR_StopReturnTrack;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Stop(AI_tdst_Node *_pst_Node)
{
	*AI_gppst_CurrentJumpNode = NULL;	/* Stop stop current meta action */
	return (AI_tdst_Node *) AI_CR_StopInstance;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Push(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_PushUserVal(&Val, st_Var.w_Type);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_PushInt(int i)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar st_Var;
	/*~~~~~~~~~~~~~~~~~~~*/

	st_Var.w_Type = ENUTYPE_INT;
	AI_PushUserVal(&i, st_Var.w_Type);
}
/**/
int AI2C_PopInt(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopUserVar(&Val, &st_Var);
	if(st_Var.w_Type == ENUTYPE_FLOAT) return (int) Val.f;
	return Val.i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_PushGameObject(OBJ_tdst_GameObject *p)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar st_Var;
	/*~~~~~~~~~~~~~~~~~~~*/

	st_Var.w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	AI_PushUserVal(&p, st_Var.w_Type);
}
/**/
OBJ_tdst_GameObject *AI2C_PopGameObject(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopUserVar(&Val, &st_Var);
	return (OBJ_tdst_GameObject *) Val.i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_PushFloat(float f)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar st_Var;
	/*~~~~~~~~~~~~~~~~~~~*/

	st_Var.w_Type = ENUTYPE_FLOAT;
	AI_PushUserVal(&f, st_Var.w_Type);
}
/**/
float AI2C_PopFloat(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopUserVar(&Val, &st_Var);
	if(st_Var.w_Type == ENUTYPE_INT) return (float) Val.i;
	return Val.f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_PushVector(MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar st_Var;
	/*~~~~~~~~~~~~~~~~~~~*/

	st_Var.w_Type = ENUTYPE_VECTOR;
	AI_PushUserVal(v, st_Var.w_Type);
}
/**/
void AI2C_PopVector(MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopUserVar(&Val, &st_Var);
	MATH_CopyVector(v, (MATH_tdst_Vector *) st_Var.pv_Addr);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_PushMessageId(AI_tdst_GlobalMessageId *v)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar st_Var;
	/*~~~~~~~~~~~~~~~~~~~*/

	st_Var.w_Type = AI_gaw_EnumLink[TYPE_MESSAGEID];
	AI_PushUserVal(v, st_Var.w_Type);
}
/**/
void AI2C_PopMessageId(AI_tdst_GlobalMessageId *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopUserVar(&Val, &st_Var);
	v->id = ((AI_tdst_GlobalMessageId *) st_Var.pv_Addr)->id;
	v->msgid = ((AI_tdst_GlobalMessageId *) st_Var.pv_Addr)->msgid;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Pop(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopUserVar(&Val, &st_Var);
	AI_PushVal(&Val, st_Var.w_Type);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    JUMP
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Meta(AI_tdst_Node *_pst_Node)
{
	*AI_gppst_CurrentJumpNode = _pst_Node + _pst_Node->l_Param;
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_JumpFalse(AI_tdst_Node *_pst_Node)
{
	/*~~*/
	int i;
	/*~~*/

	i = AI_PopValInt();
	AI_PushInt(i);
	if(i) return ++_pst_Node;
	return _pst_Node + _pst_Node->l_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_JumpFF(AI_tdst_Node *_pst_Node)
{
	AI_PushBool(FALSE);
	return _pst_Node + _pst_Node->l_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_JumpTrue(AI_tdst_Node *_pst_Node)
{
	/*~~*/
	int i;
	/*~~*/

	i = AI_PopValInt();
	AI_PushInt(i);
	if(!i) return ++_pst_Node;
	return _pst_Node + _pst_Node->l_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_JumpFalseSP(AI_tdst_Node *_pst_Node)
{
	if(AI_PopValInt()) return ++_pst_Node;
	return _pst_Node + _pst_Node->l_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_JumpTrueSP(AI_tdst_Node *_pst_Node)
{
	if(!AI_PopValInt()) return ++_pst_Node;
	return _pst_Node + _pst_Node->l_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Jump(AI_tdst_Node *_pst_Node)
{
	return _pst_Node + _pst_Node->l_Param;
}

/*$3
 =======================================================================================================================
    Different operations on int and float
 =======================================================================================================================
 */

#define AI_COMPARE_FLOAT_DIFFERENT(a, b)	(!fEq(a, b))
#define AI_COMPARE_FLOAT_EQUAL(a, b)		(fEq(a, b))
#define AI_COMPARE_FLOAT_GREATEQ(a, b)		(fSupEq(a, b))
#define AI_COMPARE_FLOAT_LESSEQ(a, b)		(fInfEq(a, b))
#define AI_COMPARE_FLOAT_GREAT(a, b)		(fSup(a, b))
#define AI_COMPARE_FLOAT_LESS(a, b)			(fInf(a, b))
#define AI_COMPARE_INT_DIFFERENT(a, b)		((a) != (b))
#define AI_COMPARE_INT_EQUAL(a, b)			((a) == (b))
#define AI_COMPARE_INT_GREATEQ(a, b)		((a) >= (b))
#define AI_COMPARE_INT_LESSEQ(a, b)			((a) <= (b))
#define AI_COMPARE_INT_GREAT(a, b)			((a) > (b))
#define AI_COMPARE_INT_LESS(a, b)			((a) < (b))

/*
 =======================================================================================================================
    Aim: Make an operation on 2 values (int or float).
 =======================================================================================================================
 */
#define AI_COMPARE(Oper) \
	{ \
		AI_tdst_UnionVar	Val1, Val2; \
		AI_tdst_PushVar		st_Var1, st_Var2; \
		AI_PopVar(&Val1, &st_Var1); \
		AI_PopVar(&Val2, &st_Var2); \
		if(IntOrFloat2(&Val2, &Val1, &st_Var2, &st_Var1)) \
			AI_PushBool(AI_COMPARE_FLOAT_##Oper(Val2.f, Val1.f)); \
		else \
			AI_PushBool(AI_COMPARE_INT_##Oper(Val2.i, Val1.i)); \
		return ++_pst_Node; \
	}
/*$off*/
AI_tdst_Node *AI_EvalKeyword_Different(AI_tdst_Node *_pst_Node)				{ AI_COMPARE(DIFFERENT); }
AI_tdst_Node *AI_EvalKeyword_Equal(AI_tdst_Node *_pst_Node)					{ AI_COMPARE(EQUAL); }
AI_tdst_Node *AI_EvalKeyword_GreatEq(AI_tdst_Node *_pst_Node)				{ AI_COMPARE(GREATEQ); }
AI_tdst_Node *AI_EvalKeyword_Great(AI_tdst_Node *_pst_Node)					{ AI_COMPARE(GREAT); }
AI_tdst_Node *AI_EvalKeyword_LessEq(AI_tdst_Node *_pst_Node)				{ AI_COMPARE(LESSEQ); }
AI_tdst_Node *AI_EvalKeyword_Less(AI_tdst_Node *_pst_Node)					{ AI_COMPARE(LESS); }
/*$on*/

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Not(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);

	if(st_Var.w_Type == ENUTYPE_FLOAT)
		Val.f = Val.f ? Cf_Zero : Cf_One;
	else
		Val.i = Val.i ? 0 : 0xFF;
	AI_PushVal(&Val, st_Var.w_Type);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Neg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);

	if(st_Var.w_Type == ENUTYPE_FLOAT)
		Val.f = fNeg(Val.f);
	else
		Val.i = -Val.i;
	AI_PushVal(&Val, st_Var.w_Type);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_VecNeg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	MATH_NegEqualVector(&Val.v);
	AI_PushVector(&Val.v);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_VecEqual(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Vec1, *pst_Vec2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Vec1 = AI_PopVectorPtr();
	pst_Vec2 = AI_PopVectorPtr();
	if((pst_Vec1->x != pst_Vec2->x) || (pst_Vec1->y != pst_Vec2->y) || (pst_Vec1->z != pst_Vec2->z))
	{
		AI_PushInt(0);
	}
	else
	{
		AI_PushInt(1);
	}

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_VecDifferent(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Vec1, *pst_Vec2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Vec1 = AI_PopVectorPtr();
	pst_Vec2 = AI_PopVectorPtr();
	if((pst_Vec1->x == pst_Vec2->x) && (pst_Vec1->y == pst_Vec2->y) && (pst_Vec1->z == pst_Vec2->z))
	{
		AI_PushInt(0);
	}
	else
	{
		AI_PushInt(1);
	}

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
AI_tdst_Node *AI_EvalKeyword_IncAffect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);

	if(st_Var.w_Type == ENUTYPE_FLOAT)
		Val.f = fAdd(Val.f, Cf_One);
	else
		Val.i++;

	/* Store value in var */
	AI_GetStoreVar(&Val, st_Var.w_Type, st_Var.pv_Addr);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_DecAffect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);

	if(st_Var.w_Type == ENUTYPE_FLOAT)
		Val.f = fSub(Val.f, Cf_One);
	else
		Val.i--;

	/* Store value in var */
	AI_GetStoreVar(&Val, st_Var.w_Type, st_Var.pv_Addr);

	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    OPERATOR FOR INT/REAL
 ***********************************************************************************************************************
 */

/*$3
 =======================================================================================================================
    Different operations on int and float
 =======================================================================================================================
 */

#define AI_OPER_FLOAT_AFFECT(a, b)	(b)
#define AI_OPER_FLOAT_MINUS(a, b)	(fSub(a, b))
#define AI_OPER_FLOAT_PLUS(a, b)	(fAdd(a, b))
#define AI_OPER_FLOAT_MUL(a, b)		(fMul(a, b))
#define AI_OPER_FLOAT_DIV(a, b)		(fDiv(a, b))
#define AI_OPER_FLOAT_OR(a, b)		(float) (lFloatToLong(a) | lFloatToLong(b))
#define AI_OPER_FLOAT_AND(a, b)		(float) (lFloatToLong(a) & lFloatToLong(b))
#define AI_OPER_FLOAT_XOR(a, b)		(float) (lFloatToLong(a) ^ lFloatToLong(b))
#define AI_OPER_FLOAT_SLEFT(a, b)	(float) (lFloatToLong(a) << lFloatToLong(b))
#define AI_OPER_FLOAT_SRIGHT(a, b)	(float) (lFloatToLong(a) >> lFloatToLong(b))
#define AI_OPER_INT_AFFECT(a, b)	(b)
#define AI_OPER_INT_MINUS(a, b)		((a) - (b))
#define AI_OPER_INT_PLUS(a, b)		((a) + (b))
#define AI_OPER_INT_MUL(a, b)		((a) * (b))
#define AI_OPER_INT_DIV(a, b)		((a) / (b))
#define AI_OPER_INT_OR(a, b)		((a) | (b))
#define AI_OPER_INT_AND(a, b)		((a) & (b))
#define AI_OPER_INT_XOR(a, b)		((a) ^ (b))
#define AI_OPER_INT_SLEFT(a, b)		((a) << (b))
#define AI_OPER_INT_SRIGHT(a, b)	((a) >> (b))

/*
 =======================================================================================================================
    Aim: Make an operation on 2 values (int or float).
 =======================================================================================================================
 */
#define AI_OPER(Oper) \
	{ \
		AI_tdst_UnionVar	Val1, Val2; \
		AI_tdst_PushVar		st_Var1, st_Var2; \
		AI_PopIntOrFloat(&Val1, &st_Var1); \
		AI_PopIntOrFloat(&Val2, &st_Var2); \
		if(IntOrFloat2FFloat(&Val2, &Val1, &st_Var2, &st_Var1)) \
		{ \
			Val1.f = AI_OPER_FLOAT_##Oper(Val2.f, Val1.f); \
			AI_PushFloat(Val1.f); \
		} \
		else \
		{ \
			Val1.i = AI_OPER_INT_##Oper(Val2.i, Val1.i); \
			AI_PushInt(Val1.i); \
		} \
		return ++_pst_Node; \
	}
/*$off*/
AI_tdst_Node *AI_EvalKeyword_Minus(AI_tdst_Node *_pst_Node)		{ AI_OPER(MINUS); }
AI_tdst_Node *AI_EvalKeyword_Plus(AI_tdst_Node *_pst_Node)		{ AI_OPER(PLUS); }
AI_tdst_Node *AI_EvalKeyword_Mul(AI_tdst_Node *_pst_Node)		{ AI_OPER(MUL); }
AI_tdst_Node *AI_EvalKeyword_Div(AI_tdst_Node *_pst_Node)		{ AI_OPER(DIV); }
AI_tdst_Node *AI_EvalKeyword_OrOp(AI_tdst_Node *_pst_Node)		{ AI_OPER(OR); }
AI_tdst_Node *AI_EvalKeyword_AndOp(AI_tdst_Node *_pst_Node)		{ AI_OPER(AND); }
AI_tdst_Node *AI_EvalKeyword_XorOp(AI_tdst_Node *_pst_Node)		{ AI_OPER(XOR); }
AI_tdst_Node *AI_EvalKeyword_SLeft(AI_tdst_Node *_pst_Node)		{ AI_OPER(SLEFT); }
AI_tdst_Node *AI_EvalKeyword_SRight(AI_tdst_Node *_pst_Node)	{ AI_OPER(SRIGHT); }
/*$on*/

/*
 =======================================================================================================================
    Aim: Make an operation on 2 values (int or float).
 =======================================================================================================================
 */
#define AI_AUTOOPER(Oper) \
	{ \
		AI_tdst_UnionVar	Val1, Val2; \
		AI_tdst_PushVar		st_Var1, st_Var2; \
		AI_PopIntOrFloat(&Val1, &st_Var1); \
		AI_PopIntOrFloat(&Val2, &st_Var2); \
		if(IntOrFloat2(&Val2, &Val1, &st_Var2, &st_Var1)) \
			Val2.f = AI_OPER_FLOAT_##Oper(Val2.f, Val1.f); \
		else \
			Val2.i = AI_OPER_INT_##Oper(Val2.i, Val1.i); \
		*(int *) st_Var2.pv_Addr = *(int *) &Val2; \
		return ++_pst_Node; \
	}

/*$off*/
AI_tdst_Node *AI_EvalKeyword_Affect(AI_tdst_Node *_pst_Node)		{ AI_AUTOOPER(AFFECT); }
AI_tdst_Node *AI_EvalKeyword_MinusAffect(AI_tdst_Node *_pst_Node)	{ AI_AUTOOPER(MINUS); }
AI_tdst_Node *AI_EvalKeyword_PlusAffect(AI_tdst_Node *_pst_Node)	{ AI_AUTOOPER(PLUS); }
AI_tdst_Node *AI_EvalKeyword_MulAffect(AI_tdst_Node *_pst_Node)		{ AI_AUTOOPER(MUL); }
AI_tdst_Node *AI_EvalKeyword_DivAffect(AI_tdst_Node *_pst_Node)		{ AI_AUTOOPER(DIV); }
AI_tdst_Node *AI_EvalKeyword_OrAffect(AI_tdst_Node *_pst_Node)		{ AI_AUTOOPER(OR); }
AI_tdst_Node *AI_EvalKeyword_AndAffect(AI_tdst_Node *_pst_Node)		{ AI_AUTOOPER(AND); }
AI_tdst_Node *AI_EvalKeyword_XorAffect(AI_tdst_Node *_pst_Node)		{ AI_AUTOOPER(XOR); }
AI_tdst_Node *AI_EvalKeyword_SLeftAffect(AI_tdst_Node *_pst_Node)	{ AI_AUTOOPER(SLEFT); }
AI_tdst_Node *AI_EvalKeyword_SRightAffect(AI_tdst_Node *_pst_Node)	{ AI_AUTOOPER(SRIGHT); }
/*$on*/
#define AI_AUTOOPERP(Oper) \
	{ \
		AI_tdst_UnionVar	Val1, Val2; \
		AI_tdst_PushVar		st_Var1, st_Var2; \
		AI_PopIntOrFloat(&Val2, &st_Var2); \
		AI_PopIntOrFloat(&Val1, &st_Var1); \
		if(IntOrFloat2(&Val2, &Val1, &st_Var2, &st_Var1)) \
			Val2.f = AI_OPER_FLOAT_##Oper(Val2.f, Val1.f); \
		else \
			Val2.i = AI_OPER_INT_##Oper(Val2.i, Val1.i); \
		*(int *) st_Var2.pv_Addr = *(int *) &Val2; \
		return ++_pst_Node; \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_AffectP(AI_tdst_Node *_pst_Node)
{
	AI_AUTOOPERP(AFFECT);
}

/*$4
 ***********************************************************************************************************************
    OPERATOR FOR VECTORS
 ***********************************************************************************************************************
 */

/*$3
 =======================================================================================================================
    Different operations on vectors
 =======================================================================================================================
 */

#define AI_VECOPER_AFFECT(d, a, b)	(MATH_CopyVector(d, b))
#define AI_VECOPER_MINUS(d, a, b)	(MATH_SubVector(d, a, b))
#define AI_VECOPER_PLUS(d, a, b)	(MATH_AddVector(d, a, b))
#define AI_VECOPER_MUL(d, a, b)		(MATH_ScaleVector(d, a, b))
#define AI_VECOPER_DIV(d, a, b)		(MATH_ScaleVector(d, a, fInv(b)))

/*
 =======================================================================================================================
    Aim: Make an operation on 2 values (int or float).
 =======================================================================================================================
 */
#define AI_VECOPER(Oper) \
	{ \
		MATH_tdst_Vector	*p1, *p2; \
		p1 = AI_PopVectorPtr(); \
		p2 = AI_PopVectorPtr(); \
		AI_VECOPER_##Oper(p2, p2, p1); \
		AI_PushVector(p2); \
		return ++_pst_Node; \
	}
#define AI_VECOPERF(Oper) \
	{ \
		AI_tdst_UnionVar	Val1, Val2; \
		AI_tdst_PushVar		st_Var1, st_Var2; \
		AI_PopVar(&Val1, &st_Var1); \
		AI_PopVar(&Val2, &st_Var2); \
		if(st_Var2.w_Type == ENUTYPE_VECTOR) \
		{ \
			IntToFloat1(&Val1, &st_Var1); \
			AI_VECOPER_##Oper(&Val2.v, &Val2.v, Val1.f); \
			AI_PushVector(&Val2.v); \
		} \
		else \
		{ \
			IntToFloat1(&Val2, &st_Var2); \
			AI_VECOPER_##Oper(&Val1.v, &Val1.v, Val2.f); \
			AI_PushVector(&Val1.v); \
		} \
		return ++_pst_Node; \
	}
#define AI_VECOPERF2(Oper) \
	{ \
		AI_tdst_UnionVar	Val1, Val2; \
		AI_tdst_PushVar		st_Var1, st_Var2; \
		AI_PopVar(&Val1, &st_Var1); \
		AI_PopVar(&Val2, &st_Var2); \
		IntToFloat1(&Val2, &st_Var2); \
		AI_VECOPER_##Oper(&Val1.v, &Val1.v, Val2.f); \
		AI_PushVector(&Val1.v); \
		return ++_pst_Node; \
	}
/*$off*/
AI_tdst_Node *AI_EvalKeyword_VecMinus(AI_tdst_Node *_pst_Node)		{ AI_VECOPER(MINUS); }
AI_tdst_Node *AI_EvalKeyword_VecPlus(AI_tdst_Node *_pst_Node)		{ AI_VECOPER(PLUS); }
AI_tdst_Node *AI_EvalKeyword_VecMul(AI_tdst_Node *_pst_Node)		{ AI_VECOPERF(MUL); }
AI_tdst_Node *AI_EvalKeyword_VecMul2(AI_tdst_Node *_pst_Node)		{ AI_VECOPERF2(MUL); }

AI_tdst_Node *AI_EvalKeyword_VecDiv(AI_tdst_Node *_pst_Node)
{
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val2, &st_Var2);
	if(st_Var2.w_Type == ENUTYPE_VECTOR)
	{
		IntToFloat1(&Val1, &st_Var1);
		AI_Check(Val1.f, "Dividing by a null value");
		AI_VECOPER_DIV(&Val2.v, &Val2.v, Val1.f);
		AI_PushVector(&Val2.v);
	}
	else
	{
		IntToFloat1(&Val2, &st_Var2);
		AI_Check(Val2.f, "Dividing by a null value");
		AI_VECOPER_DIV(&Val1.v, &Val1.v, Val2.f);
		AI_PushVector(&Val1.v);
	}
	return ++_pst_Node;
}
/*$on*/

/*
 =======================================================================================================================
    Aim: Make an operation on 2 values (int or float).
 =======================================================================================================================
 */
#define AI_VECAUTOOPER(Oper) \
	{ \
		MATH_tdst_Vector	*p1; \
		AI_tdst_UnionVar	Val2; \
		AI_tdst_PushVar		st_Var2; \
		p1 = AI_PopVectorPtr(); \
		AI_PopVar(&Val2, &st_Var2); \
		AI_VECOPER_##Oper((MATH_tdst_Vector *) st_Var2.pv_Addr, &Val2.v, p1); \
		return ++_pst_Node; \
	}
#define AI_VECAUTOOPERF(Oper) \
	{ \
		float				f1; \
		AI_tdst_UnionVar	Val2; \
		AI_tdst_PushVar		st_Var2; \
		f1 = AI_PopFloat(); \
		AI_PopVar(&Val2, &st_Var2); \
		AI_VECOPER_##Oper((MATH_tdst_Vector *) st_Var2.pv_Addr, &Val2.v, f1); \
		return ++_pst_Node; \
	}

/*$off*/
AI_tdst_Node *AI_EvalKeyword_VecAffect(AI_tdst_Node *_pst_Node)			{ AI_VECAUTOOPER(AFFECT); }
AI_tdst_Node *AI_EvalKeyword_VecMinusAffect(AI_tdst_Node *_pst_Node)	{ AI_VECAUTOOPER(MINUS); }
AI_tdst_Node *AI_EvalKeyword_VecPlusAffect(AI_tdst_Node *_pst_Node)		{ AI_VECAUTOOPER(PLUS); }
AI_tdst_Node *AI_EvalKeyword_VecMulAffect(AI_tdst_Node *_pst_Node)		{ AI_VECAUTOOPERF(MUL); }

AI_tdst_Node *AI_EvalKeyword_VecDivAffect(AI_tdst_Node *_pst_Node)
{
	float				f1;
	AI_tdst_UnionVar	Val2;
	AI_tdst_PushVar		st_Var2;
	f1 = AI_PopFloat();
	AI_Check(f1, "Dividing by a null value");
	AI_PopVar(&Val2, &st_Var2);
	AI_VECOPER_DIV((MATH_tdst_Vector *) st_Var2.pv_Addr, &Val2.v, f1);
	return ++_pst_Node;
}
/*$on*/
#define AI_VECAUTOOPERP(Oper) \
	{ \
		MATH_tdst_Vector	*p1; \
		AI_tdst_UnionVar	Val2; \
		AI_tdst_PushVar		st_Var2; \
		AI_PopVar(&Val2, &st_Var2); \
		p1 = AI_PopVectorPtr(); \
		AI_VECOPER_##Oper((MATH_tdst_Vector *) st_Var2.pv_Addr, &Val2.v, p1); \
		return ++_pst_Node; \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_VecAffectP(AI_tdst_Node *_pst_Node)
{
	AI_VECAUTOOPERP(AFFECT);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$3
 =======================================================================================================================
 =======================================================================================================================
 */

AI_tdst_Node *AI_EvalKeyword_MsgAffect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val2, &st_Var2);
	L_memcpy(&Val2.m, &Val1.m, sizeof(AI_tdst_Message));
	AI_GetStoreVar(&Val2, st_Var2.w_Type, st_Var2.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_MsgAffectP(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val2, &st_Var2);
	AI_PopVar(&Val1, &st_Var1);
	L_memcpy(&Val2.m, &Val1.m, sizeof(AI_tdst_Message));
	AI_GetStoreVar(&Val2, st_Var2.w_Type, st_Var2.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_MsgIdAffect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val2, &st_Var2);
	L_memcpy(&Val2.t, &Val1.t, sizeof(AI_tdst_GlobalMessageId));
	AI_GetStoreVar(&Val2, st_Var2.w_Type, st_Var2.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_TriggerAffect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val2, &st_Var2);
	L_memcpy(&Val2.t, &Val1.t, sizeof(SCR_tt_Trigger));
	AI_GetStoreVar(&Val2, st_Var2.w_Type, st_Var2.pv_Addr);
	return ++_pst_Node;
}

/*$3
 =======================================================================================================================
 =======================================================================================================================
 */

AI_tdst_Node *AI_EvalKeyword_TextAffect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val2, &st_Var2);
	L_memcpy(&Val2.t, &Val1.t, sizeof(TEXT_tdst_Eval));
	AI_GetStoreVar(&Val2, st_Var2.w_Type, st_Var2.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_TextAffectP(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val2, &st_Var2);
	AI_PopVar(&Val1, &st_Var1);
	L_memcpy(&Val2.t, &Val1.t, sizeof(TEXT_tdst_Eval));
	AI_GetStoreVar(&Val2, st_Var2.w_Type, st_Var2.pv_Addr);
	return ++_pst_Node;
}

/*$3
 =======================================================================================================================
 =======================================================================================================================
 */

AI_tdst_Node *AI_EvalKeyword_TextPlus(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
    int id, id2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val2, &st_Var2);
	AI_PopVar(&Val1, &st_Var1);

    id = TEXT_i_GetOneTextIndex(*(ULONG*)&Val1.t.i_FileKey);
    AI_Check(id!=-1, "Text is not loaded");

    id2 = TEXT_i_GetEntryIndex(TEXT_gst_Global.pst_AllTexts[id], Val1.t.i_Id);
    AI_Check(id!=-1, "Text entry doesnt exist");

    id2 += Val2.i;

    Val1.t.i_Id = *(int*)&TEXT_gst_Global.pst_AllTexts[id]->pst_Ids[id2].ul_IdKey;

	AI_PushVal(&Val1, st_Var1.w_Type);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_call(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Model		*pst_Model;
	ULONG				i, j;
	SCR_tt_Procedure	*pt_Proc;
	AI_tdst_Function	t_Func;
	int					res;
	AI_tdst_Instance	*pst_SaveInst;
	OBJ_tdst_GameObject	*pst_SaveObj;
	extern int			AI_EvalFunc_AIExecute_C(AI_tdst_Function *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(_pst_Node[2].w_Param != gu16_ProcLink)
	{
		_pst_Node->l_Param = -1;
		_pst_Node[2].w_Param = gu16_ProcLink;
	}
#endif
	if(_pst_Node->l_Param == -1)
	{
		_pst_Node->l_Param = -1;
		pst_Model = AI_gpst_CurrentInstanceUltra->pst_Model;
		for(i = 0; i < pst_Model->u32_NbProcList; i++)
		{
			for(j = 0; j < pst_Model->pp_ProcList[i]->u16_Num; j++)
			{
				if(!L_strcmp(pst_Model->pp_ProcList[i]->pt_All[j].pz_Name, (char *) (_pst_Node + 3)))
				{
					_pst_Node->l_Param = (int) (pst_Model->pp_ProcList[i]->pt_All + j);
					break;
				}
			}
		}
	}

	AI_Check(_pst_Node->l_Param != -1, "Invalid procedure reference");

	pt_Proc = (SCR_tt_Procedure *) _pst_Node->l_Param;
	L_memset(&t_Func, 0, sizeof(t_Func));
#ifdef ACTIVE_EDITORS
	L_strcpy(t_Func.az_Name, pt_Proc->pz_Name);
	t_Func.h_File = pt_Proc->pt_ProcList->h_File;
	t_Func.pst_OtherRootNode = pt_Proc->pu32_DbgNodes;
#endif
	t_Func.i_SizeLocalStack = pt_Proc->u16_SizeLocal;
	t_Func.pst_RootNode = pt_Proc->pu32_Nodes;

#ifdef AI_FULL_RASTERS
	t_Func.aaa = 1;
	if(AI_gb_FullRasters) 
	if(!pt_Proc->aaa)
	{
		PRO_FirstInitTrameRaster
		(
			&pt_Proc->st_Raster,
			"Trame Loop",
			"AI Procedures",
			L_strdup(pt_Proc->pz_Name),
			PRO_E_Time,
			0
		);

		pt_Proc->aaa = 1;
	}

	if(AI_gb_FullRasters)
	PRO_StartTrameRaster(&pt_Proc->st_Raster);
#endif

	pst_SaveInst = AI_gpst_CurrentInstance;
	pst_SaveObj = AI_gpst_CurrentGameObject;
	AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra;
	AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra;
	res = AI_EvalFunc_AIExecute_C(&t_Func);
	AI_gpst_CurrentInstance = pst_SaveInst;
	AI_gpst_CurrentGameObject = pst_SaveObj;
	AI_gpst_CurrentInstanceUltra = pst_SaveInst;
	AI_gpst_CurrentUltra = pst_SaveObj;

#ifdef AI_FULL_RASTERS
	if(AI_gb_FullRasters) 
	PRO_StopTrameRaster(&pt_Proc->st_Raster);
#endif

	ERR_X_Warning(res != AI_CR_Destroy, "You must not suicide an object in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_CurChangeNow, "You must not change current track in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_StopReturnTrack, "You must not use stop in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_StopAI, "You must not use stop in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_StopInstance, "You must not use stop in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	
	if(res && res < AI_CR_Max) return (AI_tdst_Node *) res;
	return _pst_Node + 3 + _pst_Node[2].l_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_CallTrigger(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	SCR_tt_Trigger		*pt_Trigger;
	int					res;
	AI_tdst_Function	t_Func;
	extern int			AI_EvalFunc_AIExecute_C(AI_tdst_Function *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	pt_Trigger = (SCR_tt_Trigger *) st_Var.pv_Addr;

#ifdef ACTIVE_EDITORS
	// !!!!! Il se peut que la procedure soit pétée, cause recompilation IA
	// Donc on resou a chaque fois en editeur (ignoble, mais ca evite les plantons)
	AI_ResolveTrigger(pt_Trigger, TRUE);
#endif

	AI_Check(pt_Trigger->pt_Proc, "Invalid trigger reference");

	L_memset(&t_Func, 0, sizeof(t_Func));
#ifdef ACTIVE_EDITORS
	L_strcpy(t_Func.az_Name, pt_Trigger->pt_Proc->pz_Name);
	t_Func.h_File = pt_Trigger->pt_Proc->pt_ProcList->h_File;
	t_Func.pst_OtherRootNode = pt_Trigger->pt_Proc->pu32_DbgNodes;
#endif
	t_Func.i_SizeLocalStack = pt_Trigger->pt_Proc->u16_SizeLocal;
	t_Func.pst_RootNode = pt_Trigger->pt_Proc->pu32_Nodes;

	{
		AI_tdst_PushVar st_Var;
		st_Var.w_Type = AI_gaw_EnumLink[TYPE_MESSAGE];
		st_Var.pv_Addr = &pt_Trigger->t_Msg;
		AI_PushVar(&st_Var);
	}

#ifdef AI_FULL_RASTERS
	t_Func.aaa = 1;
#endif

	res = AI_EvalFunc_AIExecute_C(&t_Func);

	ERR_X_Warning(res != AI_CR_Destroy, "You must not suicide an object in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_CurChangeNow, "You must not change current track in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_StopReturnTrack, "You must not use stop in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_StopAI, "You must not use stop in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);
	ERR_X_Warning(res != AI_CR_StopInstance, "You must not use stop in a procedure/trigger", AI_gpst_CurrentGameObject->sz_Name);

	if(res && res < AI_CR_Max) return (AI_tdst_Node *) res;
	return _pst_Node + 1;
}

static int inprocoff = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_inprocstack(AI_tdst_Node *_pst_Node)
{
	inprocoff++;
	
	ERR_X_Assert( AI_gi_RunFctLevel + inprocoff < 500 );
	
	AI_gul_SizeGlobalStack_SAVE[AI_gi_RunFctLevel + inprocoff] = AI_gul_SizeGlobalStack;
	AI_gul_SizeGlobalVarStack_SAVE[AI_gi_RunFctLevel + inprocoff] = AI_gul_SizeGlobalVarStack;
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_outprocstack(AI_tdst_Node *_pst_Node)
{
	inprocoff--;
	AI_gul_SizeGlobalStack = AI_gul_SizeGlobalStack_SAVE[AI_gi_RunFctLevel + inprocoff];
	AI_gul_SizeGlobalVarStack = AI_gul_SizeGlobalVarStack_SAVE[AI_gi_RunFctLevel + inprocoff];
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__CW__) && defined(RELEASE)
#pragma global_optimizer off
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Invert(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(~(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalKeyword_Switch(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	int		i32_Val;
	ULONG	u32_Num;
	/*~~~~~~~~~~~~*/

	i32_Val = AI_PopInt();
	u32_Num = _pst_Node->l_Param * 2;
	_pst_Node++;
	while(u32_Num)
	{
		u32_Num -= 2;
		if(!u32_Num)	/* Default */
		{
			if(_pst_Node[u32_Num + 1].l_Param) return _pst_Node + _pst_Node[u32_Num + 1].l_Param;
		}
		else if(_pst_Node[u32_Num].l_Param == i32_Val)
		{
			return _pst_Node + _pst_Node[u32_Num + 1].l_Param;
		}
	}

	return _pst_Node + (u32_Num * 2);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
