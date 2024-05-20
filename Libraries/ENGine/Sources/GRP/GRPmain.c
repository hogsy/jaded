/*$T GRPmain.c GC! 1.081 02/10/03 14:39:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Main functions of the GRP module */
#include "Precomp.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WAYS/WAYinit.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ENGinit.h"
#include "AIinterp/Sources/AIengine.h"
#ifdef JADEFUSION
#include "Light/LIGHTrejection.h"
#endif
#ifdef ACTIVE_EDITORS
#include "AIinterp/Sources/AIload.h"
#include "GraphicDK/Sources/SELection/SELection.h"
#include "BIGfiles/BIGread.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "ENGine/Sources/GRP/GRPsave.h"
#include "ENGine/Sources/GRP/GRPload.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_Group *GRP_pst_CreateNewGroup(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group	*pst_Group;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Create group in memory */
	pst_Group = (OBJ_tdst_Group *) MEM_p_Alloc(sizeof(OBJ_tdst_Group));
	L_memset(pst_Group, 0, sizeof(OBJ_tdst_Group));

	pst_Group->pst_AllObjects = (TAB_tdst_PFtable *) MEM_p_Alloc(sizeof(TAB_tdst_PFtable));
	TAB_PFtable_Init(pst_Group->pst_AllObjects, 0, 0.1f);

	return pst_Group;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *GRP_pst_CorresGAO
