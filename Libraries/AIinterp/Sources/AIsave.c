/*$T AIsave.c GC!1.52 10/26/99 12:22:19 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AImsg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "AIerrid.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "ENGine/Sources/WORld/WORsave.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 ===================================================================================================
 ===================================================================================================
 */
BIG_KEY AI_ul_CreateInstance
(
    WOR_tdst_World  *_pst_World,
    BIG_INDEX       _ul_GameEng,
    BIG_INDEX       _ul_Model,
    char            *_psz_Name
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_File, ul_File1;
    char        asz_Path[BIG_C_MaxLenPath];
    char        asz_Path1[BIG_C_MaxLenPath];
    char        asz_Name[BIG_C_MaxLenPath];
    char        asz_Name1[BIG_C_MaxLenPath];
    char        asz_Name2[BIG_C_MaxLenPath];
    char        *psz_Temp;
    ULONG       ul_Size;
    BIG_INDEX   aul_InitFct[AI_C_MaxTracks];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Search engine model */
    BIG_ComputeFullName(BIG_ParentFile(_ul_Model), asz_Path);
    L_strcpy(asz_Name, BIG_NameFile(_ul_Model));
    psz_Temp = L_strrchr(asz_Name, '.');
    if(L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineModel))
    {
        if(psz_Temp) *psz_Temp = 0;
        L_strcat(asz_Path, "/");
        L_strcat(asz_Path, asz_Name);
        L_strcat(asz_Name, EDI_Csz_ExtAIEngineModel);
    }

    /* Name of actor to create instance */
    if(!_pst_World)
    {
        L_strcpy(asz_Path1, asz_Path);
        L_strcat(asz_Path1, "/");
        L_strcat(asz_Path1, "Dummy Instance");
        L_strcpy(asz_Name1, asz_Name);
    }
    else
    {
        WOR_GetSubPath( _pst_World, EDI_Csz_Path_AIInstances, asz_Path1 );
        L_strcat(asz_Path1, "/");
        L_strcpy(asz_Name1, BIG_NameFile(_ul_Model));
        psz_Temp = L_strrchr(asz_Name1, '.');
        if(psz_Temp) *psz_Temp = 0;
        L_strcat(asz_Path1, asz_Name1);
        L_strcpy(asz_Name1, _psz_Name);
    }

    ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);
    if(ul_File != BIG_C_InvalidIndex)
    {
        /* Duplicate variable file (one for the instance) */
        *L_strrchr(asz_Name1, '.') = 0;
        L_strcat(asz_Name1, EDI_Csz_ExtAIEngineVars);

        /* Save dummy instance variable file */
		while(BIG_ul_SearchFileExt(asz_Path1, asz_Name1) != BIG_C_InvalidIndex)
		{
			snprintf( asz_Name2, sizeof(asz_Name2), "_%s", asz_Name1 );
			L_strcpy(asz_Name1, asz_Name2);
			ERR_X_Assert(L_strlen(asz_Name1) < BIG_C_MaxLenName);
		}

        SAV_Begin(asz_Path1, asz_Name1);
        ul_Size = 0;
        SAV_Buffer(&ul_Size, sizeof(int));
        SAV_Buffer(&ul_Size, sizeof(int));
        SAV_Buffer(&ul_Size, sizeof(int));
        SAV_Buffer(&ul_Size, sizeof(int));
        SAV_Buffer(&ul_Size, sizeof(int));
        L_memset(aul_InitFct, 0, sizeof(aul_InitFct));
        SAV_Buffer(aul_InitFct, sizeof(LONG) * AI_C_MaxTracks);
        ul_File1 = SAV_ul_End();

        /* Create a dummy instance */
        *L_strrchr(asz_Name1, '.') = 0;
        L_strcat(asz_Name1, EDI_Csz_ExtAIEngineInstance);

		/* Instance */
		while(BIG_ul_SearchFileExt(asz_Path1, asz_Name1) != BIG_C_InvalidIndex)
		{
			snprintf( asz_Name2, sizeof(asz_Name2), "_%s", asz_Name1 );
			L_strcpy(asz_Name1, asz_Name2);
			ERR_X_Assert(L_strlen(asz_Name1) < BIG_C_MaxLenName);
		}

        SAV_Begin(asz_Path1, asz_Name1);
        SAV_Buffer(&BIG_FileKey(ul_File), 4);   /* Model */
        SAV_Buffer(&BIG_FileKey(ul_File1), 4);
        ul_File = SAV_ul_End();

        LINK_FatHasChanged();
        return BIG_FileKey(ul_File);
    }

    return BIG_C_InvalidIndex;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void AI_SaveInstance(AI_tdst_Instance *_pst_Instance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                    asz_Path[BIG_C_MaxLenPath];
    char                    asz_Name[BIG_C_MaxLenName];
    char                    *psz_Temp;
    ULONG                   i, j, ul_Size, ul_Size1, ul_Size2, ul_SizeInit, ul_Offset, ul_NumDif;
	ULONG					ul_SizeR;
    int                     iNumElem, iNumElemEx;
    ULONG                   ul_Offset1, ul_SizeArr, ul_Add;
    AI_tdst_VarDes          *pst_VarDes, *pst_ModelDes;
    AI_tdst_VarInfo         *pst_VarsInfos;
    AI_tdst_EditorVarInfo   *pst_EditorVarsInfos;
    char                    *pc_Init, *pc_Init1;
    BIG_INDEX               ul_File;
    BIG_INDEX               aul_InitFct[AI_C_MaxTracks];
    BIG_INDEX               ul_CurrentInstance, _ul_CurrentModel;
    BIG_KEY                 *pul_Key;
    AI_tdst_Model           *pst_Model;
    int                     i_Offset, i_Max;
	char					az[512];
	AI_tdst_Message			*pmsg, *pmsg1;
	SCR_tt_Trigger			*ptrg, *ptrg1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Instance) return;
    ul_CurrentInstance = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Instance);
    if(ul_CurrentInstance == BIG_C_InvalidIndex) return;
    ul_CurrentInstance = BIG_ul_SearchKeyToFat(ul_CurrentInstance);
    if(ul_CurrentInstance == BIG_C_InvalidIndex) return;

    pst_Model = _pst_Instance->pst_Model;
	if(!pst_Model) return;
    pst_ModelDes = pst_Model->pst_VarDes;
    _ul_CurrentModel = LOA_ul_SearchKeyWithAddress((ULONG) pst_Model);
    ERR_X_Assert(_ul_CurrentModel != BIG_C_InvalidIndex);
    _ul_CurrentModel = BIG_ul_SearchKeyToFat(_ul_CurrentModel);
    ERR_X_Assert(_ul_CurrentModel != BIG_C_InvalidIndex);

    /*$2
     -----------------------------------------------------------------------------------------------
        Save variables
     -----------------------------------------------------------------------------------------------
     */

    /* Compute var file name */
    BIG_ComputeFullName(BIG_ParentFile(ul_CurrentInstance), asz_Path);
    L_strcpy(asz_Name, BIG_NameFile(ul_CurrentInstance));
    psz_Temp = L_strrchr(asz_Name, '.');
    if(psz_Temp) *psz_Temp = 0;
    L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);

    SAV_Begin(asz_Path, asz_Name);

    /* Compute number of different variables */
    ul_NumDif = 0;
    ul_SizeInit = 0;
	if(pst_ModelDes && _pst_Instance->pst_VarDes)
	{
		for(i = 0; i < pst_ModelDes->ul_NbVars; i++)
		{
			/* Skip array header */
			iNumElem = pst_ModelDes->pst_VarsInfos[i].i_NumElem;
			ul_SizeArr = (ULONG) iNumElem >> 30;

			/* Search if var is present in instance */
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

			ul_Offset1 = pst_Model->pst_VarDes->pst_VarsInfos[i].i_Offset;

			/* Verify that the number of elem is the same in model and instance.
			 * Else we won't copy the var in instance */
			if(j != _pst_Instance->pst_VarDes->ul_NbVars)
			{
				iNumElemEx = _pst_Instance->pst_VarDes->pst_VarsInfos[j].i_NumElem;
			}
			else
			{
				iNumElemEx = iNumElem;
			}

			/* Get size of var */
			ul_Size2 = pst_Model->pst_VarDes->pst_VarsInfos[i].w_Type;
			ul_Size2 = AI_gast_Types[ul_Size2].w_Size;
			if
			(
				(iNumElem >> 30 == iNumElemEx >> 30) &&
				L_memcmp
				(
					pst_ModelDes->pc_BufferInit + ul_Offset1 + (ul_SizeArr * sizeof(int)),
					_pst_Instance->pc_VarsBufferInit + ul_Offset1 + (ul_SizeArr * sizeof(int)),
					ul_Size2 * (iNumElem & 0x3FFFFFFF)
				)
			)
			{
				ul_NumDif++;
				ul_SizeInit += (ul_Size2 * (iNumElem & 0x3FFFFFFF));
				ul_SizeInit += ul_SizeArr * sizeof(int);
			}
		}
	}

    /* Allocate buffers for variables descriptions */
    ul_Size = ul_NumDif * sizeof(AI_tdst_VarInfo);
	ul_SizeR = ul_NumDif * 12;

    pst_VarsInfos = NULL;
    if(ul_Size)
        pst_VarsInfos = (AI_tdst_VarInfo *) MEM_p_Alloc(ul_Size);
    ul_Size1 = ul_NumDif * sizeof(AI_tdst_EditorVarInfo);

    pst_EditorVarsInfos = NULL;
    if(ul_Size1)
        pst_EditorVarsInfos = (AI_tdst_EditorVarInfo *) MEM_p_Alloc(ul_Size1);

    pc_Init = pc_Init1 = NULL;
    if(ul_SizeInit)
    {
        pc_Init = (char *) L_malloc(ul_SizeInit);
        pc_Init1 = (char *) MEM_p_Alloc(ul_SizeInit);
		L_memset(pc_Init, 0, sizeof(ul_SizeInit));
		L_memset(pc_Init1, 0, sizeof(ul_SizeInit));
    }

    /* Allocate a buffer to receive vardes for instance, and init it */
    pst_VarDes = (AI_tdst_VarDes *) MEM_p_Alloc(sizeof(AI_tdst_VarDes));
    pst_VarDes->pst_EditorVarsInfos = pst_EditorVarsInfos;
    pst_VarDes->ul_NbVarsInfos2 = 0;
    pst_VarDes->pst_EditorVarsInfos2 = NULL;
    pst_VarDes->pc_BufferInit = pc_Init1;
    pst_VarDes->ul_SizeBufferInit = ul_SizeInit;
    pst_VarDes->pst_VarsInfos = pst_VarsInfos;
    pst_VarDes->ul_NbVars = ul_NumDif;

    /* Init vars info */
    ul_Offset = 0;
    ul_NumDif = 0;
    pst_ModelDes = pst_Model->pst_VarDes;
	if(pst_ModelDes)
	{
		for(i = 0; i < pst_ModelDes->ul_NbVars; i++)
		{
			/* Skip array header */
			iNumElem = pst_ModelDes->pst_VarsInfos[i].i_NumElem;
			ul_SizeArr = (ULONG) iNumElem >> 30;

			/* Get size of var */
			ul_Size2 = pst_ModelDes->pst_VarsInfos[i].w_Type;
			ul_Size2 = AI_gast_Types[ul_Size2].w_Size;

			/* Search if var is present in instance */
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

			ul_Offset1 = pst_ModelDes->pst_VarsInfos[i].i_Offset;

			/* Verify that the number of elem is the same in model and instance.
			 * Else we won't copy the var in instance */
			if(j != _pst_Instance->pst_VarDes->ul_NbVars)
			{
				iNumElemEx = _pst_Instance->pst_VarDes->pst_VarsInfos[j].i_NumElem;
			}
			else
			{
				iNumElemEx = iNumElem;
			}

			if
			(
				(iNumElem >> 30 == iNumElemEx >> 30) &&
				L_memcmp
				(
					pst_ModelDes->pc_BufferInit + ul_Offset1 + (ul_SizeArr * sizeof(int)),
					_pst_Instance->pc_VarsBufferInit + ul_Offset1 + (ul_SizeArr * sizeof(int)),
					ul_Size2 * (iNumElem & 0x3FFFFFFF)
				)
			)
			{
				/* Init vars info struct of instance */
				pst_VarsInfos[ul_NumDif].w_Type = AI_gast_Types[pst_ModelDes->pst_VarsInfos[i].w_Type].
					w_ID;
				pst_VarsInfos[ul_NumDif].i_Offset = ul_Offset;
				pst_VarsInfos[ul_NumDif].i_NumElem = iNumElem;
				L_strcpy
				(
					pst_EditorVarsInfos[ul_NumDif].asz_Name,
					pst_ModelDes->pst_EditorVarsInfos[i].asz_Name
				);

				/* Array */
				i_Max = 1;
				i_Offset = 0;
				ul_Add = 0;
				if(ul_SizeArr)
				{
					i_Max = *(ULONG *) (_pst_Instance->pc_VarsBufferInit + ul_Offset1);
					for(j = 1; j < ul_SizeArr; j++)
						i_Max *= *(ULONG *) (_pst_Instance->pc_VarsBufferInit + ul_Offset1 + (j * 4));
					i_Offset = ul_SizeArr * sizeof(int);
					ul_Add += i_Offset;

					/* Copy size of each dim */
					L_memcpy
					(
						pc_Init + ul_Offset,
						_pst_Instance->pc_VarsBufferInit + ul_Offset1,
						ul_SizeArr * sizeof(int)
					);
					L_memcpy
					(
						pc_Init1 + ul_Offset,
						_pst_Instance->pc_VarsBufferInit + ul_Offset1,
						ul_SizeArr * sizeof(int)
					);
				}

				/* Transform references */
				while(i_Max--)
				{
					if((ULONG) i_Offset >= ul_SizeInit) 
					{
_Try_
						ERR_X_Assert(0);
_Catch_
_End_
						pc_Init = NULL;
						ul_Size = ul_Size1 = ul_SizeInit = 0;
						goto l_Err;
					}

					if(pst_VarsInfos[ul_NumDif].w_Type == TYPE_MESSAGE)
					{
						pmsg = (AI_tdst_Message *) (_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset);
						pmsg1 = (AI_tdst_Message *) (pc_Init + ul_Offset + i_Offset);
						L_memcpy(pmsg1, pmsg, sizeof(AI_tdst_Message));
						if(pmsg->pst_Sender) pmsg1->pst_Sender = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) pmsg->pst_Sender);
						if(pmsg->pst_GAO1) pmsg1->pst_GAO1 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) pmsg->pst_GAO1);
						if(pmsg->pst_GAO2) pmsg1->pst_GAO2 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) pmsg->pst_GAO2);
						if(pmsg->pst_GAO3) pmsg1->pst_GAO3 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) pmsg->pst_GAO3);
						if(pmsg->pst_GAO4) pmsg1->pst_GAO4 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) pmsg->pst_GAO4);
						if(pmsg->pst_GAO5) pmsg1->pst_GAO5 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) pmsg->pst_GAO5);
						L_memcpy
						(
							pc_Init1 + ul_Offset + i_Offset,
							_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset,
							ul_Size2
						);
					}
					else if(pst_VarsInfos[ul_NumDif].w_Type == TYPE_TRIGGER)
					{
						ptrg = (SCR_tt_Trigger *) (_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset);
						ptrg1 = (SCR_tt_Trigger *) (pc_Init + ul_Offset + i_Offset);
						L_memcpy(ptrg1, ptrg, sizeof(SCR_tt_Trigger));
						if(ptrg->t_Msg.pst_Sender) ptrg1->t_Msg.pst_Sender = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) ptrg->t_Msg.pst_Sender);
						if(ptrg->t_Msg.pst_GAO1) ptrg1->t_Msg.pst_GAO1 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) ptrg->t_Msg.pst_GAO1);
						if(ptrg->t_Msg.pst_GAO2) ptrg1->t_Msg.pst_GAO2 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) ptrg->t_Msg.pst_GAO2);
						if(ptrg->t_Msg.pst_GAO3) ptrg1->t_Msg.pst_GAO3 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) ptrg->t_Msg.pst_GAO3);
						if(ptrg->t_Msg.pst_GAO4) ptrg1->t_Msg.pst_GAO4 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) ptrg->t_Msg.pst_GAO4);
						if(ptrg->t_Msg.pst_GAO5) ptrg1->t_Msg.pst_GAO5 = (OBJ_tdst_GameObject *) LOA_ul_SearchKeyWithAddress((ULONG) ptrg->t_Msg.pst_GAO5);
						L_memcpy
						(
							pc_Init1 + ul_Offset + i_Offset,
							_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset,
							ul_Size2
						);
					}
					else if(AI_b_IsARef(pst_VarsInfos[ul_NumDif].w_Type))
					{
						pul_Key = (BIG_KEY *) (_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset);
						if(pst_VarsInfos[ul_NumDif].w_Type == TYPE_GAMEOBJECT)
						{
							if((*pul_Key == AI_C_MainActor0) || (*pul_Key == AI_C_MainActor1))
							{
								*(BIG_KEY *) (pc_Init + ul_Offset + i_Offset) = *pul_Key;
								goto zap;
							}
						}
						if(*pul_Key)
						{
							*(BIG_KEY *) (pc_Init + ul_Offset + i_Offset) = LOA_ul_SearchKeyWithAddress(*pul_Key);
							if((*pul_Key != NULL) && ((int) *pul_Key != -1))
							{
								if(*(BIG_KEY *) (pc_Init + ul_Offset + i_Offset) == BIG_C_InvalidKey)
								{
									snprintf
									(
										az, sizeof(az),
										"AI save of %s has converted a pointer to an unknown reference", 
										((OBJ_tdst_GameObject *) _pst_Instance->pst_GameObject)->sz_Name
									);
									ERR_X_Warning(0, az, NULL);
								}
							}
						}
						else
						{
							*(BIG_KEY *) (pc_Init + ul_Offset + i_Offset) = 0;
						}
zap:
						L_memcpy
						(
							pc_Init1 + ul_Offset + i_Offset,
							_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset,
							ul_Size2
						);
					}

					/* Else copy value without changing */
					else
					{
						L_memcpy
						(
							pc_Init + ul_Offset + i_Offset,
							_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset,
							ul_Size2
						);
						L_memcpy
						(
							pc_Init1 + ul_Offset + i_Offset,
							_pst_Instance->pc_VarsBufferInit + ul_Offset1 + i_Offset,
							ul_Size2
						);
					}

					i_Offset += ul_Size2;
					ul_Add += ul_Size2;
				}

				ul_Offset += ul_Add;
				ul_NumDif++;
			}
		}
	}
