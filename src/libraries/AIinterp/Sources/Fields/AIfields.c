/*$T AIfields.c GC! 1.092 08/24/00 14:22:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "MATHs/MATH.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AImsg.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_VecX(AI_tdst_Node *_pst_Node)
{
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_FLOAT;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_VecY(AI_tdst_Node *_pst_Node)
{
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_FLOAT;
#ifdef JADEFUSION
	((float *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += 1;
#else
	((float *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += 1;
#endif
	*(float *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(float *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_VecZ(AI_tdst_Node *_pst_Node)
{
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_FLOAT;
#ifdef JADEFUSION
	((float *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += 2;
#else
	((float *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += 2;
#endif
	*(float *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(float *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
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
AI_tdst_Node *AI_EvalField_MsgSender(AI_tdst_Node *_pst_Node)
{
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgId(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Id - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Id - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_INT;
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgGAO1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO1 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO1 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgGAO2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO2 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO2 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgGAO3(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO3 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO3 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgGAO4(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO4 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO4 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgGAO5(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO4 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.pst_GAO5 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgVec1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec1 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec1 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_VECTOR;
	MATH_CopyVector((MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE), (MATH_tdst_Vector *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgVec2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec2 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec2 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_VECTOR;
	MATH_CopyVector((MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE), (MATH_tdst_Vector *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgVec3(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec3 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec3 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_VECTOR;
	MATH_CopyVector((MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE), (MATH_tdst_Vector *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgVec4(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec4 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec4 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_VECTOR;
	MATH_CopyVector((MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE), (MATH_tdst_Vector *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgVec5(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec5 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.st_Vec5 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_VECTOR;
	MATH_CopyVector((MATH_tdst_Vector *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE), (MATH_tdst_Vector *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr);
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgInt1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int1 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int1 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_INT;
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgInt2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int2 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int2 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_INT;
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgInt3(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int3 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int3 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_INT;
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgInt4(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int4 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int4 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_INT;
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalField_MsgInt5(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	((char *&) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int5 - (char *) &st_Msg.pst_Sender);
#else
	((char *) AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr) += ((char *) &st_Msg.i_Int5 - (char *) &st_Msg.pst_Sender);
#endif
	AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].w_Type = ENUTYPE_INT;
	*(int *) (AI_gac_GlobalStack + AI_gul_SizeGlobalStack - MINSIZE) = *(int *)AI_gast_GlobalVarStack[AI_gul_SizeGlobalVarStack - 1].pv_Addr;
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
AI_tdst_Node *AI_EvalField_DesignFlags(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.flags - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_INT;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignInt1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.i1 - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_INT;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignInt2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.i2 - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_INT;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignText1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.st_Text1 - (char *) &st_Des.flags));
	st_Var.w_Type = AI_gaw_EnumLink[TYPE_TEXT];

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignText2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.st_Text2 - (char *) &st_Des.flags));
	st_Var.w_Type = AI_gaw_EnumLink[TYPE_TEXT];

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignFloat1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.f1 - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_FLOAT;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignFloat2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.f2 - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_FLOAT;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignVec1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.st_Vec1 - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_VECTOR;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignVec2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.st_Vec2 - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_VECTOR;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignGO1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.pst_Perso1 - (char *) &st_Des.flags));
	st_Var.w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignGO2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.pst_Perso2 - (char *) &st_Des.flags));
	st_Var.w_Type = AI_gaw_EnumLink[TYPE_GAMEOBJECT];

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignNet1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.pst_Net1 - (char *) &st_Des.flags));
	st_Var.w_Type = AI_gaw_EnumLink[TYPE_NETWORK];

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignNet2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.pst_Net2 - (char *) &st_Des.flags));
	st_Var.w_Type = AI_gaw_EnumLink[TYPE_NETWORK];

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignInt3(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.i3 - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_INT;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalField_DesignDesFlags(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	st_Des;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalType_GetGAO(st_Var.pv_Addr, &pst_GO);
	AI_Check(pst_GO, "Object is NULL");
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Design, "Design struct is not allocated for object");

	st_Var.pv_Addr = (void *) ((char *) pst_GO->pst_Extended->pst_Design + ((char *) &st_Des.ui_DesignFlags - (char *) &st_Des.flags));
	st_Var.w_Type = ENUTYPE_INT;

	AI_PushVar(&st_Var);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
