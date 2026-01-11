/*$T OBJconst.h GC! 1.081 08/30/00 14:54:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __OBJ_CONST_H__
#define __OBJ_CONST_H__

/*$3
 =======================================================================================================================
    Identity flags
 =======================================================================================================================
 */

#define OBJ_C_IdentityFlag_Bone				0x00000001	/* Bone Object */
#define OBJ_C_IdentityFlag_Anims			0x00000002	/* Objects with anims */
#define OBJ_C_IdentityFlag_Dyna				0x00000004	/* Moving objects */
#define OBJ_C_IdentityFlag_SharedMatrix		0x00000008	/* Position and hierarchy are shared */
#define OBJ_C_IdentityFlag_Lights			0x00000010	/* Lumi res */
#define OBJ_C_IdentityFlag_AI				0x00000020	/* Objets qui ont une AI à exécuter */
#define OBJ_C_IdentityFlag_DesignStruct		0x00000040	/* Objets qui ont des événements à exécuter */
#define OBJ_C_IdentityFlag_Waypoints		0x00000080	/* Objets qui servent de rep re pour les déplacements d'acteurs */
#define OBJ_C_IdentityFlag_ColMap			0x00000100	/* Objets qui ont un objet de collision */
#define OBJ_C_IdentityFlag_ZDM				0x00000200	/* Objets qui ont une ColSet */
#define OBJ_C_IdentityFlag_ZDE				0x00000400	/* Objets qui ont au moins une ZDE */
#define OBJ_C_IdentityFlag_DrawAtEnd		0x00000800	/* Draw this object at the end (FPS Owner) */
#define OBJ_C_IdentityFlag_BaseObject		0x00001000	/* Object with base data */
#define OBJ_C_IdentityFlag_ExtendedObject	0x00002000	/* Object with extended data */
#define OBJ_C_IdentityFlag_Visu				0x00004000	/* Object with visu data, must be used with base */
#define OBJ_C_IdentityFlag_Msg				0x00008000	/* Object that can receive messages */
#define OBJ_C_IdentityFlag_HasInitialPos	0x00010000	/* Object with two pos : current and init */
#define OBJ_C_IdentityFlag_Generated		0x00020000	/* Object has been generated (must not be save) */
#define OBJ_C_IdentityFlag_Links			0x00040000	/* Object has some links to other objects */
#define OBJ_C_IdentityFlag_OBBox			0x00080000	/* Object has an additionnal OBBox BV */
#define OBJ_C_IdentityFlag_DesignHelper		0x00100000	/* For design only */
#define OBJ_C_IdentityFlag_AdditionalMatrix 0x00200000	/* Object has additional matrix */
#define OBJ_C_IdentityFlag_Hierarchy		0x00400000	/* Object has hierarchy data */
#define OBJ_C_IdentityFlag_Group			0x00800000	/* Object has a group */
#define OBJ_C_IdentityFlag_AddMatArePointer 0x01000000	/* Additional matrix are pointer on matrix */
#define OBJ_C_IdentityFlag_Events			0x02000000	/* Objects with event tracks */
#define OBJ_C_IdentityFlag_FlashMatrix		0x04000000	/* Objects with a Event/Anim Flash Matrix */
#define OBJ_C_IdentityFlag_Sound			0x08000000	/* Objects with a sound bank */
#define OBJ_C_IdentityFlag_ODE				0x10000000	/* Objects "visible" for ODE solver */

#define OBJ_C_IdentityFlag_NoSPG2			0x20000000	/* no spg grid on that */

#define OBJ_C_IdentityFlag_Unvalid			0x80000000	/* This flag shoud never be set, except if unvalid object */

#define OBJ_C_VisibleObjectFlags \
		( \
			OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_Anims | OBJ_C_IdentityFlag_Waypoints | \
				OBJ_C_IdentityFlag_Links | OBJ_C_IdentityFlag_Lights \
		)

/*$3
 =======================================================================================================================
    Status flags
 =======================================================================================================================
 */