(
	OBJ_tdst_Group		*_pst_SrcGrp,
	OBJ_tdst_Group		*_pst_DestGrp,
	OBJ_tdst_GameObject *_pst_Father
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_FirstSrc, *pst_FirstDest;
	TAB_tdst_PFelem		*pst_LastSrc;
	OBJ_tdst_GameObject *pst_FatherSrc, *pst_FatherDest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((ULONG) _pst_Father == AI_C_MainActor0) return (OBJ_tdst_GameObject *) AI_C_MainActor0;
	if((ULONG) _pst_Father == AI_C_MainActor1) return (OBJ_tdst_GameObject *) AI_C_MainActor1;
	pst_FirstSrc = TAB_pst_PFtable_GetFirstElem(_pst_SrcGrp->pst_AllObjects);
	pst_FirstDest = TAB_pst_PFtable_GetFirstElem(_pst_DestGrp->pst_AllObjects);
	pst_LastSrc = TAB_pst_PFtable_GetLastElem(_pst_SrcGrp->pst_AllObjects);
	for(; pst_FirstSrc <= pst_LastSrc; pst_FirstSrc++, pst_FirstDest++)
	{
		pst_FatherSrc = (OBJ_tdst_GameObject *) pst_FirstSrc->p_Pointer;
		pst_FatherDest = (OBJ_tdst_GameObject *) pst_FirstDest->p_Pointer;
		if(_pst_Father != pst_FatherSrc) continue;
		return pst_FatherDest;
	}

	return NULL;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_RepercuteAIRefs(OBJ_tdst_Group *_pst_SrcGrp, OBJ_tdst_Group *_pst_DestGrp)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentSrc;
	TAB_tdst_PFelem		*pst_EndSrc;
	OBJ_tdst_GameObject *pst_SrcGO;
	AI_tdst_Instance	*pst_Instance;
	AI_tdst_Model		*pst_Model;
	UINT				i;
	int					j;
	char				*pc_BufferInit;
	ULONG				*pul_Val;
	AI_tdst_VarInfo		*pst_VarsInfos;
	SHORT				wType;
	ULONG				ul_Ref, ulNumDim;
	SCR_tt_Trigger		*pt_Trigger;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentSrc = TAB_pst_PFtable_GetFirstElem(_pst_DestGrp->pst_AllObjects);
	pst_EndSrc = TAB_pst_PFtable_GetLastElem(_pst_DestGrp->pst_AllObjects);
	for(; pst_CurrentSrc <= pst_EndSrc; pst_CurrentSrc++)
	{
		pst_SrcGO = (OBJ_tdst_GameObject *) pst_CurrentSrc->p_Pointer;
		if(TAB_b_IsAHole(pst_SrcGO)) continue;
		if(OBJ_b_TestIdentityFlag(pst_SrcGO, OBJ_C_IdentityFlag_AI))
		{
			if(!pst_SrcGO->pst_Extended) continue;
			if(!pst_SrcGO->pst_Extended->pst_Ai) continue;
			pst_Instance = (AI_tdst_Instance *) pst_SrcGO->pst_Extended->pst_Ai;
			pst_Model = pst_Instance->pst_Model;
			if(!pst_Model->pst_VarDes) continue;

			pc_BufferInit = pst_Instance->pc_VarsBufferInit;
			pst_VarsInfos = pst_Model->pst_VarDes->pst_VarsInfos;
			for(i = 0; i < pst_Model->pst_VarDes->ul_NbVars; i++)
			{
				pul_Val = (ULONG *) &pc_BufferInit[pst_VarsInfos[i].i_Offset];
				wType = pst_VarsInfos[i].w_Type;
				wType = AI_gast_Types[wType].w_ID;

				if(wType == TYPE_GAMEOBJECT)
				{
					ulNumDim = (ULONG) pst_VarsInfos[i].i_NumElem >> 30;
					pul_Val += ulNumDim;

					for(j = 0; j < (pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF); j++)
					{
						ul_Ref = *(pul_Val + j);
						if(ul_Ref)
						{
							*(pul_Val + j) = (ULONG) GRP_pst_CorresGAO
								(
									_pst_SrcGrp,
									_pst_DestGrp,
									(OBJ_tdst_GameObject *) ul_Ref
								);
							if(!(*(pul_Val + j))) *(pul_Val + j) = ul_Ref;
						}
					}
				}
				else if(wType == TYPE_TRIGGER)
				{
					ulNumDim = (ULONG) pst_VarsInfos[i].i_NumElem >> 30;
					pul_Val += ulNumDim;

					for(j = 0; j < (pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF); j++)
					{
						pt_Trigger = ((SCR_tt_Trigger *) pul_Val) + j;

						ul_Ref = (ULONG) pt_Trigger->t_Msg.pst_GAO1;
						if(ul_Ref)
						{
							pt_Trigger->t_Msg.pst_GAO1 = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, (OBJ_tdst_GameObject *) ul_Ref);
							if(!pt_Trigger->t_Msg.pst_GAO1) pt_Trigger->t_Msg.pst_GAO1 = (OBJ_tdst_GameObject *) ul_Ref;
						}

						ul_Ref = (ULONG) pt_Trigger->t_Msg.pst_GAO2;
						if(ul_Ref)
						{
							pt_Trigger->t_Msg.pst_GAO2 = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, (OBJ_tdst_GameObject *) ul_Ref);
							if(!pt_Trigger->t_Msg.pst_GAO2) pt_Trigger->t_Msg.pst_GAO2 = (OBJ_tdst_GameObject *) ul_Ref;
						}

						ul_Ref = (ULONG) pt_Trigger->t_Msg.pst_GAO3;
						if(ul_Ref)
						{
							pt_Trigger->t_Msg.pst_GAO3 = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, (OBJ_tdst_GameObject *) ul_Ref);
							if(!pt_Trigger->t_Msg.pst_GAO3) pt_Trigger->t_Msg.pst_GAO3 = (OBJ_tdst_GameObject *) ul_Ref;
						}

						ul_Ref = (ULONG) pt_Trigger->t_Msg.pst_GAO4;
						if(ul_Ref)
						{
							pt_Trigger->t_Msg.pst_GAO4 = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, (OBJ_tdst_GameObject *) ul_Ref);
							if(!pt_Trigger->t_Msg.pst_GAO4) pt_Trigger->t_Msg.pst_GAO4 = (OBJ_tdst_GameObject *) ul_Ref;
						}

						ul_Ref = (ULONG) pt_Trigger->t_Msg.pst_GAO5;
						if(ul_Ref)
						{
							pt_Trigger->t_Msg.pst_GAO5 = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, (OBJ_tdst_GameObject *) ul_Ref);
							if(!pt_Trigger->t_Msg.pst_GAO5) pt_Trigger->t_Msg.pst_GAO5 = (OBJ_tdst_GameObject *) ul_Ref;
						}
					}
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_RepercuteEventsRefs(OBJ_tdst_Group *_pst_SrcGrp, OBJ_tdst_Group *_pst_DestGrp)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentSrc;
	TAB_tdst_PFelem		*pst_EndSrc;
	OBJ_tdst_GameObject *pst_SrcGO;
	EVE_tdst_Data		*pst_Data;
	USHORT				i, j;
	EVE_tdst_Track		*pst_Track;
	EVE_tdst_Event		*pst_Event;
	AI_tdst_Node		*pst_Node;
	SHORT				wType;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentSrc = TAB_pst_PFtable_GetFirstElem(_pst_DestGrp->pst_AllObjects);
	pst_EndSrc = TAB_pst_PFtable_GetLastElem(_pst_DestGrp->pst_AllObjects);
	for(; pst_CurrentSrc <= pst_EndSrc; pst_CurrentSrc++)
	{
		pst_SrcGO = (OBJ_tdst_GameObject *) pst_CurrentSrc->p_Pointer;
		if(TAB_b_IsAHole(pst_SrcGO)) continue;
		if(OBJ_b_TestIdentityFlag(pst_SrcGO, OBJ_C_IdentityFlag_Events))
		{
			if(!pst_SrcGO->pst_Extended) continue;
			pst_Data = pst_SrcGO->pst_Extended->pst_Events;
			if(!pst_Data) continue;

			pst_Track = pst_Data->pst_ListTracks->pst_AllTracks;
			for(i = 0; i < pst_Data->pst_ListTracks->uw_NumTracks; i++)
			{
				if(pst_Track->pst_GO)
					pst_Track->pst_GO = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, pst_Track->pst_GO);

				pst_Event = pst_Track->pst_AllEvents;
				for(j = 0; j < pst_Track->uw_NumEvents; j++)
				{
					if((pst_Event->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_AIFunction)
					{
						pst_Node = (AI_tdst_Node *) ((char *) pst_Event->p_Data + 4);
						while(pst_Node->c_Type != CATEG_ENDTREE)
						{
							if(AI_gast_Categs[pst_Node->c_Type].w_ID == CATEG_TYPE)
							{
								wType = pst_Node->w_Param;
								wType = AI_gast_Types[wType].w_ID;
								if(wType == TYPE_GAMEOBJECT)
								{
									pst_Node->l_Param = (ULONG) GRP_pst_CorresGAO
										(
											_pst_SrcGrp,
											_pst_DestGrp,
											(OBJ_tdst_GameObject *) pst_Node->l_Param
										);
								}
							}

							pst_Node++;
						}
					}

					pst_Event++;
				}

				pst_Track++;
			}
		}
	}
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_ReinitAllGO(OBJ_tdst_Group *_pst_DestGrp)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentSrc;
	TAB_tdst_PFelem		*pst_EndSrc;
	OBJ_tdst_GameObject *pst_SrcGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentSrc = TAB_pst_PFtable_GetFirstElem(_pst_DestGrp->pst_AllObjects);
	pst_EndSrc = TAB_pst_PFtable_GetLastElem(_pst_DestGrp->pst_AllObjects);
	for(; pst_CurrentSrc <= pst_EndSrc; pst_CurrentSrc++)
	{
		pst_SrcGO = (OBJ_tdst_GameObject *) pst_CurrentSrc->p_Pointer;
		if(TAB_b_IsAHole(pst_SrcGO)) continue;

		ENG_ReinitOneObject(pst_SrcGO, UNI_Cuc_ForDuplicate);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_RepercuteHierarchy(OBJ_tdst_Group *_pst_SrcGrp, OBJ_tdst_Group *_pst_DestGrp)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentSrc, *pst_CurrentDest;
	TAB_tdst_PFelem		*pst_EndSrc;
	OBJ_tdst_GameObject *pst_SrcGO, *pst_DestGO, *pst_FatherDest;
	OBJ_tdst_GameObject *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentSrc = TAB_pst_PFtable_GetFirstElem(_pst_SrcGrp->pst_AllObjects);
	pst_CurrentDest = TAB_pst_PFtable_GetFirstElem(_pst_DestGrp->pst_AllObjects);
	pst_EndSrc = TAB_pst_PFtable_GetLastElem(_pst_SrcGrp->pst_AllObjects);
	for(; pst_CurrentSrc <= pst_EndSrc; pst_CurrentSrc++, pst_CurrentDest++)
	{
		pst_SrcGO = (OBJ_tdst_GameObject *) pst_CurrentSrc->p_Pointer;
		pst_DestGO = (OBJ_tdst_GameObject *) pst_CurrentDest->p_Pointer;

		/* Bugs may appear when hierarchy with holes. */
		if(TAB_b_IsAHole(pst_SrcGO)) continue;

		if(OBJ_b_TestIdentityFlag(pst_SrcGO, OBJ_C_IdentityFlag_Hierarchy))
		{
			pst_Father = pst_SrcGO->pst_Base->pst_Hierarchy->pst_FatherInit;
			if(pst_Father)
			{
				pst_FatherDest = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, pst_Father);
				pst_DestGO->pst_Base->pst_Hierarchy->pst_FatherInit = pst_FatherDest;
				pst_DestGO->pst_Base->pst_Hierarchy->pst_Father = pst_FatherDest;
			}
		}
	}
}

