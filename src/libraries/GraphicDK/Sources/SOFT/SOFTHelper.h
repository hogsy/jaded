/*$T SOFTHelper.h GC! 1.081 06/30/00 14:46:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __SOFTHELPER_H__
#define __SOFTHELPER_H__

#include "BASe/BAStypes.h"
#include "SOFT/SOFTstruct.h"
#include "SOFT/SOFTUVgizmo.h"
#include "ENGine/Sources/WORld/WORsecto.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* Helper flags */
#define SOFT_Cul_HF_NoMovers					0x00000001
#define SOFT_Cul_HF_Rotate						0x00000002
#define SOFT_Cul_HF_Scale						0x00000004
#define SOFT_Cul_HF_Move						0x00000008
#define SOFT_Cul_HF_SACamera					0x00000010
#define SOFT_Cul_HF_SAObject					0x00000020
#define SOFT_Cul_HF_SAWorld						0x00000040
#define SOFT_Cul_HF_ConstraintXY				0x00000080
#define SOFT_Cul_HF_ConstraintXZ				0x00000100
#define SOFT_Cul_HF_ConstraintYZ				0x00000200
#define SOFT_Cul_HF_LocalMode					0x00000400
#define SOFT_Cul_HF_SnapAngle					0x00000800
#define SOFT_Cul_HF_AllTogether					0x00001000
#define SOFT_Cul_HF_LocalCenterForRotation		0x00002000
#define SOFT_Cul_HF_SeparateCenterForRotation	0x00004000
#define SOFT_Cul_HF_HideWhileInUse				0x00010000
#define SOFT_Cul_HF_SnapGrid                    0x00020000
#define SOFT_Cul_HF_SnapGridRender              0x00040000
#define SOFT_Cul_HF_SnapGridDisplay             0x00080000
#define SOFT_Cul_HF_HideHelperToolTip           0x00100000

#define SOFT_Cul_HF_ConstraintMask				(SOFT_Cul_HF_ConstraintXY | SOFT_Cul_HF_ConstraintXZ | SOFT_Cul_HF_ConstraintYZ)
#define SOFT_Cul_HF_MoveMask					(SOFT_Cul_HF_Rotate | SOFT_Cul_HF_Scale | SOFT_Cul_HF_Move)
#define SOFT_Cul_HF_SAMask						(SOFT_Cul_HF_SACamera | SOFT_Cul_HF_SAObject | SOFT_Cul_HF_SAWorld)

/* Helper id */
#define SOFT_Cl_Helpers_RotationLocalX		0x00000001
#define SOFT_Cl_Helpers_RotationLocalY		0x00000002
#define SOFT_Cl_Helpers_RotationLocalZ		0x00000003
#define SOFT_Cl_Helpers_RotationGlobalX		0x00000004
#define SOFT_Cl_Helpers_RotationGlobalY		0x00000005
#define SOFT_Cl_Helpers_RotationGlobalZ		0x00000006
#define SOFT_Cl_Helpers_RotationCameraX		0x00000007
#define SOFT_Cl_Helpers_RotationCameraY		0x00000008
#define SOFT_Cl_Helpers_RotationCameraZ		0x00000009
#define SOFT_Cl_Helpers_RotationSphere		0x0000000A

#define SOFT_Cl_Helpers_MoveLocalX			0x0000000B
#define SOFT_Cl_Helpers_MoveLocalY			0x0000000C
#define SOFT_Cl_Helpers_MoveLocalZ			0x0000000D
#define SOFT_Cl_Helpers_MoveGlobalX			0x0000000E
#define SOFT_Cl_Helpers_MoveGlobalY			0x0000000F
#define SOFT_Cl_Helpers_MoveGlobalZ			0x00000010
#define SOFT_Cl_Helpers_MoveCameraX			0x00000011
#define SOFT_Cl_Helpers_MoveCameraY			0x00000012
#define SOFT_Cl_Helpers_MoveCameraZ			0x00000013
#define SOFT_Cl_Helpers_MoveLocalXY			0x00000014
#define SOFT_Cl_Helpers_MoveLocalXZ			0x00000015
#define SOFT_Cl_Helpers_MoveLocalYZ			0x00000016
#define SOFT_Cl_Helpers_MoveGlobalXY		0x00000017
#define SOFT_Cl_Helpers_MoveGlobalXZ		0x00000018
#define SOFT_Cl_Helpers_MoveGlobalYZ		0x00000019
#define SOFT_Cl_Helpers_MoveCameraXY		0x0000001A
#define SOFT_Cl_Helpers_MoveCameraXZ		0x0000001B
#define SOFT_Cl_Helpers_MoveCameraYZ		0x0000001C

