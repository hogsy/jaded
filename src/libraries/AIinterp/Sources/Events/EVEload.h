/*$T EVEload.h GC! 1.080 03/23/00 10:01:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVELOAD_H__
#define __EVELOAD_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void		EVE_ResolveTrackRef(struct EVE_tdst_ListTracks_ *, struct EVE_tdst_Track_ *);
extern void		EVE_ResolveListTrackRef(struct EVE_tdst_ListTracks_ *);
extern void		EVE_ResolveRefs(OBJ_tdst_GameObject *, BOOL);
extern ULONG	EVE_ul_CallbackLoadListEvents(ULONG);
extern ULONG	EVE_ul_CallbackLoadListTracks(ULONG);
extern ULONG	EVE_ul_CallbackLoadAnims(ULONG);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVELOAD_H__ */
