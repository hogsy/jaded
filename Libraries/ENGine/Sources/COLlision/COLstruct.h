/*$T COLstruct.h GC! 1.081 08/09/00 11:43:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __COL_STRUCT__
#define __COL_STRUCT__

/*$F GC Dependencies 11/22/99 */
#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"

#ifndef PSX2_TARGET
#pragma once
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplus */

/*$F
 -----------------------------------------------------------------------------------------------------------------------
										Game Material (16 bytes)
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_GameMat_
{
	ULONG	ul_CustomBits;
	float	f_Slide;
	float	f_Rebound;
	UCHAR	uc_Sound;
	UCHAR	uc_Dummy;
	USHORT	uw_Dummy;
#ifdef ACTIVE_EDITORS
	char	asz_Comment[64];
	ULONG	ul_Color;
	ULONG	ul_SoundColor;
	BOOL	b_Display;
    BOOL    b_Transparent;
#endif
} COL_tdst_GameMat;

/*$F
 -----------------------------------------------------------------------------------------------------------------------
											Game Material List
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_GameMatList_
{
	ULONG				ul_GMat;
	ULONG				*pal_Id;
	COL_tdst_GameMat	*pst_GMat;
	ULONG				ul_NbOfInstances;
} COL_tdst_GameMatList;

/*$F
 -----------------------------------------------------------------------------------------------------------------------
											Collision Report (64 bytes)
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_Report_
{
	struct OBJ_tdst_GameObject_ *pst_A;
	struct OBJ_tdst_GameObject_ *pst_B;
	MATH_tdst_Vector			st_CollidedPoint;	/* Collision Point in GCS */
	MATH_tdst_Vector			st_Recal;			/* "Recalage" needed for this collision in GCS. */
	MATH_tdst_Vector			st_Normal;			/* Normal of the collided Triangle/Edge. */
	struct COL_tdst_GameMat_	*pst_GM;			/* Collided Game Material. */
	ULONG						ul_Flag;			/* Was it a collision with a triangle, an edge */
	ULONG						ul_Triangle;		/* Index of the Collided Triangle in the Collided Element */
	USHORT						uw_Element;			/* Index of the Collided Element */
	UCHAR						uc_Design;
	UCHAR						uc_Dummy;
} COL_tdst_Report;

/*$F
 ---------------------------------------------------------------------------------------------------
											Basic Zones
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_Sphere_
{
	MATH_tdst_Vector	st_Center;
	float				f_Radius;
} COL_tdst_Sphere;

/*$F --------------------------------------------------------------------------------------------------- */

typedef struct	COL_tdst_Box_
{
	MATH_tdst_Vector	st_Max;
	MATH_tdst_Vector	st_Min;
} COL_tdst_Box;

/*$F --------------------------------------------------------------------------------------------------- */

typedef struct	COL_tdst_Cylinder_
{
	MATH_tdst_Vector	st_Center;
	float				f_Radius;
	float				f_Height;
} COL_tdst_Cylinder;

/*$F --------------------------------------------------------------------------------------------------- */

typedef struct	COL_tdst_Cone_
{
	MATH_tdst_Vector	st_Base;
	float				f_Angle;
	float				f_Height;
} COL_tdst_Cone;

/*$F --------------------------------------------------------------------------------------------------- */

typedef struct	COL_tdst_Pane_
{
	MATH_tdst_Vector	st_Normal;
	MATH_tdst_Vector	st_Center;
	float				f_Length;
	float				f_Height;
} COL_tdst_Pane;

/*$F
 ---------------------------------------------------------------------------------------------------
									Generic ZDx (8 bytes)
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_ZDx_
{
	UCHAR						uc_Flag;		/* Flags related to the Zone */
	UCHAR						uc_Type;		/* Type of Zone (Sphere/Box/Cylinder/Cone/Triangles/Plan */
	UCHAR						uc_BoneIndex;	/* Index of the bone that is linked to the Zone. */
	UCHAR						uc_Design;		/* Design Action */
	void						*p_Shape;		/* Pointer on the Shape of the zone: Sphere, Box */

#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_ *pst_GO;
	struct COL_tdst_ZDx_		*pst_Itself;
	char						*sz_Name;
	struct COL_tdst_ZDx_		*pst_ColSetZDx; /* Old ColSet ZDx from which this Zone is derived */
	UCHAR						uc_AI_Index;	/* Index of the ZDx in the IA Module. */
#endif
} COL_tdst_ZDx;