#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_RepercuteLinks(OBJ_tdst_Group *_pst_SrcGrp, OBJ_tdst_Group *_pst_DestGrp, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j, k;
	TAB_tdst_PFelem		*pst_CurrentSrc, *pst_CurrentDest;
	TAB_tdst_PFelem		*pst_EndSrc;
	OBJ_tdst_GameObject *pst_SrcGO, *pst_DestGO;
	WAY_tdst_Struct		*pst_SrcStruct, *pst_DestStruct;	
	BIG_INDEX			ul_Index;
	WAY_tdst_Network	*pst_Network;
	ULONG				ul_Keys[16][2];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(ul_Keys, 0, 16 * 2 * 4);

	pst_CurrentSrc = TAB_pst_PFtable_GetFirstElem(_pst_SrcGrp->pst_AllObjects);
	pst_CurrentDest = TAB_pst_PFtable_GetFirstElem(_pst_DestGrp->pst_AllObjects);
	pst_EndSrc = TAB_pst_PFtable_GetLastElem(_pst_SrcGrp->pst_AllObjects);
	for(; pst_CurrentSrc <= pst_EndSrc; pst_CurrentSrc++, pst_CurrentDest++)
	{
		pst_SrcGO = (OBJ_tdst_GameObject *) pst_CurrentSrc->p_Pointer;
		pst_DestGO = (OBJ_tdst_GameObject *) pst_CurrentDest->p_Pointer;

		if(TAB_b_IsAHole(pst_SrcGO)) continue;

		if
		(
			OBJ_b_TestIdentityFlag(pst_SrcGO, OBJ_C_IdentityFlag_Links)
		&&	pst_SrcGO->pst_Extended
		&&	pst_SrcGO->pst_Extended->pst_Links
		)
		{
			pst_SrcStruct = (WAY_tdst_Struct* )pst_SrcGO->pst_Extended->pst_Links;

			/* Alloc Link structure if needed */
			OBJ_GameObject_CreateExtendedIfNot(pst_DestGO);

			if(pst_DestGO->pst_Extended->pst_Links)
				WAY_FreeStruct((WAY_tdst_Struct* )pst_DestGO->pst_Extended->pst_Links);

			WAY_pst_CreateStruct(_pst_World, pst_DestGO);

			pst_DestStruct = (WAY_tdst_Struct* )pst_DestGO->pst_Extended->pst_Links;
			pst_DestStruct->ul_Num = pst_SrcStruct->ul_Num;

			if(pst_DestStruct->ul_Num)
			{
				pst_DestStruct->pst_AllLinks = (WAY_tdst_LinkList *)MEM_p_Alloc(pst_DestStruct->ul_Num * sizeof(WAY_tdst_LinkList));
				L_memset(pst_DestStruct->pst_AllLinks, 0, pst_DestStruct->ul_Num * sizeof(WAY_tdst_LinkList));

				for(i = 0; i < (int) pst_DestStruct->ul_Num; i++)
				{
					ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_SrcStruct->pst_AllLinks[i].pst_Network);

					for(k = 0; k < 16; k++)
					{
						if((ul_Keys[k][0] == ul_Index) || !ul_Keys[k][0])
							break;
					}

					if(k == 16)
						ERR_X_Assert(0);

					if(!ul_Keys[k][1])
					{
						ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_SrcStruct->pst_AllLinks[i].pst_Network);
						pst_Network = WAY_pst_CreateNetwork(_pst_World, BIG_NameFile(ul_Index), GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, pst_SrcStruct->pst_AllLinks[i].pst_Network->pst_Root));
						ul_Keys[k][0] = ul_Index;
						ul_Keys[k][1] = LOA_ul_SearchIndexWithAddress((ULONG) pst_Network);
					}
					else
						pst_Network = (WAY_tdst_Network *)LOA_ul_SearchAddress(BIG_PosFile(ul_Keys[k][1]));

					pst_DestStruct->pst_AllLinks[i].pst_Network = pst_Network;
					pst_DestStruct->pst_AllLinks[i].pst_Network->pst_Root = pst_Network->pst_Root;
					pst_DestStruct->pst_AllLinks[i].pst_Network->ul_Flags = pst_SrcStruct->pst_AllLinks[i].pst_Network->ul_Flags;

					pst_DestStruct->pst_AllLinks[i].ul_Num = pst_SrcStruct->pst_AllLinks[i].ul_Num;

					if(pst_DestStruct->pst_AllLinks[i].ul_Num)
						pst_DestStruct->pst_AllLinks[i].pst_Links = (WAY_tdst_Link *) MEM_p_Alloc(pst_DestStruct->pst_AllLinks[i].ul_Num * sizeof(WAY_tdst_Link));

					for(j = 0; j < (int) pst_DestStruct->pst_AllLinks[i].ul_Num; j++)
					{
						L_memcpy(&pst_DestStruct->pst_AllLinks[i].pst_Links[j], &pst_SrcStruct->pst_AllLinks[i].pst_Links[j], sizeof(WAY_tdst_Link));
						pst_DestStruct->pst_AllLinks[i].pst_Links[j].pst_Next = GRP_pst_CorresGAO(_pst_SrcGrp, _pst_DestGrp, pst_SrcStruct->pst_AllLinks[i].pst_Links[j].pst_Next);
					}
				}
			}
			else
				pst_DestStruct->pst_AllLinks = NULL;
		}
	}


	/* Delete the SRC addresses */
	pst_CurrentSrc = TAB_pst_PFtable_GetFirstElem(_pst_SrcGrp->pst_AllObjects);
	pst_EndSrc = TAB_pst_PFtable_GetLastElem(_pst_SrcGrp->pst_AllObjects);
	for(; pst_CurrentSrc <= pst_EndSrc; pst_CurrentSrc++)
	{
		pst_SrcGO = (OBJ_tdst_GameObject *) pst_CurrentSrc->p_Pointer;

		if(TAB_b_IsAHole(pst_SrcGO)) continue;

		if
		(
			OBJ_b_TestIdentityFlag(pst_SrcGO, OBJ_C_IdentityFlag_Links)
		&&	pst_SrcGO->pst_Extended
		&&	pst_SrcGO->pst_Extended->pst_Links
		)
		{
			pst_SrcStruct = (WAY_tdst_Struct* )pst_SrcGO->pst_Extended->pst_Links;

			for(i = 0; i < (int) pst_SrcStruct->ul_Num; i++)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_SrcStruct->pst_AllLinks[i].pst_Network);

				if(ul_Index != 0xFFFFFFFF)
				{
					/* Current SRC NetWork could be in the world ? */
					for(j = 0; j < (int) _pst_World->pst_AllNetworks->ul_Num; j++)
					{
						if(_pst_World->pst_AllNetworks->ppst_AllNetworks[j] == pst_SrcStruct->pst_AllLinks[i].pst_Network)
						{
							break;
						}
					}

					if(j == _pst_World->pst_AllNetworks->ul_Num)
						LOA_DeleteAddress(pst_SrcStruct->pst_AllLinks[i].pst_Network);
				}
			}
		}
	}

}

