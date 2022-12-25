/*$T MENframe_msg.cpp GC!1.34 05/10/99 17:34:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "MENframe.h"
#include "EDImsg.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
int EMEN_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_tdst_DragDrop   *pst_DragDrop;
    EMEN_cl_FrameIn     *po_Source;
    EMEN_cl_Frame       *po_Parent;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EDI_MESSAGE_LOCKGMENU:		mb_ChangePane = TRUE; break;
	case EDI_MESSAGE_UNLOCKGMENU:	mb_ChangePane = FALSE; break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_FATHASCHANGED:
        Refresh();
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELFILE:
    case EDI_MESSAGE_CANSELDATA:
		return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if(pst_DragDrop->i_Type == EDI_DD_Long) return FALSE;

        if(pst_DragDrop->po_SourceEditor->mi_PosInGArray != mi_PosInGArray) return FALSE;
        po_Parent = (EMEN_cl_Frame *) pst_DragDrop->i_Param2;
        if(po_Parent == this) return FALSE;

        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;

        if(pst_DragDrop->po_SourceEditor->mi_PosInGArray == mi_PosInGArray)
        {
            po_Source = (EMEN_cl_FrameIn *) pst_DragDrop->i_Param1;
            MoveMenu(po_Source, this);
        }

        break;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
