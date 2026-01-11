/*$T EOTmain.c GC! 1.078 03/13/00 17:18:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Main functions of the EOT (engine objects tables) module */
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/EOT/EOT.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "ENGine/Sources/ENGvars.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    Clears all the Engine Objects Tables
 =======================================================================================================================
 */
void EOT_SetOfEOT_Clear(EOT_tdst_SetOfEOT *pst_SetOfEOT)
{
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_Visu));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_Anims));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_Dyna));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_AI));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_ColMap));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_ZDM));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_ZDE));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_Events));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_Hierarchy));
	TAB_PFtable_Clear(&(pst_SetOfEOT->st_ODE));
}

void	EOT_AISortWithPriority(TAB_tdst_PFtable *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOT_SetOfEOT_AddOneElem(EOT_tdst_SetOfEOT *_pst_SetOfEOT, OBJ_tdst_GameObject *_pst_NewGO)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	ul_IDFlags;
	/*~~~~~~~~~~~~~~~*/

	ul_IDFlags = _pst_NewGO->ul_IdentityFlags;

	if(ul_IDFlags & OBJ_C_IdentityFlag_Visu)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_Visu), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_Anims)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_Anims), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_Dyna)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_Dyna), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_ColMap)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_ColMap), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_ZDM)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_ZDM), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_ZDE)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_ZDE), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_Events)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_Events), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_Hierarchy), (void *) _pst_NewGO, ul_IDFlags);
	}

	if(ul_IDFlags & OBJ_C_IdentityFlag_ODE)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_ODE), (void *) _pst_NewGO, ul_IDFlags);
	}


	if(ul_IDFlags & OBJ_C_IdentityFlag_AI)
	{
		TAB_PFtable_AddElemWithData(&(_pst_SetOfEOT->st_AI), (void *) _pst_NewGO, ul_IDFlags);
		EOT_AISortWithPriority(&(_pst_SetOfEOT->st_AI));
	}
}

/*
 =======================================================================================================================
    Aim:    Build all the Engine Objects Tables (by filling a set of EOT) from a given Activ Objects Table.
 =======================================================================================================================
 */
