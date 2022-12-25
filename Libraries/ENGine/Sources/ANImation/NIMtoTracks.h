/*$T NIMtoTracks.h GC! 1.081 05/02/00 09:46:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __NIMTOTRACKS_H__
#define __NIMTOTRACKS_H__

#include "ANImation/ANIstruct.h"
#include "AIinterp/Sources/Events/EVEstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
void				ANI_ConvertAnimDataToListTracks(EVE_tdst_ListTracks *, struct ANI_st_Canal *, int, int);
BIG_KEY				ANI_ConvertNIMfileToTracks(BIG_INDEX);

/*
 =======================================================================================================================
    Aim:    Return the relative time of an event (the elapsed time since the previous event of the same bone) in number
            of frame
 =======================================================================================================================
 */
_inline_ int ANI_i_GetEventRelativeTime(ULONG _ul_EventFlag)
{
	/* The 8 higher bits contain the event time (between 0 and 255 frames) */
	return _ul_EventFlag >> 24;
}

/*
 =======================================================================================================================
    Aim:    Return the type of an event (key, IK event ...)
 =======================================================================================================================
 */
_inline_ int ANI_i_GetEventType(ULONG _ul_EventFlag)
{
	return _ul_EventFlag & ANI_C_EventFlag_EventTypeMask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int ANI_i_GetKeyQuaternionType(ULONG _ul_EventFlag)
{
	return _ul_EventFlag & ANI_C_EventFlag_QuaternionTypeMask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int ANI_i_GetKeyTranslationType(ULONG _ul_EventFlag)
{
	return _ul_EventFlag & ANI_C_EventFlag_TranslationTypeMask;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __NIMTOTRACKS_H__ */
