 /*$T ACTload.h GC!1.71 02/25/00 17:04:46 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __ACTLOAD_H__
#define __ACTLOAD_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "ENGine/Sources/ACTions/ACTstruct.h"

ULONG   ACT_ul_ActionCallback(ULONG _ul_PosFile);
ULONG   ACT_ul_ActionKitCallback(ULONG _ul_PosFile);
void	ACT_ActionSaveInit(ACT_st_ActionKit *);
void	ACT_ActionRestoreInit(ACT_st_ActionKit *);

#ifdef ACTIVE_EDITORS
void ACT_DegradeActionKit(OBJ_tdst_GameObject *_pst_GO, ACT_st_ActionKit *_pst_ActionKit);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _ACTLOAD_H__ */