#endif
/*
 =======================================================================================================================
    Aim:    Duplicates all the GameObjects of the Source Group into the Destination Group.
 =======================================================================================================================
 */
#ifdef JADEFUSION
void GRP_DuplicateGroupObjects(WOR_tdst_World *_pst_World, OBJ_tdst_Group *_pst_SrcGrp, OBJ_tdst_Group *_pst_DestGrp, UINT LRLAction/*=eLRLNone*/ )
#else
void GRP_DuplicateGroupObjects(WOR_tdst_World *_pst_World, OBJ_tdst_Group *_pst_SrcGrp, OBJ_tdst_Group *_pst_DestGrp)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentSrc;
	TAB_tdst_PFelem		*pst_EndSrc;
	OBJ_tdst_GameObject *pst_SrcGO, *pst_DestGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_DestGrp->ul_Flags = _pst_SrcGrp->ul_Flags;

	/* Duplicate each object of the source group and add it to the destination one */
	pst_CurrentSrc = TAB_pst_PFtable_GetFirstElem(_pst_SrcGrp->pst_AllObjects);
	if(!pst_CurrentSrc) return;
	pst_EndSrc = TAB_pst_PFtable_GetLastElem(_pst_SrcGrp->pst_AllObjects);
	for(; pst_CurrentSrc <= pst_EndSrc; pst_CurrentSrc++)
	{
		pst_SrcGO = (OBJ_tdst_GameObject *) pst_CurrentSrc->p_Pointer;
		if(TAB_b_IsAHole(pst_SrcGO)) continue;

		/* Duplicates the object into the world */
		pst_DestGO = OBJ_GameObject_Duplicate(_pst_World, pst_SrcGO, TRUE, FALSE, NULL, 0, NULL );

		pst_DestGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Generated;
#ifdef JADEFUSION
		// Copy the LRL to the duplicate GAO when we are loading a WOW
		g_LightRejectionList.UpdateLRLWithGAODuplicate(pst_SrcGO, pst_DestGO, (eLRLAction)LRLAction);
#endif
		/* We add the newly created GameObject into the Group. */
		OBJ_AddInGroup(_pst_DestGrp, pst_DestGO);
	}

	/* If the Src Group has hierarchy, we have to deal with it in the Dest Group. */
	GRP_RepercuteHierarchy(_pst_SrcGrp, _pst_DestGrp);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Create and saves a group in a given world

    Note:   The name of the group must not include the extension
 =======================================================================================================================
 */
