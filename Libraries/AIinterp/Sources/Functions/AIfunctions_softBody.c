#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
//#include "AIinterp/Sources/AIstruct.h"
//#include "AIinterp/Sources/AIFactory.h"
#include "AIinterp/Sources/AIstack.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SoftBody.h"

//
// =======================================================================================================================
// =======================================================================================================================
//
void SoftBody_SetAnchorPoint_C(OBJ_tdst_GameObject* _p_SoftBody, MATH_tdst_Vector * _pst_Position,MATH_tdst_Vector *_pst_Speed, MATH_tdst_Vector *_pst_Point)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_p_SoftBody);

	ERR_X_Assert(p_Mdf != NULL);

	MATH_CopyVector(_pst_Point, GAO_ModifierSoftBody_SetAnchorPoint(p_Mdf, _pst_Position,_pst_Speed));
}

AI_tdst_Node *AI_EvalFunc_SoftBody_SetAnchorPoint(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	MATH_tdst_Vector *pst_Position,*pst_Speed;
	OBJ_tdst_GameObject *pst_gao;
    MATH_tdst_Vector st_Point;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_Speed = AI_PopVectorPtr();
	pst_Position = AI_PopVectorPtr();
	pst_gao = AI_PopGameObject();

	SoftBody_SetAnchorPoint_C(pst_gao, pst_Position,pst_Speed, &st_Point);
    AI_PushVector(&st_Point);

	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void SoftBody_SetDirection_C(OBJ_tdst_GameObject* _p_SoftBody,ULONG ul_Direction)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_p_SoftBody);

	ERR_X_Assert(p_Mdf != NULL);

	GAO_ModifierSoftBody_SetDirection(p_Mdf,ul_Direction);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_SetDirection(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	ULONG direction;
	OBJ_tdst_GameObject *pst_gao;	
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	direction = AI_PopInt();	
	pst_gao = AI_PopGameObject();

	SoftBody_SetDirection_C(pst_gao, direction);

	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
	
void SoftBody_GetTearingPosition_C(OBJ_tdst_GameObject* _p_SoftBody, MATH_tdst_Vector *_pst_Pos)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_p_SoftBody);

	ERR_X_Assert(p_Mdf != NULL);

	MATH_CopyVector(_pst_Pos, GAO_ModifierSoftBody_GetTearingPosition(p_Mdf));
}

