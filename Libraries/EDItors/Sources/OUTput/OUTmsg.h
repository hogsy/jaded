/*$T OUTmsg.h GC! 1.086 07/06/00 17:57:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#include "EDIeditors_infos.h"

#define EOUT_MESSAGE_CREATEWORLD					EDI_MSG_OUTPUT + 0x01
#define EOUT_MESSAGE_DESTROYWORLD					EDI_MSG_OUTPUT + 0x02
#define EOUT_MESSAGE_SAVEWORLD						EDI_MSG_OUTPUT + 0x03

#define EOUT_MESSAGE_CURVE_CHANGEINTERPERSECONDS	EDI_MSG_OUTPUT + 0x20

#define EOUT_MESSAGE_SUBOBJECT_PAINTSEL				EDI_MSG_OUTPUT + 0x30
#define EOUT_MESSAGE_SUBOBJECT_FLIPNORMALS			EDI_MSG_OUTPUT + 0x33
#define EOUT_MESSAGE_SUBOBJECT_CHANGEID				EDI_MSG_OUTPUT + 0x34
#define EOUT_MESSAGE_SUBOBJECT_EUPDATEVSEL          EDI_MSG_OUTPUT + 0x35
#define EOUT_MESSAGE_SUBOBJECT_FUPDATEVSEL          EDI_MSG_OUTPUT + 0x36
#define EOUT_MESSAGE_SUBOBJECT_FSELEXT              EDI_MSG_OUTPUT + 0x37
#define EOUT_MESSAGE_SUBOBJECT_FDETACH              EDI_MSG_OUTPUT + 0x38
#define EOUT_MESSAGE_SUBOBJECT_PIVOTCENTER          EDI_MSG_OUTPUT + 0x39

#define EOUT_MESSAGE_ANIMADDCURVE					EDI_MSG_OUTPUT + 0x40
#define EOUT_MESSAGE_ANIMDELCURVE					EDI_MSG_OUTPUT + 0x41
#define EOUT_MESSAGE_ANIMREPLACECURVE				EDI_MSG_OUTPUT + 0x42
