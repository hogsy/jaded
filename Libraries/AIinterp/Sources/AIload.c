/*$T AIload.c GC 1.134 05/13/04 17:34:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "AIerrid.h"
#include "EDIpaths.h"
#include "ENGine/Sources/ENGvars.h"
#include "BIGfiles/LOAding/LOAread.h"

#ifdef PSX2_TARGET
#include "BASe/ERRors/ERRdefs.h"
#else
#include "Res/Res.h"
#endif

#include "INOut/INOsaving.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern void AI2C_AddModelInDllContents(void);
extern void OBJ_ResolveAIRefSecondPass(OBJ_tdst_GameObject *);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
char			EDI_gaz_SlashL[1024];
#endif
BOOL			EDI_gb_NoVerbose = FALSE;
BOOL			AI_gb_Optim2 = FALSE;
BOOL			AI_gb_GenOptim2 = FALSE;
extern BOOL		EDI_gb_ComputeMap;
extern BOOL		LOA_gb_SpeedMode;
extern ULONG	LOA_ul_FileTypeSize[40];

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined( _GAMECUBE) || defined(_XENON)
void ReverseEndianess(AI_tdst_Node*pst_Root)
{
	AI_tdst_Node*pNode;
	int i32;
	short i16;
	char i8, j8;

	pNode = pst_Root + 3;

	do 
	{
		i32 = pNode->l_Param; 
		pNode->l_Param =  (i32<<24) & 0xFF000000;
		pNode->l_Param |= (i32<<8)  & 0x00FF0000;
		pNode->l_Param |= (i32>>8)  & 0x0000FF00;
		pNode->l_Param |= (i32>>24) & 0x000000FF;
		if(!(i32 & 0x000000FF)) break;
		if(!(i32 & 0x0000FF00)) break;
		if(!(i32 & 0x00FF0000)) break;
		if(!(i32 & 0xFF000000)) break;
		
		
		i16 = pNode->w_Param; 
		pNode->w_Param =  (i16<<8)  & 0xFF00;
		pNode->w_Param |= (i16>>8)  & 0x00FF;
		if(!(i16 & 0x00FF)) break;
		if(!(i16 & 0xFF00)) break;

		
		i8 = pNode->c_Flags; 
		if(!(i8 & 0xFF)) break;
		
		
		j8 = pNode->c_Type; 
		if(!(j8 & 0xFF)) break;
		
		pNode++;
	}
	while(1);

}
#else
#define  ReverseEndianess(pst_Root)	
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL AI_b_IsARef(int _i_Type)
{
	switch(_i_Type)
	{
	case TYPE_GAMEOBJECT:
	case TYPE_FUNCTIONREF:
	case TYPE_NETWORK:
	case TYPE_MODEL:
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_RealResolveOneRefNoType(ULONG _ul_Ref)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Pos;
	/*~~~~~~~~~~~*/

	ul_Pos = BIG_ul_SearchKeyToPos(_ul_Ref);
	if((int) ul_Pos != -1)
	{
		ul_Pos = LOA_ul_SearchAddress(ul_Pos);
		return ul_Pos;
	}

	return _ul_Ref;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_RealResolveOneRef(int _i_Type, ULONG _ul_Ref)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Pos;
	/*~~~~~~~~~~~*/

	if(_ul_Ref == 0) return 0;
	if(_i_Type == TYPE_GAMEOBJECT)
	{
		/* Main actors */
		switch(_ul_Ref)
		{
		case AI_C_MainActor0:
		case AI_C_MainActor1:
			return 0;	/* Changement : plus de mainactor dans les variables */
		}
	}

#ifdef ACTIVE_EDITORS
	{
		extern ULONG LOA_gul_MakeFileRefSpy ;
		extern ULONG LOA_gul_MakeFileRefSpyCaller ;
		extern ULONG LOA_gul_CurrentKey ;
		extern OBJ_tdst_GameObject*AI_gp_ResolveGO;
		if(LOA_gul_MakeFileRefSpy != -1)
		{
			if(_ul_Ref == LOA_gul_MakeFileRefSpy)
			{
				char log[512];
				ULONG ulFat,ulCaller,ulSave;

				ulSave = LOA_gul_MakeFileRefSpyCaller ;
				if(AI_gp_ResolveGO)
					LOA_gul_MakeFileRefSpyCaller = AI_gp_ResolveGO->ul_MyKey;

				ulCaller = LOA_gul_MakeFileRefSpyCaller == -1 ? LOA_gul_CurrentKey : LOA_gul_MakeFileRefSpyCaller ;
				ulFat = BIG_ul_SearchKeyToFat(ulCaller);
				if(ulFat != BIG_C_InvalidIndex)
				{
					snprintf( log, sizeof(log), "[LOA]=> %x is referenced by [%08x] %s", LOA_gul_MakeFileRefSpy, ulCaller, BIG_NameFile( ulFat ) );

					LINK_gul_ColorTxt = 0x00FF0000;
					LINK_PrintStatusMsg(log);
					LINK_gul_ColorTxt = 0;
				}
				LOA_gul_MakeFileRefSpyCaller = ulSave  ;
			}
		}

        if (AI_gp_ResolveGO)
            AI_vAddKeyDependency(_ul_Ref,AI_gp_ResolveGO->ul_MyKey,"AI_ul_RealResolveOneRef");
	}
