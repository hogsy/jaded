/*$T COLcob.h GC! 1.081 06/28/00 15:17:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __COL_CLO__
#define __COL_CLO__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplcus */

/*$F GC Dependencies 11/22/99 */
#include "BASe/BAStypes.h"

#ifdef ACTIVE_EDITORS
void						COL_Cob_SphereToBox(struct COL_tdst_Cob_ *);
void						COL_Cob_BoxToCylinder(struct COL_tdst_Cob_ *);
void						COL_Cob_CylinderToSphere(struct COL_tdst_Cob_ *);
#endif
void						COL_ColMap_AddGeometric
							(
								struct COL_tdst_Cob_ *,
								struct GEO_tdst_Object_ *,
								MATH_tdst_Vector *
							);
void						COL_ColMap_AddCob(struct OBJ_tdst_GameObject_ *, UCHAR, void *, void *);
void						COL_ComputeProximity(struct COL_tdst_Cob_ *);
void						COL_ComputeJumpPoint(struct OBJ_tdst_GameObject_ *, MATH_tdst_Vector *, MATH_tdst_Vector *, float, BOOL, BOOL, BOOL, BOOL, BOOL);
void                        COL_ColMap_RecomputeNormals(struct	COL_tdst_IndexedTriangles_ *,struct GEO_tdst_Object_ *);

struct COL_tdst_GameMat_	*COL_pst_GMat_Get(struct COL_tdst_Cob_ *, struct COL_tdst_ElementIndexedTriangles_ *);
ULONG						COL_ul_GMat_GetIndex(struct COL_tdst_Cob_ *, struct COL_tdst_ElementIndexedTriangles_ *);
void						COL_GMat_RemoveAtIndex(struct COL_tdst_Cob_ *, ULONG);
void						COL_GeoColMap_PosSet(struct OBJ_tdst_GameObject_ *, UCHAR, MATH_tdst_Vector *);
void						COL_UpdateCobFromVisualRealTime(struct OBJ_tdst_GameObject_ *, struct COL_tdst_Cob_ *, struct GEO_tdst_Object_ *);
#ifdef ACTIVE_EDITORS
void						COL_GMat_DisplayInfo(struct COL_tdst_GameMatList_ *);
void						COL_GMat_UpdateFileName(struct COL_tdst_Cob_ *);
void						COL_SynchronizeCob(struct COL_tdst_Cob_ *, BOOL, BOOL);
#endif



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* _cplusplcus */

#endif /* __COL_CLO_ */
