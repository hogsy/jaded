/*$T AIfunctions_ai.c GC 1.134 05/13/04 16:30:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "BASe/BENch/BENch.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
int TCHN_rec = 0;
#endif

SCR_tt_SFDyn	AI_gst_SFDyn[SFDYN_MAX];


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AIExecute_C(AI_tdst_Function *pst_Function)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Node		*pst_Root, **pst_MemNode;
	AI_tdst_Function	*pst_Memo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* ERR_X_Assert(pst_Function != AI_gpst_CurrentFunction); */
	pst_Memo = AI_gpst_CurrentFunction;
	pst_MemNode = AI_gppst_CurrentJumpNode;

	if(pst_Function && ((int) pst_Function != -1))
	{
		pst_Root = pst_Function->pst_RootNode;
		AI_gi_RunFctLevel++;
		pst_Root = AI_pst_RunFunction(pst_Function, &pst_Root, pst_Memo->i_SizeLocalStack);
		AI_gi_RunFctLevel--;
	}

	AI_gpst_CurrentFunction = pst_Memo;
	AI_gppst_CurrentJumpNode = pst_MemNode;

	if((unsigned int) pst_Root < AI_CR_Max)
	{
		if((unsigned int) pst_Root == AI_CR_StopInstance) return AI_CR_StopInstance;
		if((unsigned int) pst_Root == AI_CR_StopAI) return AI_CR_StopAI;
		if((unsigned int) pst_Root == AI_CR_StopReturnTrack) return AI_CR_StopReturnTrack;
		if((unsigned int) pst_Root == AI_CR_CurChangeNow) return AI_CR_CurChangeNow;
		if((unsigned int) pst_Root == AI_CR_Destroy) return AI_CR_Destroy;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_AIExecute(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	AI_tdst_Function	*pst_Function;
	int					ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	pst_Function = (AI_tdst_Function *) Val.i;

#ifdef ACTIVE_EDITORS
	AI_gst_CallStack[AI_gi_RunFctLevel].pt_Function = AI_gpst_CurrentFunction;
	ires = _pst_Node - AI_gpst_CurrentFunction->pst_RootNode;
	AI_gst_CallStack[AI_gi_RunFctLevel].pt_Node = AI_gpst_CurrentFunction->pst_OtherRootNode + ires;
#endif
	ires = AI_EvalFunc_AIExecute_C(pst_Function);

	if((unsigned int) ires < AI_CR_Max)
	{
		if(ires == AI_CR_StopInstance) return (AI_tdst_Node *) AI_CR_StopInstance;
		if(ires == AI_CR_StopAI) return (AI_tdst_Node *) AI_CR_StopAI;
		if(ires == AI_CR_StopReturnTrack) return (AI_tdst_Node *) AI_CR_StopReturnTrack;
		if(ires == AI_CR_CurChangeNow) return (AI_tdst_Node *) AI_CR_CurChangeNow;
		if(ires == AI_CR_Destroy) return (AI_tdst_Node *) AI_CR_Destroy;
	}

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AITrackChange_C(int val2, AI_tdst_Function *pst_Func)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
    if (val2 < 0)
    {
        char sMsg[256];
		snprintf( sMsg, sizeof(sMsg), "Index must be >= 0 in %s", pst_Func->az_Name );
        ERR_X_Warning(0,sMsg,NULL);
        val2 = 0;
    }

    if (val2 >= 5)
    {
        char sMsg[256];
		snprintf( sMsg, sizeof(sMsg), "Index must be < 5 in %s", pst_Func->az_Name );
        ERR_X_Warning(0,sMsg,NULL);
        val2 = 4;
    }
#endif //ACTIVE_EDITORS

	AI_M_GetCurrentInstance(pst_Instance);
	if(pst_Instance->apst_CurrentFct[val2] != pst_Func)
	{
		pst_Instance->apst_CurrentFct[val2] = pst_Func;
		pst_Instance->apst_JumpNode[val2] = pst_Func->pst_RootNode;
		if((pst_Instance->apst_JumpNode[val2] == NULL) && pst_Instance->apst_CurrentFct[val2]->pfn_CFunc)
		{
			pst_Instance->af_WaitTime[val2] = 0;
			*(int *) &pst_Instance->apst_JumpNode[val2] = -1;
		}
	}

	if(val2 < pst_Instance->uw_FirstTrack) pst_Instance->uw_FirstTrack = val2;
	if(val2 > pst_Instance->uw_LastTrack) pst_Instance->uw_LastTrack = val2;
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackChange(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Function	*pst_Func;
	AI_tdst_UnionVar	Val1, Val2;
	AI_tdst_PushVar		st_Var1, st_Var2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val2, &st_Var2);
	FloatToInt1(&Val2, &st_Var2);
	AI_Check(Val1.i, "Function is null");
	pst_Func = (AI_tdst_Function *) Val1.i;
	AI_EvalFunc_AITrackChange_C(Val2.i, pst_Func);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AITrackStop_C(int vali)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentInstance(pst_Instance);
	pst_Instance->apst_CurrentFct[vali] = NULL;
	pst_Instance->apst_JumpNode[vali] = NULL;

	if(vali == pst_Instance->uw_FirstTrack)
	{
		while(vali < AI_C_MaxTracks && !pst_Instance->apst_CurrentFct[vali]) vali++;
		pst_Instance->uw_FirstTrack = vali;
	}

	if(vali == pst_Instance->uw_LastTrack)
	{
		while(vali && !pst_Instance->apst_CurrentFct[vali]) vali--;
		pst_Instance->uw_LastTrack = vali;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackStop(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	FloatToInt1(&Val, &st_Var);
	AI_EvalFunc_AITrackStop_C(Val.i);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AITrackCurChange_C(AI_tdst_Function *pst_Func)
{
	if(AI_gpst_CurrentInstance->apst_CurrentFct[AI_gul_CurrentTrack] != pst_Func)
	{
		AI_gpst_CurrentInstance->apst_CurrentFct[AI_gul_CurrentTrack] = pst_Func;
		AI_gpst_CurrentInstance->apst_JumpNode[AI_gul_CurrentTrack] = pst_Func->pst_RootNode;
		if
		(
			(AI_gpst_CurrentInstance->apst_JumpNode[AI_gul_CurrentTrack] == NULL)
		&&	AI_gpst_CurrentInstance->apst_CurrentFct[AI_gul_CurrentTrack]->pfn_CFunc
		)
		{
			*(int *) &AI_gpst_CurrentInstance->apst_JumpNode[AI_gul_CurrentTrack] = -1;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackCurChange(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	AI_tdst_Function	*pst_Func;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_Check(Val.i, "Function is null");
	pst_Func = (AI_tdst_Function *) Val.i;
	AI_EvalFunc_AITrackCurChange_C(pst_Func);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Function *AI_EvalFunc_AITrackGet_C(int val)
{
	return AI_gpst_CurrentInstance->apst_CurrentFct[val];
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFunction(AI_EvalFunc_AITrackGet_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Function *AI_EvalFunc_AITrackCurGet_C(void)
{
	return AI_gpst_CurrentInstance->apst_CurrentFct[AI_gul_CurrentTrack];
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackCurGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFunction(AI_EvalFunc_AITrackCurGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AITrackCurGetNum_C(void)
{
	return AI_gul_CurrentTrack;
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackCurGetNum(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_AITrackCurGetNum_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AITrackCurChangeNow_C(AI_tdst_Function *pst_Func)
{
#ifdef ACTIVE_EDITORS
	TCHN_rec++;
	AI_Check(TCHN_rec != 100, "AI seams to loop with AI_TrackCurChangeNow function");
#endif
	AI_Check(pst_Func, "Function is null");
	if(AI_gpst_CurrentFunction != pst_Func)
	{
		AI_gpst_CurrentFunction = pst_Func;
		AI_gpst_CurrentInstance->apst_CurrentFct[AI_gul_CurrentTrack] = pst_Func;
		AI_gpst_CurrentInstance->apst_JumpNode[AI_gul_CurrentTrack] = AI_gpst_CurrentFunction->pst_RootNode;
		if
		(
			(AI_gpst_CurrentInstance->apst_JumpNode[AI_gul_CurrentTrack] == NULL)
		&&	AI_gpst_CurrentInstance->apst_CurrentFct[AI_gul_CurrentTrack]->pfn_CFunc
		)
		{
			*(int *) &AI_gpst_CurrentInstance->apst_JumpNode[AI_gul_CurrentTrack] = -1;
		}

		return AI_CR_CurChangeNow;
	}

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackCurChangeNow(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	ires = AI_EvalFunc_AITrackCurChangeNow_C((AI_tdst_Function *) Val.i);
	if(ires) return (AI_tdst_Node *) ires;
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AITrackCurStop_C(void)
{
	/*~~~~~~~*/
	ULONG	ul;
	/*~~~~~~~*/

	AI_gpst_CurrentInstance->apst_CurrentFct[AI_gul_CurrentTrack] = NULL;
	AI_gpst_CurrentInstance->apst_JumpNode[AI_gul_CurrentTrack] = NULL;

	ul = AI_gul_CurrentTrack;
	if(ul == AI_gpst_CurrentInstance->uw_FirstTrack)
	{
		while(ul < AI_C_MaxTracks && !AI_gpst_CurrentInstance->apst_CurrentFct[ul]) ul++;
		AI_gpst_CurrentInstance->uw_FirstTrack = (unsigned short) ul;
	}

	if(ul == AI_gpst_CurrentInstance->uw_LastTrack)
	{
		while(ul && !AI_gpst_CurrentInstance->apst_CurrentFct[ul]) ul--;
		AI_gpst_CurrentInstance->uw_LastTrack = (unsigned short) ul;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_AITrackCurStop(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_AITrackCurStop_C();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AIIsModel_C(OBJ_tdst_GameObject *pst_GO, AI_tdst_Model *pst_Model)
{
	/*~~~~~~*/
	int i_Res;
	/*~~~~~~*/

	if((!pst_GO->pst_Extended) || (!pst_GO->pst_Extended->pst_Ai))
		i_Res = 0;
	else if(((AI_tdst_Instance *) pst_GO->pst_Extended->pst_Ai)->pst_Model == pst_Model)
		i_Res = 1;
	else
		i_Res = 0;

	return i_Res;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIIsModel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_Model		*pst_Model;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_Model = AI_PopModel();
	AI_PushInt(AI_EvalFunc_AIIsModel_C(pst_GO, pst_Model));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AIHaveSameModel_C(OBJ_tdst_GameObject *pst_GO1, OBJ_tdst_GameObject *pst_GO2)
{
	if((!pst_GO1->pst_Extended) || (!pst_GO1->pst_Extended->pst_Ai))
		return 0;
	if((!pst_GO2->pst_Extended) || (!pst_GO2->pst_Extended->pst_Ai))
		return 0;
	if (((AI_tdst_Instance *) pst_GO1->pst_Extended->pst_Ai)->pst_Model != ((AI_tdst_Instance *) pst_GO2->pst_Extended->pst_Ai)->pst_Model )
		return 0;
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIHaveSameModel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_AIHaveSameModel_C(pst_GO, AI_PopGameObject() ));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AIPriorityGet_C(OBJ_tdst_GameObject *pst_GAO)
{
	AI_Check(pst_GAO->pst_Extended != NULL, "No Extended allocated");
	return (int) (pst_GAO->pst_Extended->uc_AiPrio);
}
/**/
AI_tdst_Node *AI_EvalFunc_AIPriorityGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	AI_PushInt(AI_EvalFunc_AIPriorityGet_C(pst_GAO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AIPrioritySet_C(OBJ_tdst_GameObject *pst_GAO, int prio)
{
	AI_Check(pst_GAO->pst_Extended != NULL, "No Extended allocated");
	pst_GAO->pst_Extended->uc_AiPrio = prio;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIPrioritySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	int					prio;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	prio = (UCHAR) AI_PopInt();
	AI_EvalFunc_AIPrioritySet_C(pst_GAO, prio);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AIMainActorSet_C(OBJ_tdst_GameObject *pst_GAO, int i_Num)
{
	AI_gpst_MainActors[i_Num] = pst_GAO;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIMainActorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	int					i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	i_Num = AI_PopInt();
	AI_Check(i_Num < AI_C_MainActorsNum, "Invalid Main Actor Index");
	AI_EvalFunc_AIMainActorSet_C(pst_GAO, i_Num);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AIMainActorReset_C(int i_Num)
{
	AI_Check(i_Num < AI_C_MainActorsNum, "Invalid Main Actor Index");
	AI_gpst_MainActors[i_Num] = NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIMainActorReset(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_AIMainActorReset_C(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_AIMainActorGet_C(int i_Num)
{
	AI_Check(i_Num < AI_C_MainActorsNum, "Invalid Main Actor Index");
#ifdef ACTIVE_EDITORS
	if(AI_gb_InterpForVar)
		return((OBJ_tdst_GameObject *) (i_Num + 1));
	else
#endif
		return(AI_gpst_MainActors[i_Num]);
}
/**/
AI_tdst_Node *AI_EvalFunc_AIMainActorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	int i_Num;
	/*~~~~~~*/

	i_Num = AI_PopInt();
	AI_PushGameObject(AI_EvalFunc_AIMainActorGet_C(i_Num));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AICBAdd_C
(
	OBJ_tdst_GameObject *pst_GAO,
	OBJ_tdst_GameObject *pst_Dest,
	int					i_Type,
	AI_tdst_Function	*pst_Function
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_Dest && pst_Dest->pst_Extended && pst_Dest->pst_Extended->pst_Ai, "Invalid GAO");
	pst_Instance = (AI_tdst_Instance *) pst_Dest->pst_Extended->pst_Ai;

	if(!pst_Instance->pst_CB)
	{
		pst_Instance->pst_CB = (AI_tdst_CB *) MEM_p_Alloc(sizeof(AI_tdst_CB));
		pst_Instance->i_NumCB = 1;
		i = 0;
	}
	else
	{
		/* Place libre ? */
		for(i = 0; i < pst_Instance->i_NumCB; i++)
		{
			if(!pst_Instance->pst_CB[i].pst_Func) break;
		}

		if(i == pst_Instance->i_NumCB)
		{
			pst_Instance->pst_CB = (AI_tdst_CB *) MEM_p_Realloc
				(
					pst_Instance->pst_CB,
					(pst_Instance->i_NumCB + 1) * sizeof(AI_tdst_CB)
				);
			i = pst_Instance->i_NumCB;
			pst_Instance->i_NumCB++;
		}
	}

	pst_Instance->pst_CB[i].i_Type = i_Type;
	pst_Instance->pst_CB[i].pst_Func = pst_Function;
	pst_Instance->pst_CB[i].pst_GameObject = pst_GAO;
}
/**/
AI_tdst_Node *AI_EvalFunc_AICBAdd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	AI_tdst_Function	*pst_Function;
	OBJ_tdst_GameObject *pst_GAO;
	OBJ_tdst_GameObject *pst_Dest;
	int					i_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	AI_PopVar(&Val, &st_Var);
	pst_Function = (AI_tdst_Function *) Val.i;
	i_Type = AI_PopInt();
	pst_Dest = AI_PopGameObject();
	AI_EvalFunc_AICBAdd_C(pst_GAO, pst_Dest, i_Type, pst_Function);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AICBDel_C
(
	OBJ_tdst_GameObject *pst_GAO,
	OBJ_tdst_GameObject *pst_Dest,
	int					i_Type,
	AI_tdst_Function	*pst_Function
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_Dest && pst_Dest->pst_Extended && pst_Dest->pst_Extended->pst_Ai, "Invalid GAO");
	pst_Instance = (AI_tdst_Instance *) pst_Dest->pst_Extended->pst_Ai;
	for(i = 0; i < pst_Instance->i_NumCB; i++)
	{
		if
		(
			(pst_Instance->pst_CB[i].i_Type == i_Type)
		&&	(pst_Instance->pst_CB[i].pst_Func == pst_Function)
		&&	(pst_Instance->pst_CB[i].pst_GameObject == pst_GAO)
		)
		{
			pst_Instance->pst_CB[i].pst_Func = NULL;
			break;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_AICBDel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	AI_tdst_Function	*pst_Function;
	OBJ_tdst_GameObject *pst_GAO;
	int					i_Type;
	OBJ_tdst_GameObject *pst_Dest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	AI_PopVar(&Val, &st_Var);
	pst_Function = (AI_tdst_Function *) Val.i;
	i_Type = AI_PopInt();
	pst_Dest = AI_PopGameObject();
	AI_EvalFunc_AICBDel_C(pst_GAO, pst_Dest, i_Type, pst_Function);

	return ++_pst_Node;
}

int AICB_Param = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AICBGetParam_C(void)
{
	return AICB_Param;
}
/**/
AI_tdst_Node *AI_EvalFunc_AICBGetParam(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_AICBGetParam_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AICBExecuteAll_C(int type, int param)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	AICB_Param = param;
	_GSP_EndRaster(14);
	AI_ExecCallbackAll(pst_World, type);
	_GSP_BeginRaster(14);
}
/**/
AI_tdst_Node *AI_EvalFunc_AICBExecuteAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~*/
	int i_Type, param;
	/*~~~~~~~~~~~~~~*/

	param = AI_PopInt();
	i_Type = AI_PopInt();
	AI_EvalFunc_AICBExecuteAll_C(i_Type, param);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AICBExecute_C(OBJ_tdst_GameObject *pst_GAO, int type, int param)
{
	AICB_Param = param;
	_GSP_EndRaster(14);
	AI_ExecCallback(pst_GAO, type);
	_GSP_BeginRaster(14);
}
/**/
AI_tdst_Node *AI_EvalFunc_AICBExecute(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	int					i_Type, param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	param = AI_PopInt();
	i_Type = AI_PopInt();
	AI_EvalFunc_AICBExecute_C(pst_GAO, i_Type, param);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_AIReinitUniv(AI_tdst_Node *_pst_Node)
{
	AI_ReinitUniverse();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ReinitVars_C(OBJ_tdst_GameObject *_pst_GO)
{
	AI_ReinitInstance(_pst_GO->pst_Extended->pst_Ai);
}
/**/
AI_tdst_Node *AI_EvalFunc_AIReinitVars(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	AI_Check(pst_GAO->pst_Extended, "Object does not have AI");
	AI_Check(pst_GAO->pst_Extended->pst_Ai, "Object does not have AI");
	AI_ReinitVars_C(pst_GAO);
	return ++_pst_Node;
}

extern int	WOR_gi_CurrentConsole;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AIGetCurSystem_C(void)
{
	return WOR_gi_CurrentConsole;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIGetCurSystem(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_AIGetCurSystem_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AIRunContext_C(OBJ_tdst_GameObject *_pst_GO, int flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Instance = (AI_tdst_Instance *) _pst_GO->pst_Extended->pst_Ai;
	pst_Instance->ul_Flags &= ~0xFF;
	pst_Instance->ul_Flags |= flag;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIRunContext(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GAO);
	AI_Check(pst_GAO->pst_Extended, "Object does not have AI");
	AI_Check(pst_GAO->pst_Extended->pst_Ai, "Object does not have AI");
	AI_EvalFunc_AIRunContext_C(pst_GAO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AIClearStack_C(void)
{
	AI_gul_SizeGlobalStack = AI_gul_SizeGlobalVarStack = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_AIClearStack(AI_tdst_Node *_pst_Node)
{
	AI_gul_SizeGlobalStack = AI_gul_SizeGlobalVarStack = 0;
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AITriggerIsValid_C(SCR_tt_Trigger *pt_Trigger)
{
	if(!pt_Trigger) return 0;
	if(!pt_Trigger->pt_Proc) return 0;
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_AITriggerIsValid(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	SCR_tt_Trigger		*pt_Trigger;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	pt_Trigger = (SCR_tt_Trigger *) st_Var.pv_Addr;
	AI_PushInt(AI_EvalFunc_AITriggerIsValid_C(pt_Trigger));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AITriggerSetInvalid_C(SCR_tt_Trigger *pt_Trigger)
{
	if( pt_Trigger )
		pt_Trigger->pt_Proc = NULL;
	
}
/**/
AI_tdst_Node *AI_EvalFunc_AITriggerSetInvalid(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	SCR_tt_Trigger		*pt_Trigger;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	pt_Trigger = (SCR_tt_Trigger *) st_Var.pv_Addr;
	AI_EvalFunc_AITriggerSetInvalid_C(pt_Trigger);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AITriggerGetMsg_C(SCR_tt_Trigger *pt_Trigger, AI_tdst_Message *res)
{
	if(!pt_Trigger) return;
	L_memcpy(res, &pt_Trigger->t_Msg, sizeof(pt_Trigger->t_Msg));
}
/**/
AI_tdst_Node *AI_EvalFunc_AITriggerGetMsg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	SCR_tt_Trigger		*pt_Trigger;
	AI_tdst_Message		msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	pt_Trigger = (SCR_tt_Trigger *) st_Var.pv_Addr;
	AI_EvalFunc_AITriggerGetMsg_C(pt_Trigger, &msg);
	AI_PushVal(&msg, AI_gaw_EnumLink[TYPE_MESSAGE]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_AITriggerSetMsg_C(SCR_tt_Trigger *pt_Trigger, AI_tdst_Message *res)
{
	if(!pt_Trigger) return;
	L_memcpy(&pt_Trigger->t_Msg, res, sizeof(pt_Trigger->t_Msg));
}
/**/
AI_tdst_Node *AI_EvalFunc_AITriggerSetMsg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	SCR_tt_Trigger		*pt_Trigger;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val, &st_Var);
	pt_Trigger = (SCR_tt_Trigger *) st_Var.pv_Addr;
	AI_EvalFunc_AITriggerSetMsg_C(pt_Trigger, (AI_tdst_Message *) st_Var1.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_AISFDynGet_C(OBJ_tdst_GameObject *pst_GO, int num, int min, int max)
{
	int	i;

	for(i = min; i < max; i++)
	{
		if(!AI_gst_SFDyn[i].ul_Key)
		{
			AI_gst_SFDyn[i].ul_Key = pst_GO->ul_MyKey;
			AI_gst_SFDyn[i].ul_Num = num;
			AI_gst_SFDyn[i].ul_Val = i;
#ifdef ACTIVE_EDITORS
			/*
			{
				char sz_Msg[ 512 ];
				sprintf (sz_Msg, "SFDynGet : %3d %s (%s)", i, 
						(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated) ? "-GENERATED-" : "", 
						pst_GO->sz_Name ? pst_GO->sz_Name : "?" );
				LINK_PrintStatusMsg( sz_Msg );
			}
			*/
#endif
			return AI_gst_SFDyn[i].ul_Val;
		}
		else if(AI_gst_SFDyn[i].ul_Key == pst_GO->ul_MyKey && AI_gst_SFDyn[i].ul_Num == num)
			return AI_gst_SFDyn[i].ul_Val;
	}

	/*
	i = AI_gst_SFDyn[ min ].ul_Val;
	L_memmove( AI_gst_SFDyn + min, AI_gst_SFDyn + min + 1, (max - min - 1) * sizeof( AI_gst_SFDyn[0] ) );
	AI_gst_SFDyn[ max - 1 ].ul_Key = pst_GO->ul_MyKey;
	AI_gst_SFDyn[ max - 1 ].ul_Num = num;
	AI_gst_SFDyn[ max - 1 ].ul_Val = i;
#ifdef ACTIVE_EDITORS
	{
		char sz_Msg[ 512 ];
		sprintf (sz_Msg, "SFDynGet : %3d %s (%s)", i, 
				(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated) ? "-GENERATED-" : "", 
				pst_GO->sz_Name ? pst_GO->sz_Name : "?" );
				LINK_PrintStatusMsg( sz_Msg );
	}
#endif
	return AI_gst_SFDyn[max - 1].ul_Val;
	*/
	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_AISFDynGet(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject	*pst_GO;
	int					num, min, max;

	AI_M_GetCurrentObject(pst_GO);
	max = AI_PopInt();
	min = AI_PopInt();
	num = AI_PopInt();
	AI_PushInt(AI_EvalFunc_AISFDynGet_C(pst_GO, num, min, max));
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
