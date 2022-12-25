/*$T SONmsg.h GC 1.138 04/05/04 14:04:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#include "EDIeditors_infos.h"

#define EPER_MESSAGE_REFRESH				EDI_MSG_PER + 0
#define EPER_MESSAGE_AUTOCHECKOUT			EDI_MSG_PER + 2
#define EPER_MESSAGE_EDIT_SELECTED			EDI_MSG_PER + 3
#define EPER_MESSAGE_SYNC_SELECTED			EDI_MSG_PER + 4
#define EPER_MESSAGE_FORCESYNC_SELECTED		EDI_MSG_PER + 10
#define EPER_MESSAGE_REVERT_SELECTED		EDI_MSG_PER + 5
#define EPER_MESSAGE_ADD_SELECTED			EDI_MSG_PER + 6
#define EPER_MESSAGE_DELETE_SELECTED		EDI_MSG_PER + 7
#define EPER_MESSAGE_SHOWHISTORY_SELECTED	EDI_MSG_PER + 8
#define EPER_MESSAGE_DIFF_SELECTED			EDI_MSG_PER + 9
#define EPER_MESSAGE_FLUSH_SELECTED			EDI_MSG_PER + 11
#define EPER_MESSAGE_FLUSH_DIR				EDI_MSG_PER + 12
#define EPER_MESSAGE_SERVER_SYNC_SELECTED	EDI_MSG_PER + 13
#define EPER_MESSAGE_SUBMIT_SELECTED		EDI_MSG_PER + 14
#define EPER_MESSAGE_SYNCDELETE_SELECTED	EDI_MSG_PER + 15
#define EPER_MESSAGE_DIFFDIR_SELECTED		EDI_MSG_PER + 16  // Message sent to PERCheck frame to diff a directory
#define EPER_MESSAGE_SUBMITEDIT_SELECTED	EDI_MSG_PER + 17

enum EControlType
{
	eTREECTRL,
	eTREECTRL_NOCHILD,
	ePER_CDATACTRL,
	eLISTCTRL,
	eLISTCTRLSTYLED,
	eBIG_INDEX
};