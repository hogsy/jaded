/*$T GRPmain.h GC 1.139 03/23/04 10:10:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#else
#include "OBJgrp.h"
#include "OBJstruct.h"
#endif
#ifndef __GRPMAIN_H__
#define __GRPMAIN_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
OBJ_tdst_Group	*GRP_pst_CreateNewGroup(void);
#ifdef JADEFUSION
void			GRP_DuplicateGroupObjects(struct WOR_tdst_World_ *, OBJ_tdst_Group *, OBJ_tdst_Group *, UINT LRLAction=0 /*eLRLNone*/);
#else
void			GRP_DuplicateGroupObjects(struct WOR_tdst_World_ *, OBJ_tdst_Group *, OBJ_tdst_Group *);
#endif
OBJ_tdst_Group	*GRP_pst_CreateAndSaveNewGroup(char *, char *, BOOL, BOOL, BOOL *);
void			GRP_ObjAttachGroup(OBJ_tdst_GameObject *, OBJ_tdst_Group *);
void			GRP_ObjAttachReplaceGroup(OBJ_tdst_GameObject *, OBJ_tdst_Group *);
void			GRP_ObjDetachSkeleton(OBJ_tdst_GameObject *);
void			GRP_ObjAttachSkeleton(OBJ_tdst_GameObject *, OBJ_tdst_Group *, OBJ_tdst_Group *);
void			GRP_ObjAttachReplaceSkeleton(OBJ_tdst_GameObject *, OBJ_tdst_Group *, OBJ_tdst_Group *);
void			GRP_ObjDetachGroup(OBJ_tdst_GameObject *);
void			GRP_RepercuteHierarchy(OBJ_tdst_Group *, OBJ_tdst_Group *);
void			GRP_RepercuteLinks(OBJ_tdst_Group *, OBJ_tdst_Group *, struct WOR_tdst_World_ *);
void			GRP_RepercuteAIRefs(OBJ_tdst_Group *, OBJ_tdst_Group *);
void			GRP_RepercuteEventsRefs(OBJ_tdst_Group *, OBJ_tdst_Group *);
void			GRP_ReinitAllGO(OBJ_tdst_Group *);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GRPMAIN_H__ */