#define SOFT_Cl_Helpers_ScaleLocalX			0x0000001D
#define SOFT_Cl_Helpers_ScaleLocalY			0x0000001E
#define SOFT_Cl_Helpers_ScaleLocalZ			0x0000001F
#define SOFT_Cl_Helpers_ScaleLocalXY		0x00000020
#define SOFT_Cl_Helpers_ScaleLocalXZ		0x00000021
#define SOFT_Cl_Helpers_ScaleLocalYZ		0x00000022
#define SOFT_Cl_Helpers_ScaleLocalXYZ		0x00000023

#define SOFT_Cl_Helpers_OmniNear1			0x00000024
#define SOFT_Cl_Helpers_OmniNear2			0x00000025
#define SOFT_Cl_Helpers_OmniNear3			0x00000026
#define SOFT_Cl_Helpers_OmniNear4			0x00000027
#define SOFT_Cl_Helpers_OmniNear5			0x00000028
#define SOFT_Cl_Helpers_OmniNear6			0x00000029
#define SOFT_Cl_Helpers_OmniNear7			0x0000002A
#define SOFT_Cl_Helpers_OmniNear8			0x0000002B

#define SOFT_Cl_Helpers_OmniFar1			0x0000002C
#define SOFT_Cl_Helpers_OmniFar2			0x0000002D
#define SOFT_Cl_Helpers_OmniFar3			0x0000002E
#define SOFT_Cl_Helpers_OmniFar4			0x0000002F
#define SOFT_Cl_Helpers_OmniFar5			0x00000030
#define SOFT_Cl_Helpers_OmniFar6			0x00000031
#define SOFT_Cl_Helpers_OmniFar7			0x00000032
#define SOFT_Cl_Helpers_OmniFar8			0x00000033

#define SOFT_Cl_Helpers_SpotNear1			0x00000034
#define SOFT_Cl_Helpers_SpotNear2			0x00000035
#define SOFT_Cl_Helpers_SpotNear3			0x00000036
#define SOFT_Cl_Helpers_SpotNear4			0x00000037
#define SOFT_Cl_Helpers_SpotNear5			0x00000038
#define SOFT_Cl_Helpers_SpotNear6			0x00000039
#define SOFT_Cl_Helpers_SpotNear7			0x0000003A
#define SOFT_Cl_Helpers_SpotNear8			0x0000003B

#define SOFT_Cl_Helpers_SpotLittleAlpha1	0x0000003C
#define SOFT_Cl_Helpers_SpotLittleAlpha2	0x0000003D
#define SOFT_Cl_Helpers_SpotLittleAlpha3	0x0000003E
#define SOFT_Cl_Helpers_SpotLittleAlpha4	0x0000003F
#define SOFT_Cl_Helpers_SpotLittleAlpha5	0x00000040
#define SOFT_Cl_Helpers_SpotLittleAlpha6	0x00000041
#define SOFT_Cl_Helpers_SpotLittleAlpha7	0x00000042
#define SOFT_Cl_Helpers_SpotLittleAlpha8	0x00000043

#define SOFT_Cl_Helpers_SpotFar1			0x00000044
#define SOFT_Cl_Helpers_SpotFar2			0x00000045
#define SOFT_Cl_Helpers_SpotFar3			0x00000046
#define SOFT_Cl_Helpers_SpotFar4			0x00000047
#define SOFT_Cl_Helpers_SpotFar5			0x00000048
#define SOFT_Cl_Helpers_SpotFar6			0x00000049
#define SOFT_Cl_Helpers_SpotFar7			0x0000004A
#define SOFT_Cl_Helpers_SpotFar8			0x0000004B

#define SOFT_Cl_Helpers_SpotBigAlpha1		0x0000004C
#define SOFT_Cl_Helpers_SpotBigAlpha2		0x0000004D
#define SOFT_Cl_Helpers_SpotBigAlpha3		0x0000004E
#define SOFT_Cl_Helpers_SpotBigAlpha4		0x0000004F
#define SOFT_Cl_Helpers_SpotBigAlpha5		0x00000050
#define SOFT_Cl_Helpers_SpotBigAlpha6		0x00000051
#define SOFT_Cl_Helpers_SpotBigAlpha7		0x00000052
#define SOFT_Cl_Helpers_SpotBigAlpha8		0x00000053