#endif

	ul_Pos = BIG_ul_SearchKeyToPos(_ul_Ref);
	if((int) ul_Pos != -1)
	{
		ul_Pos = LOA_ul_SearchAddress(ul_Pos);
#ifdef ACTIVE_EDITORS
		if((ul_Pos == -1) && (_ul_Ref == BIG_gst.st_ToSave.ul_UniverseKey))
		{
			WOR_gst_Universe.pst_AI = AI_pst_LoadInstance(_ul_Ref);
			return(ULONG) WOR_gst_Universe.pst_AI;
		}
#endif
		return ul_Pos;
	}

	return _ul_Ref;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_RealResolveOneRefObject(ULONG _ul_Ref)
{
	return AI_ul_RealResolveOneRef(TYPE_GAMEOBJECT, _ul_Ref);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_RealResolveOneRefFunction(ULONG _ul_Ref)
{
	return AI_ul_RealResolveOneRef(TYPE_FUNCTIONREF, _ul_Ref);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_ResolveOneRef(int _i_Type, ULONG _ul_Ref)
{
	if(AI_b_IsARef(_i_Type)) return AI_ul_RealResolveOneRef(_i_Type, _ul_Ref);
	return _ul_Ref;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL AI_SpecialWParam(char _c_Type)
{
	switch(_c_Type)
	{
	case CATEG_POPPROC: return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ResolveFctRefSimple(AI_tdst_Node *_pst_Node)
{
	while(_pst_Node->c_Type != CATEG_ENDTREE)
	{
		if(!AI_SpecialWParam(_pst_Node->c_Type)) _pst_Node->w_Param = AI_gaw_EnumLink[_pst_Node->w_Param];
		_pst_Node->c_Type = (char) AI_gaw_EnumLink[_pst_Node->c_Type];
		_pst_Node++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ResolveAllFctRef(AI_tdst_Model *_pst_Model)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i, j;
	AI_tdst_Node	*pst_Root;
#ifdef ACTIVE_EDITORS
	char			asz_Msg[512];
	ULONG			ul_Ref, ul_Model;
	ULONG			ul_Key;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Functions */
	for(i = 0; i < _pst_Model->ul_NbFunctions; i++)
	{
		if(!_pst_Model->ppst_Functions[i]) continue;
		pst_Root = _pst_Model->ppst_Functions[i]->pst_RootNode;
		if(pst_Root)
		{
			/* Only transform once each function (functions can be shared) */
			if(pst_Root->c_Flags & AI_Cflag_FctTransform) continue;
			pst_Root->c_Flags |= AI_Cflag_FctTransform;

			/* Transform all nodes */
			while(pst_Root->c_Type != CATEG_ENDTREE)
			{
				if(pst_Root->c_Type == CATEG_TYPE)
				{
					if(pst_Root->w_Param == TYPE_STRING)
					{
						pst_Root->l_Param += (int) _pst_Model->ppst_Functions[i]->pc_Strings;
					}
					else
					{
#ifdef ACTIVE_EDITORS
						ul_Ref = pst_Root->l_Param;
#endif
						pst_Root->l_Param = AI_ul_ResolveOneRef(pst_Root->w_Param, pst_Root->l_Param);
					}

					_Try_
#ifdef ACTIVE_EDITORS
					if((pst_Root->w_Param != TYPE_MODEL) && (AI_b_IsARef(pst_Root->w_Param)))
					{
						ul_Model = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Model);
						snprintf( asz_Msg, sizeof(asz_Msg), "%x, model is %s", ul_Ref, BIG_NameFile( ul_Model ) );
						ERR_X_Warning(pst_Root->l_Param != BIG_C_InvalidIndex, AI_ERR_Csz_RefNotLoaded, asz_Msg);
					}
#endif
					_Catch_ _End_
				}

				/* Change id to position in array */
				if(pst_Root->c_Type == CATEG_KEYWORD && pst_Root->w_Param == KEYWORD_CALLPROC)
				{
					pst_Root->c_Type = (char) AI_gaw_EnumLink[pst_Root->c_Type];
					pst_Root->w_Param = AI_gaw_EnumLink[pst_Root->w_Param];
					ReverseEndianess(pst_Root);
					pst_Root += pst_Root[2].l_Param + 3;
				}
				else
				{
					if(!AI_SpecialWParam(pst_Root->c_Type)) pst_Root->w_Param = AI_gaw_EnumLink[pst_Root->w_Param];
					pst_Root->c_Type = (char) AI_gaw_EnumLink[pst_Root->c_Type];
					pst_Root++;
				}
			}
		}
	}

	/* ProcList */
	for(i = 0; i < _pst_Model->u32_NbProcList; i++)
	{
		if(!_pst_Model->pp_ProcList[i]) continue;
		for(j = 0; j < _pst_Model->pp_ProcList[i]->u16_Num; j++)
		{
			pst_Root = _pst_Model->pp_ProcList[i]->pt_All[j].pu32_Nodes;
			if(pst_Root)
			{
				/* Only transform once each function (functions can be shared) */
				if(pst_Root->c_Flags & AI_Cflag_FctTransform) continue;
				pst_Root->c_Flags |= AI_Cflag_FctTransform;

				/* Transform all nodes */
				while(pst_Root->c_Type != CATEG_ENDTREE)
				{
					if(pst_Root->c_Type == CATEG_TYPE)
					{
						if(pst_Root->w_Param == TYPE_STRING)
						{
							pst_Root->l_Param += (int) _pst_Model->pp_ProcList[i]->pc_Strings;
						}
						else
						{
#ifdef ACTIVE_EDITORS
							ul_Ref = pst_Root->l_Param;
#endif
							pst_Root->l_Param = AI_ul_ResolveOneRef(pst_Root->w_Param, pst_Root->l_Param);
						}

						_Try_
#ifdef ACTIVE_EDITORS
						if((pst_Root->w_Param != TYPE_MODEL) && (AI_b_IsARef(pst_Root->w_Param)))
						{
							ul_Model = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Model);
							snprintf( asz_Msg, sizeof(asz_Msg), "%x, model is %s (proclist)", ul_Ref, BIG_NameFile( ul_Model ) );
							ERR_X_Warning(pst_Root->l_Param != BIG_C_InvalidIndex, AI_ERR_Csz_RefNotLoaded, asz_Msg);
						}
#endif
						_Catch_ _End_
					}

					/* Change id to position in array */
					if(pst_Root->c_Type == CATEG_KEYWORD && pst_Root->w_Param == KEYWORD_CALLPROC)
					{
						pst_Root->c_Type = (char) AI_gaw_EnumLink[pst_Root->c_Type];
						pst_Root->w_Param = AI_gaw_EnumLink[pst_Root->w_Param];
						ReverseEndianess(pst_Root);
						pst_Root += pst_Root[2].l_Param + 3;
					}
					else
					{
						if(!AI_SpecialWParam(pst_Root->c_Type))
							pst_Root->w_Param = AI_gaw_EnumLink[pst_Root->w_Param];
						pst_Root->c_Type = (char) AI_gaw_EnumLink[pst_Root->c_Type];
						pst_Root++;
					}
				}
			}
		}
	}

	/* Inits function for model */
#ifdef ACTIVE_EDITORS
	if(_pst_Model->pst_VarDes)
	{
		for(i = 0; i < AI_C_MaxTracks; i++)
		{
			if(_pst_Model->pst_VarDes->apst_InitFct[i])
			{
				ul_Key = AI_ul_ResolveOneRef(TYPE_FUNCTIONREF, (ULONG) _pst_Model->pst_VarDes->apst_InitFct[i]);
				if(ul_Key != BIG_C_InvalidIndex)
					_pst_Model->pst_VarDes->apst_InitFct[i] = (AI_tdst_Function *) ul_Key;
			}
		}
	}
#endif
}

#ifdef ACTIVE_EDITORS
int					AI_gi_TestContent = 0;
OBJ_tdst_GameObject *AI_gp_ResolveGO = NULL;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ResolveMsg(char *name, AI_tdst_Message *msg)
{
	msg->pst_Sender = (OBJ_tdst_GameObject *) AI_ul_RealResolveOneRef(TYPE_GAMEOBJECT, (ULONG) msg->pst_Sender);
	msg->pst_GAO1 = (OBJ_tdst_GameObject *) AI_ul_RealResolveOneRef(TYPE_GAMEOBJECT, (ULONG) msg->pst_GAO1);
	msg->pst_GAO2 = (OBJ_tdst_GameObject *) AI_ul_RealResolveOneRef(TYPE_GAMEOBJECT, (ULONG) msg->pst_GAO2);
	msg->pst_GAO3 = (OBJ_tdst_GameObject *) AI_ul_RealResolveOneRef(TYPE_GAMEOBJECT, (ULONG) msg->pst_GAO3);
	msg->pst_GAO4 = (OBJ_tdst_GameObject *) AI_ul_RealResolveOneRef(TYPE_GAMEOBJECT, (ULONG) msg->pst_GAO4);
	msg->pst_GAO5 = (OBJ_tdst_GameObject *) AI_ul_RealResolveOneRef(TYPE_GAMEOBJECT, (ULONG) msg->pst_GAO5);
#ifdef ACTIVE_EDITORS
	{
		char az[100];
        if (AI_gp_ResolveGO)
			snprintf( az, sizeof(az), "%s(%08x) : %s", AI_gp_ResolveGO->sz_Name, AI_gp_ResolveGO->ul_MyKey, name ? name : "" );
        else
			snprintf( az, sizeof(az), ": %s", name ? name : "" );
		ERR_X_Warning((int) msg->pst_Sender != -1, "Trigger with a bad object", az);
		ERR_X_Warning((int) msg->pst_GAO1 != -1, "Trigger with a bad object", az);
		ERR_X_Warning((int) msg->pst_GAO2 != -1, "Trigger with a bad object", az);
		ERR_X_Warning((int) msg->pst_GAO3 != -1, "Trigger with a bad object", az);
		ERR_X_Warning((int) msg->pst_GAO4 != -1, "Trigger with a bad object", az);
	}
#endif
}

/*
 =======================================================================================================================
    Aim: Call to resolve initial values of variables (cause there can have some references).
 =======================================================================================================================
 */
void AI_ResolveVarRef
(
	void					*_p_InstModel,
	char					*_pc_BufferInit,
	AI_tdst_EditorVarInfo	*_pst_EdVarsInfos,
	AI_tdst_VarInfo			*_pst_VarsInfos,
	ULONG					_ul_NbVars,
	BOOL					_b_ForModel
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		i, ulNumDim;
	int			j;
	ULONG		*pul_Val;
	SHORT		wType;
	SCR_tt_Trigger *pt_Trigger;
#ifdef ACTIVE_EDITORS
	BIG_INDEX	ul_Index;
	char		asz_Msg[512];
	ULONG		ul_Ref;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pc_BufferInit) return;
	for(i = 0; i < _ul_NbVars; i++)
	{
		/* Change reference (will do nothing if it's not a reference */
		if(AI_gb_Optim2)
			pul_Val = (ULONG *) &_pc_BufferInit[_pst_VarsInfos[i].i_Offset2];
		else
			pul_Val = (ULONG *) &_pc_BufferInit[_pst_VarsInfos[i].i_Offset];

#ifdef ACTIVE_EDITORS
		{
			if(AI_gi_TestContent && ((int) *pul_Val == AI_gi_TestContent))
			{
				/*~~~~~~~~~~~~*/
				char	az[512];
				/*~~~~~~~~~~~~*/

				snprintf( az, sizeof(az), "%s => %s", AI_gp_ResolveGO->sz_Name, _pst_EdVarsInfos[ i ].asz_Name );
				ERR_X_Warning(0, az, NULL);
			}
		}
#endif

		/* Get type of variable (real ID, not offset in array) */
		wType = _pst_VarsInfos[i].w_Type;
		if(!_b_ForModel) wType = AI_gast_Types[wType].w_ID;

		if(wType == TYPE_MESSAGE)
		{
			ulNumDim = (ULONG) _pst_VarsInfos[i].i_NumElem >> 30;
			pul_Val += ulNumDim;
			for(j = 0; j < (_pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF); j++)
			{
				AI_ResolveMsg(NULL, (AI_tdst_Message *) (pul_Val + j));
			}
		}
		else if(wType == TYPE_TRIGGER)
		{
			ulNumDim = (ULONG) _pst_VarsInfos[i].i_NumElem >> 30;
			pul_Val += ulNumDim;
			pt_Trigger = (SCR_tt_Trigger *) pul_Val;
			for(j = 0; j < (_pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF); j++)
			{
				AI_ResolveTrigger(pt_Trigger, FALSE);
#ifdef ACTIVE_EDITORS
				if(pt_Trigger->az_Name[0] && !pt_Trigger->pt_Proc)
				{
					ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _p_InstModel);
					snprintf
					(
						asz_Msg, sizeof(asz_Msg),
						"%s, file is %s, var is %s",
						pt_Trigger->az_Name,
						BIG_NameFile(ul_Index),
						_pst_EdVarsInfos[i].asz_Name
					);

					ERR_X_Warning(0, "Trigger not resolved", asz_Msg);
				}
#endif
				pt_Trigger++;
			}
		}
		else if(AI_b_IsARef(wType))
		{
			/*
			 * Retreive number of dimensions (for an array), and increment buffer cause the
			 * beginning of the buffer for an array contains the size of each dimension
			 */
			ulNumDim = (ULONG) _pst_VarsInfos[i].i_NumElem >> 30;
			pul_Val += ulNumDim;

			for(j = 0; j < (_pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF); j++)
			{
#ifdef ACTIVE_EDITORS
				ul_Ref = *(pul_Val + j);
				if(AI_gi_TestContent && (int) ul_Ref == AI_gi_TestContent)
				{
					/*~~~~~~~~~~~~*/
					char	az[512];
					/*~~~~~~~~~~~~*/

					snprintf( az, sizeof(az), "%s => %s (ARRAY)", AI_gp_ResolveGO->sz_Name, _pst_EdVarsInfos[ i ].asz_Name );
					ERR_X_Warning(0, az, NULL);
				}
#endif
				*(pul_Val + j) = AI_ul_ResolveOneRef(wType, *(pul_Val + j));

#ifdef ACTIVE_EDITORS
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _p_InstModel);
				if(ul_Index == BIG_C_InvalidIndex)
				{
					snprintf( asz_Msg, sizeof(asz_Msg), "%x, var is %s", ul_Ref, _pst_EdVarsInfos[ i ].asz_Name );
				}
				else
				{
					snprintf
					(
					    asz_Msg, sizeof(asz_Msg),
						"%x, file is %s, var is %s",
						ul_Ref,
						BIG_NameFile(ul_Index),
						_pst_EdVarsInfos[i].asz_Name
					);
				}

				ERR_X_Warning(*(pul_Val + j) != BIG_C_InvalidIndex, AI_ERR_Csz_RefNotLoadedVars, asz_Msg);
#endif
			}
		}

#ifdef ACTIVE_EDITORS
		else if(AI_gi_TestContent)
		{
			ulNumDim = (ULONG) _pst_VarsInfos[i].i_NumElem >> 30;
			pul_Val += ulNumDim;

			for(j = 0; j < (_pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF); j++)
			{
				if(AI_gi_TestContent && (int) *(pul_Val + j) == AI_gi_TestContent)
				{
					/*~~~~~~~~~~~~*/
					char	az[512];
					/*~~~~~~~~~~~~*/

					snprintf( az, sizeof(az), "%s => %s (ARRAY)", AI_gp_ResolveGO->sz_Name, _pst_EdVarsInfos[ i ].asz_Name );
					ERR_X_Warning(0, az, NULL);
				}
			}
		}
#endif

		/* Change id to offset in array of types */
		if(_b_ForModel) _pst_VarsInfos[i].w_Type = AI_gaw_EnumLink[_pst_VarsInfos[i].w_Type];
	}
}

#ifdef ACTIVE_EDITORS
static char gsasz_Name[128];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK ResolveCarDecDlg(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT), gsasz_Name);
		ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT1), SW_HIDE);
		return TRUE;

	case WM_COMMAND:
		if((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
		{
			GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT), gsasz_Name, 128);
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}

	return FALSE;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ResolveCarDecHiHiEditor(AI_tdst_Instance *_pst_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i, j, i_Num, ul_SizeArr;
	int				i_Type, i_Offset1, i_Offset2, iNumElem;
	AI_tdst_Model	*pst_Model;
	int				jj;
#ifdef ACTIVE_EDITORS
	char			asz_Msg[512];
	BIG_INDEX		ul_Index, ul_Index1;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Model = _pst_Instance->pst_Model;
	if(!pst_Model) return;

	/* Variables */
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
	L_memcpy
	(
		_pst_Instance->pc_VarsBufferInit,
		pst_Model->pst_VarDes->pc_BufferInit,
		pst_Model->pst_VarDes->ul_SizeBufferInit
	);
#endif
	i_Num = 0;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Check if a var is in instance and not in model
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
	for(i = 0; i < _pst_Instance->pst_VarDes->ul_NbVars; i++)
	{
		for(j = 0; j < pst_Model->pst_VarDes->ul_NbVars; j++)
		{
			if
			(
				!L_strcmpi
					(
						_pst_Instance->pst_VarDes->pst_EditorVarsInfos[i].asz_Name,
						pst_Model->pst_VarDes->pst_EditorVarsInfos[j].asz_Name
					)
			) break;
		}

		if(j == pst_Model->pst_VarDes->ul_NbVars)
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Instance);
			ul_Index1 = LOA_ul_SearchIndexWithAddress((ULONG) pst_Model);
			sprintf
			(
				asz_Msg,
				"A var (%s) is in instance %s, and not in model %s",
				_pst_Instance->pst_VarDes->pst_EditorVarsInfos[i].asz_Name,
				ul_Index == BIG_C_InvalidIndex ? "???" : BIG_NameFile(ul_Index),
				ul_Index1 == BIG_C_InvalidIndex ? "???" : BIG_NameFile(ul_Index1)
			);

			if(EDI_gb_NoVerbose)
			{
				ERR_X_Warning(0, asz_Msg, NULL);
			}
			else
			{
				ERR_X_ForceError(asz_Msg, NULL);
				L_strcpy(gsasz_Name, _pst_Instance->pst_VarDes->pst_EditorVarsInfos[i].asz_Name);

				if
				(
					DialogBox
						(
							MAI_gh_MainInstance,
							MAKEINTRESOURCE(DIALOGS_IDD_NAME),
							MAI_gh_MainWindow,
							ResolveCarDecDlg
						) == IDOK
				)
				{
					L_strcpy(_pst_Instance->pst_VarDes->pst_EditorVarsInfos[i].asz_Name, gsasz_Name);
				}
			}
		}
	}
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Variables of instance
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Initial values of instance */
	AI_ResolveVarRef
	(
		_pst_Instance,
		_pst_Instance->pst_VarDes->pc_BufferInit,
		_pst_Instance->pst_VarDes->pst_EditorVarsInfos,
		_pst_Instance->pst_VarDes->pst_VarsInfos,
		_pst_Instance->pst_VarDes->ul_NbVars,
		TRUE
	);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Resolve
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < pst_Model->pst_VarDes->ul_NbVars; i++)
	{
		/* Search in instance a variable with the same name */
		for(j = 0; j < _pst_Instance->pst_VarDes->ul_NbVars; j++)
		{
			if
			(
				!L_strcmpi
					(
						pst_Model->pst_VarDes->pst_EditorVarsInfos[i].asz_Name,
						_pst_Instance->pst_VarDes->pst_EditorVarsInfos[j].asz_Name
					)
			)
			{
				break;
			}
		}

		/* Have we found the variable ? */
		if(j != _pst_Instance->pst_VarDes->ul_NbVars)
		{
			i_Type = pst_Model->pst_VarDes->pst_VarsInfos[i].w_Type;
			if(!pst_Model->c_SecondPassDone) i_Type = AI_gaw_EnumLink[i_Type];
			i_Offset1 = pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset;
			i_Offset2 = _pst_Instance->pst_VarDes->pst_VarsInfos[j].i_Offset;

			/* Blindage vincent : pour eviter un plantage avec un offset = 0x10000000 */
			if((ULONG) i_Offset2 > _pst_Instance->pst_VarDes->ul_SizeBufferInit)
			{
				ERR_X_Warning(0, "Resolve AI : a var has a bad offset", NULL);
				i_Offset2 = 0;
			}

			/* Array header */
			iNumElem = _pst_Instance->pst_VarDes->pst_VarsInfos[j].i_NumElem;
			ul_SizeArr = (ULONG) iNumElem >> 30;
			if(iNumElem == pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem)
			{
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
				if(ul_SizeArr)
				{
					L_memcpy
					(
						&_pst_Instance->pc_VarsBufferInit[i_Offset1],
						&pst_Model->pst_VarDes->pc_BufferInit[i_Offset1],
						ul_SizeArr * sizeof(int)
					);
				}

				/* Copy the instance value in the instance init buffer */
				L_memcpy
				(
					&_pst_Instance->pc_VarsBufferInit[i_Offset1] + (ul_SizeArr * 4),
					&_pst_Instance->pst_VarDes->pc_BufferInit[i_Offset2] + (ul_SizeArr * 4),
					AI_gast_Types[i_Type].w_Size * (iNumElem & 0x3FFFFFFF)
				);
#else
				_pst_Instance->pst_VarDes->pst_VarsInfos[j].i_Offset = i_Offset1;
				_pst_Instance->pst_VarDes->pst_VarsInfos[j].w_Flags = pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags;
#endif
			}
			else
			{
				ul_SizeArr = (ULONG) pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem >> 30;
				jj = iNumElem & 0x3FFFFFFF;
				if(jj > (pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF))
					jj = pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF;
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
				L_memcpy
				(
					&_pst_Instance->pc_VarsBufferInit[i_Offset1] + (ul_SizeArr * 4),
					&_pst_Instance->pst_VarDes->pc_BufferInit[i_Offset2] + (ul_SizeArr * 4),
					AI_gast_Types[i_Type].w_Size * (jj & 0x3FFFFFFF)
				);
#else
				_pst_Instance->pst_VarDes->pst_VarsInfos[j].i_Offset = i_Offset1;
				_pst_Instance->pst_VarDes->pst_VarsInfos[j].w_Flags = pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags;
#endif
			}

			i_Num++;
		}

		/* Is it the end ? */
		if(i_Num == _pst_Instance->pst_VarDes->ul_NbVars) break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_InstanceAfterLoading(AI_tdst_Instance *_pst_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Model	*pst_Model;
	int				size;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Model = _pst_Instance->pst_Model;
#ifdef ACTIVE_EDITORS
	if(!pst_Model)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		char		asz_Msg[256];
		BIG_INDEX	ul_Index;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Instance);
		snprintf( asz_Msg, sizeof(asz_Msg), "Instance doesn't have a model !!! (%s)", BIG_NameFile( ul_Index ) );
		ERR_X_Warning(0, asz_Msg, NULL);
		return;
	}
