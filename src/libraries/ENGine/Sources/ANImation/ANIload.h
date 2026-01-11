/*$T ANIload.h GC! 1.081 04/13/00 10:21:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "SDK/Sources/BIGfiles/BIGdefs.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"

BOOL			ANI_b_AnimImport(char *, char *, char *);
void			ANI_DestroySkeleton(struct OBJ_tdst_Group_ *);
void			ANI_ResolveRefs(struct OBJ_tdst_GameObject_ *);
void			ANI_AttachSkeletonModelToWorld(struct OBJ_tdst_Group_ *, struct WOR_tdst_World_ *);
void			ANI_ApplyShape(struct OBJ_tdst_GameObject_ *);
ULONG			ANI_ul_ShapeCallback(ULONG);

#ifdef  ACTIVE_EDITORS
void			ANI_ResolveAllTracksAndOptimize(struct OBJ_tdst_GameObject_ *);
BIG_KEY			ANI_DataToTrack(BIG_KEY);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
