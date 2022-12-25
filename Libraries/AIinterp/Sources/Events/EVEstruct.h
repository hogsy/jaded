/*$T EVEstruct.h GC! 1.081 05/29/00 09:30:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVESTRUCT_H__
#define __EVESTRUCT_H__
#include "BASe/BAStypes.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "AIinterp/Sources/Events/EVEconst.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
    One event.
 -----------------------------------------------------------------------------------------------------------------------
 */
 
// Ugly Optimsation : for compressed quaternion anim tracks whose flags don't change, 
// we put the data directly in this struct :
// The 6 bytes (3 * 2 shorts) of the compressed quaternion are put 
// in the 6 last bytes (&w_Flags) of this 8 bytes struct.
// uw_NumFrames is always valid.
typedef struct	EVE_tdst_Event_
{
	USHORT	uw_NumFrames;
	short	w_Flags;
	union
	{
		void	*p_Data;
		struct
		{
#if defined(_GAMECUBE) || defined(_XENON)
			USHORT	type;
			USHORT	size;
#else
			USHORT	size;
			USHORT	type;
#endif			
		} t;
	};
} EVE_tdst_Event;

/* -----------------------------------------------------------------------------------------------------------------------
    One track of events
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	EVE_tdst_Track_
{
	OBJ_tdst_GameObject *pst_GO;		/* GO for that track (NULL is current one) */
	EVE_tdst_Event		*pst_AllEvents; /* All events for that track */
	char				*pc_AllData;	/* All Data (Must be ALIGNED !!!) */
	ULONG				ul_DataLength;	/* All Data Length */
	USHORT				uw_NumEvents;	/* Number of events for the track */
	USHORT				uw_Flags;		/* Flags */
	USHORT				uw_Gizmo;		/* Gizmo number for associated GO */
	volatile USHORT		w_Type;			/* volatile for GC Only (Flag to indicate DMA Transfer completed */
		
#ifdef ACTIVE_EDITORS
	char				asz_Name[EVE_C_Tracks_LenName];
	char				c_Color;
#endif
} EVE_tdst_Track;

/*
 -----------------------------------------------------------------------------------------------------------------------
    List of Tracks. Those info can be shared and so MUST NOT contain instanciated variables.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	EVE_tdst_ListTracks_
{
	EVE_tdst_Track				*pst_AllTracks;		/* List of all tracks */
#ifdef _GAMECUBE
	struct ACT_tdst_CacheNode_ *pst_Cache;
	ULONG						ul_GC_Flags;
#endif
	USHORT						uw_NumTracks;		/* Number of tracks for that object */
	USHORT						uw_Flags;
	ULONG						ul_NbOfInstances;	/* Number of instances that share the same List of Tracks */

#ifndef _FINAL_
	ULONG						ul_SizeOf_Ram;
#endif
} EVE_tdst_ListTracks;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Params of the TrackList which need to be instanciated.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	EVE_tdst_Params_
{
	float	f_Time;				/* Time since the beginning of the current event */
	int		i_Param1;			/* A param for the track */
	USHORT	uw_CurrentEvent;	/* Current event */
	USHORT	uw_Flags;			/* Flags */
} EVE_tdst_Params;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Data of the Events.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	EVE_tdst_Data_
{
	struct EVE_tdst_ListTracks_ *pst_ListTracks;
	struct EVE_tdst_Params_		*pst_ListParam;
} EVE_tdst_Data;


#define EVE_FrameToTime(a) (((float) (a)) * 1.0f / 60.0f)
#define EVE_TimeToFrame(a) ((USHORT) (((a) * 60.0f) + 0.5f))

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVESTRUCT_H__ */