#endif
	ERR_X_Assert(pst_Model);

	/* Instance already resolved */
	if(_pst_Instance->pc_VarsBuffer) return;

	/* Increase number of instances for that model */
	pst_Model->uw_NbInstances++;

	_pst_Instance->pc_VarsBuffer = NULL;
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
	_pst_Instance->pc_VarsBufferInit = NULL;
#endif

	if(!pst_Model->pst_VarDes) return;

	if(AI_gb_Optim2)
	{
		unsigned int i = 0, j;
		int dec, size1;
		short	w_TypeIndex;
		int		i_NumElem;

		if(pst_Model->c_SecondPassDone != 0x66)
		{
			if(pst_Model->pst_VarDes->ul_NbVars == 16)
				i = i;

			/* On comptabilise la vraie taille, chaque tableau ne prenant plus que 4 (le pointeur) plus un int par dimension. */
			size = 0;
			for(i = 0; i < pst_Model->pst_VarDes->ul_NbVars; i++)
			{
				pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset2 = pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset;
				w_TypeIndex = pst_Model->pst_VarDes->pst_VarsInfos[i].w_Type;
				if(!pst_Model->c_SecondPassDone) w_TypeIndex = AI_gaw_EnumLink[w_TypeIndex];
				i_NumElem = pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem;
				if(pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Pointer)
				{
					pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags |= AI_CVF_Pointer;
					size += 4;
					size += (i_NumElem >> 30) * 4;
				}
				else
				{
					if(i_NumElem >> 30)
					{
						size += AI_gast_Types[w_TypeIndex].w_Size * (i_NumElem & 0x0FFFFFFF);
						size += ((ULONG) i_NumElem >> 30) * 4;
					}
					else
						size += AI_gast_Types[w_TypeIndex].w_Size * (i_NumElem & 0x0FFFFFFF);
				}
			}

			/* Calcul du offset2 qui est le vraie offset après décalage */
			for(i = 0; i < pst_Model->pst_VarDes->ul_NbVars; i++)
			{
				if(!(pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Pointer)) continue;

				w_TypeIndex = pst_Model->pst_VarDes->pst_VarsInfos[i].w_Type;
				if(!pst_Model->c_SecondPassDone) w_TypeIndex = AI_gaw_EnumLink[w_TypeIndex];
				i_NumElem = pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem;
				size1 = AI_gast_Types[w_TypeIndex].w_Size * (i_NumElem & 0x0FFFFFFF);
				dec = size1 - 4;

				for(j = 0; j < pst_Model->pst_VarDes->ul_NbVars; j++)
				{
					if(i == j) continue;
					if(pst_Model->pst_VarDes->pst_VarsInfos[j].i_Offset2 > pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset2)
						pst_Model->pst_VarDes->pst_VarsInfos[j].i_Offset -= dec;
				}
			}

			pst_Model->pst_VarDes->ul_SizeBufferInit = size;
			pst_Model->c_SecondPassDone = 0x66;
		}

/*		for(i = 0; i < _pst_Instance->pst_VarDes->ul_NbVars; i++)
		{
			for(j = 0; j < pst_Model->pst_VarDes->ul_NbVars; j++)
			{
				if(_pst_Instance->pst_VarDes->pst_VarsInfos[i].i_Offset == pst_Model->pst_VarDes->pst_VarsInfos[j].i_Offset2)
				{
					_pst_Instance->pst_VarDes->pst_VarsInfos[i].w_Flags = pst_Model->pst_VarDes->pst_VarsInfos[j].w_Flags;
					_pst_Instance->pst_VarDes->pst_VarsInfos[i].i_Offset = pst_Model->pst_VarDes->pst_VarsInfos[j].i_Offset;
					break;
				}
			}
		}*/
	}


	/* Allocate buffer for current values of instance */
	if(pst_Model->pst_VarDes->ul_SizeBufferInit)
		_pst_Instance->pc_VarsBuffer = (char *) MEM_p_VMAlloc(pst_Model->pst_VarDes->ul_SizeBufferInit);

	if(AI_gb_Optim2)
	{
		int		i_Offset2;
		char	*p;
		short	w_TypeIndex;
		int		i_NumElem;

		/* Allocation des tableaux */
		for(unsigned int i = 0; i < pst_Model->pst_VarDes->ul_NbVars; i++)
		{
			if(!(pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Pointer)) continue;

			w_TypeIndex = pst_Model->pst_VarDes->pst_VarsInfos[i].w_Type;
			if(!pst_Model->c_SecondPassDone) w_TypeIndex = AI_gaw_EnumLink[w_TypeIndex];
			i_NumElem = pst_Model->pst_VarDes->pst_VarsInfos[i].i_NumElem;
			i_Offset2 = pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset;
			p = &_pst_Instance->pc_VarsBuffer[i_Offset2] + ((i_NumElem >> 30) * sizeof(int));
			*(void **) p = NULL;
		}
	}

	/* Allocate buffer for init values of instance */
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
	if(pst_Model->pst_VarDes->ul_SizeBufferInit)
		_pst_Instance->pc_VarsBufferInit = (char *) MEM_p_VMAlloc(pst_Model->pst_VarDes->ul_SizeBufferInit);