AI_tdst_Node* AI_EvalFunc_SoftBody_GetTearingPosition(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_gao;
    MATH_tdst_Vector st_Pos;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_gao = AI_PopGameObject();

	SoftBody_GetTearingPosition_C(pst_gao, &st_Pos);
    AI_PushVector(&st_Pos);

	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

void SoftBody_AttachTo_C(OBJ_tdst_GameObject* _p_SoftBody)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_p_SoftBody);

	ERR_X_Assert(p_Mdf != NULL);

	return GAO_ModifierSoftBody_AttachTo(p_Mdf);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_AttachTo(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	
	pst_Gao = AI_PopGameObject();

	SoftBody_AttachTo_C(pst_Gao);
	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

void SoftBody_Detach_C(OBJ_tdst_GameObject* _p_SoftBody)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_p_SoftBody);

	ERR_X_Assert(p_Mdf != NULL);

	GAO_ModifierSoftBody_Detach(p_Mdf);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_Detach(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_Gao = AI_PopGameObject();

	SoftBody_Detach_C(pst_Gao);
	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

FLOAT SoftBody_GetSwingRatio_C(OBJ_tdst_GameObject * _pst_Gao)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_pst_Gao);

	ERR_X_Assert(p_Mdf != NULL);

	return GAO_ModifierSoftBody_GetSwingRatio(p_Mdf);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_GetSwingRatio(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_Gao = AI_PopGameObject();

	AI_PushFloat(SoftBody_GetSwingRatio_C(pst_Gao));
	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

INT SoftBody_GetMoment_C(OBJ_tdst_GameObject * _pst_Gao)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_pst_Gao);

	ERR_X_Assert(p_Mdf != NULL);

	return GAO_ModifierSoftBody_GetMoment(p_Mdf);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_GetMoment(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_Gao = AI_PopGameObject();

	AI_PushInt(SoftBody_GetMoment_C(pst_Gao));
	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

FLOAT SoftBody_GetHeightRatio_C(OBJ_tdst_GameObject * _pst_Gao)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_pst_Gao);

	ERR_X_Assert(p_Mdf != NULL);

	return GAO_ModifierSoftBody_GetHeightRatio(p_Mdf);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_GetHeightRatio(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_Gao = AI_PopGameObject();

	AI_PushFloat(SoftBody_GetHeightRatio_C(pst_Gao));
	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

INT SoftBody_IsStillAttached_C(OBJ_tdst_GameObject * _pst_Gao)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_pst_Gao);

	ERR_X_Assert(p_Mdf != NULL);

	return GAO_ModifierSoftBody_IsStillAttached(p_Mdf);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_IsStillAttached(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_Gao = AI_PopGameObject();

	AI_PushInt(SoftBody_IsStillAttached_C(pst_Gao));
	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

FLOAT SoftBody_GetSpeed_C(OBJ_tdst_GameObject * _pst_Gao)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_pst_Gao);

	ERR_X_Assert(p_Mdf != NULL);

	return GAO_ModifierSoftBody_GetSpeed(p_Mdf);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_GetSpeed(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	pst_Gao = AI_PopGameObject();

	AI_PushFloat(SoftBody_GetSpeed_C(pst_Gao));
	return ++_pst_Node;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

void SoftBody_SetInitialSpeed_C(OBJ_tdst_GameObject * _pst_Gao,FLOAT speed)
{
	GAO_tdst_ModifierSoftBody * p_Mdf = GAO_ModifierSoftBody_Get(_pst_Gao);

	ERR_X_Assert(p_Mdf != NULL);

	return GAO_ModifierSoftBody_SetInitialSpeed(p_Mdf,speed);
}

AI_tdst_Node *AI_EvalFunc_SoftBody_SetInitialSpeed(AI_tdst_Node *_pst_Node)
{
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
	OBJ_tdst_GameObject* pst_Gao;
	FLOAT speed;
	//+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+

	speed = AI_PopFloat();
	pst_Gao = AI_PopGameObject();

	SoftBody_SetInitialSpeed_C(pst_Gao,speed);
	return ++_pst_Node;
}

OBJ_tdst_GameObject * SoftBody_GetCollidedGao_C(OBJ_tdst_GameObject * _pst_Gao)
{
    return GAO_ModifierSoftBody_GetCollidedGao(_pst_Gao);
}

AI_tdst_Node * AI_EvalFunc_SoftBody_GetCollidedGao(AI_tdst_Node *_pst_Node)
{
    OBJ_tdst_GameObject *pst_GO;

    AI_M_GetCurrentObject(pst_GO);
    AI_PushGameObject(SoftBody_GetCollidedGao_C(pst_GO));

    return ++_pst_Node;
}

FLOAT SoftBody_GetAverageSpeed_C(OBJ_tdst_GameObject * _pst_Gao)
{
    return GAO_ModifierSoftBody_GetForceWind(_pst_Gao);
}

AI_tdst_Node * AI_EvalFunc_SoftBody_GetAverageSpeed(AI_tdst_Node *_pst_Node)
{
    OBJ_tdst_GameObject * pst_Gao;

    AI_M_GetCurrentObject(pst_Gao);
    AI_PushFloat(SoftBody_GetAverageSpeed_C(pst_Gao));

    return ++_pst_Node;
}

void SoftBody_SetLocalWind_C(OBJ_tdst_GameObject * _pst_Gao, MATH_tdst_Vector * _pst_WindMin, MATH_tdst_Vector * _pst_WindMax)
{
    GAO_ModifierSoftBody_SetForceWind(_pst_Gao, _pst_WindMin, _pst_WindMax);
}

AI_tdst_Node * AI_EvalFunc_SoftBody_SetLocalWind(AI_tdst_Node *_pst_Node)
{
    OBJ_tdst_GameObject * pst_GO;
    MATH_tdst_Vector      st_WindMin;
    MATH_tdst_Vector      st_WindMax;

    AI_M_GetCurrentObject(pst_GO);
    AI_PopVector(&st_WindMin);
    AI_PopVector(&st_WindMin);

    SoftBody_SetLocalWind_C(pst_GO, &st_WindMin, &st_WindMax);

    return ++_pst_Node;
}

void SoftBody_Reset_C(OBJ_tdst_GameObject * _pst_Gao)
{
    MDF_tdst_Modifier * pst_Modifier;

    if((_pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_Gao->pst_Extended))
    {
        pst_Modifier = _pst_Gao->pst_Extended->pst_Modifiers;
        while(pst_Modifier)
        {
            if(pst_Modifier->i->ul_Type == MDF_C_Modifier_SoftBody)
            {
                GAO_ModifierSoftBody_Reinit(pst_Modifier);
            }

            pst_Modifier = pst_Modifier->pst_Next;
        }
    }
}

AI_tdst_Node * AI_EvalFunc_SoftBody_Reset(AI_tdst_Node *_pst_Node)
{
    OBJ_tdst_GameObject * pst_GO;

    AI_M_GetCurrentObject(pst_GO);

    SoftBody_Reset_C(pst_GO);

    return ++_pst_Node;
}

void SoftBody_SetActor_C(OBJ_tdst_GameObject * _pst_Gao, OBJ_tdst_GameObject* _p_Actor)
{
    MDF_tdst_Modifier * pst_Modifier;

    if((_pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_Gao->pst_Extended))
    {
        pst_Modifier = _pst_Gao->pst_Extended->pst_Modifiers;
        while(pst_Modifier)
        {
            if(pst_Modifier->i->ul_Type == MDF_C_Modifier_SoftBody)
            {
                GAO_ModifierSoftBody_SetActor(pst_Modifier, _p_Actor);
            }

            pst_Modifier = pst_Modifier->pst_Next;
        }
    }
}

AI_tdst_Node * AI_EvalFunc_SoftBody_SetActor(AI_tdst_Node *_pst_Node)
{
    OBJ_tdst_GameObject * pst_GO;

    AI_M_GetCurrentObject(pst_GO);

    SoftBody_SetActor_C(pst_GO, AI_PopGameObject());

    return ++_pst_Node;
}