void EOT_SetOfEOT_Build(EOT_tdst_SetOfEOT *_pst_SetOfEOT, TAB_tdst_PFtable *_pst_ActivObjects)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	extern void OBJ_ResetProcess(OBJ_tdst_GameObject *, ULONG , ULONG );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef RASTERS_ON
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_EOTcreation);
#endif
	/* First, we clear all the tables */
	EOT_SetOfEOT_Clear(_pst_SetOfEOT);

	/* We loop once thru the active objects table to build the EOTables */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_ActivObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_ActivObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_Visu))
		{
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_Visu.ul_NbElems <= OETVisuSize, "Object Engine Table (Visu flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_Visu.ul_NbElems <= OETWarningTreshold * OETVisuSize, "Object Engine Table (Visu flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_Visu), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_Anims))
		{
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_Anims.ul_NbElems <= OETVisuSize, "Object Engine Table (Anims flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_Anims.ul_NbElems <= OETWarningTreshold * OETVisuSize, "Object Engine Table (Anims flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_Anims), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_Dyna))
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			OBJ_ResetProcess(pst_GAO, WOR_World_GetWorldOfObject(pst_GAO)->ul_ProcessCounterDyn, OBJ_C_ProcessedDyn);
            ERR_X_ErrorAssert(_pst_SetOfEOT->st_Dyna.ul_NbElems <= OETDynaSize, "Object Engine Table (Anims flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_Dyna.ul_NbElems <= OETWarningTreshold * OETDynaSize, "Object Engine Table (Anims flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_Dyna), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_AI))
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			OBJ_ResetProcess(pst_GAO, WOR_World_GetWorldOfObject(pst_GAO)->ul_ProcessCounterAI, OBJ_C_ProcessedAI);
            ERR_X_ErrorAssert(_pst_SetOfEOT->st_AI.ul_NbElems <= OETAISize, "Object Engine Table (AI flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_AI.ul_NbElems <= OETWarningTreshold * OETAISize, "Object Engine Table (AI flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_AI), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_ColMap))
		{
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_ColMap.ul_NbElems <= OETColMapSize, "Object Engine Table (ColMap flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_ColMap.ul_NbElems <= OETWarningTreshold * OETColMapSize, "Object Engine Table (ColMap flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_ColMap), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_ZDM))
		{
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_ZDM.ul_NbElems <= OETZDMSize, "Object Engine Table (ZDM) flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_ZDM.ul_NbElems <= OETWarningTreshold * OETZDMSize, "Object Engine Table (ZDM) flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_ZDM), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_ZDE))
		{
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_ZDE.ul_NbElems <= OETZDESize, "Object Engine Table (ZDE flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_ZDE.ul_NbElems <= OETWarningTreshold * OETZDESize, "Object Engine Table (ZDE flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_ZDE), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_Events))
		{
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_Events.ul_NbElems <= OETEventsSize, "Object Engine Table (Events flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_Events.ul_NbElems <= OETWarningTreshold * OETEventsSize, "Object Engine Table (Events flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_Events), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_ODE))
		{
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_ODE.ul_NbElems <= OETODESize, "Object Engine Table (ODE flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_ODE.ul_NbElems <= OETWarningTreshold * OETODESize, "Object Engine Table (ODE flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_ODE), pst_CurrentElem);
		}
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, OBJ_C_IdentityFlag_Hierarchy))
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			OBJ_ResetProcess(pst_GAO, WOR_World_GetWorldOfObject(pst_GAO)->ul_ProcessCounterHie, OBJ_C_ProcessedHie);
			ERR_X_ErrorAssert(_pst_SetOfEOT->st_Hierarchy.ul_NbElems <= OETHierarchySize, "Object Engine Table (Hierarchy flag) is full", NULL);                        
            ERR_X_Warning(_pst_SetOfEOT->st_Hierarchy.ul_NbElems <= OETWarningTreshold * OETHierarchySize, "Object Engine Table (Hierarchy flag) is nearly full",NULL)            
			TAB_PFtable_AddElem(&(_pst_SetOfEOT->st_Hierarchy), pst_CurrentElem);
		}
	}

	/* We sort the EOT of AIs using the priority */
	EOT_AISortWithPriority(&(_pst_SetOfEOT->st_AI));

#ifdef RASTERS_ON
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_EOTcreation);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOT_SetOfEOT_OneBuild(TAB_tdst_PFtable *_pst_Table, ULONG _ul_Flag, TAB_tdst_PFtable *_pst_ActivObjects)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef RASTERS_ON
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_EOTcreation);
#endif
	TAB_PFtable_Clear(_pst_Table);

	/* We loop once thru the active objects table to build the EOTables */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_ActivObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_ActivObjects);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;
		pst_CurrentElem->ul_Flags = OBJ_ul_FlagsIdentityGet(pst_GAO);
		if(TAB_b_PFelem_TestFlag(pst_CurrentElem, _ul_Flag))
		{
			TAB_PFtable_AddElem(_pst_Table, pst_CurrentElem);
		}
	}

#ifdef RASTERS_ON
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_EOTcreation);
#endif
}

/*
 =======================================================================================================================
    Aim:    Update the identity flags of the activ objects or of an EOT

    Note:   This is done only when the identity of a objects changes
 =======================================================================================================================
 */
void EOT_UpdateIdentityFlagsOfTable(TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* We loop once thru all the objects of the EOT table to update the identity flags */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;
		pst_CurrentElem->ul_Flags = OBJ_ul_FlagsIdentityGet(pst_GAO);
	}
}

/*
 =======================================================================================================================
    Aim:    Remove object from EOT table if the tested flag is not correct

    Note:   Flags is correct if the flag in the EOT and the flag in the object is the same. It can happen that the
            flags are different if identity of the object has changed and the activation refresh is not every frame
 =======================================================================================================================
 */