l_Err:
    /* Save var description */
    SAV_Buffer(&ul_SizeR, sizeof(int));
	{
		int kk;
		for(kk = 0; kk < ul_SizeR / 12; kk++)
		{
			SAV_Buffer(&pst_VarsInfos[kk].i_Offset, 4);
			SAV_Buffer(&pst_VarsInfos[kk].i_NumElem, 4);
			SAV_Buffer(&pst_VarsInfos[kk].w_Type, 2);
			SAV_Buffer(&pst_VarsInfos[kk].w_Flags, 2);
	//	    SAV_Buffer(pst_VarsInfos, ul_Size);
		}
	}

    SAV_Buffer(&ul_Size1, sizeof(int));
    SAV_Buffer(pst_EditorVarsInfos, ul_Size1);
    i = 0;
    SAV_Buffer(&i, sizeof(int));
    SAV_Buffer(&i, sizeof(int));
    SAV_Buffer(&ul_SizeInit, sizeof(int));

    SAV_Buffer(pc_Init, ul_SizeInit);
    if(pc_Init) L_free(pc_Init);

    /* Save initial functions (don't care for an instance, save dummy things ) */
    L_memset(aul_InitFct, 0, sizeof(aul_InitFct));
    SAV_Buffer(aul_InitFct, sizeof(LONG) * AI_C_MaxTracks);

    SAV_ul_End();

    /* Set buffers in instance */
    if(_pst_Instance->pst_VarDes)
        AI_FreeVarDes(_pst_Instance->pst_VarDes);

    /*$2
     -----------------------------------------------------------------------------------------------
        Set buffers in current instance
     -----------------------------------------------------------------------------------------------
     */

    _pst_Instance->pst_VarDes = pst_VarDes;

    /*$2
     -----------------------------------------------------------------------------------------------
        Save instance
     -----------------------------------------------------------------------------------------------
     */

    BIG_ComputeFullName(BIG_ParentFile(ul_CurrentInstance), asz_Path);
    SAV_Begin(asz_Path, BIG_NameFile(ul_CurrentInstance));

    /* Search engine model */
    BIG_ComputeFullName(BIG_ParentFile(_ul_CurrentModel), asz_Path);
    L_strcpy(asz_Name, BIG_NameFile(_ul_CurrentModel));
    psz_Temp = L_strrchr(asz_Name, '.');
    if(psz_Temp) *psz_Temp = 0;
    L_strcat(asz_Name, EDI_Csz_ExtAIEngineModel);
    ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);

    SAV_Buffer(&BIG_FileKey(ul_File), 4);

    /* Compute var file name and save key */
    BIG_ComputeFullName(BIG_ParentFile(ul_CurrentInstance), asz_Path);
    L_strcpy(asz_Name, BIG_NameFile(ul_CurrentInstance));
    psz_Temp = L_strrchr(asz_Name, '.');
    if(psz_Temp) *psz_Temp = 0;
    L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);
    ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);

    SAV_Buffer(&BIG_FileKey(ul_File), 4);

    SAV_ul_End();
}

#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