/*$F
 ---------------------------------------------------------------------------------------------------
								  ColSet structure (16 bytes)
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_ColSet_
{
	UCHAR						uc_NbOfZDx;			/* Number of zones: 16 Max */
	UCHAR						uc_Flag;			/* Flag of the ColSet */
	USHORT						uw_NbOfInstances;	/* Number of instances that have this ColSet */
	COL_tdst_ZDx				*past_ZDx;			/* Array of zones */
	UCHAR						*pauc_AI_Indexes;	/* Array to translate AI indexes to the real ones */
	ULONG						ul_Dummy;			/* Dummy */

#ifdef ACTIVE_EDITORS
	struct COL_tdst_Instance_	**dpst_Instances;	/* List of pointers to collision instances that have this ColSet */
#endif
} COL_tdst_ColSet;

/*$F
 ---------------------------------------------------------------------------------------------------
									 Instance structure (32 octets)
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_Instance_
{
	COL_tdst_ColSet		*pst_ColSet;			/* Pointer on the ColSet Model */

	UCHAR				uc_NbOfZDx;				/* Total number of ZDxs */
	UCHAR				uc_NbOfShared;			/* Number of shared zones */
	UCHAR				uc_NbOfSpecific;		/* Number of specific zones */
	char				c_Priority;				/* Priority, who moves who */

	COL_tdst_ZDx		**dpst_ZDx;				/* Array of pointers to all ZDx. */
	COL_tdst_ZDx		*past_Specific;			/* Array of COL_tdst_ZDx structures */

	USHORT				uw_Activation;			/* Activation/desactivation zone BitField */
	USHORT				uw_Specific;			/* Specific/Shared zone BitField */

	USHORT				uw_Crossable;			/* Bits that define what kind of faces the GO can cross without recalage */
	USHORT				uw_Flags;				/* Flags */

	UCHAR				uc_MaxLOD;				/* Max LOD for instance */
	UCHAR				uc_MinLOD;				/* Min LOD for instance */
	UCHAR				uc_LOD;					/* Current LOD for instance */
	UCHAR				uc_UserBackFace;		/* InfoDesigners may want to specify their own BackFace vector */

	ULONG				ul_Filter_On;
	ULONG				ul_Filter_Off;
	ULONG				ul_Filter_Type;

	MATH_tdst_Vector	st_LastWall_Normal;

	BOOL				b_InTheWall;			/* Is the GameObject recaled into the wall after the collision (Not to lose the collision info the next frames) */

	MATH_tdst_Matrix	*pst_OldGlobalMatrix;	/* Global Matrix of the previous frame */
	MATH_tdst_Vector	st_BackFace;

} COL_tdst_Instance;

/*$F
 ---------------------------------------------------------------------------------------------------
								Detection list created after the SnP.
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_DetectionList_
{
	union
	{
		struct OBJ_tdst_GameObject_ **dpst_CollidedObject;
		struct OBJ_tdst_GameObject_ **dpst_UnCollidableObject;
	};

	union
	{
		ULONG						ul_NbCollidedObjects;
		ULONG						ul_NbUnCollidableObjects;	
	};
} COL_tdst_DetectionList;

/*$F
 ---------------------------------------------------------------------------------------------------
									Triangles-based Cobs structures.
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_IndexedTriangle_
{
	USHORT	auw_Index[3];
	USHORT	auw_Prox[3];	/* Index of Faces that are touched by Edge12, Edge 13 and Edge23 */
} COL_tdst_IndexedTriangle;

/*$F --------------------------------------------------------------------------------------------------- */

typedef struct	COL_tdst_ElementIndexedTriangles_
{
	USHORT						uw_NbTriangles;
	UCHAR						uc_Design;
	USHORT						uc_Flag;
	LONG						l_MaterialId;
	COL_tdst_IndexedTriangle	*dst_Triangle;

#ifdef ACTIVE_EDITORS
	struct COL_tdst_Cob_		*pst_Cob;
#endif
} COL_tdst_ElementIndexedTriangles;


/*$F
 ---------------------------------------------------------------------------------------------------
							   Octree
 ---------------------------------------------------------------------------------------------------
 */

typedef struct COL_tdst_OK3_Element_
{
	USHORT	uw_Element;
	USHORT	uw_NumTriangle;
	USHORT *puw_OK3_Triangle;
} COL_tdst_OK3_Element;

typedef struct COL_tdst_OK3_Box_
{
	MATH_tdst_Vector			st_Min;
	MATH_tdst_Vector			st_Max;
	ULONG						ul_NumElement;
	COL_tdst_OK3_Element		*pst_OK3_Element;
#ifdef ACTIVE_EDITORS
	ULONG						ul_OK3_Flag;
#endif
} COL_tdst_OK3_Box;

typedef struct COL_tdst_OK3_Node_
{
	struct COL_tdst_OK3_Node_	*pst_Next;
	struct COL_tdst_OK3_Node_	*pst_Son;
	struct COL_tdst_OK3_Box_	*pst_OK3_Box;
} COL_tdst_OK3_Node;