#define OBJ_C_StatusFlag_Active			0x00000001	/* Flag that indicates if the object is active or not */
#define OBJ_C_StatusFlag_Visible		0x00000002	/* Indicates if object is visible by one or more world views */
#define OBJ_C_StatusFlag_Culled			0x00000004
#define OBJ_C_StatusFlag_RTL			0x00000008	/* Indicates if object is real-time lightned */
#define OBJ_C_ControlFlag_SectoReinit	0x00000010	/* L'objet est réinitialisé s'il est activé par la secto */
#define OBJ_C_StatusFlag_Detection		0x00000020	/* Indicates if an object has the SnP Detection List */
#define OBJ_C_StatusFlag_HasChild		0x00000040	/* Indicates if an object has one or several child */
#define OBJ_C_ControlFlag_EnableSnP		0x00000080	/* Indicates that the Object must be inserted in the SnP */
/* The 0x00000080 Flag is taken for Control */


/*$3
 =======================================================================================================================
    IA Custom Bits
 =======================================================================================================================
 */

#define OBJ_C_CustomBit1			0x00000100
#define OBJ_C_CustomBit2			0x00000200
#define OBJ_C_CustomBit3			0x00000400
#define OBJ_C_CustomBit4			0x00000800
#define OBJ_C_CustomBit5			0x00001000
#define OBJ_C_CustomBit6			0x00002000
#define OBJ_C_CustomBit7			0x00004000
#define OBJ_C_CustomBit8			0x00008000
#define OBJ_C_CustomBitAll			0x0000FF00

/*$3
 =======================================================================================================================
    Control flags
 =======================================================================================================================
 */

#define OBJ_C_ControlFlag_SectoInvisible		0x00010000	/* Objet caché par la secto */
#define OBJ_C_ControlFlag_SectoInactive			0x00020000	/* Objet désactivé par la secto */
#define OBJ_C_ControlFlag_ForceRTL				0x00040000	/* Forces an object to be Real time ligthned */
#define OBJ_C_ControlFlag_ForceNoRTL			0x00080000	/* Forces an object not to be Real Time Lightned */
#define OBJ_C_ControlFlag_ForceDetectionList	0x00100000	/* Forces the object to have a detecion list */
#define OBJ_C_ControlFlag_ForceInvisible		0x00200000	/* Set to true to force an object to be invisible */
#define OBJ_C_ControlFlag_ForceInactive			0x00400000	/* Set to true to force an object to be inactive */
#define OBJ_C_ControlFlag_LookAt				0x00800000	/* Set to true to force object to look at camera */
#define OBJ_C_ControlFlag_RayInsensitive		0x01000000	/* Indicates if the BV of the object can receive a Ray cast or not */
#define OBJ_C_ControlFlag_EditableBV			0x02000000	/* Indicates if the BV of the object can be edit */
#define OBJ_C_ControlFlag_InPause				0x04000000	/* Object is currently in pause (no active) */
#define OBJ_C_ControlFlag_AlwaysActive			0x08000000	/* Indicates that an objet is always active */
#define OBJ_C_ControlFlag_AlwaysVisible			0x10000000	/* Indicates that an objet is always visible */
#define OBJ_C_ControlFlag_ForceInvisibleInit	0x20000000	/* Force invisible valeur init */
#define OBJ_C_ControlFlag_ForceInactiveInit		0x40000000	/* Force inactive valeur init */
#define OBJ_C_ControlFlag_AnimDontTouch			0x80000000	/* Anim have not to update matrix of object */

/*$3
 =======================================================================================================================
    Design Flags
 =======================================================================================================================
 */

#define OBJ_C_DesignFlag_Friend		0x01	/* Friend of the main character */
#define OBJ_C_DesignFlag_Enemy		0x02	/* Enemy of the main character */
#define OBJ_C_DesignFlag_Projectile 0x04	/* Projectile */
#define OBJ_C_DesignFlag_Custom1	0x08	/* ??? */
#define OBJ_C_DesignFlag_Custom2	0x10	/* ??? */
#define OBJ_C_DesignFlag_Custom3	0x20	/* ??? */
#define OBJ_C_DesignFlag_Custom4	0x40	/* ??? */
#define OBJ_C_DesignFlag_Custom5	0x80	/* ??? */


/*$3
 =======================================================================================================================
    Extra Flags
 =======================================================================================================================
 */

