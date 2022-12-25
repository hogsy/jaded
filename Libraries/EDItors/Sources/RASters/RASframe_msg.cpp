/*$T RASframe_msg.cpp GC!1.71 01/19/00 11:30:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImsg.h"
#include "RASframe.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ERAS_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELFILE:
    case EDI_MESSAGE_CANSELDATA:
    case EDI_MESSAGE_CANDRAGDROP:
        return FALSE;

    case EDI_MESSAGE_REFRESH:
        OnRefresh();
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_DATAHASCHANGED:
        FillComboCategory();
        FillComboSubCategory();
        FillComboName();
        mpo_ScrollView->Invalidate();
        return TRUE;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
