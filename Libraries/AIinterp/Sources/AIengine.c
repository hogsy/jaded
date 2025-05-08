/*$T AIengine.c GC! 1.081 08/21/02 08:57:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AImsg.h"
#include "AIinterp/Sources/AIsave.h"
#include "AIinterp/Sources/AIBench.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "AIdebug.h"
#include "AI2C_fctheader.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "TIMer/TIMdefs.h"
#include "BASe/BENch/BENch.h"
#define AI2C_NODEFINE
#include "AIinterp/Sources/AI2C_fctheader.h"
#undef AI2C_NODEFINE

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    AI to C
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef AI2C
#ifdef PSX2_TARGET
int AI2C_ai2Ccan = 1;
#else
int AI2C_ai2Ccan = 1;
#endif

int testtest(void);

extern int TCHN_rec;

/*$3
 =======================================================================================================================
    others
 =======================================================================================================================
 */
 
#ifdef GAMECUBE_USE_AI2C_DLL
#define AI2C_FCTDEF(a, b) {a, &AI2C_pfi_##b, #b},
#else  // GAMECUBE_USE_AI2C_DLL
#define AI2C_FCTDEF(a, b) {a, b, #b},
#endif // GAMECUBE_USE_AI2C_DLL

#define AI2C_FCTDEFTRIGGER(a, b, c)
AI2C_fctdef AI2C_gat_fctdefs[] =
{
#include "AI2C_fctdefs.h"
	{BIG_C_InvalidKey, testtest, NULL}
};
#undef AI2C_FCTDEF
#undef AI2C_FCTDEFTRIGGER

/*$3
 =======================================================================================================================
 =======================================================================================================================
 */

int testtest(void)
{
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI2C_i_SizeofArray(void)
{
	return sizeof(AI2C_gat_fctdefs) / sizeof(AI2C_fctdef);
}

#endif /* AI2C */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Categ.
 -----------------------------------------------------------------------------------------------------------------------
 */

#define DEFINE_CATEG(a, b, c) {b, c},
AI_tdst_Eval AI_gast_Categs[] =
{
#include "Categs/AIdefcateg.h"
};

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Types.
 -----------------------------------------------------------------------------------------------------------------------
 */

#define DEFINE_CONSTANT(a, b, c)
#define DEFINE_TYPE(a, b, c, d, e) {b, d, e},
AI_tdst_EvalVar AI_gast_Types[] =
{
#include "Types/AIdeftyp.h"
};

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Keywords.
 -----------------------------------------------------------------------------------------------------------------------
 */

#define DEFINE_KEYWORD(a, b, c, d) {b, d},
AI_tdst_Eval AI_gast_Keywords[] =
{
#include "Keywords/AIdefkey.h"
};

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Functions.
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j) {b, f, 0},
#else
#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j) {b, f},
#endif /* ACTIVE_EDITORS */
AI_tdst_Eval AI_gast_Functions[] =
{
#include "Functions/AIdeffct.h"
};

#ifdef AI_FULL_RASTERS
#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j) {c},
char *_AI_gast_Functions_Names_[] =
{
#include "Functions/AIdeffct.h"
};
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Fields.
 -----------------------------------------------------------------------------------------------------------------------
 */

#define DEFINE_FIELD(a, b, c, d, e, f) {b, f},
AI_tdst_Eval AI_gast_Fields[] =
{
#include "Fields/AIdeffields.h"
};

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Stacks
 -----------------------------------------------------------------------------------------------------------------------
 */

char				AI_gac_LocalStack[AI_C_LenLocalStack];
char				AI_gac_GlobalStack[AI_C_LenGlobalStack];
char				AI_gac_UserStack[AI_C_LenUserStack];

ULONG				AI_gul_SizeLocalStack = 0;
ULONG				AI_gul_SizeGlobalStack = 0;
ULONG				AI_gul_SizeGlobalVarStack = 0;
ULONG				AI_gul_SizeGlobalStack_SAVE[500] = { 0 };
ULONG				AI_gul_SizeGlobalVarStack_SAVE[500] = {0 };

AI_tdst_PushVar		AI_gast_GlobalVarStack[AI_C_LenGlobalVarStack];
AI_tdst_PushVar		AI_gast_UserVarStack[AI_C_LenUserVarStack];
ULONG				AI_gul_SizeUserStack = 0;
ULONG				AI_gul_SizeUserVarStack = 0;
ULONG				AI_gul_CurrentTrack = 0;
BOOL				AI_gb_CanBreakSystem = TRUE;
BOOL				AI_gb_CanBreak = TRUE;


AI_tdst_Function	*AI_gpst_CurrentFunction = NULL;
AI_tdst_Node		**AI_gppst_CurrentJumpNode = NULL;
AI_tdst_Instance	*AI_gpst_CurrentInstance = NULL;
AI_tdst_Instance	*AI_gpst_CurrentInstanceUltra = NULL;
OBJ_tdst_GameObject *AI_gpst_CurrentGameObject = NULL;
OBJ_tdst_GameObject *AI_gpst_CurrentUltra = NULL;
int					AI_gi_RunFctLevel = 0;

BOOL				AI_gb_InterpForVar = FALSE;
BOOL				AI_gb_FullRasters = FALSE;

/*
 * OBJ_tdst_GameObject *AI_gpst_MainActorsDUM = NULL; /* A LAISSER AVANT MAIN
 * ACTOR !!!!!
 */
OBJ_tdst_GameObject *AI_gpst_MainActorsProut[AI_C_MainActorsNum + 1] = { 0, 0, 0 };
OBJ_tdst_GameObject **AI_gpst_MainActors = AI_gpst_MainActorsProut + 1;

/*
 * This array will link IDs of functions, types etc... to the position to their
 * array
 */
short				AI_gaw_EnumLink[AI_ENUM_LAST];

