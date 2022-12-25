/*$T EVEnt_aifunc.c GC! 1.081 08/28/01 12:03:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "EVEstruct.h"
#include "EVEconst.h"
#include "EVEplay.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AIload.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAread.h"

#ifdef PSX2_TARGET

/* mamagouille */
#include "MainPsx2/Sources/PSX2debug.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_AIFunction_Save(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Num;
	AI_tdst_Node	*pst_Node;
	AI_tdst_Node	*pst_Copy;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Num = 0;
	if(!_pst_Event->p_Data)
	{
		SAV_Buffer(&ul_Num, 4);
		return;
	}

	/* Count nodes */
	ul_Num = 1;
	pst_Node = (AI_tdst_Node *) ((char *) _pst_Event->p_Data + 4);
	while(pst_Node->c_Type != CATEG_ENDTREE)
	{
		pst_Node++;
		ul_Num++;
	}

	SAV_Buffer(&ul_Num, 4);

	/* Save all nodes. Must "unref" all */
	pst_Copy = (AI_tdst_Node *) MEM_p_Alloc(ul_Num * sizeof(AI_tdst_Node));
	L_memcpy(pst_Copy, ((char *) _pst_Event->p_Data) + 4, ul_Num * sizeof(AI_tdst_Node));
	pst_Node = pst_Copy;
	while(pst_Node->c_Type != CATEG_ENDTREE)
	{
		pst_Node->c_Type = (char) AI_gast_Categs[pst_Node->c_Type].w_ID;	/* Unref categ */
		if(pst_Node->c_Type == CATEG_FUNCTION)
		{
			pst_Node->w_Param = AI_gast_Functions[pst_Node->w_Param].w_ID;
		}
		else if(pst_Node->c_Type == CATEG_KEYWORD)
		{
			pst_Node->w_Param = AI_gast_Keywords[pst_Node->w_Param].w_ID;
		}
		else
		{
			if(!AI_SpecialWParam(pst_Node->c_Type)) pst_Node->w_Param = AI_gast_Types[pst_Node->w_Param].w_ID;
			if(AI_b_IsARef(pst_Node->w_Param))
			{
				if(pst_Node->w_Param == TYPE_GAMEOBJECT)
				{
					if((pst_Node->l_Param == AI_C_MainActor0) || (pst_Node->l_Param == AI_C_MainActor1)) goto l_Zap;
				}

				if(pst_Node->l_Param) pst_Node->l_Param = LOA_ul_SearchKeyWithAddress(pst_Node->l_Param);
			}

l_Zap: ;
		}

		pst_Node++;
	}

	/* Save nodes */
	SAV_Buffer(pst_Copy, ul_Num * sizeof(AI_tdst_Node));
	MEM_Free(pst_Copy);
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVE_Event_AIFunction_Load(EVE_tdst_Event *_pst_Event, char *_pc_Buffer, char *_pc_DestBuffer)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Num;
	char * pc_Buf = _pc_Buffer;
	/*~~~~~~~~~~~*/

	// Number of nodes	
	ul_Num = LOA_ReadLong(&pc_Buf);	

	if(ul_Num)
	{
		ULONG ulIndex;
		AI_tdst_Node * pst_Data;
#ifdef ACTIVE_EDITORS
		_pst_Event->p_Data = MEM_p_Alloc(4 + (ul_Num * sizeof(AI_tdst_Node)));
#else
		_pst_Event->p_Data = _pc_DestBuffer;
#endif
		pst_Data = (AI_tdst_Node *) (((char *) _pst_Event->p_Data) + 4);
		for(ulIndex = 0; ulIndex < ul_Num; ++ulIndex, ++pst_Data)
		{
			LOA_ReadAINode(&pc_Buf, pst_Data);
		}

		// Store the size at the beginning of the data buffer
		*(short *) _pst_Event->p_Data = (short) (pc_Buf - _pc_Buffer);
	}

	return (pc_Buf - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS
extern int					AI_gi_TestContent;
extern OBJ_tdst_GameObject *AI_gp_ResolveGO;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_AIFunction_Resolve(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Node	*pst_Node;
#ifdef ACTIVE_EDITORS
	char			az[1024];
	BIG_KEY			ul_Key;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Event->p_Data) return;
	pst_Node = (AI_tdst_Node *) ((char *) _pst_Event->p_Data + 4);
	if(!pst_Node) return;
	while(pst_Node->c_Type != CATEG_ENDTREE)
	{
		if(pst_Node->c_Type == CATEG_TYPE)
		{
#ifdef ACTIVE_EDITORS
			ul_Key = pst_Node->l_Param;
			if(AI_gi_TestContent && (int)ul_Key == AI_gi_TestContent)
			{
				char az[512];
				sprintf(az, "%s (EVENT)", AI_gp_ResolveGO->sz_Name);
				ERR_X_Warning(0, az, NULL);
			}
#endif
			pst_Node->l_Param = AI_ul_ResolveOneRef(pst_Node->w_Param, pst_Node->l_Param);
			if(AI_b_IsARef(pst_Node->w_Param))
			{
#ifdef ACTIVE_EDITORS
				if(pst_Node->l_Param == (LONG) BIG_C_InvalidIndex)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					BIG_INDEX					ul;
					extern EVE_tdst_ListTracks	*EVE_gpst_CurrentReolve;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					ul = LOA_ul_SearchIndexWithAddress((ULONG) EVE_gpst_CurrentReolve);
					sprintf(az, "Events : Ref not loaded (%x) in %s", ul_Key, BIG_NameFile(ul));
					ERR_X_Warning(0, az, NULL);
				}

#endif
			}
		}

		if(!AI_SpecialWParam(pst_Node->c_Type)) pst_Node->w_Param = AI_gaw_EnumLink[pst_Node->w_Param];
		pst_Node->c_Type = (char) AI_gaw_EnumLink[pst_Node->c_Type];
		pst_Node++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Event *EVE_Event_AIFunction_Play(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Function	*pst_CurrentFunction;
	AI_tdst_Instance	*pst_CurrentInstance;
	OBJ_tdst_GameObject *pst_GAO;
	extern void			AI_CheckWatchBegin(void);
	extern int			AI_CheckWatchEnd(void);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Event->p_Data)
	{
		/* Reinit stack */
		AI_gul_SizeGlobalStack = AI_gul_SizeGlobalVarStack = 0;

		/* Exec function */
		pst_CurrentFunction = AI_gpst_CurrentFunction;
		pst_CurrentInstance = AI_gpst_CurrentInstance;
		pst_GAO = AI_gpst_CurrentGameObject;
		AI_gpst_CurrentFunction = NULL;
		AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra = NULL;
		if(EVE_gpst_CurrentTrack->uw_Gizmo == (unsigned short) - 1)
		{
			if(EVE_gpst_CurrentTrack->pst_GO)
				AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = EVE_gpst_CurrentTrack->pst_GO;
			else
				AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = EVE_gpst_OwnerGAO;
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			ANI_st_GameObjectAnim	*pst_GOAnim;
			TAB_tdst_PFelem			*pst_CurrentBone;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if
			(
				EVE_gpst_CurrentTrack->pst_GO
			&&  OBJ_b_TestIdentityFlag(EVE_gpst_CurrentTrack->pst_GO, OBJ_C_IdentityFlag_Anims)
			&&	EVE_gpst_CurrentTrack->pst_GO->pst_Base
			&&	EVE_gpst_CurrentTrack->pst_GO->pst_Base->pst_GameObjectAnim
			&&	EVE_gpst_CurrentTrack->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
			)
			{
				pst_GOAnim = EVE_gpst_CurrentTrack->pst_GO->pst_Base->pst_GameObjectAnim;
				if(EVE_gpst_CurrentTrack->uw_Gizmo >= pst_GOAnim->pst_Skeleton->pst_AllObjects->ul_NbElems)
					return _pst_Event;
				pst_CurrentBone = pst_GOAnim->pst_Skeleton->pst_AllObjects->p_Table + EVE_gpst_CurrentTrack->uw_Gizmo;
				AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			}
			else
				return _pst_Event;
		}

#ifdef ACTIVE_EDITORS
		AI_CheckWatchBegin();
#endif
		AI_RunNode((AI_tdst_Node *) (((char *) _pst_Event->p_Data) + 4));

#ifdef ACTIVE_EDITORS
		if(AI_CheckWatchEnd())
		{
			/*~~~~~~~~~~~~~*/
			char	az[1024];
			/*~~~~~~~~~~~~~*/

			sprintf
			(
				az,
				"EVENTS: GAO %s has modified a watch (dest GAO is %s)",
				EVE_gpst_OwnerGAO->sz_Name,
				AI_gpst_CurrentGameObject->sz_Name
			);

			MessageBox(0, az, "Events", MB_OK);
		}

#endif
		/* Is there anything in the stack ? */
		if(AI_gul_SizeGlobalStack)
		{
			EVE_gpst_CurrentParam->i_Param1 = *(int *) AI_gac_GlobalStack;
		}

		AI_gpst_CurrentFunction = pst_CurrentFunction;
		AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra = pst_CurrentInstance;
		AI_gpst_CurrentGameObject = AI_gpst_CurrentUltra = pst_GAO;
	}

	return _pst_Event;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
