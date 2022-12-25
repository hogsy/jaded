/*$T AIdebug.c GC!1.40 07/07/99 17:12:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIxxx.h"
#include "AIstruct.h"
#include "AIdebug.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/ENGinit.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AIload.h"
#include "BIGfiles/BIGfat.h"
/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

L_jmp_buf           AI_gst_ContextIn;
L_jmp_buf           AI_gst_ContextOut;
L_jmp_buf           AI_gst_ContextCheck;
char                *AI_gpc_StackIn = NULL;
int                 AI_gi_SizeStackIn = 0;
BOOL                AI_gb_ExitByBreak = FALSE;
BOOL				AI_gb_ErrorWhenBreak = FALSE;
int                 AI_gi_OneStepMode = 0;
int                 AI_gi_FctLevelWhenBreak = 0;

int                 AI_gi_NumBreak = 0;
AI_tdst_BreakPoint  AI_gast_BreakList[AI_C_MaxBreakPoint];
AI_tdst_BreakPoint  AI_gst_BreakInfo;

AI_tdst_CallStack	AI_gst_CallStack[1000];

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
void AI_InitBreakList(void)
{
    AI_ReinitBreakMode();
    AI_DelAllBreakPoints();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void AI_ReinitBreakMode(void)
{
    L_memset(&AI_gst_BreakInfo, 0, sizeof(AI_tdst_BreakPoint));
    AI_gb_ExitByBreak = FALSE;
    AI_gi_OneStepMode = 0;
    AI_gi_FctLevelWhenBreak = 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void AI_DelAllBreakPoints(void)
{
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElemW;
	TAB_tdst_PFelem		*pst_EndElemW;
	WOR_tdst_World		*pst_World;
	AI_tdst_Instance	*pst_Instance;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	AI_tdst_Node		*pst_Node;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_gi_NumBreak = 0;
	pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	if(!pst_CurrentElemW) return;
	pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
		if(!TAB_b_IsAHole(pst_World))
		{
			pst_AIEOT = &(pst_World->st_AllWorldObjects);
			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
			for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GAO)) continue;

				if((pst_GAO->pst_Extended) && (pst_GAO->pst_Extended->pst_Ai))
				{
					pst_Instance = (AI_tdst_Instance *) pst_GAO->pst_Extended->pst_Ai;
					if(!pst_Instance) continue;
					if(!pst_Instance->pst_Model) continue;

					for(i = 0; i < (int) pst_Instance->pst_Model->ul_NbFunctions; i++)
					{
						if(!pst_Instance->pst_Model->ppst_Functions) continue;
						if(!pst_Instance->pst_Model->ppst_Functions[i]) continue;
						pst_Node = pst_Instance->pst_Model->ppst_Functions[i]->pst_RootNode;
						while(pst_Node && pst_Node->c_Type != CATEG_ENDTREE)
						{
							pst_Node->c_Flags &= ~AI_Cflag_ForceBreakPoint;
							pst_Node->c_Flags &= ~AI_Cflag_BreakPoint;
							if(pst_Node->c_Type == AI_gaw_EnumLink[CATEG_KEYWORD] && pst_Node->w_Param == AI_gaw_EnumLink[KEYWORD_CALLPROC])
							{
								pst_Node += 3 + pst_Node[2].l_Param;
							}
							else
							{
								pst_Node++;
							}
						}
					}

					for(i = 0; i < (int) pst_Instance->pst_Model->u32_NbProcList; i++)
					{
						if(!pst_Instance->pst_Model->pp_ProcList) continue;
						if(!pst_Instance->pst_Model->pp_ProcList[i]) continue;
						for(j = 0; j < pst_Instance->pst_Model->pp_ProcList[i]->u16_Num; j++)
						{
							pst_Node = pst_Instance->pst_Model->pp_ProcList[i]->pt_All[j].pu32_Nodes;
							while(pst_Node && pst_Node->c_Type != CATEG_ENDTREE)
							{
								pst_Node->c_Flags &= ~AI_Cflag_ForceBreakPoint;
								pst_Node->c_Flags &= ~AI_Cflag_BreakPoint;
								if(pst_Node->c_Type == AI_gaw_EnumLink[CATEG_KEYWORD] && pst_Node->w_Param == AI_gaw_EnumLink[KEYWORD_CALLPROC])
								{
									pst_Node += 3 + pst_Node[2].l_Param;
								}
								else
								{
									pst_Node++;
								}
							}
						}
					}
				}
			}
		}
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void AI_DelBreakListForModel(AI_tdst_Model *_pst_Model)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Model) return;
    for(i = 0; i < AI_gi_NumBreak; i++)
    {
        if(AI_gast_BreakList[i].pst_Model == _pst_Model)
            AI_gast_BreakList[i].pst_Model = NULL;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void AI_AddBreakPoint
(
    ULONG               _ul_File,
    ULONG			    _ul_EngFile,
    AI_tdst_Model       *_pst_Model,
    AI_tdst_Instance    *_pst_Instance,
	int					line
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < AI_gi_NumBreak; i++)
        if(AI_gast_BreakList[i].pst_Model == NULL) break;

    if(i == AI_gi_NumBreak)
    {
        ERR_X_Assert(AI_gi_NumBreak != AI_C_MaxBreakPoint);
        AI_gi_NumBreak++;
    }

    AI_gast_BreakList[i].ul_File = _ul_File;
    AI_gast_BreakList[i].ul_EngFile = _ul_EngFile;
    AI_gast_BreakList[i].pst_Model = _pst_Model;
    AI_gast_BreakList[i].pst_Instance = _pst_Instance;
	AI_gast_BreakList[i].i_Line = line;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL AI_SearchBreakPoint
(
    AI_tdst_Function    *_pst_Function,
    AI_tdst_Model       *_pst_Model,
    AI_tdst_Instance    *_pst_Instance,
	int					line
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i;
    AI_tdst_BreakPoint  *p;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < AI_gi_NumBreak; i++)
    {
        p = &AI_gast_BreakList[i];
        if
        (
            (p->pst_Model == _pst_Model) &&
            (p->ul_EngFile == _pst_Function->h_File) &&
            ((p->pst_Instance == NULL) || (p->pst_Instance == _pst_Instance)) &&
			(p->i_Line == line)
        ) return TRUE;
    }

    return FALSE;
}

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