OBJ_tdst_Group *GRP_pst_CreateAndSaveNewGroup
(
	char	*_psz_Path,
	char	*_psz_Name,
	BOOL	_b_SameName,
	BOOL	_b_Over,
	BOOL	*pdel
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_Name[BIG_C_MaxLenName];
	char			asz_Name1[BIG_C_MaxLenName];
	BIG_INDEX		ul_Index, ul_Adr, ulul;
	OBJ_tdst_Group	*pst_Group;
	OBJ_tdst_Group	*pst_OldGroup;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Create group in memory */
	pst_Group = (OBJ_tdst_Group *) MEM_p_Alloc(sizeof(OBJ_tdst_Group));
	L_memset(pst_Group, 0, sizeof(OBJ_tdst_Group));

	/* Name of group */
	L_strcpy(asz_Name, _psz_Name);
	L_strcpy(asz_Name1, _psz_Name);
	_psz_Name = asz_Name1;
	L_strcat(asz_Name, EDI_Csz_ExtObjGroups);

	/* Assign name */
	while(!_b_Over && BIG_ul_SearchFileExt(_psz_Path, asz_Name) != BIG_C_InvalidIndex)
	{
		if(!_b_SameName)
		{
			ERR_X_ForceError("The file already exists !", NULL);
			return NULL;
		}

		sprintf(asz_Name, "_%s", asz_Name1);
		L_strcpy(asz_Name1, asz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtObjGroups);
	}

	pst_Group->sz_Name = (char *) MEM_p_Alloc(strlen(asz_Name) + 4);
	strcpy(pst_Group->sz_Name, asz_Name);

	/* Create file group in level */
	BIG_ul_CreateDir(_psz_Path);
	SAV_Begin(_psz_Path, asz_Name);
	SAV_Buffer(_psz_Path, 1);
	ul_Index = SAV_ul_End();

	/* The group is loaded and overwrite ? */
	ul_Adr = LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
	pst_OldGroup = (OBJ_tdst_Group *) ul_Adr;
	if(ul_Adr == BIG_C_InvalidIndex)
	{
		*pdel = TRUE;
		LOA_AddAddress(ul_Index, pst_Group);
	}
	else
	{
		*pdel = FALSE;
	}

	/* Create gol */
	pst_Group->pst_AllObjects = (TAB_tdst_PFtable *) MEM_p_Alloc(sizeof(TAB_tdst_PFtable));
	TAB_PFtable_Init(pst_Group->pst_AllObjects, 0, 0.1f);
	L_strcpy(asz_Name, asz_Name1);
	L_strcat(asz_Name, EDI_Csz_ExtObjGolGroups);
	BIG_ul_CreateDir(_psz_Path);
	SAV_Begin(_psz_Path, asz_Name);
	SAV_Buffer(_psz_Path, 1);
	ul_Index = SAV_ul_End();

	if(ul_Adr != BIG_C_InvalidIndex)
	{
		OBJ_FreeGroupGrl(pst_OldGroup->pst_AllObjects);
		if(pst_OldGroup->sz_Name) MEM_Free(pst_OldGroup->sz_Name);
		ulul = pst_OldGroup->ul_NbObjectsUsingMe;
		L_memcpy(pst_OldGroup, pst_Group, sizeof(OBJ_tdst_Group));
		MEM_Free(pst_Group);
		pst_Group = pst_OldGroup;
		pst_Group->ul_NbObjectsUsingMe = ulul;
	}

	LOA_AddAddress(ul_Index, pst_Group->pst_AllObjects);

	/* Real saving */
	GRP_SaveGroup(pst_Group);

	return pst_Group;
}