#define SOFT_Cl_Helpers_Curve1				0x00000054
#define SOFT_Cl_Helpers_Curve2				0x00000055
#define SOFT_Cl_Helpers_Curve3				0x00000056
#define SOFT_Cl_Helpers_Curve4				0x00000057
#define SOFT_Cl_Helpers_Curve5				0x00000058
#define SOFT_Cl_Helpers_Curve6				0x00000059
#define SOFT_Cl_Helpers_Curve7				0x0000005A
#define SOFT_Cl_Helpers_Curve8				0x0000005B
#define SOFT_Cl_Helpers_Curve9				0x0000005C
#define SOFT_Cl_Helpers_Curve10				0x0000005D
#define SOFT_Cl_Helpers_Curve11				0x0000005E
#define SOFT_Cl_Helpers_Curve12				0x0000005F
#define SOFT_Cl_Helpers_Curve13				0x00000060
#define SOFT_Cl_Helpers_Curve14				0x00000061
#define SOFT_Cl_Helpers_Curve15				0x00000062
#define SOFT_Cl_Helpers_Curve16				0x00000063

#define SOFT_Cl_Helpers_Skeleton1           0x00000064
#define SOFT_Cl_Helpers_Skeleton2			0x00000065
#define SOFT_Cl_Helpers_Skeleton3			0x00000066
#define SOFT_Cl_Helpers_Skeleton4			0x00000067

#ifdef JADEFUSION //POPOWARNING MARASME XENON ??
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha1	0x00000068
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha2	0x00000069
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha3	0x0000006A
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha4	0x0000006B
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha5	0x0000006C
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha6	0x0000006D
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha7	0x0000006E
#define	SOFT_Cl_Helpers_CylSpotLittleAlpha8	0x0000006F

#define	SOFT_Cl_Helpers_CylSpotBigAlpha1	0x00000070
#define	SOFT_Cl_Helpers_CylSpotBigAlpha2	0x00000071
#define	SOFT_Cl_Helpers_CylSpotBigAlpha3	0x00000072
#define	SOFT_Cl_Helpers_CylSpotBigAlpha4	0x00000073
#define	SOFT_Cl_Helpers_CylSpotBigAlpha5	0x00000074
#define	SOFT_Cl_Helpers_CylSpotBigAlpha6	0x00000075
#define	SOFT_Cl_Helpers_CylSpotBigAlpha7	0x00000076
#define	SOFT_Cl_Helpers_CylSpotBigAlpha8	0x00000077

#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart1	0x00000078
#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart2	0x00000079
#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart3	0x0000007A
#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart4	0x0000007B
#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart5	0x0000007C
#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart6	0x0000007D
#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart7	0x0000007E
#define	SOFT_Cl_Helpers_CylSpotLittleAlphaStart8	0x0000007F

#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart1		0x00000080
#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart2		0x00000081
#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart3		0x00000082
#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart4		0x00000083
#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart5		0x00000084
#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart6		0x00000085
#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart7		0x00000086
#define	SOFT_Cl_Helpers_CylSpotBigAlphaStart8		0x00000087

#define SOFT_Cl_Helpers_CylSpotExtraLinesH	0x00000088
#define SOFT_Cl_Helpers_CylSpotExtraLinesV	0x00000089

#define	SOFT_Cl_Helpers_SpotShadowAlpha1	0x0000008A
#define	SOFT_Cl_Helpers_SpotShadowAlpha2	0x0000008B
#define	SOFT_Cl_Helpers_SpotShadowAlpha3	0x0000008C
#define	SOFT_Cl_Helpers_SpotShadowAlpha4	0x0000008D
#define	SOFT_Cl_Helpers_SpotShadowAlpha5	0x0000008E
#define	SOFT_Cl_Helpers_SpotShadowAlpha6	0x0000008F
#define	SOFT_Cl_Helpers_SpotShadowAlpha7	0x00000090
#define	SOFT_Cl_Helpers_SpotShadowAlpha8	0x00000091