#endif

	/* Resolve cardec */
	if(_pst_Instance->pst_VarDes)
	{
		/* Resolve problems */
		AI_ResolveCarDecHiHiEditor(_pst_Instance);
#ifndef ACTIVE_EDITORS
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
		AI_FreeVarDes(_pst_Instance->pst_VarDes);
		_pst_Instance->pst_VarDes = NULL;
#endif
#endif
	}
	else
	{
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
		L_memcpy
		(
			_pst_Instance->pc_VarsBufferInit,
			pst_Model->pst_VarDes->pc_BufferInit,
			pst_Model->pst_VarDes->ul_SizeBufferInit
		);
#endif
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ResolveModelRef(AI_tdst_Model *_pst_Model)
{
	if(!_pst_Model) return;
	if(!_pst_Model->c_SecondPassDone)
	{
		/* Functions references */
		AI_ResolveAllFctRef(_pst_Model);

		/* Variables of model */
		if(_pst_Model->pst_VarDes)
		{
			AI_ResolveVarRef
			(
				_pst_Model,
				_pst_Model->pst_VarDes->pc_BufferInit,
				_pst_Model->pst_VarDes->pst_EditorVarsInfos,
				_pst_Model->pst_VarDes->pst_VarsInfos,
				_pst_Model->pst_VarDes->ul_NbVars,
				TRUE
			);
		}

		/* Mark model */
		_pst_Model->c_SecondPassDone = 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_ResolveInstanceRef(AI_tdst_Instance *_pst_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Model	*pst_Model;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Instance) return;

#ifdef ACTIVE_EDITORS
	AI_gp_ResolveGO = (OBJ_tdst_GameObject*)_pst_Instance->pst_GameObject;
#endif

	pst_Model = _pst_Instance->pst_Model;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Model
	 -------------------------------------------------------------------------------------------------------------------
	 */

	AI_ResolveModelRef(pst_Model);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Determin what we took from instance, and what we took from model
	 -------------------------------------------------------------------------------------------------------------------
	 */

	AI_InstanceAfterLoading(_pst_Instance);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_CallbackLoadProcList(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SCR_tt_ProcedureList	*pt_ProcList;
	char					*p, *pc_Buf1;
	ULONG					ul_Size;
	SCR_tt_Procedure		*pt_Proc;
	int						i, j, k, l, len;
	int						u32_Num;
	ULONG					ul_Index;
	AI_tdst_Node			*ptn;
	ULONG					ul_Mem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Count used */
	ul_Index = LOA_ul_SearchAddress(_ul_PosFile);
	if(ul_Index != (BIG_INDEX) - 1)
	{
		((SCR_tt_ProcedureList *) ul_Index)->uw_NbRefs++;
		return ul_Index;
	}
	pt_ProcList = (SCR_tt_ProcedureList *) MEM_p_Alloc(sizeof(SCR_tt_ProcedureList));
	L_memset(pt_ProcList, 0, sizeof(SCR_tt_ProcedureList));
	pt_ProcList->uw_NbRefs = 1;

	p = pc_Buf1 = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);
	if(!ul_Size)
	{
		return(ULONG) pt_ProcList;
	}

	pt_ProcList->u16_Num = LOA_ReadShort(&p);
	pt_ProcList->pt_All = NULL;
	if(pt_ProcList->u16_Num)
	{
		pt_ProcList->pt_All = (SCR_tt_Procedure *) MEM_p_Alloc(pt_ProcList->u16_Num * sizeof(SCR_tt_Procedure));
		pt_Proc = pt_ProcList->pt_All;
		for(i = 0; i < pt_ProcList->u16_Num; i++, pt_Proc++)
		{
#ifdef AI_FULL_RASTERS
			pt_Proc->aaa = 0;
#endif
			/* Le nom */
			len = LOA_ReadLong(&p);
			pt_Proc->pz_Name = NULL;
			if(len)
			{
				pt_Proc->pz_Name = (char *) MEM_p_VMAlloc(len + 1);
				for(j = 0; j < len; j++) pt_Proc->pz_Name[j] = LOA_ReadChar(&p);
				pt_Proc->pz_Name[len] = 0;
			}

			/* Pile */
			u32_Num = LOA_ReadLong(&p);
			if(u32_Num == 0xAACCBBDD)
			{
				pt_Proc->u16_SizeLocal = LOA_ReadShort(&p);
				u32_Num = LOA_ReadLong(&p);
			}

			/* Noeuds */
			pt_Proc->pu32_Nodes = NULL;
			if(u32_Num)
			{
#ifdef ACTIVE_EDITORS
				if( LOA_IsBinarizing() ) 
				{
					ptn = pt_Proc->pu32_Nodes = (AI_tdst_Node *) MEM_p_VMAlloc(u32_Num * sizeof(AI_tdst_Node));
					k = u32_Num;
					while(k--) LOA_ReadAINode_Ed(&p, ptn++);
				}
                else 
#endif                
                if( ! LOA_IsBinaryData() )
				{
					ptn = pt_Proc->pu32_Nodes = (AI_tdst_Node *) MEM_p_VMAlloc(u32_Num * sizeof(AI_tdst_Node));
					k = u32_Num;
					while(k--) LOA_ReadAINode(&p, ptn++);
				}
			}

			/* Info parent */
			pt_Proc->pt_ProcList = pt_ProcList;

			/* Noeuds debug */
#ifdef ACTIVE_EDITORS
			pt_Proc->pu32_DbgNodes = NULL;
			if(u32_Num) ptn = pt_Proc->pu32_DbgNodes = (AI_tdst_Node *) MEM_p_VMAlloc(u32_Num * sizeof(AI_tdst_Node));
#endif
			k = u32_Num;
			while(k--) LOA_ReadAINode_Ed(&p, ptn++);

			/* Information about local vars */
#ifdef ACTIVE_EDITORS
			pt_Proc->pst_LocalVars = NULL;
			pt_Proc->ul_NbLocals = 0;
#endif
			ul_Mem = LOA_ReadLong_Ed(&p, NULL);
			if(ul_Mem == 0x66666666)
			{
				ul_Mem = LOA_ReadLong_Ed(&p, NULL);
				LOA_ReadCharArray_Ed(&p, pt_Proc->asz_Comment, 1024);
			}

			if(ul_Mem)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~*/
				AI_tdst_Local	*p1 = NULL;
				/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
				pt_Proc->pst_LocalVars = p1 = (AI_tdst_Local*)MEM_p_VMAlloc(ul_Mem * sizeof(AI_tdst_Local));
				pt_Proc->ul_NbLocals = ul_Mem;
#endif
				for(l = 0; l < (int) ul_Mem; ++l)
				{
					LOA_ReadAILocal_Ed(&p, p1);
					if(p1 != NULL) p1++;
				}
			}
		}
	}

	/* Search if a corresponding C function exists */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int			i;
		BIG_KEY		ul_Key;
		extern int	AI2C_i_SizeofArray(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Key = LOA_ul_GetCurrentKey();
		for(i = 0; i < AI2C_i_SizeofArray(); i++)
		{
			if(ul_Key == AI2C_gat_fctdefs[i].ul_Key)
			{
#ifdef GAMECUBE_USE_AI2C_DLL
				pt_ProcList->pfn_CFunc = *(AI2C_gat_fctdefs[i].ppst_Func);
#else
				pt_ProcList->pfn_CFunc = AI2C_gat_fctdefs[i].pst_Func;
#endif
				break;
			}
		}
	}

	ul_Mem = LOA_ReadInt(&p);
	pt_ProcList->pc_Strings = NULL;
	if(ul_Mem == 0xC0DE6660)
	{
		ul_Mem = LOA_ReadInt(&p);
		if(ul_Mem)
		{
			pt_ProcList->pc_Strings = (char *) MEM_p_VMAlloc(ul_Mem);
			LOA_ReadString(&p, pt_ProcList->pc_Strings, ul_Mem);
		}
		ul_Mem = LOA_ReadInt(&p);
	}

	/* La clef du fichier source sert à la resolution des procedures */
	pt_ProcList->h_SourceFile = ul_Mem;

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~*/
		BIG_KEY		h;
		BIG_INDEX	f;
		/*~~~~~~~~~~*/

		h = LOA_ul_GetCurrentKey();
		f = BIG_ul_SearchKeyToFat(h);
		pt_ProcList->h_File = f;
	}
#endif
	return(ULONG) pt_ProcList;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_CallbackLoadFunction(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Function	*pst_Func;
	char				*pc_Buf, *pc_Buf1;
	ULONG				ul_Size;
	ULONG				ul_Mem;
	BIG_INDEX			ul_Index;
	ULONG				i;
	AI_tdst_Node		*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Count used */
	ul_Index = LOA_ul_SearchAddress(_ul_PosFile);
	if(ul_Index != (BIG_INDEX) - 1)
	{
		((AI_tdst_Function *) ul_Index)->uw_NbRefs++;
		return ul_Index;
	}

	MEMpro_StartMemRaster();
	pst_Func = (AI_tdst_Function *) MEM_p_Alloc(sizeof(AI_tdst_Function));
	LOA_ul_FileTypeSize[36] += sizeof(AI_tdst_Function);
	L_memset(pst_Func, 0, sizeof(AI_tdst_Function));
	pst_Func->uw_NbRefs = 1;

#ifdef BENCH_IA_TEST
	AIBnch_CreateFunctionBench(pst_Func, LOA_ul_GetCurrentKey());
#endif
	pc_Buf = pc_Buf1 = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);
	if(!ul_Size)
	{
		MEMpro_StopMemRaster(MEMpro_Id_AI_Functions);
		return(ULONG) pst_Func;
	}

	/* Size of local stack */
	pst_Func->i_SizeLocalStack = LOA_ReadInt(&pc_Buf);

	/* Search if a corresponding C function exists */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int			i;
		BIG_KEY		ul_Key;
		extern int	AI2C_i_SizeofArray(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Key = LOA_ul_GetCurrentKey();
		for(i = 0; i < AI2C_i_SizeofArray(); i++)
		{
			if(ul_Key == AI2C_gat_fctdefs[i].ul_Key)
			{
#ifdef GAMECUBE_USE_AI2C_DLL
				pst_Func->pfn_CFunc = *(AI2C_gat_fctdefs[i].ppst_Func);
#else
				pst_Func->pfn_CFunc = AI2C_gat_fctdefs[i].pst_Func;
#endif
				pst_Func->pz_name = AI2C_gat_fctdefs[i].pz_name;
				break;
			}
		}
	}

	/* Nodes */
	ul_Mem = LOA_ReadULong(&pc_Buf);
	pst_Func->pst_RootNode = NULL;
#ifdef ACTIVE_EDITORS
	if(ul_Mem)
#else
		if(ul_Mem && (!pst_Func->pfn_CFunc || !LOA_IsBinaryData()))
#endif
		{
			/*~~~~~~~~~~~~~~~*/
			AI_tdst_Node	*p;
			/*~~~~~~~~~~~~~~~*/
#ifndef ACTIVE_EDITORS
			if(!pst_Func->pfn_CFunc)
#endif
				pst_Func->pst_RootNode = (AI_tdst_Node *) MEM_p_VMAlloc(ul_Mem);

			for(i = 0, p = pst_Func->pst_RootNode; i < ul_Mem / sizeof(AI_tdst_Node); ++i)
			{
				if(pst_Func->pfn_CFunc)
					LOA_ReadAINode_Ed(&pc_Buf, p++);
				else
					LOA_ReadAINode(&pc_Buf, p++);
			}
		}

	MEMpro_StopMemRaster(MEMpro_Id_AI_Functions);

	/* Other root node (for debug) */
	ul_Mem = LOA_ReadLong_Ed(&pc_Buf, NULL);

#ifdef ACTIVE_EDITORS
	pst_Func->pst_OtherRootNode = NULL;
#endif
	if(ul_Mem)
	{
		p = NULL;
#ifdef ACTIVE_EDITORS
		pst_Func->pst_OtherRootNode = p = (AI_tdst_Node*)MEM_p_VMAlloc(ul_Mem);
#endif
		for(i = 0; i < ul_Mem / sizeof(AI_tdst_Node); ++i)
		{
			LOA_ReadAINode_Ed(&pc_Buf, p);
			if(p != NULL) p++;
		}
	}

	/* String buffer */
	MEMpro_StartMemRaster();
#ifdef JADEFUSION
	ul_Mem = LOA_ReadLong_Ed(&pc_Buf, NULL);
#else
	ul_Mem = LOA_ReadLong(&pc_Buf);
#endif
	pst_Func->pc_Strings = NULL;
	if(ul_Mem)
	{
		pst_Func->pc_Strings = (char *) MEM_p_VMAlloc(ul_Mem);
		LOA_ReadString(&pc_Buf, pst_Func->pc_Strings, ul_Mem);
	}

	MEMpro_StopMemRaster(MEMpro_Id_AI_Functions);

	/* Information about local vars */
	if(!LOA_gb_SpeedMode)
	{
		if(pc_Buf != pc_Buf1 + ul_Size)
		{
			ul_Mem = 0;
			ul_Mem = LOA_ReadLong_Ed(&pc_Buf, NULL);
#ifdef ACTIVE_EDITORS
			pst_Func->pst_LocalVars = NULL;
			pst_Func->ul_NbLocals = 0;
#endif
			if(ul_Mem)
			{
				/*~~~~~~~~~~~~~~~*/
				AI_tdst_Local	*p;
				/*~~~~~~~~~~~~~~~*/

				p = NULL;

#ifdef ACTIVE_EDITORS
				pst_Func->pst_LocalVars = p = (AI_tdst_Local*)MEM_p_VMAlloc(ul_Mem);
				pst_Func->ul_NbLocals = ul_Mem / sizeof(AI_tdst_Local);
#endif
				for(i = 0; i < ul_Mem / sizeof(AI_tdst_Local); ++i)
				{
					LOA_ReadAILocal_Ed(&pc_Buf, p);
					if(p != NULL) p++;
				}
			}
		}
	}

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~*/
		BIG_KEY		h;
		BIG_INDEX	f;
		/*~~~~~~~~~~*/

		h = LOA_ul_GetCurrentKey();
		f = BIG_ul_SearchKeyToFat(h);
		pst_Func->h_File = f;
		if(f != BIG_C_InvalidIndex) L_strcpy(pst_Func->az_Name, BIG_NameFile(f));
	}
