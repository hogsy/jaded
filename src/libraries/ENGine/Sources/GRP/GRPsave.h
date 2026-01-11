/*$T GRPsave.h GC!1.71 03/02/00 12:02:53 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif


#ifndef __GRPSAVE_H__
#define __GRPSAVE_H__

#include "ENGine/Sources/WAYs/WAYstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void     GRP_SaveGroup(OBJ_tdst_Group *);
void            GRP_DuplicateGroup(struct WOR_tdst_World_ *, OBJ_tdst_Group *, OBJ_tdst_Group *);
OBJ_tdst_Group  *GRP_pst_DuplicateGroupFile(struct WOR_tdst_World_ *, BIG_INDEX);
void			GRP_DetachAndDestroyGAOs(struct WOR_tdst_World_ *, OBJ_tdst_Group *, BOOL);
void			GRP_AfterLoaded(struct WOR_tdst_World_ *, OBJ_tdst_Group *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GRPSAVE_H__ */
