/*$T ACTcompute.h GC! 1.081 06/15/00 10:35:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __ACTCOMPUTE_H__
#define __ACTCOMPUTE_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
void						ACT_PartialAction_OnOff(OBJ_tdst_GameObject *, int , int);
void						ACT_SetAction(OBJ_tdst_GameObject *, int, int, BOOL);
struct ACT_tdst_Transition_ *ACT_pst_FindTransition(struct ACT_st_ActionKit_ *, USHORT, UCHAR, USHORT);
#ifdef ACTIVE_EDITORS
void						ACT_SetTransition(struct ACT_st_ActionKit_ *, USHORT, UCHAR, USHORT, ACT_tdst_Transition *);
void						ACT_RemoveTransition(ACT_st_ActionKit *, USHORT, UCHAR, USHORT);
#endif

#ifndef _FINAL_
void						ACT_ul_ComputeWanted();
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _ACTCOMPUTE_H__ */
