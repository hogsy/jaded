/*$T TEXTframe_msg.cpp GC! 1.098 12/11/00 10:02:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "TEXTframe.h"
#include "EDImsg.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLImem.h"
#include "LINKs/LINKmsg.h"
#include "EDIapp.h"
#include "EDIpaths.h"

extern CString	DD_GBContent;
extern CString	DD_GBID;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_String;
	EDI_tdst_DragDrop	*pst_DragDrop;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Msg)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANSELDATA:
		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		if((pst_DragDrop->i_Type == EDI_DD_Long) && (pst_DragDrop->i_Param2 == EVAV_EVVIT_Text))
		{
			return FALSE;//TODO return mul_CurrentIds != BIG_C_InvalidIndex ? TRUE : FALSE;
		}

		if(pst_DragDrop->i_Type == EDI_DD_Long) return FALSE;
		_ul_Param2 = pst_DragDrop->ul_FatFile;

	case EDI_MESSAGE_CANSELFILE:
		if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtTextFile)) return TRUE;
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtTextLang)) return TRUE;
		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_ENDDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		if((pst_DragDrop->i_Type == EDI_DD_Long) && (pst_DragDrop->i_Param2 == EVAV_EVVIT_Text))
		{
            //TODO
			break;
		}

		_ul_Param2 = pst_DragDrop->ul_FatFile;

	case EDI_MESSAGE_SELFILE:
		M_MF()->AddHistoryFile(this, BIG_FileKey(_ul_Param2));
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtTextLang)) 
            OnBankOpen(_ul_Param2);
		break;
	}

	return TRUE;
}

#endif /* ACTIVE_EDITORS */
