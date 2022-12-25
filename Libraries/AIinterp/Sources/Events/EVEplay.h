/*$T EVEplay.h GC! 1.081 10/04/00 11:29:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVEPLAY_H__
#define __EVEPLAY_H__

#include "AIinterp/Sources/Events/EVEstruct.h"
#include "SDK/Sources/TIMer/TIMdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern struct OBJ_tdst_GameObject_	*EVE_gpst_OwnerGAO;
extern struct OBJ_tdst_GameObject_	*EVE_gpst_CurrentGAO;
extern struct EVE_tdst_Track_		*EVE_gpst_CurrentTrack;
extern struct EVE_tdst_Event_		*EVE_gpst_CurrentEvent;
extern struct MATH_tdst_Matrix_		*EVE_gpst_CurrentRefMatrix;
extern struct EVE_tdst_Data_		*EVE_gpst_CurrentData;
extern struct EVE_tdst_Params_		*EVE_gpst_CurrentParam;
extern float						EVE_gf_DT;
extern ULONG						*EVE_gpul_PartialMask;

extern EVE_tdst_Track				*EVE_pst_SearchTopologicalSymetricTrack(EVE_tdst_Track *);
BOOL								EVE_b_GetSymetricEventInfo
									(
										OBJ_tdst_GameObject *,
										EVE_tdst_Event *,
										short *,
										short *
									);

extern void							*EVE_p_SymetrizeData(void *);

extern void							EVE_SetTracksRatio(EVE_tdst_Data *, float);
extern void							EVE_SetTracksTime(EVE_tdst_Data *, float);
extern void							EVE_GetTracksTime(EVE_tdst_Data *, int, float *, float *);

extern void							EVE_FlashMatrix(void);
extern void							EVE_ReinitTrack(struct EVE_tdst_Data_ *, ULONG);
extern void							EVE_ReinitData(struct EVE_tdst_Data_ *);
extern void							EVE_SetRunningTrack(struct EVE_tdst_Params_ *, BOOL);
extern void							EVE_SetRunningTracks(struct EVE_tdst_Data_ *, BOOL);
extern void							EVE_SetPauseTrack(struct EVE_tdst_Params_ *, BOOL);
extern void							EVE_SetPauseTracks(struct EVE_tdst_Data_ *, BOOL);
extern void							EVE_MainCall(struct WOR_tdst_World_ *);
extern struct EVE_tdst_Event_		*EVE_Event_AIFunction(struct EVE_tdst_Event_ *);
extern BOOL							EVE_b_RunAnimation(EVE_tdst_Data *, float *);
extern BOOL							EVE_b_RunAllTracks(EVE_tdst_Data *, float *);
extern void							EVE_SetAllTimesTo(float);

#ifdef ACTIVE_EDITORS
extern void							EVE_AddTime
									(
										OBJ_tdst_GameObject *,
										EVE_tdst_ListTracks *,
										EVE_tdst_Params *,
										EVE_tdst_Track *,
										float
									);
extern EVE_tdst_Track				*EVE_pst_GizmoToTrack(EVE_tdst_ListTracks *, OBJ_tdst_GameObject *);
extern float						EVE_f_GetTimeFromStartToEvent(EVE_tdst_Track *, struct EVE_tdst_Event_ *);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float EVE_f_GetCurrentRatio(void)
{
	return EVE_gf_DT / (((float) EVE_gpst_CurrentEvent->uw_NumFrames) * 1.0f / 60.0f);
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVEPLAY_H__ */
