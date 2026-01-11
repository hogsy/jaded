/*$T COLinit.h GC!1.71 01/27/00 13:56:28 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __COL_INIT__
#define __COL_INIT__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplcus */

/*$F GC Dependencies 11/22/99 */
#include "BASe/BAStypes.h"

void                    COL_AllocDetectionList(struct OBJ_tdst_GameObject_ *);
void                    COL_AllocInstance(struct OBJ_tdst_GameObject_ *);
void                    COL_AllocColMap(struct OBJ_tdst_GameObject_ *);
struct COL_tdst_ColSet_ *COL_AllocColSet(void);

void                    COL_FreeDetectionList(struct COL_tdst_Base_ *);
void                    COL_FreeInstance(struct COL_tdst_Base_ *);
void                    COL_FreeColMap(struct COL_tdst_Base_ *);
void                    COL_FreeColSet(struct COL_tdst_ColSet_ *);
void                    COL_Free(struct OBJ_tdst_GameObject_ *);
void                    COL_FreeMathematicalCob(struct COL_tdst_Cob_ *);
void                    COL_FreeTrianglesIndexedCob(struct COL_tdst_Cob_ *);
void                    COL_FreeCob(struct COL_tdst_Cob_ *);
void					COL_FreeGameMaterial(struct COL_tdst_Cob_ *);

void                    COL_Reinit(struct OBJ_tdst_GameObject_ *);
void                    COL_DuplicateInstance(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *, BOOL);

void                    COL_DuplicateColMap(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *, BOOL);
void					COL_SwapToSpecific(struct OBJ_tdst_GameObject_ *, UCHAR);

BOOL					COL_b_UnCollidable(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *);
void					COL_AddUnCollidableObject(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *);
void					COL_RemoveUnCollidableObject(struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *);
void					COL_FlushAndFreeUnCollidableList(struct OBJ_tdst_GameObject_ *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* _cplusplcus */

#endif /* __COL_INIT_ */
