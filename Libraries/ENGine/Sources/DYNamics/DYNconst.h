/*$T DYNconst.h GC! 1.081 10/30/00 10:50:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Constants of the world module */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __DYNCONST_H__
#define __DYNCONST_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$3
 =======================================================================================================================
    Aim:    Flags of the Dynamic (field ul_DynFlags)
 =======================================================================================================================
 */

/* Forces Info */
#define DYN_C_NoForces			0x00000001		/* No forces are used to control the movement, the speed is used
												 * directly as a command */
#define DYN_C_OneConstantForce	0x00000002		/* No ODE to solve , only one constant force is used */
#define DYN_C_BasicForces		0x00000004		/* ODE will be solved using true solution */
#define DYN_C_ComplexForces		0x00000008		/* ODE will be solved using numerical methods */

#define DYN_C_SkipFrictionWhenSpeedFromAnim		0x00000010
#define DYN_C_SlipOnGroundEdges					0x00000020


/* AdditionnalFlags */
#define DYN_C_AutoOrient			0x00000200	/* The orientation of the matrix follows the Speed */
#define DYN_C_AutoOrientInertia1	0x00000400	/* The 2 following bits define the amount of inertia in the auto
												 * orientation:
												 * £
												 * 00 = No inertia
												 * £
												 * 01 = Low inertia
												 * £
												 * 10 = Medium Inertia
												 * £
												 * 11 = High Inertia */
#define DYN_C_AutoOrientInertia2	0x00000800

#define DYN_C_VectorFriction		0x00001000	/* If set, the friction vector is used instead of the float */

#define DYN_C_AutoOrientHorizontal	0x00002000	/* If set the auto orient cannot set a non Horizontal sight */
#define DYN_C_ApplyMaxPos			0x00004000	/* Apply maximum position */

#define DYN_C_NeverDynamicFather	0x00008000	

#define DYN_C_Solid					0x00010000	/* Solid flag: indicates if the object is handled as a solid (with
												 * rotations) or a point (no rotations) by the dynamics engine */
#define DYN_C_Col					0x00020000	/* Indicates that the object has a DYN_tdst_Col structure */
#define DYN_C_IgnoreGravity			0x00040000	/* Ignore the gravity force */
#define DYN_C_IgnoreTraction		0x00080000	/* Ignore the traction force */
#define DYN_C_IgnoreStream			0x00100000	/* Ignore the stream force */
#define DYN_C_IgnoreForces			0x001C0000	/* Ignore all forces, and keep the speed given by anim or AI */
#define DYN_C_Constraint			0x00200000	/* Point or Solid constrained on segment or parabol */

#define DYN_C_OptimizeColEnable		0x00400000	/* Flag that indicates that this object has only dropped due to Gravity */
#define DYN_C_OptimizeColDisable	0x00800000	/* Flag that indicates that this object has only dropped due to Gravity */

#define DYN_C_NeverDynamicHierarchy	0x01000000	/* This GameObject will never have dynamic hierarchy */
#define DYN_C_ApplyRec				0x02000000	/* My speed can be updated by an object that collides me */

#define DYN_C_NoSpeedFactor			0x04000000	/* This object is unsensitive to the Speed Factor set by IA */

#define DYN_C_GlobalFriction		0x08000000	/* Friction is used in Global , not in Local coordinate system */

#define DYN_C_FuckMeca				0x10000000	/* Calcul simpliste meca : pos = pos + friction, .point barre */

#define DYN_C_AfterEngineCall		0x20000000	/* Dont want the current GO to be computed within classical DYN, COL and REC Module (AfterRec Cam for example) */

#define DYN_C_HorizontalGrounds		0x40000000	/* In REC Module, ground collisions for this GO are always considered as horizontal */

#define DYN_C_NoReboundOnGround		0x80000000	/* No Rebound on Grounds for this GO */



/* Default flags */
#define DYN_C_DefaultDynaFlags		0x00000041



#ifdef ACTIVE_EDITORS   

/*$3
 =======================================================================================================================
    Aim:    Strings used for editing the fields
 =======================================================================================================================
 */

#define DYN_Csz_MassString					"Mass"
#define DYN_Csz_SpeedVectorString			"Speed vector"
#define DYN_Csz_SumOfForces					"Sum of forces"
#define DYN_Csz_BasicForces					"Basic Forces"
#define DYN_Csz_FrictionString				"Friction"
#define DYN_Csz_SpeedLimitHorizString		"Horiz Speed Limit"
#define DYN_Csz_SpeedLimitVertString		"Vertical Speed Limit"
#define DYN_Csz_VectorFrictionString		"Vector Friction"
#define DYN_Csz_GravityString				"Gravity"
#define DYN_Csz_TractionString				"Traction"
#define DYN_Csz_StreamString				"Stream"
#define DYN_Csz_SolidString					"Solid"
#define DYN_Csz_ColString					"Col"
#define DYN_Csz_ConstraintString			"Constraint"
#define DYN_Csz_ReboundHorizCosAngleString	"ReboundHorizCosAngle"
#define DYN_Csz_SlideHorizCosAngleString	"SlideHorizCosAngle"
#define DYN_Csz_ReboundString				"Rebound"
#define DYN_Csz_SlideString					"Slide"

/*$3
 =======================================================================================================================
    Aim:    Offsets of the fields in the dynamics structure
 =======================================================================================================================
 */

#define DYN_Cul_OffsetP				8
#define DYN_Cul_OffsetF				20
#define DYN_Cul_OffsetInvMass		32
#define DYN_Cul_OffsetMass			64
#define DYN_Cul_OffsetSpeedVector	68

/*$3
 =======================================================================================================================
    Values for the auto-orient inertia
 =======================================================================================================================
 */

#endif
#define DYN_C_AutoOrientInertiaNo		0
#define DYN_C_AutoOrientInertiaLow		DYN_C_AutoOrientInertia1
#define DYN_C_AutoOrientInertiaMedium	DYN_C_AutoOrientInertia2
#define DYN_C_AutoOrientInertiaHi		(DYN_C_AutoOrientInertia1 + DYN_C_AutoOrientInertia2)

/*$3
 =======================================================================================================================
    Condstants for rebound and Slide
 =======================================================================================================================
 */

#define DYN_C_DefaultRebound				0
#define DYN_C_DefaultSlide					1
#define DYN_C_DefaultReboundHorizCosAngle	1
#define DYN_C_DefaultSlideHorizCosAngle		1
#define DYN_C_MinRebound					0.01f	/* If the rebound vector does'nt have this norm, it won't be applied */
#define DYN_C_SlideMode						0
#define DYN_C_ReboundMode					1

/*$3
 =======================================================================================================================
    Constraints related constants
 =======================================================================================================================
 */

#define DYN_C_NoConstraint		0
#define DYN_C_ConstraintSegment 1
#define DYN_C_ConstraintParabol 2

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __DYNCONST_H__ */ 
 