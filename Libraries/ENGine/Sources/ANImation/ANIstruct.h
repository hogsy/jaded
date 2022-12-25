/*$T ANIstruct.h GC! 1.081 05/28/01 10:11:13 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _ANISTRUCT_H__
#define _ANISTRUCT_H__
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "MATHs/MATH.h"

struct OBJ_tdst_GameObject_;
struct ACT_st_Action_;
struct ACT_st_ActionKit_;
struct ACT_tdst_Transition_;

#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEplay.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constants
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ANI_C_MaxNumberOfSimultaneousAnim	4	/* A game object can have up to 4 mixed anim */
#define ANI_C_MaxNumberOfCanal				64
#define ANI_C_MaxNumberOfObject				64	/* Max number of object in anim */
#define ANI_C_MaxSizeOfObjectTable			128

#define ANI_C_DefaultAnimFrequency			60	/* Default anim frequency = 60 Hz */
#define ANI_C_DefaultAnimFlag				0	/* No flag by default */

#define ANI_Cuc_ThisIsTheMainBone			255 /* For ucBoneFatherIndex */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Definition of the flags for the event
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ANI_C_EventFlag_EventTypeMask	0x000007					/* 3 bits for the event type */

#define ANI_C_EventFlag_LongJumpEvent	0x000000					/* Event to jump 256+ frames */
#define ANI_C_EventFlag_KeyEvent		0x000001					/* This event is a key event */
#define ANI_C_EventFlag_IKEvent			0x000002					/* Real time IK event */
#define ANI_C_EventFlag_NewFatherEvent	0x000003

/* *** Long jump event *** */
#define ANI_C_EventFlag_NumberOfFrameMask	0xFFFF00				/* Between 1 * 256 to 65535 * 256 */

/* *** Key event */
#define ANI_C_EventFlag_QuaternionTypeMask	0x000038				/* 3 bits for the quaternion type */
#define ANI_C_EventFlag_TranslationTypeMask 0x0001C0				/* 3 bits for the translation type */
#define ANI_C_EventFlag_ReservedMask		0xFFFE00				/* 15 bits reserved */

/* Key quaternion type */
#define ANI_C_EventFlag_SameQuaternion				0x000000		/* Same rotation as the previous key */
#define ANI_C_EventFlag_CompressedQuaternion		0x000008		/* Quaternion with 3 short */
#define ANI_C_EventFlag_UltraCompressedQuaternionX	0x000010		/* Quaternion with 1 short & 2 char */
#define ANI_C_EventFlag_UltraCompressedQuaternionY	0x000018		/* Idem */
#define ANI_C_EventFlag_UltraCompressedQuaternionZ	0x000020		/* Idem */
#define ANI_C_EventFlag_IdentityQuaternion			0x000028		/* The quaternion is the identity */
#define ANI_C_EventFlag_CompleteQuaternion			0x000038

/* Key translation type */
#define ANI_C_EventFlag_SameTranslation					0x000000	/* Same translation as before */
#define ANI_C_EventFlag_CompleteTranslation				0x000040	/* Translation with 3 float */
#define ANI_C_EventFlag_CompressedAbsoluteTranslation	0x000080	/* Thanslation with 3 short */
#define ANI_C_EventFlag_CompressedRelativeTranslation	0x0000C0	/* Thanslation with 3 short */
#define ANI_C_EventFlag_UltraCompressedTranslationX		0x000100	/* Translation 1 short & 2 char */
#define ANI_C_EventFlag_UltraCompressedTranslationY		0x000140	/* Idem */
#define ANI_C_EventFlag_UltraCompressedTranslationZ		0x000180	/* Idem */
#define ANI_C_EventFlag_NulTranslation					0x0001C0	/* Translation = (0,0,0) */

/*$F -----------------------------------------------------------------------------------------------------------------------


														ANIMATION MODE


	 -----------------------------------------------------------------------------------------------------------------------

  - Mode 0: The Anim Player moves the MagicBox due to the Anim, computes the distance covered by the MagicBox, moves 
			the MagicBox back to the position it was before the Anim Player and update the speed of the object so that the 
			DYN Module place the MagicBox exactly at the same position. (Speed From Anim Mode: WALK, RUN)


  - Mode 1: We have computed a precise speed to play the Anim (DYN). We update the Frequency of the Anim so that it fits 
			this precise speed. (Frequency Set Speed : NOT REALLY USED)

  - Mode 3: We have computed a precise speed to play the Anim (DYN). The Anim Player moves the MagicBox at the Animation 
			speed, but it was not the speed we wanted. We move back the MagicBox to where it was before the Anim Player so that 
			our well-computed speed moves it correctly.(Speed From Dyna	: JUMP)
	-----------------------------------------------------------------------------------------------------------------------
 */
#define ANI_C_AnimFlag_DontPlayAnim		0x80
#define ANI_C_AnimFlag_Developped		0x08

#define ANI_C_AnimFlag_PlayModeMask		0x07
#define ANI_C_AnimFlag_SpeedFromAnim	0x00
#define ANI_C_AnimFlag_UpdateFrequency	0x02
#define ANI_C_AnimFlag_SpeedFromDyna	0x06



