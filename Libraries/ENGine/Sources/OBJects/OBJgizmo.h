/*$T OBJgizmo.h GC! 1.075 03/07/00 17:04:22 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __OBJGIZMO_H__
#define __OBJGIZMO_H__

#include "OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void OBJ_Gizmo_UpdateGizmoPtr(OBJ_tdst_GameObject *);

#ifdef ACTIVE_EDITORS
extern void OBJ_Gizmo_AddEditObject(OBJ_tdst_GameObject *);
extern void OBJ_Gizmo_DelEditObject(OBJ_tdst_GameObject *);
extern void OBJ_Gizmo_Update(OBJ_tdst_GameObject *);
extern void OBJ_Gizmo_Update_LOAD(OBJ_tdst_GameObject *);
extern void OBJ_Gizmo_ForceEditionPos(OBJ_tdst_GameObject *);
extern void OBJ_Gizmo_ChangeNumberOfAdditionalMatrix(OBJ_tdst_AdditionalMatrix *,ULONG );
//extern void OBJ_Gizmo_ChangeNumberOfAdditionalMatrix(OBJ_tdst_AdditionalMatrix *,ULONG );//POPOWARNING 2 functions

#else
#define OBJ_Gizmo_AddEditObject(a)
#define OBJ_Gizmo_DelEditObject(a)
#define OBJ_Gizmo_Update(a)
#define OBJ_Gizmo_ForceEditionPos(a)
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __OBJGIZMO_H__ */
