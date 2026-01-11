/*$T EVEinit.h GC! 1.081 05/03/00 11:45:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVEINIT_H__
#define __EVEINIT_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void						EVE_AllocateMainStruct(struct OBJ_tdst_GameObject_ *);
extern void						EVE_InitData(struct EVE_tdst_Data_ *, struct EVE_tdst_ListTracks_ *);
extern void						EVE_FreeMainStruct(struct OBJ_tdst_GameObject_ *);
extern void						EVE_FreeTrack(struct EVE_tdst_Track_ *);
extern void						EVE_DeleteListTracks(struct EVE_tdst_ListTracks_ *);
#ifdef ACTIVE_EDITORS
extern struct EVE_tdst_Event_	*EVE_pst_NewEvent(struct EVE_tdst_Track_ *, int, int);
extern void						EVE_DeleteEvent(struct EVE_tdst_Track_ *, int);
extern struct EVE_tdst_Track_	*EVE_pst_NewTrack(struct EVE_tdst_Data_ *);
extern void						EVE_DeleteTrack(struct EVE_tdst_Data_ *, int);
#endif
extern EVE_tdst_Data			*EVE_pst_DuplicateData(struct EVE_tdst_Data_ *);
extern EVE_tdst_ListTracks		*EVE_pst_DuplicateListTracks(struct EVE_tdst_Data_ *);
extern BOOL						EVE_MysticalOptimization(struct OBJ_tdst_GameObject_ *, EVE_tdst_ListTracks *, float, float);
extern void						EVE_ScaleAllTranslations(EVE_tdst_ListTracks *, float);



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVEINIT_H__ */