void EOT_RemoveObjectFromTableIfBadFlag(TAB_tdst_PFtable *_pst_Table, ULONG _ul_TestedFlag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* We loop once thru all the objects of the EOT table to update the identity flags */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;
		if((pst_CurrentElem->ul_Flags & _ul_TestedFlag) != (OBJ_ul_FlagsIdentityGet(pst_GAO) & _ul_TestedFlag))
		{
			/* Flags are not correct so we remove the element from table */
			TAB_PFtable_RemoveElem(_pst_Table, pst_CurrentElem);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Sort the AI Eot using the Priority Flag

    Note:   The sorting is done using two passes only.
 =======================================================================================================================
 */
void EOT_AISortWithPriority(TAB_tdst_PFtable *_pst_EotAI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	unsigned char		uc_Prio;
	int					ai_nb[256];
	int					ai_pos[256];
	TAB_tdst_PFelem		st_Tmp;
	TAB_tdst_PFelem		st_Tmp2;
	int					i;
	TAB_tdst_PFelem		*pst_Dst;
	TAB_tdst_PFelem		*pst_FirstElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Begin:

	/* Clear the ai_nb table */
	L_memset(ai_nb, 0, 256 * sizeof(int));

	/* 1: First pass to count the number of elements of each priority */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_EotAI);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_EotAI);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		/*
		 * The AI EOT table should never have a hole or there might have priority sorting
		 * problems
		 */
		if(TAB_b_IsAHole(pst_GAO))
		{
			TAB_PFtable_RemoveHoles(_pst_EotAI);
			goto _Begin;
		}

		uc_Prio = pst_GAO->pst_Extended->uc_AiPrio;

		/* Increments the number of objects with this priority */
		ai_nb[uc_Prio]++;
	}

	/* 2: We calculate the position of each priority in the Table */
	ai_pos[0] = 0;
	for(i = 0; i < 255; i++) ai_pos[i + 1] = ai_pos[i] + ai_nb[i];

	/* 3: Second pass to sort */
	pst_FirstElem = TAB_pst_PFtable_GetFirstElem(_pst_EotAI);
	pst_CurrentElem = pst_FirstElem;
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_EotAI);
	st_Tmp.p_Pointer = (void *) 0xffffffff;
	while(pst_CurrentElem <= pst_EndElem)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		/* If not already sorted */
		if(!(pst_CurrentElem->ul_Flags & 0x40000000))
		{
			uc_Prio = pst_GAO->pst_Extended->uc_AiPrio;
			pst_Dst = pst_FirstElem + ai_pos[uc_Prio];
			if((pst_Dst != pst_CurrentElem) && (pst_Dst <= pst_EndElem))
			{
				if(st_Tmp.p_Pointer == (void *) 0xffffffff)
				{
					st_Tmp = *pst_Dst;
					*pst_Dst = *pst_CurrentElem;

					/* Mark dst pointer as sorted */
					pst_Dst->ul_Flags |= 0x40000000;

					/* Mark src pointer as empty */
					pst_CurrentElem->p_Pointer = (void *) 0xffffffff;
					ai_pos[uc_Prio]++;
				}

				/* As LONG as the tmp pointer is not empty, we place it at the right place */
				while(st_Tmp.p_Pointer != (void *) 0xffffffff)
				{
					pst_GAO = (OBJ_tdst_GameObject *) st_Tmp.p_Pointer;
					uc_Prio = pst_GAO->pst_Extended->uc_AiPrio;
					pst_Dst = pst_FirstElem + ai_pos[uc_Prio];
					if(pst_Dst <= pst_EndElem)
					{
						st_Tmp2 = *pst_Dst;
						*pst_Dst = st_Tmp;

						/* Mark dst pointer as sorted */
						pst_Dst->ul_Flags |= 0x40000000;

						/*
						 * If we just sorted the current element of the list, we increment the current
						 * element
						 */
						if(pst_Dst == pst_CurrentElem) pst_CurrentElem++;
						st_Tmp = st_Tmp2;
						ai_pos[uc_Prio]++;
					}
					else
						break;
				}
			}
			else
			{
				ai_pos[uc_Prio]++;
				pst_CurrentElem++;
			}
		}
		else
			pst_CurrentElem++;
	}
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