#endif
	return(ULONG) pst_Func;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_CallbackLoadVars(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Buf, *pc_Init;
	ULONG					ul_Size, *pul_Buf;
	ULONG					ul_Mem;
	AI_tdst_VarDes			*pst_VarDes;
	ULONG					i;
	ULONG					ul_Mem1;
	AI_tdst_EditorVarInfo2	*pst_EditorVarsInfos2;
	char					*psz_String;
	ULONG					ul_Key;
	extern LOA_tdst_Ref		*LOA_gpst_CurRef;
	BAS_tdst_barray			st_Sort;
	int						i_Sort;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEMpro_StartMemRaster();
	if(LOA_gpst_CurRef->c_Type & LOA_C_TmpAlloc)
		pst_VarDes = (AI_tdst_VarDes *) MEM_p_Alloc(sizeof(AI_tdst_VarDes));
	else
		pst_VarDes = (AI_tdst_VarDes *) MEM_p_Alloc(sizeof(AI_tdst_VarDes));
	LOA_ul_FileTypeSize[35] += sizeof(AI_tdst_VarDes);
	L_memset(pst_VarDes, 0, sizeof(AI_tdst_VarDes));

	pul_Buf = (ULONG *) BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);
	if(!ul_Size) return(ULONG) pst_VarDes;
	pc_Init = pc_Buf = (char *) pul_Buf;

	/* Var des (engine) */
	ul_Mem = LOA_ReadLong(&pc_Buf);
	pst_VarDes->ul_NbVars = ul_Mem / 12;
	ul_Mem = pst_VarDes->ul_NbVars * sizeof(AI_tdst_VarInfo);
	pst_VarDes->pst_VarsInfos = NULL;
	if(ul_Mem)
	{
		/*~~~~~~~~~~~~~~~*/
		AI_tdst_VarInfo *p;
		/*~~~~~~~~~~~~~~~*/

		if(LOA_gpst_CurRef->c_Type & LOA_C_TmpAlloc)
			pst_VarDes->pst_VarsInfos = (AI_tdst_VarInfo *) MEM_p_VMAlloc(ul_Mem);
		else
			pst_VarDes->pst_VarsInfos = (AI_tdst_VarInfo *) MEM_p_VMAlloc(ul_Mem);
		LOA_ul_FileTypeSize[35] += ul_Mem;
		for(i = 0, p = pst_VarDes->pst_VarsInfos; i < pst_VarDes->ul_NbVars; ++i, ++p)
		{
			p->i_Offset = LOA_ReadInt(&pc_Buf);
			p->i_Offset2 = p->i_Offset;
			p->i_NumElem = LOA_ReadInt(&pc_Buf);
			p->w_Type = LOA_ReadShort(&pc_Buf);
			p->w_Flags = LOA_ReadShort(&pc_Buf);
		}
	}

	/*
	 * Var des (editor) Used almost only for editor.. still used to resolve car/dec
	 * after loading
	 */
	ul_Mem = LOA_ReadLong(&pc_Buf);
	pst_VarDes->pst_EditorVarsInfos = NULL;
	if(ul_Mem)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		AI_tdst_EditorVarInfo	*p;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		if(LOA_gpst_CurRef->c_Type & LOA_C_TmpAlloc)
			pst_VarDes->pst_EditorVarsInfos = (AI_tdst_EditorVarInfo *) MEM_p_VMAlloc(ul_Mem);
		else
			pst_VarDes->pst_EditorVarsInfos = (AI_tdst_EditorVarInfo *) MEM_p_VMAlloc(ul_Mem);
		LOA_ul_FileTypeSize[35] += ul_Mem;
		for(i = 0, p = pst_VarDes->pst_EditorVarsInfos; i < pst_VarDes->ul_NbVars; ++i, ++p)
		{
			LOA_ReadString(&pc_Buf, p->asz_Name, AI_C_MaxLenVar);
		}
	}

	/* Var des2 (used only in editor) */
	ul_Mem = 0;
	ul_Mem = LOA_ReadULong_Ed(&pc_Buf, NULL);

