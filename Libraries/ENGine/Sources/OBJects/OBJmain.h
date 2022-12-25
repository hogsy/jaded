/*$T OBJmain.h GC! 1.077 03/13/00 13:49:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __OBJMAIN_H__
#define __OBJMAIN_H__

#include "ENGine/Sources/OBJects/OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void OBJ_ChangeIdentityFlags(struct OBJ_tdst_GameObject_ *, ULONG, ULONG);
extern void OBJ_ChangeControlFlags(struct OBJ_tdst_GameObject_ *, ULONG, ULONG);
extern void OBJ_UpdateStatusFlagRTL(struct OBJ_tdst_GameObject_ *);
extern BOOL OBJ_b_HasBeenProcess(struct OBJ_tdst_GameObject_ *, ULONG, ULONG);
extern void OBJ_SetProcess(struct OBJ_tdst_GameObject_ *, ULONG, ULONG);
extern void OBJ_ResetProcess(struct OBJ_tdst_GameObject_ *, ULONG, ULONG);
extern void OBJ_HierarchyMainCall(struct WOR_tdst_World_ *);
extern void	OBJ_ChangeBoneFather(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *);
#ifdef JADEFUSION
extern void OBJ_ChangeExtraFlags(OBJ_tdst_Extended * _pst_Extended, USHORT _uw_Flags, USHORT _uw_OldFlags);
#endif
#ifdef ACTIVE_EDITORS
extern void OBJ_WhenDestroyAnObject(struct WOR_tdst_World_ *, struct OBJ_tdst_GameObject_ *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
