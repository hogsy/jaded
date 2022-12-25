/*$T OBJload.h GC 1.139 04/15/04 12:06:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __OBJLOAD_H__
#define __OBJLOAD_H__

#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4-******************************************************************************************************************/

extern OBJ_tdst_GameObject	*OBJ_gpst_ObjectToLoadIn;
extern ULONG				OBJ_ul_GameObjectCallback(ULONG _ul_PosFile);
extern ULONG				OBJ_ul_GameObjectCallbackWithoutHierarchy(ULONG _ul_PosFile);
extern void					OBJ_UpdateGaoGao(struct WOR_tdst_World_ *, OBJ_tdst_GameObject *, BIG_KEY, ULONG);

/*$4-******************************************************************************************************************/

#define UPDATE_ALL				0xFFFFFFFF
#define UPDATE_MATERIAL			0x00000001
#define UPDATE_GEOMETRY			0x00000002
#define UPDATE_SOUND			0x00000004
#define UPDATE_MODIFIERS		0x00000008
#define UPDATE_COLMAP			0x00000010
#define UPDATE_COLSET			0x00000020
#define UPDATE_DESIGNSTRUCT		0x00000040
#define UPDATE_CONTROLFLAGS		0x00000080
#define UPDATE_CUSTOMBITS		0x00000100
#define UPDATE_SKELETONANDSHAPE	0x00000200
#define UPDATE_ADDITIONALMATRIX 0x00000400
#define UPDATE_AIMODEL			0x00000800
#define UPDATE_HIERARCHY		0x00000800
#define UPDATE_EVENTS			0x00001000
#define UPDATE_ODE				0x00002000
#define UPDATE_TYPEFLAGS		0x00004000
#define UPDATE_FIXFLAGS			0x00008000
#define UPDATE_EXTRALAGS		0x00010000
#define UPDATE_BV				0x00020000
#define UPDATE_FORCE_BV			0x00040000


/*$4-******************************************************************************************************************/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __OBJLOAD_H__ */
