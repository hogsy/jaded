/*$T SONmsg.h GC 1.138 04/05/04 14:04:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#include "EDIeditors_infos.h"

#define ESON_MESSAGE_REFRESH_SMODIFIER_VIEW		EDI_MSG_SND + 0

#define ESON_MESSAGE_AUTOVOLUMEOFF				EDI_MSG_SND + 2
#define ESON_MESSAGE_CREATEASSSMD				EDI_MSG_SND + 3
#define ESON_MESSAGE_REFRESH_SMODIFIER_TITLE	EDI_MSG_SND + 4
#define ESON_MESSAGE_REFRESH_BANK				EDI_MSG_SND + 5
#define ESON_MESSAGE_REINITENGINE				EDI_MSG_SND + 8
#define ESON_MESSAGE_GENERATE_MTX				EDI_MSG_SND + 9
#define ESON_MESSAGE_GAO_HAS_CHANGED			EDI_MSG_SND + 10
#define ESON_MESSAGE_UPDATERASTER				EDI_MSG_SND + 12
