 /*$T ACTinit.h GC!1.55 02/04/00 11:12:22 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __ACTINIT_H__
#define __ACTINIT_H__

#include "BASe/BASsys.h"
#include "BASe/MEMory/MEM.h"
#include "LINks/LINKstruct.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void ACT_UseAction(ACT_st_Action *_pst_Action)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    _pst_Action->uw_Counter++;

    for(i = 0; i < _pst_Action->uc_NumberOfActionItem; i++)
	{
        if(_pst_Action->ast_ActionItem[i].pst_TrackList)
            _pst_Action->ast_ActionItem[i].pst_TrackList->ul_NbOfInstances ++;
        if(_pst_Action->ast_ActionItem[i].pst_Shape)
            _pst_Action->ast_ActionItem[i].pst_Shape->uw_Counter++;

	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void ACT_UseActionKit(ACT_st_ActionKit *_pst_ActionKit)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    _pst_ActionKit->uw_Counter++;

    for(i = 0; i < _pst_ActionKit->uw_NumberOfAction; i++)
	{
        if(_pst_ActionKit->apst_Action[i])
            ACT_UseAction(_pst_ActionKit->apst_Action[i]);
	}
}

/*
 =======================================================================================================================
    Aim:    Decreaze the number of time the structure is used and if it is 0, delete it
 =======================================================================================================================
 */
_inline_ void ACT_FreeAction(ACT_st_Action **_ppst_Action)
{
    /*~~~~~~*/
    int i, j;
    /*~~~~~~*/

	for(i = 0; i < (*_ppst_Action)->uc_NumberOfActionItem; i++)
	{
		if((*_ppst_Action)->ast_ActionItem[i].pst_TrackList)
			EVE_DeleteListTracks((*_ppst_Action)->ast_ActionItem[i].pst_TrackList);
		if((*_ppst_Action)->ast_ActionItem[i].pst_Shape)
			ANI_FreeShape(&((*_ppst_Action)->ast_ActionItem[i].pst_Shape));
	}
	
    if(--(*_ppst_Action)->uw_Counter == 0)
    {
		for(i = 0; i < (*_ppst_Action)->uc_NumberOfActionItem; i++)
		{
			if((*_ppst_Action)->ast_ActionItem[i].pst_Transitions)
			{
				for(j = 0; j < (*_ppst_Action)->ast_ActionItem[i].pst_Transitions->num; j++)
                {
					MEM_Free((void *) (*_ppst_Action)->ast_ActionItem[i].pst_Transitions->base[j].ul_Val);
                    (*_ppst_Action)->ast_ActionItem[i].pst_Transitions->base[j].ul_Val = 0;
                }
				BAS_bfree((*_ppst_Action)->ast_ActionItem[i].pst_Transitions);
				MEM_Free((*_ppst_Action)->ast_ActionItem[i].pst_Transitions);
				(*_ppst_Action)->ast_ActionItem[i].pst_Transitions = NULL;
			}
		}
		
        MEM_Free(*_ppst_Action);
        LOA_DeleteAddress(*_ppst_Action);
        LINK_DelRegisterPointer(*_ppst_Action);
        *_ppst_Action = NULL;
    }
}

#ifdef ANIMS_USE_ARAM
extern int ARAM_Anims_RAM;
extern int ARAM_Anims_ARAM;
#endif
/*
 =======================================================================================================================
    Aim:    Decreaze the number of time the structure is used and if it is 0, delete it
 =======================================================================================================================
 */
_inline_ void ACT_FreeActionKit(ACT_st_ActionKit **_ppst_ActionKit)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

	ACT_ActionRestoreInit(*_ppst_ActionKit);
	
#ifdef ANIMS_USE_ARAM
    if((*_ppst_ActionKit)->uw_Counter - 1 == 0)
    {
        for(i = 0; i < (*_ppst_ActionKit)->uw_NumberOfAction; i++)    
		{
            if((*_ppst_ActionKit)->apst_Action[i])		
            {
    			EVE_tdst_ListTracks	 *pst_TrackList;
    			int j, k;

    			for(j = 0; j < (*_ppst_ActionKit)->apst_Action[i]->uc_NumberOfActionItem; j++)
    			{
    				pst_TrackList = (*_ppst_ActionKit)->apst_Action[i]->ast_ActionItem[j].pst_TrackList;
    				if(pst_TrackList && pst_TrackList->pst_Cache && (pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_InRAMGC))
    				{
						pst_TrackList->pst_Cache->l_Shares --;						
						if(!pst_TrackList->pst_Cache->l_Shares)
						{
							for(k = 0; k < (int) pst_TrackList->uw_NumTracks; k++)
							{
								if(pst_TrackList->pst_AllTracks[k].w_Type & EVE_C_Track_InRAMGC)
								{
									MEM_Free(pst_TrackList->pst_AllTracks[k].pst_AllEvents);
									pst_TrackList->pst_AllTracks[k].pst_AllEvents = pst_TrackList->pst_Cache->ppst_ListEvents[k];
									pst_TrackList->pst_AllTracks[k].w_Type &= ~EVE_C_Track_InRAMGC;
								}				
							}
							ARAM_Anims_RAM --;														
							
							MEM_Free(pst_TrackList->pst_Cache->ppst_ListEvents);
							pst_TrackList->pst_Cache->pst_TrackList = NULL;
							pst_TrackList->pst_Cache = NULL;
							pst_TrackList->ul_GC_Flags &= ~EVE_C_ListTracks_InRAMGC;
						}
    				}
    			}
    		}
		}
}
#endif
	
    for(i = 0; i < (*_ppst_ActionKit)->uw_NumberOfAction; i++)
	{
        if((*_ppst_ActionKit)->apst_Action[i])
            ACT_FreeAction(&(*_ppst_ActionKit)->apst_Action[i]);
	}

    if(--(*_ppst_ActionKit)->uw_Counter == 0)
    {
        MEM_Free(*_ppst_ActionKit);
        LOA_DeleteAddress(*_ppst_ActionKit);
        LINK_DelRegisterPointer(*_ppst_ActionKit);
        *_ppst_ActionKit = NULL;
    }

	*_ppst_ActionKit = NULL;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif __ACTINIT_H__