#define	SOFT_Cl_Helpers_SpotShadowNear1		0x00000092
#define	SOFT_Cl_Helpers_SpotShadowNear2		0x00000093
#define	SOFT_Cl_Helpers_SpotShadowNear3		0x00000094
#define	SOFT_Cl_Helpers_SpotShadowNear4		0x00000095
#define	SOFT_Cl_Helpers_SpotShadowNear5		0x00000096
#define	SOFT_Cl_Helpers_SpotShadowNear6		0x00000097
#define	SOFT_Cl_Helpers_SpotShadowNear7		0x00000098
#define	SOFT_Cl_Helpers_SpotShadowNear8		0x00000099

#define	SOFT_Cl_Helpers_SpotShadowFarAlpha1	0x0000009A
#define	SOFT_Cl_Helpers_SpotShadowFarAlpha2	0x0000009B
#define	SOFT_Cl_Helpers_SpotShadowFarAlpha3	0x0000009C
#define	SOFT_Cl_Helpers_SpotShadowFarAlpha4	0x0000009D
#define	SOFT_Cl_Helpers_SpotShadowFarAlpha5	0x0000009E
#define	SOFT_Cl_Helpers_SpotShadowFarAlpha6	0x0000009F
#define	SOFT_Cl_Helpers_SpotShadowFarAlpha7	0x000000A0
#define	SOFT_Cl_Helpers_SpotShadowFarAlpha8	0x000000A1

#define	SOFT_Cl_Helpers_SpotShadowFar1		0x000000A2
#define	SOFT_Cl_Helpers_SpotShadowFar2		0x000000A3
#define	SOFT_Cl_Helpers_SpotShadowFar3		0x000000A4
#define	SOFT_Cl_Helpers_SpotShadowFar4		0x000000A5
#define	SOFT_Cl_Helpers_SpotShadowFar5		0x000000A6
#define	SOFT_Cl_Helpers_SpotShadowFar6		0x000000A7
#define	SOFT_Cl_Helpers_SpotShadowFar7		0x000000A8
#define	SOFT_Cl_Helpers_SpotShadowFar8		0x000000A9

#define	SOFT_Cl_Helpers_CylSpotShadowAlpha1	0x000000AA
#define	SOFT_Cl_Helpers_CylSpotShadowAlpha2	0x000000AB
#define	SOFT_Cl_Helpers_CylSpotShadowAlpha3	0x000000AC
#define	SOFT_Cl_Helpers_CylSpotShadowAlpha4	0x000000AD
#define	SOFT_Cl_Helpers_CylSpotShadowAlpha5	0x000000AE
#define	SOFT_Cl_Helpers_CylSpotShadowAlpha6	0x000000AF
#define	SOFT_Cl_Helpers_CylSpotShadowAlpha7	0x000000B0
#define	SOFT_Cl_Helpers_CylSpotShadowAlpha8	0x000000B1

#define SOFT_Cl_Helpers_Gizmo				0x000000B2
#else
#define SOFT_Cl_Helpers_Gizmo               0x00000068
#endif

#define SOFT_Cl_Helpers_Portal				0x00000100
#define SOFT_Cl_Helpers_LastPortal			0x000004FF


/* Helper type */
#define SOFT_Cl_HT_Ellipse	0
#define SOFT_Cl_HT_Arrow	1
#define SOFT_Cl_HT_Square	2
#define SOFT_Cl_HT_Omni		3
#define SOFT_Cl_HT_Curve	4

#define SOFT_Cl_HT_Number	5

/* user flag for arrow */
#define SOFT_Cl_ArrowScale	0x00010000
#define SOFT_Cl_ArrowMove	0x00020000
#define SOFT_Cl_ArrowSpot	0x00040000

/* constant for curve in track */
#define SOFT_Cl_TrackTrans	0
#define SOFT_Cl_TrackRot	1

/* flags for curve helper */
#define SOFT_Cl_CurveUsed	0x00000001
#define SOFT_Cl_AnimCurve	0x00000002

/* constant for curve flags */
#define SOFT_Cl_UseGizmoFlag	0x00000001
#define SOFT_Cl_ShowMB			0x00000002

/* constant for used axes */
#define SOFT_Cl_Helper_XAxis    0x01
#define SOFT_Cl_Helper_YAxis    0x02
#define SOFT_Cl_Helper_ZAxis    0x04