#define OBJ_C_ExtraFlag_OptimAnim		0x0001	/* Optimize Anim Player (only Magic Box is played) */
#define OBJ_C_ExtraFlag_OptimCulled		0x0004	/* Do not play events an ai if object is not visible */
#define OBJ_C_ExtraFlag_LODai			0x0008	/* Activate lod distance for ai */
#define OBJ_C_ExtraFlag_COLPriority		0x0010	/* An object that has this Flag cannot be moved by a Dyna object that collides him (Passants)*/
#define OBJ_C_ExtraFlag_EndBone			0x0020	/* Indicates that this GO is at the end of one extremity of a skeleton (Head, hands ..) */
#define OBJ_C_ExtraFlag_SectActVis		0x0040	/* SectInactive if SectInvisible */
#define OBJ_C_ExtraFlag_SkipAnimT		0x0080	/* This GameObject dont use Translation animation info (Same animations can be used on other skel) */
#define OBJ_C_ExtraFlag_SlowFastOff		0x0100	/* Desactivates the Slow/Fast Collision for this GO */
#define OBJ_C_ExtraFlag_SkipTOnBone		0x0200
#define OBJ_C_ExtraFlag_SkipROnBone		0x0400
#define OBJ_C_ExtraFlag_VecBlendRot		0x0800
#define OBJ_C_ExtraFlag_NoBlendBone		0x1000
#define OBJ_C_ExtraFlag_AlwaysPlay		0x2000	/* Always play animation, even if culled */
#define OBJ_C_ExtraFlag_DontReinitPos	0x4000

#ifdef JADEFUSION
#define OBJ_C_ExtraFlag_AffectedByStaticWind     0x4000
#define OBJ_C_ExtraFlag_AffectedByDynamicWind    0x8000
#endif

/*$3
 =======================================================================================================================
    Editor flags
 =======================================================================================================================
 */

#ifdef ACTIVE_EDITORS
#define OBJ_C_EditFlags_Selected	0x00000001	/* Object is selected */

/*
 * Object musn't be destroyed when world is destroyed this flag is just used by
 * world importation never use it for anything else
 */
#define OBJ_C_EditFlags_NoDelete					0x00000002
#define OBJ_C_EditFlags_ShowBV						0x00000004
#define OBJ_C_EditFlags_ShowAltBV					0x00000008
#ifdef _XENON_RENDER
#define OBJ_C_EditFlags_ShowElementBV			    0x00000020
#endif

#define OBJ_C_EditFlags_Trace						0x00000010
#define OBJ_C_EditFlags_Hidden						0x00000040
#define OBJ_C_EditFlags_ShowGizmo					0x00000080

#define OBJ_C_EditFlags_NoSubObjectMode				0x00000100
#define OBJ_C_EditFlags_GizmoObject					0x00000200
#define OBJ_C_EditFlags_ForceLOD					0X00000400
#define OBJ_C_EditFlags_DrawSkeleton				0x00000800

#define OBJ_C_EditFlags_HideSkin					0x00001000
#define OBJ_C_EditFlags_WiredCob					0x00002000
#define OBJ_C_EditFlags_Ghost						0x00008000

#define OBJ_C_EditFlags_HiddenCob					0x00010000
#define OBJ_C_EditFlags_GhostODE					0x00020000
#define OBJ_C_EditFlags_ForceDisplayInColMapMode	0x00040000
#define OBJ_C_EditFlags_TraceMove					0x00080000

#define OBJ_C_EditFlags_RLINotReset					0x00100000
#define OBJ_C_EditFlags_RLILocked                   0x00200000
#define OBJ_C_EditFlags_ShowSlope					0x00400000

#define OBJ_C_EditFlags_Radiosity_Channel_1			0x00800000
#define OBJ_C_EditFlags_Radiosity_Channel_2			0x01000000
#define OBJ_C_EditFlags_Radiosity_Channel_3			0x02000000
#define OBJ_C_EditFlags_Radiosity_Channel_4			0x04000000
#define OBJ_C_EditFlags_Radiosity_Channel_5			0x08000000
#define OBJ_C_EditFlags_Radiosity_Channel_6			0x00004000
#define OBJ_C_EditFlags_Radiosity_Channel_7			0x00000080
#define OBJ_C_EditFlags_Radiosity_Channel_ALL			(OBJ_C_EditFlags_Radiosity_Channel_1|OBJ_C_EditFlags_Radiosity_Channel_2|OBJ_C_EditFlags_Radiosity_Channel_3|OBJ_C_EditFlags_Radiosity_Channel_4|OBJ_C_EditFlags_Radiosity_Channel_5|OBJ_C_EditFlags_Radiosity_Channel_6|OBJ_C_EditFlags_Radiosity_Channel_7 )