#endif

/*$F
 =======================================================================================================================
    




									CLASSICAL GROUPS FUNCTIONS






 =======================================================================================================================
 */

/*
 =======================================================================================================================
    Aim:    Detach a group from an object
 =======================================================================================================================
 */
void GRP_ObjDetachGroup(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group		*pst_Group;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject *pst_GrpGO;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->pst_Extended && MEM_b_CheckPointer(_pst_GO->pst_Extended))
	{
		pst_Group = _pst_GO->pst_Extended->pst_Group;
		if(pst_Group && MEM_b_CheckPointer(pst_Group))
		{
			/* Decrements the number of objects that use the group */
			pst_Group->ul_NbObjectsUsingMe--;

			if(pst_Group->ul_NbObjectsUsingMe == 0)
			{
				/*
				 * Particular case of a fuckin' BONE. We have to remove manually all the
				 * GameObjects of its group if they have the Bone Identity Flag.
				 */
				if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Bone))
				{
					pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
					pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
					for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
					{
						pst_GrpGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
						if(TAB_b_IsAHole(pst_GrpGO)) continue;
						/* For fake Rayman GOs */
						if(!(pst_GrpGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)) continue;
						if(pst_GrpGO == _pst_GO) continue;

						//TAB_Ptable_RemoveElem( ( TAB_tdst_Ptable * ) ( pst_Group->pst_AllObjects ), ( void ** ) ( pst_CurrentElem ) );

						pst_World = WOR_World_GetWorldOfObject(pst_GrpGO);
						WOR_World_DetachObject(pst_World, pst_GrpGO);
						OBJ_GameObject_Remove(pst_GrpGO, 1);
#ifdef ACTIVE_EDITORS
						SEL_DelItem(pst_World->pst_Selection, pst_GrpGO);
#endif
					}
				}

				LOA_DeleteAddress(pst_Group);
				LINK_DelRegisterPointer(pst_Group);
				OBJ_FreeGroup(pst_Group);
			}

			_pst_GO->pst_Extended->pst_Group = NULL;
			OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Group);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Attach a group to a game object

    Note:   Warning: use only GRP_ObjAttachReplaceGroup
 =======================================================================================================================
 */
