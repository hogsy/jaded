/*$T AIfunctions_action.c GC! 1.081 12/06/01 15:21:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTload.h"

#ifdef ACTIVE_EDITORS
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef PSX2_TARGET

/*
 =======================================================================================================================
    Cpp can't understand '\' in dos format
 =======================================================================================================================
 */
#define TstIdFlg(a)										OBJ_b_TestIdentityFlag(a, OBJ_C_IdentityFlag_Anims)
#define TstBase(a)										a->pst_Base
#define TstAnim(a)										a->pst_Base->pst_GameObjectAnim
#define TstAction(a)									a->pst_Base->pst_GameObjectAnim->pst_ActionKit
#define Test(a)											a && TstIdFlg(a) && TstBase(a) && TstAnim(a) && TstAction(a)
#define M_CheckGameObjectActionKit(pst_GO, pz_Message)	AI_Check(Test(pst_GO), pz_Message);

#else /* ! PSX2_TARGET */
#define M_CheckGameObjectActionKit(pst_GO, pz_Message) \
	AI_Check \
	( \
		pst_GO && OBJ_b_TestIdentityFlag \
			( \
				pst_GO, \
				OBJ_C_IdentityFlag_Anims \
			) && pst_GO->pst_Base && pst_GO->pst_Base->pst_GameObjectAnim && pst_GO->pst_Base->pst_GameObjectAnim-> \
			pst_ActionKit, \
		pz_Message \
	);
#endif /* ! PSX2_TARGET */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTHasKit_C(OBJ_tdst_GameObject *pst_GO)
{
	if
	(
		pst_GO
	&&	OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)
	&&	pst_GO->pst_Base
	&&	pst_GO->pst_Base->pst_GameObjectAnim
	&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit
	) 
		return 1;
	else 
		return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTHasKit(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTHasKit_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTActionGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionGet : Game object does not have an action kit");
	return _pst_GO->pst_Base->pst_GameObjectAnim->uw_CurrentActionIndex;
}
/**/
int AI_EvalFunc_ACTActionGet_C_CURRENT(void)
{
	return AI_gpst_CurrentGameObject->pst_Base->pst_GameObjectAnim->uw_CurrentActionIndex;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTActionGet_C(pst_GO));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTBlendActionGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionGet : Game object does not have an action kit");

	if((_pst_GO->pst_Base->pst_GameObjectAnim->uc_AnimUsed & 2) && _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1])
		return (int) _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->uw_BlendedAction;
	else
		return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTBlendActionGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTBlendActionGet_C(pst_GO));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTActionItemGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionItemGet : Game object does not have an action kit");
	return _pst_GO->pst_Base->pst_GameObjectAnim->uc_CurrentActionItemIndex;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionItemGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTActionItemGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTActionSet_C(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionSet : Game object does not have an action kit");
	_pst_GO->pst_Base->pst_GameObjectAnim->uw_Frame = 0;
	ACT_SetAction(_pst_GO, _iActionNumber, 0, FALSE);
	_pst_GO->pst_Base->pst_GameObjectAnim->uw_Frame = 0;
}
/**/
void AI_EvalFunc_ACTActionSet_C_CURRENT(int _iActionNumber)
{
	ACT_SetAction(AI_gpst_CurrentGameObject, _iActionNumber, 0, FALSE);
	AI_gpst_CurrentGameObject->pst_Base->pst_GameObjectAnim->uw_Frame = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ACTActionSet_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTFullActionSet_C(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber, int _iActionItemNumber)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_FullActionSet : Game object does not have an action kit");
	_pst_GO->pst_Base->pst_GameObjectAnim->uw_Frame = 0;
	ACT_SetAction(_pst_GO, _iActionNumber, _iActionItemNumber, FALSE);
	_pst_GO->pst_Base->pst_GameObjectAnim->uw_Frame = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTFullActionSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int		iActionNumber;
	int		iActionItemNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	iActionItemNumber = AI_PopInt();
	iActionNumber = AI_PopInt();
	AI_EvalFunc_ACTFullActionSet_C(pst_GO, iActionNumber, iActionItemNumber);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTCurrentItemMoveGet_C(OBJ_tdst_GameObject *pst_GO,  MATH_tdst_Vector *Result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event		*pst_Event, *pst_Last;
	MATH_tdst_Vector	*pst_PosInit, *pst_PosFinal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(Result)
		MATH_InitVector(Result, 0.0f, 0.0f, 0.0f);
	else
		return;

	M_CheckGameObjectActionKit(pst_GO, "ACT_CurrentItemMoveGet : Game object does not have an action kit");

	if
	(
		!pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0] 
	||	!pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data 
	||	!pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks
	)
		return;

	pst_Event = pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks->pst_AllTracks[1].pst_AllEvents;
	pst_Last = pst_Event + pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks->pst_AllTracks[1].uw_NumEvents;

	pst_PosInit = EVE_pst_Event_InterpolationKey_GetPos(pst_Event);
	pst_PosFinal = EVE_pst_Event_InterpolationKey_GetPos(pst_Last - 1);

	MATH_SubVector(Result, pst_PosFinal, pst_PosInit);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTCurrentItemMoveGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ACTCurrentItemMoveGet_C(pst_GO,  &v);
	AI_PushVector(&v);

	return ++_pst_Node;
}