/*$4
 ***********************************************************************************************************************
    Structure
 ***********************************************************************************************************************
 */

/*$off*/
/*
 -----------------------------------------------------------------------------------------------------------------------
    Helper interface 
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct SOFT_tdst_HelperInterface_
{
	LONG l_Type;
	void (*pfnv_Render) (struct SOFT_tdst_Helpers_ *, struct SOFT_tdst_Helper_ *, struct GDI_tdst_DisplayData_ *);
	LONG (*pfnl_Pick) ( struct SOFT_tdst_Helpers_ *, struct SOFT_tdst_Helper_ *, MATH_tdst_Vector *, MATH_tdst_Vector *, char );
	void (*pfnv_GetMove) (struct SOFT_tdst_Helpers_ *, struct SOFT_tdst_Helper_ *, MATH_tdst_Vector *);
	void (*pfnv_GetAxe) (struct SOFT_tdst_Helpers_ *, struct SOFT_tdst_Helper_ *, MATH_tdst_Vector *);
} SOFT_tdst_HelperInterface;
/*$on*/

/*
 -----------------------------------------------------------------------------------------------------------------------
    curve helper data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_HelperCurvePick_
{
    MATH_tdst_Vector	st_ViewPos;
	int					i_Track;
	int					i_Evt;
	int					i_Translation;
	float				f_Time;
} SOFT_tdst_HelperCurvePick;

typedef struct	SOFT_tdst_HelperCurve_
{
	ULONG						ul_Flags;
	void						*p_CurGO;
    void                        *p_OwnerGO;

	void						*p_EventData;
	void						*p_Track[2];
	int							i_TransEvt, i_TransPos;
	int							i_RotEvt;
	MATH_tdst_Matrix			*dst_Matrix;
	float						f_BoneLength[64];

	SOFT_tdst_HelperCurvePick	*dst_Pick;
	LONG						l_PickNumber;
	LONG						l_PickMax;
	LONG						l_PickCur;
} SOFT_tdst_HelperCurve;

/*
 -----------------------------------------------------------------------------------------------------------------------
    skeleton helper data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct SOFT_tdst_HelperSkeletonBone_
{
    MATH_tdst_Vector A, B;
    LONG             l_Gizmo;
} SOFT_tdst_HelperSkeletonBone;

typedef struct SOFT_tdst_HelperSkeleton_
{
    OBJ_tdst_GameObject             *pst_GO;
    LONG                            l_NbBones;
    SOFT_tdst_HelperSkeletonBone    *dst_Bone;
    LONG                            l_GizmoPicked;
} SOFT_tdst_HelperSkeleton;


/*
 -----------------------------------------------------------------------------------------------------------------------
    One helper
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_Helper_
{
	SOFT_tdst_HelperInterface	*i;
	LONG						l_Id;
	void						*p_Data;
} SOFT_tdst_Helper;

/*
 -----------------------------------------------------------------------------------------------------------------------
    All helpers
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_Helpers_
{
	struct GDI_tdst_DisplayData_	*pst_DD;
	ULONG							ul_Flags;
	ULONG							ul_FlagsSave[3];

	LONG							l_Pickable;
	LONG							l_Picked;

	float							f_AngleSnap;

	/* Center of movers */
	MATH_tdst_Vector				st_Center;
	MATH_tdst_Vector				st_Center2D;
	MATH_tdst_Vector				st_SeparateCenter;
	MATH_tdst_Matrix				st_Matrix;
	MATH_tdst_Matrix				st_GlobalMatrix;
    MATH_tdst_Matrix                st_CamToGlobalMatrix;
    MATH_tdst_Matrix                st_CamToLocalMatrix;
	MATH_tdst_Matrix				st_MatrixGlobalAxis;

    /* en sous objet : nombre de points sélectionnés */
    LONG                            l_NbVertices;
	LONG							l_OneVertexIndex;
    BOOL							b_CenterLock_On;
    OBJ_tdst_GameObject				*pst_CenterLock_GAO;
    int								i_CenterLock_Point;

    /* save starting position */
    MATH_tdst_Vector                st_StartPos;

	/* Rotation movers */
	SOFT_tdst_Ellipse				ast_Ellipse[5];
	LONG							l_CurrentEllipse;

	/* Translation/scale movers */
	SOFT_tdst_Arrow					ast_Arrow[6];
	LONG							l_CurrentArrow;

	/* Center mover */
	SOFT_tdst_Square				st_Square;

	/* Omni light helper */
	SOFT_tdst_Ellipse				ast_OmniNear[8];
	SOFT_tdst_Ellipse				ast_OmniFar[8];
	void							*ap_Omni[8];
	LONG							l_CurrentOmni;

	/* Spot light helper */
	SOFT_tdst_Arrow					ast_SpotNear[8];
	SOFT_tdst_Arrow					ast_SpotFar[8];
	SOFT_tdst_Ellipse				ast_SpotLittleAlpha[8];
	SOFT_tdst_Ellipse				ast_SpotBigAlpha[8];
	void							*ap_Spot[8];
	LONG							l_CurrentSpot;

