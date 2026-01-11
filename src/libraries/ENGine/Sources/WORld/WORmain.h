/*$T WORmain.h GC! 1.081 07/26/01 09:44:21 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Main functions of the world module */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __WORMAIN_H__
#define __WORMAIN_H__

#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions related to the world
 ***********************************************************************************************************************
 */

void						WOR_World_ActivateObjects(WOR_tdst_World *);
void						WOR_World_MakeObjectsVisible(WOR_tdst_World *);

void						WOR_World_AddLoadedObject(WOR_tdst_World *, OBJ_tdst_GameObject *, ULONG);
int							WOR_i_GetAllChildsOf(WOR_tdst_World *, OBJ_tdst_GameObject *, OBJ_tdst_GameObject **, BOOL);
struct OBJ_tdst_GameObject_ *WOR_pst_GetSymetric(struct OBJ_tdst_GameObject_ *);

#ifdef ACTIVE_EDITORS
struct OBJ_tdst_GameObject_ *WOR_pst_GetBoneFromGizmo(struct OBJ_tdst_GameObject_ *);
struct OBJ_tdst_GameObject_ *WOR_pst_GetGizmoFromBone(struct OBJ_tdst_GameObject_ *);
struct OBJ_tdst_GameObject_ *WOR_pst_GetGizmoSymetric(struct OBJ_tdst_GameObject_ *);
#endif
void						WOR_World_AttachObject(WOR_tdst_World *, OBJ_tdst_GameObject *);
void						WOR_World_DetachObject(WOR_tdst_World *, OBJ_tdst_GameObject *);

void						WOR_AddDelObj(WOR_tdst_World *, OBJ_tdst_GameObject *, void *);
void						WOR_DelObjData(WOR_tdst_World *, OBJ_tdst_GameObject *, void *);
void						WOR_DelObj(WOR_tdst_World *, OBJ_tdst_GameObject *);

void						WOR_ActivateObjectIfFlagsAllow(OBJ_tdst_GameObject *, WOR_tdst_World *);
void						WOR_MakeObjectVisibleIfFlagsAllow(OBJ_tdst_GameObject *, WOR_tdst_World *);
void						WOR_RecomputeAllChilds(OBJ_tdst_GameObject *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WORMAIN_H__ */
