/*$T GRI_struct.h GC!1.68 01/05/00 09:27:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GRI_STRUCT_H__
#define __GRI_STRUCT_H__

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define GRID_REAL_X 7
#define GRID_REAL_Y 7

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Definition of a grid element
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct GRID_tdst_Elem_
{
    char    c_Capacities;
} GRID_tdst_Elem;

/*
 -----------------------------------------------------------------------------------------------------------------------
    For shortest path aglo
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
    float   f_Eval;
    short   w_WayX;
    short   w_WayY;
#define NONE        0
#define EXPLORED    1
#define REACHED     2
    char    c_Flag;
} GRID_tdst_Eval;

/*
 -----------------------------------------------------------------------------------------------------------------------
    For shortest path aglo
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
    float   *pf_Eval;
    short   x;
    short   y;
} GRID_tdst_Best;

/*
 -----------------------------------------------------------------------------------------------------------------------
    For shortest path aglo
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
    short   x;
    short   y;
} GRID_tdst_Explored;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of one grid for a world
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  GRID_tdst_World_
{
    void            **p_Groups;			/* All compressed groups */
    void            **p_RealGroups;		/* All compressed groups in real time */
	void			*p_CompressBuf;		/* The initial grid compressed */
    GRID_tdst_Elem  *pst_RealArray;     /* Array of elems in real time */
	GRID_tdst_Eval	*pst_EvalArray;		/* Array for shortest path algo */
    GRID_tdst_Elem  *pst_EditArray;     /* Array of all elems for editor */
    float           f_MinXTotal;        /* Coords min of all groups */
    float           f_MinYTotal;
    float           f_MinXReal;         /* Current min coord in pst_RealArray */
    float           f_MinYReal;
    short           w_NumGroupsX;       /* Number of groups in x (in compressed array) */
    short           w_NumGroupsY;       /* Number of groups in y */
    short           w_NumRealGroupsX;   /* Number of groups in real time in pst_RealArray */
    short           w_NumRealGroupsY;   /* Number of groups in real time in pst_RealArray */
    short           w_XRealGroup;		/* Coord X of first group in real time array */
    short           w_YRealGroup;		/* Coord Y of first group in real time array */
    char            c_SizeGroup;        /* Number of case in a group (it's square) */
	void			*p_GameObject;
	int				mini, maxi, minj, maxj;
} GRID_tdst_World;

#endif /* !__GRI_STRUCT_H__ */