#ifdef JADEFUSION
	/* extra Cylindrical Spot light helper */
	SOFT_tdst_Ellipse				ast_Cyl_SpotLittleAlphaStart[8];
	SOFT_tdst_Ellipse				ast_Cyl_SpotBigAlphaStart[8];
	SOFT_tdst_Ellipse				ast_Cyl_SpotLittleAlpha[8];
	SOFT_tdst_Ellipse				ast_Cyl_SpotBigAlpha[8];
	SOFT_tdst_Arrow					ast_Cyl_SpotExtraLinesH[8];
	SOFT_tdst_Arrow					ast_Cyl_SpotExtraLinesV[8];

	/* extra Shadow helper */
	SOFT_tdst_Arrow					ast_ShadowNear[8];
	SOFT_tdst_Arrow					ast_ShadowFar[8];
	SOFT_tdst_Ellipse				ast_ShadowAlpha[8];
	SOFT_tdst_Ellipse				ast_ShadowFarAlpha[8];
	SOFT_tdst_Ellipse				ast_Cyl_ShadowAlpha[8];

#endif

	/* Curve helper */
	float							f_NbInterPerSecond;
	LONG							l_NbFramesBefore;
	LONG							l_NbFramesAfter;
	ULONG							ul_CurveFlag;
	ULONG							ul_GizmoDisplayFlag[2];

	SOFT_tdst_HelperCurve			ast_Curve[16];
	SOFT_tdst_Helper				ast_HelperCurve[16];

    /* skeletno helpers */
    LONG                            l_CurrentSkeleton;
    SOFT_tdst_HelperSkeleton        ast_Skeleton[4];

    /* UV helper */
#ifdef ACTIVE_EDITORS
    SOFT_tdst_UVGizmo               st_UVGizmo;
#endif

	/* All helpers */
#ifdef JADEFUSION
	SOFT_tdst_Helper				ast_Helper[256];
#else
	SOFT_tdst_Helper				ast_Helper[64];
#endif
	LONG							l_CurrentHelper;

    /* grid */
    float                           f_GridSizeXY;
    float                           f_GridSizeZ;
    float                           f_GridWidth;
    ULONG                           ul_GridColor;
    MATH_tdst_Vector                st_GridX;
    MATH_tdst_Vector                st_GridY;
    
    /* prefab helper */
    void							*pv_Prefab;
    MATH_tdst_Vector				st_PrefabPos;
    
    /* portal helper */
    int								i_PortalNb;
    WOR_tdst_Portal					*apst_Portal[ WOR_C_MaxSecto * WOR_C_MaxSectoPortals ];

} SOFT_tdst_Helpers;

/*$4
 ***********************************************************************************************************************
    Constanst
 ***********************************************************************************************************************
 */

