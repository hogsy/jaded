/*$T INTstruct.h GC!1.5 12/08/99 16:25:45 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __INT_STRUCT__
#define __INT_STRUCT__

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BASe/BAStypes.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "INTConst.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ---------------------------------------------------------------------------------------------------
    Sweep and Prune structures ...
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  INT_tdst_AxisNode_
{
    OBJ_tdst_GameObject *pst_Obj;
    USHORT              us_Ref;
    unsigned char       uc_Flags;
    unsigned char       uc_Dummy;
    union
    {
        float               *pf_BVCoord;
        MATH_tdst_Vector    *pst_Center;
    };
    float   f_Val;
} INT_tdst_AxisNode;

typedef struct  INT_tdst_AxisTable_
{
    INT_tdst_AxisNode   *pst_Nodes;
    LONG                al_Flags[INT_Cul_MaxFlags];
} INT_tdst_AxisTable;

typedef struct  INT_tdst_SnP_Manager_
{
    USHORT              us_IndexToPlace;
    USHORT              us_NbObjToRemove;
    OBJ_tdst_GameObject *apst_ObjToRemove[INT_Cul_MaxObjects];
    USHORT              us_NbRanks;
    USHORT              aus_Rank[INT_Cul_MaxObjects];
} INT_tdst_SnP_Manager;

typedef struct  INT_tdst_SnP_
{
    USHORT                  us_NbElems;
    INT_tdst_SnP_Manager    *pst_Manager;
    INT_tdst_AxisTable      *apst_AxisTable[3];
    OBJ_tdst_GameObject     *apst_IndexToObj[INT_Cul_MaxObjects];
} INT_tdst_SnP;

/* ---------------------------------------------------------------------------------------------------
    Classcial Intersection structures ...
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  INT_tdst_Box_
{
    MATH_tdst_Vector    st_Min;
    MATH_tdst_Vector    st_Max;
} INT_tdst_Box;

typedef struct  INT_tdst_Sphere_
{
    MATH_tdst_Vector    st_Center;
    float               f_Radius;
} INT_tdst_Sphere;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplus */
#endif /* __INT_STRUCT__ */
