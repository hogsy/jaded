/*$T VAVlist_msg.cpp GC!1.41 08/05/99 10:09:33 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AImsg.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_ListBox::MsgAddList(POSITION pos, AI_tdst_MessageLiFo *_pst_MsgLiFo)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i_NumFields;
    char                asz_Temp[50];
    EVAV_cl_ViewItem    *po_Item, *po_NewItem;
	AI_tdst_MessageList	*pst_Msg;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get pointer */
    if(!pos)
        po_Item = mpo_ListItems->GetTail();
    else
        po_Item = mpo_ListItems->GetAt(pos);

    /* Number of messages */
    i_NumFields = 1;
    pos = mpo_ListItems->InsertAfter
        (
            pos,
            po_NewItem = new EVAV_cl_ViewItem
                (
                    "Number",
                    EVAV_EVVIT_Int,
                    &_pst_MsgLiFo->i_NumMessages,
                    EVAV_None,
                    0,
                    0,
                    0,
                    NULL,
                    po_Item->mx_Color,
                    1
                )
        );
    po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

    /* All messages */
	pst_Msg = _pst_MsgLiFo->pst_First;
	while(pst_Msg)
    {
        sprintf(asz_Temp, "Message %d", pst_Msg->msg.i_Id);
        pos = mpo_ListItems->InsertAfter
            (
                pos,
                po_NewItem = new EVAV_cl_ViewItem
                    (
                        asz_Temp,
                        EVAV_EVVIT_SubStruct,
                        &pst_Msg->msg,
                        EVAV_None,
                        LINK_C_MSG_Struct,
                        0,
                        0,
                        NULL,
                        po_Item->mx_Color,
                        1
                    )
            );
        po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;
		i_NumFields++;
		pst_Msg = pst_Msg->pst_Next;
    }

    /* Set number of fields of the initial pointer */
    po_Item->mi_NumFields = i_NumFields;
}

