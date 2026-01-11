/*$T VAVlist_ai.cpp GC!1.71 03/01/00 16:35:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "LINKs/LINKstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "LINKs/LINKstruct_reg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"

/*
 =======================================================================================================================
    Aim:    Macro to add a new item in the list
 =======================================================================================================================
 */
#define A(a, b, c, d, e, f, g, h, i, p4, p5, p6, p7, fil, help) \
    if(c) \
    { \
        pos = mpo_ListItems->InsertAfter \
            ( \
                pos, \
                po_NewItem = new EVAV_cl_ViewItem(a, b, c, d, e, f, g, h, i, p4, p5, p6, p7, fil) \
            ); \
        i_NumFields++; \
        po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data; \
        po_NewItem->psz_Help = help; \
    }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::AIAddInstance(POSITION pos, AI_tdst_Instance *_pst_Instance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG               i, j, i_Type;
    AI_tdst_VarDes      *pst_VarDes;
    EVAV_cl_ViewItem    *po_Item, *po_NewItem;
    EVAV_tdpfnv_Change  pfn_CB;
    char                *pc_Init, *pc_Buf;
    CString             o_NameInit, o_Name;
    char                *psz_NameIn, *psz_Name;
    int                 i_NumFields;
    COLORREF            x_Col, x_Col1;
    char                asz_Track[50];
    int                 iPar4, iPar5, iPar6, iPar7;
    ULONG               ulF, ulTreat;
    int					iTreat;
    AI_tdst_VarInfo     *pst_LastVar, *pst_Better;
    int                 iFilter;
    char                *psz_Help;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Colors */
    x_Col = 0x008EB48B;     /* Init field */
    x_Col1 = 0x00B48E8B;    /* Tracks */

    pst_VarDes = _pst_Instance->pst_Model->pst_VarDes;

	/* Anti-Bug */
	if(!pst_VarDes) return;

    /* Get the pointer of instance */
    if(!pos)
        po_Item = mpo_ListItems->GetTail();
    else
        po_Item = mpo_ListItems->GetAt(pos);

    /* Init all variables */
    pfn_CB = LINK_gast_StructTypes[LINK_C_AI_Instance].pfn_CB;
    i_NumFields = 0;

    pst_Better = NULL;
    iTreat = -1;
    ulTreat = 0;
    while(ulTreat != pst_VarDes->ul_NbVars)
    {
        /* To sort vars */
        pst_LastVar = NULL;
        for(i = 0; i < pst_VarDes->ul_NbVars; i++)
        {
            if(pst_LastVar == &pst_VarDes->pst_VarsInfos[i])
                continue;
            if(pst_Better == &pst_VarDes->pst_VarsInfos[i])
                continue;
            if(!pst_Better) goto force;

            if(pst_VarDes->pst_VarsInfos[i].i_Offset > pst_Better->i_Offset)
            {
force:
                if((pst_LastVar == NULL) || (pst_VarDes->pst_VarsInfos[i].i_Offset < pst_LastVar->i_Offset))
                {
                    pst_LastVar = &pst_VarDes->pst_VarsInfos[i];
                    iTreat = (int) i;
                    continue;
                }
            }
        }

        pst_Better = pst_LastVar;
        ulTreat++;
        i = iTreat;

        if(pos == NULL) pos = mpo_ListItems->GetTailPosition();

        /* Get infos about var */
        o_NameInit = pst_VarDes->pst_EditorVarsInfos[i].asz_Name + CString(" (Init)");
        psz_NameIn = (char *) (LPCSTR) o_NameInit;
        pc_Init = _pst_Instance->pc_VarsBufferInit + pst_VarDes->pst_VarsInfos[i].i_Offset;

        pc_Buf = NULL;
        if(_pst_Instance->pc_VarsBuffer)
        {
            o_Name = pst_VarDes->pst_EditorVarsInfos[i].asz_Name;
            psz_Name = (char *) (LPCSTR) o_Name;
            pc_Buf = _pst_Instance->pc_VarsBuffer + pst_VarDes->pst_VarsInfos[i].i_Offset;
        }

        /* Treat the type */
        i_Type = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_ID;
        iPar4 = iPar5 = iPar6 = iPar7 = 0;
        ulF = 0;

        /* Is it an array ? */
        if(pst_VarDes->pst_VarsInfos[i].i_NumElem >> 30)
        {
            ulF = EVAV_Array;
            iPar4 = (ULONG) pst_VarDes->pst_VarsInfos[i].i_NumElem >> 30;
            iPar5 = ((int *) pc_Init)[0];
            if(iPar4 > 1) iPar6 = ((int *) pc_Init)[1];
            if(iPar4 > 2) iPar7 = ((int *) pc_Init)[2];
        }

        /* Filters */
        iFilter = 0;
        if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Private)
            iFilter |= EVAV_Filter_Private;

/*$off*/

        psz_Help = NULL;

        /* 
		 *-----------------------------------------------------------------------------------------
		 * Is there a separator ? 
		 *-----------------------------------------------------------------------------------------
		 */
        for(j = 0; j < pst_VarDes->ul_NbVarsInfos2; j++)
        {
            if
            (
                (pst_VarDes->pst_EditorVarsInfos2[j].i_Offset == pst_VarDes->pst_VarsInfos[i].i_Offset)
                && (pst_VarDes->pst_EditorVarsInfos2[j].uw_Flags & AI_CVF_Sep)
                && (pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst)
            )
            {
                A(pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst, EVAV_EVVIT_CustSep, pc_Init, 0, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), 0, 0, 0, 0, EVAV_Filter_Sep, psz_Help);
            }
            if
            (
                (pst_VarDes->pst_EditorVarsInfos2[j].i_Offset == pst_VarDes->pst_VarsInfos[i].i_Offset)
                && (pst_VarDes->pst_EditorVarsInfos2[j].uw_Flags & AI_CVF_Help)
            )
            {
                psz_Help = pst_VarDes->pst_EditorVarsInfos2[j].psz_StringHelp;
            }
        }

        /*
		 *-----------------------------------------------------------------------------------------
		 *	An enum ? 
		 *-----------------------------------------------------------------------------------------
		 */
        if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Enum)
        {
            for(j = 0; j < pst_VarDes->ul_NbVarsInfos2; j++)
            {
                if
                (
                    (pst_VarDes->pst_EditorVarsInfos2[j].i_Offset == pst_VarDes->pst_VarsInfos[i].i_Offset)
                    && (pst_VarDes->pst_EditorVarsInfos2[j].uw_Flags & AI_CVF_Enum)
                )
                {
                    break;
                }
            }
            ERR_X_Assert(j != pst_VarDes->ul_NbVarsInfos2);
        }

        /*
		 *-----------------------------------------------------------------------------------------
		 * All possible AI types 
		 *-----------------------------------------------------------------------------------------
		 */
        switch(i_Type)
        {
        case TYPE_MESSAGE:
            A(psz_NameIn, EVAV_EVVIT_SubStruct, pc_Init, ulF, LINK_C_MSG_Struct, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_SubStruct, pc_Buf, ulF, LINK_C_MSG_Struct, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
			break;

        case TYPE_TRIGGER:
            A(psz_NameIn, EVAV_EVVIT_Trigger, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_Trigger, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
			break;

        case TYPE_BOOL:
            A(psz_NameIn, EVAV_EVVIT_Bool, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_Bool, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_KEY:
            A(psz_NameIn, EVAV_EVVIT_Key, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_Key, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_COLOR:
            A(psz_NameIn, EVAV_EVVIT_Color, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_Color, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_INT:
            if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Enum)
            {
		        if(pst_VarDes->pst_VarsInfos[i].i_NumElem >> 30)
				{
		            A(psz_NameIn, EVAV_EVVIT_ConstInt, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
					po_NewItem->mi_Param8 = (int) pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst;
	                A(psz_Name, EVAV_EVVIT_ConstInt, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
					po_NewItem->mi_Param8 = (int) pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst;
				}
				else
				{
	                A(psz_NameIn, EVAV_EVVIT_ConstInt, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, (int) pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
		            A(psz_Name, EVAV_EVVIT_ConstInt, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), (int) pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
				}
            }
            else
            {
                A(psz_NameIn, EVAV_EVVIT_Int, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
                A(psz_Name, EVAV_EVVIT_Int, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            }
            break;

        case TYPE_FLOAT:
            if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Enum)
            {
                A(psz_NameIn, EVAV_EVVIT_ConstFloat, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, (int) pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
                A(psz_Name, EVAV_EVVIT_ConstFloat, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), (int) pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            }
            else
            {
                A(psz_NameIn, EVAV_EVVIT_Float, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
                A(psz_Name, EVAV_EVVIT_Float, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            }
            break;

        case TYPE_STRING:
            A(psz_NameIn, EVAV_EVVIT_String, *(char **) pc_Init, ulF | EVAV_ReadOnly, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_String, *(char **) pc_Buf, ulF | EVAV_ReadOnly, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_TEXT:
            A(psz_NameIn, EVAV_EVVIT_Text, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_Text, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_VECTOR:
            A(psz_NameIn, EVAV_EVVIT_Vector, pc_Init, ulF, 0, 0, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_Vector, pc_Buf, ulF, 0, 0, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_FUNCTIONREF:
            A(psz_NameIn, EVAV_EVVIT_AIFunction, pc_Init, ulF, (int) _pst_Instance, -1, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_AIFunction, pc_Buf, ulF, (int) _pst_Instance, -1, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_GAMEOBJECT:
            A(psz_NameIn, EVAV_EVVIT_GO, pc_Init, ulF, 0, -1, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_GO, pc_Buf, ulF, 0, -1, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;

        case TYPE_NETWORK:
            A(psz_NameIn, EVAV_EVVIT_NetWork, pc_Init, ulF, 0, -1, 0, pfn_CB, x_Col, iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Init | iFilter, psz_Help);
            A(psz_Name, EVAV_EVVIT_NetWork, pc_Buf, ulF, 0, -1, 0, pfn_CB, GetSysColor(COLOR_WINDOW), iPar4, iPar5, iPar6, iPar7, EVAV_Filter_Cur | iFilter, psz_Help);
            break;
        }
    /*$on*/
    }

    /* Ajout des tracks */
    for(i = 0; i < AI_C_MaxTracks; i++)
    {
        i_NumFields++;
        sprintf(asz_Track, "Track %d", i);
        pos = mpo_ListItems->InsertAfter
            (
                pos,
                new EVAV_cl_ViewItem
                    (
                        asz_Track,
                        EVAV_EVVIT_AIFunction,
                        &_pst_Instance->apst_CurrentFct[i],
                        EVAV_None,
                        (int) _pst_Instance,
                        i,
                        0,
                        pfn_CB,
                        x_Col1,
                        0,
                        0,
                        0,
                        0,
                        EVAV_Filter_Track
                    )
            );
    }

    /* Set number of fields of the initial pointer */
    po_Item->mi_NumFields = i_NumFields;
}

