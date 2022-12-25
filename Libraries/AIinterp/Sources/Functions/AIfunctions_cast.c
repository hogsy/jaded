/*$T AIfunctions_cast.c GC! 1.100 03/13/01 15:46:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_CastVector_C(float f3, float f2, float f1, MATH_tdst_Vector *pst_Vec)
{
	pst_Vec->x = f3;
	pst_Vec->y = f2;
	pst_Vec->z = f1;
}
/**/
AI_tdst_Node *AI_EvalFunc_CastVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val1, Val2, Val3;
	AI_tdst_PushVar		st_Var1, st_Var2, st_Var3;
	MATH_tdst_Vector	st_Vec;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val2, &st_Var2);
	AI_PopVar(&Val3, &st_Var3);

	IntToFloat1(&Val1, &st_Var1);
	IntToFloat1(&Val2, &st_Var2);
	IntToFloat1(&Val3, &st_Var3);

	AI_EvalFunc_CastVector_C(Val3.f, Val2.f, Val1.f, &st_Vec);
	AI_PushVector(&st_Vec);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