void GRP_ObjAttachGroup(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_Group *_pst_Group)
{
	if(MEM_b_CheckPointer(_pst_GO->pst_Extended))
	{
		if(MEM_b_CheckPointer(_pst_Group))
		{
			OBJ_SetIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Group);

			/* Attach group to object */
			_pst_GO->pst_Extended->pst_Group = _pst_Group;
			_pst_Group->ul_NbObjectsUsingMe++;
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Attach a group to an object, and performs the necessary operations to replace an old group
 =======================================================================================================================
 */
void GRP_ObjAttachReplaceGroup(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_Group *_pst_Group)
{
	/* Object doesn't have a group yet so we create one */
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Group))
	{
		/* If no extended struct, allocate */
		OBJ_GameObject_CreateExtendedIfNot(_pst_GO);
		GRP_ObjAttachGroup(_pst_GO, _pst_Group);
	}
	else
	{
		/* Old group is removed, and the new group is attached... */
		if(!(_pst_GO->pst_Extended && (_pst_GO->pst_Extended->pst_Group == _pst_Group)))
		{
			GRP_ObjDetachGroup(_pst_GO);
			GRP_ObjAttachGroup(_pst_GO, _pst_Group);
		}
	}
}

/*$F
 =======================================================================================================================
    




									SKELETON GROUPS FUNCTIONS






 =======================================================================================================================
 */