#ifdef ACTIVE_EDITORS
	pst_VarDes->ul_NbVarsInfos2 = ul_Mem / sizeof(AI_tdst_EditorVarInfo2);
#endif
	LOA_ReadLong_Ed(&pc_Buf, (LONG*)&ul_Mem1);
	pst_EditorVarsInfos2 = NULL;
	if(ul_Mem && !LOA_IsBinaryData())
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		AI_tdst_EditorVarInfo2	*p;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
		pst_EditorVarsInfos2 = (AI_tdst_EditorVarInfo2 *) MEM_p_VMAlloc(ul_Mem);
#else
		pst_EditorVarsInfos2 = (AI_tdst_EditorVarInfo2 *) MEM_p_VMAllocFromEnd(ul_Mem);
#endif
		for(i = 0, p = pst_EditorVarsInfos2; i < ul_Mem / sizeof(AI_tdst_EditorVarInfo2); ++i, ++p)
		{
			LOA_ReadInt_Ed(&pc_Buf, &p->i_Offset);

			/*
			 * The next two pointers are used to determine if strings should be loaded
			 * afterwards, The return value of LOA_ReadLong_Ed is used because the param
			 * pointer is not set in engine mode without binarized data (so that editor only
			 * fields can be used in the macro with ifdef's) but it is still needed to load
			 * the strings when not loading binary data.
			 */
			p->psz_StringCst = (CHAR *) LOA_ReadLong_Ed(&pc_Buf, NULL);
			p->psz_StringHelp = (CHAR *) LOA_ReadLong_Ed(&pc_Buf, NULL);
			LOA_ReadUShort_Ed(&pc_Buf, &p->uw_Flags);
			LOA_ReadUShort_Ed(&pc_Buf, &p->uw_p1);
			LOA_ReadLong_Ed(&pc_Buf, (LONG *) &p->p2);
		}

		for(i = 0, p = pst_EditorVarsInfos2; i < ul_Mem / sizeof(AI_tdst_EditorVarInfo2); ++i, ++p)
		{
			if(p->psz_StringCst)
			{
				ul_Mem1 = L_strlen(pc_Buf) + 1;
				psz_String = NULL;
#ifdef ACTIVE_EDITORS
				psz_String = (char *) MEM_p_VMAlloc(ul_Mem1);
#endif
				LOA_ReadString_Ed(&pc_Buf, psz_String, ul_Mem1);
#ifdef ACTIVE_EDITORS
				p->psz_StringCst = psz_String;
#endif
			}

			if(p->psz_StringHelp)
			{
				ul_Mem1 = L_strlen(pc_Buf) + 1;
				psz_String = NULL;
#ifdef ACTIVE_EDITORS
				psz_String = (char *) MEM_p_VMAlloc(ul_Mem1);
#endif
				LOA_ReadString_Ed(&pc_Buf, psz_String, ul_Mem1);
#ifdef ACTIVE_EDITORS
				p->psz_StringHelp = psz_String;
#endif
			}
		}
	}

#ifdef ACTIVE_EDITORS
	pst_VarDes->pst_EditorVarsInfos2 = pst_EditorVarsInfos2;
#else
	if(pst_EditorVarsInfos2) MEM_FreeFromEnd(pst_EditorVarsInfos2);
