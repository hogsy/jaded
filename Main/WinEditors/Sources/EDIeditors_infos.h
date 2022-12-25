/*$T EDIeditors_infos.h GC! 1.081 06/23/00 09:57:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once

/*
 * You must define one constant per editor. All specific messages must not
 * overflow another editor.
 */
#define EDI_MSG_EDI		0x00000000
#define EDI_MSG_OUTPUT	0x00000100
#define EDI_MSG_BROWSER 0x00000200
#define EDI_MSG_LOGFILE 0x00000300
#define EDI_MSG_AI		0x00000400
#define EDI_MSG_EVE		0x00000500
#define EDI_MSG_SND		0x00000600
#define EDI_MSG_PER		0x00000700


/* You must define one constant per editor. This is the ID. */
#define EDI_IDEDIT_OUTPUT		0x00000001
#define EDI_IDEDIT_BROWSER		0x00000002
#define EDI_IDEDIT_TEXTURE		0x00000004
#define EDI_IDEDIT_PROPERTIES	0x00000008
#define EDI_IDEDIT_RASTERS		0x00000010
#define EDI_IDEDIT_LOGFILE		0x00000020
#define EDI_IDEDIT_AI			0x00000040
#define EDI_IDEDIT_MENU			0x00000080
#define EDI_IDEDIT_COMUNIK		0x00000100
#define EDI_IDEDIT_MAT			0x00000200
#define EDI_IDEDIT_TEXT			0x00000800
#define EDI_IDEDIT_GRO			0x00001000
#define EDI_IDEDIT_SOUND		0x00002000
#define EDI_IDEDIT_ACTIONS		0x00004000
#define EDI_IDEDIT_EVENTS		0x00008000
#define EDI_IDEDIT_PREFAB		0x00010000
#define EDI_IDEDIT_PENC			0x00020000
#define EDI_IDEDIT_PERF			0x00080000
#define EDI_IDEDIT_PERFCHECK	0x00100000
#define EDI_IDEDIT_SELECTION	0x00200000

