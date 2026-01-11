/*$T EVEload.h GC! 1.078 03/14/00 14:58:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVESAVE_H__
#define __EVESAVE_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern void EVE_Duplicate(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
extern BOOL EVE_b_Animation(struct EVE_tdst_ListTracks_ *);

#ifdef ACTIVE_EDITORS
extern void EVE_SaveListTracks(OBJ_tdst_GameObject *, struct EVE_tdst_ListTracks_ *);
extern void EVE_CheckListTracks(OBJ_tdst_GameObject *, EVE_tdst_ListTracks *);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVESAVE_H__ */