#endif

	/* Init values */
	ul_Mem = LOA_ReadLong(&pc_Buf);
	pst_VarDes->ul_SizeBufferInit = ul_Mem;
	pst_VarDes->pc_BufferInit = NULL;
	if(ul_Mem)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		char	*pc_BufInit;
		int		j, iNbElem;
		SHORT	w_TypeIndex, w_TypeSize;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		iNbElem = 0;
//		if(LOA_gpst_CurRef->c_Type & LOA_C_TmpAlloc)
//			pst_VarDes->pc_BufferInit = pc_BufInit = (char *) MEM_p_VMAlloc(ul_Mem);
//		else
			pst_VarDes->pc_BufferInit = pc_BufInit = (char *) MEM_p_VMAlloc(ul_Mem);
		LOA_ul_FileTypeSize[35] += ul_Mem;

		BAS_binit(&st_Sort, pst_VarDes->ul_NbVars);
		for(i = 0; i < pst_VarDes->ul_NbVars; ++i)
			BAS_binsert(pst_VarDes->pst_VarsInfos[i].i_Offset, i, &st_Sort);
		
		for(i_Sort = 0; i_Sort < st_Sort.num; i_Sort++)
		{
			i = st_Sort.base[i_Sort].ul_Val;
			
			if(pst_VarDes->pst_VarsInfos[i].i_NumElem != 1)
			{
				/*~~~~~~~~~~~~~*/
				/*
				 * Read the number of each elements of each dimension at the beginning of the
				 * array the number of dimensions is stored in bits 31&32
				 */
				UCHAR	uc_nbDim;
				/*~~~~~~~~~~~~~*/

				uc_nbDim = (UCHAR) ((ULONG) pst_VarDes->pst_VarsInfos[i].i_NumElem >> 30);
#ifdef JADEFUSION
				for(j = 0; j < uc_nbDim; ++j, ++((LONG *&) pc_BufInit))
#else
				for(j = 0; j < uc_nbDim; ++j, ++((LONG *) pc_BufInit))
#endif
				{
					*(LONG *) pc_BufInit = LOA_ReadLong(&pc_Buf);
				}
			}

			iNbElem = pst_VarDes->pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF;		/* bits 31&32 contain the dimensions of
																				 * the array */
			w_TypeIndex = AI_gaw_EnumLink[pst_VarDes->pst_VarsInfos[i].w_Type]; /* get the offset in the array by the
																				 * id of the type */
			w_TypeSize = AI_gast_Types[w_TypeIndex].w_Size;
			if(w_TypeSize == sizeof(LONG))
			{
				/* most types are saved as 32 bit values */
#ifdef JADEFUSION
				for(j = 0; j < iNbElem; ++j) _LOA_ReadLong(&pc_Buf, ((LONG *&) pc_BufInit)++, LOA_eBinEngineData);
#else
				for(j = 0; j < iNbElem; ++j) _LOA_ReadLong(&pc_Buf, ((LONG *) pc_BufInit)++, LOA_eBinEngineData);
#endif
			}
			else
			{
				/* other types are loaded with their specific structures. */
				switch(pst_VarDes->pst_VarsInfos[i].w_Type)
				{
				case TYPE_VECTOR:
#ifdef JADEFUSION
					for(j = 0; j < iNbElem; ++j) LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *&) pc_BufInit)++);
#else
					for(j = 0; j < iNbElem; ++j) LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *) pc_BufInit)++);
#endif
					break;

				case TYPE_MESSAGE:
					for(j = 0; j < iNbElem; ++j)
					{
						((AI_tdst_Message *) pc_BufInit)->pst_Sender = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->pst_GAO1 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->pst_GAO2 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->pst_GAO3 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->pst_GAO4 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->pst_GAO5 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((AI_tdst_Message *) pc_BufInit)->st_Vec1);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((AI_tdst_Message *) pc_BufInit)->st_Vec2);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((AI_tdst_Message *) pc_BufInit)->st_Vec3);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((AI_tdst_Message *) pc_BufInit)->st_Vec4);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((AI_tdst_Message *) pc_BufInit)->st_Vec5);
						((AI_tdst_Message *) pc_BufInit)->i_Int1 = LOA_ReadInt(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->i_Int2 = LOA_ReadInt(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->i_Int3 = LOA_ReadInt(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->i_Int4 = LOA_ReadInt(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->i_Int5 = LOA_ReadInt(&pc_Buf);
						((AI_tdst_Message *) pc_BufInit)->i_Id = LOA_ReadInt(&pc_Buf);
#ifdef JADEFUSION
						((AI_tdst_Message *&) pc_BufInit)++;
#else
						((AI_tdst_Message *) pc_BufInit)++;
#endif
					}
					break;

				case TYPE_TEXT:
					for(j = 0; j < iNbElem; ++j)
					{
						((TEXT_tdst_Eval *) pc_BufInit)->i_FileKey = LOA_ReadInt(&pc_Buf);
						((TEXT_tdst_Eval *) pc_BufInit)->i_Id = LOA_ReadInt(&pc_Buf);
#ifdef JADEFUSION
						((TEXT_tdst_Eval *&) pc_BufInit)++;
#else
						((TEXT_tdst_Eval *) pc_BufInit)++;
#endif
					}
					break;

				case TYPE_MESSAGEID:
					for(j = 0; j < iNbElem; ++j)
					{
						((AI_tdst_GlobalMessageId *) pc_BufInit)->msgid = LOA_ReadInt(&pc_Buf);
						((AI_tdst_GlobalMessageId *) pc_BufInit)->id = LOA_ReadInt(&pc_Buf);
#ifdef JADEFUSION
						((AI_tdst_GlobalMessageId *&) pc_BufInit)++;
#else
						((AI_tdst_GlobalMessageId *) pc_BufInit)++;
#endif
					}
					break;

				case TYPE_TRIGGER:
					for(j = 0; j < iNbElem; ++j)
					{
						((SCR_tt_Trigger *) pc_BufInit)->h_KeyFile = LOA_ReadInt(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->pt_Proc = (SCR_tt_Procedure *) LOA_ReadInt(&pc_Buf);
						LOA_ReadCharArray(&pc_Buf, ((SCR_tt_Trigger *) pc_BufInit)->az_Name, AI_MaxLenTrigger);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.pst_Sender = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.pst_GAO1 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.pst_GAO2 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.pst_GAO3 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.pst_GAO4 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.pst_GAO5 = (OBJ_tdst_GameObject *) LOA_ReadLong(&pc_Buf);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((SCR_tt_Trigger *) pc_BufInit)->t_Msg.st_Vec1);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((SCR_tt_Trigger *) pc_BufInit)->t_Msg.st_Vec2);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((SCR_tt_Trigger *) pc_BufInit)->t_Msg.st_Vec3);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((SCR_tt_Trigger *) pc_BufInit)->t_Msg.st_Vec4);
						LOA_ReadVector(&pc_Buf, (MATH_tdst_Vector *) &((SCR_tt_Trigger *) pc_BufInit)->t_Msg.st_Vec5);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.i_Int1 = LOA_ReadInt(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.i_Int2 = LOA_ReadInt(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.i_Int3 = LOA_ReadInt(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.i_Int4 = LOA_ReadInt(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.i_Int5 = LOA_ReadInt(&pc_Buf);
						((SCR_tt_Trigger *) pc_BufInit)->t_Msg.i_Id = LOA_ReadInt(&pc_Buf);
#ifdef JADEFUSION
						((SCR_tt_Trigger *&) pc_BufInit)++;
#else
						((SCR_tt_Trigger *) pc_BufInit)++;
#endif
					}
					break;

				default:
					ERR_X_ErrorAssert(0, "AI_ul_CallbackLoadVars: Unsupported type", "");
				}
			}
		}
		BAS_bfree(&st_Sort);
	}

	/* Initial functions for each tracks */
	for(i = 0; i < AI_C_MaxTracks; i++)
	{
		ul_Key = LOA_ReadLong(&pc_Buf);
		if(ul_Key)
		{
			LOA_MakeFileRef
			(
				ul_Key,
				(ULONG *) &pst_VarDes->apst_InitFct[i],
				AI_ul_CallbackLoadFunction,
				LOA_C_MustExists
			);
		}
	}

	MEMpro_StopMemRaster(MEMpro_Id_AI_Vars);
	return(ULONG) pst_VarDes;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_CallbackLoadModel(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Model	*pst_Model;
	ULONG			ul_Size, i;
	ULONG			*pul_Buf;
	CHAR			*pc_Buf;
	char			asz_Ext[BIG_C_MaxLenName];
	ULONG			ul_NumFct, ul_NumProc;
	BIG_KEY			ul_Key;
	ULONG			ul_FileExt;
#ifdef ACTIVE_EDITORS
	BIG_KEY			ul_File;
	char			asz_Path[BIG_C_MaxLenPath];
	char			*psz_Temp;
#endif
	int				i_NumFct, i_NumProc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(EDI_gb_ComputeMap)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		extern void AI2C_RegisterCurrentModel(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		AI2C_RegisterCurrentModel();
	}
#endif
/**/
	MEMpro_StartMemRaster();
	pst_Model = (AI_tdst_Model *) MEM_p_Alloc(sizeof(AI_tdst_Model));
	LOA_ul_FileTypeSize[36] += sizeof(AI_tdst_Model);
	L_memset(pst_Model, 0, sizeof(AI_tdst_Model));
#ifdef ACTIVE_EDITORS
	pst_Model->c_DisplayFilter = 1;
#endif

	/* Register the model */
#ifdef ACTIVE_EDITORS
	ul_File = LOA_ul_GetCurrentKey();
	ul_File = BIG_ul_SearchKeyToFat(ul_File);
	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
	psz_Temp = L_strrchr(asz_Path, '/');
	if(psz_Temp) *psz_Temp = 0;
	LINK_RegisterPointer(pst_Model, LINK_C_AI_Model, BIG_NameFile(ul_File), asz_Path);
#endif

	/* Load the group file */
	pc_Buf = BIG_pc_ReadFileTmp(_ul_Pos, &ul_Size);
	pul_Buf = (ULONG *) pc_Buf;

	/* Count number of functions */
	ul_NumFct = 0;
	ul_NumProc = 0;
	for(i = 0; i < (ul_Size >> 3); ++i)
	{
		_LOA_ReadULong((CHAR **) &pul_Buf, NULL, LOA_eBinLookAheadData);
		_LOA_ReadCharArray((CHAR **) &pul_Buf, (CHAR *) &ul_FileExt, 4, LOA_eBinLookAheadData);
		if(ul_FileExt == *((ULONG *) EDI_Csz_ExtAIEngineFct)) ul_NumFct++;
		if(ul_FileExt == *((ULONG *) EDI_Csz_ExtAIEngineFctLib)) ul_NumProc++;
	}

	pul_Buf = (ULONG *) pc_Buf;

	/* Allocate functions */
	if(ul_NumFct)
	{
		pst_Model->ppst_Functions = (AI_tdst_Function **) MEM_p_Alloc(ul_NumFct * sizeof(AI_tdst_Function *));
		LOA_ul_FileTypeSize[36] += ul_NumFct * sizeof(AI_tdst_Function *);
	}

	pst_Model->ul_NbFunctions = ul_NumFct;

	/* Proclist */
	if(ul_NumProc)
	{
		pst_Model->pp_ProcList = (SCR_tt_ProcedureList **) MEM_p_Alloc(ul_NumProc * sizeof(SCR_tt_ProcedureList *));
	}

	pst_Model->u32_NbProcList = ul_NumProc;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Scan it
	 -------------------------------------------------------------------------------------------------------------------
	 */

	i_NumFct = 0;
	i_NumProc = 0;
	for(i = 0; i < (ul_Size >> 3); ++i)
	{
		ul_Key = LOA_ReadLong(&pc_Buf);
		LOA_ReadCharArray(&pc_Buf, asz_Ext, 4);
		if(!ul_Key) continue;

		asz_Ext[4] = 0;

		/* AI variables */
		if(!L_strcmpi(asz_Ext, EDI_Csz_ExtAIEngineVars))
		{
			LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Model->pst_VarDes, AI_ul_CallbackLoadVars, LOA_C_MustExists);
		}

		/* AI functions */
		else if(!L_strcmpi(asz_Ext, EDI_Csz_ExtAIEngineFct))
		{
			LOA_MakeFileRef
			(
				ul_Key,
				(ULONG *) &pst_Model->ppst_Functions[i_NumFct++],
				AI_ul_CallbackLoadFunction,
				LOA_C_MustExists
			);
		}

		/* ProcList */
		else if(!L_strcmpi(asz_Ext, EDI_Csz_ExtAIEngineFctLib))
		{
			LOA_MakeFileRef
			(
				ul_Key,
				(ULONG *) &pst_Model->pp_ProcList[i_NumProc++],
				AI_ul_CallbackLoadProcList,
				LOA_C_MustExists
			);
		}
	}

	/* Search if a corresponding C function exists */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int			i;
		BIG_KEY		ul_Key;
		extern int	AI2C_i_SizeofArray(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Key = LOA_ul_GetCurrentKey();
		pst_Model->ul_key = ul_Key;
		for(i = 0; i < AI2C_i_SizeofArray(); i++)
		{
			if(ul_Key == AI2C_gat_fctdefs[i].ul_Key)
			{
#ifdef GAMECUBE_USE_AI2C_DLL
				pst_Model->pfn_CFunc = *(AI2C_gat_fctdefs[i].ppst_Func);
#ifdef DEBUG
				if (!pst_Model->pfn_CFunc)
					OSReport("Function %s is not linked\n",AI2C_gat_fctdefs[i].pz_name); 
				else
					OSReport("Linking %s\n",AI2C_gat_fctdefs[i].pz_name); 				
#endif // DEBUG				
#else
				pst_Model->pfn_CFunc = AI2C_gat_fctdefs[i].pst_Func;
#endif

				
				break;
			}
		}
	}

	MEMpro_StopMemRaster(MEMpro_Id_AI_Models);
	return(ULONG) pst_Model;
}