void ACT_SetPartialAction(OBJ_tdst_GameObject *, int, BOOL);
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTPartialActionSet_C(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionSet : Game object does not have an action kit");
	ACT_SetPartialAction(_pst_GO, _iActionNumber, FALSE);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTPartialActionSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ACTPartialActionSet_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTPartialMaskSet_C(OBJ_tdst_GameObject * _pst_GO, int _i_Num, int *_pst_Array)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_BoneGO;
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_Group		*pst_Skeleton;
	ANI_tdst_Shape		*pst_Shape;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pst_GO
	||	!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Shape
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	)
	return;


	_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[0] = 0;
	_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[1] = 0;

	if(_i_Num == 0)
	{
		return;
	}

	pst_Shape = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Shape;
	pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++, i++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if(pst_Shape->auc_AI_Canal[i] == 0xFF) continue;

		for(j = 0; j < _i_Num; j++)
		{
			if(pst_Shape->auc_AI_Canal[i] == _pst_Array[j])
			{
				if (i < 32)
					_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[0] |= 1 << i;
				else
				{
				if (i < 64)
					_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[1] |= 1 << (i & 0x001F);
				}
			}
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTPartialMaskSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	int					i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PopVar(&s_Val, &st_Arr);
	i_Num = AI_PopInt();

	AI_EvalFunc_ACTPartialMaskSet_C(pst_GO, i_Num, (int *) st_Arr.pv_Addr);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTPartialBlendSet_C(OBJ_tdst_GameObject *_pst_GO, int i_Blend_IN, int i_Blend_OUT)
{
	if
	(
		!_pst_GO
	||	!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	)
	return;


	_pst_GO->pst_Base->pst_GameObjectAnim->uc_BlendLength_IN = (UCHAR) i_Blend_IN;
	_pst_GO->pst_Base->pst_GameObjectAnim->uc_BlendLength_OUT= (UCHAR) i_Blend_OUT;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTPartialBlendSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int				i_Blend_IN;
	int				i_Blend_OUT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	i_Blend_OUT = AI_PopInt();
	i_Blend_IN = AI_PopInt();

	AI_EvalFunc_ACTPartialBlendSet_C(pst_GO, i_Blend_IN, i_Blend_OUT);
	return ++_pst_Node;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTActionFrequencySet_C(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber, int _iFrequency)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionSet : Game object does not have an action kit");
	ACT_SetAction(_pst_GO, _iActionNumber, 0, TRUE);

	if(ENG_gb_EVERunning)
		_pst_GO->pst_Base->pst_GameObjectAnim->uc_Frequency = (UCHAR) (((float) _iFrequency * 60.0f / (float) ACT_C_DefaultAnimFrequency) + 0.5f);
	else
		_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->uc_AnimFrequency = (UCHAR) (((float) _iFrequency * 60.0f / (float) ACT_C_DefaultAnimFrequency) + 0.5f);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionFrequencySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Action;
	int					i_Frequency;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Frequency = AI_PopInt();
	i_Action = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ACTActionFrequencySet_C(pst_GO, i_Action, i_Frequency);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTFrequencyGet_C(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber, int _iActionItemNumber)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionSet : Game object does not have an action kit");

	if(!_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[_iActionNumber]) return 0;

	return (int) (_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[_iActionNumber]->ast_ActionItem[_iActionItemNumber].uc_Frequency);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTFrequencyGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Action;
	int					i_ActionItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_ActionItem = AI_PopInt();
	i_Action = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTFrequencyGet_C(pst_GO, i_Action, i_ActionItem));
	return ++_pst_Node;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTActionFrameSet_C(OBJ_tdst_GameObject *_pst_GO, int _iActionNumber, int _iFrame)
{
	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionSet : Game object does not have an action kit");
	// The Following line is present twice in this function --> That is normal !
	_pst_GO->pst_Base->pst_GameObjectAnim->uw_Frame = (USHORT) _iFrame;
	ACT_SetAction(_pst_GO, _iActionNumber, 0, TRUE);
	_pst_GO->pst_Base->pst_GameObjectAnim->uw_Frame = (USHORT) _iFrame;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionFrameSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Action;
	int					i_Frame;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Frame = AI_PopInt();
	i_Action = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ACTActionFrameSet_C(pst_GO, i_Action, i_Frame);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTCustomBitGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACT_CustomBitGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	AI_Check(pst_GOAnim->pst_CurrentAction, "No current action available");
	return pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uc_CustomBit;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTCustomBitGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTCustomBitGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTCustomBitGetIndex_C(OBJ_tdst_GameObject *_pst_GO, int act)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACT_CustomBitGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	if(act >= pst_GOAnim->pst_ActionKit->uw_NumberOfAction) return 0;
	if(!pst_GOAnim->pst_ActionKit->apst_Action[act]) return 0;
	return pst_GOAnim->pst_ActionKit->apst_Action[act]->ast_ActionItem[0].uc_CustomBit;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTCustomBitGetIndex(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	index = AI_PopInt();
	AI_PushInt(AI_EvalFunc_ACTCustomBitGetIndex_C(pst_GO, index));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTDesignFlagsGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACT_CustomBitGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	AI_Check(pst_GOAnim->pst_CurrentAction, "No current action available");
	return pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uw_DesignFlags;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTDesignFlagsGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTDesignFlagsGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTCustomBitTest_C(OBJ_tdst_GameObject *_pst_GO, int _iCBToTest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACT_CustomBitTest : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	AI_Check(pst_GOAnim->pst_CurrentAction, "No current action available");
	return pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uc_CustomBit & _iCBToTest;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTCustomBitTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTCustomBitTest_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTDesignFlagsTest_C(OBJ_tdst_GameObject *_pst_GO, int _iCBToTest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACT_CustomBitTest : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	AI_Check(pst_GOAnim->pst_CurrentAction, "No current action available");
	return pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uw_DesignFlags & _iCBToTest;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTDesignFlagsTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTDesignFlagsTest_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTActionFinished_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	ANI_tdst_Anim			*pst_Anim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionFinished : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_Anim = pst_GOAnim->apst_Anim[0];
	AI_Check((pst_GOAnim->uc_AnimUsed & 1) && pst_Anim, "ACT_ActionFinished : No anim");
	return pst_Anim->uw_Flag & ANI_C_AnimFlag_DontPlayAnim ? 1 : 0;
}
/**/
int AI_EvalFunc_ACTActionFinished_C_CURRENT(void)
{
	return AI_EvalFunc_ACTActionFinished_C(AI_gpst_CurrentGameObject);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionFinished(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTActionFinished_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTPartialActionFinished_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	ANI_tdst_Anim			*pst_Anim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionFinished : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_Anim = pst_GOAnim->apst_PartialAnim[0];
	if(!pst_Anim) return 1;
	return pst_Anim->uw_Flag & ANI_C_AnimFlag_DontPlayAnim ? 1 : 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTPartialActionFinished(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTPartialActionFinished_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTPartialActionGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACTPartialActionGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	if(pst_GOAnim->apst_PartialAnim[0] && !(pst_GOAnim->apst_PartialAnim[0]->uw_Flag & ANI_C_AnimFlag_DontPlayAnim))
		return (int) pst_GOAnim->uw_CurrentPartialActionIndex;
	else
		return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTPartialActionGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTPartialActionGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTPartialActionItemGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACTPartialActionItemGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	if(pst_GOAnim->apst_PartialAnim[0] && !(pst_GOAnim->apst_PartialAnim[0]->uw_Flag & ANI_C_AnimFlag_DontPlayAnim))
		return (int) pst_GOAnim->uc_CurrentPartialActionItemIndex;
	else
		return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTPartialActionItemGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTPartialActionItemGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACT_DefaultTransition_C(OBJ_tdst_GameObject *_pst_GO, int Blend)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACTPartialActionGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_GOAnim->pst_ActionKit->DefaultTrans.uc_Blend = Blend;
	pst_GOAnim->pst_ActionKit->DefaultTrans.uc_Flag = ACT_C_TF_DefaultTransition | ACT_C_TF_BlendStock;
	pst_GOAnim->pst_ActionKit->DefaultTrans.uw_Action = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACT_DefaultTransition(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Blend;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Blend = AI_PopInt();
	AI_EvalFunc_ACT_DefaultTransition_C(pst_GO, Blend);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACT_DefaultTransitionParams_C(OBJ_tdst_GameObject *_pst_GO, int Blend, int flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACTPartialActionGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_GOAnim->pst_ActionKit->DefaultTrans.uc_Blend = Blend;
	if(!flags) flags = ACT_C_TF_DefaultTransition | ACT_C_TF_BlendStock;
	pst_GOAnim->pst_ActionKit->DefaultTrans.uc_Flag = flags;
	pst_GOAnim->pst_ActionKit->DefaultTrans.uw_Action = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACT_DefaultTransitionParams(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Flags, Blend;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Flags = AI_PopInt();
	Blend = AI_PopInt();
	AI_EvalFunc_ACT_DefaultTransitionParams_C(pst_GO, Blend, Flags);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACT_PartialActionOnOff_C(OBJ_tdst_GameObject *_pst_GO, int OnOff)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(_pst_GO, "ACTPartialActionGet : Game object does not have an action kit");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	if(pst_GOAnim->apst_PartialAnim[0])
		ACT_PartialAction_OnOff(_pst_GO, 0, OnOff);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACT_PartialActionOnOff(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					OnOff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	OnOff = AI_PopInt();
	AI_EvalFunc_ACT_PartialActionOnOff_C(pst_GO, OnOff);
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTActionIsTransition_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(pst_GO, "ACT_ActionIsTransition: Game object does not have an action kit");
	pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
	return pst_GOAnim->pst_Transition ? 1 : 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionIsTransition(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTActionIsTransition_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ACTActionGetTransition_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(pst_GO, "ACT_ActionGetTransition: Game object does not have an action kit");
	pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
	AI_Check
	(
		pst_GOAnim->pst_Transition,
		"Game object is not currently in a transition. Use ACT_ActionIsTransition to check this."
	);
	return pst_GOAnim->uw_NextActionIndex;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionGetTransition(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ACTActionGetTransition_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACTKitMerge(OBJ_tdst_GameObject *pst_GODest, OBJ_tdst_GameObject *pst_GOSrc, int i_Beg, int i_End, int Over)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	ANI_st_GameObjectAnim	*pst_GODestAnim, *pst_GOSrcAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectActionKit(pst_GODest, "Game object does not have an action kit");
	M_CheckGameObjectActionKit(pst_GOSrc, "Game object does not have an action kit");

	pst_GODestAnim = pst_GODest->pst_Base->pst_GameObjectAnim;
	pst_GOSrcAnim = pst_GOSrc->pst_Base->pst_GameObjectAnim;

	AI_Check
	(
		i_Beg >= 0 && i_Beg <= i_End && i_Beg < pst_GOSrcAnim->pst_ActionKit->uw_NumberOfAction && i_Beg < pst_GODestAnim->pst_ActionKit->uw_NumberOfAction,
		"Invalid First Index"
	);
	AI_Check
	(
		i_End >= 0 && i_End < pst_GOSrcAnim->pst_ActionKit->uw_NumberOfAction && i_End < pst_GODestAnim->pst_ActionKit->uw_NumberOfAction,
		"Invalid End Index"
	);

	ACT_ActionSaveInit(pst_GODestAnim->pst_ActionKit);
	for(i = i_Beg; i <= i_End; i++)
	{
		if(pst_GOSrcAnim->pst_ActionKit->apst_Action[i] || Over)
		{
			if(pst_GODestAnim->pst_ActionKit->apst_Action[i])
				ACT_FreeAction(&pst_GODestAnim->pst_ActionKit->apst_Action[i]);
			pst_GODestAnim->pst_ActionKit->apst_Action[i] = pst_GOSrcAnim->pst_ActionKit->apst_Action[i];
			if(pst_GODestAnim->pst_ActionKit->apst_Action[i])
				ACT_UseAction(pst_GODestAnim->pst_ActionKit->apst_Action[i]);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACTKitMergeNoOver(OBJ_tdst_GameObject *pst_GODest, OBJ_tdst_GameObject *pst_GOSrc, int i_Beg, int i_End)
{
	ACTKitMerge(pst_GODest, pst_GOSrc, i_Beg, i_End, 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTKitMerge(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GODest, *pst_GOSrc;
	int					i_Beg, i_End;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GODest);
	i_End = AI_PopInt();
	i_Beg = AI_PopInt();
	pst_GOSrc = AI_PopGameObject();
	ACTKitMergeNoOver(pst_GODest, pst_GOSrc, i_Beg, i_End);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ACTKitMergeOver(OBJ_tdst_GameObject *pst_GODest, OBJ_tdst_GameObject *pst_GOSrc, int i_Beg, int i_End)
{
	ACTKitMerge(pst_GODest, pst_GOSrc, i_Beg, i_End, 1);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTKitMergeOver(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GODest, *pst_GOSrc;
	int					i_Beg, i_End;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GODest);
	i_End = AI_PopInt();
	i_Beg = AI_PopInt();
	pst_GOSrc = AI_PopGameObject();
	ACTKitMergeOver(pst_GODest, pst_GOSrc, i_Beg, i_End);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTKitRestore_C(OBJ_tdst_GameObject *pst_GO)
{
	M_CheckGameObjectActionKit(pst_GO, "Game object does not have an action kit");
	ACT_ActionRestoreInit(pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit);
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTKitRestore(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ACTKitRestore_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ACTForceModeSet_C(OBJ_tdst_GameObject *pst_GO, int Mode)
{
	M_CheckGameObjectActionKit(pst_GO, "Game object does not have an action kit");
	if
	(
		!pst_GO
	||  !OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)
	||  !pst_GO->pst_Base
	||	!pst_GO->pst_Base->pst_GameObjectAnim
	) return;

	pst_GO->pst_Base->pst_GameObjectAnim->uc_ForceMode = (UCHAR) Mode;
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTForceModeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Mode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Mode = AI_PopInt();
	AI_EvalFunc_ACTForceModeSet_C(pst_GO, Mode);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_ACTActionItemDurationGet_C(OBJ_tdst_GameObject *pst_GO, int Action, int ActionItem)
{

	M_CheckGameObjectActionKit(pst_GO, "Game object does not have an action kit");

	if
	(
		!pst_GO
	||  !pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[Action]
	||   (Action >= pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->uw_NumberOfAction)	
	||   (ActionItem >= pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[Action]->uc_NumberOfActionItem)
	||  !pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[Action]->ast_ActionItem[ActionItem].pst_TrackList
	) return (- 1.0f);

	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Event	*pst_Event, *pst_Last;
		ULONG			ul_NumFrames;
		float		f_Duration;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_NumFrames = 0;
		pst_Event = pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[Action]->ast_ActionItem[ActionItem].pst_TrackList->pst_AllTracks[0].pst_AllEvents;
		pst_Last = pst_Event + pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[Action]->ast_ActionItem[ActionItem].pst_TrackList->pst_AllTracks[0].uw_NumEvents;
		for(; pst_Event < pst_Last; pst_Event++) ul_NumFrames += pst_Event->uw_NumFrames & 0x7FFF;

		f_Duration = (float) ul_NumFrames * 1.0f / ((float) ((int) pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[Action]->ast_ActionItem[ActionItem].uc_Frequency) * 60 / ACT_C_DefaultAnimFrequency);

		return f_Duration;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTActionItemDurationGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int		Action, ActionItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ActionItem = AI_PopInt();
	Action = AI_PopInt();
	AI_PushFloat(AI_EvalFunc_ACTActionItemDurationGet_C(pst_GO, Action, ActionItem));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 Given an action and an action item, return the movement vector and the average speed of the magic box during one loop.
 =======================================================================================================================
 */
void AI_EvalFunc_ACTMagicBoxMoveGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pstDeltaMagicBox,float *_pfMagicBoxAverageSpeed,int _iActionNumber, int _iActionItemNumber)
{
    ACT_st_ActionItem *pstActionItem;
    EVE_tdst_Track  *pstTrack;
    int i;
	int w_Type;
    M_CheckGameObjectActionKit(_pst_GO, "ACT_ActionSet : Game object does not have an action kit");


	if(!_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[_iActionNumber]) return ;
    
    pstActionItem = &(_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[_iActionNumber]->ast_ActionItem[_iActionItemNumber]);

    // Find translation track
    for (i=0; i<pstActionItem->pst_TrackList->uw_NumTracks; i++)
    {
        pstTrack = &(pstActionItem->pst_TrackList->pst_AllTracks[i]);

        if (pstTrack->pst_AllEvents)
        {
            w_Type = EVE_w_Event_InterpolationKey_GetType(pstTrack->pst_AllEvents);
        	if ((w_Type != -1) && (w_Type & EVE_InterKeyType_Translation_Mask))
                break;
        }
    }

    if (i<pstActionItem->pst_TrackList->uw_NumTracks)
    {
		EVE_tdst_Track	*pst_SaveTrack = EVE_gpst_CurrentTrack;
		EVE_gpst_CurrentTrack = pstTrack;

        // Get delta vector
        MATH_CopyVector(_pstDeltaMagicBox,EVE_pst_Event_InterpolationKey_GetPos(pstTrack->pst_AllEvents + (pstTrack->uw_NumEvents-1)));

        // Get speed
        *_pfMagicBoxAverageSpeed = MATH_f_NormVector(_pstDeltaMagicBox) / AI_EvalFunc_ACTActionItemDurationGet_C(_pst_GO,_iActionNumber, _iActionItemNumber);

		EVE_gpst_CurrentTrack = pst_SaveTrack;
    }
    else
    {
        MATH_InitVectorToZero(_pstDeltaMagicBox);
        *_pfMagicBoxAverageSpeed = 0;
    }
}
/**/
AI_tdst_Node *AI_EvalFunc_ACTMagicBoxMoveGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Action;
	int					i_ActionItem;
	AI_tdst_PushVar		st_VarDeltaMB, st_VarMBSpeed;
	AI_tdst_UnionVar	ValDeltaMB, ValMBSpeed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_ActionItem = AI_PopInt();
	i_Action = AI_PopInt();
	AI_PopVar(&ValMBSpeed, &st_VarMBSpeed);
	AI_PopVar(&ValDeltaMB, &st_VarDeltaMB);
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ACTMagicBoxMoveGet_C(
        pst_GO, 
        ((MATH_tdst_Vector *) st_VarDeltaMB.pv_Addr),
        (float *)st_VarMBSpeed.pv_Addr,
        i_Action, 
        i_ActionItem);

	return ++_pst_Node;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
