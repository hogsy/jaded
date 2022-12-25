/*$T COLvars.h GC!1.71 02/01/00 18:05:28 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __COL_VARS__
#define __COL_VARS__
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplus */

#include "MATHs/MATH.h"

#define COL_Cuc_MaxRecomputeGO				50
/*
 -----------------------------------------------------------------------------------------------------------------------
    Les variables globales necessaires aux collisions
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  COL_tdst_GlobalVars_
{
    ULONG                                       ul_ReportIndex;
    struct WOR_tdst_World_                      *pst_World;

    struct OBJ_tdst_GameObject_                 *pst_A;
    struct COL_tdst_ZDx_                        *pst_A_ZDx;
    struct COL_tdst_Instance_                   *pst_A_Instance;
    struct COL_tdst_ZDx_                        **dpst_A_ZDx;
    MATH_tdst_Matrix                            *pst_A_OldGlobalMatrix; /* Matrix of the previous frame. */
    MATH_tdst_Matrix                            *pst_A_GlobalMatrix;    /* Matrix of the current frame. */
	MATH_tdst_Vector							st_A_BCS_Speed;
    MATH_tdst_Vector                            st_A_BCS_ZDxCenter;
    MATH_tdst_Vector                            st_A_BCS_OldZDxCenter;
    MATH_tdst_Vector                            st_A_BCS_DynamicCenter;
    float                                       f_A_GCS_DynamicRadius;
    float                                       f_A_GCS_ZDxRadius;
    MATH_tdst_Vector                            st_A_BCS_Move;          /* Movement vector to go from the old position
                                                                         * to the current one. */

    MATH_tdst_Vector                            st_A_BCS_MoveUnit;      /* Movement vector to go from the old position
                                                                         * to the current one (unit) */

    struct OBJ_tdst_GameObject_                 *pst_B;
    struct COL_tdst_Instance_                   *pst_B_Instance;
    struct COL_tdst_ZDx_                        **dpst_B_ZDx;
    struct COL_tdst_ZDx_                        *pst_B_ZDx;

    struct COL_tdst_ColMap_                     *pst_B_ColMap;
    struct COL_tdst_Cob_                        *pst_B_Cob;
    struct COL_tdst_ElementIndexedTriangles_    *pst_B_Element;
    struct COL_tdst_IndexedTriangle_            *pst_B_Triangle;

    MATH_tdst_Matrix                            *pst_B_GlobalMatrix;    /* Matrix of the current frame. */
    MATH_tdst_Matrix                            st_B_InvGlobalMatrix ONLY_PSX2_ALIGNED(16);   /* Inverse Matrix of the current frame. */
    struct COL_tdst_Convex_                     *pst_B_Convex;
    struct COL_tdst_Plan_                       *pst_B_Plan;
    MATH_tdst_Vector                            st_B_Edge_Normal;
    float                                       f_WallCosAngle;
    float                                       f_CornerCosAngle;
    float                                       f_MaxStepSize;
    MATH_tdst_Vector                            st_InvUnitG;
    MATH_tdst_Vector                            *pst_InvUnitG;
	ULONG										ul_FirstCollision;
	OBJ_tdst_GameObject							*apst_RecomputeGO[COL_Cuc_MaxRecomputeGO];
	UCHAR										uc_RecomputeGO;
	ULONG										ul_FirstRecomputingReport;
	BOOL										b_Recomputing;
} COL_tdst_GlobalVars;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplcus */

#endif /* __COL_VARS__ */
