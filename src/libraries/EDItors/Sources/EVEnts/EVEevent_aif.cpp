 /*$T EVEevent_aif.cpp GC! 1.081 09/28/00 14:44:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "EVEevent.h"
#include "EVEtrack.h"
#include "EVEframe.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDIeditors_infos.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "EVEmsg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void RefreshAIFct(void *, void *_po_Item, void *, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	psel = (EVAV_cl_ViewItem *) _po_Item;
	((EEVE_cl_Event *) psel->mi_Param2)->SetAIFunctionName();
	((EEVE_cl_Track *) psel->mi_Param3)->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void RefreshPlay0(void *, void *_po_Item, void *, LONG)
{
	M_MF()->SendMessageToEditors(EEVE_MESSAGE_FORCEPLAY0, 0, 0);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::FillAIFunction(EVE_tdst_Event *_pst_Evt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Node		*pst_Node;
	char				asz_Name[200];
	char				asz_Name1[200];
	int					param;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Evt->p_Data) return;
	pst_Node = (AI_tdst_Node *) ((char *) _pst_Evt->p_Data + 4);
	param = 0;
	while(pst_Node->c_Type != CATEG_ENDTREE)
	{
		if
		(
			(AI_gast_Categs[pst_Node->c_Type].w_ID == CATEG_TYPE)
		&&	(AI_gast_Types[pst_Node->w_Param].w_ID == TYPE_GAMEOBJECT)
		&&	(AI_gast_Categs[(pst_Node + 1)->c_Type].w_ID == CATEG_KEYWORD)
		&&	(AI_gast_Keywords[(pst_Node + 1)->w_Param].w_ID == KEYWORD_ULTRA)
		)
		{
			po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
				(
					"Apply To",
					EVAV_EVVIT_GO,
					&pst_Node->l_Param,
					0,
					(ULONG) mpo_Track->mpo_Frame->mpst_GAO,
					(ULONG) this,
					(ULONG) mpo_Track
				);
			po_Item->mpfn_CB = RefreshAIFct;
			pst_Node++;
		}
		else if(AI_gast_Categs[pst_Node->c_Type].w_ID == CATEG_TYPE)
		{
			sprintf(asz_Name, "Param %d", param++);
			switch(AI_gast_Types[pst_Node->w_Param].w_ID)
			{
			case TYPE_GAMEOBJECT:
				po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
					(
						asz_Name,
						EVAV_EVVIT_GO,
						&pst_Node->l_Param,
						0,
						(ULONG) mpo_Track->mpo_Frame->mpst_GAO
					);
				break;
			case TYPE_FUNCTIONREF:
				po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name, EVAV_EVVIT_AIFunction, &pst_Node->l_Param);
				break;
			case TYPE_NETWORK:
				po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name, EVAV_EVVIT_NetWork, &pst_Node->l_Param);
				break;
			case TYPE_KEY:
				po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name, EVAV_EVVIT_Key, &pst_Node->l_Param);
				break;
			case TYPE_INT:
				po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name, EVAV_EVVIT_Int, &pst_Node->l_Param);
				break;
			case TYPE_COLOR:
				po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name, EVAV_EVVIT_Color, &pst_Node->l_Param);
				break;
			case TYPE_FLOAT:
				if
				(
					(AI_gast_Categs[pst_Node[3].c_Type].w_ID == CATEG_FUNCTION)
				&&	(AI_gast_Functions[pst_Node[3].w_Param].w_ID == FUNCTION_CAST_VECTOR)
				)
				{
					sprintf(asz_Name1, "%s (Vector X)", asz_Name);
					po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name1, EVAV_EVVIT_Float, &pst_Node->l_Param);
					pst_Node++;
					sprintf(asz_Name1, "%s (Vector Y)", asz_Name);
					po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name1, EVAV_EVVIT_Float, &pst_Node->l_Param);
					pst_Node++;
					sprintf(asz_Name1, "%s (Vector Z)", asz_Name);
					po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name1, EVAV_EVVIT_Float, &pst_Node->l_Param);
				}
				else
				{
					po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem(asz_Name, EVAV_EVVIT_Float, &pst_Node->l_Param);
				}
				break;
			}

			po_Item->mpfn_CB = RefreshPlay0;
		}

		pst_Node++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::ComputeAIFunction(EVE_tdst_Event *_pst_Evt, int _i_Fct)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Node	*pst_Node;
	int				i, j, i_Size;
	int				i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Free if there's already something */
	if(_pst_Evt->p_Data) MEM_Free(_pst_Evt->p_Data);

	/* Allocate nodes */
	i_Size = sizeof(AI_tdst_Node) * (EAI_gast_Functions[_i_Fct].i_NumPars + 4);
	i_Size += 4;
	_pst_Evt->p_Data = MEM_p_Alloc(i_Size);
	L_memset(_pst_Evt->p_Data, 0, i_Size);
	*(short *) _pst_Evt->p_Data = (short) (i_Size);

	/* Ultra ? */
	pst_Node = (AI_tdst_Node *) _pst_Evt->p_Data;
	pst_Node = (AI_tdst_Node *) (((char *) pst_Node) + 4);
	pst_Node->c_Type = (char) AI_gaw_EnumLink[CATEG_TYPE];
	pst_Node->w_Param = AI_gaw_EnumLink[TYPE_GAMEOBJECT];
	pst_Node->l_Param = 0;
	pst_Node++;
	pst_Node->c_Type = (char) AI_gaw_EnumLink[CATEG_KEYWORD];
	pst_Node->w_Param = AI_gaw_EnumLink[KEYWORD_ULTRA];
	i_Num = 2;

	/* Set parameters */
	for(i = 0; i < EAI_gast_Functions[_i_Fct].i_NumPars; i++)
	{
		if(EAI_gast_Functions[_i_Fct].ai_TypePars[i] == TYPE_VECTOR)
		{
			i_Size += 3 * sizeof(AI_tdst_Node);
			_pst_Evt->p_Data = MEM_p_Realloc(_pst_Evt->p_Data, i_Size);

			/* T'avais oublie la ligne suivante, PD de CB ! */
			*(short *) _pst_Evt->p_Data = (short) (i_Size);

			pst_Node = ((AI_tdst_Node *) _pst_Evt->p_Data) + i_Num;
			pst_Node = (AI_tdst_Node *) (((char *) pst_Node) + 4);
			for(j = 0; j < 3; j++)
			{
				pst_Node->c_Type = (char) AI_gaw_EnumLink[CATEG_TYPE];
				pst_Node->w_Param = ENUTYPE_FLOAT;
				pst_Node->l_Param = 0;
				pst_Node++;
				i_Num++;
			}

			pst_Node->c_Type = (char) AI_gaw_EnumLink[CATEG_FUNCTION];
			pst_Node->w_Param = AI_gaw_EnumLink[FUNCTION_CAST_VECTOR];
			pst_Node->l_Param = 0;
			i_Num++;
		}
		else
		{
			/* Special process for special events... */
			if(EAI_gast_Functions[_i_Fct].i_EvtFlags & EVE_C_IntFirstParam)
			{
				if(i == 0)
				{
					pst_Node = ((AI_tdst_Node *) _pst_Evt->p_Data) + i_Num;
					pst_Node = (AI_tdst_Node *) (((char *) pst_Node) + 4);
					pst_Node->c_Type = (char) AI_gaw_EnumLink[CATEG_EVENTPARAM];
					pst_Node->w_Param = AI_gaw_EnumLink[EAI_gast_Functions[_i_Fct].ai_TypePars[i]];
					pst_Node->l_Param = 0;
					goto l_Next;
				}
			}

			/* Normal type */
			pst_Node = ((AI_tdst_Node *) _pst_Evt->p_Data) + i_Num;
			pst_Node = (AI_tdst_Node *) (((char *) pst_Node) + 4);
			pst_Node->c_Type = (char) AI_gaw_EnumLink[CATEG_TYPE];
			pst_Node->w_Param = AI_gaw_EnumLink[EAI_gast_Functions[_i_Fct].ai_TypePars[i]];
			pst_Node->l_Param = 0;
l_Next:
			i_Num++;
		}
	}

	/* Function */
	pst_Node = ((AI_tdst_Node *) _pst_Evt->p_Data) + i_Num;
	pst_Node = (AI_tdst_Node *) (((char *) pst_Node) + 4);
	pst_Node->c_Type = (char) AI_gaw_EnumLink[CATEG_FUNCTION];
	pst_Node->w_Param = AI_gaw_EnumLink[EAI_gast_Functions[_i_Fct].w_ID];
	pst_Node->l_Param = 0;
	pst_Node++;

	/* End mark */
	pst_Node->c_Type = (char) CATEG_ENDTREE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::SetAIFunctionName(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	AI_tdst_Node	*pst_Node;
	int				i;
	BIG_INDEX		ul_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	mi_IndexAI = 0;
	pst_Evt = pst_FindEngineEvent();
	if(!pst_Evt->p_Data)
		mo_Name = "";
	else
	{
		pst_Node = (AI_tdst_Node *) ((char *) pst_Evt->p_Data + 4);
		while(pst_Node->c_Type != CATEG_ENDTREE)
		{
			pst_Node++;
		}

		pst_Node--;
		for(i = 0; i < EIA_gi_NumFct; i++)
		{
			if(AI_gaw_EnumLink[EAI_gast_Functions[i].w_ID] == pst_Node->w_Param)
			{
				mi_IndexAI = i;
				mo_Name = EAI_gast_Functions[i].psz_Name;
				break;
			}
		}

		pst_Node = (AI_tdst_Node *) ((char *) pst_Evt->p_Data + 4);
		if
		(
			(pst_Node->l_Param != 0)
		&&	(AI_gast_Categs[(pst_Node + 1)->c_Type].w_ID == CATEG_KEYWORD)
		&&	(AI_gast_Keywords[(pst_Node + 1)->w_Param].w_ID == KEYWORD_ULTRA)
		)
		{
			ul_Pos = LOA_ul_SearchIndexWithAddress(pst_Node->l_Param);
			mo_Name += " (";
			if(ul_Pos == -1)
			{
				mo_Name += "INVALID REFERENCE";
			}
			else
			{
				mo_Name += BIG_NameFile(ul_Pos);
			}
			mo_Name += " )";
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::SetAIFunction(EVE_tdst_Event *_pst_Evt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialogCombo o_Dialog("Choose an AI function");
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < EIA_gi_NumFct; i++)
	{
		if(EAI_gast_Functions[i].i_EvtFlags & EVE_C_All)
		{
			o_Dialog.AddItem(EAI_gast_Functions[i].psz_Name, i);
		}
	}

	if(o_Dialog.DoModal() == IDOK)
	{
		mo_Name = o_Dialog.mo_Name;
		i = o_Dialog.mi_CurSelData;
		if(i != -1)
		{
			ComputeAIFunction(_pst_Evt, i);
			SetAIFunctionName();
			mpo_Track->Invalidate();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bool EEVE_cl_Event::b_TestAIFunctionType(short _s_Type) {
	if ((mi_IndexAI < 0) || (mi_IndexAI >= EIA_gi_NumFct)) return false;
	return EAI_gast_Functions[mi_IndexAI].w_ID == _s_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::SetSoundPlayerAIFunction(EVE_tdst_Event *_pst_Evt) 
{
	int i_AIFunIdx;
	for(i_AIFunIdx = 0; i_AIFunIdx < EIA_gi_NumFct; i_AIFunIdx++){
		if((EAI_gast_Functions[i_AIFunIdx].i_EvtFlags & EVE_C_All)
			&&
			EAI_gast_Functions[i_AIFunIdx].w_ID == FUNCTION_SND_SOUNDPLAYERFORANIMATORS){
				break;
		}
	}
	if (i_AIFunIdx >= EIA_gi_NumFct) {
		SetAIFunction(_pst_Evt);
	} else {
		ComputeAIFunction(_pst_Evt, i_AIFunIdx);
		SetAIFunctionName();
		// Ask user to specify a dialog file
		
		// Set param of the function
		mpo_Track->Invalidate();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
// Set parameter _i_ParamIdx (i_ParamIdx = 0, 1, ...) to _i_Value. Works only if all parameters from 0 to i_ParamIdx are
// not vectors and param _i_ParamIdx is an int.
void EEVE_cl_Event::SetIntParameter(int _i_ParamIdx, int _i_Value)
{
	AI_tdst_Node	*pst_Node;
	EVE_tdst_Event	*pst_Evt;

	pst_Evt = pst_FindEngineEvent();
	pst_Node = ((AI_tdst_Node *) pst_Evt->p_Data) + _i_ParamIdx + 2;
	pst_Node = (AI_tdst_Node *) (((char *) pst_Node) + 4);
	pst_Node->l_Param = _i_Value;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
// Get parameter i_ParamIdx (i_ParamIdx = 0, 1, ...). Works only if all parameters from 0 to i_ParamIdx are
// not vectors.
int EEVE_cl_Event::i_GetIntParameter(int i_ParamIdx)
{
	AI_tdst_Node	*pst_Node;
	EVE_tdst_Event	*pst_Evt;

	pst_Evt = pst_FindEngineEvent();
	pst_Node = ((AI_tdst_Node *) pst_Evt->p_Data) + i_ParamIdx + 2;
	pst_Node = (AI_tdst_Node *) (((char *) pst_Node) + 4);
	return pst_Node->l_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::DrawAIFunction(CDC *pdc, CRect *_po_Rect)
{
	pdc->ExtTextOut(_po_Rect->left + 4, _po_Rect->top + 2, ETO_CLIPPED, _po_Rect, mo_Name, NULL);
}

#endif /* ACTIVE_EDITORS */