/*
 =======================================================================================================================
    Aim:    Detach a Skeleton from an object
 =======================================================================================================================
 */
void GRP_ObjDetachSkeleton(OBJ_tdst_GameObject *_pst_GO)
{
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)) return;

	if(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
		ANI_DestroySkeleton(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton);
	if(_pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
		ANI_DestroySkeleton(_pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_ObjAttachSkeleton(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_Group *_pst_Model, OBJ_tdst_Group *_pst_Skeleton)
{
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		OBJ_ChangeIdentityFlags
		(
			_pst_GO,
			OBJ_ul_FlagsIdentityGet(_pst_GO) & (~OBJ_C_IdentityFlag_Anims),
			OBJ_C_UnvalidFlags
		);
	}

	if(MEM_b_CheckPointer(_pst_Model))
	{
		_pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel = _pst_Model;
		ANI_UseSkeleton(_pst_Model);
	}

	if(MEM_b_CheckPointer(_pst_Skeleton))
	{
		_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton = _pst_Skeleton;
		ANI_UseSkeleton(_pst_Skeleton);
	}
}

/*
 =======================================================================================================================
    Aim:    Attach a Skeleton group to an object, and performs the necessary operations to replace an old Skeleton
 =======================================================================================================================
 */
void GRP_ObjAttachReplaceSkeleton
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_Group		*_pst_Model,
	OBJ_tdst_Group		*_pst_Skeleton
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Object doesn't have a anim structure yet so we create one */
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		OBJ_ChangeIdentityFlags
		(
			_pst_GO,
			OBJ_ul_FlagsIdentityGet(_pst_GO) &~OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_Anims,
			OBJ_C_UnvalidFlags
		);
	}

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	if(_pst_Model == pst_GOAnim->pst_SkeletonModel) return;

	GRP_ObjDetachSkeleton(_pst_GO);
	GRP_ObjAttachSkeleton(_pst_GO, _pst_Model, _pst_Skeleton);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