typedef struct COL_tdst_OK3_
{
	ULONG				ul_NumBox;
	COL_tdst_OK3_Box	*pst_OK3_Boxes;
	COL_tdst_OK3_Node	*pst_OK3_God;
	ULONG				*paul_Tag;		/* To avoid OK3 faces computation redundancies (Faces can be in many boxes) */
} COL_tdst_OK3;


/*$F --------------------------------------------------------------------------------------------------- */

typedef struct	COL_tdst_IndexedTriangles_
{
	ULONG								l_NbPoints;
	ULONG								l_NbFaces;
	ULONG								l_NbElements;
	MATH_tdst_Vector					*dst_Point;
	MATH_tdst_Vector					*dst_FaceNormal;
	COL_tdst_ElementIndexedTriangles	*dst_Element;
	COL_tdst_OK3						*pst_OK3;
#ifdef ACTIVE_EDITORS
	USHORT								uw_NumCameraFaces;
#endif
} COL_tdst_IndexedTriangles;

/*$F
 ---------------------------------------------------------------------------------------------------
										Info of Collision Object.
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_Mathematical_
{
	void					*p_Shape;
	LONG					l_MaterialId;

#ifdef ACTIVE_EDITORS
	struct COL_tdst_Cob_	*pst_Cob;
#endif
} COL_tdst_Mathematical;

/*$F
 ---------------------------------------------------------------------------------------------------
										Collision Object (COB).
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_Cob_
{
	UCHAR					uc_Type;
	UCHAR					uc_Flag;
	USHORT					uw_NbOfInstances;
	COL_tdst_GameMatList	*pst_GMatList;
	union
	{
		COL_tdst_Mathematical		*pst_MathCob;
		COL_tdst_IndexedTriangles	*pst_TriangleCob;
	};

#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_ *pst_GO;
	struct COL_tdst_Cob_		*pst_Itself;
	ULONG						ul_EditedElement;
	char						*sz_GMatName;
	void						*p_GeoCob;			/* Geometric Object created to render and edit the Cob */
#endif
} COL_tdst_Cob;

/*$F
 ---------------------------------------------------------------------------------------------------
								    Collision Map of the object.
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_ColMap_
{
	UCHAR			uc_NbOfCob;		/* Number of Col Objects */
	UCHAR			uc_Activation;	/* Activation of those objects. */
	UCHAR			uc_CustomBits1; /* Customs */
	UCHAR			uc_CustomBits2; /* Customs */
	COL_tdst_Cob	**dpst_Cob;		/* Col Objects. */
} COL_tdst_ColMap;

/*$F
 ---------------------------------------------------------------------------------------------------
				 The Basic COL structure containing the list of overlapping objects (SnP) .
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_Base_
{
	COL_tdst_DetectionList	*pst_List;
	COL_tdst_Instance		*pst_Instance;
	COL_tdst_ColMap			*pst_ColMap;
	COL_tdst_DetectionList	*pst_UnCollidable;
} COL_tdst_Base;

/*$F
 ---------------------------------------------------------------------------------------------------
							   Info needed to be stored after a Ray cast.
 ---------------------------------------------------------------------------------------------------
 */

typedef struct	COL_tdst_RayInfo_
{
#ifdef ACTIVE_EDITORS
	MATH_tdst_Vector			st_Origin;
#endif
	struct OBJ_tdst_GameObject_ *pst_CollidedGO;
	MATH_tdst_Vector			st_CollidedPoint;
	MATH_tdst_Vector			st_Normal;
	MATH_tdst_Vector			st_EdgeNormal;
	UCHAR						uc_Flags;
	UCHAR						uc_Design;
	USHORT						uw_Element;
	ULONG						ul_Triangle;
	ULONG						ul_TriangleIndex;
	ULONG						ul_Sound;
	COL_tdst_GameMat			*pst_GMat;
} COL_tdst_RayInfo;


/*$F
 ---------------------------------------------------------------------------------------------------
							   CONVEX OBJECTS IDEA STRUCTURES REGISTRATION.
 ---------------------------------------------------------------------------------------------------
 */
#ifdef CONVEX_OBJECT
typedef struct	COL_tdst_Plan_
{
	MATH_tdst_Vector	*pst_A;
	MATH_tdst_Vector	st_Norm;
} COL_tdst_Plan;

typedef struct	COL_tdst_Convex_
{
	ULONG			ul_NbPlans;
	COL_tdst_Plan	*dst_Plan;
} COL_tdst_Convex;

#endif /* CONVEX_OBJECT */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplcus */
#endif /* __COL_STRUCT__ */
