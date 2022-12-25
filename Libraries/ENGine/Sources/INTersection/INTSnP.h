/*$T INTSnP.h GC!1.67 01/03/00 14:23:16 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __INT_SnP__
#define __INT_SnP__
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "INTstruct.h"

void    INT_SnP_RemoveOverlap(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);
void    INT_SnP_InsertionSort(INT_tdst_SnP *, ULONG);
void    INT_SnP_UpdateCollidedListAfterRemoval(OBJ_tdst_GameObject *);
void    INT_SnP_Refresh(INT_tdst_SnP *, TAB_tdst_PFtable *);
void    INT_SnP_DetachObject(OBJ_tdst_GameObject *, struct WOR_tdst_World_ *);
void    INT_SnP_AttachObject(OBJ_tdst_GameObject *, struct WOR_tdst_World_ *);
void	INT_SnP_AddObject(OBJ_tdst_GameObject *, INT_tdst_SnP *);

#ifdef ACTIVE_EDITORS
void INT_SnP_UpdateDetectionList(OBJ_tdst_GameObject *, struct WOR_tdst_World_ *);
void INT_SnP_Check(INT_tdst_SnP *, struct TAB_tdst_PFtable_ *);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __COL_SnP__ */