#define OBJ_C_EditFlags_Radiosity_TRANSPARENT		0x10000000
#define OBJ_C_EditFlags_Radiosity_DONTCUT			0x20000000
#define OBJ_C_EditFlags_Radiosity_Nopenombria		0x40000000
#define OBJ_C_EditFlags_Radiosity_DONTCUTOTHER		0x80000000


#define OBJ_C_EditorFlags_ToBeLoaded	(OBJ_C_EditFlags_ShowBV | OBJ_C_EditFlags_Hidden | OBJ_C_EditFlags_WiredCob | OBJ_C_EditFlags_ShowAltBV |OBJ_C_EditFlags_Radiosity_TRANSPARENT | OBJ_C_EditFlags_Radiosity_DONTCUT | OBJ_C_EditFlags_Radiosity_Nopenombria | OBJ_C_EditFlags_Radiosity_DONTCUTOTHER | OBJ_C_EditFlags_Radiosity_Channel_ALL)

/*$3
 =======================================================================================================================
    Additionnal extra flags
 =======================================================================================================================
 */
#define OBJ_C_EdAddFlags_HideBV						0x00000001
#define OBJ_C_EdAddFlags_TooManyLights				0x00000002
#define OBJ_C_EdAddFlags_Highlighted				0x00000004
#define OBJ_C_EdAddFlags_HiddenByWowFilter			0x00000008


/* macro to manage Editor additionnal flags */
#define OBJ_M_EdAddFlags_RefreshBeforeDisplay( GO ) GO->ul_AdditionalFlags &= ~(OBJ_C_EdAddFlags_HideBV)
#define OBJ_M_EdAddFlags_Set( GO, FLAG ) GO->ul_AdditionalFlags |= FLAG

#ifdef JADEFUSION
// -NOTE-
// define used to store information in GAO about what to merge from PS2 data in the GAO (BV, Position, etc... )
// This used by the P4 integrate script. User can also flag gao properties that they don't want overwritten.
#define OBJ_C_Merge_KeepXe_BV						0x00000001
#define OBJ_C_Merge_KeepXe_Position					0x00000002
#define OBJ_C_Merge_KeepXe_KeyGRO					0x00000004
#define OBJ_C_Merge_KeepXe_KeyRLI					0x00000008
#endif

#else /* ACTIVE_EDITORS */

/* macro to manage Editor additionnal flags */
#define OBJ_M_EdAddFlags_RefreshBeforeDisplay( GO )
#define OBJ_M_EdAddFlags_Set( GO, FLAG )

#endif /* ACTIVE_EDITORS */

/*$3
 =======================================================================================================================
    Flags for fix
 =======================================================================================================================
 */

#define OBJ_C_FixNone		0x00
#define OBJ_C_HasBeenMerge	0x10
#define OBJ_C_ProcessedAI	0x20
#define OBJ_C_ProcessedDyn	0x40
#define OBJ_C_ProcessedHie	0x80

/*$3
 =======================================================================================================================
    Other Constants related to objects
 =======================================================================================================================
 */

#define OBJ_C_DefaultStatusAndControlFlags	0
#define OBJ_C_DefaultIdentityFlags			0	/* By default, no identity flags are set */
#define OBJ_C_UnvalidFlags					0xffffffff
#define OBJ_C_BV_AABBox						0
#define OBJ_C_BV_Sphere						1
#define OBJ_C_BV_CurrentType				2
#define OBJ_C_BV_ForceComputation			0
#define OBJ_C_BV_ComputeIfNotLoaded			1
#define OBJ_C_Gizmo_MaxLenName				64	/* Max length of a gizmo name */

#define OBJ_C_OR_OR_IdentityFlags			0
#define OBJ_C_OR_OR_TypeFlags				1
#define OBJ_C_OR_OR_DesignFlags				2
#define OBJ_C_OR_OR_Capacities				3
#define OBJ_C_OR_OR_DesignFlags2			4
#define OBJ_C_OR_OR_Custom					5

#define OBJ_C_AND_OR_IdentityFlags			10
#define OBJ_C_AND_OR_TypeFlags				11
#define OBJ_C_AND_OR_DesignFlags			12
#define OBJ_C_AND_OR_Capacities				13
#define OBJ_C_AND_OR_DesignFlags2			14
#define OBJ_C_AND_OR_Custom					15

#endif /* __OBJ_CONST_H__ */