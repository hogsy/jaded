/*$T EDImsg.h GC!1.39 06/22/99 15:54:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#include "EDIeditors_infos.h"

#define EDI_MESSAGE_FATHASCHANGED       EDI_MSG_EDI + 0x03
#define EDI_MESSAGE_DATAHASCHANGED      EDI_MSG_EDI + 0x04
#define EDI_MESSAGE_DBLCLKFILE          EDI_MSG_EDI + 0x05
#define EDI_MESSAGE_SELDIR              EDI_MSG_EDI + 0x06
#define EDI_MESSAGE_SELFILE             EDI_MSG_EDI + 0x07
#define EDI_MESSAGE_CANSELFILE          EDI_MSG_EDI + 0x08
#define EDI_MESSAGE_CANSELDIR           EDI_MSG_EDI + 0x09

#define EDI_MESSAGE_CANDRAGDROP         EDI_MSG_EDI + 0x10
#define EDI_MESSAGE_ENDDRAGDROP         EDI_MSG_EDI + 0x11
#define EDI_MESSAGE_CANCELDRAGDROP      EDI_MSG_EDI + 0x12
#define EDI_MESSAGE_GAINFOCUSDRAGDROP   EDI_MSG_EDI + 0x13
#define EDI_MESSAGE_LOOSEFOCUSDRAGDROP  EDI_MSG_EDI + 0x14
#define EDI_MESSAGE_SELDATA             EDI_MSG_EDI + 0x15
#define EDI_MESSAGE_CANSELDATA			EDI_MSG_EDI + 0x16
#define EDI_MESSAGE_REFRESH				EDI_MSG_EDI + 0x17
#define EDI_MESSAGE_SELDIRDATA          EDI_MSG_EDI + 0x18
#define EDI_MESSAGE_CANSELDIRDATA		EDI_MSG_EDI + 0x19
#define EDI_MESSAGE_REFRESHMENU			EDI_MSG_EDI + 0x1a
#define EDI_MESSAGE_ADDSELDATA          EDI_MSG_EDI + 0x1b
#define EDI_MESSAGE_DELSELDATA          EDI_MSG_EDI + 0x1c
#define EDI_MESSAGE_REFRESHDLG			EDI_MSG_EDI + 0x1d
#define EDI_MESSAGE_LOCKGMENU			EDI_MSG_EDI + 0x1e
#define EDI_MESSAGE_UNLOCKGMENU			EDI_MSG_EDI + 0x1f

#define EDI_MESSAGE_HASBEENLINKED       EDI_MSG_EDI + 0x20
#define EDI_MESSAGE_HASBEENUNLINKED     EDI_MSG_EDI + 0x21

#ifdef JADEFUSION
// Used by SELection GRID to refresh the data stored in the MATerial Editor
#define EDI_MESSAGE_REFRESHDATA			EDI_MSG_EDI + 0x22
#endif
