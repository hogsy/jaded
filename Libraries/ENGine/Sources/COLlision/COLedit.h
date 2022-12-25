/*$T COLedit.h GC!1.71 01/11/00 18:00:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __COL_EDIT__
#define __COL_EDIT__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplcus */

/* GC Dependencies 11/22/99 */
#include "BASe/BAStypes.h"

#ifdef ACTIVE_EDITORS
void	COL_UpdateCameraNumberOfFaces(struct COL_tdst_Cob_ *);
void    COL_Zone_SphereToBox(struct COL_tdst_ZDx_ *);
void    COL_Zone_BoxToSphere(struct COL_tdst_ZDx_ *);
void    COL_Zone_ObjectToWorld(struct COL_tdst_ZDx_ *, struct OBJ_tdst_GameObject_ *);
void    COL_Zone_WorldToObject(struct COL_tdst_ZDx_ *, struct OBJ_tdst_GameObject_ *);
void    COL_Zone_ChangeFlags(struct COL_tdst_ZDx_ *, ULONG, ULONG);

#ifdef CONVEX_OBJECT
BOOL    COL_IsObjectConvex(struct OBJ_tdst_GameObject_ *);
void    COL_CreatePlans(struct OBJ_tdst_GameObject_ *);
#endif /* CONVEX_OBJECT */

#endif /* ACTIVE_EDITORS */
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* _cplusplcus */

#endif /* __COL_EDIT_ */
