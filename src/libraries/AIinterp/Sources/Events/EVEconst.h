/*$T EVEconst.h GC! 1.081 04/04/00 09:15:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVECONST_H__
#define __EVECONST_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


#define EVE_C_Tracks_LenName	15		/* Max len of a track name */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for list of traks
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EVE_C_Tracks_NoFlash	0x0001	/* Do never matrix flash */
#define EVE_C_Tracks_Resolved	0x0002	/* List of tracks has been resolved */
#define EVE_C_Tracks_Anims		0x8000	/* List of tracks is part of an anim (Load/Save only) */
#define EVE_C_Tracks_ForceARAM	0x4000

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for Shared Tracks
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EVE_C_Track_RunningInit 0x0001	/* A Track is currently running */
#define EVE_C_Track_AutoLoop	0x0002	/* Track must loop at the end */
#define EVE_C_Track_AutoStop	0x0004	/* Track must stop at the end */
#define EVE_C_Track_Time		0x0008	/* Time track, track param is the calculated dt */
#define EVE_C_Track_MustResolve	0x0010	/* Track contains events that needs to be resolved */
#define EVE_C_Track_Hidden		0x0020	/* Track is hidden */
#define EVE_C_Track_Selected	0x0040	/* Track is selected */
#define EVE_C_Track_Curve		0x0080	/* Display curve */

#define EVE_C_Track_FirstEvent					0x0100
#define EVE_C_Track_Under256					0x0200	/* All the Events of the Track are under 256 frames */
#define EVE_C_Track_Flash						0x0400	/* The First Event of the Track is Flash */
#define EVE_C_Track_SameFlags					0x0800	/* All Events of the Track have the same Flags (The First Event may have Flash) */
#define EVE_C_Track_SameType					0x1000	/* All Events of the Track have the same Type */
#define EVE_C_Track_SameSize					0x2000	/* All Events of the Track have the same size */
#define EVE_C_Track_Optimized					0x4000	/* Track already optimized */
#define EVE_C_Track_Anims						0x8000	/* Anim Track */


#ifdef _GAMECUBE
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for Track Type (GAMECUBE ONLY)
 -----------------------------------------------------------------------------------------------------------------------
 */
#define EVE_C_Track_UseARAM			0x0001		/* Track uses ARAM */
#define EVE_C_Track_InRAMGC			0x0002		/* Track using ARAM but currently loaded in RAM */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for ListTracks (GAMECUBE ONLY)
 -----------------------------------------------------------------------------------------------------------------------
 */
#define EVE_C_ListTracks_UseARAM	0x0001	/* This TrackList uses ARAM */
#define EVE_C_ListTracks_InRAMGC	0x0002	/* TrackList using ARAM but currently loaded in RAM */


#endif


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for Instance
 -----------------------------------------------------------------------------------------------------------------------
 */
#define EVE_C_Track_Running		0x0001	/* A track is currently running */
#define EVE_C_Track_Pause		0x0002	/* A track is currently stopped */
#define EVE_C_Track_EventDone	0x0004	/* Event has been done once */


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for events
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EVE_C_EventFlag_Empty				0x0000
#define EVE_C_EventFlag_DoOnce				0x0001	/* Do event only one time */
#define EVE_C_EventFlag_Selected			0x0002	/* Selected (editor) */
#define EVE_C_EventFlag_Flash				0x0004	/* flash matrix */
#define EVE_C_EventFlag_WaitFalse			0x0008	/* Wait until response is false */
#define EVE_C_EventFlag_WaitTrue			0x0010	/* Wait until response is true */
#define EVE_C_EventFlag_Symetric			0x0020	/* Symetric Event */
#define EVE_C_EventFlag_Basic				0x003F
#define EVE_C_EventFlag_AIFunction			0x0040
#define EVE_C_EventFlag_InterpolationKey	0x0080
#define EVE_C_EventFlag_MorphKey			0x0100	/* Morph key */
#define EVE_C_EventFlag_GotoLabel			0x0200
#define EVE_C_EventFlag_SetTimeToLabel		0x0400
#define EVE_C_EventFlag_Type				0x07C0
#define EVE_C_EventFlag_NoInterpolation		0x0800	/* Not interpoled rotation or translation */

#define EVE_C_EventFlag_LabelNum			0xF000	/* Num of label */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for AI functions
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EVE_C_None			0x00000000
#define EVE_C_All			0x00000001
#define EVE_C_IntFirstParam 0x00000002

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Types of events
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EVE_LOD_FreezeSkeleton						0x01
#define EVE_LOD_OneFrameOutOfTwo					0x02
#define EVE_LOD_SkipTranslations					0x04
#define EVE_LOD_SkipExtremityBones					0x08
#define EVE_LOD_FrameParity							0x10


#define EVE_LOD_OnlyMagicBoxWhenCulled				0x80



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVECONST_H__ */
