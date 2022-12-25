/*$T ACTstruct.h GC! 1.081 06/08/00 14:19:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __ACTSTRUCT_H__
#define __ACTSTRUCT_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
struct ANI_st_Anim_;
struct BAS_tdst_barray_;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constants
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ACT_C_SizeOfAction			32	/* Maximum number of animations in an action */
#define ACT_C_SizeOfActionKit		512 /* Maximum number of actions in an action kit */

#define ACT_C_DefaultAnimFrequency	64	/* 64=Default anim frequency, 32=Half frequency... */

#define ACT_C_NoAction				0xFFFF
#define ACT_C_NoActionItem			0xFF

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for ACT_st_ActionItem.uc_Flag:: £
    # 1,2,3:: Mode for the anim (same value as the anim, see the file ANIstruct.h")£
    # 4:: Ignore gravity (0 = gravity, 1 = no gravity)£
    # 5:: Ignore traction (0 = traction, 1 = no traction)£
    # 6:: Ignore stream (0 = stream, 1 = no stream)£
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ACT_C_ActionItemFlag_Developped					0x08
#define ACT_C_ActionItemFlag_IgnoreGravity				0x10
#define ACT_C_ActionItemFlag_IgnoreTraction				0x20
#define ACT_C_ActionItemFlag_IgnoreStream				0x40
#define ACT_C_ActionItemFlag_IgnoreVerticalTranslation	0x80

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for transition
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ACT_C_TF_BlendProgressive       0x01
#define ACT_C_TF_BlendProgressiveInv    0x02
#define ACT_C_TF_BlendFreezeBones       0x04
#define ACT_C_TF_BlendFreezeMagicBox    0x08
#define ACT_C_TF_DefaultTransition		0x10
#define ACT_C_TF_BlendStock				0x20
#define ACT_C_TF_NextDefaultTransition	0x40
#define ACT_C_TF_BlendBetweenActionItem	0x80


#ifdef _GAMECUBE

#define	ACT_MaxNumberOfActionKits	100
#define ACT_CacheLinesPerActionKit	10

typedef struct	ACT_tdst_CacheNode_
{
	struct EVE_tdst_ListTracks_			*pst_TrackList;			/* RAM Pointer on TrackList */
	struct EVE_tdst_Event_				**ppst_ListEvents;		/* All RAM pointers on ListEvents (one per track) */
	LONG								l_Counter;				/* Cache incremental counter */
	LONG								l_Shares;				/* Number of ActionKits that use this TrackList */
	LONG								l_Frames;				/* Number of frames since the last use of this track */
} ACT_tdst_CacheNode;


extern	ACT_tdst_CacheNode	ACT_gast_Cache[];
extern	LONG				ACT_gl_FirstFreeIndex;
#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
    Transition between an action item and an action
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	ACT_tdst_Transition_
{
	USHORT	uw_Action;	/* Destination action */
	UCHAR	uc_Flag;
	UCHAR	uc_Blend;
} ACT_tdst_Transition;

/*
 -----------------------------------------------------------------------------------------------------------------------
    ACT_st_ActionItem
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	ACT_st_ActionItem_
{
	struct EVE_tdst_ListTracks_ *pst_TrackList;
	struct BAS_tdst_barray_		*pst_Transitions;
	struct ANI_tdst_Shape_		*pst_Shape;
	UCHAR						uc_Repetition;
	UCHAR						uc_NumberOfFrameForBlend;
	UCHAR						uc_Frequency;
	UCHAR						uc_CustomBit;
	USHORT						uw_DesignFlags;
	UCHAR						uc_Flag;
	UCHAR						uc_Dummy;
} ACT_st_ActionItem;

/*
 -----------------------------------------------------------------------------------------------------------------------
    ACT_st_Action
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	ACT_st_Action_
{
	UCHAR				uc_NumberOfActionItem;
	UCHAR				uc_ActionItemNumberForLoop;
	USHORT				uw_Counter;
	ACT_st_ActionItem	ast_ActionItem[ACT_C_SizeOfAction];
} ACT_st_Action;

/*
 -----------------------------------------------------------------------------------------------------------------------
    ACT_st_ActionKit
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	ACT_st_ActionKit_
{
	USHORT			uw_NumberOfAction;
	USHORT			uw_Counter;
	ULONG			ul_Flag;
#ifdef _GAMECUBE
	short				w_NumCacheLines;
	short				w_NextIndex;
	LONG				al_CacheIndexes[ACT_CacheLinesPerActionKit];
#endif
	ACT_tdst_Transition DefaultTrans;
	ACT_st_Action	**ppst_ActionInits;
	ACT_st_Action	*apst_Action[ACT_C_SizeOfActionKit];
} ACT_st_ActionKit;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _ACTSTRUCT_H__ */
