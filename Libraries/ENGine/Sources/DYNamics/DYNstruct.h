/*$T DYNstruct.h GC!1.52 01/19/00 11:23:30 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __DYNSTRUCT_H__
#define __DYNSTRUCT_H__


#include "BASe/BAStypes.h"

#include "MATHs/MATH.h"
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 ---------------------------------------------------------------------------------------------------
    Aim:    Basic Dynamics structure

    Note:   If f_InvMass equals 1, then the Linear Momentum is The speed and the force is the
            acceleration.
 ---------------------------------------------------------------------------------------------------
 */

typedef struct  DYN_tdst_Dyna_
{
    ULONG						ul_DynFlags;        /* (0) Indicates the type of dynamics */
    MATH_tdst_Vector            *pst_X;             /* (+4)Pointer to the global position of the
                                                     * point. (The Rotation Matrix is 48 bytes
                                                     * before this pointer) */
    MATH_tdst_Vector            st_P;               /* (+8)Linear Momentum (or speed if mass=1) */
    MATH_tdst_Vector            st_F;               /* (+20)Sum of all Forces, (or unique force if
                                                     * flag DYN_Cuc_OneConstantForce is Set) */
    float                       f_InvMass;          /* (+32)1/m (where m is the mass) */

    struct DYN_tdst_Forces_     *pst_Forces;        /* (+36) */
    float                       f_SpeedLimitHoriz;  /* (+40) */
    struct DYN_tdst_Solid_      *pst_Solid;         /* (+44) */
    struct DYN_tdst_Col_        *pst_Col;           /* (+48) */
    float                       f_SpeedLimitVert;   /* (+52) */
    struct DYN_tdst_Constraint_ *pst_Constraint;    /* (+56) */
	float						f_MaxStepSize;			/* (+60) */

    /*
     * WARNING!!!: if you change this structure, you must update the constants defining the
     * offset (in bytes) from the beginning of the structure !! (in DYNconst.h)
     */
#ifdef ACTIVE_EDITORS

    /*
     * In the editors, we like to edit the speed and the mass rather than the Linear Momentum and
     * the inverse of the mass. That's why we need these variables
     */
    float               f_Mass;             /* (+64) Mass (for edition only, when this value is
                                             * edited, the f_InvMass value is changed) */
    MATH_tdst_Vector    st_SpeedVector;     /* (+68) Speed Vector (for edition only, when this
                                             * value is edited, the st_P vector is changed) */
    char                *sz_Name;           /* (+80) Name of the Dynamic structure */

#endif

	MATH_tdst_Vector	st_MaxPos;
}
DYN_tdst_Dyna;

/*
 ---------------------------------------------------------------------------------------------------
    Aim:    Forces structure

    Note:   Countains 4 standard forces for exact ODE solving, and any forces for complex systems
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  DYN_tdst_Forces_
{
    /* 3 constant forces: gravity, Traction, and Stream */
    MATH_tdst_Vector    st_Gravity;         /* Gravity force (multiply it by 1/m to get the gravity) */
    MATH_tdst_Vector    st_Traction;        /* Traction force */
    MATH_tdst_Vector    st_Stream;          /* Additionnal constant force (used mainly for streams)
                                             * which is cleared every frame by the engine.
                                             * Typiccaly, during the game, when a Windbox detects
                                             * that an actor is in the box, it adds its force to
                                             * the actor in this field */

    /* 1 permanent (non constant) force: friction */
    MATH_tdst_Vector    st_K;               /* (+36) friction (1 coeff for each local axis) */

    /* More forces (for later) */
    void                *pst_MoreForces;    /* (+48) */
} DYN_tdst_Forces;

typedef struct  DYN_tdst_StdSolid_
{
    /* Constant quantities */
    float                   f_InvMass;      /* 1/m (where m is the mass) */
    MATH_tdst_Matrix        st_Ibody;       /* Inertia Tensor of the Body */
    MATH_tdst_Matrix        st_IbodyInv;    /* Invert matrix of the Inertia Tensor of the Body */

    /* State Variables */
    MATH_tdst_Quaternion    st_q;           /* Orientation */
    MATH_tdst_Vector        st_P;           /* Linear Momentum */
    MATH_tdst_Vector        st_L;           /* Angular Momentum */

    /* Derived Quantities */
    MATH_tdst_Matrix        st_Iinv;        /* Invert matrix of the inertia tensor */
    MATH_tdst_Matrix        st_R;           /* Orientation Matrix (not sure necessary to store...) */
    MATH_tdst_Vector        st_v;           /* Speed */
    MATH_tdst_Vector        st_w;           /* Angular Speed (colinéaire à l'axe de rotation) */

    /* Computed Quantities */
    MATH_tdst_Vector        st_F;           /* Force (sum of all forces) */
    MATH_tdst_Vector        st_T;           /* Torque (couple: colinéaire à l'axe de rotation) */
} DYN_tdst_StdSolid;

typedef struct  DYN_tdst_Solid_
{
    MATH_tdst_Vector    st_w;               /* Angular Speed */
    MATH_tdst_Vector    st_T;               /* Torque */
	MATH_tdst_Vector	st_OldPosition;		/* Position of the Object at the last Frame */
    float               f_wLimit;           /* Angular Speed Limit */
    float               f_Factor;           /* Rebound in Physical Mode */
	float				f_Stability;		/* Stability of the System */
} DYN_tdst_Solid;

/*
 ---------------------------------------------------------------------------------------------------
    Aim:    Structure allocated to actors that have a zdm and want specific rebound or Slide values

    Note:   If this structure is not allocated, the default values for rebound, slide and
            HorizAngle are: £
            Rebound: 0 £
            Slide: 1 £
            HorizAngle (in radians) : 0 £
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  DYN_tdst_Col_
{
    float   f_Rebound;
    float   f_Slide;
    float   f_ReboundHorizCosAngle;
    float   f_SlideHorizCosAngle;
	float	f_ApplyRecFactor;
} DYN_tdst_Col;

/*
 ---------------------------------------------------------------------------------------------------
    Aim:    Structure that countains informations about constraint
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  DYN_tdst_Constraint_
{
    MATH_tdst_Vector    st_A;               /* First point of segment or first point of parabol */
    MATH_tdst_Vector    st_B;               /* Second point of segment or second point of parabol */
    MATH_tdst_Vector    st_O;               /* Center of parabol */
    float               f_r;                /* Radius of sphere around center of parabol */
	MATH_tdst_Vector	st_Tangent;			/* Tangent at the constraint point */			
    char                c_ConstraintType;   /* Type of constraint 0= no constraint */
    char                c_align[3];
} DYN_tdst_Constraint;
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __DYNSTRUCT_H__ */