/* Pour execution différée */
OBJ_tdst_GameObject *AI_gapst_Differed[AI_CXT_Max][1000];
int					AI_gai_Differed[AI_CXT_Max];

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
int			AI_gi_WatchNum = 0;
AI_Watch	AI_gst_Watch[AI_MAX_WATCH];
char		AI_gac_WatchBuf[AI_MAXBUF_WATCH];
#endif
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_CheckWatchBegin(void)
{
	/*~~~~~*/
	int i;
	int last;
	/*~~~~~*/

	if(!AI_gi_WatchNum) return;

	last = 0;
	for(i = 0; i < AI_gi_WatchNum; i++)
	{
		L_memcpy(AI_gac_WatchBuf + last, AI_gst_Watch[i].p_Data, AI_gst_Watch[i].Size);
		last += AI_gst_Watch[i].Size;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_CheckWatchEnd(void)
{
	/*~~~~~*/
	int i;
	int last;
	/*~~~~~*/

	if(!AI_gi_WatchNum) return 0;
	last = 0;
	for(i = 0; i < AI_gi_WatchNum; i++)
	{
		if(L_memcmp(AI_gac_WatchBuf + last, AI_gst_Watch[i].p_Data, AI_gst_Watch[i].Size))
		{
			return 1;
		}

		last += AI_gst_Watch[i].Size;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL AI_bIsWatched(void *p)
{
    int i;
	if(!AI_gi_WatchNum) 
        return FALSE;

    ERR_X_Assert(AI_gi_WatchNum <= AI_MAX_WATCH);
	for(i = 0; i < AI_gi_WatchNum; i++)
	{
    	if (AI_gst_Watch[i].p_Data == p)
            return TRUE;
    }
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_RemoveWatch(void *p)
{
    int i;
	if(!AI_gi_WatchNum) 
        return;

    ERR_X_Assert(AI_gi_WatchNum <= AI_MAX_WATCH);
	for(i = 0; i < AI_gi_WatchNum; i++)
	{
    	if (AI_gst_Watch[i].p_Data == p)
        {
            AI_gi_WatchNum--;
            AI_gst_Watch[i].p_Data = AI_gst_Watch[AI_gi_WatchNum].p_Data;
            AI_gst_Watch[i].Size = AI_gst_Watch[AI_gi_WatchNum].Size;
            return;
        }
    }

    ERR_X_Warning("Cannot remove ai watch that does not exist",0,0);
}

void AI_RemoveWatchOnAddressRange(void *pBegin,void *pEnd)
{
    int i;
	if(!AI_gi_WatchNum) 
        return;

    ERR_X_Assert(AI_gi_WatchNum <= AI_MAX_WATCH);
	for(i = 0; i < AI_gi_WatchNum; i++)
	{
        if ( (char *)AI_gst_Watch[i].p_Data >= (char *)pBegin  && 
            (char *)AI_gst_Watch[i].p_Data < (char *)pEnd)
        {
            AI_gi_WatchNum--;
            AI_gst_Watch[i].p_Data = AI_gst_Watch[AI_gi_WatchNum].p_Data;
            AI_gst_Watch[i].Size = AI_gst_Watch[AI_gi_WatchNum].Size;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_AddWatch(void *p, int s)
{
	if(AI_gi_WatchNum == AI_MAX_WATCH) return;
	AI_gst_Watch[AI_gi_WatchNum].p_Data = (char*)p;
	AI_gst_Watch[AI_gi_WatchNum++].Size = s;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_DelAllWatch(void)
{
	AI_gi_WatchNum = 0;
}

#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern void AI2C_UnloadFixModelList(void);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_Reset(void)
{
	int	i;

	AI_gul_SizeLocalStack = 0;
	AI_gul_SizeGlobalStack = 0;
	AI_gul_SizeGlobalVarStack = 0;
	AI_gul_SizeUserStack = 0;
	AI_gul_SizeUserVarStack = 0;
	AI_gpst_CurrentFunction = NULL;
	AI_gpst_CurrentInstance = NULL;
	AI_gpst_CurrentInstanceUltra = NULL;
	AI_gpst_CurrentGameObject = NULL;
	AI_gpst_CurrentUltra = NULL;
	AI_gi_RunFctLevel = 0;
	AI_gul_CurrentTrack = 0;
	for(i = 0; i < AI_CXT_Max; i++) AI_gai_Differed[i] = 0;

#ifdef ACTIVE_EDITORS
	AI2C_UnloadFixModelList();
#endif
}

/*
 =======================================================================================================================
    Aim:    This function fill the global array AI_gaw_EnumLink. This array will contain the link between an id of a
            type, function etc... (the id is the index in that array), and the position of the corresponding type,
            function etc... in it's AI array. £
            We fill the array one time. It will be use for the AI first pass to resolve references (AI_ResolveFctRef
            function).

    Note:   If an assert is raised, that means that there's two types, functions etc... with the same id.
 =======================================================================================================================
 */
void AI_ResolveEnums(void)
{
	/*~~*/
	int i;
	/*~~*/

	L_memset(AI_gaw_EnumLink, sizeof(AI_gaw_EnumLink), 0);

	for(i = 0; i < (int) (sizeof(AI_gast_Categs) / sizeof(AI_gast_Categs[0])); i++)
	{
		ERR_X_Assert(!AI_gaw_EnumLink[AI_gast_Categs[i].w_ID]);
		AI_gaw_EnumLink[AI_gast_Categs[i].w_ID] = i;
	}

	for(i = 0; i < (int) (sizeof(AI_gast_Types) / sizeof(AI_gast_Types[0])); i++)
	{
		ERR_X_Assert(!AI_gaw_EnumLink[AI_gast_Types[i].w_ID]);
		AI_gaw_EnumLink[AI_gast_Types[i].w_ID] = i;
	}

	for(i = 0; i < (int) (sizeof(AI_gast_Keywords) / sizeof(AI_gast_Keywords[0])); i++)
	{
		ERR_X_Assert(!AI_gaw_EnumLink[AI_gast_Keywords[i].w_ID]);
		AI_gaw_EnumLink[AI_gast_Keywords[i].w_ID] = i;
	}

	for(i = 0; i < (int) (sizeof(AI_gast_Functions) / sizeof(AI_gast_Functions[0])); i++)
	{
		ERR_X_Assert(!AI_gaw_EnumLink[AI_gast_Functions[i].w_ID]);
		AI_gaw_EnumLink[AI_gast_Functions[i].w_ID] = i;
	}

	for(i = 0; i < (int) (sizeof(AI_gast_Fields) / sizeof(AI_gast_Fields[0])); i++)
	{
		ERR_X_Assert(!AI_gaw_EnumLink[AI_gast_Fields[i].w_ID]);
		AI_gaw_EnumLink[AI_gast_Fields[i].w_ID] = i;
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_FreeVarDes(AI_tdst_VarDes *_pst_VarDes)
{
	/*~~~~~~*/
#ifdef ACTIVE_EDITORS
	ULONG	i;
#endif
	/*~~~~~~*/

	if(!_pst_VarDes) return;

	/* Unregister all pointers */
	LOA_DeleteAddress(_pst_VarDes);

	/* Free all pointers */
	if(_pst_VarDes->pc_BufferInit) MEM_Free(_pst_VarDes->pc_BufferInit);
	if(_pst_VarDes->pst_VarsInfos) MEM_Free(_pst_VarDes->pst_VarsInfos);
	if(_pst_VarDes->pst_EditorVarsInfos) MEM_Free(_pst_VarDes->pst_EditorVarsInfos);
#ifdef ACTIVE_EDITORS
	if(_pst_VarDes->pst_EditorVarsInfos2)
	{
		for(i = 0; i < _pst_VarDes->ul_NbVarsInfos2; i++)
		{
			if(_pst_VarDes->pst_EditorVarsInfos2[i].psz_StringCst)
				MEM_Free(_pst_VarDes->pst_EditorVarsInfos2[i].psz_StringCst);
			if(_pst_VarDes->pst_EditorVarsInfos2[i].psz_StringHelp)
				MEM_Free(_pst_VarDes->pst_EditorVarsInfos2[i].psz_StringHelp);
		}

		MEM_Free(_pst_VarDes->pst_EditorVarsInfos2);
	}

#endif
	MEM_Free(_pst_VarDes);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_FreeModel(AI_tdst_Model *_pst_Model)
{
	/*~~~~~~*/
	ULONG	i, j;
	/*~~~~~~*/

	/* Unregister all pointers */
	for(i = 0; i < _pst_Model->ul_NbFunctions; i++) LOA_DeleteAddress(_pst_Model->ppst_Functions[i]);
	for(i = 0; i < _pst_Model->u32_NbProcList; i++) LOA_DeleteAddress(_pst_Model->pp_ProcList[i]);
	LOA_DeleteAddress(_pst_Model);
	LINK_DelRegisterPointer(_pst_Model);

	/* Free all pointers */
	AI_FreeVarDes(_pst_Model->pst_VarDes);
	for(i = 0; i < _pst_Model->ul_NbFunctions; i++)
	{
		if(!_pst_Model->ppst_Functions[i]) continue;
		_pst_Model->ppst_Functions[i]->uw_NbRefs--;
		if(!_pst_Model->ppst_Functions[i]->uw_NbRefs)
		{
			if(_pst_Model->ppst_Functions[i]->pst_RootNode)
				MEM_FreeAlign(_pst_Model->ppst_Functions[i]->pst_RootNode);
#ifdef ACTIVE_EDITORS
			if(_pst_Model->ppst_Functions[i]->pst_OtherRootNode)
				MEM_Free(_pst_Model->ppst_Functions[i]->pst_OtherRootNode);
			if(_pst_Model->ppst_Functions[i]->pst_LocalVars)
			{
				MEM_Free(_pst_Model->ppst_Functions[i]->pst_LocalVars);
				_pst_Model->ppst_Functions[i]->ul_NbLocals = 0;
			}

#endif
			if(_pst_Model->ppst_Functions[i]->pc_Strings) MEM_Free(_pst_Model->ppst_Functions[i]->pc_Strings);
			if(_pst_Model->ppst_Functions[i]) MEM_Free(_pst_Model->ppst_Functions[i]);
		}
	}

	if(_pst_Model->ppst_Functions) MEM_Free(_pst_Model->ppst_Functions);

	for(i = 0; i < _pst_Model->u32_NbProcList; i++)
	{
		if(!_pst_Model->pp_ProcList[i]) continue;
		_pst_Model->pp_ProcList[i]->uw_NbRefs--;
		if(!_pst_Model->pp_ProcList[i]->uw_NbRefs)
		{
			for(j = 0; j < _pst_Model->pp_ProcList[i]->u16_Num; j++)
			{
				if(_pst_Model->pp_ProcList[i]->pt_All[j].pu32_Nodes) MEM_Free(_pst_Model->pp_ProcList[i]->pt_All[j].pu32_Nodes);
				if(_pst_Model->pp_ProcList[i]->pt_All[j].pz_Name) MEM_Free(_pst_Model->pp_ProcList[i]->pt_All[j].pz_Name);
#ifdef ACTIVE_EDITORS
				if(_pst_Model->pp_ProcList[i]->pt_All[j].pu32_DbgNodes) MEM_Free(_pst_Model->pp_ProcList[i]->pt_All[j].pu32_DbgNodes);
				if(_pst_Model->pp_ProcList[i]->pt_All[j].pst_LocalVars) MEM_Free(_pst_Model->pp_ProcList[i]->pt_All[j].pst_LocalVars);
#endif
			}

			MEM_Free(_pst_Model->pp_ProcList[i]->pc_Strings);
			MEM_Free(_pst_Model->pp_ProcList[i]->pt_All);
			MEM_Free(_pst_Model->pp_ProcList[i]);
		}
	}

	if(_pst_Model->pp_ProcList) MEM_Free(_pst_Model->pp_ProcList);
	MEM_Free(_pst_Model);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_FreeInstance(AI_tdst_Instance *_pst_Instance)
{
	int	i;

	if(!_pst_Instance) return;

	/* Must we delete the model ? */
	if(!_pst_Instance->pst_Model) return;

	if(AI_gb_Optim2)
	{
		/* Libération des tableaux */
		if(_pst_Instance->pst_Model->pst_VarDes)
		{
			for(i = 0; i < (int)_pst_Instance->pst_Model->pst_VarDes->ul_NbVars; i++)
			{
				int i_Offset2, size1, i_NumElem;
				short w_TypeIndex;
				char *p;

				if(!(_pst_Instance->pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Pointer)) continue;

				w_TypeIndex = _pst_Instance->pst_Model->pst_VarDes->pst_VarsInfos[i].w_Type;
				i_NumElem = _pst_Instance->pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem;
				size1 = AI_gast_Types[w_TypeIndex].w_Size * (i_NumElem & 0x0FFFFFFF);

				i_Offset2 = _pst_Instance->pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset;
				p = &_pst_Instance->pc_VarsBuffer[i_Offset2] + ((i_NumElem >> 30) * sizeof(int));

				MEM_Free(*(void **) p);
			}
		}
	}

	_pst_Instance->pst_Model->uw_NbInstances--;
	if(!_pst_Instance->pst_Model->uw_NbInstances) AI_FreeModel(_pst_Instance->pst_Model);

	/* Unregister pointer */
	LOA_DeleteAddress(_pst_Instance);
	LINK_DelRegisterPointer(_pst_Instance);

	/* Callbacks */
	if(_pst_Instance->pst_CB) MEM_Free(_pst_Instance->pst_CB);

	/* Free all pointers */
	if(_pst_Instance->pst_VarDes) AI_FreeVarDes(_pst_Instance->pst_VarDes);
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
	if(_pst_Instance->pc_VarsBufferInit) MEM_Free(_pst_Instance->pc_VarsBufferInit);
#endif

	if(_pst_Instance->pc_VarsBuffer) MEM_Free(_pst_Instance->pc_VarsBuffer);
	MEM_Free(_pst_Instance);
}

/*$4
 ***********************************************************************************************************************
    EXECUTION
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_dum(AI_tdst_Node *_pst_Node)
{
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_dumvar(void *p, void *p1)
{
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_RunNode(AI_tdst_Node *_pst_Node)
{
	/* Zap a null ultra ? */
	if
	(
		(_pst_Node->l_Param == 0)
	&&	(AI_gast_Categs[(_pst_Node + 1)->c_Type].w_ID == CATEG_KEYWORD)
	&&	(AI_gast_Keywords[(_pst_Node + 1)->w_Param].w_ID == KEYWORD_ULTRA)
	)
	{
		_pst_Node += 2;
	}

	while(((unsigned int) _pst_Node > AI_CR_Max) && (_pst_Node->c_Type != CATEG_ENDTREE))
	{
		ERR_X_Assert(AI_gast_Categs[_pst_Node->c_Type].pfn_Func != NULL);
		_pst_Node = AI_gast_Categs[_pst_Node->c_Type].pfn_Func(_pst_Node);
	}
}

static AI_tdst_Node *currentExecutionNode = NULL;

/**
 * hogsy: Fills and returns a breakpoint based on the current execution state.
 */
AI_tdst_BreakPoint *AI_FillBreakPoint( AI_tdst_BreakPoint *self, int node )
{
	if ( node == -1 )
	{
		if ( currentExecutionNode == NULL )
		{
			currentExecutionNode = *AI_gppst_CurrentJumpNode;
		}

		node = currentExecutionNode - AI_gpst_CurrentFunction->pst_RootNode;
		while ( ( node > 0 ) && ( AI_gpst_CurrentFunction->pst_OtherRootNode[ node ].l_Param == -1 ) ) node--;
		assert( node >= 0 );
	}

	self->i_Line  = AI_gpst_CurrentFunction->pst_OtherRootNode[ node ].l_Param;
	self->ul_File = BIG_ul_SearchKeyToFat( *( ( LONG * ) &AI_gpst_CurrentFunction->pst_OtherRootNode[ node ].w_Param ) );

	/* Engine context */
	self->pst_Function = AI_gpst_CurrentFunction;
	self->pst_Model    = AI_gpst_CurrentInstance->pst_Model;
	self->pst_Instance = AI_gpst_CurrentInstance;
	self->pt_Node      = AI_gpst_CurrentFunction->pst_RootNode;

	return self;
}

/*
 =======================================================================================================================
    Aim:    To interpret an IA function.
 =======================================================================================================================
 */
AI_tdst_Node *AI_pst_RunFunction(AI_tdst_Function *_pst_Function, AI_tdst_Node **_ppst_JumpNode, int _i_OfStack)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	int				i, line;
	int				i_StartTime;
	AI_tdst_Node	*pst_BkpNode;
	AI_tdst_Node	*pst_MemNode;
#endif
	AI_tdst_Node	*pst_CurrentNode;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_gppst_CurrentJumpNode = _ppst_JumpNode;
	AI_gpst_CurrentFunction = _pst_Function;

	/* AI2C function ? */
#ifdef AI2C
	if(AI2C_ai2Ccan)
	{
		if(_pst_Function->pfn_CFunc)
		{
			return (AI_tdst_Node *) _pst_Function->pfn_CFunc();
		}
	}

#endif /* AI2C */

	JADED_PROFILER_START();

#ifdef BENCH_IA_TEST
	AIBnch_BeforeRunFunc(_pst_Function);
#endif

#ifdef ACTIVE_EDITORS
	i_StartTime = 0;
	line = -1;
#endif

#ifdef AI_FULL_RASTERS
	if(AI_gb_FullRasters)
	{
		if ( !_pst_Function->aaa )
		{
			PRO_FirstInitTrameRaster(
			        &_pst_Function->st_Raster,
			        "Trame Loop",
			        "AI Functions",
			        L_strdup( _pst_Function->az_Name ),
			        PRO_E_Time,
			        0 );

			_pst_Function->aaa = 1;
		}

		PRO_StartTrameRaster( &_pst_Function->st_Raster );
	}
#endif

	/* Local stack size */
	AI_gul_SizeLocalStack += _i_OfStack;

	/* Parse all nodes */
	pst_CurrentNode = *_ppst_JumpNode;
	while(((unsigned int) pst_CurrentNode >= AI_CR_Max) && (pst_CurrentNode->c_Type != CATEG_ENDTREE))
	{
		ERR_X_Assert(AI_gast_Categs[pst_CurrentNode->c_Type].pfn_Func != NULL);

		/* Save informations in global vars */
		AI_gppst_CurrentJumpNode = _ppst_JumpNode;

#ifdef ACTIVE_EDITORS
		pst_BkpNode          = pst_CurrentNode;
		currentExecutionNode = pst_BkpNode;

		/* Test for infinite loops */
		if(i_StartTime >= 10000000)
		{
			ERR_X_ForceError("AI seems to loop...", NULL);
			goto com_break;
		}

		i_StartTime++;

		/* Detection of a breakpoint ? */
		if
		(
			AI_gi_NumBreak
		&&	AI_gpst_CurrentFunction
		&&	AI_gb_CanBreak
		&&	(
				(pst_CurrentNode->c_Flags & AI_Cflag_BreakPoint)
			||	(pst_CurrentNode->c_Flags & AI_Cflag_ForceBreakPoint)
			)
		)
		{
			/* Search current debug line */
			line = pst_BkpNode - AI_gpst_CurrentFunction->pst_RootNode;
			while((line > 0) && (AI_gpst_CurrentFunction->pst_OtherRootNode[line].l_Param == -1)) line--;
			ERR_X_Assert(line >= 0);

			i = AI_SearchBreakPoint
				(
					AI_gpst_CurrentFunction,
					AI_gpst_CurrentInstance->pst_Model,
					AI_gpst_CurrentInstance,
					AI_gpst_CurrentFunction->pst_OtherRootNode[line].l_Param
				);

			/* Line number */
			if((pst_CurrentNode->c_Flags & AI_Cflag_ForceBreakPoint) || i)
			{
com_break:
				pst_BkpNode->c_Flags &= ~AI_Cflag_ForceBreakPoint;

				if(line == -1)
				{
					i = pst_BkpNode - AI_gpst_CurrentFunction->pst_RootNode;
					while((i > 0) && (AI_gpst_CurrentFunction->pst_OtherRootNode[i].l_Param == -1)) i--;
					ERR_X_Assert(i >= 0);
				}
				else
				{
					i = line;
				}

				AI_FillBreakPoint( &AI_gst_BreakInfo, i );

				AI_gb_ExitByBreak = TRUE;
				AI_gi_FctLevelWhenBreak = AI_gi_RunFctLevel;

				/* Save stack content */
				AI_M_DebugInStack(a);
				if(L_setjmp(AI_gst_ContextIn) == 0) L_longjmp(AI_gst_ContextOut, 1);

				/* Restore stack content */
				AI_M_DebugOutStack(a);
				AI_gb_ExitByBreak = FALSE;

				/* Restore global */
				AI_gpst_CurrentFunction = AI_gst_BreakInfo.pst_Function;
				AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra = AI_gst_BreakInfo.pst_Instance;

				if(!(((unsigned int) pst_CurrentNode >= AI_CR_Max) && (pst_CurrentNode->c_Type != CATEG_ENDTREE)))
					break;
			}
			else
				goto onestep;
		}

		/* One step mode */
		else
		{
onestep:
			if(AI_gi_OneStepMode && AI_gi_OneStepMode != 3 && AI_gpst_CurrentFunction)
			{
				if(!AI_gi_RunFctLevel || (AI_gi_OneStepMode == 2) || (AI_gi_RunFctLevel <= AI_gi_FctLevelWhenBreak))
				{
					i = pst_CurrentNode - AI_gpst_CurrentFunction->pst_RootNode;
					line = -1;
					if
					(
						(pst_CurrentNode == AI_gpst_CurrentFunction->pst_RootNode)
					||	(AI_gst_BreakInfo.i_Line != AI_gpst_CurrentFunction->pst_OtherRootNode[i].l_Param)
					||	(AI_gst_BreakInfo.pst_Function != AI_gpst_CurrentFunction)
					)
					{
						if(AI_gpst_CurrentFunction->pst_OtherRootNode[i].l_Param != 0xFFFFFFFF) goto com_break;
					}
				}
			}
		}

		/* System breakpoint ? */
		if
		(
			AI_gpst_CurrentFunction
		&&	AI_gb_CanBreakSystem
		&&	AI_gast_Categs[pst_CurrentNode->c_Type].w_ID == CATEG_FUNCTION
		)
		{
			if(AI_gast_Functions[pst_CurrentNode->w_Param].b_SysBreak)
			{
				i = pst_CurrentNode - AI_gpst_CurrentFunction->pst_RootNode;
				while((i > 0) && (AI_gpst_CurrentFunction->pst_OtherRootNode[i].l_Param == -1)) i--;
				ERR_X_Assert(i >= 0);
				if
				(
					(AI_gst_BreakInfo.i_Line != AI_gpst_CurrentFunction->pst_OtherRootNode[i].l_Param)
				||	(AI_gst_BreakInfo.pst_Function != AI_gpst_CurrentFunction)
				||	(AI_gst_BreakInfo.pst_Instance != AI_gpst_CurrentInstance)
				)
				{
					line = -1;
					goto com_break;
				}
			}
		}

		/* In case of an error in evaluation */
		if(L_setjmp(AI_gst_ContextCheck))
		{
			AI_gb_ErrorWhenBreak = TRUE;
			goto com_break;
		}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#endif /* ACTIVE_EDITORS */
		/* Interpret node */
#ifdef ACTIVE_EDITORS
		AI_CheckWatchBegin();
		pst_MemNode = pst_CurrentNode;
#endif

		pst_CurrentNode = AI_gast_Categs[pst_CurrentNode->c_Type].pfn_Func(pst_CurrentNode);

#ifdef ACTIVE_EDITORS
		if(AI_CheckWatchEnd())
		{
			AI_gpst_CurrentFunction = _pst_Function;
			pst_BkpNode = pst_MemNode;
			MessageBox(0, "Watch Break", "AI", MB_OK);
			goto com_break;
		}
#endif
	}

#ifdef ACTIVE_EDITORS
	if(AI_gi_OneStepMode == 3) goto com_break;
#endif

#ifdef BENCH_IA_TEST
	AIBnch_AfterRunFunc(_pst_Function);
#endif
	/* The first node is just for size of the local stack */
	AI_gul_SizeLocalStack -= _i_OfStack;

#ifdef AI_FULL_RASTERS
	if(AI_gb_FullRasters) 
	PRO_StopTrameRaster(&_pst_Function->st_Raster);
#endif

	JADED_PROFILER_END();

	return pst_CurrentNode;
}

/*
 =======================================================================================================================
    Aim:    To interpret IA of an instance.
 =======================================================================================================================
 */
BOOL AI_RunInstance(AI_tdst_Instance *_pst_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	AI_tdst_Node	*pst_JumpNode;
	BOOL			res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Instance) return TRUE;
	if(!_pst_Instance->pst_Model) return TRUE;
	if(_pst_Instance->pst_Model->c_ModelInvalid) return TRUE;

#ifdef AI_FULL_RASTERS
	if(AI_gb_FullRasters)
	if(!_pst_Instance->aaa)
	{
		char	az[100];
		strcpy(az, ((OBJ_tdst_GameObject *) _pst_Instance->pst_GameObject)->sz_Name);
		PRO_FirstInitTrameRaster
		(
			&_pst_Instance->st_Raster,
			"Trame Loop",
			"AI Instances",
			L_strdup(az),
			PRO_E_Time,
			0
		);

		_pst_Instance->aaa = 1;
	}

	if(AI_gb_FullRasters)
	PRO_StartTrameRaster(&_pst_Instance->st_Raster);
#endif

	/* Scan all tracks */
_Try_
	AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra = _pst_Instance;
	AI_gi_RunFctLevel = 0;
	res = TRUE;
	for(i = _pst_Instance->uw_FirstTrack; i < _pst_Instance->uw_LastTrack + 1; i++)
	{
		AI_gul_SizeGlobalStack = 0;
		AI_gul_SizeGlobalVarStack = 0;
		AI_gul_SizeUserStack = 0;
		AI_gul_SizeUserVarStack = 0;
		AI_gul_CurrentTrack = i;

		/* Wait before execute track */
		if(_pst_Instance->af_WaitTime[i] > 0)
		{
			_pst_Instance->af_WaitTime[i] -= TIM_gf_dt;
			if(_pst_Instance->af_WaitTime[i] < 0.0f) _pst_Instance->af_WaitTime[i] = 0;
		}

		/* Execute function if it exists, and if we must not wait */
		if((_pst_Instance->af_WaitTime[i] <= 0) && _pst_Instance->apst_CurrentFct[i])
		{
			/* Execute function */
			pst_JumpNode = AI_pst_RunFunction(_pst_Instance->apst_CurrentFct[i], &_pst_Instance->apst_JumpNode[i], 0);

			/*
			 * Reset metaaction if we have reached the end of the tree. In that case,
			 * pst_JumpNode must not be null and must adress the CATEG_ENDTREE type. £
			 * if pst_JumpNode is NULL, we do not modify _pst_Instance->apst_JumpNode[i] cause
			 * it has been modifyed by interpretation.
			 */
			if((int) pst_JumpNode != AI_CR_Destroy)
			{
				if(pst_JumpNode && _pst_Instance->apst_CurrentFct[i])
				{
#ifdef AI2C
					if(_pst_Instance->apst_CurrentFct[i]->pfn_CFunc)
						*(int *) AI_gppst_CurrentJumpNode = 0xFFFFFFFF;
					else
						*AI_gppst_CurrentJumpNode = _pst_Instance->apst_CurrentFct[i]->pst_RootNode;
#else
					*AI_gppst_CurrentJumpNode = _pst_Instance->apst_CurrentFct[i]->pst_RootNode;
#endif
				}
			}

			if((int) pst_JumpNode == AI_CR_Destroy) { res = TRUE; goto l_End; }
			if((int) pst_JumpNode == AI_CR_StopInstance) { res = TRUE; goto l_End; }
			if((int) pst_JumpNode == AI_CR_StopAI) { res = FALSE; goto l_End; }
			if((int) pst_JumpNode == AI_CR_CurChangeNow) i--;
#ifdef ACTIVE_EDITORS
			if((int) pst_JumpNode != AI_CR_CurChangeNow) TCHN_rec = 0;
#endif
		}
	}

_Catch_
_EndThrow_

l_End:

#ifdef AI_FULL_RASTERS
	if(AI_gb_FullRasters)
	PRO_StopTrameRaster(&_pst_Instance->st_Raster);
#endif

	return res;
}

/*
 =======================================================================================================================
    Aim:    Main call of the dynamic
 =======================================================================================================================
 */
void AI_MainCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	BOOL				b_Father;
	AI_tdst_Instance	*pst_Instance;
	int					num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef BENCH_IA_TEST
	AIBnch_BeginFrameCall();
#endif
#ifdef ACTIVE_EDITORS
	L_memset(&AI_gst_BreakInfo, 0, sizeof(AI_gst_BreakInfo));
	ERR_gpst_ContextGAO = NULL;
	ERR_gpsz_ContextString = "AI";
#endif
	EVE_gpst_CurrentTrack = NULL;
	pst_AIEOT = &(_pst_World->st_EOT.st_AI);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
#ifdef ACTIVE_EDITORS
		u32 LocalTicks;
#endif
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		ULONG CurrentTicksNum;
		extern ULONG ShowNormals;
#endif
#endif
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;


#ifdef ACTIVE_EDITORS
		LocalTicks = TIM_ul_GetLowPartTimerInternalCounter();
		ERR_gpst_ContextGAO = pst_GAO;
#endif
		/* Hierarchy */
		if(OBJ_b_HasBeenProcess(pst_GAO, _pst_World->ul_ProcessCounterAI, OBJ_C_ProcessedAI)) continue;

		/* Optim culled ? */
		if(pst_GAO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_OptimCulled && !pst_GAO->uc_LOD_Vis) continue;

		b_Father = FALSE;
		while
		(
			pst_GAO->pst_Base
		&&	pst_GAO->pst_Base->pst_Hierarchy
		&&	pst_GAO->pst_Base->pst_Hierarchy->pst_Father
		&&	!OBJ_b_HasBeenProcess
			(
				pst_GAO->pst_Base->pst_Hierarchy->pst_Father,
				_pst_World->ul_ProcessCounterAI,
				OBJ_C_ProcessedAI
			)
		)
		{
			pst_GAO = pst_GAO->pst_Base->pst_Hierarchy->pst_Father;
			b_Father = TRUE;
		}

		if(b_Father) pst_CurrentElem--;
		OBJ_SetProcess(pst_GAO, _pst_World->ul_ProcessCounterAI, OBJ_C_ProcessedAI);

		if(pst_GAO->pst_Extended && pst_GAO->pst_Extended->pst_Ai)
		{
			/*
			 * Check that the object has not been desactivated y a previous AI call of this
			 * loop
			 */
			AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = pst_GAO;
			if(!OBJ_b_TestControlFlag(pst_GAO, OBJ_C_ControlFlag_ForceInactive) && !OBJ_b_TestControlFlag(pst_GAO, OBJ_C_ControlFlag_SectoInactive))
			{
				/* Differ ? */
				pst_Instance = (AI_tdst_Instance *) pst_GAO->pst_Extended->pst_Ai;
				if(pst_Instance->ul_Flags & 0xFF) 
				{
					num = pst_Instance->ul_Flags & 0xFF;
					ERR_X_Assert(AI_gai_Differed[num] != 1000);
					AI_gapst_Differed[num][AI_gai_Differed[num]++] = pst_GAO;
					continue;
				}

#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
				if (ShowNormals) 
				{
					CurrentTicksNum = scePcGetCounter0(); // Not closed
				}
#endif
#endif
				if(!AI_RunInstance((AI_tdst_Instance *) pst_GAO->pst_Extended->pst_Ai)) break;
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
				if (ShowNormals) 
				{
					CurrentTicksNum = scePcGetCounter0() - CurrentTicksNum; // Not closed
					pst_GAO->EngineTick += CurrentTicksNum;
				}
#endif
#endif
#ifdef ACTIVE_EDITORS
				pst_GAO->EngineTicksNumber = TIM_ul_GetLowPartTimerInternalCounter() - LocalTicks;
#endif

			}
		}
		
	}

#ifdef BENCH_IA_TEST
	AIBnch_EndFrameCall();
#endif
	/* For hierarchy */
	_pst_World->ul_ProcessCounterAI++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ExecCallbackAll(WOR_tdst_World *_pst_World, int _i_Num)
{
	JADED_PROFILER_START();

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* All callbacks */
	pst_AIEOT = &(_pst_World->st_EOT.st_AI);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		ULONG CurrentTicksNum;
		extern ULONG ShowNormals;
#endif
#endif
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		if (ShowNormals) 
		{
			CurrentTicksNum = scePcGetCounter0(); // Not closed
		}
#endif
#endif
		AI_ExecCallback(pst_GAO, _i_Num);
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		if (ShowNormals) 
		{
			CurrentTicksNum = scePcGetCounter0() - CurrentTicksNum; // Not closed
			pst_GAO->EngineTick += CurrentTicksNum;
		}
#endif
#endif
		
	}

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_RunContext(WOR_tdst_World *_pst_World, int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	OBJ_tdst_GameObject	**ppst_GAO;
	int					*pinum;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!ENG_gb_EngineRunning) return;

	/* Differed */
	switch(_i_Num)
	{
	case AI_C_Callback_AfterRec:
		ppst_GAO = &AI_gapst_Differed[AI_CXT_AfterRec][0];
		pinum = &AI_gai_Differed[AI_CXT_AfterRec];
		break;
	case AI_C_Callback_AfterBlend:
		ppst_GAO = &AI_gapst_Differed[AI_CXT_AfterBlend][0];
		pinum = &AI_gai_Differed[AI_CXT_AfterBlend];
		break;
	}

	for(i = 0; i < *pinum; i++)
	{
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		ULONG CurrentTicksNum;
		extern ULONG ShowNormals;
#endif
#endif
		AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = ppst_GAO[i];
		if( !OBJ_b_TestControlFlag(AI_gpst_CurrentGameObject, OBJ_C_ControlFlag_ForceInactive) )
		{
#ifdef ACTIVE_EDITORS
			u32 LocalTicks ;
			LocalTicks = TIM_ul_GetLowPartTimerInternalCounter();
#endif

#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
			if (ShowNormals) 
			{
				CurrentTicksNum = scePcGetCounter0(); // Not closed
			}
#endif
#endif
			if(!AI_RunInstance((AI_tdst_Instance *) ppst_GAO[i]->pst_Extended->pst_Ai)) 
				i = *pinum;
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
			if (ShowNormals) 
			{
				CurrentTicksNum = scePcGetCounter0() - CurrentTicksNum; // Not closed
				ppst_GAO[i]->EngineTick += CurrentTicksNum;
			}
#endif
#endif
#ifdef ACTIVE_EDITORS
			ppst_GAO[i]->EngineTicksNumber = TIM_ul_GetLowPartTimerInternalCounter() - LocalTicks;
#endif

		}
	}

	*pinum = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_DeleteFromRunContext( OBJ_tdst_GameObject *_pst_GAO )
{
	AI_tdst_Instance	*pst_Instance;
	int					i, num;
	
	if ( !_pst_GAO || !(_pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) ) return;
	if ( !_pst_GAO->pst_Extended || !_pst_GAO->pst_Extended->pst_Ai ) return;
	
	pst_Instance = (AI_tdst_Instance *) _pst_GAO->pst_Extended->pst_Ai;
	if(pst_Instance->ul_Flags & 0xFF) 
	{
		num = pst_Instance->ul_Flags & 0xFF;
		for (i = 0; i < AI_gai_Differed[num]; i++)
		{
			if ( AI_gapst_Differed[num][ i ] == _pst_GAO )
			{
				AI_gai_Differed[num]--;
				AI_gapst_Differed[num][ i ] = AI_gapst_Differed[num][ AI_gai_Differed[num] ];
				return;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ExecCallback(OBJ_tdst_GameObject *_pst_GO, int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	AI_tdst_Node		*pst_Node;
	OBJ_tdst_GameObject *pst_SGAO;
	AI_tdst_Instance	*pst_SInstance;
	AI_tdst_Function	*pst_SFunc;
	AI_tdst_Node		**pst_SJump;
	ULONG				ul_SLocalStack;
	ULONG				ul_SGStack1;
	ULONG				ul_SGStack2;
	int					i;
#ifdef ACTIVE_EDITORS
	int					i_OneStep;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(!ENG_gb_EngineRunning) return;
#endif

	if(!_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Ai) return;
	if(OBJ_b_TestControlFlag(_pst_GO, OBJ_C_ControlFlag_ForceInactive)) return;
	if(OBJ_b_TestControlFlag(_pst_GO, OBJ_C_ControlFlag_InPause)) return;
	pst_Instance = (AI_tdst_Instance *) _pst_GO->pst_Extended->pst_Ai;
	if(!pst_Instance->pst_Model) return;
	if(!pst_Instance->i_NumCB) return;
#ifdef ACTIVE_EDITORS
	if(pst_Instance->pst_Model->c_ModelInvalid) return;
#endif

	JADED_PROFILER_START();

	/* Save context */
	pst_SGAO = AI_gpst_CurrentGameObject;
	pst_SInstance = AI_gpst_CurrentInstance;
	pst_SFunc = AI_gpst_CurrentFunction;
	pst_SJump = AI_gppst_CurrentJumpNode;
	ul_SLocalStack = AI_gul_SizeLocalStack;
	ul_SGStack1 = AI_gul_SizeGlobalStack;
	ul_SGStack2 = AI_gul_SizeGlobalVarStack;
	ENG_gb_AIRunning = TRUE;
#ifdef ACTIVE_EDITORS
	i_OneStep = AI_gi_OneStepMode;
	AI_gi_OneStepMode = 0;
#endif

	for(i = 0; i < pst_Instance->i_NumCB; i++)
	{
		if(pst_Instance->pst_CB[i].pst_Func && (pst_Instance->pst_CB[i].i_Type == _i_Num))
		{
			pst_Node = pst_Instance->pst_CB[i].pst_Func->pst_RootNode;
			AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = (OBJ_tdst_GameObject *) pst_Instance->pst_CB[i].pst_GameObject;
			AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra = (AI_tdst_Instance *) AI_gpst_CurrentGameObject->pst_Extended->pst_Ai;
#ifdef ACTIVE_EDITORS
			if(!AI_gpst_CurrentInstance->pst_Model || AI_gpst_CurrentInstance->pst_Model->c_ModelInvalid) continue;
#endif

			AI_gi_RunFctLevel++;
			AI_pst_RunFunction(pst_Instance->pst_CB[i].pst_Func, &pst_Node, AI_gpst_CurrentFunction ? AI_gpst_CurrentFunction->i_SizeLocalStack : 0);
			AI_gi_RunFctLevel--;
		}
	}

	/* Restore context */
	ENG_gb_AIRunning = FALSE;
	AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = pst_SGAO;
	AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra = pst_SInstance;
	AI_gpst_CurrentFunction = pst_SFunc;
	AI_gppst_CurrentJumpNode = pst_SJump;
	AI_gul_SizeLocalStack = ul_SLocalStack;
	AI_gul_SizeGlobalStack = ul_SGStack1;
	AI_gul_SizeGlobalVarStack = ul_SGStack2;
#ifdef ACTIVE_EDITORS
	AI_gi_OneStepMode = i_OneStep;
#endif

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ReinitUniverse(void)
{
	AI_ReinitInstance(WOR_gst_Universe.pst_AI);
}

int abcd = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ReinitInstance(AI_tdst_Instance *_pst_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i, ul_SizeArr, ul_NumElem;
	int				i_Type, i_Offset, i_Offset1, i_Offset2;
	AI_tdst_Model	*pst_Model;
	char			*pc_Init;
	AI_tdst_VarDes	*pst_VarDes;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Instance) return;

	pst_Model = _pst_Instance->pst_Model;
	if(!pst_Model) return;

	_pst_Instance->ul_Flags &= ~0xFF;
	if(WOR_gst_Universe.pst_AI != _pst_Instance)	/* Not for universe */
	{
		_pst_Instance->uw_FirstTrack = AI_C_MaxTracks;
		_pst_Instance->uw_LastTrack = 0;

		for(i = 0; i < AI_C_MaxTracks; i++)
		{
			if(_pst_Instance->pst_Model->pst_VarDes->apst_InitFct[i] && i < _pst_Instance->uw_FirstTrack)
				_pst_Instance->uw_FirstTrack = (unsigned short) i;
			if(_pst_Instance->pst_Model->pst_VarDes->apst_InitFct[i] && i > _pst_Instance->uw_LastTrack)
				_pst_Instance->uw_LastTrack = (unsigned short) i;

			/* Initial track */
			if(_pst_Instance->pst_Model->pst_VarDes)
				_pst_Instance->apst_CurrentFct[i] = _pst_Instance->pst_Model->pst_VarDes->apst_InitFct[i];
			else
				_pst_Instance->apst_CurrentFct[i] = NULL;
			if(_pst_Instance->apst_CurrentFct[i])
			{
				_pst_Instance->apst_JumpNode[i] = _pst_Instance->apst_CurrentFct[i]->pst_RootNode;
				if((_pst_Instance->apst_JumpNode[i] == NULL) && _pst_Instance->apst_CurrentFct[i]->pfn_CFunc)
					*(int *) &_pst_Instance->apst_JumpNode[i] = -1;
			}
			else
				_pst_Instance->apst_JumpNode[i] = NULL;

			/* Time to wait */
			_pst_Instance->af_WaitTime[i] = 0.0f;
		}

		/* Callbacks */
		if(_pst_Instance->pst_CB)
		{
			MEM_Free(_pst_Instance->pst_CB);
			_pst_Instance->pst_CB = NULL;
			_pst_Instance->i_NumCB = 0;
		}
	}

	/* Initial values for variables */
	if(!pst_Model->pst_VarDes) return;
	for(i = 0; i < pst_Model->pst_VarDes->ul_NbVars; i++)
	{
		pst_VarDes = pst_Model->pst_VarDes;
		if(WOR_gst_Universe.pst_AI != _pst_Instance || pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Reinit)
		{
			i_Type = pst_VarDes->pst_VarsInfos[i].w_Type;
			i_Offset = i_Offset2 = pst_VarDes->pst_VarsInfos[i].i_Offset;
			if(AI_gb_Optim2)
				i_Offset2 = pst_VarDes->pst_VarsInfos[i].i_Offset2;

#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
			pc_Init = _pst_Instance->pc_VarsBufferInit;
			if(!pc_Init) pc_Init = pst_VarDes->pc_BufferInit;
#else
			pc_Init = pst_VarDes->pc_BufferInit;
#endif

			if(_pst_Instance->pc_VarsBuffer)
			{
				/* Copy header of an array if necessary */
				ul_NumElem = pst_VarDes->pst_VarsInfos[i].i_NumElem;
				ul_SizeArr =  ul_NumElem >> 30;
				if(ul_SizeArr)
				{
					if(!AI_gb_Optim2)
						L_memcpy(&_pst_Instance->pc_VarsBuffer[i_Offset], &pc_Init[i_Offset2], ul_SizeArr * 4);
				}

				if(AI_gb_Optim2)
				{
					if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Pointer)
					{
						char *p;

						p = &_pst_Instance->pc_VarsBuffer[i_Offset] + (ul_SizeArr * sizeof(int));

						if(*(void **)p == NULL)
						{
							int		ii, kk, ll, max, size1, mm;
							int		i_Offset11, i_Type1;
							char	*pp;
							int		*ppl;
							int		i_NumElem1;

							/* Allocation des tableaux */
							max = 0;
							if(_pst_Instance->pst_VarDes)
							{
								i_Offset11 = 0;
								for(kk = 0; kk <(int) _pst_Instance->pst_VarDes->ul_NbVars; kk++)
								{
									i_NumElem1 = _pst_Instance->pst_VarDes->pst_VarsInfos[kk].i_NumElem;
									i_Type1 = _pst_Instance->pst_VarDes->pst_VarsInfos[kk].w_Type;
									if(_pst_Instance->pst_VarDes->pst_VarsInfos[kk].i_Offset == pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset)
									{
										pp = _pst_Instance->pst_VarDes->pc_BufferInit + i_Offset11;
										pp += (i_NumElem1 >> 30) * 4;
										for(mm = 0; mm < (i_NumElem1 & 0x0FFFFFFF); mm++)
										{
											ppl = (int *) pp;
											for(ll = 0; ll < AI_gast_Types[i_Type].w_Size / 4; ll++)
											{
												if(ppl[ll]) break;
											}

											if(ll == AI_gast_Types[i_Type].w_Size / 4) break;
											max++;
											pp += AI_gast_Types[i_Type].w_Size;
										}

										break;
									}

									i_Offset11 += (i_NumElem1 & 0x3FFFFFFF) * AI_gast_Types[i_Type1].w_Size;
									i_Offset11 += (i_NumElem1 >> 30) * 4;
								}
							}

							max = (ULONG)(max + 1) > (ul_NumElem & 0x3FFFFFFF) ? (ul_NumElem & 0x3FFFFFFF) : max + 1;
							size1 = AI_gast_Types[i_Type].w_Size * max;
							abcd += (AI_gast_Types[i_Type].w_Size * (ul_NumElem & 0x0FFFFFFF)) - size1;

///
((int *) p)[-1] = max;
ul_NumElem=max;
//size1 = AI_gast_Types[i_Type].w_Size * (i_NumElem & 0x0FFFFFFF);
///

							*(void **) p = (char *) MEM_p_VMAlloc(size1);
						}

						ul_NumElem = ((int *) p)[-1];
						L_memcpy
						(
							*(void **) p, 
							&pc_Init[i_Offset2] + (ul_SizeArr * sizeof(int)), 
							(ul_NumElem & 0x3FFFFFFF) * AI_gast_Types[i_Type].w_Size
						);
					}
					else
					{
						L_memcpy
						(
							&_pst_Instance->pc_VarsBuffer[i_Offset] + (ul_SizeArr * sizeof(int)),
							&pc_Init[i_Offset2] + (ul_SizeArr * sizeof(int)),
							(ul_NumElem & 0x3FFFFFFF) * AI_gast_Types[i_Type].w_Size
						);
					}
				}
				else
				{
					/*
					* Copy content of variable (0x3FFFFFFF is cause we have number of dims of array
					* in high bits)
					*/
					L_memcpy
					(
						&_pst_Instance->pc_VarsBuffer[i_Offset] + (ul_SizeArr * sizeof(int)),
						&pc_Init[i_Offset] + (ul_SizeArr * sizeof(int)),
						(ul_NumElem & 0x3FFFFFFF) * AI_gast_Types[i_Type].w_Size
					);
				}
			}
		}
	}

#if !defined ACTIVE_EDITORS && defined(AI_OPTIM)
	i_Offset1 = 0;
	for(i = 0; i < _pst_Instance->pst_VarDes->ul_NbVars; i++)
	{
		pst_VarDes = _pst_Instance->pst_VarDes;
		if(WOR_gst_Universe.pst_AI != _pst_Instance || pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Reinit)
		{
			i_Type = pst_VarDes->pst_VarsInfos[i].w_Type;
			i_Offset = pst_VarDes->pst_VarsInfos[i].i_Offset;
			pc_Init = pst_VarDes->pc_BufferInit;

			if(_pst_Instance->pc_VarsBuffer)
			{
				/* Copy header of an array if necessary */
				ul_NumElem = pst_VarDes->pst_VarsInfos[i].i_NumElem;
				ul_SizeArr =  ul_NumElem >> 30;

				if(ul_SizeArr)
				{
//					if(!AI_gb_Optim2)
//						L_memcpy(&_pst_Instance->pc_VarsBuffer[i_Offset], &pc_Init[i_Offset1], ul_SizeArr * 4);
					i_Offset1 += (ul_SizeArr * 4);
				}

				if(AI_gb_Optim2)
				{
					int mmax;

					if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Pointer)
					{
						char *p;
						p = &_pst_Instance->pc_VarsBuffer[i_Offset] + (ul_SizeArr * sizeof(int));
//////////////
mmax = ((int *)p)[-1];
//mmax = i_NumElem & 0x3FFFFFFF;
//////////////
						L_memcpy
						(
							*(void **)p,
							&pc_Init[i_Offset1],
							mmax * AI_gast_Types[i_Type].w_Size
						);
					}
					else
					{
						L_memcpy
						(
							&_pst_Instance->pc_VarsBuffer[i_Offset] + (ul_SizeArr * sizeof(int)),
							&pc_Init[i_Offset1],
							(ul_NumElem & 0x3FFFFFFF) * AI_gast_Types[i_Type].w_Size
						);
					}
				}
				else
				{
					/*
					* Copy content of variable (0x3FFFFFFF is cause we have number of dims of array
					* in high bits)
					*/
					L_memcpy
					(
						&_pst_Instance->pc_VarsBuffer[i_Offset] + (ul_SizeArr * sizeof(int)),
						&pc_Init[i_Offset1],
						(ul_NumElem & 0x3FFFFFFF) * AI_gast_Types[i_Type].w_Size
					);
				}

				i_Offset1 += (ul_NumElem & 0x3FFFFFFF) * AI_gast_Types[i_Type].w_Size;
			}
		}
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_DuplicateInstance(OBJ_tdst_GameObject *_pst_Src, OBJ_tdst_GameObject *_pst_Dest, BOOL _b_CreateFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	AI_tdst_Instance	*pst_SrcInst;
	AI_tdst_VarDes		*pst_VarDes;
#ifdef ACTIVE_EDITORS
	BIG_INDEX			ul_Index, ul_Index1;

#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_Src->pst_Extended) || !(_pst_Src->pst_Extended->pst_Ai)) return;

	pst_SrcInst = (AI_tdst_Instance *) _pst_Src->pst_Extended->pst_Ai;
	if ( !pst_SrcInst->pst_Model ) return;

	/* Allocate a new instance */
	pst_Instance = (AI_tdst_Instance *) MEM_p_Alloc(sizeof(AI_tdst_Instance));

	L_memcpy(pst_Instance, pst_SrcInst, sizeof(AI_tdst_Instance));
	pst_Instance->i_NumCB = 0;
	pst_Instance->pst_CB = NULL;
	_pst_Dest->pst_Extended->pst_Ai = pst_Instance;

	/* Allocate a new buffer for variables */
	if(pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit)
	{
		/* Current vars buffer */
		pst_Instance->pc_VarsBuffer = (char *) MEM_p_Alloc(pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit);

		/* Init vars buffer */
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
		pst_Instance->pc_VarsBufferInit = (char *) MEM_p_Alloc(pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit);
		if(pst_SrcInst->pc_VarsBufferInit)
		{
			L_memcpy
			(
				pst_Instance->pc_VarsBufferInit,
				pst_SrcInst->pc_VarsBufferInit,
				pst_Instance->pst_Model->pst_VarDes->ul_SizeBufferInit
			);
		}
#endif
	}

	/* Special variables des */
	if(pst_SrcInst->pst_VarDes)
	{
		pst_VarDes = (AI_tdst_VarDes *) MEM_p_Alloc(sizeof(AI_tdst_VarDes));
		L_memcpy(pst_VarDes, pst_SrcInst->pst_VarDes, sizeof(AI_tdst_VarDes));
		pst_Instance->pst_VarDes = pst_VarDes;

		/* Init buffer */
		if(pst_VarDes->ul_SizeBufferInit)
		{
			pst_VarDes->pc_BufferInit = (char *) MEM_p_Alloc(pst_VarDes->ul_SizeBufferInit);
			L_memcpy(pst_VarDes->pc_BufferInit, pst_SrcInst->pst_VarDes->pc_BufferInit, pst_VarDes->ul_SizeBufferInit);
		}

		/* Vardes */
		if(pst_VarDes->ul_NbVars)
		{
			pst_VarDes->pst_VarsInfos = (AI_tdst_VarInfo *) MEM_p_Alloc(pst_VarDes->ul_NbVars * sizeof(AI_tdst_VarInfo));
			L_memcpy
			(
				pst_VarDes->pst_VarsInfos,
				pst_SrcInst->pst_VarDes->pst_VarsInfos,
				pst_VarDes->ul_NbVars * sizeof(AI_tdst_VarInfo)
			);

			/* Editor vardes */
			pst_VarDes->pst_EditorVarsInfos = (AI_tdst_EditorVarInfo *) MEM_p_Alloc(pst_VarDes->ul_NbVars * sizeof(AI_tdst_EditorVarInfo));
			L_memcpy
			(
				pst_VarDes->pst_EditorVarsInfos,
				pst_SrcInst->pst_VarDes->pst_EditorVarsInfos,
				pst_VarDes->ul_NbVars * sizeof(AI_tdst_EditorVarInfo)
			);
		}

		/* No complex infos for an instance */
#ifdef ACTIVE_EDITORS
		pst_VarDes->ul_NbVarsInfos2 = 0;
		pst_VarDes->pst_EditorVarsInfos2 = NULL;
#endif
	}

	LINK_RegisterDuplicate(pst_SrcInst, pst_Instance, 0);
	pst_Instance->pst_Model->uw_NbInstances++;
	AI_ReinitInstance(pst_Instance);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Create a corresponding file ?
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
	pst_Instance->pst_GameObject = _pst_Dest;
	if(_b_CreateFile)
	{
		/* To retreive name of object */
		ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Src);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);

		/* Model of AI instance */
		ul_Index1 = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance->pst_Model);
		ul_Index1 = BIG_ul_SearchKeyToFat(ul_Index1);

		ul_Index = AI_ul_CreateInstance(_pst_Dest->pst_World, ul_Index, ul_Index1, _pst_Dest->sz_Name);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
		LOA_AddAddress(ul_Index, pst_Instance);
_Try_
		AI_SaveInstance(pst_Instance);
_Catch_
_End_
	}

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Real_C_Function(void)
{
}