/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Misc.Flags.
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ANI_C_MiscFlag_UseTimeInParam1			0x01
#define ANI_C_MiscFlag_MagicBox_AfterEnd		0x02
#define ANI_C_MiscFlag_MagicBox_Interpolation	0x04	/* We arrived at the End of a BlendAnim that has NOT
														 * FREEZEBONES. There is no ActionItem to loop with. So, We
														 * will interpolate the position of the MagicBox depending on
														 * the animation last-frame speed. */
#define ANI_C_MiscFlag_MagicBox_Loop			0x08

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for UPDATE FLASH
 -----------------------------------------------------------------------------------------------------------------------
 */

#define ANI_C_UpdateLeadAnim	0x01
#define ANI_C_UpdateBlendAnim	0x02
#define ANI_C_UpdateAllAnims	0x03

/*$F
 ---------------------------------------------------------------------------------------------------
	ANI_tdst_Anim
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	ANI_tdst_Anim_
{
	USHORT				uw_Flag;
	UCHAR				uc_AnimFrequency;		/* Frequency in Hz. Default = 60 Hz */
	UCHAR				uc_NbOfCanal;
	EVE_tdst_Data		*pst_Data;

	/* --- Blend --- */
	MATH_tdst_Matrix	st_Ref;					/* Ref matrix for Anim (TOTALLY USELESS matrix for Lead Anim because
												 * the Flash Matrix has this role) */
	USHORT				uw_BlendedAction;		/* Action for a blended animation */
	UCHAR				uc_BlendedActionItem;	/* Action item for a blended animation */
	UCHAR				uc_BlendFlags;			/* Copy of the ActionItem Blend Flags */
	UCHAR				uc_MiscFlags;			/* Miscellaneous partcular-cases Blend Flags */
	UCHAR				uc_BlendTime;			/* Total time of blend */
	USHORT				uw_BlendCurTime;		/* Current Time of blend */

#ifdef ACTIVE_EDITORS
	USHORT				uw_CurrentFrame;
	USHORT				uw_TotalFrame;
	USHORT				uw_CurrentBlendFrame;
	UCHAR				uc_Mode;
	UCHAR				uc_Dummy;
#endif
} ANI_tdst_Anim;

/*$F
 ---------------------------------------------------------------------------------------------------
	ANI_tdst_Shape: Link between Bones and Visuals
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	ANI_tdst_Shape_
{
	USHORT	uw_Counter;
	USHORT	uw_LastCanal;
	UCHAR	auc_AI_Canal[ANI_C_MaxNumberOfCanal];
	UCHAR	auc_Visu[ANI_C_MaxNumberOfCanal];
} ANI_tdst_Shape;

/*$F
 ---------------------------------------------------------------------------------------------------
    ANI_st_GameObjectAnim
	---------------------
    A game object has a pointer to this structure.
    It contains the anim(s) used by the game object.
    This structure changes during the game.
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	ANI_st_GameObjectAnim_
{
	UCHAR						uc_Synchro;		/* Are tracks synchronized ? (only magic box tracks were played) */
	UCHAR						uc_AnimUsed;

	USHORT						uw_Frame;

	OBJ_tdst_Group				*pst_SkeletonModel;
	OBJ_tdst_Group				*pst_Skeleton;
	OBJ_tdst_Group				*pst_DefaultSkeleton;
	ANI_tdst_Shape				*pst_DefaultShape;
	UCHAR						auc_DefaultVisu[ANI_C_MaxNumberOfCanal];	/* AI can change default visuals */
	ANI_tdst_Shape				*pst_Shape;

	struct ACT_st_ActionKit_	*pst_ActionKit;
	MATH_tdst_Matrix			*dpst_Stock;

	ANI_tdst_Anim				*apst_PartialAnim[ANI_C_MaxNumberOfSimultaneousAnim];
	ULONG						aul_PartialMask[2];
	struct ACT_st_Action_		*pst_CurrentPartialAction;
	USHORT						uw_CurrentPartialActionIndex;
	UCHAR						uc_CurrentPartialActionItemIndex;
	UCHAR						uc_CounterForRepetitionPartial;

	UCHAR						uc_BlendLength_IN;
	UCHAR						uc_BlendLength_OUT;

	UCHAR						uc_PauseAnim;
	UCHAR						uc_Dumi;

	ANI_tdst_Anim				*apst_Anim[ANI_C_MaxNumberOfSimultaneousAnim];

	struct ACT_st_Action_		*pst_CurrentAction;
	struct ACT_tdst_Transition_ *pst_Transition;		/* Current played transition (NULL if not) */

	USHORT						uw_CurrentActionIndex;
	USHORT						uw_NextActionIndex;		/* Next action when transition */

	UCHAR						uc_CurrentActionItemIndex;
	UCHAR						uc_CounterForRepetition;
	UCHAR						uc_Frequency;			/* Flag that sets the Frequency of the Action set by EVE player */
	UCHAR						uc_Dummy;				/* Used in EyeFollow !!! */

	char						c_CullingMask;
	char						c_LOD_IA;				/* IA may force a particular LOD for animation */
	char						c_LOD_Dynamic;			/* LOD computed based on distance and BitField of possible LOD for Actor */
	char						c_LOD_Bitfield;			/* BitField of possible LOD for Actor */

	float						f_Z_Offset;

	float						f_NextActionDT;			/* dt of the EVE player SetAction in the Total DT of the Frame */

	UCHAR						uc_ForceMode;
	UCHAR						uc_Signal;
	USHORT						uw_Dummy;

} ANI_st_GameObjectAnim;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _ANISTRUCT_H__ */
