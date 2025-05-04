/*$T COLconst.h GC! 1.081 06/06/01 11:02:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __COL_CONST__
#define __COL_CONST__

#ifndef PSX2_TARGET
#pragma once
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
/*$off*/
/*
 ---------------------------------------------------------------------------------------------------
    Structure Infinite Plan
 ---------------------------------------------------------------------------------------------------
 */
#define COL_Cul_MaxDefaultCollidedObjects	150
#define COL_Cf_Infinite						1E6

#define COL_Cul_MaxNbOfCollision			1500
#define COL_Cul_MaxNbOfZDx					16
#define COL_Cul_MaxNbOfCob					8
#define COL_Cul_ColMap						0
#define COL_Cul_ColSet						1


/*
 ---------------------------------------------------------------------------------------------------
    Structure Infinite Plan
 ---------------------------------------------------------------------------------------------------
 */
#define COL_C_ColSet_AIIndexes				0x1


/*
 ---------------------------------------------------------------------------------------------------
    ZDx Flags
 ---------------------------------------------------------------------------------------------------
 */
#define COL_C_Zone_ZDM						0x01
#define COL_C_Zone_ZDE						0x04
#define COL_C_Zone_NoScale					0x08
#define COL_C_Zone_FlagX					0x10
#define COL_C_Zone_Specific					0x40
#define COL_C_Zone_Camera					0x80

/*
 ---------------------------------------------------------------------------------------------------
    Cobs Flags
 ---------------------------------------------------------------------------------------------------
 */
#define COL_C_Cob_GameMat						0x01
#define COL_C_Cob_ReadyForCamera				0x02	/* If this tag is set, the camera only collide Camera faces. If not, the camera collide every face */
#define COL_C_Cob_Movable						0x04
#define COL_C_Cob_OK3							0x08
#define COL_C_Cob_WallAlgorithm					0x10
#define COL_C_Cob_DisableColOptim				0x20
#define COL_C_RealTimeComputation				0x40

	/* -- Only Active Editors ---*/
#define COL_C_Cob_Updated						0x80	/* The current Cob has been edited ... --> Need an Update */


/*
 ---------------------------------------------------------------------------------------------------
    ZDx and ColMap types.
 ---------------------------------------------------------------------------------------------------
 */
#define COL_C_Zone_Box						1
#define COL_C_Zone_Sphere					2
#define COL_C_Zone_Cylinder					3
#define COL_C_Zone_Cone						4
#define COL_C_Zone_Triangles				5
#define COL_C_Zone_Plan						6



/*
 ---------------------------------------------------------------------------------------------------
    ZDE versus ZDE ... All the combinations.
 ---------------------------------------------------------------------------------------------------
 */

#define COL_C_SphereAndSphere				0x22
#define COL_C_SphereAndBox					0x21
#define COL_C_BoxAndSphere					0x12
#define COL_C_BoxAndBox						0x11


/*
 ---------------------------------------------------------------------------------------------------
    Collision Reports Flags
 ---------------------------------------------------------------------------------------------------
 */
#define COL_Cul_Triangle					0x00000001
#define COL_Cul_Edge12						0x00000002
#define COL_Cul_Edge13						0x00000004
#define COL_Cul_Edge23						0x00000008
#define COL_Cul_Edge						(COL_Cul_Edge12 | COL_Cul_Edge13 | COL_Cul_Edge23)
#define COL_Cul_Dynamic						0x00000010
#define COL_Cul_Static						0x00000020
#define COL_Cul_Ground						0x00000040
#define COL_Cul_Wall						0x00000080
#define COL_Cul_Sphere						0x00000100
#define COL_Cul_Box							0x00000200
#define COL_Cul_Cylinder					0x00000400
#define COL_Cul_SlipperyEdge				0x00000800
#define COL_Cul_Inactive					0x00001000
#define COL_Cul_Crossable					0x00002000
#define COL_Cul_ZDMBox						0x00004000

#define COL_Cul_Extra_Mask					0x000F0000
#define COL_Cul_Extra_Corner				0x00010000	/* Extra Flag */
#define COL_Cul_Extra_SlipperyEdge			0x00020000	/* Extra Flag */
#define COL_Cul_Extra_Minor					0x00040000	/* Extra Flag */
#define COL_Cul_Extra_ODE					0x00080000	/* Extra Flag */


#define COL_Cul_Invalid						0x80000000		/* Those reports will NEVER be used by AI. The Inactive ones can be. */


#define COL_Cul_ReportIndex					0x80000000
#define COL_Cul_ReportIndexMask				0x0000FFFF
#define COL_Cul_IgnoreEdgeWall				0x40000000


/*
 ---------------------------------------------------------------------------------------------------
    Game Materials Flags
 ---------------------------------------------------------------------------------------------------
 */

#define COL_Cul_GMat_FlagX					0x00200000
#define COL_Cul_GMat_Camera					0x00001000


/*
 ---------------------------------------------------------------------------------------------------
    Instance Flags
 ---------------------------------------------------------------------------------------------------
 */

#define COL_Cul_CornerComputed					0x0001
#define COL_Cul_CornerFound						0x0002
#define COL_Cul_StaticGround					0x0004
#define COL_Cul_LastCollisionLoop				0x0008

#define COL_Cul_Recompute_Mask					0x03F0
#define COL_Cul_Recompute_GroundAndWall			0x0010
#define COL_Cul_Recompute_GroundEdge			0x0020
#define COL_Cul_Recompute_Wall					0x0040
#define COL_Cul_Recompute_Dynamic				0x0080
#define COL_Cul_Recompute_SlowFast				0x0100
#define COL_Cul_Recompute_Force					0x0200
#define COL_Cul_Recompute_TriangleEdge			0x0400

#define COL_Cul_WallDetected					0x0800

#define COL_Cul_ForceRecomputeWhenOnlyGround	0x4000
#define COL_Cul_OnlyGravity						0x8000


/*
 ---------------------------------------------------------------------------------------------------
    Ray Casting
 ---------------------------------------------------------------------------------------------------
 */

#define COL_Cul_Ray							0xFFFF


/*
 ---------------------------------------------------------------------------------------------------
    SnP Refresh Type
 ---------------------------------------------------------------------------------------------------
 */

#define COL_SnP_RefreshXYZ					0
#define COL_SnP_RefreshXY					1
#define COL_SnP_RefreshZ					2

/*
 ---------------------------------------------------------------------------------------------------
    Miscellaneous Default Flags
 ---------------------------------------------------------------------------------------------------
 */
//#define COL_Cul_DefaultCross			0x589A
#define COL_Cul_DefaultCross			0x0
#define COL_Cul_MaxCrossed				16


/*$on*/
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
