/*$T VAVlist_action.cpp GC!1.63 12/27/99 16:55:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"

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
void EVAV_cl_ListBox::ACTAddAction(POSITION pos, void *_p_Instance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item, *po_NewItem;
    EVAV_tdpfnv_Change  pfn_CB;
    int                 i_NumFields;
    ACT_st_Action       *pst_Action;
    int                 i;
    char                az_Text[32];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get the pointer of instance */
    po_Item = pos ? mpo_ListItems->GetAt(pos) : mpo_ListItems->GetTail();

    /* Init all variables */
    pfn_CB = LINK_gast_StructTypes[LINK_C_ENG_Action].pfn_CB;
    i_NumFields = 0;

    pst_Action = (ACT_st_Action *) _p_Instance;

    /*$off*/
    A("Number of animations", EVAV_EVVIT_Int, &pst_Action->uc_NumberOfActionItem, 0, 0, ACT_C_SizeOfAction, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, (int) pst_Action, 0, 0);
    A("Loop begins at animation #", EVAV_EVVIT_Int, &pst_Action->uc_ActionItemNumberForLoop, 0, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, (int) pst_Action, 0, 0);
    /*$on*/
    i_NumFields = 2;

    for(i = 0; i < pst_Action->uc_NumberOfActionItem; i++)
    {
        sprintf(az_Text, "Animation %d", i);
        /*$off*/
        A(az_Text, EVAV_EVVIT_Pointer, &pst_Action->ast_ActionItem[i], 0, LINK_C_ENG_ActionItem, 0, 0, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
        /*$on*/
    }

    /* Set number of fields of the initial pointer */
    po_Item->mi_NumFields = i_NumFields;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::ACTAddActionKit(POSITION pos, void *_p_Instance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item, *po_NewItem;
    EVAV_tdpfnv_Change  pfn_CB;
    int                 i_NumFields;
    ACT_st_ActionKit    *pst_ActionKit;
    int                 i;
    char                az_Text[32];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get the pointer of instance */
    po_Item = pos ? mpo_ListItems->GetAt(pos) : mpo_ListItems->GetTail();

    /* Init all variables */
    pfn_CB = LINK_gast_StructTypes[LINK_C_ENG_ActionKit].pfn_CB;

    pst_ActionKit = (ACT_st_ActionKit *) _p_Instance;

    /*$off*/
    i_NumFields = 0;
    A("Number of actions", EVAV_EVVIT_Int, &pst_ActionKit->uw_NumberOfAction, 0, 0, ACT_C_SizeOfActionKit, 2, pfn_CB, po_Item->mx_Color, 0, 0, 0, (int) pst_ActionKit, 0, 0);
    /*$on*/

    /*$off*/
    for(i = 0; i < pst_ActionKit->uw_NumberOfAction; i++)
    {
        sprintf(az_Text, "Action %d", i);
		A(az_Text, EVAV_EVVIT_Pointer, pst_ActionKit->apst_Action[i], 0, LINK_C_ENG_Action, 0, 0, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
    }
    /*$on*/

    /* Set number of fields of the initial pointer */
    po_Item->mi_NumFields = i_NumFields;
}