/*$4
 ***********************************************************************************************************************
    Callbacks
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_ul_CallbackLoadInstance(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	CHAR				*pc_Buf;
	BIG_KEY				ul_Key;
#ifdef ACTIVE_EDITORS
	BIG_KEY				ul_File;
	char				asz_Path[BIG_C_MaxLenPath];
#endif
	ULONG				ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pst_Instance = (AI_tdst_Instance *) MEM_p_Alloc(sizeof(AI_tdst_Instance));
	LOA_ul_FileTypeSize[36] += sizeof(AI_tdst_Instance);
	L_memset(pst_Instance, 0, sizeof(AI_tdst_Instance));

	/* Read the instance file */
	pc_Buf = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	ul_Key = LOA_ReadLong(&pc_Buf);

	/* Reference to the model */
	if(ul_Key && (ul_Key != BIG_C_InvalidIndex))
	{
		LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Instance->pst_Model, AI_ul_CallbackLoadModel, LOA_C_MustExists);
	}

	/* Register the instance */
#ifdef ACTIVE_EDITORS
	ul_File = LOA_ul_GetCurrentKey();
	ul_File = BIG_ul_SearchKeyToFat(ul_File);
	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
	LINK_RegisterPointer(pst_Instance, LINK_C_AI_Instance, BIG_NameFile(ul_File), asz_Path);
#endif

	/* Reference to the variable description */
	ul_Key = LOA_ReadLong(&pc_Buf);
	if(ul_Key && (ul_Key != BIG_C_InvalidIndex))
	{
#ifdef ACTIVE_EDITORS
		LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Instance->pst_VarDes, AI_ul_CallbackLoadVars, LOA_C_MustExists);
#else
		LOA_MakeFileRef
		(
			ul_Key,
			(ULONG *) &pst_Instance->pst_VarDes,
			AI_ul_CallbackLoadVars,
			LOA_C_MustExists | LOA_C_TmpAlloc
		);
#endif
	}

	return(ULONG) pst_Instance;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_AllocateSaveBuffer(AI_tdst_Instance *_pst_Universe)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_VarDes	*pst_VarDes;
	ULONG			i, ul_Size;
	ULONG			ul_SizeArr, i_NumElem;
	ULONG			ul_Act;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Anti-Bug */
	if(!_pst_Universe->pst_Model || !_pst_Universe->pst_Model->pst_VarDes) return;

	pst_VarDes = _pst_Universe->pst_Model->pst_VarDes;
	ul_Size = 0;
	for(i = 0; i < _pst_Universe->pst_Model->pst_VarDes->ul_NbVars; i++)
	{
		if(_pst_Universe->pst_Model->pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Save)
		{
			ul_Act = AI_gast_Types[_pst_Universe->pst_Model->pst_VarDes->pst_VarsInfos[i].w_Type].w_Size;
			i_NumElem = pst_VarDes->pst_VarsInfos[i].i_NumElem;
			ul_SizeArr = (ULONG) i_NumElem >> 30;
			ul_Act = (i_NumElem & 0x3FFFFFFF) * ul_Act;
			ul_Act += ul_SizeArr * 4;

			ul_Size += ul_Act;
		}
	}

    L_memset(INO_gst_SavManager.ac_CurrSlotBuffer, 0, INO_Cte_SavUniverseMaxSize);
    ERR_X_Warning((ul_Size <= INO_Cte_SavUniverseMaxSize), "", NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Instance *AI_pst_LoadInstance(BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Ret;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load instance */
	MEM_gb_EnableTmp = TRUE;
	LOA_MakeFileRef(_ul_Key, (ULONG *) &pst_Ret, AI_ul_CallbackLoadInstance, LOA_C_MustExists);
	LOA_Resolve();
	MEM_gb_EnableTmp = FALSE;

	if(!pst_Ret)
	{
		ERR_X_Warning(pst_Ret, AI_ERR_Csz_RefNotLoaded, "Universe AI doesn't exist");
		return NULL;
	}

	AI_ResolveInstanceRef(pst_Ret);

	/* Reinit instance (variables init, functions ...) */
	AI_ReinitInstance(pst_Ret);

	/* Special case for universe : Need to take care of save flags */
	if(_ul_Key == BIG_UniverseKey())
	{
		AI_AllocateSaveBuffer(pst_Ret);
		WOR_gst_Universe.pst_AI = pst_Ret;
		if(WOR_gst_Universe.pst_AI) OBJ_ResolveAIRefSecondPass(NULL);
	}

	return pst_Ret;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
