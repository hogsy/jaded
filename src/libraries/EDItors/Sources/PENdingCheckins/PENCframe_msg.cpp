/*$T TEXframe_msg.cpp GC!1.62 12/27/99 12:50:04 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Res/Res.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "PENcframe.h"
#include "EDImsg.h"
#include "EDIicons.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDItors/Sources/BROwser/BROmsg.h"
#include "ENGine/Sources/ENGinit.h"

extern BOOL EDI_gb_OpVSS;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EPEN_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CPoint                      pt;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
		return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELDATA: 
		return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELFILE:
		return FALSE;	

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EDI_MESSAGE_FATHASCHANGED:
	case EDI_MESSAGE_REFRESH:
/*		if(EDI_gb_OpVSS) 
		{
			EDI_gb_OpVSS = FALSE;
			Refresh();
		}*/

		return FALSE;	
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
