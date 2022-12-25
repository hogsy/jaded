/*$T ANIinit.h GC! 1.086 07/06/00 16:55:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __ANIINIT_H__
#define __ANIINIT_H__

#include "BASe/BASsys.h"
#include "BASe/MEMory/MEM.h"
#include "LINks/LINKstruct.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

ANI_tdst_Anim				*ANI_AllocAnim(void);
void						ANI_Free(struct ANI_st_GameObjectAnim_ *);
void						ANI_SetTrackList(ANI_tdst_Anim *, struct EVE_tdst_ListTracks_ *);
void						ANI_ReinitAnimation(ANI_tdst_Anim *);
void						ANI_DestroyAnimation(ANI_tdst_Anim *);
ANI_st_GameObjectAnim		*ANI_pst_CreateGameObjectAnim(void);
void						ANI_Reinit(struct OBJ_tdst_GameObject_ *);
void						ANI_InitSkeleton(struct OBJ_tdst_Group_ *, struct OBJ_tdst_GameObject_ *);
void						ANI_pst_DuplicateGameObjectAnim
							(
								struct WOR_tdst_World_ *,
								struct OBJ_tdst_GameObject_ *,
								struct OBJ_tdst_GameObject_ *
							);
struct OBJ_tdst_GameObject_ *ANI_pst_GetReference(struct OBJ_tdst_GameObject_ *);
struct OBJ_tdst_GameObject_ *ANI_pst_GetReferenceInit(struct OBJ_tdst_GameObject_ *);
void						ANI_CloneSet(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *, ULONG);
void						ANI_ChangeDefaultSkeleton(struct  OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *);
void						ANI_RestoreDefaultSkeleton(struct  OBJ_tdst_GameObject_ *, struct  OBJ_tdst_GameObject_ *);

#ifdef ACTIVE_EDITORS
BOOL						ANI_b_IsGizmoAnim(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ **);
int							ANI_i_CurFrame(OBJ_tdst_GameObject *);
BOOL						ANI_b_BoneHasTrack(struct OBJ_tdst_GameObject_ *);
BOOL						ANI_b_GizmoHasTrack(struct OBJ_tdst_GameObject_ *);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void ANI_UseSkeleton(OBJ_tdst_Group *_pst_Skeleton)
{
	_pst_Skeleton->ul_NbObjectsUsingMe++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void ANI_UseShape(ANI_tdst_Shape *_pst_Shape)
{
	_pst_Shape->uw_Counter++;
}

/*
 =======================================================================================================================
    Aim:    Decreaze the number of time the structure is used and if it is 0, delete it
 =======================================================================================================================
 */
_inline_ void ANI_FreeShape(ANI_tdst_Shape **_ppst_Shape)
{
	if(--(*_ppst_Shape)->uw_Counter == 0)
	{
		MEM_Free(*_ppst_Shape);
		LOA_DeleteAddress(*_ppst_Shape);
		LINK_DelRegisterPointer(*_ppst_Shape);
		*_ppst_Shape = NULL;
	}
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __ANIINIT_H__ */