extern SOFT_tdst_HelperInterface	SOFT_gast_HelperInterface[SOFT_Cl_HT_Number];

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
void					SOFT_Helpers_Init(SOFT_tdst_Helpers *, struct GDI_tdst_DisplayData_ *);
void					SOFT_Helpers_Reset(SOFT_tdst_Helpers *);
void					SOFT_Helpers_HideMovers(SOFT_tdst_Helpers *);
LONG					SOFT_l_Helpers_MoversAreVisible(SOFT_tdst_Helpers *);
void					SOFT_Helper_ChangeMoverMode(SOFT_tdst_Helpers *, LONG, ULONG, BOOL);
char                    SOFT_c_Helper_GetUsedAxis( SOFT_tdst_Helpers * );
char                    SOFT_c_Helper_GetUsedSystemAxis( SOFT_tdst_Helpers * );
void					SOFT_Helpers_Render(SOFT_tdst_Helpers *, struct GDI_tdst_DisplayData_ *);
LONG					SOFT_l_Helpers_Pick(SOFT_tdst_Helpers *, LONG, LONG, LONG);
void					SOFT_Helpers_GetMove(SOFT_tdst_Helpers *, LONG, MATH_tdst_Vector *);
void					SOFT_Helpers_GetAxe(SOFT_tdst_Helpers *, LONG, MATH_tdst_Vector *);
LONG					SOFT_l_Helpers_GetDescription(SOFT_tdst_Helpers *, LONG, char *, MATH_tdst_Vector *);
LONG					SOFT_l_Helper_IsScale(SOFT_tdst_Helper *);
LONG					SOFT_l_Helper_IsMove(SOFT_tdst_Helper *);
LONG					SOFT_l_Helper_IsRotate(SOFT_tdst_Helper *);
void                    SOFT_Helpers_DetachObject(SOFT_tdst_Helpers *, OBJ_tdst_GameObject *);

SOFT_tdst_HelperCurve	*SOFT_pst_Helper_GetCurve(SOFT_tdst_Helpers *, struct EVE_tdst_Track_ *);
void					SOFT_Helper_AddCurve
						(
							SOFT_tdst_Helpers *,
							OBJ_tdst_GameObject *,
							struct EVE_tdst_Data_ *,
							struct EVE_tdst_Track_ *
						);
void					SOFT_Helper_DelCurve(SOFT_tdst_Helpers *, struct EVE_tdst_Data_ *, struct EVE_tdst_Track_ *);
void					SOFT_Helper_CurvePickReset(SOFT_tdst_HelperCurve *);
void					SOFT_Helper_CurvePickAdd
						(
							SOFT_tdst_HelperCurve *,
							MATH_tdst_Vector *,
							int,
							int,
							float,
							int,
							int
						);
void					SOFT_Helpers_CurveRenderSkel(OBJ_tdst_GameObject *, struct GDI_tdst_DisplayData_ *);
void					SOFT_Helpers_CurveRenderBone(OBJ_tdst_GameObject *, struct GDI_tdst_DisplayData_ *);
void                    SOFT_Helpers_CurveReplaceData( SOFT_tdst_Helpers *, OBJ_tdst_GameObject *, struct EVE_tdst_Data_ *, struct EVE_tdst_Data_ * );

void                    SOFT_Helpers_UVGizmoOn( SOFT_tdst_Helpers *H, OBJ_tdst_GameObject *, int, float );
void                    SOFT_Helpers_UVGizmoOff( SOFT_tdst_Helpers *H );
BOOL                    SOFT_b_Helpers_UVGizmoIsOn( SOFT_tdst_Helpers * );

LONG					SOFT_l_NearestPointOf2Lines
						(
							MATH_tdst_Vector *,
							MATH_tdst_Vector *,
							MATH_tdst_Vector *,
							MATH_tdst_Vector *,
							float *,
							float *,
							MATH_tdst_Vector *,
							float *,
							MATH_tdst_Vector *
						);

#else
#define SOFT_Helpers_Init(a, b)
#define SOFT_Helpers_Reset(a)
#define SOFT_Helpers_HideMovers(a)
#define SOFT_l_Helpers_MoversAreVisible(a)	0
#define SOFT_Helper_ChangeMoverMode(a, b, c, d)
#define SOFT_Helpers_Render(a, b)
#define SOFT_l_Helpers_Pick(a, b, c, d) -1
#define SOFT_Helpers_GetMove(a, b, c)
#define SOFT_Helpers_GetAxe(a, b, c)
#define SOFT_l_Helpers_GetDescription(a, b, c, d)	0
#define SOFT_l_Helper_IsScale(a)					0
#define SOFT_l_Helper_IsMove(a)						0
#define SOFT_l_Helper_IsRotate(a)					0
#define SOFT_Helpers_DetachObject(a,b)
#define SOFT_pst_Helper_GetCurve(a, b)				0
#define SOFT_Helper_AddCurve(a, b, c, d)
#define SOFT_Helper_DelCurve(a, b, c)
#define SOFT_Helper_CurvePickReset(a)
#define SOFT_Helper_CurvePickAdd(a, b, c, d, e, f, g)

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTHELPER_H__ */
