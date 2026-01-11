/*$T WORaccess.h GC!1.63 12/28/99 18:29:09 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$F
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Fonctions d'acces aux structures du monde
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WORLDACCES_H__
#define __WORLDACCES_H__

#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"
#include "TABles/TABles.h"
#include "MATHs/MATHfloat.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Universe "class" access functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Get universe status macros
 =======================================================================================================================
 */
#define UNI_Status()    (WOR_gst_Universe.uc_CurrentStatus)

/*
 =======================================================================================================================
    Aim:    Get the number of worlds in the universe
 =======================================================================================================================
 */
_inline_ ULONG WOR_ul_Universe_GetNbWorlds(void)
{
    return TAB_ul_PFtable_GetNbElems(&WOR_gst_Universe.st_WorldsTable);
}

/*
 =======================================================================================================================
    Aim:    Get the number of worlds in the universe
 =======================================================================================================================
 */
_inline_ ULONG WOR_ul_Universe_GetMaxNbWorlds(void)
{
    return TAB_ul_PFtable_GetMaxNbElems(&WOR_gst_Universe.st_WorldsTable);
}

/*
 =======================================================================================================================
    Aim:    Get the pointer and the flags of World

    In:     the index of the world
 =======================================================================================================================
 */
_inline_ TAB_tdst_PFelem *WOR_pst_Universe_GetWorldPFelem(int n)
{
    ERR_X_Assert
    (
        TAB_b_PFtable_IsInRange
        (
            &WOR_gst_Universe.st_WorldsTable,
            &WOR_gst_Universe.st_WorldsTable.p_Table[n]
        )
    );
    return &WOR_gst_Universe.st_WorldsTable.p_Table[n];
}

/*
 =======================================================================================================================
    Aim:    Returns true if world is active

    In:     index of the world
 =======================================================================================================================
 */
_inline_ ULONG WOR_b_Universe_IsWorldActive(ULONG _ul_CurrentWorld)
{
    ERR_X_Assert
    (
        TAB_b_PFtable_IsInRange
        (
            &WOR_gst_Universe.st_WorldsTable,
            &WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld]
        )
    );
    return((WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld].ul_Flags) & 1);
}

/*
 =======================================================================================================================
    Aim:    Makes the world active

    In:     index of the world
 =======================================================================================================================
 */
_inline_ void WOR_Universe_SetWorldActive(ULONG _ul_CurrentWorld)
{
    ERR_X_Assert
    (
        TAB_b_PFtable_IsInRange
        (
            &WOR_gst_Universe.st_WorldsTable,
            &WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld]
        )
    );
    (WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld].ul_Flags) |= 1;
}

/*
 =======================================================================================================================
    Aim:    Makes the world inactive

    In:     index of the world
 =======================================================================================================================
 */
_inline_ void WOR_Universe_SetWorldInactive(ULONG _ul_CurrentWorld)
{
    ERR_X_Assert
    (
        TAB_b_PFtable_IsInRange
        (
            &WOR_gst_Universe.st_WorldsTable,
            &WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld]
        )
    );
    (WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld].ul_Flags) &= ~1;
}

/*
 =======================================================================================================================
    Aim:    Returns the pointer to the world

    In:     the index of the world
 =======================================================================================================================
 */
_inline_ WOR_tdst_World *WOR_pst_Universe_GetWorldPointer(ULONG _ul_CurrentWorld)
{
    ERR_X_Assert
    (
        TAB_b_PFtable_IsInRange
        (
            &WOR_gst_Universe.st_WorldsTable,
            &WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld]
        )
    );
    return((WOR_tdst_World *) (WOR_gst_Universe.st_WorldsTable.p_Table[_ul_CurrentWorld].p_Pointer));
}

/*$4
 ***********************************************************************************************************************
    Functions related to World
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ WOR_tdst_World *WOR_World_GetWorldOfObject(OBJ_tdst_GameObject *_pst_GO)
{
#ifdef ACTIVE_EDITORS
    return _pst_GO->pst_World;
#else
    return WOR_gpst_CurrentWorld;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ WOR_tdst_View *WOR_pst_World_GetView(WOR_tdst_World *_pst_World, int _i_Index)
{
    return(&_pst_World->pst_View[_i_Index]);
}

/*$4
 ***********************************************************************************************************************
    Functions related to the world view
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *WOR_View_GetAbsolutePosition(WOR_tdst_View *_pst_View)
{
    return(MATH_pst_GetTranslation(&_pst_View->st_ViewPoint));
}

extern void WOR_View_SetViewPoint
            (
                WOR_tdst_View       *_pst_View,
                MATH_tdst_Matrix    *_pst_ViewPoint,
                MATH_tdst_Vector    *_pst_Speed,
                BOOL                _b_AutoCalculateSpeed,
                BOOL                _b_KeepOldOrientation
            );

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void WOR_View_SetFather(WOR_tdst_View *_pst_View, OBJ_tdst_GameObject *_pst_Father)
{
    _pst_View->pst_Father = _pst_Father;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void WOR_View_ClearFather(WOR_tdst_View *_pst_View, OBJ_tdst_GameObject *_pst_Father)
{
    _pst_View->pst_Father = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ OBJ_tdst_GameObject *WOR_View_GetFather(WOR_tdst_View *_pst_View)
{
    return(_pst_View->pst_Father);
}

#define Cl_1024f    0x44800000
#define Cl_9152f    0x460f0000

/*
 =======================================================================================================================
    Aim:    Changes a distance stored in a float format into a LODDist UCHAR

    Note:   (Range=0 -> 9152 meters) 0=0 meters, 255=9152 meters
 =======================================================================================================================
 */
_inline_ UCHAR OBJ_uc_CalcLODDist(float _f_dist)
{
    if(fSupEqPositivLong(_f_dist, Cl_9152f))
        return((UCHAR) 255);

    if(fSupEqPositivLong(_f_dist, Cl_1024f))
    {
        return((UCHAR) ((lFloatToLong(_f_dist) - 1024) >> 6) + 128);
    }
    else
        return((UCHAR) (lFloatToLong(_f_dist) >> 3));
}

/*
 =======================================================================================================================
    Aim:    Changes a distance stored in a LODDist UCHAR into a real float distance (in meters)

    Note:   (Range=0 -> 9152 meters) 0=0 meters, 255=9152 meters
 =======================================================================================================================
 */
_inline_ float OBJ_f_CalcDistFromLODDist(UCHAR _uc_dist)
{
    if(_uc_dist > 127)
    {
        return(fLongToFloat(1024 + (((LONG) (_uc_dist - 128)) << 6)));
    }
    else
        return(fLongToFloat(((LONG) (_uc_dist)) << 3));
}

/*
 =======================================================================================================================
    Aim:    Returns the % of screen covered by the BV of an object in the given view

    Note:   0= less than 1 % of the screen surface; 255= full screen or bigger
 =======================================================================================================================
 */
 /* 
 	Philippe : Je l'ai déinliné dans wor_render.c, 
 */
UCHAR OBJ_uc_CalcLODVis(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_View *_pst_View, float _f_SqrObjDist);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __WORLDACCES_H__ */