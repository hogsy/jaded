/*$T ANIplay.h GC! 1.081 05/25/00 14:54:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __ANIPLAY_H__
#define __ANIPLAY_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "ENGine/Sources/ANImation/ANIstruct.h"

void	ANI_PlayGameObjectAnimation(struct OBJ_tdst_GameObject_ *, ANI_st_GameObjectAnim *, float, MATH_tdst_Vector *);

void	ANI_PlayOneAnimationForOneCanal
		(
			MATH_tdst_Matrix *,
			struct ANI_st_Canal *,
			int _iLevelOfDetail,
			int _iCurrentFrame,
			int _iCurrentSubFrame
		);

void	ANI_ChangeAction(struct OBJ_tdst_GameObject_ *, ANI_st_GameObjectAnim *);
void	ANI_ChangePartialAction(struct OBJ_tdst_GameObject_ *);

BOOL	ANI_b_GizmoInMask(ULONG *, USHORT);
void	ANI_PartialAnim_PlayAll(struct OBJ_tdst_GameObject_ *, float);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __ANIPLAY_H__ */
